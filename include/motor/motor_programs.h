#ifndef MOTOR_MOTOR_PROGRAMS_H
#define MOTOR_MOTOR_PROGRAMS_H

#include <stddef.h>

#include "units/motor_enums.h"

class c_motor_request;
class c_motor_task;

// Shared task table describing each motor task's callbacks.
// March's named task records have a 0x20-byte stride. The 0x0c slot is not read by these users.
struct s_motor_task_definition {
    e_motor_task task;
    const char *name;
    e_motor_result (*submit)(long object_index, const c_motor_request *request);
    unsigned long : 32;
    bool (*can_interrupt)(long object_index, c_motor_task *task);
    void (*animation_event)(long object_index, c_motor_task *task, long event);
    void (*interrupted)(long object_index, c_motor_task *task);
    bool (*finished)(long object_index, c_motor_task *task);
};

typedef char motor_task_definition_size[(sizeof(void *) != 4 || ((sizeof(s_motor_task_definition) == 0x20) )) ? 1 : -1];
typedef char motor_task_definition_finished_offset[(sizeof(void *) != 4 || ((offsetof(s_motor_task_definition, finished) == 0x1c) )) ? 1 : -1];

// Data defined in tu/objectslibxbox/motor_programs.cpp that other units use.

class c_motor_program_definition;

extern c_motor_program_definition *g_program_definition[];

// Functions defined in tu/objectslibxbox/motor_programs.cpp that other units call.

e_motor_program mover_default_program(long object_index);

#endif // MOTOR_MOTOR_PROGRAMS_H
