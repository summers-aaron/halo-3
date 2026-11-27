#ifndef AI_ACTORS_H
#define AI_ACTORS_H

// Functions defined in tu/ailibxbox/actors.cpp.

struct actor_datum;
struct actor_iterator;
struct actor_nearby_actors_iterator;
struct actor_position_data;
struct actor_starting_location_definition;
union real_point3d;
struct s_game_cluster_bit_vectors;
struct s_task;

actor_datum *actor_iterator_next(actor_iterator *iterator);
long actor_get_weapon(long player_index);
bool actor_in_combat(long actor_index);
bool actor_has_ranged_weapon(long player_index);
bool actor_is_giant(actor_datum *actor);
float actor_destination_tolerance(long actor_index);
void actor_unit_control_crouch(long player_index, bool value);
void actor_unit_control_lower_weapon(long player_index, bool value);
short actor_get_rank(long player_index);
void __tls_set_g_actor_data_allocator(void *object);
bool actor_set_bring_forward(long unit_index, bool bring_forward, float value, float seconds);
void actor_attach_unit(long actor_index, long unit_index);
void actor_set_team(long actor_index, short team);
bool actor_set_active(long actor_index, bool active);
void actor_erase(long actor_index, bool immediately);
void __tls_set_g_actor_firing_position_hash_allocator(void *allocator);
long actor_select_character_variant(long character_index);
void actor_select_character_voice(long character_index, long unit_index);
void actor_unit_control_boost(long player_index, bool value);
void actor_break(long actor_index, bool value);
void actor_nearby_actors_iterator_new(actor_nearby_actors_iterator *iterator, const real_point3d *point, float radius, short team, short filter);
actor_datum *actor_nearby_actors_iterator_next(actor_nearby_actors_iterator *iterator);
bool actor_test_activation_cluster_bit_vector(long actor_index, const s_game_cluster_bit_vectors *bits);
float actor_get_maximum_damage_velocity(long actor_index);
float actor_get_gravity_scale(long actor_index);
void actor_initiate_combat(long actor_index);
bool actor_set_active_camo(long actor_index, bool camo);
void actor_set_covered(long actor_index, bool covered);
extern "C" void actor_initialize_weapon_ammo(long character_index, long weapon_index);
extern "C" void actor_handle_deleted_object_directions(long actor_index, long object_index);
bool actor_get_active_camo_state(long actor_index);
bool actor_runtime_within_structure_bsp(long player_index);
short actor_get_standing(long player_index);
bool actor_has_followers(long p0);
void actor_flush_prop(long actor_index, long prop_index);
void actor_flush_position_indices(long actor_index, bool flush);
long actor_create(long unit_index, long character_index, long squad_index, bool initially_asleep);
void actor_handle_change_internal(long index, bool changed, bool camo);
void actor_handle_task_change(long actor_index, struct s_task * task);
void actor_input_sample_position(long actor_index, long object_index, actor_position_data *data);
void actors_initialize(void);
void actor_freeze(long actor_index);
void actor_delete(long actor_index, bool erase);
void actor_customize_unit(long character_index, long unit_index, long primary_weapon_index,
    long secondary_weapon_index, short grenade_type, long equipment_index);
void actor_died(long actor_index);
void actor_handle_deleted_object(long actor_index, long object_index);
void actor_kill(long actor_index, bool killed_by_player, bool silent);
void actors_update();
long actor_place(long character_index, short squad_index, short fire_team_index, actor_starting_location_definition *starting_location, short flags);

union real_vector3d;

bool actor_get_running_blind_vector(long actor_index, real_vector3d *run_vector);

bool actor_can_vehicle_charge(long actor_index);
short actor_find_props_in_sphere(long actor_index, const real_point3d *point, float radius, long *object_indices, short maximum_count);
extern "C" void actor_handle_weapon_trade_refusal(long actor_index, long object_index);
extern "C" void actor_handle_damage(long actor_index, short damage_type, long damage_definition_index,
    long responsible_unit_index, float damage, const real_vector3d *direction);
void actor_detach_from_unit(long actor_index);
void actor_get_timeslices(long actor_index);
long actor_create_internal(long unit_index, long swarm_index, long character_index, long squad_index, bool initially_asleep);
extern "C" unsigned char actor_switch_bsp(long index);
void actor_update_goal_status(long actor_index);
void actor_input_update(long actor_index);
void actor_unit_control(long actor_index);
long actor_new(long character_index);
void actor_update(long actor_index);
void actor_estimate_position(long object_index, short ticks, const real_point3d *origin, const real_vector3d *vector1,
    const real_vector3d *vector2, const real_vector3d *vector3, real_point3d *result);
bool actor_attacking_target(long actor_index, real_vector3d *attack_vector);
extern "C" long actor_new_child_from_object(long object_index, long swarm_index, long character_index,
    unsigned short initial_state, short movement_mode, real_point3d *origin);

// Data defined in tu/ailibxbox/actors.cpp that other units use.

extern long g_movement_mode_names[];
extern long g_updating_actor_index;

#endif // AI_ACTORS_H
