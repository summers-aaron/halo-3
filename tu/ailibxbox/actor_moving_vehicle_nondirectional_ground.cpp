// ailibxbox/actor_moving_vehicle_ground_steering.obj: from the records 0x56cfb0 0x56d068 0x56d170
// @flags /TP /O2 /Oc

#include "cseries/cseries.h"
#include "math/real_math.h"
#include "memory/data.h"
#include "memory/tls_globals.h"
#include "objects/object_header.h"
#include "ai/ai_point3d.h"
#include "ai/sector_ref.h"
#include "tag_files/tag_types.h"
#include "ai/actors.h"
#include "units/units.h"
#include "ai/ai_reference_frame.h"
#include "ai/path_obstacles.h"
#include "ai/actor_moving_vehicle_directional_ground.h"
#include "ai/character_definitions.h"
#include "ai/pathfinding_utilities.h"
#include "cache/cache_files.h"
#include "ai/ai_direction.h"
#include "ai/ai_direction_mode_types.h"
#include "ai/actor_datum.h"
#include "ai/actor_moving_types.h"
#include "ai/path_obstacle_types.h"
#include "ai/special_movement_types.h"
#include "units/unit_types.h"
#include "units/vehicle_datum.h"
#include "units/unit_definitions.h"
#include "tag_files/tag_groups.h"

// @rdata 0x82021c3c
static const float k_nondirectional_ground_default_acceleration = 4.02f;

static __forceinline bool actor_moving_vehicle_nondirectional_ground_turns_left(const real_vector2d *direction, const real_vector2d *right)
{
    return dot_product2d(direction, right) > 0.0f;
}

// @address 0x56cfb0 ?steering_control@@YAXJJMPATreal_vector3d@@00@Z
void steering_control(long actor_index, long object_index, float offset, real_vector3d *forward, real_vector3d *desired, real_vector3d *result)
{
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    float angle = signed_angle_between_vectors2d(&forward->xy, &desired->xy);
    if (!steering_control2d(actor_index, object_index, forward, angle, offset, 0.0f, actor->ground_steering.previous_steering,
        &actor->ground_steering.steering, result))
    {
        actor->ground_steering.steering = 0.0f;
        *result = *forward;
    }
}

// @address 0x56d068 ?stationary_steering_control@@YAXJJPATreal_vector3d@@0@Z
void stationary_steering_control(long actor_index, long object_index, real_vector3d *forward, real_vector3d *result)
{
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    real_vector3d direction;
    if (actor->flags & 2)
    {
        direction = actor->steering_vector;
        direction.k = 0.0f;
        if (normalize3d(&direction) == 0.0f)
            direction = actor->facing_vector;
    }
    else if (!(actor->look_mode >= 3 &&
        actor->look_direction.get(actor_index, _ai_direction_mode_vector, NULL, &direction, NULL, NULL)))
    {
        direction = actor->facing_vector;
    }
    steering_control(actor_index, object_index, 0.0f, forward, &direction, result);
}

