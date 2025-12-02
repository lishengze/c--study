#pragma once

#include "que_comm.h"   // 包含队列公共定义（如QUE_HEAD_STATE_*枚举、que_proc_head结构体）
#include "mauto.h"


namespace lb_common{

/**
 * @brief 队列元数据结构，存储环形缓冲区的核心状态信息
 * @note 采用CACHE_ALIGN缓存行对齐，避免多线程伪共享问题
 */
struct que_proc_info
{
    volatile int64 wrcmt;        // 写入提交位置（已完成写入的偏移量，读者可见）
    volatile int64 roundend;     // 当前环形缓冲区结束标记（用于处理缓冲区回绕）
    volatile int64 wrpos;        // 写入指针（生产者使用，未提交的写入位置）
    int64 mask;                  // 缓冲区大小掩码（= size-1，用于环形索引计算）
    int32 forward_check;         // 前瞻检查阈值（用于预检测缓冲区溢出）
    uint32 userflag;             // 用户自定义标志位（可用于扩展状态控制）
    int64 userpos[2];            // 用户自定义位置（可用于存储额外索引信息）
    int64 filled;                // 已填充数据量（用于统计和监控）
    volatile int64 rdcmt;        // 读取提交位置（已完成读取的偏移量，写者可见）
    volatile int64 rdpos;        // 读取指针（消费者使用，未提交的读取位置）
    char userdata[QUE_USER_DATA_LEN]; // 用户自定义数据区（固定大小的额外存储空间）
}CACHE_ALIGN;  // 强制结构体按缓存行对齐

/**
 * @brief 基于共享内存的多进程/多线程安全环形缓冲区实现
 * @details 支持单生产者-单消费者和多生产者-多消费者模式，通过无锁设计实现高效数据交换
 */
class que_proc_buf
{
protected:
    char *pbuf;               // 指向共享内存缓冲区的起始地址
    que_proc_info *que;       // 指向队列元数据（que_proc_info结构体）
    int64 op_pid;             // 操作进程ID（用于进程间同步校验）

    int32 busy_readpos_check(que_proc_head &thead);  // 忙等待读位置检查
    void busy_read_check(que_proc_head &thead);      // 忙等待读操作检查
    void read_pop_cmt(int64 cmt_pos,int64 read_pos); // 读取弹出提交

public:
    /**
     * @brief 获取指定位置的数据指针
     * @param pos 全局偏移位置（不受环形缓冲区大小限制）
     * @return 指向数据区域的指针
     * @details 通过 pos & mask 计算环形缓冲区中的实际索引，跳过头部结构体大小
     */
    FORCE_INLINE char *get_data(int64 pos)
    {
        return (pbuf + (pos&(que->mask)) + sizeof(que_proc_head));
    }

    // ==== 写入操作接口 ====
    /**
     * @brief 获取写入位置（普通模式）
     * @param o_data 输出参数，指向获取到的写入缓冲区地址
     * @param len 要写入的数据长度（不包含头部）
     * @return 成功返回写入位置，失败返回0
     */
    int64 write_get(char *&o_data,int32 len);

    /**
     * @brief 提交写入（普通模式）
     * @param getpos write_get返回的写入位置
     * @param len 数据长度（不包含头部）
     * @details 设置头部状态为写入完成，更新wrcmt（写入提交位置）
     */
    FORCE_INLINE void write_cmt(int64 getpos,int32 len)
    {
        que_proc_head *thead = (que_proc_head *)(pbuf +(getpos&(que->mask)));
        thead->pid = 0;          // 清除PID标记（用于进程间校验）
        thead->len = len;        // 设置数据长度
        com_write_fence();       // 写内存屏障，确保数据写入对其他线程可见
        thead->state = QUE_HEAD_STATE_WRITEOK;  // 设置状态为写入完成

        // 处理环形缓冲区回绕（当写入位置为0时更新roundend）
        if(unlikely((getpos&(que->mask))==0))
            que->roundend = que->wrcmt;
        // 更新写入提交位置（当前位置 + 数据长度 + 头部大小）
        que->wrcmt = getpos+len+sizeof(que_proc_head);
    }

