#ifndef GAME_AIM_ASSIST_H
#define GAME_AIM_ASSIST_H

#include "game/game_types.h"
#include "cseries/cseries_wrapped.h"
#include "math/real_math_types.h"
#include "cseries/cseries.h"
#include "units/unit_control.h"
#include <string.h>

enum e_aim_assist_flags { _aim_assist_secondary_bit = 0, _aim_assist_multiplayer_bit = 1 };  // names: player_control

struct aim_assist_parameters
{
    c_flags<e_aim_assist_flags, unsigned char, 2> flags;
    bool melee_only;
    float autoaim_angle, autoaim_range, autoaim_falloff_range;
    float magnetism_angle, magnetism_range, magnetism_falloff_range;
    float deviation_angle;
    unsigned long weapon_flags;
    float secondary_autoaim_angle, secondary_autoaim_range, secondary_autoaim_falloff_range;
    float tracking_range;
    real_point3d position;
};

struct s_aim_assist_target
{
    long object_index;
    long model_target;
    real_point3d position;
    real_vector3d velocity;
    float radius;
    real_vector3d forward;
    float cone_angle;
    bool friendly;
    real_vector3d direction_to_target;
    real_vector3d direction;
    float distance, angle, primary_score, secondary_score, tertiary_score;
    long flags;
    float tracking_distance;
};

// Functions defined in tu/blamlibxbox/aim_assist.cpp.

class c_bitstream;
union real_point3d;
union real_vector3d;
struct s_aim_assist_targeting_result;

void aim_assist_targeting_encode(c_bitstream *bitstream, const s_aim_assist_targeting_result *aim_assist);
void aim_assist_get_target_position(long object_index, long target_index, union real_point3d * point);
bool player_aim_projectile(long player_index, const s_aim_assist_targeting_result *targeting, long projectile_definition_index,
    bool use_primary, const real_point3d *position, const real_point3d *target_position, real_vector3d *aim_direction);
long player_aim_assist_get_player_under_crosshair(long user_index);

struct real_matrix4x3;
struct s_model_target;

extern "C" bool aim_assist_teamless_vehicle_should_be_aimed_at(long vehicle_index,long source_index,long extra,long team,bool *friendly);
extern "C" bool aim_assist_teamless_vehicle_has_passenger(long vehicle_index,long team,bool *friendly_passenger);
void pill_compute_autoaim_target(union real_point3d const * pill_base, union real_vector3d const * pill_axis, float pill_radius,
    union real_point3d const * aim_point, union real_vector3d const * aim_vector, union real_point3d * target);
int compare_targets(const void *first, const void *second);
bool aim_assist_clear_line_of_sight(const real_point3d *source_point, const real_point3d *destination_point, float target_radius,
    long shooter_object_index, long destination_object_index, bool is_melee_only);
extern "C" bool aim_assist_retrieve_targeting_info(const s_aim_assist_targeting_result *targeting_result,
    long *object_index, long *model_target_index, const s_model_target **model_target,
    real_matrix4x3 *matrix, real_matrix4x3 *matrix2, bool *has_second);
long unit_ignore_object_index_get(long unit_index);
bool aim_assist_object_should_be_aimed_at(long object_index,long source_index,long ignored_index,e_game_team team,bool *friendly);

struct s_aim_assist_target;

bool aim_assist_from_targeting(const aim_assist_parameters *parameters, const real_point3d *point, const real_vector3d *vector, long source_index,
    long ignored_index, e_game_team team, const s_aim_assist_targeting_result *targeting_result, s_aim_assist_target *target);
extern "C" bool aim_assist_compute_target_from_model(const aim_assist_parameters *parameters, long object_index, long source_index,
    bool friendly, long model_target_index, const s_model_target *model_target, const real_matrix4x3 *matrix,
    const real_matrix4x3 *matrix2, const real_point3d *point, const real_vector3d *vector, s_aim_assist_target *target);

struct aim_assist_parameters;
struct s_cluster_reference;
template<typename, long> class c_static_array;

long find_aim_assist_targets(const aim_assist_parameters *, s_cluster_reference, const real_point3d *, const real_vector3d *, long,
    e_game_team, long, c_static_array<s_aim_assist_target, 64> &);
long find_aim_assist_targets_recursive(const aim_assist_parameters *, long, const real_point3d *, const real_vector3d *, float, float,
    float, long, e_game_team, long, long, c_static_array<s_aim_assist_target, 64> &);
long aim_assist_compute_targets_from_object(const aim_assist_parameters *, long, long, bool, const real_point3d *,
    const real_vector3d *,
    long, c_static_array<s_aim_assist_target, 64> &);
extern "C" float k_aim_assist_seated_min_angle;

extern const aim_assist_parameters *g_aim_assist_parameters;

__forceinline void aim_assist_targeting_clear(s_aim_assist_targeting_result *targeting)
{
    memset(targeting, 0, sizeof(*targeting));
    targeting->target_object_index = NONE;
    targeting->model_target = NONE;
    targeting->target_player_index = NONE;
}

#endif // GAME_AIM_ASSIST_H