// @address 0x56d170 ?actor_move_calculate_nondirectional_ground_movement@@YA?AW4e_movement_result@@JJPBTreal_vector3d@@PAT2@1F@Z
e_movement_result actor_move_calculate_nondirectional_ground_movement(
    long actor_index, long object_index, const real_vector3d *input, real_vector3d *output_desired, real_vector3d *output_move, short movement_mode)
{
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    const real_vector2d *velocity = NULL;
    vehicle_datum *object = (vehicle_datum *)object_get(object_index);
    character_vehicle_properties *props = character_vehicle_properties_get(actor_index, object->object.definition_index);
    long flags = actor->flags & ~0x200;
    real_vector3d desired;
    bool desired_flag12 = false;
    actor_path *path = &actor->path;
    bool desired_flag14 = false;
    bool desired_flag13 = true;
    e_movement_result result = _movement_result_none;
    actor->flags = flags;
    if (movement_mode == 0)
    {
        float old_speed = actor->ground_steering.throttle;
        actor->ground_steering.previous_steering = actor->ground_steering.steering;
        actor->ground_steering.previous_throttle = old_speed;
    }
    if (object->object.object_type == 1 && object->vehicle.airborne_ticks != 0)
    {
        desired_flag12 = true;
        actor->flags = flags & ~1;
    }
    real_point3d origin;
    object_get_origin(object_index, &origin);
    real_vector3d facing_vec = actor->facing_vector;
    facing_vec.k = 0.0f;
    if (normalize3d(&facing_vec) == 0.0f)
        facing_vec = *global_forward3d;
    if (actor->path_aim != 0)
        actor->flags = actor->flags | 0x400;
    real_vector3d input_dir = *input;
    input_dir.k = 0.0f;
    float input_length = normalize3d(&input_dir);
    if (input_length == 0.0f)
        actor->flags = actor->flags & ~1;
    real_vector3d perp;
    perp.i = -facing_vec.j;
    perp.j = facing_vec.i;
    perp.k = 0.0f;
    real_vector3d movement = *global_zero_vector3d;
    desired = facing_vec;
    real_vector3d free_result;
    real_vector3d test_dir;
    if (props == 0)
    {
        actor->ground_steering.throttle = 0.0f;
    }
    else
    {
        long actor_flags = actor->flags;
        if ((actor_flags & 1) == 0)
        {
            actor->ground_steering.throttle = 0.0f;
            stationary_steering_control(actor_index, object_index, &facing_vec, &desired);
        }
        else
        {
            real_point3d future_point;
            point_from_line3d(&origin, &object->object.linear_velocity, props->velocity_lookahead, &future_point);
            if (actor_moving_has_vehicle_reached_step(actor_index, object_index, &future_point, input_length, false, &desired_flag13,
                &desired_flag14, &test_dir))
            {
                actor->ground_steering.throttle = 0.0f;
                stationary_steering_control(actor_index, object_index, &facing_vec, &desired);
                result = _movement_result_advance;
            }
            else
            {
                union
                {
                    real_vector3d move_output;
                    real_point3d jump_point;
                };
                if (desired_flag14)
                {
                    test_dir.k = 0.0f;
                    if (normalize3d(&test_dir) == 0.0f)
                        desired_flag14 = false;
                }
                float direct_threshold = props->side_slip_distance;
                unsigned char allow_direct = (unsigned char)(((unsigned long)actor_flags >> 1) & 1);
                if (allow_direct != 0 && ((props->flags >> 1) & 1) != 0)
                {
                    velocity = &actor->steering_vector.xy;
                    if (velocity->i * input_dir.i + velocity->j * input_dir.j > 0.5)
                        allow_direct = 0;
                }
                short direct;
                if (allow_direct == 0 && !(input_length < direct_threshold))
                {
                    direct = 1;
                    if (input_length == 0.0f)
                        desired = actor->facing_vector;
                    else
                        desired = input_dir;
                }
                else
                {
                    direct = 0;
                    move_output = input_dir;
                    if (allow_direct != 0)
                    {
                        desired = actor->steering_vector;
                    }
                    else if (actor->destination_orders.test(_destination_flag_1))
                    {
                        desired = actor->destination_orders.m_destination_facing;
                    }
                    else if (!(actor->look_mode >= 3 &&
                        actor->look_direction.get(actor_index, _ai_direction_mode_vector, NULL, &desired, NULL, NULL)))
                    {
                        desired = facing_vec;
                    }
                    desired.k = 0.0f;
                    if (normalize3d(&desired) == 0.0f)
                        desired = actor->facing_vector;
                    if (input_length == 0.0f)
                        move_output = desired;
                    actor_move_calculate_free(actor_index, false, &move_output, &facing_vec, &free_result);
                }
                actor_datum *path_actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
                bool on_path = false;
                if (path_actor->path.valid && path_actor->path_state == 1)
                    on_path = true;
                if (on_path)
                {
                    actor_path_point *entry = &path->points[path->point_index];
                    if (entry->frame_index == 1)
                    {
                        ai_point_get_position(&entry->point, &jump_point);
                        actor_moving_jump(actor_index, &jump_point, true);
                    }
                }
                union
                {
                    special_movement sp;
                    turn_info turn;
                } temp;
                obstacles obs;
                obs.obstacle_count = 0;
                obs.disc_count = 0;
                obs.flag1_count = 0;
                obs.flag8_count = 0;
                obs.unused = 0;
                obstacles *obs_ptr = &obs;
                if (actor->ignore_obstacles)
                {
                    obs_ptr = 0;
                }
                else if (direct == 1)
                {
                    actor_special_movement_new(actor_index, &temp.sp);
                    obstacles_add_nearby_discs(actor_index, 129, 2, true, &temp.sp, &obs, 0, &origin, 0, 20.0f, input, 0, object_index, -1);
                    obstacles_recompute(&obs, props->pathfinding_radius);
                }
                if (direct == 1)
                {
                    actor_find_pathfinding_location(actor_index);
                    if (actor->sector_valid && actor->pathfinding_sector.valid())
                    {
                        float angle = angle_between_vectors2d(&facing_vec.xy, &input_dir.xy);
                        temp.turn.left = actor_moving_vehicle_nondirectional_ground_turns_left(
                            &input_dir.xy, &perp.xy);
                        temp.turn.reverse = false;
                        temp.turn.angle = angle;
                        temp.turn.rating = 1.0f;
                        temp.turn.valid = true;
                        test_turn(actor_index, &temp.turn, (unit_datum *)object, props, &origin, &facing_vec, &perp, props->turning_radius, obs_ptr);
                        if (temp.turn.rating < 0.9f)
                        {
                            direct = 0;
                            actor_move_calculate_free(actor_index, false, &input_dir, &facing_vec, &free_result);
                            actor_unit_control_boost(actor_index, false);
                        }
                    }
                }
                float turn_offset = 0.0f;
                if (desired_flag14 && direct == 1)
                {
                    turn_offset = actor_moving_vehicle_round_turn(actor_index, object_index, desired_flag13 == false, &origin.xy,
                        &path->points[path->point_index].point.point.xy, &test_dir.xy, obs_ptr);
                }
                steering_control(actor_index, object_index, turn_offset, &facing_vec, &desired, &desired);
                if (direct == 0)
                {
                    movement = free_result;
                }
                else
                {
                    movement = *global_forward3d;
                }
                real_vector2d *test_arg = desired_flag14 ? &test_dir.xy : NULL;
                float desired_speed = actor_moving_vehicle_desired_speed(actor_index, object_index, &origin.xy, &input_dir.xy,
                    &desired.xy, &facing_vec.xy, test_arg, input_length, direct == 0);
                float projected = dot_product3d(&object->object.linear_velocity, &desired);
                float max_speed = props->theoretical_maximum_speed;
                float accel;
                if (!(max_speed <= 0.0f))
                {
                    accel = props->error_scale;
                }
                else
                {
                    accel = k_nondirectional_ground_default_acceleration;
                    max_speed = 6.0f;
                }
                if (!(object->unit.boost_time <= 0.0f))
                {
                    max_speed = (unit_compute_boost_fraction(object_index) *
                                        (((const unit_definition *)tag_get('unit', object->object.definition_index))->boost_peak_power -
                                            1.0f) +
                                    1.0f) *
                        max_speed;
                }
                float speed = (desired_speed - projected / max_speed) * accel + desired_speed;
                if (!(speed <= 0.0f))
                {
                    float clamp = props->throttle_delta;
                    if (!(clamp <= 0.0f))
                    {
                        float delta = speed;
                        float prev = actor->ground_steering.previous_throttle;
                        delta -= prev;
                        if (!(delta < clamp))
                            delta = clamp;
                        speed = prev + delta;
                    }
                }
                actor->ground_steering.throttle = MAX(speed, 0.0f);
                float max_reverse = props->throttle_maximum;
                speed = PIN(speed, -max_reverse, max_reverse);
                scale_vector3d(&movement, speed, &movement);
                movement.i = PIN(movement.i, -1.0f, 1.0f);
                movement.j = PIN(movement.j, -1.0f, 1.0f);
                movement.k = PIN(movement.k, -1.0f, 1.0f);
                actor->ground_steering.direct = direct;
            }
        }
    }
    *output_desired = desired;
    *output_move = movement;
    if ((actor->flags & 1) == 0)
        actor_unit_control_boost(actor_index, false);
    if (desired_flag12)
    {
        actor_datum *flag_actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
        flag_actor->control_flags = flag_actor->control_flags | 2;
    }
    return result;
}
