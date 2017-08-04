#include "Thread.h"

namespace CN
{
    void SYS_MakeDaemonProcess()
    {
        //Do nothing, Daemon是Linux特有的特性
    }


    IThreadCtrl* SYS_CreateThreadCtrl(IThread* poThread, bool bNeedWaitfor, 
            bool bSuspend = false)
    {    
        cThreadCtrl *pThreadCtrl = new cThreadCtrl(poThread, bNeedWaitfor);    
        pThreadCtrl->Start(bSuspend);            
        return pThreadCtrl;
    }

    uint32  cThreadCtrl::StaticThreadFunc(void *arg)
    {
        cThreadCtrl *pThreadCtrl = (cThreadCtrl *)arg;
        pThreadCtrl->m_bIsStop = false;
        pThreadCtrl->m_pThread->ThrdProc();

        //线程结束的时候进行判断
#ifdef WIN32
        if (!pThreadCtrl->m_bNeedWaitfor)
        {
            CloseHandle(pThreadCtrl->m_handle);
            pThreadCtrl->m_handle = INVALID_HANDLE_VALUE;
            pThreadCtrl->m_bIsStop = true;
        }
#else
#endif
        return 0;
    }

    cThreadCtrl::cThreadCtrl(IThread *pThread, bool bNeedWaitfor)
    {
        m_bIsStop       = false;
        m_bNeedWaitfor  = bNeedWaitfor;
        m_pThread       = pThread;
#ifdef WIN32
        m_handle        = INVALID_HANDLE_VALUE;
#else
#endif
    }

    cThreadCtrl::~cThreadCtrl()
    {
#ifdef WIN32
        if (m_handle != INVALID_HANDLE_VALUE)
        {
            uint32 dwErrCode = 0;
            Kill(dwErrCode);
        }
#else

#endif
    }

    void cThreadCtrl::Resume(void)
    {
#ifdef WIN32
        ResumeThread(m_handle);
#else

#endif
    }

    void cThreadCtrl::Suspend()
    {
#ifdef WIN32
        SuspendThread(m_handle);
#else

#endif
    }

    int cThreadCtrl::Start(bool bSuspend)
    {
#ifdef WIN32
        if (bSuspend)
        {
            m_handle= (HANDLE)_beginthreadex(0, 0, StaticThreadFunc, this, CREATE_SUSPENDED, &m_dwThreadId);             
        }
        else
        {
            m_handle= (HANDLE)_beginthreadex(0, 0, StaticThreadFunc, this, 0, &m_dwThreadId);
        }
#else

#endif

        return 0;
    }

    void cThreadCtrl::Terminate(void)
    {
        m_pThread->Terminate();
    }

    bool cThreadCtrl::WaitFor(uint32 dwWaitTime)
    {
#ifdef WIN32
        if (!m_bNeedWaitfor || INVALID_HANDLE_VALUE == m_handle)
        {
            return false;
        }

        DWORD dwRet = WaitForSingleObject(m_handle, dwWaitTime);
        CloseHandle(m_handle);
        m_handle = INVALID_HANDLE_VALUE;
        m_bIsStop = true;

        if(WAIT_OBJECT_0 == dwRet)
            return true;

#else
#endif
        return false;
    }

    bool cThreadCtrl::Kill(uint32 dwExitCode)
    {
#ifdef WIN32
        if (m_handle == INVALID_HANDLE_VALUE)
        {
            return false;
        }

        if (TerminateThread(m_handle, dwExitCode))
        {
            CloseHandle(m_handle);
            m_handle = INVALID_HANDLE_VALUE;
            return TRUE;
        }
#else
#endif
        return false;
    }

}
