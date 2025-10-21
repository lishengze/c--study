#pragma once

#include <atomic>
#include <cassert>
#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include <string>

#include "bits.h"
#include "logger.h"
#include "spin_lock.h"


/**
 * @file mpmc_queue.h
 * @brief 多生产者多消费者(MPMC)无锁队列实现
 * @details 基于环形缓冲区和ticket机制实现的无锁队列，支持多线程并发读写
 *          采用原子操作和内存屏障保证线程安全，通过缓存行对齐减少伪共享
 *          支持两种模式：普通模式和用户控制容量溢出模式
 */
namespace share_common 
{

/**
 * @brief 前置声明元素槽模板类
 * @tparam T 存储在队列中的元素类型
 * @tparam users_exceeds_capacity 是否启用用户控制的容量溢出模式
 */
template <typename T, bool users_exceeds_capacity>
class element_slot;


/**
 * @brief 多生产者多消费者无锁队列类
 * @tparam T 队列中元素的类型
 * @tparam users_exceeds_capacity 当为true时启用自旋锁保护，适用于可能超出容量的场景
 * @note 采用环形缓冲区结构，使用ticket机制实现无锁同步
 *       队列大小会自动调整为2的幂次方，以优化取模运算
 */
template <typename T, bool users_exceeds_capacity = false>
class mpmc_queue 
{
public:
    typedef element_slot<T, users_exceeds_capacity> slot_type;

    /**
     * @brief 默认构造函数
     * @note 队列不会被自动初始化，需要显式调用create()方法
     */
    mpmc_queue() = default;
    
    /**
     * @brief 禁用拷贝构造函数
     */
    mpmc_queue(const mpmc_queue&) = delete;
    
    /**
     * @brief 禁用赋值运算符
     */
    mpmc_queue& operator=(const mpmc_queue&) = delete;

    /**
     * @brief 析构函数
     * @note 释放环形缓冲区占用的内存
     */
    ~mpmc_queue() 
    {
        // printf("delete[] slots_;");
        if (!slots_) {
            delete[] slots_;
        } else {
            printf("Slots Has Been Released!\n");
        }  
    }

    /**
     * @brief 初始化队列
     * @param size 队列容量（实际容量会调整为最接近的2的幂次方）
     * @return 成功初始化返回true，否则返回false
     * @note 队列大小会被调整为size+1的最小幂次方值，以优化索引计算
     */
    bool create(uint32_t size)
    {
        size = size ? size : 1; 
        // 将队列大小调整为大于等于size+1的最小2的幂次方，便于使用位运算优化
        size = roundup_pow_of_two(size + 1);
        // 分配元素槽数组，注意可能存在的伪共享问题
        slots_ = new slot_type[size];
        if (!slots_) 
        {
            LOG_ERROR("malloc failed!");
            return false;
        }
        
        // 计算掩码和位掩码，用于快速取模和移位操作
        mask_ = size - 1;
        bit_mask_ = __builtin_ctz((uint64_t)size);
        // 步长值，用于哈希计算索引，减少冲突
        stride_ = 1;
        // 初始化生产者和消费者ticket
        push_ticket_ = 0;
        pop_ticket_ = 0;
        
        // MPMC_OVERFLOW宏定义时启用溢出模式
        #ifdef MPMC_OVERFLOW
        push_ticket_ = (uint64_t(UINT32_MAX - 1)) * size / 2;
        pop_ticket_ = (uint64_t(UINT32_MAX - 1)) * size / 2;
        for (uint32_t i = 0; i < size; ++i)
        {
            slots_[i].init(UINT32_MAX - 1); // 初始化所有槽的turn值
        }
        #endif
        
        LOG_INFO("queue init successed, capacity is {}", mask_ + 1);
        return true;
    }
    

    /// @brief 将slot 映射到外部的内存地址中 -- 共享内存版本；
    /// @param size 
    /// @param buffer 
    /// @return 
    bool create_shared(uint32_t size, void* buffer)
    {
        size = size ? size : 1;
        size = roundup_pow_of_two(size + 1);
        
        // 直接使用传入的共享内存地址作为slots_
        slots_ = static_cast<slot_type*>(buffer);
        
        // 初始化所有元素槽（如果需要）
        for (uint32_t i = 0; i < size; ++i) {
            new (&slots_[i]) slot_type();
        }
        
        mask_ = size - 1;
        bit_mask_ = __builtin_ctz((uint64_t)size);
        stride_ = 1;
        push_ticket_ = 0;
        pop_ticket_ = 0;
        
        // MPMC_OVERFLOW宏定义时启用溢出模式
        #ifdef MPMC_OVERFLOW
        push_ticket_ = (uint64_t(UINT32_MAX - 1)) * size / 2;
        pop_ticket_ = (uint64_t(UINT32_MAX - 1)) * size / 2;
        for (uint32_t i = 0; i < size; ++i)
        {
            slots_[i].init(UINT32_MAX - 1); // 初始化所有槽的turn值
        }
        #endif
        
        LOG_INFO("queue init successed, capacity is {}", mask_ + 1);
        
        return true;
    }


