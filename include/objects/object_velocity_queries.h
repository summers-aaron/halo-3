#ifndef OBJECTS_OBJECT_VELOCITY_QUERIES_H
#define OBJECTS_OBJECT_VELOCITY_QUERIES_H

union real_point3d;
union real_vector3d;

bool object_get_early_mover_local_space_velocity(long,const real_point3d *,real_vector3d *,real_vector3d *,bool,bool);
// match: 0x279418, 0x11c3e0: without noalias both frames grow 16B (0x279418 locals renumber, post-evaluate velocity/origin shift)
__declspec(noalias) void object_get_velocities(long,real_vector3d *,real_vector3d *);

#endif // OBJECTS_OBJECT_VELOCITY_QUERIES_H
