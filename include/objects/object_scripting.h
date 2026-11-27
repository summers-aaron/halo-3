#ifndef OBJECTS_OBJECT_SCRIPTING_H
#define OBJECTS_OBJECT_SCRIPTING_H

#include "cache/restricted_memory_regions.h"

// Functions defined in tu/objectslibxbox/object_scripting.cpp that other units call.

void object_scripting_initialize(void);
void object_scripting_dispose(void);
void object_scripting_initialize_for_new_map(void);
void object_scripting_dispose_from_old_map(void);

bool object_scripting_get_function_variable(long object_index, long function_name, float *value);
void object_scripting_clear_all_function_variables(long object_index);
void __tls_set_g_object_scripting_state_allocator(void *allocation);
void object_scripting_set_function_variable(long object_index, long function_name, float value, float duration);
float object_scripting_get_health(long object_index);
float object_scripting_get_shield(long object_index);
void object_scripting_set_shield_effect(long object_index, float shield_effect, float seconds);
long object_at_marker(long object_index, long marker_name);
void objects_scripting_set_scale(long object_index, float scale, short ticks);
bool object_scripting_get_function_variable(long object_index, long function_variable_index, float *value, long *result_variable_index);
void object_scripting_set_velocity2(long object_index, float forward_velocity, float left_velocity, float up_velocity);

// The allocator of the per-thread scripting state (restricted region 3), defined in object_scripting.cpp.
template<long, void (*)(void *), void (*)(void *), void (*)(void *)>
class t_restricted_allocation_manager;
extern t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_object_scripting_state_allocator> g_object_scripting_state_allocator;

typedef t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_object_scripting_state_allocator> c_object_scripting_state_allocator;

template<>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_object_scripting_state_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits);

#endif // OBJECTS_OBJECT_SCRIPTING_H
