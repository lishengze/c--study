#include "mysql_pool.h"
#include <assert.h>
#include <algorithm>

//////////////////////////////////////////////////////////////////////
// CMysqlConn: mysql数据库连接类
//////////////////////////////////////////////////////////////////////

CMysqlConn::CMysqlConn(const char* pszDBServer, UINT uDBPort, const char* pszDBName, 
						const char* pszDBUser, const char* pszDBPwd)
{
	assert(pszDBServer);
	assert(pszDBName);
	assert(pszDBUser);
	assert(pszDBPwd);

	m_pMysql = NULL;
	m_strDBServer = pszDBServer;
	m_uDBPort = uDBPort;
	m_strDBName = pszDBName;
	m_strDBUser = pszDBUser;
	m_strDBPwd = pszDBPwd;
}

CMysqlConn::~CMysqlConn()
{
	Close();
}

// 打开一个mysql数据库，即建立一个数据库连接
BOOL CMysqlConn::Open()
{
	if(m_pMysql)
	{
		mysql_close(m_pMysql);	// 关闭连接	
		m_pMysql = NULL;
	}
	
	m_pMysql = mysql_init(NULL);
	if(!m_pMysql)
		return FALSE;
	
	// 连接数据库
    if(!mysql_real_connect(m_pMysql, m_strDBServer.c_str(), m_strDBUser.c_str(),
							m_strDBPwd.c_str(), m_strDBName.c_str(), m_uDBPort, NULL, 0))
    {
		int i = mysql_errno(m_pMysql);
		const char * pszErr = mysql_error(m_pMysql);

		return FALSE;
	}
	
	// 设置重连
	char chValue = 1;
	mysql_options(m_pMysql, MYSQL_OPT_RECONNECT, &chValue);	
	mysql_query(m_pMysql,"set names 'gbk'"); 
	
	return TRUE;
}

// 关闭数据库连接
void CMysqlConn::Close()
{
	if(m_pMysql)
		mysql_close(m_pMysql);	// 断开连接
	m_pMysql = NULL;	
}

// ping一下mysql，看看连接还活着
BOOL CMysqlConn::Ping()
{
	if(m_pMysql)
		return (0 == mysql_ping(m_pMysql));
	return FALSE;
}

// 设置字符集为GBK
BOOL CMysqlConn::ResetCharset()
{
	if(m_pMysql)
		return (0 == mysql_query(m_pMysql, "set names 'gbk'")); 
	return FALSE;
}

// mysql执行：delete 或 update
BOOL CMysqlConn::Execute(const char* pszSql)
{
	assert(pszSql);

	if(!m_pMysql)
		return FALSE;
	
	MYSQL_STMT *myStmt = mysql_stmt_init(m_pMysql);
	if(!myStmt)
	{
		return FALSE;
	}
	
	if(0 != mysql_stmt_prepare(myStmt, pszSql, strlen(pszSql)))
	{
		mysql_stmt_close(myStmt);
		return FALSE;
	}
	if(0 != mysql_stmt_execute(myStmt))
	{
		mysql_stmt_close(myStmt);
		return FALSE;
	}
	mysql_stmt_close(myStmt);
	
	return TRUE;		
}

// mysql插入
__int64 CMysqlConn::Insert(const char* pszSql)
{	
	assert(pszSql);

	MYSQL_STMT *myStmt = mysql_stmt_init(m_pMysql);
	if(!myStmt)
		return 0;
	
	if(0 != mysql_stmt_prepare(myStmt, pszSql, strlen(pszSql)))
	{
		int i = mysql_errno(m_pMysql);
		const char * s = mysql_error(m_pMysql);
		mysql_stmt_close(myStmt);
		return 0;
	}
	if(0 != mysql_stmt_execute(myStmt))
	{
		mysql_stmt_close(myStmt);
		return 0;
	}
	mysql_stmt_close(myStmt);
	
	__int64 i64ID = mysql_insert_id(m_pMysql);	
	return i64ID;
}

