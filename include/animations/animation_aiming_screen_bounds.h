#ifndef ANIMATIONS_ANIMATION_AIMING_SCREEN_BOUNDS_H
#define ANIMATIONS_ANIMATION_AIMING_SCREEN_BOUNDS_H

struct s_animation_compute_orientations_aiming_screen_bounds
{
    float right_yaw_per_frame;
    float left_yaw_per_frame;
    short right_frame_count;
    short left_frame_count;
    float down_pitch_per_frame;
    float up_pitch_per_frame;
    short down_pitch_frame_count;
    short up_pitch_frame_count;
};

typedef char animation_aiming_screen_bounds_size[(sizeof(s_animation_compute_orientations_aiming_screen_bounds) == 0x18) ? 1 : -1];

#endif // ANIMATIONS_ANIMATION_AIMING_SCREEN_BOUNDS_H
