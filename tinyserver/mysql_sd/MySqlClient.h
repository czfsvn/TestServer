#ifndef __MYSQL_CLIENT_20170722_H__
#define __MYSQL_CLIENT_20170722_H__

#include <list>
#include "Thread.h"
#include "Sync.h"
#include "DB.h"


class cMySqlConnection;
typedef std::list<IDBCommand*>  CDBCmdList;
// 
// 类名称   : cMySqlClient
// 功能     : <继承并实现了IMySqlConn， IThread的接口，提供数据库的连接和请求的
//              添加，并且能执行该数据库操作请求>
//
class cMySqlClient : public IMySqlConn, public CN::IThread
{
    public:
        cMySqlClient();
        ~cMySqlClient();

        //  
        // 函数名:  EscapeString    
        // 功能:    将二进制数据转义成MYSQL查询语句中使用的字符串   
        // 参数:    [in] pSrc: 需要进行转义的数据 
        //          [in] nSrcSize 需要转义的数据的长度，即pSrc所指数据的长度    
        //          [in] pszDest 存放转义后的字符串的缓冲区 
        //          [in] nDstSize pszDest's max-size   
        // 返回值:  转义成功后返回值大于0，转义失败返回值等于0  
        //  
        virtual uint32 EscapeString(const char* pSrc, int32 nSrcSize, 
                char* pszDest, int32 nDstSize);

        //  
        // 函数名:  ExecuteSql  
        // 功能:    将二进制数据转义成MYSQL查询语句中使用的字符串   
        // 参数:    [in] pszSQL: 需要执行的SQL语句   
        // 返回值:  返回值小于0时表示出错，错误码为 
        //          SDDB_ERR_UNKONWN    
        //          SDDB_ERR_CONN   
        //          返回值>=0时表示影响的记录条数   
        //  
        virtual int32  ExecuteSql  (const char* pszSQL);

        //  
        // 函数名:  ExecuteSqlRc    
        // 功能:    执行需要返回记录集的SQL语句，如select操作的SQL语句  
        // 参数:    [in] pszSQL: 需要执行的SQL语句    
        //          [out] SQL语句执行后返回的记录集，只有当SQL语句执行成功后，才会通过ppRc 
        //               返回记录机指针，执行失败后ppRc为空，注意：ppRc使用完后需要释放  
        // 返回值:  返回执行SQL语句的结果， 
        //          SDDB_ERR_UNKONWN    
        //          SDDB_ERR_CONN   
        //          SDDB_NO_RECORDSET   
        //          SDDB_HAS_RECORDSET  
        //  
        virtual int32  ExecuteSqlRc(const char* pszSQL, IMySqlRecordSet **ppRc);

        //  
        // 函数名:  GetLastError    
        // 功能:    得到最后一次出错的错误码    
        // 返回值:  返回最后一次出错的mysql错误码，错误码的含义请参见mysql帮助手册    
        //  
        virtual uint32 GetLastError(void);

        
        //  
        // 函数名:  GetLastError    
        // 功能:    得到最后一次出错的错误描述信息  
        // 返回值:  返回最后一次出错的mysql错误描述信息   
        //  
        virtual const char * GetLastErrorInfo(void);

    
        // 函数名:  GetLastError
        // 功能:    得到此连接上的所有事件数量
        // 返回值:  返回事件数量值
        virtual uint32 GetDBCommandNum();

        virtual void ThrdProc();
        virtual void Terminate();

        bool Start();
        void Stop();
        bool ConnectDB(const char *pszHostName,int32 nPort, const char *pszDBName, 
                const char *pszUser, const char *pszPasswd, 
                const char *pszCharactSet);
        bool AddRequest(IDBCommand *poDBCommand);
        bool AddRequestFront(IDBCommand *poDBCommand);

    private:
        void _ClearReqQueue();
        bool ExecuteDBCmd( void );

    protected:
        bool                m_bTerminate;
        cMySqlConnection*   m_poMySqlConn;    
        bool                m_bDbOK;
        CN::ILock*              m_poLock;
        CN::IThreadCtrl*        m_poThrdCtrl;
        CDBCmdList          m_oDBCmdList;
        time_t              m_nLastReConn;
};

#endif
