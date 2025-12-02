#pragma once

#include "comm_sys.h"
#include <atomic>

/**
 * @file mauto.h
 * @brief 多线程原子操作和内存屏障工具类
 * @details 提供跨平台的原子操作封装和内存屏障宏定义，
 *          支持32位和64位原子操作，包括CAS、fetch-add等常用操作
 *          同时定义了不同类型的内存屏障，确保多线程环境下的内存可见性
 */

// 使用std命名空间中的原子操作相关符号
using std::atomic_thread_fence;
using std::memory_order_acq_rel;
using std::atomic_thread_fence;
using std::memory_order_acquire;
using std::atomic_thread_fence;
using std::memory_order_release;
using std::atomic_thread_fence;
using std::memory_order_seq_cst;


namespace lb_common{

/**
 * @defgroup 内存屏障宏定义
 * @brief 定义不同类型的内存屏障，用于多线程同步
 * @{ 
 */

/*
// 以下是汇编实现的内存屏障宏定义（当前未使用，保留作为参考）
#define com_compiler_fence() __asm__ __volatile__("": : :"memory")
#define com_read_fence() __asm__ __volatile__("lfence" : : : "memory")
#define com_write_fence() __asm__ __volatile__("sfence" : : : "memory")
#define com_full_memory_fence() __asm__ __volatile__("mfence" : : : "memory")
*/


#define com_compiler_fence() atomic_thread_fence(memory_order_acq_rel)
#define com_read_fence()  atomic_thread_fence(memory_order_acquire)
#define com_write_fence()  atomic_thread_fence(memory_order_release)
#define com_full_memory_fence()  atomic_thread_fence(memory_order_seq_cst)
/*
#define com_compiler_fence() std::atomic_thread_fence(std::memory_order_acq_rel)
#define com_read_fence()  std::atomic_thread_fence(std::memory_order_acquire)
#define com_write_fence()  std::atomic_thread_fence(std::memory_order_release)
#define com_full_memory_fence()  std::atomic_thread_fence(std::memory_order_seq_cst)
*/
STATIC_FORCE_INLINE int32 com_cas32(volatile int32 *ptr, int32 oldval, int32 newval)
{
	return __sync_val_compare_and_swap(ptr,oldval, newval);
}
STATIC_FORCE_INLINE int32 com_fetchadd32(volatile int32 *ptr, int32 addval)
{
	return __sync_fetch_and_add(ptr, addval);
}
STATIC_FORCE_INLINE int32 com_fetchsub32(volatile int32 *ptr, int32 subval)
{
	return __sync_fetch_and_sub(ptr, subval);
}
STATIC_FORCE_INLINE int32 com_fetchstore32(volatile int32 *ptr, int32 val)
{
	return __sync_lock_test_and_set(ptr, val);
}
STATIC_FORCE_INLINE int64 com_cas64(volatile int64 *ptr, int64 oldval, int64 newval)
{
	return __sync_val_compare_and_swap(ptr,oldval,newval);
}
STATIC_FORCE_INLINE int64 com_fetchadd64(volatile int64 *ptr, int64 addval)
{
	return __sync_fetch_and_add(ptr, addval);
}
STATIC_FORCE_INLINE int64 com_fetchsub64(volatile int64 *ptr, int64 subval)
{
	return __sync_fetch_and_sub(ptr, subval);
}
STATIC_FORCE_INLINE int64 com_fetchstore64(volatile int64 *ptr, int64 val)
{
	return  __sync_lock_test_and_set(ptr, val);
}

/*

#define EBS_DEFINE_ATOMICS(S,T,X)                                          \
STATIC_FORCE_INLINE T com_cas##S (volatile T *ptr, T val, T cmpval)        \
{                                                                          \
    T result;                                                              \
                                                                           \
    __asm__ __volatile__("lock;cmpxchg" X " %2,%1"                         \
                          : "=a"(result), "=m"(*ptr)                       \
                          : "q"(val), "0"(cmpval), "m"(*ptr)               \
                          : "memory");                                     \
    return result;                                                         \
}                                              \
                                                                           \
STATIC_FORCE_INLINE T com_fetchadd##S(volatile T *ptr, T addval)           \
{                                                                          \
    T result;                                                              \
    __asm__ __volatile__("lock;xadd" X " %0,%1"                            \
                          : "=r"(result),"=m"(*ptr)                        \
                          : "0"(addval), "m"(*ptr)                         \
                          : "memory");                                     \
    return result;                                                         \
}                                                                          \
                                                                           \
STATIC_FORCE_INLINE T com_fetchstore##S(volatile T *ptr, T val)            \
{                                                                          \
    T result;                                                              \
    __asm__ __volatile__("lock;xchg" X " %0,%1"                            \
                          : "=r"(result),"=m"(*ptr)                        \
                          : "0"(val), "m"(*ptr)                            \
                          : "memory");                                     \
    return result;                                                         \
}                                                                          \

EBS_DEFINE_ATOMICS(32,int32,"")

EBS_DEFINE_ATOMICS(64,int64,"q")
*/

}