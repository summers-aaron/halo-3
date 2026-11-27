// objectslibxbox/creature_ground_motor_program.obj: from the records 0x392218 0x392340 0x3924b8 0x392610 0x3926b0 0x3929a8 0x392c60 0x392d50 0x392f68 0x392ab8
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "objects/object_header.h"
#include "math/real_math_types.h"
#include "physics/character_physics_output.h"
#include "units/action_request.h"
#include "objects/object_datum.h"
#include "motor/action_system.h"
#include "motor/motor_animation.h"
#include "objects/objects.h"
#include "units/unit_dialogue.h"
#include "motor/creature_ground_motor_program.h"
#include "motor/motor_system.h"
#include "creatures/creature_datum.h"
#include "creatures/creature_definitions.h"
#include "cache/cache_files.h"
#include "tag_files/tag_types.h"
#include "motor/motor_system_creature.h"
#include "creatures/creature_datum.h"
#include "units/unit_types.h"
#include "units/motor_program_state.h"
#include "ai/swarms.h"
#include "tag_files/tag_groups.h"
#include "physics/character_physics_definition_types.h"
#include "animations/animation_control_data.h"
#include "animations/animation_manager.h"
#include "animations/animation_interpolation.h"
#include "animations/mixing_board/channels/animation_channel.h"
#include "animations/animation_state_change_settings.h"
#include "physics/character_physics_move_output.h"
#include <string.h>
#include "physics/character_physics_component.h"
#include "units/motor_task.h"
#include "units/motor_request.h"
#include "math/real_math_globals.h"
#include "units/biped_definitions.h"
#include <new>
#include "physics/character_physics_input.h"
#include "game/game_time_inline.h"
struct unit_dialogue_information_packet;
struct ai_information_packet;
// @address 0x392218 ?infect_task_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
// @address 0x3924b8 ?infect_task_interrupted@@YA_NJPAVc_motor_task@@@Z
// @address 0x3929a8 ?creature_update_movement_desires@@YAXJPAUs_animation_control_data@@W4e_animation_state@s_character_physics_update_output_datum@@@Z
// @address 0x392c60 ?creature_move_update_jumping@@YA_NJ@Z
// @address 0x392f68 ?creature_ground_in_airborne_state@@YA_NJ@Z

// The creature motor block (creature_datum::creature.motor_program): the motor program state, then the ground program's own
// fields.
struct s_creature_ground_motor_state
{
  c_motor_program_state program_state;
  bool update_movement_desires;                   // creature_ground_update sets it when the animation may pick the movement state
  unsigned char airborne_ticks;
  unsigned char grounded_ticks;
};


static __forceinline creature_datum *creature_ground_motor_program_get(long object_index)
{
  return OBJECT_GET(object_index, creature_datum, _object_mask_creature);
}

static __forceinline s_creature_ground_motor_state *creature_ground_motor_program_motor_state(creature_datum *creature)
{
  return (s_creature_ground_motor_state *)object_header_block_get(creature, &creature->creature.motor_program);
}

static __forceinline bool creature_ground_motor_program_airborne(const creature_datum *creature)
{
  return (creature->creature.flags >> 5) & 1;
}

// 0x392218
e_motor_result infect_task_submit(long object_index, const c_motor_request *motor_request)
{
  e_motor_result result = _motor_result_failed;
bool force = motor_request->force();
c_motor_task *motor_task = motor_task_set_mode_and_state(object_index, (e_motor_task)motor_request->m_task, (e_action)motor_request->action,
    226, 0x255, 0.1f, 15, 0, 29, force);
  if (motor_task != 0)
  {
    object_datum *object = object_get(object_index);
    c_animation_manager *animation = (c_animation_manager *)object_header_block_get(object, &object->animation);
    animation->animation_interpolator()->disable();
    object_wake(object_index);
    object_disconnect_from_physics(object_index);
    long animation_object_index = motor_request->m_infect.target_object_index;
    if (animation_object_index != -1)
    {
      real_point3d origin;
      real_vector3d forward;
      real_vector3d up;
      object_get_origin(animation_object_index, &origin);
      ::object_get_orientation(animation_object_index, &forward, &up);
      ::object_set_position_direct(object_index, &origin, &forward,
          &up, 0, false);
      object_set_velocities(object_index, global_zero_vector3d, global_zero_vector3d);
    }
    motor_task->object_index = motor_request->m_infect.target_object_index;
    motor_task->duration = motor_request->m_infect.ticks;
    result = _motor_result_success;
  }
  return result;
}

