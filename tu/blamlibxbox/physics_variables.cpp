// blamlibxbox/physics_variables.obj: from the records 0xf1138 0xf11c8 0x577f38
// @flags /TP /O2 /Oc
// @address 0xf11c8 ?physics_variable_speed_update@@YAXPAMPBUphysics_variable_speed@@MM@Z
// @address 0x577f38 ?physics_variable_position_update_seek@@YA_NPAMPBUphysics_variable_position@@_NMMM@Z
#include <math.h>
#include "physics/physics_variables.h"

#include "physics/physics_variable_types.h"

static __forceinline float float_sign(float value, float zero, float one, float minus_one)
{
    if (value != zero)
        value = (value > zero) ? one : minus_one;
    return value;
}

void physics_variable_speed_update(float *value, const physics_variable_speed *speed, float input, float delta_time)
{
    float magnitude = fabsf(input);
    float away_step = speed->away_rate * magnitude;
    away_step *= delta_time;
    float toward_step = speed->toward_rate * magnitude;
    toward_step *= delta_time;
    if (input > 0.0f) {
        if (*value <= -toward_step)
            *value += toward_step;
        else if (*value >= 0.0f)
            *value += away_step;
        else
            *value = (*value / toward_step + 1.0f) * away_step;
        *value = (*value > speed->positive_limit) ? speed->positive_limit : *value;
    } else {
        if (*value >= toward_step)
            *value -= toward_step;
        else if (*value <= 0.0f)
            *value -= away_step;
        else
            *value = (*value / toward_step - 1.0f) * away_step;
        *value = (*value > -speed->negative_limit) ? *value : -speed->negative_limit;
    }
}

bool physics_variable_position_update_seek(float *value, const physics_variable_position *position, bool unused, float target, float rate, float delta_time)
{
    bool reached_target = true;
    float zero = 0.0f;
    float delta = target - *value;
    if (delta != zero) {
        float minus_one = -1.0f;
        float one = 1.0f;
        float direction = float_sign(delta, zero, one, minus_one);
        *value = direction * rate * delta_time + *value;
        if (*value < position->lower_bound)
            *value = position->lower_bound;
        else if (*value > position->upper_bound)
            *value = position->upper_bound;
        if (direction != float_sign(target - *value, zero, one, minus_one))
            *value = target;
        else
            reached_target = false;
    }
    return reached_target;
}


// @address 0xf1138 physics_variable_speed_update_seek
int physics_variable_speed_update_seek(float *v, const physics_variable_speed *s, float target, float unused, float dt)
{
    int result = 1;
    float current = *v;
    if (current > target) {
        physics_variable_speed_update(v, s, -1.0f, dt);
        if (*v <= target) *v = target;
        else result = 0;
    } else if (current < target) {
        physics_variable_speed_update(v, s, 1.0f, dt);
        if (*v >= target) *v = target;
        else result = 0;
    }
    return result;
}
