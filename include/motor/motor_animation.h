#ifndef MOTOR_MOTOR_ANIMATION_H
#define MOTOR_MOTOR_ANIMATION_H

#include "units/motor_enums.h"

// Functions defined in tu/objectslibxbox/motor_animation.cpp.

struct s_animation_control_data;
struct s_animation_state_change_settings;
struct s_state_changes_requested;
// Analog blending controls.
extern float g_animation_throttle_dampening_scale;
extern bool g_force_unit_walking;

bool motor_animation_test_mode(long object_index, long mode, long weapon_class, long weapon_type);
bool motor_animation_test_mode(long object_index, long mode_name);
bool motor_animation_running(long object_index, long impulse_name);
float motor_animation_base_channel_time(long object_index, float *authored_duration);
bool motor_animation_get_range(long object_index, long impulse, long weapon_type, short component,
    bool allow_default_mode, float *total_frames, short *total_ticks, float *key_frame, short *key_ticks);
bool motor_animation_test_mode_and_impulse(long object_index, long mode_name, long weapon_class_name, long weapon_type_name, long impulse_name);
bool motor_animation_validate_animation(long object_index, long mode, long impulse, bool *used_default_mode);
bool motor_animation_busy(long object_index);
void motor_task_interrupt(long object_index, short resource_mask, short allowed_resources);
bool motor_animation_set_mode_weapons_and_state(long object_index, long mode, long weapon_class, long weapon_type,
    long state, float rate, long unknown, long flags, bool ignore_allow);
long motor_animation_default_ground_mode(long object_index);
void motor_animation_posture_exit_end(long object_index, long mode, long exit_name, long exit_name2, long posture);
void motor_animation_unit_entry_finished(long unit_index, long);
bool motor_task_enter_seat_internal(long unit_index, long vehicle_index, short seat_index, bool skip_entrance_animation,
    long state, bool *finished_out, bool evict_occupant);
bool motor_animation_build_state_change_settings_from_control_data(long object_index,
    const s_animation_control_data *control, s_state_changes_requested *state_changes,
    s_animation_state_change_settings *settings);
void motor_animation_update_analog_blending(long object_index);

union real_point3d;
union real_vector3d;

extern "C" bool motor_animation_set_posture(long object_index, long state, long mode, bool physics,
    const real_point3d *position, const real_vector3d *forward, bool *interruptible);
void motor_animation_exit_seat_reset_velocity(long object_index, long a, long b, bool c, long d, long *e);
extern "C" bool motor_animation_try_and_exit_seat(long unit_index, long state, bool *exited);

class c_animation_channel;
class c_motor_task;

c_motor_task *motor_task_set_ping(long object_index, e_motor_task motor_task, e_action action, e_motor_resource motor_resource, long state,
    long mode, long damage, short desired_direction, short desired_region, short *actual_direction, short *actual_region);

c_animation_channel *motor_animation_task_get_channel(long object_index, c_motor_task *task);
extern "C" void motor_interrupability(unsigned int unit_index, bool *current_empty, bool *valid);
bool motor_animation_recommend_state_change_settings(long object_index, long mode, long weapon_class, long weapon_type,
    long state, float interpolation_time, long flags, long playback_flags, bool force,
    s_animation_state_change_settings *settings);
bool motor_animation_execute_state_change_settings(long object_index, const s_animation_state_change_settings *requested, bool force);
bool motor_animation_get_mode_and_state(long object_index, long *current_mode, long *current_state);
bool motor_animation_action_loops(long object_index, e_motor_resource resource);
c_animation_channel *motor_animation_resource_get_channel(long object_index, e_motor_resource resource);
bool motor_animation_resource_busy(long object_index, e_motor_resource resource);
bool motor_task_allow(long object_index, e_action action, short resource_mask);
c_motor_task *motor_task_new(long object_index, e_motor_resource resource_type, long retained_resources,
    long resource_mask, e_motor_task task_type, e_action action);
bool motor_animation_task_busy(long object_index, c_motor_task *task, e_motor_resource resource, bool first, bool second);
c_motor_task *motor_animation_get_resource_owner(long object_index, e_motor_resource resource);
c_motor_task *motor_task_find_by_action(long object_index, e_action action);
c_motor_task *motor_task_set_posture(long object_index, e_motor_task motor_task, e_action action, long mode, long state,
    bool unknown, const real_point3d *position, const real_vector3d *forward, short resource_mask, bool ignore_allow,
    bool *interruptible);
c_motor_task *motor_task_set_mode_and_state(long object_index, e_motor_task motor_task, e_action action, long mode,
    long state, float rate, short resource_mask, long unknown, long flags, bool ignore_allow);
enum e_unit_weapon_config;
c_motor_task *motor_task_start_animation(long object_index, e_motor_task motor_task, e_action action, long impulse,
    long mode, e_unit_weapon_config unit_weapon_config, short resource_mask, float interpolation_time,
    bool ignore_allow);
bool motor_task_recursive_start_animation(long object_index, e_motor_task motor_task, e_action action, long impulse, bool ignore_allow);
c_motor_task *motor_task_execute_state_change_settings(long object_index, e_motor_task motor_task, e_action action,
    const s_animation_state_change_settings *settings, short resource_mask, bool ignore_allow);
extern "C" c_motor_task *motor_task_start_custom_animation(long object_index, e_motor_task motor_task, e_action action,
    long graph_tag_index, long animation_name, bool goal_flag, bool clear_custom);

#endif // MOTOR_MOTOR_ANIMATION_H
