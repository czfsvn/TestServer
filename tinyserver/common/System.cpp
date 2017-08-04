#include "System.h"
#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace CN
{
    void SYS_GetTimeEx(STimeVal* pstTime){
#ifdef WIN32
        FILETIME        fileTime;
        LARGE_INTEGER   li;
        int64           t;

        GetSystemTimeAsFileTime(&fileTime);
        li.LowPart  = fileTime.dwLowDateTime;
        li.HighPart = fileTime.dwHighDateTime;
        t  = li.QuadPart;       /* In 100-nanosecond intervals */
        t -= EPOCHFILETIME;     /* Offset to the Epoch time */
        t /= 10;                /* In microseconds */
        pstTime->m_dwSec = (long)(t / 1000000);
        pstTime->m_dwUsec = (long)(t % 1000000);
#else

#endif
    }

    void SYS_GetLocalTime(uint32 dwTime,SLocalTime* pstLocalTime)
    {
#ifdef WIN32
        if (dwTime == 0)
        {
            SYSTEMTIME  stTime;
            GetLocalTime(&stTime);

            pstLocalTime->m_nYear   = stTime.wYear;
            pstLocalTime->m_nMon    = stTime.wMonth;
            pstLocalTime->m_nMday   = stTime.wDay;
            pstLocalTime->m_nHour   = stTime.wHour;
            pstLocalTime->m_nMin    = stTime.wMinute;
            pstLocalTime->m_nSec    = stTime.wSecond;
            pstLocalTime->m_nWday   = stTime.wDayOfWeek;
        }
        else
        {
            struct tm* pstTime = localtime((time_t*)&dwTime);

            pstLocalTime->m_nSec    = pstTime->tm_sec;      
            pstLocalTime->m_nMin   = pstTime->tm_min;      
            pstLocalTime->m_nHour   = pstTime->tm_hour;     
            pstLocalTime->m_nMday   = pstTime->tm_mday;     
            pstLocalTime->m_nMon    = pstTime->tm_mon + 1;      
            pstLocalTime->m_nYear   = pstTime->tm_year + 1900;      
            pstLocalTime->m_nWday   = pstTime->tm_wday; 
        }
#else
#endif
    }

    void SYS_Sleep(uint32 dwMilliseconds)
    {
#ifdef WIN32
        Sleep(dwMilliseconds);
#else
        usleep(dwMilliseconds * 1000);
#endif
    }
}
