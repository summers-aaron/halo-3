// ailibxbox/combat_range.obj: from the records 0x11d200 0x11da18 0x11db30 0x11df98
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "ai/combat_range_constants.h"
#include "ai/actor_destination.h"
#include "memory/tls_globals.h"
#include "units/unit_seat.h"
#include "math/real_math.h"
#include "units/unit_passenger_iterator.h"
#include "ai/actor_datum.h"
#include <math.h>
#include <float.h>
#include "ai/actors.h"
#include "ai/actor_combat.h"
#include "ai/ai_reference_frame.h"
#include "ai/objectives.h"
#include "units/bipeds.h"
#include "ai/props.h"
#include "ai/character_definitions.h"
#include "ai/combat_range.h"
#include "ai/objective_definitions.h"
#include "ai/prop_state.h"
#include "ai/character_weapon_properties.h"
#include "ai/actor_type_flood_types.h"
#include "units/unit_types.h"
#include "units/unit_definitions.h"
#include "objects/object_datum.h"
#include "ai/squad_types.h"
#include "ai/firing_position_definition.h"
#include "cache/cache_files.h"
#include "ai/ai_point3d.h"
#include "math/real_math_types.h"
#include "tag_files/tag_types.h"
#include "ai/Zone.h"
#include "scenario/scenario_tag.h"
#include "scenario/scenario.h"
#include "objects/object_header_access.h"
#include "memory/datum_index.h"

// @address 0x11d200 actor_get_weapon_range
// @address 0x11da18 ?add_actor@c_combat_range@@QAAXJ@Z
// @address 0x347268 ?biped_get_current_weapon@@YAJJ@Z
// @address 0xafb40 ?actor_weapon_properties_get@@YAPAUcharacter_weapon_properties@@J@Z
// @address 0xc0430 ?character_weapon_properties_get@@YAPAUcharacter_weapon_properties@@JJ@Z
// @address 0xc16f8 ?actor_get_weapon_bounds@@YA_NJPAUcharacter_weapon_properties@@PAM11@Z
// @address 0xadcf0 ?character_definition_general_properties_get@@YAPAUcharacter_general_properties@@J@Z
struct s_variant_reference;

extern "C" {
// @address 0x7c5d00 k_combat_range_ideal_scale
// @address 0x7c5d04 k_combat_range_spread_scale
float k_combat_range_ideal_scale[] = { 0.8f };
float k_combat_range_spread_scale[] = { 0.5f };
const float k_max_combat_range_scale = 0.5f;
const float k_combat_range_maximum_fraction = 0.8f;
const float k_combat_range_leader_distance = 4.0f;
const float k_real_max = FLT_MAX;
}

#define combat_range_actor_data tls_actor_data()
#define combat_range_object_header_data tls_object_header_data()

static __forceinline zone_definition *combat_range_zone_get(long zone_index)
{
    return (zone_definition *)global_scenario->zones.address + ((unsigned long)zone_index & 0xffff);
}

static __forceinline area_definition *combat_range_zone_area_get(zone_definition *zone, short area_index)
{
    return (area_definition *)zone->areas.address + area_index;
}


