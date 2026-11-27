// objectslibxbox/object_scripting.obj: from the records 0xe4318 0xfccf8 0x32ebb0 0x32ec18 0x32fe28 0x32fe58 0x32fea8 0x32fec0 0x32fef8 0x32ff10 0x330028 0x3301b0 0x330210 0x330270 0x330378 0x330458 0x330508 0x3305f8 0x330698 0x3306b0
// @flags /TP /O2
// @address 0x11df72c g_object_scripting_state
// @address 0xac748 ?object_try_and_get_and_verify_type@@YAPAXJK@Z
// @address 0x1d6bb0 ?restricted_region_free_member@@YAXJJ@Z
// @address 0x32ebb0 ?__tls_set_g_object_scripting_state_allocator@@YAXPAX@Z
// @address 0x32ec18 ?object_early_movers_dispose@@YAXXZ
// @address 0xe4318 ?object_scripting_get_function_variable@@YA_NJJPAM@Z
// @address 0x330028 ?object_scripting_set_function_variable@@YAXJJMM@Z
// @address 0xfccf8 ?object_scripting_clear_all_function_variables@@YAXJ@Z
// @address 0x32fe28 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_object_early_movers_globals_allocator@@YAXPAX@Z@@UAAXPAX@Z
// @address 0x330698 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_object_scripting_state_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
// @address 0x3306b0 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_object_scripting_state_allocator@@YAXPAX@Z@@UAAXPAX@Z
// @address 0x32fef8 ?object_scripting_dispose_from_old_map@@YAXXZ
// @address 0x3301b0 ?object_scripting_get_health@@YAMJ@Z
// @address 0x330210 ?object_scripting_get_shield@@YAMJ@Z
// @address 0x330270 ?object_scripting_set_shield_effect@@YAXJMM@Z
// @address 0x330378 ?object_at_marker@@YAJJJ@Z
// @address 0x32fec0 ?object_scripting_initialize_for_new_map@@YAXXZ
// @address 0x32fe58 ?object_scripting_initialize@@YAXXZ
#include "math/matrix_math_types.h"
#include "math/real_math.h"
#include "objects/object_placement.h"
#include "objects/object_datum.h"
#include <math.h>
#include "cache/restricted_memory_regions.h"
#include "objects/objects.h"
#include "hs/hs_runtime.h"
#include "objects/object_scripting.h"
#include "objects/object_early_movers.h"
#include "memory/tls_globals.h"
#include "multithreading/thread_tls.h"
#include "game/game_time_inline.h"
#include "objects/object_header.h"
#include "objects/object_early_movers_tls.h"
#include "math/real_math_types.h"
#include "objects/object_scripting_tls.h"
#include "cache/restricted_memory_layout.h"
#include "objects/object_scripting_state_types.h"
#include "cache/restricted_memory_registration.h"

extern "C" {
__declspec(thread) s_object_scripting_state *g_object_scripting_state;
}

static __forceinline object_datum *object_scripting_get_and_verify_type(long object_index, unsigned long type_mask)
{
    return (object_datum *)object_get_and_verify_type(object_index, type_mask);
}

// 0xe4318
bool object_scripting_get_function_variable(long object_index, long function_name, float *value)
{
    bool result = false;
    object_datum *object = object_scripting_get_and_verify_type(object_index, ~0UL);
    if ((bool)((object->physics_flags >> 11) & 1)) {
        long variable_index;
        result = object_scripting_get_function_variable(object_index, function_name, value, &variable_index);
    }
    return result;
}

void object_scripting_clear_all_function_variables(long object_index)
{
    if (object_index != -1) {
        object_datum *object = object_scripting_get_and_verify_type(object_index, ~0UL);
        if ((bool)((object->physics_flags >> 11) & 1)) {
            for (unsigned long variable_index = 0; variable_index < 32; variable_index++) {
                if (g_object_scripting_state->variable_object_indices[variable_index] == object_index)
                    g_object_scripting_state->variable_object_indices[variable_index] = -1;
            }
            object->m_physics_flags.set(static_cast<e_object_physics_flags>(11), false);
        }
    }
}

void __tls_set_g_object_scripting_state_allocator(void *allocation)
{
    g_object_early_movers_globals = (s_owner_table *)allocation;
}

void object_early_movers_dispose(void)
{
    g_object_early_movers_globals = 0;
}

// 0x32fe28
template<> void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_object_early_movers_globals_allocator>::deallocate(void *address)
{
    g_object_early_movers_globals = 0;
    restricted_region_free_member(3, m_member_index);
}

// 0x32fe58
void object_scripting_initialize(void)
{
    void *state = (&g_object_scripting_state_allocator)
                      ->reserve_memory("object scripting", 0, 0x304, 0);
    g_object_scripting_state = (s_object_scripting_state *)state;
    ((s_object_scripting_state *)state)->map_initialized = false;
}

