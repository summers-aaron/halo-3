#ifndef UNITS_UNIT_ESTIMATE_POSITION_H
#define UNITS_UNIT_ESTIMATE_POSITION_H

union real_point3d;
union real_vector3d;

void unit_estimate_position(long object_index, short ticks, const real_point3d *origin, const real_vector3d *vector1,
    const real_vector3d *vector2, const real_vector3d *vector3, real_point3d *result);

#endif // UNITS_UNIT_ESTIMATE_POSITION_H
