#include "memory/tls_globals.h"
#include "ai/ai_player_tls.h"
// ailibxbox/actor_perception.obj: from the records 0xae250 0xae780 0xaefb8 0xafed8 0xaff38 0xb1168 0xb17b8 0xb2590 0xb3b40 0xb3c10 0xb4c98 0xb4f08 0xb50a0 0xb51e8 0xc80f0 0xd1c58 0xfb3f8 0x11bb78 0x11e268 0x11fa70 0x11fb90 0x562860 0x562880 0x562b10 0x562d28 0x562df0 0x5630a0 0x563240 0x5637a0 0x563808 0x5638e8 0x563c90 0x563f78 0x564098 0x564120 0x5641c8 0x5642a8
// @flags /TP /O2 /Oc
#include "ai/squad_data.h"
#include "ai/ai_player_types.h"
#include "ai/character_weapon_properties.h"
#include "ai/actor_type_flood_types.h"
#include "ai/behavior_stimulus_types.h"
#include "ai/clump_importance_types.h"
#include "memory/data_types.h"
#include "math/real_math_types.h"
#include "tag_files/tag_types.h"
#include "game/game_types.h"
#include "objects/object_types.h"
#include "ai/ai_point3d.h"
#include "ai/ai_direction.h"
#include "objects/object_reference_types.h"
#include "objects/object_datum.h"
#include "objects/object_placement.h"
#include "ai/squad_types.h"
#include "cseries/cseries_wrapped.h"
#include "ai/actor_datum.h"
#include "ai/character_properties_types.h"
#include "ai/clump_datum.h"
#include "memory/tls_data_arrays.h"
#include "ai/ai_information_data.h"
#include "ai/sector_ref.h"
#include "cseries/cseries.h"
// @address 0x563f78 ?calculate_awareness@@YAXPAUprop_ref_datum@@PAUprop_view@@@Z
// @address 0xb17b8 ?actor_audibility_at_point@@YAFJPAUactor_position_data@@PBTreal_point3d@@PBUs_location@@FMF@Z
// @address 0x562b10 ?actor_perception_assess_danger_zone_danger@@YAXJJ@Z
// @address 0xfb3f8 actor_perception_initialize_audibility_table
// @address 0xafed8 ?actor_get_perception_mode@@YA?AW4e_perception_mode@@J@Z
// @address 0xb3b40 ?facing_category@@YAFPATreal_vector3d@@0M@Z
// @address 0x11bb78 ?target_get_prop_class@@YAFPAUactor_datum@@@Z
// @address 0x562860 ?perception_qsort_compare_tracks@@YAHPBX0@Z
// @address 0x5637a0 ?actor_perception_forget_recent_damage@@YAXJ@Z
// @address 0x563c90 ?calculate_prop_class@@YAFJW4e_ai_posture@@PAUprop_ref_datum@@@Z
// @address 0x564098 ?actor_is_majorly_scary@@YA_NJ@Z
// @address 0x564120 ?unit_has_scary_weapon@@YA_NJ@Z
// @address 0x5641c8 ?calculate_scariness@@YAXJPBUprop_ref_datum@@PBUprop_datum@@PAUprop_view@@@Z
// @address 0x5642a8 ?get_awareness_delta@@YAMJJF@Z
// @address 0xaff38 ?actor_perception_find_target_data@@YAXJPAUactor_position_data@@@Z
// @address 0x5630a0 ?actor_perception_assess_suicide_danger@@YA_NJJ@Z
// @address 0xb1168 ?actor_visibility_at_point@@YAFJPAUactor_position_data@@PBTreal_point3d@@1F_N2PA_N@Z
// @address 0x11fa70 ?actor_visibility_test_frustum@@YAXJPATreal_vector3d@@000MMPAM1@Z
// @address 0xd1c58 ?actor_perception_find_sense_position@@YA_NJPBTreal_point3d@@PAUactor_position_data@@@Z
// @address 0xb3c10 ?actor_compute_prop_salience@@YAXJJW4e_prop_stimulus@@@Z
#include <stdlib.h>
// match: 0xb3c10: _rotl intrinsic pragma selects the rotate instruction
#pragma intrinsic(_rotl)
#include <float.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include "game/game_time_types.h"
#include "motor/motor_animation.h"
#include "ai/actor_perception.h"
#include "ai/actor_stimulus.h"
#include "ai/actors.h"
#include "ai/ai_script.h"
#include "ai/behavior.h"
#include "ai/objectives.h"
#include "ai/prop_dialogue.h"
#include "ai/props.h"
#include "ai/swarms.h"
#include "items/equipment.h"
#include "objects/objects.h"
#include "scenario/scenario.h"
#include "scenario/scenario_tag.h"
#include "units/unit_animation.h"
#include "units/units.h"
#include "ai/character_definitions.h"
#include "ai/character_perception.h"
#include "items/weapon_definitions.h"
#include "ai/pathfinding_utilities.h"
#include "ai/styles.h"
#include "memory/data.h"
#include "ai/ai.h"
#include "game/game_allegiance.h"
#include "math/matrix_math_types.h"
#include "ai/prop_state.h"
#include "ai/prop_datum.h"
#include "ai/prop_ref_datum.h"
#include "ai/actor_prop_ref_iterator.h"
#include "ai/objective_definitions.h"
#include "units/unit_types.h"
#include "units/unit_definitions.h"
#include "units/vehicle_definitions.h"
#include "units/biped_definitions.h"
#include "units/biped_types.h"
#include "animations/animation_manager.h"
#include "items/equipment_definitions.h"
#include "game/game_globals.h"
#include "ai/ai_globals_definition.h"
#include "units/unit_seat_source.h"
#include "ai/graph_types.h"
#include "ai/s12_types.h"
#include "ai/perception_mode_types.h"
#include "ai/prop_stimulus_types.h"
// match: noalias declaration preserves caller live registers
#include "math/real_math_functions.h"



enum e_actor_flags { _actor_flag_threatening_target = 0, _actor_flag_2 = 2 };
#include "ai/prop_ref_flags.h"

#include "ai/prop_view.h"
enum e_ai_posture { _ai_posture_none = 0 };


#include "scenario/scenario_zone_set_audibility.h"
#include "scenario/scenario_definitions.h"
#include "ai/event_handling_sound_types.h"

struct perception_track {
    long prop_ref_index;
    float score;
};


// 0xb17b8
#include "ai/actor_perception_runtime.h"



#include "math/real_math_globals.h"
#include "ai/behavior_posture_table.h"
#include "ai/actor_perception_constants.h"
#include "math/real_literal_pool.h"
#include "items/projectile_definitions.h"
#include "models/model_damage_info.h"
#include "math/real_math_constants.h"
#include "math/real_math.h"
#include "ai/command_script_types.h"
#include "ai/command_script_data.h"
namespace actor_perception_salience_squads {
#include "ai/squad_hierarchy.h"
}
#include "ai/prop_type_info.h"
#include "ai/actor_posture.h"
#include "ai/actor_queries.h"
#include "items/projectile_types.h"
#include "memory/data_typed_access.h"
#include "memory/datum_index.h"
#include "game/game_time_inline.h"
#include "game/game_time_inlines.h"
#include "math/random_math_range.h"
#include "units/unit_seat_helpers.h"
#include "objects/object_header_access.h"
#include "objects/object_header.h"
#include "motor/motor_animation_state.h"
#include "ai/prop_view_inline.h"
#include "math/real_math.h"
#include "tag_files/tag_groups.h"
e_perception_mode actor_get_perception_mode(long actor_index)
{
  actor_datum *actor = datum_get_typed<actor_datum>(actor_data, actor_index);

  if (actor->root_vehicle_index != -1) {
    return _perception_mode_vehicle;
  }
  short state = actor->combat_status;
  if (state <= 1) {
    return _perception_mode_idle;
  }
  if (state >= 4) {
    return _perception_mode_focused;
  }
  return _perception_mode_alert;
}


