#ifndef UNITS_UNIT_FACING_H
#define UNITS_UNIT_FACING_H

#include "math/real_math_types.h"

// The two directions stored by unit_set_facing.
extern "C" void unit_set_facing(long unit_index, const real_vector3d *forward, const real_vector3d *up);

#endif // UNITS_UNIT_FACING_H
