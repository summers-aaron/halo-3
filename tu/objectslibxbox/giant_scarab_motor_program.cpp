// objectslibxbox/giant_scarab_motor_program.obj: from the records 0xdc250 0x3932f0 0x393350 0x393460 0x393740 0x393968 0x393a50 0x393b70 0x3944e8 0x394980
// @flags /TP /O2 /Oc
#include "animations/animation_id.h"
#include "animations/animation_state_change_settings.h"
#include "memory/tls_globals.h"
#include "objects/object_header.h"
#include "tag_files/tag_groups.h"
#include "game/game_globals_storage.h"
#include "objects/object_datum.h"
#include "units/motor_program_state.h"
#include "animations/animation_control_data.h"
// @address 0x3932f0 ?giant_scarab_activate@@YA_NJ@Z
// @address 0x393350 giant_scarab_begin_buckle_animation
// @address 0x393968 giant_scarab_reset_animation
// @address 0x2fd460 ?snap_to_position@c_mixing_board_slider@@QAAXM@Z
// @address 0xdc250 ?set_defaults@s_animation_control_data@@QAAXXZ
// @address 0x79d458 ?g_mixing_board_definition_list@@3PAVc_mixing_board_definition@@A
#include "cseries/cseries.h"
#include "math/real_math.h"
#include "motor/motor_animation.h"
#include "objects/objects.h"
#include "objects/damage.h"
#include "units/giants.h"
#include "motor/giant_scarab_motor_program.h"
#include "motor/state_change_flag_bits_types.h"
#include "units/slider_movement_pattern_types.h"
#include "motor/mixing_board_component_types.h"
#include "units/slider_target_position_types.h"
#include "animations/mixing_board/mixing_board_definition.h"
#include "animations/mixing_board/mixing_board_slider_types.h"
#include "animations/animation_manager.h"
#include "units/motor_task.h"
#include "units/unit_types.h"
#include "units/giant_datum.h"
#include "animations/mixing_board/animation_manager_mixing_board.h"
#include "units/giant_definitions.h"
#include "tag_files/string_ids.h"

struct s_giant_scarab_motor_state : c_motor_program_state
{
    bool active;
};

struct s_giant_locomotion_cycle
{
    long state;
    long direction;
    long idle_state;
    float movement_weight;
    float idle_weight;
    long mode;
    long weapon;
    long unused;
};
struct s_giant_locomotion_tag_instance
{
    long group;
    s_giant_definition *definition;
};

bool giant_scarab_activate(long object_index)
{
    unit_datum *giant = (unit_datum *)object_get_and_verify_type(object_index, NONE);
    s_giant_scarab_motor_state *motor = (s_giant_scarab_motor_state *)((char *)giant + giant->unit.motor_program.offset);
    giant_setup_foot_tracking_data(object_index);
    motor->active = false;
    return true;
}

template float t_short_fixed<13>::export_real() const;

// @address 0x79d7dc k_mixing_board_slider_targets

// @address 0xdc250 ?set_defaults@s_animation_control_data@@QAAXXZ
// match: 0x393350: inline lets giant_scarab_begin_buckle_animation inline set_defaults; out of line the r30/r31 saves and 0x70 frame drop to 0x60
#if defined(_XBOX) && !defined(__clang__)
inline void s_animation_control_data::set_defaults()
#else
void s_animation_control_data::set_defaults()
#endif
{
    mode = _string_id_current;
    state = _string_id_idle;
    moved = false;
    crouch = false;
    flags = 0;
    playback_flags = 29;
    for (int i = 0; i < 2; i++)
    {
        secondary_states[i] = -1;
        blend_ratios[i] = 0.0f;
    }
}

static __forceinline s_giant_datum *giant_scarab_motor_get(long object_index)
{
    return (s_giant_datum *)object_get_and_verify_type(object_index, NONE);
}

extern "C" void giant_scarab_reset_animation(long object_index, s_animation_control_data *control)
{
    s_giant_datum *giant = (s_giant_datum *)object_get_and_verify_type(object_index, NONE);
    c_animation_manager *unit = (c_animation_manager *)((char *)giant + giant->object.animation.offset);
    unit->get_mixing_board_slider(6)->snap_to_position(1.0f);
    giant->motor_flags.clear(1);
    giant->buckling_state = k_giant_buckle_state_none;
    (control)->set_defaults();
    control->state = _string_id_idle;
    control->flags = 784;
    control->playback_flags = 31;
    ::object_damage_handle_resurrection(object_index);
}

