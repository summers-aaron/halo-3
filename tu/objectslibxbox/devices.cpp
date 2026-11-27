// objectslibxbox/devices.obj: from the records 0xec9a0 0xeee48 0x117260 0x247fb0 0x3583b0 0x358408 0x358458 0x358530 0x3585d0 0x3586d8 0x3586e0 0x358840 0x3588c0 0x358938 0x358ae8 0x358b60 0x358ba8 0x358cf8 0x358d88 0x358e98 0x359030 0x359118 0x3592b0 0x3593d0 0x359438 0x3595a8 0x3598c8 0x359ee8 0x359fd8 0x35a088 0x35a138 0x35a1d8
#include "math/real_math_constants.h"
#include "cache/restricted_memory.h"
#include "sound/game_sound_globals.h"
#include "visibility/visibility_portal_activation_types.h"
#include "multithreading/thread_tls.h"
#include "devices/device_animation_control.h"
#include "objects/object_reference_types.h"
#include "game/game_globals_storage.h"
#include "math/real_math_types.h"
#include "math/real_math.h"
#include "math/matrix_math_types.h"
#include "objects/object_types.h"
#include "animations/animation_id.h"
#include "objects/object_iterator.h"
#include "objects/object_datum.h"
#include "objects/object_placement.h"
#include "cseries/cseries.h"
#include "cseries/crt_xdk.h"
#include "tag_files/tag_groups.h"
#include "memory/data_types.h"
#include "cache/restricted_memory_regions.h"
#include "objects/objects.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "devices/device_machines.h"
#include "devices/devices.h"
#include "devices/device_group.h"
#include "physics/havok_utilities.h"
#include "devices/device_controls.h"
#include "devices/device_terminals.h"
#include "memory/data.h"
#include "math/matrix_math.h"
#include "sound/game_sound.h"
#include "cache/cache_file_globals.h"
#include "scenario/scenario.h"
#include "scenario/scenario_access.h"
#include "devices/device_groups_storage.h"
#include "memory/tls_globals.h"
#include "devices/device_datum.h"
#include "devices/machine_datum.h"
#include "scenario/scenario_objects.h"
#include "models/model_types.h"
#include "devices/device_definitions.h"
#include "memory/datum_index_inlines.h"
#include <math.h>
#include "cache/restricted_memory_layout.h"
#include "cache/restricted_memory_regions_inline.h"
#include "cache/restricted_memory_region_members.h"
#include "animations/animation_manager.h"
#include "animations/mixing_board/channels/animation_channel.h"
#include "math/real_math_functions.h"
#include "animations/animation_definitions.h"
#include "animations/animation_graph_node_types.h"
#include "animations/animation_search_path_iterator.h"
#include "game/game_time_types.h"
#include "scenario/scenario_definitions.h"
#include "tag_files/tag_index.h"
#include "effects/effect_creation.h"
#include "physics/havok_component.h"
#include "physics/havok_component_class.h"
#include "structures/structure_portal_reference_types.h"
#include "objects/object_header_access.h"


// @flags /TP /O2
// @address 0x247fb0 ?__tls_set_g_device_groups_data_allocator@@YAXPAX@Z
// @address 0x11df728 g_device_groups_data
extern "C" {
__declspec(thread) s_data_array *g_device_groups_data;
}

static const long k_device_function_position = 132;
static const long k_device_function_power = 133;
static const long k_device_function_power_velocity = 504;
static const long k_device_function_position_velocity = 505;
static const long k_device_function_unusable = 506;
static const long k_device_function_delay = 507;
static const long k_device_function_not_fully_open = 581;

struct s_data_array;


struct s_scenario_device_element {
    char name[32];
    float value;
    unsigned long flags;
};

struct s_device_groups_data_publisher
{
    static __forceinline void publish(void *address)
    {
        data_set_new_base_address((s_data_array **)&g_device_groups_data, (s_data_array *)address);
    }
};


struct s_scenario_device_properties {
    short power_group_index;
    short position_group_index;
    unsigned long flags;
};

struct s_scenario_device : s_scenario_object {
    s_scenario_device_properties device;
};

struct s_devices_function_group_index { long value; };

void __tls_set_g_device_groups_data_allocator(void *object)
{
    data_set_new_base_address((s_data_array **)&TLS_IMPACT_DATA, (s_data_array *)object);
}

struct scenario;

namespace {
static __forceinline long devices_datum_new_with_value(float value, unsigned short flags)
{
    long index = datum_new(g_device_groups_data);
    if (index != -1) {
        s_device_group_datum *datum = datum_get<s_device_group_datum>(g_device_groups_data, index);
        datum->actual_value = value;
        datum->flags = flags;
        datum->desired_value = value;
    }
    return index;
}
}

// @address 0x358408 ?devices_dispose@@YAXXZ
void devices_dispose(void)
{
    if (device_groups_non_gamestate) {
        if (g_device_groups_data) {
            data_dispose(g_device_groups_data);
            g_device_groups_data = 0;
        }
        device_groups_non_gamestate = 0;
    }
}

// @address 0x7f1955 device_groups_non_gamestate
bool device_groups_non_gamestate;
// @address 0x3583b0 ?devices_initialize@@YAXXZ
void devices_initialize(void)
{
    g_device_groups_data = (s_data_array *)data_new("device groups", 0x400, 0xc, 0, (c_allocation_base *)&g_device_groups_data_allocator);
    device_groups_non_gamestate = 0;
}

// @address 0x358458 ?devices_initialize_for_new_map@@YAXXZ
void devices_initialize_for_new_map(void)
{
    s_data_array *data = g_device_groups_data;
    data->valid = 1;
    data_delete_all(data);
    scenario *scenario = global_scenario_get();
    for (long i = 0; i < scenario->device_groups.count; ++i) {
        s_scenario_device_element *element = TAG_BLOCK_GET_ELEMENT(&scenario->device_groups, i, s_scenario_device_element);
        unsigned short flags = 0;
        if (element->flags & 1)
            flags = 1;
        devices_datum_new_with_value(element->value, flags);
    }
}

struct s_state;


