#ifndef AI_PROPS_H
#define AI_PROPS_H

#include "ai/clump_importance_types.h"
#include "ai/prop_stimulus_types.h"

extern e_clump_importance g_stimulus_clump_importance[];

// Functions defined in tu/ailibxbox/props.cpp.

struct actor_position_data;
struct prop_ref_datum;
struct prop_state;
struct prop_view;
union real_point3d;
struct s_location;

bool prop_should_refresh_status(prop_ref_datum *prop_ref);
prop_state *prop_state_get(const prop_ref_datum *prop_ref);
bool allow_orphanhood(prop_ref_datum *prop_ref);
prop_view *prop_view_get(long prop_ref_index);
short prop_get_visibility(long actor_index, long prop_ref_index, bool enemy_prop, short visibility_type, actor_position_data *sense_position,
    actor_position_data *target_position, bool *visible);
bool pref_unreachable(long prop_ref_index);
long prop_get_target_object_index(long prop_ref_index);
bool pref_unassailable(long prop_ref_index);
float prop_track_weight(long prop_ref_index);
bool pref_inspected_orphan(prop_ref_datum *reference);
void prop_remove_tracking(long actor_index, long prop_ref_index);
void prop_state_new(prop_state *state);
void __tls_set_g_prop_data_allocator(void *object);
void prop_ref_reset_search(long index, bool clear_prop);
void actor_prop_abandon_search(long actor_index, long prop_index, short mode);
long prop_ref_get_acknowledged_by_object_index(long actor_index, long object_index);
void prop_unacknowledge(long actor_index, long prop_ref_index);
short prop_get_audibility(long actor_index, long object_index, actor_position_data *position_data, real_point3d *point, s_location *location, short flags);
bool prop_state_refresh(prop_state *state, long object_index, short refresh_type, actor_position_data *position_data);
bool prop_acknowledge(long actor_index, long prop_ref_index, bool p2);
void prop_orphan(long actor_index, long prop_ref_index, bool new_prop);
long prop_ref_get_by_object_index(long actor_index, long object_index, bool create, e_clump_importance importance);
bool prop_perceive(long actor_index, long object_index, short type, e_prop_stimulus stimulus, short stimulus_decay_ticks,
    long prop_ref_index, long swarm_index, long source_prop_ref_index);

union real_vector3d;

extern "C" void *prop_state_get_by_index(int owner_index);
extern "C" void prop_view_new(prop_view *view);
extern "C" bool prop_get_battle_vector(long prop_ref_index, real_vector3d *battle_vector, bool *unused);
extern "C" bool prop_track_reclaim_inactive_tracks(long actor_index, float distance);
extern "C" void actor_prop_update_new_position(long actor_index, long prop_ref_index, bool update_salience);
extern "C" void prop_perceive_target_for_trackers(long object_index, long target_object_index);
void __tls_set_g_prop_ref_data_allocator(void *object);
void __tls_set_g_tracking_data_allocator(void *object);

struct boid_state;
struct s_flock_instance;
struct flock_datum;
struct boid_flock_definition;

extern "C" void prop_compute_avoidance_vector(flock_datum *group, s_flock_instance *flags, boid_flock_definition *properties, boid_state *actor,
    const real_point3d *position, float *strength, real_vector3d *vector, bool unused, float *result);

#endif // AI_PROPS_H
