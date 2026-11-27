// ailibxbox/ai_atoms.obj: from the records 0xb71f8 0x565048 0x565068 0x565088 0x5650c0 0x565518 0x5656d8 0x5659b0 0x565ae8 0x565cc0 0x565f50 0x565fb0 0x566388 0x5664f8 0x566530 0x566708 0x5667c0 0x5669e8 0x566b18 0x566ba0 0x566bd8 0x566ca8 0x566cf0 0x566d88 0x566e08 0x566e48 0x566eb0 0x566ef0 0x566fd0 0x567088 0x567160 0x5675d8 0x567630 0x567668 0x5676c0 0x567718 0x5678a8 0x567908 0x567a00
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "units/action_request.h"
#include "objects/object_header.h"
#include "memory/data_types.h"
#include "math/matrix_math_types.h"
#include "math/real_math_types.h"
#include "math/real_math.h"
#include "ai/sector_ref.h"
#include "units/motor_enums.h"
#include "units/motor_program_state.h"
#include "motor/mover_inline.h"
#include "ai/ai_point3d.h"
#include "ai/cl_engine.h"
#include "game/game_types.h"
// @address 0xa0538 ?action_executing@@YA_NJW4e_action@@@Z
// @address 0xb71f8 ?cs_approach_reset@@YAXJ@Z
// @address 0x565088 ?custom_animation_update@@YAFJJPAUcs_command@@PAUs_atom_control@@J@Z
// @address 0x5659b0 ?move_direction_start@@YA_NJJPAUcs_command@@PAUs_atom_control@@J@Z
// @address 0x565ae8 ?cs_directmovement_update_facing@@YAXJJPAUs_atom_control@@@Z
// @address 0x565cc0 ?move_direction_update@@YAFJJPAUcs_command@@PAUs_atom_control@@J@Z
// @address 0x566388 ?grenade_update@@YAFJJPAUcs_command@@PAUs_atom_control@@J@Z
// @address 0x566b18 ?vehicle_atom_update@@YAFJJPAUcs_command@@PAUs_atom_control@@J@Z
// @address 0x566ba0 ?vehicle_atom_handle_deleted_object@@YAXJJJ@Z
// @address 0x566ca8 ?recording_update@@YAFJJPAUcs_command@@PAUs_atom_control@@J@Z
// @address 0x566e08 ?vocalize_atom_update@@YAFJJPAUcs_command@@PAUs_atom_control@@J@Z
// @address 0x566e48 ?die_atom_start@@YA_NJJPAUcs_command@@PAUs_atom_control@@J@Z
// @address 0x566eb0 ?behavior_atom_update@@YAFJJPAUcs_command@@PAUs_atom_control@@J@Z
// @address 0x566ef0 ?formation_atom_update@@YAFJJPAUcs_command@@PAUs_atom_control@@J@Z
// @address 0x566fd0 ?deploy_atom_update@@YAFJJPAUcs_command@@PAUs_atom_control@@J@Z
// @address 0x567088 ?approach_start@@YA_NJJPAUcs_command@@PAUs_atom_control@@J@Z
// @address 0x5675d8 ?approach_atom_end@@YAXJJPAUs_atom_control@@J@Z
// @address 0x567630 ?approach_atom_handle_deleted_object@@YAXJJJ@Z

#include "cseries/cseries.h"
#include <string.h>
#include <math.h>
#include "game/game_time_types.h"
#include "objects/object_datum.h"
#include "tag_files/tag_types.h"
#include "motor/action_system.h"
#include "motor/motor_animation.h"
#include "objects/objects.h"
#include "units/units.h"
#include "ai/actor_combat.h"
#include "ai/actor_moving.h"
#include "ai/ai_vehicles.h"
#include "ai/behavior.h"
#include "ai/joint_behavior.h"
#include "cutscene/recorded_animations.h"
#include "units/unit_dialogue.h"
#include "units/unit_speech_item.h"
#include "ai/ai_atoms.h"
#include "ai/cl_engine_data.h"
#include "ai/pathfinding_utilities.h"
#include "memory/data.h"
#include "ai/actor_datum.h"
#include "scenario/scenario_tag.h"
#include "game/player_types.h"
#include "units/unit_types.h"
#include "units/unit_dialogue_datum_types.h"
#include "cache/cache_files.h"
#include "memory/tls_globals.h"
#include "ai/command_script_data.h"
#include "hs/hs_unit_seats.h"
#include "units/unit_seat_source.h"
#include "units/unit_seat.h"
#include "scenario/scenario.h"
#include "ai/ai_reference_frame.h"
#include "ai/cs_scenario_definitions.h"
#include <stdlib.h>



struct path_state;


#include "ai/unit_dialogue_types.h"


typedef char check_orders_size[sizeof(c_destination_orders) == 0x68 ? 1 : -1];
typedef char check_actor_size[sizeof(actor_datum) == 0xA8C ? 1 : -1];




#include "units/biped_definitions.h"



static __forceinline void ai_atoms_action_request_clear(action_request *request)
{
    memset(request, 0, sizeof(*request));
}

static __forceinline long ai_atoms_seconds_to_ticks_round(float seconds)
{
    return (long)(seconds * (float)TLS_GAME_TIME_GLOBALS->ticks_per_second + 0.5f);
}

static __forceinline long ai_atoms_seconds_to_ticks(long seconds)
{
    return seconds * TLS_GAME_TIME_GLOBALS->ticks_per_second;
}



static __forceinline void ai_atoms_path_result_reset(long actor_index, bool reset_time)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    actor->path.valid = 0;
    actor->path.obstacle_object_index = -1;
    actor->path.result_value = 0;
    if (reset_time)
    {
        actor->path_result_time = 0;
        actor->path_result_count = 0;
    }
}

static __forceinline void ai_atoms_actor_path_clear(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    ai_atoms_path_result_reset(actor_index, true);
    actor->path_state = 0;
    actor->destination_orders.clear();
}

void cs_approach_reset(long script_index)
{
    command_script_datum *script = datum_get<command_script_datum>(tls_command_script_data(), script_index);
    if (script->approach_pending || script->approach_object_index != -1)
    {
        script->approach_pending = false;
        script->approach_object_index = -1;
        script->approach_within_range = false;
        script->aim_at_target = false;
        script->look_at_target = false;
    }
}

// @address 0x565048 ?pause_start@@YA_NJJPAUcs_command@@PAUs_atom_control@@J@Z
bool pause_start(long actor_index, long atom_index, cs_command *command, s_atom_control *control, long command_index)
{
    control->timer = (short)command->parameter;
    return true;
}