// 0x32fea8
// @address 0x32fea8 ?object_scripting_dispose@@YAXXZ
void object_scripting_dispose(void)
{
    g_object_scripting_state = 0;
}

// 0x32fec0
void object_scripting_initialize_for_new_map(void)
{
    for (unsigned long variable_index = 0; variable_index < 32; variable_index++) {
        g_object_scripting_state->variable_object_indices[variable_index] = -1;
    }
    g_object_scripting_state->map_initialized = true;
}

// 0x32fef8
void object_scripting_dispose_from_old_map(void)
{
    g_object_scripting_state->map_initialized = false;
}

// @address 0x32ff10 ?object_scripting_get_function_variable@@YA_NJJPAMPAJ@Z
// match: 0x32ff10: __restrict keeps the *result_variable_index store hoisted; without it stw r10,0(r6) sinks below the loop setup
bool object_scripting_get_function_variable(long object_index, long function_variable_index, float *value, long *__restrict result_variable_index)
{
    bool result = false;
    *result_variable_index = -1;
    if (object_index != -1) {
        s_object_scripting_state *state = g_object_scripting_state;
        for (unsigned long variable_index = 0; variable_index < 32; variable_index++) {
            if (state->variable_object_indices[variable_index] == object_index &&
                state->function_variables[variable_index].variable_index == function_variable_index)
            {
                float start_value = state->function_variables[variable_index].start_value;
                float end_value = state->function_variables[variable_index].end_value;
                long start_time = state->function_variables[variable_index].start_time;
                long duration = state->function_variables[variable_index].duration;
                long elapsed = game_time_get() - start_time;
                if (elapsed >= duration || duration == 0) {
                    *value = end_value;
                } else {
                    float fraction = (float)elapsed / (float)duration;
                    *value = start_value + (end_value - start_value) * fraction;
                }
                *result_variable_index = variable_index;
                result = true;
                break;
            }
        }
    }
    return result;
}

// 0x330028
void object_scripting_set_function_variable(long object_index, long function_name, float value, float duration)
{
    float duration_seconds = duration / 30.0f;
    if (object_index != -1 && function_name != 0 && function_name != -1) {
        long working_variable_index = -1;
        float current_value;
        float start_value = object_scripting_get_function_variable(object_index, function_name, &current_value,
            &working_variable_index) ? current_value : value;
        if (working_variable_index == -1) {
            long oldest_time = 0x7fffffff;
            long oldest_index = -1;
            for (unsigned long variable_index = 0; variable_index < 32; variable_index++) {
                if (g_object_scripting_state->variable_object_indices[variable_index] == -1) {
                    working_variable_index = variable_index;
                    break;
                } else if (g_object_scripting_state->function_variables[variable_index].start_time < oldest_time) {
                    oldest_time = g_object_scripting_state->function_variables[variable_index].start_time;
                    oldest_index = variable_index;
                }
            }
            if (working_variable_index == -1) working_variable_index = oldest_index;
        }
        object_datum *object = object_scripting_get_and_verify_type(object_index, ~0UL);
        g_object_scripting_state->variable_object_indices[working_variable_index] = object_index;
        g_object_scripting_state->function_variables[working_variable_index].variable_index = function_name;
        g_object_scripting_state->function_variables[working_variable_index].start_value = start_value;
        g_object_scripting_state->function_variables[working_variable_index].end_value = value;
        g_object_scripting_state->function_variables[working_variable_index].start_time = game_time_get();
        g_object_scripting_state->function_variables[working_variable_index].duration =
            game_seconds_to_ticks_round(duration_seconds > 0.0f ? duration_seconds : 0.0f);
        object->m_physics_flags.set(static_cast<e_object_physics_flags>(11), true);
    }
}

// 0x3301b0
float object_scripting_get_health(long object_index)
{
    object_datum *object = (object_datum *)object_try_and_get_and_verify_type(object_index, ~0UL);
    float result = -1.0f;
    if (object) {
        result = (bool)((object->damage_flags >> 2) & 1) ? 0.0f : object->body_vitality;
    }
    return result;
}

// 0x330210
float object_scripting_get_shield(long object_index)
{
    object_datum *object = (object_datum *)object_try_and_get_and_verify_type(object_index, ~0UL);
    float result = -1.0f;
    if (object) {
        result = (bool)((object->damage_flags >> 2) & 1) ? 0.0f : object->shield_vitality;
    }
    return result;
}

