// blamlibxbox/character_physics_mode_melee.obj: from the records 0x2f4910 0x2f4ac8 0x2f4c48 0x2f4d98 0x2f5008 0x2f50e0
// @flags /TP /O2 /Oc
#include "cseries/cseries_types.h"
// @address 0x2f4ac8 character_physics_mode_melee_clamp_velocity
#include <math.h>
#include "memory/tls_slot_aliases.h"
#include "render/render_objects_static_lighting.h"
#include "physics/character_physics_mode_melee.h"
#include "physics/character_physics_mode_melee_externs.h"
// @rdata 0x827c6054
static const float k_melee_rate_scale = 0.375f;
#include "physics/character_physics_input.h"
#include "physics/character_physics_output.h"
#include "math/real_math.h"
#include "math/real_math_globals.h"



struct s_melee_adjustment
{
    real_vector3d offset;                                   // +0
    long reserved0c;
    real_vector3d previous;                                 // +16
};

struct s_melee_magnitude { float value; };



struct s_melee_velocity_component { float value; };

struct s_melee_update_distance_component
{
    explicit s_melee_update_distance_component(float distance) : value(distance) {}
    float value;
};


static __forceinline s_melee_magnitude character_physics_mode_melee_velocity_magnitude(const real_vector3d *v)
{
    s_melee_magnitude magnitude;
    magnitude.value = sqrtf(v->j * v->j + v->i * v->i + v->k * v->k);
    return magnitude;
}

static __forceinline void character_physics_mode_melee_velocity_offset(const real_vector3d &a, const real_vector3d &b, real_vector3d &o)
{
    o.i = a.i - b.i;
    o.j = a.j - b.j;
    o.k = a.k - b.k;
}



// @address 0x79d440 g_melee_maximum_angular_speed
float g_melee_maximum_angular_speed = 18.849556f;

const real character_physics_mode_melee_vertical_tolerance = 0.3f;

extern "C" bool character_physics_mode_melee_clamp_velocity(const s_melee_clamp_data *data, real_vector3d *velocity)
{
    bool result = false;
    float speed = magnitude3d(velocity);
    float max_speed = character_physics_mode_melee_velocity_magnitude(&data->initial_velocity).value + 0.8f;
    if (speed > max_speed)
    {
        float scale = max_speed / speed;
        result = true;
        velocity->i *= scale;
        velocity->j *= scale;
        velocity->k *= scale;
    }
    s_melee_adjustment work;
    work.previous = *velocity;
    character_physics_mode_melee_velocity_offset(data->initial_velocity, *velocity, work.offset);
    float distance = normalize3d(&work.offset);
    if (distance > 1.2f)
    {
        float excess = distance - 1.2f;
        float back = -dot_product3d(&work.previous, &work.offset);
        if (back <= excess)
            excess = back;
        if (excess > 0.0f)
        {
            point_from_line3d(&work.previous, &work.offset, excess, velocity);
            result = true;
        }
    }
    if (velocity->k - data->initial_velocity.k > character_physics_mode_melee_vertical_tolerance)
    {
        velocity->k = data->initial_velocity.k + character_physics_mode_melee_vertical_tolerance;
        return true;
    }
    return result;
}





// @address 0x2f4c48 ?update@c_character_physics_mode_melee_datum@@QAAXPAUs_character_physics_update_output_datum@@PBUs_character_physics_update_input_datum@@@Z
void c_character_physics_mode_melee_datum::update(
    s_character_physics_update_output_datum *physics_output, const s_character_physics_update_input_datum *physics_input)
{
    real_vector3d velocity = physics_input->melee.velocity;
    real_vector3d target_velocity = physics_input->target_velocity;
    if (physics_input->flags & 128)
    {
        subtract_vectors3d(&target_velocity, &physics_input->reference_velocity, &target_velocity);
        subtract_vectors3d(&velocity, &physics_input->reference_velocity, &velocity);
    }
    update_internal(physics_output, physics_input->melee.time_step, physics_input->melee.duration, physics_input->melee.enabled, &velocity,
        &physics_input->melee.position, &physics_input->aiming, &target_velocity, &physics_input->target);
    if (physics_input->flags & 128)
        add_vectors3d(&physics_output->velocity, &physics_input->reference_velocity, &physics_output->velocity);
}

