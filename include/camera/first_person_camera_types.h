#ifndef CAMERA_FIRST_PERSON_CAMERA_TYPES_H
#define CAMERA_FIRST_PERSON_CAMERA_TYPES_H

#include "camera/camera_types.h"
#include "game/director_perspective_types.h"

class c_first_person_camera : public c_camera
{
public:
    // Restores the vtable of a camera whose fields were saved in place.
    c_first_person_camera() {}
    c_first_person_camera(long target)
    {
        m_orientation_enabled = false;
        m_target = target;
        m_field_of_view = 0.0f;
        m_facing.yaw = 0.0f;
        m_facing.pitch = 0.0f;
    }

    virtual e_camera_mode get_type() const { return _camera_mode_first_person; }
    virtual e_director_perspective get_perspective() const { return _director_perspective_first_person; }
    virtual void update(long user_index, float elapsed_time, s_observer_command *command);
    float m_field_of_view;
    real_euler_angles2d m_facing;
    bool m_orientation_enabled;
};

typedef char first_person_camera_size[(sizeof(void *) != 4 || sizeof(c_first_person_camera) == 0x20) ? 1 : -1];

#endif // CAMERA_FIRST_PERSON_CAMERA_TYPES_H
