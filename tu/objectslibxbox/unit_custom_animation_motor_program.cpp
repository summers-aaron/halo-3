// objectslibxbox/unit_custom_animation_motor_program.obj: from the records 0x390f60 0x3911f8 0x391248 0x391410 0x391488 0x391540 0x391e00 0x391ee0 0x3915b0
// @flags /TP /O2 /Oc
// @address 0x391410 ?unit_custom_animation_stop_animation_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
#include "physics/character_physics_input.h"
#include "animations/animation_control_data.h"
#include "math/matrix_math_types.h"
#include "math/real_math.h"
#include "tag_files/tag_types.h"
#include "animations/animation_id.h"
#include "objects/object_header.h"
#include "units/motor_enums.h"
#include "units/motor_request.h"
#include "units/motor_program_state.h"
#include "physics/character_physics_output.h"
#include "physics/character_physics_move_output.h"
#include <math.h>
#include <string.h>
#include "motor/motor_animation.h"
#include "objects/objects.h"
#include "motor/motor_system_biped.h"
#include "physics/havok.h"
#include "units/bipeds.h"
#include "units/giants.h"
#include "units/unit_animation.h"
#include "motor/motor_programs.h"
#include "motor/motor_system.h"
#include "cache/cache_files.h"
#include "animations/mixing_board/channels/animation_channel.h"
#include "objects/object_reference_types.h"
#include "units/motor_task.h"
#include "memory/datum_index.h"
#include "units/unit_types.h"
#include "units/biped_types.h"
#include "animations/animation_manager.h"
#include "items/weapon_definitions.h"
#include "objects/object_definitions.h"
#include "models/render_model_definitions.h"
#include "objects/object_runtime_tls.h"
#include "objects/object_runtime_globals_types.h"
#include "models/model_types.h"
#include "motor/mover_inline.h"
#include "units/unit_animation_storage_types.h"
#include "physics/collision_types.h"
#include "animations/animation_definition_types.h"
#include "animations/animation_graph_node_types.h"
#include "animations/animation_search_path_iterator.h"
#include "physics/collision_vector_query.h"

struct unit_custom_animation_state
{
    c_motor_program_state motor;
    unsigned char interrupted;
    unsigned char blend_in;
    short interrupted_ticks;
    real_vector3d velocity;
};
typedef char custom_animation_layout_1[(sizeof(unit_custom_animation_state) == 0x18) ? 1 : -1];
typedef char custom_animation_layout_2[(offsetof(unit_custom_animation_state, interrupted) == 8) ? 1 : -1];
typedef char custom_animation_layout_3[(offsetof(unit_custom_animation_state, velocity) == 0x0c) ? 1 : -1];

typedef c_channel_base unit_custom_animation_channel;
typedef s_unit_animation_storage unit_custom_animation_flags_block;

static __forceinline bool unit_custom_animation_motor_program_mover_set_motor_program(long index,e_motor_program program) {
 unit_datum *datum = OBJECT_GET(index, unit_datum, _object_mask_unit | _object_mask_creature);
 c_motor_program_state *state=(c_motor_program_state *)object_header_block_get(object_get(index),&datum->unit.motor_program);
 bool result=false;
 if(state->program!=program)
  result=state->set_program(index,program);
 return result;
}

static __forceinline unsigned char unit_custom_animation_object_type_get(long index) {return object_header_get(index)->object_type;}

