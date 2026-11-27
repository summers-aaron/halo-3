#ifndef AI_CLUMP_H
#define AI_CLUMP_H

#include "ai/clump_importance_types.h"

struct clump_actor_iterator
{
    long actor_index;
    long next_actor_index;
};

typedef char clump_actor_iterator_size[(sizeof(clump_actor_iterator) == 8) ? 1 : -1];

// Functions defined in tu/ailibxbox/clump.cpp.
extern "C" bool clump_can_berserk(long clump_index, long actor_index, long target_index);

long clump_get_clump_prop(long clump_index, long object_index);
long clump_get_prop_datum(long clump_index, long object_index);
long clump_aquire_prop(long clump_index, long object_index, e_clump_importance importance);
void clump_handle_deleted_object(long deleted_object_index);
void clumps_update();
void clumps_update_state(void);
void clump_add_actor(long clump_index, long actor_index);
extern "C" void clump_remove_actor(long clump_index, long actor_index);

struct clump_datum;
struct prop_ref_datum;
struct temp_prop;

void __tls_set_g_clump_data_allocator(void *allocator);
float clump_consider_object(long object_index, long clump_index, long prop_index, temp_prop *temp, e_clump_importance importance);
void clump_invitations_update(long clump_index);
void actor_remove_prop_ref(prop_ref_datum *prop_ref);
void clump_remove_prop_ref(prop_ref_datum *prop_ref);
long clump_new_prop_internal(clump_datum *clump, float weight);
extern "C" short clump_calculate_postombat_result(int p0);
void clump_perception_update(long index);
void clump_add_prop(long clump_index, long prop_index);
extern "C" void clump_delete_stale_props(long clump_index);
void clump_update_behavior_state(long index);
void clump_absorb_memory(long destination_index, long source_index);

struct s_task_group;
extern "C" short clump_timed_entry_is_recent(s_task_group *group, float *state_fractions, float *cumulative_fractions);
extern "C" short clump_group_state_get_search_flag(s_task_group *group, float *state_fractions, float *cumulative_fractions);
extern "C" short group_state_attacking_next_state(s_task_group *group, float *state_fractions, float *cumulative_fractions);
bool group_state_searching_override(float *state_fractions, float *cumulative_fractions);
bool group_state_attacking_override(float *state_fractions, float *cumulative_fractions);

union real_point3d;

long clump_new(real_point3d *point, short p1, bool p2);

#endif // AI_CLUMP_H
