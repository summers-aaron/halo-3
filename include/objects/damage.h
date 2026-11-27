#ifndef OBJECTS_DAMAGE_H
#define OBJECTS_DAMAGE_H

// Functions defined in tu/objectslibxbox/damage.cpp that other units call.

union real_vector3d;
struct s_damage_aftermath_result_data;
struct s_damage_data;
struct s_collision_test_flags;
struct s_damage_owner;
struct s_model_damage_info;
enum e_physics_model_constraint_type;

const s_model_damage_info *object_get_damage_info(long object_index);
float damage_armor_table_lookup(long material_type_a, long material_type_b, long armor_type_a, long armor_type_b);
bool object_scripting_vitality_pinned(long object_index);
void object_initialize_vitality(long object_index, float *body_override, float *shield_override);
void object_damage_handle_resurrection(long object_index);
void damage_handle_vehicle_exit(long object_index, long vehicle_index);
short object_model_targets_destroyed(long object_index, long marker_name);
long armor_modifier_bsearch_proc(const void *a, const void *b, const void *context);
void damage_set_object_list_flag(long owner, unsigned char flag);
void object_destroy_internal(long object_index, long owner_player_index, long damage_owner_object_index, unsigned char argument, unsigned char silent);
void damage_data_new(s_damage_data *damage, long definition_index);
void object_damage_constraints(long object_index, unsigned short broken_constraints, unsigned short loosened_constraints);
bool object_constraint_is_broken(long object_index, e_physics_model_constraint_type constraint_type, long constraint_index);
bool object_constraint_is_loosened(long object_index, e_physics_model_constraint_type constraint_type, long constraint_index);
void damage_effect_new_on_weapon(long weapon_index, long damage_effect);
void object_cause_fatal_damage_aftermath(long object_index);
void object_apply_damage_aftermath(long object_index, const s_damage_aftermath_result_data *result);
void object_damage_damage_section(long object_index, long section_name, float damage);
long area_of_effect_cause_damage(s_damage_data *damage, long owner_index, long ignore_index);
void damage_spawn_effect_from_marker_name(long effect_index, long object_index, const s_damage_owner *damage_owner,
    long marker_name, bool detached, bool flags);
void object_cause_damage(s_damage_data *damage_data, long object_index, short node_index, short region_index,
    short material_index, const real_vector3d *direction);
void object_damage_new(long object_index);
bool object_damage_update(long object_index);
extern "C" void object_set_cannot_die(unsigned long object_index, bool pinned);
extern "C" void compute_total_damage(long index);
extern "C" long object_set_damage_taking_internal(long list_index, short count);

union real_point3d;
struct s_damage_response_data;
struct s_damage_seat_info;
struct s_damage_seat_region_state_setting;
struct s_instantaneous_damage_response;

bool area_effect_infection_form_damage(long object_index, s_damage_data *damage, bool *result);
void damage_section_damage_one_constraint(const s_model_damage_info *info, const s_instantaneous_damage_response *response,
    unsigned short constraint, unsigned short *result);
long object_find_constraint_search_initial_region(long object_index);
float object_determine_rider_damage_scale(s_damage_data *damage, long object_index, long rider_index);
void damage_seat_get_transfer_info(long object_index, s_damage_seat_info *info, s_damage_seat_region_state_setting *setting);
void damage_section_deplete(const s_model_damage_info *info, long object_index, const s_damage_owner *owner,
    unsigned char flags, long region_index, s_damage_response_data *response);
void object_deplete_shield_internal(long object_index, const s_damage_owner *owner, s_damage_response_data *response);
bool area_effect_damage_test_obstruction(long object_index, const real_point3d *center, const s_damage_data *damage);

extern "C" {
float damage_get_object_scale(int object_index);
bool vehicle_has_passengers(long object_index, bool players_only, bool include_protected_seats, long player_index,
    bool *driver_is_enemy, long *enemy_passenger_count);
// Stored to acceleration-info flags bit 1; no code path tests that bit (only bits 0 and 2 are read).
void object_submit_acceleration(long object_index, bool ignore_node_index, long node_index, bool unused, bool set_biped_flag,
    const real_point3d *point, const real_vector3d *acceleration);
void object_cause_default_damage(long, unsigned char, unsigned char, unsigned char, const s_damage_owner *, unsigned char);
long damage_compute_connected_regions(long object_index, long region_index, long *unattached_regions);
void damage_update_section_damage(const s_model_damage_info *info, long object_index, const s_damage_owner *owner,
    unsigned char reporting_info, long section_index, float damage, long definition, s_damage_response_data *response);
}

void object_compute_damage_aftermath(long object_index, const s_damage_data *damage_data,
    const s_damage_response_data *damage_response_data, s_damage_aftermath_result_data *result);



#endif // OBJECTS_DAMAGE_H
