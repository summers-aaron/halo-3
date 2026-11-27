#ifndef CAMERA_CAMERA_TYPES_H
#define CAMERA_CAMERA_TYPES_H

#include <stddef.h>
#include "cseries/cseries_wrapped.h"
#include "math/real_math_types.h"
#include "math/matrix_math_types.h"
#include "camera/camera_mode_types.h"
#include "camera/observer_depth_of_field_types.h"

enum e_director_perspective;

#include "camera/observer_command.h"

class __declspec(novtable) c_camera
{
public:
    c_camera()
    {
        m_camera_flags.clear();
        m_target = -1;
        m_camera_state = 0;
    }

    virtual e_camera_mode get_type() const = 0;
    virtual e_director_perspective get_perspective() const = 0;
    virtual void update(long user_index, float elapsed_time, s_observer_command *command) = 0;
    virtual long get_target() const { return m_target; }
    virtual void set_target(long target);
    virtual void set_position(const real_point3d *position) {}
    virtual void set_forward(const real_vector3d *forward) {}
    virtual void set_roll(float roll) {}
    virtual void enable_orientation(bool enable) {}
    virtual void enable_movement(bool enable) {}
    virtual void post_update(s_observer_command *command) {}
    virtual void handle_deleted_player(long player_index) {}
    virtual void handle_deleted_object(long object_index);

    long m_target;
    c_flags_no_init<long, unsigned long, 32> m_camera_flags;
    long m_camera_state;
};

class c_flying_camera : public c_camera
{
public:
    // The zoom button selects the zoomed field of view and look scale. The xy-plane button holds the
    // camera in that plane while pressed, or toggles the constraint with the alternate control scheme.
    enum e_flags
    {
        _zoomed = 0,
        _lock_in_xy_plane = 1,
        _collision_enabled = 2,
        _orientation_enabled = 3,
        _movement_enabled = 4,
        _lock_in_xy_plane_held = 5
    };

    c_flying_camera() {}
    c_flying_camera(long user_index);
    virtual e_camera_mode get_type() const;
    virtual e_director_perspective get_perspective() const;
    virtual void update(long user_index, float elapsed_time, s_observer_command *command);
    virtual void set_position(const real_point3d *position);
    virtual void set_forward(const real_vector3d *forward);
    virtual void set_roll(float roll);
    virtual void enable_orientation(bool enable);
    virtual void enable_movement(bool enable);
    virtual void set_target(long target);
    float get_field_of_view() const;
    void set_lock_in_xy_plane(bool enabled) { m_flags.set(_lock_in_xy_plane, enabled); }
    void set_collision(bool enabled) { m_flags.set(_collision_enabled, enabled); }

private:
    real_point3d m_position;
    real_euler_angles2d m_angles;
    float m_roll;
    float m_velocity[2];
    c_flags_no_init<e_flags, long, 5> m_flags;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char camera_base_size[(sizeof(void *) != 4 || (sizeof(c_camera) == 16)) ? 1 : -1];
typedef char flying_camera_size[(sizeof(void *) != 4 || (sizeof(c_flying_camera) == 52)) ? 1 : -1];
#endif

#endif // CAMERA_CAMERA_TYPES_H