extern "C" bool giant_scarab_begin_buckle_animation(long object_index, s_animation_control_data *control_data)
{
    bool result = false;
    s_giant_datum *giant = giant_scarab_motor_get(object_index);
    s_giant_scarab_anim *anim = (s_giant_scarab_anim *)&giant->variant_index;
    if (anim->count == 0 && giant->motor_flags.test(0))
    {
        giant->motor_flags.clear(0);
        anim->index = 0;
        if (giant->motor_flags.test(3))
            anim->index = 2;
        else if (giant->motor_flags.test(2))
            anim->index = 1;
        s_giant_datum *giant2 = giant_scarab_motor_get(object_index);
        TAG_POINTER(s_giant_variant) *const &anims_addr =
            &TAG_GET('gint', s_giant_definition, giant2->object.definition_index)->variants;
        s_giant_variant *entry = &(*anims_addr)[giant2->variant_index];
        if (entry)
        {
            (control_data)->set_defaults();
            result = true;
            control_data->state = entry->animation;
            control_data->flags = 784;
            control_data->playback_flags = 80;
        }
    }
    return result;
}

static __forceinline s_giant_definition *giant_locomotion_tag_definition(long index)
{
    return TAG_GET('gint', s_giant_definition, index);
}
static __forceinline bool giant_locomotion_is_locomotion(long state)
{
    bool result = false;
    switch (state)
    {
    case _string_id_move_front:
    case _string_id_move_back:
    case _string_id_move_left:
    case _string_id_move_right:
    case _string_id_turn_left_45:
    case _string_id_turn_right_45:
    case _string_id_move_front_turn_left_45:
    case _string_id_move_front_turn_right_45:
    case _string_id_move_back_turn_left_45:
    case _string_id_move_back_turn_right_45:
    case _string_id_move_left_turn_left_45:
    case _string_id_move_left_turn_right_45:
    case _string_id_move_right_turn_left_45:
    case _string_id_move_right_turn_right_45:
        result = true;
        break;
    default:
        result = false;
        break;
    }
    return result;
}
static __forceinline bool giant_locomotion_use_channel(c_animation_manager *m)
{
    bool use_channel = true;
    bool channel_ready = m->m_state_channel.m_enabled && m->m_state_channel.m_graph_index != -1
        && (unsigned short)m->m_state_channel.m_animation_id.m_animation_index < 0x8000;
    if (channel_ready)
        use_channel = ((_rotl(m->m_state_channel.m_state_flags.get_unsafe(), 31) & 1) != 0);
    return use_channel;
}
static __forceinline bool giant_locomotion_channel_valid(const c_animation_manager *m)
{
    return m->m_state_channel.m_enabled && m->m_state_channel.m_graph_index != -1 && m->m_state_channel.m_animation_id.m_animation_index >= 0;
}
static __forceinline float giant_locomotion_authored_duration(const c_animation_manager *m)
{
    float result = 0.0f;
    if (giant_locomotion_channel_valid(m))
        result = ((const c_channel_base *)m)->get_authored_duration();
    return result;
}

// @address 0x393740 ?giant_update_movement_speed@@YA_NJ@Z
bool giant_update_movement_speed(long object_index)
{
    s_giant_datum *g = giant_scarab_motor_get(object_index);
    s_giant_definition *definition = giant_locomotion_tag_definition(g->object.definition_index);
    c_animation_manager *m = (c_animation_manager *)((char *)g + g->object.animation.offset);
    c_mixing_board_slider *s = m->get_mixing_board_slider(5);
    long state = m->m_current_goal.state;
    bool result = false;
    if (giant_locomotion_is_locomotion(state) && !((const c_animation_manager *)m)->state_channel_finished())
    {
        result = true;
        s_giant_locomotion_cycle c;
        if (giant_build_locomotion_cycle(object_index, &c) && c.state == state && !(g->giant_flags & 1))
        {
            if (s->m_target_position != 3)
            {
                float time = MAX(definition->accelerate, 0.0001f);
                s->move_to_target(_slider_target_position_full, _slider_movement_pattern_none, time);
            }
        }
        else if (s->m_target_position != 1)
        {
            float remaining = giant_locomotion_authored_duration(m) - ((const c_animation_manager *)m)->get_authored_state_time();
            float time = MAX(definition->decelerate, 0.0001f);
            if (remaining <= time)
            {
                time = MAX(remaining, 0.001f);
                s->move_to_target(_slider_target_position_one, _slider_movement_pattern_none, time);
            }
        }
    }
    return result;
}

