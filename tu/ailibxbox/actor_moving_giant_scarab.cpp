// ailibxbox/actor_moving_giant_scarab.obj: from the records 0x56e2b8 0x56e488 0xc6b00
// @flags /TP /O2 /Oc

#include "ai/pathfinding_collision.h"
#include "memory/data.h"
#include "ai/actor_behavior_state.h"
#include "ai/ai_direction_mode_types.h"
#include "cseries/cseries.h"
#include "math/real_math.h"
#include "math/matrix_math.h"
#include "memory/tls_globals.h"
#include "ai/sector_ref.h"
#include "tag_files/tag_types.h"
#include <math.h>
#include "objects/objects.h"
#include "ai/character_definitions.h"
#include "ai/pathfinding_utilities.h"
#include "ai/sector.h"
#include "ai/actors.h"
#include "ai/actor_moving.h"
#include "cache/cache_files.h"

#include "ai/ai_direction.h"
#include "ai/path_obstacle_types.h"
#include "ai/path_types.h"
#include "ai/actor_moving_types.h"
#include "ai/actor_datum.h"
#include "ai/pathfinding_pill_result.h"
#include "objects/object_header.h"
#include "units/unit_types.h"

#include "ai/character_scarab_properties.h"

static __forceinline float scarab_cosine(float angle)
{
    return (float)cos((double)angle);
}
static __forceinline float scarab_sine(float angle)
{
    return (float)sin((double)angle);
}
static __forceinline void scarab_rotate_vector_about_axis(real_vector3d *vector, const real_vector3d *axis, float sine, float cosine)
{
    float scale = (axis->i * vector->i + axis->j * vector->j + axis->k * vector->k) * (1.0f - cosine);
    float cross_i = vector->j * axis->k - vector->k * axis->j;
    float cross_j = vector->k * axis->i - vector->i * axis->k;
    float cross_k = vector->i * axis->j - vector->j * axis->i;
    vector->i = axis->i * scale + vector->i * cosine - cross_i * sine;
    vector->j = axis->j * scale + vector->j * cosine - cross_j * sine;
    vector->k = axis->k * scale + vector->k * cosine - cross_k * sine;
}

// @address 0x56e2b8 ?steering_control@@YAXJJMPATreal_vector2d@@PATreal_vector3d@@1@Z
void steering_control(long actor_index, long object_index, float offset, real_vector2d *forward, real_vector3d *desired, real_vector3d *result)
{
    (void)offset;
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    unit_datum *object = (unit_datum *)object_header_get(object_index)->datum;
    character_vehicle_properties *properties = character_vehicle_properties_get(actor_index, object->object.definition_index);
    float angle = signed_angle_between_vectors2d(forward, &desired->xy);
    if (properties->steering_angle_scale > 0.0f)
        angle *= properties->steering_angle_scale;
    float maximum = properties->maximum_steering_angle > 0.0f ? 0.017453292f * properties->maximum_steering_angle : 1.5707964f;
    angle = PIN(angle, -maximum, maximum);
    float delta = angle - actor->ground_steering.previous_steering;
    float delta_maximum = 6.2831855f;
    if (properties->maximum_steering_delta > 0.0f)
        delta_maximum = 0.017453292f * properties->maximum_steering_delta;
    delta = PIN(delta, -delta_maximum, delta_maximum);
    angle = delta + actor->ground_steering.previous_steering;
    real_vector3d direction;
    direction.i = forward->i;
    direction.j = forward->j;
    direction.k = 0.0f;
    float cosine = scarab_cosine(angle);
    float sine = scarab_sine(angle);
    scarab_rotate_vector_about_axis(&direction, global_up3d, sine, cosine);
    actor->ground_steering.steering = angle;
    *result = direction;
}

// @address 0x56e488 ?stationary_steering_control@@YAXJJPATreal_vector2d@@PATreal_vector3d@@@Z
void stationary_steering_control(long actor_index, long object_index, real_vector2d *forward, real_vector3d *result)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    real_vector3d direction;
    if (actor->flags & 2)
    {
        direction = actor->steering_vector;
        direction.k = 0.0f;
        if (normalize3d(&direction) == 0.0f)
            direction = actor->facing_vector;
    }
    else if (!(actor->look_mode >= 3 && actor->look_direction.get(actor_index, _ai_direction_mode_vector, NULL, &direction, NULL, NULL)))
    {
        direction = actor->facing_vector;
    }
    steering_control(actor_index, object_index, 0.0f, forward, &direction, result);
}




