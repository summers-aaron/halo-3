// ailibxbox/ai_actions.obj: from the records 0xc7428 0x1963b0 0x54ebd8 0x54efa0 0x54f518 0x54f7d0 0x54f8b0 0x54f958 0x54fa38 0x54fd10 0x5501d8 0x5502d8 0x5504f0 0x5505c0
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "ai/ai_information_data.h"
#include "tag_files/tag_types.h"
#include "units/weapon_addition_method_types.h"
#include "ai/ai_direction_mode_types.h"
#include "objects/object_reference_types.h"
#include "math/real_math_types.h"
#include "math/matrix_math_types.h"
#include "ai/sector_ref.h"
#include "game/game_types.h"
#include "objects/object_placement.h"
#include "units/motor_enums.h"
#include "units/action_request.h"
#include "ai/ai_point3d.h"
#include "ai/ai_direction.h"
#include "ai/prop_ref_datum.h"
#include "ai/prop_state.h"
#include "math/real_math.h"
#include <math.h>
#include "ai/actor_datum.h"
#include "ai/actors.h"
#include "motor/action_system.h"
#include "motor/motor_animation.h"
#include "objects/objects.h"
#include "units/units.h"
#include "ai/actor_combat.h"
#include "ai/ai.h"
#include "ai/ai_hint_definitions.h"
#include "ai/props.h"
#include "items/weapons.h"
#include "objects/widgets/muffins.h"
#include "units/unit_dialogue.h"
#include "ai/activities.h"
#include "ai/character_definitions.h"
#include "ai/character_properties_types.h"
#include "objects/object_datum.h"
#include "game/game_allegiance.h"
#include "memory/tls_globals.h"
#include "units/unit_types.h"
#include "objects/object_types.h"
#include "ai/weapon_datum_types.h"
#include "cache/cache_files.h"
#include "cseries/cseries.h"
#include "game/game_time_inlines.h"
#include <string.h>
#include "ai/ai_reference_frame.h"
#include "ai/sector_analysis.h"
#include "ai/behavior_posture.h"
#include "ai/ai_dialogue.h"
#include "units/biped_types.h"
#include "units/biped_definitions.h"
#include "animations/animation_manager.h"
#include "animations/animation_definitions_internal.h"
#include "ai/character_behavior_properties.h"
#include "game/game_time_inline.h"
#include "ai/dialogue_event.h"
#include "objects/object_header_access.h"
#include "memory/data.h"
#include "units/motor_program_state.h"
#include "objects/object_header.h"
#include "motor/mover_inline.h"
#include "tag_files/tag_groups.h"

// NB: actor_evade (0x54fa38) needs the DATUM_INDEX_TO_ABSOLUTE_INDEX macro form
// from memory/data_types.h; datum_get/object_header_get break its bytes.
typedef char ai_actions_manager_flags_offset[(offsetof(c_animation_manager, m_flags) == 0x80) ? 1 : -1];
typedef char ai_actions_manager_state_offset[(offsetof(c_animation_manager, m_current_goal.state) == 0x90) ? 1 : -1];
typedef char ai_actions_manager_interpolator_offset[(offsetof(c_animation_manager, m_interpolator) == 0x68) ? 1 : -1];
typedef char ai_actions_biped_reanimation_offset[(offsetof(biped_datum, biped.swarm_actor_index) == 0x6ac) ? 1 : -1];
typedef char ai_actions_reanimation_character_offset[(offsetof(biped_definition, reanimation_character.index) == 0x458) ? 1 : -1];
typedef char ai_actions_unit_resurrection_byte_offset[(offsetof(unit_datum, unit.berserk_charges) == 0x240) ? 1 : -1];


static __forceinline bool ai_direction_actor_path_executing(long actor_index)
{
    bool executing = false;
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    if (actor->path.valid && actor->path_state == 1)
        executing = true;
    return executing;
}


static __forceinline void ai_direction_object_get_position(long object_index, bool aiming, real_point3d *position)
{
    unit_datum *object = (unit_datum *)object_get(object_index);
    if (((1 << object->object.type) & 0x1003) != 0 && !aiming)
    {
        if (object->unit.player_index != NONE)
            unit_get_camera_position(object_index, position);
        else
            unit_get_head_position(object_index, position);
    }
    else
    {
        ai_get_center_of_mass(object_index, position);
    }
}

