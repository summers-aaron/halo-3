#include "memory/data.h"
// ailibxbox/actor_moving_vehicle_directional_ground.obj: from the records 0x56af20 0x56b020 0x56b5c8 0x56c178
// @flags /TP /O2 /fp:fast
#include "objects/object_header.h"
#include "math/real_math_types.h"
#include "math/real_math.h"
#include "math/rotate_vector2d.h"
#include "ai/actor_datum.h"
#include "units/unit_types.h"
#include "game/game_types.h"
// @address 0x56af20 ?rate_turn@@YAXJJPAUcharacter_vehicle_properties@@PAUturn_info@@MPATreal_point2d@@PATreal_vector2d@@M3@Z
#include <math.h>
#include "ai/ai_reference_frame.h"
#include "ai/path_obstacles.h"
#include "ai/actor_moving_vehicle_directional_ground.h"
#include "ai/character_definitions.h"
#include "ai/pathfinding_utilities.h"
#include "ai/sector.h"
#include "game/game_allegiance.h"
#include "ai/actor_moving_types.h"

#include "ai/pathfinding_collision.h"
#include "ai/path_obstacle_types.h"
#include "ai/special_movement_types.h"
#include "memory/tls_slot_aliases.h"

struct character_vehicle_properties;

void rate_turn(long a, long b, character_vehicle_properties *properties, turn_info *turn, float f1,
  real_point2d *point, real_vector2d *vector, float speed, real_vector2d *facing)
{
  bool backwards = turn->angle < 0.0f;
  bool moving = speed > 0.0f;
  float rating = 10.0f;
  if ((backwards || turn->reverse) && moving)
  {
    float scaled_speed = speed * (1.0f / 15.0f);
    rating = rating / (1.0f + 0.5f * scaled_speed);
  }
  if (backwards ^ turn->reverse)
    rating = rating / (1.0f + f1 * 0.1f);
  if (turn->reverse)
    rating *= 0.8f;
  rating = rating / (1.0f + (float)fabs(turn->angle) * 0.1f);
  if (facing)
  {
    rating *= 3.0f + dot_product2d(&turn->direction, facing);
    rating *= 0.25f;
  }
  turn->rating = rating;
}

