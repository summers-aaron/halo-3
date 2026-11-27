#ifndef AI_ACTOR_COMBAT_H
#define AI_ACTOR_COMBAT_H

// Functions defined in tu/ailibxbox/actor_combat.cpp.

class c_ai_point3d;
struct character_weapon_properties;
union real_point3d;
union real_vector3d;

bool actor_should_lower_weapon(long actor_index);
void actor_get_gun_position(long actor_index, bool unknown, real_point3d *position);
bool actor_get_weapon_bounds(long actor_index, character_weapon_properties *properties, float *minimum, float *maximum, float *distance);
bool actor_request_grenade_throw(long actor_index, bool force);
void actor_handle_projectile_impact(long projectile_index, long object_index);
bool actor_combat_check_collateral_damage(long actor_index, float radius, float distance, const real_point3d *point, short *count, short *flags);
bool actor_combat_find_grenade_target(long actor_index, const real_point3d *position, long target_index, long *object_index);
bool actor_combat_plan_grenade_trajectory(long actor_index, short grenade_type, const c_ai_point3d *point, long target_type, long target_index, short flags);
bool actor_aim_projectile(long actor_index, const real_point3d *point, real_vector3d *vector, float *error, float *velocity, long *target_object_index);
long actor_aim_grenade(long actor_index, const real_point3d *point, real_vector3d *vector);
bool actor_target_choose_target_marker(long actor_index, long object_index);
bool actor_weapon_aim(long actor_index, long weapon_index, short barrel_index, const real_point3d *origin, const real_point3d *target, bool high_arc,
    real_vector3d *aim_vector, float *unused, float *result_distance, float *result_speed, bool *linear);
void actor_combat_update(long actor_index);
extern "C" bool actor_combat_try_and_get_throw_velocity(long actor_index, long object_index, real_vector3d *velocity);

struct character_firing_pattern;

bool actor_combat_build_grenade_trajectory(short grenade_type, const real_point3d *origin, float speed, float speed_limit, const real_point3d *target,
    float *maximum_time_scale, bool high_arc, real_vector3d *velocity, float *time, float *ballistic_speed, real_vector3d *ballistic_velocity, float *gravity);
void actor_start_burst(long actor_index, long weapon_object_index, character_weapon_properties *weapon_properties, character_firing_pattern *pattern);

#endif // AI_ACTOR_COMBAT_H
