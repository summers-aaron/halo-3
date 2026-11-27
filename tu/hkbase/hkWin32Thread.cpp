// hkbase/hkWin32Thread.obj: from the records 0x60c550 0x60c560 0x60c5a8 0x60c630 0x60c688 0x60c6b0 0x60c6c0
// @flags /TP /O2 /Oc
// @address 0x60c550 ??0phys_thread@@QAA@XZ
// @address 0x60c560 ?stop_thread@phys_thread@@QAAXXZ
// @address 0x60c5a8 ?worker_start_thread@phys_thread@@QAA?AW4phys_result@@P6APAXPAX@Z0@Z
// @address 0x60c630 ?get_status@phys_thread@@QAA?AW4Status@1@XZ
// @address 0x60c688 ?get_current_thread_id@phys_thread@@SA_KXZ
// @address 0x60c6b0 ?get_child_thread_id@phys_thread@@QAA_KXZ
// @address 0x60c6c0 ??1phys_thread@@QAA@XZ
// @address 0x1b65a8 ?havok_next_thread_paramerters_get@@YAXJPAJPAK@Z
// @address 0x1b65a8 ?havok_notify_thread_started@@YAXPAXJK0@Z
// @address 0x1b65a8 ?havok_notify_thread_stopped@@YAXPAX@Z

#include <xtl.h>
#include "phys/phys_base.h"
#include <stddef.h>
#include "phys/phys_multithreading_util.h"
#include "phys/phys_thread.h"
#include "multithreading/threads.h"

phys_thread::phys_thread()
{
    m_worker_thread = 0;
}

void phys_thread::stop_thread()
{
    if (m_worker_thread != 0) {
        havok_notify_thread_stopped(m_worker_thread);
        CloseHandle(m_worker_thread);
        m_worker_thread = 0;
    }
}

phys_result phys_thread::worker_start_thread(void *(*function)(void *), void *argument)
{
    long thread_idx;
    unsigned long thread_stack_size;
    havok_next_thread_paramerters_get(g_next_phys_thread_index, &thread_idx, &thread_stack_size);
    unsigned long lock_thread_id;
    m_worker_thread = CreateThread(0, thread_stack_size, (LPTHREAD_START_ROUTINE)function,
        reinterpret_cast<void *>(static_cast<intptr_t>(thread_idx)), 0, &lock_thread_id);
    m_child_thread_id = lock_thread_id;
    if (m_worker_thread == 0) {
        return PHYS_FAILURE;
    } else {
        havok_notify_thread_started(m_worker_thread, thread_idx, lock_thread_id, argument);
        return PHYS_SUCCESS;
    }
}

phys_thread::Status phys_thread::get_status()
{
    if (m_worker_thread == 0)
        return THREAD_STATUS_NOT_STARTED;
    else {
        unsigned long thread_exit_code;
        GetExitCodeThread(m_worker_thread, &thread_exit_code);
        if (thread_exit_code == 259)
            return THREAD_STATUS_RUNNING;
        else
            return THREAD_STATUS_TERMINATED;
    }
}

unsigned __int64 phys_thread::get_current_thread_id()
{
    return (unsigned int)GetCurrentThreadId();
}

unsigned __int64 phys_thread::get_child_thread_id()
{
    return m_child_thread_id;
}

phys_thread::~phys_thread()
{
    stop_thread();
}