// @address 0x3585d0 ?device_unplace@@YAXJ@Z
void device_unplace(long object_index)
{
    device_datum *device = (device_datum *)object_header_get(object_index)->datum;
    if (device->power_group_index != -1) {
        if (datum_get<s_device_group_datum>(g_device_groups_data, device->power_group_index)->flags & 4)
            datum_delete(g_device_groups_data, device->power_group_index);
    }
    long position_group_index = device->position_group_index;
    device->power_group_index = -1;
    if (position_group_index != -1) {
        if (datum_get<s_device_group_datum>(g_device_groups_data, position_group_index)->flags & 4)
            datum_delete(g_device_groups_data, position_group_index);
    }
    device->position_group_index = -1;
    device->position_control.position = 0.0f;
    device->position_control.velocity = 0.0f;
    device->position_control.duration = 0.0f;
    device->position_control.deceleration_start = 0.0f;
    device->position_control.deceleration_duration = 0.0f;
    device->position_control.cruise_velocity = 0.0f;
    device->position_control.final_velocity = 0.0f;
    device->position_control.target_position = 0.0f;
    device->power_and_overlay_control.position = 0.0f;
    device->power_and_overlay_control.velocity = 0.0f;
    device->power_and_overlay_control.duration = 0.0f;
    device->power_and_overlay_control.deceleration_start = 0.0f;
    device->power_and_overlay_control.deceleration_duration = 0.0f;
    device->power_and_overlay_control.cruise_velocity = 0.0f;
    device->power_and_overlay_control.final_velocity = 0.0f;
    device->power_and_overlay_control.target_position = 0.0f;
}

// @address 0x3586d8 ?device_delete@@YAXJ@Z
void device_delete(long object_index)
{
    device_unplace(object_index);
}

// @address 0x358840 ?device_group_get_from_scenario_index@@YAJF@Z
long device_group_get_from_scenario_index(short scenario_index)
{
    long group_index = -1;
    if (scenario_index >= 0 && scenario_index < global_scenario_get()->device_groups.count) {
        group_index = datum_absolute_index_to_index(g_device_groups_data, scenario_index);
        s_device_group_datum *group = datum_get<s_device_group_datum>(g_device_groups_data, group_index);
        if (group->flags & 4)
            group_index = -1;
    }
    return group_index;
}

// @address 0x358b60 device_has_actual_position
bool device_has_actual_position(long object_index, long, bool *out)
{
    bool result = false;
    if (object_index != -1) {
        device_datum *device = (device_datum *)object_header_get(object_index)->datum;
        if (device->device_flags & 128) {
            *out = true;
            result = true;
        }
    }
    return result;
}

// 0x358cf8 (dev: device_set_animation_changed(long))
// @address 0x358cf8 ?device_set_animation_changed@@YAXJ@Z
void device_set_animation_changed(long object_index)
{
    device_datum *device = (device_datum *)object_header_get(object_index)->datum;
    if ((1 << device->object_type) & 0x190) {
        device->device_flags |= 4;
        object_wake(object_index);
    }
    for (long child_index = device->first_child_object_index; child_index != -1;
         child_index = ((device_datum *)object_header_get(child_index)->datum)->next_object_index) {
        device_set_animation_changed(child_index);
    }
}

// 0x358d88 (dev: device_group_change_only_once_more_set(long,bool))
// @address 0x358d88 ?device_group_change_only_once_more_set@@YAXJ_N@Z
void device_group_change_only_once_more_set(long group_index, bool change_only_once_more)
{
    if (group_index != -1) {
        s_device_group_datum *group = datum_get<s_device_group_datum>(g_device_groups_data, group_index);
        if (change_only_once_more)
            group->flags |= 1;
        else
            group->flags &= ~1;
        group->flags &= ~2;

        c_object_iterator<device_datum> iterator;
        iterator.begin(0x190, 0);
        while (iterator.next()) {
            device_datum *device = iterator.get_datum();
            if (device->power_group_index == group_index)
                object_wake(iterator.get_index());
            if (device->position_group_index == group_index)
                object_wake(iterator.get_index());
        }
    }
}


// @address 0x3588c0 ?device_set_power@@YAXJM@Z
void device_set_power(long device_index, float power)
{
    if (device_index != -1) {
        device_datum *device = (device_datum *)object_header_get(device_index)->datum;
        device_set_animation_changed(device_index);
        device->power = power;
        object_wake(device_index);
        device_group_set_desired_value(device_index, device->power_group_index, power);
        simulation_action_object_update(device_index, 0x10000);
    }
}

// @address 0x358ae8 ?device_set_actual_position@@YAXJM@Z
void device_set_actual_position(long object_index, float position)
{
    if (object_index != -1) {
        device_datum *device = (device_datum *)object_header_get(object_index)->datum;
        if (device->position_group_index != -1)
            device_group_set_actual_value(device->position_group_index, position);
        device->device_flags |= 128;
        object_wake(object_index);
    }
}

// @address 0x3593d0 ?device_group_new@@YAJMK@Z
long device_group_new(float value, unsigned long flags)
{
    return devices_datum_new_with_value(value, flags);
}

const float k_device_animation_minimum_velocity = 0.0000001f;
#define DEVICES_MIN(a, b) (((a) > (b)) ? (b) : (a))
#define DEVICES_MAX(a, b) (((a) > (b)) ? (a) : (b))
#define DEVICES_PIN(v, lo, hi) ((DEVICES_MAX((v), (lo)) < (hi)) ? DEVICES_MAX((v), (lo)) : (hi))
// @address 0x358ba8 ?device_group_set_actual_value@@YAXJM@Z
void device_group_set_actual_value(long device_group_index, float value)
{
    if (device_group_index != -1) {
        value = DEVICES_PIN(value, 0.0f, 1.0f);
        s_device_group_datum *group = datum_get<s_device_group_datum>(g_device_groups_data, device_group_index);
        group->actual_value = value;

        c_object_iterator<device_datum> iterator;
        iterator.begin(0x190, 0);
        while (iterator.next()) {
            long device_index = iterator.get_index();
            device_datum *device = iterator.get_datum();
            if (device->power_group_index == device_group_index) {
                device_set_animation_changed(device_index);
                device->power = value;
                device->power_velocity = 0.0f;
                object_wake(device_index);
                simulation_action_object_update(device_index, 0x10000);
            }
            if (device->position_group_index == device_group_index) {
                device_set_animation_changed(device_index);
                device->device_position = value;
                device->position_velocity = 0.0f;
                object_wake(device_index);
                simulation_action_object_update(device_index, 0x4000);
            }
        }
    }
}

