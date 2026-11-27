// objectslibxbox/vehicle_motor_program.obj: from the records 0x38ed38 0x38ed88 0x38ee70 0x38eec0 0x38eef8 0x38f020 0x38f058 0x38f1f0 0x38f258 0x38f3f0 0x38f458 0x38f5f8 0x38f698 0x38f6d0 0x38f7c0 0x38f9e0 0x38feb0 0x390358 0x390550
// @flags /TP /O2 /Oc
#include "units/vehicle_utilities.h"
#include "objects/object_types.h"
#include "units/unit_weapon_config_types.h"
#include "objects/object_datum.h"
#include "objects/object_placement.h"
#include "units/vehicle_types.h"
#include "units/unit_types.h"
#include "units/motor_task.h"
#include "units/motor_program_state.h"
#include "units/motor_request.h"
// @address 0x38ed88 ?vehicle_surge_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
// @address 0x38ee70 ?vehicle_surge_update@@YA_NJPAVc_motor_task@@@Z
// @address 0x38eec0 ?vehicle_surge_finished@@YA_NJPAVc_motor_task@@@Z
// @address 0x38eef8 ?unit_phase_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
// @address 0x38f020 ?unit_phase_update@@YA_NJPAVc_motor_task@@@Z
// @address 0x38f258 ?vehicle_close_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
// @address 0x38f5f8 ?vehicle_animation_task_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
// @address 0x38f698 ?vehicle_animation_finished@@YA_NJPAVc_motor_task@@@Z
// @address 0x38ed38 ?vehicle_program_default_finished@@YA_NJPAVc_motor_task@@@Z
// @address 0x38f1f0 ?vehicle_open_finished@@YA_NJPAVc_motor_task@@@Z
// @address 0x38f3f0 ?vehicle_close_finished@@YA_NJPAVc_motor_task@@@Z
// @address 0x38f6d0 ?vehicle_program_activate@@YA_NJ@Z
// @address 0x38f458 ?vehicle_flip_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
// @address 0xa1ad0 ?vehicle_is_flipped@@YA_NJ@Z
// @address 0x10f7e8 ?damage_compute_damage_owner_from_object_index@@YAXJPAUs_damage_owner@@@Z
#include <math.h>
#include "cseries/cseries.h"
#include "cseries/bit_operations.h"
#include "math/matrix_math.h"
#include "game/game_time_types.h"
#include "motor/motor_animation.h"
#include "objects/objects.h"
#include "sound/game_sound.h"
#include "objects/damage.h"
#include "objects/damage_owner.h"
#include "physics/havok.h"
#include "physics/havok_vehicle_physics.h"
#include "physics/havok_vehicle_physics_definitions.h"
#include "units/vehicles.h"
#include "animations/animation_id.h"
#include "motor/motor_system.h"
#include "math/matrix_math_types.h"
#include "phys/phys_dynamics.h"
#include "cache/cache_files.h"
#include "physics/havok_component.h"
#include "physics/collision_result_types.h"
#include "animations/animation_control_data.h"
#include "physics/collisions.h"
#include "physics/havok_utilities.h"
#include "math/matrix_math_constants.h"
#include "scenario/scenario.h"

#include "memory/data_types.h"
#include "memory/tls_data_arrays.h"

#include "tag_files/tag_types.h"


#include "physics/havok_contact_point_iterator.h"
#include "units/vehicle_datum.h"
#include "units/vehicle_definitions.h"
#include "memory/tls_globals.h"
#include "physics/havok_component_class.h"


struct s_vehicle_phase_motor_state : c_motor_program_state {
    bool active;
    short duration;
    real_point3d position;
};

#include "game/game_time_access.h"

// @address 0xdd5b0 ?motor_task_allow@@YA_NJW4e_action@@F@Z
// @address 0xdd7a8 ?motor_task_new@@YAPAVc_motor_task@@JW4e_motor_resource@@JJW4e_motor_task@@W4e_action@@@Z

/* the vehicle-type component embedded in the vehicle datum (0x458) */
#include "units/vehicle_type_component_internals.h"


/* the block at datum + motor_program_state.offset */
struct s_vehicle_motor_state : c_motor_program_state {
    unsigned short animation_state;
    union { unsigned char surge_active; bool reset_throttle; };
};


/* the physics block of the vehicle tag definition (tag_instances[datum->definition_index] + 0x388) */

/* 0x38f258: the vehicle object and its close motor block */
struct s_vehicle_close_object : object_datum {
    _unit_datum unit;
};
struct s_vehicle_close_motor : c_motor_program_state {
    unsigned short closed;
};


#include "animations/animation_manager.h"
#include "animations/suspension_animation_iterator.h"
#include "physics/havok_ray_test_types.h"
#include "physics/collision_types.h"
#include "objects/object_header.h"
#include "math/real_math_constants.h"
#include "physics/havok_component_class.h"
#include "units/unit_passenger_iterator.h"
#include "game/game_globals.h"
#include "game/game_globals_storage.h"
#include "math/real_math.h"
#include "animations/animation_state_change_settings.h"
#include "game/falling_damage_types.h"
#include "objects/damage_data.h"
#include "memory/data.h"
#include "tag_files/tag_groups.h"
static __forceinline s_vehicle_motor_state *vehicle_motor_program_state_get(vehicle_datum *datum)
{
    return (s_vehicle_motor_state *)((char *)datum + datum->unit.motor_program.offset);
}

static __forceinline long vehicle_motor_program_seconds_to_ticks(float seconds)
{
    return (long)(seconds * game_time_globals->ticks_per_second + 0.5f);
}

// 0x38ed88
e_motor_result vehicle_surge_submit(long object_index, const c_motor_request *request)
{
    long result = 0;
    long action = request->action;
    if (motor_task_allow(object_index, (e_action)action, 15)) {
        c_motor_task *task = motor_task_new(object_index, _motor_resource_0, 0, 15, (e_motor_task)request->m_task, (e_action)action);
        if (task) {
            vehicle_datum *datum = (vehicle_datum *)object_get(object_index);
            vehicle_motor_program_state_get(datum)->surge_active = true;
            task->duration = vehicle_motor_program_seconds_to_ticks(request->m_surge.duration);
            result = 2;
        }
    }
    return (e_motor_result)result;
}

// 0x38ee70
bool vehicle_surge_update(long object_index, c_motor_task *motor_task)
{
    vehicle_datum *datum = (vehicle_datum *)object_get(object_index);
    datum->unit.control_flags |= 0x2000;
    short remaining = (short)(motor_task->duration - 1);
    if (remaining >= 0) {
        motor_task->duration = remaining;
        return true;
    }
    motor_task->duration = remaining;
    return false;
}