    /**
     * @brief 写入数据（普通模式，单生产者安全）
     * @param userdata 要写入的数据指针
     * @param len 数据长度
     * @return 成功返回写入位置，失败返回0
     */
    int64 write(char *userdata,int32 len)
    {
        char *pd = NULL;
        int64 wr = write_get(pd,len);
        if(likely(wr != 0)){       // 分支预测优化：大概率成功路径
            memcpy(pd,userdata,len);
            write_cmt(wr,len);
            return wr;
        }
        return 0;  // 缓冲区已满或其他错误
    }

    /**
     * @brief 获取写入位置（多线程安全模式）
     * @param o_data 输出参数，指向获取到的写入缓冲区地址
     * @param len 要写入的数据长度
     * @return 成功返回写入位置，失败返回0
     * @note 用于多生产者场景，内部实现包含竞争控制
     */
    int64 write_get_mth(char *&o_data,int32 len);

    /**
     * @brief 提交写入（多线程安全模式）
     * @param getpos write_get_mth返回的写入位置
     * @param len 数据长度（不包含头部）
     * @details 使用CAS原子操作更新wrcmt，确保多生产者情况下的数据一致性
     */
    FORCE_INLINE void write_cmt_mth(int64 getpos,int32 len)
    {
        que_proc_head *thead = (que_proc_head *)(pbuf +(getpos&(que->mask)));
        thead->state = QUE_HEAD_STATE_WRITEOK;  // 设置状态为写入完成

        // 计算新的写入提交位置
        register int64 d = getpos + len + sizeof(que_proc_head);
        // CAS循环：直到成功更新wrcmt或检测到其他线程已完成更新
        do{
            register int64 t = que->wrcmt;
            if(likely(t < d)){  // 只有当前wrcmt小于新位置时才需要更新
                // 原子比较并交换：如果当前wrcmt等于t，则更新为d
                if(likely(com_cas64(&(que->wrcmt), t, d) == t))
                    break;  // CAS成功，退出循环
            }
            else{
                break;  // 其他线程已更新wrcmt，无需操作
            }
        }while(1);
    }

    /**
     * @brief 写入数据（多线程安全模式，多生产者安全）
     * @param userdata 要写入的数据指针
     * @param len 数据长度
     * @return 成功返回写入位置，失败返回0
     */
    inline int64 write_mth(char *userdata,int32 len){
        char *pd = NULL;
        int64 wr = write_get_mth(pd,len);
        if(likely(wr != 0)){
            memcpy(pd,userdata,len);
            write_cmt_mth(wr,len);
            return wr;
        }
        return 0;
    }

    // ==== 读取操作接口 ====
    /**
     * @brief 获取读取数据（从上次提交位置开始）
     * @param o_data 输出参数，指向读取到的数据缓冲区
     * @return 成功返回数据长度，失败返回0或错误码
     */
    int32 read_get(char *&o_data);

    /**
     * @brief 提交读取（更新读取位置）
     * @details 从rdcmt位置开始处理，更新头部状态为空闲，推进读取指针
     */
    FORCE_INLINE void read_cmt()
    {
        register int64 rd = (que->rdcmt);
        // 处理环形缓冲区回绕：当读取到roundend时跳转到下一轮
        if(unlikely(rd == que->roundend)){
            rd += ((que->mask)+1 - (rd&(que->mask)));
        }

        que_proc_head *thead = (que_proc_head *)(pbuf + (rd&(que->mask)));
        thead->state = QUE_HEAD_STATE_IDLE;  // 设置头部状态为空闲
        com_write_fence();                    // 写内存屏障，确保状态更新可见
        // 更新读取提交位置（当前位置 + 数据长度 + 头部大小）
        (que->rdcmt) = rd + thead->len + sizeof(que_proc_head);
    }

    /**
     * @brief 计算下一个数据位置
     * @param curpos 当前位置
     * @param read_len 已读取的数据长度
     * @return 下一个数据的起始位置
     */
    FORCE_INLINE int64 next_pos(int64 curpos,int32 read_len){
        return curpos + read_len + sizeof(que_proc_head);
    }

    /**
     * @brief 获取读取数据（从指定位置开始）
     * @param o_data 输出参数，指向读取到的数据缓冲区
     * @param io_pos 输入输出参数，指定起始读取位置，返回实际读取位置
     * @return 成功返回数据长度，失败返回0或错误码
     */
    int32 read_get(char *&o_data,int64 &io_pos);

