#pragma once

#include <atomic>
#include <thread>
#include "macro.h"

namespace share_common
{

template<typename T>
class lock_guard
{
public:
    lock_guard(T){}
};


#if (defined __aarch64__) || (defined __arm__)
    static inline void cpu_relax()
    {
    	asm volatile("yield" ::: "memory");
    }
#else // __x86_64__
    static inline void cpu_relax()
    {
    	asm volatile("rep;nop": : :"memory");
    }
#endif

struct spinlock_node
{
    spinlock_node():next(nullptr),locked(0){}

    struct spinlock_node *next;
    int locked;
};

class spin_lock
{
public:
    spin_lock():tail_(nullptr){}

    inline void lock(spinlock_node* cur_node)
    {
        spinlock_node* prev = __atomic_exchange_n(&tail_, cur_node, __ATOMIC_ACQUIRE);
        if (LIKELY(prev == nullptr))
        {
    	    return;
    	}
    	prev->next = cur_node;

        while (!__atomic_load_n(&cur_node->locked, __ATOMIC_ACQUIRE))
        {
            cpu_relax();
        }
    }

    inline void unlock(spinlock_node* cur_node)
    {
        spinlock_node* next = cur_node->next;

    	if (LIKELY(!next))
        {
            spinlock_node* tmp = cur_node;
    		if (LIKELY(__atomic_compare_exchange_n(&tail_, &tmp, 
                nullptr, 0, __ATOMIC_RELEASE, __ATOMIC_RELAXED)))
            {      
    		    return;
            }
            
    		while (!(*(volatile spinlock_node**)&cur_node->next))
            {      
    		    cpu_relax();
            }
            next = cur_node->next;
    	}

        __atomic_store_n(&next->locked, 1, __ATOMIC_RELEASE);
    }

private:
    spinlock_node* tail_;
};

template<>
class lock_guard<spin_lock>
{
public:
    lock_guard(spin_lock& in): obj_(in), cur_node_()
    {
        obj_.lock(&cur_node_);
    }

    ~lock_guard()
    {
        obj_.unlock(&cur_node_);
    }
    
    lock_guard(const lock_guard&) = delete;
    lock_guard& operator=(const lock_guard&) = delete;
    
private:
    spin_lock& obj_;
    spinlock_node cur_node_;
};


}   // namespace tech
