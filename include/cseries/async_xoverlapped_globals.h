#ifndef CSERIES_ASYNC_XOVERLAPPED_GLOBALS_H
#define CSERIES_ASYNC_XOVERLAPPED_GLOBALS_H

#include <xtl.h>
#include "cseries/async_xoverlapped.h"

struct s_task_slot
{
    XOVERLAPPED overlapped;
    c_overlapped_task *task;
    unsigned long calling_result;
    bool terminated;
};

struct s_overlapped_globals
{
    s_task_slot slots[64];
    long task_start_time;
    unsigned long pending_poll_time;
    bool task_started;
    bool pending_poll;
    bool debug_rendering;
};

extern "C" s_overlapped_globals g_overlapped_globals;

#endif
