#ifndef CSERIES_ASYNC_QUEUE_H
#define CSERIES_ASYNC_QUEUE_H

#include "cseries/async_helpers.h"

// Queue storage owned by async.cpp, distinct from its 288-byte task payload.
// task_id combines the pool counter and slot index.
struct s_async_queue_element
{
    e_async_priority priority;
    long task_id;
    s_async_task task;
    long category;
    e_async_completion (*callback)(s_async_task *);
    c_synchronized_long *done;
    s_async_queue_element *next;
};

typedef char async_queue_element_size[(sizeof(void *) != 4 || ((sizeof(s_async_queue_element) == 0x138) )) ? 1 : -1];
typedef char async_queue_element_payload_offset[(offsetof(s_async_queue_element, task) == 8) ? 1 : -1];
typedef char async_queue_element_category_offset[(sizeof(void *) != 4 || ((offsetof(s_async_queue_element, category) == 0x128))) ? 1 : -1];
typedef char async_queue_element_next_offset[(sizeof(void *) != 4 || ((offsetof(s_async_queue_element, next) == 0x134) )) ? 1 : -1];

#endif // CSERIES_ASYNC_QUEUE_H