// @address 0x359438 ?setup_device_animation_control@@YAXPAUs_device_animation_control@@MMMMMM@Z
void setup_device_animation_control(s_device_animation_control *control, float target_position, float duration,
    float acceleration_duration, float deceleration_duration, float initial_velocity, float final_velocity)
{
    duration = DEVICES_MAX(duration, 0.0f);
    acceleration_duration = DEVICES_MAX(acceleration_duration, 0.0f);
    deceleration_duration = DEVICES_MAX(deceleration_duration, 0.0f);
    acceleration_duration = DEVICES_MIN(acceleration_duration, duration);
    float deceleration_start = duration - acceleration_duration;
    deceleration_duration = DEVICES_MIN(deceleration_start, deceleration_duration);

    float initial_position = DEVICES_PIN(control->position, 0.0f, 1.0f);
    float final_position = DEVICES_PIN(target_position, 0.0f, 1.0f);
    float distance = final_position - initial_position;

    control->position = final_position;
    control->velocity = final_velocity;
    control->duration = 0.0f;
    control->deceleration_start = 0.0f;
    control->deceleration_duration = 0.0f;
    control->cruise_velocity = 0.0f;
    control->final_velocity = 0.0f;
    control->target_position = final_position;

    if (fabsf(duration) < 0.0001f)
        return;
    if (fabsf(distance) < 0.000001f)
        return;

    float half_acceleration = acceleration_duration * 0.5f;
    float half_deceleration = deceleration_duration * 0.5f;
    control->target_position = final_position;
    control->position = initial_position;
    control->velocity = initial_velocity;
    control->duration = duration;
    control->deceleration_start = deceleration_start;
    control->deceleration_duration = deceleration_duration;
    control->cruise_velocity = 0.0f;
    control->final_velocity = final_velocity;

    float cruise_time = (deceleration_start - deceleration_duration) + half_deceleration + half_acceleration;
    if (cruise_time != 0.0f)
        control->cruise_velocity = (distance - half_acceleration * initial_velocity - half_deceleration * final_velocity) / cruise_time;
    else
        control->cruise_velocity = distance / duration;
    float cruise_velocity = control->cruise_velocity;
    if (fabsf(cruise_velocity) < k_device_animation_minimum_velocity)
        control->cruise_velocity = cruise_velocity < 0.0f ? -k_device_animation_minimum_velocity : k_device_animation_minimum_velocity;
}


class c_restricted_memory;


class c_animation_channel;

static __forceinline device_datum *devices_device_get(long object_index)
{
    return (device_datum *)object_header_get(object_index)->datum;
}

static __forceinline bool devices_device_has_animation(long object_index)
{
    return devices_device_get(object_index)->animation.offset != -1;
}

static __forceinline void *devices_device_block_get(long object_index, const s_object_block_reference *reference)
{
    return (char *)devices_device_get(object_index) + reference->offset;
}

struct render_model_definition;
class c_model_animation_graph;
struct s_animation_event_data;
// @address 0x358530 ?device_new@@YA_NJPAUobject_placement_data@@PA_N@Z
bool device_new(long object_index, object_placement_data *placement, bool *out_of_memory)
{
    device_datum *device = (device_datum *)object_header_get(object_index)->datum;
    device->position_group_index = -1;
    device->power_group_index = -1;
    device->position_channel.initialize();
    device->power_and_overlay_track_channel.initialize();
    device->position_control.position = 0.0f;
    device->position_control.velocity = 0.0f;
    device->position_control.duration = 0.0f;
    device->position_control.deceleration_start = 0.0f;
    device->position_control.deceleration_duration = 0.0f;
    device->position_control.cruise_velocity = 0.0f;
    device->position_control.final_velocity = 0.0f;
    device->position_control.target_position = 0.0f;
    device->power_and_overlay_control.position = 0.0f;
    device->power_and_overlay_control.velocity = 0.0f;
    device->power_and_overlay_control.duration = 0.0f;
    device->power_and_overlay_control.deceleration_start = 0.0f;
    device->power_and_overlay_control.deceleration_duration = 0.0f;
    device->power_and_overlay_control.cruise_velocity = 0.0f;
    device->power_and_overlay_control.final_velocity = 0.0f;
    device->power_and_overlay_control.target_position = 0.0f;
    return true;
}

// @address 0x3586e0 ?device_preprocess_node_orientations@@YAXJPBV?$c_static_flags@$0PP@@@JPAUreal_orientation@@@Z
void device_preprocess_node_orientations(
    long object_index, const c_static_flags<255> *node_mask, long node_orientations_count, real_orientation *node_orientations)
{
    device_datum *device = devices_device_get(object_index);
    s_object_definition *definition = (s_object_definition *)tag_get(0, device->definition);
    if (definition->model.index != -1) {
        s_model_definition *model = (s_model_definition *)tag_get(0, definition->model.index);
        if (model->render_model_index != -1) {
            const render_model_definition *render_model = (const render_model_definition *)tag_get(0, model->render_model_index);
            if (devices_device_has_animation(object_index)) {
                c_animation_channel *position = (c_animation_channel *)&device->position_channel;
                if (position->valid())
                    position->apply_weighted_node_orientations((const render_model_definition *)render_model, 1.0f, node_mask,
                        node_orientations_count, node_orientations, 0, object_index);
                c_animation_channel *power = (c_animation_channel *)&device->power_and_overlay_track_channel;
                if (power->valid())
                    power->apply_weighted_node_orientations((const render_model_definition *)render_model, 1.0f, node_mask,
                        node_orientations_count, node_orientations, 0, object_index);
            }
        }
    }
}

// @address 0x359ee8 ?device_set_overlay_track@@YA_NJJ@Z
bool device_set_overlay_track(long object_index, long value)
{
    bool result = false;
    if (object_index != -1)
    {
        device_datum *state = (device_datum *)object_try_and_get_and_verify_type(object_index, 400);
        if (state != 0 && (unsigned short)devices_device_get(object_index)->animation.offset != 0xffff)
        {
            unsigned char ok;
            float zero = k_real_zero_storage;
            state->power_and_overlay_control.position = zero;
            state->power_and_overlay_control.velocity = zero;
            state->power_and_overlay_control.duration = zero;
            state->power_and_overlay_control.deceleration_start = zero;
            state->power_and_overlay_control.deceleration_duration = zero;
            state->power_and_overlay_control.cruise_velocity = zero;
            state->power_and_overlay_control.final_velocity = zero;
            state->power_and_overlay_control.target_position = zero;
            result = device_setup_power_and_overlay_track_animation_channel(object_index, value);
            ok = (unsigned char)result;
            if (ok == 0)
                device_setup_power_and_overlay_track_animation_channel(object_index, 133);
            object_wake(object_index);
            if (ok != 0)
                state->device_flags |= 16;
            else
                state->device_flags &= ~16;
        }
    }
    return result;
}

