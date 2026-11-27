#ifndef SIMULATION_GAME_INTERFACE_SIMULATION_GAME_ACTION_H
#define SIMULATION_GAME_INTERFACE_SIMULATION_GAME_ACTION_H

#include "math/real_math_types.h"

// Functions defined in tu/networkinglibxbox/simulation_game_action.cpp.

class c_global_material_type;
struct object_placement_data;
union real_point3d;
union real_vector3d;
struct s_damage_aftermath_result_data;
struct s_game_engine_event_data;
struct s_location;
struct s_player_editor_request_event_data;
struct s_projectile_hit;
enum e_simulation_entity_type;

void simulation_action_pickup_ammo(long unit_index, long item_definition_index, long weapon_definition_index, long amount);
void simulation_action_object_update(long handle, unsigned long value);
bool simulation_query_object_is_predicted(long object_index);
void simulation_action_object_delete(long object_index);
bool simulation_query_object_placement(const object_placement_data *placement);
bool simulation_query_local_predictive_control(long object_index, long control_type);
bool simulation_query_predictive_event(long object_index, long control_type, long *controlling_player_index);
void simulation_action_game_engine_globals_create(void);
void simulation_action_game_engine_globals_update(unsigned long flags);
void simulation_action_game_statborg_create(void);
void simulation_action_game_statborg_update(unsigned long player_mask);
void simulation_action_game_engine_player_create(short player_absolute_index);
void simulation_action_game_engine_player_update(short handle, unsigned long mask);
void simulation_action_game_engine_player_delete(short player_index);
void simulation_action_object_create(long object_index);
void simulation_action_object_create_build_entity_types(long object_index, long parent_object_index, long maximum_entity_count,
    long *entity_count, e_simulation_entity_type *entity_types, long *entity_object_indices);
void simulation_action_actor_create(long unit_index);
void simulation_action_actor_died(long object_index);
void simulation_action_breakable_surfaces_create(void);
void simulation_action_weapon_state_update(long weapon_index);
void simulation_action_projectile_attached(long projectile_index, long parent_object_index, short parent_node_index,
    const real_point3d *position, const s_location *location);
void simulation_action_projectile_detonate(long projectile_index, unsigned long flags,
    const c_global_material_type *hit_material_type, const real_vector3d *hit_normal, const s_projectile_hit *hit, bool force);
void simulation_action_damage_aftermath(long object_index, const s_damage_aftermath_result_data *result);
void simulation_action_pickup_equipment(long unit_index, long equipment_definition_index);
void simulation_action_unit_exit_vehicle(long unit_index);
void simulation_request_player_event(const s_player_editor_request_event_data *event_data);
void simulation_request_player_respawn_target(long player_index, long target_index);
void simulation_request_projectile_supercombine(long projectile_index);
void simulation_update_object_interpolation(long object_index);
void simulation_disable_object_interpolation(long object_index);
void simulation_set_object_orientation_for_interpolation(long object_index, const real_vector3d *forward, const real_vector3d *up);
bool simulation_get_object_interpolated_position(long object_index, real_point3d *point);
bool simulation_get_object_interpolated_orientation(long object_index, real_vector3d *vector, real_vector3d *vector2);
void simulation_action_multiplayer_event(const s_game_engine_event_data *source);
void simulation_action_weapon_effect(long weapon_index, const real_point3d *position, long effect_type);
void simulation_action_unit_board_vehicle(long unit_index);
void simulation_action_unit_grenade_initiate(long unit_index, short grenade_type);
void simulation_action_unit_melee_initiate(long unit_index, long melee_index, long melee_type, long attack_type);
void simulation_action_unit_reload(long unit_index, bool successful, bool empty);
void simulation_action_weapon_empty_click(long weapon_index);

struct collision_result;
struct s_simulation_action_projectile_fire_result;

extern "C" void simulation_action_sandbox_group_create(long group_index);
extern "C" void simulation_action_sandbox_group_update(long surface_index, unsigned long flags, bool immediate);
extern "C" void simulation_action_sandbox_groups_delete(void);
extern "C" void simulation_action_pickup_powerup(long object_index, long value);
extern "C" void simulation_action_unit_melee_clang(long type, const real_point3d *position, const real_vector3d *direction);
extern "C" void simulation_action_unit_reload_interrupt(long unit_index, bool successful, bool empty);
extern "C" void simulation_request_put_away_weapon(long unit_index, short set_index);
extern "C" void simulation_request_drop_weapon(long unit_index, short set_index);
extern "C" void simulation_request_unit_grenade_release(long unit_index, short hand, long grenade_type,
    const real_point3d *origin, const real_vector3d *direction);
extern "C" void simulation_action_pickup_weapon(long unit_index, long weapon_index, long desired_weapon_index);
extern "C" void simulation_action_weapon_fire(long weapon_index, long barrel_index, unsigned char fire_mode,
    const s_simulation_action_projectile_fire_result *projectile_results, long result_count, bool simulation_queue);
extern "C" void simulation_action_projectile_impact(bool impact, long projectile_index, float fraction, float velocity,
    const real_vector3d *direction, const collision_result *collision, bool from_impact);

struct s_simulation_object_interpolation_data
{
    unsigned long flags;
    real_point3d position;
    real_vector3d up;
    real_vector3d forward;
};

typedef char simulation_object_interpolation_size[(sizeof(s_simulation_object_interpolation_data) == 0x28) ? 1 : -1];

bool simulation_query_predictive_control(long object_index, long control_type, long *controlling_player_index);
void simulation_action_game_engine_globals_delete(void);
void simulation_action_game_statborg_delete(void);
extern "C" void simulation_update_object_interpolated_orientation(long object_index, s_simulation_object_interpolation_data *data);
extern "C" void simulation_update_object_interpolated_position(long object_index, s_simulation_object_interpolation_data *interpolation_data);

enum e_damage_section_response_type;
void simulation_action_damage_section_response(long object_index, long section_index, long response_index, e_damage_section_response_type response_type);

#endif // SIMULATION_GAME_INTERFACE_SIMULATION_GAME_ACTION_H
