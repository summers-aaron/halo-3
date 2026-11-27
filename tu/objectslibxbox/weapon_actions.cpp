// objectslibxbox/weapon_actions.obj: from the records 0xee150 0x37e5b0 0x37e638 0x37e6a8 0x37e8b0 0x37e9a8 0x37eab0 0x37ebc8 0x37ed30 0x37eda8 0x37ee20 0x37eff0 0x37f0e0 0x37f2a0 0x37f2f0 0x37f398 0x37f460 0x37f7b0 0x37fae0 0x37fcc8 0x37fe68 0x380470 0x380ab0 0x380bc8 0x3817f0 0x3818b8 0x381990 0x381b30 0x381cb8 0x382020 0x382070
// @compiler xdk6130
// @flags /TP /O2
#include "units/unit_inventory_types.h"
#include "math/real_constant_pool.h"
#include "math/real_math_constants.h"
#include "physics/character_physics_globals.h"
#include "effects/effect_creation.h"
#include "game/game_globals.h"
#include "items/weapon_definitions.h"
#include "ai/actor_type_flood_types.h"
#include "objects/object_datum.h"
#include "math/real_math_types.h"
#include "math/real_math.h"
#include "tag_files/tag_groups.h"
#include "objects/object_placement.h"
#include "units/motor_enums.h"
#include "units/motor_request.h"
#include "units/action_request.h"
#include "units/motor_program_state.h"
#include "ai/actor_globals.h"
#include <stdlib.h>
#include <new>
#include <string.h>
#include "cseries/cseries.h"
#include "cseries/bit_operations.h"
#include "units/unit_messages.h"
// match: 0x37fae0: intrinsic pragma selects hardware rotate instruction
#pragma intrinsic(_rotl)
// @address 0xa05d8 ?weapon_reloading@@YA_NJJPAF@Z
// @address 0xdd5b0 ?motor_task_allow@@YA_NJW4e_action@@F@Z
// @address 0x33e620 ?motor_system_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
// @address 0x355a60 ?motor_task_interrupt@@YAXJFF@Z
// @address 0xe8f00 ?motor_system_in_airborne_state@@YA_NJ@Z
#include "motor/motor_animation.h"
#include "objects/objects.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "units/units.h"
#include "ai/actor_combat.h"
#include "ai/ai.h"
#include "ai/behavior.h"
#include "items/items.h"
#include "items/projectiles.h"
#include "items/weapons.h"
#include "motor/motor_system.h"
#include "motor/weapon_actions.h"
#include "objects/object_early_movers.h"
#include "units/weapon_addition_method_types.h"
#include "units/motor_task.h"
#include "objects/object_types.h"
#include "interface/chud/chud_message_context_types.h"
#include "objects/object_reference_types.h"
#include "units/unit_animation.h"
#include "ai/actors.h"
#include "ai/props.h"
#include "motor/action_system.h"
#include "physics/havok.h"
#include "ai/character_definitions.h"
#include "math/matrix_math.h"
#include "game/players.h"
#include "math/matrix_math_types.h"
#include "animations/mixing_board/channels/animation_channel.h"
#include "cache/cache_files.h"
#include "scenario/scenario.h"
#include "interface/first_person_weapons.h"
#include "interface/chud/chud_messaging.h"
#include "animations/animation_interpolation.h"
#include "units/unit_types.h"
#include "units/unit_definitions.h"
#include "models/render_model_definitions.h"
#include "ai/actor_datum.h"
#include "game/player_types.h"
#include "memory/tls_globals.h"
#include "game/player_mapping_types.h"
#include "units/biped_types.h"
#include "physics/character_physics_mode_ground_datum.h"
#include "objects/object_definitions.h"
#include "models/model_types.h"
#include "game/game_time_types.h"
#include "ai/character_behavior_properties.h"

#include "ai/weapon_datum_types.h"

#include "items/projectile_types.h"

struct s_weapon_actions_grenade_aim_options {
    float walking_speed;
    float run_forward;
    float run_backward;
    float run_sideways;
    float run_acceleration;
    float sneak_forward;
    float sneak_backward;
    float sneak_sideways;
    float sneak_acceleration;
    float airborne_acceleration;
    float forward_distance;
    float side_distance;
    float up_distance;
};

#include "units/unit_weapon_pickup_result.h"
#include "game/player_iterator.h"

#include "memory/data_inline.h"
#include "memory/data.h"
#include "objects/object_header.h"
#include "math/random_math_range.h"
#include "ai/actor_behavior_state.h"
#include "motor/mover_inline.h"
#include "tag_files/tag_index.h"

static __forceinline bool weapon_actions_seat_flag_test(unsigned long flags, long bit)
{
    return (flags >> bit) & 1;
}

static __forceinline long weapon_actions_inventory_get_weapon(long index, short slot)
{
    unit_datum *unit = ((unit_datum *)object_get(index));
    long result = -1;
    if (slot != -1)
        result = unit->unit.weapon_object_indices[slot];
    return result;
}

static __forceinline s_weapon_action_block *weapon_actions_header_block_get(unit_datum *unit)
{
    return (s_weapon_action_block *)((char *)unit + unit->unit.action_offset);
}

static __forceinline bool weapon_actions_unit_acceleration_prevent_action(long index)
{
    unit_datum *unit = ((unit_datum *)object_get(index));
    return (unit->unit.unit_flags & (1 << 24)) != 0;
}

static __forceinline bool weapon_actions_unit_can_use_weapon(long unit_index, long weapon_index)
{
    return ::unit_can_use_weapon_in_seat(unit_index, -1, weapon_index);
}

static __forceinline long weapon_actions_unit_current_weapon_get(const unit_datum *unit)
{
    long result = -1;
    if (unit->unit.current_weapon_indices[0] != -1)
        result = unit->unit.weapon_object_indices[unit->unit.current_weapon_indices[0]];
    return result;
}

bool unit_weapon_in_overheated_loop(long unit_index, long weapon_slot)
{
    bool result = false;
    unit_datum *unit = (unit_datum *)object_get(unit_index);
    long current_weapon_index = unit->unit.current_weapon_indices[weapon_slot];
    if (current_weapon_index != -1) {
        long weapon_index = unit->unit.weapon_object_indices[current_weapon_index];
        if (weapon_index != -1) {
            unsigned short flags = ((weapon_datum *)object_get(weapon_index))->flags;
            result = flag_test(flags, 1) && !flag_test(flags, 2);
        }
    }
    return result;
}

void unit_action_send_message_to_first_person(long unit_index, e_unit_messages message)
{
    long hand = message == 19 || message == 21 || message == 25;
    long weapon_index = -1;
    unit_datum *unit = (unit_datum *)object_get(unit_index);
    long current_weapon_index = unit->unit.current_weapon_indices[hand];
    if (current_weapon_index != -1)
        weapon_index = unit->unit.weapon_object_indices[current_weapon_index];
    first_person_weapon_handle_message(unit_index, weapon_index, message, hand);
}

static __forceinline void weapon_actions_weapon_unready(long weapon_index)
{
    weapon_datum *weapon = (weapon_datum *)object_get(weapon_index);
    weapon->control_flags = 0;
    weapon_reset(weapon_index);
}

// @address 0x37e8b0 ?unit_action_weapon_put_away_end@@YAXJ_N@Z
void unit_action_weapon_put_away_end(long unit_index, bool primary)
{
    long hand = !primary;
    long weapon_index = weapon_actions_inventory_get_weapon(
        unit_index, ((unit_datum *)object_get(unit_index))->unit.current_weapon_indices[hand]);
    if (weapon_index != -1) {
        unit_datum *unit = ((unit_datum *)object_get(unit_index));
        weapon_interrupt_reload(weapon_index, false);
        weapon_put_away(weapon_index, false, true);
        weapon_actions_weapon_unready(weapon_index);
        if (weapon_can_be_stowed(weapon_index, unit_index))
            weapon_stow(weapon_index, unit_index);
        else
            item_hide_inventory(weapon_index);
        unit->unit.current_weapon_indices[hand] = -1;
        unit_action_send_message_to_first_person(unit_index, primary ? _unit_message_put_away_end_primary : _unit_message_put_away_end_secondary);
    }
}