// @address 0x565068 ?pause_update@@YAFJJPAUcs_command@@PAUs_atom_control@@J@Z
short pause_update(long actor_index, long atom_index, cs_command *command, s_atom_control *control, long command_index)
{
    long result;
    if (control->timer <= 0)
        result = 0;
    else
        result = 2;
    return result;
}

short custom_animation_update(long actor_index, long atom_index, cs_command *command, s_atom_control *control, long command_index)
{
    return ((unsigned char)action_executing(atom_index, _action_custom_animation) != 0 ? -1 : 0) & 2;
}

bool move_direction_start(long actor_index, long unit_index, cs_command *command, s_atom_control *control, long command_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    object_get_origin(unit_index, &control->origin);
    vector3d_from_angle(&control->direction, command->angle * 0.017453292f);
    control->flags |= 4;
    control->index = -1;
    if (command->index >= 0 && command->index <= 3)
    {
        switch (command->index)
        {
        case 0:
            control->index = 0;
            break;
        case 1:
            control->index = 2;
            break;
        case 2:
            control->index = 3;
            break;
        case 3:
            control->index = 1;
            break;
        }
    }
    if (actor->unit_index == unit_index)
        ai_atoms_actor_path_clear(actor_index);
    control->flags = (control->flags & ~12) | (1 << 2);
    return true;
}

// @address 0x565ae8 ?cs_directmovement_update_facing@@YAXJJPAUs_atom_control@@@Z
void cs_directmovement_update_facing(long actor_index, long object_index, s_atom_control *control)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    long type;
    real_vector3d direction;
    if (object_index == actor->unit_index)
        direction = actor->facing_vector;
    else
        ::object_get_orientation(object_index, &direction, 0);
    type = control->index;
    switch (type)
    {
    case 0:
        control->direction = direction;
        break;
    case 1:
        negate_vector3d(&direction, &control->direction);
        break;
    case 2:
    case 3:
    {
        const real_vector3d up = { 0.0f, 0.0f, 1.0f };
        real_vector3d perpendicular;
        cross_product3d(&up, &direction, &perpendicular);
        if (::normalize3d(&perpendicular) == 0.0f)
        {
            object_datum *object = (object_datum *)object_get_and_verify_type(object_index, _object_mask_all);
            cross_product3d(&object->up, &direction, &perpendicular);
            if (::normalize3d(&perpendicular) == 0.0f)
                perpendicular = global_identity4x3->forward;
        }
        if (type == 2)
            control->direction = perpendicular;
        else
            negate_vector3d(&perpendicular, &control->direction);
        break;
    }
    }
}

// @address 0x565cc0 ?move_direction_update@@YAFJJPAUcs_command@@PAUs_atom_control@@J@Z
short move_direction_update(long actor_index, long unit_index, cs_command *command, s_atom_control *control, long command_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    short result = 2;
    real_point3d origin;
    real_vector3d displacement;
    object_get_origin(unit_index, &origin);
    vector_from_points3d(&control->origin, &origin, &displacement);
    float distance = dot_product3d(&displacement, &control->direction);
    if (distance > command->parameter)
        result = 0;
    if (control->flags & 8)
        cs_directmovement_update_facing(actor_index, unit_index, control);
    if (!actor->swarm)
    {
        actor->blind_panic_control_active = true;
        actor->control_vector = control->direction;
    }
    if (!actor->swarm)
    {
        unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, NONE);
        const real_vector3d *up;
        if (mover_has_variable_camera_up(unit_index))
            up = &unit->object.up;
        else
            up = &global_identity4x3->up;
        real_vector3d movement;
        switch (control->index)
        {
        case 0:
            movement = control->direction;
            break;
        case 2:
            cross_product3d(up, &control->direction, &movement);
            normalize3d(&movement);
            break;
        case 3:
            cross_product3d(&control->direction, up, &movement);
            normalize3d(&movement);
            break;
        case 1:
            scale_vector3d(&control->direction, -1.0f, &movement);
            break;
        }
        actor->look_mode = 4;
        actor->look_direction.to_vector(&movement);
    }
    return result;
}

// @address 0x565f50 ?approach_object_start@@YA_NJJPAUcs_command@@PAUs_atom_control@@J@Z
bool approach_object_start(long actor_index, long atom_index, cs_command *command, s_atom_control *control, long command_index)
{
    bool result = false;
    long object_index = command->object_index;
    if (object_index != -1 && object_try_and_get_and_verify_type(object_index, 0xffffffff) != 0)
    {
        result = true;
        control->object_index = object_index;
    }
    control->flags |= 0x10;
    return result;
}

// @address 0x566388 ?grenade_update@@YAFJJPAUcs_command@@PAUs_atom_control@@J@Z
short grenade_update(long actor_index, long unit_index, cs_command *command, s_atom_control *control, long script_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    command_script_datum *script = datum_get<command_script_datum>(tls_command_script_data(), script_index);
    if (script->grenade_throw_started)
    {
        control->timer = (short)((actor->grenade_timer > 0 || action_executing(actor->unit_index, _action_grenade))
            ? ai_atoms_seconds_to_ticks(1) : 0);
    }
    else if (!motor_animation_busy(unit_index))
    {
        if (::actor_combat_plan_grenade_trajectory(actor_index, script->grenade_throw_trajectory_type, &script->grenade_target, -1, -1, 2))
            script->grenade_throw_started = true;
    }
    return control->timer != 0 ? 2 : 0;
}

// @address 0x5664f8 ?actor_is_using_equipment@@YAFJJPAUcs_command@@PAUs_atom_control@@J@Z
short actor_is_using_equipment(long actor_index, long object_index, cs_command *command, s_atom_control *control, long command_index)
{
    return ((unsigned char)action_executing(object_index, _action_equipment) != 0 ? -1 : 0) & 2;
}

// @address 0x566708 ?jump_targeted_start@@YA_NJJPAUcs_command@@PAUs_atom_control@@J@Z
bool jump_targeted_start(long actor_index, long script, cs_command *cmd, s_atom_control *atom, long unused)
{
    bool result = false;
    actor_datum *a = datum_get<actor_datum>(actor_data, actor_index);
    if (script != a->unit_index || a->root_vehicle_index == -1)
    {
        atom->flags = (atom->flags & ~(7 << 5)) | (5 << 5);
        atom->index = 0;
        atom->direction.i = cmd->parameter;
        atom->direction.j = cmd->angle;
        atom->timer = (short)ai_atoms_seconds_to_ticks_round(2.0f);
        result = true;
    }
    return result;
}


