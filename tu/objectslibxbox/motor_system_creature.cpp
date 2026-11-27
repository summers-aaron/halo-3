// objectslibxbox/motor_system_creature.obj: from the records 0xe12a8 0x108098
// @flags /TP /O2 /Oc
#include "math/real_math_types.h"
#include "math/real_math.h"
#include "physics/character_physics_component.h"
#include "physics/character_physics_input.h"
#include "physics/character_physics_output.h"
#include "physics/character_physics_move_input.h"
#include "physics/character_physics_move_output.h"
#include "objects/objects.h"
#include "motor/motor_system_creature.h"
#include "physics/character_physics.h"
#include "physics/havok.h"
#include "memory/data_typed_access.h"
#include "cseries/bit_test.h"
#include "tag_files/tag_types.h"
#include "tag_files/tag_groups.h"
#include "objects/object_header.h"
#include "physics/character_physics_mode_flying.h"
#include "physics/character_physics_mode_ground_datum.h"
#include "physics/character_physics_mode_dead.h"
#include "physics/havok_component.h"
#include "physics/havok_component_class.h"
#include "creatures/creature_datum.h"
#include "creatures/creature_definitions.h"
#include "physics/physics_constants_globals.h"

#include "physics/character_physics_ground_helpers.h"



// @address 0x10b880 ?move@c_character_physics_component@@QAAXPAUs_character_physics_move_output_datum@@PBUs_character_physics_move_input_datum@@@Z
// @address 0x9a080 ?object_get_origin@@YAPATreal_point3d@@JPAT1@@Z
// @address 0x10b600 ?character_physics_move_input_datum_initialize@@YAXPAUs_character_physics_move_input_datum@@PAVc_character_physics_component@@PBUs_character_physics_definition@@JJ_N3333333PBTreal_point3d@@PBTreal_vector3d@@55@Z

