#ifndef AI_ACTOR_PERCEPTION_H
#define AI_ACTOR_PERCEPTION_H

#include "ai/perception_mode_types.h"
#include "ai/prop_stimulus_types.h"
#include "ai/actor_datum.h"
#include "ai/props.h"
#include "ai/prop_view.h"

// Functions defined in tu/ailibxbox/actor_perception.cpp.

struct actor_datum;
struct actor_position_data;
union real_point3d;
struct s_location;

void actor_perception_find_target_data(long object_index, struct actor_position_data * _sym_p1);
short actor_visibility_at_point(long actor_index, actor_position_data *position, const real_point3d *point,
	const real_point3d *next_point, short visibility_type, bool use_frustum, bool debug, bool *out_of_range);
void actor_perception_unreachable(long actor_index, long prop_ref_index, bool unreachable, bool update_salience);
bool actor_perception_danger_zone_interesting(long actor_index, long object_index, float distance, short priority);
short target_get_prop_class(actor_datum *actor);
// Local inline body for inlined call sites, named apart from the out-of-line function above:
// this build emits bl for cross-unit calls, so inlined sites need this copy (was duplicated in
// actor_perception.cpp and actors.cpp).
static __forceinline short inl_target_get_prop_class(actor_datum *actor)
{
	if (actor->target_prop_index != -1) {
		prop_view *target_view = prop_view_get(actor->target_prop_index);
		if (target_view)
			return target_view->status;
	}
	return 0;
}
short actor_audibility_at_point(long actor_index, actor_position_data *position, const real_point3d *point,
	const s_location *location, short sound_type, float sound_distance, short flags);
void actor_perception_forget_recent_damage(long actor_index);
bool actor_is_majorly_scary(long actor_index);
bool unit_has_scary_weapon(long unit_index);
bool actor_perception_find_sense_position(long actor_index, const real_point3d *point, actor_position_data *out);
void actor_perception_set_target(long actor_index, long prop_ref_index);
float actor_get_inherent_scariness(long unit_index);
void actor_perception_unassailable(long actor_index, long prop_ref_index, bool unassailable, bool update_salience);
void actor_prop_state_refresh(long actor_index, long prop_ref_index, actor_position_data *sense_position, actor_position_data *target_position, bool new_track);
void actor_perception_update(long actor_index);
void actor_compute_prop_salience(long actor_index, long prop_ref_index, e_prop_stimulus stimulus);

struct audibility_table;
union real_vector3d;

extern "C" void actor_perception_initialize_audibility_table(audibility_table *table, long scenario_index, long zone_set_index);
extern "C" short actor_perception_compute_target_blockage(const real_point3d *origin, const real_vector3d *aiming,
	const real_point3d *target, long object_index, real_vector3d *out_vector, float *out_distance);
bool squad_ancestor_of_squad_group(unsigned int squad_index, int group_index);
extern "C" void actor_perception_update_best_target(long actor_index);
void actor_visibility_test_frustum(long actor_index, real_vector3d *direction, real_vector3d *right, real_vector3d *up,
	real_vector3d *target, float distance, float scale, float *peripheral_out, float *central_out);
bool actor_perception_assess_suicide_danger(long actor_index, long prop_ref_index);
void actor_combat_status_update(long actor_index);
void actor_perception_player_state_refresh(long actor_index, long prop_ref_index);
e_perception_mode actor_get_perception_mode(long actor_index);

#endif // AI_ACTOR_PERCEPTION_H