// @address 0x390f60 ?unit_custom_animation_play_animation_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
e_motor_result unit_custom_animation_play_animation_submit(long object_index, const c_motor_request *request)
{
 long animation_index = request->m_custom_animation.graph_tag_index;
 long animation_name = request->m_custom_animation.animation_name;
 bool goal_flag = request->m_custom_animation.goal;
 bool interpolate = request->m_custom_animation.interpolate;
 long land_index = request->m_custom_animation.object_index;
 e_motor_result result = _motor_result_failed;
 if (object_index != -1 && animation_index != -1)
 {
  unit_datum *datum = OBJECT_GET(object_index, unit_datum, _object_mask_all);
  if (land_index != -1 && datum->unit.parent_seat_index != -1)
   motor_animation_exit_seat_reset_velocity(object_index, 0, 0, false, 0, 0);
  bool clear_custom;
  if (request->m_custom_animation.clear_custom || tls_object_runtime_globals()->custom_animations_hold_on_last_frame)
   clear_custom = true;
  else
   clear_custom = false;
  if (motor_task_start_custom_animation(object_index, _motor_task_custom_animation, request->action, animation_index,
      animation_name, goal_flag, clear_custom) != 0)
  {
   unit_custom_animation_state *motor_state = (unit_custom_animation_state *)((char *)datum + datum->unit.motor_program.offset);
   unit_custom_animation_flags_block *anim_task = (unit_custom_animation_flags_block *)((char *)datum + datum->unit.animation_storage.offset);
   motor_state->blend_in = request->m_custom_animation.blend_in;
   motor_state->velocity = *global_zero_vector3d;
   object_set_always_active(object_index, true);
   if (interpolate != 0)
   {
    object_start_interpolation(object_index, 0.26666668f);
   }
   else
   {
    unit_datum *object = OBJECT_GET(object_index, unit_datum, _object_mask_all);
    c_animation_manager *manager = (c_animation_manager *)((char *)object + object->object.animation.offset);
    manager->m_interpolator.m_time = 0.0f;
    manager->m_interpolator.m_flags = 0;
    manager->m_interpolator.m_duration = 0.0f;
    object_wake(object_index);
   }
   unsigned short flags = anim_task->flags;
   anim_task->custom_animation_object_index = land_index;
   anim_task->flags = (unsigned short)(flags | 17);
   motor_state->interrupted = 0;
   if (land_index != -1)
   {
    if (datum->object.parent_object_index != -1)
     object_detach(object_index);
    if (((1 << unit_custom_animation_object_type_get(object_index)) & 1) != 0)
     biped_enter_default_physics(object_index);
    bool disconnect = ((datum->object.physics_flags >> 7) & 1) != 0;
    if (disconnect)
     object_disconnect_from_physics(object_index);
    object_set_position_direct(object_index, global_origin3d, global_forward3d, global_up3d, 0, false);
    object_set_velocities(object_index, global_zero_vector3d, global_zero_vector3d);
    object_wake(object_index);
    object_set_in_limbo(object_index, false);
    object_activate(object_index);
   }
   else if (request->m_custom_animation.use_position != 0 && unit_custom_animation_object_type_get(object_index) == 0)
   {
    biped_enter_posture_physics(object_index, &request->m_custom_animation.position, &request->m_custom_animation.alignment);
   }
   object_set_at_rest(object_index, true);
   object_compute_node_matrices(object_index);
   result = _motor_result_success;
  }
 }
 return result;
}

// @address 0x3911f8 ?unit_custom_animation_play_animation_interrupted@@YA_NJPAVc_motor_task@@@Z
bool unit_custom_animation_play_animation_interrupted(long index,c_motor_task *task) {
 if(mover_get_motor_program(index)==11) {
  unit_datum *datum = OBJECT_GET(index, unit_datum, _object_mask_unit);
  unit_custom_animation_state *state=(unit_custom_animation_state *)object_header_block_get(object_get(index),&datum->unit.motor_program);
  state->interrupted=true;
 }
 return true;
}

static __forceinline void unit_custom_animation_motor_program_set_animation_flag(unsigned short *flags, unsigned short bit)
{
    *flags = (unsigned short)(*flags | bit);
}