// @address 0x359fd8 ?device_setup_position_animation_channel@@YA_NJJ@Z
bool device_setup_position_animation_channel(long object_index, long name)
{
    bool success = false;
    if (object_index != -1 && devices_device_has_animation(object_index))
    {
        device_datum *device = devices_device_get(object_index);
        void *manager = devices_device_block_get(object_index, &device->animation);
        success = ((c_animation_manager *)manager)->setup_animation_channel_by_name(
            (c_animation_channel *)&device->position_channel, name, 64);
        if (!success)
            success = ((c_animation_manager *)manager)
                          ->setup_animation_channel_by_parallel_impulse((c_animation_channel *)&device->position_channel, 226, 226, name, 64);
        object_wake(object_index);
    }
    return success;
}

// @address 0x35a088 ?device_setup_power_and_overlay_track_animation_channel@@YA_NJJ@Z
bool device_setup_power_and_overlay_track_animation_channel(long object_index, long animation_name)
{
    bool success = false;
    if (object_index != -1 && devices_device_has_animation(object_index))
    {
        device_datum *device = devices_device_get(object_index);
        void *manager = devices_device_block_get(object_index, &device->animation);
        success = ((c_animation_manager *)manager)->setup_animation_channel_by_name(
            (c_animation_channel *)&device->power_and_overlay_track_channel, animation_name, 64);
        if (!success)
            success = ((c_animation_manager *)manager)
                          ->setup_animation_channel_by_parallel_impulse(
                              (c_animation_channel *)&device->power_and_overlay_track_channel, 226, 226, animation_name, 64);
        object_wake(object_index);
    }
    return success;
}

// @address 0x35a138 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_device_groups_data_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template <>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_device_groups_data_allocator>::allocate(unsigned int size, const char *name)
{
    m_member_index = restricted_region_add_member<s_device_groups_data_publisher>(3, name, "", size, 0, device_groups_data_set_new_base_address);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(3, m_member_index);
}

// @address 0x35a1d8 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_device_groups_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
template <>
void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_device_groups_data_allocator>::deallocate(void *allocation)
{
    g_device_groups_data = 0;
    restricted_region_free_member(3, m_member_index);
}

// match: noalias declaration preserves caller live registers

struct s_location;


static __forceinline long devices_animation_graph_find_node(const c_model_animation_graph *graph, long name)
{
    for (long node_index = 0; node_index < graph->m_nodes.count; node_index++)
    {
        if (((const animation_graph_node *)graph->m_nodes.address)[node_index].name == name)
            return node_index;
    }
    return -1;
}

static __forceinline object_datum *devices_position_track_object_get(long object_index)
{
    return object_header_get(object_index)->datum;
}

// @rdata 0x8202490c
static const float devices_position_track_epsilon = 0.0001f;
// @rdata 0x8202aed4
static const float devices_position_track_frames_to_seconds = 0.033333335f;


static __forceinline float devices_animation_control_update_velocity(s_device_animation_control *control, float velocity, float cruise_velocity,
    float target_velocity, float time, float seconds_per_tick)
{
    float rate = (target_velocity - velocity) / time;
    float minimum_velocity = DEVICES_MIN(velocity, cruise_velocity);
    float maximum_velocity = DEVICES_MAX(velocity, cruise_velocity);
    control->velocity = rate * seconds_per_tick + velocity;
    return DEVICES_PIN(control->velocity, minimum_velocity, maximum_velocity);
}

// @address 0x3595a8 device_update_animation_control
extern "C" bool device_update_animation_control(long object_index, s_device_animation_control *control, const c_animation_manager *manager,
    c_animation_channel *channel, float playback_ratio, float *out_playback_ratio)
{
    bool finished = false;
    device_datum *device = (device_datum *)object_header_get(object_index)->datum;
    if (channel->valid())
    {
        float seconds_per_tick = game_time_globals->tick_length;
        float current_velocity = control->velocity;
        if (fabsf(current_velocity) < k_device_animation_minimum_velocity)
            control->velocity = current_velocity < 0.0f ? -k_device_animation_minimum_velocity : k_device_animation_minimum_velocity;
        float duration = DEVICES_MAX(control->duration - seconds_per_tick, 0.0f);
        control->duration = duration;
        if (fabsf(duration) < 0.0001f)
        {
            finished = true;
            control->duration = 0.0f;
            control->velocity = control->final_velocity;
            control->position = control->target_position;
        }
        else if (duration < seconds_per_tick * 2.0f)
        {
            control->position = (control->target_position - control->position) / duration + control->position;
        }
        else
        {
            float acceleration_time = DEVICES_MAX(duration - control->deceleration_start, 0.0f);
            float cruise_time = DEVICES_MAX(duration - control->deceleration_duration, 0.0f);
            float deceleration_time = DEVICES_MIN(duration, control->deceleration_duration);
            float cruise_velocity = control->cruise_velocity;
            float velocity = control->velocity;
            if (acceleration_time > 0.0f)
                control->velocity = devices_animation_control_update_velocity(
                    control, velocity, cruise_velocity, cruise_velocity, acceleration_time, seconds_per_tick);
            else if (cruise_time > 0.0f)
                control->velocity = devices_animation_control_update_velocity(
                    control, velocity, cruise_velocity, cruise_velocity, cruise_time, seconds_per_tick);
            else
                control->velocity = devices_animation_control_update_velocity(
                    control, velocity, cruise_velocity, control->final_velocity, deceleration_time, seconds_per_tick);
            control->position = control->velocity * seconds_per_tick + control->position;
            setup_device_animation_control(control, control->target_position, duration, acceleration_time, deceleration_time,
                control->velocity, control->final_velocity);
        }
        control->position = DEVICES_PIN(control->position, 0.0f, 1.0f);
        playback_ratio = control->position;
        channel->update_to_playback_ratio(playback_ratio, manager, object_animation_callback, object_index);
        device->device_flags |= 4;
    }
    if (out_playback_ratio)
        *out_playback_ratio = playback_ratio;
    return finished;
}