// @address 0x37eab0 ?unit_action_weapon_set_unarmed_animation@@YAXJ_N@Z
void unit_action_weapon_set_unarmed_animation(long unit_index, bool force)
{
    long weapon_class = 1;
    long weapon_type = 1;
    unit_datum *unit = ((unit_datum *)object_get(unit_index));
    long parent_index = unit->object.parent_object_index;
    if ((unit->object.parent_object_index) != -1 && unit->unit.parent_seat_index != -1) {
        unit_datum *parent_definition_datum = (unit_datum *)object_get(parent_index);
        unit_definition *definition =
            (unit_definition *)tag_get('unit', parent_definition_datum->object.definition_index);
        unit_seat *seat = (unit_seat *)tag_block_get_element_with_size(
            &definition->seats, unit->unit.parent_seat_index, sizeof(unit_seat));
        unit_datum *parent = ((unit_datum *)object_get(unit->object.parent_object_index));
        long parent_weapon_index = weapon_actions_unit_current_weapon_get(parent);
        if (weapon_actions_seat_flag_test(seat->flags, 3) && !weapon_actions_seat_flag_test(seat->flags, 2) && parent_weapon_index != -1) {
            weapon_datum *weapon = ((weapon_datum *)object_get(parent_weapon_index));
            weapon_definition *weapon_tag =
                (weapon_definition *)tag_get('weap', weapon->definition_index);
            weapon_class = weapon_tag->animation_weapon_class;
            weapon_type = weapon_tag->animation_weapon_type;
        }
    }
    unit_animation_set_weapon(unit_index, weapon_class, weapon_type, force);
}

// @address 0x37ebc8 ?unit_action_weapon_switch_submit@@YA_NJPBUaction_request@@@Z
bool unit_action_weapon_switch_submit(long unit_index, const action_request *request)
{
    unit_datum *unit = ((unit_datum *)object_get(unit_index));
    bool primary = request->action == _action_primary_weapon_switch;
    long weapon_set_index = !primary;
    short task;
    if (primary) {
        if ((unsigned char)unit->unit.desired_weapon_indices[1] == 255)
            task = 12;
        else
            task = 4;
    } else
        task = 8;
    bool result = false;
    if (request->immediate ||
        ((unsigned char)unit->unit.desired_weapon_indices[weapon_set_index] != (unsigned char)unit->unit.current_weapon_indices[weapon_set_index] &&
            motor_task_allow(unit_index, (e_action)request->action, task) && !weapon_actions_unit_acceleration_prevent_action(unit_index)))
    {
        long weapon_index = weapon_actions_inventory_get_weapon(
            unit_index, ((unit_datum *)object_get(unit_index))->unit.desired_weapon_indices[weapon_set_index]);
        if ((unsigned char)unit->unit.desired_weapon_indices[weapon_set_index] == 255 ||
            (weapon_index != -1 && weapon_actions_unit_can_use_weapon(unit_index, weapon_index)))
        {
            if (unit_action_weapon_put_away_submit(unit_index, request->action, request->immediate, request->simulated, request->force, primary)) {
                bool ready = request->force;
                unit_action_weapon_ready(unit_index, 226, ready, request->force, primary);
            }
            result = true;
        }
    }
    return result;
}

// @address 0x37ed30 ?unit_action_weapon_switch_interrupt@@YAXJW4e_action@@@Z
void unit_action_weapon_switch_interrupt(long unit_index, e_action action)
{
    bool primary = action == 54;
    if (unit_action_weapon_put_away_submit(unit_index, 54, true, false, false, primary))
        unit_action_weapon_ready(unit_index, 226, true, false, primary);
}

// @address 0x37eda8 ?unit_action_weapon_switch_finished@@YAXJW4e_action@@@Z
void unit_action_weapon_switch_finished(long unit_index, e_action action)
{
    bool primary = action == 54;
    if (unit_action_weapon_put_away_submit(unit_index, 54, true, false, false, primary))
        unit_action_weapon_ready(unit_index, 226, false, false, primary);
}

// @address 0x37e6a8 unit_action_weapon_put_away_submit
bool unit_action_weapon_put_away_submit(long unit_index, long action, bool immediate, bool simulated, bool flags, bool primary)
{
    long weapon_index = weapon_actions_inventory_get_weapon(
        unit_index, ((unit_datum *)object_get(unit_index))->unit.current_weapon_indices[!primary]);
    bool result = true;
    if (weapon_index != -1)
    {
        long animation = primary ? 39 : 467;
        c_animation_channel *channel = motor_animation_resource_get_channel(unit_index, (e_motor_resource)(primary ? 2 : 3));
        if (channel->m_definition_index == animation && !immediate)
        {
            result = false;
        }
        else if (flags && immediate)
        {
            result = true;
        }
        else if (weapon_put_away(weapon_index, flags, immediate))
        {
            unit_unzoom(unit_index, false);
            if (!flags)
                unit_action_send_message_to_first_person(unit_index, primary ? _unit_message_put_away_primary : _unit_message_put_away_secondary);
            if (immediate)
            {
                if (simulated)
                {
                    short resource_mask;
                    if (primary)
                    {
                        if ((unsigned char)((unit_datum *)object_get(unit_index))->unit.desired_weapon_indices[1] == 0xff)
                            resource_mask = 12;
                        else
                            resource_mask = 4;
                    }
                    else
                    {
                        resource_mask = 8;
                    }
                    motor_task_interrupt(unit_index, resource_mask, resource_mask);
                }
                result = true;
            }
            else
            {
                c_motor_request request;
                request.setup_weapon_animation((e_action)action, animation, primary ? 0 : 3, k_standard_interpolation_time);
                request.set_force(true);
                result = !motor_system_submit(unit_index, &request);
            }
        }
        if (result)
            unit_action_weapon_put_away_end(unit_index, primary);
    }
    return result;
}

// @address 0x37ee20 unit_action_weapon_animation_submit
extern "C" bool unit_action_weapon_animation_submit(long object_index, long *source)
{
    long index = *source;
    float interpolation = 0.06666667f;
    long animation, config;
    switch (index)
    {
    case _action_primary_weapon_primary_recoil:
    case _action_primary_weapon_secondary_recoil:
        interpolation = k_real_zero_storage;
        config = 0;
        break;
    case _action_primary_weapon_primary_chamber:
    case _action_primary_weapon_secondary_chamber:
    case _action_primary_weapon_primary_charged:
    case _action_primary_weapon_secondary_charged:
    case _action_primary_weapon_overheat_enter:
    case _action_primary_weapon_overheat_loop:
    case _action_primary_weapon_overheat_exit:
        config = 0;
        break;
    case _action_secondary_weapon_primary_recoil:
    case _action_secondary_weapon_secondary_recoil:
        interpolation = k_real_zero_storage;
        config = 3;
        break;
    case _action_secondary_weapon_primary_chamber:
    case _action_secondary_weapon_secondary_chamber:
    case _action_secondary_weapon_primary_charged:
    case _action_secondary_weapon_secondary_charged:
    case _action_secondary_weapon_overheat_enter:
    case _action_secondary_weapon_overheat_loop:
    case _action_secondary_weapon_overheat_exit:
        config = 3;
        break;
    default:
        // match: 0x37ee20: unreachable hint omits dead branch check
        __assume(0);
    }
    switch (index)
    {
    case _action_primary_weapon_primary_recoil: animation = 6; break;
    case _action_primary_weapon_secondary_recoil: animation = 7; break;
    case _action_primary_weapon_primary_chamber: animation = 4; break;
    case _action_primary_weapon_secondary_chamber: animation = 5; break;
    case _action_primary_weapon_primary_charged: animation = 8; break;
    case _action_primary_weapon_secondary_charged: animation = 9; break;
    case _action_secondary_weapon_primary_recoil: animation = 472; break;
    case _action_secondary_weapon_secondary_recoil: animation = 473; break;
    case _action_secondary_weapon_primary_chamber: animation = 470; break;
    case _action_secondary_weapon_secondary_chamber: animation = 471; break;
    case _action_secondary_weapon_primary_charged: animation = 474; break;
    case _action_secondary_weapon_secondary_charged: animation = 475; break;
    case _action_primary_weapon_overheat_enter: animation = 113; break;
    case _action_primary_weapon_overheat_loop: animation = 106; break;
    case _action_primary_weapon_overheat_exit: animation = 117; break;
    case _action_secondary_weapon_overheat_enter: animation = 665; break;
    case _action_secondary_weapon_overheat_loop: animation = 666; break;
    case _action_secondary_weapon_overheat_exit: animation = 667; break;
    default:
        // match: 0x37ee20: unreachable hint omits dead branch check
        __assume(0);
    }
    c_motor_request request;
    request.setup_weapon_animation((e_action)index, animation, config, interpolation);
    return motor_system_submit(object_index, &request) != 0;
}