// @address 0x1963b0 ?get@c_ai_direction@@QAA_NJW4e_ai_direction_mode@@PBTreal_point3d@@PATreal_vector3d@@PA_NPAT3@@Z
bool c_ai_direction::get(long actor_index, e_ai_direction_mode mode, const real_point3d *point, real_vector3d *direction, bool *unknown, real_point3d *result)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    bool aiming = (mode == 3);
    bool success = false;
    bool has_point = false;
    real_point3d target_point;

    switch (type)
    {
    case NONE:
        success = false;
        break;

    case 0:
        if (TEST_BIT(actor->flags, 0))
        {
            *direction = actor->throttle;
            if (ai_direction_actor_path_executing(actor_index) &&
                actor->root_vehicle_index == NONE &&
                actor->behavior_mode == 4)
            {
                actor_path *path = &actor->path;
                if (path->point_index < path->point_count - 1)
                {
                    const float &magnitude = magnitude3d(&actor->throttle);
                    if (magnitude < 1.0f)
                    {
                        ai_vector_from_points3d(&path->points[path->point_index].point,
                            &path->points[path->point_index + 1].point, direction);
                    }
                }
            }
            success = true;
        }
        break;

    case 2:
        if (actor->target_prop_index != NONE)
        {
            prop_ref_datum *prop_ref = datum_get<prop_ref_datum>(prop_ref_data, actor->target_prop_index);
            prop_state *state = prop_state_get(prop_ref);
            has_point = true;
            success = true;
            if (prop_ref->status >= 1 && prop_ref->status <= 2)
            {
                if (aiming)
                {
                    long object_index = prop_get_target_object_index(prop_ref->prop_index);
                    if ((_rotl(actor->target_flags, 31) & 1) == 0)
                    {
                        actor_target_choose_target_marker(actor_index, object_index);
                        actor->target_flags |= FLAG(1);
                    }
                    object_marker marker;
                    if (actor->target_marker_name != 0 &&
                        internal_object_get_markers_by_string_id(object_index, actor->target_marker_name, &marker, 1, false) != 0)
                    {
                        target_point = marker.matrix.position;
                    }
                    else
                    {
                        target_point = state->center;
                    }
                }
                else
                {
                    unit_datum *object = (unit_datum *)object_get(prop_ref->object_index);
                    if (((1 << object->object.type) & 0x1003) != 0 && object->unit.player_index != NONE)
                        unit_get_camera_position(prop_ref->object_index, &target_point);
                    else
                        target_point = state->head_position;
                }
            }
            else if (state->flags != NONE)
            {
                ai_point_get_position(&state->pathfinding_point, &target_point);
                if (!aiming)
                    actor_estimate_position(prop_ref->object_index, 1, &target_point, NULL, NULL, NULL, &target_point);
            }
            else
            {
                success = false;
            }
        }
        break;

    case 1:
        if (object_index != NONE)
        {
            prop_ref_datum *prop_ref = datum_get<prop_ref_datum>(prop_ref_data, object_index);
            prop_state *state = prop_state_get(prop_ref);
            has_point = true;
            if (prop_ref->status >= 1 && prop_ref->status <= 2)
            {
                ai_direction_object_get_position(prop_ref->object_index, aiming, &target_point);
            }
            else if (prop_ref->status >= 1 && state->flags != NONE)
            {
                ai_point_get_position(&state->pathfinding_point, &target_point);
                if (!aiming)
                    actor_estimate_position(prop_ref->object_index, 1, &target_point, NULL, NULL, NULL, &target_point);
            }
            else
            {
                success = false;
                break;
            }
            success = true;
        }
        break;

    case 3:
        target_point = this->point;
        has_point = true;
        success = true;
        break;

    case 4:
        ai_point_get_position(&m_point, &target_point);
        has_point = true;
        success = true;
        break;

    case 5:
        *direction = vector;
        success = true;
        break;

    case 6:
        if (actor->danger_zone.danger_type > 0)
        {
            target_point = actor->danger_zone.position;
            has_point = true;
            success = true;
        }
        break;

    case 7:
        if (object_index != actor->unit_index && object_index != NONE)
        {
            ai_direction_object_get_position(object_index, aiming, &target_point);
            has_point = true;
            success = true;
        }
        break;
    }

    if (success)
    {
        if (mode == 3 && has_point)
        {
            long weapon_index = actor_get_weapon(actor_index);
            real_point3d origin;
            if (point)
                origin = *point;
            else
                actor_get_gun_position(actor_index, !TEST_BIT(actor->flags, 13), &origin);

            if (weapon_index != NONE)
                success = actor_weapon_aim(actor_index, weapon_index, 0, &origin, &target_point, false, direction, NULL, NULL, NULL, NULL);
            else
                success = false;

            if (!success)
            {
                direction->i = target_point.x - origin.x;
                direction->j = target_point.y - origin.y;
                direction->k = target_point.z - origin.z;
                success = normalize3d(direction) > 0.0f;
            }
        }
        else
        {
            if (has_point)
            {
                const real_point3d *origin;
                switch (mode)
                {
                case 0:
                case 1:
                    origin = &actor->position.body;
                    break;
                case 2:
                default:
                    origin = &actor->position.head;
                    break;
                }
                direction->i = target_point.x - origin->x;
                direction->j = target_point.y - origin->y;
                direction->k = target_point.z - origin->z;
            }
            if (mode == 1 && actor->unit_index != NONE)
            {
                if (mover_has_variable_camera_up(actor->unit_index))
                {
                    unit_datum *unit = (unit_datum *)object_get(actor->unit_index);
                    real_vector3d side;
                    cross_product3d(&unit->object.up, direction, &side);
                    cross_product3d(&side, &unit->object.up, direction);
                }
                else
                {
                    direction->k = 0.0f;
                }
            }
            success = normalize3d(direction) > 0.0f;
        }
    }

    if (success && has_point)
    {
        if (unknown && result)
        {
            *unknown = true;
            *result = target_point;
        }
    }
    return success;
}

