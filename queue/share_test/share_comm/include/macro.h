#pragma once


#ifdef __GNUC__
/**
 * @brief      生成有利于分支预测的代码,告知编译器条件表达式"x"为true的概率较高
 *
 * @param      x     条件表达式
 *
 * @return     条件表达式的布尔值
 */
#define LIKELY(x) (__builtin_expect(!!(x), 1))
#else
#define LIKELY(x) (x)
#endif

#ifdef __GNUC__
/**
 * @brief      生成有利于分支预测的代码,告知编译器条件表达式"x"为false的概率较高
 *
 * @param      x     条件表达式
 *
 * @return     条件表达式的布尔值
 */
#define UNLIKELY(x) (__builtin_expect(!!(x), 0))
#else
#define UNLIKELY(x) (x)
#endif



#ifndef __always_inline
    #define __always_inline	inline __attribute__((always_inline))
#endif

#if (defined __aarch64__) || (defined __arm__)
    #define L1_CACHE_LINE 64
    #define L2_CACHE_LINE 64
#else // __x86_64__
    /*
     * 64-ia-32-architectures-optimization-manual
     *  E.2.5.4 Data Prefetching
     *      Spatial Prefetcher: This prefetcher strives to complete every cache line fetched to the L2 cache with 
     *      the pair line that completes it to a 128-byte aligned chunk
    */
    #define L1_CACHE_LINE 64
    #define L2_CACHE_LINE 128
#endif



struct alignas(L2_CACHE_LINE) 
l2cache_padding
{
	char t[0];
};
struct alignas(L1_CACHE_LINE) 
l1cache_padding
{
	char t[0];
};

#define L1CACHE_PADDING(name)	struct l1cache_padding name
#define L2CACHE_PADDING(name)	struct l2cache_padding name

#define __STL_TEMPLATE_NULL template<> 