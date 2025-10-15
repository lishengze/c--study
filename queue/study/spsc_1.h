#include <atomic>    // 用于原子操作，保证多线程安全
#include <cstddef>   // 用于size_t等标准类型
#include <memory>    // 用于std::unique_ptr智能指针
#include <stdexcept> // 用于标准异常处理

/*
## 实现原理解析

`SPSCQueue` 是一个单生产者-单消费者模式的无锁队列实现，具有以下关键技术点：

1. **环形缓冲区设计**：
   - 使用数组实现环形缓冲区，通过取模运算 `%` 实现索引的自动环绕
   - 故意浪费一个位置（`capacity_ = capacity + 1`），用于区分队列满和空的状态

2. **无锁同步机制**：
   - 使用两个原子变量 `head_` 和 `tail_` 分别跟踪消费者读取位置和生产者写入位置
   - 通过内存序（memory order）控制多线程间的内存访问顺序，而非互斥锁
   - `relaxed`：仅保证原子性，不建立同步关系
   - `acquire`：确保后续读取操作不会被重排到此操作之前
   - `release`：确保之前的写入操作不会被重排到此操作之后

3. **线程安全性保证**：
   - 仅允许一个生产者和一个消费者线程访问，避免了多生产者或多消费者的竞争条件
   - 生产者只更新 `tail_`，消费者只更新 `head_`，避免了同一变量的写竞争
   - 通过内存序控制确保读写操作的可见性和顺序性

4. **边界条件处理**：
   - 队列空：`head_ == tail_`
   - 队列满：`(tail_ + 1) % capacity_ == head_`
   - 元素计数：根据 `head_` 和 `tail_` 的相对位置计算

这个实现在单生产者-单消费者场景下可以实现无锁高效的线程间通信，避免了互斥锁带来的性能开销。
*/

// 单生产者-单消费者无锁队列
// 特点：仅支持一个生产者线程和一个消费者线程并发访问，无需互斥锁即可保证线程安全
template <typename T> // T 为队列中存储的元素类型
class SPSCQueue {
public:
    // 构造函数，指定队列容量
    explicit SPSCQueue(size_t capacity) 
        : capacity_(capacity + 1),  // 额外增加一个位置用于区分满和空状态
          buffer_(std::make_unique<T[]>(capacity + 1)), // 分配队列缓冲区内存
          head_(0), // 初始化消费者读取位置为0
          tail_(0) {} // 初始化生产者写入位置为0

    // 禁止拷贝和移动操作，确保队列的唯一性和线程安全性
    SPSCQueue(const SPSCQueue&) = delete;
    SPSCQueue& operator=(const SPSCQueue&) = delete;
    SPSCQueue(SPSCQueue&&) = delete;
    SPSCQueue& operator=(SPSCQueue&&) = delete;

    // 入队操作，由生产者线程调用
    // 参数：item - 要入队的元素
    // 返回值：true表示入队成功，false表示队列已满
    bool enqueue(const T& item) {
        // 以relaxed内存序读取尾指针，不需要与其他线程同步
        const size_t current_tail = tail_.load(std::memory_order_relaxed);
        // 计算下一个写入位置，使用模运算实现环形缓冲区
        const size_t next_tail = (current_tail + 1) % capacity_;

        // 检查队列是否已满（通过检查下一个尾指针是否与头指针重合）
        // 使用acquire内存序读取头指针，确保能看到消费者最新的修改
        if (next_tail == head_.load(std::memory_order_acquire)) {
            return false;  // 队列满，入队失败
        }

        // 存入数据（此时消费者无法访问该位置，因为head_未更新）
        buffer_[current_tail] = item;
        // 使用release内存序更新尾指针，确保之前的写入对消费者可见
        tail_.store(next_tail, std::memory_order_release);
        return true;  // 入队成功
    }

    // 出队操作，由消费者线程调用
    // 参数：item - 用于存储出队元素的引用
    // 返回值：true表示出队成功，false表示队列为空
    bool dequeue(T& item) {
        // 以relaxed内存序读取头指针，不需要与其他线程同步
        const size_t current_head = head_.load(std::memory_order_relaxed);
        
        // 检查队列是否为空（通过检查头指针是否与尾指针重合）
        // 使用acquire内存序读取尾指针，确保能看到生产者最新的修改
        if (current_head == tail_.load(std::memory_order_acquire)) {
            return false;  // 队列空，出队失败
        }

        // 读取数据（此时生产者无法覆盖该位置，因为tail_未更新）
        item = buffer_[current_head];
        // 使用release内存序更新头指针，确保之前的读取操作完成，且让生产者可见
        head_.store((current_head + 1) % capacity_, std::memory_order_release);
        return true;  // 出队成功
    }

    // 检查队列是否为空
    // 注意：在多线程环境下，该返回值可能立即过时，仅作为参考
    bool empty() const {
        // 以acquire内存序读取头和尾指针，确保看到最新状态
        return head_.load(std::memory_order_acquire) == tail_.load(std::memory_order_acquire);
    }

    // 检查队列是否已满
    // 注意：在多线程环境下，该返回值可能立即过时，仅作为参考
    bool full() const {
        // 计算下一个尾指针位置
        const size_t next_tail = (tail_.load(std::memory_order_acquire) + 1) % capacity_;
        // 检查是否与头指针重合
        return next_tail == head_.load(std::memory_order_acquire);
    }

    // 获取当前队列中的元素数量
    // 注意：在多线程环境下，该返回值可能立即过时，仅作为参考
    size_t size() const {
        // 获取当前的头和尾指针位置
        const size_t current_head = head_.load(std::memory_order_acquire);
        const size_t current_tail = tail_.load(std::memory_order_acquire);
        
        // 根据头尾指针的相对位置计算队列中元素数量
        if (current_tail >= current_head) {
            // 尾指针在头指针后面或重合（空队列）
            return current_tail - current_head;
        } else {
            // 尾指针在头指针前面（队列环绕）
            return (capacity_ - current_head) + current_tail;
        }
    }

    // 获取队列容量
    // 返回值：队列可容纳的最大元素数量
    size_t capacity() const {
        return capacity_ - 1;  // 减去我们额外增加的一个位置
    }

private:
    const size_t capacity_;          // 队列容量（实际可用为capacity_-1，因为需要一个额外位置区分满和空）
    std::unique_ptr<T[]> buffer_;    // 存储元素的缓冲区，使用智能指针自动管理内存
    std::atomic<size_t> head_;       // 消费者读取位置（出队索引），原子变量确保多线程可见性
    std::atomic<size_t> tail_;       // 生产者写入位置（入队索引），原子变量确保多线程可见性
};