// 0x330270
void object_scripting_set_shield_effect(long object_index, float shield_effect, float seconds)
{
    if (object_index != -1) {
        object_datum *object = object_scripting_get_and_verify_type(object_index, ~0UL);
        if (shield_effect == 0.0f && seconds == 0.0f) {
            object->m_damage_flags.set(static_cast<e_object_damage_flags>(13), false);
            object->current_shield_damage = 0.0f;
            object->shield_damage = 0.0f;
            object->shield_decay_timer = -1;
        } else {
            long ticks = game_seconds_to_ticks_round(seconds);
            object->shield_decay_timer = (signed char)(ticks > 127 ? 127 : ticks);
            if (object->shield_decay_timer == 0) {
                object->current_shield_damage = shield_effect;
                object->shield_damage = 0.0f;
            } else {
                object->shield_damage = (shield_effect - object->current_shield_damage) / (float)object->shield_decay_timer;
            }
            object->m_damage_flags.set(static_cast<e_object_damage_flags>(13), true);
        }
    }
}

// 0x330378
// @address 0x330378 ?object_at_marker@@YAJJJ@Z
long object_at_marker(long object_index, long marker_name)
{
    if (object_index != -1) {
        object_datum *object = object_scripting_get_and_verify_type(object_index, ~0UL);
        long child_index = object->first_child_object_index;
        object_marker marker;
        internal_object_get_markers_by_string_id(object_index, marker_name, &marker, 1, false);
        while (child_index != -1) {
            object_datum *child = object_scripting_get_and_verify_type(child_index, ~0UL);
            if (child->parent_node_index == marker.node_index &&
                distance3d(&marker.node_to_marker.position, &child->position) < 1.0f) {
                return child_index;
            }
            child_index = child->next_object_index;
        }
    }
    return -1;
}

// 0x330458
// @address 0x330458 ?objects_scripting_set_scale@@YAXJMF@Z
void objects_scripting_set_scale(long object_index, float scale, short ticks)
{
    if (object_index != -1) {
        object_datum *object = object_scripting_get_and_verify_type(object_index, ~0UL);
        long child_index = object->first_child_object_index;
        object_set_scale(object_index, scale, (float)ticks / 30.0f, true);
        while (child_index != -1) {
            object_datum *child = object_scripting_get_and_verify_type(child_index, ~0UL);
            objects_scripting_set_scale(child_index, scale, ticks);
            child_index = child->next_object_index;
        }
    }
}

static __forceinline real_vector3d *object_scripting_velocity_cross(const real_vector3d *a, const real_vector3d *b, real_vector3d *r)
{
    float i = a->j * b->k - a->k * b->j;
    float k = a->i * b->j - a->j * b->i;
    float j = a->k * b->i - a->i * b->k;
    r->i = i;
    r->j = j;
    r->k = k;
    return r;
}

static __forceinline real_vector3d *object_scripting_velocity_scale(const real_vector3d *v, float s, real_vector3d *r)
{
    r->i = v->i * s;
    r->j = v->j * s;
    r->k = v->k * s;
    return r;
}

static __forceinline real_vector3d *object_scripting_velocity_add(const real_vector3d *a, const real_vector3d *b, real_vector3d *r)
{
    r->i = a->i + b->i;
    r->j = a->j + b->j;
    r->k = a->k + b->k;
    return r;
}

// 0x330508
// @address 0x330508 ?object_scripting_set_velocity2@@YAXJMMM@Z
void object_scripting_set_velocity2(long object_index, float forward_velocity, float left_velocity, float up_velocity)
{
    if (object_index != -1) {
        object_datum *object = object_scripting_get_and_verify_type(object_index, ~0UL);
        real_vector3d left, forward, up, velocity;
        object_scripting_velocity_cross(&object->up, &object->forward, &left);
        object_scripting_velocity_scale(&object->forward, forward_velocity, &forward);
        object_scripting_velocity_scale(&left, left_velocity, &left);
        object_scripting_velocity_scale(&object->up, up_velocity, &up);
        object_scripting_velocity_add(&forward, &left, &velocity);
        object_scripting_velocity_add(&velocity, &up, &velocity);
        ::object_set_velocities(object_index, &velocity, 0);
    }
}

// @address 0x3305f8 ?reserve_memory@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_object_scripting_state_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
template<>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_object_scripting_state_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
  int member_index = restricted_memory_register_member(&g_restricted_regions[3], name, type, size, 0,
      __tls_set_g_hs_runtime_globals_allocator, 0, 0);
  unsigned int offset = g_restricted_regions[3].get_member_offset(member_index);
  void *address = restricted_region_get_base_address(3) + offset;
  int owner = get_current_thread_index();
  m_thread_id = owner;
  m_member_index = member_index;
  g_object_scripting_state = (s_object_scripting_state *)address;
  return restricted_region_get_base_address(3) + g_restricted_regions[3].get_member_offset(member_index);
}

// 0x330698
template<> void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_object_scripting_state_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// 0x3306b0
template<> void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_object_scripting_state_allocator>::deallocate(void *address)
{
    __tls_set_g_hs_runtime_globals_allocator(0);
    restricted_region_free_member(3, m_member_index);
}

c_object_scripting_state_allocator g_object_scripting_state_allocator;