// @address 0x5669e8 ?vehicle_atom_start@@YA_NJJPAUcs_command@@PAUs_atom_control@@J@Z
bool vehicle_atom_start(long actor_index, long script_index, cs_command *command, s_atom_control *control, long command_index)
{
    bool result = false;
    if (command->point_ref != -1)
    {
        actor_attach_to_vehicle(actor_index, command->point_ref, true);
        if (command->seat_ref != -1)
        {
            actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
            unit_seat_source seats[64];
            short count = (short)hs_get_unit_seats(command->point_ref, command->seat_ref, _vehicle_seat_desire_any,
                _unit_seat_occupancy_empty, seats, 64);
            for (short i = 0; i < count; ++i)
            {
                unit_seat_source *seat = &seats[i];
                long vehicle_index = seat->unit_index;
                if (vehicle_index != -1)
                {
                    short seat_index = seat->seat_index;
                    if (seat_index != -1 && unit_has_seat_entrance_animation(actor->unit_index, vehicle_index, seat_index)
                        && ::unit_seat_get_occupant(vehicle_index, seat_index) == -1
                        && !(_rotl(seat->definition->flags, 21) & 1))
                    {
                        actor_attach_to_seat(actor_index, seat->unit_index, seat->seat_index, 0.0f, true);
                        result = true;
                        break;
                    }
                }
            }
        }
        else
            result = true;
    }
    return result;
}

short vehicle_atom_update(long actor_index, long object_index, cs_command *command, s_atom_control *control, long command_index)
{
    short result = 2;
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    if (!(control->flags & 0x100) && actor->root_vehicle_index == -1)
    {
        if (actor->vehicle_desire == 3 && actor->destination_unit_index != -1 && actor->destination_seat_index != 0xffff)
        {
            *global_new_behavior = 102;
            control->flags |= 0x100;
        }
    }
    else
        result = 0;
    return result;
}

void vehicle_atom_handle_deleted_object(long actor_index, long script_index, long target)
{
    command_script_datum *script = datum_get<command_script_datum>(tls_command_script_data(), script_index);
    cs_command *vehicle = &script->command[0];
    if (vehicle->object_index != target)
        return;
    vehicle->object_index = -1;
}

short recording_update(long actor_index, long object_index, cs_command *command, s_atom_control *control, long command_index)
{
    short result = 0;
    if (recorded_animation_controlling_unit(object_index))
        result = 2;
    return result;
}

// @address 0x566cf0 ?action_atom_update@@YAFJJPAUcs_command@@PAUs_atom_control@@J@Z
short action_atom_update(long actor_index, long unit_index, cs_command *command, s_atom_control *control, long command_index)
{
    short result = 0;
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    if (actor->action.type != 0 && actor->action.type == 5)
        result = 2;
    else if (actor->unit_index != -1 && motor_animation_busy(actor->unit_index))
        result = 2;
    return result;
}

// @address 0x566d88 ?vocalize_atom_start@@YA_NJJPAUcs_command@@PAUs_atom_control@@J@Z
bool vocalize_atom_start(long actor_index, long unit_index, cs_command *command, s_atom_control *control, long unused)
{
    unit_speech_item speech;
    memset(&speech, 0, sizeof(speech));
    speech.type = 14;
    speech.vocalization = command->index;
    speech.pad10 = 0;
    speech.delay = 0;
    speech.sound_index = -1;
    speech.dialogue_index = -1;
    speech.information.type = -1;
    speech.information.subject = -1;
    speech.information.cause = -1;
    ::unit_speak(unit_index, &speech);
    return true;
}

static __forceinline void ai_atoms_action_construct(c_ai_action *action)
{
    memset(&action->direction, 0, sizeof(action->direction));
    memset(action, 0, sizeof(*action));
}
static __forceinline void ai_atoms_orders_construct(c_destination_orders *orders)
{
    ai_atoms_action_construct(&orders->action);
    memset(orders, 0, sizeof(*orders));
}

static __forceinline void ai_atoms_actor_stop_path(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    bool moving = false;
    if (actor->path.valid != 0 && actor->path_state == 1)
        moving = true;
    if (moving)
        ai_atoms_actor_path_clear(actor_index);
}