static __forceinline void motor_system_creature_move_input_initialize_ground(
    s_character_physics_move_input_datum *input, c_character_physics_component *component, bool enabled)
{
    input->ground_havok_component_index = component->get_mode_ground()->get_support_havok_component_index();
    input->ground_rigid_body_index = component->get_mode_ground()->get_support_rigid_body_index();
    input->ground_enabled = enabled;
    input->sub_initialized = true;
}
static __forceinline void motor_system_creature_move_input_initialize_flying(
    s_character_physics_move_input_datum *input, c_character_physics_component *component)
{
    input->sub_initialized = true;
}
static __forceinline void motor_system_creature_input_initialize_dead(s_character_physics_update_input_datum *input,
    c_character_physics_component *component)
{
    input->desired_up = *component->get_mode_dead()->get_resting_surface_normal();
    input->dead_size_increase = false;
    input->dead_size_reset = false;
    input->sub_initialized = true;
}
static __forceinline void object_set_desired_velocities(
    long object_index, const real_vector3d *linear_velocity, const real_vector3d *angular_velocity, bool unknown)
{
    object_datum *object = (object_datum *)object_get_and_verify_type(object_index, 4096);
    if (linear_velocity)
        object->linear_velocity = *linear_velocity;
    havok_object_set_velocities(object_index, linear_velocity, angular_velocity, unknown);
}
static __forceinline bool motor_system_creature_velocities_unknown(long object_index)
{
    creature_datum *creature = (creature_datum *)object_get_and_verify_type(object_index, 4096);
    bool result = false;
    if (!test_bit(creature->object.damage_flags, 2)) {
        if (test_bit(creature->creature.flags, 5) || test_bit((creature->creature.flags >> 4) & 0xfff, 0))
            result = true;
    }
    return result;
}
// @address 0xe12a8 ?motor_system_creature_physics_update@@YA_NJPAUs_character_physics_update_input_datum@@PAUs_character_physics_update_output_datum@@@Z
bool motor_system_creature_physics_update(long object_index, s_character_physics_update_input_datum *input,
    s_character_physics_update_output_datum *output)
{
    creature_datum *creature = (creature_datum *)object_get_and_verify_type(object_index, 4096);
    creature_definition *definition = TAG_GET('crea', creature_definition, creature->object.definition_index);
    const c_animation_manager *animation_manager = (const c_animation_manager *)object_header_block_get(creature,
        &creature->object.animation);
    real_point3d origin;
    real_vector3d linear_velocity;
    object_get_origin(object_index, &origin);
    if (test_bit(definition->physics.flags, 5))
        linear_velocity = creature->object.linear_velocity;
    else
        object_get_velocities(object_index, &linear_velocity, 0);
    c_character_physics_component *component = &creature->creature.physics;
    bool bd3 = test_bit(definition->physics.flags, 3);
    bool b8 = test_bit(creature->creature.flags, 8);
    bool b10 = test_bit(creature->creature.flags, 10);
    bool b5 = test_bit(creature->creature.flags, 5);
    bool b9 = test_bit(creature->object.physics_flags, 9);
    bool b7 = test_bit(creature->object.physics_flags, 7);
    character_physics_update_input_datum_initialize(object_index, input, component, &definition->physics,
        creature->object.havok_component_index, animation_manager,
        1.0f, _havok_group_giant, b7, false, true, false,
        b9, b5, b10, b8,
        false, bd3, false, false, global_gravity_get(), 1.0f,
        &creature->creature.control.movement, &origin, &creature->object.forward, &creature->object.up,
            &creature->object.forward, &creature->object.forward, &linear_velocity, 0);
    switch (creature->creature.physics.get_mode()) {
    case c_character_physics_component::_mode_ground:
        character_physics_update_input_datum_initialize_ground(input, component, false, false,
            test_bit(creature->creature.flags, 6), 0.0f, 0.0f);
        break;
    case c_character_physics_component::_mode_flying:
        character_physics_update_input_datum_initialize_flying(object_index, input, component,
            creature->creature.turning_amount, 0.0f);
    switch (component->get_mode()) {
    case c_character_physics_component::_mode_ground:
        component->get_mode_ground()->snap_facing(&creature->object.forward, &creature->object.up, input);
        break;
    case c_character_physics_component::_mode_flying:
        component->get_mode_flying()->snap_facing(&creature->object.forward, &creature->object.up, input);
        break;
    case c_character_physics_component::_mode_dead:
        component->get_mode_dead()->snap_facing(&creature->object.forward, &creature->object.up, input);
        break;
    case c_character_physics_component::_mode_sentinel:
    case c_character_physics_component::_mode_sentinel_b:
    case c_character_physics_component::_mode_melee:
        break;
    default:
        // match: 0xe12a8: __assume(0) closes the switch; break adds an edge flipping EXTRN order, r17-r26 colors, spill layout
        __assume(0);
    }
        break;
    case c_character_physics_component::_mode_dead:
        motor_system_creature_input_initialize_dead(input, &creature->creature.physics);
        break;
    }
    component->update(output, input);
    bool moving = magnitude_squared3d(&input->acceleration) > 0.0f;
    if (moving)
        creature->creature.flags |= 16;
    else
        creature->creature.flags &= ~16;
    object_set_desired_velocities(object_index, &output->velocity, 0,
        motor_system_creature_velocities_unknown(object_index));
    return true;
}
// @address 0x108098 ?motor_system_creature_move@@YA_NJPAUs_character_physics_move_output_datum@@@Z
bool motor_system_creature_move(long object_index, s_character_physics_move_output_datum *output)
{
    creature_datum *creature = (creature_datum *)object_get_and_verify_type(object_index, 4096);
    bool result = false;
    creature_definition *definition = TAG_GET('crea', creature_definition, creature->object.definition_index);
    if (creature->object.havok_component_index != -1) {
        c_havok_component *havok_component = datum_get_typed<c_havok_component>(g_havok_component_data,
            creature->object.havok_component_index);
        if (havok_component_has_active_entity(havok_component) || !test_bit(creature->object.physics_flags, 7)) {
            real_point3d origin;
            object_get_origin(object_index, &origin);
            s_character_physics_move_input_datum input;
            character_physics_move_input_datum_initialize(&input, &creature->creature.physics, &definition->physics,
                object_index,
                creature->object.havok_component_index, test_bit(creature->object.physics_flags, 9),
                    test_bit(creature->creature.flags, 5), false, false, false,
                test_bit(definition->physics.flags, 3), false, false, &origin, &creature->object.up,
                    &creature->object.forward, &creature->creature.control.movement);
            switch (creature->creature.physics.m_mode) {
            case c_character_physics_component::_mode_flying: motor_system_creature_move_input_initialize_flying(&input,
                &creature->creature.physics); break;
            case c_character_physics_component::_mode_ground: motor_system_creature_move_input_initialize_ground(&input,
                &creature->creature.physics, true); break;
            default: input.dead_enabled = false; input.sub_initialized = true; break;
            }
            creature->creature.physics.move(output, &input);
            unsigned long output_flags = output->flags;
            if (test_bit(output_flags,
                2)) creature->object.physics_flags |= 8; else creature->object.physics_flags &= ~8;
            if (test_bit(~output_flags, 0)) creature->creature.flags |= 1024; else creature->creature.flags &= ~1024;
            result = true;
        }
        if (test_bit(definition->physics.flags, 5)) creature->object.physics_flags &= ~512;
    }
    return result;
}
