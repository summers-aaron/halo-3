#ifndef ANIMATIONS_ANIMATION_STATE_CHANGE_SETTINGS_H
#define ANIMATIONS_ANIMATION_STATE_CHANGE_SETTINGS_H

#include "animations/animation_manager.h"
#include "motor/state_change_flag_bits_types.h"

// Layout and declarations from motor_animation.cpp.
struct s_state_changes_requested
{
    c_flags<e_goal_settings_comparison_bits, unsigned long, 4> goals;
    c_flags<e_channel_settings_comparison_bits, unsigned long, 10> transition;
    c_flags<e_channel_settings_comparison_bits, unsigned long, 10> primary;
};

struct s_animation_state_change_settings
{
    s_animation_goal_settings goals;
    c_animation_channel_settings transition;
    c_animation_channel_settings primary;
    c_flags<e_state_change_flag_bits, unsigned short, 16> flags;
    float interpolation_time;

    __forceinline void clear();

    bool compare_settings(const s_animation_state_change_settings &other, s_state_changes_requested *change_flags) const;
    bool changes_should_interpolate(const s_animation_state_change_settings &other) const;
};

#endif // ANIMATIONS_ANIMATION_STATE_CHANGE_SETTINGS_H
