// objectslibxbox/motor_animation.obj: from the records 0xb38f0 0xc15d0 0xd76b8 0xd8188 0xdc7d8 0xdc840 0xdcd10 0xdd038 0xdd0d0 0xdd230 0xdd308 0xdd3a8 0xdd5b0 0xdd7a8 0xe8418 0xe8bf0 0xe8fe8 0x102240 0x354ee0 0x354fd8 0x3550d8 0x355460 0x355538 0x3556b0 0x3557a8 0x3558d0 0x3559c8 0x355a60 0x355b38 0x355e10 0x355ec8 0x355f60 0x356178 0x3561f8 0x356528 0x356738 0x356830 0x356e80 0x356fb8 0x357110 0x357610 0x357718 0x3577d8 0x357bb0 0x357ca0 0x357e48 0x357fc0 0x358270
// @address 0x22dbc literal_0_1f
#include "math/real_constant_pool.h"
// @flags /TP /O2 /Oc
#include "units/unit_animation_storage_types.h"
#include "animations/animation_search_path_iterator.h"
#include "animations/animation_definition_types.h"
#include "math/real_math_constant_storage.h"
#include "math/real_math_globals.h"
#include "units/unit_seat.h"
#include "units/unit_seat_flags.h"
#include "units/unit_weapon_config_types.h"
#include "objects/object_datum.h"
#include "objects/object_header.h"
#include "game/players.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "units/units.h"
#include "ai/ai_vehicles.h"
#include "animations/animation_definitions.h"
#include "game/game_engine_notifications.h"
#include "items/items.h"
#include "items/weapons.h"
#include "motor/action_system.h"
#include "motor/motor_animation.h"
#include "motor/motor_system.h"
#include "motor/weapon_actions.h"
#include "objects/damage.h"
#include "objects/damage_owner.h"
#include "objects/objects.h"
#include "units/unit_animation.h"
#include "ai/character_definitions.h"
#include "animations/animation_manager.h"
#include "animations/animation_manager_mixing_board_inline.h"
#include "animations/mixing_board/channels/animation_channel.h"
#include "motor/motor_programs.h"
#include "objects/object_early_movers.h"
#include "ai/actor_datum.h"
#include "ai/character_properties_types.h"
#include "objects/object_reference_types.h"
#include "objects/object_types.h"

#include "motor/state_change_flag_bits_types.h"
#include "cache/cache_files.h"
#include "animations/animation_control_data.h"
#include "animations/mixing_board/mixing_board_definition.h"
#include "objects/object_type_definition.h"

class c_model_animation;
#include "math/real_math_types.h"
#include "objects/object_definitions.h"
#include "models/render_model_definitions.h"
#include "models/model_types.h"
#include "items/weapon_definitions.h"
#include "items/item_types.h"
#include "math/real_math_limit.h"
#include "math/matrix_math_types.h"
#include "units/unit_types.h"
#include "units/biped_types.h"
#include "units/biped_definitions.h"
#include "ai/weapon_datum_types.h"
#include "units/unit_definitions.h"
#include "motor/motor_program_definition.h"
#include "objects/object_flags.h"
#include "units/motor_request.h"
#include "animations/animation_id.h"
#include "math/matrix_math_functions.h"
#include "math/real_math_functions.h"
#include "units/bipeds.h"
#include "game/game_time_types.h"
#include "game/game_time_inline.h"
#include "objects/object_placement.h"
#include "units/motor_enums.h"
#include "units/motor_task.h"
#include "ai/actor_globals.h"
#include <ppcintrinsics.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "tag_files/tag_types.h"
#include "cseries/cseries_wrapped.h"
#include "memory/data_types.h"
#include "memory/datum_index.h"
#include "memory/tls_data_arrays.h"
#include "animations/animation_state_change_settings.h"
#include "units/action_request.h"
#include "tag_files/tag_groups.h"
#include "memory/data_typed_access.h"
// @address 0x355e10 ?motor_animation_set_mode_weapons_and_state@@YA_NJJJJJMJJ_N@Z
// @address 0x355f60 ?motor_animation_fix_pill@@YA_NJJJ@Z
// @address 0x356738 ?motor_task_set_posture@@YAPAVc_motor_task@@JW4e_motor_task@@W4e_action@@JJ_NPBTreal_point3d@@PBTreal_vector3d@@F2PA_N@Z
// @address 0x357718 ?motor_task_set_mode_and_state@@YAPAVc_motor_task@@JW4e_motor_task@@W4e_action@@JJMFJJ_N@Z
// @address 0x357bb0 ?motor_task_recursive_start_animation@@YA_NJW4e_motor_task@@W4e_action@@J_N@Z
// @address 0x3577d8 ?motor_task_start_animation@@YAPAVc_motor_task@@JW4e_motor_task@@W4e_action@@JJW4e_unit_weapon_config@@FM_N@Z
// @address 0x357ca0 ?motor_task_set_ping@@YAPAVc_motor_task@@JW4e_motor_task@@W4e_action@@W4e_motor_resource@@JJJFFPAF3@Z
// @address 0x358270 ?motor_task_execute_state_change_settings@@YAPAVc_motor_task@@JW4e_motor_task@@W4e_action@@PBUs_animation_state_change_settings@@F_N@Z
// @address 0xc15d0 ?motor_animation_action_loops@@YA_NJW4e_motor_resource@@@Z
// @address 0xd76b8 ?motor_animation_resource_get_channel@@YAPAVc_animation_channel@@JW4e_motor_resource@@@Z
// @address 0xdc7d8 ?clear@c_animation_channel_settings@@QAAXXZ
// @address 0xdcd10 ??0c_animation_channel_settings@@QAA@XZ
// @address 0xdd038 ?compare_goals@s_animation_goal_settings@@QBA?AV?$c_flags@W4e_goal_settings_comparison_bits@@K$03@@ABU1@@Z
// @address 0xdd230 ?compare_base_settings@c_base_channel_settings@@QBA?AV?$c_flags@W4e_channel_settings_comparison_bits@@K$09@@ABV1@@Z
// @address 0xdd0d0 ?compare_settings@c_animation_channel_settings@@QBA?AV?$c_flags@W4e_channel_settings_comparison_bits@@K$09@@ABV1@@Z
// @address 0x102240 ?motor_animation_task_get_channel@@YAPAVc_animation_channel@@JPAVc_motor_task@@@Z
// @address 0xe8418 ?motor_animation_test_mode@@YA_NJJJJ@Z
// @address 0xe8bf0 ?motor_animation_test_mode@@YA_NJJ@Z
// @address 0x354ee0 ?motor_animation_running@@YA_NJJ@Z
// @address 0x355460 ?motor_animation_test_mode_and_impulse@@YA_NJJJJJ@Z
// @address 0x3557a8 ?motor_animation_task_busy@@YA_NJPAVc_motor_task@@W4e_motor_resource@@_N2@Z
// @address 0x3558d0 ?motor_animation_get_resource_owner@@YAPAVc_motor_task@@JW4e_motor_resource@@@Z
// @address 0x355ec8 ?motor_task_find_by_action@@YAPAVc_motor_task@@JW4e_action@@@Z
// @address 0x356178 ?motor_animation_default_ground_mode@@YAJJ@Z
// @address 0x356e80 ?motor_animation_unit_entry_finished@@YAXJJ@Z


// Forces walking animation regardless of movement speed.
struct object_region_information;
bool g_force_unit_walking;


/* enums and fixed classes */
typedef c_flags<e_goal_settings_comparison_bits, unsigned long, 4> c_goal_settings_comparison_flags;




typedef s_unit_animation_storage s_motor_animation_unit_animation;


enum e_action_interruption_context {
    _action_interruption_primary_event_bit,
    _action_interruption_keyframe_event_bit,
};

struct s_motor_program_data { e_motor_program program; };


/* 0x3556b0 */


struct s_motor_task_block { c_motor_task tasks[4]; };

enum e_motor_enter_seat_weapon_flags {
    _motor_enter_seat_weapon_flag_support = 3,
    _motor_enter_seat_weapon_flag_must_be_readied = 13,
};

struct s_motor_enter_seat_node_orientations {
    real_quaternion rotation;
    real_point3d position;
};

enum {
    k_motor_animation_start_default_flags = 29,
    k_motor_animation_start_flag_single = 1 << 10,
    k_motor_animation_start_flag_dual = 1 << 11,
};

struct s_motor_animation_analog_channels {
    c_animation_channel state;
    c_animation_channel transition;
};

static __forceinline c_animation_manager *object_get_block(char *object)
{
    return (c_animation_manager *)(object + ((object_datum *)object)->animation.offset);
}

static __forceinline s_motor_animation_unit_animation *motor_animation_unit_animation_get(unit_datum *object)
{
    return (s_motor_animation_unit_animation *)((char *)object + object->unit.animation_storage.offset);
}
static __forceinline c_animation_manager *motor_animation_manager_get(unit_datum *object)
{
    return (c_animation_manager *)((char *)object + object->object.animation.offset);
}
static __forceinline s_motor_task_block *motor_animation_tasks_get(unit_datum *object)
{
    return (s_motor_task_block *)((char *)object + object->unit.motor_tasks.offset);
}
static __forceinline long channel_impulse_name_get(const c_animation_channel *channel)
{
    return channel->m_definition_index;
}
static __forceinline bool motor_task_valid(const c_motor_task *task)
{
    return task->task != NONE && task->program != NONE && task->enabled != 0;
}
static __forceinline bool motor_task_conflicts(const c_motor_task *task, short resources)
{
    return (resources & (short)task->resources) != 0;
}
static __forceinline bool motor_task_program_default(const c_motor_task *task)
{
    return motor_task_valid(task) && task->task == 0;
}
static __forceinline bool animation_manager_validate_mode(const c_animation_manager *manager, long name)
{
    bool result = false;
    if (manager->m_graph_tag_index != NONE) {
        const c_model_animation_graph *graph = (const c_model_animation_graph *)tag_get('jmad', manager->m_graph_tag_index);
        result = find_named_block_entry_with_size(&graph->m_modes, name, sizeof(c_animation_mode)) != 0;
    }
    return result;
}

/* 0xc15d0 */
bool motor_animation_action_loops(long object_index, e_motor_resource resource)
{
    bool result = false;
    c_animation_channel *channel = motor_animation_resource_get_channel(object_index, resource);
    long definition_index = channel->m_definition_index;
    switch (definition_index) {
    case 8:
    case 9:
    case _primary_weapon_overheat_loop_animation_name:
    case _secondary_weapon_overheat_loop_animation_name:
        result = true;
        break;
    }
    return result;
}

/* 0xd76b8 */
c_animation_channel *motor_animation_resource_get_channel(long object_index, e_motor_resource resource)
{
    s_object_header *header = object_header_get(object_index);
    void *result = 0;
    char *object = (char *)header->datum;
    object_type_definition *definition = object_type_definitions[header->object_type];
    short count = 0;
    if (definition->mixing_board_definition_index != -1) {
        c_mixing_board_definition *layout = &g_mixing_board_definition_list[definition->mixing_board_definition_index];
        count = (short)layout->component_types[1].count;
    }
    switch (resource) {
    case 0:
        result = object_get_block(object);
        break;
    case 1:
        if (count > 2)
            result = &((short *)object_get_block(object)->get_mixing_board_animation_channel(0))[g_mixing_board_component_sizes[1]];
        break;
    case 2:
        if (count > 0)
            result = object_get_block(object)->get_mixing_board_animation_channel(0);
        break;
    case 3:
        if (count > 1)
            result = &((char *)object_get_block(object)->get_mixing_board_animation_channel(0))[g_mixing_board_component_sizes[1]];
        break;
    default:
        // match: 0xd76b8: default __assume(0) omits the resource>3 range check (cmplwi r4,3, early bgtlr) from the channel switch.
        __assume(0);
    }
    return (c_animation_channel *)result;
}