static __forceinline real_vector2d *giant_cycle_scale(const real_vector2d *v, float scale, real_vector2d *result)
{
    result->i = v->i * scale;
    result->j = v->j * scale;
    return result;
}
static __forceinline float giant_cycle_normalize(real_vector2d *v)
{
    float length = sqrtf(magnitude_squared2d(v));
    if (fabsf(length - 0.0f) < 0.0001f)
        length = 0.0f;
    else
        giant_cycle_scale(v, 1.0f / length, v);
    return length;
}
// @address 0x393460 ?giant_build_locomotion_cycle@@YA_NJPAUs_giant_locomotion_cycle@@@Z
bool giant_build_locomotion_cycle(long object_index, s_giant_locomotion_cycle *cycle)
{
    unit_datum *giant = (unit_datum *)object_get_and_verify_type(object_index, NONE);
    bool result = false;
    long state = _string_id_move_front, right = 838, left = 837;
    float movement = 0.0f, turning = 0.0f;
    long primary = _string_id_turn_right_45, secondary = 838;
    float forward_amount = MAX(fabsf(giant->unit.throttle.i), 0.0f);
    float side_amount = MAX(fabsf(giant->unit.throttle.j), 0.0f);
    if (forward_amount >= side_amount)
    {
        movement = forward_amount;
        if (giant->unit.throttle.i >= 0.0f) { state = _string_id_move_front; right = 838; left = 837; }
        else { state = _string_id_move_back; right = 840; left = 839; }
    }
    else
    {
        movement = side_amount;
        if (giant->unit.throttle.j <= 0.0f) { state = _string_id_move_right; right = 844; left = 843; }
        else { state = _string_id_move_left; right = 842; left = 841; }
    }
    real_vector2d forward = { giant->object.forward.i, giant->object.forward.j };
    real_vector2d aiming = { giant->unit.desired_facing_vector.i, giant->unit.desired_facing_vector.j };
    giant_cycle_normalize(&forward);
    giant_cycle_normalize(&aiming);
    float angle = signed_angle_between_vectors2d(&forward, &aiming) * 57.29578f;
    angle = PIN(angle, -45.0f, 45.0f);
    turning = MAX(fabsf(angle / 45.0f), 0.0f);
    if (angle > 0.0f) { primary = _string_id_turn_left_45; secondary = left; }
    else { primary = _string_id_turn_right_45; secondary = right; }
    float threshold = 0.8f;
    if (giant->unit.control_flags & 8) threshold = 0.35f;
    if (movement >= 0.2f || turning >= threshold)
    {
        real_vector3d weights;
        if ((1.0f - movement) + turning <= 1.0f)
        {
            primary = state;
            weights.j = 0.5f * turning;
            weights.k = 0.5f * MAX(1.0f - movement, 0.0f);
            weights.i = MAX(0.5f - weights.j - weights.k, 0.0f);
        }
        else
        {
            weights.j = 0.5f * movement;
            weights.k = 0.5f * MAX(1.0f - turning, 0.0f);
            weights.i = MAX(0.5f - weights.j - weights.k, 0.0f);
        }
        float total = weights.i + weights.j + weights.k;
        if (total > 0.0f)
            scale_vector3d(&weights, 1.0f / total, &weights);
        cycle->movement_weight = weights.j;
        cycle->idle_weight = weights.k;
        cycle->state = primary;
        cycle->direction = secondary;
        cycle->mode = 784;
        cycle->weapon = 31;
        cycle->idle_state = 834;
        result = true;
    }
    return result;
}

// @address 0x393a50 giant_scarab_update_animation_from_slider
extern "C" void giant_scarab_update_animation_from_slider(
    long object_index, s_giant_datum *motor, t_short_fixed<13> *weight, c_animation_manager *manager, bool suppress_events)
{
    c_animation_channel *channel = &manager->m_state_channel;
    t_short_fixed<13> weight_value = *weight;
    float playback_ratio = motor->playback_rate_scale * weight_value.export_real();
    if (suppress_events)
    {
        if (channel->valid())
        {
            channel->set_frame_position_from_playback_ratio(playback_ratio);
            return;
        }
    }
    else if (channel->valid())
    {
        channel->update_to_playback_ratio(playback_ratio, manager, object_animation_callback, object_index);
    }
}

