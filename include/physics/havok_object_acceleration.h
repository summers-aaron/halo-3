#ifndef PHYSICS_HAVOK_OBJECT_ACCELERATION_H
#define PHYSICS_HAVOK_OBJECT_ACCELERATION_H

union real_point3d;
union real_vector3d;

// This function only modifies memory reachable through its pointer parameters.
// match: 0x318d78: keeps applied_object_index in r30 across the detach/set-velocities calls instead of reloading the stack slot.
__declspec(noalias) bool havok_object_apply_acceleration(long object_index, long unknown, const real_point3d *position,
    const real_vector3d *linear_acceleration, const real_vector3d *angular_acceleration, long *applied_object_index,
    real_vector3d *linear_velocity, real_vector3d *angular_velocity);

#endif // PHYSICS_HAVOK_OBJECT_ACCELERATION_H