// @address 0x3598c8 device_set_position_track
extern "C" bool device_set_position_track(long device_index, long animation_name, float interpolation_time)
{
    bool result = false;
    if (device_index != -1)
    {
        device_datum *device = (device_datum *)object_try_and_get_and_verify_type(device_index, 400);
        if (device)
        {
            s_object_definition *definition = (s_object_definition *)tag_get(0, device->definition);
            if (definition->model.index != -1)
            {
                s_model_definition *model = (s_model_definition *)tag_get(0, definition->model.index);
                if (model->render_model_index != -1)
                {
                    const render_model_definition *render_model = (const render_model_definition *)tag_get(0, model->render_model_index);
                    if (devices_device_has_animation(device_index))
                    {
                        c_animation_manager *manager = (c_animation_manager *)((char *)device + device->animation.offset);
                        bool animated = false;
                        float velocity = 0.0f;
                        real_matrix4x3 original;
                        matrix4x3_rotation_from_vectors(&original, &device->forward, &device->up);
                        original.position = device->position;
                        real_matrix4x3 desired = original;
                        if (device->device_flags & 8)
                        {
                            c_animation_id animation_id = device->position_channel.m_animation_id;
                            float frame = device->position_channel.m_frame_position * devices_position_track_frames_to_seconds;
                            real_matrix4x3 current;
                            {
                                real_matrix4x3 initial;
                                animation_definitions_get_animation_root_matrix(
                                    manager->m_graph_tag_index, animation_id, 0.0f, render_model, &initial);
                                animation_definitions_get_animation_root_matrix(
                                    manager->m_graph_tag_index, animation_id, frame, render_model, &current);
                                c_model_animation_graph *graph = (c_model_animation_graph *)tag_get(0, manager->m_graph_tag_index);
                                long node_index = devices_animation_graph_find_node(graph, 459);
                                if (node_index != -1)
                                {
                                    object_datum *object = devices_position_track_object_get(device_index);
                                    desired = ((real_matrix4x3 *)((char *)object + object->node_matrices.offset))[(short)node_index];
                                }
                                else
                                {
                                    real_matrix4x3 inverse;
                                    matrix4x3_inverse(&initial, &inverse);
                                    matrix4x3_compute_product(&current, &inverse, &initial);
                                    matrix4x3_compute_product(&desired, &initial, &desired);
                                }
                            }
                            animated = true;
                            float seconds_per_tick = game_time_globals->tick_length;
                            if (seconds_per_tick > 0.0f)
                            {
                                float previous_frame = frame - seconds_per_tick;
                                if (previous_frame > 0.0f)
                                {
                                    real_matrix4x3 previous;
                                    animation_definitions_get_animation_root_matrix(manager->m_graph_tag_index,
                                        animation_id, previous_frame, render_model, &previous);
                                    real_vector3d delta;
                                    delta.k = current.position.z - previous.position.z;
                                    delta.i = current.position.x - previous.position.x;
                                    delta.j = current.position.y - previous.position.y;
                                    float distance_squared = delta.i * delta.i + delta.j * delta.j + delta.k * delta.k;
                                    if (distance_squared > 0.0f)
                                        velocity = sqrtf(distance_squared) / seconds_per_tick;
                                }
                            }
                        }
                        device->position_control.position = 0.0f;
                        device->position_control.velocity = 0.0f;
                        device->position_control.duration = 0.0f;
                        device->position_control.deceleration_start = 0.0f;
                        device->position_control.deceleration_duration = 0.0f;
                        device->position_control.cruise_velocity = 0.0f;
                        device->position_control.final_velocity = 0.0f;
                        device->position_control.target_position = 0.0f;
                        device->device_position = 0.0f;
                        device->position_velocity = 0.0f;
                        if (animated && device->parent_object_index == -1)
                        {
                            real_vector3d forward = desired.forward;
                            real_vector3d up = desired.up;
                            normalize3d(&forward);
                            normalize3d(&up);
                            if (fabsf(original.position.x - desired.position.x) >= devices_position_track_epsilon ||
                                fabsf(original.position.y - desired.position.y) >= devices_position_track_epsilon ||
                                fabsf(original.position.z - desired.position.z) >= devices_position_track_epsilon ||
                                fabsf(original.forward.i - forward.i) >= devices_position_track_epsilon ||
                                fabsf(original.forward.j - forward.j) >= devices_position_track_epsilon ||
                                fabsf(original.forward.k - forward.k) >= devices_position_track_epsilon ||
                                fabsf(original.up.i - up.i) >= devices_position_track_epsilon ||
                                fabsf(original.up.j - up.j) >= devices_position_track_epsilon ||
                                fabsf(original.up.k - up.k) >= devices_position_track_epsilon)
                            {
                                object_set_position_direct(device_index, &desired.position, &forward, &up, 0, false);
                                if (devices_position_track_object_get(device_index)->node_orientations.offset != -1)
                                {
                                    object_datum *object = devices_position_track_object_get(device_index);
                                    real_orientation *node_orientations = (real_orientation *)((char *)object + object->node_orientations.offset);
                                    real_orientation *original_node_orientations =
                                        (real_orientation *)((char *)object + object->original_node_orientations.offset);
                                    real_orientation orientation;
                                    matrix4x3_to_orientation(&original, &orientation);
                                    orientations_multiply(&orientation, node_orientations, node_orientations);
                                    orientations_multiply(&orientation, original_node_orientations, original_node_orientations);
                                    real_matrix4x3 matrix;
                                    real_matrix4x3 inverse;
                                    matrix4x3_rotation_from_vectors(&matrix, &device->forward, &device->up);
                                    matrix.position = device->position;
                                    matrix4x3_inverse(&matrix, &inverse);
                                    matrix4x3_to_orientation(&inverse, &orientation);
                                    orientations_multiply(&orientation, node_orientations, node_orientations);
                                    orientations_multiply(&orientation, original_node_orientations, original_node_orientations);
                                }
                            }
                        }
                        if (interpolation_time > 0.0f && devices_position_track_object_get(device_index)->node_orientations.offset != -1)
                            object_start_interpolation(device_index, interpolation_time);
                        result = device_setup_position_animation_channel(device_index, animation_name);
                        unsigned char success = (unsigned char)result;
                        if (success == 0)
                        {
                            device_setup_position_animation_channel(device_index, 132);
                        }
                        else if (velocity > 0.0f)
                        {
                            c_animation_id animation_id = device->position_channel.m_animation_id;
                            float duration = device->position_channel.get_authored_duration();
                            if (duration > 0.0f)
                            {
                                real_matrix4x3 first;
                                real_matrix4x3 last;
                                animation_definitions_get_animation_root_matrix(
                                    manager->m_graph_tag_index, animation_id, 0.0f, render_model, &first);
                                animation_definitions_get_animation_root_matrix(
                                    manager->m_graph_tag_index, animation_id, duration, render_model, &last);
                                real_vector3d delta;
                                delta.j = last.position.y - first.position.y;
                                delta.k = last.position.z - first.position.z;
                                delta.i = last.position.x - first.position.x;
                                float distance_squared = delta.i * delta.i + delta.j * delta.j + delta.k * delta.k;
                                if (distance_squared > 0.0f)
                                    device->position_control.velocity = sqrtf(distance_squared) * velocity;
                            }
                        }
                        object_wake(device_index);
                        object_compute_node_matrices(device_index);
                        device->device_flags |= 4;
                        if (success != 0)
                            device->device_flags |= 8;
                        else
                            device->device_flags &= ~8;
                    }
                }
            }
        }
    }
    return result;
}

