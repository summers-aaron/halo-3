#ifndef CAMERA_STATIC_CAMERA_H
#define CAMERA_STATIC_CAMERA_H

#include "camera/camera_types.h"
#include "game/director_perspective_types.h"

// Functions and types defined in tu/blamlibxbox/static_camera.cpp.

class c_static_camera : public c_camera
{
public:
    // Restores the vtable of a camera whose fields were saved in place.
    c_static_camera() {}
    c_static_camera(long user_index);
    virtual e_camera_mode get_type() const { return _camera_mode_static; }
    virtual e_director_perspective get_perspective() const { return _director_perspective_authored; }
    virtual void update(long user_index, float elapsed_time, s_observer_command *command);
    virtual void set_position(const real_point3d *position);
    virtual void set_forward(const real_vector3d *forward);

private:
    real_point3d m_position;
    real_vector3d m_forward;
    real_vector3d m_up;
    float m_field_of_view;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char static_camera_size[(sizeof(void *) != 4 || (sizeof(c_static_camera) == 56)) ? 1 : -1];
#endif

#endif // CAMERA_STATIC_CAMERA_H
