#ifndef __THREAD_20170722_H__
#define __THREAD_20170722_H__

#include "TypeDef.h"
#ifndef WIN32
#include <pthread.h>
#endif

namespace CN
{
    class IThread
    {
        public:
            virtual ~IThread()  {}

            virtual void ThrdProc() = 0;

            virtual void Terminate() = 0;

    };

    class IThreadCtrl
    {
        public:
            virtual ~IThreadCtrl()   {}

            virtual void Resume(void)   =   0;

            virtual void Suspend()      =   0;

            virtual void Terminate(void)=   0;

            virtual bool Kill(uint32  exitCode) =   0;

            virtual bool WaitFor(uint32 waitTime = INFINITE)    =   0;

            virtual uint32  GetThreadId(void)   =   0;

            virtual IThread* GetThread(void)    =   0;

            virtual void Release(void)      =   0;
    };

    class cThreadCtrl : public IThreadCtrl
    {
        public:
            cThreadCtrl(IThread *pThread, bool bNeedWaitfor);
            virtual ~cThreadCtrl();

        public:
            virtual void Resume(void);
            virtual void Suspend();
            virtual void Terminate(void);              /*Force to terminate the thread*/
            virtual bool Kill(uint32 dwExitCode);
            virtual bool WaitFor(uint32 dwWaitTime = INFINITE);
            virtual void Release(void)          { delete this;          }
            virtual uint32 GetThreadId(void)    { return m_dwThreadId;  }
            virtual IThread* GetThread(void)  { return m_pThread;     }

            int Start(bool bSuspend);

        private:
#ifdef WIN32
            static uint32   StaticThreadFunc(void *arg);
#else
            static void*    StaticThreadFunc(void* argv);
#endif

        protected:
            bool        m_bIsStop;
            bool        m_bNeedWaitfor;
            uint32      m_dwThreadId;
            IThread*  m_pThread;
#ifdef WIN32
            HANDLE      m_handle;           
#else
            pthread_t   m_threadId;
#endif
    };
}

#endif
