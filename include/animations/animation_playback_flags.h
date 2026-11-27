#ifndef ANIMATIONS_ANIMATION_PLAYBACK_FLAGS_H
#define ANIMATIONS_ANIMATION_PLAYBACK_FLAGS_H

enum e_animation_playback_flags
{
    _animation_playback_flag_none = 0,
    _animation_playback_complete_bit = 2,
    _animation_playback_disable_weapon_ik_bit = 3,
    _animation_playback_disable_weapon_aim_first_person_bit = 4,
    _animation_playback_disable_look_screen_bit = 5,
    _animation_playback_force_weapon_ik_on_bit = 7
};

#endif // ANIMATIONS_ANIMATION_PLAYBACK_FLAGS_H
