// objectslibxbox/unit_animation.obj: from the records 0xbf570 0xc0c60 0xda688 0xdbdc8 0x109a48 0x10e5f0 0x11fc98 0x121af8 0x122138 0x3483b0 0x348518 0x360c70 0x3615a0 0x361850 0x3619d0 0x361b20 0x361b98 0x361c88 0x361d58 0x361e60 0x361f60 0x362490 0x3625f8 0x362740 0x362810 0x362988 0x362a08 0x362a88 0x362b50
// @flags /TP /O2 /Oc
// @address 0x7c888c g_short_fixed_8_scale_initialized
// @address 0x7c8890 g_short_fixed_8_scale
#include "cseries/cseries.h"
#include "tag_files/tag_groups.h"
#include "tag_files/string_ids.h"
#include "animations/animation_manager_mixing_board_inline.h"
#include "units/motor_task.h"
#include "math/short_fixed_storage.h"
struct s_unit_animation_motor_tasks { c_motor_task tasks[4]; };
#include "animations/mixing_board/mixing_board_slider.h"
#include "cseries/cseries_wrapped.h"
#include "models/render_model_definitions.h"
#include "animations/animation_id.h"
#include "objects/object_reference_types.h"
#include "units/unit_seat.h"
#include "motor/motor_animation.h"
#include "models/models.h"
#include "models/model_types.h"
#include "units/unit_definitions.h"
#include "ai/weapon_datum_types.h"
#include "items/weapon_definitions.h"
#include "motor/motor_system.h"
#include "motor/mover.h"
#include "objects/object_types.h"
#include "objects/objects.h"
#include "units/bipeds.h"
#include "units/unit_animation.h"
#include "units/units.h"
#include "units/giants.h"
#include "objects/object_datum.h"
#include "objects/object_header.h"
#include "game/player_mapping_types.h"
#include "camera/observer_globals_types.h"
#include "animations/animation_search_path_iterator.h"
#include "animations/mixing_board/channels/facial_animation_channel_types.h"
#include "units/unit_types.h"
#include "animations/animation_manager.h"
#include "animations/mixing_board/mixing_board.h"
#include "objects/object_placement.h"
#include "units/unit_animation_key_frame_status_types.h"
#include "units/slider_movement_pattern_types.h"
#include "motor/mixing_board_component_types.h"
#include "cache/cache_files.h"
#include "units/slider_target_position_types.h"
#include "animations/mixing_board/mixing_board_definition.h"
#include "animations/mixing_board/mixing_board_slider_types.h"
#include "animations/mixing_board/channels/auxilary_channel.h"
#include "animations/mixing_board/channels/aiming_channel.h"
#include "animations/animation_playback_flags.h"
#include "math/matrix_math_functions.h"
#include "math/real_math_functions.h"
#include "math/real_math_globals.h"
#include "animations/animation_definitions.h"
#include "animations/mixing_board/channels/animation_channel.h"
// @address 0x361f60 ?unit_animation_set_weapon@@YA_NJJJ_N@Z
// @address 0x122200 ?setup_animation_channel_by_parallel_impulse@c_animation_manager@@QBA_NPAVc_animation_channel@@JJJK@Z
// @address 0xc1a90 ?euler_angles2d_from_vector3d@@YAPATreal_euler_angles2d@@PAT1@PBTreal_vector3d@@@Z
// @address 0x9fb10 ?matrix4x3_inverse_transform_point@@YAPATreal_point3d@@PBUreal_matrix4x3@@PBT1@PAT1@@Z
// @address 0x116558 ?matrix4x3_rotation_from_angles@@YAXPAUreal_matrix4x3@@MMM@Z
// @address 0xf07d0 ?matrix4x3_rotation_from_vectors@@YAXPAUreal_matrix4x3@@PBTreal_vector3d@@1@Z
// @address 0x263870 ?model_find_node@@YAJJJ@Z
// @address 0x355a60 ?motor_task_interrupt@@YAXJFF@Z
// @address 0x9b308 ?normalize3d@@YAMPATreal_vector3d@@@Z
// @address 0xac748 ?object_try_and_get_and_verify_type@@YAPAXJK@Z
// @address 0x2fd128 ?reset_mixing_board@@YAXPAVc_animation_manager@@JPBVc_mixing_board_definition@@@Z
// @address 0x11a238 ?state_channel_finished@c_animation_manager@@QBA_NXZ
// @address 0xc0cc8 ?get_slider_value_from_chain@c_animation_manager@@QBA?AV?$t_short_fixed@$0N@@@JW4e_mixing_board_parent_slider_type@@@Z
// @address 0xbf570 ?unit_animation_get_action_name@@YAJJ@Z
// @address 0x79d458 ?g_mixing_board_definition_list@@3PAVc_mixing_board_definition@@A
// @address 0xdc770 ?valid@c_animation_manager@@QBA_NXZ
// @address 0xc0c60 ?unit_animation_weapon_raised@@YA_NJ@Z
// @address 0xdbdc8 ?unit_animation_allows_playback_adjustment@@YA_NJW4e_animation_playback_flags@@@Z
// @address 0x109a48 ?unit_animation_preprocess_root_node_matrix@@YAXJPAUreal_matrix4x3@@@Z
// @address 0x10e5f0 unit_animation_allows_weapon_ik
// @address 0x11fc98 ?unit_set_emotional_state@@YAXJJMM@Z
// @address 0x3615a0 ?unit_animation_get_desired_seat@@YAJJ_N0@Z
// @address 0x361850 ?unit_animation_reset@@YAXJ@Z
// @address 0x361b20 ?unit_animation_get_key_frame_status@@YA?AW4e_unit_animation_key_frame_status@@J_N@Z
// @address 0x361e60 ?unit_animation_test_weapon@@YA_NJJJJ_N@Z
// @address 0x362490 ?aim_eye_matrix_at_world_point@@YAXPBTreal_point3d@@PAUreal_matrix4x3@@1@Z
// @address 0x11db050 ?maximum_pitch@?1??aim_eye_matrix_at_world_point@@YAXPBTreal_point3d@@PAUreal_matrix4x3@@1@Z@4MB
// @address 0x11db054 ?maximum_yaw@?1??aim_eye_matrix_at_world_point@@YAXPBTreal_point3d@@PAUreal_matrix4x3@@1@Z@4MB
// @address 0x11db058 ?$S1@?1??aim_eye_matrix_at_world_point@@YAXPBTreal_point3d@@PAUreal_matrix4x3@@1@Z@4IA
// @address 0x3625f8 ?unit_animation_apply_eye_tracking@@YAXJPBV?$c_static_flags@$0PP@@@JPAUreal_matrix4x3@@@Z
// @address 0x362740 ?unit_animation_compute_interpolation_time@@YAM_N0JJM@Z
// @address 0x362810 ?unit_animation_reset_interpolators@@YAXJ@Z
// @address 0x362988 ?unit_animation_raise_weapon@@YAXJM@Z
// @address 0x362a08 ?unit_animation_lower_weapon@@YAXJM@Z
// @address 0x362a88 ?unit_animation_allows_gunner_control@@YA_NJ@Z
// @address 0x362b50 ?unit_animation_setup_mixing_board@@YAXPAVc_animation_manager@@@Z
// @address 0x348518 ?unit_animation_setup_mixing_board_channels@@YAXPAVc_animation_manager@@@Z
// @address 0x122138 ?unit_animation_locate_constant_overlay@@YAXJ@Z
// @address 0x361c88 ?unit_animation_set_animation_time@@YA_NJM@Z
// @address 0x361d58 ?unit_animation_build_root_matrix_by_seat_and_state@@YA_NJJJ_NJPAUreal_matrix4x3@@@Z
#include "cseries/crt_xdk.h"
#include <stdlib.h>
#include <string.h>
#include "math/real_math_constants.h"
#include "multithreading/thread_tls.h"

