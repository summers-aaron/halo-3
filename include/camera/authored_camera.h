#ifndef CAMERA_AUTHORED_CAMERA_H
#define CAMERA_AUTHORED_CAMERA_H

#include <stddef.h>
#include "camera/first_person_camera_types.h"

// Functions and types defined in tu/blamlibxbox/authored_camera.cpp.

class c_authored_camera : public c_camera
{
public:
    c_authored_camera();
    c_authored_camera(long user_index);

    virtual e_camera_mode get_type() const { return _camera_mode_authored; }
    virtual e_director_perspective get_perspective() const;
    virtual void update(long user_index, float elapsed_time, s_observer_command *command);
    virtual long get_target() const;
    virtual void set_target(long) {}
    virtual void handle_deleted_object(long object_index);

    bool m_sub_camera_active;
    bool m_sub_camera_following;
    c_first_person_camera m_camera;
    real_point3d m_position;
    real_vector3d m_forward;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char authored_camera_sub_camera_offset[(sizeof(void *) != 4 || ((offsetof(c_authored_camera, m_camera) == 0x14))) ? 1 : -1];
typedef char authored_camera_position_offset[(sizeof(void *) != 4 || ((offsetof(c_authored_camera, m_position) == 0x34))) ? 1 : -1];
typedef char authored_camera_size[(sizeof(void *) != 4 || (sizeof(c_authored_camera) == 76)) ? 1 : -1];
#endif

extern "C" void authored_camera_clear_target(c_authored_camera *camera, long object_index);

inline void c_authored_camera::handle_deleted_object(long object_index)
{
    authored_camera_clear_target(this, object_index);
}

#endif
