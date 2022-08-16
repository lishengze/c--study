#ifndef __OS_SEM_H__
#define __OS_SEM_H__

#include <sys/sem.h>
#include <semaphore.h>
#include <fcntl.h>

#include "global_declare.h"

/************************************************************
* Function : 无名信号量初始化
* brief    : 
* args[in] : 
* args[out]: 
* return   : 
*************************************************************/
inline int InitSem(sem_t* pSemtex, int processFlag, int value)
{
    int ret = 0;

    if (pSemtex == (sem_t*)0) {
        printf("%s:%s@%d  Null point!\n",  __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }
    ret = sem_init(pSemtex, processFlag, value);
    if (ret < 0) {
        printf("%s:%s@%d  faild!\n",  __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }

    return 0;
}

inline void DestroySem(sem_t* pSemtex)
{
    sem_destroy(pSemtex);
}

/************************************************************
* Function : OpenNamedSem
* brief    : 有名信号量初始化
* args[in] : 
* args[out]: 
* return   : 
*************************************************************/
inline int OpenNamedSem(const char* pName, sem_t** pSemtex, int value)
{
    if (pSemtex == (sem_t**)0) {
        printf("%s:%s@%d  Null point!\n",  __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }
    
    *pSemtex = sem_open(pName, O_CREAT, 0644, value);
    if (*pSemtex == SEM_FAILED) {
        printf("%s:%s@%d  faild!\n",  __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }

    return 0;
}

inline void CloseNamedSem(sem_t* pSemtex)
{
    sem_close(pSemtex);
}

inline int UnlinkNamedSem(const char* pName)
{
    return sem_unlink(pName);
}

/************************************************************
* Function : PostSem
* brief    : 通用信号量操作
* args[in] : 
* args[out]: 
* return   : 
*************************************************************/
inline void PostSem(sem_t* pSemtex)
{
    sem_post(pSemtex);
}

inline void WaitSem(sem_t* pSemtex)
{
    sem_wait(pSemtex);
}


// #ifdef __cplusplus
// extern "C" {
// #endif

// int OpenNamedSem(const char* pName, sem_t** pSemtex, int value);
// int InitSem(sem_t* pSemtex, int processFlag, int value);
// void PostSem(sem_t* pSemtex);
// void WaitSem(sem_t* pSemtex);
// void DestroySem(sem_t* pSemtex);
// void CloseNamedSem(sem_t* pSemtex);
// int UnlinkNamedSem(const char* pName);

// #ifdef __cplusplus
// }
// #endif

#endif