#include "memory/data_types.h"
#include "memory/tls_data_arrays.h"

#include "tag_files/tag_types.h"

#include "units/unit_animation_types.h"

enum { k_animation_looping_playback_default_flags = 31 };

// @address 0x8c443e enable_player_transitions
bool enable_player_transitions;


// c_channel_base::get_animation (header: prototype only)
static __forceinline const c_model_animation *unit_animation_channel_get_animation(const c_channel_base *channel)
{
    const c_model_animation *result = 0;
    if (channel->m_animation_id.valid())
        result = ((const c_model_animation_graph *)tag_get(0, channel->m_graph_index))
            ->get_animation(channel->m_animation_id);
    return result;
}
// @address 0x79d7dc k_mixing_board_slider_targets

// Converts seconds to the facial channel's fixed-point time.
static __forceinline short real_to_short_fixed(float value) { return (short)(value * 256.0f); }
static __forceinline void unit_animation_channel_set_target_emotion(c_facial_animation_channel *channel, long emotion, float value, float rate)
{
    if (emotion >= 0 && emotion < 9) {
        float scaled = value * 127.0f;
        scaled = MIN(MAX(scaled, -127.0f), 127.0f);
        channel->m_emotions[emotion].m_target.m_value = (signed char)(long)scaled;
        channel->m_emotions[emotion].m_rate.m_value = real_to_short_fixed(rate);
    }
}

// c_animation_manager methods the header has only as prototypes (or not at all): the unit's inline bodies.
// c_animation_manager::set_target_emotion
static __forceinline void unit_animation_set_target_emotion(c_animation_manager *object, long emotion, float value, float rate)
{
    c_facial_animation_channel *channel = object->get_mixing_board_facial_animation_channel();
    if (channel != 0)
        unit_animation_channel_set_target_emotion(channel, emotion, value, rate);
}
// Resets the object interpolators for its mixing-board definition.
static __forceinline void unit_animation_manager_reset_interpolators(const c_animation_manager *object)
{
    const c_mixing_board_definition *definition = mixing_board_definition_get_by_type(object->m_mixing_board_type);
    if (definition != 0 && definition->reset_interpolators != 0)
        definition->reset_interpolators(object->m_object_index);
}
// c_animation_manager::get_state_animation
static __forceinline const c_model_animation *unit_animation_get_state_animation(const c_animation_manager *object)
{
    return unit_animation_channel_get_animation(&object->m_state_channel);
}
// Links a component to its parent slider.
static __forceinline void unit_animation_link_component_to_slider(const c_animation_manager *object, long type, long component_index,
    long parent_slider_index, e_mixing_board_parent_slider_type parent_slider_type)
{
    const c_mixing_board_definition *definition = mixing_board_definition_get_by_type(object->m_mixing_board_type);
    long offset = definition->component_types[type].offset;
    long size = g_mixing_board_component_sizes[type];
    if (type == 0) {
        c_mixing_board_slider *slider = object->get_mixing_board_slider(component_index);
        slider->m_parent_slider_type = parent_slider_type;
        slider->m_parent_slider_index = (signed char)parent_slider_index;
    } else {
        // The mixing board is allocated directly after the manager object.
        const char *mixing_board = (const char *)object + sizeof(c_animation_manager);
        c_channel_base *channel = (c_channel_base *)(mixing_board + offset + component_index * size);
        channel->m_parent_slider_type.m_storage = (unsigned char)parent_slider_type;
        channel->m_parent_slider_index = (signed char)parent_slider_index;
    }
}


long unit_animation_get_action_name(long unit_index)
{
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 8195);
    c_animation_manager *animation = (c_animation_manager *)object_header_block_get(unit, &unit->object.animation);
    c_animation_channel *action = animation->get_mixing_board_animation_channel(0);
    unsigned long result = 0xffffffff;
    bool action_valid = action->m_enabled != 0 && action->m_graph_index != -1 &&
        (unsigned short)action->m_animation_id.m_animation_index < 32768;
    if (action_valid)
        result = action->m_definition_index;
    return result;
}

// 0xc0c60
bool unit_animation_weapon_raised(long unit_index)
{
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 8195);
    c_animation_manager *animation = (c_animation_manager *)object_header_block_get(object_get(unit_index), &unit->object.animation);
    t_short_fixed<13> weapon_slider_value = animation->get_slider_value_from_chain(3, _mixing_board_parent_slider_regular);
    bool weapon_raised = weapon_slider_value == 0;
    return weapon_raised;
}

// 0xdbdc8
bool unit_animation_allows_playback_adjustment(long unit_index, e_animation_playback_flags playback_flag)
{
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 8195);
    c_animation_manager *animation = (c_animation_manager *)object_header_block_get(object_get(unit_index), &unit->object.animation);
    bool result = false;
    const c_model_animation *state_animation = unit_animation_get_state_animation(animation);
    if (state_animation != 0 && (state_animation->animation_flags & (1 << playback_flag)) != 0)
        result = true;
    const c_animation_channel *channel = animation->get_mixing_board_animation_channel(0);
    const c_model_animation *channel_animation = unit_animation_channel_get_animation(channel);
    if (channel_animation != 0 && (channel_animation->animation_flags & (1 << playback_flag)) != 0)
        result = true;
    return result;
}

void unit_animation_preprocess_root_node_matrix(long object_index, real_matrix4x3 *matrix)
{
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(object_index, 8195);
    long root_index = ((s_unit_animation *)object_header_block_get(object_get(object_index), &unit->unit.animation_storage))->root_index;
    if (root_index != -1 && object_try_and_get_and_verify_type(root_index, -1) != 0)
    {
        unit_datum *root_unit = (unit_datum *)object_get_and_verify_type(root_index, 8195);
        real_matrix4x3 *node_matrix = (real_matrix4x3 *)object_header_block_get(object_get(root_index), &root_unit->object.node_matrices);
        matrix4x3_compute_product(node_matrix, matrix, matrix);
    }
}

