// blamlibxbox/character_physics_utilities.obj: from the records 0xe0ed0
// @flags /TP /O2 /Oc
#include "cseries/cseries.h"
#include "memory/tls_globals.h"
#include "math/real_math.h"
#include "math/matrix_math.h"
#include "objects/object_header.h"
#include <ppcintrinsics.h>
#include "physics/character_physics_utilities.h"
// @address 0x7c5d0c k_flying_angular_velocity_threshold
float k_flying_angular_velocity_threshold = 0.034906585f;
#include "units/units.h"

#include "physics/character_physics_definition_types.h"


// @rdata 0x8202490c
static const float character_physics_utilities_final_axis_epsilon = 0.0001f;



static __forceinline float character_physics_game_seconds_to_ticks_real(float seconds)
{
    return seconds * (float)tls_game_time_globals()->ticks_per_second;
}

static __forceinline float character_physics_game_tick_length()
{
    return tls_game_time_globals()->tick_length;
}

// @address 0xe0ed0 ?character_physics_flying_update_turning@@YA_NJPBUs_character_physics_flying_definition@@PBTreal_vector3d@@1MPAM@Z
bool character_physics_flying_update_turning(long object_index, const s_character_physics_flying_definition *definition,
    const real_vector3d *vector, const real_vector3d *vector2, float p4, float *p5)
{
    object_datum *object = (object_datum *)object_get_and_verify_type(object_index, 0xffffffff);
    float axis_epsilon = 0.0001f;
    real_rectangle2d limits;
    real_vector3d axis;
    bool turning = false;
    if (magnitude_squared3d(&object->linear_velocity) < 0.25f)
    {
        if (magnitude_squared3d(&object->angular_velocity) < k_flying_angular_velocity_threshold * k_flying_angular_velocity_threshold)
        {
            if (magnitude_squared3d(vector2) < axis_epsilon)
                turning = dot_product3d(&object->forward, vector) > p4;
        }
    }
    if (turning)
    {
        axis = object->forward;
    }
    else
    {
        axis = *vector;
        float push = definition->pitch_ratio * vector2->k;
        if (push != 0.0f)
        {
            axis.k += push;
            if (normalize3d(&axis) == 0.0f)
                axis = *vector;
        }
    }
    real_vector3d cross;
    cross_product3d(&object->forward, &object->up, &cross);
    float dot = dot_product3d(&cross, vector);
    float turn = dot * 3.3333333f;
    turn = turn * vector2->i - vector2->j;
    if (turn > 1.5f)
        turn = 1.5f;
    float target = definition->bank_angle * turn;
    float current = *p5;
    float slowdown;
    if (target * current <= 0.0f)
    {
        slowdown = 1.0f;
    }
    else
    {
        float abs_target = target < 0.0f ? -target : target;
        if (abs_target > 0.001f)
        {
            float ratio = current / target;
            if (ratio > 1.0f)
                ratio = 1.0f;
            slowdown = 1.0f - ratio;
        }
        else
        {
            slowdown = 0.0f;
        }
    }
    float blend = slowdown * definition->bank_apply_time + (1.0f - slowdown) * definition->bank_decay_time;
    float ticks = character_physics_game_seconds_to_ticks_real(blend);
    if (ticks > 0.0f)
        *p5 = (target - current) / ticks + current;
    else
        *p5 = target;
    limits.x0 = -3.1415927f;
    limits.x1 = 3.1415927f;
    limits.y0 = -1.5707964f;
    limits.y1 = 1.5707964f;
    if (definition->angular_acceleration_maximum == 0.0f)
    {
        object->forward = axis;
    }
    else
    {
        unit_euler_aiming_update(0, 0.0f, &object->forward, &axis, character_physics_game_tick_length(), &object->angular_velocity, &limits,
            definition->angular_velocity_maximum, definition->angular_acceleration_maximum, 0);
        axis_epsilon = character_physics_utilities_final_axis_epsilon;
    }
    float separation = dot_product3d(&object->forward, &object->up);
    if (fabs(separation) >= axis_epsilon)
    {
        real_vector3d scaled;
        scale_vector3d(&object->forward, separation, &scaled);
        subtract_vectors3d(&object->up, &scaled, &object->up);
        normalize3d(&object->up);
    }
    return true;
}
