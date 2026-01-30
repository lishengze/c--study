#include <cstdlib>
#include <utility>  // for std::forward
#include <cassert>  // for assert
#include <iostream> // for std::cout
#include <cstddef>  // for offsetof
#include <vector>

template <typename T>
class ListMemoryPool {
private:
    // 内存块节点：包含空闲内存 + 下一个块的指针（单链表）
    struct BlockNode {
        BlockNode() : next_(nullptr), bUsed_(false) {
            T* tmpObj = new (data) T();  //  placement new 构造对象，申请内存时便构造对象

            std::cout << "BlockNode allocate  address: " << (void*)(this) << std::endl;
        }
        bool        bUsed_;  // 标记该块是否已被分配;
        BlockNode*  next_;   // 指向下一个空闲块                
        // alignas(T) char data[sizeof(T)];  // 存储 T 对象的内存（按 T 对齐）

        char data[sizeof(T)];

        // ~B
        // T data;
    };

    BlockNode* free_list_;       // 空闲内存块链表头
    const size_t kBlockNum;      // 每次扩容时预分配的块数量（可配置）
    size_t allocated_count_;     // 已分配的对象数量
    size_t free_count_;          // 空闲块数量

    std::vector<BlockNode*> allocated_blocks_;  // 已分配的块指针（用于析构时释放）;

    // 辅助函数：从系统分配 n 个块，添加到空闲链表
    void AllocateBlocks(size_t n) {bool bUsed_;  // 标记该块是否已被分配;
        assert(n > 0);
        // 一次性分配 n 个 BlockNode 的连续内存（减少 malloc 调用）
        BlockNode* new_blocks = new BlockNode[n];

        allocated_blocks_.push_back(new_blocks);  // 记录已分配的块指针


        assert(new_blocks != nullptr && "ListMemoryPool: malloc failed!");

        // for (int i = 0; i < n; ++i) {
            
        //     new_blocks[i].bUsed_ = false;
        //     new_blocks[i].next_ = nullptr;

        //     std::cout << "list memroy pool: allocate " << i << ", address: " << (void*)(&(new_blocks[i])) << std::endl;
        // }

        // 将新分配的块串联成链表
        for (size_t i = 0; i < n - 1; ++i) {
            
            new_blocks[i].next_ = &new_blocks[i + 1];
        }
        new_blocks[n - 1].next_ = free_list_;  // 最后一个块指向原空闲链表头
        free_list_ = new_blocks;              // 更新空闲链表头
        free_count_ += n;                     // 增加空闲块计数
    }

public:
    // 构造函数：指定每次扩容的块数量（默认 10 个）
    explicit ListMemoryPool(size_t pre_allocate_num = 3)
        : free_list_(nullptr)
        , kBlockNum(pre_allocate_num)
        , allocated_count_(0)
        , free_count_(0) {
        // 初始化时预分配一批块（可选，也可延迟到第一次 allocate 时）
        std::cout << "Block.size: " << sizeof(BlockNode) << std::endl;
        AllocateBlocks(kBlockNum);

        // Release();

    }

    void Release() {

        for (auto block : allocated_blocks_) {
            delete[] block;
        }

        // // 释放所有内存（必须确保所有对象已 Deallocate）
        // std::cout << "ListMemoryPool: Release all memory" << std::endl;

        // assert(allocated_count_ == 0 && "ListMemoryPool: There are still allocated objects!");
        // // 遍历所有空闲块链表，释放内存（因块是连续分配的，只需释放链表头）
        // BlockNode* current = free_list_;
        // int index = 0;
        // while (current != nullptr) {
        //     BlockNode* next_ = current->next_;
        //     std::cout << "ListMemoryPool: Free block " << index++ << ", "<< (void*)(current) << std::endl;

        //     delete current; // todo;

        //     // std::free(current);

        //     current = next_;            
        // }
        // free_list_ = nullptr;

    }

    // 析构函数：释放所有从系统分配的内存（必须确保所有对象已 Deallocate）
    ~ListMemoryPool() {
        Release();
    }

    // 禁止拷贝和移动（避免内存管理混乱）
    ListMemoryPool(const ListMemoryPool&) = delete;
    ListMemoryPool& operator=(const ListMemoryPool&) = delete;
    ListMemoryPool(ListMemoryPool&&) = delete;
    ListMemoryPool& operator=(ListMemoryPool&&) = delete;

