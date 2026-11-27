#ifndef CSERIES_ASYNC_XOVERLAPPED_H
#define CSERIES_ASYNC_XOVERLAPPED_H

// Functions defined in tu/blamlibxbox/async_xoverlapped.cpp.

class c_overlapped_task;

bool overlapped_task_start_internal(c_overlapped_task *task, const char *file, long line);
void overlapped_task_block_until_finished(c_overlapped_task *task);
void overlapped_task_terminate(c_overlapped_task *task);
void overlapped_update();
extern "C" unsigned long overlapped_get_signin_info(unsigned long user_index, unsigned long flags, void *signin_info);

struct s_task_slot;

void overlapped_task_wait_for_all_tasks_to_finish();
extern "C" s_task_slot *find_task_slot(c_overlapped_task *task);
void task_block_until_finished(s_task_slot *task_slot);

void overlapped_initialize();
void overlapped_dispose();

#endif // CSERIES_ASYNC_XOVERLAPPED_H