bool actor_get_weapon_range(long actor_index, float *minimum, float *maximum, float *distance)
{
    character_weapon_properties *best = actor_weapon_properties_get(actor_index);
    actor_datum *player = (actor_datum *)(combat_range_actor_data->data + (actor_index & 0xffff) * 0xa8c);
    bool result = false;
    if (!best) {
        if (player->root_vehicle_index != -1) {
            long vehicle_index = player->root_vehicle_index;
            bool found_gunner = false;
            c_multi_unit_passenger_iterator iterator;
            iterator.begin(vehicle_index);
            if (iterator.next()) {
                const cache_file_tag_instance *tags = global_tag_instances;
                do {
                    object_datum *unit = object_get(iterator.m_passenger_index);
                    if (unit->identifier_type)
                        continue;
                    long weapon_index = biped_get_current_weapon(iterator.m_passenger_index);
                    if (weapon_index == -1)
                        continue;
                    long owner_index = ((unit_datum *)unit)->unit.actor_index;
                    s_object_header *headers = (s_object_header *)combat_range_object_header_data->data;
                    long definition_index = headers[datum_index_to_absolute_index(weapon_index)].datum->definition_index;
                    if (owner_index == -1)
                        owner_index = actor_index;
                    character_weapon_properties *properties = character_weapon_properties_get(
                        ((actor_datum *)(combat_range_actor_data->data + (owner_index & 0xffff) * 0xa8c))->character_index,
                        definition_index);
                    if (!properties)
                        continue;
                    bool gunner = false;
                    if (iterator.m_seat_index != -1 && iterator.m_parent_index != -1) {
                        object_datum *vehicle = headers[datum_index_to_absolute_index(iterator.m_parent_index)].datum;
                        unit_definition *definition = (unit_definition *)tags[datum_index_to_absolute_index(vehicle->definition_index)].definition;
                        gunner = (((unit_seat *)definition->seats.address)[iterator.m_seat_index].flags >> 3) & 1;
                    }
                    if (!best || (gunner && !found_gunner) || best->maximum_range > properties->maximum_range)
                        best = properties;
                    found_gunner = found_gunner || gunner;
                } while (iterator.next());
            }
        }
    }
    if (best)
        result = actor_get_weapon_bounds(actor_index, best, minimum, maximum, distance);
    if (!result && ::actor_get_standing(actor_index) == 3) {
        if (minimum)
            *minimum = 3.0f;
        if (maximum)
            *maximum = 6.0f;
        if (distance)
            *distance = 15.0f;
        return true;
    }
    return result;
}



typedef char combat_range_size[(sizeof(c_combat_range) == 0x68) ? 1 : -1];
typedef char combat_range_target_offset[(offsetof(c_combat_range, m_leader_position) == 0x18) ? 1 : -1];
typedef char combat_range_actor_count_offset[(offsetof(c_combat_range, m_actor_count) == 0x3e) ? 1 : -1];


__forceinline bool c_combat_range::set_target_from_actor(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(combat_range_actor_data, actor_index);
    m_leader_position_valid = false;
    if (actor->target_prop_index != -1) {
        prop_state *prop = (prop_state *)prop_state_get_by_index(actor->target_prop_index);
        m_leader_position = prop->position;
        m_leader_position_valid = true;
    }
    return m_leader_position_valid;
}

__forceinline bool c_combat_range::set_weapon_range_from_actor(long actor_index)
{
    m_weapon_range_valid = actor_get_weapon_range(actor_index, &m_weapon_minimum, &m_weapon_maximum, &m_weapon_distance);
    return m_weapon_range_valid;
}

__forceinline void c_combat_range::set_constraints_from_actor(long actor_index)
{
    if (actor_index != -1) {
        long squad_index = datum_get<actor_datum>(combat_range_actor_data, actor_index)->squad_index;
        if (squad_index != -1) {
            squad_datum *squad = (squad_datum *)(tls_squad_data()->data + (squad_index & 0xffff) * 0xb0);
            if (::squad_valid_task((squad_datum *)(const s_variant_reference *)squad))
                set_constraints_from_task(squad->objective_index, squad->task_index);
        }
    }
}

__forceinline void c_combat_range::get_constraints(short standing, float *minimum, float *maximum)
{
    float constraint_maximum = k_real_max;
    float constraint_minimum = 0.0f;
    if (standing == 3 && m_leader_valid) {
        constraint_minimum = m_leader_minimum;
        constraint_maximum = m_leader_maximum;
    } else if (m_target_valid) {
        constraint_minimum = m_follower_minimum;
        constraint_maximum = m_follower_maximum;
    }
    *minimum = constraint_minimum;
    *maximum = constraint_maximum;
}

__forceinline void c_combat_range::set_constraints(float minimum, float maximum)
{
    m_follower_minimum = minimum;
    m_follower_maximum = maximum;
    m_target_valid = true;
}