// @address 0x391248 unit_custom_animation_play_animation_finished
extern "C" bool unit_custom_animation_play_animation_finished(long object_index, c_motor_task *task)
{
 unit_datum *datum = OBJECT_GET(object_index, unit_datum, _object_mask_all);
 unit_custom_animation_state *state = (unit_custom_animation_state *)((char *)datum + datum->unit.motor_program.offset);
 c_animation_manager *manager = (c_animation_manager *)((char *)datum + datum->object.animation.offset);
 unit_custom_animation_channel *channel = (unit_custom_animation_channel *)motor_animation_task_get_channel(object_index, task);
 bool result = true;
 bool interrupted = false;
 if ((unsigned char)((channel->m_playback_flags.get_unsafe() >> 14) & 1) != 0)
 {
  if (state->blend_in != 0)
  {
   interrupted = true;
   manager->m_flags |= 1;
   if (unit_custom_animation_object_type_get(object_index) == 0)
   {
    const real_vector3d *zero_vector = global_zero_vector3d;
    unit_datum *object = OBJECT_GET(object_index, unit_datum, _object_mask_all);
    unit_custom_animation_motor_program_set_animation_flag(&((biped_datum *)object)->biped.flags, 0x800);
    unit_custom_animation_motor_program_set_animation_flag(&((biped_datum *)object)->biped.flags, 0x1000);
    state->velocity = *zero_vector;
    biped_get_camera_velocities(object_index, &state->velocity, 0);
    limit3d(&state->velocity, 3.0f);
   }
   if (object_index != -1)
   {
    unit_datum *object = OBJECT_GET(object_index, unit_datum, _object_mask_all);
    object->object.damage_flags |= 0x80;
    object_wake(object_index);
   }
   unit_datum *object = OBJECT_GET(object_index, unit_datum, _object_mask_all);
   c_animation_manager *reset = (c_animation_manager *)((char *)object + object->object.animation.offset);
   reset->m_interpolator.m_time = 0.0f;
   reset->m_interpolator.m_duration = 0.0f;
   reset->m_interpolator.m_flags = 0;
   object_wake(object_index);
  }
  else if ((unsigned char)((channel->m_playback_flags.get_unsafe() >> 1) & 1) != 0)
  {
   result = false;
  }
  else
  {
   unit_custom_animation_flags_block *block = (unit_custom_animation_flags_block *)((char *)datum + (short)datum->unit.animation_storage.offset);
   bool block_active = (unsigned char)((block->flags >> 10) & 1) != 0;
   if (block_active)
   {
    manager->m_flags |= 1;
    interrupted = true;
   }
   else
   {
    manager->m_flags |= 1;
   }
  }
 }
 if (interrupted)
  state->interrupted = 1;
 return result;
}

// 0x391410
e_motor_result unit_custom_animation_stop_animation_submit(long object_index,const c_motor_request *request)
{
 e_motor_program program=mover_default_program(object_index);
 e_motor_result result=_motor_result_failed;
 if(program!=_motor_program_none) {
  unit_custom_animation_motor_program_mover_set_motor_program(object_index,program);
  result=_motor_result_pending;
 }
 return result;
}

// @address 0x391488 ?unit_custom_animation_death_ping_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
e_motor_result unit_custom_animation_death_ping_submit(long object_index, const c_motor_request *request)
{
 e_motor_result result = _motor_result_failed;
 if (unit_custom_animation_object_type_get(object_index) == 0) {
  if (request->m_ping.type >= 3) {
   if (motor_task_allow(object_index, request->action, 15)) {
    unit_datum *datum = OBJECT_GET(object_index, unit_datum, _object_mask_unit);
    c_motor_program_state *state = (c_motor_program_state *)object_header_block_get(object_get(object_index), &datum->unit.motor_program);
    e_motor_program program = mover_default_program(object_index);
    motor_task_interrupt(object_index, 15, 0);
    state->set_program(object_index, program);
    motor_system_submit(object_index, request);
   }
  }
 }
 return result;
}

// @address 0x391540 ?unit_custom_animation_activate@@YA_NJ@Z
bool unit_custom_animation_activate(long object_index)
{
 unit_datum *datum = OBJECT_GET(object_index, unit_datum, _object_mask_unit | _object_mask_creature);
 unit_custom_animation_state *state = (unit_custom_animation_state *)object_header_block_get(object_get(object_index), &datum->unit.motor_program);
 unsigned short flags = state->motor.state;
 state->blend_in = 0;
 state->interrupted = 0;
 state->interrupted_ticks = 0;
 state->motor.state = flags | 4;
 // match: 0x391540: value range assumption guides branch lowering
 __assume((flags | 4) != 0);
 ::havok_object_rebuild(object_index, 0);
 return true;
}

// @address 0x391ee0 ?unit_custom_animation_move@@YA?AW4e_motor_program@@JPAVc_motor_request@@@Z
e_motor_program unit_custom_animation_move(long object_index, c_motor_request *request)
{
 e_motor_program program = _motor_program_none;
 unit_datum *datum = OBJECT_GET(object_index, unit_datum, _object_mask_unit);
 unit_custom_animation_state *state = (unit_custom_animation_state *)object_header_block_get(object_get(object_index), &datum->unit.motor_program);
 if (datum->object.object_type == 0) {
  s_character_physics_move_output_datum output;
  motor_system_biped_move(object_index, false, false, false, false, &output);
 }
 if (state->interrupted) {
  state->interrupted_ticks++;
  if (state->interrupted_ticks > 1)
   program = mover_default_program(object_index);
 }
 return program;
}

