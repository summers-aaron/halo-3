// objectslibxbox/vehicle_type_alien_fighter.obj: from the records 0x3729e8 0x372b80 0x373a70 0x373b80 0x373cd8 0x373d50 0x373df0 0x373e80 0x373f00 0x374058
// @flags /TP /O2 /Oc
#include "physics/physics_constants_tls.h"
#include "units/vehicle_type_alien_fighter.h"
#include "units/vehicle_alien_fighter_constants.h"
#include "models/render_model_definitions.h"
#include "memory/tls_globals.h"
#include "simulation/simulation_queue_event_types.h"
#include "units/vehicle_datum.h"
#include "objects/object_header.h"
#include "tag_files/tag_groups.h"
#include "math/matrix_math_types.h"
#include "math/real_math.h"
#include "tag_files/tag_types.h"
#include "animations/animation_id.h"
#include "objects/object_datum.h"
#include "units/vehicle_types.h"
#include "units/vehicle_definitions.h"
#include "units/vehicles.h"
#include "physics/physics_constants_globals.h"
// @address 0x373b80 ?compute_function_value@c_vehicle_type_alien_fighter@@QAA_NJJPAMPA_N@Z
// @address 0x373cd8 ?trick_duration@c_vehicle_type_alien_fighter@@AAAMJ@Z
// @address 0x373df0 ?start_trick@c_vehicle_type_alien_fighter@@QAAXJW4e_vehicle_trick_type@@@Z
#include <math.h>
#include "game/game_time_types.h"
#include "objects/objects.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "units/units.h"
#include "physics/havok_vehicle_physics.h"
#include "units/vehicle_utilities.h"
#include "math/matrix_math.h"
#include "animations/animation_manager.h"
#include "networking/network_configuration.h"
#include "simulation/game_interface/simulation_game_events.h"
#include "animations/mixing_board/channels/animation_channel.h"
#include "networking/network_configuration_types.h"
#include "physics/physics_variable_types.h"
#include "physics/physics_constants_types.h"
#include "render/render_cameras.h"





#include "units/vehicle_alien_fighter_definition.h"
#include "tag_files/string_ids.h"

typedef char alien_fighter_forward_speed_offset[(offsetof(s_vehicle_alien_fighter_definition, forward_speed) == 0x14) ? 1 : -1];
typedef char alien_fighter_vertical_speed_offset[(offsetof(s_vehicle_alien_fighter_definition, vertical_speed) == 0x24) ? 1 : -1];
typedef char alien_fighter_definition_size[(sizeof(s_vehicle_alien_fighter_definition) == 0x64) ? 1 : -1];
typedef char alien_fighter_trick_event_delay_offset[(offsetof(s_network_configuration, event_relevance)
  + 21 * sizeof(s_simulation_event_relevance_entry)
  + offsetof(s_simulation_event_relevance_entry, delay) == 0x61c)
  ? 1 : -1];
#include "math/real_math_constant_storage.h"

extern "C" {

// @address 0x58718 k_alien_fighter_thrust_function_scale
const float k_alien_fighter_thrust_function_scale = 6.6666665f;

}



#include "models/model_types.h"



static __forceinline bool alien_fighter_flags_test(unsigned short flags, long bit) { return ((flags >> bit) & 1) != 0; }
static __forceinline float alien_fighter_arctangent(float y, float x)
{
  return (float)atan2((double)y, (double)x);
}





// @address 0x373a70 ?process_animation_channels@c_vehicle_type_alien_fighter@@QAAXJP6AXJPBUrender_model_definition@@PAVc_animation_channel@@MMMPAX@Z21_N@Z
void c_vehicle_type_alien_fighter::process_animation_channels(long object_index,
    void (*callback)(long, const render_model_definition *, c_animation_channel *, float, float, float, void *),
    void *user, c_animation_channel *channel, bool refresh)
{
  vehicle_datum *object = (vehicle_datum *)object_get_and_verify_type(object_index, NONE);
  vehicle_definition *definition = (vehicle_definition *)tag_get(NONE, object->object.definition_index);
  s_model_definition *model = (s_model_definition *)tag_get(NONE, definition->model.index);
  render_model_definition *render_model = (render_model_definition *)tag_get(NONE, model->render_model_index);
  c_animation_manager *manager = (c_animation_manager *)((char *)object + object->object.animation.offset);
  if (refresh) {
    animation_id = manager->find_parallel_impulse_animation_id(226, 226, 124);
  }
  if (animation_id.valid()) {
    if (manager->setup_animation_channel_by_index(channel, animation_id, 0))
      vehicle_impulse_channel_node_preprocess_callback(object_index, render_model, (vehicles_animation_channel *)channel, animation_weight,
          0.0f, 1.0f, user);
  }
}