// @address 0x37eff0 unit_action_weapon_reaction_submit_helper
extern "C" void unit_action_weapon_reaction_submit_helper(long index, long action)
{
    float interpolation = 0.06666667f;
    long animation, canonical_action, config;
    switch (action)
    {
    case _action_primary_weapon_overheat_loop:
        interpolation = k_real_zero_storage;
    case _action_primary_weapon_overheat_enter:
        if (!unit_weapon_in_overheated_loop(index, 0))
            return;
        animation = 106;
        canonical_action = _action_primary_weapon_overheat_loop;
        config = 0;
        break;
    case _action_secondary_weapon_overheat_loop:
        interpolation = k_real_zero_storage;
    case _action_secondary_weapon_overheat_enter:
        if (!unit_weapon_in_overheated_loop(index, 1))
            return;
        animation = 666;
        canonical_action = _action_secondary_weapon_overheat_loop;
        config = 3;
        break;
    default:
        return;
    }
    c_motor_request request;
    request.setup_weapon_animation((e_action)canonical_action, animation, config, interpolation);
    motor_system_submit(index, &request);
}

// @address 0x37f0e0 ?unit_action_weapon_reload_submit@@YA_NJW4e_action@@JJ@Z
bool unit_action_weapon_reload_submit(long unit_index, e_action action, long weapon_set_index, long magazine_index)
{
    bool result = false;
    unit_datum *unit = ((unit_datum *)object_get(unit_index));
    s_weapon_action_block *block = weapon_actions_header_block_get(unit);
    block->flags &= ~8;
    long config;
    long animation;
    short resource_mask;
    if (weapon_set_index == 0) {
        config = 0;
        resource_mask = 4;
        animation = (magazine_index != 0) + 2;
    } else {
        config = 3;
        resource_mask = 8;
        animation = (magazine_index != 0) + 468;
    }
    if (motor_task_allow(unit_index, action, resource_mask) && !weapon_actions_unit_acceleration_prevent_action(unit_index)) {
        unit_datum *reload_unit = ((unit_datum *)object_get(unit_index));
        signed char current_weapon_index = reload_unit->unit.current_weapon_indices[weapon_set_index];
        if (current_weapon_index != -1) {
            long weapon_index = reload_unit->unit.weapon_object_indices[current_weapon_index];
            if (weapon_index != -1 && ::weapon_reload(weapon_index, magazine_index)) {
                ::unit_unzoom(unit_index, false);
                short magazine = (short)magazine_index;
                weapon_datum *weapon = ((weapon_datum *)object_get(weapon_index));
                if (weapon->magazines[magazine].state != 2) {
                    c_motor_request motor;
                    motor.setup_weapon_animation(action, animation, config, k_standard_interpolation_time);
                    block->flags |= 8;
                    motor_system_submit(unit_index, &motor);
                    block->flags &= ~8;
                } else {
                    motor_task_interrupt(unit_index, resource_mask, resource_mask);
                }
                result = true;
            }
        }
    }
    return result;
}

// @address 0x37f2a0 ?unit_action_weapon_reload_submit_helper@@YA_NJPBUaction_request@@@Z
bool unit_action_weapon_reload_submit_helper(long unit_index, const action_request *request)
{
    long action = request->action;
    long weapon_set_index = action > 62;
    long magazine_index;
    switch (action)
    {
    case _action_primary_weapon_primary_reload:
    case _action_secondary_weapon_primary_reload:
        magazine_index = 0;
        break;
    case _action_primary_weapon_secondary_reload:
    case _action_secondary_weapon_secondary_reload:
        magazine_index = 1;
        break;
    default:
        // match: 0x37f2a0: unreachable hint omits dead branch check
        __assume(0);
    }
    return unit_action_weapon_reload_submit(unit_index, (e_action)action, weapon_set_index, magazine_index);
}

// @address 0x37f2f0 ?unit_action_weapon_reload_update_helper@@YA_NJW4e_action@@@Z
bool unit_action_weapon_reload_update_helper(long index, e_action action)
{
    bool result = false;
    long weapon;
    switch (action)
    {
    case _action_primary_weapon_primary_reload:
    case _action_primary_weapon_secondary_reload:
        weapon = weapon_actions_inventory_get_weapon(index, ((unit_datum *)object_get(index))->unit.current_weapon_indices[0]);
        break;
    case _action_secondary_weapon_primary_reload:
    case _action_secondary_weapon_secondary_reload:
        weapon = weapon_actions_inventory_get_weapon(index, ((unit_datum *)object_get(index))->unit.current_weapon_indices[1]);
        break;
    default:
        // match: 0x37f2f0: unreachable hint omits dead branch check
        __assume(0);
    }
    short magazine;
    switch (action)
    {
    case _action_primary_weapon_primary_reload:
    case _action_secondary_weapon_primary_reload:
        magazine = 0;
        break;
    case _action_primary_weapon_secondary_reload:
    case _action_secondary_weapon_secondary_reload:
        magazine = 1;
        break;
    default:
        // match: 0x37f2f0: unreachable hint omits dead branch check
        __assume(0);
    }
    if (weapon != -1)
        result = weapon_reloading(weapon, magazine, 0);
    return result;
}

// @address 0x37f398 ?unit_action_weapon_reload_interrupt_helper@@YAXJW4e_action@@@Z
void unit_action_weapon_reload_interrupt_helper(long unit_index, e_action action)
{
    unit_datum *unit = ((unit_datum *)object_get(unit_index));
    if (!((unsigned long)weapon_actions_header_block_get(unit)->flags & 8))
    {
        bool successful = false;
        bool empty = false;
        long weapon;
        if (action > 62)
        {
            weapon = -1;
            if (unit->unit.current_weapon_indices[1] != -1)
                weapon = unit->unit.weapon_object_indices[unit->unit.current_weapon_indices[1]];
            empty = true;
        }
        else
        {
            weapon = -1;
            if (unit->unit.current_weapon_indices[0] != -1)
                weapon = unit->unit.weapon_object_indices[unit->unit.current_weapon_indices[0]];
            successful = true;
        }
        if (weapon != -1)
        {
            weapon_interrupt_reload(weapon, false);
            simulation_action_unit_reload_interrupt(unit_index, successful, empty);
        }
    }
}

static __forceinline long weapon_actions_find_marker_group(long render_model_index, long name)
{
    long result = NONE;
    if (render_model_index != NONE)
    {
        const render_model_definition *model = (const render_model_definition *)tag_get('mode', render_model_index);
        for (long index = 0; index < model->marker_groups.count; index++)
        {
            const render_model_marker_group *group = &((const render_model_marker_group *)model->marker_groups.address)[index];
            if (group->name == name)
            {
                result = index;
                break;
            }
        }
    }
    return result;
}

static __forceinline long weapon_actions_get_default_markers_by_group(
    long render_model_index, long group_index, bool mirror, object_marker *markers, long maximum_count)
{
    long count = 0;
    if (group_index != NONE)
    {
        // match: 0x37f460: const reference binding preserves temporary lifetime
        const unsigned long &absolute_index = (unsigned short)render_model_index;
        const render_model_definition *model = (const render_model_definition *)global_tag_instances[absolute_index].definition;
        const render_model_marker_group *group = &((const render_model_marker_group *)model->marker_groups.address)[group_index];
        for (long index = 0; index < group->markers.count; index++)
        {
            const render_model_marker *marker = &((const render_model_marker *)group->markers.address)[index];
            if (count >= maximum_count)
                break;
            object_marker *result = &markers[count++];
            long node_index = marker->node_index;
            result->node_index = (short)node_index;
            matrix4x3_from_point_and_quaternion(&result->node_to_marker, &marker->translation, &marker->rotation);
            const render_model_node *node = &((const render_model_node *)model->nodes.address)[node_index];
            real_matrix4x3 node_matrix;
            matrix4x3_from_point_and_quaternion(&node_matrix, &node->default_translation, &node->default_rotation);
            matrix4x3_compute_product(&node_matrix, &result->node_to_marker, &result->matrix);
            result->scale = marker->scale;
            if (mirror)
            {
                negate_vector3d(&result->matrix.left, &result->matrix.left);
            }
        }
    }
    return count;
}

