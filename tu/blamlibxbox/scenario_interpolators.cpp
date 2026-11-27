//  0x11df76c scenario_interpolator_globals
#include "scenario/scenario_interpolator_globals.h"
// blamlibxbox/scenario_interpolators.obj: from the records 0x12a188 0x242a40 0x242a50 0x242ba8 0x242c50 0x242c98 0x242cb0 0x242cd8 0x242d10 0x242e10 0x242e90 0x242f20 0x242ff0 0x243100 0x243218 0x2432b8 0x2432d0
// @flags /TP /O2 /Oc
#include "tag_files/tag_types.h"
#include "multithreading/thread_tls.h"
#include "memory/tls_globals.h"
// @address 0x242c50 ?scenario_interpolators_initialize@@YAXXZ
// @address 0x242c98 ?scenario_interpolators_dispose@@YAXXZ
// @address 0x242cb0 ?scenario_interpolators_initialize_for_new_map@@YAXXZ
#include "scenario/scenario_tag.h"
#include "scenario/scenario.h"
#include "scenario/scenario_access.h"
#include <string.h>
#include "cache/restricted_memory_regions.h"
#include "game/game_time.h"
#include "scenario/scenario_interpolators.h"
#include "scenario/scenario_interpolators_allocator.h"
#include "scenario/scenario_interpolator_types.h"
#include "math/function_definitions.h"
#include "math/real_math_trigonometry.h"
#include "tag_files/tag_groups.h"
// @address 0x242a50 ?scenario_interpolator_time_fraction_smooth@@YAMM@Z
// @address 0x242ba8 ?scenario_get_interpolator_state@@YAPAUs_scenario_interpolator_state@@JPAJ@Z

#include "cache/restricted_memory_layout.h"



#include <math.h>
#include "cache/restricted_memory.h"
#include "cache/restricted_memory_registration.h"
#include "cache/restricted_memory_regions_inline.h"
static const float scenario_interpolators_pi = 3.1415927f;

#define SCENARIO_INTERPOLATORS_MIN(a, b) ((a) < (b) ? (a) : (b))
#define SCENARIO_INTERPOLATORS_MAX(a, b) ((a) > (b) ? (a) : (b))
#define SCENARIO_INTERPOLATORS_PIN(value, lower, upper) SCENARIO_INTERPOLATORS_MIN(SCENARIO_INTERPOLATORS_MAX(value, lower), upper)
#define SCENARIO_INTERPOLATORS_PIN_LOW(value, lower) ((value) < (lower) ? (lower) : (value))


struct s_scenario_interpolator_tag_element
{
    long name;
    long accelerator_name;
    unsigned long definition_flags;
    c_function_definition function;
    short source_index;
    short other_index;
};


// 0x242a40
void __tls_set_g_scenario_interpolator_globals_allocator(void *allocation)
{
    scenario_interpolator_globals = (s_scenario_interpolator_globals *)allocation;
}

// 0x242a50
float scenario_interpolator_time_fraction_smooth(float fraction)
{
    return SCENARIO_INTERPOLATORS_PIN(
        0.5f - 0.5f * cosine(SCENARIO_INTERPOLATORS_PIN(fraction, 0.0f, 1.0f) * scenario_interpolators_pi), 0.0f,
        1.0f);
}



static __forceinline s_scenario_interpolator_globals *scenario_interpolators_globals_get()
{
    return (s_scenario_interpolator_globals *)scenario_interpolator_globals;
}


static __forceinline unsigned char scenario_interpolators_function_type(const c_function_definition *function)
{
    return *(const unsigned char *)function->m_data.address;
}

// 0x12a188: advances every interpolator of the scenario by the frame delta
void scenario_interpolators_frame_update(float delta)
{
    if (scenario_interpolator_globals == 0)
        return;
    if (global_scenario == 0 || global_scenario->interpolators.count <= 0)
        return;
    float now = game_time_get_safe_in_seconds();
    for (long index = 0; index < global_scenario->interpolators.count; ++index)
    {
        s_scenario_interpolator_state *state = &scenario_interpolators_globals_get()->states[index];
        s_scenario_interpolator_tag_element *definition = TAG_BLOCK_GET_ELEMENT(&global_scenario->interpolators, index, s_scenario_interpolator_tag_element);
        if (state->flags & 1)
        {
            float value = 0.0f;
            if (state->end_time > state->start_time)
            {
                float fraction = SCENARIO_INTERPOLATORS_PIN_LOW((now - state->start_time) / (state->end_time - state->start_time), 0.0f);
                if (state->flags & 2)
                    fraction = scenario_interpolator_time_fraction_smooth(fraction);
                else
                    fraction = SCENARIO_INTERPOLATORS_PIN(fraction, 0.0f, 1.0f);
                value = state->start * (1.0f - fraction) + state->target * fraction;
            }
            state->last_time = now;
            state->current = value;
        }
        if (scenario_interpolators_function_type(&definition->function) == 3)
            state->flags |= 4;
        else
            state->flags &= ~4;
    }
    for (long index = 0; index < global_scenario->interpolators.count; ++index)
    {
        s_scenario_interpolator_state *state = &scenario_interpolators_globals_get()->states[index];
        s_scenario_interpolator_tag_element *definition = TAG_BLOCK_GET_ELEMENT(&global_scenario->interpolators, index, s_scenario_interpolator_tag_element);
        if ((state->flags & 1) && definition->source_index >= 0 && definition->source_index < global_scenario->interpolators.count)
        {
            s_scenario_interpolator_state *source = &scenario_interpolators_globals_get()->states[definition->source_index];
            s_scenario_interpolator_tag_element *source_definition =
                TAG_BLOCK_GET_ELEMENT(&global_scenario->interpolators, definition->source_index, s_scenario_interpolator_tag_element);
            if ((unsigned short)source_definition->source_index == 0xffff && (unsigned short)source_definition->other_index == 0xffff)
                state->integrated += delta * source->current;
        }
    }
}

