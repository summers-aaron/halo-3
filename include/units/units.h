#ifndef UNITS_UNITS_H
#define UNITS_UNITS_H

union real_point3d;
union real_vector3d;
class c_global_material_type;
void unit_cause_melee_damage_response(long unit_index, long effect_index, long damage_effect_index, long target_index,
    c_global_material_type material, const real_point3d *position, const real_vector3d *direction);

extern "C" {
extern float k_unit_assassination_angle;
bool unit_can_be_assassinated(long unit_index, const real_point3d *point, const real_vector3d *direction);
}

// Functions defined in tu/objectslibxbox/units.cpp.

struct real_matrix4x3;
union real_point3d;
union real_rectangle2d;
union real_vector3d;
struct s_damage_aftermath_result_data;
struct s_damage_data;
struct s_scenario_unit_datum;
struct s_unit_weapon_set;
struct unit_control_data;
struct unit_seat;
struct unit_seat_source;
enum e_unit_seat_flags;

bool unit_adjust_first_person_camera(long unit_index, real_vector3d *vector);
long unit_get_aiming_unit_index(long unit_index);
long unit_get_active_primary_weapon(long unit_index, long *owner_unit_index);
void unit_get_camera_position(long unit_index, real_point3d *position);
short unit_inventory_get_must_be_readied_weapon(long object_index);
bool unit_can_zoom(long unit_index);
short unit_inventory_next_grenade(long unit_index, short current, short direction);
void unit_control_clear(unit_control_data *control);
void unit_control(long unit_index, const unit_control_data *control);
void unit_get_head_position(long unit_index, real_point3d *position);
bool unit_get_body_position(long unit_index, real_point3d *position);
long unit_seat_get_occupant(long parent_index, short seat_index);
bool unit_seat_test_flag(long unit_index, short seat_index, e_unit_seat_flags bit);
bool unit_get_aim_position(long unit_index, real_point3d *position);
bool unit_get_gun_position(long unit_index, bool primary, real_point3d *position);
float unit_get_weapon_dual_error_theta(long unit_index, long weapon_index);
bool unit_has_seat_entrance_animation(long unit_index, long parent_index, short seat_index);
float unit_compute_boost_fraction(long unit_index);
float unit_get_zoom_magnification(long unit_index, short zoom_level);
long unit_get_current_backpack_weapon(long unit_index);
bool unit_has_seat_animation(long unit_index, long parent_index, short seat_index);
short unit_rotate_zoom_level(long unit_index, short zoom_level);
unit_seat *unit_get_current_seat(long unit_index);
void unit_check_ping_thresholds(long unit_index, float shield_damage, float body_damage, long *result);
void unit_handle_weapon_state_change(long unit_index, long weapon_index, long state);
bool any_unit_is_dangerous(long *unit_index);
bool unit_is_dying(long unit_index);
short unit_get_driver_seat(long object_index);
void unit_update_driver_and_gunner(long unit_index);
void unit_add_equipment_to_inventory(long unit_index, long equipment_index);
bool unit_has_weapon_definition_index(long unit_index, long weapon_definition_index);
void unit_get_all_seats_internal(long object_index, struct unit_seat_source *seats, short *count, short max, unsigned char force);
void unit_apply_predicted_weapon_state(long unit_index, long weapon_slot);
void unit_update_predicted_vehicle(long unit_index);
void unit_update_predicted_controller(long unit_index);
void unit_active_camouflage_disable(long unit_index, float transition_time);
void unit_active_camouflage_ding(long unit_index, float amount, float regrowth_seconds);
bool unit_evict_parent_seat_occupant(long unit_index, bool force, bool immediate);
void unit_seat_try_opening(long unit_index, long seat_index, bool a, bool b, bool opening);
void unit_correct_dual_wield_state(long unit_index, long seat_index, bool a, bool b, bool opening);
void unit_delete_current_equipment(long unit_index);
void unit_set_actively_controlled(long unit_index, bool actively_controlled);
void unit_drop_current_equipment(long unit_index);
void unit_delete_all_weapons(long unit_index);
bool unit_put_away_weapon(long unit_index, short weapon_set_index, bool flag);
bool unit_placement_try_to_put_unit_in_seat_using_marker_name(long unit_index, long parent_index, long marker_name);
bool unit_can_pickup_equipment(long unit_index, long equipment_index);
bool unit_can_use_weapon_in_seat(long unit_index, long seat_label, long weapon_index);
void unit_start_running_blindly(long unit_index);
short unit_weapon_next_index(long unit_index, long seat_label, short current_index, short direction);
void unit_adjust_projectile_ray(long unit_index, real_point3d *origin, real_vector3d *forward, real_vector3d *velocity, real_point3d *offset,
    bool snap_to_aim_line, bool use_aiming_vector, bool find_good_point);
void unit_unzoom(long unit_index, bool unknown);
void unit_handle_deleted_object(long unit_index, long object_index);
void unit_notify_of_tracking_or_locking(long unit_index, long object_index, short flags);
bool unit_can_see_point(long unit_index, const real_point3d *point, float angle);
long unit_get_best_seat_entrance_animation(long actor_unit_index, long unit_index, short seat_index, real_point3d *start, real_point3d *entrance);
bool unit_add_grenade_to_inventory(long unit_index, long equipment_index);
bool unit_clip_vector_to_aiming_screen_bounds(long unit_index, real_vector3d *vector, bool secondary);
void unit_damage_aftermath_apply(long unit_index, const s_damage_aftermath_result_data *result);
void unit_perform_detach_and_damage(long unit_index, long object_index);
void unit_add_starting_profile_equipment(long p0, short p1, bool p2, bool p3);
void unit_set_weapon_type(long unit_index, long inventory_index, long definition_index,
    short unknown, short control_flags, const s_unit_weapon_set *unit_weapon_set);