static __forceinline bool actor_moving_giant_scarab_unit_pathfinding_is_attached(long unit_index)
{
    return (((unit_datum *)object_header_get(unit_index)->datum)->unit.flags400 & 1) != 0;
}

// @address 0xc6b00 ?actor_move_calculate_giant_scarab_movement@@YA?AW4e_movement_result@@JJPATreal_vector3d@@00F@Z
e_movement_result actor_move_calculate_giant_scarab_movement(
    long actor_index, long object_index, real_vector3d *movement, real_vector3d *desired, real_vector3d *result, short iteration)
{
    actor_datum *actor = &((actor_datum *)((s_data_array *)actor_data)->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(actor_index)];
    unit_datum *object = (unit_datum *)object_header_get(object_index)->datum;
    character_vehicle_properties *properties = character_vehicle_properties_get(actor_index, object->object.definition_index);
    character_scarab_properties *scarab_properties = NULL;
    for (long tag_index = actor->character_definition_index; tag_index != -1; )
    {
        character_definition *definition = (character_definition *)tag_get('char', tag_index);
        if (definition->scarab_properties.count > 0)
        {
            scarab_properties = (character_scarab_properties *)definition->scarab_properties.address;
            break;
        }
        tag_index = definition->parent_character.index;
    }
    e_movement_result movement_result = _movement_result_none;
    float threshold = 0.0f;
    short turn = -1;
    actor->flags &= ~0x200;
    if (iteration == 0)
        actor->ground_steering.previous_steering = actor->ground_steering.steering;
    real_point3d origin;
    object_get_origin(object_index, &origin);
    real_vector3d facing = actor->facing_vector;
    facing.k = 0.0f;
    if (normalize3d(&facing) == 0.0f)
        facing = *global_forward3d;
    if (actor->path_aim)
        actor->flags |= 0x400;
    real_vector3d move = *movement;
    move.k = 0.0f;
    float dist = normalize3d(&move);
    if (dist == 0.0f)
        actor->flags &= ~1;
    real_vector3d left;
    left.i = -facing.j;
    left.j = facing.i;
    left.k = 0.0f;
    normalize3d(&left);
    real_vector3d throttle = *global_zero_vector3d;
    real_vector3d desired_facing = facing;
    if (properties != NULL && scarab_properties != NULL)
    {
        if ((actor->flags & 1) == 0)
        {
            stationary_steering_control(actor_index, object_index, &facing.xy, &desired_facing);
        }
        else if (dist < properties->steering_distance)
        {
            stationary_steering_control(actor_index, object_index, &facing.xy, &desired_facing);
            movement_result = _movement_result_advance;
        }
        else
        {
            real_vector3d free_movement;
            bool hit = false;
            float radius = 8.0f;
            if (properties != NULL)
                radius = properties->pathfinding_radius;
            actor_find_pathfinding_location(actor_index);
            pathfinding_test_pill2d_result pill_result;
            hit = pathfinding_test_pill2d(false, NULL, -1, 2, true, &actor->position.body, actor->pathfinding_sector, g_sector_none, &move,
                radius, dist, false, false, false, NULL, &pill_result);
            if (!hit && (pill_result.left_side_blocked ^ pill_result.right_side_blocked))
            {
                real_vector3d perpendicular;
                if (pill_result.left_side_blocked)
                {
                    perpendicular.i = -move.j;
                    perpendicular.j = move.i;
                    perpendicular.k = 0.0f;
                }
                else
                {
                    perpendicular.i = move.j;
                    perpendicular.j = -move.i;
                    perpendicular.k = 0.0f;
                }
                real_point3d point;
                point_from_line3d(&actor->position.body, &move, dist, &point);
                path_collision_result collision;
                pathfinding_test_vector(&point, pill_result.sector, g_sector_none, &perpendicular, radius, NULL, &collision);
                if (collision.collision)
                {
                    point_from_line3d(&point, &perpendicular, -(radius - collision.t), &point);
                    vector_from_points3d(&actor->position.body, &point, &move);
                    *movement = move;
                    dist = normalize3d(&move);
                }
            }
            threshold = properties->side_slip_distance;
            bool slowing = (actor->flags & 2) != 0;
            if (slowing && dist > properties->pathfinding_radius)
            {
                float limit = actor->ground_steering.direct == 0 ? scarab_properties->indirect_steering_limit : scarab_properties->direct_steering_limit;
                limit = PIN(limit, 0.0f, 1.0f);
                float dot = dot_product2d(&movement->xy, &actor->steering_vector.xy);
                dot /= dist;
                if (dot > limit)
                    slowing = false;
            }
            if (!actor_moving_giant_scarab_unit_pathfinding_is_attached(actor->unit_index) &&
                (slowing || (dist < threshold && actor->destination_orders.flags.test(_destination_flag_1))))
            {
                real_vector3d movement_direction = *movement;
                turn = 0;
                if (slowing)
                {
                    desired_facing = actor->steering_vector;
                    float far_distance = scarab_properties->far_distance;
                    float near_distance = scarab_properties->near_distance;
                    if (dist < far_distance && actor_path_executing(actor_index) && actor->path.valid)
                    {
                        actor_path *path = &actor->path;
                        real_point3d estimated;
                        actor_estimate_position(actor->unit_index, 1, &path->points[path->point_index].point.point, NULL, NULL, NULL, &estimated);
                        real_vector3d direction;
                        if (actor->look_direction.get(actor_index, _ai_direction_mode_vector, &estimated, &direction, NULL, NULL))
                        {
                            float blend = 0.0f;
                            if (dist < near_distance)
                                blend = 1.0f;
                            else
                                blend = 1.0f - (dist - near_distance) / (far_distance - near_distance);
                            desired_facing.k = 0.0f;
                            direction.k = 0.0f;
                            normalize3d(&desired_facing);
                            normalize3d(&direction);
                            scale_vector3d(&direction, blend, &direction);
                            scale_vector3d(&desired_facing, 1.0f - blend, &desired_facing);
                            add_vectors3d(&desired_facing, &direction, &desired_facing);
                        }
                    }
                }
                else if (actor->destination_orders.flags.test(_destination_flag_1))
                {
                    desired_facing = actor->destination_orders.m_destination_facing;
                }
                movement_direction.k = 0.0f;
                desired_facing.k = 0.0f;
                if (normalize3d(&desired_facing) == 0.0f)
                    desired_facing = actor->facing_vector;
                if (normalize3d(&movement_direction) == 0.0f)
                    movement_direction = desired_facing;
                actor_move_calculate_free(actor_index, false, &movement_direction, &facing, &free_movement);
            }
            else
            {
                desired_facing = *movement;
                desired_facing.k = 0.0f;
                if (normalize3d(&desired_facing) == 0.0f)
                    desired_facing = actor->facing_vector;
                turn = 1;
            }
            bool aligned = true;
            if (actor->unit_index != -1 && actor_moving_giant_scarab_unit_pathfinding_is_attached(actor->unit_index) &&
                facing.i * move.i + facing.j * move.j < 0.95f)
            {
                aligned = false;
                actor->flags |= 0x400;
            }
            if (aligned)
            {
                float speed = 1.0f;
                if (turn == 0)
                {
                    throttle = free_movement;
                    scale_vector3d(&throttle, 1.0f / MAX(fabs(throttle.i), MAX(fabs(throttle.j), fabs(throttle.k))), &throttle);
                }
                else
                {
                    throttle = *global_forward3d;
                }
                speed = actor_moving_vehicle_desired_speed(actor_index, object_index, &origin.xy,
                    &move.xy, &desired_facing.xy, &facing.xy, NULL, dist,
                    turn != 1);
                if (properties->throttle_maximum > 0.0f)
                    speed = PIN(speed, -properties->throttle_maximum, properties->throttle_maximum);
                scale_vector3d(&throttle, speed, &throttle);
                throttle.i = PIN(throttle.i, -1.0f, 1.0f);
                throttle.j = PIN(throttle.j, -1.0f, 1.0f);
                throttle.k = PIN(throttle.k, -1.0f, 1.0f);
            }
            actor->ground_steering.direct = turn;
        }
    }
    *desired = desired_facing;
    *result = throttle;
    return movement_result;
}