// 0x10e5f0
extern "C" bool unit_animation_allows_weapon_ik(long unit_index)
{
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 8195);
    c_animation_manager *animation = (c_animation_manager *)object_header_block_get(object_get(unit_index), &unit->object.animation);
    bool result = true;
    long action = animation->get_mixing_board_animation_channel(0)->m_definition_index;
    switch (action)
    {
        case _string_id_invalid: break;
        case _string_id_chamber_1: break;
        case _string_id_chamber_2: break;
        case _string_id_fire_1: break;
        case _string_id_fire_2: break;
        case _string_id_charged_1: break;
        case _string_id_charged_2: break;
        case _string_id_melee: break;
        case _string_id_melee_airborne: break;
        case _string_id_leap_melee: break;
        case _string_id_melee_1sthit: break;
        case _string_id_melee_1sthit2idle: break;
        case _string_id_melee_2ndhit: break;
        case _string_id_melee_2ndhit2idle: break;
        case _string_id_melee_3rdhit: break;
        case _string_id_melee_lunge: case _string_id_melee_lunge_airborne:
        case _string_id_melee_lunge_unaimed: case _string_id_melee_lunge_unaimed_airborne: break;
        case _string_id_melee_back: break;
        case _string_id_melee_left: break;
        case _string_id_melee_right: break;
        case _string_id_melee_dash: break;
        case _string_id_melee_dash_airborne: break;
        case _string_id_melee_strike_1: break;
        case _string_id_melee_strike_2: break;
        case _string_id_melee_strike_3: break;
        case _string_id_melee_strike_4: break;
        case _string_id_melee_off_hand: break;
        case _string_id_melee_flurry: break;
        case _string_id_melee_tackle: break;
        case _string_id_infection_spew: result = false; break;
        case _string_id_pelvis: result = false; break;
        default:
            result = false;
            break;
    }
    if (mover_is_in_entering_state(unit_index))
        result = false;
    else switch (animation->get_state_name())
    {
        case _string_id_resurrect_front: case _string_id_resurrect_back: case _string_id_melee_continuous:
        case _string_id_leap_start: case _string_id_leap_airborne: case _string_id_airborne_dead:
        case _string_id_landing_dead: case _string_id_exit:
        case _string_id_flying_front: case _string_id_flying_back: case _string_id_flying_left:
        case _string_id_flying_right: case _string_id_h_ping: case _string_id_user_animation:
            result = false;
            break;
        case _string_id_exit_and_detach:
            result = true;
            break;
    }
    if (unit_animation_allows_playback_adjustment(unit_index, _animation_playback_disable_weapon_ik_bit))
        result = false;
    if (unit_animation_allows_playback_adjustment(unit_index, _animation_playback_force_weapon_ik_on_bit))
        result = true;
    return result;
}

// 0x11fc98
void unit_set_emotional_state(long unit_index, long emotion, float value, float rate)
{
    if (unit_index != -1) {
        unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 8195);
        c_animation_manager *animation = (c_animation_manager *)object_header_block_get(object_get(unit_index), &unit->object.animation);
        unit_animation_set_target_emotion(animation, emotion, value, rate);
    }
}

// 0x3483b0
// @address 0x3483b0 unit_animation_snap_mixing_sliders
void unit_animation_snap_mixing_sliders(long unit_index)
{
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, ~0UL);
    c_animation_manager *animation = (c_animation_manager *)object_header_block_get(object_get(unit_index), &unit->object.animation);
    animation->get_mixing_board_slider(5)->snap_to_target(_slider_target_position_one);
    animation->get_mixing_board_slider(6)->snap_to_target(_slider_target_position_one);
    animation->get_mixing_board_slider(7)->snap_to_target(_slider_target_position_one);
    unit_animation_reset_interpolators(unit_index);
}

// 0x348518
void unit_animation_setup_mixing_board_channels(c_animation_manager *animation_manager)
{
    unit_animation_setup_mixing_board(animation_manager);
    unit_animation_link_component_to_slider(animation_manager, 2, 4, 5, _mixing_board_parent_slider_regular);
    unit_animation_link_component_to_slider(animation_manager, 2, 6, 5, _mixing_board_parent_slider_regular);
    unit_animation_link_component_to_slider(animation_manager, 3, 4, 7, _mixing_board_parent_slider_regular);
}

long unit_animation_get_desired_seat(long object_index, bool, bool)
{
    long result = _string_id_current;
    switch (((unit_datum *)object_get(object_index))->unit.animation_state) {
    case _string_id_asleep: result = _string_id_asleep; break;
    case _string_id_patrol: result = _string_id_patrol; break;
    case _string_id_combat: result = _string_id_combat; break;
    case _string_id_flee: result = _string_id_flee; break;
    case _string_id_flaming: result = _string_id_flaming; break;
    case _string_id_stunned: result = _string_id_stunned; break;
    case _string_id_climb: result = _string_id_climb; break;
    case _string_id_berserk: result = _string_id_berserk; break;
    }
    return result;
}


static __forceinline short part_find_value(const c_model_animation *part, unsigned short type)
{
    for (long i = 0; i < part->frame_events.count(); i++) {
        const s_frame_event *entry = &part->frame_events[i];
        if (entry->type == type)
            return entry->frame;
    }
    return -1;
}

static __forceinline void unit_animation_stop_animations(long unit_index)
{
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 8195);
    c_animation_manager *animation = (c_animation_manager *)object_header_block_get(unit, &unit->object.animation);
    s_unit_animation *unit_animation = (s_unit_animation *)object_header_block_get(unit, &unit->unit.animation_storage);

    animation->reset_mixing_board();
    unit_animation->saved_weapon_type = -1;
    unit_animation->saved_weapon_class = -1;
    unit_animation->flags.set(_unit_animation_auxilary_animations_bit, false);
}


static __forceinline c_channel_base *unit_animation_get_component(const c_animation_manager *animation, long type, long index)
{
    const c_mixing_board_definition *definition = mixing_board_definition_get_by_type(animation->m_mixing_board_type);
    long size = type == 3 ? g_mixing_board_component_sizes[3] : g_mixing_board_component_sizes[2];
    // The mixing board is allocated directly after the manager object.
    const char *mixing_board = (const char *)animation + sizeof(c_animation_manager);
    return (c_channel_base *)(mixing_board + definition->component_types[type].offset + index * size);
}
static __forceinline bool unit_animation_channel_valid(const c_channel_base *channel)
{
    return channel->m_enabled != 0 && channel->m_graph_index != -1 && channel->m_animation_id.valid();
}