static __forceinline short giant_scarab_game_difficulty_level_get()
{
    return tls_game_globals()->options.game_mode == 1 ? tls_game_globals()->options.campaign_difficulty_level : 1;
}

static __forceinline s_giant_variant *giant_scarab_get_variant(long object_index)
{
    s_giant_datum *giant = giant_scarab_motor_get(object_index);
    s_giant_definition *definition = TAG_GET('gint', s_giant_definition, giant->object.definition_index);
    TAG_POINTER(s_giant_variant) *const &animations = &definition->variants;
    return &(*animations)[giant->variant_index];
}

// @address 0x393b70 ?giant_update_buckling_state@@YA_NJPAUs_animation_control_data@@@Z
bool giant_update_buckling_state(long object_index, s_animation_control_data *control_data)
{
    bool result = false;
    s_giant_datum *giant = giant_scarab_motor_get(object_index);
    if (giant->motor_flags.test(3) && giant->variant_index != 2)
    {
        giant->motor_flags.clear(1);
        giant->buckling_state = k_giant_buckle_state_none;
        if (!giant_scarab_begin_buckle_animation(object_index, control_data))
            giant_scarab_reset_animation(object_index, control_data);
        object_start_interpolation(object_index, 0.5f);
        result = true;
    }

    s_giant_variant *variant = giant_scarab_get_variant(object_index);
    if (variant)
    {
        c_animation_manager *manager = (c_animation_manager *)((char *)giant + giant->object.animation.offset);
        long state_name = manager->get_state_name();
        bool settled = fabsf(variant->settle_scale) >= 0.0001f;
        if (state_name == variant->animation)
        {
            short difficulty = PIN(giant_scarab_game_difficulty_level_get(), 0, 3);
            float buckle_duration = variant->buckle_duration;
            float rise_duration = variant->rise_duration;
            float buckled_duration = variant->buckled_durations[difficulty];
            c_mixing_board_slider *slider = manager->get_mixing_board_slider(6);
            result = true;
            control_data->state = _string_id_current;
            if (giant->buckling_state == k_giant_buckle_state_none)
            {
                giant->buckling_state = k_giant_buckle_state_descending;
                giant_setup_buckling_parameters(object_index);
                slider->snap_to_target(_slider_target_position_one);
                if (!(fabsf(giant->buckle_position) < k_real_epsilon))
                    slider->snap_to_position(giant->buckle_position);
                if (!settled)
                    slider->move_to_target(_slider_target_position_full, (e_slider_movement_pattern)variant->buckle_movement_pattern, buckle_duration);
                long impulse = variant->impulse;
                if (impulse != -1 && variant->position != 0.0f)
                {
                    c_animation_channel *channel = manager->get_mixing_board_animation_channel(3);
                    if (!channel->valid() || !channel->playing() || channel->m_definition_index != impulse)
                        manager->setup_animation_channel_by_parallel_impulse(channel, _string_id_current, _string_id_current, impulse, 0x401f);
                    c_mixing_board_slider *impulse_slider = manager->get_mixing_board_slider(7);
                    impulse_slider->snap_to_target(_slider_target_position_full);
                    impulse_slider->move_to_target(_slider_target_position_one, _slider_movement_pattern_ease_in, buckle_duration);
                    manager->get_mixing_board_slider(8)->snap_to_position(variant->position);
                    giant_scarab_update_animation_from_slider(object_index, (s_giant_datum *)giant, &slider->m_position, manager, true);
                }
            }
            else if (giant->buckling_state == k_giant_buckle_state_descending)
            {
                bool finished = !slider->active();
                if (settled)
                    finished = fabsf(slider->get_value().export_real() - 1.0f) < 0.0001f;
                if (!finished)
                {
                    if (settled)
                    {
                        float tick_length = tls_game_time_globals()->tick_length;
                        float slider_value = slider->get_value().export_real();
                        float position = slider_value + giant->buckle_velocity * tick_length;
                        giant->buckle_velocity += giant->buckle_acceleration * tick_length;
                        position = PIN(position, 0.0f, 1.0f);
                        slider->snap_to_position(position);
                    }
                    giant_scarab_update_animation_from_slider(object_index, (s_giant_datum *)giant, &slider->m_position, manager, false);
                }
                else
                {
                    giant->buckling_state = k_giant_buckle_state_buckled;
                    giant->buckle_timer = buckled_duration;
                    giant_scarab_update_animation_from_slider(object_index, (s_giant_datum *)giant, &slider->m_position, manager, false);
                    long impulse = variant->buckled_impulse;
                    if (impulse != -1 && variant->buckled_position != 0.0f)
                    {
                        c_animation_channel *channel = manager->get_mixing_board_animation_channel(3);
                        if (!channel->valid() || !channel->playing() || channel->m_definition_index != impulse)
                            manager->setup_animation_channel_by_parallel_impulse(channel, _string_id_current, _string_id_current, impulse, 0x401f);
                        c_mixing_board_slider *impulse_slider = manager->get_mixing_board_slider(7);
                        impulse_slider->snap_to_target(_slider_target_position_full);
                        impulse_slider->move_to_target(_slider_target_position_one, _slider_movement_pattern_ease_in, buckled_duration);
                        manager->get_mixing_board_slider(8)->snap_to_position(variant->buckled_position);
                    }
                    else
                    {
                        manager->get_mixing_board_animation_channel(3)->reset();
                    }
                }
            }
            else if (giant->buckling_state == k_giant_buckle_state_buckled)
            {
                if (slider->active())
                    object_start_interpolation(object_index, 0.5f);
                if (giant->variant_index == 2)
                {
                    giant->buckling_state = k_giant_buckle_state_finished;
                    manager->get_mixing_board_animation_channel(3)->reset();
                }
                else if (!((bool)(_rotl(giant->unit.unit_flags, 10) & 1)))
                {
                    giant->buckle_timer -= tls_game_time_globals()->tick_length;
                    if (giant->buckle_timer < 0.0f)
                    {
                        giant->buckle_timer = 0.0f;
                        slider->move_to_target(_slider_target_position_one, (e_slider_movement_pattern)variant->rise_movement_pattern, rise_duration);
                        giant->buckling_state = k_giant_buckle_state_ascending;
                        manager->get_mixing_board_animation_channel(3)->reset();
                    }
                }
            }
            else if (giant->buckling_state == k_giant_buckle_state_ascending)
            {
                if (slider->active())
                    giant_scarab_update_animation_from_slider(object_index, (s_giant_datum *)giant, &slider->m_position, manager, false);
                else
                    giant_scarab_reset_animation(object_index, control_data);
            }
            else
            {
                result = false;
                ((c_animation_manager *)manager)->m_flags |= FLAG(0);
                giant->motor_flags.set(6);
            }
        }
    }
    return result;
}