// @address 0x567088 ?approach_start@@YA_NJJPAUcs_command@@PAUs_atom_control@@J@Z
bool approach_start(long actor_index, long unit_index, cs_command *command, s_atom_control *control, long script_index)
{
    command_script_datum *script = datum_get<command_script_datum>(tls_command_script_data(), script_index);
    actor_find_pathfinding_location(actor_index);
    float radius = script->approach_max_distance_sq;
    script->approach_within_range = false;
    script->approach_pending = true;
    if (radius <= 0.0f)
        script->approach_max_distance_sq = 16.0f;
    float path_radius = script->approach_max_distance_sq;
    if (script->approach_follow_distance_sq < path_radius)
        script->approach_follow_distance_sq = (sqrtf(path_radius) + 2.0f) * (sqrtf(path_radius) + 2.0f);
    ai_atoms_actor_path_clear(actor_index);
    return true;
}
// @address 0x567160 ?approach_update@@YAFJJPAUcs_command@@PAUs_atom_control@@J@Z
short approach_update(long actor_index, long unit_index, cs_command *command, s_atom_control *control, long script_index)
{
    command_script_datum *script = datum_get<command_script_datum>(tls_command_script_data(), script_index);
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    short result = 2;
    bool proceed = false;
    real_point3d origin;
    if (script->approach_pending)
    {
        if (script->approach_object_index == -1)
        {
            float best_distance = script->approach_max_distance_sq;
            c_data_iterator<player_datum> iterator;
            iterator.m_iterator.data = tls_player_data();
            iterator.m_iterator.absolute_index = -1;
            iterator.m_iterator.index = -1;
            while (iterator.next())
            {
                long object_index = iterator.get_datum()->unit_index;
                if (object_index != -1)
                {
                    object_get_origin(object_index, &origin);
                    float dist_sq = distance_squared3d(&actor->position.body, &origin);
                    if (dist_sq < best_distance)
                    {
                        best_distance = dist_sq;
                        script->approach_object_index = object_index;
                    }
                }
            }
            if (script->approach_object_index != -1)
            {
                script->aim_at_target = true;
                script->aim_target.type = 1;
                script->aim_target.index = script->approach_object_index;
                script->look_at_target = true;
                script->look_target.type = 1;
                script->look_target.index = script->approach_object_index;
                script->face_lower_weapon = true;
            }
        }
        if (script->approach_object_index != -1)
        {
            object_get_origin(script->approach_object_index, &origin);
            float dist_sq = distance_squared3d(&actor->position.body, &origin);
            if (dist_sq < script->approach_max_distance_sq)
                script->approach_pending = false;
        }
    }
    if (!script->approach_pending)
    {
        proceed = true;
        if (script->approach_object_index != -1)
        {
            object_get_origin(script->approach_object_index, &origin);
            float dist_sq = distance_squared3d(&actor->position.body, &origin);
            if (dist_sq < script->approach_distance_sq)
            {
                proceed = false;
                ai_atoms_actor_stop_path(actor_index);
                if (!script->approach_within_range)
                {
                    script->approach_within_range = true;
                    command->flags &= ~2;
                }
            }
            else
            {
                if (dist_sq > script->approach_follow_distance_sq)
                {
                    proceed = false;
                    script->approach_pending = true;
                }
            }
        }
        else
        {
            result = 0;
        }
    }
    if (proceed && actor->uncovering && script->approach_object_index != -1)
    {
        long target_object = -1;
        c_ai_point3d point;
        c_sector_ref sector;
        bool destination_found = false;
        s_object_header *header = object_header_get(script->approach_object_index);
        if (((1 << header->object_type) & 0x1003) != 0)
        {
            unit_datum *seat_object = (unit_datum *)header->datum;
            if (!actor->use_object)
            {
                if (seat_object->unit.parent_seat_index != -1)
                {
                    bool valid = false;
                    if (unit_get_best_seat_entrance_animation(actor->unit_index, seat_object->object.parent_object_index,
                        seat_object->unit.parent_seat_index,
                            &point.point, &origin) == -1)
                        object_get_origin(script->approach_object_index, &origin);
                    if (vehicle_find_destination(actor_index, seat_object->object.parent_object_index, &origin, &point, &sector, &valid, false))
                    {
                        destination_found = true;
                        target_object = valid ? -1 : seat_object->object.parent_object_index;
                    }
                }
            }
        }
        if (!destination_found)
        {
            object_get_pathfinding_location(script->approach_object_index, &sector, &point);
            if (actor->use_object)
            {
                s_object_header *header2 = object_header_get(script->approach_object_index);
                if (((1 << header2->object_type) & 0x1003) != 0)
                    unit_get_head_position(script->approach_object_index, &origin);
                else
                    object_get_center_of_mass(script->approach_object_index, &origin);
                ai_reference_frame_inverse_transform_point(point.attachment_index, &origin, &point.point);
            }
            target_object = script->approach_object_index;
        }
        c_destination_orders orders;
        ai_atoms_orders_construct(&orders);
        if (actor->use_object)
            orders.to_air_point(&point);
        else
            orders.to_point(&point, &sector);
        float stop_distance_squared = script->approach_distance_sq;
        orders.target_index = target_object;
        if (stop_distance_squared > 0.0f)
            orders.f24 = sqrtf(stop_distance_squared) * 0.75f;
        actor_move(actor_index, &orders, 0);
    }
    return result;
}

short vocalize_atom_update(long actor_index, long atom_index, cs_command *command, s_atom_control *atom, long command_index)
{
    s_unit_dialogue_datum *unit = (s_unit_dialogue_datum *)object_get_and_verify_type(atom_index, _object_mask_biped);
    s_unit_dialogue *dialogue = (s_unit_dialogue *)((char *)unit + unit->dialogue_offset);
    return (dialogue->current.type == 14) ? 2 : 0;
}

bool die_atom_start(long actor_index, long object_index, cs_command *command, s_atom_control *control, long command_index)
{
    object_datum *unit = (object_datum *)object_get_and_verify_type(object_index, _object_mask_biped);
    if (command->index == 1)
        unit->damage_flags |= 0x80;
    else
        unit->damage_flags |= 0x40;
    object_wake(object_index);
    return true;
}

short behavior_atom_update(long actor_index, long atom_index, cs_command *command, s_atom_control *atom, long command_index)
{
    short result = 2;
    short flags = atom->flags;
    if (flags & 0x100)
        result = 0;
    else
    {
        atom->flags = flags | 0x100;
        *global_new_behavior = command->index;
    }
    return result;
}



// March 82fb02e8 calls behavior_formation_setup (82ee8ce0).
inline void ai_atoms_formation_setup(long actor_index, short formation, long ai_index,
    long point_ref, long seat, c_formation_data *state)
{
    state->formation = formation;
    state->formation_requested = 1;
    state->ai_index = ai_index;
    state->point_ref = point_ref;
    state->seat = seat;
}

// @address 0x566ef0 ?formation_atom_update@@YAFJJPAUcs_command@@PAUs_atom_control@@J@Z
short formation_atom_update(long actor_index, long atom_index, cs_command *command, s_atom_control *control, long command_index)
{
    short result = 2;
    if (control->flags & 0x100)
        result = 0;
    else
    {
        long point = command->point_ref;
        if (valid_point_ref(point))
        {
            long seat = command->seat_ref;
            if (valid_point_ref(seat))
            {
                c_formation_data *state = (c_formation_data *)global_new_state_data;
                ai_atoms_formation_setup(actor_index, command->index, command->formation_index,
                    command->point_ref, command->seat_ref, state);
                control->flags |= 0x100;
                *global_new_behavior = 192;
            }
            else
                result = 1;
        }
        else
            result = 1;
    }
    return result;
}

short deploy_atom_update(long actor_index, long atom_index, cs_command *command, s_atom_control *control, long command_index)
{
    short result = 2;
    if (control->flags & 0x100)
        result = 0;
    else
    {
        long target = command->point_ref;
        if (valid_point_ref(target))
        {
            deploy_turret_state_data *request = ((deploy_turret_state_data *)global_new_state_data);
            if (valid_point_ref(target))
            {
                request->point_ref = target;
                request->keep = 1;
            }
            control->flags |= 0x100;
            *global_new_behavior = 196;
        }
        else
            result = 1;
    }
    return result;
}

void approach_atom_end(long actor_index, long atom_index, s_atom_control *atom_control, long script_index)
{
    command_script_datum *script = datum_get<command_script_datum>(tls_command_script_data(), script_index);
    if (!script->approach_pending && script->approach_object_index == -1)
        return;
    script->approach_pending = false;
    script->approach_object_index = -1;
    script->approach_within_range = false;
    script->aim_at_target = false;
    script->look_at_target = false;
}