static __forceinline long weapon_actions_get_default_markers(long render_model_index, long name, bool mirror, object_marker *markers, long maximum_count)
{
    long group_index = weapon_actions_find_marker_group(render_model_index, name);
    return weapon_actions_get_default_markers_by_group(render_model_index, group_index, mirror, markers, maximum_count);
}

static __forceinline long weapon_actions_grenade_player_index(const unit_datum *unit)
{
    return unit->unit.player_index != NONE ? unit->unit.player_index : unit->unit.controller_player_index;
}

static __forceinline void weapon_actions_grenade_damage_owner_set(s_damage_owner *damage_owner, long object_index, long player_index, long team)
{
    memset(damage_owner, 0, sizeof(*damage_owner));
    damage_owner->object_index = object_index;
    damage_owner->player_index = player_index;
    damage_owner->team = (char)team;
}

// @address 0x37f460 unit_throw_grenade_create
extern "C" bool unit_throw_grenade_create(long unit_index, long projectile_definition_index, long *projectile_index)
{
    unit_datum *unit = ((unit_datum *)object_get(unit_index));
    s_weapon_action_block *action = weapon_actions_header_block_get(unit);
    bool result = false;
    object_placement_data placement;
    s_damage_owner owner;
    weapon_actions_grenade_damage_owner_set(&owner, unit_index, weapon_actions_grenade_player_index(unit), unit->unit.team);
    object_placement_data_new(&placement, projectile_definition_index, unit_index, &owner);
    placement.flags |= 4;
    const s_object_definition *definition =
        (const s_object_definition *)tag_get('obje', projectile_definition_index);
    const s_model_definition *model = (const s_model_definition *)tag_get('hlmt', definition->model.index);
    object_marker hand_marker;
    internal_object_get_markers_by_string_id(unit_index, 225, &hand_marker, 1, false);
    short hand_node_index = hand_marker.node_index;
    object_marker projectile_marker[1];
    if (weapon_actions_get_default_markers(model->render_model_index, 225, (placement.flags & 1) != 0, projectile_marker, 1))
    {
        real_matrix4x3 inverse;
        real_matrix4x3 transform;
        matrix4x3_inverse(&projectile_marker[0].matrix, &inverse);
        matrix4x3_compute_product(&hand_marker.matrix, &inverse, &transform);
        placement.forward = transform.forward;
        placement.up = transform.up;
        placement.position = transform.position;
    }
    else
    {
        placement.forward = ((unit_datum *)object_get(unit_index))->unit.aiming_vector;
        normalize3d(perpendicular3d(&placement.forward, &placement.up));
        placement.position = hand_marker.matrix.position;
    }
    long created_index = object_new(&placement);
    if (created_index != NONE)
    {
        object_attach_to_node_with_type(unit_index, created_index, hand_node_index);
        action->grenade_index = created_index;
        if (projectile_index)
            *projectile_index = created_index;
        action->grenade_throw_state = 2;
        result = true;
    }
    if (!result)
        action->grenade_throw_state = 3;
    return result;
}

// @address 0x37f7b0 unit_throw_grenade
extern "C" long unit_throw_grenade(long unit_index, const real_point3d *position, const real_vector3d *direction,
    const real_point3d *origin, const real_vector3d *forward, const real_vector3d *up, real_vector3d *velocity, bool primary, bool keep)
{
    long result = -1;
    unit_datum *unit = ((unit_datum *)object_get(unit_index));
    s_weapon_action_block *block = weapon_actions_header_block_get(unit);
    if (block->grenade_index != -1 && !object_try_and_get_and_verify_type(block->grenade_index, 0xffffffff))
        block->grenade_index = -1;
    if (block->grenade_index != -1)
    {
        long grenade_index = block->grenade_index;
        long grenade_type = block->grenade_type;
        bool created = false;
        object_datum *grenade = object_get(grenade_index);
        block->grenade_index = -1;
        block->grenade_type = -1;
        unsigned long flags = grenade->flags;
        session_flag_set(grenade->flags, 17, false);
        grenade->physics_flags &= ~(1 << 1);
        object_detach(grenade_index);
        session_flag_set(grenade->flags, 17, weapon_actions_seat_flag_test(flags, 17));
        real_point3d camera_position;
        real_vector3d vector;
        if (simulation_query_local_predictive_control(unit_index, 1))
        {
            unit_get_camera_position(unit_index, &camera_position);
            vector = unit->unit.aiming_vector;
            simulation_request_unit_grenade_release(unit_index, !primary, grenade_type, !primary && origin ? origin : &camera_position, &vector);
        }
        else if (!block->immediate)
        {
            if (keep)
            {
                created = true;
            }
            else
            {
                if (origin || (forward && up))
                {
                    object_set_position_direct(grenade_index, origin, forward, up, 0, false);
                    havok_object_set_position(grenade_index, false, false, true);
                }
                if (weapon_actions_seat_flag_test(grenade->physics_flags, 3))
                {
                    real_vector3d angular_velocity;
                    object_get_center_of_mass(grenade_index, &camera_position);
                    if (object_get_early_mover_local_space_velocity(grenade_index, &camera_position, &vector, &angular_velocity, true, false))
                    {
                        velocity->i += vector.i;
                        velocity->j += vector.j;
                        velocity->k += vector.k;
                    }
                }
                cross_product3d(&unit->object.up, &unit->object.forward, &vector);
                object_set_velocities(grenade_index, &k_real_math_zero_vector3d_storage, &k_real_math_zero_vector3d_storage);
                if ((1 << grenade->object_identifier.get_type()) & (1 << 5))
                    projectile_accelerate(grenade_index, velocity, &vector);
                else
                    object_set_velocities(grenade_index, velocity, 0);
                unit_get_camera_position(unit_index, &camera_position);
                created = object_force_inside_bsp(grenade_index, &camera_position, unit_index);
            }
        }
        if (created)
        {
            simulation_action_object_create(grenade_index);
            result = grenade_index;
        }
        else
        {
            object_delete(grenade_index);
        }
        ::unit_active_camouflage_ding(unit_index, 0.6f, 0.0f);
    }
    block->grenade_throw_state = 3;
    return result;
}

// @address 0x37fae0 ?unit_get_inherited_velocity@@YA_NJ_NPBTreal_point3d@@PBTreal_vector3d@@PAT2@@Z
bool unit_get_inherited_velocity(long unit_index, bool inherit, const real_point3d *position, const real_vector3d *direction, real_vector3d *result)
{
    biped_datum *object = (biped_datum *)object_get(unit_index);
    bool success = false;
    real_vector3d velocity;
    ::object_get_velocities(unit_index, &velocity, 0);
    real_vector3d local_velocity, angular_velocity;
    if ((bool)(_rotl(object->object.physics_flags, 29) & 1) &&
        ::object_get_early_mover_local_space_velocity(unit_index, position, &local_velocity,
            &angular_velocity, true, false))
    {
        subtract_vectors3d(&velocity, &local_velocity, &velocity);
        success = true;
    }
    if (((1 << object_header_get(unit_index)->object_type) & 1) && !motor_system_in_airborne_state(unit_index)) {
        biped_datum *biped = (biped_datum *)object_get(unit_index);
        if (biped->biped.physics.m_mode == 1) {
            const real_vector3d *ground = biped->biped.physics.get_mode_ground()->get_ground_plane();
            real_vector3d perpendicular;
            scale_vector3d(ground, dot_product3d(ground, &velocity), &perpendicular);
            subtract_vectors3d(&velocity, &perpendicular, &velocity);
        }
    }
    scale_vector3d(direction, dot_product3d(direction, &velocity), result);
    if (inherit && success)
        add_vectors3d(result, &local_velocity, result);
    return success;
}

static __forceinline bool weapon_actions_actor_get_grenade_override(long actor_index, long *projectile_definition)
{
    character_engage_properties *properties = character_definition_engage_multi_properties_get(actor_index);
    bool result = false;
    if (properties && properties->override_grenade_projectile.index != -1)
    {
        *projectile_definition = properties->override_grenade_projectile.index;
        result = true;
    }
    return result;
}

static __forceinline s_grenade_definition *weapon_actions_grenade_definition_get(long grenade_type)
{
    return ((s_game_globals *)global_game_globals)->grenades.count()
        ? &((s_grenade_definition *)((s_game_globals *)global_game_globals)->grenades.address)[grenade_type]
        : 0;
}