static __forceinline float actor_perception_dot_product3d_any(const float *a, const float *b)
{
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

static __forceinline bool actor_perception_vector_plane_intersection(
    const real_point3d *point, const real_vector3d *vector, const real_plane3d *plane, float *t)
{
    bool result = true;
    float dot = actor_perception_dot_product3d_any(plane->normal.n, vector->n);
    if (fabs(dot) > 0.01)
        *t = -(dot_product3d(point, &plane->n) - plane->d) / dot;
    else
        result = false;
    return result;
}





const float k_actor_perception_close_distance = 6.0f;
const float k_actor_perception_idle_close_distance = 3.0f;

void actor_perception_find_target_data(long object_index, struct actor_position_data * position)
{
    object_datum *object = object_get(object_index);
    long root_index = inl_object_get_ultimate_parent(object_index);
    object_datum *root = object_get(root_index);
    object_get_origin(object_index, &position->body);
    object_get_velocities(root_index, &position->velocity, 0);
    position->location = root->location;
    if ((1 << object->object_identifier.get_type()) & 0x1003) {
        unit_get_head_position(object_index, &position->head);
    } else if (object->object_identifier.get_type() == 11) {
        object_marker marker;
        if (internal_object_get_markers_by_string_id(object_index, 157, &marker, 1, false) > 0)
            position->head = marker.matrix.position;
        else
            position->head = object_get(object_index)->bounding_center;
    } else {
        position->head = position->body;
    }
}

// 0xb1168
short actor_visibility_at_point(long actor_index, actor_position_data *position, const real_point3d *point, const real_point3d *next_point,
    short visibility_type, bool use_frustum, bool debug, bool *out_of_range)
{
    short result = 0;
    if (visibility_type == 0 || visibility_type == 1)
    {
        actor_datum *actor = datum_get_typed<actor_datum>(actor_data, actor_index);
        s_mode_entry *properties = (s_mode_entry *)actor_perception_properties_get(actor_index);
        if (properties)
        {
            float maximum_distance = properties->maximum_distance;
            real_vector3d vector;
            vector_from_points3d(&position->head, point, &vector);
            if (next_point && !actor->swarm)
            {
                real_vector3d line;
                vector_from_points3d(point, next_point, &line);
                if (magnitude_squared3d(&line) > 0.0025000002f)
                {
                    real_plane3d plane;
                    float t;
                    plane3d_from_point_and_normal(&plane, &position->head, &actor->up_vector);
                    if (actor_perception_vector_plane_intersection(point, &line, &plane, &t))
                    {
                        t = PIN(t, 0.0f, 1.0f);
                        if (t > 0.0f)
                        {
                            real_point3d closest_point;
                            point_from_line3d(point, &line, t, &closest_point);
                            vector_from_points3d(&position->head, &closest_point, &vector);
                        }
                    }
                }
            }
            float distance_squared = magnitude_squared3d(&vector);
            if (distance_squared < maximum_distance * maximum_distance)
            {
                float scale = 1.0f;
                scale = MAX(scale, 0.15f);
                if (distance_squared < maximum_distance * scale * (maximum_distance * scale))
                {
                    float peripheral_distance;
                    float central_distance;
                    if (!actor->swarm && use_frustum)
                    {
                        actor_visibility_test_frustum(actor_index, &vector, &actor->looking_vector, &actor->left_vector, &actor->up_vector,
                            maximum_distance, scale, &peripheral_distance, &central_distance);
                        if (actor->root_vehicle_index != -1)
                        {
                            unit_seat_source sources[64];
                            short source_count = unit_get_all_seats(actor->root_vehicle_index, sources, 64);
                            for (short source_index = 0; source_index < source_count; source_index++)
                            {
                                long occupant_index = ::unit_seat_get_occupant(sources[source_index].unit_index, sources[source_index].seat_index);
                                if (occupant_index != -1)
                                {
                                    unit_datum *unit =
                                        (unit_datum *)object_get_and_verify_type(occupant_index, 0x2003);
                                    if (unit->unit.player_index != -1)
                                    {
                                        real_vector3d left;
                                        real_vector3d up;
                                        float seat_peripheral_distance;
                                        float seat_central_distance;
                                        cross_product3d(global_up3d, &unit->unit.looking_vector, &left);
                                        ::normalize3d(&left);
                                        cross_product3d(&unit->unit.looking_vector, &left, &up);
                                        actor_visibility_test_frustum(actor_index, &vector, &unit->unit.looking_vector, &left, &up,
                                            maximum_distance, scale, &seat_peripheral_distance, &seat_central_distance);
                                        if (seat_peripheral_distance > peripheral_distance)
                                            peripheral_distance = seat_peripheral_distance;
                                        if (seat_central_distance > central_distance)
                                            central_distance = seat_central_distance;
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        central_distance = maximum_distance * scale;
                        peripheral_distance = 0.7f * central_distance;
                    }
                    if (visibility_type == 0 && distance_squared < peripheral_distance * peripheral_distance)
                    {
                        float close_distance = actor_get_perception_mode(actor_index) == 3 ? k_actor_perception_idle_close_distance
                                                                                           : k_actor_perception_close_distance;
                        result = distance_squared < close_distance * close_distance ? 3 : 2;
                    }
                    else if (distance_squared < central_distance * central_distance)
                    {
                        result = 1;
                    }
                }
                else if (out_of_range)
                {
                    *out_of_range = true;
                }
            }
            else if (out_of_range)
            {
                *out_of_range = true;
            }
        }
    }
    return result;
}


short facing_category(real_vector3d *a, real_vector3d *b, float scale)
{
    short result = 4;
    float dot = -dot_product3d(a, b);
    float distance;
    if (dot <= 0.0f)
        distance = FLT_MAX;
    else if (dot >= 1.0f)
        distance = 0.0f;
    else {
        float sine = sqrtf(1.0f - dot * dot);
        distance = sine * scale;
    }
    if (dot > 0.9925f || distance < 0.5f)
        result = 0;
    else if (dot > 0.9063f || distance < 1.5f)
        result = 1;
    else if (dot > 0.5f)
        result = 2;
    else if (dot > 0.0f)
        result = 3;
    else
        result = 4;
    return result;
}

extern "C" {
// @address 0x239e0 k_behavior_posture_info
}

static __forceinline prop_state *actor_perception_prop_state_get(const prop_ref_datum *prop_ref)
{
    if (prop_ref->view_index != NONE)
        return &datum_get_typed<tracking_datum>(TLS_PROP_DATA, prop_ref->view_index)->state;
    return &datum_get_typed<prop_datum>(prop_data, prop_ref->prop_index)->state;
}

static __forceinline bool actor_perception_ai_index_matches_actor(actor_datum *actors, long ai_index, long actor_index)
{
    bool matches = false;
    switch ((unsigned long)ai_index >> 29) {
    case 1: case 2: case 5: {
        actor_datum *actor = &actors[datum_index_to_absolute_index(actor_index)];
        if (actor->squad_index != -1)
            matches = ai_index_test_squad(actor->squad_index, ai_index);
        break;
    }
    case 3: case 4:
        matches = !(ai_index_get_actor(ai_index) - actor_index);
        break;
    }
    return matches;
}

// 0xb3c10
extern "C" {
// @address 0x18e8 k_actor_perception_defending_status_weight
const float k_actor_perception_defending_status_weight = 0.85f;
// @address 0x960 literal_0_66f
const float literal_0_66f = 0.66f;
}

static __forceinline long actor_perception_salience_seconds_to_ticks_round()
{
#if defined(_XBOX) && !defined(__clang__)
    return (long)((float)TLS_GAME_TIME_GLOBALS->ticks_per_second * ACTOR_PERCEPTION_REAL_CONSTANTS[-7082] + 0.5f);
#else
    return (long)((float)TLS_GAME_TIME_GLOBALS->ticks_per_second * 5.0f + 0.5f);
#endif
}
static __forceinline float actor_perception_salience_dot(const real_vector3d &a, real_vector3d b) { return a.i*b.i + a.j*b.j + a.k*b.k; }
void actor_compute_prop_salience(long actor_index, long prop_ref_index, e_prop_stimulus stimulus)
{
    actor_datum *actors = (actor_datum *)actor_data->data;
    actor_datum *actor = &actors[datum_index_to_absolute_index(actor_index)];
    prop_ref_datum *prop_ref = datum_get_typed<prop_ref_datum>(prop_ref_data, prop_ref_index);
    prop_datum *prop = datum_get_typed<prop_datum>(prop_data, prop_ref->prop_index);
    stimulus = _prop_stimulus_none;
    prop_state *state = actor_perception_prop_state_get(prop_ref);
    prop_view *view = prop_view_get(prop_ref);
    stimulus = MAX(stimulus, (e_prop_stimulus)(signed char)prop_ref->stimulus_type);
    float salience;
    unsigned char salient;
    if (!prop->dead_2e && prop_ref->awareness_class > 0) {
        object_header_datum *headers = (object_header_datum *)object_header_data->data;
#if defined(_XBOX) && !defined(__clang__)
        const float *constants = ACTOR_PERCEPTION_REAL_CONSTANTS;
#endif
        float zero = 0.0f;
        float half = 0.5f;
        float four = 0.0f, two = 0.0f;
        float status_weight = zero;
        float range_weight = zero;
        float target_bonus = zero;
        float preference_bonus = zero;
        float damage_bonus = zero;
        bool self = false;
        long object_index = prop_ref->object_index;
        object_datum *object = headers[datum_index_to_absolute_index(object_index)].datum;
        short object_type = object->object_identifier.get_type();
        if (object_type == 0) {
            long unit_index = actor->unit_index;
            if (unit_index != -1)
                self = object_accepted_local_physics_object_get(object_index) == unit_index;
        }
        salient = prop->active && !self && TEST_BIT(66, prop_ref->type)
            && (!state->inactive || (state->inactive_time != -1
                && game_time_get() - state->inactive_time < actor_perception_salience_seconds_to_ticks_round()))
            && !(state->inactive && TEST_BIT(3, object->object_identifier.m_source));
        if (salient && state->posture != 0 && k_behavior_posture_info[state->posture].unaware)
            salient = false;
        if (salient && !actor->friendly_player_in_vehicle) {
            if (prop->owner_index != -1) {
                actor_datum *owner = &actors[datum_index_to_absolute_index(prop->owner_index)];
                if (actor->encounter_index != owner->encounter_index)
                    salient = false;
            } else if (actor->encounter_index != 0xffff) {
                salient = actor->unowned_target_salient;
            }
        }
#if defined(_XBOX) && !defined(__clang__)
        four = constants[-986];
        two = constants[-7053];
#else
        four = 4.0f;
        two = 2.0f;
#endif
        float one = 1.0f;
        if (salient) {
            if (actor->swarm) {
                range_weight = zero;
            } else if (actor->vehicle_style_disabled) {
                if (actor->root_vehicle_index == state->parent) {
                    unit_datum *unit = (unit_datum *)headers[datum_index_to_absolute_index(actor->unit_index)].datum;
                    range_weight = half;
                    if (unit->object.parent_object_index != -1 && unit->unit.parent_seat_index != -1) {
                        unit_datum *parent = (unit_datum *)headers[datum_index_to_absolute_index(unit->object.parent_object_index)].datum;
                        unit_definition *definition =
                            (unit_definition *)global_tag_instances[datum_index_to_absolute_index(parent->object.definition_index)].definition;
                        unit_seat *seat = (unit_seat *)tag_block_get_element_with_size(
                            &definition->seats, unit->unit.parent_seat_index, sizeof(unit_seat));
                        long occupant = ::unit_seat_get_occupant(unit->object.parent_object_index, seat->index);
                        if (occupant == object_index)
                            range_weight = one;
                    }
                } else {
                    range_weight = zero;
                }
            } else if (!actor_has_ranged_weapon(actor_index)) {
                float charge_distance = two;
                character_charge_properties *charge = character_definition_charge_multi_properties_get(actor_index);
                if (charge && charge->melee_range > charge_distance)
                    charge_distance = charge->melee_range;
                if (state->parent != -1)
                    range_weight = zero;
                else if (pref_unreachable(prop_ref_index))
                    range_weight = zero;
                else if (prop_ref->distance < charge_distance &&
                    ((prop_ref->awareness_class >= 1 && prop_ref->awareness_class <= 2) || !view || !TEST_BIT(view->flags, 6)))
                    range_weight = one;
            }
            range_weight *= four;
        }
        if (salient) {
            if (state->inactive) {
#if defined(_XBOX) && !defined(__clang__)
                status_weight = constants[0];
#else
                status_weight = 0.15f;
#endif
            } else {
                unsigned long dead = actor->swarm;
                if (!dead && stimulus == 3 && !actor_is_giant(actor))
                status_weight = one;
            else if (stimulus == 2 && !actor_is_giant(actor))
                status_weight = one;
            else if (prop_ref->awareness_class >= 1) {
                if (dead) {
                    status_weight = 0.3f;
                } else if (prop_ref->awareness_class <= 2) {
                    float close_weight = zero;
                    if (prop_ref->distance < four) {
                        float distance = prop_ref->distance * 0.25f;
                        close_weight = one - distance * distance;
                    }
                    if (stimulus == 1)
                    {
                        if (view && view->category <= 1)
                            status_weight = k_actor_perception_defending_status_weight;
                        else
                            status_weight = 0.65f;
                    }
                    else
                        status_weight = 0.6f;
                    if (status_weight <= close_weight)
                        status_weight = close_weight;
                } else if (prop_ref->awareness_class >= 3) {
                    if (view && !TEST_BIT(view->flags, 6))
                        status_weight = 0.3f;
                    else
#if defined(_XBOX) && !defined(__clang__)
                        status_weight = constants[0];
#else
                        status_weight = 0.15f;
#endif
                }
            }
            }
            status_weight *= 7.0f;
        }
        if (salient) {
            if (prop_ref_index == actor->target_prop_index && !state->inactive) {
                if ((bool)(_rotl(actor->target_flags, 30) & 1))
                    target_bonus = 0.1f;
                else if (prop_ref->awareness_class >= 1 && prop_ref->awareness_class <= 2)
                    target_bonus = one;
                else
                    target_bonus = half;
            }
            target_bonus *= two;
        }
        if (salient) {
            if (view) {
                damage_bonus = 0.1f * view->salience;
                damage_bonus = PIN(damage_bonus, zero, one);
            }
            damage_bonus *= two;
        }
        float distance;
        float distance_weight;
        if (actor_is_giant(actor)) {
            distance = prop_ref->distance;
            distance_weight = literal_0_66f / (one + 0.025f * distance);
            distance_weight = PIN(distance_weight, zero, one);
        } else {
            distance = prop_ref->distance;
            distance_weight = one / (one + 0.1f * distance);
            distance_weight = PIN(distance_weight, zero, one);
        }
#if defined(_XBOX) && !defined(__clang__)
        distance_weight *= constants[-4899];
#else
        distance_weight *= 8.0f;
#endif

        if (salient) {
            if (state->prefer_target ||
                (actor->scripted_target_index != -1 && prop->owner_index != -1 &&
                    actor_perception_ai_index_matches_actor(actors, actor->scripted_target_index, prop->owner_index)))
            {
                preference_bonus = one;
            } else if (prop->enemy) {
                preference_bonus = 0.05f;
            } else if (distance < 100.0f && actor->root_vehicle_index != -1) {
                real_vector3d direction;
                long occupant_index;
                unit_get_child_actor(actor->root_vehicle_index, &occupant_index, false);
                if (occupant_index != -1) {
                    unit_datum *occupant = (unit_datum *)headers[datum_index_to_absolute_index(occupant_index)].datum;
                    if (!game_team_is_enemy((e_game_team)actor->team, (e_game_team)occupant->unit.team)) {
                        if (view)
                            direction = view->direction;
                        else {
                            vector_from_points3d(&actor->position.body, &state->position, &direction);
                            normalize3d(&direction);
                        }
                        preference_bonus = actor_perception_salience_dot(occupant->unit.facing_vector, direction) < zero
                            ? zero
                            : actor_perception_salience_dot(occupant->unit.facing_vector, direction);
                        if (prop_ref->distance > 50.0f)
                            preference_bonus *= one - (prop_ref->distance - 50.0f) * 0.02f;
                        preference_bonus = PIN(preference_bonus, zero, one);
                    }
                }
            }
            if (actor->prefer_target_team != -1 && prop->team == actor->prefer_target_team)
                preference_bonus = MAX(preference_bonus, half);
#if defined(_XBOX) && !defined(__clang__)
            preference_bonus *= constants[-4899];
#else
            preference_bonus *= 8.0f;
#endif
        }
        salience = (distance_weight + damage_bonus + preference_bonus + target_bonus + range_weight + status_weight) * 0.032258064f;
        if (prop_ref->type != 1 && !prop_ref->maximum_awareness)
            salient = false;
    } else {
        salience = 0.0f;
        salient = false;
    }
    prop_ref->salience = salience;
    if (salient) prop_ref->m_flags.m_storage |= 1; else prop_ref->m_flags.m_storage &= ~1;
    prop_ref->m_flags.set(_prop_ref_flag_3, false);
    prop = datum_get_typed<prop_datum>(prop_data, prop_ref->prop_index);
    if (prop->maximum_salience < salience)
        prop->maximum_salience = salience;
}

// @address 0x563808 ?actor_perception_unreachable@@YAXJJ_N0@Z
void actor_perception_unreachable(long actor_index, long prop_ref_index, bool unreachable, bool update_salience)
{
    prop_ref_datum *prop_ref = datum_get_typed<prop_ref_datum>(prop_ref_data, prop_ref_index);
    long view_index = prop_ref->view_index;
    if (view_index != -1) {
        tracking_datum *view_datum = datum_get_typed<tracking_datum>(TLS_PROP_DATA, view_index);
        if (view_datum != 0) {
            prop_view *view = &view_datum->tracking_view;
            if (view != 0) {
            if (unreachable) {
                view->unreachable_time = TLS_GAME_TIME_GLOBALS->time;
                if (!actor_has_ranged_weapon(actor_index))
                    actor_perception_unassailable(actor_index, prop_ref_index, unreachable, false);
            } else {
                view->unreachable_time = -1;
            }
            if (update_salience)
                actor_compute_prop_salience(actor_index, prop_ref_index, _prop_stimulus_none);
            }
        }
    }
}


bool actor_perception_danger_zone_interesting(long actor_index, long object_index, float distance, short priority)
{
    actor_datum *actor = datum_get_typed<actor_datum>(actor_data, actor_index);
    bool result = false;
    if (actor->danger_zone.danger_type < priority) {
        result = true;
    } else if (actor->danger_zone.danger_type == priority && actor->danger_zone.object_index != object_index &&
               (distance < actor->danger_zone.distance || actor->danger_zone.forced)) {
        result = true;
    }
    return result;
}


// 0xfb3f8
extern "C" void actor_perception_initialize_audibility_table(audibility_table *table, long scenario_index, long zone_set_index)
{
    scenario_index = global_scenario_index;
    table->audibility = 0;
    actor_audibility_bsp *bsp = table->bsps;
    for (long bsp_left = 15; bsp_left >= 0; bsp_left--, bsp++) {
        bsp->values = 0;
        bsp->count = 0;
    }
    if (zone_set_index != -1) {
        scenario *scenario = (struct scenario *)tag_get('scnr', scenario_index);
        long audibility_index = ((s_scenario_zone_set *)scenario->zone_sets.address)[zone_set_index].audibility_index;
        if (audibility_index != -1) {
            table->audibility = &((scenario_zone_set_audibility *)scenario->zone_set_audibility.address)[audibility_index];
            for (long bsp_index = 0; bsp_index < scenario->structure_bsps.count; bsp_index++) {
                actor_audibility_bsp &bsp = table->bsps[bsp_index];
                scenario_zone_set_audibility *audibility = (scenario_zone_set_audibility *)table->audibility;
                scenario_audibility_room_bounds &entry =
                    ((scenario_audibility_room_bounds *)audibility->bsp_cluster_to_room_bounds_mappings.address)[bsp_index];
                long count = entry.room_index_count;
                short *values = &((short *)((scenario_zone_set_audibility *)table->audibility)->bsp_cluster_to_room_indices.address)[entry.first_room_index];
                bsp.values = values;
                bsp.count = count;
            }
        }
    }
}

// the class of the actor's current target prop (0 when it has none or the target has no view)
short target_get_prop_class(actor_datum *actor)
{
    return inl_target_get_prop_class(actor);
}






short actor_audibility_at_point(long actor_index, actor_position_data *position, const real_point3d *point, const s_location *location,
    short sound_type, float sound_distance, short flags)
{
    actor_datum *actor =
        datum_get_typed<actor_datum>(actor_data, actor_index);
    s_mode_entry *properties = (s_mode_entry *)actor_perception_properties_get(actor_index);
    actor_position_data *actor_position = (actor_position_data *)position;
    short result = 0;
    if (properties != 0 && sound_type != 0 && (unsigned char)actor_position->location.cluster_reference.bsp_index != 255 &&
        (unsigned char)location->cluster_reference.bsp_index != 255)
    {
        real_vector3d delta;
        vector_from_points3d(&actor_position->head, point, &delta);
        float distance_squared = magnitude_squared3d(&delta);
        float hearing_distance = properties->hearing_distance;
        if (!(dot_product3d(&actor_position->forward, &delta) >= 0.0f))
            hearing_distance *= 0.8f;
        if (actor->behavior_mode == 3)
            hearing_distance *= 0.7f;
        else if (actor->behavior_mode == 1)
            hearing_distance *= 0.4f;
        if (sound_type == 4) {
            hearing_distance *= 0.2f;
        } else {
            if (sound_type == 1)
                hearing_distance *= 0.45f;
            else if (sound_type == 3)
                hearing_distance *= 0.7f;
        }
        if (scenario_location_deafening(&actor_position->location) || scenario_location_deafening(location))
            hearing_distance *= 0.25f;
        if (flags != 0 && flags != 1)
            hearing_distance *= 0.7f;
        if (!(distance_squared >= hearing_distance * hearing_distance)) {
            audibility_table table;
            actor_perception_initialize_audibility_table(&table, global_scenario_index, global_zone_set_index);
            if (!structure_get_cluster_ai_deafening(
                    (s_structure_cluster_mapping *)&table, actor_position->location.cluster_reference, location->cluster_reference)) {
                float cluster_distance = structure_audibility_get_distance(
                    (s_structure_cluster_mapping *)&table, actor_position->location.cluster_packed, location->cluster_packed);
                float audible_distance = cluster_distance * AI_AUDIBILITY_DISTANCE_SCALE;
                audible_distance = MAX(audible_distance, sqrtf(distance_squared));
                if (!(audible_distance >= hearing_distance)) {
                    result = (short)(sound_type >= 3 ? 3 : 2);
                }
            }
        }
    }
    return result;
}

// @address 0x11fb90 ?actor_get_vision_distances@@YAXJMMMPAM0@Z
void actor_get_vision_distances(long actor_index, float distance, float scale, float angle, float *peripheral_out, float *central_out)
{
    s_mode_entry *properties = (s_mode_entry *)actor_perception_properties_get((int)actor_index);
    float peripheral = 0.0f;
    float central = 0.0f;
    if (properties)
    {
        if (angle > properties->maximum_angle)
        {
            central = 0.0f;
            peripheral = central;
        }
        else
        {
            float near_distance = scale * distance;
            float far_distance = properties->maximum_vision_distance * scale;
            float peripheral_near = 0.7f * near_distance;
            float peripheral_far = 0.7f * far_distance;
            peripheral_far = peripheral_far > 3.5f ? 3.5f : peripheral_far;
            if (angle > properties->far_angle)
            {
                central = far_distance;
                peripheral = peripheral_far;
            }
            else
            {
                float near_angle = properties->near_angle;
                float peripheral_angle = near_angle * 0.8f;
                if (angle < near_angle)
                    central = near_distance;
                else
                {
                    float delta = properties->far_angle - near_angle;
                    if (delta <= 0.0f)
                        central = far_distance;
                    else
                    {
                        float t = (angle - near_angle) / delta;
                        central = near_distance * (1.0f - t) + far_distance * t;
                    }
                }
                if (angle < peripheral_angle)
                    peripheral = peripheral_near;
                else
                {
                    float delta = properties->far_angle - peripheral_angle;
                    if (delta <= 0.0f)
                        peripheral = peripheral_far;
                    else
                    {
                        float t = (angle - peripheral_angle) / delta;
                        peripheral = peripheral_near * (1.0f - t) + peripheral_far * t;
                    }
                }
            }
        }
    }
    *central_out = central;
    *peripheral_out = peripheral;
}

void actor_visibility_test_frustum(long actor_index, real_vector3d *direction, real_vector3d *right, real_vector3d *up,
    real_vector3d *target, float distance, float scale, float *peripheral_out, float *central_out)
{
    float x = right->i * direction->i + (right->j * direction->j + right->k * direction->k);
    float y = up->i * direction->i + (up->j * direction->j + up->k * direction->k);
    float z = target->i * direction->i + (target->j * direction->j + target->k * direction->k);
    float elevation = (float)atan2(z, sqrtf(y * y + x * x));
    if (elevation <= 0.5235988f && elevation >= -0.7853982f) {
        float azimuth = (float)atan2(y, x);
        actor_get_vision_distances(actor_index, distance, scale, (float)fabs(azimuth), peripheral_out, central_out);
    } else {
        *peripheral_out = 0.0f;
        *central_out = 0.0f;
    }
}

// 0x562860
int perception_qsort_compare_tracks(const void *a, const void *b)
{
    const perception_track *first = (const perception_track *)a;
    const perception_track *second = (const perception_track *)b;
    if (first->score >= second->score)
        return -1;
    return 1;
}

// 0x562b10
static __forceinline int actor_perception_dz_object_get_type(long object_index)
{
    // match: 0x562b10: zero-bit rotate keeps the signed compare form
    return _rotl(object_header_get(object_index)->object_type, 0) & 0xff;
}
static __forceinline s_equipment_definition *actor_perception_dz_equipment_definition_get(unsigned short definition_index)
{
    return (s_equipment_definition *)((void *)global_tag_instances[definition_index].definition);
}
static __forceinline const float &actor_perception_vehicle_margin_reference();
static __forceinline const float &actor_perception_danger_zone_zero_reference();

static __forceinline void actor_perception_dz_get_owner(long object_index, s_damage_owner *owner)
{
    object_header_datum *header = &((object_header_datum *)object_header_data->data)[(unsigned short)object_index];
    object_datum *object = header->datum;
    *owner = object->damage_owner;
}
void actor_perception_assess_danger_zone_danger(long actor_index, long prop_ref_index)
{
    actor_datum *actor = (actor_datum *)(actor_data->data + DATUM_INDEX_TO_ABSOLUTE_INDEX(actor_index) * sizeof(actor_datum));
    prop_ref_datum *prop_ref =
        (prop_ref_datum *)(prop_ref_data->data + DATUM_INDEX_TO_ABSOLUTE_INDEX(prop_ref_index) * sizeof(prop_ref_datum));
    object_header_datum *header = &((object_header_datum *)object_header_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(prop_ref->object_index)];
    object_datum *object = header->datum;
    float radius;
    if (actor_perception_dz_object_get_type(prop_ref->object_index) != 3) {
        radius = ((projectile_definition *)tag_get('proj', object->definition_index))->ai_radius;
    } else {
        s_equipment_definition *definition = actor_perception_dz_equipment_definition_get((unsigned short)object->definition_index);
        radius = definition->ai_radius;
        if (definition->equipment_types[4].count != 0) {
            s_damage_owner info;
            actor_perception_dz_get_owner(prop_ref->object_index, &info);
            if (!game_team_is_enemy((e_game_team)(short)actor->team, (e_game_team)info.team))
                return;
        }
    }
    if (radius > actor_perception_danger_zone_zero_reference() && prop_ref->distance < radius + actor_perception_vehicle_margin_reference() &&
        actor_perception_danger_zone_interesting(actor_index, prop_ref->object_index, prop_ref->distance, 3))
    {
        long parent_index = -1;
        memset(&actor->danger_zone.danger_type, 0, sizeof(actor->danger_zone));
        actor->danger_zone.danger_type = 3;
        actor->danger_zone.object_index = prop_ref->object_index;
        actor->danger_zone.radius = radius;
        actor->danger_zone.state = 0;
        actor->danger_zone.pref_index = prop_ref_index;
        if (object->damage_owner.object_index != -1) {
            unit_datum *parent = (unit_datum *)object_try_and_get_and_verify_type(object->damage_owner.object_index, -1);
            if (parent != 0 && ((1 << parent->object.object_type) & 0x1003) != 0) {
                parent_index = object->damage_owner.object_index;
                if (actor->unit_index != -1 && parent_index == actor->unit_index)
                    actor->danger_zone.state = 2;
                else if (!game_team_is_enemy((e_game_team)(short)actor->team, (e_game_team)parent->unit.team))
                    actor->danger_zone.state = 1;
            }
        }
        actor->danger_zone.parent_index = parent_index;
        actor_stimulus_switched_danger_zone(actor_index, prop_ref->object_index);
    }
}

// the object's first marker with a nonzero 12-bit index
// @address 0x562d28 ?destroyed_vehicle_is_dangerous@@YA_NJPAF@Z
bool destroyed_vehicle_is_dangerous(long object_index, short *out)
{
    bool found = false;
    short value = -1;
    object_datum *object = (*datum_get_typed<object_header_datum>(object_header_data, object_index)).datum;
    bool has_markers = (object->damage_flags >> 2) & 1;
    if (has_markers) {
        value = 0x7fff;
        int count = (unsigned int)object->damage_sections.size >> 3;
        s_damage_object_section_state *markers = (s_damage_object_section_state *)((char *)object + object->damage_sections.offset);
        for (short i = 0; i < count; i++) {
            s_damage_object_section_state *marker = &markers[i];
            if (marker->delayed_response_index != 0) {
                if (marker->delayed_response_index < 0x7fff) {
                    found = true;
                    value = marker->delayed_response_index;
                }
                break;
            }
        }
    }
    if (out) {
        if (found)
            *out = value;
        else
            *out = -1;
    }
    return found;
}

static __forceinline bool actor_perception_vehicle_flag(unsigned long flags, long bit) { return (flags >> bit) & 1; }
static __forceinline float actor_perception_vehicle_magnitude_squared(real_vector3d const *v) { return v->i*v->i + (v->j*v->j + v->k*v->k); }
static const float actor_perception_vehicle_wreck_radius_value = 3.5f;
static const float actor_perception_vehicle_speed_squared = 1.0f;
// @rdata 0x82022850
static const float actor_perception_vehicle_margin_value = 10.0f;
// @address 0x562df0 ?actor_perception_assess_vehicle_danger@@YA_NJJ@Z
bool actor_perception_assess_vehicle_danger(long actor_index, long prop_ref_index)
{
    actor_datum *actor = datum_get_typed<actor_datum>(actor_data, actor_index);
    prop_ref_datum *reference =
        datum_get_typed<prop_ref_datum>(prop_ref_data, prop_ref_index);
    unit_datum *vehicle = (unit_datum *)(*datum_get_typed<object_header_datum>(object_header_data, reference->object_index)).datum;
    vehicle_definition *definition =
            (vehicle_definition *)((void *)global_tag_instances[datum_index_to_absolute_index(vehicle->object.definition_index)].definition);
    float wreck_radius = actor_perception_vehicle_wreck_radius_value;
    long own_vehicle = ((actor_datum *)actor)->root_vehicle_index;
    bool result = false;
    bool larger = false;
    if (own_vehicle == -1) {
        larger = true;
    } else {
        unit_datum *own = (unit_datum *)(*datum_get_typed<object_header_datum>(object_header_data, own_vehicle)).datum;
        vehicle_definition *own_definition =
            (vehicle_definition *)((void *)global_tag_instances[datum_index_to_absolute_index(own->object.definition_index)].definition);
        larger = own_definition->havok_vehicle_physics.size < definition->havok_vehicle_physics.size;
    }
    if (larger) {
        real_vector3d velocity;
        object_get_velocities(reference->object_index, &velocity, 0);
        if (!actor_perception_vehicle_flag(definition->flags, 10) &&
            actor_perception_vehicle_magnitude_squared(&velocity) > actor_perception_vehicle_speed_squared &&
            reference->distance < definition->bounding_radius + actor_perception_vehicle_margin_reference() &&
            actor_perception_danger_zone_interesting(actor_index, reference->object_index, reference->distance, 4))
        {
            memset(&actor->danger_zone.danger_type, 0, sizeof(actor->danger_zone));
            actor->danger_zone.danger_type = 4;
            actor->danger_zone.object_index = reference->object_index;
            actor->danger_zone.parent_index = vehicle->unit.driver_object_index;
            actor->danger_zone.radius = definition->bounding_radius;
            actor->danger_zone.state = 0;
            actor->danger_zone.pref_index = prop_ref_index;
            if (actor->danger_zone.parent_index != -1) {
                unit_datum *driver = (unit_datum *)(*datum_get_typed<object_header_datum>(object_header_data, actor->danger_zone.parent_index)).datum;
                if (!game_team_is_enemy((e_game_team)(short)actor->team, (e_game_team)driver->unit.team)) actor->danger_zone.state = 1;
            }
            actor_stimulus_switched_danger_zone(actor_index, reference->object_index);
            result = true;
        }
    }
    if (!result && vehicle->unit.driver_object_index == -1 && reference->distance < wreck_radius + actor_perception_vehicle_margin_reference() &&
        actor_perception_danger_zone_interesting(actor_index, reference->object_index, reference->distance, 3))
    {
        result = destroyed_vehicle_is_dangerous(reference->object_index, 0);
        if (result) {
            memset(&actor->danger_zone.danger_type, 0, sizeof(actor->danger_zone));
            actor->danger_zone.danger_type = 3;
            actor->danger_zone.object_index = reference->object_index;
            actor->danger_zone.radius = wreck_radius;
            actor->danger_zone.state = 0;
            actor->danger_zone.pref_index = prop_ref_index;
            actor->danger_zone.parent_index = -1;
            actor_stimulus_switched_danger_zone(actor_index, reference->object_index);
        }
    }
    return result;
}

// record verbatim, decls renamed on clash
__forceinline actor_datum *actor_perception_suicide_actor_get(long index)
{
    return datum_get_typed<actor_datum>(actor_data, index);
}
__forceinline prop_ref_datum *actor_perception_suicide_prop_ref_get(long index)
{
    return datum_get_typed<prop_ref_datum>(prop_ref_data, index);
}
__forceinline prop_datum *actor_perception_suicide_prop_get(long index)
{
    return datum_get_typed<prop_datum>(prop_data, index);
}
__forceinline unit_definition *actor_perception_suicide_definition_get(long index)
{
    return (unit_definition *)((void *)global_tag_instances[datum_index_to_absolute_index(index)].definition);
}

bool actor_perception_assess_suicide_danger(long actor_index, long prop_ref_index) {
 actor_datum *actor=actor_perception_suicide_actor_get(actor_index);
 prop_ref_datum *pref=actor_perception_suicide_prop_ref_get(prop_ref_index);
 float radius=1.5f;
 bool suicide=false;
 bool result=false;
 if(pref->awareness_class>=1 && radius>0.0f && pref->distance<radius+10.0f) {
  object_datum *unit=object_get(pref->object_index);
  unit_definition *definition=actor_perception_suicide_definition_get(unit->definition_index);
  if(motor_animation_get_state(pref->object_index)==80) suicide=(definition->flags>>1)&1;
  // match: 0x5630a0: fresh prop_ref lookups reload object_index for both calls; pref->object_index reuses the object_get index register
  else if(unit_animation_get_action_name(actor_perception_suicide_prop_ref_get(prop_ref_index)->object_index)==10) suicide=(definition->flags>>8)&1;
  if(suicide && actor_perception_danger_zone_interesting(actor_index,actor_perception_suicide_prop_ref_get(prop_ref_index)->object_index,pref->distance,1)) {
   prop_datum *prop=actor_perception_suicide_prop_get(pref->prop_index);
   memset(&actor->danger_zone, 0, sizeof(actor->danger_zone));
   actor->danger_zone.danger_type=1;
   actor->danger_zone.object_index=pref->object_index;
   actor->danger_zone.radius=1.5f;
   actor->danger_zone.state=!prop->active;
   actor->danger_zone.pref_index=-1;
   actor_stimulus_switched_danger_zone(actor_index,pref->object_index);
   result=true;
  }
 }
 return result;
}

void actor_perception_forget_recent_damage(long actor_index)
{
    actor_prop_ref_iterator iterator;
    prop_ref_datum *prop_ref;
    actor_prop_ref_iterator_new(&iterator, actor_index);
    while ((prop_ref = actor_prop_ref_iterator_next(&iterator)) != 0) {
        if (prop_ref->stimulus_type == 3) {
            prop_ref->stimulus_type = -1;
            prop_ref->stimulus_decay_ticks = 0;
        }
    }
}


// 0x563c90
short calculate_prop_class(long actor_index, e_ai_posture posture, prop_ref_datum *prop_ref)
{
    prop_view *view = prop_view_get(prop_ref);
    short prop_class = 0;
    if (prop_ref->awareness_class == 0) {
        prop_class = 0;
    } else if (prop_ref->type != 1 && prop_ref->type != 6) {
        prop_class = 0;
    } else if (prop_state_get(prop_ref)->inactive) {
        prop_class = 1;
    } else if (prop_ref->stimulus_type == 3) {
        prop_class = 13;
    } else if (prop_ref->awareness_class >= 1 && prop_ref->awareness_class <= 2) {
        bool unattended = false;
        if (posture > 0 && k_behavior_posture_info[posture].unaware) {
            prop_datum *prop = datum_get_typed<prop_datum>(prop_data, prop_ref->prop_index);
            if (prop->owner_index != -1) {
                actor_datum *actor = datum_get_typed<actor_datum>(actor_data, prop->owner_index);
                unattended = true;
                if (actor->target_prop_index != -1) {
                    prop_ref_datum *target_ref = datum_get_typed<prop_ref_datum>(prop_ref_data, actor->target_prop_index);
                    if ((datum_get_typed<prop_datum>(prop_data, target_ref->prop_index))->owner_index == actor_index)
                        unattended = false;
                }
            }
            if (unattended)
                prop_class = 2;
        }
        if (!unattended) {
            prop_class = 7;
            if (prop_ref->distance < 2.0f) {
                prop_class = 12;
            } else {
                if (prop_ref->distance < 12.0f)
                    prop_class = 8;
                if (view && view->category <= 2) {
                    prop_class = 9;
                    if (prop_state_get(prop_ref)->heard_shooting) {
                        prop_class = 10;
                        if (view->category <= 1)
                            prop_class = 11;
                    }
                }
            }
        }
    } else if (prop_ref->awareness_class >= 3) {
        if (view) {
            if ((view->flags & 16) != 0)
                prop_class = 3;
            else if ((view->flags & 64) != 0)
                prop_class = 4;
            else if ((float)(TLS_GAME_TIME_GLOBALS->time - view->tracking_index) * TLS_GAME_TIME_GLOBALS->tick_length < 1.0f && view->status >= 6)
                prop_class = 6;
            else
                prop_class = 5;
        } else {
            prop_class = 3;
        }
    }
    if (view)
        view->status = prop_class;
    return prop_class;
}

bool actor_is_majorly_scary(long actor_index)
{
  actor_datum *owner = datum_get_typed<actor_datum>(actor_data, actor_index);
  character_general_properties *definition = (character_general_properties *)character_definition_general_properties_get(owner->character_index);
  bool result = false;
  if (definition && global_game_globals->ai_globals.count() > 0)
    result = definition->scariness >= ((s_ai_globals_definition *)global_game_globals->ai_globals.address)->scariness_threshold;
  return result;
}


bool unit_has_scary_weapon(long unit_index)
{
    object_header_datum *headers = (object_header_datum *)object_header_data->data;
    long weapon_index = -1;
    unit_datum *unit = (unit_datum *)headers[datum_index_to_absolute_index(unit_index)].datum;
    if (unit->unit.current_weapon_index != -1)
        weapon_index = unit->unit.weapon_object_indices[unit->unit.current_weapon_index];
    bool result = false;
    if (weapon_index != -1 && global_game_globals->ai_globals.count() > 0) {
        object_datum *weapon = (object_datum *)headers[datum_index_to_absolute_index(weapon_index)].datum;
        result = ((weapon_definition *)global_tag_instances[datum_index_to_absolute_index(weapon->definition_index)].definition)->scariness >=
            ((s_ai_globals_definition *)global_game_globals->ai_globals.address)->weapon_scariness_threshold;
    }
    return result;
}



void calculate_scariness(long actor_index, const prop_ref_datum *prop_ref, const prop_datum *owner_prop, prop_view *view)
{
  const prop_datum *prop = datum_get_typed<prop_datum>(prop_data, prop_ref->prop_index);
  if (prop->active && prop_state_get(prop_ref)->inactive == false) {
    actor_datum *actor = datum_get_typed<actor_datum>(actor_data, actor_index);
    float scariness;
    if (owner_prop->owner_index != -1) {
      actor_datum *owner = datum_get_typed<actor_datum>(actor_data, owner_prop->owner_index);
      scariness = owner->inherent_scariness;
    } else {
      scariness = actor_get_inherent_scariness(prop_ref->object_index);
    }
    float delta = scariness - actor->inherent_scariness;
    if (delta < 0.0f)
      delta = 0.0f;
    view->salience = delta;
  } else {
    view->salience = 0.0f;
  }
}

// 0x5642a8
float get_awareness_delta(long actor_index, long prop_ref_index, short perception)
{
    float delta = 0.0f;
    s_mode_entry *properties = actor_perception_properties_get(actor_index);
    if (properties) {
        prop_ref_datum *prop_ref = datum_get_typed<prop_ref_datum>(prop_ref_data, prop_ref_index);
        switch (prop_ref->type - 1) {
        case 2: {
            delta = 1.0f;
            object_header_datum *header = datum_get_typed<object_header_datum>(object_header_data, prop_ref->object_index);
            if (header->object_type == 3) {
                object_datum *object = (object_datum *)header->datum;
                s_equipment_definition *definition =
                    (s_equipment_definition *)global_tag_instances[datum_index_to_absolute_index(object->definition_index)].definition;
                if (definition->activation_delay > 0.0f)
                    delta = TLS_GAME_TIME_GLOBALS->tick_length / definition->activation_delay;
            }
            break;
        }
        case 1:
        case 3:
        case 7:
            delta = 1.0f;
            break;
        case 0:
            if (prop_state_get(prop_ref)->parent != -1) {
                delta = 1.0f;
                break;
            }
        case 4:
        case 5:
        case 6: {
            e_perception_mode mode = actor_get_perception_mode(actor_index);
            long table[4][4] = {
                { 0, 1, 2, 3 },
                { 0, 1, 2, 3 },
                { 0, 2, 4, 4 },
                { 0, 0, 2, 4 }
            };
            if (properties->rate > 0.0f)
                delta = TLS_GAME_TIME_GLOBALS->tick_length / properties->rate;
            else
                delta = 1.0f;
            short level = (short)table[mode][perception];
            switch (level) {
            case 0:
                delta = 0.0f;
                break;
            case 1:
                delta = delta * 0.5f;
                break;
            case 3:
                delta = delta * 2.0f;
                break;
            case 4:
                delta = 1.0f;
                break;
            case 2:
            default:
                break;
            }
            delta = (delta > 0.0f ? delta : 0.0f) < 1.0f ? (delta > 0.0f ? delta : 0.0f) : 1.0f;
            break;
        }
        default:
            break;
        }
    }
    return delta;
}


void calculate_awareness(prop_ref_datum *ref, prop_view *view) {
 if ((*datum_get_typed<prop_datum>(prop_data, ref->prop_index)).active && !prop_state_get(ref)->inactive) {
  float one=AI_PERCEPTION_REAL_CONSTANTS[0];
  if(view->vitality > one) view->vitality=one;
  float awareness;
  if(ref->stimulus_type == 3) awareness=one;
  else if(ref->awareness >= 2) {
   if(view->category == 4) awareness=AI_PERCEPTION_REAL_CONSTANTS[200];
   else if(view->category == 3) awareness=AI_PERCEPTION_REAL_CONSTANTS[199];
   else if(view->category == 2) awareness=AI_PERCEPTION_REAL_CONSTANTS[198];
   else awareness=AI_PERCEPTION_REAL_CONSTANTS[197];
  } else awareness=AI_PERCEPTION_REAL_CONSTANTS[200];
  if(awareness >= view->vitality) view->vitality=awareness;
  else view->vitality=0.995f*view->vitality+0.004999995f*awareness;
 } else view->vitality=AI_PERCEPTION_REAL_CONSTANTS[200];
}


typedef real_vector2d real_vector2d_t;

static __forceinline biped_datum *actor_perception_blockage_object_get(long index)
{
    return (biped_datum *)(*datum_get_typed<object_header_datum>(object_header_data, index)).datum;
}
static __forceinline bool actor_perception_blockage_test_flag(unsigned long flags, long bit)
{
    return (flags >> bit) & 1;
}
static __forceinline float actor_perception_blockage_dot2d(real_vector2d horizontal, const real_vector3d &offset)
{
    return horizontal.i * offset.i + horizontal.j * offset.j;
}
// @rdata 0x82022514
static const float k_actor_perception_blockage_outer_margin = 0.6f;
// @rdata 0x82022518
static const float k_actor_perception_blockage_inner_margin = 0.3f;
// @rdata 0x820242d8
static const float k_actor_perception_blockage_crouch_scale = 2.0f;
static __forceinline float actor_perception_blockage_cone_threshold(float distance)
{
    return distance * 0.8660254f;
}
static __forceinline bool actor_perception_blockage_object_valid(long index)
{
    bool valid = false;
    if (index != -1) {
        object_header_datum *header = datum_get_typed<object_header_datum>(object_header_data, index);
        if (((1 << header->object_type) & 1) != 0) {
            biped_datum *object = (biped_datum *)header->datum;
            if (actor_perception_blockage_test_flag(object->object.physics_flags, 7)
                && (object->biped.physics.m_mode == 1 || object->biped.physics.m_mode == 4))
                valid = true;
        }
    }
    return valid;
}
// @address 0x11e268 actor_perception_compute_target_blockage
extern "C" short actor_perception_compute_target_blockage(const real_point3d *origin, const real_vector3d *aiming,
    const real_point3d *target, long object_index, real_vector3d *out_vector, float *out_distance)
{
    short result = 0;
    if (actor_perception_blockage_object_valid(object_index)) {
        if (out_distance)
            *out_distance = 0.0f;
        if (out_vector)
            *out_vector = *global_zero_vector3d;
        biped_datum *object = actor_perception_blockage_object_get(object_index);
        const biped_definition *definition =
            (const biped_definition *)tag_get('bipd', object->object.definition_index);
        float height = actor_perception_blockage_test_flag(definition->physics.flags, 1)
            ? definition->physics.radius * k_actor_perception_blockage_crouch_scale
            : definition->physics.height_standing;
        real_vector2d horizontal = aiming->xy;
        if (normalize2d(&horizontal) > 0.0f) {
            real_vector3d offset;
            vector_from_points3d(origin, target, &offset);
            float distance = magnitude2d(&offset.xy);
            if (out_distance)
                *out_distance = distance;
            float dot = actor_perception_blockage_dot2d(horizontal, offset);
            if (dot > actor_perception_blockage_cone_threshold(distance)) {
                real_vector3d projection;
                point_from_line3d(&offset, aiming, -dot, &projection);
                if (out_vector) {
                    out_vector->i = -projection.i;
                    out_vector->j = -projection.j;
                    out_vector->k = -projection.k;
                }
                if (projection.k > -0.3f && projection.k < height + k_actor_perception_blockage_inner_margin)
                    result = 2;
                else if (projection.k > -0.6f && projection.k < height + k_actor_perception_blockage_outer_margin)
                    result = 1;
                else
                    result = 0;
                if (result > 0) {
                    float distance_squared = projection.i * projection.i + projection.j * projection.j;
                    if (distance_squared >= 0.36f) {
                        if (distance_squared < 1.21f)
                            result = 1;
                        else
                            result = 0;
                    }
                }
            }
        }
    }
    return result;
}

extern "C" {
// @address 0x72d280 k_clump_state_combat_status
short k_clump_state_combat_status[] = { 0, 0, 2, 3 };
// @address 0x72f78c k_movement_combat_status_index
short k_movement_combat_status_index[] = { 0, 3, 0, 3 };
// @address 0x72c808 k_movement_combat_status_table
short k_movement_combat_status_table[] = { 0, 1, 2, 3 };
}

static __forceinline short actor_perception_ai_player_index_get(long player_index)
{
 short result = -1;
 for (long index = 0; index < 4; index++) {
  if (tls_ai_players()[index].player_index == player_index) { result = (short)index; break; }
 }
 return result;
}
static __forceinline short actor_perception_latched_combat_status(long actor_index)
{
 short status=global_style_status_mapping[0];
 style_definition *style=actor_get_task_style(actor_index);
 if(style) status=global_style_status_mapping[style->combat_status];
 return status;
}


// 0xd1c58 actor_perception_find_sense_position: actor sense-position fallback.
// The datum is the 0xa8c-byte actor array entry; a swarm-driven actor forwards
// to swarm_perception_find_sense_position, otherwise its stored 52-byte sense
// position (a full actor_position_data) is copied out with memcpy.
bool actor_perception_find_sense_position(long actor_index, const real_point3d *point, actor_position_data *out)
{
    actor_datum *actor = datum_get_typed<actor_datum>(actor_data, actor_index);
    if (actor->swarm && actor->swarm_index != -1)
        return swarm_perception_find_sense_position(actor->swarm_index, point, out);
    memcpy(out, &actor->position, sizeof(actor->position));
    return true;
}


// The AI-script unit uses the C++ declaration of the accepted C operation.
// Keep the linkage bridge transparent so both declarations share its body.

extern "C++" {
    bool squad_ancestor_of_squad_group(unsigned int squad_index, int group_index)
    {
        return actor_perception_salience_squads::squad_ancestor_of_squad_group(squad_index, group_index);
    }
}

// @address 0xb4f08 ?actor_perception_set_target@@YAXJJ@Z
void actor_perception_set_target(long actor_index, long prop_ref_index)
{
    actor_datum *actor = datum_get_typed<actor_datum>(actor_data, actor_index);
    long old_target_prop_index = actor->target_prop_index;
    actor->target_prop_index = prop_ref_index;
    if (old_target_prop_index != -1)
        actor_compute_prop_salience(actor_index, old_target_prop_index, _prop_stimulus_none);
    if (prop_ref_index != -1)
        actor_compute_prop_salience(actor_index, actor->target_prop_index, _prop_stimulus_none);
}

// @address 0xae510 ?actor_get_weapon@@YAJJ@Z
// @address 0xae5e0 ?actor_executing_layer@@YAFJF@Z
// @address 0xeba28 ?equipment_active_fraction@@YAMJ@Z
static __forceinline unit_datum *scariness_object_get(long index)
{
    return (unit_datum *)(*datum_get_typed<object_header_datum>(object_header_data, index)).datum;
}
static __forceinline void *scariness_tag_get(long index)
{
    return ((void *)global_tag_instances[datum_index_to_absolute_index(index)].definition);
}
static __forceinline long scariness_inventory_get_weapon(unit_datum *unit, signed char slot)
{
    long result = -1;
    if (slot != -1)
        result = unit->unit.weapon_object_indices[slot];
    return result;
}

// @address 0xae250 ?actor_get_inherent_scariness@@YAMJ@Z
float actor_get_inherent_scariness(long unit_index)
{
    float scariness = AI_PERCEPTION_REAL_CONSTANTS[200];
    if (unit_index != -1) {
        unit_datum *unit = scariness_object_get(unit_index);
        s_game_globals *game_globals = (s_game_globals *)global_game_globals;
        s_ai_globals_definition *ai_globals = 0;
        if (game_globals->ai_globals.count() > 0)
            ai_globals = (s_ai_globals_definition *)game_globals->ai_globals.address;
        if ((_rotl(unit->object.damage_flags, 30) & 1) == 0 && ((1 << unit->object.identifier.get_type()) & 0x1003) != 0) {
            long actor_index = unit->unit.actor_index;
            actor_datum *actor = 0;
            if (actor_index != -1)
                actor = (actor_datum *)(actor_data->data + (unsigned short)actor_index * sizeof(actor_datum));
            long weapon_index;
            if (actor) {
                character_general_properties *general = (character_general_properties *)character_definition_general_properties_get(actor->character_index);
                scariness = general->scariness;
                weapon_index = actor_get_weapon(actor_index);
            } else {
                weapon_index = scariness_inventory_get_weapon(unit, unit->unit.current_weapon_index);
                if (unit->unit.player_index != -1 && ai_globals)
                    scariness = ai_globals->player_scariness;
            }
            long secondary_weapon_index = scariness_inventory_get_weapon(unit, unit->unit.secondary_weapon_index);
            if (unit->object.parent_object_index != -1) {
                unit_datum *parent = scariness_object_get(unit->object.parent_object_index);
                if (parent->object.identifier.get_type() == 1) {
                    unit_definition *vehicle = (unit_definition *)scariness_tag_get(parent->object.definition_index);
                    short seat_index = unit->unit.parent_seat_index;
                    if (seat_index != -1)
                        scariness += ((unit_seat *)vehicle->seats.address)[seat_index].scariness;
                }
            }
            if (weapon_index != -1) {
                unit_datum *weapon = scariness_object_get(weapon_index);
                weapon_definition *definition = (weapon_definition *)scariness_tag_get(weapon->object.definition_index);
                scariness += definition->scariness;
            }
            if (secondary_weapon_index != -1) {
                unit_datum *weapon = scariness_object_get(secondary_weapon_index);
                weapon_definition *definition = (weapon_definition *)scariness_tag_get(weapon->object.definition_index);
                scariness += definition->scariness;
            }
            if (actor && ai_globals) {
                if (actor->berserking)
                    scariness += ai_globals->berserk_scariness;
                if (actor_executing_layer(actor_index, 18) != -1)
                    scariness += ai_globals->ability_scariness;
            }
            if (ai_globals && unit->unit.player_index != -1) {
                long equipment_index = unit->unit.equipment_object_index;
                if (equipment_index != -1) {
                    float fraction = equipment_active_fraction(equipment_index);
                    unit_datum *equipment = scariness_object_get(equipment_index);
                    s_equipment_definition *definition = (s_equipment_definition *)scariness_tag_get(equipment->object.definition_index);
                    if (fraction > AI_PERCEPTION_REAL_CONSTANTS[200] &&
                        (AI_PERCEPTION_REAL_CONSTANTS[0] - fraction) * definition->duration > definition->activation_delay
                        && definition->equipment_types[7].count != 0)
                        scariness += ai_globals->equipment_scariness;
                }
            }
        } else {
            scariness = AI_PERCEPTION_REAL_CONSTANTS[200];
        }
    }
    return scariness;
}

static __forceinline unsigned long *actor_perception_random_seed_address()
{
    return TLS_RANDOM_SEED;
}

// @address 0xc80f0 ?actor_perception_unassailable@@YAXJJ_N0@Z
void actor_perception_unassailable(long actor_index, long prop_ref_index, bool unassailable, bool update_salience)
{
    prop_ref_datum *prop_ref = datum_get_typed<prop_ref_datum>(prop_ref_data, prop_ref_index);
    prop_view *view = prop_view_get(prop_ref);
    if (view != 0) {
        if (!unassailable) {
            view->unassailable_until = -1;
        } else if (prop_ref->awareness_class >= 1) {
            view->unassailable_until = game_time_get() +
                game_seconds_to_ticks_round(
                    _real_random_range(actor_perception_random_seed_address(), 5.0f, 7.0f));
        }
        if (update_salience)
            actor_compute_prop_salience(actor_index, prop_ref_index, _prop_stimulus_none);
    }
}

extern "C" {
// @address 0x72d2b8 k_prop_type_info
const prop_type k_prop_type_info[] = {
    { 0, 0, -1, 65535, -1, 0, 0 },
    { 1, 5, 2, 1, 2, 0, 0 },
    { 2, 2, 2, 1, 0, 0, 0 },
    { 3, 5, 2, 1, 0, 0, 0 },
    { 4, 1, 1, 0, 0, 0, 0 },
    { 5, 1, 1, 0, 0, 0, 0 },
    { 6, 1, 1, 0, 0, 0, 0 },
    { 7, 5, 1, 0, 0, 0, 0 },
    { 8, 1, 1, 0, 0, 0, 0 },
};
}
// @address 0x562880 ?actor_acknowledged_props_update@@YAXJ@Z
void actor_acknowledged_props_update(long actor_index)
{
    actor_datum *actor = datum_get_typed<actor_datum>(actor_data, actor_index);
    perception_track tracks[256];
    short track_count = 0;
    bool cleanup_attempted = false;
    actor_prop_ref_iterator iterator;
    // match: 0x562880: reuse the actor lookup when initializing the shared cursor.
    iterator.index = -1;
    iterator.next_index = actor->first_prop_ref_index;
    prop_ref_datum *prop_ref;
    while ((prop_ref = (prop_ref_datum *)actor_prop_ref_iterator_next(&iterator)) != 0) {
        float weight = prop_track_weight(iterator.index);
        if (weight > 0.0f && prop_ref->awareness_class >= 1 && k_prop_type_info[prop_ref->type].tracking_mode == 2) {
            tracks[track_count].prop_ref_index = iterator.index;
            tracks[track_count].score = weight;
            track_count++;
        } else if (prop_ref->view_index != NONE) {
            prop_remove_tracking(actor_index, iterator.index);
        }
    }
    if (track_count > 4)
        qsort(tracks, track_count, sizeof(perception_track), perception_qsort_compare_tracks);
    for (short i = 0; i < track_count; i++) {
        short track_index = track_count > 4 ? (short)((i + 4) % track_count) : i;
        perception_track *track = &tracks[track_index];
        long prop_ref_index = track->prop_ref_index;
        prop_ref = datum_get_typed<prop_ref_datum>(prop_ref_data, prop_ref_index);
        if (track_index < 4) {
            if (prop_ref->view_index == NONE && track->score > actor->track_threshold) {
                actor_position_data target_position;
                actor_position_data sense_position;
                ::actor_perception_find_target_data(prop_ref->object_index, &target_position);
                if (actor_perception_find_sense_position(actor_index, &target_position.body, &sense_position)) {
                    prop_ref->view_index = datum_new(TLS_PROP_DATA);
                    if (prop_ref->view_index == NONE && !cleanup_attempted) {
                        if (prop_track_reclaim_inactive_tracks(actor_index, track->score))
                            prop_ref->view_index = datum_new(TLS_PROP_DATA);
                        cleanup_attempted = true;
                    }
                    if (prop_ref->view_index != NONE) {
                        tracking_datum *view_datum = datum_get_typed<tracking_datum>(TLS_PROP_DATA, prop_ref->view_index);
                        prop_state_new(&view_datum->state);
                        prop_view_new(&view_datum->tracking_view);
                        actor_prop_state_refresh(actor_index, prop_ref_index, &sense_position, &target_position, true);
                    }
                }
            }
        } else if (prop_ref->view_index != NONE) {
            prop_remove_tracking(actor_index, prop_ref_index);
            actor_compute_prop_salience(actor_index, prop_ref_index, _prop_stimulus_none);
        }
    }
}

// @rdata 0x8202251c
static const float k_actor_perception_zero_salience = 0.0f;

static __forceinline void actor_perception_refresh_awareness_count(actor_datum *actor)
{
    if (actor->target_prop_index != -1) {
        const prop_ref_datum *target_ref = datum_get_typed<prop_ref_datum>(prop_ref_data, actor->target_prop_index);
        signed char awareness = target_ref->awareness_class;
        if (awareness >= 1 && awareness <= 2) {
            actor->target_age = 0;
            return;
        }
    }
    long count = actor->target_age;
    if (count != -1 && count < 0x7fff)
        actor->target_age = count + 1;
}

// @address 0xb4c98 actor_perception_update_best_target
extern "C" void actor_perception_update_best_target(long actor_index)
{
    float best_salience = k_actor_perception_zero_salience;
    long best_prop = -1;
    actor_datum *actor = datum_get_typed<actor_datum>(actor_data, actor_index);
    short best_class = 0;
    long best_index = -1;
    bool target_seen = false;
    e_ai_posture posture = (e_ai_posture)actor_get_posture(actor_index);
    prop_datum *prop;
    memset(&actor->situation, 0, sizeof(actor->situation));
    actor_prop_ref_iterator iterator;
    actor_prop_ref_iterator_new(&iterator, actor_index);
    prop_ref_datum *prop_ref;
    while ((prop_ref = (prop_ref_datum *)actor_prop_ref_iterator_next(&iterator)) != 0) {
        long current_prop = iterator.index;
        prop_view *view = 0;
        if (prop_ref->view_index != -1) {
            tracking_datum *view_datum = datum_get_typed<tracking_datum>(TLS_PROP_DATA, prop_ref->view_index);
            if (view_datum)
                view = &view_datum->tracking_view;
        }
        prop = datum_get_typed<prop_datum>(prop_data, prop_ref->prop_index);
        if ((prop_ref->m_flags.m_storage & 8) != 0)
            actor_compute_prop_salience(actor_index, current_prop, _prop_stimulus_none);
        if ((prop_ref->m_flags.m_storage & 1) == 0)
            continue;
        if (current_prop == actor->target_prop_index)
            target_seen = true;
        if (!(prop_ref->salience <= best_salience)) {
            best_prop = current_prop;
            best_salience = prop_ref->salience;
        }
        short prop_class = calculate_prop_class(actor_index, posture, prop_ref);
        if (prop_class > best_class) {
            best_class = prop_class;
            best_index = current_prop;
        }
        if (!view)
            continue;
        calculate_awareness(prop_ref, view);
        calculate_scariness(actor_index, prop_ref, prop, view);
        actor->situation.prop_count++;
    }
    actor->situation.best_prop_class = best_class;
    actor->situation.best_prop_index = best_index;
    if (best_prop != actor->target_prop_index && (((_rotl(actor->target_flags, 29) & 1) == 0) || !target_seen)) {
        actor_perception_set_target(actor_index, best_prop);
        stimulus_register(actor_index, _behavior_stimulus_changed_target, 1, 3);
    }
    actor_perception_refresh_awareness_count(actor);
    actor->target_flags &= ~FLAG(1);
    actor_combat_status_update(actor_index);
}


static __forceinline const float &actor_perception_vehicle_margin_reference()
{
    return actor_perception_vehicle_margin_value;
}

static __forceinline const float &actor_perception_danger_zone_zero_reference()
{
    return k_actor_perception_zero_salience;
}

// @rdata 0x8202240c..0x82022428
static const short actor_perception_combat_status_table[14] = { 1, 1, 1, 2, 3, 4, 6, 7, 8, 8, 9, 9, 9, 9 };
// @rdata-end

// @address 0x5638e8 ?actor_combat_status_update@@YAXJ@Z
void actor_combat_status_update(long actor_index)
{
    actor_datum *actor = datum_get_typed<actor_datum>(actor_data, actor_index);
    short clump_status = 1;
    short danger_status = 0;
    short state_status = actor_perception_combat_status_table[actor->situation.best_prop_class];
    if (actor->clump_index != -1)
    {
        clump_datum *clump = datum_get_typed<clump_datum>(clump_data, actor->clump_index);
        clump_status = k_clump_state_combat_status[clump->combat_state];
    }
    if (actor->root_vehicle_index == -1 && state_status == 1 && actor->combat_status == 0)
        state_status = 0;
    if (actor->danger_zone.danger_type > 0)
        danger_status = 2;
    short status = MAX(state_status, MAX(clump_status, danger_status));

    if (status < 3)
    {
        bool forced = false;
        if (actor->combat_status > 3)
        {
            actor->combat_status_time = 0;
            actor->combat_status = 3;
        }
        if (actor->squad_index != -1 && actor->combat_status > 0)
        {
            squad_datum *squad = datum_get_typed<squad_datum>(squad_data, actor->squad_index);
            if (squad_valid_task(squad))
            {
                s_task *task = &((s_task *)((s_objective *)global_scenario->objectives.address)[squad->objective_index].tasks.address)[squad->task_index];
                long movement = task_get_movement(actor->team, task);
                if (status < k_movement_combat_status_table[k_movement_combat_status_index[movement]])
                {
                    status = k_movement_combat_status_table[k_movement_combat_status_index[movement]];
                    forced = true;
                }
            }
        }
        if (!forced)
        {
            short style_status = actor_perception_latched_combat_status(actor_index);
            if (actor->combat_status >= style_status && status < style_status)
                status = style_status;
        }
        short current_status = actor->combat_status;
        if (status < current_status)
        {
            short ticks = (short)actor->combat_status_time;
            status = current_status;
            switch (current_status)
            {
            case 3:
                if (TLS_GAME_TIME_GLOBALS->tick_length * (float)ticks < 0.0f)
                    break;
                status = 2;
            case 2:
                if (TLS_GAME_TIME_GLOBALS->tick_length * (float)ticks >= 15.0f)
                    status = 1;
                break;
            }
        }
        else
            actor->combat_status_time = 0;
    }
    else
        actor->combat_status_time = 0;

    if (actor->command_index != -1)
    {
        command_script_datum *script = datum_get_typed<command_script_datum>(tls_command_script_data(), actor->command_index);
        if (script->forced_combat_status > status)
        {
            status = script->forced_combat_status;
            actor->combat_status_time = 0;
        }
    }
    if (actor->combat_status != status)
    {
        if (status >= 3 && actor->combat_status < 2)
            stimulus_register(actor_index, _behavior_stimulus_danger, 2, 3);
        actor->combat_status_time = 0;
        actor->combat_status = status;
    }
    else
        actor->combat_status_time++;
    if (actor->combat_status >= 7)
        actor->combat_active = true;
}
static __forceinline bool prop_status_location_valid(const s_location *location)
{
    return (unsigned char)location->cluster_reference.bsp_index != 255;
}
static __forceinline s_cluster_reference *prop_status_cluster_reference_invalidate(s_cluster_reference *reference)
{
    *(short *)reference = -1;
    return reference;
}
static __forceinline s_cluster_reference prop_status_invalid_cluster_reference_get()
{
    s_cluster_reference result;
    prop_status_cluster_reference_invalidate(&result);
    return result;
}
static __forceinline s_cluster_reference prop_status_location_cluster_reference_get(const s_location *location)
{
    if (prop_status_location_valid(location))
        return location->cluster_reference;
    return prop_status_invalid_cluster_reference_get();
}
static __forceinline s_cluster_reference prop_status_position_cluster_reference_get(const actor_position_data *position)
{
    if ((unsigned char)position->location.cluster_reference.bsp_index != 255)
        return position->location.cluster_reference;
    return prop_status_invalid_cluster_reference_get();
}
static __forceinline unit_datum *prop_status_object_get(object_header_datum *headers, long index)
{
    return (unit_datum *)headers[datum_index_to_absolute_index(index)].datum;
}
static __forceinline long prop_status_get_root_object(object_header_datum *headers, long index)
{
    while (index != -1) {
        unit_datum *object = prop_status_object_get(headers, index);
        if (!actor_perception_vehicle_flag(object->object.flags, 26))
            break;
        index = object->object.parent_object_index;
    }
    return index;
}
static __forceinline prop_ref_datum *prop_status_prop_ref_get(long prop_ref_index)
{
    return datum_get_typed<prop_ref_datum>(prop_ref_data, prop_ref_index);
}
static __forceinline short prop_status_get_ineffability(long actor_index, long prop_ref_index)
{
    prop_ref_datum *prop_ref = prop_status_prop_ref_get(prop_ref_index);
    prop_datum *prop = datum_get_typed<prop_datum>(prop_data, prop_ref->prop_index);
    short result = 0;
    if (prop->ineffability_ticks > 0) {
        result = 3;
    } else if (prop->enemy && prop->active) {
        actor_datum *actor = datum_get_typed<actor_datum>(actor_data, actor_index);
        long squad_index = actor->squad_index;
        if (squad_index != -1 && ((*datum_get_typed<squad_datum>(squad_data, squad_index)).flags & 0x400))
            result = 3;
    }
    return result;
}
// @address 0xaefb8 ?prop_status_refresh@@YAXJJPAUactor_position_data@@0@Z
void prop_status_refresh(long actor_index, long prop_ref_index, actor_position_data *sense_position, actor_position_data *target_position)
{
    actor_datum *actor = datum_get_typed<actor_datum>(actor_data, actor_index);
    if (!actor->active)
        return;
    prop_ref_datum *prop_ref = datum_get_typed<prop_ref_datum>(prop_ref_data, prop_ref_index);
    prop_view *view = prop_view_get(prop_ref);
    prop_datum *prop = datum_get_typed<prop_datum>(prop_data, prop_ref->prop_index);
    unit_datum *track_object = 0;
    long game_time = game_time_get();
    bool blind = actor_is_blind(actor_index);
    bool same_target = actor->target_prop_index == prop_ref_index;
    object_header_datum *headers = (object_header_datum *)object_header_data->data;
    unit_datum *object = prop_status_object_get(headers, prop_ref->object_index);
    if (object->object.object_type == 0)
        track_object = object;
    if (actor->type == 15 && actor->unit_index != -1) {
        long vehicle = prop_status_get_root_object(headers, actor->unit_index);
        long marker_index = -1;
        bool adopted = false;
        if (vehicle != actor->unit_index) {
            unit_datum *root_object = prop_status_object_get(headers, vehicle);
            long seat_index = root_object->unit.driver_object_index;
            if (seat_index != -1) {
                if (seat_index != actor->unit_index)
                    marker_index = prop_status_object_get(headers, seat_index)->unit.actor_index;
            } else if (root_object->unit.actor_index != -1) {
                marker_index = root_object->unit.actor_index;
            }
        }
        if (marker_index != -1) {
            long new_prop_ref_index = prop_ref_get_by_object_index(marker_index, prop_ref->object_index, false, _clump_importance_none);
            if (new_prop_ref_index != -1) {
            prop_ref_datum *new_prop_ref = datum_get_typed<prop_ref_datum>(prop_ref_data, new_prop_ref_index);
            prop_view *new_view = prop_view_get(new_prop_ref);
            prop_ref->maximum_awareness = new_prop_ref->maximum_awareness;
            prop_ref->flags_1e = (new_prop_ref->flags_1e & 0xF0) | (prop_ref->flags_1e & 0x0F);
            if (view) {
                if (new_view) {
                    view->search_state = new_view->search_state;
                    view->audibility = new_view->audibility;
                    view->ineffability = new_view->ineffability;
                    view->sighting_awareness = new_view->sighting_awareness;
                    view->sighting_search_state = new_view->sighting_search_state;
                } else {
                    view->search_state = 4;
                    view->sighting_search_state = 4;
                    view->ineffability = 0;
                    view->audibility = 0;
                    view->sighting_awareness = 0;
                }
            }
            adopted = true;
            }
        }
        if (!adopted) {
            prop_ref->maximum_awareness = 0;
            prop_ref->flags_1e &= 0x0f;
            if (!view)
                return;
            view->search_state = 4;
            view->sighting_search_state = 4;
            view->audibility = 0;
            view->ineffability = 0;
            view->sighting_awareness = 0;
        }
    } else {
        bool transparent_hit = false;
        object_header_datum *headers_los = (object_header_datum *)object_header_data->data;
        long node;
        if (track_object && track_object->object.parent_object_index != -1) {
            node = prop_status_get_root_object(headers_los, track_object->object.parent_object_index);
        } else {
            node = prop_status_get_root_object(headers_los, prop_ref->object_index);
        }
        long *hit_object = same_target ? &actor->target_object_index : 0;
        long ignore_b = actor->unit_index;
        if (ignore_b != -1) {
            long root_b = ignore_b;
            do {
                root_b = ignore_b;
                ignore_b = prop_status_object_get(headers_los, ignore_b)->object.parent_object_index;
            } while (ignore_b != -1);
            ignore_b = root_b;
        } else {
            ignore_b = -1;
        }
        long mode = prop->enemy && prop->active ? 3 : 0;
        actor_position_data *sense = (actor_position_data *)sense_position;
        actor_position_data *target = (actor_position_data *)target_position;
        s_cluster_reference target_cluster = prop_status_position_cluster_reference_get(target);
        s_cluster_reference sense_cluster = prop_status_position_cluster_reference_get(sense);
        bool alternate = !same_target;
        short los = ai_test_line_of_sight(&sense->head, sense_cluster, &target->head, target_cluster, mode, false, node, ignore_b, false,
            alternate, hit_object, &transparent_hit);
        prop_ref->flags_1e = (prop_ref->flags_1e & 0xF0) | (los & 0x0F);
        if (prop->enemy) {
            long clump_index = (*datum_get_typed<actor_datum>(actor_data, actor_index)).clump_index;
            if (clump_index != -1 && los <= 1 && prop_status_prop_ref_get(prop_ref_index)->distance < 25.0f)
                (*datum_get_typed<clump_datum>(clump_data, clump_index)).last_visible_enemy_time = game_time_get();
        }
        if (view)
            view->search_state = los;
        if (transparent_hit)
            prop_ref->m_flags.m_storage |= (unsigned char)FLAG(2);
        else
            prop_ref->m_flags.m_storage &= (unsigned char)~FLAG(2);
        if (!prop->dead_2e && !blind) {
            bool visible_out = false;
            short visibility = prop_get_visibility(actor_index, prop_ref_index, prop->enemy, los, sense_position, target_position, &visible_out);
            if (view && prop_ref->awareness_class >= 3) {
                prop_state *state = prop_state_get(prop_ref);
                if (!(view->flags & 0x40)) {
                    long *hit_object2 = same_target ? &actor->target_object_index : 0;
                    long ignore_b2 = actor->unit_index;
                    if (ignore_b2 != -1) {
                        s_data_array *header_array2 = object_header_data;
                        long root_b2 = ignore_b2;
                        do {
                            root_b2 = ignore_b2;
                            ignore_b2 = prop_status_object_get((object_header_datum *)header_array2->data, ignore_b2)->object.parent_object_index;
                        } while (ignore_b2 != -1);
                        ignore_b2 = root_b2;
                    } else {
                        ignore_b2 = -1;
                    }
                    long mode2 = prop->enemy && prop->active ? 3 : 0;
                    s_cluster_reference state_cluster = prop_status_location_cluster_reference_get(
                        (const s_location *)&state->location);
                    s_cluster_reference sense_cluster2 = prop_status_position_cluster_reference_get(sense);
                    short los2 = ai_test_line_of_sight(&sense->head, sense_cluster2, &state->target_position, state_cluster, mode2, false,
                        node, ignore_b2, false, alternate, hit_object2, 0);
                    view->sighting_search_state = los2;
                    view->sighting_awareness = actor_visibility_at_point(actor_index, sense_position, &state->target_position, 0, los2, true, false, 0);
                } else {
                    view->sighting_awareness = 3;
                    if (same_target)
                        actor->target_object_index = -1;
                }
            }
            short audibility = prop_get_audibility(actor_index, prop_ref->object_index, sense_position, &target->body, (s_location *)&target->location, los);
            short ineffability = prop_status_get_ineffability(actor_index, prop_ref_index);
            prop_ref->flags_1e = (prop_ref->flags_1e & 0x0F) | (visibility << 4);
            if (view) {
                view->audibility = audibility;
                view->ineffability = ineffability;
            }
            signed char level = (signed char)MAX(visibility, MAX(audibility, ineffability));
            prop_ref->maximum_awareness = level;
            if (level == 1 && prop_ref->awareness_class >= 1)
                prop_ref->maximum_awareness = 2;
        } else {
            prop_ref->maximum_awareness = 0;
            prop_ref->flags_1e &= 0x0f;
            if (!view)
                return;
            view->ineffability = 0;
            view->audibility = 0;
        }
    }
    if (view) {
        if (prop_ref->maximum_awareness >= 2)
            view->perception_time = game_time;
        if (prop_ref->awareness > 0) {
            object_get_pathfinding_location(prop_ref->object_index, 0, &view->pathfinding_point);
            view->tracking_index = game_time;
        }
    }
}


static __forceinline const long &actor_perception_refresh_unit_player_index(long object_index)
{
    unsigned long header_offset = ((unsigned long)object_index * sizeof(object_header_datum)) & (0xffff * sizeof(object_header_datum));
    object_header_datum *header = (object_header_datum *)((char *)object_header_data->data + header_offset);
    unit_datum *unit = (unit_datum *)header->datum;
    return unit->unit.player_index;
}

static __forceinline void actor_perception_refresh_aiming_vector(long unit_index, real_vector3d *aiming)
{
    const object_header_datum *headers = (object_header_datum *)object_header_data->data;
    long absolute_index = unit_index;
    absolute_index = datum_index_to_absolute_index(unit_index);
    *aiming = ((const unit_datum *)headers[absolute_index].datum)->unit.aiming_vector;
}

static __forceinline void actor_perception_refresh_player_index(long player_index, short *index)
{
    *index = actor_perception_ai_player_index_get(player_index);
}

static __forceinline void actor_perception_refresh_register_impulse(actor_datum *actor, const prop_ref_datum *reference)
{
    actor->attention_ticks = (short)game_seconds_to_ticks_round(4.0f);
    actor->attention_direction.object_index = reference->object_index;
    actor->attention_direction.type = 7;
    short *impulse = &actor->attention_level;
    *impulse = 1;
}

// @rdata 0x82024448
static const float k_actor_perception_player_relevance_distance = 25.0f;
// @address 0xb51e8 ?actor_perception_player_state_refresh@@YAXJJ@Z
void actor_perception_player_state_refresh(long actor_index, long prop_ref_index)
{
    actor_datum *actor = datum_get_typed<actor_datum>(actor_data, actor_index);
    prop_ref_datum *reference = datum_get_typed<prop_ref_datum>(prop_ref_data, prop_ref_index);
    prop_datum *prop = datum_get_typed<prop_datum>(prop_data, reference->prop_index);
    actor->boarding_dialogue_active = false;
    if (!prop->active && reference->distance < 3.0f && reference->awareness_class >= 1 && reference->awareness_class <= 2) {
        long object_index = reference->object_index;
        real_point3d position;
        object_get_origin(object_index, &position);
        object_index = reference->object_index;
        real_vector3d aiming;
        actor_perception_refresh_aiming_vector(object_index, &aiming);
        real_vector3d direction;
        vector_from_points3d(&actor->position.body, &position, &direction);
        if (normalize3d(&direction) == 0.0f)
            direction = *global_forward3d;
        short facing = facing_category(&direction, &aiming, reference->distance);
        if (!(prop->flags & 1)) {
            if (facing <= 2) {
                if (TLS_GAME_TIME_GLOBALS->time - actor->creation_time > game_seconds_to_ticks_round(4.0f))
                    actor_dialogue_event(actor_index, 0x54, object_index, 0);
                prop->flags |= 1;
                actor->boarding_dialogue_ticks = 0;
            }
        } else {
            const long &player_index = actor_perception_refresh_unit_player_index(object_index);
            if (player_index != -1) {
                if (facing <= 0) {
                    actor_perception_refresh_player_index(player_index, &actor->boarding_dialogue_player_index);
                    actor->boarding_dialogue_active = true;
                } else {
                    actor->boarding_dialogue_active = false;
                    actor->boarding_dialogue_player_index = -1;
                    actor->boarding_dialogue_ticks = 0;
                }
            }
        }
        if (!actor->player_seen) {
            if (actor->root_vehicle_index == -1 && actor->combat_status <= 3 && actor->attention_level <= 1) {
                actor_perception_refresh_register_impulse(actor, reference);
                actor->player_seen = true;
            } else {
                actor->player_seen = true;
            }
        }
    }
    if (actor->clump_index != -1) {
        prop_view *view = prop_view_get(reference);
        if (view && view->status >= 5 && reference->distance < k_actor_perception_player_relevance_distance) {
            clump_datum *clump = datum_get_typed<clump_datum>(clump_data, actor->clump_index);
            clump->last_visible_enemy_time = TLS_GAME_TIME_GLOBALS->time;
        }
    }
}

// @rdata 0x82022dc0 pooled
static const float k_actor_prop_state_half = 0.5f;
// @rdata 0x82021694
static const float k_actor_prop_state_unknown_distance = FLT_MAX;
static __forceinline long actor_prop_state_game_seconds_to_ticks_round(float seconds)
{
    return (long)(seconds * (float)TLS_GAME_TIME_GLOBALS->ticks_per_second + k_actor_prop_state_half);
}
// match: 0xB2590: named tick rate preserves the timeout register allocation.
static __forceinline long actor_prop_state_unreachable_timeout_ticks(float seconds)
{
    const long &ticks_per_second = TLS_GAME_TIME_GLOBALS->ticks_per_second;
    return (long)(seconds * (float)ticks_per_second + k_actor_prop_state_half);
}
static __forceinline bool actor_perception_prop_should_refresh_state(const prop_ref_datum *prop_ref)
{
    return (prop_ref->awareness_class >= 1 && prop_ref->awareness_class <= 2)
        || (prop_ref->awareness_class >= 1 && prop_ref->maximum_awareness > 0);
}
static __forceinline bool actor_perception_prop_allow_state_update(prop_datum *prop)
{
    bool allow = false;
    if (k_prop_type_info[prop->type].state_update_mode == 1
        && game_time_get() > ((prop_datum *)prop)->state.flags)
        allow = true;
    return allow;
}
static __forceinline void actor_perception_unit_get_aiming_vector(long unit_index, real_vector3d *aiming_vector)
{
    *aiming_vector = ((unit_datum *)object_get(unit_index))->unit.aiming_vector;
}
// @address 0xb2590 ?actor_prop_state_refresh@@YAXJJPAUactor_position_data@@0_N@Z
void actor_prop_state_refresh(long actor_index, long prop_ref_index, actor_position_data *sense_position, actor_position_data *target_position, bool new_track)
{
    prop_ref_datum *prop_ref = datum_get_typed<prop_ref_datum>(prop_ref_data, prop_ref_index);
    object_datum *object = datum_get_typed<object_header_datum>(object_header_data, prop_ref->object_index)->datum;
    prop_datum *prop = datum_get_typed<prop_datum>(prop_data, prop_ref->prop_index);
    unit_datum *unit = 0;
    prop_state *state = 0;
    prop_view *view = 0;
    long time = game_time_get();
    bool refresh = false;
    if (TEST_BIT(0x1003, object->object_identifier.get_type()))
        unit = (unit_datum *)object;
    if (actor_perception_prop_should_refresh_state(prop_ref) || new_track)
        refresh = true;
    if (prop_ref->awareness_class >= 1 && prop_ref->awareness_class <= 2) {
        prop_ref->type = (signed char)prop->type;
    } else if (prop_ref->type != prop->type && unit && prop->type == 6) {
        if (game_ticks_to_seconds((float)(time - unit->unit.type_change_time)) > 5.0f)
            prop_ref->type = (signed char)prop->type;
    }
    if (prop_ref->view_index != NONE) {
        tracking_datum *datum = datum_get_typed<tracking_datum>(TLS_PROP_DATA, prop_ref->view_index);
        state = &datum->state;
        view = &datum->tracking_view;
    } else if (actor_perception_prop_allow_state_update(prop)) {
        state = &((prop_datum *)prop)->state;
    }
    if (state && refresh) {
        if (view && prop->owned_by_actor && prop->owner_index != NONE) {
            actor_datum *owner = datum_get_typed<actor_datum>(actor_data, prop->owner_index);
            long creature_index = swarm_perception_creature_from_swarm(owner->swarm_index, sense_position, prop_ref->object_index);
            if (creature_index != prop_ref->object_index)
                prop_ref->object_index = creature_index;
        }
        prop_state_refresh(state, prop_ref->object_index, (short)(signed char)prop_ref->stimulus_type, target_position);
        if ((*datum_get_typed<prop_datum>(prop_data, prop_ref->prop_index)).active && state->parent != NONE)
            stimulus_register(actor_index, _behavior_stimulus_enemy_vehicle, 1, 3);
    }
    prop_state *known_state = actor_perception_prop_state_get(prop_ref);
    if (view) {
        real_vector3d *direction = &view->direction;
        vector_from_points3d(&sense_position->body, &known_state->position, direction);
        prop_ref->distance = normalize3d(direction);
        if (prop_ref->distance == 0.0f)
            *direction = *global_forward3d;
        if (view->unreachable_time != NONE && view->unreachable_time + actor_prop_state_unreachable_timeout_ticks(5.0f) < time)
            actor_perception_unreachable(actor_index, prop_ref_index, false, false);
        if (prop_ref->awareness_class >= 1 && prop_ref->awareness_class <= 2)
            actor_prop_update_new_position(actor_index, prop_ref_index, true);
        if (view->unassailable_until != NONE && view->unassailable_until < time)
            actor_perception_unassailable(actor_index, prop_ref_index, false, false);
    } else {
        if (known_state->flags == NONE)
            prop_ref->distance = k_actor_prop_state_unknown_distance;
        else {
            real_vector3d vector;
            vector_from_points3d(&sense_position->body, &known_state->position, &vector);
            prop_ref->distance = magnitude3d(&vector);
        }
    }
    if (view && refresh) {
        real_vector3d velocity;
        object_get_velocities(prop_ref->object_index, &velocity, 0);
        if (-dot_product3d(&velocity, &view->direction) < k_actor_prop_state_half) {
            view->flags &= ~FLAG(0);
            view->approach_ticks = 0;
        } else {
            view->approach_ticks++;
            if (view->approach_ticks >= actor_prop_state_game_seconds_to_ticks_round(0.2f)) {
                view->approach_ticks = (short)game_seconds_integer_to_ticks(1);
                view->flags |= FLAG(0);
            }
        }
        real_vector3d aiming_vector;
        if (unit)
            actor_perception_unit_get_aiming_vector(prop_ref->object_index, &aiming_vector);
        else
            aiming_vector = object->forward;
        view->category = (signed char)facing_category(&view->direction, &aiming_vector, prop_ref->distance);
    }
    if (refresh) {
        if (known_state->b6b) {
            long parent_prop_ref_index = prop_ref_get_by_object_index(actor_index, known_state->parent, false, _clump_importance_none);
            if (parent_prop_ref_index != NONE) {
                prop_ref_datum *parent_prop_ref = datum_get_typed<prop_ref_datum>(prop_ref_data, parent_prop_ref_index);
                real_point3d origin;
                object_get_origin(parent_prop_ref->object_index, &origin);
                parent_prop_ref->distance = distance3d(&sense_position->body, &origin);
                actor_perception_assess_vehicle_danger(actor_index, parent_prop_ref_index);
            }
        } else if (prop_ref->type == 3) {
            actor_perception_assess_danger_zone_danger(actor_index, prop_ref_index);
        } else if (prop_ref->type == 4) {
            actor_perception_assess_vehicle_danger(actor_index, prop_ref_index);
        } else if (unit) {
            actor_perception_assess_suicide_danger(actor_index, prop_ref_index);
        }
        if (prop->enemy)
            actor_perception_player_state_refresh(actor_index, prop_ref_index);
        if (unit && unit->object.parent_object_index == NONE && prop_ref->m_flags.test(_prop_ref_unboardable_bit))
            prop_ref->m_flags.set(_prop_ref_unboardable_bit, false);
    }
    actor_compute_prop_salience(actor_index, prop_ref_index, _prop_stimulus_none);
    prop_ref->state_refresh_ticks = 0;
}

static __forceinline unsigned char actor_danger_zone_object_get_type(long object_index)
{
    return (*datum_get_typed<object_header_datum>(object_header_data, object_index)).object_type;
}
// @address 0x563240 ?actor_perception_refresh_danger_zone@@YAXJ@Z
void actor_perception_refresh_danger_zone(long actor_index)
{
    actor_datum *actor = (actor_datum *)(actor_data->data + DATUM_INDEX_TO_ABSOLUTE_INDEX(actor_index) * sizeof(actor_datum));
    if (actor->danger_zone.danger_type > 0) {
        object_datum *object = (object_datum *)object_try_and_get_and_verify_type(actor->danger_zone.object_index, NONE);
        if (!object) {
            actor->danger_zone.danger_type = 0;
        } else {
            object_get_origin(actor->danger_zone.object_index, &actor->danger_zone.position);
            object_get_velocities(actor->danger_zone.object_index, &actor->danger_zone.velocity, 0);
            actor_position_data sense_position;
            if (actor_perception_find_sense_position(actor_index, &actor->danger_zone.position, &sense_position)) {
                actor->danger_zone.distance = distance3d(&sense_position.body, &actor->danger_zone.position);
                actor->danger_zone.midpoint_distance_squared = distance_squared3d(&sense_position.body, &actor->danger_zone.midpoint);
            }
            point_from_line3d(&actor->danger_zone.position, &actor->danger_zone.velocity, 1.5f, &actor->danger_zone.projected_position);
            midpoint3d(&actor->danger_zone.position, &actor->danger_zone.projected_position, &actor->danger_zone.midpoint);
            actor->danger_zone.extent = distance3d(&actor->danger_zone.midpoint, &actor->danger_zone.position) + actor->danger_zone.radius;
            prop_ref_datum *prop_ref = (prop_ref_datum *)datum_try_and_get(prop_ref_data, actor->danger_zone.pref_index);
            bool acknowledged = prop_ref ? (prop_ref->awareness_class >= 1 && prop_ref->awareness_class <= 2) : false;
            bool alerted = false;
            bool attached = false;
            switch (actor->danger_zone.danger_type) {
            case 2: {
                projectile_datum *projectile = (projectile_datum *)object;
                attached = projectile->projectile.flags.test(_projectile_perception_attached_bit);
                break;
            }
            case 3: {
                short ticks = NONE;
                if (actor->unit_index != NONE && object->parent_object_index == actor->unit_index)
                    attached = true;
                switch (actor_danger_zone_object_get_type(actor->danger_zone.object_index)) {
                case 1:
                    if (!destroyed_vehicle_is_dangerous(actor->danger_zone.object_index, &ticks))
                        actor->danger_zone.danger_type = 0;
                    break;
                case 3:
                    break;
                case 5: {
                    projectile_datum *projectile = (projectile_datum *)object;
                    if (projectile->projectile.detonation_timer > 0.0f && projectile->projectile.detonation_timer_delta > 0.0f)
                        ticks = (short)(long)((1.0f - projectile->projectile.detonation_timer) / projectile->projectile.detonation_timer_delta + 0.5f);
                    break;
                }
                default:
                    // match: 0x563240: unreachable hint omits dead branch check
                    __assume(0);
                }
                actor->danger_zone.ticks = ticks;
                break;
            }
            case 4: {
                unit_datum *vehicle = (unit_datum *)object;
                vehicle_definition *definition =
            (vehicle_definition *)((void *)global_tag_instances[datum_index_to_absolute_index(vehicle->object.definition_index)].definition);
                real_vector3d velocity;
                object_get_velocities(actor->danger_zone.object_index, &velocity, 0);
                if (actor_perception_vehicle_magnitude_squared(&velocity) < 0.2f * 0.2f || actor->danger_zone.distance > definition->bounding_radius + 10.0f) {
                    actor->danger_zone.danger_type = 0;
                } else if (!acknowledged) {
                    long driver_prop_ref_index = NONE;
                    if (vehicle->unit.driver_object_index != NONE) {
                        driver_prop_ref_index = prop_ref_get_acknowledged_by_object_index(actor_index, vehicle->unit.driver_object_index);
                        if (driver_prop_ref_index != NONE) {
                            prop_ref_datum *driver_prop_ref = datum_get_typed<prop_ref_datum>(prop_ref_data, driver_prop_ref_index);
                            acknowledged = driver_prop_ref->awareness_class >= 1 && driver_prop_ref->awareness_class <= 2;
                            alerted = acknowledged;
                        }
                    }
                }
                break;
            }
            case 1:
                if (motor_animation_get_state(actor->danger_zone.object_index) == 80 ||
                    unit_animation_get_action_name(actor->danger_zone.object_index) == 10)
                {
                    float duration;
                    float time = motor_animation_base_channel_time(actor->danger_zone.object_index, &duration);
                    actor->danger_zone.ticks = (short)game_seconds_to_ticks_round(duration - time);
                } else {
                    actor->danger_zone.danger_type = 0;
                }
                break;
            default:
                // match: 0x563240: unreachable hint omits dead branch check
                __assume(0);
            }
            if (!acknowledged && alerted && actor->attention_level <= 0) {
                actor->attention_ticks = (short)game_seconds_to_ticks_round(2.0f);
                actor->attention_direction.to_danger();
                actor->attention_level = 0;
            }
            if (!acknowledged && actor->danger_zone.pref_index != NONE && (actor->danger_zone.state != 0 || attached)) {
                prop_perceive(actor_index, actor->danger_zone.object_index, 0, _prop_stimulus_none, 0, actor->danger_zone.pref_index, NONE, NONE);
                acknowledged = true;
            }
            actor->danger_zone.forced = attached;
            if (attached) {
                actor->danger_zone.evade = false;
                actor->danger_zone.active = false;
            }
        }
    }
}


static __forceinline bool actor_perception_prop_acknowledged_once(const prop_ref_datum *prop_ref)
{
    const prop_type *type = &k_prop_type_info[prop_ref->type];
    prop_datum *prop = datum_get_typed<prop_datum>(prop_data, prop_ref->prop_index);
    bool acknowledged_once = false;
    if (prop->status == 1 && type->state_update_mode == 0)
        acknowledged_once = true;
    return acknowledged_once;
}
// search ticks drained per update while a prop is being searched for: 2 when the view's bit-4 flag is set;
// a prop that is not the actor's current target drains one more (the decay is even, so the extra tick is or-ed in)
static __forceinline long actor_perception_search_decay(const prop_view *view)
{
    return TEST_BIT(view->flags, 4) ? 2 : 0;
}

// @address 0xae780 ?actor_perception_update@@YAXJ@Z
void actor_perception_update(long actor_index)
{
    actor_datum *actor = datum_get_typed<actor_datum>(actor_data, actor_index);
    short highest_state_refresh_ticks = 1;
    short highest_status_refresh_ticks = 1;
    bool status_refreshed = false;
    bool state_refreshed = false;
    if (actor->tracking_timeslice)
        actor_acknowledged_props_update(actor_index);
    actor_perception_refresh_danger_zone(actor_index);

    actor_prop_ref_iterator iterator;
    actor_prop_ref_iterator_new(&iterator, actor_index);
    prop_ref_datum *prop_ref;
    while ((prop_ref = (prop_ref_datum *)actor_prop_ref_iterator_next(&iterator)) != 0) {
        bool refresh_state = false;
        bool refresh_status = false;
        actor_position_data target_position;
        actor_position_data sense_position;

        if (prop_ref->stimulus_decay_ticks != 0) {
            if (--prop_ref->stimulus_decay_ticks == 0) {
                prop_ref->stimulus_type = _prop_stimulus_none;
                prop_ref->m_flags.set(_prop_ref_flag_3, true);
            }
        }
        prop_view *view = prop_view_get(prop_ref);
        if (view) {
            if (view->orphan_ticks > 0)
                view->orphan_ticks--;
            if (prop_ref->awareness >= 1) {
                if (view->visible_ticks < SHRT_MAX)
                    view->visible_ticks++;
            } else {
                view->visible_ticks = 0;
            }
        }

        if (prop_should_refresh_status(prop_ref)) {
            if (!status_refreshed && actor->perception_timeslice
                && prop_ref->status_refresh_ticks >= actor->highest_status_refresh_ticks) {
                refresh_state = true;
                prop_ref->status_refresh_ticks = 0;
                refresh_status = true;
                status_refreshed = true;
            } else {
                prop_ref->status_refresh_ticks += k_prop_type_info[prop_ref->type].refresh_rate;
                if (iterator.index == actor->target_prop_index)
                    prop_ref->status_refresh_ticks += 25;
            }
            if (prop_ref->status_refresh_ticks > highest_status_refresh_ticks)
                highest_status_refresh_ticks = prop_ref->status_refresh_ticks;
        }
        if (actor_perception_prop_should_refresh_state(prop_ref)) {
            if (!state_refreshed && prop_ref->state_refresh_ticks >= actor->highest_state_refresh_ticks) {
                refresh_state = true;
                prop_ref->state_refresh_ticks = 0;
                state_refreshed = true;
            } else {
                prop_ref->state_refresh_ticks += k_prop_type_info[prop_ref->type].refresh_rate;
                if (iterator.index == actor->target_prop_index)
                    prop_ref->state_refresh_ticks += 25;
            }
        } else {
            prop_ref->state_refresh_ticks = 0;
        }
        if (iterator.index == actor->target_prop_index)
            refresh_state = true;

        if (refresh_state) {
            actor_perception_find_target_data(prop_ref->object_index, &target_position);
            if (actor_perception_find_sense_position(actor_index, &target_position.body, &sense_position)) {
                if (refresh_status)
                    prop_status_refresh(actor_index, iterator.index, &sense_position, &target_position);
            } else {
                refresh_state = false;
            }
        }

        switch (prop_ref->awareness_class) {
        case 0:
            if (actor_perception_prop_acknowledged_once(prop_ref)) {
                prop_acknowledge(actor_index, iterator.index, true);
                prop_ref->accumulated_awareness = 1.0f;
            } else if (prop_ref->maximum_awareness > 0) {
                prop_ref->accumulated_awareness += get_awareness_delta(actor_index, iterator.index, prop_ref->maximum_awareness);
                if (prop_ref->accumulated_awareness >= 1.0f) {
                    if (prop_acknowledge(actor_index, iterator.index, prop_ref->awareness > 0))
                        prop_ref->accumulated_awareness = 1.0f;
                    else
                        prop_ref->accumulated_awareness = 0.0f;
                }
            } else {
                prop_ref->accumulated_awareness = 0.0f;
            }
            break;
        case 4:
            if (prop_ref->awareness == 0) {
                actor_stimulus_prop_sighted(actor_index, iterator.index, false);
                prop_ref->accumulated_awareness = 0.0f;
            } else {
                prop_ref->accumulated_awareness += get_awareness_delta(actor_index, iterator.index, prop_ref->maximum_awareness);
                if (prop_ref->accumulated_awareness >= 1.0f)
                    prop_acknowledge(actor_index, iterator.index, true);
            }
            break;
        case 2:
            if (allow_orphanhood(prop_ref)) {
                if (prop_ref->awareness > 0) {
                    prop_acknowledge(actor_index, iterator.index, true);
                } else {
                    prop_view *orphan_view = prop_view_get(prop_ref);
                    real_point3d origin;
                    object_get_origin(prop_ref->object_index, &origin);
                    if (!orphan_view || !orphan_view->orphan_ticks
                        || distance_squared2d(&origin.xy, &prop_state_get(prop_ref)->position.xy) > 1.0f)
                        actor_stimulus_prop_sighted(actor_index, iterator.index, false);
                }
            } else {
                actor_flush_prop(actor_index, iterator.index);
                prop_unacknowledge(actor_index, iterator.index);
            }
            break;
        case 1:
            if (prop_ref->awareness == 0 && prop_should_refresh_status(prop_ref)) {
                if (allow_orphanhood(prop_ref)) {
                    prop_view *orphan_view = prop_view_get(prop_ref);
                    prop_ref->awareness_class = 2;
                    if (orphan_view)
                        orphan_view->orphan_ticks = (short)game_seconds_to_ticks_round(0.33f);
                } else {
                    prop_unacknowledge(actor_index, iterator.index);
                }
            }
            break;
        case 3:
            if (allow_orphanhood(prop_ref)) {
                prop_view *search_view = prop_view_get(iterator.index);
                if (search_view && !TEST_BIT(search_view->flags, 6)) {
                    float search_time = 1.5f;
                    character_weapon_properties *weapon = actor_weapon_properties_get(actor_index);
                    if (weapon && weapon->bombardment_range > 0.0f)
                        search_time = 10.0f;
                    if (search_view->sighting_awareness >= 2 || search_view->sighting_count > 0) {
                        search_view->sighting_count++;
                        if (game_ticks_to_seconds((float)search_view->sighting_count) >= search_time)
                            search_view->flags |= FLAG(6);
                    }
                }
                if (prop_ref->awareness > 0) {
                    prop_ref->accumulated_awareness = 0.0f;
                    prop_ref->awareness_class = 4;
                } else if (search_view) {
                    short decay;
                    if (iterator.index == actor->target_prop_index)
                        decay = actor_perception_search_decay(search_view);
                    else
                        decay = actor_perception_search_decay(search_view) | 1;
                    search_view->sighting_ticks -= decay;
                    if (search_view->sighting_ticks < 0) {
                        actor_flush_prop(actor_index, iterator.index);
                        prop_unacknowledge(actor_index, iterator.index);
                    }
                }
            } else {
                actor_flush_prop(actor_index, iterator.index);
                prop_unacknowledge(actor_index, iterator.index);
            }
            break;
        }

        if (refresh_state)
            actor_prop_state_refresh(actor_index, iterator.index, &sense_position, &target_position, false);
        if (prop_ref->awareness_class >= 1)
            actor_stimulus_allegiance_update(actor_index, iterator.index);
        if (prop_ref->state_refresh_ticks > highest_state_refresh_ticks)
            highest_state_refresh_ticks = prop_ref->state_refresh_ticks;
    }

    if (inl_target_get_prop_class(actor) >= 6)
        actor->target_flags |= FLAG(_actor_flag_threatening_target);
    actor->highest_status_refresh_ticks = highest_status_refresh_ticks;
    actor->highest_state_refresh_ticks = highest_state_refresh_ticks;
}