// @address 0x2f4910 character_physics_mode_melee_compute_rate
extern "C" float character_physics_mode_melee_compute_rate(float value, long count, unsigned char fast, long, long)
{
    float threshold = fast ? 0.15f : 0.1f;
    float step = 0.0f;
    float scaled = value * k_melee_rate_scale;
    float maximum = TLS_GAME_TIME_GLOBALS->tick_length * 12.0f;
    if (count > 1 && value > threshold)
        step = (value - threshold) * 2.0f / (float)(count - 1);
    step = MAX(scaled, step);
    if (MAX(step, 0.001f) >= maximum)
        return maximum;
    if (step <= 0.001f)
        return 0.001f;
    return step;
}


static __forceinline float character_physics_mode_melee_maximum_real(float a, float b)
{
    return a > b ? a : b;
}

static __forceinline long character_physics_mode_melee_minimum_long(long a, long b)
{
    return a < b ? a : b;
}

// @address 0x2f5008 character_physics_mode_melee_predict_ticks
extern "C" float character_physics_mode_melee_predict_ticks(s_rate_input *input, float value)
{
    float limit = input->maximum;
    if (limit != 0.0f && value > limit)
        value = limit;
    unsigned long flags = input->flags;
    float rate = character_physics_mode_melee_compute_rate(value, input->count, flags & 1, (flags >> 1) & 1, 0);
    float offset = rate * 4.0f;
    offset *= 0.5f;
    float three_times = rate * 3.0f;
    float twice = rate * 2.0f;
    float adjustment = character_physics_mode_melee_maximum_real((twice + three_times) * 0.5f, 0.0f);
    float result = 6.0f;
    if (rate >= 0.001f)
    {
        float estimate = (value - (adjustment + offset)) / rate + result;
        result = character_physics_mode_melee_maximum_real(estimate, 0.0001f);
    }
    return result;
}

// @rdata 0x8202251c
static const float k_melee_prediction_zero = 0.0f;
// @rdata 0x82022dbc
static const float k_melee_prediction_tenth = 0.1f;
// @rdata 0x82022dc0
static const float k_melee_prediction_half = 0.5f;
// @rdata 0x820242d4
static const float k_melee_prediction_maximum_rate = 3.0f;
// @rdata 0x8202490c
static const float k_melee_prediction_epsilon = 0.0001f;


static __forceinline float character_physics_mode_melee_magnitude3d_yzx(const real_vector3d *v)
{
    return sqrtf(v->j * v->j + v->k * v->k + v->i * v->i);
}

static __forceinline s_melee_velocity_component character_physics_mode_melee_first_velocity_component(const real_vector3d *velocity)
{
    s_melee_velocity_component component;
    component.value = velocity->i;
    return component;
}

