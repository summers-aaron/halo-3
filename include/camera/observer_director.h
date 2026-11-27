#ifndef CAMERA_OBSERVER_DIRECTOR_H
#define CAMERA_OBSERVER_DIRECTOR_H

// Functions defined in tu/blamlibxbox/observer_director.cpp.

extern "C" long observer_director_get_dead_camera_mode(long unit_index, long user_index);

class c_observer_director;

extern "C" bool observer_director_is_free_camera(const c_observer_director *director);

union real_point3d;
union real_vector3d;

extern "C" void observer_compute_object_frame(int object_index, real_point3d *position, real_vector3d *direction);

#endif // CAMERA_OBSERVER_DIRECTOR_H
