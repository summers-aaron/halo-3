#ifndef ITEMS_PROJECTILES_H
#define ITEMS_PROJECTILES_H

// Functions defined in tu/objectslibxbox/projectiles.cpp.

union real_point3d;
union real_vector3d;
struct s_location;
struct s_collision_test_flags;

float projectile_definition_get_velocity_multiplier(long tag_index);
bool projectile_aim_linear(float speed, const real_point3d *origin, const real_point3d *target,
    real_vector3d *result_aim_vector, float *result_speed, float *result_time, float *result_distance);
bool projectile_aim(long definition_index, const real_point3d *origin, const real_point3d *target, float *speed,
    float *minimum_speed, float *maximum_time_scale, float *desired_speed, bool high_arc, real_vector3d *aim_vector,
    float *result_speed, float *result_time, float *result_distance, bool *linear);
void projectile_accelerate(long projectile_index, const real_vector3d *acceleration, const real_vector3d *normal);
void projectile_attach(long projectile_index, long parent_index, short parent_node_index, const real_point3d *position, const s_location *location, bool p5);
bool projectile_aim_ballistic(float speed, float gravity_scale, const real_point3d *origin, const real_point3d *target,
    float *result_minimum_speed, float *maximum_time_scale, float *desired_speed, bool high_arc, real_vector3d *result_aim_vector,
    float *result_speed, float *result_time, float *result_distance, float *result_vertical_speed, float *result_horizontal_speed);
bool projectile_estimate_collision_along_arc(long object_index, float p1, long p2, s_collision_test_flags flags, bool p4, real_point3d *point,
    float *out_distance);
bool dangerous_projectiles_near_player(long *projectile_index);
void projectile_detonate(long projectile_index, long object_index, bool force, float scale);

struct s_damage_owner;
struct s_effect_vector;

extern "C" long projectile_count_attached_to_object_for_supercombine(long index, long definition, long *projectile, long *object);
extern "C" float projectile_estimate_time_to_target(long tag_index, float value);
extern "C" void projectile_play_pickup_sound(long player_index, long definition_index);
extern "C" void projectile_create_detonation_effect(long tag_index, const real_point3d *origin, const real_vector3d *forward,
    const real_vector3d *up, const s_damage_owner *damage_owner, unsigned char flags, bool first_effect, bool second_effect,
    const s_location *location, const s_effect_vector *override_vector);

struct collision_result;

bool projectile_collision(long projectile_index, collision_result *collision, real_point3d *position,
    real_vector3d *velocity, real_vector3d *relative_velocity, float time, long collision_count, bool *detonate);
void projectile_initial_update(long projectile_index, bool use_seed, unsigned short supplied_seed, bool *distributed, unsigned short *result_seed);



class c_global_material_type;
struct projectile_definition;
struct s_projectile_material_response_definition;

const s_projectile_material_response_definition *projectile_definition_find_material_response(projectile_definition *definition,
    c_global_material_type material_type);
extern s_projectile_material_response_definition default_projectile_material_response;
extern long g_projectile_last_damaged_parent;

extern "C" const float k_projectiles_angular_velocity_scale;

class c_global_material_type;
extern "C" void projectile_create_scaled_impact_effect(long definition_index, float scale, float error, const real_vector3d *direction,
    const real_point3d *position, const real_vector3d *normal, c_global_material_type material, bool object_impact, long object_index,
    long node_index, bool effect, bool extra);

extern "C" void projectile_create_impact_effects(long definition_index, const real_point3d *origin, const real_vector3d *forward,
    const s_damage_owner *damage_owner, c_global_material_type material, const real_vector3d *normal, const s_location *location);

#endif // ITEMS_PROJECTILES_H