// @address 0x121af8 ?unit_animation_locate_auxilary_animations@@YAXJ@Z
void unit_animation_locate_auxilary_animations(long unit_index)
{
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 8195);
    s_unit_animation *unit_animation = (s_unit_animation *)object_header_block_get(object_get(unit_index), &unit->unit.animation_storage);
    c_animation_manager *animation = (c_animation_manager *)object_header_block_get(object_get(unit_index), &unit->object.animation);
    unit_animation->flags.set(_unit_animation_aiming_euler_bit, false);
    unit_animation->flags.set(_unit_animation_looking_euler_bit, false);
    if (animation != 0)
    {
        long yaw = -1;
        long pitch = -1;
        long state = animation->get_state_name();
        long weapon_class = animation->m_current_goal.weapon_class;
        long weapon_type = animation->m_current_goal.weapon_type;
        switch (state)
        {
                case _string_id_idle: case _string_id_turn_left: case _string_id_turn_right:
                case _string_id_land_soft: case _string_id_land_hard:
                case _string_id_flying_front: case _string_id_flying_back: case _string_id_flying_left:
                case _string_id_flying_right: case _string_id_opening: case _string_id_closing: case _string_id_leap_land:
                    yaw = _string_id_aim_still_up; pitch = _string_id_aim_still_down; break;
                case _string_id_move_front: case _string_id_move_back: case _string_id_move_left: case _string_id_move_right:
                case _string_id_slide_front: case _string_id_slide_back: case _string_id_slide_left: case _string_id_slide_right:
                case _string_id_airborne: case _string_id_evade_left: case _string_id_evade_right:
                case _string_id_stunned_front: case _string_id_stunned_back: case _string_id_stunned_left:
                case _string_id_stunned_right: case _string_id_turn_left_45: case _string_id_turn_right_45: case _string_id_airborne_arc:
                    yaw = _string_id_aim_move_up; pitch = _string_id_aim_move_down; break;
                case _string_id_enter: case _string_id_exit:
                    if (unit->object.parent_object_index == -1) { yaw = _string_id_aim_still_up; pitch = _string_id_aim_still_down; }
                    break;
                case _string_id_airborne_open: case _string_id_airborne_closed:
                    yaw = _string_id_airborne_overlay; break;
        }
        c_channel_base *yaw_channel = unit_animation_get_component(animation, 3, 0);
        animation->setup_aiming_channel((c_aiming_channel *)yaw_channel, weapon_class, weapon_type, yaw, 0);
        c_channel_base *pitch_channel = unit_animation_get_component(animation, 3, 1);
        animation->setup_aiming_channel((c_aiming_channel *)pitch_channel, weapon_class, weapon_type, pitch, 0);
        c_channel_base *turn_left = unit_animation_get_component(animation, 3, 2);
        animation->setup_aiming_channel((c_aiming_channel *)turn_left, weapon_class, weapon_type, 87, 0);
        c_channel_base *turn_right = unit_animation_get_component(animation, 3, 3);
        animation->setup_aiming_channel((c_aiming_channel *)turn_right, weapon_class, weapon_type, 88, 0);
        if (unit_animation_channel_valid(turn_left) || unit_animation_channel_valid(turn_right) ||
            animation_manager_get_auxilary_animation(animation, weapon_class, weapon_type, 87) ||
                animation_manager_get_auxilary_animation(animation, weapon_class, weapon_type, 88))
            unit_animation->flags.set(_unit_animation_looking_euler_bit, true);
        if (unit_animation_channel_valid(yaw_channel) || unit_animation_channel_valid(pitch_channel) ||
            animation_manager_get_auxilary_animation(animation, weapon_class, weapon_type, 41) ||
                animation_manager_get_auxilary_animation(animation, weapon_class, weapon_type, 43))
            unit_animation->flags.set(_unit_animation_aiming_euler_bit, true);
        bool auxiliary_valid = false;
        c_channel_base *channel = unit_animation_get_component(animation, 2, 0);
        animation->setup_auxilary_channel((c_auxilary_channel *)channel, 226, 226, 81, 0x8040);
        if (unit_animation_channel_valid(channel)) auxiliary_valid = true;
        channel = unit_animation_get_component(animation, 2, 1);
        animation->setup_auxilary_channel((c_auxilary_channel *)channel, 226, 226, 82, 0x8040);
        if (unit_animation_channel_valid(channel)) auxiliary_valid = true;
        channel = unit_animation_get_component(animation, 2, 2);
        animation->setup_auxilary_channel((c_auxilary_channel *)channel, 226, 226, 83, 0x8040);
        if (unit_animation_channel_valid(channel)) auxiliary_valid = true;
        unit_animation->flags.set(_unit_animation_auxilary_animations_bit, auxiliary_valid);
        unit_animation_locate_constant_overlay(unit_index);
    }
}

void unit_animation_locate_constant_overlay(long unit_index)
{
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 8195);
    c_animation_manager *animation = (c_animation_manager *)object_header_block_get(object_get(unit_index), &unit->object.animation);
    s_unit_animation *unit_animation = (s_unit_animation *)object_header_block_get(object_get(unit_index), &unit->unit.animation_storage);
    const c_animation_channel *channel = animation->get_mixing_board_animation_channel(2);
    if (animation->setup_animation_channel_by_parallel_impulse((c_animation_channel *)channel, 226, 226, 491, k_animation_looping_playback_default_flags))
        unit_animation->flags.set(_unit_animation_flag_constant_overlay_unknown, false);
}

// 0x361850
void unit_animation_reset(long unit_index)
{
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 8195);
    s_unit_animation *unit_animation = (s_unit_animation *)object_header_block_get(unit, &unit->unit.animation_storage);

    motor_task_interrupt(unit_index, 15, 0);
    unit_animation_stop_animations(unit_index);

    unit_animation->flags.clear();
    unit_animation->root_index = -1;
    memset(unit_animation->aim_bounds, 0, sizeof(unit_animation->aim_bounds));
    memset(unit_animation->look_bounds, 0, sizeof(unit_animation->look_bounds));
    unit_animation->pad4 = 0;

    unit_definition *definition = (unit_definition *)tag_get(0, unit->object.definition_index);
    unit_animation->node_index_1 = (short)model_find_node(definition->model.index, 0x101);
    unit_animation->node_index_2 = (short)model_find_node(definition->model.index, 0x102);

    c_animation_manager *animation = (c_animation_manager *)object_header_block_get(unit, &unit->object.animation);
    if (animation != 0)
    {
        animation->reset_weapon();
        animation->reset_facial_animation_channels();
        unit_animation_manager_reset_interpolators(animation);
    }

    unit_animation->saved_weapon_class = -1;
    unit_animation->saved_weapon_type = -1;
}

// @address 0x360c70 unit_animation_setup_weapon_aux_channels
extern "C" void unit_animation_setup_weapon_aux_channels(long unit_index)
{
    unit_animation_locate_auxilary_animations(unit_index);
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 8195);
    c_animation_manager *animation = (c_animation_manager *)object_header_block_get(object_get(unit_index), &unit->object.animation);
    if (animation != 0 && (unsigned char)giant_is_animation_state_current(unit_index))
    {
        long weapon_class = animation->m_current_goal.weapon_class;
        long weapon_type = animation->m_current_goal.weapon_type;
        animation->setup_aiming_channel((c_aiming_channel *)unit_animation_get_component(animation, 3, 0), weapon_class, weapon_type, 41, 0);
        animation->setup_aiming_channel((c_aiming_channel *)unit_animation_get_component(animation, 3, 1), weapon_class, weapon_type, 43, 0);
    }
}

// @address 0x3619d0 ?unit_animation_get_key_frame_status@@YA?AW4e_unit_animation_key_frame_status@@JPBVc_animation_channel@@_N@Z
e_unit_animation_key_frame_status unit_animation_get_key_frame_status(long unused, const c_animation_channel *model, bool primary)
{
    e_unit_animation_key_frame_status result = _unit_animation_key_frame_status_none;
    bool valid = model->m_enabled && model->m_graph_index != -1 && model->m_animation_id.m_animation_index >= 0;
    if (valid) {
        const c_model_animation *part = 0;
        if (model->m_animation_id.m_animation_index >= 0)
            part = ((const c_model_animation_graph *)tag_get(0, model->m_graph_index))
                ->get_animation(model->m_animation_id);
        short value;
        if (primary)
            value = part_find_value(part, 0);
        else
            value = part_find_value(part, 1);
        if (value == -1)
            result = _unit_animation_key_frame_status_none;
        else if (value > (long)model->m_frame_position)
            result = _unit_animation_key_frame_status_after;
        else
            result = _unit_animation_key_frame_status_before;
    }
    return result;
}