// @address 0x54ebd8 ?get_entry_point@c_ai_action@@QAA_NJPBVc_ai_point3d@@PBVc_sector_ref@@PAV2@PATreal_vector3d@@PAV3@@Z
bool c_ai_action::get_entry_point(long actor_index, const c_ai_point3d *point, const c_sector_ref *sector, c_ai_point3d *out_point,
    real_vector3d *out_vector, c_sector_ref *out_sector)
{
    bool result = false;
    c_sector_ref tmp_sector;
    real_vector3d tmp_vector;
    real_point3d tmp_a;
    real_point3d tmp_b;
    c_ai_point3d tmp_out;
    switch (type - 1)
    {
    case 1:
        if (this->point.location[0] != point->location[0] || !point_valid)
            break;
        if (direction.type == -1)
            break;
        if (!direction.get(actor_index, _ai_direction_mode_vector, 0, &tmp_vector, 0, 0))
            break;
        if (!runtime_cornering_get_entry(actor_index, point, *sector, &this->point, &tmp_vector, corner_type, corner_flag, &tmp_out, &tmp_sector))
            break;
        result = true;
        break;
    case 0:
        if (this->point.location[0] != point->location[0] || !point_valid)
            break;
        if (direction.type == -1)
            break;
        ai_point_get_position(&this->point, &tmp_a);
        if (!direction.get(actor_index, _ai_direction_mode_vector, 0, &tmp_vector, 0, 0))
            break;
        if (!ai_reference_frame_transform_vector(this->point.location[0], &tmp_vector, &tmp_vector))
            break;
        if (!ai_hint_find_root_entry_point(actor_index, posture_name, 84, &tmp_a, &tmp_vector, 0, &tmp_out, 0, &tmp_sector))
            break;
        result = true;
        break;
    case 2:
        if (this->point.location[0] != point->location[0] || !point_valid)
            break;
        if (direction.type == -1)
            break;
        ai_point_get_position(&this->point, &tmp_a);
        if (!direction.get(actor_index, _ai_direction_mode_vector, 0, &tmp_vector, 0, 0))
            break;
        if (!ai_hint_find_pill_entry_point(actor_index, 479, 84, 476, 12, &tmp_a, &tmp_vector, &tmp_b, 0, &tmp_vector, 0))
            break;
        tmp_out = this->point;
        if (!ai_reference_frame_inverse_transform_point(this->point.location[0], &tmp_b, &tmp_out.point))
            break;
        tmp_sector.structure_bsp_index = -1;
        tmp_sector.sector_index = 0xffff;
        result = true;
        break;
    case 3:
        if (!point_valid)
            break;
        ai_point_get_position(&this->point, &tmp_b);
        if (!ai_hint_find_pill_entry_point(actor_index, 476, 687, 476, 12, &tmp_b, global_up3d, &tmp_a, 0, &tmp_vector, 0))
            break;
        tmp_out = this->point;
        if (!ai_reference_frame_inverse_transform_point(this->point.location[0], &tmp_a, &tmp_out.point))
            break;
        tmp_sector = *sector;
        result = true;
        break;
    default:
        // match: 0x54ebd8: unreachable hint omits dead branch check
        __assume(0);
    }
    if (result)
    {
        if (out_point)
            *out_point = tmp_out;
        if (out_sector)
            *out_sector = tmp_sector;
        if (out_vector)
            *out_vector = tmp_vector;
    }
    return result;
}

// @address 0x54f7d0 ?actor_stow_weapon@@YAXJ_N0@Z
void actor_stow_weapon(long actor_index, bool stow, bool force)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    if (actor->weapon_stowed == stow)
        return;

    if (!force && actor->berserking != 0)
        return;

    if (actor->unit_index != -1)
    {
        bool can_stow = true;
        if (stow)
        {
            unit_datum *unit = (unit_datum *)object_get(actor->unit_index);
            if (unit->unit.current_weapon_index != -1)
            {
                long weapon_index = unit->unit.weapon_object_indices[unit->unit.current_weapon_index];
                if (weapon_index != -1)
                    can_stow = weapon_can_be_stowed(weapon_index, actor->unit_index);
            }
        }

        if (can_stow)
            actor->weapon_stowed = stow;
    }
}