static __forceinline float alien_fighter_safe_divide(float numerator, float denominator)
{
  float result = 0.0f;
  if (fabs(denominator) > 0.001f)
    result = numerator / denominator;
  return result;
}
bool c_vehicle_type_alien_fighter::compute_function_value(long object_index, long function, float *value, bool *force_active)
{
  vehicle_datum *object = (vehicle_datum *)object_get_and_verify_type(object_index, NONE);
  vehicle_definition *definition = (vehicle_definition *)tag_get(NONE, object->object.definition_index);
  s_vehicle_alien_fighter_definition *alien_fighter = (s_vehicle_alien_fighter_definition *)definition->alien_fighter.address;
  bool result = true;
  switch (function) {
  case _string_id_thrust:
    *value = thrust;
    break;
  case _string_id_engine_rpm: {
    float forward_value = alien_fighter_safe_divide((float)fabs(forward), (float)fabs(alien_fighter->forward_speed.positive_limit));
    float vertical_value = alien_fighter_safe_divide((float)fabs(vertical), (float)fabs(alien_fighter->vertical_speed.positive_limit));
    *value = forward_value > vertical_value ? forward_value : vertical_value;
    break;
  }
  case _string_id_wingtip_contrail_new: {
    float lower = 0.15f;
    float upper = 0.3f;
    float speed = alien_fighter_safe_divide(magnitude3d(&object->object.linear_velocity), alien_fighter->forward_speed.positive_limit);
    float numerator = speed * thrust - lower;
    float scaled = 0.0f;
    if (fabs(upper - lower) > 0.001f)
      scaled = numerator * k_alien_fighter_thrust_function_scale;
    *value = scaled;
    break;
  }
  default:
    result = false;
    break;
  }
  return result;
}

extern "C" {

// @address 0x587c0 k_alien_fighter_trick_angle_period
// Two-pi rounded to float, then widened to double.
const double k_alien_fighter_trick_angle_period = 6.2831855f;


// @address 0x587b8 k_alien_fighter_backward_trick_minimum_angle
const float k_alien_fighter_backward_trick_minimum_angle = 2.5132742f;


// @address 0x587b4 k_alien_fighter_backward_trick_maximum_angle
const float k_alien_fighter_backward_trick_maximum_angle = 3.7699113f;


// @address 0x587b0 k_alien_fighter_roll_trick_minimum_angle
const float k_alien_fighter_roll_trick_minimum_angle = 0.08726646f;


// @address 0x587ac k_alien_fighter_roll_trick_maximum_angle
const float k_alien_fighter_roll_trick_maximum_angle = 6.195919f;

}
// @address 0x373f00 ?compute_new_trick_type@c_vehicle_type_alien_fighter@@CA?AW4e_vehicle_trick_type@@J@Z
e_vehicle_trick_type c_vehicle_type_alien_fighter::compute_new_trick_type(long object_index)
{
  vehicle_datum *vehicle = (vehicle_datum *)object_get_and_verify_type(object_index, NONE);
  e_vehicle_trick_type trick_type = _vehicle_trick_type_none;
  if (alien_fighter_flags_test(vehicle->vehicle.flags, 3) && !(vehicle->unit.control_flags & 0x800)) {
    float forward_abs = vehicle->unit.throttle.i >= 0.0f ? vehicle->unit.throttle.i : -vehicle->unit.throttle.i;
    float strafe_abs = vehicle->unit.throttle.j >= 0.0f ? vehicle->unit.throttle.j : -vehicle->unit.throttle.j;
    if (forward_abs > 0.75f || strafe_abs > 0.7f) {
      float angle = (float)fmod((double)(alien_fighter_arctangent(vehicle->unit.throttle.j,
    vehicle->unit.throttle.i) + 12.566371f), k_alien_fighter_trick_angle_period);
      if (angle >= k_alien_fighter_backward_trick_minimum_angle && angle <= k_alien_fighter_backward_trick_maximum_angle)
        trick_type = _vehicle_trick_type_backward;
      else if (angle >= k_alien_fighter_roll_trick_minimum_angle && angle <= k_alien_fighter_roll_trick_maximum_angle) {
        if (angle >= 0.0f && angle <= k_alien_fighter_backward_trick_minimum_angle)
          trick_type = _vehicle_trick_type_roll_left;
        else
          trick_type = _vehicle_trick_type_roll_right;
      }
    }
  }
  return trick_type;
}