void approach_atom_handle_deleted_object(long actor_index, long script_index, long target)
{
    command_script_datum *script = datum_get<command_script_datum>(tls_command_script_data(), script_index);
    if (script->approach_object_index != target)
        return;
    cs_approach_reset(script_index);
}

// @address 0x567668 ?melee_update@@YAFJJPAUcs_command@@PAUs_atom_control@@J@Z
short melee_update(long actor_index, long unit_index, cs_command *command, s_atom_control *control, long command_index)
{
    unsigned char executing = action_executing(datum_get<actor_datum>(actor_data, actor_index)->unit_index, _action_melee_ai);
    return (executing != '\0') ? 2 : 0;
}

// @address 0x5676c0 ?smash_update@@YAFJJPAUcs_command@@PAUs_atom_control@@J@Z
short smash_update(long actor_index, long unit_index, cs_command *command, s_atom_control *control, long command_index)
{
    unsigned char executing = action_executing(datum_get<actor_datum>(actor_data, actor_index)->unit_index, _action_smash);
    return (executing != '\0') ? 2 : 0;
}

struct s_attached_point;
// @address 0x5650c0 ?cs_movement_perform@@YA_NJPAUcs_command@@JPA_N@Z
bool cs_movement_perform(long actor_index, cs_command *command, long command_index, bool *failed)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    command_script_datum *script = datum_get<command_script_datum>(tls_command_script_data(), command_index);
    bool v = false;
    if (valid_point_ref(command->point_ref))
    {
        long unit = actor->unit_index;
        v = true;
        long point_index = command->point_ref & 0xffff;
        cs_point_set *set = cs_get_point_set((command->point_ref >> 16) & 0xffff);
        cs_point *point = TAG_BLOCK_GET_ELEMENT(&set->points, point_index, cs_point);
        if (unit != -1)
        {
            c_destination_orders orders;
            ai_atoms_orders_construct(&orders);
            bool air = command->atom_type == _ai_atom_go_by || command->atom_type == _ai_atom_fly_by;
            actor->ignore_obstacles = script->ignore_obstacles;
            long vehicle = actor->root_vehicle_index;
            if (vehicle != -1 && !actor->driver)
            {
                if (!action_executing(unit, _action_vehicle_exit))
                {
                    bool flag = false;
                    if (ai_vehicle_get_driver(vehicle, (e_game_team)actor->team) == -1
                        && ((unit_datum *)object_get_and_verify_type(actor->root_vehicle_index, NONE))->unit.driver_object_index == -1)
                        flag = true;
                    if (flag)
                    {
                        action_submit_event(actor->unit_index, 13);
                        *failed = true;
                    }
                }
                else
                    *failed = true;
            }
            switch (command->atom_type)
            {
            case _ai_atom_fly_to:
            case _ai_atom_fly_by:
            case _ai_atom_fly_to_and_face:
                orders.to_air_point(&point->position);
                orders.flags.set(_destination_flag_2, air);
                orders.f24 = command->parameter;
                break;
            case _ai_atom_go_to:
            case _ai_atom_go_by:
            case _ai_atom_go_to_and_face:
            {
                c_sector_ref *sector = &point->sector;
                if (sector->valid())
                {
                    bool set_radius = air | (command->parameter != 0.0f);
                    orders.to_point(&point->position, sector);
                    if (set_radius)
                        orders.flags.m_storage |= 4;
                    else
                        orders.flags.m_storage &= ~4;
                    if (set_radius)
                        orders.f20 = command->parameter;
                }
                else
                    v = false;
                break;
            }
            case _ai_atom_go_to_and_posture:
            {
                c_sector_ref *sector = &point->sector;
                if (sector->valid())
                {
                    real_vector3d euler;
                    vector3d_from_euler_angles2d(&euler, &point->facing);
                    orders.to_point(&point->position, sector);
                    v = orders.posture(actor_index, &point->position, &euler, command->seat_ref);
                }
                else
                    v = false;
                break;
            }
            default:
                break;
            }
            if (v)
            {
                signed char type = command->atom_type;
                if (type >= _ai_atom_go_by && (type <= _ai_atom_go_to_and_face || type == _ai_atom_fly_to_and_face) && valid_point_ref(command->seat_ref))
                {
                    cs_point_set *seat_set = cs_get_point_set((command->seat_ref >> 16) & 0xffff);
                    cs_point *seat_point = TAG_BLOCK_GET_ELEMENT(&seat_set->points, command->seat_ref & 0xffff, cs_point);
                    ai_vector_from_points3d(&point->position, &seat_point->position, &orders.m_destination_facing);
                    if (normalize3d(&orders.m_destination_facing) != 0.0f)
                    {
                        if (type == 2)
                            orders.flags.m_storage |= 1;
                        else
                            orders.flags.m_storage |= 2;
                    }
                }
                v = actor_move(actor_index, &orders, 0);
                if (v)
                    actor->flags &= ~0x10000000;
            }
        }
    }
    return v;
}
// @address 0x565518 ?move_atom_start@@YA_NJJPAUcs_command@@PAUs_atom_control@@J@Z
bool move_atom_start(long actor_index, long unit_index, cs_command *command, s_atom_control *control, long command_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    command_script_datum *script = datum_get<command_script_datum>(tls_command_script_data(), command_index);
    bool valid = true;
    bool result = false;
    if (!valid_point_ref(command->point_ref))
        valid = false;
    else if (command->atom_type >= _ai_atom_go_by && (command->atom_type <= _ai_atom_go_to_and_face || command->atom_type == _ai_atom_fly_to_and_face))
    {
        if (!valid_point_ref(command->seat_ref))
            valid = false;
    }
    if (valid)
    {
        if (actor->swarm_index != NONE)
        {
            if (command->atom_type == _ai_atom_go_to)
            {
                long point_index = command->point_ref & 0xffff;
                cs_point_set *set = cs_get_point_set((command->point_ref >> 16) & 0xffff);
                control->point = TAG_BLOCK_GET_ELEMENT(&set->points, point_index, cs_point)->position;
                control->flags |= 512;
                result = true;
            }
        }
        else
        {
            bool failed = false;
            if (command->atom_type == _ai_atom_go_to_and_face)
                control->timer = 0;
            if (cs_movement_perform(actor_index, command, command_index, &failed))
            {
                control->flags |= 1;
                result = true;
            }
            else if (script->failsafe_pathfinding || failed)
            {
                control->flags &= ~1;
                result = true;
            }
        }
    }
    return result;
}

