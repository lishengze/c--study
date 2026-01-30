#include <iostream>
#include <vector>
#include <list>
#include <mutex>
#include <thread>
#include <chrono>
#include <cstddef>
#include <cstring>
#include <algorithm>
#include <atomic>
#include <stdexcept>

// 内存池模板类，T为内存池分配的对象类型
template <typename T>
class MemoryPool {
public:
    // 内存块结构体：管理单个连续内存块的基础信息
    struct MemoryBlock {
        void* start_addr;    // 内存块首地址
        size_t elem_count;   // 该块可容纳的元素数量（T类型）
        bool is_used;        // 是否被使用
        size_t align;        // 内存对齐值

        MemoryBlock(void* addr, size_t count, size_t al)
            : start_addr(addr), elem_count(count), is_used(false), align(al) {}
    };

    // 空闲非连续块节点：管理单个T类型对象的空闲内存（用于非连续申请）
    struct FreeNode {
        void* addr;          // 单个T对象的内存地址
        FreeNode* next;      // 下一个空闲节点

        FreeNode(void* a = nullptr, FreeNode* n = nullptr) : addr(a), next(n) {}
    };

    // 构造函数：预分配init_elem_count个T类型对象的内存，初始化定时检查（默认5秒检查一次，使用率阈值80%）
    explicit MemoryPool(size_t init_elem_count, 
                        size_t check_interval_ms = 5000, 
                        float usage_threshold = 0.8f)
        : elem_size_(sizeof(T)),
          elem_align_(alignof(T)),
          check_interval_ms_(check_interval_ms),
          usage_threshold_(usage_threshold),
          stop_check_thread_(false) {
        if (init_elem_count == 0) {
            throw std::invalid_argument("Initial element count must be greater than 0");
        }
        if (usage_threshold <= 0 || usage_threshold >= 1) {
            throw std::invalid_argument("Usage threshold must be in (0,1)");
        }
        // 预分配初始内存
        expand(init_elem_count);
        // 启动定时检查线程
        check_thread_ = std::thread(&MemoryPool::check_and_expand, this);
    }

    // 析构函数：释放所有内存，停止定时检查线程
    ~MemoryPool() {
        // 停止后台检查线程
        stop_check_thread_ = true;
        if (check_thread_.joinable()) {
            check_thread_.join();
        }
        // 释放所有内存块
        std::lock_guard<std::mutex> lock(mtx_);
        for (auto& block : all_blocks_) {
            ::operator delete(block.start_addr, block.align);
        }
        // 释放空闲非连续块链表
        FreeNode* cur = free_list_;
        while (cur) {
            FreeNode* next = cur->next;
            delete cur;
            cur = next;
        }
    }

    // 禁用拷贝构造和赋值运算符（内存池对象不可拷贝）
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;

    // 移动构造和移动赋值（可选，此处禁用简化实现）
    MemoryPool(MemoryPool&&) = delete;
    MemoryPool& operator=(MemoryPool&&) = delete;

    /**
     * @brief 获取内存池的核心接口
     * @param elem_count 需要申请的元素数量
     * @param is_continuous 是否需要连续的内存空间
     * @return 申请到的内存首地址，失败返回nullptr
     */
    void* get(size_t elem_count, bool is_continuous) {
        if (elem_count == 0) {
            std::cerr << "Error: Element count must be greater than 0" << std::endl;
            return nullptr;
        }
        std::lock_guard<std::mutex> lock(mtx_);

        if (is_continuous) {
            // 申请连续内存：从空闲连续块中找合适的块（首次适配）
            return allocate_continuous(elem_count);
        } else {
            // 申请非连续内存：从空闲链表中取单个节点，数量elem_count则取elem_count个节点
            return allocate_discontinuous(elem_count);
        }
    }

