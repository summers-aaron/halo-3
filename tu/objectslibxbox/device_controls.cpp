// objectslibxbox/device_controls.obj: from the records 0x36f618 0x36f6b8
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "objects/object_header.h"
#include "memory/data.h"
#include "tag_files/tag_groups.h"
#include "devices/devices.h"
#include "devices/device_group.h"
#include "devices/device_controls.h"
#include "devices/device_definitions.h"

// @address 0x36f618 ?control_place@@YAXJPAUs_scenario_control@@@Z
void control_place(long object_index, s_scenario_control *placement)
{
    control_datum *control = OBJECT_GET(object_index, control_datum, _object_mask_control);
    if ((placement->control_flags & 1) != 0)
        control->control_flags |= 1;
    if ((placement->control_flags & 16) != 0)
        control->control_flags |= 2;
    control->control_flags &= ~2UL;
    if ((placement->device_flags & 4) != 0)
        control->control_flags |= 4;
    else
        control->control_flags &= ~4UL;
    control->custom_label_index = (short)(placement->custom_label - 1);
}

// @address 0x36f6b8 ?control_toggle@@YAXJ@Z
void control_toggle(long object_index)
{
    control_datum *control = OBJECT_GET(object_index, control_datum, _object_mask_control);
    s_control_definition *definition = TAG_GET('ctrl', s_control_definition, control->definition_index);
    long group_index = control->position_group_index;
    if (group_index == -1)
        return;
    bool toggled = false;
    if ((control->control_flags & 4) == 0 || (control->control_flags & 2) == 0) {
        s_device_group_datum *group = &((s_device_group_datum *)g_device_groups_data->data)[(unsigned short)group_index];
        float value;
        switch (definition->type) {
        case 0:
            if (group->actual_value > 0.5f)
                value = 0.0f;
            else
                value = 1.0f;
            break;
        case 1:
            value = 1.0f;
            break;
        case 2:
            value = 0.0f;
            break;
        case 3:
            value = definition->call_value;
            break;
        default:
            // match: 0x36f6b8: unreachable hint omits dead branch check
            __assume(0);
        }
        if (device_group_set_desired_value(object_index, group_index, value)) {
            long effect_index;
            if (value > 0.5f)
                effect_index = definition->on.index;
            else
                effect_index = definition->off.index;
            device_effect_new(object_index, effect_index);
            toggled = true;
            control->control_flags |= 2;
        }
    }
    if (!toggled)
        device_effect_new(object_index, definition->deny.index);
}