static __forceinline void creature_ground_motor_program_infect_request(action_request *request, const biped_definition *definition)
{
  memset(request, 0, sizeof(*request));
  request->action = 4;
  request->initial_animation.animation = definition->reanimation_character.index;
  request->initial_animation.animation_index = definition->reanimation_morph_muffins.index;
  request->initial_animation.flag_a = false;
  request->initial_animation.flag_b = false;
  request->initial_animation.animation_mode = -1;
  request->initial_animation.flag_c = true;
}
// @address 0x392340 ?infect_task_update@@YA_NJPAVc_motor_task@@@Z
bool infect_task_update(long object_index, c_motor_task *task)
{
  long target_index = task->object_index;
  unit_datum *target = OBJECT_TRY_AND_GET(target_index, unit_datum, _object_mask_biped);
  bool result = false;
  if (target && action_executing(target_index, _action_get_infected))
  {
    --task->ticks_remaining;
    if ((float)task->ticks_remaining > 0.0f)
      result = true;
    else
    {
      biped_definition *definition =
          TAG_GET('bipd', biped_definition, target->object.definition_index);
      if (definition->reanimation_character.index != -1)
      {
        action_request request;
        creature_ground_motor_program_infect_request(&request, definition);
        if (action_submit(target_index, &request))
        {
          if (target->unit.actor_index != -1)
            ::unit_scream(task->object_index, 7, -1, 13, 0, 0);
        }
        else
          object_delete(task->object_index);
      }
      else
        object_delete(target_index);
    }
  }
  if (!result)
    object_delete(object_index);
  return result;
}

// 0x3924b8
bool infect_task_interrupted(long object_index, c_motor_task *motor_task)
{
  creature_datum *creature = creature_ground_motor_program_get(object_index);
  if (creature->object.parent_object_index == -1)
    ::object_reconnect_to_physics(object_index);
  long task_object_index = motor_task->object_index;
  unit_datum *target = OBJECT_TRY_AND_GET(task_object_index, unit_datum, _object_mask_biped);
  if (target == 0 || !action_executing(task_object_index, _action_get_infected))
    return true;
  biped_definition *definition =
      TAG_GET('bipd', biped_definition, target->object.definition_index);
  if (definition->reanimation_character.index == -1)
    return true;
  motor_task_interrupt(task_object_index, 15, 0);
  c_motor_request request;
  request.set_force(false);
  request.action = _action_none;
  request.m_weapon_draw.alternate = true;
  request.m_weapon_draw.instant = true;
  request.m_valid = true;
  request.m_task = _motor_task_weapon_draw;
  motor_system_submit(motor_task->object_index, &request);
  action_request submit;
  memset(&submit, 0, sizeof(submit));
  submit.action = 7;
  submit.motor.has_vector = true;
  submit.motor.vector.i = target->object.forward.i * -1.0f;
  submit.motor.vector.j = target->object.forward.j * -1.0f;
  submit.motor.vector.k = target->object.forward.k * -1.0f;
  submit.motor.flag = true;
  submit.motor.mode = 2;
  action_submit(motor_task->object_index, &submit);
  return true;
}

// 0x3929a8
void creature_update_movement_desires(long object_index, s_animation_control_data *animation_control_data,
    s_character_physics_update_output_datum::e_animation_state animation_state)
{
  creature_datum *creature = creature_ground_motor_program_get(object_index);
  if (!creature_ground_motor_program_motor_state(creature)->update_movement_desires)
    return;
  bool handled = false;
  if (creature_ground_motor_program_airborne(creature) &&
      (creature->creature.physics.get_mode() == c_character_physics_component::_mode_ground ||
          creature->creature.physics.get_mode() == c_character_physics_component::_mode_dead) &&
      creature->object.parent_object_index == -1)
  {
    if (creature->object.linear_velocity.k > 0.0f)
      animation_control_data->state = 77;
    else
      animation_control_data->state = 31;
    handled = true;
  }
  if (!handled)
  {
    switch (animation_state)
    {
    case 1: animation_control_data->state = 12; break;
    case 2: animation_control_data->state = 24; break;
    case 3: animation_control_data->state = 23; break;
    case 4: animation_control_data->state = 21; break;
    case 5: animation_control_data->state = 22; break;
    }
  }
}

bool creature_ground_in_airborne_state(long object_index)
{
  bool result = (creature_ground_motor_program_get(object_index)->creature.flags >> 5) & 1;
  return result;
}

static __forceinline bool creature_ground_motor_program_swarm_creature_aim_jump(long object_index, real_vector3d *out)
{
  bool result = false;
  creature_datum *creature = creature_ground_motor_program_get(object_index);
  swarm_creature_state *state;
  if (creature->creature.link_type == 0)
    state = (swarm_creature_state *)object_header_block_get(creature, &creature->creature.creature_reference);
  else
    state = 0;
  if (state != 0 && state->jump_requested)
  {
    state->jump_requested = false;
    result = true;
    *out = state->jump_velocity;
  }
  return result;
}

