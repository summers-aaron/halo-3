#ifndef AI_ACTOR_DYNAMIC_FIRING_POSITION_H
#define AI_ACTOR_DYNAMIC_FIRING_POSITION_H

#include "memory/data_types.h"
#include "memory/static_arrays.h"
#include "ai/ai_point3d.h"
#include "ai/sector_ref.h"
#include "ai/firing_position_definition.h"

struct dynamic_firing_set_datum
{
    short identifier;
    union { short firing_set_index; short type; };
    long object_index;
    short position_count;
    short reference_count;
    c_ai_point3d position;
    real_vector3d up, forward;
    c_sector_ref sector;
    bool valid;
    long update_time;
    c_static_flags_no_init<32> initialized_points;
    firing_position_definition points[32];
    short point_types[32];
};


extern __declspec(thread) s_data_array *dynamic_firing_set_data;

// Functions defined in tu/ailibxbox/actor_dynamic_firing_position.cpp that other units call.

void __tls_set_g_dynamic_firing_set_allocator(void *object);
void dynamic_firing_set_discard(long actor_index);
void actor_dynamic_firing_set_support_object(long actor_index, long object_index);
void actor_dynamic_firing_positions_update(long actor_index);

long dynamic_firing_set_find(long object_index, short firing_set_index);
long dynamic_firing_set_new_internal(long object_index, short firing_set_index);

#endif // AI_ACTOR_DYNAMIC_FIRING_POSITION_H
