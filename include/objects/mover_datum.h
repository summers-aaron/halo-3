#ifndef OBJECTS_MOVER_DATUM_H
#define OBJECTS_MOVER_DATUM_H

#include <stddef.h>
#include "objects/object_datum.h"

// mover_new allocates the three blocks through references at 0x160, 0x164 and 0x168.
struct mover_datum
{
    object_datum object;
    s_object_block_reference motor_tasks;
    s_object_block_reference motor_program;
    s_object_block_reference action;
};

typedef char mover_datum_size[(sizeof(mover_datum) == 0x16c) ? 1 : -1];
typedef char mover_datum_tasks_offset[(offsetof(mover_datum, motor_tasks) == 0x160) ? 1 : -1];
typedef char mover_datum_program_offset[(offsetof(mover_datum, motor_program) == 0x164) ? 1 : -1];
typedef char mover_datum_action_offset[(offsetof(mover_datum, action) == 0x168) ? 1 : -1];

#endif // OBJECTS_MOVER_DATUM_H
