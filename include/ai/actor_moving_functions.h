#ifndef AI_ACTOR_MOVING_FUNCTIONS_H
#define AI_ACTOR_MOVING_FUNCTIONS_H

#include "math/real_math_types.h"

class c_ai_point3d;
class c_sector_ref;
class c_destination_orders;
struct firing_position_ref;

// Functions defined in tu/ailibxbox/actor_moving.cpp.

struct obstacles;
struct path_state;
struct special_movement;

void actor_move_calculate_free(
    long actor_index,
    bool three_d,
    union real_vector3d const * movement_direction,
    union real_vector3d const * facing_direction,
    union real_vector3d * result);
bool actor_move_animation_busy(long actor_index);
bool actor_impulse_submit(
    long actor_index,
    long impulse_type,
    unsigned char impulse_flags,
    union real_vector3d const * impulse_position);
bool actor_test_movement_type(long actor_index, long movement_type);
bool actor_path_step_get_vector(long actor_index, short step_index, real_vector3d *vector);
float actor_get_max_jump_speed(long actor_index, bool *out_limited);
bool actor_test_jump(long actor_index, union real_point3d *point, union real_point3d *point2);
bool actor_special_movement_new(long actor_index, special_movement *special);
bool actor_move_halt(long actor_index);
bool actor_move_to_firing_position_update_status(long actor_index, firing_position_ref reference, bool engaged);
bool actor_moving_jump(long actor_index, real_point3d *position, bool force);
void actor_destination_update(long actor_index);
void actor_move_update(long actor_index);
long actor_get_movement_type(long actor_index);
bool steering_control2d(
    long actor_index,
    long vehicle_index,
    const real_vector3d *desired,
    float angle,
    float delta,
    float scale,
    float current,
    float *out_angle,
    real_vector3d *out_direction);
bool actor_move(long actor_index, c_destination_orders *orders, path_state *path_state);
float actor_moving_vehicle_desired_speed(
    long actor_index,
    long object_index,
    const real_point2d *point,
    const real_vector2d *direction,
    const real_vector2d *facing,
    const real_vector2d *movement,
    const real_vector2d *turn,
    float distance,
    bool ignore_direction);
bool actor_path_refresh(long actor_index, bool changed, path_state *extra);
bool actor_move_find_evasion_direction(
    long actor_index,
    real_vector3d *avoidance_vector,
    real_vector3d *alignment_vector,
    short *evade_direction_reference,
    float minimum_distance,
    bool *blocked_reference);
float actor_moving_vehicle_round_turn(
    long actor_index,
    long vehicle_index,
    short type,
    real_point2d *position,
    real_point2d *step,
    real_vector2d *next_step_direction,
    obstacles *obstacle_list);

struct firing_position;
class c_cover_state_data;

extern "C" float actor_path_distance_remaining(long actor_index, short path_index);
extern "C" bool actor_moving_try_and_get_path_direction(
    long actor_index,
    float *distance_squared,
    real_vector3d *direction);
extern "C" bool actor_moving_vehicle_test_destination(
    long i,
    long fp,
    firing_position *f,
    c_cover_state_data *o,
    void *path,
    bool use_path,
    long owner);
extern "C" bool actor_moving_has_vehicle_reached_step(
    long actor_index,
    long vehicle_index,
    real_point3d *position,
    float distance,
    bool flying,
    bool *last_step_ref,
    bool *next_step_valid_ref,
    real_vector3d *next_step_direction_ref);
extern "C" bool actor_moving_is_reserved_hint_less(long a, long b, const void *context);

struct actor_moving_reserved_hints;
class c_hint_ref;
struct path_input;

// match: 0xccbf8: pointer __restrict qualifier prevents alias reload
extern "C" void actor_moving_compute_reserved_hints(long actor_index, actor_moving_reserved_hints *__restrict reserved);
void actor_path_input_new(long actor_index, path_input *input);
bool actor_moving_jump(
    long actor_index,
    c_hint_ref *hint_ref,
    bool test_levels,
    char jump_levels,
    c_ai_point3d *destination,
    c_sector_ref destination_sector,
    bool unlimited);

#endif // AI_ACTOR_MOVING_FUNCTIONS_H
