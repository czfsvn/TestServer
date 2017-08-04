#ifndef __DB_RECORD_SET_20170722_H__
#define __DB_RECORD_SET_20170722_H__

#include "TypeDef.h"
#include "DB.h"
#include <mysql/mysql.h>

class cMySqlRecordSet : public IMySqlRecordSet
{
    friend class cMySqlConnection;
    public:
        cMySqlRecordSet();
        virtual ~cMySqlRecordSet();

        //  
        // 函数名:  GetRecordCount  
        // 功能:    获取记录条数    
        // 返回值:  记录条数    
        //  
        virtual uint32 GetRecordCount();

        //
        // 函数名:  GetFieldCount
        // 功能:    获取字段个数
        // 返回值:  字段个数
        //
        virtual uint32 GetFieldCount();

        //
        // 函数名:  GetRecord
        // 功能:    获取下一个记录行
        // 返回值:  如果返回值为TRUE表示获取成功，如果为false表示获取失败
        //
        virtual bool GetRecord();

        // 
        // 函数名:  GetFieldValue
        // 功能:    获取当前记录行中某一个字段的值
        // 参数:    [in] dwIndex:字段对应的索引，从0开始
        // 返回值:  对应索引的字段值
        //
        virtual const char* GetFieldValue(uint32 dwIndex);

        //  
        // 函数名:  GetFieldLength  
        // 功能:    获取字段缓冲区长度  
        // 参数:    [in] dwIndex:字段对应的索引，从0开始 
        // 返回值:  索引对应字段的缓冲区长度    
        //  
        virtual uint32 GetFieldLength(uint32 dwIndex);

        //   
        // 函数名:  Release 
        // 功能:    释放结果记录集  
        //  
        virtual void Release(); 

    protected:
        MYSQL_RES*      m_res;
        MYSQL_ROW       m_row; 
        uint64*         m_adwLengths;

};
#endif
