#include "Sync.h"
#include "Common.h"
#include <exception>

namespace CN
{
    ILock* SYS_CreateThreadLock()
    {
        return new cThreadLock;   
    }

#ifdef WIN32
    cThreadLock::cThreadLock(void)
    {
        InitializeCriticalSection(&m_CriticalSection);
    }

    cThreadLock::~cThreadLock(void)
    {
        DeleteCriticalSection(&m_CriticalSection);
    }

    void cThreadLock::Lock(void)
    {
        EnterCriticalSection(&m_CriticalSection);
    }

    void cThreadLock::UnLock(void)
    {
        LeaveCriticalSection(&m_CriticalSection);
    }

    void cThreadLock::Release(void)
    {
        SAFE_DELETE(this);
    }
#else
    cThreadLock::cThreadLock(void)
    {
        if (pthread_mutex_init(&m_mutex, NULL) != 0)
        {
            //throw std::exception();
        }
    }

    cThreadLock::~cThreadLock(void)
    {
        pthread_mutex_destroy(&m_mutex);
    }

    void cThreadLock::Lock(void)
    {
        pthread_mutex_lock(&m_mutex);
    }

    void cThreadLock::UnLock(void)
    {
        pthread_mutex_unlock(&m_mutex);
    }

    void cThreadLock::Release(void)
    {
        pthread_mutex_destroy(&m_mutex);
    }

#endif
}