// @address 0x394980 ?giant_scarab_update@@YA_NJPAUs_animation_control_data@@@Z
bool giant_scarab_update(long object_index, s_animation_control_data *control_data)
{
    s_animation_control_data control;
    control.set_defaults();
    s_giant_datum *giant = (s_giant_datum *)object_get_and_verify_type(object_index, NONE);
    s_giant_definition *definition = TAG_GET('gint', s_giant_definition, giant->object.definition_index);
    giant_update_animation_desires(object_index, &control);
    c_animation_manager *unit = (c_animation_manager *)((char *)giant + giant->object.animation.offset);
    c_mixing_board_slider *slider = unit->get_mixing_board_slider(5);
    float position = slider->get_value().export_real();
    float playback_rate = position * (1.0f - definition->minimum_playback_rate) + definition->minimum_playback_rate;
    bool channel_valid = unit->m_state_channel.m_enabled != 0 && unit->m_state_channel.m_graph_index != -1
        && (unsigned short)unit->m_state_channel.m_animation_id.m_animation_index < 0x8000;
    if (channel_valid)
        unit->m_state_channel.m_playback_speed = MAX(playback_rate, 0.0001f);
    bool result = giant_handle_motor_update(object_index, true);
    if (control.mode != _string_id_current || control.state != _string_id_current)
    {
        s_animation_state_change_settings settings;
        s_state_changes_requested requested;
        requested.goals.clear();
        requested.transition.clear();
        requested.primary.clear();
        motor_animation_build_state_change_settings_from_control_data(object_index, &control, &requested, &settings);
        result = (0 != motor_task_execute_state_change_settings(object_index, _motor_task_default, _action_none, &settings, 1, control.moved)) | result;
    }
    return result;
}

