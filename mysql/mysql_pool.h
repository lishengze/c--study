#pragma warning(disable : 4786)	

#include "global_declare.h"


#include <vector>
#include <string>
using namespace std;

#define DEFAULT_POOL_SIZE		20	// 缺省mysql连接池中的数量
#define DEFAULT_POOL_TIMEOUT	60	// 获取池中mysql连接的超时
#define MYSQL_ROW               int
#define MYSQL_FIELD             char
#define DWORD                   long long
#define UINT                    unsigned int
#define BOOL                    bool
#define __int64                 long long
#define MYSQL                   sql::Connection*
#define FALSE                   false
#define TRUE                    true

// 自定义数据库查询回调函数
typedef BOOL (* LPFN_RetrieveRecordData)(MYSQL_ROW& sqlRow, MYSQL_FIELD* pSqlFields, int iFieldCount, DWORD dwUserData);

// Mysql数据库连接类
class CMysqlConn
{
public:
	CMysqlConn(const char* pszDBServer, UINT uDBPort, const char* pszDBName, 
				const char* pszDBUser, const char* pszDBPwd);
	virtual ~CMysqlConn();
	
public:
	// 打开/关闭一个mysql连接
	BOOL Open();
	void Close();
	
	// ping连接是否已关闭
	BOOL Ping();
	// 重置字符集
	BOOL ResetCharset();
	
	
public:
	// ================SQL语句操作(简单实现几个)================
	// 查询
	BOOL	Select(const char* pszSql, LPFN_RetrieveRecordData lpfnRetrieveRecordData, DWORD dwUserData);
	// 执行
	BOOL	Execute(const char* pszSql);
	// 插入，如果主键是自增整型，返回插入后的主键值
	__int64 Insert(const char* pszSql);
	
	
protected:
	MYSQL*	m_pMysql;			// mysql数据库操作对象

	// 以下是连接mysql需要的参数
	string	m_strDBServer;		// mysql数据库所在服务器
	UINT	m_uDBPort;			// mysql数据库连接端口
	string	m_strDBName;		// 数据库名称
	string	m_strDBUser;		// 数据库账户
	string	m_strDBPwd;			// 数据库密码
	
};

// 数据库连接池实现
class CMysqlPool  
{
public:
	CMysqlPool();
	virtual ~CMysqlPool();

	// 创建mysql连接池
	BOOL Create(const char* pszDBServer, UINT uDBPort, const char* pszDBName, 
				const char* pszDBUser, const char* pszDBPwd,
	 			DWORD dwPoolSize = DEFAULT_POOL_SIZE, 
				DWORD dwTimeOut = DEFAULT_POOL_TIMEOUT);			
	// 销毁连接池
 	void Destroy();
 	
public:

	// 获取一个mysql连接
 	CMysqlConn* Get();

	// 释放一个mysql连接
 	void Release(CMysqlConn* pConn);
	
protected:
	HANDLE				m_hSemaphore;       // 信号量句柄
	DWORD				m_dwPoolSize;		// 连接池大小 
	DWORD				m_dwTimeOut;		// 超时，单位秒
	std::mutex    	    m_csPool;			// 连接池锁

	vector<CMysqlConn*>	m_vecIdle;			// 闲队列
	vector<CMysqlConn*>	m_vecBusy;			// 忙队列
};