// @address 0x37fcc8 unit_throw_grenade_initiate
extern "C" void unit_throw_grenade_initiate(long unit_index, bool infinite)
{
    long projectile_definition = -1;
    bool create = false;
    unit_datum *unit = ((unit_datum *)object_get(unit_index));
    long actor = unit->unit.actor;
    s_weapon_action_block *block = weapon_actions_header_block_get(unit);
    if (actor != -1 && weapon_actions_actor_get_grenade_override(actor, &projectile_definition))
    {
        create = true;
    }
    else
    {
        signed char grenade_type = unit->unit.current_grenade_index;
        if (grenade_type != -1)
        {
            projectile_definition = weapon_actions_grenade_definition_get(grenade_type)->projectile.index;
            create = true;
            if (actor == -1 && !block->immediate)
            {
                if (unit->unit.grenade_counts[grenade_type] > 0)
                {
                    unit->unit.grenade_counts[unit->unit.current_grenade_index]--;
                    simulation_action_object_update(unit_index, 0x4000000);
                }
                else if (!infinite)
                {
                    create = false;
                }
            }
        }
    }
    if (projectile_definition != -1 && create)
    {
        long projectile_index;
        if (unit_throw_grenade_create(unit_index, projectile_definition, &projectile_index))
        {
            block->grenade_type = unit->unit.current_grenade_index;
            if (projectile_index != -1)
            {
                projectile_datum *projectile = (projectile_datum *)object_get(projectile_index);
                projectile->projectile.flags.set(_projectile_use_physics_bit, true);
            }
        }
    }
}

// @rdata 0x82022514
static const float k_grenade_fumble_velocity_minimum = 0.6f;
// @address 0x37fe68 ?unit_throw_grenade_release@@YAXJ_NPBTreal_point3d@@PBTreal_vector3d@@@Z
void unit_throw_grenade_release(long unit_index, bool release, const real_point3d *position, const real_vector3d *direction)
{
    unit_datum *unit = ((unit_datum *)object_get(unit_index));
    s_weapon_action_block *block = weapon_actions_header_block_get(unit);
    unit_definition *definition = (unit_definition *)tag_get('unit', unit->object.definition_index);
    if (block->grenade_index != -1 && !object_try_and_get_and_verify_type(block->grenade_index, 0xffffffff))
        block->grenade_index = -1;
    if (block->grenade_index != -1)
    {
        long grenade_index = block->grenade_index;
        short grenade_type = (short)block->grenade_type;
        bool player_aimed = false;
        bool attached = false;
        real_vector3d velocity;
        real_point3d player_origin;
        object_marker markers[1];
        if (unit->object.parent_object_index != -1 && unit->unit.parent_seat_index != -1 && grenade_type != -1)
        {
            unit_datum *parent = ((unit_datum *)object_get(unit->object.parent_object_index));
            unit_definition *parent_definition =
                (unit_definition *)tag_get('unit', parent->object.definition_index);
            unit_seat *seat = (unit_seat *)tag_block_get_element_with_size(
                &parent_definition->seats, unit->unit.parent_seat_index, sizeof(unit_seat));
            if (weapon_actions_seat_flag_test(seat->flags, 11))
                attached = internal_object_get_markers_by_string_id(unit->object.parent_object_index, seat->boarding_grenade_marker, markers, 1, false) > 0;
        }
        if (unit->unit.actor != -1 && grenade_type != -1)
        {
            real_point3d grenade_origin;
            object_get_origin(grenade_index, &grenade_origin);
            actor_aim_grenade(unit->unit.actor, &grenade_origin, &velocity);
        }
        else
        {
            real_vector3d aiming_vector;
            real_point3d aiming_origin;
            if (position && direction)
            {
                aiming_origin = *position;
                aiming_vector = *direction;
            }
            else
            {
                unit_get_camera_position(unit_index, &aiming_origin);
                aiming_vector = unit->unit.aiming_vector;
            }
            if (unit->unit.player_index != -1)
            {
                s_weapon_actions_grenade_aim_options *options = (s_weapon_actions_grenade_aim_options *)global_game_globals->player_information.address;
                real_vector3d forward = aiming_vector;
                real_vector3d left;
                real_vector3d up;
                real_point3d spawn_position;
                if (normalize3d(cross_product3d(global_up3d, &forward, &left)) == 0.0f)
                    left = *global_up3d;
                normalize3d(cross_product3d(&forward, &left, &up));
                point_from_line3d(&aiming_origin, &forward, options->forward_distance, &spawn_position);
                point_from_line3d(&spawn_position, &left, options->side_distance, &spawn_position);
                point_from_line3d(&spawn_position, &up, options->up_distance, &spawn_position);
                player_origin = spawn_position;
                player_aimed = true;
                release = false;
            }
            real_point3d origin;
            if (player_aimed)
                origin = player_origin;
            else
                object_get_origin(grenade_index, &origin);
            real_vector3d inherited;
            if (unit_get_inherited_velocity(unit_index, false, &origin, &aiming_vector, &inherited))
            {
                object_in_early_mover_join(unit_index, grenade_index);
                object_header_get(grenade_index)->flags |= 8;
            }
            scale_vector3d(&aiming_vector, definition->grenade_throw_speed, &velocity);
            add_vectors3d(&inherited, &velocity, &velocity);
        }
        if (release)
        {
            float fraction = block->release_ticks > 0 ? (float)block->throw_ticks / (float)block->release_ticks : 1.0f;
            if (fraction < 1.0f)
            {
                real_vector3d fumble;
                float scale = _real_random_range(TLS_RANDOM_SEED, k_grenade_fumble_velocity_minimum, 1.4f);
                scale_vector3d(&unit->unit.aiming_vector, scale, &fumble);
                scale_vector3d(&velocity, fraction, &velocity);
                point_from_line3d(&velocity, &fumble, 1.0f - fraction, &velocity);
            }
        }
        long thrown_index = unit_throw_grenade(unit_index, position, direction, player_aimed ? &player_origin : 0, 0, 0, &velocity, true, attached);
        if (thrown_index != -1)
        {
            if (attached)
            {
                projectile_datum *projectile = (projectile_datum *)object_get(thrown_index);
                projectile->projectile.flags.set(_projectile_attached_bit, true);
                projectile_attach(thrown_index, unit->object.parent_object_index, markers[0].node_index, &markers[0].matrix.position, 0, 0);
                ::unit_cause_player_melee_attack(unit_index, 34, 0, 1.0f);
            }
            prop_perceive_target_for_trackers(unit_index, thrown_index);
        }
        if (unit->unit.player_index != -1 && grenade_type != -1)
        {
            player_datum *player = datum_get<player_datum>(player_data, unit->unit.player_index);
            long remaining = MAX(0, block->duration_ticks - block->throw_ticks);
            player->grenade_recovery_ticks = (signed char)MIN(MAX(remaining, player->grenade_recovery_ticks), 127);
        }
    }
}

// @address 0x380ab0 ?unit_action_throw_grenade_update@@YA_NJW4e_action@@@Z
bool unit_action_throw_grenade_update(long unit_index, e_action action)
{
    bool result = true;
    unit_datum *unit = ((unit_datum *)object_get(unit_index));
    s_weapon_action_block *block = weapon_actions_header_block_get(unit);
    block->throw_ticks++;
    switch (block->grenade_throw_state)
    {
    case 1:
        if (block->throw_ticks >= 2)
            unit_throw_grenade_initiate(unit_index, false);
        break;
    case 3:
        if (!motor_animation_running(unit_index, 34) && !motor_animation_running(unit_index, 112))
        {
            unit_throw_grenade_release(unit_index, true, 0, 0);
            block->grenade_throw_state = 0;
            result = false;
        }
        break;
    case 2:
        if (block->throw_ticks >= block->release_ticks)
            unit_throw_grenade_release(unit_index, false, 0, 0);
        break;
    }
    return result;
}

// @address 0x380bc8 ?unit_action_throw_grenade_interrupt@@YAXJW4e_action@@@Z
void unit_action_throw_grenade_interrupt(long unit_index, e_action action)
{
    unit_datum *unit = ((unit_datum *)object_get(unit_index));
    s_weapon_action_block *block = weapon_actions_header_block_get(unit);
    unit_throw_grenade_release(unit_index, true, 0, 0);
    block->grenade_throw_state = 0;
}