// @address 0xd8188 ?motor_animation_resource_busy@@YA_NJW4e_motor_resource@@@Z
bool motor_animation_resource_busy(long object_index, e_motor_resource resource)
{
    unit_datum *object = (unit_datum *)object_get(object_index);
    s_motor_task_block *tasks = motor_animation_tasks_get(object);
    bool current_empty = false;
    bool weapons_valid = false;
    bool busy = false;
    motor_interrupability(object_index, &current_empty, &weapons_valid);
    for (short task_index = 0; task_index < 4; task_index++) {
        c_motor_task *task = &tasks->tasks[task_index];
        if (motor_task_conflicts(task, (short)(1 << resource))) {
            busy = motor_animation_task_busy(object_index, (c_motor_task *)task, (e_motor_resource)task_index, current_empty, weapons_valid);
            if (busy)
                break;
        }
    }
    return busy;
}

/* 0xdc7d8 */
inline void c_animation_channel_settings::clear()
{
    clear_base();
    m_playback_ratio = 0.0f;
    m_playback_rate = 1.0f;
    m_flags.clear();
    for (long i = 0; i < 2; i++)
    {
        m_blend_ids[i].clear();
        m_blend_ratios[i] = 0.0f;
    }
}

// match: 0x355b38: auto_inline(off) keeps motor_animation_resource_get_channel a call in motor_animation_update_impulses_on_mode_change; removal inlines it.
#pragma auto_inline(off)
/* 0xdd038 */
c_goal_settings_comparison_flags s_animation_goal_settings::compare_goals(const s_animation_goal_settings &other) const
{
    c_goal_settings_comparison_flags result;
    result.clear();
    result.set(_comparison_goal_field_0_bit, mode != other.mode);
    result.set(_comparison_goal_field_1_bit, weapon_class != other.weapon_class);
    result.set(_comparison_goal_field_2_bit, weapon_type != other.weapon_type);
    result.set(_comparison_goal_field_3_bit, state != other.state);
    return result;
}

/* 0xdd230 */
inline c_flags<e_channel_settings_comparison_bits, unsigned long, 10> c_base_channel_settings::compare_base_settings(const c_base_channel_settings &other) const
{
    c_flags<e_channel_settings_comparison_bits, unsigned long, 10> result;
    result.clear();
    result.set(_comparison_channel_graph_bit, m_graph_index != other.m_graph_index);
    result.set(_comparison_channel_definition_bit, m_definition_index != other.m_definition_index);
    result.set(_comparison_channel_animation_bit, m_animation_id != other.m_animation_id);
    result.set(_comparison_channel_playback_flags_bit, m_playback_flags.get_unsafe() != other.m_playback_flags.get_unsafe());
    result.set(_comparison_channel_location_bit, m_location != other.m_location);
    return result;
}

/* 0xdd0d0 */
c_flags<e_channel_settings_comparison_bits, unsigned long, 10> c_animation_channel_settings::compare_settings(const c_animation_channel_settings &other) const
{
    c_flags<e_channel_settings_comparison_bits, unsigned long, 10> result = compare_base_settings(other);
    result.set(_comparison_channel_flags_bit, m_flags.test(_channel_settings_paused_bit) != other.m_flags.test(_channel_settings_paused_bit));
    result.set(_comparison_playback_ratio_bit, (float)fabs(m_playback_ratio - other.m_playback_ratio) >= 0.0001f);
    result.set(_comparison_playback_rate_bit, (float)fabs(m_playback_rate - other.m_playback_rate) >= 0.0001f);
    for (long i = 0; i < 2; i++)
    {
        if (m_blend_ids[i] != other.m_blend_ids[i])
            result.set(_comparison_blend_ids_bit, true);
        if (m_blend_ratios[i] != other.m_blend_ratios[i])
            result.set(_comparison_blend_ratios_bit, true);
    }
    return result;
}
// match: 0x355b38: auto_inline(on) ends the off-region above so only the compare helpers are affected.
#pragma auto_inline(on)

// @address 0xdd308 ?compare_settings@s_animation_state_change_settings@@QBA_NABU1@PAUs_state_changes_requested@@@Z
bool s_animation_state_change_settings::compare_settings(const s_animation_state_change_settings &other, s_state_changes_requested *change_flags) const
{
    c_goal_settings_comparison_flags goal_changes;
    goal_changes = goals.compare_goals(other.goals);
    change_flags->goals = goal_changes;
    c_flags<e_channel_settings_comparison_bits, unsigned long, 10> primary_changes;
    primary_changes = primary.compare_settings(other.primary);
    change_flags->primary = primary_changes;
    c_flags<e_channel_settings_comparison_bits, unsigned long, 10> transition_changes;
    transition_changes = transition.compare_settings(other.transition);
    change_flags->transition = transition_changes;
    return change_flags->goals.get_unsafe() == 0 && change_flags->primary.get_unsafe() == 0 && change_flags->transition.get_unsafe() == 0;
}


typedef c_flags<e_action_interruption_context, unsigned short, 2> c_action_interruption_flags;

static __forceinline bool motor_animation_action_allow_interruption(e_action action, e_action task_action, c_action_interruption_flags *context)
{
    bool allow = true;
    if (task_action == _action_none)
        allow = true;
    else if (action == _action_none)
        allow = false;
    else if (action < k_action_count && task_action < k_action_count) {
        const s_action_usage *new_usage = &g_action_usage_definitions[action];
        const s_action_usage *task_usage = &g_action_usage_definitions[task_action];
        if ((task_usage->flags & 4) != 0 && context->test(_action_interruption_keyframe_event_bit))
            allow = true;
        else if (task_usage->priority > new_usage->priority)
            allow = false;
        else if (task_usage->priority < new_usage->priority)
            allow = true;
        else if ((new_usage->flags & 1) != 0) {
            if ((task_usage->flags & 2) != 0 && context->test(_action_interruption_primary_event_bit))
                allow = false;
            else
                allow = true;
        } else
            allow = false;
    }
    return allow;
}

static __forceinline void motor_animation_get_interruption_context(
    long object_index, e_motor_resource resource, e_action action, c_action_interruption_flags *context)
{
    context->clear();
    c_animation_channel *channel = motor_animation_resource_get_channel(object_index, resource);
    c_flags_no_init<e_animation_frame_event_types, unsigned short, 13> event_flags = channel->get_past_event_flags();
    context->set(_action_interruption_primary_event_bit, event_flags.test(_animation_frame_event_type_primary_keyframe));
    context->set(_action_interruption_keyframe_event_bit, event_flags.test(_animation_frame_event_type_4));
}

// @address 0xdd5b0 ?motor_task_allow@@YA_NJW4e_action@@F@Z
bool motor_task_allow(long object_index, e_action action, short resource_mask)
{
    unit_datum *object = (unit_datum *)object_get(object_index);
    s_motor_task_block *tasks = motor_animation_tasks_get(object);
    bool allow = true;
    bool weapons_valid;
    bool current_empty;
    motor_interrupability(object_index, &current_empty, &weapons_valid);
    for (short task_index = 0; task_index < 4; task_index++) {
        c_motor_task *task = (c_motor_task *)&tasks->tasks[task_index];
        if (task->valid() && task->conflicts(resource_mask) &&
            motor_animation_task_busy(object_index, task, (e_motor_resource)task_index, current_empty, weapons_valid)) {
            c_action_interruption_flags context;
            context.clear();
            motor_animation_get_interruption_context(object_index, (e_motor_resource)task_index, task->action, &context);
            allow &= motor_animation_action_allow_interruption(action, task->action, &context);
            if (!allow)
                break;
        }
    }
    return allow;
}

static __forceinline s_motor_program_data *motor_animation_program_get(unit_datum *object)
{
    return (s_motor_program_data *)((char *)object + object->unit.motor_program.offset);
}
// @address 0xdd7a8 ?motor_task_new@@YAPAVc_motor_task@@JW4e_motor_resource@@JJW4e_motor_task@@W4e_action@@@Z
c_motor_task *motor_task_new(
    long object_index, e_motor_resource resource_type, long retained_resources, long resource_mask, e_motor_task task_type, e_action action)
{
    unit_datum *object = (unit_datum *)object_get(object_index);
    s_motor_program_data *program = motor_animation_program_get(object);
    s_motor_task_block *tasks = motor_animation_tasks_get(object);
    for (short index = 0; index < 4; index++) {
        c_motor_task *task = (c_motor_task *)&tasks->tasks[index];
        if (task->valid() && task->conflicts((short)resource_mask))
            motor_system_interrupted(object_index, task, (retained_resources & (1 << index)) == 0);
    }
    c_motor_task *task = (c_motor_task *)&tasks->tasks[resource_type];
    task->set(task_type, action, program->program, (short)resource_mask);
    return task;
}

/* 0xe8418 */
bool motor_animation_test_mode(long object_index, long mode, long weapon_class, long weapon_type)
{
    unit_datum *object = (unit_datum *)object_get(object_index);
    c_animation_manager *manager = (c_animation_manager *)motor_animation_manager_get(object);
    return manager->find_goal_animation_id(mode, weapon_class, weapon_type, 12, 2).valid();
}

/* 0xe8bf0 */
bool motor_animation_test_mode(long object_index, long mode_name)
{
    unit_datum *object = (unit_datum *)object_get(object_index);
    c_animation_manager *manager = motor_animation_manager_get(object);
    return animation_manager_validate_mode(manager, mode_name);
}

/* 0x102240 */
c_animation_channel *motor_animation_task_get_channel(long object_index, c_motor_task *task)
{
    void *result = 0;
    char *object = (char *)object_header_get(object_index)->datum;
    c_motor_task *tasks = (c_motor_task *)(object + ((unit_datum *)object)->unit.motor_tasks.offset);
    for (short i = 0; i < 4; i++) {
        if (&tasks[i] == task) {
            short index = i;
            if (index != -1)
                result = motor_animation_resource_get_channel(object_index, (e_motor_resource)index);
            break;
        }
    }
    return (c_animation_channel *)result;
}

// @address 0xb38f0 ?motor_animation_get_mode_and_state@@YA_NJPAJ0@Z
// match: 0xb38f0: pointer __restrict qualifier prevents alias reload
bool motor_animation_get_mode_and_state(long _sym_p0, long * __restrict _sym_p1, long * __restrict _sym_p2)
{
  bool result = false;
  char *object = (char *)object_header_get(_sym_p0)->datum;
  c_animation_manager *block = object_get_block(object);
  bool valid = (block->m_flags >> 6) & 1;
  if (valid) {
    *((int *)_sym_p1) = block->m_current_goal.mode;
    result = true;
    *((int *)_sym_p2) = block->m_current_goal.state;
  } else {
    *((int *)_sym_p1) = -1;
    *((int *)_sym_p2) = -1;
  }
  return result;
}

/* 0x354ee0 */
bool motor_animation_running(long object_index, long impulse_name)
{
    unit_datum *object = (unit_datum *)object_get(object_index);
    c_animation_manager *manager = motor_animation_manager_get(object);
    c_animation_channel *first_channel = manager->get_mixing_board_animation_channel(0);
    c_animation_channel *second_channel = manager->get_mixing_board_animation_channel(1);
    bool result = false;
    if (channel_impulse_name_get(first_channel) == impulse_name)
        result = true;
    else if (channel_impulse_name_get(second_channel) == impulse_name)
        result = true;
    else {
        int state;
        int mode;
        if (::motor_animation_get_mode_and_state(object_index, (long *)(&mode), (long *)(&state)) && state == impulse_name)
            result = true;
    }
    return result;
}

static __forceinline bool motor_animation_channel_view_valid(const c_animation_channel *channel)
{
    return channel->m_enabled && channel->m_graph_index != NONE && channel->m_animation_id.m_animation_index >= 0;
}

