#ifndef OBJECTS_DAMAGE_DEPENDENCIES_H
#define OBJECTS_DAMAGE_DEPENDENCIES_H

union real_point3d;
union real_vector3d;
class c_map_variant;
struct s_location;
enum e_achievement;

// Damage callers' C-linkage views. Owner declarations currently differ in
// parameter types or arity; preserve these views until callers and owners can
// be reconciled together, including the floating-point metagame argument.
extern "C" {
void map_variant_get_object_transform(c_map_variant *, long, real_point3d *, real_vector3d *, real_vector3d *);
void actor_new_child_from_object(long object_index, long swarm_index, long character_index, unsigned short initial_state,
    bool movement_mode, real_point3d *origin);
void game_results_try_and_grant_achievement(int player_index, e_achievement achievement);
void campaign_metagame_handle_kill(long object_index, long owner_player_index, long damage_owner_object_index, int team,
    unsigned char reporting_info, float damage, bool friendly);
void breakable_surface_damage_area_of_effect(const s_location *location, const real_point3d *position, float radius);
void giant_update_variant_scale(long object_index, bool update);
}

#endif // OBJECTS_DAMAGE_DEPENDENCIES_H
