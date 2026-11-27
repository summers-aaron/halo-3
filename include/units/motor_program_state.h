#ifndef UNITS_MOTOR_PROGRAM_STATE_H
#define UNITS_MOTOR_PROGRAM_STATE_H

#include <stddef.h>

#include "units/motor_enums.h"

class c_motor_program_definition;

class c_motor_program_state {
public:
    e_motor_program program;
    short state;

    void initialize() { program = _motor_program_none; state = 0; }
    bool set_program(long object_index, e_motor_program new_program);
    c_motor_program_definition *definition_get();
};

typedef char motor_prog_state_size[(sizeof(c_motor_program_state) == 8) ? 1 : -1];
typedef char motor_prog_state_state_offset[(offsetof(c_motor_program_state, state) == 4) ? 1 : -1];

#endif // UNITS_MOTOR_PROGRAM_STATE_H