// @address 0x354fd8 ?motor_animation_base_channel_time@@YAMJPAM@Z
float motor_animation_base_channel_time(long object_index, float *authored_duration)
{
    unit_datum *object = (unit_datum *)object_get(object_index);
    c_animation_channel *channel = (c_animation_channel *)motor_animation_manager_get(object);
    if (authored_duration) {
        float duration = 0.0f;
        if (motor_animation_channel_view_valid(channel))
            duration = ((c_channel_base *)channel)->get_authored_duration();
        *authored_duration = duration;
    }
    float time = 0.0f;
    if (motor_animation_channel_view_valid(channel))
        time = channel->m_frame_position * (1.0f / 30.0f);
    return time;
}

/* 0x3550d8 */


static const unsigned long k_animation_playback_default_flags = 29;


static __forceinline const c_model_animation_graph *motor_animation_graph_get(long graph_index)
{
    return (const c_model_animation_graph *)tag_get('jmad', graph_index);
}

static __forceinline const c_model_animation_graph *motor_animation_manager_get_graph(const c_animation_manager *manager)
{
    return motor_animation_graph_get(manager->m_graph_tag_index);
}

static __forceinline const c_model_animation *motor_animation_manager_get_animation(const c_animation_manager *manager, c_animation_id animation_id)
{
    return motor_animation_manager_get_graph(manager)->get_animation(animation_id);
}

static __forceinline const c_model_animation *motor_animation_channel_get_animation(const c_animation_channel *channel)
{
    const c_model_animation *animation = 0;
    if (channel->m_animation_id.valid())
        animation = motor_animation_graph_get(channel->m_graph_index)->get_animation(channel->m_animation_id);
    return animation;
}

bool motor_animation_get_range(long object_index, long impulse, long weapon_type, short component, bool allow_default_mode,
    float *total_frames, short *total_ticks, float *key_frame, short *key_ticks)
{
    unit_datum *object = (unit_datum *)object_get(object_index);
    c_animation_manager *manager = (c_animation_manager *)motor_animation_manager_get(object);
    const c_model_animation *animation = 0;
    c_animation_id animation_id;
    bool result = false;
    animation_id = manager->find_goal_animation_id(impulse, 226, 226, weapon_type, 1);
    if (animation_id.valid()) {
        animation = motor_animation_manager_get_animation(manager, animation_id);
    } else {
        c_animation_channel channel;
        if (manager->setup_animation_channel_by_parallel_impulse(&channel, 226, 226, weapon_type, k_animation_playback_default_flags) && channel.valid()) {
            animation_id = channel.m_animation_id;
            animation = motor_animation_channel_get_animation(&channel);
        }
        animation_channel_initialize_and_disable(&channel);
    }
    if (!animation && allow_default_mode && impulse != 138) {
        int mode;
        int state;
        if (!::motor_animation_get_mode_and_state(object_index, (long *)(&mode), (long *)(&state)) || mode != 138) {
            animation_id = manager->find_goal_animation_id(138, 226, 226, weapon_type, 1);
            if (animation_id.valid())
                animation = motor_animation_manager_get_animation(manager, animation_id);
        }
    }
    if (animation) {
        animation_definitions_compute_movement_totals(motor_animation_manager_get_graph(manager), animation_id, component, key_frame, total_frames);
        if (total_ticks)
            *total_ticks = (short)game_seconds_to_ticks_round(animation->get_frame_count() / 30.0f);
        if (key_ticks) {
            long key_frame_index = animation->get_primary_key_frame_index();
            if (key_frame_index != NONE)
                *key_ticks = (short)game_seconds_to_ticks_round(key_frame_index / 30.0f);
            else
                *key_ticks = NONE;
        }
        result = true;
    }
    return result;
}

/* 0x355460 */
bool motor_animation_test_mode_and_impulse(long object_index, long mode_name, long weapon_class_name, long weapon_type_name, long impulse_name)
{
    unit_datum *object = (unit_datum *)object_get(object_index);
    c_animation_manager *manager = motor_animation_manager_get(object);
    if (mode_name == 226) {
        mode_name = manager->m_current_goal.mode;
        if (mode_name == -1)
            mode_name = 138;
    } else if (mode_name == 1) {
        mode_name = 138;
    }
    if (weapon_class_name == 226)
        weapon_class_name = manager->m_current_goal.weapon_class;
    if (weapon_type_name == 226)
        weapon_type_name = manager->m_current_goal.weapon_type;
    c_animation_id animation_id = ((const c_model_animation_graph *)tag_get('jmad', manager->m_graph_tag_index))
                                      ->find_overlay(mode_name, weapon_class_name, weapon_type_name, impulse_name, 0, 0, 0);
    return animation_id.m_animation_index >= 0;
}

/* 0x355538 */
static __forceinline char *motor_animation_validate_animation_manager_get(long object_index)
{
    unit_datum *object = (unit_datum *)object_get(object_index);
    return (char *)motor_animation_manager_get(object);
}

static __forceinline bool motor_animation_validate_animation_test_mode_and_impulse(
    long object_index, long mode, long weapon_class, long weapon_type, long impulse)
{
    c_animation_manager *manager = (c_animation_manager *)motor_animation_validate_animation_manager_get(object_index);
    if (mode == 226) {
        mode = manager->m_current_goal.mode;
        if (mode == NONE)
            mode = 138;
    } else if (mode == 1) {
        mode = 138;
    }
    if (weapon_class == 226)
        weapon_class = manager->m_current_goal.weapon_class;
    if (weapon_type == 226)
        weapon_type = manager->m_current_goal.weapon_type;
    c_animation_id animation_id = ((const c_model_animation_graph *)tag_get('jmad', manager->m_graph_tag_index))
                                      ->find_overlay(mode, weapon_class, weapon_type, impulse, 0, 0, 0);
    return animation_id.m_animation_index >= 0;
}

static __forceinline bool motor_animation_validate_animation_test_mode_and_state(long object_index, long mode, long weapon_class, long weapon_type, long state)
{
    c_animation_manager *manager = (c_animation_manager *)motor_animation_validate_animation_manager_get(object_index);
    return ((const c_animation_manager *)manager)->find_goal(mode, weapon_class, weapon_type, state, 0);
}

// @address 0x355538 ?motor_animation_validate_animation@@YA_NJJJPA_N@Z
bool motor_animation_validate_animation(long object_index, long mode, long impulse, bool *used_default_mode)
{
    bool valid = false;
    if (used_default_mode)
        *used_default_mode = false;
    if (motor_animation_test_mode_and_impulse(object_index, mode, 226, 226, impulse)) {
        valid = true;
    } else if (motor_animation_validate_animation_test_mode_and_state(object_index, mode, 226, 226, impulse)) {
        valid = true;
    } else if (used_default_mode) {
        if (motor_animation_validate_animation_test_mode_and_impulse(object_index, 138, 226, 226, impulse)) {
            valid = true;
            *used_default_mode = true;
        } else if (motor_animation_validate_animation_test_mode_and_state(object_index, 138, 226, 226, impulse)) {
            valid = true;
            *used_default_mode = true;
        }
    }
    return valid;
}

// @address 0x3556b0 motor_interrupability
// match: 0x3556b0: pointer __restrict qualifier prevents alias reload
extern "C" void motor_interrupability(unsigned int unit_index, bool *__restrict current_empty, bool *__restrict valid)
{
      *current_empty = false;
    *valid = false;
    s_object_header *header = object_header_get(unit_index);
    if (((1 << header->object_type) & 0x1003) == 0)
        return;
    unit_datum *unit = (unit_datum *)header->datum;
    if (unit->unit.player_index == NONE)
        return;
    if (unit->unit.current_weapon_index != -1) {
        long weapon_index = unit->unit.weapon_object_indices[unit->unit.current_weapon_index];
        if (weapon_index != -1)
            *current_empty = ((weapon_datum *)object_header_get(weapon_index)->datum)->ready_ticks <= 0;
    }
    if (unit->unit.desired_weapon_index != -1) {
        long weapon_index = unit->unit.weapon_object_indices[unit->unit.desired_weapon_index];
        if (weapon_index != -1) {
            bool empty = ((weapon_datum *)object_header_get(weapon_index)->datum)->ready_ticks <= 0;
            if (!empty)
                *current_empty = false;
        }
    }
    *valid = true;
}

/* 0x3557a8 */
bool motor_animation_task_busy(long object_index, c_motor_task *task, e_motor_resource resource, bool first, bool second)
{
    c_motor_task *data = (c_motor_task *)task;
    bool result = false;
    if (motor_task_valid(data) && !task->program_default()) {
        result = true;
        if (data->action >= 0 && data->action < k_action_count) {
            s_action_usage *usage = &g_action_usage_definitions[data->action];
            if ((usage->flags & 8) == 0)
                result = false;
            else if ((usage->flags & 16) != 0)
                result = !first;
            if (result && second && first) {
                if (motor_animation_resource_get_channel(object_index, resource)->get_past_event_flags().test(_animation_frame_event_type_4))
                    result = false;
            }
        }
    }
    return result;
}

/* 0x3558d0 */
c_motor_task *motor_animation_get_resource_owner(long object_index, e_motor_resource resource)
{
    unit_datum *object = (unit_datum *)object_get(object_index);
    s_motor_task_block *tasks = motor_animation_tasks_get(object);
    c_motor_task *result = 0;
    for (short index = 0; index < 4; index++) {
        c_motor_task *task = &tasks->tasks[index];
        if (motor_task_conflicts(task, (short)(1 << resource)) && motor_task_valid(task) && !motor_task_program_default(task))
            result = (c_motor_task *)task;
    }
    return result;
}

// @address 0x3559c8 ?motor_animation_busy@@YA_NJ@Z
bool motor_animation_busy(long object_index)
{
    unit_datum *object = (unit_datum *)object_get(object_index);
    s_motor_task_block *tasks = motor_animation_tasks_get(object);
    bool weapons_valid = false;
    bool current_empty = false;
    bool busy = false;
    motor_interrupability(object_index, &current_empty, &weapons_valid);
    for (short task_index = 0; task_index < 4; task_index++) {
        busy = motor_animation_task_busy(object_index, (c_motor_task *)&tasks->tasks[task_index],
            (e_motor_resource)task_index, current_empty, weapons_valid);
        if (busy)
            break;
    }
    return busy;
}

// @address 0x355a60 ?motor_task_interrupt@@YAXJFF@Z
void motor_task_interrupt(long object_index, short resource_mask, short allowed_resources)
{
    unit_datum *object = (unit_datum *)object_get(object_index);
    s_motor_task_block *tasks = motor_animation_tasks_get(object);
    for (short index = 0; index < 4; index++) {
        c_motor_task *task = (c_motor_task *)&tasks->tasks[index];
        if (task->valid()) {
            long task_resources = (short)task->resources;
            if ((short)(resource_mask & task_resources) != 0) {
                if (allowed_resources == 0 || (task_resources & ~allowed_resources) == 0)
                    motor_system_interrupted(object_index, task, true);
            }
        }
    }
}