// 0x242ba8
s_scenario_interpolator_state *scenario_get_interpolator_state(long name, long *out_index)
{
    s_scenario_interpolator_state *result = 0;
    long index = -1;
    if (scenario_interpolator_globals != 0 && global_scenario_get() != 0 && name != 0)
    {
        for (long interpolator_index = 0; interpolator_index < global_scenario_get()->interpolators.count; ++interpolator_index)
        {
            s_scenario_interpolator_tag_element *interpolator =
                TAG_BLOCK_GET_ELEMENT(&global_scenario_get()->interpolators, interpolator_index, s_scenario_interpolator_tag_element);
            if (interpolator->name && interpolator->name == name)
            {
                index = interpolator_index;
                scenario_interpolators_globals_get()->name = name;
                break;
            }
        }
    }
    if (index != -1)
    {
        if (out_index)
            *out_index = index;
        result = &scenario_interpolators_globals_get()->states[index];
    }
    else if (out_index)
        *out_index = -1;
    return result;
}

// 0x242c50
void scenario_interpolators_initialize(void)
{
    scenario_interpolator_globals = (s_scenario_interpolator_globals *)(
        ((t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
        &__tls_set_g_scenario_interpolator_globals_allocator>
                            *)&g_scenario_interpolator_globals_allocator)
                       ->reserve_memory("scenario interpolator globals", 0, 516, 0));
}

// 0x242c98
void scenario_interpolators_dispose(void)
{
    scenario_interpolator_globals = 0;
}

// 0x242cb0
void scenario_interpolators_initialize_for_new_map(void)
{
    void *interpolator_globals = scenario_interpolator_globals;
    if (interpolator_globals == 0)
        return;
    memset(interpolator_globals, 0, sizeof(s_scenario_interpolator_globals));
}

// @address 0x5bec98 ?scenario_interpolators_dispose_from_old_map@@YAXXZ
void scenario_interpolators_dispose_from_old_map(void)
{
}


// 0x242cd8: the current value of an interpolator, evaluated at its own index
float scenario_interpolator_get_current_value(long index)
{
    float result = 0.0f;
    s_scenario_interpolator_state *state = &scenario_interpolators_globals_get()->states[index];
    if (state)
        result = scenario_interpolator_evaluate_at(index, state->current);
    return result;
}

// 0x242d10: the value of interpolator index, offset by its integrated value, scaled by its linked interpolator and mapped through its function
float scenario_interpolator_evaluate_at(long index, float value)
{
    float result = 0.0f;
    s_scenario_interpolator_state *state = &scenario_interpolators_globals_get()->states[index];
    if (state)
    {
        float offset = state->integrated + value;
        s_scenario_interpolator_tag_element *definition =
            TAG_BLOCK_GET_ELEMENT(&global_scenario_get()->interpolators, index, s_scenario_interpolator_tag_element);
        short link = definition->other_index;
        if (link >= 0 && link < global_scenario_get()->interpolators.count)
        {
            s_scenario_interpolator_tag_element *linked =
                TAG_BLOCK_GET_ELEMENT(&global_scenario_get()->interpolators, link, s_scenario_interpolator_tag_element);
            if ((unsigned short)linked->other_index == 0xffff)
            {
                float legacy = definition->function.evaluate_legacy(offset, 0.0f);
                result = definition->function.map_to_output_range_legacy(scenario_interpolator_get_current_value(definition->other_index) * legacy);
            }
        }
        else
            result = definition->function.evaluate_scalar(offset, 0.0f);
    }
    return result;
}