    /**
     * @brief 释放内存接口
     * @param addr 需要释放的内存首地址
     * @return 释放成功返回true，失败返回false
     */
    bool free(void* addr) {
        if (addr == nullptr) {
            std::cerr << "Error: Free null pointer" << std::endl;
            return false;
        }
        std::lock_guard<std::mutex> lock(mtx_);

        // 先检查是否是连续内存块
        if (free_continuous_block(addr)) {
            return true;
        }
        // 再检查是否是非连续内存块
        if (free_discontinuous_block(addr)) {
            return true;
        }

        std::cerr << "Error: Address " << addr << " not managed by this memory pool" << std::endl;
        return false;
    }

    // 打印内存池当前使用情况（调试用）
    void print_status() const {
        std::lock_guard<std::mutex> lock(mtx_);
        size_t total_elem = 0;
        size_t used_elem = 0;
        for (const auto& block : all_blocks_) {
            total_elem += block.elem_count;
            if (block.is_used) {
                used_elem += block.elem_count;
            }
        }
        // 计算非连续空闲节点数量
        size_t free_discontinuous = 0;
        FreeNode* cur = free_list_;
        while (cur) {
            free_discontinuous++;
            cur = cur->next;
        }
        // 计算连续空闲块总元素数
        size_t free_continuous = 0;
        for (const auto& block : all_blocks_) {
            if (!block.is_used) {
                free_continuous += block.elem_count;
            }
        }

        std::cout << "===== MemoryPool Status (Type: " << typeid(T).name() << ") =====" << std::endl;
        std::cout << "Total elements capacity: " << total_elem << std::endl;
        std::cout << "Used elements: " << used_elem << std::endl;
        std::cout << "Free elements (continuous): " << free_continuous << std::endl;
        std::cout << "Free elements (discontinuous): " << free_discontinuous << std::endl;
        std::cout << "Memory usage rate: " << (total_elem == 0 ? 0 : (double)used_elem / total_elem * 100) << "%" << std::endl;
        std::cout << "================================" << std::endl;
    }

private:
    // 定时检查并扩充内存的后台线程函数
    void check_and_expand() {
        while (!stop_check_thread_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(check_interval_ms_));
            std::lock_guard<std::mutex> lock(mtx_);
            // 计算当前内存使用率
            size_t total_elem = 0;
            size_t used_elem = 0;
            for (const auto& block : all_blocks_) {
                total_elem += block.elem_count;
                if (block.is_used) {
                    used_elem += block.elem_count;
                }
            }
            if (total_elem == 0) continue;
            float usage = (float)used_elem / total_elem;
            // 使用率超过阈值则扩充（扩充当前已用的50%）
            if (usage >= usage_threshold_) {
                size_t expand_elem = used_elem * 0.5;
                if (expand_elem < 1) expand_elem = 1;
                do_expand(expand_elem);
                std::cout << "Timed check: Memory usage " << usage * 100 << "% >= " 
                          << usage_threshold_ * 100 << "%, expand " << expand_elem << " elements" << std::endl;
            }
        }
    }

    // 内存扩充对外接口（加锁后调用do_expand）
    void expand(size_t elem_count) {
        std::lock_guard<std::mutex> lock(mtx_);
        do_expand(elem_count);
    }

    // 实际执行内存扩充的核心函数（需在加锁后调用）
    void do_expand(size_t elem_count) {
        // 计算需要分配的总字节数（按对齐要求）
        size_t total_bytes = elem_count * elem_size_;
        // 按C++17对齐分配内存，兼容低版本可替换为posix_memalign（Linux）/_aligned_malloc（Windows）
        void* mem = ::operator new(total_bytes, std::align_val_t(elem_align_));
        if (mem == nullptr) {
            throw std::bad_alloc();
        }
        // 初始化内存块并加入总块列表
        all_blocks_.emplace_back(mem, elem_count, elem_align_);
        MemoryBlock& new_block = all_blocks_.back();
        // 将新扩充的内存拆分为非连续空闲节点，加入空闲链表（供非连续申请使用）
        split_to_free_list(&new_block);
        std::cout << "Memory pool expanded: " << elem_count << " elements (" 
                  << total_bytes / 1024.0 << "KB) allocated at " << mem << std::endl;
    }

    // 将连续内存块拆分为单个T对象的空闲节点，加入空闲链表（需加锁）
    void split_to_free_list(MemoryBlock* block) {
        char* base = static_cast<char*>(block->start_addr);
        for (size_t i = 0; i < block->elem_count; ++i) {
            void* elem_addr = base + i * elem_size_;
            // 头插法加入空闲链表（效率更高）
            free_list_ = new FreeNode(elem_addr, free_list_);
        }
    }

    // 分配连续内存（需加锁）：首次适配算法，找第一个能容纳elem_count的空闲连续块
    void* allocate_continuous(size_t elem_count) {
        for (auto& block : all_blocks_) {
            if (!block.is_used && block.elem_count >= elem_count) {
                block.is_used = true;
                // 若块大小大于需求，拆分剩余部分为新的空闲块
                if (block.elem_count > elem_count) {
                    char* new_base = static_cast<char*>(block.start_addr) + elem_count * elem_size_;
                    size_t new_elem = block.elem_count - elem_count;
                    all_blocks_.emplace_back(new_base, new_elem, block.align);
                    // 拆分剩余部分为非连续空闲节点
                    split_to_free_list(&all_blocks_.back());
                    // 更新原块的大小
                    block.elem_count = elem_count;
                }
                return block.start_addr;
            }
        }
        // 无合适块，触发自动扩充
        std::cout << "No enough continuous memory, expand " << elem_count << " elements" << std::endl;
        do_expand(elem_count);
        // 扩充后再次尝试分配
        return allocate_continuous(elem_count);
    }

    // 分配非连续内存（需加锁）：从空闲链表取elem_count个节点，返回第一个节点地址
    void* allocate_discontinuous(size_t elem_count) {
        FreeNode** cur = &free_list_;
        size_t count = 0;
        void* first_addr = nullptr;
        FreeNode* prev = nullptr;

        // 遍历空闲链表，取elem_count个连续节点
        while (*cur != nullptr && count < elem_count) {
            if (first_addr == nullptr) {
                first_addr = (*cur)->addr;
            }
            prev = *cur;
            cur = &(*cur)->next;
            count++;
        }

        // 找到足够的节点
        if (count == elem_count) {
            // 从空闲链表中移除这些节点
            if (prev != nullptr) {
                prev->next = nullptr;
            }
            // 释放这些节点的结构体（内存保留供使用，仅释放链表节点）
            FreeNode* del = free_list_;
            free_list_ = *cur;
            while (del != nullptr) {
                FreeNode* next = del->next;
                delete del;
                del = next;
            }
            return first_addr;
        }

        // 无足够的空闲节点，触发自动扩充
        std::cout << "No enough discontinuous memory, expand " << elem_count << " elements" << std::endl;
        do_expand(elem_count);
        // 扩充后再次尝试分配
        return allocate_discontinuous(elem_count);
    }

    // 释放连续内存块（需加锁）：找到对应块并标记为未使用，尝试归并相邻空闲块
    bool free_continuous_block(void* addr) {
        for (auto it = all_blocks_.begin(); it != all_blocks_.end(); ++it) {
            if (it->start_addr == addr && it->is_used) {
                it->is_used = false;
                // 归并相邻的空闲块，减少内存碎片
                merge_adjacent_blocks(it);
                return true;
            }
        }
        return false;
    }

    // 释放非连续内存块（需加锁）：将地址重新加入空闲链表
    bool free_discontinuous_block(void* addr) {
        // 检查地址是否属于当前内存池的管理范围
        if (!is_addr_in_pool(addr)) {
            return false;
        }
        // 头插法加入空闲链表，供后续复用
        free_list_ = new FreeNode(addr, free_list_);
        return true;
    }

    // 检查地址是否在内存池的管理范围内（需加锁）
    bool is_addr_in_pool(void* addr) const {
        char* target = static_cast<char*>(addr);
        for (const auto& block : all_blocks_) {
            char* start = static_cast<char*>(block.start_addr);
            char* end = start + block.elem_count * elem_size_;
            if (target >= start && target < end) {
                // 检查是否是对齐的T对象地址
                if ((target - start) % elem_size_ == 0) {
                    return true;
                }
                break;
            }
        }
        return false;
    }

    // 归并相邻的空闲内存块（需加锁）：减少内存碎片，提升连续内存申请效率
    void merge_adjacent_blocks(std::list<MemoryBlock>::iterator target_it) {
        MemoryBlock& target = *target_it;
        char* target_start = static_cast<char*>(target.start_addr);
        char* target_end = target_start + target.elem_count * elem_size_;

        // 遍历所有块，寻找可归并的前驱和后继块
        for (auto it = all_blocks_.begin(); it != all_blocks_.end();) {
            if (it == target_it) {
                ++it;
                continue;
            }
            MemoryBlock& block = *it;
            if (!block.is_used && block.align == target.align) {
                char* block_start = static_cast<char*>(block.start_addr);
                char* block_end = block_start + block.elem_count * elem_size_;
                // 后继块：当前块的结束地址 == 目标块的开始地址
                if (block_end == target_start) {
                    // 归并后继块到目标块
                    target.elem_count += block.elem_count;
                    target.start_addr = block.start_addr;
                    it = all_blocks_.erase(it);
                    target_start = static_cast<char*>(target.start_addr);
                }
                // 前驱块：目标块的结束地址 == 当前块的开始地址
                else if (target_end == block_start) {
                    // 归并目标块到前驱块
                    target.elem_count += block.elem_count;
                    target_end = block_end;
                    it = all_blocks_.erase(it);
                } else {
                    ++it;
                }
            } else {
                ++it;
            }
        }
    }

