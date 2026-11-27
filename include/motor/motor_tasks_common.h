#ifndef MOTOR_MOTOR_TASKS_COMMON_H
#define MOTOR_MOTOR_TASKS_COMMON_H

#include "motor/motor_event_types.h"

// Functions defined in tu/objectslibxbox/motor_tasks_common.cpp.

class c_motor_request;

struct s_motor_task_definition;

// The ping task; biped_climb_submit runs it before re-standing the biped
extern s_motor_task_definition g_ping_task;

extern "C" int animated_task_submit(long object_index, long task, const c_motor_request *request);

class c_motor_task;

bool weapon_draw_event(long object_index, class c_motor_task *motor_task, enum e_motor_event motor_event);

#endif // MOTOR_MOTOR_TASKS_COMMON_H
