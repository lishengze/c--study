#pragma once

#include    <stdio.h>
#include    <stdlib.h>
#include    <stddef.h>
#include    <stdarg.h>
#include    <malloc.h>
#include    <math.h>
#include    <string.h>
#include    <stdint.h>
#include    <ctype.h>
#include    <assert.h>

#include    <errno.h>
#include    <unistd.h>
#include    <pthread.h>
#include    <memory.h>
#include    <limits.h>
#include    <fcntl.h>
#include    <signal.h>
#include    <time.h>
#include    <sys/types.h>
#include    <sys/mman.h>
#include    <sys/wait.h>
#include    <sys/file.h>
#include    <sys/ioctl.h>
#include    <sys/stat.h>
#include    <sys/time.h>
#include    <sys/ipc.h>
#include    <sys/sem.h>
#include    <sys/shm.h>
#include    <sys/signalfd.h>
#include    <sys/eventfd.h>
#include	<sys/timerfd.h>
#include	<sys/epoll.h>

/*
#include    <netdb.h>
#include    <sys/socket.h>
#include    <netinet/tcp.h>
#include    <netinet/in.h>
#include    <arpa/inet.h>
#include    <ifaddrs.h>
#include    <net/if.h>
*/

#include    <new>


namespace lb_common{
	
typedef int8_t                   int8;
typedef uint8_t                 uint8;

typedef int16_t                 int16;
typedef uint16_t               uint16;

typedef int32_t                 int32;
typedef uint32_t               uint32;

typedef int64_t                 int64;
typedef uint64_t               uint64;


#ifndef NULL
#   ifdef __cplusplus
#       define NULL            nullptr
#   else
#       define NULL        ((void*) 0)
#   endif
#endif

#ifndef BOOL
#define BOOL                      bool
#endif

#ifndef  TRUE
#define TRUE                      true
#endif

#ifndef  FALSE
#define FALSE                    false
#endif

#ifndef CACHE_ALIGN_SIZE
#define CACHE_ALIGN_SIZE            64
#endif

#ifndef CACHE_ALIGN
#define CACHE_ALIGN __attribute__((aligned(CACHE_ALIGN_SIZE)))
//#define CACHE_ALIGN alignas((aligned(CACHE_ALIGN_SIZE)))
#endif

#ifndef BYTE8_ALIGN
#define BYTE8_ALIGN __attribute__((aligned(8)))
//#define BYTE8_ALIGN alignas(8)
#endif

#ifndef likely
#define likely(x) __builtin_expect ((x), 1)
#endif

#ifndef unlikely
#define unlikely(x) __builtin_expect ((x), 0)
#endif

#ifndef FORCE_INLINE
#define FORCE_INLINE inline __attribute__((always_inline)) 
#endif

#ifndef STATIC_FORCE_INLINE
#define STATIC_FORCE_INLINE static inline __attribute__((always_inline)) 
#endif

#ifndef SPIN_LOCK_BUSY_COUNT
#define SPIN_LOCK_BUSY_COUNT  60000 
#endif

#ifndef SPIN_LOCK_CHECK_COUNT
#define SPIN_LOCK_CHECK_COUNT 100000 
#endif

}