// @address 0x355b38 ?motor_animation_update_impulses_on_mode_change@@YAXJ@Z
void motor_animation_update_impulses_on_mode_change(long object_index)
{
    s_object_header *header = object_header_get(object_index);
    unit_datum *object = (unit_datum *)header->datum;
    c_animation_manager *target = (c_animation_manager *)object_get_block((char *)object);
    c_animation_channel *second_block = motor_animation_resource_get_channel(object_index, _units_primary_weapon_resource);
    c_animation_channel *third_block = 0;
    object_type_definition *definition = object_type_definitions[header->object_type];
    short count = 0;
    if (definition->mixing_board_definition_index != -1) {
        c_mixing_board_definition *layout = &g_mixing_board_definition_list[definition->mixing_board_definition_index];
        count = (short)layout->component_types[1].count;
    }
    if (count > 1) {
        // match: 0x355b38: volatile load preserves memory access order
        c_animation_manager *manager = (c_animation_manager *)((char *)object + const_cast<volatile short &>(object->object.animation.offset));
        third_block = manager->get_mixing_board_animation_channel(1);
    }
    c_animation_channel *first_block = motor_animation_resource_get_channel(object_index, _motor_resource_overlay);
    if (second_block != 0 && !target->update_animation_channel_on_graph_change(second_block, 226, 226)) {
        c_motor_task *entry = &motor_animation_tasks_get((unit_datum *)object_get(object_index))->tasks[2];
        if (entry != 0 && motor_task_valid(entry))
            motor_system_interrupted(object_index, (c_motor_task *)entry, true);
    }
    if (first_block != 0 && !target->update_animation_channel_on_graph_change(first_block, 226, 226)) {
        c_motor_task *entry = &motor_animation_tasks_get((unit_datum *)object_get(object_index))->tasks[1];
        if (entry != 0 && motor_task_valid(entry))
            motor_system_interrupted(object_index, (c_motor_task *)entry, true);
    }
    if (third_block != 0) {
        long value = -1;
        if (((1 << object_header_get(object_index)->object_type) & 0x1003) != 0)
            value = motor_animation_unit_animation_get(object)->weapon_type;
        if (!target->update_animation_channel_on_graph_change(third_block, 486, value)) {
            c_motor_task *entry = &motor_animation_tasks_get((unit_datum *)object_get(object_index))->tasks[3];
            if (entry != 0 && motor_task_valid(entry))
                motor_system_interrupted(object_index, (c_motor_task *)entry, true);
        }
    }
}


/* 0x355e10 */
bool motor_animation_set_mode_weapons_and_state(
    long object_index, long mode, long weapon_class, long weapon_type, long state, float rate, long unknown, long flags, bool ignore_allow)
{
    bool result = false;
    s_animation_state_change_settings settings;
    if (motor_animation_recommend_state_change_settings(object_index, mode, weapon_class, weapon_type, state, rate, unknown, flags, ignore_allow, &settings))
        result = motor_animation_execute_state_change_settings(object_index, &settings, ignore_allow);
    return result;
}

/* 0x355ec8 */
c_motor_task *motor_task_find_by_action(long object_index, e_action action)
{
    unit_datum *object = (unit_datum *)object_get(object_index);
    s_motor_task_block *tasks = motor_animation_tasks_get(object);
    c_motor_task *result = 0;
    for (short index = 0; index < 4; index++) {
        c_motor_task *task = &tasks->tasks[index];
        if (motor_task_valid(task) && task->action == action) {
            result = (c_motor_task *)task;
            break;
        } else {
            continue;
        }
    }
    return result;
}




static __forceinline object_datum *motor_animation_fix_pill_unit_get(long object_index)
{
    return (object_datum *)object_header_get(object_index)->datum;
}
static __forceinline render_model_definition *motor_animation_fix_pill_render_model_get(long model_index)
{
    s_model_definition *model = (s_model_definition *)tag_get('hlmt', model_index);
    render_model_definition *result = 0;
    if (model->render_model_index != NONE)
        result = (render_model_definition *)tag_get('mode', model->render_model_index);
    return result;
}
static __forceinline void *motor_animation_fix_pill_block_element_get(const long *block, long index, long size)
{
#ifdef HALO3_64BIT
    // tag blocks keep their 12-byte on-disk layout: the address is a 32-bit slot at offset 4
    return (char *)((const s_tag_block *)block)->address + index * size;
#else
    return (char *)((void **)block)[1] + index * size;
#endif
}
static __forceinline real_matrix4x3 *motor_animation_fix_pill_node_matrix_get(long object_index, short node)
{
    object_datum *object = motor_animation_fix_pill_unit_get(object_index);
    return (real_matrix4x3 *)((char *)object + object->node_matrices.offset) + node;
}
static __forceinline real_orientation *motor_animation_fix_pill_orientation_get(
    const object_datum *object, const s_object_block_reference *reference, long size, long *count)
{
    *count = reference->size / size;
    return (real_orientation *)((char *)object + reference->offset);
}

/* 0x355f60 */
bool motor_animation_fix_pill(long object_index, long mode, long state)
{
    object_datum *unit = motor_animation_fix_pill_unit_get(object_index);
    real_matrix4x3 *root = motor_animation_fix_pill_node_matrix_get(object_index, 0);
    s_object_definition *definition = (s_object_definition *)tag_get(
        'unit', unit->definition);
    render_model_definition *model = motor_animation_fix_pill_render_model_get(definition->model.index);
    render_model_node *node = (render_model_node *)motor_animation_fix_pill_block_element_get(
        &model->nodes.count, 0, sizeof(render_model_node));
    real_matrix4x3 original_inverse;
    memcpy(&original_inverse, &node->default_inverse_matrix, sizeof(real_matrix4x3));
    real_point3d original_translation = node->default_translation;
    real_quaternion original_rotation = node->default_rotation;
    real_matrix4x3 *current = motor_animation_fix_pill_node_matrix_get(object_index, 0);
    real_point3d position = current->position;
    position.z -= original_translation.z;
    real_matrix4x3 transformed;
    matrix4x3_compute_product(current, &original_inverse, &transformed);
    real_vector3d forward = transformed.forward;
    forward.k = 0.0f;
    if (normalize3d(&forward) == 0.0f)
        forward = k_real_math_identity_matrix_storage.forward;
    object_set_position_direct(object_index, &position, &forward, &k_real_math_identity_matrix_storage.up, 0, false);
    if (mode != NONE && state != NONE)
        motor_animation_set_mode_weapons_and_state(object_index, mode, 226, 226, state, 0.0f, 2, 29, false);
    long count;
    real_orientation *original = motor_animation_fix_pill_orientation_get(
        unit, &unit->original_node_orientations, sizeof(real_orientation), &count);
    original[0].translation = original_translation;
    original[0].rotation = original_rotation;
    object_compute_node_matrices(object_index);
    return true;
}

/* 0x356178 */
long motor_animation_default_ground_mode(long object_index)
{
    long result = 138;
    if (motor_animation_test_mode(object_index, 139)) {
        object_datum *object = (object_datum *)object_header_get(object_index)->datum;
        if (object->shield_vitality > 0.0f)
            result = 139;
    }
    return result;
}

// match: noalias declaration preserves caller live registers




static __forceinline biped_datum *motor_animation_posture_object_get(long object_index)
{
    return (biped_datum *)object_header_get(object_index)->datum;
}
static __forceinline real_orientation *motor_animation_posture_orientations_get(const biped_datum *object)
{
    return (real_orientation *)((char *)object + object->object.original_node_orientations.offset);
}
static __forceinline void motor_animation_posture_matrix_from_point_and_vectors(
    real_matrix4x3 *matrix, const real_point3d *position, const real_vector3d *forward)
{
    matrix4x3_rotation_from_vectors(matrix, forward, &k_real_math_identity_matrix_storage.up);
    matrix->position = *position;
}
static __forceinline void motor_animation_posture_matrix_rotation_to_quaternion(const real_matrix4x3 *matrix, real_quaternion *rotation)
{
    matrix3x3_rotation_to_quaternion(&matrix->rotation, rotation);
}
static __forceinline void motor_animation_posture_quaternion_inverse(real_quaternion *rotation)
{
    rotation->w *= -1.0f;
}


// @address 0x3561f8 ?motor_animation_posture_exit_end@@YAXJJJJJ@Z
void motor_animation_posture_exit_end(long object_index, long mode, long exit_name, long exit_name2, long posture)
{
    biped_datum *object = motor_animation_posture_object_get(object_index);
    if (object->object.object_type != 0 || object->biped.physics.m_mode != 4)
    {
        motor_animation_set_mode_weapons_and_state(object_index, mode, exit_name, exit_name2, posture, 0.0f, 2, 29, false);
        if (!object->object.m_physics_flags.test(_object_physics_connected_bit))
            object_reconnect_to_physics(object_index);
        return;
    }
    real_matrix4x3 matrix_a;
    object_get_world_matrix(object_index, &matrix_a);
    biped_datum *node_object = motor_animation_posture_object_get(object_index);
    real_matrix4x3 matrix_b;
    short node_offset = node_object->object.node_matrices.offset;
    memcpy(&matrix_b, (char *)node_object + node_offset, sizeof(matrix_b));
    real_matrix4x3 matrix_c;
    real_matrix4x3 inverse;
    union { real_point3d point; real_quaternion quat; } local_point;
    if (unit_animation_build_root_matrix_by_seat_and_state(object_index, mode, posture, false, 3, &matrix_c))
    {
        matrix4x3_inverse(&matrix_c, &inverse);
        matrix4x3_compute_product(&matrix_b, &inverse, &matrix_c);
        real_vector3d forward_vec;
        forward_vec = matrix_c.forward;
        forward_vec.k = 0.0f;
        if (posture == 79)
        {
            s_object_definition *object_def =
                (s_object_definition *)tag_get('obje', object->object.definition_index);
            s_model_definition *model_def =
                (s_model_definition *)tag_get('hlmt', object_def->model.index);
            long render_index = model_def->render_model_index;
            void *render_def = 0;
            if (render_index != -1)
                render_def = tag_get('mode', render_index);
            local_point.point = matrix_b.position;
            local_point.point.z = matrix_b.position.z - ((render_model_node *)((render_model_definition *)render_def)->nodes.address)->default_translation.z;
            matrix4x3_from_point_and_nonaxial_vectors_using_up(&matrix_a, &local_point.point, &forward_vec, &k_real_math_identity_matrix_storage.up);
            object_set_position_direct(object_index, &matrix_a.position, &matrix_a.forward, &matrix_a.up, 0, false);
        }
        else if (normalize3d(&forward_vec) > 0.0f)
            object_set_position_direct(object_index, 0, &forward_vec, &k_real_math_identity_matrix_storage.up, 0, false);
    }
    if (posture == 79)
    {
        biped_enter_dead_physics(object_index);
        object_set_position_direct(object_index, &matrix_a.position, &matrix_a.forward, &matrix_a.up, 0, false);
        biped_exit_seat_end(object_index, -1);
    }
    else
        biped_enter_default_physics(object_index);
    motor_animation_set_mode_weapons_and_state(object_index, mode, exit_name, exit_name2, posture, 0.0f, 2, 29, false);
    real_orientation *orientations = motor_animation_posture_orientations_get(object);
    object_get_world_matrix(object_index, &inverse);
    matrix4x3_inverse(&inverse, &matrix_c);
    matrix4x3_compute_product(&matrix_c, &matrix_b, &matrix_a);
    motor_animation_posture_matrix_rotation_to_quaternion(&matrix_a, &local_point.quat);
    orientations->rotation = local_point.quat;
    orientations->translation = matrix_a.position;
    object_compute_node_matrices(object_index);
}

// @address 0x356528 motor_animation_set_posture
extern "C" bool motor_animation_set_posture(
    long object_index, long state, long mode, bool physics, const real_point3d *position, const real_vector3d *forward, bool *interruptible)
{
    bool result = false;
    biped_datum *object = motor_animation_posture_object_get(object_index);
    if (interruptible)
        *interruptible = false;
#if defined(_XBOX) && !defined(__clang__)
    result = motor_animation_set_mode_weapons_and_state(object_index, mode, 226, 226, state, literal_0_1f[0], 0, 29, false);
    if (!result) {
        result = motor_animation_set_mode_weapons_and_state(object_index, mode, 226, 226, 12, literal_0_1f[0], 0, 31, false);
        if (result)
            *interruptible = true;
    }
#else
    result = motor_animation_set_mode_weapons_and_state(object_index, mode, 226, 226, state, 0.1f, 0, 29, false);
    if (!result) {
        result = motor_animation_set_mode_weapons_and_state(object_index, mode, 226, 226, 12, 0.1f, 0, 31, false);
        if (result)
            *interruptible = true;
    }
#endif
    if (result && physics && object->object.object_type == 0) {
        if (object->object.parent_object_index != NONE)
            object_detach(object_index);
        real_matrix4x3 original;
        ::object_get_world_matrix(object_index, &original);
        biped_enter_posture_physics(object_index, position, forward);
        real_orientation *orientations = motor_animation_posture_orientations_get(object);
        real_matrix4x3 posture;
        motor_animation_posture_matrix_from_point_and_vectors(&posture, position, forward);
        real_point3d new_position;
        matrix4x3_transform_point(&original, &orientations[0].translation, &new_position);
        matrix4x3_inverse_transform_point(&posture, &new_position, &new_position);
        real_quaternion original_rotation, posture_rotation, new_rotation;
        motor_animation_posture_matrix_rotation_to_quaternion(&original, &original_rotation);
        motor_animation_posture_matrix_rotation_to_quaternion(&posture, &posture_rotation);
        motor_animation_posture_quaternion_inverse(&posture_rotation);
        quaternions_multiply(&original_rotation, &orientations[0].rotation, &new_rotation);
        quaternions_multiply(&posture_rotation, &new_rotation, &new_rotation);
        orientations[0].translation = new_position;
        orientations[0].rotation = new_rotation;
        object_compute_node_matrices(object_index);
    }
    return result;
}

