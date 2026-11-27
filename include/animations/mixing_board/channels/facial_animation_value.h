#ifndef ANIMATIONS_MIXING_BOARD_CHANNELS_FACIAL_ANIMATION_VALUE_H
#define ANIMATIONS_MIXING_BOARD_CHANNELS_FACIAL_ANIMATION_VALUE_H

#include "math/short_fixed.h"

class c_facial_animation_value {
public:
    float unpack() const { return (float)m_value / 127.0f; }
    float unpack_scaled() const { return (float)m_value * 0.007874016f; }
    void pack(float value);
    bool active() const { return m_value > 0; }
    signed char m_value;
};

struct s_facial_emotion {
    t_short_fixed<8> m_rate;
    c_facial_animation_value m_value;
    c_facial_animation_value m_target;
};

typedef char facial_animation_value_size[(sizeof(c_facial_animation_value) == 1) ? 1 : -1];
typedef char facial_animation_emotion_value_size[(sizeof(s_facial_emotion) == 4) ? 1 : -1];

#endif