bool actor_exit_posture(long actor_index)
{
    action_request request;
    bool result = false;
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    if (actor->unit_index != -1)
    {
        memset(&request, 0, sizeof(request));
        request.action = 12;
        request.bytes.a = actor->behavior_mode < 4 ? 1 : 0;
        request.bytes.b = actor->active == 0 ? 1 : 0;
        result = action_submit(actor->unit_index, &request);
    }
    return result;
}

// @address 0x54f958 actor_can_fire_while_moving
extern "C" unsigned char actor_can_fire_while_moving(unsigned short actor_index)
{
    bool result = false;
    long object_index = datum_get<actor_datum>(actor_data, actor_index)->unit_index;
    if (object_index != -1 && action_executing(object_index, _action_ai_impulse))
    {
        object_datum *object = (object_datum *)object_get(object_index);
        c_animation_manager *definition = (c_animation_manager *)object_header_block_get(object, &object->animation);
        unsigned char supports_action = (definition->m_flags >> 6) & 1;
        if (supports_action)
        {
            long type = definition->m_current_goal.state;
            if (type == 54 || type == 55 || type == 56 || type == 57)
                result = true;
        }
    }
    return result;
}

extern "C" {
// @address 0x239e0 k_behavior_posture_info
}

// @address 0x54fd10 ai_posture_allow_automatic_exit
extern "C" bool ai_posture_allow_automatic_exit(unsigned short actor_index)
{
    bool result = true;
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    long ticks = (long)((float)TLS_GAME_TIME_GLOBALS->ticks_per_second * 1.5f + 0.5f);
    if (TLS_GAME_TIME_GLOBALS->time - actor->creation_time < ticks)
        result = false;
    else
    {
        long posture = actor_get_posture(actor_index);
        if (posture > 0)
            result = k_behavior_posture_info[posture].automatic_exit_blocked == 0;
    }
    return result;
}

bool ai_resurrect_get_marker(long object_index, object_marker *marker, bool *marker_up)
{
    bool result = false;
    bool up = false;
    object_marker local_marker;

    if (!marker)
    {
        marker = &local_marker;
    }

    if (internal_object_get_markers_by_string_id(object_index, 0x250, marker, 1, false) > 0)
    {
        result = true;
        if (dot_product3d(&marker->matrix.forward, global_up3d) > 0.0f)
        {
            up = true;
        }
    }

    if ((!result || !up) && internal_object_get_markers_by_string_id(object_index, 0x251, marker, 1, false) > 0)
    {
        result = true;
        if (dot_product3d(&marker->matrix.forward, global_up3d) > 0.0f)
        {
            up = false;
        }
    }

    if (marker_up)
    {
        *marker_up = up;
    }

    return result;
}

// @address 0x5504f0 object_get_action_points
extern "C" void object_get_action_points(long object_index, object_placement_data *out)
{
    object_datum *object = (object_datum *)object_get(object_index);
    real_rgb_color *colors = (real_rgb_color *)object_header_block_get(object, &object->change_colors);
    if (colors)
    {
        short count = (short)((int)(object->change_colors.size / sizeof(real_rgb_color)) / 2);
        for (short i = 0; i < count; i++)
        {
            out->change_colors[i] = colors[i];
            out->active_change_colors |= 1 << i;
        }
    }
}



static __forceinline void ai_actions_actor_scream(long actor_index, short scream_type)
{
    long unit_index = datum_get<actor_datum>(actor_data, actor_index)->unit_index;
    if (unit_index != -1)
        ::unit_scream(unit_index, scream_type, -1, 13, 0, 0);
}

static __forceinline unsigned long ai_actions_teams_are_friendly(e_game_team team_a, e_game_team team_b)
{
    if (team_a == team_b)
        return true;
    return game_team_is_ally(team_a, team_b);
}
// @address 0x5502d8 ?actor_resurrect@@YAJJFFF@Z
long actor_resurrect(long unit_index, short team, short resurrect_type, short squad_index)
{
    long actor_index = -1;
    biped_datum *unit = (biped_datum *)object_get(unit_index);
    long character_index;
    if (unit->biped.swarm_actor_index != -1 && ai_actions_teams_are_friendly((e_game_team)unit->unit.team, (e_game_team)team))
        character_index = unit->biped.swarm_actor_index;
    else
        character_index =
            ((biped_definition *)tag_get(0, unit->object.definition_index))
                ->reanimation_character.index;
    if (character_index != -1) {
        actor_index = ::actor_create(unit_index, character_index, squad_index, false);
        if (actor_index != -1) {
            if (squad_index == -1) {
                actor_set_active(actor_index, true);
                actor_set_team(actor_index, team);
            }
            action_request request;
            memset(&request, 0, sizeof(request));
            request.action = 5;
            request.resurrect.ticks = (short)game_seconds_integer_to_ticks(3);
            request.resurrect.type = resurrect_type;
            if (action_submit(unit_index, &request)) {
                unit = (biped_datum *)object_get(unit_index);
                if (*(long *)((char *)unit + unit->unit.dialogue_reference.offset) == -1)
                    actor_select_character_voice(character_index, unit_index);
                actor_customize_unit(character_index, unit_index, -1, -1, -1, -1);
                ai_actions_actor_scream(actor_index, 32);
                c_dialogue_event event(100, unit_index, -1, -1, -1, 0);
                ai_dialogue_event(&event);
            } else {
                actor_delete(actor_index, false);
                actor_index = -1;
            }
        }
    }
    return actor_index;
}

