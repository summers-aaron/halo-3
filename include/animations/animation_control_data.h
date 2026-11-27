#ifndef ANIMATIONS_ANIMATION_CONTROL_DATA_H
#define ANIMATIONS_ANIMATION_CONTROL_DATA_H

// March set_defaults (82863788) initializes these fields through playback_flags
// Shared with motor animation playback.
struct s_animation_control_data
{
    long mode;
    long state;
    bool moved;
    union { bool crouch; bool crouching; };
    union
    {
        long secondary_states[2];
        struct { long secondary_state, secondary_mode; };
    };
    union
    {
        float blend_ratios[2];
        struct { float primary_weight, secondary_weight; };
    };
    long flags;
    unsigned long playback_flags;

    void set_defaults();
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char animation_control_data_size[(sizeof(s_animation_control_data) == 0x24) ? 1 : -1];
#endif

#endif // ANIMATIONS_ANIMATION_CONTROL_DATA_H
