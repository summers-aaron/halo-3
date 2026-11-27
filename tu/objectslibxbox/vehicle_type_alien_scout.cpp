// objectslibxbox/vehicle_type_alien_scout.obj: from the records 0x3714e8 0x372208 0x372388 0x372498 0x3727b8 0x372970
// @flags /TP /O2 /Oc
#include "units/vehicles.h"
#include "physics/collision_test_vector.h"
#include "units/vehicle_type_alien_scout.h"
#include "math/real_math_types.h"
#include "tag_files/tag_types.h"
#include "animations/animation_id.h"
#include "objects/object_datum.h"
#include "objects/object_placement.h"
#include "units/unit_types.h"
#include "math/real_math.h"
#include "math/real_math_constants.h"
#include "math/real_math_trigonometry.h"
#include "objects/object_header.h"
#include "objects/objects.h"
#include "units/units.h"
#include "effects/effects.h"
#include "math/random_math.h"
#include "units/vehicle_type_component.h"
#include "units/vehicle_utilities.h"
#include "physics/havok_vehicle_physics.h"
#include "physics/physics_variables.h"
#include "physics/collision_types.h"
#include "tag_files/tag_index.h"
#include "animations/animation_manager.h"
#include "units/vehicle_datum.h"
#include "cache/cache_files.h"
#include "effects/effect_vector_types.h"
#include "effects/effect_deterministic_types.h"
#include "units/vehicle_definitions.h"
#include "models/model_types.h"
#include "effects/decals_globals.h"
#include <stddef.h>
#include <math.h>
#include "physics/physics_variable_types.h"
#include "tag_files/tag_groups.h"
#include "effects/effect_creation_flags.h"
#include "effects/effect_creation_data.h"
#include "math/local_random_seed.h"
#include "units/vehicle_alien_scout_definitions.h"
#include "tag_files/string_ids.h"
// @address 0x372970 ?compute_throttle@c_vehicle_type_alien_scout@@CAXJPATreal_vector3d@@@Z
// @address 0x367060 ?vehicle_compute_relative_throttle@@YAXJPATreal_vector3d@@@Z
// @address 0x7e3648 g_local_random_seed

struct effect_vector_field {
  long type;
  real_vector2d direction;
};

struct collision_bsp;


// @address 0x372388 ?process_animation_channels@c_vehicle_type_alien_scout@@QAAXJP6AXJPBUrender_model_definition@@PAVc_animation_channel@@MMMPAX@Z21_N@Z
void c_vehicle_type_alien_scout::process_animation_channels(long object_index, void (*callback)(long, const render_model_definition *,
    c_animation_channel *, float, float, float, void *), void *user, c_animation_channel *target, bool refresh)
{
  object_datum *object = object_get(object_index);
  const c_animation_manager *attachments =
      (const c_animation_manager *)object_header_block_get(object, &object->animation);
  const render_model_definition *effect = (const render_model_definition *)tag_get(0,
      ((const s_model_definition *)tag_get(0,
          ((const vehicle_definition *)tag_get(0, object->definition_index))->model.index))->render_model_index);

  if (refresh)
    marker = attachments->find_parallel_impulse_animation_id(226, 226, 124);
  if ((unsigned short)marker.m_animation_index < 0x8000 &&
      attachments->setup_animation_channel_by_index((c_animation_channel *)target, marker, 0))
    vehicle_impulse_channel_node_preprocess_callback(object_index, effect, (vehicles_animation_channel *)target, scale, 0.0f, 1.0f, user);
}

void c_vehicle_type_alien_scout::compute_throttle(long object_index, real_vector3d *throttle)
{
  vehicle_datum *object = (vehicle_datum *)object_get_and_verify_type(object_index, 2);
  vehicle_definition *definition = TAG_GET('vehi', vehicle_definition, object->object.definition_index);
  vehicle_type_alien_scout_handling *alien_scout = (vehicle_type_alien_scout_handling *)definition->alien_scout.address;
  if (alien_scout->control_type == 4) {
    *throttle = object->unit.throttle;
    throttle->j = 0.0f;
  } else {
    vehicle_compute_relative_throttle(object_index, throttle);
  }
}