    bool slot_attach(void* buffer) {
        slots_ = static_cast<slot_type*>(buffer);
        return true;
    }


    /**
     * @brief 根据ticket计算元素槽索引
     * @param ticket 当前操作的ticket值
     * @return 元素槽在环形缓冲区中的索引
     * @note 使用哈希算法(ticket * stride_) % size避免缓存冲突
     */
    size_t idx(uint64_t ticket)
    {
        return (ticket * stride_) & mask_;
    }
    
    /**
     * @brief 从ticket中提取turn值
     * @param ticket 当前操作的ticket值
     * @return turn值，用于判断元素槽状态
     * @note 通过右移bit_mask_位实现除法操作，获取循环次数
     */
    uint32_t turn(uint64_t ticket)
    {
        return uint32_t(ticket >> bit_mask_);
    }

    /**
     * @brief 尝试入队（非阻塞）
     * @tparam Args 可变参数模板
     * @param args 元素构造函数参数
     * @return 成功入队返回true，队列满时返回false
     * @note 非阻塞版本，使用CAS操作尝试获取入队权限
     */
    template <class... Args>
    bool trypush(Args&&... args)
    {
        // 获取当前生产者ticket
        uint64_t ticket = push_ticket_.load(std::memory_order_acquire);
        auto index = idx(ticket);
        auto cur_turn = turn(ticket);
        // 检查元素槽是否可入队
        if (slots_[index].may_enqueue(cur_turn))
        {
            // CAS操作尝试更新ticket，成功则执行入队
            if (push_ticket_.compare_exchange_strong(ticket, ticket + 1))
            {
                slots_[index].pre_validated_enqueue(std::forward<Args>(args)...);
                return true;
            }
        }
        return false;
    }

    /**
     * @brief 入队（阻塞）
     * @tparam Args 可变参数模板
     * @param args 元素构造函数参数
     * @note 阻塞版本，会一直等待直到入队成功
     */
    template <class... Args>
    int push(Args&&... args)
    {
        // printf("-------- mpmc_queue push\n");
        // 获取并自增生产者ticket
        uint64_t ticket = push_ticket_++;
        auto index = idx(ticket);  // slot 位置;
        auto cur_turn = turn(ticket);
        // printf("-------- mpmc_queue push ticket: %ld, index: %ld, cur_turn: %d\n", ticket, index, cur_turn);
        // 执行入队操作，可能阻塞
        LOG_DEBUG("cur_turn: {}, index: {}, push_ticket: {}", cur_turn, index, push_ticket_);
        slots_[index].enqueue(cur_turn, std::forward<Args>(args)...);
        return index;
    }

    /**
     * @brief 尝试出队（非阻塞）
     * @param element 用于存储出队元素的引用
     * @return 成功出队返回true，队列为空返回false
     * @note 非阻塞版本，使用CAS操作尝试获取出队权限
     */
    bool trypop(T& element)
    {
        // 获取当前消费者ticket
        uint64_t ticket = pop_ticket_.load(std::memory_order_acquire);
        auto index = idx(ticket);
        auto cur_turn = turn(ticket);
        LOG_DEBUG("cur_turn: {}, index: {}, pop_ticket: {}", cur_turn, index, pop_ticket_);
        // 检查元素槽是否可出队
        if (slots_[index].may_dequeue(cur_turn))
        {
            // CAS操作尝试更新ticket，成功则执行出队
            if (pop_ticket_.compare_exchange_strong(ticket, ticket + 1))
            {
                slots_[index].pre_validated_dequeue(element);
                return true;
            }
        }
        return false;
    }

