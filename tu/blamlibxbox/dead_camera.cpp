// blamlibxbox/dead_camera.obj: from the records 0x2976a8 0x2978e0 0x297d28 0x297d40
// @flags /TP /O2 /Oc
// @address 0x761454 ?g_observer_default_field_of_view@@3MA
// @address 0x8c42a0 ?g_camera_globals@@3Us_camera_globals@@A
// @address 0x11df8cc ?g_observer_globals@@3PAUs_player_globals@@A
#include "math/matrix_math_types.h"
#include "math/real_math_types.h"
#include "math/real_math.h"
#include "objects/object_header.h"
#include "objects/object_datum.h"
#include "game/game_globals_storage.h"
#include "objects/objects.h"
#include "camera/observer.h"
#include "memory/data.h"
#include "math/random_math.h"
#include "math/random_math_range.h"
#include "memory/data_inline.h"
#include "units/unit_types.h"
#include "units/unit_seat.h"
#include "units/biped_types.h"
#include "game/player_types.h"
#include "camera/camera_globals.h"
#include "camera/dead_camera.h"

#include "camera/observer_globals_types.h"

// @address 0x587e8 k_dead_camera_random_pitch_offset
// @address 0x587ec k_dead_camera_random_pitch_scale
// @address 0x587f0 k_dead_camera_target_pitch
// @rdata 0x820587e8..0x820587f4
extern "C" {
const float k_dead_camera_random_pitch_offset = 0.47123894f;
const float k_dead_camera_random_pitch_scale = 0.6283184f;
const float k_dead_camera_target_pitch = -0.62831855f;
}
// @rdata-end

// @address 0x297d28 ?handle_deleted_player@c_dead_camera@@UAAXJ@Z
void c_dead_camera::handle_deleted_player(long player_index)
{
	if (m_player_index == player_index)
	{
		m_player_index = -1;
	}
}

// @address 0x297d40 ?handle_deleted_object@c_dead_camera@@UAAXJ@Z
void c_dead_camera::handle_deleted_object(long object_index)
{
	if (m_target == object_index)
	{
		m_target = -1;
	}

	if (m_killer_object_index == object_index)
	{
		m_killer_object_index = -1;
	}
}

