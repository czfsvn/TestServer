#ifndef __DB_BASE_20170722_H__
#define __DB_BASE_20170722_H__

#include "TypeDef.h"

struct PVersion
{    
    uint16 wMajorVersion;   
    uint16 wMinorVersion;   
    uint16 wCompatibleVersion;  
    uint16 wBuildNumber;
};

const PVersion P_VERSION = { 3, 0, 1, 2};

class IDBBase
{
    public:
        virtual ~IDBBase(void)  {}

        virtual void AddRef(void)   =   0;

        virtual uint32 QueryRef(void) = 0;

        virtual void Release(void)  =   0;

        virtual const char* GetModuleName(void) =   0; 
};

#endif 