__forceinline void c_combat_range::set_leader_constraints(float minimum, float maximum)
{
    m_leader_minimum = minimum;
    m_leader_maximum = maximum;
    m_leader_valid = true;
}



void c_combat_range::add_actor(long actor_index)
{
    character_general_properties *properties = (character_general_properties *)character_definition_general_properties_get(
        ((actor_datum *)(combat_range_actor_data->data + (actor_index & 0xffff) * 0xa8c))->character_index);
    if (::actor_get_standing(actor_index) == 3) {
        m_leader_index = actor_index;
        return;
    }
    float bounds[2];
    if ((!properties || !(properties->flags & 0x100)) && actor_get_weapon_bounds(actor_index, 0, &bounds[0], &bounds[1], 0)) {
        float maximum = 4.0f;
        if (properties && properties->leader_distance > 0.0f)
            maximum = properties->leader_distance;
        if (m_maximum > maximum)
            maximum = m_maximum;
        m_maximum = maximum;
        float minimum = bounds[0];
        if (!(m_minimum > minimum))
            minimum = m_minimum;
        m_minimum = minimum;
        float distance = m_distance;
        if (!(distance > bounds[1]))
            distance = bounds[1];
        m_distance = distance;
        m_actor_count++;
    }
}

// @address 0x11db30 ?place_actor@c_combat_range@@QAA_NJ@Z
bool c_combat_range::place_actor(long actor_index)
{
    bool result = false;
    if ((m_leader_position_valid || set_target_from_actor(actor_index)) &&
        (m_weapon_range_valid || set_weapon_range_from_actor(actor_index))) {
        if (!m_target_valid)
            set_constraints_from_actor(actor_index);

        short standing = ::actor_get_standing(actor_index);
        float ideal_fraction = 0.3f;
        float ideal_scale = ideal_fraction;
        actor_datum *actor = datum_get<actor_datum>(combat_range_actor_data, actor_index);
        character_general_properties *properties = (character_general_properties *)character_definition_general_properties_get(actor->character_index);
        float minimum = m_weapon_minimum;
        float spread = m_weapon_maximum - minimum;
        float range_scale = 1.0f;
        if (m_actor_count > 0 && actor->root_vehicle_index == -1 && (!properties || !(properties->flags & 0x100))) {
            switch (standing) {
            case 0:
                break;
            case 1:
            case 2:
                if (m_leader_index != -1) {
                    float weapon_range = k_combat_range_leader_distance;
                    if (properties && properties->leader_distance > 0.0f)
                        weapon_range = properties->leader_distance;
                    float leader_minimum;
                    float leader_maximum;
                    actor_get_weapon_range(m_leader_index, &leader_minimum, &leader_maximum, 0);
                    real_point3d destination;
                    actor_get_destination(m_leader_index, &destination);
                    float value = distance2d(&destination.xy, &m_leader_position.xy) -
                        k_combat_range_spread_scale[0] * ((leader_maximum - leader_minimum) * k_combat_range_ideal_scale[0]) - weapon_range;
                    if (minimum <= value)
                        minimum = value;
                }
                break;
            default: {
                float follower_minimum;
                float follower_maximum;
                get_constraints(1, &follower_minimum, &follower_maximum);
                minimum = m_maximum + MAX(m_minimum, follower_minimum);
                range_scale = k_combat_range_spread_scale[0];
                ideal_scale = k_combat_range_ideal_scale[0];
                break;
            }
            }
        }
        float leader_minimum;
        float leader_maximum;
        get_constraints(standing, &leader_minimum, &leader_maximum);
        float weapon_minimum = m_weapon_minimum;
        minimum = MAX(leader_minimum, MAX(minimum, weapon_minimum));
        float maximum_range = range_scale * spread + minimum;
        if (standing != 3 && minimum < weapon_minimum)
            minimum = weapon_minimum;
        float overflow = maximum_range - leader_maximum;
        if (overflow > 0.0f) {
            minimum -= overflow;
            maximum_range -= overflow;
        }
        if (standing != 3 && minimum < weapon_minimum) {
            float ratio = MAX(k_max_combat_range_scale, 1.0f - (weapon_minimum - minimum) / (maximum_range - minimum));
            minimum = (1.0f - ratio) * (maximum_range - minimum) + minimum;
            range_scale = ratio;
        }
        minimum = MAX(range_scale * k_max_combat_range_scale, minimum);
        m_range_center = minimum;
        m_range_minimum = maximum_range;
        m_range_spread = MAX(0.0f, minimum - (range_scale * k_max_combat_range_scale));
        m_range_maximum = range_scale * spread * k_combat_range_maximum_fraction + minimum;
        m_range_vertical = m_range_minimum - m_weapon_maximum + m_weapon_distance;
        m_range_ideal = (m_range_maximum - m_range_center) * ideal_scale + m_range_center;
        if (actor->root_vehicle_index != -1) {
            float adjusted = (m_range_maximum - m_range_center) * k_combat_range_maximum_fraction + m_range_center;
            float limit = (m_weapon_maximum - m_weapon_minimum) * ideal_fraction + m_weapon_minimum;
            if (adjusted <= limit)
                limit = adjusted;
            if (limit <= m_range_ideal)
                limit = m_range_ideal;
            m_range_ideal = limit;
        }
        result = true;
    }
    return result;
}