// @address 0x391e00 ?unit_custom_animation_update@@YA_NJPAUs_animation_control_data@@@Z
bool unit_custom_animation_update(long object_index, s_animation_control_data *animation_control_data)
{
    unit_datum *datum = OBJECT_GET(object_index, unit_datum, _object_mask_unit);
    if (unit_custom_animation_object_type_get(object_index) == 0 && datum->object.havok_component_index != -1 && datum->object.parent_object_index == -1)
    {
        s_character_physics_update_input_datum input;
        s_character_physics_update_output_datum output;
        if (motor_system_biped_physics_preupdate_moving(object_index, &input, &output, false, false, 0.0f, false, false))
        {
            bool result = false;
            motor_system_biped_physics_postupdate_moving(object_index, &input, &output, &result);
        }
    }
    if (unit_custom_animation_object_type_get(object_index) == 12)
        giant_handle_motor_update(object_index, false);
    object_set_requires_motion(object_index);
    return true;
}

static __forceinline void *unit_custom_animation_tag_get(long tag_index)
{
    return global_tag_instances[datum_index_to_absolute_index(tag_index)].definition;
}

__forceinline long c_animation_manager::find_node(long name) const
{
    const c_model_animation_graph *graph = (const c_model_animation_graph *)unit_custom_animation_tag_get(m_graph_tag_index);
    for (long node_index = 0; node_index < graph->m_nodes.count; node_index++)
    {
        if (((const animation_graph_node *)graph->m_nodes.address)[node_index].name == name)
            return node_index;
    }
    return -1;
}

// @rdata 0x8202490c
static const float unit_custom_animation_epsilon = 0.0001f;

static __forceinline void unit_custom_animation_restore_graph(long object_index)
{
    unit_datum *datum = OBJECT_GET(object_index, unit_datum, _object_mask_unit);
    c_animation_manager *animation = (c_animation_manager *)object_header_block_get(object_get(object_index), &datum->object.animation);
    const s_object_definition *object_definition = (const s_object_definition *)unit_custom_animation_tag_get(datum->object.definition_index);
    const s_model_definition *model = (const s_model_definition *)unit_custom_animation_tag_get(object_definition->model.index);
    if (animation->m_graph_tag_index != model->animation_graph.index)
    {
        animation->reset_graph(model->animation_graph.index, object_definition->model.index, true);
        animation->reset_weapon();
        unit_animation_reset(object_index);
    }
}

static __forceinline bool unit_custom_animation_unit_is_dual_wielding(long unit_index)
{
    unit_datum *datum = OBJECT_GET(unit_index, unit_datum, _object_mask_unit);
    bool result = false;
    if (datum->unit.current_weapon_indices[0] != -1 && datum->unit.current_weapon_indices[1] != -1)
        result = true;
    return result;
}

static __forceinline const real_matrix4x3 *unit_custom_animation_object_get_node_matrix(long object_index, short node_index)
{
    unit_datum *object = OBJECT_GET(object_index, unit_datum, _object_mask_all);
    return (const real_matrix4x3 *)object_header_block_get(object_get(object_index), &object->object.node_matrices) + node_index;
}

static __forceinline bool unit_custom_animation_object_can_interpolate(long object_index)
{
    unit_datum *object = OBJECT_GET(object_index, unit_datum, _object_mask_all);
    return object->object.node_orientations.offset != -1;
}

static __forceinline void unit_custom_animation_object_reset_interpolation(long object_index)
{
    unit_datum *object = OBJECT_GET(object_index, unit_datum, _object_mask_all);
    c_animation_manager *animation = (c_animation_manager *)object_header_block_get(object_get(object_index), &object->object.animation);
    animation->animation_interpolator()->disable();
    object_wake(object_index);
}

static __forceinline const render_model_definition *unit_custom_animation_model_get_render_model(long model_index)
{
    const s_model_definition *model = (const s_model_definition *)unit_custom_animation_tag_get(model_index);
    const render_model_definition *render_model = 0;
    if (model->render_model_index != -1)
        render_model = (const render_model_definition *)unit_custom_animation_tag_get(model->render_model_index);
    return render_model;
}

static __forceinline void unit_custom_animation_matrix4x3_from_point_and_vectors(
    real_matrix4x3 *matrix, const real_point3d *position, const real_vector3d *forward, const real_vector3d *up)
{
    matrix4x3_rotation_from_vectors(matrix, forward, up);
    matrix->position = *position;
}

