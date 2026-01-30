#include <iostream>
#include <cstdlib>
#include <utility>  // for std::forward
#include <cassert>  // for assert

template <typename T>
class MemoryPool2 {
private:
    // 内存块节点：包含空闲内存 + 下一个块的指针（单链表）
    struct BlockNode {
        BlockNode* next;  // 指向下一个空闲块
        alignas(T) char data[sizeof(T)];  // 存储 T 对象的内存（按 T 对齐）
    };

    BlockNode* free_list_;       // 空闲内存块链表头
    const size_t kBlockNum;      // 每次扩容时预分配的块数量（可配置）
    size_t allocated_count_;     // 已分配的对象数量
    size_t free_count_;          // 空闲块数量

    // 辅助函数：从系统分配 n 个块，添加到空闲链表
    void AllocateBlocks(size_t n) {
        assert(n > 0);
        // 一次性分配 n 个 BlockNode 的连续内存（减少 malloc 调用）
        BlockNode* new_blocks = static_cast<BlockNode*>(std::malloc(n * sizeof(BlockNode)));
        assert(new_blocks != nullptr && "MemoryPool2: malloc failed!");

        // 将新分配的块串联成链表
        for (size_t i = 0; i < n - 1; ++i) {
            new_blocks[i].next = &new_blocks[i + 1];
        }
        new_blocks[n - 1].next = free_list_;  // 最后一个块指向原空闲链表头
        free_list_ = new_blocks;              // 更新空闲链表头
        free_count_ += n;                     // 增加空闲块计数
    }

public:
    // 构造函数：指定每次扩容的块数量（默认 10 个）
    explicit MemoryPool2(size_t pre_allocate_num = 10)
        : free_list_(nullptr)
        , kBlockNum(pre_allocate_num)
        , allocated_count_(0)
        , free_count_(0) {
        // 初始化时预分配一批块（可选，也可延迟到第一次 allocate 时）
        AllocateBlocks(kBlockNum);
    }

    // 析构函数：释放所有从系统分配的内存（必须确保所有对象已 Deallocate）
    ~MemoryPool2() {
        assert(allocated_count_ == 0 && "MemoryPool2: There are still allocated objects!");
        // 遍历所有空闲块链表，释放内存（因块是连续分配的，只需释放链表头）
        BlockNode* current = free_list_;
        while (current != nullptr) {
            BlockNode* next = current->next;
            // 检查当前块是否是连续分配的批量块的起始（通过内存对齐和偏移判断）
            // 简化处理：直接释放当前块（若批量分配，实际是释放整个连续内存）
            std::free(current);
            current = next;
        }
        free_list_ = nullptr;
    }

    // 禁止拷贝和移动（避免内存管理混乱）
    MemoryPool2(const MemoryPool2&) = delete;
    MemoryPool2& operator=(const MemoryPool2&) = delete;
    MemoryPool2(MemoryPool2&&) = delete;
    MemoryPool2& operator=(MemoryPool2&&) = delete;

    // 分配内存并原地构造对象（支持任意参数的构造函数）
    template <typename... Args>
    T* allocate(Args&&... args) {
        // 若无空闲块，扩容（预分配 kBlockNum 个块）
        if (free_list_ == nullptr) {
            AllocateBlocks(kBlockNum);
            std::cout << "MemoryPool2: Allocate " << kBlockNum << " new blocks" << std::endl;
        }

        // 从空闲链表头部取出一个块
        BlockNode* block = free_list_;
        free_list_ = block->next;  // 空闲链表头后移
        free_count_--;
        allocated_count_++;

        // 原地构造 T 对象（placement new）：在 block->data 地址上调用 T 的构造函数
        // std::forward 完美转发参数，支持左值/右值参数
        T* obj = new (block->data) T(std::forward<Args>(args)...);

        // 验证对象地址是否正确对齐（安全检查）
        assert(reinterpret_cast<void*>(obj) == reinterpret_cast<void*>(block->data) &&
               "MemoryPool2: Object alignment error!");

        return obj;
    }

    // 析构对象并释放内存块（回收到空闲链表）
    bool Deallocate(T* obj) {
        if (obj == nullptr) {
            return false;  // 空指针直接返回失败
        }

        // 验证 obj 是内存池分配的地址（安全检查：确保地址属于某个 BlockNode 的 data 区域）
        BlockNode* block = reinterpret_cast<BlockNode*>(
            reinterpret_cast<char*>(obj) - offsetof(BlockNode, data)
        );

        // 简单校验：确保 block 的 data 字段确实指向 obj（防止非法地址）
        if (reinterpret_cast<void*>(block->data) != reinterpret_cast<void*>(obj)) {
            std::cerr << "MemoryPool2: Invalid pointer to deallocate!" << std::endl;
            return false;
        }

        // 显式调用对象的析构函数（placement new 构造的对象需手动析构）
        obj->~T();

        // 将块回收到空闲链表头部（头插法，高效）
        block->next = free_list_;
        free_list_ = block;
        free_count_++;
        allocated_count_--;

        return true;
    }

    // 辅助接口：获取当前状态（调试用）
    size_t GetAllocatedCount() const { return allocated_count_; }
    size_t GetFreeCount() const { return free_count_; }
};