static __forceinline bool creature_ground_motor_program_ai_creature_aim_jump(long object_index, real_vector3d *out)
{
  bool result = false;
  creature_datum *creature = creature_ground_motor_program_get(object_index);
  long link_type = creature->creature.link_type;
  if (link_type == 0)
    result = creature_ground_motor_program_swarm_creature_aim_jump(object_index, out);
  return result;
}

// 0x392c60
bool creature_move_update_jumping(long object_index)
{
  bool result = false;
  creature_datum *creature = creature_ground_motor_program_get(object_index);
  real_vector3d velocity;
  if ((creature->creature.control.control_flags & 16) != 0 && !creature_ground_motor_program_airborne(creature) &&
      creature->creature.link_type != -1 &&
      (unsigned char)creature_ground_motor_program_ai_creature_aim_jump(object_index, &velocity) != 0)
  {
    object_set_velocities(object_index, &velocity, 0);
    result = true;
  }
  return result;
}

static __forceinline void creature_ground_motor_program_set_airborne(unsigned short &flags, bool airborne = true)
{
  if (airborne)
    flags |= 1 << 5;
  else
    flags &= ~(1 << 5);
}
// @address 0x392610 ?creature_ground_activate@@YA_NJ@Z
bool creature_ground_activate(long object_index)
{
  creature_datum *creature = creature_ground_motor_program_get(object_index);
  s_creature_ground_motor_state *motor_state = creature_ground_motor_program_motor_state(creature);
  creature_ground_motor_program_set_airborne(creature->creature.flags);
  creature->creature.physics.set_mode(c_character_physics_component::_mode_ground);
  motor_task_set_mode_and_state(object_index, _motor_task_default, _action_none, 1, 1, 0.0f, 1, 2, 31, false);
  motor_state->update_movement_desires = false;
  motor_state->airborne_ticks = 0;
  motor_state->grounded_ticks = 0;
  return true;
}

// @address 0x2b10c k_creature_ground_airborne_delay
static __forceinline float creature_ground_motor_program_airborne_duration(const creature_definition *definition)
{
  bool longer_duration = (definition->physics.flags >> 3) & 1;
#if defined(_XBOX) && !defined(__clang__)
  // match: 0x392d50: the 2.0f scale (-0x6e34) and 0.15f delay load off one 0x8202b10c base; spelled literals reschedule both duration tests
  const float *constants = k_creature_ground_airborne_delay;
  return (longer_duration ? constants[-7053] : 1.0f) * constants[0];
#else
  return (longer_duration ? 2.0f : 1.0f) * 0.15f;
#endif
}
static __forceinline float creature_ground_motor_program_ticks_to_seconds(float ticks)
{
  return tls_game_time_globals()->tick_length * ticks;
}
static __forceinline float creature_ground_motor_program_dot(const real_vector3d *a, const real_vector3d *b)
{
  return a->i * b->i + a->j * b->j + a->k * b->k;
}
// @address 0x3926b0 ?creature_update_turning@@YAXJ@Z
void creature_update_turning(long object_index)
{
  creature_datum *creature = creature_ground_motor_program_get(object_index);
  creature_definition *definition =
      TAG_GET('crea', creature_definition, creature->object.definition_index);
  if (!motor_animation_resource_busy(object_index, _motor_resource_base))
  {
    float velocity = definition->turning_velocity_maximum;
    float acceleration = definition->turning_acceleration_maximum;
    if (creature->creature.control.turning_speed == 1) {
      velocity *= definition->casual_turning_modifier;
      acceleration *= definition->casual_turning_modifier;
    }
    real_vector3d facing;
    real_vector3d angular_velocity;
    if (velocity == 0.0f && acceleration == 0.0f) {
      facing = creature->creature.control.forward;
      angular_velocity = *global_zero_vector3d;
    } else {
      object_get_orientation(object_index, &facing, 0);
      angular_velocity = creature->creature.turning_velocity;
      angular_accelerate_to_position(
          &facing, &creature->creature.control.forward, game_tick_length(), &angular_velocity, velocity, acceleration);
    }
    real_vector3d forward, left, up;
    {
      real_vector3d up_velocity = *global_zero_vector3d;
      real_vector3d desired_up;
      object_get_orientation(object_index, 0, &up);
      {
        real_vector3d desired_left;
        real_vector3d_build_axes_from_vectors(&facing, &creature->creature.control.up, &forward, &desired_left, &desired_up);
      }
      {
        real_vector3d up_forward;
        real_vector3d_build_axes_from_vectors(&facing, &up, &up_forward, &left, &up);
        if (angle_between_vectors3d(&desired_up, &up) > game_tick_length() * velocity) {
          angular_accelerate_to_position(&up, &desired_up, game_tick_length(), &up_velocity, velocity, acceleration * 2.0f);
          real_vector3d_build_axes_from_vectors(&facing, &up, &up_forward, &left, &up);
        }
        else
          up = desired_up;
      }
    }
    {
      real_point3d position = creature->object.position;
      object_type_fix_transform(object_index, &position, &forward, &up);
    }
    float up_dot = creature_ground_motor_program_dot(&up, &creature->object.up);
    float forward_dot = creature_ground_motor_program_dot(&forward, &creature->object.forward);
    if (up_dot < 0.9999f || forward_dot < 0.9999f) {
      creature->object.up = up;
      creature->object.forward = forward;
      object_set_requires_motion(object_index);
    }
    creature->creature.turning_velocity = angular_velocity;
  }
}