/* 0x356738 */
c_motor_task *motor_task_set_posture(long object_index, e_motor_task motor_task, e_action action, long mode, long state, bool unknown,
    const real_point3d *position, const real_vector3d *forward, short resource_mask, bool ignore_allow, bool *interruptible)
{
    c_motor_task *result = 0;
    if (interruptible)
        *interruptible = false;
    resource_mask |= 1;
    if (ignore_allow || motor_task_allow(object_index, action, resource_mask)) {
        bool interrupt = false;
        if (motor_animation_set_posture(object_index, mode, state, unknown, position, forward, &interrupt)) {
            if (!interrupt)
                result = motor_task_new(object_index, _motor_resource_0, 1, resource_mask, motor_task, action);
            if (interruptible)
                *interruptible = interrupt;
        }
    }
    return result;
}


/* 0x356e80 */
void motor_animation_unit_entry_finished(long unit_index, long)
{
    unit_datum *unit = (unit_datum *)object_header_get(unit_index)->datum;
    if (unit->object.parent_object_index == NONE)
        return;
    if (unit->unit.parent_seat_index == NONE)
        return;
    unit_datum *parent = (unit_datum *)object_header_get(unit->object.parent_object_index)->datum;
    unit_definition *definition = (unit_definition *)tag_get('unit', parent->object.definition_index);
    unit_seat *seat = &definition->seats[unit->unit.parent_seat_index];
    ::object_set_hidden(unit_index, (seat->flags & 1) != 0);
    if ((bool)(_rotl(seat->flags, 13) & 1) && seat->region_index != NONE) {
        bool hidden = false;
        int region_count;
        int unknown_out[2];
        object_region_information *regions;
        ::object_get_region_information(unit->object.parent_object_index, (long *)(&region_count), (char **)(unknown_out), (char **)(0),
            &regions);
        if (seat->region_index < region_count) {
            object_region_information *region = &regions[seat->region_index];
            hidden = region->state < 3;
        }
        ::object_set_hidden(unit_index, hidden);
    }
    if (!(bool)(_rotl(parent->unit.unit_flags, 2) & 1))
        ::unit_correct_dual_wield_state(unit->object.parent_object_index, unit->unit.parent_seat_index, false, false, false);
}

// match: 0x356830: the inline clear boundary and alignment cast preserve the 128-byte request storage.
static __forceinline void motor_animation_clear_action_request(action_request *request)
{
    memset((__int64 *)request, 0, sizeof(*request));
}


static __forceinline biped_datum *motor_enter_seat_object_get(long object_index)
{
    return (biped_datum *)object_header_get(object_index)->datum;
}
static __forceinline long motor_enter_seat_object_get_type(long object_index)
{
    return object_header_get(object_index)->object_type;
}
static __forceinline c_character_physics_component::e_mode motor_enter_seat_mover_get_physics_mode(long object_index)
{
    biped_datum *object = motor_enter_seat_object_get(object_index);
    c_character_physics_component::e_mode mode = c_character_physics_component::_mode_none;
    if (object->object.object_type == 0)
        mode = object->biped.physics.get_mode();
    return mode;
}
static __forceinline long motor_enter_seat_unit_inventory_get_weapon(long unit_index, short inventory_index)
{
    biped_datum *unit = motor_enter_seat_object_get(unit_index);
    long weapon_index = NONE;
    if (inventory_index != NONE)
        weapon_index = unit->unit.weapon_object_indices[inventory_index];
    return weapon_index;
}
static __forceinline const weapon_definition *motor_enter_seat_weapon_definition_get(long weapon_index)
{
    biped_datum *weapon = motor_enter_seat_object_get(weapon_index);
    return (const weapon_definition *)tag_get('weap', weapon->object.definition_index);
}
static __forceinline bool motor_enter_seat_weapon_must_be_readied(long weapon_index)
{
    return (motor_enter_seat_weapon_definition_get(weapon_index)->flags & (1 << _motor_enter_seat_weapon_flag_must_be_readied)) != 0;
}
static __forceinline bool motor_enter_seat_weapon_is_support_weapon(long weapon_index)
{
    return (motor_enter_seat_weapon_definition_get(weapon_index)->flags & (1 << _motor_enter_seat_weapon_flag_support)) != 0;
}
static __forceinline void motor_enter_seat_object_reset_interpolation(long object_index)
{
    biped_datum *object = motor_enter_seat_object_get(object_index);
    c_animation_manager *animation = (c_animation_manager *)((char *)object + object->object.animation.offset);
    animation->m_interpolator.m_time = 0.0f;
    animation->m_interpolator.m_flags = 0;
    animation->m_interpolator.m_duration = 0.0f;
    object_wake(object_index);
}
static __forceinline void motor_enter_seat_unit_reset_seat_acceleration_data(long unit_index)
{
    biped_datum *unit = motor_enter_seat_object_get(unit_index);
    unit->unit.m_unit_flags.set(_unit_flag_reset_seat_acceleration, true);
    unit->unit.seat_acceleration = *global_zero_vector3d;
}

// @address 0x356830 ?motor_task_enter_seat_internal@@YA_NJJF_NJPA_N0@Z
bool motor_task_enter_seat_internal(long unit_index, long vehicle_index, short seat_index, bool skip_entrance_animation, long state,
    bool *finished_out, bool evict_occupant)
{
    bool finished = false;
    bool result = false;
    if (evict_occupant || unit_can_enter_seat(unit_index, vehicle_index, seat_index, NULL, NULL, NULL)) {
        biped_datum *unit = motor_enter_seat_object_get(unit_index);
        if (unit->object.parent_object_index == NONE) {
            if (motor_enter_seat_mover_get_physics_mode(unit_index) == c_character_physics_component::_mode_sentinel)
                motor_animation_posture_exit_end(unit_index, motor_animation_default_ground_mode(unit_index), 226, 226, 12);
            if (((1 << motor_enter_seat_object_get_type(unit_index)) & 1) != 0 && motor_enter_seat_object_get(unit_index)->biped.physics.is_sentinel_mode())
                biped_enter_default_physics(unit_index);
            motor_task_interrupt(unit_index, 15, 0);
            long entrance_animation = NONE;
            if (!skip_entrance_animation)
                entrance_animation = unit_get_best_seat_entrance_animation(unit_index, vehicle_index, seat_index, NULL, NULL);
            bool unit_is_vehicle = ((1 << motor_enter_seat_object_get_type(unit_index)) & 2) != 0;
            biped_datum *parent = motor_enter_seat_object_get(vehicle_index);
            unit_definition *definition =
                (unit_definition *)tag_get('unit', parent->object.definition_index);
            unit_seat *seat = &definition->seats[(long)seat_index];
            action_submit_event(unit_index, 47);
            biped_datum *object = motor_enter_seat_object_get(unit_index);
            real_matrix4x3 *node_matrix = (real_matrix4x3 *)((char *)object + object->object.node_matrices.offset);
            object_marker markers;
            internal_object_get_markers_by_string_id(vehicle_index, seat->marker_name, &markers, 1, false);
            real_matrix4x3 inverse;
            matrix4x3_inverse(&markers.matrix, &inverse);
            real_matrix4x3 multiplied;
            matrix4x3_compute_product(&inverse, node_matrix, &multiplied);
            object_attach_to_marker_with_type(vehicle_index, seat->marker_name, unit_index, 0);
            unit->unit.parent_seat_index = seat_index;
            simulation_action_object_update(unit_index, 0x8000);
            unit_update_driver_and_gunner(vehicle_index);
            if (!unit_is_vehicle) {
                action_submit_event(unit_index, 47);
                long weapon_index = motor_enter_seat_unit_inventory_get_weapon(unit_index, motor_enter_seat_object_get(unit_index)->unit.current_weapon_index);
                if (weapon_index != NONE) {
                    bool must_be_readied = motor_enter_seat_weapon_must_be_readied(weapon_index);
                    if (must_be_readied || !unit_can_use_weapon_in_seat(unit_index, NONE, weapon_index)) {
                        if (must_be_readied || motor_enter_seat_weapon_is_support_weapon(weapon_index)) {
                            action_request request;
                            motor_animation_clear_action_request(&request);
                            request.action = 46;
                            request.exit_vehicle.immediate = true;
                            action_submit(unit_index, &request);
                        } else {
                            unit_action_weapon_put_away_end(unit_index, true);
                        }
                        weapon_index = NONE;
                    }
                }
                if (weapon_index == NONE || seat->test_flag(_unit_seat_hide_inventory_bit)) {
                    long hidden_weapon_index = unit->unit.hidden_weapon_index;
                    for (long i = 0; i < 4; i++) {
                        if (i != hidden_weapon_index) {
                            long item_index = unit->unit.weapon_object_indices[i];
                            if (item_index != NONE && motor_enter_seat_object_get(item_index)->unit.kind == 3)
                                item_hide_inventory(item_index);
                        }
                    }
                }
                if (weapon_index == NONE)
                    unit_action_weapon_set_unarmed_animation(unit_index, true);
                long mode = seat->label;
                long entrance_state = entrance_animation != NONE ? entrance_animation : 12;
                motor_animation_set_mode_weapons_and_state(unit_index, mode, 226, 226, entrance_state, 0.0f, 2, 29, false);
            }
            s_motor_enter_seat_node_orientations *orientations =
                (s_motor_enter_seat_node_orientations *)((char *)unit + unit->object.original_node_orientations.offset);
            real_quaternion rotation;
            matrix3x3_rotation_to_quaternion(&multiplied.rotation, &rotation);
            orientations->position = multiplied.position;
            orientations->rotation = rotation;
            if (entrance_animation == NONE) {
                motor_enter_seat_object_reset_interpolation(unit_index);
                motor_enter_seat_unit_reset_seat_acceleration_data(unit_index);
                finished = true;
            }
            object_compute_node_matrices(unit_index);
            game_engine_unit_entered_vehicle(unit_index, vehicle_index);
            unit_unzoom(unit_index, false);
            motor_enter_seat_unit_reset_seat_acceleration_data(unit_index);
            unit->unit.seat_exit_queued = 0;
            if (unit->unit.actor_index != NONE)
                actor_handle_seat_exit(unit->unit.actor_index, unit->object.parent_object_index, unit->unit.parent_seat_index);
            if (unit->unit.player_index != NONE)
                ai_player_handle_seat_enter(unit->unit.player_index, unit->object.parent_object_index, unit->unit.parent_seat_index);
            if (unit->unit.player_index != NONE && seat->test_flag(_unit_seat_driver_bit))
                player_set_facing(unit->unit.player_index, &parent->unit.facing_vector);
            result = true;
            if (evict_occupant) {
                long occupant = unit_seat_get_occupant(vehicle_index, seat_index);
                if (occupant != unit_index && occupant != NONE) {
                    action_request request;
                    motor_animation_clear_action_request(&request);
                    request.action = 15;
                    request.exit_vehicle.immediate = false;
                    request.exit_vehicle.exit = true;
                    result = action_submit(occupant, &request);
                }
            }
            if (result && !parent->unit.m_unit_flags.test(_unit_flag_no_dual_wield_update))
                unit_correct_dual_wield_state(unit->object.parent_object_index, unit->unit.parent_seat_index, false, false, true);
        }
    }
    if (finished_out != NULL)
        *finished_out = finished;
    return result;
}

