#ifndef DEVICES_DEVICE_DATUM_H
#define DEVICES_DEVICE_DATUM_H

#include <stddef.h>
#include "objects/object_datum.h"
#include "devices/device_animation_control.h"
#include "animations/mixing_board/channels/animation_channel.h"

// Device state shared by machines, controls and terminals.
// Holds position and power/overlay animation controls with their channels.
struct device_datum : object_datum
{
    unsigned long device_flags;
    long power_group_index;
    float power;
    float power_velocity;
    long position_group_index;
    float device_position;
    float position_velocity;
    short delay_ticks;
    short pad17e;
    s_device_animation_control position_control;
    s_device_animation_control power_and_overlay_control;
    c_animation_channel position_channel;
    c_animation_channel power_and_overlay_track_channel;
};

typedef char device_datum_animation_offset[(offsetof(device_datum, animation) + offsetof(s_object_block_reference, offset) == 0x150) ? 1 : -1];
typedef char device_datum_flags_offset[(offsetof(device_datum, device_flags) == 0x160) ? 1 : -1];
typedef char device_datum_position_offset[(offsetof(device_datum, device_position) == 0x174) ? 1 : -1];
typedef char device_datum_position_control_offset[(offsetof(device_datum, position_control) == 0x180) ? 1 : -1];
typedef char device_datum_overlay_control_offset[(offsetof(device_datum, power_and_overlay_control) == 0x1a0) ? 1 : -1];
typedef char device_datum_position_channel_offset[(offsetof(device_datum, position_channel) == 0x1c0) ? 1 : -1];
typedef char device_datum_overlay_channel_offset[(offsetof(device_datum, power_and_overlay_track_channel) == 0x1f4) ? 1 : -1];
typedef char device_datum_size[(sizeof(device_datum) == 0x228) ? 1 : -1];

#endif // DEVICES_DEVICE_DATUM_H
