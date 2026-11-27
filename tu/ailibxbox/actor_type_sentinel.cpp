// ailibxbox/actor_type_sentinel.obj: from the records 0x570fd8
// @flags /TP /O2 /Oc
#include "ai/behavior_info.h"
#include "memory/data.h"
#include "memory/tls_globals.h"
#include "math/real_math_types.h"
#include "math/real_math.h"
#include "objects/object_placement.h"
#include "objects/object_datum.h"
#include "units/unit_control.h"
#include "units/unit_types.h"
#include "ai/prop_ref_datum.h"
#include "ai/prop_datum.h"
#include "ai/prop_state.h"
#include "ai/actor_datum.h"
#include "cseries/cseries.h"
// @address 0x570fd8 ?sentinel_enforcer_weapon_control_get_impulse@@YAFJPAVc_behavior_state@@@Z
#include <math.h>
#include <stdlib.h>
// Select the available enforcer weapon from target distance and threat.
#include "objects/objects.h"
#include "ai/actor_combat.h"
#include "ai/actor_perception.h"
#include "ai/behavior.h"
#include "ai/props.h"
#include "ai/weapon_datum_types.h"





/* 0x10 */



#include "ai/character_weapon_properties.h"
#include "ai/character_definitions.h"
#include "units/unit_inventory_inline.h"
#include "objects/object_markers_inline.h"
#define actor_data tls_actor_data()
#define object_header_data tls_object_header_data()




static __forceinline bool actor_prop_is_majorly_scary(prop_ref_datum *prop_ref)
{
    // match: 0x570fd8: pre-existing identity rotate; removing it moves three words in sentinel_enforcer_weapon_control_get_impulse (check 9).
    long prop_offset = ((long)_rotl(prop_ref->prop_index, 0) & 0xffff) * sizeof(prop_datum);
    prop_datum *prop = (prop_datum *)(prop_data->data + prop_offset);
    bool result = false;
    if (prop->actor_index != NONE)
        result = actor_is_majorly_scary(prop->actor_index);
    return result;
}

/* the actor's character and the weapon's definition */
static __forceinline character_weapon_properties *actor_weapon_properties_get(long actor_index, long weapon_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    weapon_datum *weapon = (weapon_datum *)object_get_and_verify_type(weapon_index, 4);
    return character_weapon_properties_get(actor->character_index, weapon->definition_index);
}

short sentinel_enforcer_weapon_control_get_impulse(long actor_index, c_behavior_state *state)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    if (actor->target_prop_index != NONE && actor->target_state != 2)
    {
        long unit_index = actor->unit_index;
        prop_ref_datum *target = datum_get<prop_ref_datum>(prop_ref_data, actor->target_prop_index);
        unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 0x2003);
        prop_state *target_state = prop_state_get(target);
        short weapon_index = NONE;
        character_weapon_properties *first_properties = 0;
        character_weapon_properties *second_properties = 0;
        character_weapon_properties *third_properties = 0;
        short current_weapon_index = unit->unit.current_weapon_set.weapon_indices[0];
        bool majorly_scary = target_state->parent_object_index != NONE || actor_prop_is_majorly_scary(target);
        real_vector2d target_offset;
        vector_from_points2d(&actor->position.body.xy, &target_state->position.xy, &target_offset);
        float distance = sqrtf(target_offset.i * target_offset.i + target_offset.j * target_offset.j);
        object_marker markers[1];

        long first_weapon = unit->unit.weapon_object_indices[0];
        if (first_weapon != NONE && object_get_markers_by_string_id(unit_index, 671, markers, 1) > 0)
            first_properties = actor_weapon_properties_get(actor_index, first_weapon);

        long second_weapon = unit_inventory_get_weapon(actor->unit_index, 1);
        if (second_weapon != NONE && object_get_markers_by_string_id(actor->unit_index, 672, markers, 1) > 0)
            second_properties = actor_weapon_properties_get(actor_index, second_weapon);

        long third_weapon = unit_inventory_get_weapon(actor->unit_index, 1);
        if (third_weapon != NONE && object_get_markers_by_string_id(actor->unit_index, 673, markers, 1) > 0)
            third_properties = actor_weapon_properties_get(actor_index, third_weapon);

        if (first_properties && majorly_scary &&
            (!third_properties || distance < third_properties->maximum_firing_range ||
             current_weapon_index == 0 && distance < 1.5f * third_properties->maximum_firing_range))
        {
            weapon_index = 0;
        }
        else if (first_properties && !second_properties &&
            (!third_properties || target->distance < third_properties->maximum_firing_range))
        {
            weapon_index = 0;
        }
        else if (third_properties && distance >= third_properties->maximum_firing_range &&
            (actor_executing(actor_index, 49) || majorly_scary))
        {
            weapon_index = 1;
        }
        else if (third_properties && !second_properties)
        {
            weapon_index = 1;
        }
        else if (second_properties)
        {
            weapon_index = 2;
            if (third_properties)
            {
                short count = 0;
                if (actor_combat_check_collateral_damage(actor_index, 2.5f, 3.0f, &target_state->position, &count, 0) && count > 3)
                    weapon_index = 1;
            }
        }

        if (weapon_index != NONE)
        {
            unit->unit.desired_weapon_set.weapon_indices[0] = (signed char)weapon_index;
            unit->unit.desired_weapon_set.weapon_indices[1] = NONE;
        }
    }
    return NONE;
}

// @address 0x7c5710 ?sentinel_enforcer_weapon_control_impulse@@3VBehavior_impulse@@A
Behavior_impulse sentinel_enforcer_weapon_control_impulse = Behavior_impulse(151, 0x3ff007ff,
    sentinel_enforcer_weapon_control_get_impulse);