// match: the unsigned flag wrapper preserves terminal test/set signedness and scheduling.
static __forceinline c_flags_no_init<long, unsigned short, 16> &devices_terminal_flag_bits(s_terminal_datum *terminal)
{
    return *(c_flags_no_init<long, unsigned short, 16> *)&terminal->flags;
}

static __forceinline device_datum *devices_touched_object_get(long object_index)
{
    return (device_datum *)object_header_get(object_index)->datum;
}
static __forceinline void devices_control_touched(long control_index, long unit_index)
{
    device_datum *control = devices_touched_object_get(control_index);
    s_control_definition *definition = (s_control_definition *)tag_get(0, control->definition);
    if (definition->triggers_when == 0)
        control_toggle(control_index);
}
static __forceinline bool devices_terminal_allow_interaction(long terminal_index)
{
    s_terminal_datum *terminal = (s_terminal_datum *)devices_touched_object_get(terminal_index);
    bool result = false;
    if ((unsigned short)terminal->state == 0 && !devices_terminal_flag_bits(terminal).test(0))
        result = true;
    return result;
}
static __forceinline void devices_terminal_touched(long terminal_index, long unit_index)
{
    object_wake(terminal_index);
    if (devices_terminal_allow_interaction(terminal_index))
    {
        s_terminal_datum *terminal = (s_terminal_datum *)devices_touched_object_get(terminal_index);
        devices_terminal_flag_bits(terminal).set(0, true);
        terminal->hidden_object_index = unit_index;
    }
}

// @address 0x359030 ?device_touched@@YAXJJ@Z
void device_touched(long device_index, long unit_index)
{
    device_datum *device = devices_touched_object_get(device_index);
    switch (device->object_type)
    {
    case _object_type_terminal:
        devices_terminal_touched(device_index, unit_index);
        break;
    case _object_type_control:
        devices_control_touched(device_index, unit_index);
        break;
    }
}


static __forceinline bool devices_control_allow_interaction(long device_index, const real_point3d *point, const real_vector3d *vector)
{
    control_datum *control = (control_datum *)object_try_and_get_and_verify_type(device_index, 0x100);
    bool result = true;
    if (control && !(control->control_flags & 1))
    {
        object_marker marker;
        if (internal_object_get_markers_by_string_id(device_index, 151, &marker, 1, false) == 1 &&
            dot_product3d(vector, &marker.matrix.forward) > 0.0f)
        {
            result = false;
        }
    }
    return result;
}

static __forceinline s_device_group_datum *devices_group_get(long index)
{
    return datum_get<s_device_group_datum>(g_device_groups_data, index);
}

static __forceinline bool devices_groups_allow_interaction(long device_index)
{
    device_datum *device = (device_datum *)object_header_get(device_index)->datum;
    bool result = false;
    if (device->position_group_index != -1)
    {
        result = true;
        s_device_group_datum *position_group = devices_group_get(device->position_group_index);
        s_device_group_datum *power_group = devices_group_get(device->power_group_index);
        if ((position_group->flags & 1) && (position_group->flags & 2))
            result = false;
        if (device->device_flags & 2)
            result = false;
        if (power_group->actual_value != 1.0f)
            result = false;
    }
    return result;
}

// @address 0x359118 ?device_allow_interaction@@YA_NJPBTreal_point3d@@PBTreal_vector3d@@@Z
bool device_allow_interaction(long device_index, const real_point3d *point, const real_vector3d *vector)
{
    bool result = false;
    if (devices_control_allow_interaction(device_index, point, vector) && devices_groups_allow_interaction(device_index))
    {
        result = true;
        if (object_header_get(device_index)->object_type == 4)
            result = terminal_can_activate(device_index, point);
    }
    return result;
}


static __forceinline s_scenario_zone_set_visibility *devices_portal_zones_get()
{
    return (s_scenario_zone_set_visibility *)global_scenario->zone_set_pvs.address;
}
static __forceinline s_scenario_zone_set *devices_portal_sets_get()
{
    return (s_scenario_zone_set *)global_scenario->zone_sets.address;
}
static __forceinline s_portal_reference devices_door_portal_reference_get(long zone_set_index, long bsp_index, long portal_index)
{
    s_portal_reference result;
    short bsp_part = (short)(bsp_index << 10);
    const s_scenario_bsp_machine_door_mapping *mappings = (const s_scenario_bsp_machine_door_mapping *)
        devices_portal_zones_get()[devices_portal_sets_get()[zone_set_index].pvs_index].machine_door_mappings.address;
    result.value = (short)((((const short *)mappings[bsp_index].machine_door_portal_references.address)[portal_index] &
                               1023) |
        bsp_part);
    return result;
}
static __forceinline long devices_portal_index(s_portal_reference reference)
{
    return reference.portal_index;
}
static __forceinline long devices_portal_bsp_index(s_portal_reference reference)
{
    return reference.bsp_index;
}
static __forceinline void devices_portal_activation_set_active(s_portal_reference reference, bool active)
{
    long portal_index = devices_portal_index(reference);
    unsigned long *words = g_active_portal_bitvectors + 16 * devices_portal_bsp_index(reference);
    if (active)
        words[portal_index >> 5] |= 1UL << (portal_index & 31);
    else
        words[portal_index >> 5] &= ~(1UL << (portal_index & 31));
}
static __forceinline void devices_set_door_occlusion(long bsp_index, long door_index, float occlusion)
{
    if (bsp_index != -1)
    {
        long quantized_occlusion = quantize_real(occlusion, 0.0f, 1.0f, 16, false, false);
        long occlusion_index = game_sound_compute_group_item_offset(bsp_index, door_index);
        g_game_sound_globals->occlusion[occlusion_index] = (short)quantized_occlusion;
    }
}