// @rdata 0x8202490c
static const float directional_ground_normal_epsilon = 0.0001f;
static __forceinline float directional_ground_normalize(real_vector2d *vector)
{
    float length = magnitude2d(vector);
    if (!(fabs(length) < directional_ground_normal_epsilon)) {
        scale_vector2d(vector, 1.0f / length, vector);
    } else {
        length = 0.0f;
    }
    return length;
}
static __forceinline float directional_ground_arccosine(float cosine)
{
    return (float)acos((double)PIN(cosine, -1.0f, 1.0f));
}
// @address 0x56b020 ?generate_turns@@YAFJJPAUcharacter_vehicle_properties@@PATreal_point3d@@PATreal_vector2d@@22MPBTreal_vector3d@@PAUturn_info@@F@Z
short generate_turns(long actor_index, long object_index, character_vehicle_properties *properties,
    real_point3d *position, real_vector2d *forward, real_vector2d *right, real_vector2d *facing,
    float radius, const real_vector3d *displacement, turn_info *turns, short maximum_turn_count)
{
    object_datum *object = (object_datum *)object_get_and_verify_type(object_index, 8195);
    short turn_count = 0;
    real_point2d target;
    point_from_line2d(&position->xy, &displacement->xy, 1.0f, &target);
    real_vector2d movement_direction = { displacement->i, displacement->j };
    directional_ground_normalize(&movement_direction);
    BLAM_ASSUME(maximum_turn_count >= 8);
    real_point2d centers[2];
    point_from_line2d(&position->xy, right, -radius, &centers[0]);
    point_from_line2d(&position->xy, right, radius, &centers[1]);
    bool moving_forward = dot_product2d(forward, &movement_direction) >= 0.0f;
    float speed = dot_product2d(&object->linear_velocity.xy, forward);
    for (short side = 0; side < 2; ++side) {
        float distance = distance2d(&centers[side], &target);
        bool left = side == 1;
        if (distance < properties->minimum_turn_distance) {
            turns[turn_count].angle = 1.5707963705062866f;
            if (!left)
                rotate_vector2d(forward, (float)sin(-1.5707963705062866f),
                    (float)cos(1.5707963705062866f), &turns[turn_count].direction);
            else
                rotate_vector2d(forward, (float)sin(1.5707963705062866f),
                    (float)cos(1.5707963705062866f), &turns[turn_count].direction);
            turns[turn_count].left = left;
            turns[turn_count].reverse = false;
            turns[turn_count].rating = 1.0f;
            turns[turn_count].index = turn_count;
            ++turn_count;
            turns[turn_count].angle = -1.5707963705062866f;
            if (!left)
                rotate_vector2d(forward, (float)sin(1.5707963705062866f),
                    (float)cos(-1.5707963705062866f), &turns[turn_count].direction);
            else
                rotate_vector2d(forward, (float)sin(-1.5707963705062866f),
                    (float)cos(-1.5707963705062866f), &turns[turn_count].direction);
            turns[turn_count].left = left;
            turns[turn_count].reverse = false;
            turns[turn_count].rating = 1.0f;
            turns[turn_count].index = turn_count;
            ++turn_count;
        } else {
            short tangent_count = 2;
            real_vector2d toward_target;
            vector_from_points2d(&centers[side], &target, &toward_target);
            directional_ground_normalize(&toward_target);
            float tangent_offset;
            if (radius >= distance) {
                tangent_count = 1;
                tangent_offset = 0.0f;
            } else {
                tangent_offset = directional_ground_arccosine(radius / distance);
            }
            float target_cosine = dot_product2d(right, &toward_target);
            target_cosine = PIN(target_cosine, -1.0f, 1.0f);
            float angle = directional_ground_arccosine(target_cosine);
            if (left) {
                if (moving_forward)
                    angle = 3.1415927410125732f - angle;
                else
                    angle += 3.1415927410125732f;
            } else if (!moving_forward) {
                angle = 6.2831854820251465f - angle;
            }
            angle = PIN(angle, 0.0f, 6.2831854820251465f);
            float tangent_angle[2];
            tangent_angle[0] = angle + tangent_offset;
            if (tangent_angle[0] > 6.284185409545898f)
                tangent_angle[0] -= 6.2831854820251465f;
            tangent_angle[1] = angle - tangent_offset;
            if (tangent_angle[1] < -0.0010000000474974513f)
                tangent_angle[1] += 6.2831854820251465f;
            for (short tangent = 0; tangent < tangent_count; ++tangent) {
                if (!(tangent_angle[tangent] >= 0.0f))
                    tangent_angle[tangent] = 0.0f;
                float complement = tangent_angle[tangent] - 6.2831854820251465f;
                float rotation_angle = left ? complement : -complement;
                real_vector2d direction;
                rotate_vector2d(forward, (float)sin(rotation_angle), (float)cos(rotation_angle), &direction);
                bool points_forward = dot_product2d(&direction, &movement_direction) > 0.0f;
                turns[turn_count].angle = tangent_angle[tangent];
                turns[turn_count].left = left;
                turns[turn_count].reverse = !points_forward;
                turns[turn_count].direction = direction;
                turns[turn_count].index = turn_count;
                rate_turn(actor_index, object_index, properties, &turns[turn_count], distance, &centers[side], forward, speed, facing);
                ++turn_count;
                turns[turn_count].angle = complement;
                turns[turn_count].left = left;
                turns[turn_count].reverse = points_forward;
                turns[turn_count].direction = direction;
                turns[turn_count].index = turn_count;
                rate_turn(actor_index, object_index, properties, &turns[turn_count], distance, &centers[side], forward, speed, facing);
                ++turn_count;
            }
        }
    }
    return turn_count;
}

static __forceinline bool directional_ground_sector_valid(c_sector_ref sector)
{
    return sector.structure_bsp_index != -1 || sector.sector_index != 0xffff;
}
static __forceinline float directional_ground_dot_product3d(const real_vector3d *a, const real_vector3d *b)
{
    return a->i * b->i + (a->j * b->j + a->k * b->k);
}
// @rdata 0x820226cc
static const float directional_ground_side_test_fraction = 0.7f;
// @rdata 0x82029244
static const float directional_ground_quarter_turn = 0.7853982f;
// @rdata 0x820340ac
static const float directional_ground_minimum_rating_scale = 0.33f;
// @rdata 0x82022510
static const float directional_ground_short_side_fraction = 0.8f;

