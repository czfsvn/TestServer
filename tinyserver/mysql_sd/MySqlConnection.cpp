#include "MySqlConnection.h"
#include "StrHelper.h"
#include <mysql/errmsg.h>
#include <cstdio>
#include <iostream>

const static char*  g_stpCharSets[] =
{
    "big5",
    "dec8",
    "cp850",
    "hp8",
    "koi8r",
    "latin1",
    "latin2",
    "swe7",
    "ascii",
    "ujis",
    "sjis",
    "hebrew",
    "tis620",
    "euckr",
    "koi8u",
    "gb2312",
    "greek",
    "cp1250",
    "gbk",
    "latin5",
    "armscii8",
    "utf8",
    "ucs2",
    "cp866",
    "keybcs2",
    "macce",
    "macroman",
    "cp852",
    "latin7",
    "cp1251",
    "cp1256",
    "cp1257",
    "binary",
    "geostd8",
    "cp932",
    "eucjpms",
    NULL,
};

cMySqlConnection::cMySqlConnection(void)
{
    ZeroString(m_szServerAddr);
    ZeroString(m_szDBName);
    ZeroString(m_szUserID);
    ZeroString(m_szPassword);
    //ZeroString(m_szCharactSet);
    m_myConnection = NULL;
    m_bConnected   = false;
    //memset(m_szDBSCharSet, 0, sizeof(m_szDBSCharSet));
    StrSafeCopy(m_szCharactSet, "latin1");
}

cMySqlConnection::~cMySqlConnection(void)
{
    if (m_myConnection != NULL)
    {
        mysql_close(m_myConnection);
    }
}

void cMySqlConnection::SetConnectString(const char* pszServerAddr, int32 nServerPort, 
        const char* pszDBName, const char* pszUserID, const char* pszPassword, 
        const char* pszCharactSet)
{
    StrSafeCopy(m_szServerAddr, pszServerAddr);
    StrSafeCopy(m_szDBName, pszDBName);
    StrSafeCopy(m_szUserID, pszUserID);
    StrSafeCopy(m_szPassword, pszPassword);
    if ('\0' != pszCharactSet[0])
    {
        StrSafeCopy(m_szCharactSet, pszCharactSet);
        //INFO("设置DB连接编码为:%s\n", m_szCharactSet);
    }
    else
    {
        //INFO("没有设置DB连接编码, 使用默认编码%s\n", m_szCharactSet);
    }
    m_nServerPort = nServerPort ;
}


bool cMySqlConnection::Connect(const char* pszServerAddr, int32 nServerPort, const char* pszDBName, 
        const char* pszUserID, const char* pszPassword, const char* pszCharactSet)
{
    SetConnectString(pszServerAddr, nServerPort, pszDBName, pszUserID, pszPassword, pszCharactSet);
    return Connect();
}


bool cMySqlConnection::Connect()
{
    if (NULL == m_myConnection)
    {
        m_myConnection = mysql_init(NULL);
    }
    if (NULL == m_myConnection)
    {
        return false;
    }

    if('\0' != m_szCharactSet[0])
    {
        if (false == IsValidCharacterSet(m_szCharactSet))
        {
            return false;
        }
        if(mysql_options(m_myConnection, MYSQL_SET_CHARSET_NAME, m_szCharactSet))
        {
            return false;
        }
    }

    MYSQL* myConn = mysql_real_connect(m_myConnection, m_szServerAddr, 
            m_szUserID, m_szPassword, m_szDBName, m_nServerPort, NULL, CLIENT_MULTI_STATEMENTS);
    if (NULL == myConn)
    {
        //char* pErrStr = (char*)mysql_error(m_myConnection);
        //CRITICAL("mysql_real_connect failed: %s\n", pErrStr);
        Close();
        return false;
    }

    m_bConnected = true;

    if (false == SetServerNames())
    {
        Close();
        return false;
    }

    if (false == GetDBSCharSetInfo())
    {
        Close();
        return false;
    }

    if (false == IsCompatiableCharSet())
    {
        //CRITICAL("编码不一致：应用程序设置编码为%s\n", m_szCharactSet);
        Close();
        return false;
    }

    return true;
}

void cMySqlConnection::Close()
{
    if(m_myConnection != NULL)
    {
        mysql_close(m_myConnection);
        m_myConnection = NULL;
    }

    m_bConnected   = false;
}