// @address 0x372208 vehicle_type_alien_scout_compute_throttle
extern "C" float vehicle_type_alien_scout_compute_throttle(float current, float input, bool mode, const s_scout_speed_definition *definition,
                              const real_vector3d *throttle, const real_vector3d *velocity, const real_vector3d *direction,
                              float speed_blend, float normal_blend, float mode_blend)
{
    if (definition->maximum - definition->minimum > 0.001f) {
        float magnitude = sqrtf(throttle->i * throttle->i + throttle->j * throttle->j + throttle->k * throttle->k);
        float throttle_scale = fabsf(magnitude) < 1.0f ? fabsf(sqrtf(throttle->j * throttle->j + throttle->k * throttle->k + throttle->i * throttle->i)) : 1.0f;
        float speed = direction ? velocity->i * direction->i + velocity->k * direction->k + velocity->j * direction->j :
                      sqrtf(velocity->i * velocity->i + velocity->j * velocity->j + velocity->k * velocity->k);
        float ratio;
        if (definition->speed_scale <= 0.001f) {
            ratio = 1.0f;
        } else {
            ratio = speed / definition->speed_scale;
            ratio = PIN(ratio, 0.0f, 1.0f);
        }
        float blend = mode ? mode_blend : normal_blend;
        physics_variable_speed rates;
        rates.negative_limit = 1.0f;
        rates.positive_limit = 1.0f;
        rates.away_rate = rates.toward_rate = definition->response;

        float target = ((1.0f - blend) * throttle_scale + (1.0f - input) * blend) * (ratio * speed_blend + (1.0f - speed_blend));
        physics_variable_speed_update_seek(&current, &rates, target, 1.0f, TLS_GAME_TIME_GLOBALS->tick_length);
    }
    return current;
}



static __forceinline float vehicle_type_alien_scout_dot(const real_vector3d *a, const real_vector3d *b)
{
  return a->k * b->k + (a->j * b->j) + a->i * b->i;
}

static __forceinline real_vector3d *vehicle_type_alien_scout_reflect_components(const real_vector3d *vector, const real_vector3d *normal, real_vector3d *result)
{
  float reflection = 2.0f * vehicle_type_alien_scout_dot(vector, normal);
  result->i = vector->i - normal->i * reflection;
  result->j = vector->j - normal->j * reflection;
  result->k = vector->k - normal->k * reflection;
  return result;
}

static __forceinline void alien_scout_clear_creation_flags(long *flags)
{
  *flags = 0;
}

static __forceinline void alien_scout_set_creation_flag(long *flags, e_effect_creation_flags bit, bool enable)
{
  if (enable)
    *flags |= (1 << bit);
  else
    *flags &= ~(1 << bit);
}

static __forceinline void vehicle_type_alien_scout_initialize_creation_data(s_effect_creation_data *creation_data,
    c_tag_index definition_index, long vector_count, const s_effect_vector *vectors, const real_plane3d *plane, const s_location *hint,
    e_effect_deterministic deterministic)
{
  s_effect_creation_data *const *creation_data_ref = &creation_data;
  alien_scout_clear_creation_flags(&(*creation_data_ref)->flags);
  effect_creation_data_initialize(creation_data);
  creation_data->definition_index = definition_index;
  creation_data->effect_vector_count = vector_count;
  creation_data->effect_vectors = vectors;
  creation_data->plane = plane;
  alien_scout_clear_creation_flags(&creation_data->flags);
  if (deterministic == _effect_deterministic)
    alien_scout_set_creation_flag(&creation_data->flags, _effect_creation_deterministic_bit, true);
  if (hint)
    creation_data->location_hint = *hint;
}

static __forceinline void vehicle_type_alien_scout_create_effect(c_tag_index definition_index, const s_damage_owner *damage_owner,
    long parent_effect_index, const real_vector3d *velocity, long vector_count, const s_effect_vector *vectors, float scale,
    const s_location *hint, float boost, long unused, const real_rgb_color *color, const effect_vector_field *field,
    long region_permutation_index)
{
  s_effect_creation_data creation_data;
  alien_scout_clear_creation_flags(&creation_data.flags);
  vehicle_type_alien_scout_initialize_creation_data(&creation_data, definition_index, vector_count, vectors, 0, hint, _effect_deterministic);
  if (damage_owner)
    creation_data.damage_owner = *damage_owner;
  if (velocity)
    creation_data.velocity = *velocity;
  creation_data.scale_a = scale;
  creation_data.parent_effect_index = parent_effect_index;
  creation_data.scale_b = boost;
  creation_data.reserved58 = (long)(ptrdiff_t)field;
  creation_data.reserved54 = (long)(ptrdiff_t)color;
  creation_data.region_permutation_index = region_permutation_index;
  effect_new_from_creation_data(&creation_data);
}