bool unit_action_kamikaze_submit(long unit_index, const action_request *request)
{
    bool result = false;
    unit_datum *unit = (unit_datum *)object_get(unit_index);
    if (unit->unit.actor != -1) {
        char *element = 0;
        if (((s_game_globals *)global_game_globals)->grenades.count())
            element = (char *)((s_game_globals *)global_game_globals)->grenades.address + 68;
        if (element) {
            c_motor_request motor;
            motor.setup_animation(_action_kamikaze, 53, 53, 0.2f, NULL, false);
            result = motor_system_submit(unit_index, &motor) == _motor_result_success;
        }
    }
    return result;
}

static __forceinline bool weapon_actions_actor_allow_weapon_stow(long actor_index)
{
    bool result = true;
    character_general_properties *properties = character_definition_general_properties_get(
        datum_get<actor_datum>(actor_data, actor_index)->character_definition_index);
    if (properties && (properties->flags & (1 << 6)))
        result = false;
    return result;
}

bool unit_action_weapon_stow_immediate(long unit_index, bool immediate)
{
    bool result = false;
    long actor_index = ((unit_datum *)object_get(unit_index))->unit.actor;
    if (actor_index == -1 || weapon_actions_actor_allow_weapon_stow(actor_index))
    {
        if (unit_action_weapon_put_away_submit(unit_index, -1, true, false, false, true))
        {
            ((unit_datum *)object_get(unit_index))->unit.desired_weapon_indices[0] = -1;
            result = true;
            if (immediate)
                unit_animation_set_weapon(unit_index, 135, 0, true);
        }
    }
    return result;
}

void unit_action_kamikaze_event(long unit_index, e_action action, long flags)
{
    if (flags & 1) {
        short activated = 0;
        long actor_index = ((unit_datum *)object_get(unit_index))->unit.actor;
        short layer = actor_executing_layer(actor_index, 18);
        if (layer != -1) {
            actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
            activated = ::equipment_activate_impulse_get_impulse(actor_index, actor_behavior_state_get(actor, layer));
        }
        if ((unsigned char)activated) {
            action_submit_event(unit_index, 46);
            action_submit_event(unit_index, 47);
        }
    }
}

bool action_stow_submit(long unit_index, const action_request *request)
{
    c_motor_request motor;
    motor.setup_weapon_stow(_action_stow);
    return motor_system_submit(unit_index, &motor) != 0;
}

bool action_draw_submit(long unit_index, const action_request *request)
{
    bool result = false;
    unit_datum *unit = (unit_datum *)object_get(unit_index);
    signed char current_weapon_index = unit->unit.current_weapon_indices[0];
    if (current_weapon_index == -1 || unit->unit.weapon_object_indices[current_weapon_index] != 0) {
        c_motor_request motor;
        motor.setup_weapon_draw((e_action)request->action, request->immediate, false);
        result = motor_system_submit(unit_index, &motor) != 0;
    }
    return result;
}

static __forceinline int weapon_actions_invalid_controller(long index)
{
    if (index < 0) return 1;
    if (index >= 4) return 1;
    return 0;
}

static __forceinline bool weapon_actions_unit_is_dual_wielding(long unit_index)
{
    unit_datum *unit = ((unit_datum *)object_get(unit_index));
    bool result = false;
    if (unit->unit.current_weapon_indices[0] != -1 && unit->unit.current_weapon_indices[1] != -1)
        result = true;
    return result;
}

// @address 0x381990 ?unit_action_weapon_drop_submit@@YA_NJPBUaction_request@@@Z
bool unit_action_weapon_drop_submit(long unit_index, const action_request *request)
{
    unit_datum *unit = ((unit_datum *)object_get(unit_index));
    bool primary = request->action == 46;
    long weapon_set_index = !primary;
    long current_inventory_index = unit->unit.current_weapon_indices[weapon_set_index];
    long weapon_index = weapon_actions_inventory_get_weapon(
        unit_index, ((unit_datum *)object_get(unit_index))->unit.current_weapon_indices[weapon_set_index]);
    bool result = false;
    if (weapon_index != -1)
    {
        e_unit_drop_type drop_type;
        bool thrown;
        if (!primary)
        {
            drop_type = _unit_drop_type_drop_dual;
            thrown = false;
        }
        else
        {
            if (weapon_actions_unit_is_dual_wielding(unit_index))
            {
                drop_type = _unit_drop_type_drop_secondary;
                thrown = !request->immediate;
            }
            else
            {
                drop_type = _unit_drop_type_drop;
                thrown = !request->immediate;
            }
        }
        long other_weapon_index = weapon_actions_inventory_get_weapon(
            unit_index, ((unit_datum *)object_get(unit_index))->unit.current_weapon_indices[primary != 0]);
        if (other_weapon_index != -1)
        {
            item_datum *weapon = (item_datum *)object_get(weapon_index);
            item_datum *other_weapon = (item_datum *)object_get(other_weapon_index);
            if (weapon->object.definition_index == other_weapon->object.definition_index)
                weapon_steal_goodies(other_weapon_index, weapon_index);
        }
        if (!request->simulated)
            simulation_request_drop_weapon(unit_index, (short)weapon_set_index);
        unit_action_weapon_put_away_end(unit_index, primary);
        unit_inventory_drop_weapon(unit_index, current_inventory_index, drop_type, thrown);
        unit_action_weapon_ready(unit_index, 226, false, false, primary);
        ::unit_unzoom(unit_index, false);
        result = true;
    }
    return result;
}

// @address 0x381b30 ?unit_action_weapon_pickup_submit@@YA_NJPBUaction_request@@@Z
bool unit_action_weapon_pickup_submit(long unit_index, const action_request *request)
{
    item_datum *weapon = (item_datum *)object_get(request->weapon);
    bool result = false;
    unit_weapon_pickup_result pickup;
    short task;
    if (weapon->unit_object_index == -1 &&
        ::unit_can_pickup_weapon(
            unit_index, request->weapon, (e_weapon_addition_method)request->mode, (unit_weapon_pickup_result *)&pickup) &&
        ::unit_test_pickup_with_mode(
            unit_index, (unit_weapon_pickup_result const *)&pickup, (e_weapon_addition_method)request->mode, (short *)(&task)))
    {
        unsigned short task_index = task;
        if (motor_task_allow(unit_index, _action_weapon_pickup, task_index)) {
            motor_task_interrupt(unit_index, task_index, 0);
            if (::unit_add_weapon_to_inventory(unit_index, request->weapon, (e_weapon_addition_method)request->mode)) {
                ::unit_unzoom(unit_index, false);
                player_iterator iterator = { ((unit_datum *)object_get(unit_index))->unit.player_index, NONE };
                while (player_iterate_output_users(&iterator)) {
                    if (tls_player_mapping_globals()->local_player_count < 2 && !weapon_actions_invalid_controller(iterator.controller_index))
                        ::chud_messaging_post(iterator.controller_index,
                            ((weapon_definition *)tag_get('weap', weapon->object.definition_index))->item.picked_up_message,
                            (e_chud_message_context)false);
                }
                result = true;
            }
        }
    }
    return result;
}

static __forceinline long weapon_actions_desired_ready_weapon_get(long unit_index, long inventory_index)
{
    unit_datum *unit = ((unit_datum *)object_get(unit_index));
    long result = -1;
    if (inventory_index != -1)
    {
        for (long hand = 0; hand < 2; ++hand)
        {
            if (unit->unit.current_weapon_indices[hand] == inventory_index)
            {
                return -1;
            }
        }
        result = unit->unit.weapon_object_indices[inventory_index];
    }
    return result;
}

static __forceinline item_datum *weapon_actions_ready_weapon_get(long weapon_index)
{
    return (item_datum *)object_get(weapon_index);
}

static __forceinline void weapon_actions_inventory_stow(long unit_index)
{
    long newest_time = (-2147483647 - 1);
    long stow_weapon = -1;
    unit_datum *unit = ((unit_datum *)object_get(unit_index));
    for (long i = 0; i < NUMBEROF(unit->unit.weapon_object_indices); ++i)
    {
        long weapon_index = unit->unit.weapon_object_indices[i];
        long ready_time = unit->unit.weapon_ages[i];
        if (weapon_index != -1 && weapon_actions_ready_weapon_get(weapon_index)->owner_relation != 1 && newest_time < ready_time)
        {
            newest_time = ready_time;
            stow_weapon = weapon_index;
        }
    }
    if (stow_weapon != -1 && weapon_can_be_stowed(stow_weapon, unit_index) && weapon_actions_ready_weapon_get(stow_weapon)->owner_relation != 3)
    {
        item_show_inventory_unattached(stow_weapon, unit_index);
        weapon_stow(stow_weapon, unit_index);
    }
    for (long i = 0; i < NUMBEROF(unit->unit.weapon_object_indices); ++i)
    {
        long weapon_index = unit->unit.weapon_object_indices[i];
        if (weapon_index != -1 && weapon_index != stow_weapon && weapon_actions_ready_weapon_get(weapon_index)->owner_relation == 3)
            item_hide_inventory(weapon_index);
    }
}

