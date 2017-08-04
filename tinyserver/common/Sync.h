#ifndef __SYNC_20170722_H__
#define __SYNC_20170722_H__

#ifdef WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

namespace CN
{
    class ILock
    {
        public:
            virtual ~ILock()    {}

            virtual void Lock() =   0;

            virtual void UnLock()   =   0;

            virtual void Release()  =   0;
    };

    class cThreadLock : public ILock
    {
        public:            
            cThreadLock(void);
            virtual ~cThreadLock(void);

            virtual void Lock(void);
            virtual void UnLock(void); 
            virtual void Release(void);

        private:
#ifdef WIN32
            CRITICAL_SECTION m_CriticalSection;
#else
            pthread_mutex_t  m_mutex;
#endif
    };
}
#endif
