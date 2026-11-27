#ifndef ANIMATIONS_MIXING_BOARD_CHANNELS_AIMING_CHANNEL_H
#define ANIMATIONS_MIXING_BOARD_CHANNELS_AIMING_CHANNEL_H

#include "animations/mixing_board/channels/channel_base.h"

extern float k_aiming_interpolation_stop_delta;
extern float k_aiming_interpolation_start_delta;

class c_aiming_channel_settings : public c_base_channel_settings {
public:
 union { struct { float m_yaw; float m_pitch; }; real_euler_angles2d m_angles; };
 bool m_use_angles;
};

class c_aiming_channel : public c_channel_base
{
public:
    void initialize()
    {
        m_parent_slider_index = -1;
        m_parent_slider_type = _mixing_board_parent_slider_none;
        m_enabled = true;
        clear();
        clear();
        float zero = 0.0f;
        m_yaw = zero;
        m_pitch = zero;
        m_target_yaw = zero;
        m_target_pitch = zero;
        m_yaw_min = zero;
        m_yaw_max = zero;
        m_pitch_min = zero;
        m_pitch_max = zero;
    }

    void set_yaw(float yaw);
    void set_pitch(float pitch);

private:
    friend class c_animation_manager;
    bool setup_channel(const c_aiming_channel_settings *settings);

public:
    union { struct { float m_yaw; float m_pitch; }; real_euler_angles2d m_angles; };
    float m_target_yaw;
    float m_target_pitch;
    float m_yaw_min;
    float m_yaw_max;
    float m_pitch_min;
    float m_pitch_max;
};

typedef char aiming_channel_size[(sizeof(c_aiming_channel) == 52) ? 1 : -1];

#endif // ANIMATIONS_MIXING_BOARD_CHANNELS_AIMING_CHANNEL_H
