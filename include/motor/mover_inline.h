#ifndef MOTOR_MOVER_INLINE_H
#define MOTOR_MOVER_INLINE_H

#include "units/unit_types.h"
#include "objects/object_header.h"
#include "memory/tls_globals.h"

static __forceinline long mover_get_motor_program(long unit_index)
{
    unit_datum *unit = (unit_datum *)((s_object_header *)object_header_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(unit_index)].datum;
    return *(long *)((char *)unit + unit->unit.motor_program_offset);
}

static __forceinline bool mover_has_variable_camera_up(long unit_index)
{
    bool result = false;
    long program = mover_get_motor_program(unit_index);
    if (program == 7 || program == 8)
        result = true;
    return result;
}

#endif // MOTOR_MOVER_INLINE_H