bool cMySqlConnection::ReConnect()
{
    Close();
    return Connect();
}

bool cMySqlConnection::CheckConnection()
{
    if(NULL == m_myConnection)
    {
        return Connect();
    }

    return true;
}

int32 cMySqlConnection::Execute(const char* pszSQL)
{
    if(false == CheckConnection())
    {
        return DB_ERR_CONN;
    }

    if (0 != mysql_query(m_myConnection, pszSQL))
    {
        uint32 dwErr = mysql_errno(m_myConnection);
        if(CR_SERVER_LOST == dwErr || CR_CONN_HOST_ERROR == dwErr || CR_SERVER_GONE_ERROR == dwErr)
        {
            if(false == ReConnect())
            {
                return DB_ERR_CONN;
            }

            if(0 != mysql_query(m_myConnection, pszSQL))
            {
                return DB_ERR_UNKNOWN;
            }
        }
        else
        {
            return DB_ERR_UNKNOWN;
        }
    }
    mysql_free_result(mysql_store_result(m_myConnection));

    return (int32)mysql_affected_rows(m_myConnection);
}

int32 cMySqlConnection::ExecuteEx(const char* pszSQL, cMySqlRecordSet& rcdSet)
{
    if(false == CheckConnection())
    {
        return DB_ERR_CONN;
    }

    if (0 != mysql_query(m_myConnection, pszSQL))
    {
        uint32 dwErr = mysql_errno(m_myConnection);
        if(CR_SERVER_LOST == dwErr || CR_CONN_HOST_ERROR == dwErr || CR_SERVER_GONE_ERROR == dwErr)
        {
            if(false == ReConnect())
            {
                return DB_ERR_CONN;
            }

            if(0 != mysql_query(m_myConnection, pszSQL))
            {
                return DB_ERR_UNKNOWN;
            }
        }
        else
        {
            return DB_ERR_UNKNOWN;
        }
    }

    if (rcdSet.m_res != NULL)
        mysql_free_result(rcdSet.m_res); 
    rcdSet.m_res = mysql_store_result(m_myConnection);
    if(NULL == rcdSet.m_res)
    {
        if(0 == mysql_field_count(m_myConnection))      //执行的是insert或update语句
        {
            return DB_NO_RECORDSET;
        }
        else
        {
            return DB_ERR_UNKNOWN;
        }

        return DB_HAS_RECORDSET;       ///Rescord Set
    }
    else
    {
        if(mysql_num_rows(rcdSet.m_res) != 0)
        {
            return DB_HAS_RECORDSET;
        }
        else
        {
            return DB_NO_RECORDSET;
        }
    }

    return  DB_ERR_UNKNOWN;
}

int32 cMySqlConnection::ExecuteEx(const char* pszSQL, int32 nLength, cMySqlRecordSet& rcdSet)
{
    if(false == CheckConnection())
    {
        return DB_ERR_CONN;
    }

    if (0 != mysql_real_query(m_myConnection, pszSQL, nLength))
    {
        uint32 dwErr = mysql_errno(m_myConnection);
        if(CR_SERVER_LOST == dwErr || CR_CONN_HOST_ERROR == dwErr || CR_SERVER_GONE_ERROR == dwErr)
        {
            if(false == ReConnect())
            {
                return DB_ERR_CONN;
            }

            if(0 != mysql_real_query(m_myConnection, pszSQL, nLength))
            {
                return DB_ERR_UNKNOWN;
            }
        }
        else
        {
            return DB_ERR_UNKNOWN;
        }
    }

    if (rcdSet.m_res != NULL)
        mysql_free_result(rcdSet.m_res); 
    rcdSet.m_res = mysql_store_result(m_myConnection);
    if(NULL == rcdSet.m_res)
    {
        if(0 == mysql_field_count(m_myConnection))      //执行的是insert或update语句
        {
            return DB_NO_RECORDSET;
        }
        else
        {
            return DB_ERR_UNKNOWN;
        }

        return DB_HAS_RECORDSET;       ///Rescord Set
    }
    else
    {
        if(mysql_num_rows(rcdSet.m_res) != 0)
        {
            return DB_HAS_RECORDSET;
        }
        else
        {
            return DB_NO_RECORDSET;
        }
    }

    return  DB_ERR_UNKNOWN;
}