float c_vehicle_type_alien_fighter::trick_duration(long object_index)
{
  object_datum *object = (object_datum *)object_get_and_verify_type(object_index, NONE);
  vehicle_definition *definition = (vehicle_definition *)tag_get(NONE, object->definition_index);
  s_vehicle_alien_fighter_definition *alien_fighter = (s_vehicle_alien_fighter_definition *)definition->alien_fighter.address;
  if (type > 0) {
    if (type > 2) {
      if (type != 3)
        return 0.0f;
      return alien_fighter->loop_trick_duration;
    }
    return alien_fighter->roll_trick_duration;
  }
  return 0.0f;
}



const float alien_fighter_real_one = 1.0f;
const float alien_fighter_real_zero = 0.0f;

// @address 0x373d50 ?compute_trick_t@c_vehicle_type_alien_fighter@@AAAMJ@Z
float c_vehicle_type_alien_fighter::compute_trick_t(long object_index)
{
  float elapsed = (float)this->count * tls_game_time_globals()->tick_length;
  float duration = this->trick_duration(object_index);
  float result = alien_fighter_real_zero;
  if (elapsed < duration) {
    if (this->type == 1 || this->type == 2) {
      float t = alien_fighter_real_one - elapsed / duration;
      result = alien_fighter_real_one - t * t;
    } else {
      float t = elapsed / duration;
      result = t * t;
    }
  }
  return result;
}


static __forceinline void alien_fighter_request_trick(long object_index, e_vehicle_trick_type trick)
{
  long reference = object_index;
  long controlling_player_index = -1;
  if (simulation_query_predictive_event(object_index, 0, &controlling_player_index)) {
    long player_index = controlling_player_index;
    controlling_player_index = trick;
    simulation_event_generate(_simulation_event_type_vehicle_trick, 1, &reference, player_index,
    sizeof(controlling_player_index), &controlling_player_index,
        g_network_configuration.event_relevance[_simulation_event_type_vehicle_trick].delay);
  }
}

void c_vehicle_type_alien_fighter::start_trick(long object_index, e_vehicle_trick_type trick)
{
  type = trick;
  count = 0;
  alien_fighter_request_trick(object_index, trick);
}

// @address 0x373e80 ?is_running_trick@c_vehicle_type_alien_fighter@@QAA_NJ@Z
bool c_vehicle_type_alien_fighter::is_running_trick(long fighter_index)
{
  bool result = false;
  if (type != 0) {
    float scaled = (float)this->count * tls_game_time_globals()->tick_length;
    result = scaled < trick_duration(fighter_index);
  }
  return result;
}

// @address 0x3729e8 ?update_control@c_vehicle_type_alien_fighter@@QAAXJ@Z
void c_vehicle_type_alien_fighter::update_control(long object_index)
{
  vehicle_datum *object = (vehicle_datum *)object_get_and_verify_type(object_index, NONE);
  vehicle_definition *definition = (vehicle_definition *)tag_get(NONE, object->object.definition_index);
  s_vehicle_alien_fighter_definition *alien_fighter = (s_vehicle_alien_fighter_definition *)definition->alien_fighter.address;
  bool running_trick = is_running_trick(object_index);
  if (object->unit.throttle_scale != 0.0f) {
    float throttle = object->unit.throttle.i;
    if (!(throttle < 0.0f)) {
      float limit = alien_fighter->cruising_throttle;
      if (!(throttle > limit))
        object->unit.throttle.i = limit;
    }
    if (vehicle_update_boost(object_index, (unsigned char)running_trick) != 0 && (unsigned char)running_trick != 0) {
      signed char trick = type;
      if (trick == _vehicle_trick_type_roll_left) {
        const real_matrix4x3 *identity = &k_negative_identity4x3;
        identity -= 1;
        // match: 0x3729e8: value range assumption guides branch lowering
        __assume(identity != &k_negative_identity4x3);
        object->unit.throttle = identity->left;
        float speed = alien_fighter->vertical_speed.positive_limit;
        if (speed < 0.0f)
          speed = -speed;
        vertical = speed;
      }
      else if (trick == _vehicle_trick_type_roll_right) {
        object->unit.throttle = k_negative_identity4x3.left;
        float speed = alien_fighter->vertical_speed.negative_limit;
        if (speed < 0.0f)
          speed = -speed;
        vertical = -speed;
      }
    }
  }
  vehicle_update_control_classic(object_index, &object->object.forward, &object->object.up,
    object->unit.throttle.i, object->unit.throttle.j,
    &alien_fighter->steering_control, &alien_fighter->turning_control,
    &alien_fighter->forward_speed,
    &alien_fighter->vertical_speed,
    false, false, &turn, &forward, &vertical);
}