// 0x38eec0
bool vehicle_surge_finished(long object_index, c_motor_task *motor_task)
{
    vehicle_datum *datum = (vehicle_datum *)object_get(object_index);
    vehicle_motor_program_state_get(datum)->surge_active = 0;
    return true;
}

// 0x38eef8
e_motor_result unit_phase_submit(long object_index, const c_motor_request *request)
{
    long result = 0;
    long action = request->action;
    if (motor_task_allow(object_index, (e_action)action, 15)) {
        c_motor_task *task = motor_task_new(object_index, _motor_resource_0, 0, 15, (e_motor_task)request->m_task, (e_action)action);
        if (task) {
            vehicle_datum *datum = (vehicle_datum *)object_get(object_index);
            s_vehicle_phase_motor_state *state = (s_vehicle_phase_motor_state *)((char *)datum + datum->unit.motor_program.offset);
            if (state->set_program(object_index, _motor_program_phase)) {
                state = (s_vehicle_phase_motor_state *)((char *)datum + datum->unit.motor_program.offset);
                state->duration = (short)vehicle_motor_program_seconds_to_ticks(request->m_phase.duration);
                state->position = request->m_phase.position;
                state->active = true;
            }
            result = 2;
        }
    }
    return (e_motor_result)result;
}

// 0x38f020
bool unit_phase_update(long object_index, c_motor_task *motor_task)
{
    vehicle_datum *datum = (vehicle_datum *)object_get(object_index);
    return vehicle_motor_program_state_get(datum)->program == 12;
}


// @address 0x3577d8 ?motor_task_start_animation@@YAPAVc_motor_task@@JW4e_motor_task@@W4e_action@@JJW4e_unit_weapon_config@@FM_N@Z

static __forceinline s_vehicle_close_object *vehicle_close_object_get(long object_index)
{
    return (s_vehicle_close_object *)object_get(object_index);
}

static __forceinline void vehicle_close_go_to_last_frame_of_animation(long object_index, long state)
{
    if (object_index != -1) {
        s_vehicle_close_object *object = vehicle_close_object_get(object_index);
        unsigned short animation_offset = (unsigned short)object->animation.offset;
        if (animation_offset != 0xffff) {
            c_animation_manager *animation = (c_animation_manager *)((char *)object + (short)animation_offset);
            if (animation->get_state_name() == state)
                animation->set_state_position_to_last_frame();
        }
    }
}

// 0x38f258
e_motor_result vehicle_close_submit(long object_index, const c_motor_request *motor_request)
{
    const c_motor_request *request = (const c_motor_request *)motor_request;
    e_motor_result result = _motor_result_none;
    s_vehicle_close_object *object = vehicle_close_object_get(object_index);
    s_vehicle_close_motor *state = (s_vehicle_close_motor *)((char *)object + object->unit.motor_program.offset);
    if (state->closed == 1) {
        result = _motor_result_pending;
    } else if (motor_task_recursive_start_animation(object_index, _motor_task_close, (e_action)request->action, 97, request->force())) {
        result = _motor_result_success;
        if (request->m_open.immediate == 1) {
            vehicle_close_go_to_last_frame_of_animation(object_index, 97);
            for (long child = object->first_child_object_index; child != -1; child = vehicle_close_object_get(child)->next_object_index)
                vehicle_close_go_to_last_frame_of_animation(child, 97);
            state->closed = 1;
            result = _motor_result_pending;
        } else {
            if (request->m_open.pause == 1) {
                ((c_animation_manager *)((char *)object + object->animation.offset))->pause_state_channel(true);
                state->closed = 1;
            } else {
                ((c_animation_manager *)((char *)object + object->animation.offset))->pause_state_channel(false);
                state->closed = 0;
            }
        }
    }
    return result;
}


// 0x38f5f8
e_motor_result vehicle_animation_task_submit(long object_index, const c_motor_request *motor_request)
{
    const c_motor_request *request = (const c_motor_request *)motor_request;
    bool force = request->force();
    long mode = request->m_animation.mode;
    float interpolation_time = request->m_animation.interpolation;
    long impulse = request->m_animation.animation;
    long action = request->action;
    e_motor_result result = _motor_result_none;
    vehicle_datum *datum = (vehicle_datum *)object_get(object_index);
    s_vehicle_motor_state *state = vehicle_motor_program_state_get(datum);
    if (motor_task_start_animation(object_index, _motor_task_animation, (e_action)action, impulse, mode, _unit_weapon_config_none, 0,
        interpolation_time, force)) {
        result = _motor_result_success;
        state->animation_state = 4;
    }
    return result;
}

// 0x38f698
bool vehicle_animation_finished(long object_index, c_motor_task *motor_task)
{
    vehicle_datum *datum = (vehicle_datum *)object_get(object_index);
    vehicle_motor_program_state_get(datum)->animation_state = 3;
    return true;
}

// 0x38ed38
bool vehicle_program_default_finished(long object_index, c_motor_task *motor_task)
{
    s_vehicle_close_object *object = vehicle_close_object_get(object_index);
    ((c_animation_manager *)((char *)object + object->animation.offset))->pause_state_channel(true);
    return false;
}

// 0x38f1f0
bool vehicle_open_finished(long object_index, c_motor_task *motor_task)
{
    s_vehicle_close_object *object = vehicle_close_object_get(object_index);
    s_vehicle_motor_state *state = (s_vehicle_motor_state *)((char *)object + object->unit.motor_program.offset);
    ((c_animation_manager *)((char *)object + object->animation.offset))->pause_state_channel(true);
    state->animation_state = 3;
    return true;
}

// 0x38f3f0
bool vehicle_close_finished(long object_index, c_motor_task *motor_task)
{
    s_vehicle_close_object *object = vehicle_close_object_get(object_index);
    s_vehicle_motor_state *state = (s_vehicle_motor_state *)((char *)object + object->unit.motor_program.offset);
    ((c_animation_manager *)((char *)object + object->animation.offset))->pause_state_channel(true);
    state->animation_state = 1;
    return true;
}

/* 0x38f458: the vehicle flip motor program */

static __forceinline vehicle_datum *vehicle_flip_object_get(long object_index)
{
    return (vehicle_datum *)object_get(object_index);
}