// @address 0x372498
void c_vehicle_type_alien_scout::create_effects(long vehicle_index)
{
  unit_datum *vehicle = (unit_datum *)object_get_and_verify_type(vehicle_index, 2);
  const vehicle_definition *definition = (const vehicle_definition *)tag_get('vehi', vehicle->object.definition_index);
  if (definition->effect.index != -1 && vehicle->unit.seat_acceleration_fractions[0] > 0.0f)
  {
    object_marker markers[16];
    long marker_count = internal_object_get_markers_by_string_id(vehicle_index, 175, markers, 16, false);
    float boost = unit_compute_boost_fraction(vehicle_index);
    for (long marker_index = 0; marker_index < marker_count; ++marker_index)
    {
      object_marker *marker = &markers[marker_index];
      collision_result collision;
      real_vector3d random_direction;
      _random_vector_in_cone3d(&g_local_random_seed, 0, "..\\units\\vehicle_type_alien_scout.cpp", 621,
          &marker->matrix.forward, 0.0f, 15.0f, &random_direction);
      real_vector3d velocity;
      scale_vector3d(&random_direction, 1.0f, &velocity);
      if (collision_test_vector(k_collision_test_structure_and_water, false, &marker->matrix.position, &velocity, vehicle_index, -1, -1, &collision))
      {
        float scale = PIN((0.0f - marker->matrix.forward.k) * (1.0f - collision.fraction) * vehicle->unit.seat_acceleration_fractions[0], 0.0f, 1.0f);
        if (scale > 0.0f)
        {
          effect_vector_field field;
          field.type = 0;
          memset(&field.direction, 0, sizeof(field.direction));
          real_point3d midpoint;
          midpoint.x = (collision.point.x + marker->matrix.position.x) * 0.5f;
          midpoint.y = (collision.point.y + marker->matrix.position.y) * 0.5f;
          midpoint.z = (collision.point.z + marker->matrix.position.z) * 0.5f;
          real_vector3d forward = random_direction;
          normalize3d_with_default(&forward, global_down3d);
          s_effect_vector vectors[8];
          build_common_effect_vectors(&collision.point, &forward, &collision.plane.n, vectors);
          vectors[7].marker_name = 174;
          vectors[7].point = midpoint;
          vehicle_type_alien_scout_reflect_components(&forward, &collision.plane.n, &vectors[7].vector);
          vehicle_type_alien_scout_create_effect(c_tag_index(definition->effect.index), 0, 0, 0, 8, vectors, scale,
              &vehicle->object.location, boost, 0, &objects_get_default_argb_white()[2].rgb, &field, 0);
        }
      }
    }
  }
}

// @address 0x3727b8 ?compute_function_value@c_vehicle_type_alien_scout@@QAA_NJJPAMPA_N@Z
bool c_vehicle_type_alien_scout::compute_function_value(long vehicle_index, long function, float *value, bool *active)
{
  vehicle_datum *object = (vehicle_datum *)object_get_and_verify_type(vehicle_index, 2);
  vehicle_definition *definition = TAG_GET('vehi', vehicle_definition, object->object.definition_index);
  vehicle_type_alien_scout_handling *scout = (vehicle_type_alien_scout_handling *)definition->alien_scout.address;
  bool result = true;
  switch (function) {
  case _string_id_anti_gravity_strength:
    if (definition->model.index != -1)
      *value = object->vehicle.function_value;
    break;
  case _string_id_wingtip_contrail:
    *value = unit_compute_boost_fraction(vehicle_index) * m_throttle.k;
    break;
  case _string_id_hover:
    *value = m_a;
    break;
  case _string_id_engine_power:
    *value = m_throttle.j;
    break;
  case _string_id_anti_gravity_engine_position: {
    float limit = scout->engine.maximum;
    float base = scout->engine.minimum;
    if (limit - base > k_real_one_thousandth_storage) {
      {
        real_vector3d throttle;
        void (*compute)(long, real_vector3d *) = compute_throttle;
        compute(vehicle_index, &throttle);
      }
      *value = (float)fmod((double)(float)fmod((double)((float)TLS_GAME_TIME_GLOBALS->time * TLS_GAME_TIME_GLOBALS->tick_length * base + m_throttle.i),
          1.0), 1.0);
    }
    break;
  }
  case _string_id_gear_position:
    *value = m_b;
    break;
  default:
    result = false;
    break;
  }
  return result;
}