// @address 0x54f518 actor_set_berserk
extern "C" void actor_set_berserk(long actor_index, bool berserk, bool latch)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    if (berserk == (unsigned char)actor->pending_berserking)
        return;
    character_charge_properties *charge = character_definition_charge_multi_properties_get(actor_index);
    actor->pending_berserking = berserk;
    bool found = false;
    actor->berserk_ticks = -1;
    if (latch)
        actor->berserking = berserk;
    if (charge)
    {
        if (berserk)
            actor->berserk_ticks = (short)(int)((float)TLS_GAME_TIME_GLOBALS->ticks_per_second * charge->berserk_duration + 0.5f);
        if (actor->unit_index != -1)
        {
            long object_index = -1;
            if (berserk)
            {
                if (charge->berserk_weapon.index != -1)
                {
                    unit_datum *unit = (unit_datum *)object_get(actor->unit_index);
                    for (short i = 0; i < 4; i++)
                    {
                        long weapon_index = unit->unit.weapon_object_indices[i];
                        if (weapon_index != -1
                            && ((weapon_datum *)object_get(weapon_index))->definition_index == charge->berserk_weapon.index)
                        {
                            found = true;
                            unit->unit.desired_weapon_indices[0] = (char)i;
                            break;
                        }
                    }
                    if (!found)
                    {
                        object_placement_data placement;
                        object_placement_data_new(&placement, charge->berserk_weapon.index, actor->unit_index, 0);
                        object_index = object_new(&placement);
                        if (object_index != -1 && !unit_add_weapon_to_inventory(actor->unit_index, object_index, _weapon_addition_method_silent))
                        {
                            object_delete(object_index);
                            object_index = -1;
                        }
                    }
                }
            }
            else
            {
                object_index = unit_get_current_backpack_weapon(actor->unit_index);
            }
            if (object_index != -1)
            {
                unit_datum *unit = (unit_datum *)object_get(actor->unit_index);
                for (short i = 0; i < 4; i++)
                {
                    if (unit->unit.weapon_object_indices[i] == object_index)
                    {
                        unit->unit.desired_weapon_indices[0] = (char)i;
                        found = true;
                        actor->weapon_stowed = false;
                        break;
                    }
                }
            }
        }
    }
    if (!found)
        actor->weapon_stowed = berserk && (actor->weapon_lowered != 0 || actor_has_ranged_weapon(actor_index));
    if (!actor->swarm)
    {
        unit_datum *unit = (unit_datum *)object_get(actor->unit_index);
        if (berserk)
            unit->unit.unit_flags |= 32;
        else
            unit->unit.unit_flags &= ~32;
    }
}