// 0x361b20
e_unit_animation_key_frame_status unit_animation_get_key_frame_status(long unit_index, bool primary)
{
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 8195);
    c_animation_manager *animation = (c_animation_manager *)object_header_block_get(object_get(unit_index), &unit->object.animation);
    e_unit_animation_key_frame_status status = _unit_animation_key_frame_status_none;
    if (animation->valid())
        status = (e_unit_animation_key_frame_status)unit_animation_get_key_frame_status(unit_index, (c_animation_channel *)animation, primary);
    return status;
}


// 0x361c88

// 0x361e60

static __forceinline const render_model_definition *unit_animation_model_get_render_model(long model_index)
{
    const s_model_definition *model =
        (const s_model_definition *)tag_get(0, model_index);
    const render_model_definition *render_model = 0;
    if (model->render_model_index != -1)
        render_model = (const render_model_definition *)tag_get(0, model->render_model_index);
    return render_model;
}

bool unit_animation_build_root_matrix_by_seat_and_state(long unit_index, long seat, long state, bool armed, long flags, real_matrix4x3 *matrix)
{
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 8195);
    c_animation_manager *animation = (c_animation_manager *)object_header_block_get(object_get(unit_index), &unit->object.animation);
    bool result = false;
    c_animation_id animation_id = animation->find_goal_animation_id(seat, armed ? 1 : 226, 1, state, flags);
    if (animation_id.valid())
    {
        const unit_definition *definition =
            (const unit_definition *)tag_get(0, unit->object.definition_index);
        const render_model_definition *render_model = unit_animation_model_get_render_model(definition->model.index);
        // match: 0x361d58: the graph argument is the graph tag index stored at +0x7c.
        animation_definitions_get_animation_root_matrix(
            animation->m_graph_tag_index, animation_id, k_real_zero_storage, render_model, matrix);
        result = true;
    }
    return result;
}

// 0x361e60
bool unit_animation_test_weapon(long unit_index, long label, long weapon_class, long weapon_type, bool must_be_readied)
{
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 8195);
    c_animation_manager *animation = (c_animation_manager *)object_header_block_get(object_get(unit_index), &unit->object.animation);
    bool result = animation->find_goal_animation_id(label, weapon_class, weapon_type, 12, 2).valid();
    if (!result && must_be_readied && label != 138)
    {
        result = animation->find_goal_animation_id(138, weapon_class, weapon_type, 12, 2).valid();
    }
    return result;
}

bool unit_animation_set_animation_time(long unit_index, float time)
{
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 8195);
    c_animation_manager *animation = (c_animation_manager *)object_header_block_get(object_get(unit_index), &unit->object.animation);
    bool result = false;
    if (animation->valid())
    {
        const c_animation_channel *channel = &animation->m_state_channel;
        bool playing = channel->m_enabled != 0 && channel->m_graph_index != -1 && (unsigned short)channel->m_animation_id.m_animation_index < 0x8000;
        if (playing)
            animation->m_state_channel.set_frame_position(time * 30.0f);
        result = true;
    }
    return result;
}
// @address 0x361b98 unit_animation_is_running_damage_overlay
unsigned char unit_animation_is_running_damage_overlay(unsigned int overlay_unit_index)
{
    bool damage_overlay_running = false;
    const c_mixing_board_definition *mixing_board = 0;
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(overlay_unit_index, 8195);
    long unit_animation_offset = unit->unit.animation_storage.offset;
    c_animation_manager *animation = (c_animation_manager *)object_header_block_get(unit, &unit->object.animation);
    unsigned long mixing_board_type = animation->m_mixing_board_type;
    if (mixing_board_type <= 2)
        mixing_board = &g_mixing_board_definition_list[mixing_board_type];
    c_animation_channel *channel =
        (c_animation_channel *)((char *)animation + mixing_board->component_types[1].offset + ((unsigned long)g_mixing_board_component_sizes[1] + 0x58) * 2);
    bool valid = channel->m_enabled != 0 && channel->m_graph_index != -1 &&
        (unsigned short)channel->m_animation_id.m_animation_index < 0x8000;
    if (valid && channel->playing()) {
        unsigned char flag = (((s_unit_animation *)((char *)unit + unit_animation_offset))->flags.get_unsafe() >> 9) & 1;
        if (flag) damage_overlay_running = true;
    }
    return damage_overlay_running;
}
static __forceinline c_motor_task *unit_animation_motor_task_get(long unit_index, long index)
{
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 8195);
    s_unit_animation_motor_tasks *tasks = (s_unit_animation_motor_tasks *)object_header_block_get(unit, &unit->unit.motor_tasks);
    return &tasks->tasks[index];
}
static __forceinline bool unit_animation_update_channel(const c_animation_manager *animation, c_animation_channel *channel, long weapon_class, long weapon_type)
{
    bool result = false;
    if (animation->m_graph_tag_index != -1)
        result = animation->update_animation_channel_on_graph_change(channel, weapon_class, weapon_type);
    return result;
}
static __forceinline bool unit_animation_base_valid(const c_channel_base *channel)
{
    return channel->m_enabled != 0 && channel->m_graph_index != -1 && channel->m_animation_id.valid();
}
static __forceinline unsigned char unit_animation_interpolation_discouraged(unsigned short flags)
{
    unsigned char result = (flags >> 4) & 1;
    return result;
}

// 0x361f60
bool unit_animation_set_weapon(long unit_index, long weapon_class, long weapon_type, bool force)
{
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 8195);
    if ((bool)(_rotl(unit->object.damage_flags, 30) & 1))
        return true;
    c_animation_manager *animation = (c_animation_manager *)object_header_block_get(unit, &unit->object.animation);
    s_unit_animation *unit_animation = (s_unit_animation *)object_header_block_get(unit, &unit->unit.animation_storage);
    long previous_class = animation->m_current_goal.weapon_class;
    long previous_type = animation->m_current_goal.weapon_type;
    bool holster = !force && weapon_type != 1;
    bool restore = !force && weapon_type == 1;
    long desired_type = weapon_type;
    if (holster) {
        weapon_class = 486;
        desired_type = 226;
    } else if (restore) {
        weapon_class = unit_animation->saved_weapon_class;
        desired_type = 226;
    }
    unsigned long playback_flags = 29;
    const c_animation_channel *base = &animation->m_state_channel;
    if (unit_animation_base_valid(base))
        playback_flags = base->m_playback_flags.get_unsafe();
    bool result = animation->set_goal(226, weapon_class, desired_type, 226, 38, playback_flags);
    if (result) {
        bool class_changed = previous_class != weapon_class && weapon_class != 226;
        bool type_changed = previous_type != desired_type && desired_type != 226;
        if (class_changed || type_changed) {
            long state = animation->get_state_name();
            float zero = k_real_zero_storage;
            float interpolation = unit_animation_compute_interpolation_time(true, false, state, state, zero);
            c_animation_channel *first_channel = animation->get_mixing_board_animation_channel(0);
            c_animation_channel *weapon_channel = animation->get_mixing_board_animation_channel(1);
            c_animation_channel *second_channel = animation->get_mixing_board_animation_channel(2);
            if (!unit_animation_update_channel(animation, first_channel, 226, 226)) {
                c_motor_task *task = unit_animation_motor_task_get(unit_index, 2);
                if (task != 0 && task->valid())
                    motor_system_interrupted(unit_index, task, true);
            }
            if (!unit_animation_update_channel(animation, second_channel, 226, 226)) {
                c_motor_task *task = unit_animation_motor_task_get(unit_index, 1);
                if (task != 0 && task->valid())
                    motor_system_interrupted(unit_index, task, true);
            }
            if (!(interpolation <= zero) && !unit_animation_interpolation_discouraged(animation->m_flags))
                object_start_interpolation(unit_index, interpolation);
            if (holster) {
                unit_animation->saved_weapon_class = previous_class;
                unit_animation->saved_weapon_type = weapon_type;
                weapon_channel->reset();
                result = unit_animation_update_channel(animation, weapon_channel, 486, unit_animation->saved_weapon_type);
                if (!result) {
                    c_motor_task *task = unit_animation_motor_task_get(unit_index, 3);
                    if (task != 0 && task->valid())
                        motor_system_interrupted(unit_index, task, true);
                }
            } else if (restore) {
                c_motor_task *task = unit_animation_motor_task_get(unit_index, 3);
                weapon_channel->reset();
                unit_animation->saved_weapon_type = -1;
                unit_animation->saved_weapon_class = -1;
                if (task != 0 && task->valid())
                    motor_system_interrupted(unit_index, task, false);
            }
            const c_mixing_board_definition *definition = mixing_board_definition_get_by_type(animation->m_mixing_board_type);
            if (definition != 0 && definition->locate_auxilary_animations != 0)
                definition->locate_auxilary_animations(animation->m_object_index);
        }
    }
    return result;
}

