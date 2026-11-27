#ifndef UNITS_BIPEDS_H
#define UNITS_BIPEDS_H

// Functions defined in tu/objectslibxbox/bipeds.cpp that other units call.

struct real_matrix4x3;
union real_point3d;
union real_vector3d;
void biped_get_sight_position(long object_index, long estimate, bool a, const real_point3d *position, const real_vector3d *forward,
    const real_vector3d *up, const real_vector3d *velocity, real_point3d *sight_position);
extern "C" void biped_estimate_camera_position(long object_index, short estimate_mode, bool offset_camera,
    const real_point3d *estimated_body_position, const real_vector3d *estimated_up,
    const real_vector3d *desired_facing, const real_vector3d *desired_gun_offset, real_point3d *estimated_position);
struct s_animation_control_data;
struct s_animation_event_data;
struct s_biped_melee_target_parameters;
enum e_biped_seat_impulse_type;
enum e_effect_type;
enum e_contact_point;
class c_animation_manager;
class ik_point_iterator;

void biped_impulse_vehicle_seat(long unit_index, e_biped_seat_impulse_type impulse_type);

bool biped_should_remain_active(long biped_index);
void biped_locate_auxilary_animations(long biped_index);
bool biped_calculate_invisible_crouched_uber_melee_z_offset(long biped_index, float *z_offset);
bool unit_animation_eye_tracking_in_progress(long unit_index);
long biped_get_current_weapon(long unit_index);
void biped_get_melee_target_physics_pill(long biped_index, real_point3d *position, float *height, float *offset);
void biped_get_physics_pill_from_definition(long _sym_p0, union real_point3d * position, float * height, float * offset);
float biped_get_physics_model_node_matrix_z_offset(long biped_index);
void biped_compute_lean(long biped_index, real_point3d *camera_position, real_vector3d *camera_forward, real_vector3d *camera_up);
void biped_bring_ragdoll_inside_world(long biped_index, bool flag);
void biped_enter_dead_physics(long biped_index);
void biped_enter_posture_physics(long biped_index, const real_point3d *position, const real_vector3d *forward);
void biped_enter_climbing_physics(long biped_index, const real_point3d *position);
void biped_enter_default_physics(long biped_index);
void biped_exit_invisible_crouched_uber_melee(long biped_index);
void biped_died(long biped_index, long killer_index, const real_vector3d *damage_vector, bool alternate, bool unused);
void biped_calculate_melee_target_parameters(long biped_index, long target_index, const real_vector3d *aiming_vector,
    s_biped_melee_target_parameters *parameters);
bool biped_fix_position(long biped_index, long ignore_object_index, const real_point3d *position_a,
    const real_point3d *position_b, real_point3d *position_c, float unknown_float, float scale, bool h, bool use_physics_pill);
void biped_exit_seat_end(long biped_index, long seat_index);
void biped_handle_footfall_events(long biped_index, const s_animation_event_data *animation_event_data);
long biped_calculate_better_melee_target(long unit_index, long target_index, long current_target);
void biped_bumped_object(long biped_index, long object_index, const real_vector3d *normal);
bool biped_bring_ragdoll_inside_world(long biped_index, bool flag, const real_point3d *position, float radius);
void biped_adjust_node_matrices(long biped_index, long node_matrices_count, real_matrix4x3 *node_matrices);
bool biped_update_melee_turning(long object_index, real_vector3d *aiming_vector);
void biped_get_autoaim_pill(long biped_index, union real_point3d *position, union real_vector3d *vector, float *radius);
bool biped_can_trade_weapon_with_player(long player_index, long unit_index);
extern "C" void biped_start_melee_dash(long biped_index, long target_unit_index, long unknown, bool forced);
extern "C" int biped_get_camera_velocities(long biped_index, real_vector3d *velocity, real_vector3d *camera_velocity);
extern "C" unsigned char biped_get_animation_root_velocity(long unit_index, real_vector3d *velocity);
extern "C" float biped_get_scale(long object_index);
extern "C" void biped_update_aiming_channel(long biped_index);
extern "C" bool biped_is_melee_target_in_range(const s_biped_melee_target_parameters *movement, bool sprinting, bool unarmed);
extern "C" bool weapon_is_energy_blade(long weapon_index);
extern "C" void biped_process_ik(long object_index, long marker_name, long weapon_name, bool mode_ik, float weight,
    const c_animation_manager *animation, ik_point_iterator *iterator, long node_count, real_matrix4x3 *node_matrices);
extern "C" void biped_apply_inverse_kinematics(long model_index, long node_index, const real_matrix4x3 *basis, const real_matrix4x3 *target,
    float weight, long node_count, real_matrix4x3 *node_matrices);
void biped_make_footstep(long biped_index, e_effect_type effect_type, e_contact_point contact_point, float volume);

class c_sector_ref;

void biped_find_pathfinding_location(long biped_index, short *, long *, c_sector_ref *sector_ref, real_point3d *point, long *object_index,
    unsigned long *flags);

// Data defined in tu/objectslibxbox/bipeds.cpp that other units use.

extern float global_lean_pivot_offset;

bool biped_find_surface(long biped_index, long a, long b, float radius, const real_point3d *position, const real_vector3d *direction,
    real_point3d *surface_point, real_vector3d *surface_direction, short *bsp_index, long *surface_index, long *reference_index,
    long *material_index, unsigned long *flags);

bool unit_fix_position(long a, long b, const real_point3d *c, const real_point3d *d, real_point3d *e, float f, float g, bool h);
struct s_scenario_biped;
void biped_place(long biped_index, s_scenario_biped *scenario_biped);
bool biped_compute_function_value(long biped_index, long function_type, long unused_argument, float *value_out, bool *positive_out, bool *deterministic);
void biped_postprocess_node_matrices(long biped_index, long node_count, real_matrix4x3 *node_matrices);

#endif // UNITS_BIPEDS_H
