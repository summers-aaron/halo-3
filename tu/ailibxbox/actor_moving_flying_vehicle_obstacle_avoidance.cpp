// ailibxbox/actor_moving_flying_vehicle_obstacle_avoidance.obj: from the records 0xba100
// @flags /TP /O2 /Oc
#include "math/real_math.h"
#include "memory/tls_globals.h"
#include "physics/collision_types.h"
#include "memory/data.h"
#include "objects/object_header.h"
#include "ai/actor_datum.h"
#include <math.h>
#include <string.h>
#include "ai/character_definitions.h"
#include "ai/actor_moving_flying_vehicle_obstacle_avoidance.h"
#include "physics/collision_test_vector.h"

const float k_real_max = 3.402823466e+38f;






static __forceinline void *obstacle_avoidance_datum_get(s_data_array *array, long index, long size)
{
    return array->data + (index & 0xffff) * size;
}

static __forceinline float obstacle_avoidance_cosine(float angle)
{
    return (float)cos((double)angle);
}

static __forceinline float obstacle_avoidance_sine(float angle)
{
    return (float)sin((double)angle);
}

static __forceinline real_vector3d *obstacle_avoidance_cross_product3d(const real_vector3d *a, const real_vector3d *b, real_vector3d *result)
{
    result->i = a->j * b->k - a->k * b->j;
    result->j = a->k * b->i - a->i * b->k;
    result->k = a->i * b->j - a->j * b->i;
    return result;
}