static __forceinline real_vector3d *vehicle_flip_vector_from_points(const real_point3d *a, const real_point3d *b, real_vector3d *v)
{
    v->i = b->x - a->x;
    v->j = b->y - a->y;
    v->k = b->z - a->z;
    return v;
}

static __forceinline real_vector3d *vehicle_flip_cross_product3d(const real_vector3d *a, const real_vector3d *b, real_vector3d *v)
{
    float i = a->j * b->k - a->k * b->j;
    float k = a->i * b->j - a->j * b->i;
    float j = a->k * b->i - a->i * b->k;
    v->i = i;
    v->j = j;
    v->k = k;
    return v;
}

static const float vehicle_flip_zero(0.0f);

static __forceinline void vehicle_flip_set_flag(unsigned short *flags, long bit, bool value)
{
    if (value)
        *flags |= 1 << bit;
    else
        *flags &= ~(1 << bit);
}

// 0x38f458
e_motor_result vehicle_flip_submit(long object_index, const c_motor_request *motor_request)
{
    e_motor_result result = _motor_result_none;
    vehicle_datum *vehicle = vehicle_flip_object_get(object_index);
    if (vehicle->object.type == 1) {
        if (vehicle_is_flipped(object_index)) {
            vehicle_datum *owner = 0;
            long owner_index = motor_request->m_flip.owner_index;
            if (owner_index != -1) {
                owner = vehicle_flip_object_get(owner_index);
                owner->unit.last_vehicle_index = object_index;
                owner->unit.last_vehicle_exit_time = game_time_globals->time;
            }
            if (fabs(vehicle->object.forward.k) > 0.70710677f) {
                vehicle->vehicle.flip_direction = (vehicle->object.forward.k < vehicle_flip_zero) + 3;
            } else if (owner_index != -1) {
                real_matrix4x3 vehicle_matrix, owner_matrix;
                real_vector3d direction;
                vehicle_flip_vector_from_points(&::object_get_world_matrix(owner_index, &owner_matrix)->position,
                    &::object_get_world_matrix(object_index, &vehicle_matrix)->position, &direction);
                const real_vector3d up = { vehicle_flip_zero, vehicle_flip_zero, 1.0f };
                vehicle_flip_cross_product3d(&up, &direction, &direction);
                vehicle->vehicle.flip_direction = (dot_product3d(&direction, &vehicle->object.forward) > vehicle_flip_zero) + 1;
            } else {
                vehicle->vehicle.flip_direction = 1;
            }
            vehicle_flip_set_flag(&vehicle->vehicle.flags, 5, true);
            vehicle->vehicle.flip_time = 0;
            s_damage_owner damage;
            damage_compute_damage_owner_from_object_index(motor_request->m_flip.owner_index, &damage);
            object_set_damage_owner(object_index, &damage, false);
            result = _motor_result_pending;
        }
    }
    return result;
}

/* the vehicle tag definition (tag_instances[datum->definition_index]); flags bit 0 at 0x388 */

// @address 0x33e620 ?motor_system_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
// @address 0x357718 ?motor_task_set_mode_and_state@@YAPAVc_motor_task@@JW4e_motor_task@@W4e_action@@JJMFJJ_N@Z

// 0x38f6d0
bool vehicle_program_activate(long object_index)
{
    vehicle_datum *datum = (vehicle_datum *)object_get(object_index);
    vehicle_definition *definition =
        (vehicle_definition *)tag_get(0, datum->object.definition_index);
    if (definition->havok_vehicle_physics.flags & 1)
        object_set_at_rest(object_index, true);
    c_motor_request update;
    update.setup_open(_action_none, true, false);
    if (motor_system_submit(object_index, &update) == _motor_result_none)
        motor_task_set_mode_and_state(object_index, _motor_task_default, _action_none, 1, 12, 0.0f, 1, 0, 31, false);
    return true;
}


static __forceinline void vehicle_motor_program_open_pause(s_vehicle_close_object *object, s_vehicle_motor_state *state, unsigned char pause)
{
    if (pause == 1) {
        c_animation_manager *animation = (c_animation_manager *)((char *)object + object->animation.offset);
        state->animation_state = 3;
        animation->pause_state_channel(true);
    } else {
        c_animation_manager *animation = (c_animation_manager *)((char *)object + object->animation.offset);
        animation->pause_state_channel(false);
        state->animation_state = 2;
    }
}
// @address 0x38f058 ?vehicle_open_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
e_motor_result vehicle_open_submit(long object_index, const c_motor_request *motor_request)
{
    e_motor_result result = _motor_result_none;
    s_vehicle_close_object *object = vehicle_close_object_get(object_index);
    s_vehicle_motor_state *state = (s_vehicle_motor_state *)((char *)object + object->unit.motor_program.offset);
    if (state->animation_state == 3) {
        result = _motor_result_pending;
    } else if (motor_task_recursive_start_animation(object_index, _motor_task_open, (e_action)motor_request->action, 96, motor_request->force())) {
        result = _motor_result_success;
        if (motor_request->m_open.immediate == 1) {
            vehicle_close_go_to_last_frame_of_animation(object_index, 96);
            for (long child = object->first_child_object_index; child != NONE; child = vehicle_close_object_get(child)->next_object_index)
                vehicle_close_go_to_last_frame_of_animation(child, 96);
            state->animation_state = 3;
            result = _motor_result_pending;
        } else {
            vehicle_motor_program_open_pause(object, state, motor_request->m_open.pause);
        }
    }
    return result;
}

// @rdata 0x8202490c
static const float vehicle_motor_program_suspension_epsilon = 0.0001f;
extern "C" {

}