// @address 0x56b5c8 ?test_turn@@YA_NJPAUturn_info@@PAUunit_datum@@PAUcharacter_vehicle_properties@@PATreal_point3d@@PATreal_vector3d@@4MPAUobstacles@@@Z
bool test_turn(long actor_index, turn_info *turn, unit_datum *unit, character_vehicle_properties *properties,
    real_point3d *position, real_vector3d *forward, real_vector3d *right, float radius, obstacles *obstacles)
{
    bool result = true;
    // match: 0x56b5c8: the raw TLS slot, not tls_actor_data(); the inline getter swaps r10/r11 here (probe 2026-11-14)
    actor_datum *actor = &((actor_datum *)actor_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(actor_index)];
    bool left_blocked = false;
    bool center_blocked = false;
    bool right_blocked = false;
    bool blocked = false;
    bool facing_destination = false;
    if (!actor->blind_panic_control_active) {
        real_point2d center;
        if (turn->left)
            point_from_line2d(&position->xy, &right->xy, radius, &center);
        else
        {
            float negative_radius = -radius;
            point_from_line2d(&position->xy, &right->xy, negative_radius, &center);
        }
        float pathfinding_radius = properties->pathfinding_radius;
        actor_find_pathfinding_location(actor_index);
        const real_point3d *point = &actor->pathfinding_point.point;
        real_vector3d local_right_vector;
        ai_reference_frame_inverse_transform_vector(actor->pathfinding_point.location[0], right, &local_right_vector);
        const real_vector3d *local_right = &local_right_vector;
        BLAM_ASSUME_VALID_REAL(local_right_vector.j);
        real_point2d local_center;
        if (turn->left)
            point_from_line2d(&point->xy, &local_right->xy, radius, &local_center);
        else
            point_from_line2d(&point->xy, &local_right->xy, -radius, &local_center);
        real_vector3d local_left;
        scale_vector3d(local_right, -1.0f, &local_left);
        float destination_distance_squared;
        real_vector3d destination_direction;
        if (actor_moving_try_and_get_path_direction(actor_index, &destination_distance_squared, &destination_direction))
            facing_destination = destination_distance_squared < properties->deceleration_distance * properties->deceleration_distance;
        c_sector_ref sector = actor->pathfinding_sector;
        path_collision_result right_result;
        path_collision_result left_result;
        c_sector_ref right_sector;
        c_sector_ref left_sector;
        if (directional_ground_sector_valid(sector)) {
            pathfinding_test_vector(point, sector, g_sector_none, local_right, pathfinding_radius * directional_ground_side_test_fraction, NULL, &right_result);
            right_sector = right_result.sector;
            pathfinding_test_vector(point, sector, g_sector_none, &local_left, pathfinding_radius * directional_ground_side_test_fraction, NULL, &left_result);
            left_sector = left_result.sector;
        } else {
            return true;
        }

        real_vector2d right_offset;
        real_vector2d left_offset;
        real_vector2d center_offset;
        float right_distance;
        float left_distance;
        if (turn->left) {
            right_distance = radius - right_result.t;
            left_distance = radius + left_result.t;
            scale_vector2d(&local_right->xy, -right_distance, &right_offset);
            scale_vector2d(&local_right->xy, -left_distance, &left_offset);
            scale_vector2d(&local_right->xy, -radius, &center_offset);
        } else {
            right_distance = radius + right_result.t;
            left_distance = radius - left_result.t;
            scale_vector2d(&local_right->xy, right_distance, &right_offset);
            scale_vector2d(&local_right->xy, left_distance, &left_offset);
            scale_vector2d(&local_right->xy, radius, &center_offset);
        }
        real_point3d center_point = *point;
        real_point3d right_point;
        point_from_line2d(&point->xy, &local_right->xy, right_result.t, &right_point.xy);
        right_point.z = point->z;
        real_point3d left_point;
        point_from_line2d(&point->xy, &local_right->xy, -left_result.t, &left_point.xy);
        left_point.z = point->z;

        float turn_steps = ceil(fabs(turn->angle) / 0.7853982f);
        short step_count = (short)turn_steps;
        for (short step = 1; step <= step_count; step++) {
            float angle;
            if (turn->angle < 0.0f)
                angle = (float)(-step) * 0.7853982f;
            else
                angle = (float)step * 0.7853982f;
            if (step == step_count)
                angle = turn->angle;
            if (!turn->left)
                angle *= -1.0f;
            float sine = (float)sin(angle);
            float cosine = (float)cos(angle);
            union point_with_direction {
                real_point3d point;
                struct { real_vector2d direction; float z; };
            };
            point_with_direction new_right;
            point_with_direction new_center;
            point_with_direction new_left;
            if (!right_blocked) {
                rotate_vector2d(&right_offset, sine, cosine, &new_right.direction);
                point_from_line2d(&local_center, &new_right.direction, 1.0f, &new_right.point.xy);
                new_right.point.z = point->z;
            }
            rotate_vector2d(&center_offset, sine, cosine, &new_center.direction);
            point_from_line2d(&local_center, &new_center.direction, 1.0f, &new_center.point.xy);
            new_center.point.z = point->z;
            if (!left_blocked) {
                rotate_vector2d(&left_offset, sine, cosine, &new_left.direction);
                point_from_line2d(&local_center, &new_left.direction, 1.0f, &new_left.point.xy);
                new_left.point.z = point->z;
            }
            real_vector3d direction;
            vector_from_points3d(&center_point, &new_center.point, &direction);
            float center_length = normalize3d(&direction) + pathfinding_radius;
            if (facing_destination && directional_ground_dot_product3d(&direction, &destination_direction) > 0.70710677f)
                break;
            if (!right_blocked) {
                float right_travel = distance3d(&new_right.point, &right_point);
                float right_length = right_travel + pathfinding_radius;
                right_blocked = pathfinding_test_vector(&right_point, right_sector, g_sector_none, &direction, right_length, NULL, &right_result);
                if (!right_blocked) {
                    pathfinding_test_vector(&right_point, right_sector, g_sector_none, &direction, right_length - pathfinding_radius, NULL, &right_result);
                    right_sector = right_result.sector;
                } else {
                    turn->rating *= (1.0f - directional_ground_minimum_rating_scale) * (float)(step - 1) / (float)step_count +
                        directional_ground_minimum_rating_scale;
                    result = false;
                    if (step == 1 && right_result.t < pathfinding_radius * directional_ground_short_side_fraction)
                        blocked = true;
                }
            }
            path_collision_result center_result;
            center_blocked = pathfinding_test_vector(&center_point, sector, g_sector_none, &direction, center_length, NULL, &center_result);
            if (!center_blocked) {
                pathfinding_test_vector(&center_point, sector, g_sector_none, &direction, center_length - pathfinding_radius, NULL, &center_result);
                sector = center_result.sector;
            }
            if (!left_blocked) {
                float left_length = distance3d(&new_left.point, &left_point) + pathfinding_radius;
                left_blocked = pathfinding_test_vector(&left_point, left_sector, g_sector_none, &direction, left_length, NULL, &left_result);
                if (!left_blocked) {
                    pathfinding_test_vector(&left_point, left_sector, g_sector_none, &direction, left_length - pathfinding_radius, NULL, &left_result);
                    left_sector = left_result.sector;
                } else {
                    turn->rating *= (1.0f - directional_ground_minimum_rating_scale) * (float)(step - 1) / (float)step_count +
                        directional_ground_minimum_rating_scale;
                    result = false;
                    if (step == 1 && left_result.t < pathfinding_radius * directional_ground_short_side_fraction)
                        blocked = true;
                }
            }
            if (blocked || center_blocked || (right_blocked && left_blocked)) {
                turn->angle = turn->left ? angle : -angle;
                turn->rating *= (float)(step - 1) / (float)step_count;
                result = false;
                // match: 0x56b5c8: forward skip keeps one angle/rating store pair; break+recheck duplicates them, shifting frame/register colors (436w).
                goto finished;
            }
            center_point = new_center.point;
            right_point = new_right.point;
            left_point = new_left.point;
        }

        if (obstacles) {
            short best_index = NONE;
            float best_penetration = 0.0f;
            for (short obstacle_index = 0; obstacle_index < obstacles->disc_count; obstacle_index++) {
                const obstacle_disc *disc = &obstacles->discs[obstacle_index];
                object_datum *object = object_get(disc->object_index);
                if (object->object_identifier.get_type() == _object_type_biped) {
                    unit_datum *obstacle_unit = (unit_datum *)object;
                    if (obstacle_unit->unit.actor_index != NONE) {
                        actor_datum *obstacle_actor = datum_get<actor_datum>(tls_actor_data(), obstacle_unit->unit.actor_index);
                        if (game_team_is_enemy((e_game_team)actor->team, (e_game_team)obstacle_actor->team))
                            continue;
                    }
                }
                float distance = distance2d(&disc->center, &center);
                float maximum_distance = disc->radius + radius + pathfinding_radius;
                float minimum_distance = radius - pathfinding_radius - disc->radius;
                if (distance < maximum_distance && distance > minimum_distance) {
                    real_vector2d obstacle_direction;
                    vector_from_points2d(&center, &disc->center, &obstacle_direction);
                    directional_ground_normalize(&obstacle_direction);
                    bool in_front = dot_product2d(&obstacle_direction, &forward->xy) >= 0.0f;
                    float obstacle_cosine = dot_product2d(&obstacle_direction, &right->xy);
                    obstacle_cosine = (float)(obstacle_cosine > 1.0 ? 1.0 : (obstacle_cosine < -1.0 ? -1.0 : obstacle_cosine));
                    float obstacle_angle = directional_ground_arccosine(obstacle_cosine);
                    if (turn->left) {
                        if (in_front)
                            obstacle_angle = 3.1415927f - obstacle_angle;
                        else
                            obstacle_angle = 3.1415927f + obstacle_angle;
                    } else if (!in_front) {
                        obstacle_angle = 6.2831855f - obstacle_angle;
                    }
                    bool intersects = false;
                    if (turn->angle > 0.0)
                        intersects = turn->angle > obstacle_angle;
                    else
                        intersects = turn->angle + 6.2831855f < obstacle_angle;
                    float penetration = 0.0f;
                    if (intersects) {
                        if (distance < radius)
                            penetration = distance - minimum_distance;
                        else
                            penetration = maximum_distance - distance;
                    } else {
                        real_vector2d end_offset;
                        real_point2d end_point;
                        if (turn->left)
                            rotate_vector2d(&right->xy,
                                (float)sin(3.1415927f + turn->angle), (float)cos(3.1415927f + turn->angle),
                                &end_offset);
                        else
                            rotate_vector2d(&right->xy, (float)sin(-turn->angle),
                                (float)cos(-turn->angle), &end_offset);
                        scale_vector2d(&end_offset, radius, &end_offset);
                        point_from_line2d(&center, &end_offset, 1.0f, &end_point);
                        penetration = disc->radius + pathfinding_radius - distance2d(&disc->center, &end_point);
                        if (penetration > 0.0f)
                            intersects = true;
                    }
                    if (intersects) {
                        float penetration_fraction = penetration / pathfinding_radius;
                        if (best_index == NONE || best_penetration < penetration_fraction) {
                            best_index = obstacle_index;
                            best_penetration = penetration_fraction;
                        }
                    }
                }
            }
            if (best_index != NONE) {
                result = false;
                if (best_penetration > 1.0)
                    turn->rating = 0.0f;
                else
                    turn->rating *= 1.0f - best_penetration;
            }
        }
finished:
        return result;
    }
    return true;
}