static __forceinline long ai_actions_seconds_to_ticks_round(float seconds)
{
    return (long)((float)TLS_GAME_TIME_GLOBALS->ticks_per_second * seconds + 0.5f);
}
// @address 0x54efa0 ?execute@c_ai_action@@QAA?AW4e_ai_action_result@@J@Z
e_ai_action_result c_ai_action::execute(long actor_index)
{
    actor_datum *actor = &((actor_datum *)actor_data->data)[(unsigned short)actor_index];
    e_ai_action_result result = _ai_action_result_2;
    action_request request1, request2, request3, request4;
    if (type != 0)
    {
        switch (type)
        {
        case 1:
            if (actor->unit_index != -1)
            {
                behavior_posture_info *posture = activities_try_and_get_posture(posture_name);
                if (posture)
                {
                    memset(&request1, 0, sizeof(request1));
                    request1.action = 11;
                    request1.posture.posture = posture_name;
                    request1.posture.a = false;
                    request1.posture.b = posture->locked;
                    if (point_valid && direction.type != -1)
                    {
                        ai_point_get_position(&point, &request1.posture.position);
                        direction.get(actor_index, _ai_direction_mode_vector, 0, &request1.posture.forward, 0, 0);
                        ai_reference_frame_transform_vector(point.reference_frame(), &request1.posture.forward, &request1.posture.forward);
                        request1.posture.has_position = true;
                    }
                    else
                        request1.posture.has_position = false;
                    if (action_submit(actor->unit_index, &request1))
                        result = _ai_action_result_1;
                }
            }
            break;
        case 2:
            if (actor->unit_index != -1)
            {
                memset(&request2, 0, sizeof(request2));
                request2.action = 24 + (corner_flag != 0);
                request2.vectors.value = corner_type;
                if (point_valid && direction.type != -1)
                {
                    ai_point_get_position(&point, &request2.vectors.position);
                    direction.get(actor_index, _ai_direction_mode_vector, 0, &request2.vectors.velocity, 0, 0);
                    ai_reference_frame_transform_vector(
                        point.reference_frame(), &request2.vectors.velocity, &request2.vectors.velocity);
                }
                else
                {
                    request2.vectors.position = actor->position.body;
                    memcpy(&request2.vectors.velocity, &actor->facing_vector, sizeof(actor->facing_vector));
                }
                if (action_submit(actor->unit_index, &request2))
                    result = _ai_action_result_1;
            }
            break;
        case 3:
            if (point_valid && direction.type != -1)
            {
                memset(&request3, 0, sizeof(request3));
                request3.action = 28;
                request3.motion.value = 0;
                if (direction.get(actor_index, _ai_direction_mode_vector, 0, &request3.motion.velocity, 0, 0))
                {
                    ai_point_get_position(&point, &request3.motion.position);
                    if (action_submit(actor->unit_index, &request3))
                        result = _ai_action_result_1;
                }
            }
            break;
        case 4:
        {
            memset(&request4, 0, sizeof(request4));
            request4.action = 30;
            ai_point_get_position(&point, &request4.vectors.position);
            memcpy(&request4.vectors.velocity, &actor->facing_vector, sizeof(actor->facing_vector));
            if (action_submit(actor->unit_index, &request4))
                result = _ai_action_result_1;
            break;
        }
        case 5:
        {
            bool ready = true;
            actor->flags |= 0x200;
            actor->flags &= ~1;
            actor->shooting_suppression_ticks = (short)ai_actions_seconds_to_ticks_round(2.0f);
            result = _ai_action_result_0;
            if (direction.type != -1)
            {
                real_vector3d forward;
                if (direction.get(actor_index, _ai_direction_mode_vector, 0, &forward, 0, 0))
                    { const float &threshold = 0.98f;
                    ready = dot_product3d(&actor->facing_vector, &forward) > threshold; }
                else
                    result = _ai_action_result_2;
            }
            if (result == _ai_action_result_0)
            {
                if (ready)
                    result = (e_ai_action_result)((actor_impulse_submit(actor_index, impulse_index, 0, 0) == 0) + 1);
                else
                {
                    actor->flags |= 2;
                    actor->look_mode = 4;
                    actor->look_direction = direction;
                    actor->path_aim = true;
                }
            }
            break;
        }
        case 9:
            if (direction.type != -1)
            {
            if (--ticks <= 0)
                result = _ai_action_result_1;
            else
            {
                actor->flags |= 0x200;
                actor->flags &= ~1;
                actor->shooting_suppression_ticks = (short)ai_actions_seconds_to_ticks_round(2.0f);
                actor->flags |= 2;
                actor->look_mode = 4;
                actor->look_direction = direction;
                actor->path_aim = true;
                result = _ai_action_result_0;
            }
            }
            else
                result = _ai_action_result_2;
            break;
        default:
            UNREACHABLE();
        }
    }
    return result;
}


static __forceinline void ai_actions_action_clear(c_ai_action *action)
{
    action->type = 0;
    action->point_valid = false;
    action->b5 = false;
    action->ticks = 0;
    action->impulse_index = 0;
    action->point.set(global_origin3d);
    action->direction.clear();
}