// @address 0x392d50 ?creature_ground_move@@YA?AW4e_motor_program@@JPAVc_motor_request@@@Z
e_motor_program creature_ground_move(long object_index, c_motor_request *request)
{
  creature_datum *creature = creature_ground_motor_program_get(object_index);
  s_character_physics_move_output_datum output;
  bool moved = motor_system_creature_move(object_index, &output);
  if (creature->object.havok_component_index != -1)
  {
    s_creature_ground_motor_state *motor_state = creature_ground_motor_program_motor_state(creature);
    creature_definition *definition =
        TAG_GET('crea', creature_definition, creature->object.definition_index);
    bool jumping = creature_move_update_jumping(object_index);
    if (moved)
    {
      if (creature_ground_motor_program_airborne(creature))
      {
        if (!(output.flags & 1))
        {
          float duration = creature_ground_motor_program_airborne_duration(definition);
          if (creature_ground_motor_program_ticks_to_seconds(motor_state->airborne_ticks) > duration)
            creature_ground_motor_program_set_airborne(creature->creature.flags, false);
        }
      }
      else if (jumping)
        creature_ground_motor_program_set_airborne(creature->creature.flags, true);
      else if (output.flags & 1)
      {
        float duration = creature_ground_motor_program_airborne_duration(definition);
        motor_state->grounded_ticks = (unsigned char)MIN(motor_state->grounded_ticks + 1, 254);
        if (creature_ground_motor_program_ticks_to_seconds(motor_state->grounded_ticks) > duration)
          creature_ground_motor_program_set_airborne(creature->creature.flags, true);
      }
      else
        motor_state->grounded_ticks = 0;
      if (creature_ground_motor_program_airborne(creature))
      {
        motor_state->airborne_ticks = (unsigned char)MIN(motor_state->airborne_ticks + 1, 254);
        motor_state->grounded_ticks = 0;
      }
      else
        motor_state->airborne_ticks = 0;
    }
  }
  return _motor_program_none;
}



static __forceinline bool creature_ground_update_control(long object_index, s_animation_control_data *animation_control)
{
  creature_datum *creature = creature_ground_motor_program_get(object_index);
  c_animation_manager *animation = (c_animation_manager *)object_header_block_get(creature, &creature->object.animation);
  s_creature_ground_motor_state *motor_state = creature_ground_motor_program_motor_state(creature);
  bool result = false;
  animation_control->mode = 226;
  animation_control->state = 226;
  motor_state->update_movement_desires = false;
  bool dead = (creature->object.damage_flags >> 2) & 1;
  if (!dead)
  {
    animation_control->mode = creature->creature.control.action;
    animation_control->state = creature->creature.control.animation;
    if (creature->creature.control.control_flags & 1)
    {
      creature->creature.flags |= 2;
    }
    else
    {
      bool marker_active = (creature->creature.flags >> 1) & 1;
      if (marker_active && (creature->creature.control.control_flags & 4) == 0)
        animation_control->state = animation->get_state_name();
      else
        motor_state->update_movement_desires = true;
    }
    result = true;
  }
  return result;
}

// @address 0x392ab8 ?creature_ground_update@@YA_NJPAUs_animation_control_data@@@Z
bool creature_ground_update(long object_index, s_animation_control_data *animation_control_data)
{
  s_animation_control_data control;
  control.set_defaults();
  animation_control_data = &control;
  bool result = false;
  creature_datum *creature = creature_ground_motor_program_get(object_index);
  if (creature->object.havok_component_index != -1)
  {
    result = result | creature_ground_update_control(object_index, animation_control_data);
    if (creature->object.parent_object_index == -1)
      creature_update_turning(object_index);
    s_character_physics_update_input_datum input;
    s_character_physics_update_output_datum output;
    if ((unsigned char)motor_system_creature_physics_update(object_index, &input, &output) != 0)
      creature_update_movement_desires(object_index, &control, output.animation_state);
  }
  {
    s_animation_state_change_settings settings;
    s_state_changes_requested requested;
    motor_animation_build_state_change_settings_from_control_data(object_index, &control, &requested, &settings);
    result = (0 != motor_task_execute_state_change_settings(object_index, _motor_task_default, _action_none, &settings, 1, control.moved)) | result;
  }
  return result;
}