// @address 0xeee48 device_update_portals
extern "C" void device_update_portals(long object_index)
{
    s_machine_datum *device = (s_machine_datum *)object_header_get(object_index)->datum;
    s_machine_definition *definition = (s_machine_definition *)tag_get(0, device->definition);
    bool active = false;
    if (device->position_group_index != -1) {
        bool present = ((device->device_flags & 0x100) != 0 &&
                           datum_get<s_device_group_datum>(g_device_groups_data, device->position_group_index)->actual_value != 0.0f) ||
            device->device_position != 0.0f;
        active = present;
    }
    float occlusion = (definition->open_occlusion - definition->closed_occlusion) * device->device_position + definition->closed_occlusion;
    for (long door_index = device->pathfinding_object_index; door_index < device->pathfinding_object_index + device->pathfinding_count; door_index++) {
        s_portal_reference portal = devices_door_portal_reference_get(global_zone_set_index, device->pathfinding_bsp_index, door_index);
        devices_portal_activation_set_active(portal, active);
        devices_set_door_occlusion(device->pathfinding_bsp_index, door_index, PIN(occlusion, 0.0f, 1.0f));
    }
}


// @address 0x358e98 ?device_place@@YAXJPAUs_scenario_device@@@Z
void device_place(long object_index, s_scenario_device *scenario_device)
{
    device_datum *device = (device_datum *)object_header_get(object_index)->datum;
    s_scenario_device_properties *properties = &scenario_device->device;
    if (properties->power_group_index == -1)
        device->power_group_index = device_group_new((properties->flags & 2) ? 0.0f : 1.0f, 4);
    else
        device->power_group_index = device_group_get_from_scenario_index(properties->power_group_index);
    if (properties->position_group_index == -1)
        device->position_group_index = device_group_new((properties->flags & 1) ? 1.0f : 0.0f, 4 | ((properties->flags >> 2) & 1));
    else
        device->position_group_index = device_group_get_from_scenario_index(properties->position_group_index);
    device->power = datum_get<s_device_group_datum>(g_device_groups_data, device->power_group_index)->actual_value;
    device->device_position = datum_get<s_device_group_datum>(g_device_groups_data, device->position_group_index)->actual_value;
    if (properties->flags & 8)
        device->device_flags |= 1;
    if (properties->flags & 0x10)
        device->device_flags |= 2;
    device_setup_position_animation_channel(object_index, 132);
    device_setup_power_and_overlay_track_animation_channel(object_index, 133);
    device->device_flags |= 0x80;
}

union real_rgb_color;
struct effect_vector_field;

// @address 0x3592b0 ?device_effect_new@@YAXJJ@Z
void device_effect_new(long object_index, long effect_index)
{
    if (effect_index != NONE)
    {
        device_datum *device = (device_datum *)object_header_get(object_index)->datum;
        s_damage_owner owner = device->damage_owner;
        switch (tag_get_group_tag(effect_index))
        {
        case 'effe':
            effect_new_from_object(effect_index, &owner, 0, object_index,
                device->device_position, device->power, 0, 0, 1);
            break;
        case 'snd!':
            object_impulse_sound_new(object_index, effect_index, NONE, global_origin3d, global_forward3d, 1.0f);
            break;
        }
    }
}

// @address 0x358938 ?device_group_set_desired_value@@YA_NJJM@Z
bool device_group_set_desired_value(long device_index, long device_group_index, float value)
{
    bool changed = false;
    value = DEVICES_PIN(value, 0.0f, 1.0f);
    if (device_group_index != -1) {
        s_device_group_datum *group = datum_get<s_device_group_datum>(g_device_groups_data, device_group_index);
        if (group->actual_value != value) {
            if (!TEST_BIT(group->flags, 0) || !TEST_BIT(group->flags, 1)) {
                group->actual_value = value;
                changed = true;
                group->flags |= 2;
            }
        }
    }
    if (changed) {
        c_object_iterator<device_datum> iterator;
        iterator.begin(0x190, 0);
        while (iterator.next()) {
            device_datum *device = iterator.get_datum();
            long object_index = iterator.get_index();
            s_device_definition *definition =
                (s_device_definition *)tag_get(0, device->definition_index);
            if (device->power_group_index == device_group_index) {
                simulation_action_object_update(object_index, 0x20000);
                device_effect_new(object_index, value > 0.0f ? definition->repowered.index : definition->depowered.index);
                object_wake(object_index);
            }
            if (device->position_group_index == device_group_index) {
                simulation_action_object_update(object_index, 0x8000);
                object_wake(object_index);
            }
        }
    }
    return changed;
}


static __forceinline bool devices_update_flag_test(unsigned long flags, long bit)
{
    return ((flags >> bit) & 1) != 0;
}
static __forceinline bool devices_update_game_is_predicted()
{
    bool result = false;
    if (tls_game_globals()->options.simulation_type == 4)
        result = true;
    return result;
}
static __forceinline bool devices_update_object_has_animation_manager(long object_index)
{
    device_datum *object = (device_datum *)object_header_get(object_index)->datum;
    return (unsigned short)object->animation.offset != 0xffff;
}

