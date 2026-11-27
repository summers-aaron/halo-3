#ifndef CSERIES_ASYNC_H
#define CSERIES_ASYNC_H

#include "cseries/async_types.h"

// Functions defined in tu/blamlibxbox/async.cpp.

struct s_async_queue_element;
struct s_async_task;
class c_synchronized_long;

// Task submission, defined in gui_custom_bitmap_storage.cpp.
long async_task_add_ex(e_async_priority priority, s_async_task *task, e_async_category category,
    e_async_completion (*work_callback)(s_async_task *), c_synchronized_long *done, bool block);

void async_idle();
void async_initialize(void);
extern "C" void async_block_until_content_finished(void);
long async_task_add_ex(s_async_queue_element *async_queue_element);
unsigned long async_main(void *thread_parameter);
extern "C" bool async_test_content_catalogue_busy(void);
extern "C" void async_task_remove(s_async_queue_element *task);
void async_work_function(void);

class c_synchronized_long;

bool simple_yield_function(c_synchronized_long *completion_flag);
bool async_task_change_priority(long task_id, e_async_priority priority);

#endif // CSERIES_ASYNC_H
