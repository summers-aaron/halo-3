#ifndef DEVICES_DEVICE_CONTROLS_H
#define DEVICES_DEVICE_CONTROLS_H

#include <stddef.h>
#include "devices/device_datum.h"
#include "scenario/scenario_objects.h"

struct control_datum : device_datum
{
    unsigned long control_flags;
    short custom_label_index;
};

struct s_scenario_control : s_scenario_object
{
    short power_group_index;
    short position_group_index;
    unsigned long device_flags;
    unsigned long control_flags;
    short custom_label;
};

typedef char control_datum_flags_offset[(sizeof(void *) != 4 || (offsetof(control_datum, control_flags) == 0x228)) ? 1 : -1];
typedef char scenario_control_device_flags_offset[(offsetof(s_scenario_control, device_flags) == 0x58) ? 1 : -1];
typedef char scenario_control_flags_offset[(offsetof(s_scenario_control, control_flags) == 0x5c) ? 1 : -1];
typedef char scenario_control_label_offset[(offsetof(s_scenario_control, custom_label) == 0x60) ? 1 : -1];
typedef char scenario_control_size[(sizeof(s_scenario_control) == 0x64) ? 1 : -1];

// Functions defined in tu/objectslibxbox/device_controls.cpp that other units call.

void control_place(long object_index, s_scenario_control *placement);
void control_toggle(long object_index);

#endif // DEVICES_DEVICE_CONTROLS_H
