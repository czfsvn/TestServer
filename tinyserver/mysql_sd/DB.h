#ifndef __DB_20170722_H__
#define __DB_20170722_H__

#include "DBBase.h"

#define DB_MAX_NAME_SIZE                64
#define DB_MAX_CHARACTSET_SIZE          32

const char  CNDB_MODULENAME[]        = "CN_DB";

const PVersion  CNDB_VERSION    = P_VERSION;

enum ESDDBCode
{    
    DB_ERR_UNKNOWN    = -999, // Unknown Error    
    DB_ERR_CONN       = -2,   // Failed to connect to DB server    
    DB_DISCONNECT     = -1,   // Disconnect from DB server    
    DB_SUCCESS        = 0,    // Operation succeed    
    DB_NO_RECORDSET   = 1,    // Operation succeed, no rows affected    
    DB_HAS_RECORDSET  = 2     // Operation succeed, some rows affected
};

struct DBAccount
{
    char m_szHostName[DB_MAX_NAME_SIZE];          // IP address with string format 
                                                    // of DB Server
    char m_szDBName[DB_MAX_NAME_SIZE];            // Name of the Database to be connect
    char m_szLoginName[DB_MAX_NAME_SIZE];         // Login Name
    char m_szLoginPwd[DB_MAX_NAME_SIZE];          // User password
    char m_szCharactSet[DB_MAX_CHARACTSET_SIZE];  // Database encoding
    uint16 m_wConnPort;                             // Listen Port of DB Server, Local 
                                                    // host byte order
    char m_szRsv[2];                                // Reserved
};

struct DBConnInfo
{
    int32  m_nSN;           // DB Connection Identity
    DBAccount m_stAccount; // DB Login Account Information structure
};

class IMySqlRecordSet
{
    public:
        virtual ~IMySqlRecordSet() {}

        virtual uint32  GetRecordCount(void) =  0;

        virtual uint32  GetFieldCount(void) =   0;

        virtual bool GetRecord(void) =  0;

        virtual const char* GetFieldValue(uint32 dwIndex) =     0;

        virtual uint32  GetFieldLength(uint32 dwIndex) =    0;

        virtual void Release(void)  =   0;
};

class IMySqlConn
{
    public:
        virtual uint32 EscapeString(const char* pSrc, int32 nSrcSize,
                                    char* pszDest,    int32 nDstSize) = 0;

        virtual int32 ExecuteSql(const char* pszSQL) = 0;

        virtual int32 ExecuteSqlRc(const char* pszSQL, IMySqlRecordSet** ppoRc) = 0;

        virtual uint32  GetLastError(void) = 0;

        virtual const char* GetLastErrorInfo(void) = 0;

        virtual uint32 GetDBCommandNum() = 0;
};

class IDBCommand
{
    public:
        virtual int32 GetDBSn(void) = 0;

        virtual void OnExecuteSql(IMySqlConn* poDBConnection) = 0;

        virtual void OnExecute(void) = 0;

        virtual void Release(void)  = 0;
};


class IDBModule : public IDBBase
{
    public:
        virtual bool Connect(DBConnInfo* pstDBConnInfo) = 0;

        virtual void DisConnect(int32 dbSn) =   0;

        virtual uint32 EscapeString(int32 nDBSn, const char* pcSrc,
                int32 nSrcSize, char* pszDest, 
                int32 nDstSize  ) = 0;

        virtual bool AddDBCommand(IDBCommand* poDBCmd)  = 0;

        virtual bool Run(int32 nCount = -1) = 0;

        virtual uint32 GetDBCommandNum(int nDBSn = -1) = 0;
};

IDBModule* DBGetModule(const PVersion* pstVersion);

typedef IDBModule* (*PFN_DBGetModule)(const PVersion* pstVersion);

#endif