static __forceinline void weapon_actions_aim_assist_clear(s_aim_assist_targeting_result *result)
{
    memset(result, 0, sizeof(*result));
    result->target_object_index = -1;
    result->model_target = -1;
    result->target_player_index = -1;
}

// @address 0xee150 ?unit_action_weapon_ready@@YAXJJ_N00@Z
void unit_action_weapon_ready(long unit_index, long animation, bool immediate, bool force, bool primary)
{
    unit_datum *unit = ((unit_datum *)object_get(unit_index));
    s_weapon_action_block *block = weapon_actions_header_block_get(unit);
    if (!((unsigned long)block->flags & 16))
    {
        bool result = false;
        long weapon_set_index = !primary;
        long other_weapon_set_index = primary != false;
        long dual_wielding = weapon_actions_inventory_get_weapon(unit_index,
                                 ((unit_datum *)object_get(unit_index))->unit.desired_weapon_indices[other_weapon_set_index]) !=
            -1;
        bool is_vehicle = ((1 << unit->object.object_type) & 2) != 0;
        long inventory_index = unit->unit.desired_weapon_indices[weapon_set_index];
        long weapon_index = weapon_actions_desired_ready_weapon_get(unit_index, inventory_index);
        if (weapon_index != -1)
        {
            bool can_ready = true;
            bool uninterrupted = true;
            item_datum *weapon = weapon_actions_ready_weapon_get(weapon_index);
            weapon_definition *definition =
                (weapon_definition *)tag_get('weap', weapon->object.definition_index);
            bool must_ready = weapon_actions_seat_flag_test(definition->flags, 3) || weapon_actions_seat_flag_test(definition->flags, 13);
            bool must_be_readied = must_ready && !is_vehicle && unit->object.parent_object_index == -1;
            long weapon_class = definition->animation_weapon_class;
            if (dual_wielding)
                weapon_class = 486;
            bool valid_animation = unit_animation_test_weapon(unit_index, animation, weapon_class, definition->animation_weapon_type, must_be_readied);
            if (valid_animation || is_vehicle)
            {
                block->flags |= 16;
                unit_animation_set_weapon(unit_index, weapon_class, definition->animation_weapon_type, primary);
                block->flags &= ~16;
                if ((unsigned char)unit->unit.current_weapon_indices[weapon_set_index] != 255)
                {
                    uninterrupted = false;
                    result = true;
                }
            }
            else if ((unsigned short)unit->unit.parent_seat_index == 65535)
                can_ready = false;
            if (can_ready && uninterrupted)
            {
                unit_show_weapon_from_inventory(weapon_index, unit_index, (e_unit_hand)weapon_set_index);
                unit->unit.current_weapon_indices[weapon_set_index] = (signed char)inventory_index;
                unit->unit.weapon_ages[inventory_index] = TLS_GAME_TIME_GLOBALS->time;
                bool overheated = unit_weapon_in_overheated_loop(unit_index, weapon_set_index);
                weapon_ready(weapon_index, force, immediate);
                if (!force)
                {
                    e_unit_messages message = _unit_message_ready_primary;
                    if (primary)
                    {
                        message = _unit_message_ready_primary;
                        if (overheated) message = _unit_message_ready_overheated_primary;
                    }
                    else
                    {
                        message = _unit_message_ready_secondary;
                        if (overheated) message = _unit_message_ready_overheated_secondary;
                    }
                    unit_action_send_message_to_first_person(unit_index, message);
                }
                unit_unzoom(unit_index, false);
                weapon_actions_aim_assist_clear(&unit->unit.targeting);
                if (!immediate && (!dual_wielding || !primary))
                {
                    bool submitted = false;
                    if (overheated)
                    {
                        c_motor_request request;
                        request.setup_weapon_animation(_action_none, primary ? 114 : 466, primary ? 0 : 3, k_standard_interpolation_time);
                        submitted = motor_system_submit(unit_index, &request) == _motor_result_success;
                    }
                    if (!submitted)
                    {
                        c_motor_request request;
                        request.setup_weapon_animation(_action_none, primary ? 38 : 465, primary ? 0 : 3, k_standard_interpolation_time);
                        motor_system_submit(unit_index, &request);
                    }
                }
                result = true;
            }
        }
        weapon_actions_inventory_stow(unit_index);
        if (!result)
            unit_action_weapon_set_unarmed_animation(unit_index, primary);
    }
}

struct effect_vector_field;

static __forceinline bool weapon_actions_weapon_prevents_grenade_throwing(long weapon_index)
{
    bool result = false;
    if (weapon_index != -1) {
        weapon_datum *weapon = (weapon_datum *)object_get(weapon_index);
        weapon_definition *definition = (weapon_definition *)tag_get('weap', weapon->definition_index);
        const bool &flags_prevent = definition->m_flags.test(_weapon_definition_prevents_grenade_throwing);
        long weapon_type = weapon->weapon_state;
        bool in_range = weapon_type >= 5 && weapon_type <= 10;
        const bool &range_result = in_range;
        result = range_result | flags_prevent;
    }
    return result;
}

static __forceinline bool weapon_actions_grenade_acceleration_prevent_action(long unit_index)
{
    return (((unit_datum *)object_get(unit_index))->unit.unit_flags_bytes[BIG_ENDIAN_VIEW_INDEX(0, 4)] & 1) != 0;
}

static __forceinline short weapon_actions_grenade_count_get(long unit_index, short grenade_type)
{
    unit_datum *unit = ((unit_datum *)object_get(unit_index));
    short result;
    if (grenade_type != -1)
        result = unit->unit.grenade_counts[grenade_type];
    else
        result = 0;
    return result;
}

static __forceinline long weapon_actions_grenade_inventory_get_weapon(const unit_datum *unit, short slot)
{
    long result = -1;
    if (slot != -1)
        result = unit->unit.weapon_object_indices[slot];
    return result;
}

static __forceinline long weapon_actions_grenade_inventory_get_weapon(long unit_index, short slot)
{
    const unit_datum *unit = ((unit_datum *)object_get(unit_index));
    return weapon_actions_grenade_inventory_get_weapon(unit, slot);
}

static __forceinline s_grenade_definition *weapon_actions_grenade_info_get(long grenade_type)
{
    const s_tag_block &block = ((const s_game_globals *)global_game_globals)->grenades;
    return block.count ? &((s_grenade_definition *)block.address)[grenade_type] : 0;
}

