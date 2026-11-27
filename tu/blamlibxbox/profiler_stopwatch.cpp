// blamlibxbox/profiler_stopwatch.obj: from the records 0xede70 0x219f60 0x219fe8
// @flags /TP /O2 /J
#include "cseries/crt_xdk.h"
#include "profiler/profiler_stopwatch.h"

__forceinline __int64 c_stop_watch::get_absolute_time()
{
    __int64 counter;
    if (QueryPerformanceCounter((_LARGE_INTEGER *)&counter))
        return counter;
    return GetTickCount();
}

// 0xede70
// @address 0xede70 ??0c_stop_watch@@QAA@XZ
c_stop_watch::c_stop_watch()
{
    m_accumulated = 0;
    m_start_time = get_absolute_time();
    m_paused = false;
}

// @address 0x219f60 ?start@c_stop_watch@@QAA_JXZ
__int64 c_stop_watch::start()
{
    __int64 total_elapsed = m_accumulated;
    if (!m_paused)
        total_elapsed = get_current_delta() + total_elapsed;
    if (m_paused)
    {
        m_paused = false;
        m_start_time = get_absolute_time();
    }
    return total_elapsed;
}

// @address 0x219fe8 ?get_current_delta@c_stop_watch@@ABA_JXZ
__int64 c_stop_watch::get_current_delta() const
{
    __int64 elapsed = get_absolute_time() - m_start_time;
    if (elapsed < 0)
        elapsed = 0;
    return elapsed;
}
