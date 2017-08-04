#ifndef __DB_MODULE_20170722_H__
#define __DB_MODULE_20170722_H__

#include <map>
#include "TypeDef.h" 
#include "Singleton.h" 
#include "Sync.h" 
#include "DB.h" 
#include "MySqlClient.h" 

using namespace CN;
typedef std::map<int32, cMySqlClient*>  cDBClientMap;

class cDBModule : public IDBModule
{
    cDBModule(void);
    virtual ~cDBModule();

    DECLARE_SINGLETON(cDBModule);
public:
    // Interface ISDBase
    virtual void AddRef(void);    
    virtual uint32  QueryRef(void);   
    virtual void Release(void);  
    virtual PVersion GetVersion(void);    
    virtual const char * GetModuleName(void);

    //  
    // 函数名:  Connect 
    // 功能:    添加DB连接  
    // 参数:    [in] pstDBConnInfo: 需要添加的DB连接信息，参见SDBConnInfo 
    // 返回值:  添加连接成功返回true，添加连接失败返回false，失败的原因可能是   
    //          无法连接DB，或者是DB编号为SDBConnInfo的DB连接之前已经被添加了 
    //  
    virtual bool Connect(DBConnInfo* pstDBConnInfo);

    //  
    // 函数名:  Disconnect  
    // 功能:    删除DB连接  
    // 参数:    [in] 需要删除的DB连接信息   
    //  
    virtual void DisConnect(int32 nDBSn);

    //  
    // 函数名:  EscapeString    
    // 功能:    将二进制数据转义成MYSQL查询语句中使用的字符串   
    // 参数:    [in] nDBSn: 在那个DB连接上进行转义    
    //          [in] pSrc: 需要进行转义的数据   
    //          [in] nSrcSize 需要转义的数据的长度，即pSrc所指数据的长度 
    //          [in] pszDest 存放转义后的字符串的缓冲区 
    //         [in] nDstSize pszDest's max-size    
    // 返回值:  转义成功后返回值大于0，转义失败返回值等于0  
    //  
    virtual uint32 EscapeString(int32 nDBSn, const char* pcSrc, 
            int32 nSrcSize, char* pszDest, int32 nDstSize);

    //  
    // 函数名:  AddDBCommand    
    // 功能:    添加DB事件处理请求  
    // 返回值:  添加成功返回true，添加失败返回false，失败的原因可能是 
    //          IDBCommand::GetDBSn所返回的DB编号所对应的DB连接不存在   
    //  
    virtual bool AddDBCommand(IDBCommand *poDBCommand);

    //    
    // 函数名:  AddDBCommandFront    
    // 功能:    添加DB事件处理请求到队列头部    
    // 返回值:  添加成功返回true，添加失败返回false，失败的原因可能是    
    //           IDBCommand::GetDBSn所返回的DB编号所对应的DB连接不存在    
    //    
    virtual bool AddDBCommandFront(IDBCommand *poDBCommand);

    //  
    // 函数名:  Run 
    // 功能:    处理DB扩展事件，该函数需要在主循环中不断的被调用    
    // 参数:    [in] 每次处理DB事件数量的上限  
    // 返回值:  如果有DB事件被处理返回true，否则返回false   
    //  
    virtual bool Run(int32 nCount);

    //
    // 函数名:  GetDBEventNum
    // 功能:    得到某nDBSn对应的事件数量,默认参数为得到所有DBSn的事件数量
    // 返回值:  返回事件数量值,如果未找到nDBSn,返回-1
    virtual uint32 GetDBCommandNum(int nDBSn = -1);

    bool Init();

private:
    void UnInit();

    void AddResponse(IDBCommand* poDBCommand);
    friend class cMySqlClient;
    private:
    cMySqlClient * _FindMySqlClient(int32 nSN);
    bool ExecResultCmd();
    void _ClearResultCmd();

private:
    uint32              m_dwRef;
    CN::ILock*              m_poLock;
    CDBCmdList          m_oResultCmdList;
    cDBClientMap        m_oDBSnClientMap;

};
#endif
