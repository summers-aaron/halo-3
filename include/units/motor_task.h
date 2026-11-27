#ifndef UNITS_MOTOR_TASK_H
#define UNITS_MOTOR_TASK_H

#include <stddef.h>

#include "units/motor_enums.h"

class c_motor_task {
public:
    e_motor_task task;
    e_action action;
    e_motor_program program;
    bool interrupted;
    union {
        struct {
            bool posture_exit_to_ground;      // posture task
        };
        long object_index;                    // infection task target
    };
    union {
        unsigned short duration;
        short ticks_remaining;                // signed infection countdown
    };
    // Bytes 0x16-0x17: genuine unnamed gap (March methods never touch +22; resources pinned at +24 by March set/clear).
    short pad_16;
    unsigned short resources;
    bool enabled;

    bool program_default();

    __forceinline void clear() {
        task = _motor_task_none;
        action = _action_none;
        program = _motor_program_none;
        resources = 0;
        interrupted = false;
    }

    __forceinline bool valid() const {
        return task != _motor_task_none && program != _motor_program_none && enabled;
    }

    __forceinline bool conflicts(short resource_mask) const {
        return (short)(resource_mask & resources) != 0;
    }

    __forceinline bool set(e_motor_task new_task, e_action new_action, e_motor_program new_program, short resource_mask) {
        task = new_task;
        action = new_action;
        program = new_program;
        enabled = true;
        resources = resource_mask;
        interrupted = false;
        if (!valid())
            enabled = false;
        return enabled;
    }
};

typedef char motor_task_size[(sizeof(c_motor_task) == 0x1c) ? 1 : -1];
typedef char motor_task_task_offset[(offsetof(c_motor_task, task) == 0x0) ? 1 : -1];
typedef char motor_task_action_offset[(offsetof(c_motor_task, action) == 0x4) ? 1 : -1];
typedef char motor_task_program_offset[(offsetof(c_motor_task, program) == 0x8) ? 1 : -1];
typedef char motor_task_interrupted_offset[(offsetof(c_motor_task, interrupted) == 0xc) ? 1 : -1];
typedef char motor_task_object_index_offset[(offsetof(c_motor_task, object_index) == 0x10) ? 1 : -1];
typedef char motor_task_posture_exit_offset[(offsetof(c_motor_task, posture_exit_to_ground) == 0x10) ? 1 : -1];
typedef char motor_task_duration_offset[(offsetof(c_motor_task, duration) == 0x14) ? 1 : -1];
typedef char motor_task_ticks_remaining_offset[(offsetof(c_motor_task, ticks_remaining) == 0x14) ? 1 : -1];
typedef char motor_task_resources_offset[(offsetof(c_motor_task, resources) == 0x18) ? 1 : -1];
typedef char motor_task_enabled_offset[(offsetof(c_motor_task, enabled) == 0x1a) ? 1 : -1];

#endif // UNITS_MOTOR_TASK_H