static __forceinline bool vehicle_motor_program_suspension_ray_cast(long component_index, s_havok_ray_test *ray)
{
    bool hit = false;
    if (component_index != NONE) {
        c_havok_component *component = datum_get<c_havok_component>((s_data_array *)g_havok_component_data, component_index);
        if ((bool)((component->m_flags >> 3) & 1) && component->m_aabb_phantom)
            hit = havok_ray_cast_in_aabb_phantom(component->m_aabb_phantom, ray, NONE, NONE);
    }
    return hit;
}
static __forceinline void vehicle_motor_program_suspension_ray(const real_point3d *position, const real_vector3d *up,
    float extension, float compression, s_havok_ray_test *ray)
{
    ray->start = *position;
    ray->vector = *up;
    ray->start.x += ray->vector.i * extension;
    ray->start.y += ray->vector.j * extension;
    ray->start.z += ray->vector.k * extension;
    float scale = -compression;
    ray->vector.i *= scale;
    ray->vector.j *= scale;
    ray->vector.k *= scale;
}
static __forceinline float vehicle_motor_program_volume_clamp(float value, float minimum, float maximum)
{
    float result;
    if (value > minimum) {
        if (value < maximum) result = value;
        else result = maximum;
    } else result = minimum;
    return result;
}
static __forceinline object_region_information *vehicle_motor_program_suspension_regions(long object_index)
{
    vehicle_datum *vehicle = (vehicle_datum *)object_get(object_index);
    return (object_region_information *)((char *)vehicle + vehicle->object.region_information.offset);
}
// @address 0x38feb0 ?update_suspension@@YA_NJ_N@Z
bool update_suspension(long object_index, bool update)
{
    bool played_sound = false;
    vehicle_datum *vehicle = (vehicle_datum *)object_get(object_index);
    c_animation_manager *animation = (c_animation_manager *)((char *)vehicle + vehicle->object.animation.offset);
    vehicle_definition *definition =
        (vehicle_definition *)tag_get(0, vehicle->object.definition_index);
    if (!(definition->havok_vehicle_physics.flags & 1)) {
        bool changed = false;
        suspension_animation_iterator iterator;
        float maximum_delta = 0.0f;
        object_region_information *regions = vehicle_motor_program_suspension_regions(object_index);
        while (animation->find_next_vehicle_suspension(&iterator)) {
            long index = iterator.suspension_index;
            unsigned char old_byte = vehicle->vehicle.smoothed_suspension[index];
            float old_value = old_byte == 255 ? 1.0f : (float)old_byte * (1.0f / 255.0f);
            object_marker marker;
            long marker_name = iterator.marker;
            if (internal_object_get_markers_by_string_id(object_index, marker_name, &marker, 1, false) == 1 && marker_name != 0) {
                bool damaged = false;
                for (long i = 0; i < definition->havok_vehicle_physics.friction_points.count; ++i) {
                    s_havok_vehicle_friction_point_definition *friction = &((s_havok_vehicle_friction_point_definition
                        *)definition->havok_vehicle_physics.friction_points.address)[i];
                    if (friction->region_name == iterator.region && friction->destroyed_runtime_region_index != NONE) {
                        if (regions[friction->destroyed_runtime_region_index].state >= friction->destroyed_model_state)
                            damaged = true;
                        break;
                    }
                }
                float extension = damaged ? iterator.scale : iterator.extension;
                float limit = damaged ? iterator.limit : iterator.rate;
                float compression = damaged ? iterator.offset : iterator.compression;
                s_havok_ray_test ray;
                vehicle_motor_program_suspension_ray(&marker.matrix.position, &vehicle->object.up, extension, compression, &ray);
                float value;
                if (marker.node_index == 0) {
                    if (vehicle_motor_program_suspension_ray_cast(vehicle->object.havok_component, &ray)) {
                        float range = compression - limit;
                        if (fabsf(range) < vehicle_motor_program_suspension_epsilon) {
                            value = 0.0f;
                        } else {
                            value = 1.0f - (ray.fraction * compression - limit) / range;
                            value = PIN(value, 0.0f, 1.0f);
                        }
                    } else {
                        value = collision_test_point(k_collision_test_camera, &ray.start, object_index, NONE, 0) ? 1.0f : 0.0f;
                    }
                    vehicle->vehicle.suspension_samples[index] = quantize_real_to_byte_lower_bound(0.0f, 1.0f, value);
                } else {
                    value = 0.0f;
                }
                float delta = value - old_value;
                if (delta > maximum_delta)
                    maximum_delta = delta;
                if (update)
                    value = (value + old_value) * 0.5f;
                long new_byte = quantize_real_to_byte_lower_bound(0.0f, 1.0f, value);
                if ((long)vehicle->vehicle.smoothed_suspension[index] != (long)new_byte) {
                    changed = true;
                    vehicle->vehicle.smoothed_suspension[index] = (long)new_byte;
                }
            }
        }
        if (definition->suspension_sound.index != NONE && maximum_delta > 0.3f && update) {
            float volume = (maximum_delta - 0.3f) * 1.6666667f;
            float clamped = PIN(volume, 0.0f, 1.0f);
            object_impulse_sound_new(object_index, definition->suspension_sound.index, NONE, global_origin3d, global_forward3d, clamped);
            played_sound = true;
        }
        if (changed)
            object_set_requires_motion(object_index);
    }
    if (update)
        vehicle->vehicle.flags |= 64;
    else
        vehicle->vehicle.flags &= ~64;
    return played_sound;
}

/* 0x38f7c0: pushes the vehicle along its up vector while its parent rises (impulse at 0x764) */
typedef char dp20_layout_5[(offsetof(vehicle_datum, unit.motor_program) == 0x164) ? 1 : -1];
typedef char dp20_layout_6[(offsetof(vehicle_datum, vehicle.upside_down_ticks) == 0x44a) ? 1 : -1];
typedef char dp20_layout_7[(offsetof(s_vehicle_close_object, animation.offset) == 0x150) ? 1 : -1];
typedef char dp20_layout_8[(offsetof(vehicle_datum, object.forward) == 0x60) ? 1 : -1];
typedef char dp20_layout_9[(offsetof(vehicle_datum, vehicle.suspension_samples) == 0x4bc) ? 1 : -1];
typedef char dp20_layout_10[(offsetof(vehicle_datum, vehicle.impulse) == 0x764) ? 1 : -1];

typedef char vehicle_impulse_parent_height_offset[(offsetof(unit_datum, unit.secondary_trigger) == 0x210) ? 1 : -1];

const float k_vehicle_impulse_rise_scale = 35.0f;
const float k_vehicle_impulse_fall_scale = 20.0f;
const float k_vehicle_impulse_maximum = 3.0f;
const float k_vehicle_impulse_decay = 0.9f;
const float k_vehicle_impulse_airborne_decay = 0.25f;