static __forceinline void unit_custom_animation_clear_physics_flag(unsigned long *flags, long bit)
{
    *flags &= ~(1 << bit);
}

static __forceinline void *unit_custom_animation_block_get(const unit_datum *object, const object_header_block_reference *block)
{
    return (char *)object + block->offset;
}

// @address 0x3915b0 ?unit_custom_animation_deactivate@@YAXJ@Z
void unit_custom_animation_deactivate(long object_index)
{
    unit_datum *datum = OBJECT_GET(object_index, unit_datum, _object_mask_unit);
    unit_custom_animation_state *motor = (unit_custom_animation_state *)unit_custom_animation_block_get(datum, &datum->unit.motor_program);
    c_animation_manager *animation = (c_animation_manager *)unit_custom_animation_block_get(datum, &datum->object.animation);
    unit_custom_animation_flags_block *custom_animation =
        (unit_custom_animation_flags_block *)unit_custom_animation_block_get(datum, &datum->unit.animation_storage);
    const c_model_animation *state_animation = animation->get_state_animation();
    bool world_relative = state_animation != 0 && state_animation->animation_is_world_relative();
    motor_task_interrupt(object_index, 15, 0);
    motor->motor.state &= 0xfffb;
    havok_object_rebuild(object_index, 0);
    if (custom_animation->test_flag(4))
    {
        long saved_land_index = custom_animation->custom_animation_object_index;
        custom_animation->flags &= 0xfbef;
        custom_animation->custom_animation_object_index = -1;
        object_set_always_active(object_index, false);
        bool frozen = datum->object.object_type == 0 && motor->blend_in != 0;
        if (frozen)
            object_freeze_node_orientations(object_index, true);
        unit_custom_animation_restore_graph(object_index);
        bool dual_wielding = unit_custom_animation_unit_is_dual_wielding(object_index);
        long weapon_class = 226;
        long weapon_type = 226;
        if (datum->unit.current_weapon_indices[0] != -1)
        {
            long weapon_index = datum->unit.weapon_object_indices[datum->unit.current_weapon_indices[0]];
            if (weapon_index != -1)
            {
                unit_datum *weapon = OBJECT_GET(weapon_index, unit_datum, _object_mask_weapon);
                const weapon_definition *definition = (const weapon_definition *)unit_custom_animation_tag_get(weapon->object.definition_index);
                weapon_class = dual_wielding ? 486 : definition->animation_weapon_class;
                weapon_type = definition->animation_weapon_type;
            }
        }
        long state;
        long mode;
        if (frozen)
        {
            state = 79;
            mode = 138;
        }
        else
        {
            state = 12;
            mode = motor_animation_test_mode_and_impulse(object_index, 137, weapon_class, weapon_type, 12) ? 137 : 138;
        }
        animation->m_flags &= 0xfffe;
        if (datum->object.object_type == 0 && ((biped_datum *)datum)->biped.physics.m_mode == 4)
        {
            motor_animation_posture_exit_end(object_index, mode, weapon_class, weapon_type, state);
        }
        else
        {
            motor_animation_set_mode_weapons_and_state(object_index, mode, weapon_class, weapon_type, state, 0.0f, 2, 31, true);
            long node_index = animation->find_node(459);
            if (node_index != -1)
            {
                const real_matrix4x3 *node_matrix = unit_custom_animation_object_get_node_matrix(object_index, (short)node_index);
                real_matrix4x3 matrix;
                matrix4x3_from_point_and_nonaxial_vectors_using_up(&matrix, &node_matrix->position, &node_matrix->forward, global_up3d);
                real_point3d point;
                point_from_line3d(&matrix.position, global_up3d, datum->object.bounding_radius * 0.1f, &point);
                real_vector3d vector;
                scale_vector3d(global_up3d, -datum->object.bounding_radius, &vector);
                collision_result collision;
                if (collision_test_vector(k_collision_test_structure, false, &point, &vector, object_index, -1, -1, &collision))
                    matrix.position = collision.point;
                real_vector3d forward = matrix.forward;
                real_vector3d up = matrix.up;
                normalize3d(&forward);
                normalize3d(&up);
                real_matrix4x3 world_matrix;
                object_get_world_matrix(object_index, &world_matrix);
                if (fabsf(world_matrix.position.x - matrix.position.x) >= unit_custom_animation_epsilon ||
                    fabsf(world_matrix.position.y - matrix.position.y) >= unit_custom_animation_epsilon ||
                    fabsf(world_matrix.position.z - matrix.position.z) >= unit_custom_animation_epsilon ||
                    fabsf(world_matrix.forward.i - forward.i) >= unit_custom_animation_epsilon ||
                    fabsf(world_matrix.forward.j - forward.j) >= unit_custom_animation_epsilon ||
                    fabsf(world_matrix.forward.k - forward.k) >= unit_custom_animation_epsilon ||
                    fabsf(world_matrix.up.i - up.i) >= unit_custom_animation_epsilon ||
                    fabsf(world_matrix.up.j - up.j) >= unit_custom_animation_epsilon ||
                    fabsf(world_matrix.up.k - up.k) >= unit_custom_animation_epsilon)
                {
                    object_set_position_direct(object_index, &matrix.position, &matrix.forward, &matrix.up, 0, false);
                    if (unit_custom_animation_object_can_interpolate(object_index))
                    {
                        unit_datum *object = OBJECT_GET(object_index, unit_datum, _object_mask_all);
                        real_orientation *node_orientations =
                            (real_orientation *)object_header_block_get(object_get(object_index), &object->object.node_orientations);
                        real_orientation *original_orientations = (real_orientation *)object_header_block_get(
                            object_get(object_index), &object->object.original_node_orientations);
                        real_orientation orientation;
                        if (!world_relative)
                        {
                            matrix4x3_to_orientation(&world_matrix, &orientation);
                            orientations_multiply(&orientation, node_orientations, node_orientations);
                            orientations_multiply(&orientation, original_orientations, original_orientations);
                        }
                        unit_custom_animation_matrix4x3_from_point_and_vectors(&world_matrix, &datum->object.position,
                            &datum->object.forward, &datum->object.up);
                        real_matrix4x3 inverse;
                        matrix4x3_inverse(&world_matrix, &inverse);
                        matrix4x3_to_orientation(&inverse, &orientation);
                        orientations_multiply(&orientation, node_orientations, node_orientations);
                        orientations_multiply(&orientation, original_orientations, original_orientations);
                    }
                }
            }
            else if (saved_land_index != -1)
            {
                unit_custom_animation_object_reset_interpolation(object_index);
                const s_object_definition *object_definition = (const s_object_definition *)unit_custom_animation_tag_get(datum->object.definition_index);
                const render_model_definition *render_model = unit_custom_animation_model_get_render_model(object_definition->model.index);
                real_matrix4x3 matrix;
                if (render_model != 0)
                    matrix4x3_compute_product(unit_custom_animation_object_get_node_matrix(object_index, 0),
                        &((const render_model_node *)render_model->nodes.address)->default_inverse_matrix, &matrix);
                else
                    memcpy(&matrix, unit_custom_animation_object_get_node_matrix(object_index, 0), sizeof(matrix));
                unit_custom_animation_clear_physics_flag(&datum->object.physics_flags, 8);
                if (datum->object.parent_object_index == -1)
                {
                    object_set_position_direct(object_index, &matrix.position, &matrix.forward, &matrix.up, 0, false);
                    object_set_velocities(object_index, global_zero_vector3d, global_zero_vector3d);
                    object_reconnect_to_physics(object_index);
                }
            }
            if (datum->object.object_type == 12)
                giant_handle_motor_update(object_index, false);
        }
        if (datum->unit.current_weapon_indices[1] != -1)
        {
            long weapon_index = datum->unit.weapon_object_indices[datum->unit.current_weapon_indices[1]];
            if (weapon_index != -1)
            {
                unit_datum *weapon = OBJECT_GET(weapon_index, unit_datum, _object_mask_weapon);
                const weapon_definition *definition = (const weapon_definition *)unit_custom_animation_tag_get(weapon->object.definition_index);
                long secondary_class = dual_wielding ? 486 : definition->animation_weapon_class;
                unit_animation_set_weapon(object_index, secondary_class, definition->animation_weapon_type, false);
            }
        }
        if (frozen)
        {
            object_freeze_node_orientations(object_index, false);
            object_set_velocities(object_index, &motor->velocity, 0);
            unit_custom_animation_object_reset_interpolation(object_index);
        }
    }
}