static __forceinline real_vector3d *alien_fighter_cross_product3d(const real_vector3d *a, const real_vector3d *b, real_vector3d *result)
{
  float i = a->j * b->k - a->k * b->j;
  float k = a->i * b->j - a->j * b->i;
  float j = a->k * b->i - a->i * b->k;
  result->i = i;
  result->j = j;
  result->k = k;
  return result;
}
static __forceinline float alien_fighter_cosine(float angle) { return (float)cos(angle); }
static __forceinline float alien_fighter_sine(float angle) { return (float)sin(angle); }
static __forceinline void alien_fighter_rotate_vector(real_vector3d *vector, const real_vector3d *perpendicular, float sine, float cosine)
{
  vector->i = vector->i * cosine + perpendicular->i * sine;
  vector->j = vector->j * cosine + perpendicular->j * sine;
  vector->k = vector->k * cosine + perpendicular->k * sine;
}
static __forceinline void alien_fighter_yaw_vectors(real_vector3d *vector, const real_vector3d *axis, float sine, float cosine)
{
  real_vector3d perpendicular;
  alien_fighter_cross_product3d(axis, vector, &perpendicular);
  alien_fighter_rotate_vector(vector, &perpendicular, sine, cosine);
}
static __forceinline void alien_fighter_pitch_vectors(real_vector3d *forward, real_vector3d *up, float sine, float cosine)
{
  real_vector3d negative_forward;
  negative_forward.i = -forward->i;
  negative_forward.j = -forward->j;
  negative_forward.k = -forward->k;
  real_vector3d *rotated = forward;
  alien_fighter_rotate_vector(rotated, up, sine, cosine);
  alien_fighter_rotate_vector(up, &negative_forward, sine, cosine);
}

// @address 0x374058 ?compute_reference_and_trick_rotation@c_vehicle_type_alien_fighter@@AAA_NJPBUs_havok_vehicle_physics_instance@@PAUreal_matrix3x3@@1@Z
bool c_vehicle_type_alien_fighter::compute_reference_and_trick_rotation(
    long object_index, const s_havok_vehicle_physics_instance *instance, real_matrix3x3 *reference_rotation, real_matrix3x3 *trick_rotation)
{
  vehicle_datum *object = (vehicle_datum *)object_get_and_verify_type(object_index, NONE);
  float t = compute_trick_t(object_index);
  bool running = t > 0.0f ? true : false;

  reference_rotation->forward = object->unit.facing_vector;
  const real_vector3d *up = global_up3d;
  reference_rotation->up = *up;
  real_vector3d projection;
  scale_vector3d(&reference_rotation->forward, dot_product3d(&reference_rotation->forward, &reference_rotation->up), &projection);
  subtract_vectors3d(&reference_rotation->up, &projection, &reference_rotation->up);
  if (normalize3d(&reference_rotation->up) == 0.0f) {
    const real_vector3d *fallback = global_forward3d;
    reference_rotation->up = *fallback;
  }
  alien_fighter_cross_product3d(&reference_rotation->up, &reference_rotation->forward, &reference_rotation->left);

  if (running) {
    *trick_rotation = *reference_rotation;
    switch (type) {
    case _vehicle_trick_type_roll_left: {
      float angle = -t * 6.086836f + 0.19634955f;
      float cosine = alien_fighter_cosine(angle);
      float sine = alien_fighter_sine(angle);
      alien_fighter_yaw_vectors(&trick_rotation->up, &trick_rotation->forward, sine, cosine);
      break;
    }
    case _vehicle_trick_type_roll_right: {
      float angle = t * 6.086836f - 0.19634955f;
      float cosine = alien_fighter_cosine(angle);
      float sine = alien_fighter_sine(angle);
      alien_fighter_yaw_vectors(&trick_rotation->up, &trick_rotation->forward, sine, cosine);
      break;
    }
    case _vehicle_trick_type_backward: {
      float angle = t * 6.2831855f;
      float cosine = alien_fighter_cosine(angle);
      float sine = alien_fighter_sine(angle);
      alien_fighter_pitch_vectors(&trick_rotation->forward, &trick_rotation->up, sine, cosine);
      break;
    }
    default:
      // match: 0x374058: unreachable hint omits dead branch check
      __assume(0);
    }
    alien_fighter_cross_product3d(&trick_rotation->up, &trick_rotation->forward, &trick_rotation->left);
  }
  else {
    *trick_rotation = instance->world_matrix.rotation;
  }
  return running;
}