// @address 0x5678a8 ?swarm_to_update@@YAFJJPAUcs_command@@PAUs_atom_control@@J@Z
short swarm_to_update(long actor_index, long unit_index, cs_command *command, s_atom_control *control, long command_index)
{
    real_point3d origin;
    object_get_origin(unit_index, &origin);
    float distance = sqrtf(::ai_distance_squared3d((c_ai_point3d const *)((const s_attached_point *)&control->index),
        (real_point3d const *)&origin));
    short result = distance > control->origin.x ? 2 : 0;
    return result;
}
// @address 0x567908 ?swarm_from_start@@YA_NJJPAUcs_command@@PAUs_atom_control@@J@Z
bool swarm_from_start(long actor,long object,cs_command *command,s_atom_control *control,long unused)
{
 bool result=false;
 if(valid_point_ref(command->point_ref)) {
  long point_index=command->point_ref&0xffff;
  cs_point_set *set=cs_get_point_set((command->point_ref>>16)&0xffff);
  cs_point *point=TAG_BLOCK_GET_ELEMENT(&set->points, point_index, cs_point);
  object_get_origin(object,&control->origin);
  ai_vector_from_points3d(&control->origin,&point->position,&control->direction);
  scale_vector3d(&control->direction,-1.0f,&control->direction);
  if(normalize3d(&control->direction)>0.0f) {
   control->index=-1;
   control->flags|=4;
   control->flags&=~8;
   result=true;
  }
 }
 return result;
}
// @address 0x567a00 ?swarm_from_update@@YAFJJPAUcs_command@@PAUs_atom_control@@J@Z
short swarm_from_update(long actor_index, long object_index, cs_command *command, s_atom_control *control, long unused)
{
    short result = 2;
    real_point3d origin;
    real_vector3d vec1;
    object_get_origin(object_index, &origin);
    vector_from_points3d(&control->origin, &origin, &vec1);
    if (dot_product3d(&control->direction, &vec1) > command->parameter)
        result = 0;
    return result;
}

static __forceinline bool ai_atoms_recording_name_matches(const char *name, const char *point_name)
{
    return ::csstricmp((char const *)(name), (char const *)(point_name)) == 0;
}

// @address 0x566bd8 ?recording_start@@YA_NJJPAUcs_command@@PAUs_atom_control@@J@Z
bool recording_start(long actor_index, long unit_index, cs_command *command, s_atom_control *control, long command_index)
{
    struct scenario *scenario = global_scenario;
    bool result = false;
    long point_index = command->point_ref;
    if (point_index >= 0 && point_index < scenario->recording_points.count)
    {
        const char *point_name = ((char *)scenario->recording_points.address) + (command->point_ref) * 40;
        short recording = -1;
        for (short i = 0; i < scenario->recorded_animations.count; i++)
        {
            if (ai_atoms_recording_name_matches(((char *)scenario->recorded_animations.address) + i * 64, point_name))
            {
                recording = i;
                break;
            }
        }
        if (recording != -1)
            result = recorded_animation_play_internal(unit_index, recording, 0);
    }
    return result;
}

static const float ai_atoms_jump_delay = 0.5f;
static const float ai_atoms_jump_zero = 0.0f;
static const float ai_atoms_jump_low_threshold = 0.7f;

// @address 0x5667c0 ?jump_update@@YAFJJPAUcs_command@@PAUs_atom_control@@J@Z
short jump_update(long actor_index, long unit_index, cs_command *command, s_atom_control *control, long command_index)
{
    short result = 2;
    short flags = control->flags;
    if (!(flags & 0x20))
        result = 0;
    else
    {
        actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
        bool airborne = unit_index == actor->unit_index ? actor->airborne : true;
        long started = flags & 0x40;
        if (started && airborne)
            control->timer = 0;
        if (control->timer == 0)
            result = 0;
        else
        {
            bool face = false;
            if (control->index > 0)
                --control->index;
            if (started)
                face = control->index > 0;
            else if (control->index == 0 && !actor->airborne && !motor_animation_busy(actor->unit_index))
            {
                real_vector2d direction = actor->facing_vector.xy;
                float magnitude = normalize2d(&direction);
                real_vector2d jump_direction;
                if (magnitude == ai_atoms_jump_zero)
                    jump_direction = *global_forward2d;
                else
                    jump_direction = direction;
                actor->leap_requested = true;
                actor->leap_active = control->direction.j < control->direction.i * ai_atoms_jump_low_threshold;
                actor->leap_direction = jump_direction;
                actor->leap_target_valid = true;
                actor->leap_speed = control->direction.i;
                actor->leap_height = control->direction.j;
                actor->leap_prop_index = -1;
                flags = control->flags | 0x40;
                control->flags = flags;
                if (!(flags & 0x80))
                {
                    float tick_rate = (float)TLS_GAME_TIME_GLOBALS->ticks_per_second;
                    float ticks = ai_atoms_jump_delay * tick_rate;
                    control->index = (short)(long)(ticks + 0.5f);
                }
            }
            else
                face = true;
            if (face)
            {
                actor->blind_panic_control_active = true;
                actor->control_vector = actor->facing_vector;
            }
        }
    }
    return result;
}

static __forceinline long ai_atoms_move_facing(short mode, const command_script_datum *script)
{
    if (mode == 2 || (mode == 3 && !script->failsafe_pathfinding))
        return true;
    return false;
}

static __forceinline float ai_atoms_move_dot(const real_vector2d *direction, const real_vector3d *facing)
{
    return direction->i * facing->i + direction->j * facing->j;
}