static __forceinline void directional_ground_obstacles_new(obstacles *list)
{
    list->obstacle_count = 0;
    list->disc_count = 0;
    list->flag1_count = 0;
    list->flag8_count = 0;
    list->unused = 0;
}

// @rdata 0x820397e4
static const float directional_ground_default_maximum_speed = 6.0f;
// @rdata 0x82021c3c
static const float directional_ground_default_error_scale = 4.02f;
// @rdata 0x82022838
static const float directional_ground_minimum_steering_scale = 0.05f;
// @rdata 0x82022dc0
static const float directional_ground_half = 0.5f;
// @rdata 0x820242d8
static const float directional_ground_two = 2.0f;


// @address 0x56c178
e_movement_result actor_move_calculate_directional_ground_movement(long actor_index, long vehicle_index,
    const real_vector3d *displacement, real_vector3d *out_direction, real_vector3d *out_throttle, short iteration)
{
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    unit_datum *unit = (unit_datum *)object_get(vehicle_index);
    character_vehicle_properties *properties = character_vehicle_properties_get(actor_index, unit->object.definition_index);
    obstacles *obstacle_list = 0;
    bool next_step_valid = false;
    bool update_turn = false;
    bool movement_valid = true;
    bool last_step = false;
    e_movement_result result = _movement_result_none;
    const real_vector2d *turn_direction = 0;
    float throttle_delta = 0.0f;
    if (iteration == 0)
    {
        actor->vehicle_turn.previous_throttle = actor->vehicle_turn.throttle;
        actor->vehicle_turn.previous_steering = actor->vehicle_turn.steering;
    }
    actor->flags &= ~FLAG(9);
    real_vector3d forward = actor->facing_vector;
    forward.k = 0.0f;
    normalize3d(&forward);
    float facing_angle = signed_angle_between_vectors2d(global_forward2d, &forward.xy);
    // match: 0x56c178: zero-bit rotate preserves 32-bit GPR value
    if (!TEST_BIT((long)_rotl((unsigned long)actor->flags, 0), 0) || !properties)
    {
        actor->vehicle_turn.throttle = 0.0f;
        actor->vehicle_turn.valid = false;
    }
    else
    {
        real_point3d position;
        object_get_origin(vehicle_index, &position);
        actor_path *path = &actor->path;
        real_vector3d direction = *displacement;
        direction.k = 0.0f;
        float distance = normalize3d(&direction);
        float radius = properties->turning_radius;
        actor_find_pathfinding_location(actor_index);
        real_point3d destination;
        point_from_line3d(&position, &unit->object.linear_velocity, properties->velocity_lookahead, &destination);
        real_vector3d next_direction;
        if (actor_moving_has_vehicle_reached_step(actor_index, vehicle_index, &destination, distance, false, &last_step, &next_step_valid, &next_direction))
        {
            result = _movement_result_advance;
        }
        else
        {
            if (next_step_valid)
            {
                next_direction.k = 0.0f;
                if (normalize3d(&next_direction) == 0.0f)
                    next_step_valid = false;
            }
            real_vector3d right;
            right.i = -forward.j;
            right.j = forward.i;
            right.k = 0.0f;
            obstacles obstacle_storage;
            obstacle_list = &obstacle_storage;
            if (actor->ignore_obstacles)
                obstacle_list = 0;
            else
            {
                special_movement special;
                actor_special_movement_new(actor_index, &special);
                directional_ground_obstacles_new(&obstacle_storage);
                obstacles_add_nearby_discs(actor_index, 129, 2, true, &special, &obstacle_storage, 0, &position, 0,
                    radius * 3.0f, &direction, 0, vehicle_index, NONE);
                obstacles_recompute(&obstacle_storage, properties->pathfinding_radius);
            }
            turn_info *state = &actor->vehicle_turn;
            if (actor->uncovering)
                update_turn = true;
            else if (!state->valid)
                update_turn = true;
            else
            {
                float alignment = dot_product2d(&state->direction, &forward.xy);
                if (alignment > 0.0f)
                {
                    if (alignment > 0.93f)
                        update_turn = true;
                    else
                    {
                        turn_direction = &state->direction;
                        bool left = dot_product2d(turn_direction, &right.xy) > 0.0f;
                        update_turn = (state->left && ((state->angle > 0.0f && !left) || (state->angle < 0.0f && left))) ||
                            (!state->left && ((state->angle > 0.0f && left) || (state->angle < 0.0f && !left)));
                    }
                }
            }
            if (update_turn)
            {
                turn_info turns[8];
                short turn_count = generate_turns(actor_index, vehicle_index, properties, &position,
                    &forward.xy, &right.xy, next_step_valid ? &next_direction.xy : 0,
                    radius, displacement, turns, 8);
                if (fabs(state->angle) > 0.1f)
                    turns[state->index].rating *= 1.1f;
                short tested = 0;
                short best_turn;
                short first_turn = NONE;
                movement_valid = false;
                while (!movement_valid)
                {
                    float best_rating = 0.0f;
                    best_turn = NONE;
                    for (short turn_index = 0; turn_index < turn_count; ++turn_index)
                    {
                        if (turns[turn_index].rating > best_rating)
                        {
                            best_rating = turns[turn_index].rating;
                            best_turn = turn_index;
                        }
                    }
                    if (best_turn == NONE)
                        break;
                    if (first_turn == NONE)
                        first_turn = best_turn;
                    if (TEST_BIT(tested, best_turn))
                    {
                        if (turns[best_turn].rating <= 0.0f)
                            break;
                        movement_valid = true;
                    }
                    const unsigned long &turn_clear = test_turn(
                        actor_index, &turns[best_turn], unit, properties, &position, &forward, &right, radius, obstacle_list);
                    if (turn_clear)
                        movement_valid = true;
                    else
                        tested |= FLAG(best_turn);
                }
                if (!movement_valid)
                    best_turn = first_turn;
                if (best_turn != NONE)
                {
                    state->left = turns[best_turn].left;
                    state->reverse = turns[best_turn].reverse;
                    state->angle = turns[best_turn].angle;
                    state->rating = turns[best_turn].rating;
                    state->direction = turns[best_turn].direction;
                    state->index = turns[best_turn].index;
                    state->valid = true;
                }
            }
            else
            {
                float angle_delta = facing_angle - actor->vehicle_turn.facing_angle;
                if (state->left)
                    state->angle -= angle_delta;
                else
                    state->angle += angle_delta;
            }
            if (movement_valid)
            {
                float round_angle = 0.0f;
                if (next_step_valid && state->angle > 0.0f && !state->reverse)
                    round_angle = actor_moving_vehicle_round_turn(actor_index, vehicle_index, !last_step,
                        &position.xy, &path->points[path->point_index].point.point.xy,
                        &next_direction.xy, obstacle_list);
                float angle = state->left ? state->angle : -state->angle;
                float scale = 0.0f;
                if (actor->combat_status > 3)
                    scale = 1.0f;
                else
                {
                    float stopping_scale = properties->deceleration_distance / (MAX(distance, 0.001f) * directional_ground_two) * directional_ground_half;
                    float turn_scale = (1.0f - MAX(dot_product2d(&state->direction, &forward.xy), 0.0f)) * directional_ground_half;
                    scale = MAX(stopping_scale, turn_scale);
                    scale *= directional_ground_two;
                    scale = properties->minimum_urgency + (1.0f - properties->minimum_urgency) * scale;
                    scale = PIN(scale, directional_ground_minimum_steering_scale, 1.0f);
                }
                actor->vehicle_turn.steering_scale = scale;
                steering_control2d(actor_index, vehicle_index, &forward, angle, round_angle, scale,
                    actor->vehicle_turn.previous_steering, &actor->vehicle_turn.steering, out_direction);
                float speed = 1.0f;
                if (state->angle >= 0.0f)
                    speed = actor_moving_vehicle_desired_speed(actor_index, vehicle_index, &position.xy,
                        &direction.xy, &out_direction->xy, &forward.xy,
                        next_step_valid ? &next_direction.xy : 0, distance, false);
                else
                    speed *= -1.0f;
                float forward_speed = directional_ground_dot_product3d(&unit->object.linear_velocity, &forward);
                float maximum_speed;
                float error_scale;
                if (properties->theoretical_maximum_speed > 0.0f)
                {
                    error_scale = properties->error_scale;
                    maximum_speed = properties->theoretical_maximum_speed;
                }
                else
                {
                    error_scale = directional_ground_default_error_scale;
                    maximum_speed = directional_ground_default_maximum_speed;
                }
                float throttle = speed + (speed - (forward_speed / maximum_speed > 1.0f ? 1.0f : forward_speed / maximum_speed)) * error_scale;
                if (throttle > 0.0f)
                {
                    if (properties->throttle_delta > 0.0f)
                    {
                        throttle_delta = throttle - actor->vehicle_turn.previous_throttle;
                        throttle_delta = MIN(throttle_delta, properties->throttle_delta);
                        throttle = actor->vehicle_turn.previous_throttle + throttle_delta;
                    }
                }
                actor->vehicle_turn.throttle = MAX(throttle, 0.0f);
                float throttle_maximum = properties->throttle_maximum;
                if (actor->throttle_override_valid &&
                    actor->throttle_override != 0.0f)
                    throttle_maximum = actor->throttle_override;
                out_throttle->i = throttle;
                if (throttle > throttle_maximum)
                    throttle = throttle_maximum;
                else if (throttle < -throttle_maximum)
                    throttle = -throttle_maximum;
                out_throttle->i = throttle;
                if (throttle > 1.0f)
                    throttle = 1.0f;
                else if (throttle < -1.0f)
                    throttle = -1.0f;
                out_throttle->i = throttle;
            }
            else
            {
                actor->flags &= ~FLAG(0);
                actor->flags &= ~FLAG(9);
                result = _movement_result_done;
            }
        }
    }
    actor->vehicle_turn.facing_angle = facing_angle;
    return result;
}