// 0x242e10: starts the interpolator named name, from its current value to target over duration seconds
long hs_interpolator_start(long name, float target, float duration)
{
    long index = -1;
    s_scenario_interpolator_state *state = scenario_get_interpolator_state(name, &index);
    if (state)
    {
        float now = game_time_get_safe_in_seconds();
        state->start = state->current;
        state->target = target;
        state->last_time = now;
        state->integrated = 0.0f;
        unsigned char flags = state->flags;
        flags |= 1;
        flags &= ~2;
        state->start_time = now;
        state->end_time = now + duration;
        state->flags = flags;
    }
    return index;
}

// 0x242e90: the current value of the interpolator named name, or its evaluated value when evaluate is set
float hs_interpolator_get_current_value(long name, bool evaluate)
{
    long index = -1;
    s_scenario_interpolator_state *state = scenario_get_interpolator_state(name, &index);
    float result = 0.0f;
    if (state)
    {
        result = state->current;
        if (evaluate)
        {
            s_scenario_interpolator_state *evaluated = &scenario_interpolators_globals_get()->states[index];
            result = evaluated ? scenario_interpolator_evaluate_at(index, evaluated->current) : 0.0f;
        }
    }
    return result;
}

// 0x242f20: the interpolator named name at a time fraction, smoothed or clamped by its flags
float hs_interpolator_evaluate_at_time_fraction(long name, float time_fraction, bool evaluate)
{
    long index = -1;
    s_scenario_interpolator_state *state = scenario_get_interpolator_state(name, &index);
    float result = 0.0f;
    if (state)
    {
        float fraction = time_fraction;
        if (state->flags & 2)
            fraction = scenario_interpolator_time_fraction_smooth(fraction);
        else
            fraction = SCENARIO_INTERPOLATORS_PIN(fraction, 0.0f, 1.0f);
        result = (1.0f - fraction) * state->start + state->target * fraction;
        if (evaluate)
            result = scenario_interpolator_evaluate_at(index, result);
    }
    return result;
}

// 0x242ff0: the interpolator named name at an absolute time between its start and end times
float hs_interpolator_evaluate_at_time(long name, float time, bool evaluate)
{
    long index = -1;
    s_scenario_interpolator_state *state = scenario_get_interpolator_state(name, &index);
    float result = 0.0f;
    if (state)
    {
        if (state->end_time > state->start_time)
        {
            float fraction = SCENARIO_INTERPOLATORS_PIN_LOW((time - state->start_time) / (state->end_time - state->start_time), 0.0f);
            if (state->flags & 2)
                fraction = scenario_interpolator_time_fraction_smooth(fraction);
            else
                fraction = SCENARIO_INTERPOLATORS_PIN(fraction, 0.0f, 1.0f);
            result = (1.0f - fraction) * state->start + state->target * fraction;
            if (evaluate)
                result = scenario_interpolator_evaluate_at(index, result);
        }
    }
    return result;
}

// 0x243100: the interpolator named name, advanced by time_delta from its last time
float hs_interpolator_evaluate_at_time_delta(long name, float time_delta, bool evaluate)
{
    long index = -1;
    s_scenario_interpolator_state *state = scenario_get_interpolator_state(name, &index);
    float result = 0.0f;
    if (state)
    {
        if (state->end_time > state->start_time)
        {
            float now = state->last_time + time_delta;
            float fraction = SCENARIO_INTERPOLATORS_PIN_LOW((now - state->start_time) / (state->end_time - state->start_time), 0.0f);
            if (state->flags & 2)
                fraction = scenario_interpolator_time_fraction_smooth(fraction);
            else
                fraction = SCENARIO_INTERPOLATORS_PIN(fraction, 0.0f, 1.0f);
            result = (1.0f - fraction) * state->start + state->target * fraction;
            if (evaluate)
                result = scenario_interpolator_evaluate_at(index, result);
        }
    }
    return result;
}

// @address 0x243218 ?reserve_memory@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_scenario_interpolator_globals_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
template<>
void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_scenario_interpolator_globals_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
  int member_index = restricted_memory_register_member(&g_restricted_regions[1], name, type, size, 0,
      __tls_set_g_scenario_interpolator_globals_allocator, 0, 0);
  unsigned int offset = g_restricted_regions[1].get_member_offset(member_index);
  void *address = restricted_region_get_base_address(1) + offset;
  int owner = get_current_thread_index();
  m_thread_id = owner;
  m_member_index = member_index;
  scenario_interpolator_globals = (s_scenario_interpolator_globals *)address;
  return restricted_region_get_member_address(1, member_index);
}

// 0x2432b8
template<> void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_scenario_interpolator_globals_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// 0x2432d0
template<> void t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_scenario_interpolator_globals_allocator>::deallocate(void *memory)
{
    long member_index = m_member_index;
    scenario_interpolator_globals = 0;
    restricted_region_free_member(1, member_index);
}


// @address 0x11df76c scenario_interpolator_globals
__declspec(thread) s_scenario_interpolator_globals *scenario_interpolator_globals;

c_scenario_interpolator_allocation_manager g_scenario_interpolator_globals_allocator;
