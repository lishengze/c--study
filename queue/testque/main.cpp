/*
 * test_proc_que.cpp - 基于共享内存的环形队列(que_proc_buf)的多线程/多进程测试程序
 * 功能：创建多个生产者和消费者线程，验证队列在并发环境下的性能和正确性
 * 核心测试场景：多生产者写入数据，多消费者读取数据，测试无锁同步机制的有效性
 */
#include "que_proc_buf.h"
//#include "que_mth_buf.h"  // 可选的另一种队列实现
#include "comm_sys.h"
#include "mutils.h"

#include "struct.h"
#include "test.h"
#include "test_log.h"


using namespace lb_common;

/* 全局变量定义 */
// 读取线程参数数组，存储每个读取线程的位置信息
int64 g_read_th_arg[10];
// 写入线程参数数组，存储每个写入线程的标识字符
char g_write_th_arg[10];

// 写入线程数量
int32 g_wr_th_num = 0;
// 读取线程数量
int32 g_rd_th_num = 0;
// 线程控制标志：0-未启动，1-运行中，2-停止
int32 g_stop_th_flag = 0;
// 每次写入队列的数据长度
int32 g_wr_que_len= 0;

// 全局队列实例，用于测试的基于共享内存的环形队列
que_proc_buf g_test_que;
//que_mth_buf g_test_que;  // 可选的另一种队列实现

/*
 * 写入线程函数 - 生产者线程
 * 功能：持续向队列写入数据，根据配置使用普通模式或多线程安全模式
 * 参数：arg - 线程标识参数，此处为写入数据的填充字符
 * 返回值：NULL - 线程结束
 */
void *write_thread_func(void *arg)
{
    // 计算线程休眠时间，根据写入线程数量动态调整
    int32 tus = g_wr_th_num/2;
    if(tus == 0){
        tus = 1;
    }
    // 若只有一个读取线程，增加写入线程休眠时间，降低队列压力
    if(g_rd_th_num == 1)
        tus++;
    
    // 等待启动信号(g_stop_th_flag != 0)
    while(g_stop_th_flag == 0);
    
    // 获取线程标识字符
    char tc = *((char *)arg);
    // 分配写入数据缓冲区并初始化为指定字符
    char *tv = new char[g_wr_que_len];
    for(int32 i=0;i<g_wr_que_len;i++){
        tv[i] = tc;
    }
    tv[g_wr_que_len-1] = '\0';  // 确保字符串结束符
    
    printf("write thread start,c=%c\n",tc);
    
    int64 count = 0;  // 写入计数器
    int32 i= 0;       // 循环计数器
    char *pbuf;       // 指向队列缓冲区的指针
    int64 tpos;       // 写入位置
    do{
        i = 0;
        // 根据写入线程数量选择不同的写入模式
        if(g_wr_th_num == 1){
            // 单生产者模式 - 使用普通写入接口
            do{
                // 获取写入位置和缓冲区指针
                tpos = g_test_que.write_get(pbuf,g_wr_que_len);
                if(tpos > 0)  // 成功获取到写入位置
                    break;
                
                // 每30000000次循环打印一次队列已满信息（避免频繁打印影响性能）
                if((i/30000000) == 0){
                    // printf("write queue have fulled,count=%ld,c=%c\n",count,tc);
                    i = 0;
                }
                i++;
            }while(g_stop_th_flag == 1);  // 当标志为1时继续尝试

            if(tpos >0){
                // memcpy(pbuf,tv,g_wr_que_len);  // 实际应用中需要复制数据
                g_test_que.write_cmt(tpos,g_wr_que_len);  // 提交写入
                count++;
            }

            printf("Single Thread Write data,count=%ld,\n", count);
        }
        else{
            // 多生产者模式 - 使用多线程安全写入接口
            do{
                // 获取写入位置和缓冲区指针（多线程安全版本）
                tpos = g_test_que.write_get_mth(pbuf,g_wr_que_len);
                if(tpos > 0)  // 成功获取到写入位置
                    break;
                
                // 每30000000次循环打印一次队列已满信息
                if((i/30000000) == 0){
                    // printf("write queue have fulled,count=%ld,c=%c\n",count,tc);
                    i = 0;
                }
                i++;
            }while(g_stop_th_flag == 1);  // 当标志为1时继续尝试
            
            if(tpos >0){
                // memcpy(pbuf,tv,g_wr_que_len);  // 实际应用中需要复制数据
                g_test_que.write_cmt_mth(tpos,g_wr_que_len);  // 提交写入（多线程安全版本）
                count++;
            }
        }
        
        comm_utils::sleep_us(10000);  // 可选：控制写入速率
        
    }while(g_stop_th_flag == 1);  // 当标志为1时继续运行，为2时退出
    
    printf("write thread end,count=%ld,c=%c\n",count,tc);
    
    delete[] tv;  // 释放缓冲区
    return NULL;
}