static __forceinline float vehicle_type_alien_scout_tick_length()
{
  return TLS_GAME_TIME_GLOBALS->tick_length;
}




static __forceinline float vehicle_type_alien_scout_forward_speed(const real_vector3d *velocity, const real_vector3d *forward)
{
  return velocity->i * forward->i + (velocity->j * forward->j + velocity->k * forward->k);
}

static __forceinline float vehicle_type_alien_scout_drag(const vehicle_type_alien_scout_handling *handling, const real_vector3d *velocity)
{
  // match: 0x3714e8: k, j, i read in this order and j*j + i*i summed first
  float k = velocity->k;
  float j = velocity->j;
  float i = velocity->i;
  float j_squared = j * j;
  float i_squared = (i * i);
  float speed = square_root((j_squared + i_squared) + k * k);
  return (speed * handling->drag) * (speed * handling->drag);
}



static __forceinline float vehicle_type_alien_scout_dot2d(float ai, float aj, float bi, float bj)
{
  return ai * bi + aj * bj;
}

static __forceinline float vehicle_type_alien_scout_dot2d_vector(const real_vector2d *a, float bi, float bj)
{
  return a->i * bi + a->j * bj;
}

static __forceinline float vehicle_type_alien_scout_airborne_acceleration(float mass, float fraction, float hover, float acceleration)
{
  float hover_blend = 1.0f - hover;
  float scale = fraction * hover_blend;
  return scale * mass * acceleration;
}

static __forceinline void vehicle_type_alien_scout_add_scaled_vector(
    const real_vector3d *point, const real_vector3d *vector, float scale, real_vector3d *result)
{
  real_vector3d step;
  scale_vector3d(vector, scale, &step);
  add_vectors3d(point, &step, result);
}

#define VEHICLE_TYPE_ALIEN_SCOUT_SIGN(value) ((value) != 0.0f ? ((value) < 0.0f ? -1 : 1) : 0)


// @rdata 0x8202490c
static const float vehicle_type_alien_scout_effect_epsilon = 0.0001f;