// @address 0x38f7c0 vehicle_impulse_update
extern "C" bool vehicle_impulse_update(long object_index)
{
    bool result = false;
    vehicle_datum *vehicle = (vehicle_datum *)object_get(object_index);
    vehicle_definition *definition =
        (vehicle_definition *)tag_get(0, vehicle->object.definition_index);
    bool enabled = TEST_BIT(definition->flags, 12);
    if (enabled)
    {
        long havok_component_index = vehicle->object.havok_component_index;
        if (havok_component_index != -1)
        {
            float delta = 0.0f;
            long parent_object_index = vehicle->unit.driver_object_index;
            if (parent_object_index != -1)
            {
                unit_datum *parent = (unit_datum *)object_get(parent_object_index);
                float old_parent_height = vehicle->vehicle.parent_height;
                float seconds_per_tick = game_time_globals->tick_length;
                delta = -((parent->unit.secondary_trigger - old_parent_height) * seconds_per_tick);
                vehicle->vehicle.parent_height = parent->unit.secondary_trigger;
            }

            unsigned char airborne_ticks = vehicle->vehicle.airborne_ticks;
            bool airborne = airborne_ticks > 0;
            if (!airborne)
            {
                if (delta > k_real_epsilon)
                {
                    if (vehicle->vehicle.impulse < rasterizer_states_mode_7_depth_bias)
                    {
                        float raised_impulse = delta * k_vehicle_impulse_rise_scale + vehicle->vehicle.impulse;
                        vehicle->vehicle.impulse = raised_impulse;
                        vehicle->vehicle.impulse = PIN(vehicle->vehicle.impulse, -1.0f, k_vehicle_impulse_maximum);
                    }
                }
                else if (delta < rasterizer_states_mode_7_depth_bias)
                {
                    vehicle->vehicle.impulse += delta * k_vehicle_impulse_fall_scale;
                    vehicle->vehicle.impulse = PIN(vehicle->vehicle.impulse, -1.0f, k_vehicle_impulse_maximum);
                }
                else
                {
                    vehicle->vehicle.impulse *= k_vehicle_impulse_decay;
                }
            }
            else
            {
                vehicle->vehicle.impulse *= k_vehicle_impulse_airborne_decay;
            }

            float impulse = vehicle->vehicle.impulse;
            real_vector3d *up = &vehicle->object.up;
            if (fabsf(impulse) >= k_real_epsilon)
            {
                c_havok_component *component = datum_get<c_havok_component>((s_data_array *)g_havok_component_data, havok_component_index);
                if (component->m_rigid_bodies.getSize() > 0)
                {
                    real_vector3d velocity;
                    ((c_havok_component *)component)->get_rigid_body_linear_velocity(0, &velocity);
                    velocity.i += impulse * up->i;
                    velocity.j += impulse * up->j;
                    velocity.k += impulse * up->k;
                    ((c_havok_component *)component)->set_rigid_body_linear_velocity(0, &velocity);
                }
                result = true;
            }
        }
    }
    return result;
}


static __forceinline short vehicle_motor_program_flip_body_index(const c_havok_component *component)
{
    signed char index = component->m_object_postion_controlling_rigid_body_index;
    return VALID_INDEX(index, component->m_rigid_bodies.getSize()) ? index : NONE;
}

// match: 0x38f9e0: precise mode is required for fenv_access(on) to compile (C3198); the pair blocks FP reassociation in the flip-speed inline.
#pragma float_control(precise, on, push)
// match: 0x38f9e0: fenv_access(on) blocks FP reassociation in the flip-speed inline; removing the block reassociates caller fdiv/fmuls (xsame: +103 lines).
#pragma fenv_access(on)
static __forceinline float vehicle_motor_program_flip_speed(float height, float seconds_per_tick, const real_bounds *bounds)
{
    float minimum = bounds->lower;
    float minimum_result = MAX(height / seconds_per_tick * -2.0f, minimum);
    float maximum = bounds->upper;
    return minimum_result < maximum ? MAX(height / seconds_per_tick * -2.0f, minimum) : maximum;
}
// match: 0x38f9e0: restores default FP contraction after the flip-speed inline.
#pragma fenv_access(off)
// match: 0x38f9e0: pops the precise mode pushed above.
#pragma float_control(pop)

static __forceinline real_vector3d *vehicle_motor_program_flip_cross(const real_vector3d *a, const real_vector3d *b, real_vector3d *result)
{
    float aj = 0.0f, ak = 0.0f, bk = 0.0f, bj = 0.0f, bi = 0.0f, ai = 0.0f;
    bk = b->k;
    bj = b->j;
    bi = b->i;
    aj = a->j;
    ak = a->k;
    ai = a->i;
    float i = aj * bk - ak * bj;
    float j = ak * bi - ai * bk;
    float k = ai * bj - aj * bi;
    result->i = i;
    result->j = j;
    result->k = k;
    return result;
}

static __forceinline float vehicle_motor_program_flip_dot(const real_vector3d *a, const real_vector3d *b)
{
    return a->i * b->i + (a->k * b->k + a->j * b->j);
}

static __forceinline void vehicle_motor_program_flip_adjust_axis(
    const real_vector3d *forward, const real_vector3d *up, const float *vertical, real_vector3d *axis)
{
    float aj = 0.0f, ak = 0.0f, bj = 0.0f, bk = 0.0f, ai = 0.0f, bi = 0.0f;
    float i = 0.0f, scale = 0.0f, j = 0.0f, k = 0.0f;
    bk = up->k;
    aj = forward->j;
    ak = forward->k;
    bj = up->j;
    bi = up->i;
    ai = forward->i;
    i = aj * bk - ak * bj;
    j = ak * bi - ai * bk;
    k = ai * bj - aj * bi;
    scale = -*vertical;
    axis->i += scale * i;
    axis->j += scale * j;
    axis->k += scale * k;
}