// 0x362490
void aim_eye_matrix_at_world_point(const real_point3d *focus_point, real_matrix4x3 *eye_matrix, real_matrix4x3 *head_matrix)
{
    static real const maximum_yaw = 30.0f * k_degrees_to_radians;
    static real const maximum_pitch = 15.0f * k_degrees_to_radians;

    float scale = eye_matrix->scale;
    real_matrix4x3 original;
    matrix4x3_rotation_from_vectors(&original, &head_matrix->forward, &head_matrix->up);
    original.position = eye_matrix->position;

    union { real_vector3d vector; real_point3d point; } direction;
    matrix4x3_inverse_transform_point(&original, focus_point, &direction.point);
    normalize3d(&direction.vector);

    real_euler_angles2d angles;
    euler_angles2d_from_vector3d(&angles, &direction.vector);

    angles.yaw = PIN(angles.yaw, -maximum_yaw, maximum_yaw);
    angles.pitch = PIN(angles.pitch, -maximum_pitch, maximum_pitch);

    real_matrix4x3 rotation;
    matrix4x3_rotation_from_angles(&rotation, angles.yaw, angles.pitch, k_real_zero_storage);
    matrix4x3_compute_product(&original, &rotation, eye_matrix);
    eye_matrix->scale = scale;
}

template<long k_bit_count> class c_static_flags;

// 0x3625f8
void unit_animation_apply_eye_tracking(long unit_index, const c_static_flags<255> *static_flags, long count, real_matrix4x3 *matrix)
{
    if (unit_animation_eye_tracking_in_progress(unit_index))
    {
        unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 8195);
        real_point3d *focus_point = 0;
        if (unit->unit.control_flags & 0x8000)
        {
            focus_point = &unit->unit.gaze_position;
        }
        else if ((bool)(_rotl(unit->unit.unit_flags, 9) & 1))
        {
            long user = 0;
            do
            {
                if (tls_player_mapping_globals()->player_indices[user] != -1)
                {
                    if (user != -1)
                        focus_point = &observer_get_globals()->blocks[user].camera_result.position;
                    break;
                }
                user++;
            } while (user < 4);
        }
        object_marker marker;
        if (focus_point != 0 && internal_object_get_markers_by_string_id(unit_index, 157, &marker, 1, false) > 0)
        {
            signed char marker_a = (signed char)unit->unit.left_eye_marker;
            if (marker_a >= 0 && marker_a < count)
                aim_eye_matrix_at_world_point(focus_point, &matrix[marker_a], &marker.matrix);
            signed char marker_b = (signed char)unit->unit.right_eye_marker;
            if (marker_b >= 0 && marker_b < count)
                aim_eye_matrix_at_world_point(focus_point, &matrix[marker_b], &marker.matrix);
        }
    }
}

// 0x362740

float unit_animation_compute_interpolation_time(
    bool unit_is_player, bool force_standard, long current_state, long desired_state, float minimum_interpolation_time)
{
    float interpolation_time = k_standard_interpolation_time;

    if (desired_state == 33 || desired_state == 32 || desired_state == 712)
        interpolation_time = 0.13333334f;

    if (force_standard || unit_is_player)
        interpolation_time = k_standard_interpolation_time;

    if (current_state == 12 && (desired_state == 21 || desired_state == 22 || desired_state == 23 || desired_state == 24))
        interpolation_time = k_standard_interpolation_time;

    if ((current_state == 21 || current_state == 22 || current_state == 23 || current_state == 24) &&
        (desired_state == 21 || desired_state == 22 || desired_state == 23 || desired_state == 24 || desired_state == 12))
        interpolation_time = k_standard_interpolation_time;

    interpolation_time = MAX(interpolation_time, minimum_interpolation_time);
    return interpolation_time;
}


// 0x362810
void unit_animation_reset_interpolators(long unit_index)
{
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 8195);
    c_animation_manager *animation = (c_animation_manager *)object_header_block_get(object_get(unit_index), &unit->object.animation);
    if (animation != 0)
    {
        c_mixing_board_slider *slider;
        slider = animation->get_mixing_board_slider(3);
        slider->snap_to_target(_slider_target_position_one);
        slider = animation->get_mixing_board_slider(0);
        slider->snap_to_target(_slider_target_position_one);
        slider = animation->get_mixing_board_slider(1);
        slider->snap_to_target(_slider_target_position_one);
        slider = animation->get_mixing_board_slider(2);
        slider->snap_to_target(_slider_target_position_one);
    }
}

// 0x362988
void unit_animation_raise_weapon(long unit_index, float duration)
{
    if (unit_index != -1)
    {
        unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 8195);
        c_animation_manager *animation = (c_animation_manager *)object_header_block_get(object_get(unit_index), &unit->object.animation);
        animation->get_mixing_board_slider(3)->move_to_target(_slider_target_position_one, _slider_movement_pattern_light_ease_in, duration);
    }
}

// 0x362a08
void unit_animation_lower_weapon(long unit_index, float duration)
{
    if (unit_index != -1)
    {
        unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 8195);
        c_animation_manager *animation = (c_animation_manager *)object_header_block_get(object_get(unit_index), &unit->object.animation);
        animation->get_mixing_board_slider(3)->move_to_target(_slider_target_position_full, _slider_movement_pattern_full_ease_in_and_out, duration);
    }
}

// 0x362a88
bool unit_animation_allows_gunner_control(long unit_index)
{
    bool result = true;
    if (unit_index != -1)
    {
        unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 8195);
        c_animation_manager *animation = (c_animation_manager *)object_header_block_get(object_get(unit_index), &unit->object.animation);
        long state = animation->get_state_name();
        if (!animation->state_channel_finished())
        {
            if (state == 96 && unit_animation_get_key_frame_status(unit_index, true) != _unit_animation_key_frame_status_after)
                result = false;
            if (state == 97 && unit_animation_get_key_frame_status(unit_index, true) != _unit_animation_key_frame_status_before)
                result = false;
        }
    }
    return result;
}