/* 0x357718 */
c_motor_task *motor_task_set_mode_and_state(long object_index, e_motor_task motor_task, e_action action, long mode, long state, float rate,
    short resource_mask, long unknown, long flags, bool ignore_allow)
{
    c_motor_task *result = 0;
    s_animation_state_change_settings settings;
    if (motor_animation_recommend_state_change_settings(object_index, mode, 226, 226, state, rate, unknown, flags, ignore_allow, &settings))
        result = motor_task_execute_state_change_settings(object_index, motor_task, action, &settings, resource_mask, ignore_allow);
    return result;
}


typedef s_unit_animation_storage s_motor_animation_start_weapons;
static __forceinline bool motor_animation_start_allows_interpolation_in(const c_channel_base *channel)
{
    bool result = true;
    if (channel->valid()) {
        const c_model_animation *animation = 0;
        if (channel->m_animation_id.m_animation_index >= 0)
            animation = ((const c_model_animation_graph *)tag_get('jmad', channel->m_graph_index))->get_animation(channel->m_animation_id);
        result = (animation->animation_flags & 1) == 0;
    }
    return result;
}

static __forceinline bool motor_animation_start_unit_has_weapons(long object_index)
{
    bool result = false;
    unit_datum *object = (unit_datum *)(unit_datum *)object_get(object_index);
    if (object->unit.current_weapon_index != NONE && object->unit.desired_weapon_index != NONE)
        result = true;
    return result;
}

static __forceinline unsigned long motor_animation_start_playback_flags(long object_index, long weapon_class, e_unit_weapon_config unit_weapon_config)
{
    unsigned long flags = k_motor_animation_start_default_flags;
    if (weapon_class == 486 || (weapon_class == 226 && motor_animation_start_unit_has_weapons(object_index))) {
        if (unit_weapon_config == _unit_weapon_config_primary || unit_weapon_config == _unit_weapon_config_dual)
            flags |= k_motor_animation_start_flag_dual;
        else
            flags |= k_motor_animation_start_flag_single;
    }
    return flags;
}

/* 0x3577d8 */
c_motor_task *motor_task_start_animation(long object_index, e_motor_task motor_task, e_action action, long impulse, long mode,
    e_unit_weapon_config unit_weapon_config, short resource_mask, float interpolation_time, bool ignore_allow)
{
    unit_datum *object = (unit_datum *)(unit_datum *)object_get(object_index);
    c_animation_manager *manager = (c_animation_manager *)((char *)object + object->object.animation.offset);
    c_animation_manager *block = (c_animation_manager *)manager;
    long weapon_class = 226;
    long weapon_type = 226;
    c_motor_task *result = 0;
    short retained_resources = 0;
    e_motor_resource resource = (e_motor_resource)((unit_weapon_config == _unit_weapon_config_dual) + 2);
    c_animation_channel *channel = motor_animation_resource_get_channel(object_index, resource);
    if ((1 << object_header_get(object_index)->object_type & 0x1003) != 0) {
        s_motor_animation_start_weapons *weapons = (s_motor_animation_start_weapons *)((char *)object + object->unit.animation_storage.offset);
        if (unit_weapon_config == _unit_weapon_config_primary || unit_weapon_config == _unit_weapon_config_dual) {
            weapon_class = 486;
            weapon_type = weapons->weapon_type;
        } else if (unit_weapon_config == _unit_weapon_config_secondary && block->m_current_goal.weapon_class == 486) {
            weapon_class = weapons->weapon_class;
            weapon_type = 226;
        }
    }
    bool can_interpolate_out = channel->animation_allows_interpolation_out();
    if (mode != NONE && mode != 226 && mode != block->m_current_goal.mode)
        retained_resources = (short)(retained_resources | 1);
    else
        mode = 226;
    if ((ignore_allow || motor_task_allow(object_index, action, (short)(resource_mask | retained_resources | (1 << resource)))) &&
        motor_animation_test_mode_and_impulse(object_index, mode, weapon_class, weapon_type, impulse))
    {
        short started_resources = 0;
        if (mode != 226) {
            motor_animation_set_mode_weapons_and_state(object_index, mode, 226, 226, 226, 0.0f, 0, k_motor_animation_start_default_flags, false);
            motor_task_interrupt(object_index, 1, 0);
        }
        if (manager->setup_animation_channel_by_parallel_impulse(channel, weapon_class, weapon_type, impulse,
                motor_animation_start_playback_flags(object_index, weapon_class, unit_weapon_config)))
        {
            if (interpolation_time > 0.0f && can_interpolate_out && motor_animation_start_allows_interpolation_in(channel))
                object_start_interpolation(object_index, interpolation_time);
            started_resources = (short)(started_resources | 1 << resource);
        }
        if (started_resources != 0)
            result = motor_task_new(object_index, resource, started_resources, started_resources | resource_mask, motor_task, action);
    }
    if (result == 0) {
        short resources = (short)(resource_mask | 1);
        if ((ignore_allow || motor_task_allow(object_index, action, resources)) &&
            motor_animation_set_mode_weapons_and_state(
                object_index, mode, 226, 226, impulse, interpolation_time, 0, k_motor_animation_start_default_flags, false))
            result = motor_task_new(object_index, _motor_resource_0, 1, resources, motor_task, action);
    }
    return result;
}


static __forceinline void motor_animation_system_event(long object_index, c_motor_task *task, long event)
{
    if (task != 0 && task->valid()) {
        c_motor_program_definition *program = g_program_definition[task->program];
        s_motor_task_definition *definition = program->find_task_definition(task->task);
        if (definition != 0 && definition->animation_event != 0)
            definition->animation_event(object_index, task, event);
    }
}



/* 0x357bb0 */
bool motor_task_recursive_start_animation(long object_index, e_motor_task motor_task, e_action action, long impulse, bool ignore_allow)
{
    bool result = false;
    object_datum *object = (object_datum *)object_header_get(object_index)->datum;
    c_animation_manager *block = (c_animation_manager *)((char *)object + object->animation.offset);
    if (block->m_current_goal.state != impulse &&
        motor_task_start_animation(object_index, motor_task, action, impulse, 226, _unit_weapon_config_none, 0, 0.0f, ignore_allow) != 0)
    {
        result = true;
        long child_index = object->first_child_object_index;
        while (child_index != NONE) {
            s_object_header *child_header = object_header_get(child_index);
            object_datum *child = (object_datum *)child_header->datum;
            if ((bool)(_rotl(child->flags, 6) & 1) && ((result << child_header->object_type) & 0x1803) != 0)
                motor_task_recursive_start_animation(child_index, motor_task, action, impulse, ignore_allow);
            child_index = child->next_object_index;
        }
    }
    return result;
}

/* 0x357ca0 */
c_motor_task *motor_task_set_ping(long object_index, e_motor_task motor_task, e_action action, e_motor_resource motor_resource, long state,
    long mode, long damage, short desired_direction, short desired_region, short *actual_direction, short *actual_region)
{
    unit_datum *object = (unit_datum *)object_get(object_index);
    c_animation_manager *manager = motor_animation_manager_get(object);
    c_motor_task *result = 0;
    short resources = 0;
    short interruptions = 0;
    if (motor_task_allow(object_index, action, (short)(1 << motor_resource))) {
        bool played = false;
        if (state != NONE && mode != NONE) {
            long change_flags = 2;
            if (damage != NONE)
                change_flags |= 12;
            if (motor_animation_set_mode_weapons_and_state(object_index, mode, 226, 226, state, 0.0f, change_flags, 29, false)) {
                resources = 15;
                interruptions |= 1;
                played = true;
            }
        }
        c_animation_channel *channel = motor_animation_resource_get_channel(object_index, motor_resource);
        if (((c_animation_manager *)manager)
                ->setup_animation_channel_by_death_or_damage(
                    channel, 226, damage, desired_direction, desired_region, actual_direction, actual_region))
        {
            if (((1 << object_header_get(object_index)->object_type) & 0x1003) && motor_resource == _motor_resource_1)
                motor_animation_unit_animation_get(object)->flag_bits.set(_motor_animation_unit_flag_overlay, true);
            resources |= (1 << motor_resource);
            interruptions |= (1 << motor_resource);
            played = true;
        }
        if (played)
            result = motor_task_new(object_index, motor_resource, interruptions, resources, _motor_task_ping, action);
    }
    return result;
}

/* 0x358270 */
c_motor_task *motor_task_execute_state_change_settings(long object_index, e_motor_task motor_task, e_action action,
    const s_animation_state_change_settings *settings, short resource_mask, bool ignore_allow)
{
    c_motor_task *result = 0;
    if ((ignore_allow || motor_task_allow(object_index, action, resource_mask)) &&
        motor_animation_execute_state_change_settings(object_index, settings, ignore_allow))
    {
        result = motor_task_new(object_index, _motor_resource_0, 1, resource_mask, motor_task, action);
        if (result != 0 && motor_task == 0)
            motor_animation_system_event(object_index, result, 1);
    }
    return result;
}


// @address 0xdd3a8 motor_task_start_custom_animation
extern "C" c_motor_task *motor_task_start_custom_animation(long object_index, e_motor_task motor_task, e_action action,
    long graph_tag_index, long animation_name, bool goal_flag, bool clear_custom)
{
    c_motor_task *result = 0;
    unit_datum *object = (unit_datum *)object_header_get(object_index)->datum;
    if ((unsigned short)object->object.animation.offset != 0xffff
        && ((1 << object_header_get(object_index)->object_type) & 0x1003) != 0
        && motor_task_allow(object_index, action, 15)) {
        const c_model_animation_graph *definition = (const c_model_animation_graph *)tag_get('jmad', graph_tag_index);
        if (definition != 0) {
            c_animation_id animation_id = definition->find_animation(animation_name);
            if (animation_id.m_animation_index >= 0) {
                bool custom_set = false;
                short manager_offset = object->object.animation.offset;
                short unit_offset = object->unit.animation_storage.offset;
                long unit_byte_offset = unit_offset;
                c_animation_manager *manager = (c_animation_manager *)((char *)object + manager_offset);
                const s_object_definition *object_definition = (const s_object_definition *) tag_get('obje', object->object.definition_index);
                if (manager->m_graph_tag_index == graph_tag_index
                    || manager->reset_graph(graph_tag_index, object_definition->model.index, true)) {
                    c_animation_id manager_animation =
                        ((const c_model_animation_graph *)tag_get('jmad', manager->m_graph_tag_index))
                            ->find_animation(animation_name);
                    unit_animation_reset(object_index);
                    long goal_flags = 0x4001;
                    if (goal_flag)
                        goal_flags |= 2;
                    else
                        goal_flags &= ~2;
                    const c_model_animation *animation =
                        ((const c_model_animation_graph *)tag_get('jmad', manager->m_graph_tag_index))->get_animation(
                            manager_animation);
                    if (animation->animation_type == 0) {
                        custom_set = (manager)->set_custom_goal(manager_animation, goal_flags);
                        if (custom_set) {
                            if (!clear_custom)
                                *(unsigned short *)((char *)object + unit_byte_offset) |= 0x400;
                            else
                                *(unsigned short *)((char *)object + unit_byte_offset) &= ~0x400;
                        }
                    }
                    unit_animation_locate_constant_overlay(object_index);
                }
                if (custom_set) {
                    result = motor_task_new(object_index, _motor_resource_0, 0, 15, _motor_task_custom_animation, action);
                    manager->m_flags &= ~1;
                }
            }
        }
    }
    return result;
}