void unit_euler_aiming_update(const real_matrix4x3 *matrix, float yaw, real_vector3d *aiming_vector, const real_vector3d *target_vector,
    float dt, real_vector3d *angular_velocity, const real_rectangle2d *input_aiming_bounds, float angular_velocity_limit,
    float angular_acceleration_limit, float *output_yaw);
bool unit_can_enter_seat(long unit_index, long parent_index, short seat_index, long *occupant_index, bool *boarding, bool *unknown);
void unit_estimate_position(long object_index, short estimate_mode, const real_point3d *body_position,
    const real_vector3d *vector1, const real_vector3d *vector2, const real_vector3d *vector3, real_point3d *estimated_position);
void unit_consider_nearby_seats(long unit_index, long target_index, long *out_unit, short *out_seat_a, short *out_seat_b, float *out_distance, bool *out_flag);
void unit_place(long unit_index, s_scenario_unit_datum *scenario_unit);
void unit_damage_aftermath_compute(long index, long damage_flags, const s_damage_data *damage, s_damage_aftermath_result_data *result);
bool unit_can_access_object(long unit_index, long object_index);
bool unit_fix_position(long unit_index, long p1, real_point3d const *point, real_point3d const *point2, real_point3d *output_point,
    float minimum_search_radius, float maximum_search_radius, bool search_everywhere);
void unit_cause_player_melee_attack(long unit_index, long animation, short mode, float scale);
void unit_cause_ai_melee_attack(long unit_index, short melee_type);
extern "C" long unit_get_melee_target(long unit_index);
extern "C" void unit_active_camouflage_enable(long unit_index, float transition_time, bool hold);
extern "C" bool unit_is_player_controlled(long unit_index);
extern "C" int unit_calculate_is_in_illegal_position(int unit_index);
extern "C" void unit_cause_random_idle_action(long unit_index);
extern "C" void unit_detach_and_reset_motor_program(long unit_index);
extern "C" void unit_force_drop_inventory_weapon(long unit_index, long weapon_index, bool unknown, long zero);
extern "C" void unit_cause_self_damage(long unit_index, long damage_index, float scale);

struct euler_axis_plan;

extern "C" unsigned char unit_vehicle_is_flying(long object_index);
bool unit_update_control(long unit_index);
void unit_update_vision_mode(long unit_index);
bool unit_update_seats(long unit_index);
bool unit_can_use_seat_entry_marker(long unit_index, long target_index, short seat_index, const real_point3d *position, const real_vector3d *forward);
void unit_add_initial_weapons(long unit_index);
bool unit_update_active_camouflage(long unit_index);
extern "C" void unit_update_active_camouflage_fade(long unit_index);
void unit_update_team_index(long unit_index);
void unit_euler_axis_couple(struct euler_axis_plan * plan1, struct euler_axis_plan * plan2, float max_velocity, float acceleration);
void unit_delete_all_weapons_internal(long unit_index);
extern "C" void unit_dialogue_set_variant(long object_index, unsigned char alternate);
extern "C" float unit_get_boarding_progress(long object_index);
void unit_running_blind(long unit_index, real_vector3d *run_vector);
void unit_arbiter_active_camouflage_play_sound(long unit_index, short sound);
void unit_seat_acceleration_update(long index);
bool unit_update_weapons(long unit_index);
bool unit_update_aiming(long unit_index);
void unit_died(long unit_index, long damage_type, const real_vector3d *direction, bool alternate, bool force_weapon_drop);
void unit_toss_item(long unit_index, long object_index, long marker_name, float magnitude, bool randomize);

struct unit_datum;
struct unit_passenger_iterator;

struct unit_datum * unit_passenger_iterator_next(struct unit_passenger_iterator * iterator);

struct unit_interface_state;

void unit_build_unit_interface_state(long unit_index, struct unit_interface_state * state);
void unit_euler_axis_buildplan(float position, float velocity, float maximum_velocity, float acceleration, euler_axis_plan *plan);

enum e_weapon_addition_method;

bool unit_add_weapon_to_inventory(long unit_index, long object_index, e_weapon_addition_method addition_method);

enum e_unit_drop_type;
enum e_unit_animation_key_frame_status;
enum e_animation_playback_flags;
struct unit_weapon_pickup_result;

void unit_drop_item(long unit_index, long object_index, e_unit_drop_type drop_type);
void unit_inventory_set_weapon_index(long unit_index, short inventory_index, long object_index, e_unit_drop_type drop_type);
e_unit_animation_key_frame_status unit_animation_get_key_frame_status(long unit_index, bool flag);
bool unit_animation_allows_playback_adjustment(long unit_index, e_animation_playback_flags playback_flag);
bool unit_can_pickup_weapon(long unit_index, long weapon_index, e_weapon_addition_method method, unit_weapon_pickup_result *result);
bool unit_test_pickup_with_mode(long unit_index, const unit_weapon_pickup_result *result, e_weapon_addition_method method, short *inventory_index);

// The animation state names that count as entering a seat: five string ids.
extern "C" long g_unit_entering_state_names[5];

enum e_unit_hand;

void unit_inventory_drop_weapon(long unit_index, short inventory_index, e_unit_drop_type drop_type, bool thrown);
void unit_show_weapon_from_inventory(long weapon_index, long unit_index, e_unit_hand hand);


class c_global_material_type;
c_global_material_type unit_override_damage_info_material_type(long unit_index, c_global_material_type material);

extern "C" void unit_update_melee_action_state(long unit_index);
extern "C" void unit_set_player_melee_remaining(long unit_index, long remaining);

void unit_delete(long unit_index);

#endif // UNITS_UNITS_H