// @address 0x380470 ?unit_action_throw_grenade_submit@@YA_NJPBUaction_request@@@Z
bool unit_action_throw_grenade_submit(long unit_index, const action_request *request)
{
    s_grenade_definition *grenades = 0;
    unit_datum *unit = ((unit_datum *)object_get(unit_index));
    s_weapon_action_block *block = weapon_actions_header_block_get(unit);
    long weapon_index = weapon_actions_grenade_inventory_get_weapon(
        unit_index, ((unit_datum *)object_get(unit_index))->unit.current_weapon_indices[0]);
    bool control = false;
    bool result = false;
    bool throwing = false;
    long grenade_type = unit->unit.current_grenade_index;
    if (grenade_type != -1)
    {
        grenades = weapon_actions_grenade_info_get(grenade_type);
    }
    if (request->immediate == 0)
    {
        long player_index = unit->unit.player_index;
        if (player_index != -1 && datum_get<player_datum>(player_data, player_index)->grenade_recovery_ticks > 0)
            control = true;
    }
    if (unit->object.object_type == 0)
    {
        if (!weapon_actions_weapon_prevents_grenade_throwing(weapon_index) &&
            (!weapon_actions_grenade_acceleration_prevent_action(unit_index) && !control || request->simulated))
            throwing = true;
    }
    if (throwing)
    {
        long parent_index = unit->object.parent_object_index;
        if (parent_index != -1 && unit->unit.parent_seat_index != -1)
        {
            unit_datum *parent = ((unit_datum *)object_get(parent_index));
            unit_definition *definition = (unit_definition *)tag_get('unit', parent->object.definition_index);
            unit_seat *seat = (unit_seat *)tag_block_get_element_with_size(
                &definition->seats, unit->unit.parent_seat_index, sizeof(unit_seat));
            object_marker marker;
            unsigned long seat_flags = seat->flags;
            if (!weapon_actions_seat_flag_test(seat_flags, 11))
            {
                if (!weapon_actions_seat_flag_test(seat_flags, 5))
                    throwing = false;
            }
            if (weapon_actions_seat_flag_test(seat_flags, 11) &&
                (seat->boarding_grenade_marker == 0 ||
                    internal_object_get_markers_by_string_id(parent_index, seat->boarding_grenade_marker, &marker, 1, false) != 1))
            {
                throwing = false;
            }
        }
    }
    if (throwing && action_executing(unit_index, _action_equipment))
        throwing = false;
    if (throwing && grenades)
    {
        short grenade_count = weapon_actions_grenade_count_get(unit_index, ((unit_datum *)object_get(unit_index))->unit.current_grenade_index);
        if (grenade_count > 0 || request->force != 0)
        {
            if (request->simulated == 0 && request->immediate != 0 && action_executing(unit_index, _action_grenade))
            {
                c_motor_task *task = motor_task_find_by_action(unit_index, _action_grenade);
                if (task)
                    motor_system_interrupted(unit_index, task, true);
            }
            if (request->simulated != 0)
            {
                action_submit_event(unit_index, 47);
                if (action_executing(unit_index, _action_grenade))
                    unit_throw_grenade_release(unit_index, true, 0, 0);
                block->grenade_throw_state = 1;
                block->throw_ticks = 0;
                block->immediate = 0;
                block->grenade_type = -1;
                unit_throw_grenade_initiate(unit_index, request->force);
                unit_throw_grenade_release(unit_index, false, &request->grenade.position, &request->grenade.direction);
            }
            else if (!action_executing(unit_index, _action_grenade))
            {
                c_motor_request motor;
                action_submit_event(unit_index, 47);
                motor.setup_grenade((e_action)request->action);
                if (motor_system_submit(unit_index, &motor) != _motor_result_failed)
                {
                    long held_weapon = -1;
                    unit_datum *unit_weapons = ((unit_datum *)object_get(unit_index));
                    signed char current_slot = unit_weapons->unit.current_weapon_indices[0];
                    if (current_slot != -1)
                        held_weapon = unit_weapons->unit.weapon_object_indices[current_slot];
                    c_motor_task *clear_task = motor_task_find_by_action(unit_index, _action_melee_player);
                    if (clear_task)
                        motor_system_interrupted(unit_index, clear_task, true);
                    clear_task = motor_task_find_by_action(unit_index, _action_primary_weapon_primary_reload);
                    if (clear_task)
                        motor_system_interrupted(unit_index, clear_task, true);
                    clear_task = motor_task_find_by_action(unit_index, _action_primary_weapon_secondary_reload);
                    if (clear_task)
                        motor_system_interrupted(unit_index, clear_task, true);
                    clear_task = motor_task_find_by_action(unit_index, _action_secondary_weapon_primary_reload);
                    if (clear_task)
                        motor_system_interrupted(unit_index, clear_task, true);
                    clear_task = motor_task_find_by_action(unit_index, _action_secondary_weapon_secondary_reload);
                    if (clear_task)
                        motor_system_interrupted(unit_index, clear_task, true);
                    if (held_weapon != -1)
                        weapon_interrupt_reload(held_weapon, false);
                    c_motor_task *throw_task = motor_task_find_by_action(unit_index, (e_action)request->action);
                    c_animation_channel *channel = motor_animation_task_get_channel(unit_index, throw_task);
                    float duration = channel->get_playback_duration();
                    float key_time = channel->get_authored_primary_key_time();
                    block->grenade_type = -1;
                    block->grenade_throw_state = 1;
                    block->throw_ticks = 0;
                    block->immediate = request->immediate;
                    block->duration_ticks = (unsigned char)(long)((float)(double)TLS_GAME_TIME_GLOBALS->ticks_per_second * duration + 0.5f);
                    if (key_time >= 0.0f)
                        block->release_ticks = (unsigned char)(long)((float)(double)TLS_GAME_TIME_GLOBALS->ticks_per_second * key_time + 0.5f);
                    else
                        block->release_ticks = 0;
                    if (unit->object.parent_object_index == -1)
                    {
                        bool overheated = unit_weapon_in_overheated_loop(unit_index, 0);
                        unit_unzoom(unit_index, false);
                        unit_action_send_message_to_first_person(unit_index, (e_unit_messages)((overheated != 0) + 26));
                    }
                    if ((long)grenades->throwing_effect.index != -1)
                    {
                        long effect_storage;
                        c_tag_index *effect = new (&effect_storage) c_tag_index(grenades->throwing_effect.index);
                        effect_new_from_object(*effect, 0, 0, unit_index, 0.0f, 0.0f, 0, 0, 1);
                    }
                    simulation_action_unit_grenade_initiate(unit_index, unit->unit.current_grenade_index);
                    result = true;
                }
            }
        }
    }
    return result;
}

// @address 0x381cb8 ?unit_action_weapon_trade_submit@@YA_NJPBUaction_request@@@Z
bool unit_action_weapon_trade_submit(long unit_index, const action_request *request)
{
    bool result = false;
    unit_datum *unit = ((unit_datum *)object_get(unit_index));
    if (unit->unit.player_index == NONE)
    {
        bool trade = false;
        bool facing_checked = false;
        player_datum *player = datum_get<player_datum>(player_data, request->unit_index);
        long player_unit_index = player->unit_index;
        long program = mover_get_motor_program(unit_index);
        if (!motor_animation_busy(unit_index) || action_executing(unit_index, _action_ai_impulse))
        {
            switch (program)
            {
            case 0:
            case 1:
            {
                real_vector3d forward;
                real_point3d player_head_position;
                real_point3d unit_head_position;
                object_get_orientation(unit_index, &forward, NULL);
                unit_get_head_position(player->unit_index, &player_head_position);
                unit_get_head_position(unit_index, &unit_head_position);
                real_vector3d direction;
                vector_from_points3d(&player_head_position, &unit_head_position, &direction);
                trade = dot_product3d(&forward, &direction) < 0.0f;
                facing_checked = true;
                break;
            }
            case 3:
            case 6:
            case 9:
                trade = true;
                break;
            }
        }
        if (trade)
        {
            long weapon_index = weapon_actions_inventory_get_weapon(
                unit_index, ((unit_datum *)object_get(unit_index))->unit.current_weapon_indices[0]);
            long player_weapon_index = weapon_actions_inventory_get_weapon(
                player_unit_index, ((unit_datum *)object_get(player_unit_index))->unit.current_weapon_indices[0]);
            if (weapon_index != NONE)
            {
                action_request drop_request;
                memset(&drop_request, 0, sizeof(drop_request));
                drop_request.action = 46;
                drop_request.immediate = true;
                unit_action_weapon_drop_submit(unit_index, &drop_request);
                result = ::unit_add_weapon_to_inventory(player_unit_index, weapon_index, (::e_weapon_addition_method)4);
                if (result)
                {
                    bool traded = false;
                    if (player_weapon_index != NONE)
                    {
                        if (weapon_actions_ready_weapon_get(player_weapon_index)->owner_relation != 0)
                            unit_force_drop_inventory_weapon(player_unit_index, player_weapon_index, false, 0);
                        traded = ::unit_add_weapon_to_inventory(unit_index, player_weapon_index, (::e_weapon_addition_method)3);
                    }
                    if (unit->unit.actor != NONE)
                    {
                        if (traded)
                            ai_handle_weapon_trade(unit->unit.actor, request->unit_index, weapon_index, player_weapon_index);
                        else
                            ai_handle_weapon_trade(unit->unit.actor, request->unit_index, weapon_index, NONE);
                    }
                    if (action_executing(unit_index, _action_ai_impulse))
                    {
                        c_motor_task *task = motor_task_find_by_action(unit_index, _action_ai_impulse);
                        if (task)
                            motor_system_interrupted(unit_index, task, true);
                    }
                }
                else
                {
                    ::unit_add_weapon_to_inventory(unit_index, weapon_index, (::e_weapon_addition_method)3);
                }
            }
        }
        else if (unit->unit.actor != NONE && facing_checked)
        {
            if (ai_globals->enabled)
                actor_handle_weapon_trade_refusal(unit->unit.actor, player->unit_index);
            result = true;
        }
    }
    return result;
}
