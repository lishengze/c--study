
// =============================================================================
// 文件名: que_proc_buf.cpp
// 功能: 实现基于共享内存的进程间通信环形缓冲区操作
// 核心技术: 无锁同步(CAS/内存屏障)、环形缓冲区管理、多线程安全机制
// 注意: 配合que_proc_buf.h头文件使用，需在多进程环境下初始化共享内存
// =============================================================================
#include "que_proc_buf.h"
#include "mutils.h"

namespace lb_common{

// -----------------------------------------------------------------------------\n// 写入操作相关函数\n// -----------------------------------------------------------------------------\n
/**
 * @brief 获取普通模式下的可写缓冲区地址
 * @param o_data [out] - 输出可写数据区域指针(跳过头部)
 * @param len [in] - 待写入数据长度(不含头部)
 * @return int64_t - 成功返回写入位置(用于后续提交)，失败返回0
 * @note 普通模式不保证多线程安全，需外部同步
 * @note 内部自动处理环形缓冲区绕回逻辑
 */
int64 que_proc_buf::write_get(char *&o_data,int32 len)
{
	// 计算总长度(数据长度+头部长度)
	len += sizeof(que_proc_head);
	
	// 读取当前写入提交位置(volatile变量，确保内存可见性)
	register int64 tc = (que->wrcmt);
	
	// 检查空间是否充足(缓冲区总容量=mask+1)
	if(unlikely(tc + len - (que->rdcmt) > (que->mask)))
		return 0;
	
	// 计算当前写入偏移位置
	register int64 wr = (tc & (que->mask));
	
	// 尝试直接写入(无绕回情况)
	if(likely(wr + len <= (que->mask) + 1)){
		o_data = (pbuf + wr + sizeof(que_proc_head));  // 跳过头部
	}
	// 绕回处理(当前位置+长度超过缓冲区末尾)
	else{
		// 计算绕回后的起始位置
		tc += ((que->mask) + 1 - wr);
		// 二次检查空间是否充足
		if(tc + len - (que->rdcmt) > (que->mask))
			return 0;
		// 设置绕回后的写入地址
		o_data = (pbuf + sizeof(que_proc_head));  // 从缓冲区起始位置开始
	}
	return tc;  // 返回写入位置标记
}

/**
 * @brief 获取多线程安全模式的可写缓冲区地址
 * @param o_data [out] - 输出可写数据区域指针(跳过头部)
 * @param len [in] - 待写入数据长度(不含头部)
 * @return int64_t - 成功返回写入位置标记，失败返回0
 * @note 使用CAS原子操作保证多生产者安全
 * @note 自动填充数据头部信息(长度/PID)
 */
int64 que_proc_buf::write_get_mth(char *&o_data,int32 len)
{
	// 计算总长度(含头部)
	len += sizeof(que_proc_head);
	
	int64 ret = 0;  // 返回值(写入位置标记)
	register int64 t;  // 临时变量(新写入位置)
	register int64 wr;  // 当前写入位置
	
	// CAS循环获取写入权限(自旋锁机制)
	do{
		// 读取当前写入位置(volatile变量)
		wr = (que->wrpos);
		// 检查空间是否充足
		if(unlikely(wr + len - (que->rdcmt) > (que->mask)))
			return 0;
		
		// 计算当前偏移位置
		t = (wr & (que->mask));
		
		// 无绕回情况
		if(likely(t + len <= (que->mask) + 1)){
			t = wr + len;  // 新写入位置=当前位置+长度
			ret = wr;      // 返回原始位置
		}
		// 绕回情况
		else{
			ret = wr + ((que->mask) + 1 - t);  // 跳转到缓冲区起始位置
			t = ret + len;                      // 新写入位置
			// 二次检查空间
			if(t - (que->rdcmt) > (que->mask))
				return 0;
		}
		
		// CAS原子操作更新写入位置: wr->t
		if(likely(com_cas64(&(que->wrpos), wr, t) == wr)){
			// 记录绕回点(用于读操作判断)
			if(ret > wr || (wr & (que->mask)) == 0)
				que->roundend = wr;
			
			// 初始化数据头部
			que_proc_head *thead = (que_proc_head *)(pbuf + (ret & (que->mask)));
			thead->pid = op_pid;          // 记录当前进程ID
			thead->len = len - sizeof(que_proc_head);  // 数据长度(不含头部)
			com_write_fence();            // 写内存屏障(确保数据可见性)
			thead->state = QUE_HEAD_STATE_WRITING;  // 标记为写入中
			
			// 设置输出数据指针(跳过头部)
			o_data = (pbuf + (ret & (que->mask)) + sizeof(que_proc_head));
			return ret;  // 返回写入位置标记
		}
	}while(1);  // CAS失败则重试
}

// -----------------------------------------------------------------------------\n// 读取操作相关函数\n// -----------------------------------------------------------------------------\n
/**
 * @brief 获取可读数据缓冲区(自动推进读取位置)
 * @param o_data [out] - 输出可读数据区域指针
 * @return int32_t - 成功返回数据长度，失败返回0
 * @note 循环检查数据状态，处理写入中/丢弃状态的数据
 * @note 内部实现忙等待机制，避免CPU空转
 */
int32 que_proc_buf::read_get(char *&o_data)
{
	int32 i = 0;          // 重试计数器
	int64 tre;            // 绕回位置标记
	int64 wr;             // 当前写入提交位置
	que_proc_head *thead; // 数据头部指针

retry_read:
	// 获取当前写入提交位置
	wr = (que->wrcmt);
	register int64 rd = (que->rdcmt);
	// 无数据可读
	if(unlikely(rd >= wr))
		return 0;

	// 计算当前读取偏移位置
	register int64 t = (rd & (que->mask));
	thead = (que_proc_head *)(pbuf + t);
	o_data = (pbuf + t + sizeof(que_proc_head));  // 跳过头部

	// 计算当前段可用数据长度
	if(likely((wr & (que->mask)) != 0))
		t = (wr & (que->mask)) - t;  // 无绕回情况
	else
		t = wr - rd;                 // 绕回情况

	// 有可用数据
	if(likely(t > 0)){
		// 数据已写入完成
		if(likely(thead->state == QUE_HEAD_STATE_WRITEOK)){
			return thead->len;
		}
		// 数据已被丢弃
		else if(thead->state == QUE_HEAD_STATE_DISCARD){
			(que->rdcmt) += thead->len + sizeof(que_proc_head);  // 推进读取位置
			goto retry_read;  // 重新尝试读取
		}
		
		// 写入中状态，有限次数重试
		i++;
		if(i == QUE_READ_CHECK_ROUND){
			i = 0;
			busy_read_check(*thead);  // 忙等待检查(含进程存活判断)
		}
		goto retry_read;  // 重试
	}

	// 处理绕回情况
	tre = que->roundend;
	// 绕回位置无效
	if(unlikely(tre >= wr || tre < rd)){
		return 0;
	}

	// 计算绕回段数据长度
	t = tre - rd;
	if(likely(t > 0)){
		// 数据已写入完成
		if(likely(thead->state == QUE_HEAD_STATE_WRITEOK)){
			return thead->len;
		}
		// 数据已被丢弃
		else if(thead->state == QUE_HEAD_STATE_DISCARD){
			(que->rdcmt) += thead->len + sizeof(que_proc_head);
			goto retry_read;
		}
		
		// 写入中状态处理
		i++;
		if(i == QUE_READ_CHECK_ROUND){
			i = 0;
			busy_read_check(*thead);
		}
		goto retry_read;
	}

	// 跨绕回点读取
	t = (rd & (que->mask));
	if(t != 0){
		rd += ((que->mask) + 1 - t);  // 跳转到缓冲区起始位置
		thead = (que_proc_head *)(pbuf);  // 从头开始读取
		o_data = (pbuf + sizeof(que_proc_head));
	}

	// 检查跨绕回点后的数据
	t = wr - rd;
	if(likely(t > 0)){
		if(likely(thead->state == QUE_HEAD_STATE_WRITEOK)){
			(que->rdcmt) = rd;  // 更新读取提交位置
			return thead->len;
		}
		else if(thead->state == QUE_HEAD_STATE_DISCARD){
			(que->rdcmt) = rd + thead->len + sizeof(que_proc_head);
			goto retry_read;
		}
		
		i++;
		if(i == QUE_READ_CHECK_ROUND){
			i = 0;
			busy_read_check(*thead);
		}
		goto retry_read;
	}
	else{
		return 0;  // 无数据
	}
}

/**
 * @brief 忙等待时检查写入中数据状态
 * @param thead [in] - 数据头部引用
 * @note 处理写入进程崩溃场景，标记孤立数据为丢弃状态
 * @note 调用pthread_yield()减少CPU占用
 */
void que_proc_buf::busy_read_check(que_proc_head &thead)
{
	if(thead.state == QUE_HEAD_STATE_WRITING){
		int32 tlen = thead.len;
		pthread_yield();  // 让出CPU，降低忙等优先级
		
		// 检查写入进程是否存活
		if(thead.pid > 0){
			if(comm_utils::exist_pid(thead.pid) == 0){
				thead.state = QUE_HEAD_STATE_DISCARD;  // 标记为丢弃
				com_write_fence();  // 写屏障确保状态可见性
				(que->rdcmt) += tlen + sizeof(que_proc_head);  // 推进读取位置
			}
		}
	}
}

/**
 * @brief 获取指定位置的可读数据(用户控制读取位置)
 * @param o_data [out] - 输出可读数据区域指针
 * @param io_pos [in/out] - 输入当前读取位置，输出更新后的位置
 * @return int32_t - 成功返回数据长度，失败返回0
 * @note 支持多读者独立读取，通过io_pos参数跟踪各自位置
 */
int32 que_proc_buf::read_get(char *&o_data,int64 &io_pos)
{
	int32 i = 0;          // 重试计数器
	int64 tre;            // 绕回位置标记
	int64 wr;             // 写入提交位置
	que_proc_head *thead; // 数据头部指针

retry_pos_read:
	register int64 curpos = io_pos;  // 当前读取位置
	wr = (que->wrcmt);
	// 无数据可读
	if(unlikely(curpos >= wr))
		return 0;
	// 同步读取位置到最新提交点
	if(unlikely(curpos < (que->rdcmt))){
		curpos = (que->rdcmt);
		io_pos = curpos;
	}

	// 计算当前读取偏移
	register int64 t = (curpos & (que->mask));
	thead = (que_proc_head *)(pbuf + t);
	o_data = (pbuf + t + sizeof(que_proc_head));

	// 计算可用数据长度
	if(likely((wr & (que->mask)) != 0))
		t = (wr & (que->mask)) - t;
	else
		t = wr - curpos;

	if(likely(t > 0)){
		// 数据可用
		if(likely(thead->state == QUE_HEAD_STATE_WRITEOK)){
			return thead->len;
		}
		// 数据已丢弃
		else if(thead->state == QUE_HEAD_STATE_DISCARD){
			curpos += thead->len + sizeof(que_proc_head);
			io_pos = curpos;
			goto retry_pos_read;
		}
		
		// 写入中状态处理
		i++;
		if(i == QUE_READ_CHECK_ROUND){
			int discard_len = busy_readpos_check(*thead);
			if(discard_len != 0){
				curpos += discard_len + sizeof(que_proc_head);
				io_pos = curpos;
			}
			i = 0;
		}
		goto retry_pos_read;
	}

	// 处理绕回情况
	tre = que->roundend;
	if(unlikely(tre >= wr || tre < curpos)){
		return 0;
	}

	t = tre - curpos;
	if(likely(t > 0)){
		if(likely(thead->state == QUE_HEAD_STATE_WRITEOK)){
			return thead->len;
		}
		else if(thead->state == QUE_HEAD_STATE_DISCARD){
			curpos += thead->len + sizeof(que_proc_head);
			io_pos = curpos;
			goto retry_pos_read;
		}
		
		i++;
		if(i == QUE_READ_CHECK_ROUND){
			int discard_len = busy_readpos_check(*thead);
			if(discard_len != 0){
				curpos += discard_len + sizeof(que_proc_head);
				io_pos = curpos;
			}
			i = 0;
		}
		goto retry_pos_read;
	}

	// 跨绕回点读取
	t = (curpos & (que->mask));
	if(t != 0){
		curpos += ((que->mask) + 1 - t);
		thead = (que_proc_head *)(pbuf);
	o_data = (pbuf + sizeof(que_proc_head));
	}

	t = wr - curpos;
	if(likely(t > 0)){
		if(likely(thead->state == QUE_HEAD_STATE_WRITEOK)){
			io_pos = curpos;
			return thead->len;
		}
		else if(thead->state == QUE_HEAD_STATE_DISCARD){
			curpos += thead->len + sizeof(que_proc_head);
			io_pos = curpos;
			goto retry_pos_read;
		}
		
		i++;
		if(i == QUE_READ_CHECK_ROUND){
			int discard_len = busy_readpos_check(*thead);
			if(discard_len != 0){
				curpos += discard_len + sizeof(que_proc_head);
				io_pos = curpos;
			}
			i = 0;
		}
		goto retry_pos_read;
	}
	else{
		return 0;
	}
}

/**
 * @brief 带状态检查的忙等待处理
 * @param thead [in] - 数据头部引用
 * @return int32_t - 成功返回丢弃数据长度，失败返回0
 * @note 与busy_read_check类似，但返回数据长度用于位置更新
 */
int32 que_proc_buf::busy_readpos_check(que_proc_head &thead)
{
	if(thead.state == QUE_HEAD_STATE_WRITING){
		int32 tlen = thead.len;
		pthread_yield();
		if(thead.pid > 0){
			if(comm_utils::exist_pid(thead.pid) == 0){
				thead.state = QUE_HEAD_STATE_DISCARD;
				return tlen;  // 返回丢弃数据长度
			}
		}
	}
	return 0;
}

/**
 * @brief 弹出并读取数据(自动推进读取位置)
 * @param o_buf [out] - 输出数据缓冲区
 * @param buf_len [in] - 缓冲区大小
 * @return int32_t - 成功返回数据长度，-1=缓冲区不足，0=无数据
 * @note 使用CAS原子操作保证多消费者安全
 * @note 完整处理数据状态流转(WRITEOK->READING->IDLE)
 */
int32 que_proc_buf::read_pop(char *o_buf,int32 buf_len)
{
	int32 i = 0;          // 重试计数器
	int64 ts = 0;         // 起始读取位置
	int64 tre;            // 绕回位置标记
	int64 wr;             // 写入提交位置
	que_proc_head *thead; // 数据头部指针
	register int64 rd = (que->rdpos);  // 当前读取位置

retry_pop:
	// 获取当前写入提交位置和读取位置
	wr = (que->wrcmt);
	rd = (que->rdpos);
	// 无数据可读
	if(unlikely(rd >= wr))
		return 0;

	// 计算当前读取偏移
	register int64 t = (rd & (que->mask));
	thead = (que_proc_head *)(pbuf + t);

	// 计算可用数据长度
	if(likely((wr & (que->mask)) != 0))
		t = (wr & (que->mask)) - t;
	else
		t = wr - rd;

	if(likely(t > 0)){
		// 数据已写入完成
		if(likely(thead->state == QUE_HEAD_STATE_WRITEOK)){
			// 检查缓冲区大小
			if(unlikely(thead->len > buf_len))
				return -1;

			// CAS更新读取位置
			wr = rd + thead->len + sizeof(que_proc_head);
			if(likely(com_cas64(&(que->rdpos), rd, wr) == rd)){
				thead->pid = op_pid;          // 标记读取进程ID
				com_write_fence();            // 写屏障确保状态可见性
				thead->state = QUE_HEAD_STATE_READING;  // 标记为读取中
				ts = (ts == 0 ? rd : ts);      // 记录起始读取位置
				// break;  // 成功获取数据，跳出循环
			}
			else{
				goto retry_pop;  // CAS失败重试
			}
		}
		// 数据已丢弃
		else if(thead->state == QUE_HEAD_STATE_DISCARD){
			wr = rd + thead->len + sizeof(que_proc_head);
			com_cas64(&(que->rdpos), rd, wr);  // 推进读取位置
			ts = (ts == 0 ? rd : ts);
			goto retry_pop;
		}
		
		// 写入中状态处理
		i++;
		if(i == QUE_READ_CHECK_ROUND){
			i = 0;
			busy_readpos_check(*thead);
		}
		goto retry_pop;
	}

	// 处理绕回情况
	tre = que->roundend;
	if(unlikely(tre >= wr || tre < rd)){
		return 0;
	}

	t = tre - rd;
	if(likely(t > 0)){
		if(likely(thead->state == QUE_HEAD_STATE_WRITEOK)){
			if(unlikely(thead->len > buf_len))
				return -1;

			wr = rd + thead->len + sizeof(que_proc_head);
			if(likely(com_cas64(&(que->rdpos), rd, wr) == rd)){
				thead->pid = op_pid;
				com_write_fence();
				thead->state = QUE_HEAD_STATE_READING;
				ts = (ts == 0 ? rd : ts);
				// break;
			}
			else{
				goto retry_pop;
			}
		}
		else if(thead->state == QUE_HEAD_STATE_DISCARD){
			wr = rd + thead->len + sizeof(que_proc_head);
			com_cas64(&(que->rdpos), rd, wr);
			ts = (ts == 0 ? rd : ts);
			goto retry_pop;
		}
		
		i++;
		if(i == QUE_READ_CHECK_ROUND){
			i = 0;
			busy_read_check(*thead);
		}
		goto retry_pop;
	}

	// 跨绕回点读取
	tre = rd;
	t = (rd & (que->mask));
	if(t != 0){
		rd += ((que->mask) + 1 - t);
		thead = (que_proc_head *)(pbuf);
	}

	t = wr - rd;
	if(likely(t > 0)){
		if(likely(thead->state == QUE_HEAD_STATE_WRITEOK)){
			if(unlikely(thead->len > buf_len))
				return -1;

			wr = rd + thead->len + sizeof(que_proc_head);
			if(likely(com_cas64(&(que->rdpos), tre, wr) == tre)){
				thead->pid = op_pid;
				com_write_fence();
				thead->state = QUE_HEAD_STATE_READING;
				ts = (ts == 0 ? tre : ts);
				// break;
			}
			else{
				goto retry_pop;
			}
		}
		else if(thead->state == QUE_HEAD_STATE_DISCARD){
			wr = rd + thead->len + sizeof(que_proc_head);
			com_cas64(&(que->rdpos), tre, wr);
			ts = (ts == 0 ? tre : ts);
			goto retry_pop;
		}
		
		i++;
		if(i == QUE_READ_CHECK_ROUND){
			i = 0;
			busy_read_check(*thead);
		}
		goto retry_pop;
	}
	else{
		return 0;
	}

	// 复制数据到输出缓冲区
	i = thead->len;
	memcpy(o_buf, ((char *)thead) + sizeof(que_proc_head), i);
	thead->state = QUE_HEAD_STATE_IDLE;  // 标记为空闲
	
	// 提交读取完成，更新全局读取提交位置
	read_pop_cmt(wr, ts);
	return i;
}

/**
 * @brief 提交读取完成状态，更新全局读取位置
 * @param cmt_pos [in] - 当前读取完成位置
 * @param read_pos [in] - 起始读取位置
 * @note 批量处理连续空闲数据，减少CAS操作次数
 * @note 处理跨绕回点的连续数据提交
 */
void que_proc_buf::read_pop_cmt(int64 cmt_pos,int64 read_pos)
{
	que_proc_head *thead;
	register int64 rc = (que->rdcmt);
	
	// 从起始读取位置开始提交
	if(read_pos == rc){
		do{
			// CAS更新读取提交位置
			if(com_cas64(&(que->rdcmt), rc, cmt_pos) != rc)
				break;
			
			rc = (que->rdcmt);
			// 读取位置已追上，无需继续提交
			if(rc >= (que->rdpos))
				break;
			
			// 计算下一个待检查位置
			if(rc != que->roundend){
				read_pos = rc;
			}
			else{
				read_pos = rc + ((que->mask) + 1 - (rc & (que->mask)));
			}
			
			// 检查下一个数据块状态
			thead = (que_proc_head *)(pbuf + (read_pos & (que->mask)));
			if(thead->state == QUE_HEAD_STATE_IDLE ||
			   thead->state == QUE_HEAD_STATE_DISCARD){
				// 连续空闲数据，继续提交
				cmt_pos = read_pos + thead->len + sizeof(que_proc_head);
				continue;
			}
			break;
		}while(1);
	}
	// 批量提交模式(跳过中间连续空闲数据)
	else if(cmt_pos - rc > que->forward_check){
		while(rc < cmt_pos){
			if(rc != que->roundend){
				read_pos = rc;
			}
			else{
				read_pos = rc + ((que->mask) + 1 - (rc & (que->mask)));
			}
			
			thead = (que_proc_head *)(pbuf + (read_pos & (que->mask)));
			
			// 处理空闲/丢弃状态数据
			if(thead->state == QUE_HEAD_STATE_IDLE ||
			   thead->state == QUE_HEAD_STATE_DISCARD){
				int32 tlen = thead->len;
				// CAS更新提交位置
				if(com_cas64(&(que->rdcmt), rc, read_pos + tlen + sizeof(que_proc_head)) != rc)
					break;
				rc = (que->rdcmt);
				continue;
			}
			// 处理读取中状态数据
			else if(thead->state == QUE_HEAD_STATE_READING){
				int32 tlen = thead->len;
				if(thead->pid == 0) break;
				// 检查读取进程是否存活
				if(comm_utils::exist_pid(thead->pid) == 0){
					thead->state = QUE_HEAD_STATE_DISCARD;
					com_cas64(&(que->rdcmt), rc, read_pos + tlen + sizeof(que_proc_head));
					rc = (que->rdcmt);
					continue;
				}
			}
			break;
		}
	}
}

// -----------------------------------------------------------------------------\n// 缓冲区初始化与管理函数\n// -----------------------------------------------------------------------------\n
/**
 * @brief 计算缓冲区所需大小(向上取2的幂)
 * @param que_size [in] - 期望的缓冲区大小
 * @return int64_t - 实际分配的缓冲区大小(2的幂)
 * @note 环形缓冲区大小必须为2的幂，便于位运算实现取模
 */
int64 que_proc_buf::need_buf_size(int64 que_size)
{
	int32 t = comm_utils::calc_power(que_size);
	return (1L << t);  // 返回2^t
}

/**
 * @brief 初始化共享内存缓冲区
 * @param pinfo [in] - 共享内存控制块指针
 * @param shm_addr [in] - 共享内存基地址
 * @param que_size [in] - 缓冲区大小
 * @param shm_isexist [in] - 是否已存在共享内存(0=新建,1=已存在)
 * @param check_len [in] - 前向检查长度
 * @param user_flag [in] - 用户自定义标志
 * @note 初始化控制块字段，设置缓冲区掩码和初始位置
 * @note 根据共享内存是否存在执行不同初始化逻辑
 */
void que_proc_buf::init_shm(que_proc_info *pinfo, char *shm_addr, int64 que_size,
	int32 shm_isexist, int32 check_len, uint32 user_flag)
{
	pbuf = shm_addr;       // 共享内存数据缓冲区基地址
	que = pinfo;           // 控制块指针
	op_pid = comm_utils::get_pid();  // 当前进程ID
	
	// 计算实际缓冲区大小(2的幂)
	int64 tnum = need_buf_size(que_size);
	
	// 新建共享内存或缓冲区大小不匹配时初始化
	if(shm_isexist == 0 || (que->mask) != tnum){
		(que->wrpos) = CACHE_ALIGN_SIZE;    // 写入位置(缓存行对齐)
		(que->wrcmt) = CACHE_ALIGN_SIZE;    // 写入提交位置
		que->roundend = 0;                  // 绕回位置标记
		(que->mask) = 0;                    // 缓冲区掩码(大小-1)
		que->forward_check = check_len;     // 前向检查长度
		que->userflag = user_flag;          // 用户标志
		que->userpos[0] = CACHE_ALIGN_SIZE; // 用户自定义位置0
		que->userpos[1] = CACHE_ALIGN_SIZE; // 用户自定义位置1
		(que->rdpos) = CACHE_ALIGN_SIZE;    // 读取位置
		(que->rdcmt) = CACHE_ALIGN_SIZE;    // 读取提交位置
		memset(que->userdata, 0, QUE_USER_DATA_LEN);  // 用户数据区
		(que->mask) = tnum - 1;             // 设置掩码(大小-1)
	}
	else{
		// 共享内存已存在，仅更新可变参数
		que->userflag = user_flag;
		que->forward_check = check_len;
	}
}

/**
 * @brief 启动缓冲区，设置初始状态
 * @param recove_type [in] - 恢复类型(重启/继续/自动)
 * @note 根据恢复类型设置不同的初始位置
 * @note 处理异常关闭后的缓冲区恢复
 */
void que_proc_buf::start(int32 recove_type)
{
	// 确保基础位置不小于缓存行对齐值
	if((que->rdcmt) < CACHE_ALIGN_SIZE)
		(que->rdcmt) = CACHE_ALIGN_SIZE;
	if((que->wrcmt) < CACHE_ALIGN_SIZE)
		(que->wrcmt) = CACHE_ALIGN_SIZE;
	(que->wrpos) = (que->wrcmt);
	if(que->roundend > (que->wrcmt))
		que->roundend = (que->wrcmt);
	// 读写位置同步
	if((que->rdcmt) >= (que->wrcmt)){
		(que->rdcmt) = (que->wrcmt);
	}
	(que->rdpos) = (que->rdcmt);

	// 重启模式：重置所有位置
	if(recove_type == QUE_RECOVE_TYPE_RESTART){
		(que->wrpos) = CACHE_ALIGN_SIZE;
		(que->wrcmt) = CACHE_ALIGN_SIZE;
		que->roundend = 0;
		(que->rdpos) = CACHE_ALIGN_SIZE;
		(que->rdcmt) = CACHE_ALIGN_SIZE;
	}
	// 继续模式：读写位置对齐到写入提交位置
	else if(recove_type == QUE_RECOVE_TYPE_CONTINUE){
		(que->wrpos) = (que->wrcmt);
		(que->rdcmt) = (que->wrcmt);
		(que->rdpos) = (que->rdcmt);
	}
	// 自动恢复模式：处理异常关闭的数据
	else{
		int64 tr;          // 临时读取位置
		int64 t = (que->rdcmt);  // 当前读取提交位置
		int64 te = (que->wrcmt) > (que->wrpos) ? (que->wrcmt) : (que->wrpos);
		que_proc_head *thead;
		
		// 遍历所有未处理数据
		while(t < te){
			// 计算当前读取位置
			if(t != que->roundend){
				tr = t;
			}
			else{
				tr = t + ((que->mask) + 1 - (t & (que->mask)));
			}
			// 跳过已处理位置
			if(t < que->rdcmt){
				t = que->rdcmt;
				continue;
			}
			
			// 检查数据状态
			thead = (que_proc_head *)(pbuf + (tr & (que->mask)));
			if(thead->state == QUE_HEAD_STATE_WRITING ||
			   thead->state == QUE_HEAD_STATE_READING){
				int32 tlen = thead->len;
				// 检查写入/读取进程是否存活
				if(thead->pid > 0){
					if(comm_utils::exist_pid(thead->pid) == 0){
						thead->state = QUE_HEAD_STATE_DISCARD;  // 标记为丢弃
					}
				}
			}
			
			// 处理空闲/丢弃状态数据
			if(thead->state == QUE_HEAD_STATE_IDLE){
				if(t == (que->rdcmt)){
					com_cas64(&(que->rdcmt), t, tr + thead->len + sizeof(que_proc_head));
					t = que->rdcmt;
					continue;
				}
			}
			else if(thead->state == QUE_HEAD_STATE_DISCARD){
				if(t == (que->rdcmt)){
					com_cas64(&(que->rdcmt), t, tr + thead->len + sizeof(que_proc_head));
					t = que->rdcmt;
					continue;
				}
				if(t >= (que->wrcmt)){
					com_cas64(&(que->wrcmt), t, tr + thead->len + sizeof(que_proc_head));
				}
			}
			else if(thead->state == QUE_HEAD_STATE_WRITEOK){
				if(t >= (que->wrcmt)){
					com_cas64(&(que->wrcmt), t, tr + thead->len + sizeof(que_proc_head));
				}
			}
			
			// 移动到下一个数据块
			t = tr + thead->len + sizeof(que_proc_head);
		}
	}
	return;
}

}  // namespace lb_common