private:
    const size_t elem_size_;          // 单个T类型对象的大小（sizeof(T)）
    const size_t elem_align_;         // T类型对象的对齐要求（alignof(T)）
    size_t check_interval_ms_;        // 定时检查的时间间隔（毫秒）
    float usage_threshold_;           // 触发定时扩充的内存使用率阈值
    std::list<MemoryBlock> all_blocks_; // 管理所有内存块（连续/非连续的基础）
    FreeNode* free_list_ = nullptr;   // 空闲非连续块链表（单个T对象）
    mutable std::mutex mtx_;          // 互斥锁，保证线程安全
    std::thread check_thread_;        // 定时检查内存的后台线程
    std::atomic<bool> stop_check_thread_; // 停止后台线程的标志（原子变量）
};

// 测试示例
int test() {
    try {
        // 初始化内存池：预分配10个int类型对象，3秒检查一次，使用率阈值70%
        MemoryPool<int> mp(10, 3000, 0.7f);
        mp.print_status();

        // 1. 申请非连续内存：5个int（非连续）
        void* p1 = mp.get(5, false);
        std::cout << "Allocate discontinuous 5 int: " << p1 << std::endl;
        mp.print_status();

        // 2. 申请连续内存：8个int（连续）
        void* p2 = mp.get(8, true);
        std::cout << "Allocate continuous 8 int: " << p2 << std::endl;
        mp.print_status();

        // 3. 申请超大连续内存：20个int（触发自动扩充）
        void* p3 = mp.get(20, true);
        std::cout << "Allocate continuous 20 int (expand): " << p3 << std::endl;
        mp.print_status();

        // 4. 释放内存
        mp.free(p1);
        std::cout << "Free discontinuous 5 int: " << p1 << std::endl;
        mp.free(p2);
        std::cout << "Free continuous 8 int: " << p2 << std::endl;
        mp.print_status();

        // 5. 重新申请内存（复用释放的内存）
        void* p4 = mp.get(5, false);
        std::cout << "Reallocate discontinuous 5 int (reuse): " << p4 << std::endl;
        void* p5 = mp.get(8, true);
        std::cout << "Reallocate continuous 8 int (reuse): " << p5 << std::endl;
        mp.print_status();

        // 6. 释放最后申请的内存
        mp.free(p3);
        mp.free(p4);
        mp.free(p5);
        std::cout << "Free all remaining memory" << std::endl;
        mp.print_status();

        // 等待定时检查触发（3秒），观察自动扩充
        std::cout << "Wait for timed check (3 seconds)..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(4));
        mp.print_status();

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}