// @address 0xec9a0 ?device_update@@YA_NJ@Z
bool device_update(long object_index)
{
    s_machine_datum *device = (s_machine_datum *)object_header_get(object_index)->datum;
    s_control_definition *definition = (s_control_definition *)tag_get(0, device->definition_index);
    c_animation_manager *animation_manager = 0;
    bool has_animation = false;
    bool result = false;

    if (devices_update_flag_test(device->physics_flags, 13))
    {
        if (fabsf(device->power) >= devices_position_track_epsilon || devices_update_flag_test(device->physics_flags, 12))
        {
            if (device->havok_component_index != -1)
            {
                c_havok_component *component = datum_get<c_havok_component>(g_havok_component_data, device->havok_component_index);
                havok_component_update_phantom_state((c_havok_component *)component,
                    device->power > 0.0f || devices_update_flag_test(device->physics_flags, 12));
                result = true;
            }
        }
    }

    if (devices_update_object_has_animation_manager(object_index) && device->position_channel.valid())
    {
        animation_manager = (c_animation_manager *)((char *)device + device->animation.offset);
        has_animation = true;
    }

    if (definition->type == 2)
    {
        if (has_animation)
            device->position_channel.set_frame_position_from_playback_ratio(device->device_position);
        device->position_velocity = 0.0f;
        float depowered_speed = definition->depowered_position_transition_time;
        float powered_speed = definition->position_transition_time;
        float speed = device->power * powered_speed + (1.0f - device->power) * depowered_speed;
        if (fabsf(speed) > devices_position_track_epsilon)
        {
            device->device_position += speed * game_time_globals->tick_length;
            if (device->device_position >= 1.0f)
            {
                device->device_position -= 1.0f;
                if (has_animation)
                {
                    device->position_channel.update_to_playback_ratio(1.0f, animation_manager, object_animation_callback, object_index);
                    device->position_channel.set_frame_position_from_playback_ratio(0.0f);
                }
            }
            simulation_action_object_update(object_index, 0x4000);
            device_set_animation_changed(object_index);
            if (has_animation)
                device->position_channel.update_to_playback_ratio(device->device_position, animation_manager, object_animation_callback, object_index);
            if (device->position_group_index != -1)
                devices_group_get(device->position_group_index)->actual_value = device->device_position;
            result = true;
        }
    }

    bool door_tested = false;
    if (!(device->machine_flags.get_unsafe() & 1) && definition->type == 0)
    {
        result = true;
        if (((game_time_globals->time + object_index) & 3) == 0 && !devices_update_game_is_predicted())
        {
            float radius = definition->automatic_activation_radius;
            if (radius < devices_position_track_epsilon)
                radius = device->bounding_radius;
            long object_indices[16];
            long count = objects_in_sphere(1, 3, &device->location, &device->bounding_center, radius, object_indices, 16);
            door_tested = machine_door_test_object(object_index, object_indices, (short)count);
            if (door_tested)
            {
                if (device->position_group_index != -1)
                    device_group_set_desired_value(object_index, device->position_group_index, 1.0f);
                device->door_open_ticks = -3;
            }
        }
    }

    if ((unsigned short)device->pathfinding_object_index != 0xffff)
        device_update_portals(object_index);

    if (definition->type == 0)
    {
        if (device->device_position == 1.0f && !(device->machine_flags.get_unsafe() & 2) && !devices_update_game_is_predicted() && !door_tested)
        {
            device->door_open_ticks++;
            if (((game_time_globals->time + object_index) & 3) == 0 &&
                game_time_globals->tick_length * (float)device->door_open_ticks > definition->call_value)
            {
                if (device->position_group_index != -1)
                    device_group_set_desired_value(object_index, device->position_group_index, 0.0f);
            }
            result = true;
        }
        else
        {
            device->door_open_ticks = 0;
        }
    }

    bool node_matrices_changed = (device->device_flags & 4) != 0;
    if (node_matrices_changed)
    {
        result = true;
        object_compute_node_matrices(object_index);
        device->device_flags &= ~4;
    }

    long parent_object_index = device->parent_object_index;
    bool requires_motion = node_matrices_changed;
    while (!requires_motion && parent_object_index != -1)
    {
        s_object_header *parent_header = object_header_get(parent_object_index);
        object_datum *parent = parent_header->datum;
        if (TEST_BIT(parent_header->flags, 2))
            requires_motion = true;
        parent_object_index = parent->parent_object_index;
    }

    if (requires_motion)
    {
        havok_object_keyframed_rigid_bodies_to_node_transforms(object_index, false);
        device->machine_flags.set_unsafe(device->machine_flags.get_unsafe() | 0x40);
        result = true;
        object_set_requires_motion(object_index);
    }
    else
    {
        havok_object_halt_keyframed_rigid_bodies(object_index);
        device->machine_flags.set_unsafe(device->machine_flags.get_unsafe() & ~0x40);
    }
    return result;
}

static __forceinline float devices_function_ticks_to_seconds(float ticks)
{
    return game_time_globals->tick_length * ticks;
}
static __forceinline s_devices_function_group_index devices_function_group_index(const s_machine_datum *machine)
{
    s_devices_function_group_index result;
    result.value = machine->position_group_index;
    return result;
}
// @address 0x117260 ?device_compute_function_value@@YA_NJJJPAMPA_N1@Z
bool device_compute_function_value(long object_index, long function, long index,
    float *out_value, bool *out_active, bool *out_changed)
{
    device_datum *device = (device_datum *)object_header_get(object_index)->datum;
    s_device_definition *definition = (s_device_definition *)tag_get(0, device->definition_index);
    bool result = true;
    float value = 0.0f;
    switch (function)
    {
    case k_device_function_position:
        value = device->device_position;
        break;
    case k_device_function_power:
        value = device->power;
        break;
    case k_device_function_power_velocity:
        if (device->power_velocity != 0.0f && definition->power_transition_time != 0.0f)
            value = fabsf(device->power_velocity) / definition->power_transition_time;
        break;
    case k_device_function_position_velocity:
        if (device->position_velocity != 0.0f)
        {
            if ((device->device_flags & 8))
            {
                if (!(device->device_flags & 32) && fabsf(device->position_control.cruise_velocity) >= 0.0001f)
                    value = fabsf(device->position_velocity) / fabsf(device->position_control.cruise_velocity);
            }
            else if (fabsf(definition->position_transition_time) >= 0.0001f)
                value = fabsf(device->position_velocity) / definition->position_transition_time;
        }
        break;
    case k_device_function_unusable:
        if (device->power == 0.0f)
            value = 1.0f;
        if (device->object_type == _object_type_machine && device->position_group_index != -1)
        {
            s_machine_datum *machine = (s_machine_datum *)object_header_get(object_index)->datum;
            s_device_group_datum *group = datum_get<s_device_group_datum>(g_device_groups_data, devices_function_group_index(machine).value);
            if ((machine->machine_flags.get_unsafe() & 1)
                || (machine->machine_flags.get_unsafe() & 4)
                || (machine->machine_flags.get_unsafe() & 8)
                || (machine->machine_flags.get_unsafe() & 128))
                value = 1.0f;
            if ((group->flags & 1) != 0 && (group->flags & 2) != 0)
                value = 1.0f;
            if (machine->device_position == 1.0f || (machine->machine_flags.get_unsafe() & 16))
                value = 0.0f;
        }
        break;
    case k_device_function_delay:
        if (definition->delay_time > 0.0f)
            value = devices_function_ticks_to_seconds((float)device->delay_ticks) / definition->delay_time;
        break;
    case k_device_function_not_fully_open:
        value = (float)(device->device_position <= 0.9f);
        break;
    default:
        result = false;
        break;
    }
    value = DEVICES_PIN(value, 0.0f, 1.0f);
    if (result)
    {
        *out_value = value;
        *out_active = value > 0.0f;
    }
    return result;
}


c_device_groups_data_allocator g_device_groups_data_allocator;