// mysql查询
BOOL CMysqlConn::Select(const char* pszSql, LPFN_RetrieveRecordData lpfnRetrieveRecordData, DWORD dwUserData)
{
	if(!m_pMysql)
		return FALSE;
	
	if(NULL == lpfnRetrieveRecordData)
		return FALSE;
	
	if(0 != mysql_real_query(m_pMysql, pszSql, strlen(pszSql)))
	{
		return FALSE;	
	}
	
	MYSQL_RES *resRecord = mysql_store_result(m_pMysql);
	int iFieldCount = resRecord->field_count;
	
	MYSQL_ROW sqlRow;
	while (sqlRow = mysql_fetch_row(resRecord))
    {
		if(!lpfnRetrieveRecordData(sqlRow, resRecord->fields, iFieldCount, dwUserData))
			break;
	}
	mysql_free_result(resRecord);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CMysqlPool: mysql数据库连接池类
//////////////////////////////////////////////////////////////////////

CMysqlPool::CMysqlPool()
{
	::InitializeCriticalSection(&m_csPool);
}

CMysqlPool::~CMysqlPool()
{
	Destroy();
	::DeleteCriticalSection(&m_csPool);
}

// 创建mysql连接池
BOOL CMysqlPool::Create(const char* pszDBServer, UINT uDBPort, const char* pszDBName, 
						const char* pszDBUser, const char* pszDBPwd,
						DWORD dwPoolSize, DWORD dwTimeOut)
{
	m_dwTimeOut = dwTimeOut;
	m_dwPoolSize = dwPoolSize;
	
	// 创建信号量
	m_hSemaphore = ::CreateSemaphore(NULL, dwPoolSize, dwPoolSize, NULL);
	if (NULL == m_hSemaphore)
	{
		return FALSE;
	}
	
	// 创建数据库连接池
	for(DWORD i = 0; i < dwPoolSize; ++i)
	{
		// 创建一个mysql数据库连接
		CMysqlConn *pConn = new CMysqlConn(pszDBServer, uDBPort, pszDBName, pszDBUser, pszDBPwd);
		if(!pConn->Open())	
		{
			delete pConn;
			continue;
		}
		m_vecIdle.push_back(pConn);
	}

	return m_vecIdle.size() > 0;
	
}

// 销毁mysql连接池
void CMysqlPool::Destroy()
{
	::CloseHandle(m_hSemaphore);
	m_hSemaphore = NULL;
	
	// 释放idle队列
	vector<CMysqlConn*>::iterator it;
    for(it = m_vecIdle.begin(); it != m_vecIdle.end(); ++it)
	{
		CMysqlConn* pConn =  *it;
		delete pConn;
	}
	m_vecIdle.clear();
	
	// 释放busy队列
	while(!m_vecBusy.empty())
	{
		CMysqlConn* pConn =  m_vecBusy.back();
		m_vecBusy.pop_back();
		delete pConn;
	}	
}

// 从mysql连接池获取一个连接
CMysqlConn* CMysqlPool::Get()
{
	DWORD dwRet = ::WaitForSingleObject(m_hSemaphore, m_dwTimeOut*1000);
	
	if (WAIT_OBJECT_0 != dwRet)				// 超时，说明资源池没有可用mysql连接
	{
		printf("数据库没有可用连接。\r\n");
		return NULL;
	}
	
	// 从连接池中获取一个闲置连接
	CMysqlConn* pConn = NULL;

	::EnterCriticalSection(&m_csPool);

 	if (!m_vecIdle.empty())
 	{
		pConn = m_vecIdle.back();			// 移出idle队列
 		m_vecIdle.pop_back();	
 		m_vecBusy.push_back(pConn);			// 加入busy队列
 	}
	::LeaveCriticalSection(&m_csPool);
	
	if(NULL == pConn)
		return NULL;
	
	// 如果一个连接长时间无通信，可能被防火墙关闭，此时可以通过mysql_ping函数测试一下
	// 本例中通过重新设置字符集
	// 重新设置字符集，并判断数据库连接是否已断开
	if(!pConn->ResetCharset())			
	{
		if(!pConn->Open())
			return NULL;
	}
	
	printf("==》资源池：记得还我哦。\r\n");
	return pConn;
}

// 释放一个连接到mysql连接池
void CMysqlPool::Release(CMysqlConn* pConn)
{
	if(NULL == pConn)
		return;
	
	// 释放一个信号量
	::ReleaseSemaphore(m_hSemaphore, 1, NULL); 

	::EnterCriticalSection(&m_csPool);

	// 从Busy队列中释放该连接
	vector<CMysqlConn*>::iterator it = find(m_vecBusy.begin(), m_vecBusy.end(), pConn);
	if(it != m_vecBusy.end())
	{
		printf("POOL SIZE : %d, %d\r\n", m_vecIdle.size(), m_vecBusy.size());
		m_vecBusy.erase(it);				// 移出busy队列
		m_vecIdle.push_back(pConn);			// 加入idle队列
		printf("POOL SIZE : %d, %d\r\n", m_vecIdle.size(), m_vecBusy.size());
	}
	::LeaveCriticalSection(&m_csPool);
	
	printf("《==资源池说：有借有还再借不难，常来玩啊。\r\n");
}
