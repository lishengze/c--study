#pragma once

#include "comm_sys.h"


namespace lb_common{

#define QUE_USER_DATA_LEN          48

//恢复类型
#define QUE_RECOVE_TYPE_RESTART    0  //读写从新开始
#define QUE_RECOVE_TYPE_CONTINUE   1  //读从写位置继续
#define QUE_RECOVE_TYPE_RESERVE    2  //读写位置继续


#define QUE_HEAD_STATE_IDLE        0
#define QUE_HEAD_STATE_WRITING     1
#define QUE_HEAD_STATE_WRITEOK     2
#define QUE_HEAD_STATE_READING     4
#define QUE_HEAD_STATE_DISCARD     8


#define QUE_READ_CHECK_ROUND       2000


struct que_mth_head
{
	int32 len;
	volatile int32 state;
};

struct que_proc_head
{
	int64 pid;
	int32 len;
	volatile int32 state;
};


}

