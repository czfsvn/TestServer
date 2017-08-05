#include "DBModule.h"
#include "System.h"

cDBModule* cDBModule::m_poInstance = NULL;

IDBModule* DBGetModule(const PVersion* pstVersion)
{
    if(false == cDBModule::CreateInstance())
        return NULL;
    cDBModule::Instance()->AddRef();

    if(false == cDBModule::Instance()->Init())
    {
        //CRITICAL("SDDBGetModule, Init cDBModule failed");
        cDBModule::DestroyInstance();
        return NULL;
    }

    return cDBModule::Instance();
}

cDBModule::cDBModule(void)
{
    m_dwRef     = 0;
    m_poLock    = NULL;
}

cDBModule::~cDBModule()
{

}

void cDBModule::AddRef(void)
{ 
    m_dwRef++;
}

uint32 cDBModule::QueryRef(void)
{  
    return m_dwRef;
}

void cDBModule::Release(void)
{   
    if(m_dwRef <= 1)
    {
        UnInit();
        cDBModule::DestroyInstance();
        //delete this;
        return;
    }
    m_dwRef--;
}

PVersion cDBModule::GetVersion(void)
{ 
    return CNDB_VERSION;
}

const char * cDBModule::GetModuleName(void)
{  
    return CNDB_MODULENAME;
}

bool cDBModule::Connect(DBConnInfo* pstDBConnInfo)
{  
    if(NULL == pstDBConnInfo)
    {
        return false;
    }

    pstDBConnInfo->m_stAccount.m_szCharactSet[DB_MAX_CHARACTSET_SIZE - 1] = '\0';

    cMySqlClient *poMySqlClient = _FindMySqlClient(pstDBConnInfo->m_nSN);
    if(poMySqlClient != NULL)
    {
        //WARN("cDBModule::Connect, db sn %d already exist", pstDBConnInfo->m_nSN);
        return false;
    }

    poMySqlClient = new cMySqlClient;
    if(NULL == poMySqlClient)
    {
        //CRITICAL("cDBModule::Connect, new cMySqlClient failed");
        return false;
    }

    if(false == poMySqlClient->ConnectDB(
                pstDBConnInfo->m_stAccount.m_szHostName, 
                pstDBConnInfo->m_stAccount.m_wConnPort, 
                pstDBConnInfo->m_stAccount.m_szDBName,
                pstDBConnInfo->m_stAccount.m_szLoginName,
                pstDBConnInfo->m_stAccount.m_szLoginPwd,
                pstDBConnInfo->m_stAccount.m_szCharactSet))
    {
        delete poMySqlClient;
        return false;
    }

    if(false == poMySqlClient->Start())
    {
        delete poMySqlClient;
        return false;
    }

    m_oDBSnClientMap[pstDBConnInfo->m_nSN] = poMySqlClient;

    return true;
}

void cDBModule::DisConnect(int32 nDBSn)
{
    cMySqlClient * poMySqlClient = _FindMySqlClient(nDBSn);
    if(NULL == poMySqlClient)
        return;

    poMySqlClient->Stop();
    m_oDBSnClientMap.erase(nDBSn);
    delete poMySqlClient;
}

uint32 cDBModule::EscapeString(int32 nDBSn, const char* pSrc, int32 nSrcSize, 
        char* pszDest, int32 nDstSize)
{    
    if(NULL == pSrc || NULL == pszDest)
        return 0;

    cMySqlClient * poMySqlClient = _FindMySqlClient(nDBSn);
    if(NULL == poMySqlClient)
        return 0;

    return poMySqlClient->EscapeString(pSrc, nSrcSize, pszDest, nDstSize);
}

bool cDBModule::AddDBCommand(IDBCommand *poDBCommand)
{  
    if(NULL == poDBCommand)
        return false;

    cMySqlClient* poMySqlClient = _FindMySqlClient(poDBCommand->GetDBSn());
    if(NULL == poMySqlClient)
        return false;

    poMySqlClient->AddRequest(poDBCommand);
    return true;
}

bool cDBModule::AddDBCommandFront(IDBCommand *poDBCommand)
{    
    if(NULL == poDBCommand)
        return false;

    cMySqlClient* poMySqlClient = _FindMySqlClient(poDBCommand->GetDBSn());
    if(NULL == poMySqlClient)
        return false;

    poMySqlClient->AddRequestFront(poDBCommand);
    return true;
}

bool cDBModule::Run(int32 nCount)
{   do
    {
        if ( !ExecResultCmd() )
            return false;
    } while(--nCount != 0);
        return true;
}

uint32 cDBModule::GetDBCommandNum(int nDBSn)
{
    uint32 dwDBCmdNum = 0;
    cDBClientMap::iterator it;
    if (-1 == nDBSn)
    {
        it = m_oDBSnClientMap.begin();
        while(it != m_oDBSnClientMap.end())
        {
            dwDBCmdNum += it->second->GetDBCommandNum();
            ++it;
        }
    }
    else
    {
        it = m_oDBSnClientMap.find(nDBSn);
        if (it == m_oDBSnClientMap.end())
        {
            return -1;
        }
        dwDBCmdNum = it->second->GetDBCommandNum();
    }
    return dwDBCmdNum;
}

bool cDBModule::Init()
{
    m_poLock = CN::SYS_CreateThreadLock();
    if(NULL == m_poLock)
    {
        //CRITICAL("cDBModule::Init, create lock failed");
        return false;
    }

    return true;
}

void cDBModule::UnInit()
{
    for(cDBClientMap::iterator it = m_oDBSnClientMap.begin(); it != m_oDBSnClientMap.end(); ++it)
    {
        cMySqlClient * poMySqlClient = it->second;
        ////assert(poMySqlClient != NULL);

        poMySqlClient->Stop();
        delete poMySqlClient;
    }
    m_oDBSnClientMap.clear();

    _ClearResultCmd();

    if(m_poLock != NULL)
    {
        m_poLock->Release();
        m_poLock = NULL;
    }
}

cMySqlClient * cDBModule::_FindMySqlClient(int32 nSN)
{
    cDBClientMap::iterator it = m_oDBSnClientMap.find(nSN);
    if(it == m_oDBSnClientMap.end())
        return NULL;

    return it->second;
}

void cDBModule::AddResponse(IDBCommand* poDBCommand)
{
    //assert(poDBCommand != NULL);

    m_poLock->Lock();
    m_oResultCmdList.push_back(poDBCommand);
    m_poLock->UnLock();
}

bool cDBModule::ExecResultCmd()
{
    IDBCommand *poDBCommand = NULL;

    m_poLock->Lock();
    if( !m_oResultCmdList.empty() )
    {
        poDBCommand = m_oResultCmdList.front();
        m_oResultCmdList.pop_front();
    }
    m_poLock->UnLock();

    if(NULL == poDBCommand)
        return false;

    poDBCommand->OnExecute();
    poDBCommand->Release();
    return true;
}

void cDBModule::_ClearResultCmd()
{
    while(ExecResultCmd()) {}
}