// @address 0x5656d8 ?move_atom_update@@YAFJJPAUcs_command@@PAUs_atom_control@@J@Z
short move_atom_update(long actor_index, long unit_index, cs_command *command, s_atom_control *control, long command_index)
{
    short result = 2;
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    command_script_datum *script = datum_get<command_script_datum>(tls_command_script_data(), command_index);
    if (actor->swarm_index != NONE)
    {
        if (command->atom_type == _ai_atom_go_to)
        {
            real_point3d origin;
            object_get_origin(unit_index, &origin);
            if (ai_distance_squared3d((const c_ai_point3d *)&control->index, &origin) < 1.0f)
                result = 0;
        }
        else
        {
            result = 1;
        }
        return result;
    }
    if ((control->flags & 1) == 0)
    {
        result = 2;
        if (actor->uncovering)
        {
            bool failed = false;
            if (cs_movement_perform(actor_index, command, command_index, &failed))
                control->flags |= 1;
            else if (!script->failsafe_pathfinding && !failed)
                result = 1;
        }
    }
    if ((control->flags & 1) != 0)
    {
        short mode = datum_get<actor_datum>(actor_data, actor_index)->path_state;
        unsigned char facing = (unsigned char)ai_atoms_move_facing(mode, script);
        if (facing)
        {
            unsigned long order_flag = _rotl((unsigned short)actor->destination_orders.flags.m_storage, 31) & 1;
            if (order_flag != 0 && actor->vehicle_driver_type == 0)
            {
                real_vector2d direction = actor->destination_orders.m_destination_facing.xy;
                float alignment = 1.0f;
                if (normalize2d(&direction) > 0.0f)
                    alignment = ai_atoms_move_dot(&direction, &actor->facing_vector);
                if (alignment < 0.984f)
                {
                    actor->aim_mode = 2;
                    facing = false;
                    actor->aim_direction.to_vector(&actor->destination_orders.m_destination_facing);
                    actor->look_mode = 4;
                    actor->look_direction.to_vector(&actor->destination_orders.m_destination_facing);
                    actor->path_aim = 1;
                }
            }
        }
        if (facing)
        {
            actor_move_halt(actor_index);
            result = 0;
        }
        else
        {
            actor = datum_get<actor_datum>(actor_data, actor_index);
            if (actor->path_state != 2)
            {
                bool clear = false;
                if (actor->path.valid != 0 && datum_get<actor_datum>(actor_data, actor_index)->path_state == 1)
                    clear = true;
                if (!clear && actor->path_state != 3)
                    control->flags &= ~1;
            }
        }
    }
    return result;
}

static const float ai_atoms_jump_delay_fraction = 0.33f;
// @rdata 0x82024fd4 ai_atoms_degrees_to_radians
static const float ai_atoms_degrees_to_radians = 0.017453292f;
static __forceinline bool ai_atoms_jump_object_airborne(long object_index)
{
    const object_datum *object = (const object_datum *)object_get_and_verify_type(object_index, _object_mask_biped);
    return object->parent_object_index == -1 &&
        dot_product3d(&object->linear_velocity, &object->forward) > 2.0f;
}
static __forceinline float ai_atoms_jump_cosine(float angle)
{
    return (float)cos((double)angle);
}
static __forceinline float ai_atoms_jump_sine(float angle)
{
    return (float)sin((double)angle);
}

static __forceinline float ai_atoms_jump_scale(float value, float scale)
{
    return value * scale;
}

static __forceinline float ai_atoms_jump_constant(const float *value);

// @address 0x566530 ?jump_start@@YA_NJJPAUcs_command@@PAUs_atom_control@@J@Z
bool jump_start(long actor_index, long unit_index, cs_command *command, s_atom_control *control, long command_index)
{
    const actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    // match: 0x566530: the game-time slot named directly below; TLS_GAME_TIME_GLOBALS flips the cos/sin fmuls operand tie
    bool result = false;
    if (unit_index != actor->unit_index || actor->root_vehicle_index == -1)
    {
        control->flags = (control->flags & ~0xe0) | 0x20;
        bool airborne = unit_index == actor->unit_index ? (actor->flags & 1) != 0 : ai_atoms_jump_object_airborne(unit_index);
        control->index = airborne
            ? 0
            : (long)((float)TLS_GAME_TIME_GLOBALS->ticks_per_second * ai_atoms_jump_constant(&ai_atoms_jump_delay_fraction) + 0.5f);
        control->timer = (short)(long)((float)TLS_GAME_TIME_GLOBALS->ticks_per_second * 2.0f + 0.5f);
        float cosine = ai_atoms_jump_cosine(ai_atoms_degrees_to_radians * command->parameter);
        control->direction.i = ai_atoms_jump_scale(cosine, command->angle);
        float sine = ai_atoms_jump_sine(ai_atoms_degrees_to_radians * command->parameter);
        control->direction.j = ai_atoms_jump_scale(sine, command->angle);
        if (control->direction.i < 0.0f)
            control->index = 0;
        result = true;
    }
    return result;
}

static __forceinline float ai_atoms_jump_constant(const float * value)
{
    return *value;
}

// @rdata 0x82021694 ai_atoms_swarm_max_distance
static const float ai_atoms_swarm_max_distance = 3.4028234663852886e+38f;
// @rdata 0x82022518 ai_atoms_swarm_min_radius
static const float ai_atoms_swarm_min_radius = 0.3f;
static __forceinline unsigned long ai_atoms_swarm_random_next(unsigned long *seed)
{
    *seed = *seed * 1664525UL + 1013904223UL;
    return *seed >> 16;
}
static __forceinline float ai_atoms_swarm_random_real(unsigned long *seed)
{
    return (float)ai_atoms_swarm_random_next(seed) * 1.5259022e-05f;
}
static __forceinline float ai_atoms_swarm_random_range(unsigned long *seed, float minimum, float maximum)
{
    float random = ai_atoms_swarm_random_real(seed);
    return minimum + (maximum - minimum) * random;
}
// @address 0x567718 ?swarm_to_start@@YA_NJJPAUcs_command@@PAUs_atom_control@@J@Z
bool swarm_to_start(long actor_index, long unit_index, cs_command *command, s_atom_control *control, long command_index)
{
    bool result = false;
    if (valid_point_set(command->point_ref))
    {
        short point_index = (short)command->point_ref;
        cs_point_set *point_set = cs_get_point_set((command->point_ref >> 16) & 0xffff);
        if (point_index == -1)
        {
            float closest_distance = ai_atoms_swarm_max_distance;
            real_point3d origin;
            object_get_origin(unit_index, &origin);
            long point_count = point_set->points.count;
            short index = 0;
            if (point_count > 0)
            {
                cs_point *points = (cs_point *)point_set->points.address;
                do
                {
                    float distance = ai_distance_squared3d(&points[index].position, &origin);
                    if (distance < closest_distance)
                    {
                        closest_distance = distance;
                        point_index = index;
                    }
                    ++index;
                } while (index < point_count);
            }
        }
        if (point_index != -1)
        {
            const cs_point *point = TAG_BLOCK_GET_ELEMENT(&point_set->points, point_index, cs_point);
            control->flags |= 0x200;
            control->point = point->position;
            control->origin.x = ai_atoms_swarm_random_range(tls_random_seed(), ai_atoms_swarm_min_radius, command->parameter);
            result = true;
        }
    }
    return result;
}

// @rdata 0x8202251c
static const float k_approach_zero = 0.0f;
// @rdata 0x82022db8
static const float k_approach_contact_distance = 0.4f;

