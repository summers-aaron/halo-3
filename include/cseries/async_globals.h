#ifndef CSERIES_ASYNC_GLOBALS_H
#define CSERIES_ASYNC_GLOBALS_H

#include "cseries/async_queue.h"

struct s_async_globals
{
    long task_counter;
    s_async_queue_element elements[25];
    s_async_queue_element *free_list;
    s_async_queue_element *work_list;
    s_async_queue_element *current_task;
    long tasks_added;
};

typedef char async_globals_work_list_offset[(sizeof(void *) != 4 || offsetof(s_async_globals, work_list) == 0x1e80) ? 1 : -1];
extern "C" s_async_globals g_async_globals;

struct s_async_task_buffer
{
    char bytes[32768];
    long offset;
    void *address;
    long size;
};
extern "C" s_async_task_buffer g_async_task_buffer;

#endif
