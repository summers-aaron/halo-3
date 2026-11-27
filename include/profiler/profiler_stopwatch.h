#ifndef PROFILER_PROFILER_STOPWATCH_H
#define PROFILER_PROFILER_STOPWATCH_H

#include <stddef.h>

// March profiler_stopwatch.cpp: accumulated cycles at +0, last start at +8,
// and the pause state at +0x10. get_current_delta is private and const.
class c_stop_watch
{
public:
    c_stop_watch();
    ~c_stop_watch() {}
    __int64 start();

private:
    __int64 get_current_delta() const;
    static __forceinline __int64 get_absolute_time();

    __int64 m_accumulated;
    __int64 m_start_time;
    bool m_paused;
};

typedef char stop_watch_size[(sizeof(c_stop_watch) == 0x18) ? 1 : -1];

#endif // PROFILER_PROFILER_STOPWATCH_H