// 0x362b50
void unit_animation_setup_mixing_board(c_animation_manager *animation_manager)
{
    unit_animation_link_component_to_slider(animation_manager, 1, 2, 2, _mixing_board_parent_slider_regular);
    unit_animation_link_component_to_slider(animation_manager, 3, 0, 3, _mixing_board_parent_slider_inverse);
    unit_animation_link_component_to_slider(animation_manager, 3, 1, 3, _mixing_board_parent_slider_regular);
    unit_animation_link_component_to_slider(animation_manager, 0, 3, 0, _mixing_board_parent_slider_regular);
    unit_animation_link_component_to_slider(animation_manager, 3, 2, 4, _mixing_board_parent_slider_inverse);
    unit_animation_link_component_to_slider(animation_manager, 3, 3, 4, _mixing_board_parent_slider_regular);
    unit_animation_link_component_to_slider(animation_manager, 0, 4, 1, _mixing_board_parent_slider_regular);
}

// @rdata 0x820332b0
static const float unit_animation_fixed_one = 256.0f;

static __forceinline void unit_animation_import_duration(t_short_fixed<8> *duration, float value)
{
    if ((g_short_fixed_8_scale_initialized & 1) == 0) {
        g_short_fixed_8_scale = unit_animation_fixed_one;
        g_short_fixed_8_scale_initialized |= 1;
    }
    duration->m_value = (short)(value * g_short_fixed_8_scale);
}

template<typename T> static __forceinline const T &unit_animation_min(const T &a, const T &b) { return a < b ? a : b; }
static __forceinline void unit_animation_rotate_components(const real_matrix4x3 *matrix, float i, float j, float k, real_vector3d *out)
{
    out->i = i * matrix->forward.i + j * matrix->forward.j + k * matrix->forward.k;
    out->j = i * matrix->left.i + j * matrix->left.j + k * matrix->left.k;
    out->k = i * matrix->up.i + j * matrix->up.j + k * matrix->up.k;
}
static __forceinline void unit_animation_copy_bounds(float *out, const float *bounds)
{
    out[0] = bounds[0];
    out[1] = bounds[1];
    out[2] = bounds[2];
    out[3] = bounds[3];
}

static __forceinline void unit_animation_set_aim(c_channel_base *channel, const real_euler_angles2d &angles) {
    c_aiming_channel *aim = (c_aiming_channel *)channel; aim->set_yaw(angles.yaw); aim->set_pitch(angles.pitch);
}

static __forceinline bool unit_animation_update_channel_valid(const c_channel_base *channel)
{
    const c_channel_base *animation_channel = channel;
    const c_channel_base *mixing_channel = animation_channel;
    bool valid = unit_animation_channel_valid(mixing_channel);
    return valid;
}
static __forceinline c_channel_base *unit_animation_update_get_component(const c_animation_manager *animation, long type, long index)
{
    c_channel_base *channel = unit_animation_get_component(animation, type, index);
    return channel;
}
static __forceinline void *unit_animation_block_get(unit_datum *unit, const object_header_block_reference *reference)
{
    return (char *)unit + reference->offset;
}

static __forceinline long unit_animation_update_weapon_index(long unit_index, short slot)
{
    const unit_datum *unit = (const unit_datum *)object_get_and_verify_type(unit_index, 8195);
    long result = -1;
    if (slot != -1)
        result = unit->unit.weapon_object_indices[slot];
    return result;
}
static __forceinline long unit_animation_update_magazine_state(long weapon_index)
{
    const weapon_datum *weapon = (const weapon_datum *)object_get_and_verify_type(weapon_index, 4);
    const weapon_definition *definition =
        (const weapon_definition *)tag_get(0, weapon->definition_index);
    long result = 0;
    for (long index = 0; index < definition->magazines.s_tag_block::count; ++index)
    {
        result = weapon->magazines[(short)index].state;
        if (result != 0)
            break;
    }
    return result;
}