static __forceinline void motor_animation13_locate_auxilary_animations(c_animation_manager *manager)
{
    if (manager->m_mixing_board_type <= 2) {
        c_mixing_board_definition *system = (c_mixing_board_definition *)&g_mixing_board_definition_list[manager->m_mixing_board_type];
        if (system != 0 && system->locate_auxilary_animations != 0)
            system->locate_auxilary_animations(manager->m_object_index);
    }
}

static __forceinline c_motor_program_definition *motor_animation13_program_definition_get(long *state)
{
    if (*state != NONE)
        return g_program_definition[*state];
    return 0;
}


// @address 0x357e48 ?motor_animation_recommend_state_change_settings@@YA_NJJJJJMJJ_NPAUs_animation_state_change_settings@@@Z
bool motor_animation_recommend_state_change_settings(long object_index, long mode, long weapon_class, long weapon_type, long state,
    float interpolation_time, long flags, long playback_flags, bool force, s_animation_state_change_settings *settings)
{
    unit_datum *object = (unit_datum *)object_header_get(object_index)->datum;
    c_animation_manager *manager = (c_animation_manager *)((char *)object + object->object.animation.offset);
    long current_mode = manager->m_current_goal.mode;
    long current_state = manager->m_current_goal.state;
    bool result = false;
    if (((1 << object_header_get(object_index)->object_type) & 0x1003) != 0) {
        if (object->unit.player_index != NONE && !enable_player_transitions)
            flags |= 0x200;
    }
    if (mode == 226 && state == 226 && (playback_flags & 0x10) == 0 && !manager->state_channel_finished()) {
        result = true;
        manager->get_current_settings(settings);
        settings->interpolation_time = 0.0f;
    } else if (manager->recommend_state_change(mode, weapon_class, weapon_type, state, flags, playback_flags, settings)) {
        result = true;
        bool force_standard = !((current_mode == mode || mode == 226) && !force);
        settings->interpolation_time = unit_animation_compute_interpolation_time(false, force_standard, current_state, state, interpolation_time);
    }
    return result;
}

// @address 0x357fc0 ?motor_animation_execute_state_change_settings@@YA_NJPBUs_animation_state_change_settings@@_N@Z
bool motor_animation_execute_state_change_settings(long object_index, const s_animation_state_change_settings *requested, bool force)
{
    bool result = false;
    unit_datum *object = (unit_datum *)object_header_get(object_index)->datum;
    c_animation_manager *manager = (c_animation_manager *)((char *)object + object->object.animation.offset);
    long old_mode = manager->m_current_goal.mode;
    long old_state = manager->m_current_goal.state;
    s_animation_state_change_settings current;
    current.flags.clear();
    manager->get_current_settings(&current);
    s_state_changes_requested changed;
    changed.goals.clear();
    changed.transition.clear();
    changed.primary.clear();
    bool same = requested->compare_settings(current, &changed);
    if (!force && same)
        result = true;
    else if (changed.goals.is_empty() && changed.primary.get_unsafe() == 32) {
        manager->pause_state_channel(requested->primary.m_flags.test(_channel_settings_paused_bit));
        result = true;
    } else {
        long *motor = (long *)((char *)object + object->unit.motor_program.offset);
        c_motor_program_definition *program = motor_animation13_program_definition_get(motor);
        s_animation_state_change_settings desired;
        memcpy(&desired, requested, sizeof(desired));
        if (program == 0 || program->state_change == 0 || program->state_change(object_index, &desired)) {
            result = manager->execute_state_change(&desired);
            if (result) {
                bool mode_changed = (old_mode != desired.goals.mode && desired.goals.mode != 226) || force;
                bool state_changed = (old_state != desired.goals.state && desired.goals.state != 226) || force;
                if (mode_changed) {
                    motor_animation_update_impulses_on_mode_change(object_index);
                    if (((1 << object_header_get(object_index)->object_type) & 0x1003) != 0) {
                        unsigned short *unit_flags = (unsigned short *)((char *)object + object->unit.animation_storage.offset);
                        bool acceleration = animation_manager_has_acceleration_overlay(manager);
                        if (acceleration)
                            *unit_flags |= 2;
                        else
                            *unit_flags &= ~2;
                    }
                }
                if (mode_changed || state_changed) {
                    if (((1 << object_header_get(object_index)->object_type) & 0x1003) != 0)
                        motor_animation13_locate_auxilary_animations(manager);
                }
            }
        }
    }
    return result;
}

/* 0xdcd10 */
c_animation_channel_settings::c_animation_channel_settings()
{
    clear();
}

/* 0x357110 */



static __forceinline bool motor_animation_exit_test_bit(unsigned long flags, long bit)
{
    return ((flags >> bit) & 1) != 0;
}
static __forceinline real_vector3d *motor_animation_exit_add_vectors3d(const real_vector3d *a, const real_vector3d *b, real_vector3d *result)
{
    result->i = a->i + b->i;
    result->j = a->j + b->j;
    result->k = a->k + b->k;
    return result;
}
static __forceinline real_vector3d *motor_animation_exit_subtract_vectors3d(const real_vector3d *a, const real_vector3d *b, real_vector3d *result)
{
    result->i = a->i - b->i;
    result->j = a->j - b->j;
    result->k = a->k - b->k;
    return result;
}
static __forceinline unit_datum *motor_animation_exit_unit_get(long object_index)
{
    return (unit_datum *)object_header_get(object_index)->datum;
}

static __forceinline long motor_animation_exit_unit_inventory_get_weapon(long unit_index, short inventory_index)
{
    unit_datum *unit = motor_animation_exit_unit_get(unit_index);
    long weapon_index = NONE;
    if (inventory_index != NONE)
        weapon_index = unit->unit.weapon_object_indices[inventory_index];
    return weapon_index;
}

// @address 0x357110 motor_animation_exit_seat
extern "C" void motor_animation_exit_seat(long unit_index, long fast, long, long immediate, long *motor_program)
{
    unit_datum *unit = motor_animation_exit_unit_get(unit_index);
    bool vehicle = ((1 << unit->object.object_type) & 2) != 0;
    if (motor_program != NULL)
        *motor_program = -1;
    if (unit->object.parent_object_index == NONE || unit->unit.parent_seat_index == NONE)
        return;
    long parent_index = unit->object.parent_object_index;
    long seat_index = unit->unit.parent_seat_index;

    long desired_seat = NONE;
    long state = NONE;
    if (!vehicle) {
        desired_seat = unit_animation_get_desired_seat(unit_index, false, true);
        state = (unsigned char)fast ? 12 : 31;
    }
    if (!motor_animation_exit_test_bit(motor_animation_exit_unit_get(parent_index)->unit.unit_flags, 30))
        unit_seat_try_opening(parent_index, seat_index, false, false, true);
    unit->unit.last_vehicle_index = parent_index;
    unit->unit.last_vehicle_exit_time = game_time_globals->time;
    unit->unit.parent_seat_index = NONE;
    object_detach_immediate(unit_index);
    object_set_at_rest(unit_index, false);
    object_set_hidden(unit_index, false);

    real_vector3d linear_velocity;
    real_point3d center;
    real_vector3d early_mover_velocity;
    real_vector3d early_mover_angular_velocity;
    object_get_velocities(unit_index, &linear_velocity, NULL);
    object_get_center_of_mass(unit_index, &center);
    bool early_mover = object_get_early_mover_local_space_velocity(unit_index, &center, &early_mover_velocity, &early_mover_angular_velocity, true, false);
    if (early_mover) {
        motor_animation_exit_subtract_vectors3d(&linear_velocity, &early_mover_velocity, &linear_velocity);
    }
    limit3d(&linear_velocity, 5.0f);
    if (early_mover) {
        motor_animation_exit_add_vectors3d(&linear_velocity, &early_mover_velocity, &linear_velocity);
    }
    object_set_velocities(unit_index, &linear_velocity, NULL);
    simulation_action_object_update(unit_index, 0x8000);

    if (!vehicle) {
        if (motor_animation_exit_unit_inventory_get_weapon(unit_index, motor_animation_exit_unit_get(unit_index)->unit.current_weapon_index) == NONE) {
            unit->unit.desired_weapon_indices[0] = (unsigned char)unit_weapon_next_index(unit_index, desired_seat, NONE, 0);
            if ((unsigned char)immediate)
                unit_action_weapon_ready(unit_index, desired_seat, true, true, true);
            else
                unit_action_weapon_ready(unit_index, desired_seat, false, false, true);
        } else {
            unit_definition *definition =
                (unit_definition *)
                    tag_get('unit', motor_animation_exit_unit_get(parent_index)->object.definition_index);
            unit_seat *seat = &definition->seats[seat_index];
            if (motor_animation_exit_test_bit(seat->flags, 30)) {
                long best_time = 0x80000000;
                long best_weapon = NONE;
                for (long i = 0; i < 4; i++) {
                    long weapon_index = unit->unit.weapon_object_indices[i];
                    long last_used_time = unit->unit.weapon_ages[i];
                    if (weapon_index != NONE && ((item_datum *)object_header_get(weapon_index)->datum)->owner_relation != 1 &&
                        best_time < last_used_time)
                    {
                        best_time = last_used_time;
                        best_weapon = weapon_index;
                    }
                }
                if (best_weapon != NONE && weapon_can_be_stowed(best_weapon, unit_index) &&
                    ((item_datum *)object_header_get(best_weapon)->datum)->owner_relation != 3)
                {
                    item_show_inventory_unattached(best_weapon, unit_index);
                    weapon_stow(best_weapon, unit_index);
                }
            }
        }
        motor_animation_fix_pill(unit_index, desired_seat, state);
    }

    unit_update_driver_and_gunner(parent_index);
    if (unit_seat_test_flag(parent_index, seat_index, _unit_seat_driver_bit)) {
        s_damage_owner damage_owner;
        damage_compute_damage_owner_from_object_index(unit_index, &damage_owner);
        object_set_damage_owner(parent_index, &damage_owner, false);
    }
    if (unit->object.object_type == 0) {
        long vehicle_index = parent_index;
        long object_index = parent_index;
        while (object_index != NONE) {
            unit_datum *object = motor_animation_exit_unit_get(object_index);
            if (!motor_animation_exit_test_bit(object->object.object_flags, 26))
                break;
            object_index = object->object.parent_object_index;
        }
        if (object_index != NONE && ((1 << object_header_get(object_index)->object_type) & 2) != 0)
            vehicle_index = object_index;
        biped_exit_seat_end(unit_index, vehicle_index);
    }
    object_compute_node_matrices(unit_index);
    if (unit->unit.actor_index != NONE && ai_globals->enabled)
        actor_handle_seat_exit(unit->unit.actor_index, parent_index, seat_index, (e_motor_program *)motor_program);
    if (unit->unit.player_index != NONE)
        ai_player_handle_seat_exit(unit->unit.player_index, parent_index, seat_index);
    damage_handle_vehicle_exit(unit_index, parent_index);
    motor_enter_seat_unit_reset_seat_acceleration_data(unit_index);
}

// @address 0x357610 ?motor_animation_exit_seat_reset_velocity@@YAXJJJ_NJPAJ@Z
void motor_animation_exit_seat_reset_velocity(long object_index, long a, long b, bool c, long d, long *e)
{
    real_vector3d velocity, camera_velocity;
    velocity = *global_zero_vector3d;
    biped_datum *object = motor_animation_posture_object_get(object_index);
    camera_velocity = *global_zero_vector3d;
    if (e != NULL)
        *e = -1;
    if (((1 << object->object.object_type) & 1) != 0)
        biped_get_camera_velocities(object_index, &velocity, &camera_velocity);
    motor_animation_exit_seat(object_index, a, b, d, e);
    if (c) {
        limit3d(&velocity, 3.0f);
        object_set_velocities(object_index, &velocity, &camera_velocity);
    }
}


