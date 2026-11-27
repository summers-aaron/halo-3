#ifndef CAMERA_FIRST_PERSON_CAMERA_H
#define CAMERA_FIRST_PERSON_CAMERA_H

// Functions defined in tu/blamlibxbox/first_person_camera.cpp that other units call.

union real_point3d;
union real_vector3d;
struct s_observer_command;

void first_person_camera_deterministic(long unit_index, real_point3d *position, real_vector3d *forward);
void first_person_camera_for_unit_and_vector(long unit_index, const real_vector3d *forward, s_observer_command *result);

extern "C" {
extern const float k_first_person_zoom_distance_min;
extern const float k_first_person_zoom_distance_max;
extern float g_first_person_camera_fov_min;
extern float g_first_person_camera_fov_max;
}

#endif // CAMERA_FIRST_PERSON_CAMERA_H
