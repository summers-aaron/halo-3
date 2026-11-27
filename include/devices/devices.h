#ifndef DEVICES_DEVICES_H
#define DEVICES_DEVICES_H

struct s_data_array;
struct object_placement_data;



extern "C" __declspec(thread) s_data_array *g_device_groups_data;

static __forceinline s_data_array *tls_device_groups_data()
{
    return g_device_groups_data;
}


// Functions defined in tu/objectslibxbox/devices.cpp.

union real_point3d;
union real_vector3d;
struct s_device_animation_control;

void __tls_set_g_device_groups_data_allocator(void *object);
void device_groups_data_set_new_base_address(void *object);
void device_set_animation_changed(long object_index);
void device_group_change_only_once_more_set(long group_index, bool change_only_once_more);
void device_set_power(long device_index, float power);
void device_set_actual_position(long object_index, float position);
void device_group_set_actual_value(long device_group_index, float value);
void setup_device_animation_control(s_device_animation_control *control, float target_position, float duration, float acceleration_duration,
    float deceleration_duration, float initial_velocity, float final_velocity);
bool device_set_overlay_track(long object_index, long value);
void device_touched(long device_index, long unit_index);
bool device_allow_interaction(long device_index, const real_point3d *point, const real_vector3d *vector);
void device_effect_new(long object_index, long effect_index);
bool device_group_set_desired_value(long device_index, long device_group_index, float value);
extern "C" bool device_set_position_track(long device_index, long animation_name, float interpolation_time);
extern "C" bool device_has_actual_position(long object_index, long, bool *out);
bool device_new(long object_index, object_placement_data *placement, bool *out_of_memory);
bool device_setup_position_animation_channel(long object_index, long name);
bool device_setup_power_and_overlay_track_animation_channel(long object_index, long animation_name);

class c_animation_channel;
class c_animation_manager;

extern "C" bool device_update_animation_control(long object_index, s_device_animation_control *control, const c_animation_manager *manager,
    c_animation_channel *channel, float playback_ratio, float *out_playback_ratio);

// Data defined in tu/objectslibxbox/devices.cpp that other units use.
extern bool device_groups_non_gamestate;

void devices_dispose(void);
void devices_initialize_for_new_map(void);

#endif // DEVICES_DEVICES_H