// @address 0x11df98 ?set_constraints_from_task@c_combat_range@@QAA_NFF@Z
bool c_combat_range::set_constraints_from_task(short objective_index, short task_index)
{
    float dy = 0.0f;
    m_target_valid = false;
    if (m_leader_position_valid) {
        s_objective *objective = (s_objective *)global_scenario->objectives.address + objective_index;
        s_task *task = (s_task *)objective->tasks.address + task_index;
        bool leader_ready = false;
        bool found = false;
        bool found_leader = false;
        float minimum = 3.4028234663852886e+38f;
        float maximum = 0.0f;
        float leader_minimum = 3.4028234663852886e+38f;
        float leader_maximum = 0.0f;
        if (m_leader_index != -1)
            leader_ready = datum_get<actor_datum>(combat_range_actor_data, m_leader_index)->goal_status == 3;
        for (short area_index = 0; area_index < task->areas.count; ++area_index) {
            squads_area_zone_set_entry *area = (squads_area_zone_set_entry *)task->areas.address + area_index;
            if (leader_ready && !(area->flags & 1))
                continue;
            short type = area->type;
            if (type == 1)
                continue;
            if ((unsigned short)area->zone_index == 0xffff)
                continue;
            if (area->area_index == -1)
                continue;
            zone_definition *zone = combat_range_zone_get(area->zone_index);
            area_definition *definition = combat_range_zone_area_get(zone, area->area_index);
            short firing_position = definition->first_firing_position;
            long end = firing_position + definition->firing_position_count;
            for (; firing_position < end; ++firing_position) {
                if (firing_position >= 0 && firing_position < zone->firing_positions.count) {
                    real_point3d position;
                    ::ai_point_get_position(
                        (::c_ai_point3d const *)&((firing_position_definition *)zone->firing_positions.address)[firing_position],
                        &position);
                    float dx = m_leader_position.x - position.x;
                    dy = m_leader_position.y - position.y;
                    float distance_squared = dy * dy + dx * dx;
                    if (minimum > distance_squared)
                        minimum = distance_squared;
                    if (maximum <= distance_squared)
                        maximum = distance_squared;
                    if (type == 2) {
                        if (leader_minimum > distance_squared)
                            leader_minimum = distance_squared;
                        if (leader_maximum <= distance_squared)
                            leader_maximum = distance_squared;
                        found_leader = true;
                    }
                    found = true;
                }
            }
        }
        if (found) {
            set_constraints(sqrtf(minimum), sqrtf(maximum));
            if (found_leader)
                set_leader_constraints(sqrtf(leader_minimum), sqrtf(leader_maximum));
        }
    }
    return m_target_valid;
}
