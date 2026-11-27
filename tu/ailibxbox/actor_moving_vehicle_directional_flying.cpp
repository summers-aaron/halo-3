// ailibxbox/actor_moving_vehicle_directional_flying.obj: from the records 0x56ca38
// @flags /TP /O2 /Oc
#include "math/real_math.h"
#include "memory/data.h"
#include "memory/tls_globals.h"
#include "objects/object_header.h"
#include "ai/actor_datum.h"
#include <math.h>

#include "ai/actor_moving_types.h"
#include "ai/actor_moving_vehicle_nondirectional_flying.h"
#include "ai/character_definitions.h"
#include "ai/pathfinding_utilities.h"
#include "ai/actor_moving_flying_vehicle_obstacle_avoidance.h"
#include "ai/actor_moving_vehicle_directional_flying.h"

extern "C" {
// actor_moving_vehicle_test_destination
// actor_moving_nondirectional_flying_vector_avoidance
}

// @address 0x56ca38 ?actor_move_calculate_directional_flying_movement@@YA?AW4e_movement_result@@JJPATreal_vector3d@@00F@Z
e_movement_result actor_move_calculate_directional_flying_movement(long actor_index, long object_index, real_vector3d *movement,
    real_vector3d *desired_facing, real_vector3d *desired_throttle, short iteration)
{
    actor_datum *actor = &((actor_datum *)tls_actor_data()->data)[(unsigned short)actor_index];
    actor_datum *vehicle = (actor_datum *)actor;
    object_datum *object = (object_datum *)object_get_and_verify_type(object_index, 8195);
    character_vehicle_properties *properties = character_vehicle_properties_get(actor_index, object->definition_index);
    short turn_index = NONE;
    bool last_step = false;
    e_movement_result result = _movement_result_none;

    actor->flags &= ~FLAG(9);
    if (iteration == 0)
    {
        vehicle->previous_yaw = vehicle->yaw;
        vehicle->previous_pitch = vehicle->pitch;
        vehicle->previous_throttle = vehicle->vehicle_throttle;
    }
    if (actor->root_vehicle_index == NONE)
        vehicle->output_animation_state = actor_get_movement_type(actor_index);

    real_vector3d desired = actor->facing_vector;
    real_vector3d throttle = *global_zero_vector3d;
    real_vector3d avoidance = *global_zero_vector3d;
    real_point3d origin;
    object_get_origin(object_index, &origin);
    real_vector3d direction = *movement;
    float distance = normalize3d(&direction);

    if (properties)
    {
        if (!TEST_BIT(actor->flags, 0))
            scale_vector3d(&actor->facing_vector, 2.0f, movement);

        bool next_step_valid;
        real_vector3d next_step_direction;
        if (actor_moving_has_vehicle_reached_step(actor_index, object_index, &origin, distance, true, &last_step, &next_step_valid, &next_step_direction))
        {
            result = _movement_result_advance;
            scale_vector3d(&actor->facing_vector, 2.0f, movement);
        }
        actor_find_pathfinding_location(actor_index);

        real_point3d target;
        point_from_line3d(&origin, movement, 1.0f, &target);
        real_vector2d movement2d = movement->xy;
        float movement_length = normalize2d(&movement2d);
        real_vector2d facing2d = actor->facing_vector.xy;
        normalize2d(&facing2d);
        real_vector2d right2d = { -facing2d.j, facing2d.i };
        float half_boost_distance = properties->deceleration_distance / 2.0;
        float radius = properties->pathfinding_radius > 0.0f ? properties->pathfinding_radius : object->bounding_radius;

        float throttle_scale = 1.0f;
        float avoidance_urgency;
        desired = *movement;
        actor_moving_flying_vehicle_compute_avoidance(actor_index, object_index, &desired, movement, &avoidance, &throttle_scale, &avoidance_urgency, true);
        if (normalize3d(&desired) == 0.0f)
            desired = actor->facing_vector;

        float urgency = 0.0f;
        float minimum_urgency = 0.05f;
        if (vehicle->urgency_override != 0.0f)
        {
            urgency = vehicle->urgency_override;
        }
        else
        {
            float movement_length_squared = movement_length * movement_length;
            float boost_scale = 0.5f *
                (((properties->deceleration_distance / 2.0f) * (properties->deceleration_distance / 2.0f)) /
                    MAX(movement_length_squared, 0.001f));
            float facing_scale = 0.5f * (1.0f - MAX(dot_product2d(&movement2d, &facing2d), 0.0f));
            urgency = 4.0f * boost_scale * facing_scale;
        }
        minimum_urgency = MAX(properties->minimum_urgency, minimum_urgency);
        urgency = PIN(urgency, minimum_urgency, 1.0f);
        urgency = MAX(urgency, avoidance_urgency);

        real_vector3d steering_direction = desired;
        steering_control(actor_index, object_index, &desired, urgency, &actor->facing_vector, &desired);
        vehicle->urgency = urgency;

        float speed = properties->throttle_maximum;
        real_vector2d steering_direction2d = steering_direction.xy;
        normalize2d(&steering_direction2d);
        speed *= actor_moving_vehicle_desired_speed(
            actor_index, object_index, &origin.xy, &movement2d, &steering_direction2d, &facing2d, NULL, distance, false);
        set_real_vector3d(&throttle, 1.0f, 0.0f, 0.0f);
        speed = speed > throttle_scale ? throttle_scale : speed;
        scale_vector3d(&throttle, speed, &throttle);

        if (properties->theoretical_maximum_speed > 0.0f)
        {
            float projected_speed = dot_product3d(&object->linear_velocity, &actor->facing_vector);
            float speed_error = throttle.i - projected_speed / properties->theoretical_maximum_speed;
            long sign = speed_error < 0.0f ? -1 : 1;
            throttle.i += square_root(fabsf(speed_error)) * properties->error_scale * (float)sign;
        }

        add_vectors3d(&throttle, &avoidance, &throttle);
        scale_vector3d(&throttle, 1.0f - properties->throttle_blend, &throttle);
        real_vector3d previous_throttle;
        scale_vector3d(&vehicle->previous_throttle, properties->throttle_blend, &previous_throttle);
        add_vectors3d(&throttle, &previous_throttle, &throttle);
        throttle.i = PIN(throttle.i, -1.0f, 1.0f);
        throttle.j = PIN(throttle.j, -1.0f, 1.0f);
        throttle.k = PIN(throttle.k, -1.0f, 1.0f);
        vehicle->movement_mode = turn_index;
    }

    vehicle->vehicle_throttle = throttle;
    *desired_facing = desired;
    *desired_throttle = throttle;
    return result;
}
