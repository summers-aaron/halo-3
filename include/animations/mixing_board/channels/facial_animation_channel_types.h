#ifndef ANIMATIONS_MIXING_BOARD_CHANNELS_FACIAL_ANIMATION_CHANNEL_TYPES_H
#define ANIMATIONS_MIXING_BOARD_CHANNELS_FACIAL_ANIMATION_CHANNEL_TYPES_H

#include <string.h>
#include "cseries/cseries_wrapped.h"
#include "math/short_fixed.h"
#include "animations/animation_id.h"
#include "animations/mixing_board/channels/channel_base.h"
#include "game/facial_animation_settings_flags_types.h"

enum e_facial_animation_state_flags
{
    _facial_animation_state_emotion_bit,
    _facial_animation_state_gesture_bit,
    _facial_animation_state_lipsync_bit,
    k_facial_animation_state_flags
};

#include "animations/mixing_board/channels/facial_animation_value.h"

extern "C" const long k_facial_emotion_string_ids[9];

class c_facial_animation_channel : public c_channel_base
{
    friend class c_animation_manager;
public:
    __forceinline void set_target_emotion_by_name(long name, float value, float time)
    {
        for (long emotion_index = 0; emotion_index < 9; ++emotion_index)
        {
            if (name == k_facial_emotion_string_ids[emotion_index])
            {
                m_emotions[emotion_index].m_target.m_value = (signed char)(long)(MIN(MAX(value * 127.0f, -127.0f), 127.0f));
                m_emotions[emotion_index].m_rate.m_value = (short)(time * 256.0f);
                break;
            }
        }
    }

    void initialize_base()
    {
        m_parent_slider_index = -1;
        m_parent_slider_type = _mixing_board_parent_slider_none;
        m_enabled = true;
        clear();
    }

    void initialize();

    __forceinline void initialize_inline()
    {
        clear();
        memset(m_emotions, 0, sizeof(m_emotions));
        m_sound_index = -1;
        m_lipsync_index = -1;
        m_frame = 0.0f;
        m_gesture_frame = 0;
        m_gesture_duration = 0;
        m_lipsync_frames_remaining = 0;
        m_state_flags.clear();
        m_settings_flags.clear();
        m_animation_ids[0].clear();
        m_animation_ids[1].clear();
        m_blend = 0.0f;
        m_scale = 1.0f;
    }

    __forceinline bool call_setup_channel_base(const c_base_channel_settings *settings)
    {
        return setup_channel_base(settings);
    }

    __forceinline void set_animations(c_animation_id animation_a, c_animation_id animation_b)
    {
        m_animation_ids[0] = animation_a;
        m_animation_ids[1] = animation_b;
    }

    bool facial_animation_in_progress() const;
    bool lipsync_in_progress() const { return m_lipsync_frames_remaining.m_value > 0; }

    c_animation_id m_animation_ids[2];
    s_facial_emotion m_emotions[9];
    long m_sound_index;
    long m_lipsync_index;
    float m_frame;
    float m_blend;
    float m_scale;
    t_short_fixed<8> m_gesture_frame;
    t_short_fixed<8> m_gesture_duration;
    t_short_fixed<8> m_lipsync_frames_remaining;
    c_flags_no_init<e_facial_animation_settings_flags, unsigned char, 3> m_settings_flags;
    c_flags_no_init<e_facial_animation_state_flags, unsigned char, k_facial_animation_state_flags> m_state_flags;

protected:
    bool update();
    void apply_gesture_orientations(const render_model_definition *model, float weight, long positive_frame,
        long negative_frame, const c_static_flags<255> *mask, long count, real_orientation *orientations,
        animation_node_orientations_callback callback, long callback_arg);
    void apply_node_orientations(const render_model_definition *render_model, float weight, const c_static_flags<255> *static_flags,
        long node_count, real_orientation *orientations,
        animation_node_orientations_callback callback, long user_data);
};

typedef char facial_emotion_size[(sizeof(s_facial_emotion) == 4) ? 1 : -1];
typedef char facial_animation_channel_size[(sizeof(c_facial_animation_channel) == 92) ? 1 : -1];

#endif // ANIMATIONS_MIXING_BOARD_CHANNELS_FACIAL_ANIMATION_CHANNEL_TYPES_H
