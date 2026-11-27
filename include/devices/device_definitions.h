#ifndef DEVICES_DEVICE_DEFINITIONS_H
#define DEVICES_DEVICE_DEFINITIONS_H

#include <stddef.h>
#include "objects/object_definitions.h"

struct s_device_definition : s_object_definition
{
    unsigned long device_flags;
    float power_transition_time;
    float power_acceleration_time;
    float position_transition_time;
    float position_acceleration_time;
    float depowered_position_transition_time;
    float depowered_position_acceleration_time;
    unsigned short lightmap_flags;
    s_tag_reference open_up;
    s_tag_reference close_down;
    s_tag_reference opened;
    s_tag_reference closed;
    s_tag_reference depowered;
    s_tag_reference repowered;
    float delay_time;
    s_tag_reference delay_effect;
    float automatic_activation_radius;
};

// Machine tag layout shared by reference-frame and device-machine code.
struct s_machine_definition : s_device_definition
{
    short type;
    unsigned short flags;
    long action_string;
    float open_occlusion;
    float closed_occlusion;
    long pad1a0;
    short pathfinding_policy;
};

typedef char machine_definition_type_offset[(offsetof(s_machine_definition, type) == 0x190) ? 1 : -1];
typedef char machine_definition_flags_offset[(offsetof(s_machine_definition, flags) == 0x192) ? 1 : -1];
typedef char machine_definition_open_occlusion_offset[(offsetof(s_machine_definition, open_occlusion) == 0x198) ? 1 : -1];
typedef char machine_definition_closed_occlusion_offset[(offsetof(s_machine_definition, closed_occlusion) == 0x19c) ? 1 : -1];

struct s_control_definition : s_device_definition
{
    short type;
    short triggers_when;
    float call_value;
    long action_string;
    s_tag_reference on;
    s_tag_reference off;
    s_tag_reference deny;
};

typedef char device_definition_open_offset[(offsetof(s_device_definition, open_up) == 0x118) ? 1 : -1];
typedef char device_definition_size[(sizeof(s_device_definition) == 0x190) ? 1 : -1];
typedef char control_definition_type_offset[(offsetof(s_control_definition, type) == 0x190) ? 1 : -1];
typedef char control_definition_call_value_offset[(offsetof(s_control_definition, call_value) == 0x194) ? 1 : -1];
typedef char control_definition_on_offset[(offsetof(s_control_definition, on) == 0x19c) ? 1 : -1];
typedef char control_definition_off_offset[(offsetof(s_control_definition, off) == 0x1ac) ? 1 : -1];
typedef char control_definition_deny_offset[(offsetof(s_control_definition, deny) == 0x1bc) ? 1 : -1];
typedef char control_definition_size[(sizeof(s_control_definition) == 0x1cc) ? 1 : -1];

typedef char machine_action_string_offset[(offsetof(s_machine_definition, action_string) == 0x194) ? 1 : -1];
#endif // DEVICES_DEVICE_DEFINITIONS_H