    /**
     * @brief 出队（阻塞）
     * @param element 用于存储出队元素的引用
     * @note 阻塞版本，会一直等待直到出队成功
     */
    int pop(T& element)
    {
        // 获取并自增消费者ticket
        uint64_t ticket = pop_ticket_++;
        auto index = idx(ticket);
        auto cur_turn = turn(ticket); 
        // 执行出队操作，可能阻塞
        slots_[index].dequeue(cur_turn, element);
        return index;
    }


public: // 用于测试;
    // 缓存行填充，避免伪共享
    L2CACHE_PADDING(pad0_);
    // 位掩码，用于计算turn值
    uint32_t bit_mask_;
    // 索引掩码，用于计算数组索引
    uint32_t mask_;
    // 步长值，用于哈希计算
    int stride_;
    // 元素槽数组
    slot_type* slots_;

    // 生产者ticket，使用缓存行对齐避免伪共享
    alignas(L2_CACHE_LINE) std::atomic<uint64_t> push_ticket_;
    // 消费者ticket，使用缓存行对齐避免伪共享
    alignas(L2_CACHE_LINE) std::atomic<uint64_t> pop_ticket_;

    // 缓存行填充
    L2CACHE_PADDING(pad1_);

};


/**
 * @brief 元素槽类，管理单个元素的存储和状态
 * @tparam T 元素类型
 * @tparam users_exceeds_capacity 是否启用多线程安全模式
 * @note 用于跟踪元素的入队/出队状态，实现无锁同步
 */
template <typename T, bool users_exceeds_capacity>
class element_slot
{
public:
    /**
     * @brief 构造函数
     * @note 初始化当前turn值为0
     */
element_slot():current_turn_(0){}

    /**
     * @brief 初始化元素槽状态
     * @param turn 初始turn值
     */
    void init(uint32_t turn){current_turn_ = turn;}
    
    /**
     * @brief 检查是否可以入队
     * @param turn 当前操作的turn值
     * @return 可以入队返回true，否则返回false
     * @note 当current_turn_等于turn << 1时表示可以入队
     */
    bool may_enqueue(uint32_t turn)
    {
        auto cur_turn = current_turn_.load(std::memory_order_acquire);
        return cur_turn == (turn << 1);
    }

    /**
     * @brief 检查是否可以出队
     * @param turn 当前操作的turn值
     * @return 可以出队返回true，否则返回false
     * @note 当current_turn_等于(turn << 1) + 1时表示可以出队
     */
    bool may_dequeue(uint32_t turn)
    {
        auto cur_turn = current_turn_.load(std::memory_order_acquire);
        return cur_turn == ((turn << 1) + 1);
    }

    /**
     * @brief 入队操作
     * @tparam Args 可变参数模板
     * @param turn 当前操作的turn值
     * @param args 元素构造函数参数
     * @note 阻塞等待直到可以入队，然后构造元素并更新状态
     */
    template <class... Args>
    void enqueue(uint32_t turn, Args&&... args)
    {
        auto cur_turn = current_turn_.load(std::memory_order_acquire);  

        while (cur_turn != (turn << 1))
        {
            cur_turn = current_turn_.load(std::memory_order_acquire);
        }
        // 在元素存储位置原地构造对象
        new (&element_) T(std::forward<Args>(args)...);
        // 更新状态为已入队，使用release内存序
        current_turn_.store(cur_turn + 1, std::memory_order_release);
    }

    /**
     * @brief 预验证入队操作
     * @tparam Args 可变参数模板
     * @param args 元素构造函数参数
     * @note 假设已通过may_enqueue检查，直接执行入队
     */
    template <class... Args>
    void pre_validated_enqueue(Args&&... args)
    {
        auto cur_turn = current_turn_.load(std::memory_order_relaxed);
        new (&element_) T(std::forward<Args>(args)...);
        current_turn_.store(cur_turn + 1, std::memory_order_release);
    }

    /**
     * @brief 出队操作
     * @param turn 当前操作的turn值
     * @param element 用于存储出队元素的引用
     * @note 阻塞等待直到可以出队，然后移动元素并销毁原对象
     */
    void dequeue(uint32_t turn, T& element)
    {
        auto cur_turn = current_turn_.load(std::memory_order_acquire);
        // 等待直到可以出队
        while (cur_turn != ((turn << 1) + 1))
        {
            cur_turn = current_turn_.load(std::memory_order_acquire);
        }
        // 移动元素
        element = std::move(element_);
        // 销毁原对象
        element_.~T();
        // 更新状态为已出队，使用release内存序
        current_turn_.store(cur_turn + 1, std::memory_order_release);
    }