// @address 0xc7428 ?process_pending_ground_actions@@YA_NJ@Z
bool process_pending_ground_actions(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    bool result = false;

    if (actor->unit_index != NONE)
    {
        const long &weapon_index = actor_get_weapon(actor_index);
        if (weapon_index != NONE)
            actor->weapon_lowered = false;
    }

    if (actor->unit_index != NONE)
    {
        long motor_program = mover_get_motor_program(actor->unit_index);
        if (motor_program == 0)
        {
            bool berserk = actor->pending_berserking;
            if (actor->berserking != berserk)
            {
                if (berserk)
                {
                    action_request request;
                    csmemset(&request, 0, sizeof(request));
                    request.action = 36;
                    request.immediate = unit_get_current_backpack_weapon(actor->unit_index) == NONE && actor_has_ranged_weapon(actor_index);
                    if (action_submit(actor->unit_index, &request))
                    {
                        ai_actions_actor_scream(actor_index, 30);
                        c_dialogue_event event(97, actor->unit_index, NONE, NONE, NONE, NULL);
                        ai_dialogue_event(&event);
                    }
                    result = true;
                }
                actor->berserking = actor->pending_berserking;
            }
        }
    }

    if (!motor_animation_busy(actor->unit_index))
    {
        if (!result && actor->weapon_lowered != actor->weapon_stowed)
        {
            if (actor->weapon_stowed)
            {
                action_submit_event(actor->unit_index, 50);
                actor->weapon_lowered = true;
            }
            else
            {
                bool facing = true;
                c_ai_direction direction;
                csmemset(&direction, 0, sizeof(direction));
                if (actor->target_prop_index != NONE && actor->command_script_index == NONE && actor->root_vehicle_index == NONE)
                {
                    direction.clear();
                    direction.to_target();
                    real_vector3d vector;
                    if (direction.get(actor_index, _ai_direction_mode_vector, NULL, &vector, NULL, NULL))
                        facing = dot_product3d(&actor->facing_vector, &vector) > 0.9f;
                }
                if (facing)
                {
                    action_request request;
                    csmemset(&request, 0, sizeof(request));
                    request.immediate = !actor_should_lower_weapon(actor_index);
                    request.action = 51;
                    if (action_submit(actor->unit_index, &request))
                        actor->weapon_lowered = false;
                }
                else if (direction.type != NONE)
                {
                    actor->flags |= 2;
                    actor->look_mode = 4;
                    actor->look_direction = direction;
                    actor->path_aim = true;
                }
            }
            result = true;
        }

        if (!result && mover_get_motor_program(actor->unit_index) == 0)
        {

            if (actor->action.type != 0 && actor->action.execute(actor_index) != _ai_action_result_0)
                ai_actions_action_clear(&actor->action);
        }
    }
    return result;
}

static __forceinline real_vector3d *ai_actions_project_vector_onto_plane3d(const real_vector3d *vector, const real_plane3d *plane, real_vector3d *result)
{
    real_vector3d side;
    cross_product3d(&plane->normal, vector, &side);
    cross_product3d(&side, &plane->normal, result);
    return result;
}



// match: 0x54fa38: shared mover helpers change register allocation (35 words)
static __forceinline unit_datum *ai_direction_object_get(long object_index)
{
    return (unit_datum *)((s_object_header *)object_header_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(object_index)].datum;
}

static __forceinline long ai_actions_mover_get_motor_program_state(long unit_index)
{
    unit_datum *unit = ai_direction_object_get(unit_index);
    long *motor_program = (long *)((char *)unit + unit->unit.motor_program.offset);
    return *motor_program;
}

// Local twin of mover_has_variable_camera_up (motor/mover_inline.h): the shared
// spelling changes actor_evade's register allocation (35 words differ).
static __forceinline bool ai_actions_mover_has_variable_camera_up(long unit_index)
{
    bool variable_up = false;
    long motor_program = ai_actions_mover_get_motor_program_state(unit_index);
    if (motor_program == 7 || motor_program == 8)
        variable_up = true;
    return variable_up;
}

// @address 0x54fa38 ?actor_evade@@YA_NJ@Z
bool actor_evade(long actor_index)
{
    actor_datum *actor = &((actor_datum *)actor_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(actor_index)];
    bool result = false;

    if (actor->root_vehicle_index == NONE && !actor_move_animation_busy(actor_index) &&
        actor->target_prop_index != NONE && !actor->airborne && actor->unit_index != NONE)
    {
        unit_datum *unit = (unit_datum *)((s_object_header *)object_header_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(actor->unit_index)].datum;
        prop_ref_datum *prop_ref = &((prop_ref_datum *)prop_ref_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(actor->target_prop_index)];
        prop_state *state = prop_state_get(prop_ref);

        real_vector3d avoidance_vector;
        vector_from_points3d(&actor->position.body, &state->position, &avoidance_vector);

        real_vector3d alignment_vector;
        if (ai_actions_mover_get_motor_program_state(actor->unit_index) != 4)
        {
            real_plane3d plane;
            plane3d_from_point_and_normal(&plane, &actor->position.body, &unit->object.up);
            ai_actions_project_vector_onto_plane3d(&avoidance_vector, &plane, &alignment_vector);
        }
        else
        {
            alignment_vector = avoidance_vector;
        }
        normalize3d(&alignment_vector);
        normalize3d(&avoidance_vector);

        float facing_dot;
        if (ai_actions_mover_has_variable_camera_up(actor->unit_index))
            facing_dot = 1.0f;
        else
            facing_dot = dot_product3d(&alignment_vector, &actor->facing_vector);

        if (facing_dot > 0.4f)
        {
            short evade_direction = ai_actions_mover_has_variable_camera_up(actor->unit_index) ? 5 : 4;
            bool blocked;
            if (actor_move_find_evasion_direction(actor_index, &avoidance_vector, &alignment_vector, &evade_direction, 0.0f, &blocked))
            {
                long impulse;
                switch (evade_direction)
                {
                case 0:
                    impulse = 54;
                    break;
                case 1:
                    impulse = 55;
                    break;
                case 2:
                    impulse = 56;
                    break;
                case 3:
                    impulse = 57;
                    break;
                default:
                    UNREACHABLE();
                }
                result = actor_impulse_submit(actor_index, impulse, 0, &alignment_vector);
            }
        }
    }
    return result;
}

