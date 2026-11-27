#ifndef CAMERA_FOLLOWING_CAMERA_H
#define CAMERA_FOLLOWING_CAMERA_H

#include "camera/camera_types.h"
#include "game/director_perspective_types.h"

// Functions and types defined in tu/blamlibxbox/following_camera.cpp.

struct s_unit_camera;

class c_following_camera : public c_camera
{
public:
    c_following_camera() {}
    c_following_camera(long target)
    {
        c_following_camera::set_target(target);
    }

    virtual e_camera_mode get_type() const { return _camera_mode_following; }
    virtual e_director_perspective get_perspective() const { return _director_perspective_third_person; }
    virtual void update(long user_index, float elapsed_time, s_observer_command *command);
    virtual void set_target(long target);

private:
    float m_yaw_offset;
    float m_pitch_offset;
    float m_focus_scale;
    real_vector3d m_acceleration_offset;    // following_camera_update_acceleration's lagging offset
    long m_last_unit_index;
    bool m_spare_flag0;
    bool m_crouching;
    bool m_spare_flag1;
    bool m_tight_camera_track;
    unsigned short m_spare_flags;
    short m_last_seat_index;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char following_camera_size[(sizeof(void *) != 4 || (sizeof(c_following_camera) == 52)) ? 1 : -1];
#endif

void following_camera_deterministic(long index, real_point3d *position, real_vector3d *direction);

float compute_t_from_pitch(const s_unit_camera *camera, float pitch, bool tight);

union real_euler_angles3d;
struct real_matrix4x3;
union real_vector3d;

extern "C" void following_camera_deterministic(const real_matrix4x3 *matrix, real_euler_angles3d *angles);
void camera_track_splut(const s_unit_camera *camera, float time, real_vector3d *forward, real_vector3d *position, bool alternate);

// The field of view range of c_following_camera::update.
extern "C" float k_following_camera_fov_min;
extern "C" float k_following_camera_fov_max;

#endif // CAMERA_FOLLOWING_CAMERA_H