    /**
     * @brief 预验证出队操作
     * @param element 用于存储出队元素的引用
     * @note 假设已通过may_dequeue检查，直接执行出队
     */
    void pre_validated_dequeue(T& element)
    {
        auto cur_turn = current_turn_.load(std::memory_order_relaxed);
        element = std::move(element_);
        element_.~T();
        current_turn_.store(cur_turn + 1, std::memory_order_release);
    }

private:
    // 元素存储位置
    T element_;
    // 当前turn值，用于跟踪元素状态
    std::atomic<uint32_t> current_turn_;
};

/**
 * @brief 多线程安全模式的元素槽特化版本
 * @tparam T 元素类型
 * @note 当users_exceeds_capacity为true时使用此特化版本
 *       增加了自旋锁保护，适用于可能超出容量的场景
 */
template <typename T>
class element_slot<T, true>
{
public:
    /**
     * @brief 构造函数
     * @note 初始化turn值和自旋锁
     */
element_slot():current_turn_(0),spin_lock_obj_(){}
    
    /**
     * @brief 初始化元素槽状态
     * @param turn 初始turn值
     */
    void init(uint32_t turn){current_turn_ = turn;}
    
    /**
     * @brief 检查是否可以入队
     * @param turn 当前操作的turn值
     * @return 可以入队返回true，否则返回false
     */
    bool may_enqueue(uint32_t turn)
    {
        auto cur_turn = current_turn_.load(std::memory_order_acquire);
        return cur_turn == (turn << 1);
    }

    /**
     * @brief 检查是否可以出队
     * @param turn 当前操作的turn值
     * @return 可以出队返回true，否则返回false
     */
    bool may_dequeue(uint32_t turn)
    {
        auto cur_turn = current_turn_.load(std::memory_order_acquire);
        return cur_turn == ((turn << 1) + 1);
    }

    /**
     * @brief 入队操作（带自旋锁保护）
     * @tparam Args 可变参数模板
     * @param turn 当前操作的turn值
     * @param args 元素构造函数参数
     * @note 使用自旋锁确保多线程安全
     */
    template <class... Args>
    void enqueue(uint32_t turn, Args&&... args)
    {
        // printf("+++++++ enqueue 2222\n");
        auto cur_turn = current_turn_.load(std::memory_order_acquire);
        while (cur_turn != (turn << 1))
        {
            cur_turn = current_turn_.load(std::memory_order_acquire);
        }

        // printf("+++++++ enqueue cur_turn: %d\n", cur_turn);
        
        // 使用自旋锁保护临界区
        { 
            lock_guard<spin_lock> lock(spin_lock_obj_);
            current_turn_.store(cur_turn + 1, std::memory_order_relaxed); // RELEASE ?
            new (&element_) T(std::forward<Args>(args)...);
        }
    }

    /**
     * @brief 预验证入队操作（带自旋锁保护）
     * @tparam Args 可变参数模板
     * @param args 元素构造函数参数
     */
    template <class... Args>
    void pre_validated_enqueue(Args&&... args)
    {
        // 使用自旋锁保护临界区
        { 
            lock_guard<spin_lock> lock(spin_lock_obj_);
            auto cur_turn = current_turn_.load(std::memory_order_relaxed);
            current_turn_.store(cur_turn + 1, std::memory_order_relaxed); // RELEASE ?
            new (&element_) T(std::forward<Args>(args)...);
        }
    }

    /**
     * @brief 出队操作（带自旋锁保护）
     * @param turn 当前操作的turn值
     * @param element 用于存储出队元素的引用
     */
    void dequeue(uint32_t turn, T& element)
    {
        auto cur_turn = current_turn_.load(std::memory_order_acquire);
        while (cur_turn != ((turn << 1) + 1))
        {
            cur_turn = current_turn_.load(std::memory_order_acquire);
        }
        
        // 使用自旋锁保护临界区
        { 
            lock_guard<spin_lock> lock(spin_lock_obj_);
            current_turn_.store(cur_turn + 1, std::memory_order_relaxed); // RELEASE ?
            element = std::move(element_);
            element_.~T();
        }
    }

    /**
     * @brief 预验证出队操作（带自旋锁保护）
     * @param element 用于存储出队元素的引用
     */
    void pre_validated_dequeue(T& element)
    {
        // 使用自旋锁保护临界区
        { 
            lock_guard<spin_lock> lock(spin_lock_obj_);
            auto cur_turn = current_turn_.load(std::memory_order_relaxed);
            current_turn_.store(cur_turn + 1, std::memory_order_relaxed); // RELEASE ?
            element = std::move(element_);
            element_.~T();
        }
    }

private:
    // 元素存储位置
    T element_;
    // 当前turn值
    std::atomic<uint32_t> current_turn_;
    // 自旋锁，用于多线程同步
    spin_lock spin_lock_obj_;
};


}   // namespace tech