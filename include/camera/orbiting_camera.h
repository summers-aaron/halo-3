#ifndef CAMERA_ORBITING_CAMERA_H
#define CAMERA_ORBITING_CAMERA_H

#include "camera/camera_types.h"
#include "camera/camera_globals.h"
#include "game/director_perspective_types.h"

// Functions and types defined in tu/blamlibxbox/orbiting_camera.cpp.

class c_orbiting_camera : public c_camera
{
public:
    c_orbiting_camera() {}
    c_orbiting_camera(long target)
    {
        m_distance = g_camera_globals.orbiting_camera_minimum_distance;
        m_height = g_camera_globals.orbiting_camera_height;
        m_orientation_enabled = true;
        m_movement_enabled = true;
        m_angles.yaw = 0.0f;
        m_distance_overridden = false;
        m_angles.pitch = 0.0f;
        c_orbiting_camera::set_target(target);
    }

    virtual e_camera_mode get_type() const { return _camera_mode_orbiting; }
    virtual e_director_perspective get_perspective() const { return _director_perspective_third_person; }
    virtual void update(long user_index, float elapsed_time, s_observer_command *command);
    virtual void set_target(long target);
    virtual void set_forward(const real_vector3d *forward);
    virtual void enable_orientation(bool enable);
    virtual void enable_movement(bool enable);

    // March: observer_director.obj inline; the next update starts a transition to the new distance
    void set_distance(float distance)
    {
        m_distance_overridden = true;
        m_distance = distance;
    }

private:
    friend class c_director;
    real_euler_angles2d m_angles;
    float m_distance;
    float m_height;
    bool m_orientation_enabled;
    bool m_movement_enabled;
    bool m_distance_overridden;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char orbiting_camera_size[(sizeof(void *) != 4 || (sizeof(c_orbiting_camera) == 36)) ? 1 : -1];
#endif

void make_valid_real_euler_angles2d(union real_euler_angles2d * angles);

#endif // CAMERA_ORBITING_CAMERA_H