uint32 cMySqlConnection::EscapeStringEx(const char* pszSrc, int32 nSize, char* pszDest)
{
    if (NULL == m_myConnection)
    {
        return 0;
    }

#ifdef WIN32
    __try
#else
        try
#endif
        {
            return mysql_real_escape_string(m_myConnection, pszDest, pszSrc, nSize);
        }
#ifdef WIN32
    __except(EXCEPTION_EXECUTE_HANDLER)
#else
        catch(...)
#endif
        {
            return 0;
        }
}

uint32 cMySqlConnection::GetLastError() 
{ 
    if(m_myConnection!=NULL)
    {
        return (uint32)mysql_errno(m_myConnection);
    }
    return  0;
}

const char* cMySqlConnection::GetLastErrorInfo()
{ 
    if(m_myConnection!=NULL)
    {
        return mysql_error(m_myConnection);
    }
    return NULL;
}

bool cMySqlConnection::GetDBSCharSetInfo()
{

    //WARING:　sql 语句中变量的顺序一定和枚举定义一致
    std::string pszSql = "select @@character_set_client, @@character_set_connection,\
                    @@character_set_database, @@character_set_results,@@character_set_server,\
                    @@character_set_system";


    cMySqlRecordSet rcdSet;

    if (DB_HAS_RECORDSET != ExecuteEx(pszSql.c_str(), rcdSet))
    {
        return false;
    }
    if (true == rcdSet.GetRecord())
    {
        for (int i = 0; i < CHARACTER_SET_COUNT; i++)
        {
            const char* pszCharSetName = rcdSet.GetFieldValue(i);
            StrSafeCopy(m_szDBSCharSet[i], pszCharSetName);
        }
        /*
        INFO("DBServer编码信息:\n");
        INFO("character_set_client    :%s\n",      m_szDBSCharSet[0]);
        INFO("character_set_connection:%s\n", m_szDBSCharSet[1]);
        INFO("character_set_database  :%s\n",    m_szDBSCharSet[2]);
        INFO("character_set_results   :%s\n",    m_szDBSCharSet[3]);
        INFO("character_set_server    :%s \n",      m_szDBSCharSet[4]);
        INFO("character_set_system    :%s\n",     m_szDBSCharSet[5]);
        */

        return true;
    }
    return false;
}

#ifndef _WIN32
#define stricmp strcasecmp
#endif 
bool cMySqlConnection::IsCompatiableCharSet()
{
    //只有character_set_client, character_set_connection, character_set_results,
    //character_set_database,character_set_server,和m_szCharactSet完全一致

    if (0 != stricmp(m_szDBSCharSet[CHARACTER_SET_CLIENT] , m_szDBSCharSet[CHARACTER_SET_CONNECTION]))
    {
        return false;
    }

    if (0 != stricmp(m_szDBSCharSet[CHARACTER_SET_CLIENT] , m_szDBSCharSet[CHARACTER_SET_DATABASE]))
    {
        return false;
    }

    if (0 != stricmp(m_szDBSCharSet[CHARACTER_SET_CLIENT] , m_szDBSCharSet[CHARACTER_SET_RESULTS]))
    {
        return false;
    }

    if (0 != stricmp(m_szDBSCharSet[CHARACTER_SET_CLIENT] , m_szCharactSet))
    {
        return false;
    }
    return true;
}

bool cMySqlConnection::SetServerNames()
{
    //WARING:　sql 语句中变量的顺序一定和枚举定义一致
    char pszSql[1024] = {0};
#ifdef WIN32
    _snprintf(pszSql, 1024, "set names %s", m_szCharactSet);
#else
    snprintf(pszSql, 1024, "set names %s", m_szCharactSet);
#endif

    if (Execute(pszSql) < 0)
    {
        //CRITICAL("设置编码：Set names %s 失败\n", m_szCharactSet);
        return false;
    }
    return true;
}
        
bool cMySqlConnection::IsValidCharacterSet(const char* pszSetName)
{
    for (int i = 0; g_stpCharSets[i] != NULL ; i++)
    {
        if (0 == stricmp(pszSetName, g_stpCharSets[i]))
        {
            return true;
        }
    }
    //CRITICAL("DB 编码设置的编码 %s 不存在\n", pszSetName);
    return false;
}
