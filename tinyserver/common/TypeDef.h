#ifndef __TYPY_DEF_20170122_H__
#define __TYPY_DEF_20170122_H__

#ifdef WIN32
typedef __int64_t       int64;
typedef __int32_t       int32;
typedef __int16_t       int16;
typedef __int8_t        int8;
typedef __uint64_t      uint64;
typedef __uint32_t      uint32;
typedef __uint16_t      uint16;
typedef __uint8_t       uint8;
#else
typedef signed long     int64;
typedef signed int      int32;
typedef signed short    int16;
typedef signed char     int8;
typedef unsigned long   uint64;
typedef unsigned int    uint32;
typedef unsigned short  uint16;
typedef unsigned char   uint8;
#endif

#define INFINITE            0xFFFFFFFF  // Infinite timeout


namespace CN
{
    typedef struct 
    {
        uint32  m_dwSec;     ///< seconds
        uint32  m_dwUsec;    ///< microsecond(1/1,000,000 second)
    }STimeVal;

    typedef struct
    {
        int32 m_nSec;       ///< Seconds.     [0-60]
        int32 m_nMin;       ///< Minutes.     [0-59]
        int32 m_nHour;      ///< Hours.       [0-23]
        int32 m_nMday;      ///< Day.         [1-31]
        int32 m_nMon;       ///< Month.       [0-11]
        int32 m_nYear;      ///< Year
        int32 m_nWday;      ///< Day of week. [0-6]
    }SLocalTime;
}
#endif