/*
 * 读取线程函数 - 消费者线程（模式1）
 * 功能：从队列中读取数据，根据配置使用不同的读取模式
 * 参数：arg - 线程参数（未使用）
 * 返回值：NULL - 线程结束
 */
void *read_thread_func(void *arg)
{
    // 计算线程休眠时间，根据读取线程数量动态调整
    int32 tus = g_rd_th_num/2;
    if(tus == 0)
        tus = 1;
    
    // 等待启动信号
    while(g_stop_th_flag == 0);
    
    printf("read thread start\n");
    
    char tcache[4096];  // 读取数据缓冲区
    char tc;             // 用于验证数据一致性的字符
    int64 count = 0;     // 读取计数器
    int32 len = 0;       // 单次读取长度
    char *pbuf;          // 指向队列数据的指针
    
    do{
        if(g_rd_th_num == 1){
            // 单消费者模式 - 直接读取并提交
            while((len = g_test_que.read_get(pbuf)) > 0){
                assert(len == g_wr_que_len);  // 验证读取长度是否符合预期
                /* 数据验证代码（注释掉以提高性能）
                tc = pbuf[0];
                for(int32 j=1;j<len-2;j++){
                    assert(tc == pbuf[j]);
                }
                */
                std::cout << "Single Thread read data, count: " << count << std::endl;
                g_test_que.read_cmt();  // 提交读取（单消费者模式）
                count++;
            }
        }
        else{
            // 多消费者模式 - 使用弹出接口
            while((len = g_test_que.read_pop(tcache,sizeof(tcache))) > 0){
                assert(len == g_wr_que_len);  // 验证读取长度是否符合预期
                /* 数据验证代码（注释掉以提高性能）
                tc = tcache[0];
                for(int32 j=1;j<len-2;j++){
                    assert(tc == tcache[j]);
                }
                */
                count++;
            }
        }
        
        // comm_utils::sleep_us(tus);  // 可选：控制读取速率
        
    }while(g_stop_th_flag == 1 || g_test_que.get_used()>0);  // 标志为1或队列非空时继续运行
    
    printf("EXAMPE : read thread end,count=%ld\n",count);
    
    return NULL;
}

/*
 * 读取线程函数 - 消费者线程（模式2）
 * 功能：读取指定位置的数据，用于多消费者分别读取不同区域的场景
 * 参数：arg - 指向存储读取位置的指针
 * 返回值：NULL - 线程结束
 */
void *readpos_thread_func(void *arg)
{
    // 计算线程休眠时间
    int32 tus = g_rd_th_num/2;
    if(tus == 0)
        tus = 1;
    
    // 等待启动信号
    while(g_stop_th_flag == 0);
    
    int64 &tpos = *((int64 *)arg);  // 当前读取位置（引用传递）
    char tc;                       // 用于验证数据一致性的字符
    int64 count = 0;               // 读取计数器
    int32 len = 0;                 // 单次读取长度
    char *pbuf;                    // 指向队列数据的指针
    tpos = g_test_que.get_read_pos();  // 初始化读取位置
    
    printf("read pos thread start,read_pos=%ld\n",tpos);
    
    do{
        // 从指定位置读取数据
        while((len = g_test_que.read_get(pbuf,tpos)) > 0){
            assert(len == g_wr_que_len);  // 验证读取长度
            /* 数据验证代码（注释掉以提高性能）
            tc = pbuf[0];
            for(int32 j=1;j<len-2;j++){
                assert(tc == pbuf[j]);
            }
            */
            tpos = g_test_que.next_pos(tpos,len);  // 更新读取位置
            if(g_rd_th_num == 1)
                g_test_que.read_cmt_pos(tpos);  // 提交读取位置（单消费者模式）
            
            count++;
        }
        
        // comm_utils::sleep_us(tus);  // 可选：控制读取速率
        
    }while(g_stop_th_flag == 1 || g_test_que.get_used()>0);  // 标志为1或队列非空时继续运行
    
    printf("read pos thread end,count=%ld,read_pos=%ld\n",count,tpos);
    
    return NULL;
}

/*
 * 提交读取位置线程函数
 * 功能：协调多个读取线程的进度，提交最小的已读取位置
 * 参数：arg - 线程参数（未使用）
 * 返回值：NULL - 线程结束
 */
void *cmt_read_thread_func(void *arg)
{
    // 计算线程休眠时间
    int32 tus = g_rd_th_num/2;
    if(tus == 0)
        tus = 1;
    if(g_rd_th_num == 1)
        return NULL;  // 单消费者模式下不需要此线程
    
    // 等待启动信号
    while(g_stop_th_flag == 0);
    
    printf("commit pos thread start\n");
    
    int32 i= 0;
    int64 tpos = 0;
    
    do{
        tpos = g_read_th_arg[0];
        // 找出所有读取线程中的最小读取位置
        for(i=1;i<g_rd_th_num;i++){
            int64 t= g_read_th_arg[i];
            if(tpos > t){
                tpos = t;
            }
        }
        // g_test_que.read_cmt_mth(tpos);  // 使用多线程安全模式提交
        g_test_que.read_cmt_pos(tpos);    // 提交最小读取位置
        // comm_utils::sleep_us(tus);  // 可选：控制提交频率
        
    }while(g_stop_th_flag == 1 || g_test_que.get_used()>0);  // 标志为1或队列非空时继续运行
    
    printf("commit pos thread end,commit_pos=%ld\n",tpos);
    
    return NULL;
}

