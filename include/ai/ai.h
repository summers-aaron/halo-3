#ifndef AI_AI_H
#define AI_AI_H

#include "ai/ai_sound_volume_types.h"
#include "ai/sound_types.h"
#include "game/game_types.h"
#include "objects/object_reference_types.h"

// Functions defined in tu/ailibxbox/ai.cpp.

union real_point3d;
union real_vector3d;
struct s_damage_data;
struct s_location;

float ai_chance_this_tick(float chance);
void ai_erase(long squad_index, bool immediately);
void ai_initialize_for_saved_game(long saved_game_index);
long ai_get_unit_responsible_for_damage(long object_index, bool prefer_vehicle);
void ai_handle_damage(long object_index, long damaging_object_index, short damage_type, long damage_definition_index,
    float damage, const real_vector3d *direction, bool ignore_actor);
long actor_endangering_player(bool require_target,bool all_players,unsigned long *player_mask);
void ai_handle_weapon_trade(long actor_index, long player_index, long old_weapon, long new_weapon);
bool ai_enemies_can_see_player(long *unit_index);
void *ai_scratch_allocate(unsigned int size);
bool ai_test_line_of_fire(long p0, long p1, const real_point3d *p2, const real_vector3d *p3, long *p4);
bool ai_find_location(const real_point3d *point, s_location *location);
bool ai_try_vehicle_eviction(long actor_index, long unit_index, bool evict);
void ai_handle_stun(long unit_index, float stun, const real_vector3d *direction);
void ai_get_center_of_mass(long object_index, real_point3d *point);
void ai_update_team_status();
bool ai_adjust_damage(long damaging_unit_index, long damaged_unit_index, s_damage_data *damage_data, float *damage);
void ai_handle_death(long unit_index, long damage_object, short damage_type, float damage);
void ai_handle_projectile_attach(long parent_index, long child_index);
void ai_update(void);
bool ai_test_ballistic_line_of_fire(long actor_index, const real_point3d *origin, float speed,
    const real_vector3d *velocity, float gravity, long target_index, bool in_vehicle);

struct real_matrix4x3;

void ai_swarm_spawn_from_effect(int p0, short p1, int p2, short p3, float p7, float p8, const real_matrix4x3 *p6);
extern "C" void ai_unit_setup_auto_turret(long object_index, long owner_index);
extern "C" void ai_break_squad_tasks_around_object(long object_index);
extern "C" void ai_create_initial_squads();
extern "C" bool ai_test_short_line_of_sight(long actor_index, long ignore_object_index, const real_point3d *start, const real_point3d *end);
extern "C" void ai_sound_new_at_position(long object_index, const real_point3d *position, short sound_type, short volume);

struct line_of_fire_pill;

short ai_find_line_of_fire_friend_pills(long object_index, short maximum_count, line_of_fire_pill *shapes);
extern "C" void ai_initialize_avoidance_rays(void);
void ai_globals_update();
short ai_test_line_of_sight(const real_point3d *start, s_cluster_reference start_cluster, const real_point3d *end, s_cluster_reference end_cluster,
    short mode, bool transparent, long ignore_a, long ignore_b, bool skip_objects, bool alternate, long *hit_object, bool *transparent_hit);
void ai_handle_allegiance_status_changed(e_game_team team_a, e_game_team team_b, bool broken, bool permanently, bool update);
void ai_handle_unit_effect(long unit_index, e_sound_type effect_type, e_ai_sound_volume volume);
long ai_release_inactive_swarms(long unused, unsigned char *working_memory, long working_memory_size,
    bool *more_to_release, char *description, long description_size);
long ai_release_inactive_squads(long unused, unsigned char *working_memory, long working_memory_size,
    bool *more_to_release, char *description, long description_size);
void ai_find_inactive_squads(long unused, unsigned char *working_memory, long working_memory_size);

void ai_initialize();
void ai_dispose();
void ai_initialize_for_new_map();
void ai_dispose_from_old_map();
void ai_initialize_for_new_structure_bsp(unsigned long mask);
void ai_dispose_from_old_structure_bsp(unsigned long mask);

#endif // AI_AI_H