    // 分配内存并原地构造对象（支持任意参数的构造函数）
    template <typename... Args>
    T* allocate(Args&&... args) {
        // 若无空闲块，扩容（预分配 kBlockNum 个块）
        if (free_list_ == nullptr) {
            AllocateBlocks(kBlockNum);
            std::cout << "ListMemoryPool: Allocate " << kBlockNum << " new blocks" << std::endl;
        }

        // 从空闲链表头部取出一个块
        BlockNode* block = free_list_;
        free_list_ = block->next_;  // 空闲链表头后移
        free_count_--;
        allocated_count_++;

        // 原地构造 T 对象（placement new）：在 block->data 地址上调用 T 的构造函数
        // std::forward 完美转发参数，支持左值/右值参数
        T* obj = new (block->data) T(std::forward<Args>(args)...);

        // 验证对象地址是否正确对齐（安全检查）
        assert(reinterpret_cast<void*>(obj) == reinterpret_cast<void*>(block->data) &&
               "ListMemoryPool: Object alignment error!");

        return obj;
    }

    T* get(size_t n) {
        assert(n > 0);
        
        // 检查空闲块数量，如果不足则扩容
        if (free_count_ < n) {
            // 计算需要分配的新块数量（至少满足当前需求）
            size_t need_blocks = n - free_count_;
            // 按kBlockNum的整数倍分配，避免频繁扩容
            size_t allocate_blocks = ((need_blocks + kBlockNum - 1) / kBlockNum) * kBlockNum;
            AllocateBlocks(allocate_blocks);
            std::cout << "ListMemoryPool: Allocate " << allocate_blocks << " new blocks for batch allocation" << std::endl;
        }

        // 保存第一个块，用于返回
        BlockNode* first_block = free_list_;
        BlockNode* current_block = first_block;

        // 从空闲链表中取出n个块
        for (size_t i = 0; i < n; ++i) {
            current_block->bUsed_ = true;
            BlockNode* next_block = current_block->next_;
            
            // 构造对象
            T* obj = new (current_block->data) T();
            
            // 验证对象地址是否正确
            assert(reinterpret_cast<void*>(obj) == reinterpret_cast<void*>(current_block->data) &&
                   "ListMemoryPool: Object alignment error!");
            
            current_block = next_block;
        }

        // current_block->next_ = nullptr; // 最后一个块的next_设为nullptr；

        // 更新空闲链表头和计数器
        free_list_ = current_block;
        free_count_ -= n;
        allocated_count_ += n;

        return reinterpret_cast<T*>(first_block->data);
    }

    // 析构对象并释放内存块（回收到空闲链表）
    bool free(T* obj) {
        if (obj == nullptr) {
            return false;  // 空指针直接返回失败
        }

        // 验证 obj 是内存池分配的地址（安全检查：确保地址属于某个 BlockNode 的 data 区域）
        BlockNode* cur_block_ptr = reinterpret_cast<BlockNode*>(
            reinterpret_cast<char*>(obj) - offsetof(BlockNode, data)
        );

        // 简单校验：确保 block 的 data 字段确实指向 obj（防止非法地址）
        if (reinterpret_cast<void*>(cur_block_ptr->data) != reinterpret_cast<void*>(obj)) {
            std::cerr << "ListMemoryPool: Invalid pointer to deallocate!" << std::endl;
            return false;
        }

        while(cur_block_ptr != nullptr && cur_block_ptr->bUsed_) {
            // 显式调用对象的析构函数（placement new 构造的对象需手动析构）
            T* tmpObj = reinterpret_cast<T*>(cur_block_ptr->data);
            tmpObj->~T();

            cur_block_ptr->bUsed_ = false;

            std::cout << "ListMemoryPool: Give Back block " << (void*)cur_block_ptr << std::endl;

            BlockNode* next_block = cur_block_ptr->next_;
            cur_block_ptr->next_ = free_list_;
            free_list_ = cur_block_ptr;
            cur_block_ptr = next_block;

            free_count_++;
            allocated_count_--;            
        }

        return true;
    }

    // 辅助接口：获取当前状态（调试用）
    size_t GetAllocatedCount() const { return allocated_count_; }
    size_t GetFreeCount() const { return free_count_; }
};

class TestObject4 {
public:
    TestObject4(int value = 0) : value_(value) {
        // std::cout << "TestObject4 constructed with value: " << value_ << std::endl;
    }
    
    ~TestObject4() {
        std::cout << "TestObject4 destructed with value: " << value_ << std::endl;
    }
    
    int getValue() const { return value_; }

private:
    int value_;
};

inline int TestMemoryPool4() {
    ListMemoryPool<TestObject4> pool;
    
    // 批量分配5个对象
    const int batch_size = 5;
    TestObject4* first_obj = pool.get(batch_size);

    std::cout << "FirstObj Address: " << first_obj << std::endl;

    
    // 批量释放对象
    bool result = pool.free(first_obj);
    // bool result = pool.deallocate_batch(&first_obj, batch_size);
    std::cout << "Batch deallocate result: " << (result ? "success" : "failed") << std::endl;
    

    
    return 0;
}