// @address 0x356fb8 motor_animation_try_and_exit_seat
extern "C" bool motor_animation_try_and_exit_seat(long unit_index, long state, bool *exited)
{
    bool result = false;
    unit_datum *unit = (unit_datum *)object_header_get(unit_index)->datum;
    if (exited)
        *exited = false;
    long parent_index = unit->object.parent_object_index;
    if (parent_index != NONE) {
        short seat_index = unit->unit.parent_seat_index;
        if (seat_index != NONE) {
            action_submit_event(unit_index, 47);
            if (unit->object.object_type == 1) {
                motor_animation_exit_seat_reset_velocity(unit_index, 0, 0, false, 0, NULL);
                result = true;
                if (exited)
                    *exited = true;
            } else if (motor_animation_set_mode_weapons_and_state(unit_index, 226, 226, 226, state, 0.0f, 2, 29, false)) {
                ::object_set_hidden(unit_index, false);
                simulation_action_unit_exit_vehicle(unit_index);
                game_engine_record_vehicle_exit(unit_index, parent_index);
                result = true;
            }
            if (result) {
                unit_datum *parent = (unit_datum *)object_header_get(parent_index)->datum;
                if (!(bool)(_rotl(parent->unit.unit_flags, 2) & 1))
                    unit_seat_try_opening(parent_index, seat_index, false, false, false);
            }
        }
    }
    return result;
}

typedef void (c_animation_channel_settings::*motor_animation_channel_settings_clear_member)();
motor_animation_channel_settings_clear_member motor_animation_channel_settings_clear_odr_use() { return &c_animation_channel_settings::clear; }

typedef s_unit_animation_storage s_motor_unit_animation_view;
static __forceinline s_motor_unit_animation_view *motor_animation_unit_view_get(unit_datum *unit)
{
    return (s_motor_unit_animation_view *)((char *)unit + unit->unit.animation_storage.offset);
}
inline bool s_animation_state_change_settings::changes_should_interpolate(const s_animation_state_change_settings &other) const
{
    c_goal_settings_comparison_flags goals_changed;
    goals_changed = goals.compare_goals(other.goals);
    c_flags<e_channel_settings_comparison_bits, unsigned long, 10> primary_changed;
    primary_changed = primary.compare_settings(other.primary);
    c_flags<e_channel_settings_comparison_bits, unsigned long, 10> transition_changed;
    transition_changed = transition.compare_settings(other.transition);
    return goals_changed.get_unsafe() != 0 || (primary_changed.get_unsafe() & 0x13) != 0 || (transition_changed.get_unsafe() & 0x13) != 0;
}

static __forceinline bool motor_animation_playback_flag_test(unsigned short flags, long bit) { return (flags & FLAG(bit)) != 0; }
// @address 0xdc840 ?motor_animation_build_state_change_settings_from_control_data@@YA_NJPBUs_animation_control_data@@PAUs_state_changes_requested@@PAUs_animation_state_change_settings@@@Z
bool motor_animation_build_state_change_settings_from_control_data(long object_index, const s_animation_control_data *control,
    s_state_changes_requested *state_changes, s_animation_state_change_settings *settings)
{
    unit_datum *object = (unit_datum *)object_get(object_index);
    c_animation_manager *manager = (c_animation_manager *)((char *)object + object->object.animation.offset);
    long mode = 226;
    long state = 226;
    bool built = false;
    s_animation_state_change_settings current;
    manager->get_current_settings(&current);
    memcpy(settings, &current, sizeof(current));
    if (control->mode != NONE && control->mode != 0)
        mode = control->mode;
    if (control->state != NONE && control->state != 0)
        state = control->state;
    if (object->object.parent_object_index == NONE) {
        if (manager->m_current_goal.mode == 0x103) {
            mode = 0x103;
        } else if ((_rotl(object->object.damage_flags, 30) & 1) != 0) {
            mode = 0x8a;
        } else if ((FLAG(object->object.object_type) & 0x1003) != 0) {
            long unit_mode = motor_animation_unit_view_get((unit_datum *)object_get(object_index))->desired_mode;
            if (unit_mode != NONE)
                mode = unit_mode;
        }
    }
    c_motor_task *task = (c_motor_task *)&motor_animation_tasks_get((unit_datum *)object_get(object_index))->tasks[0];
    const c_animation_channel *state_channel = manager->get_state_channel();
    bool channel_valid = state_channel->m_enabled != 0
        && state_channel->m_graph_index != NONE
        && (unsigned short)state_channel->m_animation_id.m_animation_index < 0x8000;
    bool settings_changed = false;
    bool build = false;
    long current_mode = manager->m_current_goal.mode;
    if (mode != current_mode && mode != 226)
        settings_changed = true;
    if (state != manager->m_current_goal.state && state != 226)
        settings_changed = true;
    if (const bool &playback_complete = state_channel->playback_complete()) {
        settings_changed = true;
    } else if (const bool &reached_last_frame = state_channel->reached_last_frame()) {
        const c_model_animation *animation = state_channel->get_dominant_animation();
        if (animation != 0
            && animation->get_loop_frame_index() == 0
            && !motor_animation_playback_flag_test(state_channel->m_playback_flags.m_storage, 5))
            settings_changed = true;
    }
    if (!task->valid()
        || !channel_valid
        || (settings_changed
            && task->program_default()
            && !motor_animation_resource_busy(object_index, _motor_resource_base)))
        build = true;
    if (build) {
        if (state == 0x341) {
            if ((unsigned short)settings->primary.m_animation_id.m_animation_index < 0x8000)
                settings->primary.m_flags.set(_channel_settings_paused_bit, true);
            if ((unsigned short)settings->transition.m_animation_id.m_animation_index < 0x8000)
                settings->transition.m_flags.set(_channel_settings_paused_bit, true);
        } else {
            long flags = control->flags;
            if ((FLAG(((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_index)].object_type) & 0x1003) != 0
                && object->unit.player_index != NONE
                && !enable_player_transitions)
                flags |= 0x200;
            s_animation_state_change_settings recommended;
            if (manager->recommend_state_change(mode, 226, 226, state, flags, control->playback_flags, &recommended)) {
                memcpy(settings, &recommended, sizeof(recommended));
                built = true;
            }
        }
    } else if (task->program_default()) {
        built = true;
    }
    if (built) {
        for (long i = 0; i < 2; i++) {
            if (control->secondary_states[i] != NONE) {
                c_animation_id animation = manager->find_goal_animation_id(
                    settings->goals.mode, settings->goals.weapon_class, settings->goals.weapon_type,
                    control->secondary_states[i], 0);
                if (animation.m_animation_index >= 0) {
                    settings->primary.m_blend_ids[i] = animation;
                    settings->primary.m_blend_ratios[i] = control->blend_ratios[i];
                }
            }
        }
    }
    if (current.changes_should_interpolate(*settings))
        settings->interpolation_time = 0.26666668f;
    else
        settings->interpolation_time = 0.0f;
    return current.compare_settings(*settings, state_changes) == 0;
}
/* 0xe8fe8: analog blend throttling */
// @address 0x73cc30 ?g_animation_throttle_dampening_scale@@3MA
float g_animation_throttle_dampening_scale = 0.5f;
// @address 0x8c443d ?g_force_unit_walking@@3_NA


static __forceinline bool motor_animation_channel_valid(const c_animation_channel *channel)
{
    return channel->m_enabled && channel->m_graph_index != NONE && channel->m_animation_id.m_animation_index >= 0;
}

static __forceinline unit_datum *motor_animation_analog_object_get(long object_index)
{
    return (unit_datum *)object_header_get(object_index)->datum;
}
static __forceinline bool motor_animation_analog_has_manager(long object_index)
{
    unit_datum *object = motor_animation_analog_object_get(object_index);
    return (unsigned short)object->object.animation.offset != 0xffff;
}
static __forceinline void *motor_animation_analog_header_block_get(long object_index, const s_object_block_reference *reference)
{
    return (char *)motor_animation_analog_object_get(object_index) + reference->offset;
}
static __forceinline bool motor_animation_analog_update_desired_velocity(
    c_animation_manager *manager, long object_index, const real_vector3d *velocity, bool force_walking)
{
    s_motor_animation_analog_channels *channels = (s_motor_animation_analog_channels *)manager;
    bool result = false;
    if (motor_animation_channel_valid(&channels->transition))
        result = animation_channel_update_blend_from_velocity(&channels->transition, velocity, 0.0f, force_walking);
    else
        result = animation_channel_update_blend_from_velocity(&channels->state, velocity, 0.0f, force_walking);
    return result;
}
static __forceinline float motor_animation_analog_property_scale(const character_movement_properties *properties)
{
    float result = 0.0f;
    if (properties)
        result = properties->animation_throttle_scale;
    return result;
}
static __forceinline real_vector3d *motor_animation_analog_subtract_vectors3d(const real_vector3d *a, const real_vector3d *b, real_vector3d *result)
{
    result->i = a->i - b->i;
    result->j = a->j - b->j;
    result->k = a->k - b->k;
    return result;
}
static __forceinline real_vector3d *motor_animation_analog_point_from_line3d(
    const real_vector3d *point, const real_vector3d *vector, float t, real_vector3d *result)
{
    result->i = point->i + vector->i * t;
    result->j = point->j + vector->j * t;
    result->k = point->k + vector->k * t;
    return result;
}

// @address 0xe8fe8 ?motor_animation_update_analog_blending@@YAXJ@Z
void motor_animation_update_analog_blending(long object_index)
{
    unit_datum *object = motor_animation_analog_object_get(object_index);
    if (!motor_animation_analog_has_manager(object_index))
        return;
    bool use_velocity = true;
    c_animation_manager *manager = (c_animation_manager *)motor_animation_analog_header_block_get(object_index, &object->object.animation);
    s_motor_animation_analog_channels *channels = (s_motor_animation_analog_channels *)manager;
    if ((1 << object->object.identifier.get_type()) & 0x1003) {
        use_velocity = false;
        if ((1 << object->object.identifier.get_type()) & 1) {
            const biped_definition *definition =
                (const biped_definition *)tag_get('bipd', object->object.definition_index);
            use_velocity = (definition->physics.flags >> 2) & 1;
        }
        if (!use_velocity) {
            float throttle_scale = 0.0f;
            if (object->unit.actor_index != NONE) {
                actor_datum *actor = datum_get_typed<actor_datum>(actor_data, object->unit.actor_index);
                character_movement_properties *properties =
                    character_definition_movement_properties_get(actor->character_index);
                throttle_scale = motor_animation_analog_property_scale(properties);
            }

            real_vector3d target = object->unit.throttle;
            if (!motor_animation_channel_valid(&channels->transition)) {
                g_animation_throttle_dampening_scale = PIN(g_animation_throttle_dampening_scale, 0.0f, 1.0f);
                real_vector3d difference;
                motor_animation_analog_subtract_vectors3d(&target, &object->unit.analog_blending_throttle, &difference);
                motor_animation_analog_point_from_line3d(&object->unit.analog_blending_throttle, &difference,
                    1.0f - g_animation_throttle_dampening_scale, &object->unit.analog_blending_throttle);
            } else {
                object->unit.analog_blending_throttle = target;
            }
            animation_manager_update_analog_blending(manager, object_index, &object->unit.analog_blending_throttle, throttle_scale,
                (object->unit.control_flags >> 17 & 1) | g_force_unit_walking);
        }
    }

    if (use_velocity) {
        real_vector3d velocity = object->object.linear_velocity;
        real_matrix4x3 matrix;
        matrix4x3_rotation_from_vectors(&matrix, &object->object.forward, &object->object.up);
        real_vector3d local_velocity;
        matrix4x3_inverse_transform_vector(&matrix, &velocity, &local_velocity);
        motor_animation_analog_update_desired_velocity(manager, object_index, &local_velocity, g_force_unit_walking);
    }
}
