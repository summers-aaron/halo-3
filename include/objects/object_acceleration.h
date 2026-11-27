#ifndef OBJECTS_OBJECT_ACCELERATION_H
#define OBJECTS_OBJECT_ACCELERATION_H

union real_point3d;
union real_vector3d;

void object_apply_acceleration(long object_index, long unknown, const real_point3d *position,
    const real_vector3d *linear_acceleration, const real_vector3d *angular_acceleration);

#endif