// @address 0xba100 actor_moving_flying_vehicle_compute_avoidance
extern "C" bool actor_moving_flying_vehicle_compute_avoidance(long actor_index, long object_index, real_vector3d *desired_vector,
    // match: 0xba100: pointer __restrict qualifier prevents alias reload
    const real_vector3d *current_vector, real_vector3d *avoidance_vector, float *__restrict throttle, float *__restrict urgency,
    bool use_object_axes)
{
    collision_result collision;
    bool success = true;
    object_datum *object = (object_datum *)object_get_and_verify_type(object_index, _object_mask_unit);
    long definition_index = object->definition_index;
    character_vehicle_properties *properties = character_vehicle_properties_get(actor_index, definition_index);
    float closest_distance = k_real_max;
    real_vector3d new_vector = *desired_vector;
    bool hits[4];
    hits[0] = false;
    hits[1] = false;
    hits[2] = false;
    hits[3] = false;
    *urgency = 0.0f;

    actor_datum *actor = (actor_datum *)obstacle_avoidance_datum_get(tls_actor_data(), actor_index, sizeof(actor_datum));
    bool avoidance_disabled = actor->avoidance_disabled;
    if (!avoidance_disabled && properties->obstacle_avoidance_distance > 0.0f)
    {
        float maximum_distance = properties->obstacle_avoidance_distance;
        float sense_distance = maximum_distance;
        {
            real_vector3d current_direction;
            memcpy(&current_direction, current_vector, sizeof(current_direction));
            normalize3d(&current_direction);
            real_vector3d scaled_vector;
            scale_vector3d(desired_vector, sense_distance, &scaled_vector);
            if (sense_distance > dot_product3d(&current_direction, &scaled_vector))
            {
                sense_distance = dot_product3d(&current_direction, &scaled_vector);
            }
        }

        set_real_vector3d(avoidance_vector, 0.0f, 0.0f, 0.0f);

        const real_vector3d *forward;
        const real_vector3d *up;
        const real_vector3d *left_axis;
        real_vector3d left;
        real_vector3d forward_direction;
        real_vector3d up_direction;
        if (use_object_axes)
        {
            forward = &object->forward;
            up = &object->up;
            obstacle_avoidance_cross_product3d(up, forward, &left);
            left_axis = &left;
        }
        else
        {
            memcpy(&forward_direction, desired_vector, sizeof(forward_direction));
            if (normalize3d(&forward_direction) == 0.0f)
            {
                memcpy(&forward_direction, global_forward3d, sizeof(forward_direction));
            }
            obstacle_avoidance_cross_product3d(global_up3d, &forward_direction, &left);
            if (normalize3d(&left) == 0.0f)
            {
                memcpy(&left, global_left3d, sizeof(left));
            }
            obstacle_avoidance_cross_product3d(&forward_direction, &left, &up_direction);
            if (normalize3d(&up_direction) == 0.0f)
            {
                memcpy(&up_direction, global_up3d, sizeof(up_direction));
            }
            forward = &forward_direction;
            up = &up_direction;
            left_axis = &left;
        }

        *throttle = 1.0f;
        real_point3d points[4];
        {
            real_point3d position = object->bounding_center;
            point_from_line3d(&position, up, properties->pathfinding_radius, &points[0]);
            point_from_line3d(&position, left_axis, properties->pathfinding_radius, &points[1]);
            point_from_line3d(&position, up, -properties->pathfinding_radius, &points[2]);
            point_from_line3d(&position, left_axis, -properties->pathfinding_radius, &points[3]);
        }

        float ray_lengths[4];
        ray_lengths[0] = sense_distance + properties->pathfinding_radius;
        ray_lengths[1] = sense_distance + properties->pathfinding_radius;
        ray_lengths[2] = sense_distance + properties->pathfinding_radius;
        ray_lengths[3] = sense_distance + properties->pathfinding_radius;

        float distances[4];
        for (short ray_index = 0; ray_index < 4; ray_index++)
        {
            real_vector3d ray;
            scale_vector3d(forward, ray_lengths[ray_index], &ray);
            const s_collision_test_flags flags = k_flying_vehicle_collision_flags;
            const real_point3d *ray_point = &points[ray_index];
            bool hit = collision_test_vector(flags, false, ray_point, &ray, object_index, NONE, NONE, &collision);
            if (hit)
            {
                float ray_length = ray_lengths[ray_index];
                float ray_distance = collision.t * ray_length;
                float distance = ray_distance - properties->pathfinding_radius;
                float scaled_distance = distance / sense_distance;
                if (scaled_distance < 0.1f)
                {
                    distance = 0.1f * sense_distance;
                    success = false;
                }
                distances[ray_index] = distance;
                hits[ray_index] = true;
                if (distances[ray_index] < closest_distance)
                {
                    closest_distance = distances[ray_index];
                }
            }
        }

        if (success)
        {
            float up_angle = 0.0f;
            float left_angle = 0.0f;
            float maximum_angle;
            if (properties->maximum_steering_angle > 0.0f)
            {
                maximum_angle = k_degrees_to_radians * properties->maximum_steering_angle;
            }
            else
            {
                maximum_angle = 1.5707964f;
            }

            if (closest_distance < k_real_max)
            {
                *throttle = 0.3f + 0.7f * closest_distance / sense_distance;
                *urgency = 1.0f - PIN(closest_distance / sense_distance, 0.0f, 1.0f);
            }

            if (hits[0])
            {
                float scale = MAX(1.0f - distances[0] / sense_distance, 0.0f);
                float angle = maximum_angle * sqrt(scale);
                angle = PIN(angle, 0.0f, maximum_angle);
                left_angle += angle;
                avoidance_vector->k -= sqrt(scale) * 1.0f;
            }
            if (hits[1])
            {
                float scale = MAX(1.0f - distances[1] / sense_distance, 0.0f);
                float angle = maximum_angle * sqrt(scale);
                angle = PIN(angle, 0.0f, maximum_angle);
                up_angle -= angle;
                avoidance_vector->j -= sqrt(scale) * 1.0f;
            }
            if (hits[2])
            {
                float scale = MAX(1.0f - distances[2] / sense_distance, 0.0f);
                float angle = maximum_angle * sqrt(scale);
                angle = PIN(angle, 0.0f, maximum_angle);
                left_angle -= angle;
                avoidance_vector->k += sqrt(scale) * 1.0f;
            }
            if (hits[3])
            {
                float scale = MAX(1.0f - distances[3] / sense_distance, 0.0f);
                float angle = maximum_angle * sqrt(scale);
                angle = PIN(angle, 0.0f, maximum_angle);
                up_angle += angle;
                avoidance_vector->j += sqrt(scale) * 1.0f;
            }

            float left_cosine = obstacle_avoidance_cosine(left_angle);
            float left_sine = obstacle_avoidance_sine(left_angle);
            rotate_vector_about_axis(&new_vector, left_axis, left_sine, left_cosine);
            float up_cosine = obstacle_avoidance_cosine(up_angle);
            float up_sine = obstacle_avoidance_sine(up_angle);
            rotate_vector_about_axis(&new_vector, up, up_sine, up_cosine);
        }

        if (use_object_axes)
        {
            *desired_vector = new_vector;
        }
        else
        {
            real_vector3d world_vector;
            real_vector3d scaled_axis;
            real_vector3d scaled_forward;
            real_vector3d object_left;
            scale_vector3d(forward, avoidance_vector->i, &scaled_forward);
            scale_vector3d(left_axis, avoidance_vector->j, &scaled_axis);
            add_vectors3d(&scaled_forward, &scaled_axis, &world_vector);
            scale_vector3d(up, avoidance_vector->k, &scaled_axis);
            add_vectors3d(&world_vector, &scaled_axis, &world_vector);
            obstacle_avoidance_cross_product3d(&object->up, &object->forward, &object_left);
            set_real_vector3d(avoidance_vector, dot_product3d(&world_vector, &object->forward), dot_product3d(&world_vector, &object_left),
                dot_product3d(&world_vector, &object->up));
        }
    }

    return success;
}