int TestExampe(int iWriteThreadNum=1,int iReadThreadNum=1,int iDataBlockLen=1024,int iReadType=0, int iQueueSize=2) {

    int32 read_type= 0;  // 读取类型：0-普通模式，1-位置模式
    int64 que_size = iQueueSize; // 单位MB;
    que_size = (que_size<<20);  // 转换为字节(MB -> B)
    g_wr_th_num = iWriteThreadNum;
    g_rd_th_num = iReadThreadNum;
    g_wr_que_len = iDataBlockLen;
    read_type = iReadType;


    // 打印测试配置信息
    printf("que_size=%lx, write_thread_num=%d, write_len=%d\n",que_size,g_wr_th_num,g_wr_que_len);
    printf("read_thread_num=%d, read_type=%d, cache_line=%d\n",g_rd_th_num,read_type,CACHE_ALIGN_SIZE);
    
    // 初始化线程参数
    for(int8 i=0;i<10;i++){
        g_write_th_arg[i] = '0' + i;  // 写入线程标识字符：'0'-'9'
        g_read_th_arg[i] = 64;        // 读取线程初始位置
    }
    
    // 计算所需共享内存大小
    int64 ts = que_proc_buf::need_buf_size(que_size);
    ts += sizeof(que_proc_info);  // 加上队列元数据大小
    
    void *tp = NULL;
    // 创建或打开共享内存
    int32 ret = comm_utils::map_shm(tp,"test_proc_que",ts,0);
    printf("mmap que,ret=%d\n",ret);
    if(ret < 0){
        return ret;  // 共享内存创建失败
    }
    
    // 初始化队列
    que_proc_info *tqinfo = (que_proc_info *)tp;
    g_test_que.init_shm(tqinfo,((char *)tp) + sizeof(que_proc_info),que_size,ret,1024,0);
    g_test_que.start(QUE_RECOVE_TYPE_RESTART);  // 启动队列，设置重启恢复模式
    
    
    g_stop_th_flag = 0;  // 初始化线程控制标志
    ret = 0;
    // 创建读取线程
    for(int32 i=0;i<g_rd_th_num;i++)
    {
        pthread_t thid;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        void *tparg = reinterpret_cast<void *>(&(g_read_th_arg[i]));
        if(read_type == 0){
            // 创建普通读取线程
            if(::pthread_create(&thid,&attr,read_thread_func,tparg) !=0){
                g_stop_th_flag = 2;
                ret = -1;
            }
        }
        else{
            // 创建位置模式读取线程
            if(::pthread_create(&thid,&attr,readpos_thread_func,tparg) !=0){
                g_stop_th_flag = 2;
                ret = -1;
            }
        }
    }
    // 创建提交线程（多消费者模式）
    if(read_type != 0 && ret == 0){
        pthread_t thid;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        if(::pthread_create(&thid,&attr,cmt_read_thread_func,NULL) !=0){
            g_stop_th_flag = 2;
            ret = -1;
        }
    }
    // 创建写入线程
    if(ret == 0){
        for(int32 i=0;i<g_wr_th_num;i++){
            pthread_t thid;
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            void *tparg = reinterpret_cast<void *>(&(g_write_th_arg[i]));
            
                if(::pthread_create(&thid,&attr,write_thread_func,tparg) !=0){
                    g_stop_th_flag = 2;
                    ret = -1;
                }
            
        }
    }
    
    printf("create thread,ret=%d\n",ret);
    
    // 启动测试
    if(ret == 0){
        g_stop_th_flag = 1;  // 设置标志为运行中
        sleep(2);  // 测试持续时间10秒
    }
    
    // 结束测试
    g_stop_th_flag = 2;  // 设置标志为停止
    sleep(2);  // 等待线程结束
    
    // 打印测试结果
    printf("thread end,que_wrcmt=%ld,que_rdcmt=%ld,que_uesd=%ld\n",
        g_test_que.get_write_pos(),
        g_test_que.get_read_pos(),
        g_test_que.get_used());
    
    return 1;
}

/*
 * 主函数 - 程序入口
 * 功能：解析命令行参数，初始化队列，创建线程，协调测试流程
 * 参数：
 *   argc - 参数数量
 *   argv - 参数数组：[0]程序名, [1]队列大小(MB), [2]写入线程数, [3]读取线程数, [4]写入长度, [5]读取类型
 * 返回值：0-成功，非0-失败
 */
int main(int argc,char *argv[])
{
    LOG->Init();
    // test_struct();

    TestMain();

    // TestExampe();

    return 0;
}