// @address 0x2f4d98 character_physics_mode_melee_predict_impact
extern "C" void character_physics_mode_melee_predict_impact(
    s_rate_input *input,
    const real_point3d *target,
    const real_vector3d *forward,
    float time_step,
    const real_point3d *position,
    const real_vector3d *velocity,
    float duration,
    real_point3d * out_position,
    // match: 0x2f4d98: pointer __restrict qualifier prevents alias reload
    real_vector3d *__restrict out_direction,
    // match: 0x2f4d98: pointer __restrict qualifier prevents alias reload
    float *__restrict out_speed,
    // match: 0x2f4d98: pointer __restrict qualifier prevents alias reload
    float *__restrict out_rate,
    bool * out_clamped)
{
    bool clamped = false;
    const float zero = k_melee_prediction_zero;
    float scale = zero;
    real_vector3d offset;
    offset.i = position->x - target->x;
    offset.j = position->y - target->y;
    offset.k = position->z - target->z;

    float distance = character_physics_mode_melee_magnitude3d_yzx(&offset);
    float length;
    if (distance > k_melee_prediction_epsilon)
    {
        const float tenth = k_melee_prediction_tenth;
        float margin;
        if (input->flags & 1)
            margin = 0.15f;
        else
            margin = tenth;
        float lead = zero;
        bool lead_flag = ((input->flags >> 1) & 1) != 0;
        if (lead_flag && !(input->flags & 1))
            lead = tenth;
        lead = duration - lead;
        if (lead < zero)
            lead = zero;
        length = distance - (lead + margin);
        if (length <= k_melee_prediction_epsilon)
            length = k_melee_prediction_epsilon;
        scale = length / distance;
        offset.i *= scale;
        offset.j *= scale;
        offset.k *= scale;
    }
    else
    {
        length = k_melee_prediction_epsilon;
    }

    float rate = character_physics_mode_melee_predict_ticks(input, length);
    float capped_rate = rate > k_melee_prediction_maximum_rate ? k_melee_prediction_maximum_rate : rate;

    real_vector3d predicted;
    predicted.i = character_physics_mode_melee_first_velocity_component(velocity).value * (TLS_GAME_TIME_GLOBALS->tick_length * capped_rate);
    float inverse = zero;
    predicted.j = velocity->j * (TLS_GAME_TIME_GLOBALS->tick_length * capped_rate);
    predicted.k = velocity->k * (TLS_GAME_TIME_GLOBALS->tick_length * capped_rate);
    float predicted_squared = predicted.j * predicted.j + predicted.k * predicted.k + predicted.i * predicted.i;
    if (predicted_squared > 0.25f)
    {
        inverse = k_melee_prediction_half / sqrtf(predicted_squared);
        predicted.i *= inverse;
        predicted.j *= inverse;
        predicted.k *= inverse;
    }

    real_vector3d direction;
    direction.i = predicted.i + offset.i;
    direction.j = predicted.j + offset.j;
    float clamp_scale = zero;
    direction.k = predicted.k + offset.k;

    float speed = character_physics_mode_melee_magnitude3d_yzx(&direction);
    if (speed <= k_melee_prediction_epsilon)
        speed = k_melee_prediction_epsilon;

    float speed_rate = character_physics_mode_melee_predict_ticks(input, speed);
    if (input->maximum != zero && speed > input->maximum)
    {
        clamp_scale = input->maximum / speed;
        speed = input->maximum;
        clamped = true;
        direction.i *= clamp_scale;
        direction.j *= clamp_scale;
        direction.k *= clamp_scale;
    }

    real_point3d impact;
    impact.x = target->x + direction.i;
    impact.y = target->y + direction.j;
    impact.z = target->z + direction.k;

    float normalized = normalize3d(&direction);
    real_vector3d result_direction;
    if (normalized == zero)
        result_direction = *forward;
    else
    {
        result_direction.words[2] = direction.words[2];
        result_direction.words[1] = direction.words[1];
        result_direction.words[0] = direction.words[0];
    }
    *out_position = impact;
    *out_speed = speed;
    *out_direction = result_direction;
    *out_rate = speed_rate;
    *out_clamped = clamped;
}



// @rdata 0x82024264
static const float k_melee_update_maximum_distance = 5.0f;
// @rdata 0x82022bc4
static const float k_melee_update_maximum_speed = 3.5f;
// @rdata 0x820242d4
static const float k_melee_update_acceleration_ticks = 3.0f;

static __forceinline float melee_update_dot(const real_vector3d *a, const real_vector3d *b)
{
    return a->i * b->i + (a->j * b->j + a->k * b->k);
}

static __forceinline real_vector3d *melee_update_scale(const real_vector3d *vector, float scale, real_vector3d *result)
{
    result->i = vector->i * scale;
    result->j = vector->j * scale;
    result->k = vector->k * scale;
    return result;
}

static __forceinline long melee_update_round(float value)
{
    return (long)(value + 0.5f);
}

static __forceinline s_melee_update_distance_component melee_update_distance_component(float speed, float ticks)
{
    return s_melee_update_distance_component(speed * ticks);
}

static __forceinline float melee_update_stopping_distance(float rate, float zero)
{
    return character_physics_mode_melee_maximum_real(
        melee_update_distance_component(rate, 2.0f).value * zero + melee_update_distance_component(rate, 3.0f).value * 0.5f, zero);
}