// @address 0xda688 ?unit_animation_update_mixing_board@@YAXJ@Z
void unit_animation_update_mixing_board(long unit_index)
{
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 8195);
    unit_datum *object = (unit_datum *)unit;
    s_unit_animation *state = (s_unit_animation *)unit_animation_block_get(unit, &unit->unit.animation_storage);
    unit_definition *definition =
        (unit_definition *)tag_get(0, unit->object.definition_index);
    c_animation_manager *manager = (c_animation_manager *)unit_animation_block_get(unit, &unit->object.animation);
    c_animation_manager *manager_state = manager;
    c_animation_channel temporary;
    c_animation_channel *impact = manager->get_mixing_board_animation_channel(2);
    c_mixing_board_slider *impact_slider = manager->get_mixing_board_slider(2);
    if (unit_animation_update_channel_valid(impact)) {
        float time_remaining = impact->get_frames_remaining() * 0.033333335f;
        float maximum_time = 0.1f;
        float time = unit_animation_min(time_remaining, maximum_time);
        t_short_fixed<8> duration;
        unit_animation_import_duration(&duration, time);
        if (impact_slider->get_target() != _slider_target_position_full || impact_slider->time_remaining() > duration) {
            impact_slider->move_to_target(_slider_target_position_full, _slider_movement_pattern_none, duration);
            mixing_board_slider_update(impact_slider);
        }
        unit_seat *seat = unit_get_current_seat(unit_index);
        if (unit_animation_is_running_damage_overlay(unit_index) && seat) impact_slider->snap_to_position(seat->soft_ping_weight);
    } else impact_slider->snap_to_target(_slider_target_position_one);
    state->flags.set(_unit_animation_primary_aiming_screen_bounds_bit, false);
    state->flags.set(_unit_animation_secondary_aiming_screen_bounds_bit, false);
    if (!definition->m_flags.test(_unit_definition_simple_creature_bit) && manager_state->m_graph_tag_index != -1 &&
        manager_state->m_current_goal.state != _string_id_user_animation) {
        if (state->flags.test(_unit_animation_auxilary_animations_bit)) {
            for (long i = 0; i < 3; ++i) {
                c_channel_base *channel = unit_animation_update_get_component(manager, 2, i);
                if (unit_animation_update_channel_valid(channel)) {
                    float ratio = object->unit.seat_acceleration.n[i];
                    ratio *= 0.5f;
                    ratio += 0.5f;
                    ratio = PIN(ratio, 0.0f, 1.0f);
                    const c_model_animation *animation = unit_animation_channel_get_animation(channel);
                    ((c_auxilary_channel *)channel)->set_frame_position(animation->get_frame_position_from_playback_ratio(ratio));
                }
            }
        }
        if (!definition->m_flags.test(_unit_definition_has_no_aiming_bit)) {
            real_euler_angles2d aim_angles = *global_zero_angles2d;
            c_mixing_board_slider *aim_slider = manager->get_mixing_board_slider(0);
            c_mixing_board_slider *look_slider = manager->get_mixing_board_slider(1);
            c_mixing_board_slider *look_blend_slider = manager->get_mixing_board_slider(4);
            real_matrix4x3 matrix; real_orientation orientation;
            object_get_world_matrix(unit_index, &matrix);
            matrix4x3_to_orientation(&matrix, &orientation);
            object_type_preprocess_node_orientations(unit_index, 0, 1, &orientation);
            matrix4x3_from_orientation(&matrix, &orientation);
            matrix.position = *global_origin3d;
            bool suppress = false, transition = false;
            c_channel_base *aim0 = unit_animation_update_get_component(manager, 3, 0);
            c_channel_base *aim1 = unit_animation_update_get_component(manager, 3, 1);

            if (unit_animation_update_channel_valid(aim0) || unit_animation_update_channel_valid(aim1)) {
                real_vector3d input = object->unit.aiming_vector;
                real_vector3d local;
                unit_animation_rotate_components(&matrix, input.i, input.j, input.k, &local);
                euler_angles2d_from_vector3d(&aim_angles, &local);
                state->flags.set(_unit_animation_primary_aiming_screen_bounds_bit, true);
                if (unit_animation_update_channel_valid(aim1))
                {
                    unit_animation_copy_bounds(state->aim_bounds, &((c_aiming_channel *)aim1)->m_yaw_min);
                    unit_animation_set_aim(aim1, aim_angles);
                }
                if (unit_animation_update_channel_valid(aim0))
                {
                    unit_animation_copy_bounds(state->aim_bounds, &((c_aiming_channel *)aim0)->m_yaw_min);
                    unit_animation_set_aim(aim0, aim_angles);
                }
                suppress = !unit_animation_allows_playback_adjustment(unit_index, _animation_playback_disable_weapon_aim_first_person_bit);
                if (!definition->m_flags.test(_unit_definition_allow_aim_while_opening_or_closing_bit) &&
                    (manager_state->m_current_goal.state == _string_id_opening || manager_state->m_current_goal.state == _string_id_closing) &&
                    !manager->state_channel_paused())
                    suppress = false;
                c_animation_channel *primary = manager->get_mixing_board_animation_channel(0);
                if (unit_animation_update_channel_valid(primary)) {
                    long weapon_class = manager_state->m_current_goal.weapon_class;
                    if (weapon_class != _string_id_dual)
                    {
                        long weapon_index = unit_animation_update_weapon_index(unit_index,
                            ((unit_datum *)object_get_and_verify_type(unit_index, 8195))->unit.current_weapon_index);
                        if (weapon_index != -1)
                        {
                            switch (unit_animation_update_magazine_state(weapon_index))
                            {
                            case 1:
                                if (primary->get_playback_time_remaining() > k_standard_interpolation_time)
                                { suppress = false; transition = true; }
                                break;
                            case 2:
                            case 3:
                                suppress = false;
                                transition = true;
                                break;
                            }
                        }
                    }
                    long impulse = primary->m_definition_index;
                    switch (impulse)
                    {
                    case _string_id_reload_1:
                        if (weapon_class != _string_id_dual && primary->get_playback_time_remaining() > k_standard_interpolation_time)
                        {
                            suppress = false;
                            transition = true;
                        }
                        break;
                    case _string_id_melee: case _string_id_melee_1sthit: case _string_id_melee_2ndhit: case _string_id_melee_3rdhit:
                    case _string_id_melee_lunge: case _string_id_melee_lunge_airborne:
                    case _string_id_melee_strike_1: case _string_id_melee_strike_2: case _string_id_melee_strike_3: case _string_id_melee_strike_4:
                        if (primary->get_playback_time_remaining() > k_standard_interpolation_time) { suppress = false; transition = true; }
                        break;
                    }
                }
            }
            if (suppress)
                aim_slider->move_to_target(_slider_target_position_full, _slider_movement_pattern_full_ease_in_and_out, k_standard_interpolation_time);
            else if (transition && aim_slider->get_target() != _slider_target_position_one)
                aim_slider->move_to_target(_slider_target_position_half, _slider_movement_pattern_full_ease_in_and_out, 0.13333334f);
            else
                aim_slider->move_to_target(_slider_target_position_one, _slider_movement_pattern_full_ease_in_and_out, 0.13333334f);
            suppress = false;
            c_channel_base *look0 = unit_animation_update_get_component(manager, 3, 2);
            c_channel_base *look1 = unit_animation_update_get_component(manager, 3, 3);

            if (unit_animation_update_channel_valid(look0)) {
                float k = object->unit.looking_vector.k;
                float i = object->unit.looking_vector.i;
                float margin = 0.78539819f;
                float minimum_weight = 0.0f;
                float maximum_weight = 1.0f;
                float unused_scale = 1.0f;
                real_vector3d local;
                real_euler_angles2d look_angles;
                float j = object->unit.looking_vector.j;
                unit_animation_rotate_components(&matrix, i, j, k, &local);
                euler_angles2d_from_vector3d(&look_angles, &local);
                if ((object->object.object_type == 0 && unit_animation_update_channel_valid(aim0)) || unit_animation_update_channel_valid(aim1)) {
                    look_angles.yaw = PIN(look_angles.yaw, state->aim_bounds[0]-margin, state->aim_bounds[1]+margin);
                    look_angles.pitch = PIN(look_angles.pitch, state->aim_bounds[2]-margin, state->aim_bounds[3]+margin);
                }
                float weight = PIN(aim_slider->get_value().export_real(), minimum_weight, maximum_weight);
                look_angles.yaw -= aim_angles.yaw * weight; look_angles.pitch -= aim_angles.pitch * weight;
                if (unit_animation_update_channel_valid(look0)) unit_animation_set_aim(look0, look_angles);
                if (unit_animation_update_channel_valid(look1)) unit_animation_set_aim(look1, look_angles);
                state->flags.set(_unit_animation_secondary_aiming_screen_bounds_bit, true);
                if (!unit_animation_update_channel_valid(look1)) {
                    look_blend_slider->snap_to_target(_slider_target_position_one);
                    unit_animation_copy_bounds(state->look_bounds, &((c_aiming_channel *)look0)->m_yaw_min);
                } else {
                    t_short_fixed<13> blend_value = manager->get_mixing_board_slider(3)->get_value();
                    float blend = blend_value.export_real();
                    const c_aiming_channel *look_a = (const c_aiming_channel *)look0;
                    const c_aiming_channel *look_b = (const c_aiming_channel *)look1;
                    float inverse = 1.0f - blend;
                    float a[4], b[4];
                    a[0] = look_a->m_yaw_min;
                    a[1] = look_a->m_yaw_max;
                    a[2] = look_a->m_pitch_min;
                    a[3] = look_a->m_pitch_max;
                    b[0] = look_b->m_yaw_min;
                    b[1] = look_b->m_yaw_max;
                    b[2] = look_b->m_pitch_min;
                    b[3] = look_b->m_pitch_max;
                    state->look_bounds[0] = a[0] * inverse + b[0] * blend;
                    state->look_bounds[1] = a[1] * inverse + b[1] * blend;
                    state->look_bounds[2] = a[2] * inverse + b[2] * blend;
                    state->look_bounds[3] = a[3] * inverse + b[3] * blend;
                    look_blend_slider->freeze(); look_blend_slider->m_position = blend_value;
                }
                suppress = !unit_animation_allows_playback_adjustment(unit_index, _animation_playback_disable_look_screen_bit);
            }
            if (suppress)
                look_slider->move_to_target(_slider_target_position_full, _slider_movement_pattern_light_ease_in_and_out, k_standard_interpolation_time);
            else
                look_slider->move_to_target(_slider_target_position_one, _slider_movement_pattern_light_ease_in_and_out, 0.13333334f);
        }
    }
    temporary.initialize();
}