// @address 0x3944e8 ?giant_update_animation_desires@@YA_NJPAUs_animation_control_data@@@Z
bool giant_update_animation_desires(long object_index, s_animation_control_data *control_data)
{
    long locomotion_flags = 784;
    long locomotion_count = 31;
    control_data->set_defaults();
    control_data->state = _string_id_current;
    control_data->flags = locomotion_flags;
    control_data->playback_flags = locomotion_count;
    s_giant_datum *giant = giant_scarab_motor_get(object_index);
    s_giant_definition *definition = giant_locomotion_tag_definition(giant->object.definition_index);
    c_animation_manager *manager = (c_animation_manager *)((char *)giant + giant->object.animation.offset);
    c_mixing_board_slider *slider = manager->get_mixing_board_slider(5);
    bool result;
    if (manager->m_current_goal.state != _string_id_idle
        && ((!giant_locomotion_use_channel(manager) && !((const c_animation_manager *)manager)->state_channel_finished())
            || (unsigned short)giant->buckling_state != 0))
    {
        bool moving = giant_update_movement_speed(object_index);
        result = (bool)(moving | giant_update_buckling_state(object_index, control_data));
    }
    else
    {
        control_data->set_defaults();
        control_data->state = _string_id_idle;
        control_data->flags = locomotion_flags;
        control_data->playback_flags = locomotion_count;
        {
            bool filled = false;
            if (!(giant->active_interpolation_time > 0.0f) && !giant_scarab_begin_buckle_animation(object_index, control_data)
                && (unsigned short)giant->buckling_state == 0)
            {
                s_giant_locomotion_cycle cycle;
                if (giant_build_locomotion_cycle(object_index, &cycle))
                {
                    control_data->blend_ratios[0] = cycle.movement_weight;
                    control_data->blend_ratios[1] = cycle.idle_weight;
                    control_data->flags = cycle.mode;
                    control_data->playback_flags = cycle.weapon;
                    control_data->state = cycle.state;
                    control_data->secondary_states[0] = cycle.direction;
                    control_data->secondary_states[1] = cycle.idle_state;
                    filled = true;
                }
            }
            if (filled)
            {
                float time = MAX(definition->accelerate, 0.0001f);
                slider->move_to_target(_slider_target_position_full, _slider_movement_pattern_none, time);
            }
            else
            {
                slider->snap_to_target(_slider_target_position_full);
            }
        }
        result = true;
    }
    s_giant_datum *giant2 = giant_scarab_motor_get(object_index);
    if (((giant2->giant_flags & 1) != 0) && (((_rotl(giant->giant_flags, 30) & 1) == 0)))
    {
        c_animation_channel *channel = ((const c_animation_manager *)manager)->get_mixing_board_animation_channel(3);
        bool channel_ready = channel->m_enabled != 0 && channel->m_graph_index != -1 && (unsigned short)channel->m_animation_id.m_animation_index < 0x8000;
        if (!channel_ready || !channel->playing())
        {
            s_giant_definition *scarab_definition = TAG_GET('gint', s_giant_definition, giant2->object.definition_index);
            TAG_POINTER(s_giant_variant) *const &animations = &scarab_definition->variants;
            s_giant_variant *entry = &(*animations)[giant2->variant_index];
            if (entry != 0)
            {
                long impulse = entry->impulse;
                if (impulse != -1 && !(entry->position <= 0.0f) &&
                    ((const c_animation_manager *)manager)->setup_animation_channel_by_parallel_impulse(
                        channel, _string_id_current, _string_id_current, impulse, 0x401f))
                {
                    float authored = 0.0f;
                    bool channel_valid = manager->m_state_channel.m_enabled && manager->m_state_channel.m_graph_index != -1
                        && (unsigned short)manager->m_state_channel.m_animation_id.m_animation_index < 0x8000;
                    if (channel_valid)
                        authored = ((const c_channel_base *)manager)->get_authored_duration();
                    float state_time = ((const c_animation_manager *)manager)->get_authored_state_time();
                    float remaining = authored - state_time;
                    c_mixing_board_slider *duration_slider = manager->get_mixing_board_slider(7);
                    duration_slider->snap_to_target(_slider_target_position_one);
                    duration_slider->move_to_target(_slider_target_position_full, _slider_movement_pattern_ease_in, remaining);
                    manager->get_mixing_board_slider(8)->snap_to_position(entry->position);
                }
            }
        }
    }
    return result;
}