// @address 0x38f9e0
bool vehicle_update_flipping(long vehicle_index)
{
    bool result = false;
    float up_height;
    vehicle_datum *vehicle = (vehicle_datum *)object_get(vehicle_index);
    const vehicle_definition *definition =
        (const vehicle_definition *)tag_get(0, vehicle->object.definition_index);
    if ((bool)((vehicle->vehicle.flags >> 5) & 1) && vehicle->vehicle.flip_direction &&
        vehicle->vehicle.flip_time < vehicle_motor_program_seconds_to_ticks(2.0f) && !((up_height = vehicle->object.up.k) > 0.9f))
    {
        c_havok_component *component = (c_havok_component *)datum_get<c_havok_component>((s_data_array *)g_havok_component_data,
            vehicle->object.havok_component_index);
        short controlling_body_index = vehicle_motor_program_flip_body_index(component);
        if (controlling_body_index != NONE)
        {
            float direction = (vehicle->vehicle.flip_direction == 2 || vehicle->vehicle.flip_direction == 4) ? 0.3 : -0.3;
            float total_mass = 0.0f;
            float inverse_mass = 0.0f;
            real_point3d center_of_mass = *global_origin3d;
            real_vector3d axis;
            if (vehicle->vehicle.flip_direction == 4 || vehicle->vehicle.flip_direction == 3)
                vehicle_motor_program_flip_cross(&vehicle->object.forward, &vehicle->object.up, &axis);
            else
                axis = vehicle->object.forward;

            float angular_speed = vehicle_motor_program_flip_speed(up_height, game_time_globals->tick_length,
                &definition->havok_vehicle_physics.flipping_angular_velocity) * direction;
            if (vehicle->vehicle.flip_direction == 2 || vehicle->vehicle.flip_direction == 1)
            {
                vehicle_motor_program_flip_adjust_axis(&vehicle->object.forward, &vehicle->object.up, &vehicle->object.forward.k, &axis);
            }

            for (long body_index = 0; body_index < component->m_rigid_bodies.getSize(); body_index++)
            {
                long mass = (long)component->get_rigid_body_mass(body_index);
                real_point3d body_center;
                component->get_rigid_body_center_of_mass(body_index, &body_center);
                float body_mass = (float)mass;
                total_mass += body_mass;
                center_of_mass.x += body_center.x * body_mass;
                center_of_mass.y += body_center.y * body_mass;
                center_of_mass.z += body_center.z * body_mass;
            }
            inverse_mass = 1.0f / total_mass;
            center_of_mass.x *= inverse_mass;
            center_of_mass.y *= inverse_mass;
            center_of_mass.z *= inverse_mass;

            for (long body_index = 0; body_index < component->m_rigid_bodies.getSize(); body_index++)
            {
                real_vector3d angular_velocity;
                real_vector3d linear_velocity;
                component->get_rigid_body_angular_velocity(body_index, &angular_velocity);
                component->get_rigid_body_linear_velocity(body_index, &linear_velocity);
                scale_vector3d(&axis, angular_speed, &angular_velocity);
                real_point3d body_center;
                component->get_rigid_body_center_of_mass(body_index, &body_center);
                const real_vector3d *forward = 0;
                real_vector3d displacement;
                vector_from_points3d(&center_of_mass, &body_center, &displacement);
                real_vector3d normalized_displacement;
                normalized_displacement.i = 0.0f;
                normalized_displacement = displacement;
                float distance = normalize3d(&normalized_displacement);
                if (distance != 0.0f)
                {
                    vehicle_motor_program_flip_cross(&axis, &normalized_displacement, &linear_velocity);
                    scale_vector3d(&linear_velocity, distance * angular_speed, &linear_velocity);
                }
                if (vehicle_get_type(vehicle_index) == _vehicle_type_human_tank)
                {
                    forward = &vehicle->object.forward;
                    float projection = vehicle_motor_program_flip_dot(&linear_velocity, forward);
                    scale_vector3d(forward, projection, &linear_velocity);
                }
                component->set_rigid_body_angular_velocity(body_index, &angular_velocity);
                component->set_rigid_body_linear_velocity(body_index, &linear_velocity);
            }
        }
        vehicle->vehicle.flip_time++;
        result = true;
    }
    else
    {
        vehicle->vehicle.flip_time = 0;
        vehicle->vehicle.flip_direction = 0;
        vehicle_flip_set_flag(&vehicle->vehicle.flags, 5, false);
    }
    return result;
}

static __forceinline bool vehicle_type_vector_is_upsides_down(const real_vector3d *up, float threshold)
{
    return up->k < threshold;
}

/* every vehicle type's own vector_is_upsides_down, dispatched on the vehicle type */
__forceinline bool c_vehicle_type_component::vector_is_upsides_down(long vehicle_index, const real_vector3d *up)
{
    bool result = false;
    switch (vehicle_get_type(vehicle_index))
    {
    case _vehicle_type_alien_fighter:
        result = vehicle_type_vector_is_upsides_down(up, 0.075f);
        break;
    case _vehicle_type_mantis:
        result = vehicle_type_vector_is_upsides_down(up, 0.075f);
        break;
    case _vehicle_type_guardian:
        result = false;
        break;
    case _vehicle_type_human_tank:
        result = vehicle_type_vector_is_upsides_down(up, 0.0f);
        break;
    case _vehicle_type_human_jeep:
        result = vehicle_type_vector_is_upsides_down(up, 0.0f);
        break;
    case _vehicle_type_human_boat:
        result = vehicle_type_vector_is_upsides_down(up, 0.0f);
        break;
    case _vehicle_type_alien_scout:
        result = vehicle_type_vector_is_upsides_down(up, 0.0f);
        break;
    case _vehicle_type_turret:
        result = vehicle_type_vector_is_upsides_down(up, 0.0f);
        break;
    case _vehicle_type_vtol:
        result = vehicle_type_vector_is_upsides_down(up, 0.0f);
        break;
    case _vehicle_type_chopper:
        result = vehicle_type_vector_is_upsides_down(up, 0.0f);
        break;
    default:
        // match: 0x390358: unreachable default omits the switch bounds guard inlined into the caller; without it the caller grows by 12B.
        __assume(0);
    }
    return result;
}

// @address 0x390358
void compute_airborne_ticks(long vehicle_index, s_havok_vehicle_physics_instance *instance)
{
    vehicle_datum *vehicle = (vehicle_datum *)object_get(vehicle_index);
    const s_havok_vehicle_physics_definition *physics = &(
        (const vehicle_definition *)tag_get(0, vehicle->object.definition_index))
                                                       ->havok_vehicle_physics;
    bool grounded = false;

    if (vehicle->vehicle.airborne_ticks < 255)
        vehicle->vehicle.airborne_ticks++;

    if (instance)
    {
        for (long wheel_index = 0; wheel_index < physics->anti_gravity_points.count; wheel_index++)
        {
            const s_havok_vehicle_anti_gravity_point *wheel = &instance->anti_gravity_points[wheel_index];
            if (wheel->active)
            {
                vehicle->vehicle.airborne_ticks = 0;
                break;
            }
        }
        for (long point_index = 0; point_index < physics->friction_points.count; point_index++)
        {
            const s_friction_vehicle_physics_point_datum *point = &instance->friction_points[point_index];
            if (point->grounded)
            {
                grounded = true;
                break;
            }
        }
        c_havok_contact_point_iterator iterator(vehicle_index, false);
        grounded = grounded | iterator.valid();
    }
    else
    {
        grounded = true;
    }

    if (grounded)
    {
        if (vehicle->vehicle.braking_ticks < 255)
            vehicle->vehicle.braking_ticks++;
        vehicle->vehicle.airborne_ticks = 0;
    }
    else
    {
        vehicle->vehicle.braking_ticks = 0;
    }

    if (grounded && (*((c_vehicle_type_component *)vehicle->vehicle.type_state)).vector_is_upsides_down(vehicle_index, &vehicle->object.up))
    {
        if (vehicle->vehicle.upside_down_ticks < 255)
            vehicle->vehicle.upside_down_ticks++;
    }
    else
    {
        vehicle->vehicle.upside_down_ticks = 0;
    }
}


