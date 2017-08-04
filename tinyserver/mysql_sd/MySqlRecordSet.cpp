#include "MySqlRecordSet.h"
#include <iostream>

cMySqlRecordSet::cMySqlRecordSet() : m_res(NULL),
    m_row(NULL), m_adwLengths(NULL)
{
}

cMySqlRecordSet::~cMySqlRecordSet()
{
    if (m_res != NULL)
    {
        mysql_free_result(m_res);
        m_res = NULL;
    }
}

uint32 cMySqlRecordSet::GetRecordCount()
{   
    return ( (m_res != NULL) ? (uint32)mysql_num_rows(m_res) : 0);
}

uint32 cMySqlRecordSet::GetFieldCount()
{   
    return ( (m_res != NULL) ? (uint32)mysql_num_fields(m_res) : 0);
}

bool cMySqlRecordSet::GetRecord()
{  
    if (NULL == m_res)
        return false;

    m_row = mysql_fetch_row(m_res);
    if(m_row != NULL)
    {
        m_adwLengths = mysql_fetch_lengths(m_res);
    }

    return (m_row != NULL);
}

const char* cMySqlRecordSet::GetFieldValue(uint32 dwIndex)
{ 
    if (NULL == m_row)
        return NULL;
    if (dwIndex >= GetFieldCount())
        return NULL;
    return m_row[dwIndex];
}

uint32 cMySqlRecordSet::GetFieldLength(uint32 dwIndex)
{  
    if (NULL == m_row)
        return 0;
    if (dwIndex >= GetFieldCount())
        return 0;
    return m_adwLengths[dwIndex];
}

void cMySqlRecordSet::Release()
{  
    if (m_res != NULL)
        mysql_free_result(m_res);

    m_res = NULL;
    m_row = NULL;

    delete this;
}
