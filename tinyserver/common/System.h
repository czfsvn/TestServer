#ifndef __SYSTEM_20170722_H__
#define __SYSTEM_20170722_H__

#include "TypeDef.h"
#include "Sync.h"
#include "Thread.h"

namespace CN
{
    IThreadCtrl* SYS_CreateThreadCtrl(IThread* poThread, bool bNeedWaitfor, 
            bool bSuspend = false);

    ILock* SYS_CreateThreadLock();

    void SYS_GetTimeEx(STimeVal* pstTime);

    void SYS_GetLocalTime(uint32 dwTime, SLocalTime* pstLocalTime);

    void SYS_Sleep(uint32 dwMilliseconds);

    void SYS_MakeDaemonProcess();

    bool SYS_CreateDirectory(const char* pszDir);
}



#endif
