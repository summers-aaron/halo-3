// objectslibxbox/damage_owner.obj: from the records 0x10f7e8
// @flags /TP /O2 /Oc
#include "objects/object_types.h"
#include <string.h>
#include "objects/damage_owner.h"
#include "units/vehicle_type_component.h"
#include "units/vehicle_auto_control.h"
#include "units/vehicle_datum.h"
#include "objects/object_header.h"
#include "objects/objects.h"

// @address 0x29720 k_damage_owner_unknown
// @rdata 0x82029720..0x8202972c
extern "C" {
const s_damage_owner k_damage_owner_unknown = { -1, -1, -1, 0 };
}
// @rdata-end

// match: 0x10f7e8: the inline boundary keeps the false-result copy after the vehicle test and the non-vehicle branch leading directly to the driver check.
static __forceinline bool try_get_vehicle_auto_control_damage_owner(vehicle_datum *vehicle, long vehicle_index, s_damage_owner *owner)
{
    bool result = false;
    s_vehicle_auto_control *control = vehicle_get_auto_control(vehicle->vehicle.type_state, vehicle_index);
    if (control)
    {
        bool found = false;
        if (control->active)
        {
            *owner = control->damage_owner;
            found = true;
        }
        result = found;
    }
    return result;
}

static __forceinline void damage_owner_get_object_owner(long object_index, s_damage_owner *owner)
{
    memcpy(owner, &object_get(object_index)->damage_owner, sizeof(*owner));
}

// @address 0x10f7e8 ?damage_compute_damage_owner_from_object_index@@YAXJPAUs_damage_owner@@@Z
void damage_compute_damage_owner_from_object_index(long object_index, s_damage_owner *owner)
{
    if (object_index != -1)
    {
        unit_datum *object = (unit_datum *)object_get(object_index);
        bool handled = false;
        if ((1 << object->object.type) & 0x1003)
        {
            if (((1 << object->object.type) & 2) && try_get_vehicle_auto_control_damage_owner((vehicle_datum *)object, object_index, owner))
            {
                handled = true;
            }
            else if (object->unit.driver_object_index != -1)
            {
                handled = true;
                damage_compute_damage_owner_from_object_index(object->unit.driver_object_index, owner);
            }
            else if (object->unit.actor_index != -1 || object->unit.player_index != -1)
            {
                owner->set(object_index, object->unit.player_index, object->unit.team);
                handled = true;
            }
        }
        if (!handled)
            damage_owner_get_object_owner(object_index, owner);
    }
    else
    {
        memcpy(owner, &k_damage_owner_unknown, sizeof(*owner));
    }
}