    /**
     * @brief 提交读取位置（直接设置）
     * @param dealpos 已处理到的位置
     * @details 仅当dealpos大于当前rdcmt时才更新
     */
    FORCE_INLINE void read_cmt_pos(int64 dealpos)
    {
        if(dealpos > (que->rdcmt))
            (que->rdcmt) = dealpos;
    }

    /**
     * @brief 提交读取位置（多线程安全模式）
     * @param dealpos 已处理到的位置
     * @return 成功返回旧的rdcmt值，失败返回0
     * @details 使用CAS原子操作确保多消费者情况下的线程安全
     */
    inline int64 read_cmt_mth(int64 dealpos)
    {
        register int64 t;
        while(1){
            t = (que->rdcmt);
            if(unlikely(t > dealpos))  // 处理位置异常（已被其他线程超前处理）
                return 0;
            // 原子比较并交换：将rdcmt从t更新为dealpos
            if(com_cas64(&(que->rdcmt),t,dealpos)== t)
                return t;  // CAS成功，返回旧值
        }
    }

    /**
     * @brief 弹出数据（高级读取接口）
     * @param o_buf 输出缓冲区
     * @param buf_len 缓冲区大小
     * @return >0: 读取到的数据长度; 0: 无数据; <0: 缓冲区大小不足
     */
    int32 read_pop(char *o_buf,int32 buf_len);

    // ==== 状态查询接口 ====
    /**
     * @brief 获取已使用空间大小
     * @return 已使用字节数
     */
    FORCE_INLINE int64 get_used()
    {
        return ((que->wrcmt>que->wrpos?(que->wrcmt):(que->wrpos)) -(que->rdcmt));
    }

    /**
     * @brief 获取空闲空间大小
     * @return 空闲字节数
     */
    FORCE_INLINE int64 get_free()
    {
        return ((que->mask)+1 -(que->wrcmt>que->wrpos?(que->wrcmt):(que->wrpos))+(que->rdcmt));
    }

    // ==== 初始化与管理接口 ====
    /**
     * @brief 计算所需缓冲区大小
     * @param que_size 用户指定的队列大小
     * @return 实际需要的共享内存大小（包含元数据和对齐）
     */
    static int64 need_buf_size(int64 que_size);

    /**
     * @brief 初始化共享内存
     * @param pinfo 队列元数据指针
     * @param shm_addr 共享内存地址
     * @param que_size 队列大小
     * @param shm_isexist 共享内存是否已存在（0:新创建, 1:已存在）
     * @param check_len 校验长度
     * @param user_flag 用户自定义标志
     */
    void init_shm(que_proc_info *pinfo,char *shm_addr,int64 que_size,
        int32 shm_isexist,int32 check_len=1024,uint32 user_flag=0);

    /**
     * @brief 启动队列服务
     * @param recove_type 恢复类型（0:不恢复, 1:从上次位置恢复）
     */
    void start(int32 recove_type =0);

    // ==== 内联访问接口 ====
    FORCE_INLINE int64 get_write_pos(){return que->wrcmt;}  // 获取写入提交位置
    FORCE_INLINE int64 get_read_pos(){return (que->rdcmt);} // 获取读取提交位置
    FORCE_INLINE int64 get_size(){return (que->mask)+1;}    // 获取队列总大小

    FORCE_INLINE uint32 get_user_flag(){return que->userflag;}       // 获取用户标志
    FORCE_INLINE void set_user_flag(uint32 val){que->userflag=val;}  // 设置用户标志
    FORCE_INLINE int64 *get_user_pos(){return que->userpos;}         // 获取用户位置数组
    FORCE_INLINE char *get_user_data(){return que->userdata;}        // 获取用户数据区

    /**
     * @brief 构造函数
     * @details 初始化成员变量为默认值
     */
    que_proc_buf()
    {
        pbuf = NULL;
        que = NULL;
        op_pid = 0;
    }

    /**
     * @brief 析构函数
     * @note 共享内存由外部管理，此处不负责释放
     */
    ~que_proc_buf(){}
};


} // namespace lb_common