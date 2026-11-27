#ifndef DEVICES_DEVICE_ANIMATION_CONTROL_H
#define DEVICES_DEVICE_ANIMATION_CONTROL_H

struct s_device_animation_control
{
    float position;
    float velocity;
    float duration;
    float deceleration_start;
    float deceleration_duration;
    float cruise_velocity;
    float final_velocity;
    float target_position;
};

typedef char s_device_animation_control_size_assert[(sizeof(s_device_animation_control) == 0x20) ? 1 : -1];

#endif // DEVICES_DEVICE_ANIMATION_CONTROL_H
