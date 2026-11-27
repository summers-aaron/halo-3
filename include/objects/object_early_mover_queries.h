#ifndef OBJECTS_OBJECT_EARLY_MOVER_QUERIES_H
#define OBJECTS_OBJECT_EARLY_MOVER_QUERIES_H

union real_point3d;
union real_vector3d;

// These functions modify memory only through their pointer parameters.
// match: 0xf35b8: dropping either noalias reschedules the projectile_update prologue: stw velocity$ sinks below the flag tests, GPRs recolor, lfs pairs swap
__declspec(noalias) bool object_move_in_early_mover_local_space(long, union real_vector3d *, union real_vector3d *, bool);
// match: 0xf35b8: dropping either noalias reschedules the projectile_update prologue: stw velocity$ sinks below the flag tests, GPRs recolor, lfs pairs swap
__declspec(noalias) bool object_get_early_mover_local_space_velocity(
    long, union real_point3d const *, union real_vector3d *, union real_vector3d *, bool, bool);

#endif // OBJECTS_OBJECT_EARLY_MOVER_QUERIES_H
