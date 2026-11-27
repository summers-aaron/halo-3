#ifndef CSERIES_ASYNC_COMPARE_H
#define CSERIES_ASYNC_COMPARE_H

#include "cseries/async_types.h"
struct s_async_task;
struct s_compare_tasks_data;
// match: 0x1ed990: __restrict is part of the decorated name (PIAU); without it callers reference PAU and miss the accepted definition
bool async_compare_tasks(s_compare_tasks_data *__restrict compare_data, e_async_completion (*callback_a)(s_async_task *),
    e_async_completion (*callback_b)(s_async_task *), e_async_priority priority_a, e_async_priority priority_b, const s_async_task *task_a,
    const s_async_task *task_b);

#endif // CSERIES_ASYNC_COMPARE_H