static __forceinline vehicle_datum *vehicle_motor_program_update_get(long object_index)
{
    return (vehicle_datum *)object_get(object_index);
}
static __forceinline c_animation_manager *vehicle_motor_program_animation_get(vehicle_datum *vehicle)
{
    // match: 0x390550: vehicle_program_update reloads the animation block offset at 0x390e7c after valid().
    const volatile short &offset = vehicle->object.animation.offset;
    return (c_animation_manager *)((char *)vehicle + offset);
}
static __forceinline void vehicle_motor_program_rebuild_phantom(c_havok_component *component)
{
    vehicle_datum *object = vehicle_motor_program_update_get(component->m_object_index);
    if (TEST_BIT(component->m_flags, 9) || TEST_BIT(component->m_flags, 14)) {
        unsigned long object_flags = object->object.physics_flags >> 17;
        if (!bit_test(object_flags, 0)) {
            if (TEST_BIT(component->m_flags, 13) && !TEST_BIT(component->m_flags, 14))
                havok_object_rebuild(component->m_object_index, 0);
        } else if (TEST_BIT(component->m_flags, 14)) {
            havok_object_rebuild(component->m_object_index,
                // match: 0x390550: identity rotate keeps the shift-and-mask extract separate.
                _rotl(object_flags, 0) & 1);
        }
    }
}
static __forceinline e_vehicle_type vehicle_motor_program_definition_type(long definition_index)
{
    const vehicle_definition *definition =
        (const vehicle_definition *)tag_get(0, definition_index);
    for (long type = 0; type < k_vehicle_type_count; type++)
        if (definition->types[type].count)
            return (e_vehicle_type)type;
    return _vehicle_type_none;
}
static __forceinline bool vehicle_motor_program_predicted()
{
    bool predicted = false;
    if (tls_game_globals()->options.game_simulation == 4)
        predicted = true;
    return predicted;
}
static __forceinline bool vehicle_motor_program_animation_active(c_animation_manager *manager)
{
    return manager && manager->valid() && !bit_test(manager->m_flags, 0) &&
        !manager->state_channel_paused();
}
// @address 0x390550
bool vehicle_program_update(long vehicle_index, s_animation_control_data *control_data)
{
    s_animation_control_data control;
    control.set_defaults();
    vehicle_datum *vehicle = vehicle_motor_program_update_get(vehicle_index);
    const vehicle_definition *definition =
        (const vehicle_definition *)tag_get(0, vehicle->object.definition_index);
    bool physics_disabled = vehicle_physics_disabled(vehicle_index);
    bool result = false;
    if (bit_test(vehicle->object.physics_flags, 13) && vehicle->object.havok_component_index != NONE) {
        c_havok_component *component = datum_get<c_havok_component>((s_data_array *)g_havok_component_data, vehicle->object.havok_component_index);
        bool force;
        if (TEST_BIT(vehicle->unit.control_flags, 13))
            force = true;
        else {
            force = false;
            if (bit_test(vehicle->object.physics_flags, 12))
                force = true;
        }
        havok_component_update_phantom_state(component, force);
        vehicle_motor_program_rebuild_phantom(component);
        if (force)
            result = true;
    }
    if (!bit_test(vehicle->object.physics_flags, 9)) {
        result = true;
        vehicle->object.last_motion_time = game_time_globals->time;
    }
    if (!physics_disabled && bit_test(vehicle->object.physics_flags, 7)) {
        if (vehicle->object.parent_object_index != NONE) {
            object_set_at_rest(vehicle_index, true);
        } else if (!TEST_BIT(definition->havok_vehicle_physics.flags, 0)) {
            c_vehicle_type_component *type_component = &(*((c_vehicle_type_component *)vehicle->vehicle.type_state));
            type_component->update_control(vehicle_index);
            result |= vehicle_update_flipping(vehicle_index);
            result |= vehicle_impulse_update(vehicle_index);
            bool at_rest = bit_test(vehicle->object.physics_flags, 9);
            if (at_rest && !(*((c_vehicle_type_component
                *)vehicle_motor_program_update_get(vehicle_index)->vehicle.type_state)).should_override_deactivation(vehicle_index)) {
                if (bit_test(vehicle->vehicle.flags, 6) && vehicle->object.havok_component_index != NONE) {
                    c_havok_component *component = datum_get<c_havok_component>((s_data_array *)g_havok_component_data, vehicle->object.havok_component_index);
                    real_rectangle3d aabb;
                    if (havok_vehicle_physics_get_phantom_aabb(&definition->havok_vehicle_physics, &aabb)) {
                        real_matrix4x3 matrix;
                        object_get_world_matrix(vehicle_index, &matrix);
                        component->move_aabb_phantom(&matrix, &aabb);
                        update_suspension(vehicle_index, false);
                        c_animation_manager *animation = vehicle_motor_program_animation_get(vehicle_motor_program_update_get(vehicle_index));
                        animation->m_interpolator.m_time = 0.0f;
                        animation->m_interpolator.m_flags = 0;
                        animation->m_interpolator.m_duration = 0.0f;
                        object_wake(vehicle_index);
                    }
                }
                if (vehicle_motor_program_definition_type(vehicle->object.definition_index) != _vehicle_type_turret)
                    compute_airborne_ticks(vehicle_index, NULL);
            } else {
                s_havok_vehicle_physics_instance instance;
                if (at_rest && (*((c_vehicle_type_component
                    *)vehicle_motor_program_update_get(vehicle_index)->vehicle.type_state)).should_override_deactivation(vehicle_index))
                    object_set_at_rest(vehicle_index, false);
                result = true;
                real_vector3d velocity;
                object_get_velocities(vehicle_index, &velocity, NULL);
                s_vehicle_motor_state *motor = (s_vehicle_motor_state *)((char *)vehicle + vehicle->unit.motor_offset);
                if (motor->reset_throttle) {
                    vehicle->unit.throttle = *global_zero_vector3d;
                    object_get_orientation(vehicle_index, &vehicle->unit.facing_vector, NULL);
                }
                if (havok_vehicle_physics_instance_new(&instance, vehicle_index, type_component->is_e_braking(vehicle_index), true)) {
                    if (!bit_test(vehicle->vehicle.flags, 2))
                        type_component->update_physics(vehicle_index, &instance);
                    vehicle_update_engine_scales((vehicle_utilities_engine_input *)&(*((c_vehicle_type_component
                        *)vehicle_motor_program_update_get(vehicle_index)->vehicle.type_state)), vehicle_index);
                    update_suspension(vehicle_index, true);
                    compute_airborne_ticks(vehicle_index, &instance);
                    const s_havok_vehicle_physics_definition *physics = &definition->havok_vehicle_physics;
                    float total = 0.0f;
                    for (long i = 0; i < physics->anti_gravity_points.count; i++) {
                        const s_havok_vehicle_anti_gravity_point *point = &instance.anti_gravity_points[i];
                        total += point->compression;
                    }
                    vehicle->vehicle.function_value = physics->anti_gravity_points.count ? total / physics->anti_gravity_points.count : 0.0f;
                }
            }
            if (!vehicle_motor_program_predicted() && tls_game_globals()->options.game_mode != 2 &&
                type_component->kills_riders_at_terminal_velocity(vehicle_index))
            {
                bool invulnerable = bit_test(vehicle->unit.unit_flags, 9);
                const s_falling_damage_definition *globals =
                    (const s_falling_damage_definition *)global_game_globals->falling_damage.address;
                real_vector3d velocity;
                object_get_velocities(vehicle_index, &velocity, NULL);
                if (!invulnerable && velocity.k < -globals->runtime_maximum_falling_velocity) {
                    c_multi_unit_passenger_iterator iterator;
                    iterator.begin(vehicle_index);
                    while (iterator.next()) {
                        if (bit_test(vehicle_motor_program_update_get(iterator.passenger_index)->unit.unit_flags, 9)) {
                            invulnerable = true;
                            break;
                        }
                    }
                    if (!invulnerable) {
                        iterator.begin(vehicle_index);
                        while (iterator.next()) {
                            long passenger_index = iterator.passenger_index;
                            object_datum *passenger = object_get(passenger_index);
                            if ((FLAG(passenger->object_type) & 1) && !bit_test(passenger->damage_flags, 8)) {
                                s_damage_data damage;
                                damage_data_new(&damage, globals->distance_damage.index);
                                damage.reporting_info = 4;
                                damage.flags |= 4;
                                damage_compute_damage_owner_from_object_index(passenger_index, &damage.damage_owner);
                                object_cause_damage(&damage, passenger_index, NONE, NONE, NONE, NULL);
                            }
                        }
                    }
                }
            }
            // match: 0x390550: zero-bit rotate preserves 32-bit GPR value
            if (TEST_BIT(_rotl(_rotl(vehicle->unit.control_flags, 0), 0), 0))
                vehicle->unit.unit_flags |= FLAG(22);
            else
                vehicle->unit.unit_flags &= ~FLAG(22);
            float power = vehicle->unit.powered_seat_power;
            if (bit_test(vehicle->unit.unit_flags, 22)) {
                if (power < 1.0f) {
                    power += game_time_globals->tick_length * definition->powered_seat_power_rate;
                    vehicle->unit.powered_seat_power = power;
                    if (power > 1.0f)
                        power = 1.0f;
                    vehicle->unit.powered_seat_power = power;
                    result = true;
                }
            } else if (power > 0.0f) {
                power -= game_time_globals->tick_length * definition->powered_seat_power_rate;
                vehicle->unit.powered_seat_power = power;
                power = MAX(power, 0.0f);
                vehicle->unit.powered_seat_power = power;
                result = true;
            }
            float function_value;
            bool function_active;
            if (type_component->compute_function_value(vehicle_index, 0x21b, &function_value, &function_active)) {
                bool braking = type_component->is_e_braking(vehicle_index);
                bool damaged = function_value >= definition->damaged_threshold ? true : false;
                bool changed;
                if (braking)
                    changed = damaged != bit_test(vehicle->vehicle.flags, 1);
                else
                    changed = damaged != bit_test(vehicle->vehicle.flags, 0);
                if (changed) {
                    short regions = damaged ? 0xffff : 0;
                    if (braking) {
                        for (long i = 0; i < definition->havok_vehicle_physics.friction_points.count; i++) {
                            const s_havok_vehicle_friction_point_definition *point = &(
                                (const s_havok_vehicle_friction_point_definition *)definition->havok_vehicle_physics.friction_points.address)[i];
                            if (bit_test(point->flags, 4) && point->destroyed_runtime_region_index != NONE)
                                regions &= ~FLAG(point->destroyed_runtime_region_index);
                        }
                    }
                    object_set_model_state_property_per_region(vehicle_index, NONE, 0, regions, false);
                    if (braking) {
                        operation_flag_set(vehicle->vehicle.flags, 1, damaged);
                        operation_flag_set(vehicle->vehicle.flags, 0, false);
                    } else {
                        operation_flag_set(vehicle->vehicle.flags, 1, false);
                        operation_flag_set(vehicle->vehicle.flags, 0, damaged);
                    }
                }
            }
        }
    }
    bool requires_motion = false;
    if (vehicle->object.parent_object_index == NONE) {
        if (vehicle->object.havok_component_index == NONE || !bit_test(vehicle->object.physics_flags, 9))
            requires_motion = true;
    }
    vehicle_datum *animation_object = vehicle_motor_program_update_get(vehicle_index);
    c_animation_manager *manager = (c_animation_manager *)vehicle_motor_program_animation_get(animation_object);
    if (vehicle_motor_program_animation_active(manager)) {
        c_animation_manager *animation = vehicle_motor_program_animation_get(animation_object);
        if (!bit_test(animation->m_flags, 6) || animation->m_current_goal.state != 12)
            requires_motion = true;
    }
    if (requires_motion) {
        result = true;
        object_set_requires_motion(vehicle_index);
    }
    control.set_defaults();
    control.mode = 226;
    control.state = 0x341;
    s_animation_state_change_settings settings;
    s_state_changes_requested changes;
    motor_animation_build_state_change_settings_from_control_data(vehicle_index, &control, &changes, &settings);
    motor_task_execute_state_change_settings(vehicle_index, _motor_task_default, _action_none, &settings, 1, control.moved);
    return result;
}
