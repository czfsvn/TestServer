#ifndef __MYSQL_CONNECTION_20170722_H__
#define __MYSQL_CONNECTION_20170722_H__

#include <cstring>
#include <mysql/mysql.h>
#include "DB.h"
#include "MySqlRecordSet.h"

enum 
{
    CHARACTER_SET_CLIENT = 0,
    CHARACTER_SET_CONNECTION,
    CHARACTER_SET_DATABASE,
    CHARACTER_SET_RESULTS,
    CHARACTER_SET_SERVER,
    CHARACTER_SET_SYSTEM,
    CHARACTER_SET_COUNT,
};

class cMySqlConnection 
{
    public:
        cMySqlConnection(void);
        ~cMySqlConnection(void);

        void SetConnectString(const char* pszServerAddr, int32 nServerPort, 
                const char* pszDBName, const char* pszUserID, 
                const char* pszPassword, const char* pszCharactSet);
        
        bool Connect(const char* pszServerAddr, int32 nServerPort, 
                const char* pszDBName, const char* pszUserID, 
                const char* pszPassword, const char* pszCharactSet);

        bool Connect();
        
        bool ReConnect();

        bool CheckConnection();

        void Close();

        //  
        // 函数名:  GetAffectedRows 
        // 功能:    返回受到最后一个UPDATE、DELETE或INSERT查询影响(变化)的行数
        //      
        uint32 GetAffectedRows() 
        { 
            return (uint32)mysql_affected_rows(m_myConnection); 
        }

        //  
        // 函数名:  EscapeString    
        // 功能:    将二进制数据转义成MYSQL查询语句中使用的字符串   
        // 参数:    [in] pSrc: 需要进行转义的数据  
        //          [in] pszDest 存放转义后的字符串的缓冲区 
        // 返回值:  转义成功后返回值大于0，转义失败返回值等于0  
        //
        uint32 EscapeString(const char* pszSrc, char* pszDest)
        {
            return EscapeStringEx(pszSrc, (int32)strlen(pszSrc), pszDest);
        }

        //   
        // 函数名:  EscapeString    
        // 功能:    将二进制数据转义成MYSQL查询语句中使用的字符串   
        // 参数:    [in] pSrc: 需要进行转义的数据  
        //          [in] nSize 需要转义的数据的长度，即pSrc所指数据的长度   
        //          [in] pszDest 存放转义后的字符串的缓冲区  
        // 返回值:  转义成功后返回值大于0，转义失败返回值等于0  
        //  
        uint32 EscapeStringEx(const char* pszSrc, int32 nSize, char* pszDest); 

        //   
        // 函数名:  ExecuteEx   
        // 功能:    执行需要返回记录集的SQL语句，如select操作的SQL语句  
        // 参数:    [in] pszSQL: 需要执行的SQL语句   
        //          [out] SQL语句执行后返回的记录集，只有当SQL语句执行成功后，才会通过ppRc    
        //                  返回记录机指针，执行失败后ppRc为空，注意：ppRc使用完后需要释放  
        // 返回值:  返回执行SQL语句的结果， 
        //  
        int32 ExecuteEx(const char* pszSQL, cMySqlRecordSet& rcdSet); 

        //   
        // 函数名:  ExecuteEx   
        // 功能:    执行需要返回记录集的SQL语句，如select操作的SQL语句  
        // 参数:    [in] pszSQL: 需要执行的SQL语句   
        //          [in] nLength: SQL语句长度   
        //          [out] SQL语句执行后返回的记录集，只有当SQL语句执行成功后，才会通过ppRc    
        //                  返回记录机指针，执行失败后ppRc为空，注意：ppRc使用完后需要释放    
        // 返回值:  返回执行SQL语句的结果， 
        //  
        int32 ExecuteEx(const char* pszSQL, int32 nLength, cMySqlRecordSet& rcdSet); 

        //  
        // 函数名:  Execute 
        // 功能:    将二进制数据转义成MYSQL查询语句中使用的字符串   
        // 参数:    [in] pszSQL: 需要执行的SQL语句    
        // 返回值:  返回值小于0时表示出错，错误码为 
        //          SDDB_ERR_UNKONWN    
        //          SDDB_ERR_CONN   
        //          返回值>=0时表示影响的记录条数   
        //  
        int32 Execute(const char* pszSQL); 
        //   
        // 函数名:  GetLastError    
        // 功能:    得到最后一次出错的错误码    
        // 返回值:  返回最后一次出错的mysql错误码，错误码的含义请参见mysql帮助手册    
        uint32 GetLastError(); 

        //  
        // 函数名:  GetLastError    
        // 功能:    得到最后一次出错的错误描述信息  
        // 返回值:  返回最后一次出错的mysql错误描述信息   
        //  
        const char* GetLastErrorInfo();

        bool GetDBSCharSetInfo();               
        bool IsCompatiableCharSet();   
        bool SetServerNames();
        bool IsValidCharacterSet(const char* pszSetName);   

    private:
        char    m_szServerAddr[DB_MAX_NAME_SIZE];
        char    m_szDBName[DB_MAX_NAME_SIZE];
        char    m_szUserID[DB_MAX_NAME_SIZE];
        char    m_szPassword[DB_MAX_NAME_SIZE];
        char    m_szCharactSet[DB_MAX_CHARACTSET_SIZE];
        
        int32   m_nServerPort;
        MYSQL*  m_myConnection;
        bool    m_bConnected;

        //DBServer 的使用的编码集
        char                m_szDBSCharSet[CHARACTER_SET_COUNT][DB_MAX_CHARACTSET_SIZE];
};

#endif