// @address 0x2976a8 ?set_target@c_dead_camera@@UAAXJ@Z
void c_dead_camera::set_target(long target)
{
	this->m_field_of_view = g_observer_default_field_of_view;
	this->m_target = target;
	this->m_focus_distance = g_camera_globals.dead_camera_focus_distance;
	this->m_time = 0.0f;
	this->m_falling_to_death = false;
	float pitch;
	if (target != -1)
	{
		const s_object_header *header = object_header_get(target);
		euler_angles2d_from_vector3d(&this->m_angles, &header->datum->forward);
		pitch = k_dead_camera_target_pitch;
	}
	else
	{
		unsigned long *seed = get_local_random_seed_address();
		random_seed_next(seed);
		this->m_angles.yaw = ((float)(double)(*seed >> 16) * 1.5259022e-05f) * 6.2831855f;
		random_seed_next(seed);
		pitch = -((((float)(double)(*seed >> 16) * 1.5259022e-05f) * k_dead_camera_random_pitch_scale) + k_dead_camera_random_pitch_offset);
	}
	this->m_angles.pitch = pitch;
	this->m_killer_object_index = -1;
	long dead_unit_index = this->m_target;
	if (dead_unit_index != -1 && tls_game_globals()->options.game_mode == 2)
	{
		if (((1 << object_header_get(dead_unit_index)->object_type) & _object_mask_unit) != 0)
		{
			const unit_datum *unit = (const unit_datum *)object_get(dead_unit_index);
			long best = 0;
			for (long i = 1; i < 4; i++)
			{
				if (unit->unit.damage_records[i].damage > unit->unit.damage_records[best].damage)
					best = i;
			}
			const s_unit_damage_record *record = &unit->unit.damage_records[best];
			const player_datum *player = DATUM_TRY_AND_GET_CONST(tls_player_data(), player_datum, record->player_index);
			if (player != 0)
			{
				this->m_killer_object_index = player->unit_index;
				return;
			}
			long object_index = record->object_index;
			if (object_try_and_get_and_verify_type(object_index, 0xffffffff) != 0)
				this->m_killer_object_index = object_index;
		}
	}
}
// @address 0x2978e0 ?update@c_dead_camera@@UAAXJMPAUs_observer_command@@@Z
void c_dead_camera::update(long, float dt, s_observer_command *command)
{
	if (this->m_target != -1)
	{
		const s_object_header *header = object_header_get(this->m_target);
		if (header->object_type == _object_type_biped)
		{
			if (!this->m_falling_to_death
				&& flag_test(OBJECT_GET(this->m_target, biped_datum, _object_mask_biped)->biped.flags, (int)_biped_kill_volume_damaged))
			{
				this->m_falling_to_death = true;
				this->m_position = g_observer_globals->blocks[this->m_user_index].camera_result.position;
			}
			if (this->m_falling_to_death)
			{
				command->position = this->m_position;
				object_datum *target = object_get(this->m_target);
				real_matrix4x3 *nodes = (real_matrix4x3 *)object_header_block_get(target, &target->node_matrices);
				vector_from_points3d(&this->m_position, &nodes->position, &command->forward);
				normalize3d_with_default(&command->forward, global_down3d);
				generate_up_vector3d(&command->forward, &command->up);
				command->field_of_view = this->m_field_of_view;
				object_get_velocities(this->m_target, &command->velocity, 0);
				command->flags = 1;
				float timer = g_camera_globals.dead_camera_transition_time - this->m_time;
				if (timer < 0.0f)
					timer = 0.0f;
				command->timer = timer;
			}
			else
			{
				command->focus_distance = this->m_focus_distance;
				object_datum *target = object_get(this->m_target);
				real_matrix4x3 *nodes = (real_matrix4x3 *)object_header_block_get(target, &target->node_matrices);
				command->position = nodes->position;
				command->position.z = command->position.z + g_camera_globals.dead_camera_height;
				bool found = false;
				real_point3d killer_position;
				if (this->m_killer_object_index != -1)
				{
					object_get_origin(this->m_killer_object_index, &killer_position);
					found = true;
				}
				if (found)
				{
					vector_from_points3d(&command->position, &killer_position, &command->forward);
					euler_angles2d_from_vector3d(&this->m_angles, &command->forward);
					float minimum_pitch = -g_camera_globals.dead_camera_maximum_pitch_down;
					this->m_angles.pitch = minimum_pitch > this->m_angles.pitch ? minimum_pitch : this->m_angles.pitch;
				}
				vector3d_from_euler_angles2d(&command->forward, &this->m_angles);
				generate_up_vector3d(&command->forward, &command->up);
				command->focus_distance = this->m_focus_distance;
				if (tls_game_globals()->game_lost)
				{
					float t = this->m_time * 0.16666667f;
					float maximum_distance = g_camera_globals.dead_camera_focus_distance * 5.0f;
					float clamped = t < 1.0f ? t : 1.0f;
					float p = (float)pow((double)clamped, 0.5);
					float next = ((1.0f - p) * 0.9f + p * 0.25f) * dt + this->m_focus_distance;
					this->m_focus_distance = next;
					if (next > maximum_distance)
						next = maximum_distance;
					this->m_focus_distance = next;
				}
				command->field_of_view = this->m_field_of_view;
				// Point view of the shared zero vector; s_observer_command unions velocity the same way.
				union { const real_vector3d *as_vector; const real_point3d *as_point; } zero = { global_zero_vector3d };
				command->focus_position = *zero.as_point;
				command->velocity = *zero.as_vector;
				command->flags = 1;
				float remaining = g_camera_globals.dead_camera_transition_time - this->m_time;
				command->timer = g_camera_globals.dead_camera_minimum_transition_time <= remaining
					? remaining : g_camera_globals.dead_camera_minimum_transition_time;
				command->parameter_timers[0] = 0.0f;
				command->parameter_flags[0] = 3;
				if (this->m_time == 0.0f)
				{
					command->focus_distance = g_camera_globals.dead_camera_initial_focus_distance;
					command->parameter_timers[3] = 0.0f;
					command->parameter_flags[3] = 3;
				}
			}
		}
		long root = -1;
		if (this->m_target != -1)
		{
			long current = this->m_target;
			do
			{
				root = current;
				current = object_get(current)->parent_object_index;
			} while (current != -1);
		}
		object_get_center_of_mass(root, &command->parent_position);
		command->flags = command->flags | 0x400;
	}
	float &time = this->m_time;
	time = time + dt;
	observer_validate_camera_command(command);
}