#define ALIEN_FIGHTER_ABS(value) ((value) >= 0.0f ? (value) : -(value))

static __forceinline const s_vehicle_alien_fighter_definition *alien_fighter_definition_get(long definition_index)
{
  return (const s_vehicle_alien_fighter_definition *)((vehicle_definition *)tag_get(NONE, definition_index))->alien_fighter.address;
}

static __forceinline bool alien_fighter_can_select_new_trick(long vehicle_index, unsigned char count)
{
  const vehicle_datum *vehicle = (const vehicle_datum *)object_get_and_verify_type(vehicle_index, NONE);
  return count > (long)(alien_fighter_definition_get(vehicle->object.definition_index)->maximum_trick_frequency
    * (float)tls_game_time_globals()->ticks_per_second + 0.5f);
}

static __forceinline bool alien_fighter_vehicle_is_running_trick(long vehicle_index)
{
  bool result;
  if (vehicle_get_type(vehicle_index) != 4)
    result = false;
  else {
    vehicle_datum *vehicle = (vehicle_datum *)object_get_and_verify_type(vehicle_index, NONE);
    result = ((c_vehicle_type_alien_fighter *)vehicle->vehicle.type_state)->is_running_trick(vehicle_index);
  }
  return result;
}

static __forceinline real_matrix3x3 *alien_fighter_matrix3x3_from_forward_and_up(real_matrix3x3 *matrix,
    const real_vector3d *forward, const real_vector3d *up)
{
  matrix->forward = *forward;
  matrix->left.i = up->j * forward->k - up->k * forward->j;
  matrix->left.j = up->k * forward->i - up->i * forward->k;
  matrix->left.k = up->i * forward->j - up->j * forward->i;
  matrix->up = *up;
  return matrix;
}

static __forceinline void alien_fighter_update_pitch_vectors(real_vector3d *forward, real_vector3d *up, float sine, float cosine)
{
  real_vector3d negative_forward;
  negate_vector3d(forward, &negative_forward);
  forward->i = forward->i * cosine + up->i * sine;
  forward->j = forward->j * cosine + up->j * sine;
  forward->k = forward->k * cosine + up->k * sine;
  up->i = up->i * cosine + negative_forward.i * sine;
  up->j = up->j * cosine + negative_forward.j * sine;
  up->k = up->k * cosine + negative_forward.k * sine;
}

static __forceinline void alien_fighter_update_yaw_vectors(real_vector3d *vector, const real_vector3d *axis, float sine, float cosine)
{
  real_vector3d perpendicular;
  cross_product3d(axis, vector, &perpendicular);
  vector->i = vector->i * cosine + perpendicular.i * sine;
  vector->j = vector->j * cosine + perpendicular.j * sine;
  vector->k = vector->k * cosine + perpendicular.k * sine;
}

static __forceinline float alien_fighter_update_projection_dot(const real_vector3d *a, const real_vector3d *b)
{
  return (a->i * b->i + a->j * b->j) + a->k * b->k;
}

static __forceinline real_vector3d *alien_fighter_update_scale_vector3d(const real_vector3d *vector, const float &scale, real_vector3d *result)
{
  result->i = vector->i * scale;
  result->j = vector->j * scale;
  result->k = vector->k * scale;
  return result;
}