// @address 0x2f50e0 ?update_internal@c_character_physics_mode_melee_datum@@AAAXPAUs_character_physics_update_output_datum@@MM_NPBTreal_vector3d@@PBTreal_point3d@@223@Z
void c_character_physics_mode_melee_datum::update_internal(s_character_physics_update_output_datum *physics_output, float time_step,
    float duration, bool enabled, const real_vector3d *velocity, const real_point3d *position, const real_vector3d *forward,
    const real_vector3d *target_velocity, const real_point3d *target)
{
    physics_output->velocity = *target_velocity;
    const float zero = 0.0f;
    if (enabled)
    {
        real_point3d new_target;
        real_vector3d new_direction;
        float distance;
        float predicted_ticks;
        bool clamped;
        character_physics_mode_melee_predict_impact((s_rate_input *)this, target, forward, time_step, position, velocity, duration,
            &new_target, &new_direction, &distance, &predicted_ticks, &clamped);
        if (!m_flags.test(_initialized_bit))
        {
            m_flags.set(_clamped_bit, clamped);
            m_initial_speed = melee_update_dot(velocity, &new_direction);
            m_target = new_target;
            m_direction = new_direction;
            m_velocity = *velocity;
            m_distance = distance;
            m_ticks_remaining = 0;
            m_elapsed_ticks = 0;
            bool fast = m_flags.test(_fast_bit);
            long duration_ticks = character_physics_mode_melee_minimum_long(30, melee_update_round((float)(fast ? 6 : 2) + predicted_ticks));
            m_duration_ticks = duration_ticks;
            m_rate = character_physics_mode_melee_compute_rate(distance, m_count, fast, m_flags.test(_lead_bit), 1);
            physics_output->velocity = *global_zero_vector3d;
            m_flags.set(_initialized_bit, true);
        }
        else
        {
            float cosine = melee_update_dot(&m_direction, &new_direction);
            cosine = PIN(cosine, -1.0f, 1.0f);
            float angular_speed = arccosine(cosine) / TLS_GAME_TIME_GLOBALS->tick_length;
            if (clamped)
                m_flags.set(_clamped_bit, true);
            if (distance < k_melee_update_maximum_distance && angular_speed < g_melee_maximum_angular_speed)
            {
                float speed = melee_update_dot(&physics_output->velocity, &m_direction);
                speed = MAX(speed, zero);
                float reverse_speed = -speed;
                point_from_line3d(&physics_output->velocity, &m_direction, reverse_speed,
                    &physics_output->velocity);
                point_from_line3d(
                    &physics_output->velocity, &new_direction, speed, &physics_output->velocity);
                m_target = new_target;
                m_direction = new_direction;
                m_velocity = *velocity;
                m_distance = distance;
                if (!m_flags.test(_decelerating_bit))
                {
                    bool fast = m_flags.test(_fast_bit);
                    long ticks = fast ? 6 : 2;
                    long duration_ticks = character_physics_mode_melee_minimum_long(30, melee_update_round((float)ticks + distance));
                    float rate = character_physics_mode_melee_compute_rate(distance, m_count, fast, m_flags.test(_lead_bit), 1);
                    m_rate = MAX(m_rate, rate);
                    m_duration_ticks = MAX(m_duration_ticks, duration_ticks);
                }
            }
            else
                m_flags.set(_initialized_bit, false);
        }
    }
    c_flags_no_init<e_flags, unsigned long, 8> phase_flags = m_flags;
    if (phase_flags.test(_initialized_bit))
    {
        float rate = m_rate;
        float acceleration = rate * (1.0f / 3.0f);
        float stopping_distance = melee_update_stopping_distance(rate, zero);
        float relative_distance = zero;
        float initial_speed = zero;
        float tick_length = TLS_GAME_TIME_GLOBALS->tick_length;
        real_vector3d displacement;
        melee_update_scale(&physics_output->velocity, tick_length, &displacement);
        real_vector3d previous_displacement;
        melee_update_scale(&m_velocity, tick_length, &previous_displacement);
        float speed = melee_update_dot(&displacement, &m_direction);
        float relative_speed = -melee_update_dot(&previous_displacement, &m_direction);
        float speed_twice = speed * 2.0f;
        float current_stopping_distance = character_physics_mode_melee_maximum_real((speed_twice + speed * 3.0f) * 0.5f, zero);
        float next_speed = speed + acceleration;
        float next_stopping_distance = character_physics_mode_melee_maximum_real((next_speed * 3.0f + (next_speed + speed)) * 0.5f, zero);
        if (relative_speed > zero || phase_flags.test(_lead_bit) || phase_flags.test(_fast_bit))
            relative_distance = relative_speed * 1.5f;
        if (rate < 0.001f)
        {
            m_ticks_remaining = 0;
            m_flags.set_unsafe(phase_flags.get_unsafe() | 192);
        }
        else
        {
            bool decelerating = phase_flags.test(_decelerating_bit);
            if (!decelerating && m_distance > current_stopping_distance + relative_distance && (float)(m_duration_ticks - m_elapsed_ticks) > 3.0f)
            {
                float elapsed_ticks = (float)m_elapsed_ticks;
                if (elapsed_ticks < 3.0f && m_distance > next_stopping_distance + relative_distance)
                {
                    float increase = rate - speed <= acceleration ? rate - speed : acceleration;
                    point_from_line3d(&displacement, &m_direction, increase, &displacement);
                    float distance_to_cruise = (3.0f - elapsed_ticks) * ((increase + speed + rate) * 0.5f) + stopping_distance;
                    long cruise_ticks = melee_update_round((float)floor((m_distance - distance_to_cruise) / rate));
                    float acceleration_ticks = k_melee_update_acceleration_ticks - (float)m_elapsed_ticks;
                    float cruise = (float)cruise_ticks;
                    m_ticks_remaining = (long)(cruise + acceleration_ticks + 2.0f);
                }
                else
                {
                    long cruise_ticks = melee_update_round((float)floor((m_distance - stopping_distance) / rate));
                    m_ticks_remaining = (long)((float)cruise_ticks + 2.0f);
                }
            }
            else
            {
                real_vector3d unit_displacement = displacement;
                float length = normalize3d(&unit_displacement);
                if (!decelerating)
                {
                    initial_speed = PIN(m_initial_speed * 1.5f, zero, k_melee_update_maximum_speed);
                    float minimum_speed = initial_speed * tick_length;
                    float relative_minimum = relative_speed * -1.5f;
                    if (relative_minimum < zero)
                        relative_minimum = zero;
                    m_flags.set_unsafe(phase_flags.get_unsafe() | 64);
                    m_minimum_speed = minimum_speed <= relative_minimum ? minimum_speed : relative_minimum;
                    m_deceleration = (length - m_minimum_speed) * (1.0f / 3.0f);
                    if (m_deceleration <= 0.0001f)
                        m_flags.set(_finished_bit, true);
                }
                if (m_flags.test(_finished_bit))
                    m_ticks_remaining = 0;
                else if (length < m_minimum_speed)
                {
                    m_ticks_remaining = 0;
                    m_flags.set(_finished_bit, true);
                }
                else if (length - m_deceleration < m_minimum_speed)
                {
                    m_ticks_remaining = 0;
                    m_flags.set(_finished_bit, true);
                    point_from_line3d(&displacement, &unit_displacement, -(length - m_minimum_speed),
                        &displacement);
                }
                else
                {
                    point_from_line3d(&displacement, &unit_displacement, -m_deceleration, &displacement);
                    m_ticks_remaining = melee_update_round((float)floor(length / m_deceleration - 0.5f));
                }
            }
        }
        melee_update_scale(&displacement, (float)TLS_GAME_TIME_GLOBALS->ticks_per_second, &physics_output->velocity);
    }
    m_elapsed_ticks++;
    if (m_count > 0)
    {
        m_count--;
        if (m_count == 0)
            m_count = -1;
    }
    bool finished = false;
    if (m_distance > k_melee_update_maximum_distance || !m_flags.test(_initialized_bit) || m_flags.test(_finished_bit) ||
        m_ticks_remaining <= 0 || m_elapsed_ticks >= m_duration_ticks + 6)
        finished = true;
    if (finished)
        physics_output->flags |= 4;
    else
        physics_output->flags &= ~4UL;
    if (finished)
        character_physics_mode_melee_clamp_velocity((const s_melee_clamp_data *)this, &physics_output->velocity);
}
