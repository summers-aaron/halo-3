// objectslibxbox/creature_flying_motor_program.obj: from the records 0x392f98 0x393038 0x3932c8
// @flags /TP /O2 /Oc
#include "units/motor_enums.h"
#include "animations/animation_state_change_settings.h"
#include "physics/character_physics_output.h"
#include "physics/character_physics_input.h"
#include <new>
#include "objects/object_header.h"
#include "tag_files/tag_groups.h"
#include "motor/motor_animation.h"
#include "physics/character_physics_utilities.h"
#include "physics/character_physics_component.h"

#include "units/motor_request.h"
#include "creatures/creature_datum.h"
#include "creatures/creature_definitions.h"
#include "physics/character_physics_mode_flying.h"
#include "animations/animation_manager.h"
#include "animations/animation_control_data.h"
#include "motor/motor_system_creature.h"
#include "physics/character_physics_definition_types.h"
#include "physics/character_physics_mode_dead.h"
#include "physics/character_physics_mode_ground_datum.h"





static __forceinline creature_datum *creature_flying_motor_program_get(long object_index)
{
	return OBJECT_GET(object_index, creature_datum, _object_mask_creature);
}

// @address 0x392f98 ?creature_flying_activate@@YA_NJ@Z
bool creature_flying_activate(long object_index)
{
	creature_datum *creature = creature_flying_motor_program_get(object_index);
	creature->creature.physics.set_mode(c_character_physics_component::_mode_flying);
	motor_task_set_mode_and_state(object_index, _motor_task_default, _action_none, 1, 1, 0.0f, 1, 2, 31, false);
	creature->creature.flags |= 1 << 5;
	return true;
}

// @address 0x3932c8 ?creature_flying_move@@YA?AW4e_motor_program@@JPAVc_motor_request@@@Z
e_motor_program creature_flying_move(long object_index, c_motor_request *request)
{
	int move_output[6];

	motor_system_creature_move(object_index, (s_character_physics_move_output_datum *)move_output);
	return _motor_program_none;
}



// @address 0x393038 ?creature_flying_update@@YA_NJPAUs_animation_control_data@@@Z
bool creature_flying_update(long object_index, s_animation_control_data *animation_control_data)
{
	s_animation_control_data control;
	control.set_defaults();
	bool result = false;
	creature_datum *creature = creature_flying_motor_program_get(object_index);
	creature_definition *definition = TAG_GET('crea', creature_definition, creature->object.definition_index);
	bool dead = (creature->object.damage_flags >> 2) & 1;
	if (dead && creature->creature.physics.m_mode != c_character_physics_component::_mode_dead)
		creature->creature.physics.set_mode(c_character_physics_component::_mode_dead);
	if (creature->object.havok_component_index != NONE)
	{
		control.mode = 226;
		control.state = 226;
		creature_datum *object = creature_flying_motor_program_get(object_index);
		bool control_valid = false;
		bool object_dead = (object->object.damage_flags >> 2) & 1;
		c_animation_manager *animation = (c_animation_manager *)object_header_block_get(object, &object->object.animation);
		if (!object_dead)
		{
			unsigned short flags = object->creature.control.flags;
			control.mode = object->creature.control.action;
			control.state = object->creature.control.animation;
			if (flags & 1)
			{
				object->creature.flags |= 2;
			}
			else
			{
				bool marker_active = (object->creature.flags >> 1) & 1;
				if (marker_active && (flags & 4) == 0)
					control.state = animation->m_current_goal.state;
			}
			control_valid = true;
		}
		result = control_valid;
		if (creature->object.parent_object_index == NONE && !dead)
		{
			character_physics_flying_update_turning(object_index, &definition->physics.flying, &creature->creature.control.facing,
				&creature->creature.control.movement, 0.9f, &creature->creature.turning_amount);
		}
		s_character_physics_update_input_datum input;
		s_character_physics_update_output_datum output;
		if (motor_system_creature_physics_update(object_index, &input, &output))
		{
			c_character_physics_component *physics = &creature->creature.physics;
			creature->object.forward = output.forward;
			creature->object.up = output.up;
			switch (physics->m_mode)
			{
			case c_character_physics_component::_mode_ground:
				physics->get_mode_ground()->snap_facing(&creature->object.forward, &creature->object.up, &input);
				break;
			case c_character_physics_component::_mode_flying:
				physics->get_mode_flying()->snap_facing(&creature->object.forward, &creature->object.up, &input);
				break;
			case c_character_physics_component::_mode_dead:
				physics->get_mode_dead()->snap_facing(&creature->object.forward, &creature->object.up, &input);
				break;
			case c_character_physics_component::_mode_sentinel:
			case c_character_physics_component::_mode_sentinel_b:
			case c_character_physics_component::_mode_melee:
				break;
			default:
				// match: 0x393038: unreachable hint omits dead branch check
				__assume(0);
			}
		}
	}
	{
		s_animation_state_change_settings settings;
		s_state_changes_requested requested;
		settings.flags.clear();
		requested.goals.clear();
		requested.transition.clear();
		requested.primary.clear();
		motor_animation_build_state_change_settings_from_control_data(object_index, &control, &requested, &settings);
		result = (0 != motor_task_execute_state_change_settings(object_index, _motor_task_default, _action_none, &settings, 1, control.moved)) | result;
	}
	return result;
}