// @address 0x372b80 ?update_physics@c_vehicle_type_alien_fighter@@QAAXJPAUs_havok_vehicle_physics_instance@@@Z
void c_vehicle_type_alien_fighter::update_physics(long vehicle_index, s_havok_vehicle_physics_instance *instance)
{
  vehicle_datum *object = (vehicle_datum *)object_get_and_verify_type(vehicle_index, NONE);
  const vehicle_definition *definition = (const vehicle_definition *)tag_get(NONE, object->object.definition_index);
  const s_vehicle_alien_fighter_definition *alien_fighter = alien_fighter_definition_get(object->object.definition_index);
  float minimum_pitch = 0.0f;
  float vertical_control = 0.0f;
  float maximum_pitch = 0.5890486f;
  if (alien_fighter_can_select_new_trick(vehicle_index, count)) {
    e_vehicle_trick_type trick = compute_new_trick_type(vehicle_index);
    if (trick != _vehicle_trick_type_none)
      start_trick(vehicle_index, trick);
  }

  float pitch = alien_fighter_arctangent(object->object.forward.k, sqrtf(magnitude_squared2d(&object->object.forward.xy)));
  float boost = alien_fighter_flags_test(object->vehicle.flags, 4)
    ? 1.0f + unit_compute_boost_fraction(vehicle_index) * (definition->boost_peak_power - 1.0f)
    : 1.0f;
  float pitch_scale = 1.0f + (pitch >= 0.0f ? -1.0f : 1.0f) * alien_fighter->dive_speed_scale
    * (PIN(ALIEN_FIGHTER_ABS(pitch), minimum_pitch, maximum_pitch) - minimum_pitch) / (maximum_pitch - minimum_pitch);
  float maximum_forward_speed = alien_fighter->forward_speed.positive_limit * pitch_scale * boost;
  real_matrix3x3 reference_rotation;
  (void)&reference_rotation.left.j;
  real_matrix3x3 trick_rotation;
  bool running_trick = compute_reference_and_trick_rotation(vehicle_index, instance, &reference_rotation, &trick_rotation);
  count = (unsigned char)MIN(count + 1, 254);

  real_vector3d velocity;
  (void)&velocity.j;
  real_vector3d angular_velocity;
  object_get_velocities(vehicle_index, &velocity, &angular_velocity);
  subtract_vectors3d(&velocity, &g_physics_constants->velocity_frame, &velocity);
  real_vector3d desired_velocity;
  scale_vector3d(&object->object.forward, forward * pitch_scale * boost, &desired_velocity);
  float acceleration_scale;
  if (alien_fighter_flags_test(object->vehicle.flags, 4) && !running_trick && !alien_fighter_flags_test(object->vehicle.flags, 3))
    acceleration_scale = 1.0f;
  else if (forward > 0.0f)
    acceleration_scale = ALIEN_FIGHTER_ABS(maximum_forward_speed) < 0.001f ? 0.0f : forward / maximum_forward_speed;
  else
    acceleration_scale = ALIEN_FIGHTER_ABS(alien_fighter->forward_speed.negative_limit) < 0.001f
      ? 0.0f : -forward / alien_fighter->forward_speed.negative_limit;

  real_vector3d acceleration;
  vehicle_compute_acceleration(&velocity, &desired_velocity, alien_fighter->forward_speed.away_rate * acceleration_scale,
    alien_fighter->forward_speed.toward_rate * acceleration_scale, &acceleration, tls_game_time_globals()->tick_length);
  if (forward == 0.0f) {
    real_vector3d projection;
    scale_vector3d(&reference_rotation.left, dot_product3d(&reference_rotation.left, &velocity), &projection);
    real_vector3d lateral_velocity;
    subtract_vectors3d(&velocity, &projection, &lateral_velocity);
    lateral_velocity.k = 0.0f;
    if (normalize3d(&lateral_velocity) != 0.0f) {
      real_vector3d correction;
      scale_vector3d(&lateral_velocity, -alien_fighter->off_stick_decel_scale * ALIEN_FIGHTER_ABS(alien_fighter->forward_speed.toward_rate), &correction);
      add_vectors3d(&acceleration, &correction, &acceleration);
    }
  }
  if (forward <= 0.0f) {
    real_vector3d new_velocity;
    add_vectors3d(&velocity, &acceleration, &new_velocity);
    for (long axis = 0; axis < 3; axis++) {
      bool nonpositive;
      if (acceleration.n[axis] <= 0.0f)
        nonpositive = true;
      else
        nonpositive = false;
      acceleration.n[axis] = (new_velocity.n[axis] >= 0.0f ? 1 : 0) == nonpositive ? acceleration.n[axis] : -velocity.n[axis];
    }
    acceleration.k = 0.0f;
  }

  bool running_roll;
  // match: 0x372b80: zero-bit rotate preserves 32-bit GPR value
  if (alien_fighter_vehicle_is_running_trick(vehicle_index) && ((long)_rotl(_rotl(_rotl(type, 0), 0), 0) == 1 || type == 2))
    running_roll = true;
  else
    running_roll = false;
  if (!alien_fighter_flags_test(object->vehicle.flags, 4) || running_roll) {
    float trick_scale = running_roll ? alien_fighter->strafe_boost_scale * boost * (1.0f - compute_trick_t(vehicle_index)) : 1.0f;
    vertical_control = vertical;
    float positive_limit = alien_fighter->forward_speed.positive_limit * trick_scale;
    float negative_limit = alien_fighter->forward_speed.positive_limit * trick_scale;
    float vertical_fraction = trick_scale * vertical_control > 0.0f
      ? (ALIEN_FIGHTER_ABS(positive_limit) < 0.001f ? 0.0f : (trick_scale / positive_limit) * vertical_control)
      : (ALIEN_FIGHTER_ABS(negative_limit) < 0.001f ? 0.0f : (-trick_scale / negative_limit) * vertical_control);
    real_vector3d desired_vertical_velocity;
    alien_fighter_update_scale_vector3d(&reference_rotation.left, vertical_control * trick_scale, &desired_vertical_velocity);
    float vertical_speed = dot_product3d(&reference_rotation.left, &velocity);
    real_vector3d current_vertical_velocity;
    scale_vector3d(&reference_rotation.left, vertical_speed, &current_vertical_velocity);
    real_vector3d vertical_acceleration;
    vehicle_compute_acceleration(&current_vertical_velocity, &desired_vertical_velocity,
    alien_fighter->vertical_speed.away_rate * vertical_fraction,
      (trick_scale * vertical_fraction) * alien_fighter->vertical_speed.toward_rate, &vertical_acceleration,
    tls_game_time_globals()->tick_length);
    float vertical_component = alien_fighter_update_projection_dot(&acceleration, &reference_rotation.left);
    real_vector3d vertical_correction;
    scale_vector3d(&reference_rotation.left, vertical_component * vertical_fraction, &vertical_correction);
    subtract_vectors3d(&acceleration, &vertical_correction, &acceleration);
    add_vectors3d(&vertical_acceleration, &acceleration, &acceleration);
    if (vertical_control == 0.0f) {
      float current_speed = dot_product3d(&velocity, &reference_rotation.left);
      float absolute_speed = ALIEN_FIGHTER_ABS(current_speed);
      float damping = PIN(alien_fighter->off_stick_decel_scale * alien_fighter->vertical_speed.toward_rate, -absolute_speed, absolute_speed);
      real_vector3d correction;
      scale_vector3d(&reference_rotation.left, current_speed < 0.0f ? damping : -damping, &correction);
      add_vectors3d(&acceleration, &correction, &acceleration);
    }
  }

  float horizontal_speed = sqrtf(magnitude_squared2d(&velocity.xy));
  float speed = MAX(ALIEN_FIGHTER_ABS(forward), MAX(ALIEN_FIGHTER_ABS(vertical), horizontal_speed));
  lift_scale = PIN(speed - alien_fighter->full_gravity_speed, 0.0f, alien_fighter->zero_gravity_speed)
    / (alien_fighter->zero_gravity_speed - alien_fighter->full_gravity_speed);
  lift_scale = PIN(lift_scale, 0.0f, 1.0f);
  lift_scale = running_trick ? 1.0f : lift_scale;
  if (forward == 0.0f && !running_trick)
    lift_scale *= 1.0f - PIN(velocity.k, 0.0f, 1.0f) * 0.2f;
  acceleration.k *= lift_scale;
  real_vector3d magic_force;
  scale_vector3d(&acceleration, instance->mass * object->unit.throttle_scale, &magic_force);

  float turn_rate = alien_fighter->turning_control.turn_rate;
  real_matrix3x3 current_rotation;
  alien_fighter_matrix3x3_from_forward_and_up(&current_rotation, &object->object.forward, &object->object.up);
  real_matrix3x3 desired_rotation;
  if (running_trick)
    desired_rotation = trick_rotation;
  else {
    desired_rotation.forward = object->unit.facing_vector;
    point_from_line3d(global_up3d, &desired_rotation.forward,
      -desired_rotation.forward.k, &desired_rotation.up);
    if (normalize3d(&desired_rotation.up) == 0.0f)
      desired_rotation.up = *global_forward3d;
  }
  if (unit_is_player_controlled(vehicle_index))
    alien_fighter_update_pitch_vectors(&desired_rotation.forward, &desired_rotation.up,
      alien_fighter_sine(alien_fighter->fixed_gun_pitch), alien_fighter_cosine(alien_fighter->fixed_gun_pitch));
  if (!running_trick || type != 3) {
    float forward_limit = alien_fighter->forward_speed.positive_limit;
    float turn_limit = alien_fighter->turning_control.turn_bounds.upper_bound;
    float forward_fraction = forward_limit < 0.001f ? 0.0f : ALIEN_FIGHTER_ABS(forward) / forward_limit;
    float absolute_turn_fraction = (float)fabs(turn_limit < 0.001f ? 0.0f : turn / turn_limit);
    float vertical_fraction = alien_fighter->vertical_speed.positive_limit < 0.001f ? 0.0f : vertical / alien_fighter->vertical_speed.positive_limit;
    float dead_zone = ALIEN_FIGHTER_ABS(absolute_turn_fraction) * 2.0f;
    float side_speed = cross_product2d(&desired_rotation.forward.xy, &velocity.xy);
    float side_correction = ALIEN_FIGHTER_ABS(side_speed) < 0.001f ? 0.0f
      : side_speed < 0.0f ? MIN(side_speed + dead_zone, 0.0f) - dead_zone
      : MAX(side_speed - dead_zone, 0.0f) + dead_zone;
    float turn_fraction_max = MAX(forward_fraction, absolute_turn_fraction);
    float skid = turn_fraction_max / maximum_forward_speed * side_correction * turn_limit;
    float bank = vertical_fraction * turn_limit * -0.7f;
    float yaw = bank < 0.0f
      ? (skid < 0.0f ? (skid > bank ? bank : skid) : skid + bank)
      : (skid >= 0.0f ? (skid > bank ? skid : bank) : skid + bank);
    alien_fighter_update_yaw_vectors(&desired_rotation.up, &desired_rotation.forward,
      alien_fighter_sine(yaw), alien_fighter_cosine(yaw));
  }
  cross_product3d(&desired_rotation.up, &desired_rotation.forward, &desired_rotation.left);
  matrix3x3_transpose(&current_rotation, &current_rotation);
  real_vector3d angular_acceleration;
  {
  real_matrix3x3 relative_rotation;
  matrix3x3_multiply(&desired_rotation, &current_rotation, &relative_rotation);
  real_quaternion quaternion;
  matrix3x3_rotation_to_quaternion(&relative_rotation, &quaternion);
  float angle;
  real_vector3d axis;
  quaternion_to_angle_and_vector(&quaternion, &angle, &axis);
  real_vector3d desired_angular_velocity;
  scale_vector3d(&axis, angle * turn_rate / 3.1415927f, &desired_angular_velocity);
  angular_acceleration.i = 0.0f;
  subtract_vectors3d(&desired_angular_velocity, &object->object.angular_velocity, &angular_acceleration);
  scale_vector3d(&angular_acceleration, 1.0f / tls_game_time_globals()->tick_length, &angular_acceleration);
  }
  real_vector3d magic_torque;
  {
  real_matrix3x3 inertia_rotation = instance->world_matrix.rotation;
  real_matrix3x3 inertia;
  matrix3x3_multiply(&inertia_rotation, &instance->inertia_tensor, &inertia);
  matrix3x3_multiply(&inertia, matrix3x3_transpose(&inertia_rotation, &inertia_rotation), &inertia);
  matrix3x3_transform_vector(&inertia, &angular_acceleration, &magic_torque);
  }
  scale_vector3d(&magic_torque, alien_fighter->flying_torque_scale * object->unit.throttle_scale, &magic_torque);

  float thrust_deceleration = 0.05f;
  float angular_speed = sqrtf(magnitude_squared3d(&object->object.angular_velocity)) / alien_fighter->turning_control.turn_rate;
  float thrust_adjustment = 0.0f;
  float previous_thrust = thrust;
  if (angular_speed > previous_thrust) {
    thrust_adjustment = PIN((1.0f - thrust) * (1.0f - thrust) * 0.2f, 0.01f, 0.05f);
    thrust_adjustment = angular_speed - thrust > thrust_adjustment ? thrust_adjustment : angular_speed - thrust;
  } else {
    thrust_adjustment = -MAX((thrust * thrust) * thrust_deceleration, 0.005f);
    thrust_adjustment = MAX(angular_speed - thrust, thrust_adjustment);
  }
  previous_thrust += thrust_adjustment;
  thrust = previous_thrust;
  for (long wheel_index = 0; wheel_index < instance->definition->anti_gravity_points.count; wheel_index++)
    instance->anti_gravity_points[wheel_index].strength = object->unit.throttle_scale;
  havok_vehicle_physics_update(vehicle_index, instance, 0.0f, true, &magic_force, &magic_torque, 0);
}
