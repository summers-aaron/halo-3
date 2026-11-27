#ifndef ANIMATIONS_MIXING_BOARD_CHANNELS_AUXILARY_CHANNEL_H
#define ANIMATIONS_MIXING_BOARD_CHANNELS_AUXILARY_CHANNEL_H

#include "animations/mixing_board/channels/channel_base.h"

class c_auxilary_channel : public c_channel_base
{
public:
    using c_channel_base::setup_channel_base;
    void initialize()
    {
        m_parent_slider_index = -1;
        m_parent_slider_type = _mixing_board_parent_slider_none;
        m_enabled = true;
        clear();
        m_frame_position = 0.0f;
    }

    void set_frame_position(float frame_position);
    float get_playback_ratio() const;

    float m_frame_position;
};

typedef char auxilary_channel_size[(sizeof(c_auxilary_channel) == 24) ? 1 : -1];

class c_auxilary_channel_settings : public c_base_channel_settings {
public:
 union { struct { float m_yaw; float m_pitch; }; real_euler_angles2d m_angles; };
 bool m_use_angles;
};

#endif // ANIMATIONS_MIXING_BOARD_CHANNELS_AUXILARY_CHANNEL_H
