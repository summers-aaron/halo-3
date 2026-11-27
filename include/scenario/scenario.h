#ifndef SCENARIO_SCENARIO_H
#define SCENARIO_SCENARIO_H

#include "objects/cluster_reference.h"

// Functions defined in tu/blamlibxbox/scenario.cpp that other units call.

struct bsp3d;
struct s_cluster_reference;
union real_point3d;
struct s_location;
struct s_scenario_game_state;
struct scenario;

extern scenario *global_scenario;
struct s_game_globals;
extern s_game_globals *global_game_globals;
extern long global_scenario_index;
extern long global_scenario_game_globals_index;
extern long global_zone_set_index;
extern unsigned long g_active_structure_bsp_mask;
extern unsigned long g_touched_structure_bsp_mask;
extern unsigned long g_touched_cinematic_zone_mask;

long scenario_leaf_index_from_point(const real_point3d *point, long *structure_bsp_index);
short scenario_structure_index_from_point(const real_point3d *point);
void scenario_location_from_point(s_location *location, const real_point3d *point);
void scenario_location_from_leaf(s_location *location, long structure_bsp_index, long leaf_index);
s_cluster_reference scenario_cluster_reference_from_point(const real_point3d *point);
s_cluster_reference scenario_cluster_reference_from_point(long structure_bsp_index, const real_point3d *point);
s_cluster_reference scenario_cluster_reference_from_leaf_index(long structure_bsp_index, long leaf_index);
bool scenario_location_deafening(const s_location *location);
struct bsp3d * global_bsp3d_get(long structure_bsp_index);
short scenario_object_name_index_from_string(scenario *scenario, const char *name);
bool scenario_game_states_match(const s_scenario_game_state *a, const s_scenario_game_state *b);
void scenario_tags_fixup();

struct s_scenario_bits_change;
struct s_scenario_bits_state;
struct scenario_bsp_state_out;

extern "C" bool scenario_zone_set_is_active(long zone_set_index);
extern "C" long scenario_zone_set_get_data(void);
extern "C" bool scenario_commit_zone_resources(int reserved);
extern "C" void scenario_get_bsp_state(scenario_bsp_state_out *out);
extern "C" long cinematic_tag_reference_get_music(long cinematic_index, long scene_index, long shot_index, long entry_index);
extern "C" bool flock_create_from_key(long key);
extern "C" bool scenario_apply_zone_set_change(long zone_set_index, unsigned long old_structure_bsp_mask, unsigned long new_structure_bsp_mask,
    long touched_structure_bsp_mask, s_scenario_bits_state *state, long touched_cinematic_zone_mask, bool);
extern "C" void scenario_update_designer_zones(const s_scenario_bits_change *changes);
extern "C" bool scenario_switch_zone_set(long zone_set_index, bool unknown);
extern "C" bool scenario_set_game_state(const s_scenario_game_state *p0);
extern "C" unsigned char scenario_activate_zone_set(long zone_set_index);

// Data defined in tu/blamlibxbox/scenario.cpp that other units use.

struct s_collision_bsp_cache;

extern s_collision_bsp_cache g_collision_bsp_cache;
bool scenario_test_pvs(s_cluster_reference source, s_cluster_reference target);

void scenario_initialize();
void scenario_dispose();
void scenario_initialize_for_new_map();
void scenario_dispose_from_old_map();
void scenario_initialize_for_new_structure_bsp(unsigned long structure_bsp_mask);
void scenario_dispose_from_old_structure_bsp(unsigned long structure_bsp_mask);

#endif // SCENARIO_SCENARIO_H
