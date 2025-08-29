// =============================================================================
// 文件名: mutils.cpp
// 功能: 实现通用工具函数，包括时间管理、内存操作、文件处理、进程管理等
// 注意: 所有函数均属于lb_common命名空间
// =============================================================================
#include "mutils.h"
//#include <iconv.h>  // 注释：字符编码转换功能当前未启用

namespace lb_common{

// -----------------------------------------------------------------------------\n// 时间相关函数\n// -----------------------------------------------------------------------------\n
/**
 * @brief 获取CPU时间戳计数器(TSC)值
 * @return uint64_t - 高分辨率时间戳，可用于精确计时
 * @note 不同CPU核心可能存在TSC不一致问题，多线程环境需谨慎使用
 * @note 针对不同架构(x86_32/x86_64/ARM)提供了汇编实现
 */
uint64 comm_utils::get_rdtsc()
{
#ifdef __i386__  // 32位x86架构
	uint64 t;
	__asm__ volatile("rdtsc" : "=A"(t));  // 执行rdtsc指令，结果存入t
	return t;
#elif defined(__x86_64__) || defined(__amd64__)  // 64位x86架构
	uint64 a,d;
	__asm__ volatile("rdtsc" : "=a"(a), "=d"(d));  // EDX:EAX组合为64位时间戳
	return (d<<32)|a;  // 高32位(d)与低32位(a)组合
#else // ARM架构
	uint32 cc = 0;
	__asm__ volatile("mrc p15, 0, %0, c9, c13, 0":"=r"(cc));  // 读取性能计数器
	return (uint64)cc;
#endif
}

/**
 * @brief 获取系统单调时间(纳秒级)
 * @return uint64_t - 自系统启动以来的纳秒数
 * @note 使用CLOCK_MONOTONIC时钟，不受系统时间调整影响，适合测量时间间隔
 */
uint64 comm_utils::get_tick()
{
	uint64 ret;
	struct timespec tms = {0,0};
    clock_gettime(CLOCK_MONOTONIC,&tms);  // 获取单调时钟
	ret = (uint64)(tms.tv_sec*1000000000 + tms.tv_nsec);  // 转换为纳秒
	return ret;
}

/**
 * @brief 秒级睡眠
 * @param sec [in] - 睡眠秒数
 * @note 调用系统sleep函数，精度较低
 */
void comm_utils::sleep_s(int32 sec)
{
	sleep(sec);
}

/**
 * @brief 毫秒级睡眠
 * @param millisec [in] - 睡眠毫秒数
 * @note 使用nanosleep实现，精度较高且不会被信号中断后无法恢复
 */
void comm_utils::sleep_ms(int32 millisec)
{
	struct timespec req;
	req.tv_sec = millisec/1000;               // 秒部分
	req.tv_nsec = (millisec - req.tv_sec*1000)*1000000;  // 纳秒部分(1毫秒=1e6纳秒)
	nanosleep(&req,NULL);  // 高精度睡眠
}

/**
 * @brief 微秒级睡眠
 * @param us [in] - 睡眠微秒数
 * @note 使用nanosleep实现，适合需要高精度延迟的场景
 */
void comm_utils::sleep_us(int32 us)
{
	struct timespec req;
	req.tv_sec = us/1000000;                  // 秒部分
	req.tv_nsec = (us - req.tv_sec*1000000)*1000;  // 纳秒部分(1微秒=1e3纳秒)
	nanosleep(&req,NULL);
}

/**
 * @brief 获取当前时间(时分秒编码)
 * @return int32_t - 格式为HHMMSS的整数(如14:30:25返回143025)
 */
int32 comm_utils::get_time()
{
	struct tm tmloc;
	time_t curtime = time(0);        // 获取当前时间戳
	localtime_r(&curtime,&tmloc);    // 线程安全地转换为本地时间

	return (tmloc.tm_hour*10000 + tmloc.tm_min*100 + tmloc.tm_sec);
}

/**
 * @brief 获取当前时间字符串
 * @param o_buf [out] - 输出缓冲区
 * @param buf_len [in] - 缓冲区长度(至少9字节)
 * @note 输出格式为"HH:MM:SS"，如"14:30:25"
 */
void comm_utils::get_time(char *o_buf,int32 buf_len)
{
	assert(buf_len >= 9);  // 确保缓冲区足够容纳8个字符+终止符
	struct tm tmloc;
	time_t curtime = time(0);
	localtime_r(&curtime,&tmloc);
	
	snprintf(o_buf,buf_len,"%02d:%02d:%02d",tmloc.tm_hour,tmloc.tm_min,tmloc.tm_sec);
	o_buf[8] = '\0';  // 显式添加终止符
}

/**
 * @brief 获取当前日期(年月日编码)
 * @return int32_t - 格式为YYYYMMDD的整数(如2023年10月5日返回20231005)
 */
int32 comm_utils::get_date()
{
	struct tm p;
	time_t curtime = time(0);
	localtime_r(&curtime,&p);

	return ((1900+p.tm_year)*10000 + (1+p.tm_mon)*100 + p.tm_mday);  // tm_year是自1900年的年数
}

// -----------------------------------------------------------------------------\n// 内存操作函数\n// -----------------------------------------------------------------------------\n
/**
 * @brief 对齐内存分配
 * @param size [in] - 分配大小(字节)
 * @param alignment [in] - 对齐要求(字节，必须是2的幂)
 * @return void* - 对齐的内存指针，失败返回NULL
 * @note 使用posix_memalign实现，需用aligned_free释放
 */
void *comm_utils::aligned_malloc(size_t size,size_t alignment)
{
	void *p = NULL;
	posix_memalign(&p,alignment,size);  // POSIX标准对齐分配函数
	return p;
}

/**
 * @brief 释放对齐分配的内存
 * @param p [in] - 由aligned_malloc分配的内存指针
 */
void comm_utils::aligned_free(void *p)
{
	free(p);  // posix_memalign分配的内存可直接用free释放
}

// -----------------------------------------------------------------------------\n// 共享内存操作函数\n// -----------------------------------------------------------------------------\n
/**
 * @brief 创建或打开共享内存
 * @param o_addr [out] - 输出映射后的内存地址
 * @param shm_name [in] - 共享内存名称(格式如"/my_shm")
 * @param shm_size [in] - 共享内存大小(字节)
 * @param hugepage [in] - 是否使用大页内存(1=使用，0=不使用)
 * @return int32_t - 1=已存在, 0=新建成功, <0=失败
 * @note 成功后需调用mlock锁定内存防止换出
 * @note 失败返回值:-1=打开失败,-2=创建失败,-3=设置大小失败,-4=映射失败
 */
int32 comm_utils::map_shm(void *&o_addr,const char *shm_name,int64 shm_size,int32 hugepage)
{
	int32 shmisexist = 0;
	int32 shmfd = shm_open(shm_name,O_RDWR,S_IRWXU|S_IRGRP|S_IWGRP);
	if(shmfd < 0){
		if(errno != ENOENT)  // 不是不存在的错误
			return -1;
		
		// 创建新的共享内存
		shmfd = shm_open(shm_name,O_RDWR|O_CREAT|O_EXCL,S_IRWXU|S_IRGRP|S_IWGRP);
		if(shmfd < 0){
			assert(errno != EEXIST);  // 确保不是已存在错误
			return -2;
		}
		shmisexist = 0;
	}
	else{
		shmisexist = 1;  // 共享内存已存在
	}

	// 设置共享内存大小
	if(ftruncate(shmfd,shm_size) < 0){
		close(shmfd);
		return -3;
	}

	// 映射共享内存
	void *pshm_addr = NULL;
	if(hugepage == 1){
		// 使用大页内存(需要系统支持)
		pshm_addr = mmap(NULL,shm_size,PROT_READ|PROT_WRITE,MAP_HUGETLB|MAP_SHARED,
			shmfd,0);
	}
	else{
		// 普通共享内存
		pshm_addr = mmap(NULL,shm_size,PROT_READ|PROT_WRITE,MAP_SHARED,
			shmfd,0);
	}
	if(pshm_addr == MAP_FAILED){
		return -4;
	}
	mlock(pshm_addr,shm_size);  // 锁定内存防止被交换到磁盘
	o_addr = pshm_addr;

	close(shmfd);
	return shmisexist;
}

/**
 * @brief 关闭共享内存
 * @param shm_addr [in] - 映射的内存地址
 * @param shm_name [in] - 共享内存名称(NULL表示不删除)
 * @param shm_size [in] - 共享内存大小
 */
void comm_utils::close_shm(void *shm_addr,const char *shm_name,int64 shm_size)
{
	if(NULL != shm_addr)
		munmap(shm_addr, shm_size);  // 解除映射
	if(NULL != shm_name && shm_name[0] != '\0')
		shm_unlink(shm_name);  // 删除共享内存对象(仅最后一个进程调用有效)
}

// -----------------------------------------------------------------------------\n// 文件操作函数\n// -----------------------------------------------------------------------------\n
/**
 * @brief 可靠文件写入
 * @param tfp [in] - 文件指针
 * @param data [in] - 数据缓冲区
 * @param len [in] - 数据长度
 * @return int32_t - 成功写入字节数，<0=失败
 * @note 处理部分写入和EINTR/EAGAIN错误，确保数据可靠写入
 * @note 返回-1=首次写入失败，-2=部分写入后失败
 */
int32 comm_utils::write_file(FILE *tfp,char *data,int32 len)
{
	register int nleft = len;       // 剩余字节数
	register int nwritten = 0;      // 已写入字节数
	char  *ptr = data;              // 当前数据指针

	while(nleft > 0)
	{
	     if((nwritten = fwrite(ptr, sizeof(char),nleft,tfp)) < 0) {
	         if (errno == EINTR || errno == EAGAIN){
	             continue;  // 被信号中断或资源暂时不可用，重试
	         }
	         else if(nleft == len){
	             return -1;  // 首次写入即失败
	         }else{
	             return -2;  // 部分写入后失败
	         }
	     }else if(nwritten == 0) {
	         continue;  // 未写入任何数据，重试
	     }
	     nleft -= nwritten;
	     ptr += nwritten;
	}
	//fflush(tfp);  // 注释：根据需要决定是否显式刷新
	return len;
}

/**
 * @brief 可靠文件读取
 * @param tfp [in] - 文件指针
 * @param buf [out] - 接收缓冲区
 * @param len [in] - 要读取的字节数
 * @return int32_t - 实际读取字节数，<0=错误
 * @note 处理部分读取、EINTR/EAGAIN错误和文件结束
 */
int32 comm_utils::read_file(FILE *tfp, char *buf, int len)
{
	int32 nleft = len;       // 剩余需读取字节数
	int32 readlen = 0;       // 已读取字节数
	int32 ret = 0;
	//GetSysLastError() = 0;
    while (nleft > 0) {
        if ((ret = fread(buf+readlen,sizeof(char), nleft,tfp)) <= 0)
        {
            if (errno == EINTR || errno == EAGAIN) {
                continue;  // 被中断或资源暂时不可用，重试
            }
		    else if(feof(tfp)){
			    return readlen;  // 已到文件尾，返回已读取字节数
		    }
            else {
                return ret;  // 其他错误
            }
        }
        nleft -= ret;
        readlen += ret;
    }
    return readlen;
}

/**
 * @brief 检查文件是否存在
 * @param file_name [in] - 文件名
 * @return int32_t - 1=存在，0=不存在
 */
int32 comm_utils::exist_file(const char *file_name)
{
	if(access(file_name,F_OK) == 0)  // 使用access系统调用检查文件存在性
		return 1;
	return 0;
}

/**
 * @brief 检查目录是否存在
 * @param path_name [in] - 目录名
 * @return int32_t - 1=存在，0=不存在，-1=错误(如权限问题)
 */
int32 comm_utils::exist_path(const char *path_name)
{
	struct stat attr;
    int32 ret = stat(path_name,&attr);
	if(ret == 0){
		// 验证是否为目录
    	return S_ISDIR(attr.st_mode) ? 1 : 0;
	}
    else if(errno == ENOENT){
		return 0;  // 路径不存在
    }
	else{
        // 其他错误(如权限不足)
        return -1;
    }
}

/**
 * @brief 创建目录
 * @param path_name [in] - 目录名
 * @return int32_t - 0=成功，-1=失败
 * @note 仅创建单级目录，需确保父目录存在
 */
int32 comm_utils::make_path(const char *path_name)
{
	if(exist_path(path_name) == 0){
		return mkdir(path_name,0775);  // 创建目录，权限rwxrwxr-x
	}
	return 0;  // 目录已存在
}

// -----------------------------------------------------------------------------\n// 进程操作函数\n// -----------------------------------------------------------------------------\n
/**
 * @brief 检查进程是否存在
 * @param proc_id [in] - 进程ID
 * @return int32_t - 1=存在，0=不存在
 * @note 通过检查/proc/[pid]目录是否存在实现
 */
int32 comm_utils::exist_pid(int64 proc_id)
{
	char pid_file[256];
	memset(pid_file,0,sizeof(pid_file));
	snprintf(pid_file,255,"%s%ld","/proc/",proc_id);
	
	return exist_path(pid_file);
}

/**
 * @brief 获取当前进程ID
 * @return int64_t - 进程ID
 */
int64 comm_utils::get_pid()
{
	return (int64)(getpid());
}

// -----------------------------------------------------------------------------\n// 数学与其他工具函数\n// -----------------------------------------------------------------------------\n
/**
 * @brief 计算大于等于n的最小2的幂
 * @param n [in] - 输入整数
 * @return int32_t - 2的幂指数(如n=5返回3，因为2^3=8≥5)
 * @note 使用位运算高效实现
 */
int32 comm_utils::calc_power(int64 n)
{
	if(unlikely(n < 1))  //  unlikely优化分支预测
		return 0;
	
	int64 ts = n;
	int32 tn = 0;
	while(ts != 1){
		tn++;	
		ts = (ts >> 1);  // 右移相当于除以2
	}
	if((1UL << tn) < n)  // 如果当前幂次仍小于n
		tn++;
	return tn;
}

/* 注释：字符编码转换功能当前未启用
int32 comm_utils::utf8_to_gb(const char *gb_name,char *dst,char *src,
	int32 dstsize,int32 srcsize)
{
	iconv_t tcd;
	if((tcd = iconv_open(gb_name,"utf-8")) == 0)
		return -1;
	
	memset(dst,0,dstsize);
	char **tsource = &src;
	char **tdest = &dst;
	size_t tsn = srcsize;
	size_t tdn = dstsize;
	iconv(tcd,tsource,&tsn,tdest,&tdn);
	iconv_close(tcd);
	return 0;
}
int32 comm_utils::gb_to_utf8(const char *gb_name,char *dst,char *src,
	int32 dstsize,int32 srcsize)
{
	iconv_t tcd;
	if((tcd = iconv_open("utf-8",gb_name)) == 0)
		return -1;
	
	memset(dst,0,dstsize);
	char **tsource = &src;
	char **tdest = &dst;
	size_t tsn = srcsize;
	size_t tdn = dstsize;
	iconv(tcd,tsource,&tsn,tdest,&tdn);
	iconv_close(tcd);
	return 0;
}
*/

}  // namespace lb_common