// @address 0x3714e8
void c_vehicle_type_alien_scout::update_physics(long vehicle_index, s_havok_vehicle_physics_instance *instance)
{
  float tick_length = vehicle_type_alien_scout_tick_length();
  vehicle_datum *vehicle = (vehicle_datum *)object_get_and_verify_type(vehicle_index, 2);
  bool boosted = unit_compute_boost_fraction(vehicle_index) != 0.0f ? true : false;
  const vehicle_definition *definition =
      TAG_GET('vehi', vehicle_definition, vehicle->object.definition_index);
  const vehicle_type_alien_scout_handling *handling = (const vehicle_type_alien_scout_handling *)definition->alien_scout.address;
  const s_havok_vehicle_physics_definition *physics = &definition->havok_vehicle_physics;
  float angular_damping = 0.5f;
  float physics_scale = vehicle->unit.throttle_scale;
  real_vector3d force = *global_zero_vector3d;
  real_vector3d torque = *global_zero_vector3d;
  real_vector3d linear_velocity;
  real_vector3d angular_velocity;
  real_vector3d early_linear_velocity;
  real_vector3d early_angular_velocity;
  object_get_localized_velocities(vehicle_index, &linear_velocity, &angular_velocity, &early_linear_velocity, &early_angular_velocity);
  real_vector3d throttle;
  compute_throttle(vehicle_index, &throttle);
  float minimum_turn_scale = 0.3f;
  if (vehicle->object.up.k > -0.2f) {
    real_vector3d local_velocity;
    matrix4x3_inverse_transform_vector(&instance->world_matrix, &linear_velocity, &local_velocity);
    if (m_a > 0.0f) {
      float boost_scale = 1.0f;
      if (vehicle->vehicle.m_flags.test(_vehicle_alien_scout_boost))
        boost_scale = unit_compute_boost_fraction(vehicle_index) * (definition->boost_peak_power - 1.0f) + 1.0f;
      real_vector3d acceleration;
      if (handling->control_type == 4) {
        float limit = throttle.i >= 0.0f ? handling->acceleration : handling->deceleration;
        set_real_vector3d(&acceleration, throttle.i * limit * boost_scale, 0.0f, 0.0f);
      } else {
        float angle = arctangent(throttle.j, throttle.i);
        float forward_fraction = (1.5707964f - fabs(angle)) / 1.5707964f;
        float forward_weight = fabs(forward_fraction);
        float side_fraction = (angle > 0.0f ? 1.0f : -1.0f) * (1.0f - forward_weight);
        float side_weight = fabs(side_fraction);
        bool forward = forward_fraction > 0.0f ? true : false;
        bool left = side_fraction > 0.0f ? true : false;
        bool pitch_forward = forward;
        bool roll_left = left;
        float forward_limit = forward ? handling->forward_speed : handling->reverse_speed;
        float side_limit = left ? handling->left_speed : handling->right_speed;
        if (handling->control_type == 1) {
          float slope_limit = 0.43196902f;
          float forward_slope_scale = 0.33f;
          float side_slope_scale = 0.33f;
          float pitch = arctangent(instance->world_matrix.forward.k,
              magnitude2d(&instance->world_matrix.forward.xy));
          float roll = arctangent(
              instance->world_matrix.left.k, magnitude2d(&instance->world_matrix.left.xy));
          float pitch_fraction = MIN((pitch / slope_limit) * (pitch / slope_limit), 1.0f);
          float roll_fraction = MIN((roll / slope_limit) * (roll / slope_limit), 1.0f);
          forward_limit -= (pitch_fraction < 0.0f ? -pitch_fraction : pitch_fraction) * forward_slope_scale * forward_limit *
              (pitch_forward == (pitch > 0.0f) ? 1.0f : -1.0f);
          side_limit -= (roll_fraction < 0.0f ? -roll_fraction : roll_fraction) * side_slope_scale * side_limit * (roll_left == (roll > 0.0f) ? 1.0f : -1.0f);
        }
        float speed = forward_weight * forward_limit + side_weight * side_limit;
        if (vehicle->vehicle.m_flags.test(_vehicle_alien_scout_trick))
          speed *= 0.8f;
        speed *= boost_scale;
        real_vector3d desired_velocity;
        scale_vector3d(&throttle, speed, &desired_velocity);
        subtract_vectors3d(&desired_velocity, &local_velocity, &acceleration);
        scale_vector3d(&acceleration, 1.0f / tick_length, &acceleration);
        acceleration.k = 0.0f;
        bool accelerating = vehicle->unit.throttle.i != 0.0f || vehicle->unit.throttle.j != 0.0f;
        float acceleration_angle = accelerating ? arctangent(desired_velocity.j, desired_velocity.i)
                                                : arctangent(-acceleration.j, -acceleration.i);
        float forward_acceleration_fraction = (1.5707964f - fabs(acceleration_angle)) / 1.5707964f;
        float forward_acceleration_weight = fabs(forward_acceleration_fraction);
        float side_acceleration_fraction = (acceleration_angle > 0.0f ? 1.0f : -1.0f) * (1.0f - forward_acceleration_weight);
        float side_acceleration_weight = fabs(side_acceleration_fraction);
        float forward_acceleration = accelerating ? handling->acceleration : handling->deceleration;
        float side_acceleration = accelerating ? handling->lateral_acceleration : handling->lateral_deceleration;
        float acceleration_limit = forward_acceleration_weight * forward_acceleration + side_acceleration_weight * side_acceleration;
        if (vehicle->vehicle.braking_ticks != 0 && fabs(m_steering) > k_pi * 0.25f) {
          float braking_rate = 0.05f;
          float maximum_braking_scale = 0.98f;
          float braking_scale = braking_rate * (float)vehicle->vehicle.braking_ticks;
          if (braking_scale > maximum_braking_scale)
            braking_scale = maximum_braking_scale;
          acceleration_limit *= 1.0f - braking_scale;
        }
        limit3d(&acceleration, acceleration_limit * boost_scale);
      }
      matrix4x3_transform_vector(&instance->world_matrix, &acceleration, &acceleration);
      if (handling->drag > 0.0f || handling->minimum_deceleration > 0.0f) {
        float drag = vehicle_type_alien_scout_drag(handling, &linear_velocity);
        real_vector3d direction = linear_velocity;
        float magnitude = normalize3d(&direction);
        if (magnitude != 0.0f) {
          float deceleration = handling->minimum_deceleration - drag;
          float maximum_deceleration = (float)TLS_GAME_TIME_GLOBALS->ticks_per_second * magnitude;
          deceleration = maximum_deceleration > deceleration ? deceleration : maximum_deceleration;
          deceleration = MAX(deceleration, 0.0f);
          vehicle_type_alien_scout_add_scaled_vector(&acceleration, &direction, -(drag + deceleration), &acceleration);
        }
      }
      point_from_line3d(&force, &acceleration, instance->mass * m_a, &force);
    }
    vehicle_calculate_turning_torque(instance, &angular_velocity, &torque, handling->control_type == 0, handling->turning_scale, m_a, m_steering);
    if (m_a < 1.0f) {
      float turning_rate = 80.0f * k_degrees_to_radians;
      real_vector2d turning = *global_zero_vector2d;
      real_vector3d left;
      cross_product3d(&vehicle->object.up, &vehicle->object.forward, &left);
      real_vector2d horizontal_forward = vehicle->object.forward.xy;
      real_vector2d horizontal_left = left.xy;
      normalize2d(&horizontal_forward);
      normalize2d(&horizontal_left);
      if (vehicle->object.up.k > 0.0f) {
        float damping = 2.0f;
        real_vector2d tilt;
        tilt.i = dot_product2d(&vehicle->object.up.xy, &horizontal_forward);
        const real_vector2d *up_vector = &vehicle->object.up.xy;
        tilt.j = dot_product2d(&horizontal_left, up_vector);
        real_vector2d angular_speed;
        angular_speed.i = vehicle_type_alien_scout_dot2d_vector(&angular_velocity.xy, horizontal_left.i, horizontal_left.j);
        angular_speed.j = -vehicle_type_alien_scout_dot2d_vector(&angular_velocity.xy, horizontal_forward.i, horizontal_forward.j);
        real_vector2d correction = *global_zero_vector2d;
        correction.i += tilt.i * -1.0f;
        correction.j += tilt.j * -1.0f;
        correction.i += angular_speed.i * (-1.0f / damping);
        correction.j += angular_speed.j * (-1.0f / damping);
        float turn_scale = 1.0f;
        real_vector2d response;
        response.i = (float)VEHICLE_TYPE_ALIEN_SCOUT_SIGN(correction.i * throttle.i) * turn_scale * fabs(correction.i);
        response.j = (float)VEHICLE_TYPE_ALIEN_SCOUT_SIGN(correction.j * throttle.j) * turn_scale * fabs(correction.j);
        float forward_scale = PIN(1.0f + response.i, minimum_turn_scale, 2.5f);
        turning.i += turning_rate * throttle.i * forward_scale;
        float side_scale = PIN(1.0f + response.j, minimum_turn_scale, 2.5f);
        turning.j += turning_rate * throttle.j * side_scale;
        float upright_rate = 200.0f * k_degrees_to_radians;
        turning.i += upright_rate * correction.i * (1.0f - vehicle->object.up.k);
        turning.j += upright_rate * correction.j * (1.0f - vehicle->object.up.k);
      } else {
        turning.i += turning_rate * throttle.i;
        turning.j += turning_rate * throttle.j;
      }
      real_vector3d airborne_torque = *global_zero_vector3d;
      point_from_line3d(&airborne_torque, &left, turning.i * instance->inertia_tensor.left.j, &airborne_torque);
      // match: 0x3714e8: keeps the torque scale as the first multiply operand in the forward i-component fmadds.
      const float &forward_turning_torque = -turning.j * instance->inertia_tensor.forward.i;
      point_from_line3d(&airborne_torque, &vehicle->object.forward, forward_turning_torque, &airborne_torque);
      point_from_line3d(&torque, &airborne_torque, 1.0f - m_a, &torque);
    }
    if (vehicle->vehicle.m_flags.test(_vehicle_alien_scout_trick) && handling->forward_speed > 0.001f) {
      float speed_fraction = vehicle_type_alien_scout_forward_speed(&linear_velocity, &vehicle->object.forward) / handling->forward_speed;
      speed_fraction = PIN(speed_fraction, 0.0f, 1.0f);
      real_vector3d left;
      cross_product3d(&vehicle->object.up, &vehicle->object.forward, &left);
      if (speed_fraction > 0.0f) {
        float pitch_rate = 300.0f * k_degrees_to_radians;
        float lift = 3.6000001f;
        point_from_line3d(&torque, &left, -pitch_rate * instance->inertia_tensor.left.j * speed_fraction * m_a, &torque);
        point_from_line3d(&force, global_up3d, lift * instance->mass * speed_fraction * m_a, &force);
      }
      if (vehicle->vehicle.airborne_ticks != 0) {
        real_vector3d forward;
        cross_product3d(&left, global_up3d, &forward);
        if (normalize3d(&forward) > 0.0f) {
          float forward_acceleration = 1.8000001f;
          float upward_acceleration = 0.90000004f;
          float period = 1.0f;
          float fraction = PIN(1.0f - (float)vehicle->vehicle.airborne_ticks * tick_length / period, 0.0f, 1.0f);
          point_from_line3d(&force, &forward,
              vehicle_type_alien_scout_airborne_acceleration(instance->mass, fraction, m_a, forward_acceleration), &force);
          point_from_line3d(&force, global_up3d,
              vehicle_type_alien_scout_airborne_acceleration(instance->mass, fraction, m_a, upward_acceleration), &force);
        }
      }
    }
    scale_vector3d(&force, physics_scale, &force);
    scale_vector3d(&torque, physics_scale, &torque);
  }
  long region_count;
  char *region_permutations;
  object_region_information *region_information;
  object_get_region_information(vehicle_index, &region_count, &region_permutations, 0, &region_information);
  for (long index = 0; index < physics->anti_gravity_points.count; ++index) {
    const s_havok_vehicle_anti_gravity_point_definition *point =
        TAG_BLOCK_GET_ELEMENT(&physics->anti_gravity_points, index, s_havok_vehicle_anti_gravity_point_definition);
    instance->anti_gravity_points[index].strength = physics_scale;
    if (TEST_BIT(point->flags, 0) && point->damage_source_region_index >= 0 && point->damage_source_region_index < region_count)
      instance->anti_gravity_points[index].oscillation = point->runtime_damage_state_errors[region_information[point->damage_source_region_index].state];
    else
      instance->anti_gravity_points[index].oscillation = 0.0f;
  }
  instance->steering = m_steering;
  havok_vehicle_physics_update(vehicle_index, instance, 0.0f, true, &force, &torque, 0);
  vehicle_update_antigravity_hover(instance, &m_a);
  float engine_fraction = vehicle_type_alien_scout_compute_throttle(
      m_throttle.j, m_a, boosted, &handling->engine, &throttle, &linear_velocity, 0, 0.5f, minimum_turn_scale, 0.1f);
  m_throttle.j = engine_fraction;
  float engine_rate = (handling->engine.maximum - handling->engine.minimum) * engine_fraction;
  m_throttle.i += engine_rate * vehicle_type_alien_scout_tick_length();
  m_throttle.i = (float)fmod((double)m_throttle.i, 1.0);
  m_throttle.k = vehicle_type_alien_scout_compute_throttle(
      m_throttle.k, m_a, boosted, &handling->contrail, &throttle, &linear_velocity, &instance->world_matrix.forward, 0.9f, 0.0f, 0.0f);
  float engine_function;
  bool deterministic;
  const unsigned long &function_owner = vehicle->object.definition_index;
  if (!object_get_function_value(vehicle_index, 0x4a4, function_owner, &engine_function, &deterministic))
    engine_function = 0.0f;
  float effect_rate = fabs(engine_function) * (handling->maximum_effect_rate - handling->minimum_effect_rate) +
      handling->minimum_effect_rate;
  float rate_scale = 1.0f;
  if (object_get_function_value(vehicle_index, 0x4a5, vehicle->object.definition_index, &rate_scale, &deterministic)) {
    float scale = PIN(rate_scale, 0.0f, 1.0f);
    if (fabs(scale - 1.0f) >= vehicle_type_alien_scout_effect_epsilon) {
      float minimum = handling->minimum_effect_rate * scale;
      effect_rate = minimum + (effect_rate - minimum) * scale;
    }
  }
  m_b += vehicle_type_alien_scout_tick_length() * effect_rate;
  m_b = (float)fmod((double)m_b, 1.0);
}
