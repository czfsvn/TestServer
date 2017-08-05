#include "TypeDef.h"
#include <iostream>
#include "DBModule.h"
#include "StrHelper.h"

class TestEvent : public IDBCommand
{
    public:
        TestEvent() : m_poRec(NULL), m_nSn(1), m_bResult(0){}
        ~TestEvent(){}

        virtual int32 GetDBSn(void) {   return m_nSn;   } 

        virtual void OnExecuteSql(IMySqlConn* poDBConnection)
        {
            std::string sql = "select * from mysql.user";
            int32 num = poDBConnection->ExecuteSqlRc(sql.c_str(), &m_poRec);
            if (num)
            {
                m_bResult = true;
            }
            else
            {
                m_bResult = false;
            }
        }

        virtual void OnExecute(void)
        {
            if (m_bResult == false)
            {
                if (NULL != m_poRec)
                    m_poRec->Release();
                return;
            }

            int32 cnt = m_poRec->GetRecordCount();
            for (int32 idx = 0; idx < cnt; idx ++)
            {
                if (m_poRec->GetRecord() == false)
                    break;

                std::cout<<m_poRec->GetFieldValue(0)<<std::endl;
                std::cout<<m_poRec->GetFieldValue(1)<<std::endl;
                std::cout<<m_poRec->GetFieldValue(2)<<std::endl;
                std::cout<<m_poRec->GetFieldValue(3)<<std::endl;
                std::cout<<m_poRec->GetFieldValue(4)<<std::endl;
                std::cout<<"============================\n"<<std::endl;
            }
            

        }

        virtual void Release(void)
        {
            
        }

    private:
        IMySqlRecordSet* m_poRec;;
        int32 m_nSn;
        bool m_bResult;
};

void Test_DBModule()
{
    DBConnInfo conInfo;
    conInfo.m_nSN = 1;
    StrSafeCopy(conInfo.m_stAccount.m_szHostName, "LOCALHOST"); 
    StrSafeCopy(conInfo.m_stAccount.m_szDBName, "mysql_cpp_data"); 
    StrSafeCopy(conInfo.m_stAccount.m_szLoginName, "root");
    StrSafeCopy(conInfo.m_stAccount.m_szLoginPwd, "123456");
    StrSafeCopy(conInfo.m_stAccount.m_szCharactSet, "utf8");
    conInfo.m_stAccount.m_wConnPort = 3306;

    DBGetModule(NULL)->Connect(&conInfo);
    cDBModule::Instance()->AddDBCommand(new TestEvent());
    while (1)
    {
        cDBModule::Instance()->Run(10);
    }
}

int32 main()
{
    std::cout<<"Hello, world!"<<std::endl;
    Test_DBModule();
    sleep(10000);
    return 0;
}
