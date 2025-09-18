#pragma once

#include "comm_sys.h"

/**
 * @file mutils.h
 * @brief 通用工具类头文件
 * @details 提供系统级通用工具函数，包括时间管理、内存分配、文件操作、进程管理等功能
 *          采用静态成员函数设计，无需实例化即可使用
 */

/*
// 以下头文件和命名空间声明当前未使用，保留作为参考
#include <string>
#include <cstring>
#include <vector>

using namespace std::vector;
using namespace std::string;
*/


namespace lb_common{

/**
 * @class comm_utils
 * @brief 通用工具类
 * @details 提供一系列静态工具函数，涵盖时间操作、内存管理、共享内存、文件操作和进程管理等功能
 *          所有方法均为静态方法，无需创建类实例即可直接调用
 */
class comm_utils
{
public:

    /**
     * @brief 获取CPU时间戳计数器(TSC)值
     * @return 返回CPU的TSC计数器值，可用于高精度时间测量
     * @note TSC值在不同CPU核心间可能不同步，使用时需注意
     */
    static uint64 get_rdtsc();
    
    /**
     * @brief 获取系统滴答数
     * @return 返回系统滴答计数值
     */
    static uint64 get_tick();
    
    /**
     * @brief 秒级睡眠
     * @param sec 睡眠的秒数
     */
    static void sleep_s(int32 sec);
    
    /**
     * @brief 毫秒级睡眠
     * @param millisec 睡眠的毫秒数
     */
    static void sleep_ms(int32 millisec);
    
    /**
     * @brief 微秒级睡眠
     * @param us 睡眠的微秒数
     * @note 实际精度受操作系统调度影响
     */
    static void sleep_us(int32 us);
    
    /**
     * @brief 获取当前时间（秒级）
     * @return 返回从某个固定时间点到现在的秒数
     */
    static int32 get_time();
    
    /**
     * @brief 获取格式化时间字符串
     * @param o_buf 输出缓冲区
     * @param buf_len 缓冲区长度
     */
    static void get_time(char *o_buf,int32 buf_len);
    
    /**
     * @brief 获取日期
     * @return 返回日期表示值
     */
    static int32 get_date();

    
    /**
     * @brief 对齐内存分配
     * @param size 分配的内存大小
     * @param alignment 对齐要求（字节数）
     * @return 返回对齐后的内存指针，失败返回nullptr
     */
    static void *aligned_malloc(size_t size,size_t alignment);
    
    /**
     * @brief 释放对齐分配的内存
     * @param p 由aligned_malloc分配的内存指针
     */
    static void aligned_free(void *p);
    
    /**
     * @brief 映射共享内存
     * @param o_addr 输出参数，映射后的内存地址
     * @param shm_name 共享内存名称
     * @param shm_size 共享内存大小
     * @param hugepage 是否使用大页内存（1-使用，0-不使用）
     * @return 1-共享内存已存在并打开，0-新建并打开，<0-失败
     */
    static int32 map_shm(void *&o_addr,const char *shm_name,int64 shm_size,int32 hugepage=1);
    
    /**
     * @brief 关闭共享内存
     * @param shm_addr 共享内存地址
     * @param shm_name 共享内存名称
     * @param shm_size 共享内存大小
     */
    static void close_shm(void *shm_addr,const char *shm_name,int64 shm_size);

    
    /**
     * @brief 写入文件
     * @param tfp 文件指针
     * @param data 待写入数据
     * @param len 数据长度
     * @return 成功写入的字节数，<0表示失败
     */
    static int32 write_file(FILE *tfp,char *data,int32 len);
    
    /**
     * @brief 读取文件
     * @param tfp 文件指针
     * @param buf 接收缓冲区
     * @param len 要读取的长度
     * @return 成功读取的字节数，<0表示失败
     */
    static int32 read_file(FILE *tfp, char *buf, int len);
    
    /**
     * @brief 检查文件是否存在
     * @param file_name 文件名
     * @return 1-存在，0-不存在，<0-出错
     */
    static int32 exist_file(const char *file_name);
    
    /**
     * @brief 创建目录
     * @param path_name 目录路径
     * @return 0-成功，<0-失败
     */
    static int32 make_path(const char *path_name);
    
    /**
     * @brief 检查目录是否存在
     * @param path_name 目录路径
     * @return 1-存在，0-不存在，<0-出错
     */
    static int32 exist_path(const char *path_name);
    
    /**
     * @brief 检查进程是否存在
     * @param proc_id 进程ID
     * @return 1-存在，0-不存在，<0-出错
     */
    static int32 exist_pid(int64 proc_id);
    
    /**
     * @brief 获取当前进程ID
     * @return 当前进程ID
     */
    static int64 get_pid();

    /**
     * @brief 计算n的幂次方
     * @param n 底数
     * @return 计算结果
     */
    static int32 calc_power(int64 n);

    /*
    // 以下字符编码转换函数当前未实现
    static int32 utf8_to_gb(const char *gb_name,char *dst,char *src,
        int32 dstsize,int32 srcsize);
    static int32 gb_to_utf8(const char *gb_name,char *dst,char *src,
        int32 dstsize,int32 srcsize);
    */
};

} // namespace lb_common