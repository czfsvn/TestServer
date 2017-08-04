#include "MySqlClient.h"
#include "MySqlConnection.h"
#include "MySqlRecordSet.h"
#include "DBModule.h"
#include "System.h"
#include <cstring>


cMySqlClient::cMySqlClient()
{
    m_bTerminate        = false;
    m_poMySqlConn       = NULL;
    m_bDbOK             = false;
    m_poLock            = NULL;
    m_poThrdCtrl        = NULL;
    m_nLastReConn       = 0;
}

cMySqlClient::~cMySqlClient()
{
    if(m_poLock != NULL)
        m_poLock->Release();

    if(m_poMySqlConn != NULL)
        delete m_poMySqlConn;
}

bool cMySqlClient::Start()
{
    m_poLock = CN::SYS_CreateThreadLock();
    if(NULL == m_poLock)
    {
        //CRITICAL("cMySqlClient::Start, create thread lock failed");
        return false;
    }

    m_poThrdCtrl = CN::SYS_CreateThreadCtrl(this, true);
    if(NULL == m_poThrdCtrl)
    {
        //CRITICAL("cMySqlClient::Start, create thread failed");
        return false;
    }

    return true;
}

void cMySqlClient::Stop()
{
    Terminate();
    if(m_poThrdCtrl != NULL)
    {
        m_poThrdCtrl->WaitFor(3000);
        m_poThrdCtrl->Release();
        m_poThrdCtrl = NULL;
    }

    if(m_poLock != NULL)
    {
        m_poLock->Release();
        m_poLock = NULL;
    }
}

bool cMySqlClient::ConnectDB(const char *pszHostName,int32 nPort, const char *pszDBName, 
        const char *pszUser, const char *pszPasswd, const char *pszCharactSet)
{
    if(NULL == pszHostName || NULL == pszDBName || NULL == pszUser || NULL == pszPasswd)
    {
        //assert(false);
        return false;
    }

    m_poMySqlConn = new cMySqlConnection;
    if(NULL == m_poMySqlConn)
    {
        //CRITICAL("cMySqlClient::ConnectDB, new CMySQLConnection failed");
        return false;
    }

    if(!m_poMySqlConn->Connect(pszHostName, nPort, pszDBName, pszUser, pszPasswd, pszCharactSet))                                                                              
    {
        //WARN("Connect to Database %s error: %d, %s", pszDBName, 
        //m_poMySqlConn->GetLastError(), m_poMySqlConn->GetLastErrorInfo());
        return  false;
    }

    m_bDbOK = true;

    return true;
}

bool cMySqlClient::AddRequest(IDBCommand *poDBCommand)
{
    //assert(poDBCommand != NULL);

    m_poLock->Lock();
    m_oDBCmdList.push_back(poDBCommand);
    m_poLock->UnLock();

    return true;
}

uint32 cMySqlClient::EscapeString(const char* pSrc, int32 nSrcSize, char* pszDest, int32 nDstSize)
{ 
    if(NULL == m_poMySqlConn)
    {
        return 0;
    }

    return m_poMySqlConn->EscapeStringEx(pSrc, nSrcSize, pszDest);
}

int32 cMySqlClient::ExecuteSql(const char* pszSQL)
{
    if (false == m_bDbOK)
    {
        return DB_DISCONNECT;
    }

    return m_poMySqlConn->Execute(pszSQL);
}

int32 cMySqlClient::ExecuteSqlRc(const char* pszSQL, IMySqlRecordSet **ppRc)
{ 
    //assert(ppRc != NULL);

    if (false == m_bDbOK)
    {
        *ppRc = NULL;
        return DB_DISCONNECT;
    }

    cMySqlRecordSet*poRS = new cMySqlRecordSet();
    *ppRc = poRS;

    int32 nRetCode = m_poMySqlConn->ExecuteEx( pszSQL, (int32)strlen(pszSQL), *poRS);

    if(DB_ERR_CONN == nRetCode)
    {
        m_bDbOK = false;
    }

    return nRetCode;
}

uint32 cMySqlClient::GetLastError(void)
{   
    return m_poMySqlConn->GetLastError();
}

const char * cMySqlClient::GetLastErrorInfo(void)
{ 
    return m_poMySqlConn->GetLastErrorInfo();
}

void cMySqlClient::ThrdProc()
{
    //IDBCommand *poDBCommand = NULL;

    while(!m_bTerminate)
    {
        if(m_bDbOK)
        {
            if ( !ExecuteDBCmd() )
                CN::SYS_Sleep(1);
        }
        else
        {
            //每30秒重联一次
            time_t nNow = time(NULL);
            if(nNow - m_nLastReConn > 10)
            {
                if(m_poMySqlConn->ReConnect())
                {
                    m_bDbOK = true;
                }

                m_nLastReConn = nNow;
            }
            else
            {
                _ClearReqQueue();
                CN::SYS_Sleep(1);
            }
        }
    }

    //退出前确保所有的请求都已执行完
    _ClearReqQueue();

    m_poMySqlConn->Close();
    m_bDbOK = false;
}

void cMySqlClient::Terminate()
{
    m_bTerminate = true;
}

void cMySqlClient::_ClearReqQueue()
{
    while(ExecuteDBCmd()) {}
}

bool cMySqlClient::ExecuteDBCmd( void )
{
    IDBCommand *poDBCommand = NULL;
    m_poLock->Lock();
    if( !m_oDBCmdList.empty() )
    {
        poDBCommand = m_oDBCmdList.front();
        m_oDBCmdList.pop_front();
    }
    m_poLock->UnLock();
    if(poDBCommand == NULL)
        return false;

    poDBCommand->OnExecuteSql(this);
    cDBModule::Instance()->AddResponse(poDBCommand);
    return true;
}

uint32 cMySqlClient::GetDBCommandNum()
{
    uint32 dwNum = 0;
    m_poLock->Lock();
    dwNum = (int)m_oDBCmdList.size();
    m_poLock->UnLock();

    return dwNum;
}

bool cMySqlClient::AddRequestFront(IDBCommand *poDBCommand)
{
    //assert(poDBCommand != NULL);

    m_poLock->Lock();
    m_oDBCmdList.push_front(poDBCommand);
    m_poLock->UnLock();

    return true;
}
