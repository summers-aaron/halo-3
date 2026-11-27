#ifndef CAMERA_DEAD_CAMERA_H
#define CAMERA_DEAD_CAMERA_H

#include "camera/camera_types.h"
#include "game/director_perspective_types.h"

// Functions and types defined in tu/blamlibxbox/dead_camera.cpp.

class c_dead_camera : public c_camera
{
public:
    // Restores the vtable of a camera whose fields were saved in place.
    c_dead_camera() {}
    c_dead_camera(long user_index);
    virtual e_camera_mode get_type() const { return _camera_mode_dead; }
    virtual e_director_perspective get_perspective() const { return _director_perspective_third_person; }
    virtual void update(long user_index, float elapsed_time, s_observer_command *command);
    virtual void set_target(long target);
    virtual void handle_deleted_player(long player_index);
    virtual void handle_deleted_object(long object_index);

    real_point3d m_position;            // the body's last camera position once it fell to its death
    real_euler_angles2d m_angles;
    float m_focus_distance;
    float m_field_of_view;
    float m_time;                       // seconds since the camera took over
    long m_player_index;
    long m_killer_object_index;         // the object the camera turns to, from the unit's damage records
    bool m_falling_to_death;
    unsigned char m_user_index;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char dead_camera_size[(sizeof(void *) != 4 || (sizeof(c_dead_camera) == 60)) ? 1 : -1];
#endif

// Pitch constants of c_dead_camera::set_target.
extern "C" const float k_dead_camera_target_pitch;
extern "C" const float k_dead_camera_random_pitch_scale;
extern "C" const float k_dead_camera_random_pitch_offset;

#endif // CAMERA_DEAD_CAMERA_H