struct character_vitality_properties;

// @rdata 0x82021694
static const float k_ai_actions_resurrection_max_distance = 3.402823466e+38f;
// @rdata 0x820221fc
static const float k_ai_actions_resurrection_search_radius = 1.0f;

static __forceinline long ai_actions_resurrection_weapon_get(long unit_index, short inventory_index)
{
    unit_datum *unit = (unit_datum *)object_get(unit_index);
    long weapon_index = -1;
    if (inventory_index != -1)
        weapon_index = unit->unit.weapon_object_indices[inventory_index];
    return weapon_index;
}

static __forceinline float ai_actions_resurrection_distance3d(const real_point3d *a, const real_point3d *b)
{
    float x = b->x - a->x;
    float y = b->y - a->y;
    float z = b->z - a->z;
    return sqrtf(y * y + (z * z + x * x));
}

// @address 0x5505c0
bool ai_handle_resurrection(long unit_index)
{
    unit_datum *unit = (unit_datum *)object_get(unit_index);
    bool result = false;
    if (unit->unit.actor_index != -1)
    {
        long actor_index = unit->unit.actor_index;
        actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
        if (actor->character_index != -1)
        {
            character_definition *character =
                (character_definition *)tag_get(0, actor->character_index);
            if (character->unit.index != -1 && character->unit.index != unit->object.definition_index)
            {
                object_placement_data placement;
                long new_unit_index = -1;
                long old_unit_index = unit_index;
                object_placement_data_new(&placement, character->unit.index, -1, 0);
                object_get_origin(old_unit_index, &placement.position);
                object_get_orientation(old_unit_index, &placement.forward, &placement.up);
                placement.model_variant_index = 0;
                object_get_action_points(old_unit_index, &placement);
                new_unit_index = object_new(&placement);
                if (new_unit_index != -1)
                {
                    actor_select_character_voice(actor->character_index, new_unit_index);
                    actor_customize_unit(actor->character_index, new_unit_index, -1, -1, -1, -1);
                    actor_attach_unit(actor_index, new_unit_index);
                    actor_impulse_submit(actor_index, 72, 0, 0);
                    unit_datum *new_unit = (unit_datum *)object_get(new_unit_index);
                    c_animation_manager *animation = (c_animation_manager *)object_header_block_get(new_unit, &new_unit->object.animation);
                    animation->m_interpolator.m_time = 0.0f;
                    animation->m_interpolator.m_flags = 0;
                    animation->m_interpolator.m_duration = 0.0f;
                    object_wake(new_unit_index);
                    object_compute_node_matrices(new_unit_index);
                    if (object_has_muffins(old_unit_index))
                        muffins_transfer_to_new_object(old_unit_index, new_unit_index);
                    object_delete(old_unit_index);
                    result = true;
                }
            }
        }
        unit_datum *inventory_unit = (unit_datum *)object_get(unit_index);
        if (ai_actions_resurrection_weapon_get(unit_index, inventory_unit->unit.current_weapon_index) == -1)
        {
            float nearest_distance = k_ai_actions_resurrection_max_distance;
            long nearest_weapon_index = -1;
            long weapon_indices[8];
            short weapon_count = objects_in_sphere(
                0, 4, &actor->position.location, &actor->position.head, k_ai_actions_resurrection_search_radius, weapon_indices, 8);
            for (short i = 0; i < weapon_count; i++)
            {
                long weapon_index = weapon_indices[i];
                unit_datum *weapon = (unit_datum *)object_get(weapon_index);
                float distance = ai_actions_resurrection_distance3d(&actor->position.body, &weapon->object.position);
                if (distance < nearest_distance)
                {
                    nearest_distance = distance;
                    nearest_weapon_index = weapon_index;
                }
            }
            if (nearest_weapon_index == -1 && actor->character_index != -1)
            {
                character_vitality_properties *vitality =
                    character_definition_vitality_properties_get(actor->character_index);
                if (vitality && vitality->resurrection_weapon.index != -1)
                {
                    object_placement_data placement;
                    object_placement_data_new(&placement, vitality->resurrection_weapon.index, unit_index, 0);
                    nearest_weapon_index = object_new(&placement);
                }
            }
            if (nearest_weapon_index != -1)
                unit_add_weapon_to_inventory(unit_index, nearest_weapon_index, _weapon_addition_method_silent);
        }
        unit->unit.berserk_charges = false;
    }
    return result;
}