static __forceinline object_datum *ai_atoms_approach_object_get(long index)
{
    return (object_datum *)object_get_and_verify_type(index, NONE);
}
static __forceinline long ai_atoms_approach_object_root(long index)
{
    long root = -1;
    while (index != -1)
    {
        root = index;
        index = ai_atoms_approach_object_get(index)->parent_object_index;
    }
    return root;
}

// @address 0x565fb0 ?approach_object_update@@YAFJJPAUcs_command@@PAUs_atom_control@@J@Z
short approach_object_update(long actor_index, long unit_index, cs_command *command, s_atom_control *control, long command_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    long target_index = control->object_index;
    short result = 2;
    if (object_try_and_get_and_verify_type(target_index, 0xffffffff))
    {
        if (!actor->swarm)
        {
            real_point3d target;
            object_get_origin(target_index, &target);
            real_vector3d direction;
            direction.k = k_approach_zero;
            vector_from_points2d(&actor->position.body.xy, &target.xy, &direction.xy);
            float distance = normalize3d(&direction);
            actor->blind_panic_control_active = true;
            actor->control_vector = direction;
            if (command->push && actor->unit_index != -1)
            {
                object_datum *unit = ai_atoms_approach_object_get(actor->unit_index);
                if (unit->parent_object_index == -1)
                {
                    long root = ai_atoms_approach_object_root(control->object_index);
                    object_datum *target_object = ai_atoms_approach_object_get(root);
                    float radius = unit->bounding_radius;
                    float target_radius = target_object->bounding_radius;
                    if (((1 << unit->object_type) & 1) != 0)
                        radius = TAG_GET('bipd', biped_definition, unit->definition_index)->physics.radius;
                    if (((1 << target_object->object_type) & 1) != 0)
                        target_radius = TAG_GET('bipd', biped_definition, target_object->definition_index)->physics.radius;
                    if (distance - radius - target_radius < k_approach_contact_distance)
                    {
                        action_request request;
                        ai_atoms_action_request_clear(&request);
                        // match: 0x565fb0: the request is cleared a second time before its type is set.
                        memset(&request, 0, sizeof(request));
                        request.type = _action_melee_ai;
                        request.melee_ai.melee_type = 0;
                        request.melee_ai.has_vector = true;
                        request.melee_ai.vector = direction.xy;
                        action_submit(actor->unit_index, &request);
                    }
                }
            }
        }
    }
    else
        result = 1;
    return result;
}

#if !defined(_XBOX) || defined(__clang__)
// The Xbox link folds these bodies into shared "return true" (0x42d380) and "return 0" (0x42d298) code; the PC
// build needs them as functions of their own.
bool action_atom_start(long actor_index, long unit_index, cs_command *command, s_atom_control *control, long command_index)
{
    return true;
}

bool custom_animation_start(long actor_index, long unit_index, cs_command *command, s_atom_control *control, long command_index)
{
    return true;
}

bool melee_start(long actor_index, long unit_index, cs_command *command, s_atom_control *control, long command_index)
{
    return true;
}

bool smash_start(long actor_index, long unit_index, cs_command *command, s_atom_control *control, long command_index)
{
    return true;
}

short die_atom_update(long actor_index, long unit_index, cs_command *command, s_atom_control *control, long command_index)
{
    return 0;
}

// unit_actor_try_use_equipment takes only the actor and unit; this gives it the atom start signature.
static bool equipment_atom_start(long actor_index, long unit_index, cs_command *command, s_atom_control *control, long command_index)
{
    return unit_actor_try_use_equipment(actor_index, unit_index) != 0;
}

// The image holds these 29 definitions 16 bytes apart from rva 0x72d094, in g_atom_definitions order.
s_atom_definition g_atom_pause = { pause_start, pause_update, 0, 0 };
s_atom_definition g_atom_go_to = { move_atom_start, move_atom_update, 0, 0 };
s_atom_definition g_atom_go_by = { move_atom_start, move_atom_update, 0, 0 };
s_atom_definition g_atom_go_to_and_face = { move_atom_start, move_atom_update, 0, 0 };
s_atom_definition g_atom_go_to_and_posture = { move_atom_start, move_atom_update, 0, 0 };
s_atom_definition g_atom_move_direction = { move_direction_start, move_direction_update, 0, 0 };
s_atom_definition g_atom_approach_object = { approach_object_start, approach_object_update, 0, 0 };
s_atom_definition g_atom_grenade = { grenade_start, grenade_update, 0, 0 };
s_atom_definition g_atom_equipment = { equipment_atom_start, actor_is_using_equipment, 0, 0 };
s_atom_definition g_atom_vehicle = { vehicle_atom_start, vehicle_atom_update, 0, vehicle_atom_handle_deleted_object };
s_atom_definition g_atom_jump = { jump_start, jump_update, 0, 0 };
s_atom_definition g_atom_targeted_jump = { jump_targeted_start, jump_update, 0, 0 };
s_atom_definition g_atom_recording = { recording_start, recording_update, 0, 0 };
s_atom_definition g_atom_action = { action_atom_start, action_atom_update, 0, 0 };
s_atom_definition g_atom_vocalize = { vocalize_atom_start, vocalize_atom_update, 0, 0 };
s_atom_definition g_atom_die = { die_atom_start, die_atom_update, 0, 0 };
s_atom_definition g_atom_behavior = { 0, behavior_atom_update, 0, 0 };
s_atom_definition g_atom_fly_to = { move_atom_start, move_atom_update, 0, 0 };
s_atom_definition g_atom_fly_by = { move_atom_start, move_atom_update, 0, 0 };
s_atom_definition g_atom_fly_to_and_face = { move_atom_start, move_atom_update, 0, 0 };
s_atom_definition g_atom_formation = { 0, formation_atom_update, 0, 0 };
s_atom_definition g_atom_approach = { approach_start, approach_update, approach_atom_end, approach_atom_handle_deleted_object };
s_atom_definition g_atom_deploy_turret = { 0, deploy_atom_update, 0, 0 };
s_atom_definition g_atom_custom_animation = { custom_animation_start, custom_animation_update, 0, 0 };
s_atom_definition g_atom_play_sound = { pause_start, pause_update, 0, 0 };
s_atom_definition g_atom_melee = { melee_start, melee_update, 0, 0 };
s_atom_definition g_atom_smash = { smash_start, smash_update, 0, 0 };
s_atom_definition g_atom_swarm_to = { swarm_to_start, swarm_to_update, 0, 0 };
s_atom_definition g_atom_swarm_from = { swarm_from_start, swarm_from_update, 0, 0 };
#endif
