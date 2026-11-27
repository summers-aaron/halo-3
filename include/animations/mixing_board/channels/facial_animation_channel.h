#ifndef ANIMATIONS_MIXING_BOARD_CHANNELS_FACIAL_ANIMATION_CHANNEL_H
#define ANIMATIONS_MIXING_BOARD_CHANNELS_FACIAL_ANIMATION_CHANNEL_H

template<long k_bits> class t_short_fixed;

extern "C" {
extern t_short_fixed<8> k_facial_animation_gesture_duration;
extern float k_facial_animation_gesture_table[10];
}

extern const t_short_fixed<8> k_facial_animation_blink_duration;

#endif // ANIMATIONS_MIXING_BOARD_CHANNELS_FACIAL_ANIMATION_CHANNEL_H

