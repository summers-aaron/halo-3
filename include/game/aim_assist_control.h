#ifndef GAME_AIM_ASSIST_CONTROL_H
#define GAME_AIM_ASSIST_CONTROL_H

class c_aim_assist_flags
{
public:
    void clear() { m_flags = 0; }
    void set(long bit, bool enable)
    {
        if (enable)
            m_flags |= (1 << bit);
        else
            m_flags &= ~(1 << bit);
    }

    unsigned char m_flags;
};
union real_vector3d;
union real_euler_angles2d;
struct s_aim_assist_targeting_result;
extern "C" { void aim_assist_compute_control(long input_user_index, c_aim_assist_flags flags, const real_vector3d *aiming_vector, short zoom_level,
    real_euler_angles2d *angles, s_aim_assist_targeting_result *result); }

#endif // GAME_AIM_ASSIST_CONTROL_H
