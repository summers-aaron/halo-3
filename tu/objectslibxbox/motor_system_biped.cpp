// objectslibxbox/motor_system_biped.obj: from the records 0xe6440 0xe6660 0xe6c18 0xe8c70 0x10b210 0x10b4e0 0x10d7c8 0x10ee80 0x10f0d8 0x36d600 0x36daa0 0x36dc20 0x36dda0 0x36df68 0x36e6f8 0x36e968 0x36eab0 0x36f0d0 0x36f290 0x36f448
#include "physics/physics_constants_tls.h"
#include "objects/object_reference_types.h"
#include "physics/havok_group_types.h"
#include "objects/ragdoll_source_types.h"
#include "math/real_math.h"
#include "math/real_math_constant_storage.h"
#include "game/game_globals_storage.h"
#include "memory/data_types.h"
#include "tag_files/tag_types.h"
#include "objects/object_types.h"
#include "game/player_traits.h"
#include "objects/object_datum.h"
#include "objects/object_placement.h"
#include "physics/character_physics_move_output.h"
#include "physics/character_physics_input.h"
#include "physics/character_physics_output.h"
#include "units/motor_enums.h"
#include "units/motor_request.h"
#include "ai/sector_ref.h"
#include "animations/animation_id.h"
#include "math/matrix_math.h"
#include <math.h>
#include "cseries/crt_xdk.h"
#include "objects/objects.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "units/units.h"
#include "math/periodic_functions.h"
#include "motor/motor_system.h"
#include "motor/motor_system_biped.h"
#include "objects/damage_owner.h"
#include "physics/havok.h"
#include "units/bipeds.h"
#include "game/game_time.h"
#include "game/game_globals.h"
#include "physics/character_physics_component.h"
#include "physics/character_physics.h"
#include "objects/object_early_movers.h"
#include "game/materials.h"
#include "physics/character_physics_mode_flying.h"
#include "units/biped_melee_target_parameters_types.h"
#include "cache/cache_files.h"
#include "motor/motor_animation.h"
#include "scenario/scenario.h"
#include "physics/havok_component.h"
#include "game/game_time_types.h"
#include "memory/tls_globals.h"
#include "units/biped_types.h"
#include "physics/havok_component_class.h"
#include "physics/havok_contact_point.h"
#include "game/player_types.h"
#include "units/biped_definitions.h"
#include "ai/actor_datum.h"
#include "ai/command_script_data.h"
#include "units/motor_program_state.h"
#include "game/player_control_constants.h"
#include "physics/character_physics_definition_types.h"
#include "physics/character_physics_move_input.h"
#include "physics/character_physics_mode_melee.h"
#include "animations/animation_manager.h"
#include "physics/character_physics_mode_ground_datum.h"
#include "physics/character_physics_mode_dead.h"
#include "cseries/cseries.h"
#include "physics/character_physics_ground_helpers.h"
#include "game/falling_damage_types.h"
#include "objects/damage_data.h"
#include "objects/damage.h"
#include "game/game_ragdoll.h"
#include "game/player_information_definition.h"
#include "physics/character_physics_shape_types.h"
#include "physics/collision_test_flags.h"
#include "physics/collision_types.h"
#include "physics/collision_vector_query.h"
#include "memory/data.h"
#include "objects/object_header.h"
#include "objects/object_node_matrices_inline.h"
#include "physics/havok_component_datum_access.h"
#include "physics/character_physics_globals.h"
#include "tag_files/tag_groups.h"
#include "game/game_time_access.h"
#include "game/game_time_inline.h"
extern "C" {
#include "animations/animation_definition_storage.h"
}
#include "physics/physics_constants_globals.h"
// @flags /TP /O2 /Oc
struct s_biped_input_arguments {
    const real_point3d *position;
    c_character_physics_component *component;
};

// @address 0xe6660 ?biped_set_forward_vector@@YAXJPBTreal_vector3d@@@Z
// @address 0x36f290 ?biped_ground_plane_fix_transform_to_physics@@YAXJPAUreal_matrix4x3@@@Z
// @address 0x10b4e0 ?biped_calculate_crouch_velocity_delta@@YA_NJPAM@Z
// @address 0x36daa0 ?biped_build_axes_from_desired_facing@@YAXPBTreal_vector3d@@00PAT1@@Z
// @address 0x36dc20 ?biped_get_control_vectors@@YAXJ_NPATreal_vector3d@@11@Z
// @address 0x36dda0 biped_compute_sprint_speed_scale
// @address 0x36e968 ?biped_calculate_crouch_delta@@YAMJ@Z
// @address 0xade58 ?matrix4x3_transform_point@@YAPATreal_point3d@@PBUreal_matrix4x3@@PBT1@PAT1@@Z
// @address 0xed598 ?matrix4x3_transform_vector@@YAPATreal_vector3d@@PBUreal_matrix4x3@@PBT1@PAT1@@Z
// @address 0x9a080 ?object_get_origin@@YAPATreal_point3d@@JPAT1@@Z
// @address 0x10f0d8 ?biped_get_sentinel_animation_node_position_and_velocity@@YAXJPATreal_point3d@@PATreal_vector3d@@@Z
// @address 0x10ee80 ?biped_falling_damage@@YAXJ@Z

// @address 0x11db044 g_biped_forward_sticky_threshold_cosine
float g_biped_forward_sticky_threshold_cosine;
// @address 0x11db048 g_biped_forward_threshold_cosine
float g_biped_forward_threshold_cosine;
// @address 0x11db04c g_biped_forward_threshold_initialization_flags
unsigned int g_biped_forward_threshold_initialization_flags;
struct real_matrix4x3;


typedef char command_script_datum_crouch_speed_offset[(offsetof(command_script_datum, crouch_speed) == 0x11c) ? 1 : -1];


struct s_game_globals;


static __forceinline real_vector3d *motor_system_biped_cross_product3d_inline(const real_vector3d *a, const real_vector3d *b, real_vector3d *result)
{
    float i = a->j * b->k - a->k * b->j;
    float k = a->i * b->j - a->j * b->i;
    float j = a->k * b->i - a->i * b->k;
    result->i = i;
    result->j = j;
    result->k = k;
    return result;
}

static __forceinline bool crouch_melee_is_running(c_character_physics_component *physics)
{
    return physics->get_mode_melee()->is_running_invisible_crouched_uber_melee();
}
static __forceinline int crouch_physics_get_mode(c_character_physics_component *physics)
{
    return physics->m_mode;
}
static __forceinline actor_datum *crouch_actor_get(int actor_index)
{
    return datum_get<actor_datum>((s_data_array *)actor_data, actor_index);
}
static __forceinline bool crouch_cs_get_modifier(int actor_index, float *modifier)
{
    actor_datum *actor = crouch_actor_get(actor_index);
    bool has_modifier = false;
    if (actor->command_index != -1 && actor->swarm == 0) {
        command_script_datum *script = datum_get<command_script_datum>(tls_command_script_data(), actor->command_index);
        if (script->crouch_speed > 0.0f) {
            *modifier = script->crouch_speed;
            has_modifier = true;
        }
    }
    return has_modifier;
}
static __forceinline bool crouch_biped_is_running_invisible_crouched_uber_melee(int biped_index)
{
    biped_datum *biped = ((biped_datum *)object_get_and_verify_type(biped_index, 1));
    bool is_running = false;
    if (crouch_physics_get_mode(&biped->biped.physics) == 6 && (unsigned char)crouch_melee_is_running(&biped->biped.physics) != 0) {
        is_running = true;
    }
    return is_running;
}
static __forceinline float crouch_game_tick_length(void)
{
    return game_time_globals->tick_length;
}
static __forceinline bool crouch_actor_get_velocity(int actor_index, float *velocity)
{
    actor_datum *actor = crouch_actor_get(actor_index);
    bool has_velocity = false;
    if (actor->command_index != -1) {
        has_velocity = crouch_cs_get_modifier(actor_index, velocity);
    }
    return has_velocity;
}

static __forceinline bool crouch_physics_flag_test(const unsigned long *flags, int bit)
{
    return (*flags & (1 << bit)) != 0;
}

static __forceinline short crouch_game_tick_rate(void)
{
    return game_time_globals->ticks_per_second;
}

bool biped_calculate_crouch_velocity_delta(long biped_index, float *velocity_delta)
{
    biped_datum *biped = ((biped_datum *)object_get_and_verify_type(biped_index, 1));
    biped_definition *definition = ((biped_definition *)tag_get(1651077220, biped->object.definition_index));
    float crouch_delta = biped_calculate_crouch_delta(biped_index);
    bool result = false;
    if (!crouch_physics_flag_test(&definition->physics.flags, 1)
        && (motor_system_in_airborne_state(biped_index) || crouch_biped_is_running_invisible_crouched_uber_melee(biped_index))
        && crouch_delta != 0.0f) {
        *velocity_delta = (definition->physics.height_standing - definition->physics.height_crouching) * (float)crouch_game_tick_rate() * crouch_delta;
        result = true;
    }
    return result;
}

void biped_build_axes_from_desired_facing(const real_vector3d *desired_facing_vector, const real_vector3d *current_facing_vector,
    const real_vector3d *up_vector, real_vector3d *axes)
{
    if (up_vector) {
        axes[0] = *desired_facing_vector;
        motor_system_biped_cross_product3d_inline(up_vector, desired_facing_vector, &axes[1]);
        normalize3d(&axes[1]);
        motor_system_biped_cross_product3d_inline(&axes[1], up_vector, &axes[0]);
        normalize3d(&axes[0]);
    } else {
        axes[0] = *desired_facing_vector;
        axes[0].k = 0.0f;
        if (normalize3d(&axes[0]) == 0.0f) {
            axes[0] = *current_facing_vector;
            axes[0].k = 0.0f;
            if (normalize3d(&axes[0]) == 0.0f) {
                const real_vector3d *forward = &k_real_math_identity_matrix_storage.forward;
                axes[0] = *forward;
            }
        }
        set_real_vector3d(&axes[1], -axes[0].j, axes[0].i, 0.0f);
    }
}

void biped_get_control_vectors(long biped_index, bool use_up, real_vector3d *facing, real_vector3d *aiming, real_vector3d *movement)
{
    biped_datum *biped = ((biped_datum *)object_get_and_verify_type(biped_index, 1));
    ::object_get_orientation(biped_index, facing, 0);
    real_vector3d axes[2];
    biped_build_axes_from_desired_facing(&biped->unit.desired_facing_vector, &biped->object.forward, use_up ? &biped->object.up : 0, axes);
    scale_vector3d(&axes[0], biped->unit.throttle.i, movement);
    point_from_line3d(movement, &axes[1], biped->unit.throttle.j, movement);
    *aiming = biped->unit.desired_aiming_vector;
    if (use_up) {
        real_vector3d left;
        motor_system_biped_cross_product3d_inline(&biped->object.up, aiming, &left);
        motor_system_biped_cross_product3d_inline(&left, &biped->object.up, aiming);
    } else {
        aiming->k = 0.0f;
    }
    if (normalize3d(aiming) == 0.0f) {
        *aiming = biped->unit.desired_aiming_vector;
    }
}


void biped_set_forward_vector(long biped_index, const real_vector3d *input_vector)
{
    object_datum *biped = (object_datum *)object_get(biped_index);
    real_vector3d forward = *input_vector;
    normalize3d(&forward);
    object_adjust_node_orientations_for_facing_change(biped_index, &biped->forward, &forward);
    biped->forward = forward;
}

extern "C" float biped_compute_sprint_speed_scale(long unit_index)
{
    float speed_scale = 1.0f;
    unit_datum *unit = (unit_datum *)object_get(unit_index);
    c_motor_program_state *unit_flags = (c_motor_program_state *)((char *)unit + unit->unit.motor_offset);
    if (unit->unit.player_index != -1) {
        float ramp = 0.0f;
        s_player_control_constants *globals = (s_player_control_constants *)global_game_globals->player_control.address;
        player_datum *player = datum_get<player_datum>(player_data, unit->unit.player_index);
        long delay_ticks = game_seconds_to_ticks_round(globals->sprint_ticks_seconds);
        long elapsed_ticks = player->sprint_ticks - (long)((float)player->sprint_recovery_ticks * globals->sprint_recovery_rate + 0.5f);
        float trait_speed = player->traits.get_movement_traits()->get_speed();
        if (elapsed_ticks > delay_ticks) {
            float fraction = (float)(elapsed_ticks - delay_ticks) / (float)(game_seconds_to_ticks_round(globals->sprint_recovery_seconds) - 1);
            fraction = PIN(fraction, 0.0f, 1.0f);
            ramp = transition_function_evaluate(1, fraction);
            if (ramp > 0.0f) {
                unit_flags->state |= 2;
            }
        }
        speed_scale = (globals->sprint_speed_scale - 1.0f) * ramp + trait_speed;
    }
    return speed_scale;
}

float biped_calculate_crouch_delta(long biped_index)
{
    biped_datum *biped = ((biped_datum *)object_get_and_verify_type(biped_index, 1));
    biped_definition *definition = ((biped_definition *)tag_get(1651077220, biped->object.definition_index));
    float crouch_velocity = definition->crouch_velocity;
    if (biped->unit.actor_index != -1) {
        crouch_actor_get_velocity(biped->unit.actor_index, &crouch_velocity);
    }
    float crouch_delta = crouch_game_tick_length() * crouch_velocity;
    if ((unsigned char)crouch_physics_flag_test(&biped->unit.unit_flags, 22) != 0) {
        crouch_delta = crouch_delta < 1.0f - biped->unit.crouch_amount ? crouch_delta : 1.0f - biped->unit.crouch_amount;
    } else {
        crouch_delta = -(crouch_delta < biped->unit.crouch_amount ? crouch_delta : biped->unit.crouch_amount);
    }
    if ((unsigned char)crouch_biped_is_running_invisible_crouched_uber_melee(biped_index) != 0) {
        crouch_delta = 1.0f - biped->unit.crouch_amount > 1.0f ? 1.0f : 1.0f - biped->unit.crouch_amount;
    }
    return crouch_delta;
}


void biped_get_sentinel_animation_node_position_and_velocity(long biped_index, real_point3d *position, real_vector3d *velocity)
{
    bool success = false;
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, 1);
    const c_animation_manager *animation_manager = (c_animation_manager *)object_header_block_get(biped_index, &biped->object.animation);
    real_matrix4x3 world_matrix;
    ::object_get_world_matrix(biped_index, &world_matrix);
    if (animation_manager->get_pill_offset_data(position, velocity)) {
        matrix4x3_transform_point(&world_matrix, position, position);
        matrix4x3_transform_vector(&world_matrix, velocity, velocity);
        success = true;
    } else {
        const biped_definition *definition = TAG_GET('bipd', biped_definition, biped->object.definition_index);
        short node_index = definition->sentinel_animation_node_index;
        if (node_index != -1) {
            long node_count;
            biped_datum *node_object = (biped_datum *)object_get_and_verify_type(biped_index, -1);
            const real_matrix4x3 *node_matrices = (const real_matrix4x3 *)object_header_block_get(node_object, &node_object->object.node_matrices);
            node_count = node_object->object.node_matrices.size / sizeof(real_matrix4x3);
            if (node_index >= 0 && node_index < node_count) {
                real_point3d origin;
                object_get_origin(biped_index, &origin);
                *position = node_matrices[node_index].position;
                position->z = origin.z;
                *velocity = k_real_math_zero_vector3d_storage;
                success = true;
            }
        }
    }
    if (!success) {
        *position = world_matrix.position;
        *velocity = k_real_math_zero_vector3d_storage;
    }
}


static __forceinline real_point3d *motor_system_biped_point3d_from_line3d(
    const real_point3d *point, const real_vector3d *vector, float scale, real_point3d *result)
{
    result->x = point->x + vector->i * scale;
    result->y = point->y + vector->j * scale;
    result->z = point->z + vector->k * scale;
    return result;
}

void biped_ground_plane_fix_transform_to_physics(long biped_index, real_matrix4x3 *transform)
{
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, 1);
    if (biped->object.havok_component_index != -1 && biped->biped.physics.m_mode == 1) {
        biped_definition *definition = TAG_GET('bipd', biped_definition, biped->object.definition_index);
        float offset = definition->physics.radius;
        const real_vector3d *plane = biped->biped.physics.get_mode_ground()->get_ground_plane();
        const real_vector3d up = {0.0f, 0.0f, 1.0f};
        const real_vector3d forward = {1.0f, 0.0f, 0.0f};
        real_vector3d left;
        motor_system_biped_cross_product3d_inline(&up, &transform->forward, &left);
        if (normalize3d(&left) == 0.0f) {
            motor_system_biped_cross_product3d_inline(&forward, &transform->forward, &left);
        }
        real_vector3d facing;
        motor_system_biped_cross_product3d_inline(&left, &up, &facing);
        normalize3d(&facing);
        real_point3d position = transform->position;
        matrix4x3_rotation_from_vectors(transform, &facing, &k_real_math_identity_matrix_storage.up);
        transform->position = position;
        motor_system_biped_point3d_from_line3d(&transform->position, plane, offset, &transform->position);
        motor_system_biped_point3d_from_line3d(&transform->position, &up, -offset, &transform->position);
    }
}
static __forceinline bool motor_system_biped_input_test_flag(unsigned long flags, long bit) { return (flags >> bit) & 1; }

static __forceinline const real_vector3d &motor_system_biped_input_aiming_vector(long index)
{
    return ((biped_datum *)object_get_and_verify_type(index, 1))->unit.aiming_vector;
}

static __forceinline void motor_system_biped_input_initialize_ground(
    s_character_physics_move_input_datum *input, c_character_physics_component *component, bool enabled)
{
    input->sub_initialized = true;
    input->ground_havok_component_index = component->get_mode_ground()->get_support_havok_component_index();
    input->ground_rigid_body_index = component->get_mode_ground()->get_support_rigid_body_index();
    input->ground_enabled = enabled;
}

static __forceinline short motor_system_biped_input_body_index(const c_havok_component *component)
{
    long index = component->m_object_postion_controlling_rigid_body_index;
    return index >= 0 && index < component->m_rigid_bodies.m_size ? index : -1;
}
static __forceinline void motor_system_biped_input_body_position(const c_havok_component *component, long index, real_point3d *out)
{
    const phys_rigid_body *body = component->m_rigid_bodies[index].m_rigid_body;
    out->x = body->get_contact_position()(0); out->y = body->get_contact_position()(1); out->z = body->get_contact_position()(2);
}
static __forceinline void motor_system_biped_input_initialize_sentinel(s_character_physics_move_input_datum *input, const real_point3d *position)
{
    input->sub_initialized = true; input->sentinel_physics_position = *position;
}

static __forceinline void motor_system_biped_input_initialize_dead(
    s_character_physics_move_input_datum *input, const c_character_physics_component *component, bool enabled)
{
    input->dead_enabled = enabled;
    input->sub_initialized = true;
}
static __forceinline void motor_system_biped_input_initialize_flying(
    s_character_physics_move_input_datum *input, const c_character_physics_component *component)
{
    input->sub_initialized = true;
}
static __forceinline void motor_system_biped_input_initialize_melee(s_character_physics_move_input_datum *input) { input->sub_initialized = true; }

// @address 0x36e6f8 ?biped_initialize_character_physics_move_input@@YAXJPAUs_character_physics_move_input_datum@@_N1111@Z
void biped_initialize_character_physics_move_input(long object_index, s_character_physics_move_input_datum *input,
    bool b0, bool b1, bool b2, bool b3, bool b4)
{
    biped_datum *biped = ((biped_datum *)object_get_and_verify_type(object_index, -1));
    long offset = biped->unit.animation_storage.offset;
    const biped_definition *definition =
        (const biped_definition *)tag_get(0, biped->object.definition_index);
    {
    real_matrix4x3 world_matrix;
    real_vector3d forward;
    object_get_world_matrix(object_index, &world_matrix);
    if (motor_system_biped_input_test_flag(definition->unit.flags, 11)) {
        forward = world_matrix.forward;
    } else {
        forward = motor_system_biped_input_aiming_vector(object_index);
    }
    unsigned short flags = *(const unsigned short *)((const char *)biped + offset);
    bool interp_flag;
    if (motor_system_biped_input_test_flag(flags, 2)) {
        interp_flag = true;
        if (!motor_system_biped_input_test_flag(flags, 4))
            interp_flag = false;
    } else {
        interp_flag = false;
    }
    bool physics_flag = motor_system_biped_input_test_flag(biped->object.physics_flags, 9);
    character_physics_move_input_datum_initialize(input, &biped->biped.physics, &definition->physics,
        object_index, biped->object.havok_component_index, physics_flag,
        b1, interp_flag, b2, false, b4, biped->unit.actor_index != -1, b4,
        &world_matrix.position, &world_matrix.up, &forward, &biped->unit.throttle);
    }
    c_character_physics_component *physics = &biped->biped.physics;
    switch (biped->biped.physics.get_mode()) {
    case c_character_physics_component::_mode_ground:
        motor_system_biped_input_initialize_ground(input, physics, false);
        break;
    case c_character_physics_component::_mode_dead:
        motor_system_biped_input_initialize_dead(input, physics, b3);
        break;
    case c_character_physics_component::_mode_sentinel:
    case c_character_physics_component::_mode_sentinel_b: {
        unsigned short component_index = (unsigned short)biped->object.havok_component_index;
        const c_havok_component *component =
            datum_get<c_havok_component>(g_havok_component_data, component_index);
        short rigid_body_index = motor_system_biped_input_body_index(component);
        real_point3d sentinel_position;
        motor_system_biped_input_body_position(component, rigid_body_index, &sentinel_position);
        motor_system_biped_input_initialize_sentinel(input, &sentinel_position);
        break;
    }
    case c_character_physics_component::_mode_flying:
        motor_system_biped_input_initialize_flying(input, physics);
        break;
    case c_character_physics_component::_mode_melee:
        motor_system_biped_input_initialize_melee(input);
        break;
    default:
        // match: 0x36e6f8: unreachable hint omits dead branch check
        __assume(0);
    }
}




static __forceinline bool motor_system_biped_is_sentinel_mode(const c_character_physics_component *physics)
{
    c_character_physics_component::e_mode mode = physics->get_mode();
    return mode == c_character_physics_component::_mode_sentinel
        || mode == c_character_physics_component::_mode_sentinel_b;
}

static __forceinline bool motor_system_biped_is_physics_model_havok_component(const c_havok_component *component)
{
    return motor_system_biped_input_test_flag(component->m_flags, 8);
}

static __forceinline void motor_system_biped_set_desired_velocities(long object_index, const real_vector3d *linear_velocity,
    const real_vector3d *angular_velocity, bool unknown)
{
    if (linear_velocity) {
        biped_datum *object =
            (biped_datum *)((biped_datum *)object_get_and_verify_type(object_index, 1));
        object->object.linear_velocity = *linear_velocity;
    }
    havok_object_set_velocities(object_index, linear_velocity, angular_velocity, unknown);
}

static __forceinline void motor_system_biped_set_flag448(biped_datum *biped, bool enable)
{
    if (enable)
        biped->biped.flags |= 0x4000;
    else
        biped->biped.flags &= ~0x4000;
}

// @address 0xe8c70 ?motor_system_biped_physics_postupdate_moving@@YAXJPAUs_character_physics_update_input_datum@@PAUs_character_physics_update_output_datum@@PA_N@Z
void motor_system_biped_physics_postupdate_moving(long biped_index, s_character_physics_update_input_datum *input,
    s_character_physics_update_output_datum *output, bool *crouch_flag)
{
    biped_datum *biped =
        (biped_datum *)((biped_datum *)object_get_and_verify_type(biped_index, 1));
    if (motor_system_biped_is_sentinel_mode(&biped->biped.physics) && output->origin_valid)
        object_set_position_direct(biped_index, &output->origin, 0, 0, 0, false);
    biped_physics_update_crouch(biped_index, output, crouch_flag);
    if (motor_system_biped_input_test_flag(biped->object.physics_flags, 7)) {
        if (biped->object.havok_component_index != -1) {
            const c_havok_component *component = havok_component_get(biped->object.havok_component_index);
            if (!motor_system_biped_is_physics_model_havok_component(component)) {
                bool accelerating;
                if (motor_system_is_relaxed(biped_index) != 0 || magnitude_squared3d(&input->acceleration) <= 0.0f)
                    accelerating = false;
                else
                    accelerating = true;
                bool remain_active;
                if (biped_should_remain_active(biped_index) || accelerating)
                    remain_active = true;
                else
                    remain_active = false;
                motor_system_biped_set_desired_velocities(biped_index, &output->velocity, 0, remain_active);
            }
        }
        if ((output->flags & 0x10) != 0)
            motor_system_biped_set_flag448(biped, motor_system_biped_input_test_flag(output->flags, 5));
    } else {
        real_point3d new_position;
        motor_system_biped_point3d_from_line3d(&biped->object.position, &output->velocity, crouch_game_tick_length(), &new_position);
        object_set_position_internal(biped_index, &new_position, 0, 0, 0, false, true, false, false);
        object_set_requires_motion(biped_index);
        motor_system_biped_set_desired_velocities(biped_index, &k_real_math_zero_vector3d_storage, 0, false);
        motor_system_biped_set_flag448(biped, false);
    }
}


static __forceinline void biped_clear_pathfinding_cache(long object_index)
{
    biped_datum *biped = ((biped_datum *)object_get_and_verify_type(object_index, 1));
    if (!motor_system_biped_input_test_flag(biped->object.physics_flags, 9)) {
        biped->biped.pathfinding_structure_index = -1;
        biped->biped.pathfinding_surface_index = -1;
        biped->biped.pathfinding_sector.structure_bsp_index = -1;
        biped->biped.pathfinding_sector.sector_index = -1;
        biped->biped.pathfinding_instanced_geometry_index = -1;
        biped->biped.pathfinding_object_index = -1;
        biped->biped.pathfinding_bsp_reference = -1;
    }
}

// @address 0x10b210 ?motor_system_biped_move@@YA_NJ_N000PAUs_character_physics_move_output_datum@@@Z
bool motor_system_biped_move(long object_index, bool b1, bool b2, bool b3, bool b4, s_character_physics_move_output_datum *output)
{
    biped_datum *biped = ((biped_datum *)object_get_and_verify_type(object_index, 1));
    float velocity_delta;
    real_vector3d velocity;
    s_character_physics_move_input_datum input;
    bool result = false;
    if (biped->object.havok_component_index != -1) {
        c_havok_component *havok_entry = havok_component_get(biped->object.havok_component_index);
        float crouch_delta = biped_calculate_crouch_delta(object_index);
        if (biped_calculate_crouch_velocity_delta(object_index, &velocity_delta)) {
            object_get_velocities(object_index, &velocity, 0);
            velocity.k = velocity.k - velocity_delta;
            object_set_velocities(object_index, &velocity, 0);
        }
        biped->unit.crouch_amount = PIN(biped->unit.crouch_amount + crouch_delta, 0.0f, 1.0f);
        long mover_offset = biped->unit.motor_offset;
        c_motor_program_state *mover = (c_motor_program_state *)((char *)biped + mover_offset);
        mover->state &= ~1;
        if (havok_component_has_active_entity(havok_entry) || !motor_system_biped_input_test_flag(biped->object.physics_flags, 7)) {
            biped->biped.flags &= ~8;
            object_get_origin(object_index, &biped->biped.position);
            biped_clear_pathfinding_cache(object_index);
            if ((biped->object.parent_object_index == -1 && motor_system_biped_input_test_flag(biped->object.physics_flags, 7))
                || motor_system_biped_input_test_flag(biped->object.physics_flags, 4)) {
                bool extra;
                if (b4) {
                    extra = true;
                } else {
                    const biped_definition *definition = ((biped_definition *)tag_get(0, biped->object.definition_index));
                    extra = false;
                    if (motor_system_biped_input_test_flag(definition->physics.flags, 3))
                        extra = true;
                }
                biped_initialize_character_physics_move_input(object_index, &input, false, b1, b2, b3, extra);
                biped->biped.physics.move(output, &input);
                if (biped->biped.physics.get_mode() == c_character_physics_component::_mode_ground) {
                    if (motor_system_biped_input_test_flag(output->flags, 2))
                        biped->object.physics_flags |= 8;
                    else
                        biped->object.physics_flags &= ~8;
                }
                if (motor_system_biped_input_test_flag(~output->flags, 0))
                    biped->biped.flags |= 0x400;
                else
                    biped->biped.flags &= ~0x400;
                biped_update_pathfinding_cache(object_index, output);
                biped_bumped_object(object_index, output->material, &havok_entry->m_rigid_bodies[0].m_pre_simulation_linear_velocity);
                result = true;
            }
        }
    }
    biped->biped.flags &= ~4;
    return result;
}

// @address 0x36d600 ?biped_move_calculate_controlled_by_aiming@@YAXJPBTreal_vector3d@@00_NPAT1@PAF@Z
void biped_move_calculate_controlled_by_aiming(long biped_index, const real_vector3d *aiming_vector, const real_vector3d *movement_vector,
    const real_vector3d *default_vector, bool use_up, real_vector3d *movement_direction, short *movement_type)
{
    bool valid = true;
    long type = 0;
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, 1);
    if (!(g_biped_forward_threshold_initialization_flags & 1)) {
        g_biped_forward_threshold_initialization_flags |= 1;
        g_biped_forward_threshold_cosine = (float)cos((double)(40.0f * 0.017453292f));
    }
    if (!(g_biped_forward_threshold_initialization_flags & 2)) {
        g_biped_forward_threshold_initialization_flags |= 2;
        g_biped_forward_sticky_threshold_cosine = (float)cos((double)(50.0f * 0.017453292f));
    }
    real_vector3d axes[4];

    if (use_up) {
        axes[0] = *aiming_vector;
        if (normalize3d(&axes[0]) == 0.0f) {
            valid = false;
            axes[0] = *default_vector;
        }
        if (biped->unit.player_index != -1) {
            const real_vector3d up = {0.0f, 0.0f, 1.0f};
            motor_system_biped_cross_product3d_inline(&up, &axes[0], &axes[2]);
            normalize3d(&axes[2]);
        } else {
            motor_system_biped_cross_product3d_inline(&biped->object.up, &axes[0], &axes[2]);
            normalize3d(&axes[2]);
            motor_system_biped_cross_product3d_inline(&axes[2], &biped->object.up, &axes[0]);
            normalize3d(&axes[0]);
        }
    } else {
        axes[0] = *aiming_vector;
        axes[0].k = 0.0f;
        if (normalize3d(&axes[0]) == 0.0f) {
            valid = false;
            axes[0] = *default_vector;
        }
        set_real_vector3d(&axes[2], -axes[0].j, axes[0].i, 0.0f);
    }

    if (!valid) {
        *movement_type = (short)type;
        if (movement_direction) {
            *movement_direction = *movement_vector;
            if (!use_up) {
                movement_direction->k = 0.0f;
                if (normalize3d(movement_direction) == 0.0f) {
                    *movement_direction = *default_vector;
                }
            }
        }
    } else {
        bool moving_forward = *movement_type == 0 || *movement_type == 1;
        bool moving_sideways = *movement_type == 2 || *movement_type == 3;
        negate_vector3d(&axes[0], &axes[1]);
        negate_vector3d(&axes[2], &axes[3]);

        float forward_amount;
        float sideways_amount;
        float vertical_amount;
        if (use_up) {
            forward_amount = dot_product3d(movement_vector, &axes[0]);
            sideways_amount = dot_product3d(movement_vector, &axes[2]);
            vertical_amount = dot_product3d(movement_vector, &biped->object.up);
        } else {
            const real_vector3d up = {0.0f, 0.0f, 1.0f};
            forward_amount = dot_product2d(&movement_vector->xy, &axes[0].xy);
            sideways_amount = dot_product2d(&movement_vector->xy, &axes[2].xy);
            vertical_amount = dot_product3d(movement_vector, &up);
        }

        float threshold = moving_sideways ? g_biped_forward_threshold_cosine
            : (moving_forward ? g_biped_forward_sticky_threshold_cosine : g_biped_forward_threshold_cosine);
        bool forward_dominant = (float)fabs(forward_amount) > threshold;
        bool vertical_dominant = (float)fabs(vertical_amount) > 0.9f;
        if (forward_dominant || vertical_dominant) {
            type = forward_amount >= 0.0f ? 0 : 1;
        } else if (sideways_amount >= 0.0f) {
            type = 2;
        } else {
            type = 3;
        }
        *movement_type = type;
        if (movement_direction) {
            *movement_direction = axes[type];
        }
    }
}

// @address 0x10ee80 ?biped_falling_damage@@YAXJ@Z


// match: noalias declaration preserves caller live registers

void biped_falling_damage(long object_index)
{
    s_falling_damage_definition *options = (s_falling_damage_definition *)global_game_globals->falling_damage.address;
    biped_datum *biped = ((biped_datum *)object_get_and_verify_type(object_index, 1));
    c_motor_program_state *mover = (c_motor_program_state *)((char *)biped + biped->unit.motor_offset);
    biped_definition *definition = (biped_definition *)((biped_definition *)tag_get(0, biped->object.definition_index));
    bool skip_damage = motor_system_biped_input_test_flag(biped->unit.unit_flags, 9)
        || motor_system_biped_input_test_flag(definition->flags, 2)
        || motor_system_biped_input_test_flag(biped->object.damage_flags, 8);
    float threshold = definition->physics.ground.gravity_scale;
    float scale;
    if (threshold > 0.0f)
        scale = threshold;
    else
        scale = 1.0f;
    if (game_globals->options.game_mode != 2 && biped->object.parent_object_index == -1) {
        union {
            real_vector3d linear;
            real_point3d center;
        };
        float crouch_delta;
        float velocity_z;
        real_vector3d early_offset;
        real_vector3d early_axis;
        object_get_velocities(object_index, &linear, 0);
        velocity_z = linear.k;
        short mover_flags = (short)mover->state;
        if ((mover_flags & 1) != 0 && biped_calculate_crouch_velocity_delta(object_index, &crouch_delta))
            velocity_z -= crouch_delta;
        object_get_center_of_mass(object_index, &center);
        if (object_get_early_mover_local_space_velocity(object_index, &center, &early_offset, &early_axis, false, false))
            velocity_z -= early_offset.k;
        if (biped->biped.physics.get_mode() == c_character_physics_component::_mode_ground
            && velocity_z < -(options->runtime_maximum_falling_velocity * scale)) {
            if (!skip_damage && !motor_system_biped_input_test_flag(biped->object.damage_flags, 2)) {
                biped->biped.flags |= 16;
                s_damage_data damage;
                damage_data_new(&damage, options->distance_damage.index);
                damage.reporting_info = 4;
                damage.flags |= 4;
                damage_compute_damage_owner_from_object_index(object_index, &damage.owner_data);
                object_cause_damage(&damage, object_index, -1, -1, -1, 0);
            }
            if (game_globals->options.game_mode != 2
                && motor_system_biped_input_test_flag(biped->object.object_flags, 18)
                && biped->unit.player_index == -1
                && !simulation_query_object_is_predicted(object_index))
                object_delete(object_index);
        }
    }
}


static __forceinline void *motor_system_biped_crouch_header_block_get(long object_index, const object_header_block_reference *reference)
{
    return (char *)((biped_datum *)object_get(object_index)) + reference->offset;
}
static __forceinline c_havok_component *motor_system_biped_crouch_havok_component_get(long index)
{
    return datum_get<c_havok_component>(g_havok_component_data, index);
}
static __forceinline short motor_system_biped_crouch_controlling_rigid_body_index(const c_havok_component *component)
{
    long rigid_body_index = component->m_object_postion_controlling_rigid_body_index;
    if (rigid_body_index < 0 || rigid_body_index >= component->m_rigid_bodies.m_size)
        rigid_body_index = -1;
    return (short)rigid_body_index;
}

// @rdata 0x8202490c
static const float k_motor_system_biped_crouch_epsilon = 0.0001f;
// @address 0x36eab0 ?biped_physics_update_crouch@@YAXJPAUs_character_physics_update_output_datum@@PA_N@Z
void biped_physics_update_crouch(long biped_index, s_character_physics_update_output_datum *output, bool *crouching)
{
    bool crouch_requested = crouch_physics_flag_test(&output->flags, 1);
    biped_datum *biped = ((biped_datum *)object_get(biped_index));
    bool crouch_control = crouch_physics_flag_test(&biped->unit.control_flags, 0);
    c_motor_program_state *mover = (c_motor_program_state *)motor_system_biped_crouch_header_block_get(biped_index, &biped->unit.motor_program);
    biped_definition *definition = ((biped_definition *)tag_get(1651077220, biped->object.definition_index));

    if (biped->biped.physics.get_mode() == c_character_physics_component::_mode_melee)
    {
        c_character_physics_mode_melee_datum *melee = biped->biped.physics.get_mode_melee();
        if (melee->get_exit_invisible_crouched_uber_melee_offset() != 0 && !crouch_biped_is_running_invisible_crouched_uber_melee(biped_index))
            melee->decrement_invisible_crouched_uber_melee_offset();
    }

    bool was_crouching = crouch_physics_flag_test(&biped->unit.unit_flags, 22);
    *crouching = was_crouching;
    bool running_melee = crouch_biped_is_running_invisible_crouched_uber_melee(biped_index);

    if (biped->biped.physics.get_mode() != c_character_physics_component::_mode_sentinel && !running_melee && !game_time_is_paused())
    {
        if ((!was_crouching && crouch_control) || (was_crouching && !crouch_control && crouch_requested))
        {
            if (crouch_control)
                biped->unit.unit_flags |= 0x400000;
            else
                biped->unit.unit_flags &= ~0x400000;
            havok_object_rebuild(biped_index, 0);
        }
    }

    if (biped->biped.physics.get_mode() == c_character_physics_component::_mode_melee &&
        !biped->biped.physics.get_mode_melee()->is_invisible_crouched_uber_melee_physics_initialized() && running_melee && !*crouching)
    {
        c_character_physics_mode_melee_datum *melee = biped->biped.physics.get_mode_melee();
        float range = -(definition->physics.height_standing - definition->physics.height_crouching);
        long exit_offset = (long)(range / (crouch_game_tick_length() * 0.15f) + 0.5f);
        havok_object_rebuild(biped_index, 0);
        melee->start_invisible_crouched_uber_melee(exit_offset);
    }

    float crouch_delta = biped_calculate_crouch_delta(biped_index);
    if (!crouch_physics_flag_test(&definition->physics.flags, 1) && (motor_system_in_airborne_state(biped_index) || running_melee))
        output->velocity.k += (definition->physics.height_standing - definition->physics.height_crouching) * (float)crouch_game_tick_rate() * crouch_delta;

    if (biped->object.parent_object_index == -1 && biped->unit.player_index != -1 && !crouch_physics_flag_test(&biped->object.damage_flags, 2) &&
        !motor_system_in_airborne_state(biped_index))
    {
        float velocity = (definition->physics.height_standing - definition->physics.height_crouching) * (float)crouch_game_tick_rate() * crouch_delta;
        if (!(fabs(velocity - 0.0f) < k_motor_system_biped_crouch_epsilon))
        {
            const float search_radius = 0.4f;
            long closest_index = -1;
            float closest_distance_squared = search_radius * search_radius;
            real_point3d origin;
            long object_indices[8];
            object_get_origin(biped_index, &origin);
            long object_count = objects_in_sphere(0, 1, &biped->object.location, &origin, search_radius, object_indices, 8);
            for (long object_index = 0; object_index < object_count; object_index++)
            {
                long candidate_index = object_indices[object_index];
                biped_datum *candidate = ((biped_datum *)object_get(candidate_index));
                if (candidate->object.parent_object_index == -1 && crouch_physics_flag_test(&candidate->object.damage_flags, 2))
                {
                    object_marker marker;
                    if (internal_object_get_markers_by_string_id(candidate_index, 177, &marker, 1, false) == 1)
                    {
                        float distance_squared = distance_squared3d(&origin, &marker.matrix.position);
                        if (distance_squared < closest_distance_squared)
                        {
                            closest_index = candidate_index;
                            closest_distance_squared = distance_squared;
                        }
                    }
                }
            }
            if (closest_index != -1)
            {
                object_marker marker;
                if (internal_object_get_markers_by_string_id(closest_index, 177, &marker, 1, false) == 1)
                {
                    biped_datum *closest = ((biped_datum *)object_get(closest_index));
                    if (motor_system_is_relaxed(closest_index) != 1 && game_can_create_additonal_ragdoll(_ragdoll_source_death))
                    {
                        c_motor_request request;
                        request.setup_force_relaxation_state(_action_none, 2, false);
                        motor_system_submit(closest_index, &request);
                    }
                    if (motor_system_is_relaxed(closest_index) == 1)
                    {
                        long havok_component_index = closest->object.havok_component_index;
                        if (havok_component_index != -1)
                        {
                            c_havok_component *component = motor_system_biped_crouch_havok_component_get(havok_component_index);
                            long rigid_body_index = motor_system_biped_crouch_controlling_rigid_body_index(component);
                            if (crouch_physics_flag_test(&component->m_flags, 8) && rigid_body_index != -1)
                            {
                                float acceleration_scale = 0.3f;
                                float maximum_offset = 0.02f;
                                real_point3d center_of_mass;
                                component->get_rigid_body_center_of_mass(rigid_body_index, &center_of_mass);
                                real_vector3d offset;
                                vector_from_points3d(&center_of_mass, &origin, &offset);
                                scale_vector3d(&marker.matrix.up, dot_product3d(&offset, &marker.matrix.up), &offset);
                                limit3d(&offset, maximum_offset);
                                motor_system_biped_point3d_from_line3d(&center_of_mass, &offset, 1.0f, &center_of_mass);
                                real_vector3d acceleration;
                                scale_vector3d(global_up3d, acceleration_scale * -velocity, &acceleration);
                                component->rigid_body_apply_acceleration_at_point(rigid_body_index, &center_of_mass, &acceleration);
                            }
                        }
                    }
                }
            }
        }
    }
    mover->state |= 1;
}



static __forceinline c_character_physics_component *motor_system_biped_initialize_component_input(
    long object_index, s_character_physics_update_input_datum *input, const s_biped_input_arguments &arguments,
    biped_datum *biped, const s_character_physics_definition *definition, long havok_component_index, const c_animation_manager *animation_manager,
    float scale, e_havok_group havok_group, bool connected, bool facing_constrained, unsigned long biped_flags, bool animation_disabled,
    bool at_rest, bool use_actor_gravity, bool reduced_adhesion, bool component_flag, bool stun_active, bool snap_to_ground,
    bool reference_relative, bool limit_upward,
    float movement_scale, float acceleration_scale,
    const real_vector3d *forward, const real_vector3d *up, const real_vector3d *facing, const real_vector3d *aiming,
    const real_vector3d *external_velocity, const phys_shape *shape)
{
    character_physics_update_input_datum_initialize(object_index, input, arguments.component, definition, havok_component_index, animation_manager,
        scale, havok_group, connected, facing_constrained, TEST_BIT(biped_flags, 3), animation_disabled, at_rest, use_actor_gravity,
        reduced_adhesion, component_flag, stun_active, snap_to_ground, reference_relative, limit_upward, movement_scale,
        acceleration_scale, &biped->unit.throttle, arguments.position,
        forward, up, facing, aiming, external_velocity, shape);
    return arguments.component;
}


// mode-specific fields of s_character_physics_update_input_datum that the canonical header leaves as padding


template<typename t_storage>
static __forceinline bool motor_system_biped_initialize_flag_test(const t_storage *flags, long bit)
{
    typedef c_flags_no_init<long, t_storage, sizeof(t_storage) * 8> flags_type;
    return ((const flags_type *)flags)->test(bit);
}

static __forceinline const phys_shape *motor_system_biped_physics_definition_get_shape(const s_character_physics_definition *definition)
{
    const phys_shape *shape = NULL;
    if (definition->pills.count)
        shape = (const phys_shape *)&((const s_character_physics_pill *)definition->pills.address)->shape;
    else if (!motor_system_biped_input_test_flag(definition->flags, 0))
        shape = (const phys_shape *)&((const s_character_physics_dead_sphere *)definition->spheres.address)->transform;
    else
        shape = (const phys_shape *)&((const s_character_physics_dead_sphere *)definition->spheres.address)->shape;
    return shape;
}
static __forceinline long motor_system_biped_game_difficulty_get()
{
    if (game_globals->options.game_mode == 1)
        return game_globals->options.campaign_difficulty;
    return 1;
}
static __forceinline bool motor_system_biped_character_physics_flag(short flags)
{
    return (flags & 1) != 0;
}
static __forceinline void motor_system_biped_havok_rigid_body_position(const c_havok_component *component, long index, real_point3d *position)
{
    const phys_rigid_body *body = component->m_rigid_bodies[index].m_rigid_body;
    set_real_point3d(position, body->get_contact_position()(0), body->get_contact_position()(1), body->get_contact_position()(2));
}
static __forceinline void motor_system_biped_update_input_initialize_dead(s_character_physics_update_input_datum *input,
    const c_character_physics_mode_dead_datum *mode,
    bool size_increase, bool size_reset)
{
    const real_vector3d *ground_plane = mode->m_contact_ticks ? &mode->m_ground_plane : global_up3d;
    input->desired_up = *ground_plane;
    input->dead_size_increase = size_increase;
    input->dead_size_reset = size_reset;
    input->sub_initialized = true;
}
template<long k_depth>
static __forceinline long motor_system_biped_update_input_word(long value)
{
    // match: 0x36df68: const-reference over rotl-by-0 pins r10/r11 coloring in inlined callers; a direct passthrough swaps them
    const long &word = (long)_rotl((unsigned long)motor_system_biped_update_input_word<k_depth - 1>(value), 0);
    return word;
}
template<>
__forceinline long motor_system_biped_update_input_word<0>(long value)
{
    return value;
}

static __forceinline void motor_system_biped_update_input_initialize_melee(s_character_physics_update_input_datum *input,
    c_character_physics_component *component,
    bool movement, const real_point3d *target_position, const real_vector3d *target_velocity, float radius, float target_radius, float distance)
{
    // NOTE: keep the pun + <7> depth: a typed bool store emits extra normalization
    // (clrlwi/cntlzw/rlwinm/xori) and dropping the depth recolors 0x36df68 GPRs.
    *(unsigned char *)&input->melee_movement =
        (unsigned char)motor_system_biped_update_input_word<7>(movement);
    input->melee.radius = radius;
    input->melee.target_radius = target_radius;
    input->melee.target_position = *target_position;
    input->melee.target_velocity = *target_velocity;
    input->movement_magnitude = distance;
    input->sub_initialized = true;
}
static __forceinline void motor_system_biped_update_input_initialize_sentinel(s_character_physics_update_input_datum *input,
    c_character_physics_component *component,
    c_animation_id animation_id, const real_point3d *previous_position, const real_point3d *position, const real_vector3d *velocity)
{
    input->animation = animation_id;
    input->previous_position = *previous_position;
    input->position = *position;
    input->velocity = *velocity;
    input->sub_initialized = true;
}

static __forceinline void motor_system_biped_input_snap_facing(c_character_physics_component *component,
    real_vector3d *forward, real_vector3d *up, const s_character_physics_update_input_datum *input)
{
    switch (component->get_mode()) {
    case c_character_physics_component::_mode_ground:
        component->get_mode_ground()->snap_facing(forward, up, input);
        break;
    case c_character_physics_component::_mode_flying:
        component->get_mode_flying()->snap_facing(forward, up, input);
        break;
    case c_character_physics_component::_mode_dead:
        component->get_mode_dead()->snap_facing(forward, up, input);
        break;
    case c_character_physics_component::_mode_sentinel:
    case c_character_physics_component::_mode_sentinel_b:
    case c_character_physics_component::_mode_melee:
        break;
    default:
        // match: 0x36df68: unreachable-default hint keeps the ground-then-dead branch order; a plain default swaps the dead/ground branch entries
        __assume(0);
    }
}

static __forceinline bool motor_system_biped_transition_channel_valid_inline(const c_animation_channel *channel)
{
    return channel->m_enabled && motor_system_biped_update_input_word<36>(channel->m_graph_index) != -1
        && (unsigned short)channel->m_animation_id.m_animation_index < 0x8000;
}
static __forceinline const c_animation_channel *motor_system_biped_choose_active_channel(
    const c_animation_channel *channel, const c_animation_channel *transition)
{
    if (motor_system_biped_transition_channel_valid_inline(transition))
        channel = transition;
    return channel;
}
static __forceinline const c_animation_channel *motor_system_biped_get_active_channel(const c_animation_manager *manager)
{
    return motor_system_biped_choose_active_channel(&manager->m_state_channel, &manager->m_transition_channel);
}
static __forceinline c_animation_id motor_system_biped_channel_get_animation_id(const c_animation_channel *channel)
{
    return channel->m_animation_id;
}
static __forceinline c_animation_id motor_system_biped_get_active_animation_id(const c_animation_manager *manager)
{
    return motor_system_biped_channel_get_animation_id(motor_system_biped_get_active_channel(manager));
}

// @address 0x36df68 ?biped_initialize_character_physics_update_input@@YAXJPAUs_character_physics_update_input_datum@@_N1M11@Z
void biped_initialize_character_physics_update_input(long biped_index, s_character_physics_update_input_datum *input,
    bool disable_scale, bool use_actor_gravity, float flying_friction, bool dead_size_reset, bool force_flag)
{
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, 1);
    s_biped_input_arguments arguments;
    biped_datum *animation_owner = (biped_datum *)object_get_and_verify_type(biped_index, -1);
    arguments.component = &animation_owner->biped.physics;
    const biped_definition *definition = TAG_GET('bipd', biped_definition, biped->object.definition_index);
    c_havok_component *havok_component = motor_system_biped_crouch_havok_component_get(biped->object.havok_component_index);
    const unsigned short *unit_animation_flags = (const unsigned short *)((const char *)biped + biped->unit.animation_storage.offset);
    const c_animation_manager *animation_manager = (const c_animation_manager *)((char *)animation_owner + biped->object.animation.offset);
    bool stunned_state = animation_manager->get_state_name() == 182;
    const phys_shape *shape = NULL;
    float scale = 1.0f;
    float gravity = g_physics_constants->gravity;
    float stun_fraction = (game_ticks_to_seconds((float)biped->biped.stun_ticks)) / definition->stun_duration;
    bool component_flag = motor_system_biped_character_physics_flag(animation_owner->biped.physics.m_flags);
    real_point3d origin;
    object_get_origin(biped_index, &origin);
    real_vector3d linear_velocity;
    havok_component->get_rigid_body_linear_velocity(0, &linear_velocity);
    real_vector3d aiming;
    if (motor_system_biped_initialize_flag_test(&definition->unit.flags, 11))
        aiming = biped->object.forward;
    else
        aiming = biped->unit.aiming_vector;
    if (disable_scale) {
        scale = 0.0f;
    } else {
        if (animation_owner->biped.physics.get_mode() == c_character_physics_component::_mode_ground) {
            bool sprint_scale = motor_system_biped_initialize_flag_test(&definition->physics.flags, 2) && !stunned_state;
            if (sprint_scale) {
                const s_player_information_definition *information = (const s_player_information_definition *)
                    global_game_globals->player_information.address;
                scale *= biped_compute_sprint_speed_scale(biped_index);
                scale *= 1.0f - information->stun_movement_penalty * biped->unit.look_difficulty_scale;
            }
        }
        if (motor_system_biped_initialize_flag_test(&definition->flags, 6) && biped->unit.aiming_speed == 0) {
            float difficulty_scale = game_difficulty_get_value_by_difficulty(8, (short)motor_system_biped_game_difficulty_get());
            scale = 1.0f + difficulty_scale * ((float)((unsigned long)biped_index % 137) / 137.0f);
        }
    }
    // match: 0x36df68: using control_flags directly moves the object-header TLS offset from r16 to r17.
    const unsigned long biped_flags = (unsigned long)motor_system_biped_update_input_word<22>((long)biped->unit.control_flags);
    if (!motor_system_biped_initialize_flag_test(&biped_flags, 0) && motor_system_biped_initialize_flag_test(&biped->unit.unit_flags, 22))
        shape = motor_system_biped_physics_definition_get_shape(&definition->physics);
    if (biped->unit.player_index != -1) {
        player_datum *player = &((player_datum *)player_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(biped->unit.player_index)];
        gravity *= player->traits.get_movement_traits()->get_gravity();
    } else if (use_actor_gravity) {
        gravity *= biped_get_scale(biped_index);
    }
    // match: 0x36df68: reading physics.flags directly moves the incoming force_flag from r28 to r29.
    force_flag |= (motor_system_biped_update_input_word<32>(definition->physics.flags) & 8) != 0;
    float acceleration_scale =
        TEST_BIT(biped_flags, 11) ? 1.0f + unit_compute_boost_fraction(biped_index) * (definition->unit.boost_peak_power - 1.0f) : 1.0f;
    bool stun_active;
    if (stun_fraction > 0.0f)
        stun_active = true;
    else
        stun_active = false;
    bool facing_constrained;
    if (animation_manager->get_state_name() == 15 || animation_manager->get_state_name() == 16)
        facing_constrained = true;
    else
        facing_constrained = false;
    bool animation_disabled = motor_system_biped_initialize_flag_test(unit_animation_flags, 2);
    arguments.position = &origin;
    c_character_physics_component *component = motor_system_biped_initialize_component_input(
        biped_index, input, arguments, biped, &definition->physics, biped->object.havok_component_index, animation_manager,
        scale, _havok_group_biped,
        motor_system_biped_initialize_flag_test(&biped->object.physics_flags, 7),
        facing_constrained,
        biped_flags,
        animation_disabled,
        motor_system_biped_initialize_flag_test(&biped->object.physics_flags, 9),
        use_actor_gravity,
        motor_system_biped_initialize_flag_test(&biped->biped.flags, 10),
        component_flag,
        stun_active,
        force_flag,
        force_flag,
        motor_system_biped_initialize_flag_test(&biped->biped.flags, 14),
        gravity,
        acceleration_scale,
        &biped->object.forward, &biped->object.up, &biped->unit.facing_vector, &aiming, &linear_velocity, shape);
    switch (animation_owner->biped.physics.get_mode()) {
    case c_character_physics_component::_mode_ground:
        character_physics_update_input_datum_initialize_ground(input, component, stunned_state, animation_manager->get_mode_name() == 137,
            motor_system_biped_initialize_flag_test(&biped->biped.flags, 2), stun_fraction, biped->unit.crouch_amount);
        if (definition->physics.ground.ground_adhesion_velocity_scale > 0.0f)
            input->ground_adhesion = definition->physics.ground.ground_adhesion_velocity_scale;
        break;
    case c_character_physics_component::_mode_flying:
        character_physics_update_input_datum_initialize_flying(biped_index, input, component, flying_friction, biped->unit.crouch_amount);
        break;
    case c_character_physics_component::_mode_dead:
        motor_system_biped_update_input_initialize_dead(input, animation_owner->biped.physics.get_mode_dead(), true, dead_size_reset);
        break;
    case c_character_physics_component::_mode_melee: {
        long target_index = motor_system_biped_update_input_word<0>(animation_owner->biped.physics.get_mode_melee()->get_target_unit_index());
        float radius = definition->physics.radius;
        union { real_point3d target_position; long target_position_words[3]; };
        target_position = origin;
        real_vector3d target_velocity = *global_zero_vector3d;
        float target_radius;
        float distance = 0.0f;
        target_radius = 0.0f;
        bool movement = false;
        if (target_index != -1) {
            s_object_header *target_header = object_header_get(target_index);
            if (((1 << target_header->object_type) & 1) != 0 && !motor_system_biped_input_test_flag(target_header->datum->damage_flags, 2)) {
                s_biped_melee_target_parameters parameters;
                biped_calculate_melee_target_parameters(biped_index, target_index, &biped->unit.aiming_vector, &parameters);
                target_position_words[0] = motor_system_biped_update_input_word<16>(parameters.target_position_words[0]);
                target_position_words[1] = parameters.target_position_words[1];
                target_position_words[2] = parameters.target_position_words[2];
                target_radius = parameters.target_radius;
                distance = parameters.distance;
                target_velocity = parameters.target_velocity;
                movement = true;
            }
        }
        motor_system_biped_update_input_initialize_melee(input, component, movement, &target_position, &target_velocity, radius, target_radius, distance);
        break;
    }
    case c_character_physics_component::_mode_sentinel:
    case c_character_physics_component::_mode_sentinel_b: {
        real_point3d position;
        real_vector3d velocity;
        biped_get_sentinel_animation_node_position_and_velocity(biped_index, &position, &velocity);
        real_point3d rigid_body_position;
        motor_system_biped_havok_rigid_body_position(havok_component, 0, &rigid_body_position);
        motor_system_biped_update_input_initialize_sentinel(
            input, component, motor_system_biped_get_active_animation_id(animation_manager), &rigid_body_position, &position, &velocity);
        break;
    }
    default:
#if defined(_XBOX) && !defined(__clang__)
        // match: 0x36df68: a normal default inserts cmplwi and bgt before the physics-mode jump table.
        __assume(0);
#else
        break;
#endif
    }
    real_vector3d old_forward = biped->object.forward;
    if (!motor_system_biped_input_test_flag(motor_system_biped_update_input_word<1>(biped->object.physics_flags), 4)) {
        motor_system_biped_input_snap_facing(component, &biped->object.forward, &biped->object.up, input);
    }
    object_adjust_node_orientations_for_facing_change(biped_index, &old_forward, &biped->object.forward);
}

static __forceinline bool motor_system_biped_animation_flag(const unsigned short *animation_flags)
{
    unsigned short flags = *animation_flags;
    if (flags & 4)
        return true;
    return false;
}

// @address 0xe6c18 ?motor_system_biped_physics_preupdate_moving@@YA_NJPAUs_character_physics_update_input_datum@@PAUs_character_physics_update_output_datum@@_N2M22@Z
bool motor_system_biped_physics_preupdate_moving(long object_index, s_character_physics_update_input_datum *input,
    s_character_physics_update_output_datum *output, bool b0, bool b1, float scale, bool b2, bool b3)
{
    biped_datum *biped = ((biped_datum *)object_get_and_verify_type(object_index, 1));
    const unsigned short *unit_animation_flags = (const unsigned short *)((const char *)biped + biped->unit.animation_storage.offset);
    c_animation_manager *animation = (c_animation_manager *)((char *)biped + biped->object.animation.offset);
    c_motor_program_state *motor = (c_motor_program_state *)((char *)biped + biped->unit.motor_offset);
    bool result = false;
    biped_initialize_character_physics_update_input(object_index, input, b0, b1, scale, b2, b3);
    motor->state &= ~2;
    if (!motor_system_biped_input_test_flag(biped->object.physics_flags, 9)
        || !motor_system_biped_input_test_flag(biped->object.damage_flags, 2)
        || (!animation->paused() && !animation->state_channel_finished()
            && !motor_system_biped_animation_flag(unit_animation_flags))) {
        if (motor_system_biped_input_test_flag(biped->biped.flags, 6)) {
            point_from_line2d(&input->acceleration.xy,
                &biped->biped.velocity, 1.0f, &input->acceleration.xy);
            biped->biped.velocity = k_real_math_zero_vector3d_storage.xy;
        }
        c_character_physics_component *physics = &biped->biped.physics;
        physics->update(output, input);
        real_vector3d old_facing = biped->object.forward;
        biped->object.forward = output->facing;
        input->forward = biped->object.forward;
        unsigned long physics_flags = biped->object.physics_flags;
        biped->object.up = output->up;
        if (!motor_system_biped_input_test_flag(physics_flags, 4)) {
            switch (physics->get_mode()) {
            case c_character_physics_component::_mode_ground:
                physics->get_mode_ground()->snap_facing(&biped->object.forward, &biped->object.up, input);
                break;
            case c_character_physics_component::_mode_flying:
                physics->get_mode_flying()->snap_facing(&biped->object.forward, &biped->object.up, input);
                break;
            case c_character_physics_component::_mode_dead:
                physics->get_mode_dead()->snap_facing(&biped->object.forward, &biped->object.up, input);
                break;
            case c_character_physics_component::_mode_sentinel:
            case c_character_physics_component::_mode_sentinel_b:
            case c_character_physics_component::_mode_melee:
                break;
            default:
                // match: 0xe6c18: unreachable hint omits dead branch check
                __assume(0);
            }
        }
        object_adjust_node_orientations_for_facing_change(object_index, &old_facing, &biped->object.forward);
        result = true;
    }
    return result;
}

static __forceinline c_character_physics_component::e_mode motor_system_biped_physics_mode(
    c_enum<c_character_physics_component::e_mode, unsigned char, 0, 7> mode)
{
    return mode;
}



static __forceinline void motor_system_biped_melee_bounding_sphere(const object_datum *object, real_point3d *center, float *radius)
{
    *center = object->bounding_center;
    *radius = object->bounding_radius;
}


// match: 0x36f0d0: keep the forwarding boundary; calling the extern directly shifts 26 words in 17 runs under the full image.
static __forceinline bool motor_system_biped_collision_test_vector_inline(s_collision_test_flags flags, const real_point3d *point,
    const real_vector3d *vector, long first_ignore_object_index, long second_ignore_object_index, collision_result *collision)
{
    return collision_test_vector(flags, false, point, vector, first_ignore_object_index, second_ignore_object_index, -1, collision);
}

// @address 0x36f0d0 ?biped_can_melee_target_unit@@YA_NJJ@Z
bool biped_can_melee_target_unit(long biped_index, long target_index)
{
    biped_datum *biped =
        (biped_datum *)object_get_and_verify_type(biped_index, 1);
    bool result = true;
    if (biped->unit.player_index != -1) {
        s_object_header *target_header = object_header_get(target_index);
        if (((1 << target_header->object_type) & 1) != 0) {
            const biped_definition *definition =
                TAG_GET('bipd', const biped_definition, target_header->datum->definition_index);
            real_point3d point;
            float height;
            long count;
            if ((definition->physics.flags & 1) != 0) {
                float radius;
                motor_system_biped_melee_bounding_sphere(target_header->datum, &point, &radius);
                count = 1;
                height = 0.0f;
            } else {
                float offset;
                biped_get_melee_target_physics_pill(target_index, &point, &height, &offset);
                count = 3;
            }
            real_point3d camera;
            unit_get_camera_position(biped_index, &camera);
            result = false;
            for (long i = 0; i < count; i++) {
                collision_result collision;
                real_vector3d direction;
                collision.global_material.m_index = -1;
                direction.i = point.x - camera.x;
                direction.j = point.y - camera.y;
                direction.k = point.z - camera.z;
                if (!motor_system_biped_collision_test_vector_inline(
                        k_biped_ground_collision_flags, &camera, &direction, biped_index, target_index, &collision)) {
                    result = true;
                    break;
                }
                if (count > 1) {
                    long long divisor = (long long)(count - 1);
                    point.z = point.z + height / (float)divisor;
                }
            }
        }
    }
    return result;
}

// @address 0x36f448 ?biped_ground_plane_fix_transform_from_physics@@YAXJPAUreal_matrix4x3@@@Z
void biped_ground_plane_fix_transform_from_physics(long biped_index, real_matrix4x3 *transform)
{
    biped_datum *biped = ((biped_datum *)object_get_and_verify_type(biped_index, 1));
    if (biped->object.havok_component_index != -1) {
        c_character_physics_component::e_mode mode = motor_system_biped_physics_mode((c_character_physics_component::e_mode)biped->biped.physics.m_mode);
        if (mode == c_character_physics_component::_mode_ground) {
            const real_vector3d *plane = biped->biped.physics.get_mode_ground()->get_ground_plane();
            biped_definition *definition = ((biped_definition *)tag_get(1651077220, biped->object.definition_index));
            float offset = definition->physics.radius;
            real_vector3d left;
            motor_system_biped_cross_product3d_inline(plane, &transform->forward, &left);
            if (normalize3d(&left) == 0.0f) {
                motor_system_biped_cross_product3d_inline(&biped->object.forward, &transform->forward, &left);
            }
            real_vector3d facing;
            motor_system_biped_cross_product3d_inline(&left, plane, &facing);
            normalize3d(&facing);
            real_point3d position = transform->position;
            matrix4x3_rotation_from_vectors(transform, &facing, plane);
            transform->position = position;
            motor_system_biped_point3d_from_line3d(&transform->position, global_up3d, offset, &transform->position);
            motor_system_biped_point3d_from_line3d(&transform->position, plane, -offset, &transform->position);
        }
    }
}

// @rdata 0x82029354..0x82029360
static const short k_motor_system_biped_opposite_facing[6] = { 1, 0, 3, 2, 4, 0 };
// @rdata-end
// @address 0xe6440 ?biped_update_desired_facing@@YAXJ_NPAFPATreal_vector3d@@PA_N@Z
void biped_update_desired_facing(long biped_index, bool use_up, short *facing_direction_ref, real_vector3d *movement_vector_ref, bool *snap_facing_ref)
{
    biped_datum *biped = (biped_datum *)object_get(biped_index);
    real_vector3d *desired_facing = &biped->unit.throttle;
    *snap_facing_ref = false;
    short facing_direction = *facing_direction_ref;
    real_vector3d facing;
    real_vector3d aiming;
    real_vector3d movement;
    real_vector3d axes[2];
    biped_get_control_vectors(biped_index, use_up, &facing, &aiming, &movement);
    if (fabsf(desired_facing->i) > 0.0f || fabsf(desired_facing->j) > 0.0f || fabsf(desired_facing->k) > 0.0f)
    {
        if ((biped->unit.control_flags & 0x10000) != 0)
        {
            biped_move_calculate_controlled_by_aiming(biped_index, &movement, &aiming, &facing, use_up, &biped->unit.facing_vector, &facing_direction);
            biped_build_axes_from_desired_facing(&biped->unit.facing_vector, &biped->object.forward, use_up ? &biped->object.up : 0, axes);
            float forward_amount = dot_product3d(&movement, &axes[0]);
            float left_amount = dot_product3d(&movement, &axes[1]);
            desired_facing->i = forward_amount;
            desired_facing->j = left_amount;
            desired_facing->k = 0.0f;
            limit3d(desired_facing, 1.0f);
            if (biped->unit.player_index == -1 &&
                !motor_animation_resource_busy(biped_index, _motor_resource_0) &&
                dot_product3d(&movement, movement_vector_ref) > 0.95f &&
                *facing_direction_ref != -1 &&
                *facing_direction_ref != facing_direction &&
                *facing_direction_ref != k_motor_system_biped_opposite_facing[facing_direction])
            {
                *snap_facing_ref = true;
            }
        }
        else
        {
            facing_direction = 0;
        }
    }
    else
    {
        facing_direction = -1;
    }
    *movement_vector_ref = movement;
    *facing_direction_ref = facing_direction;
}
static c_havok_contact_point *motor_system_biped_pathfinding_contact_get(long component_index,
    const s_havok_contact_point_reference *contact_reference)
{
    if (component_index == -1)
        return 0;
    long body_index = contact_reference->rigid_body_index;
    c_havok_component *component =
        datum_get<c_havok_component>(g_havok_component_data, component_index);
    if (body_index < 0 || body_index >= component->m_rigid_bodies.m_size)
        return 0;
    long contact_index = contact_reference->contact_point_index;
    if (contact_index < 0)
        return 0;
    c_havok_component::c_rigid_body *body = &component->m_rigid_bodies[body_index];
    if (contact_index >= body->m_contact_points.getSize())
        return 0;
    return body->get_contact_point(contact_index);
}

// @address 0x10d7c8 ?biped_update_pathfinding_cache@@YAXJPAUs_character_physics_move_output_datum@@@Z
void biped_update_pathfinding_cache(long object_index, s_character_physics_move_output_datum *output)
{
    biped_datum *biped = ((biped_datum *)object_get_and_verify_type(object_index, 1));
    if (biped->object.havok_component_index == -1
        || motor_system_biped_input_test_flag(biped->object.physics_flags, 9)
        || (unsigned short)output->contact.rigid_body_index == 0xffff)
        return;
    long component_index = ((biped_datum *)object_get_and_verify_type(object_index, 1))->object.havok_component_index;
    c_havok_contact_point *contact = motor_system_biped_pathfinding_contact_get(component_index, &output->contact);
    if (!contact)
        return;
    unsigned long identifier = contact->m_environment_shape_key;
    bool found = false;
    if (identifier != 0xffffffff) {
        long type = (long)(identifier >> 29);
        long index = (long)(identifier & 0xffff);
        switch (type) {
        case 3: {
        biped->biped.pathfinding_instanced_geometry_index = index;
        long material = contact->m_structure_bsp_index;
        biped->biped.pathfinding_surface_index = (long)((identifier >> 16) & 0x1fff);
        biped->biped.pathfinding_object_index = -1;
        found = true;
        biped->biped.pathfinding_bsp_reference = -1;
        biped->biped.pathfinding_structure_index = material;
        biped->biped.pathfinding_sector.clear();
        break;
        }
        case 1: {
        biped->biped.pathfinding_instanced_geometry_index = -1;
        long material = contact->m_structure_bsp_index;
        biped->biped.pathfinding_surface_index = index;
        biped->biped.pathfinding_object_index = -1;
        found = true;
        biped->biped.pathfinding_bsp_reference = -1;
        biped->biped.pathfinding_structure_index = material;
        biped->biped.pathfinding_sector.clear();
        break;
        }
        default:
            break;
        }
    }
    if (!found && contact->m_other_object_index != -1) {
        object_datum *other =
            (object_datum *)((biped_datum *)object_get_and_verify_type(contact->m_other_object_index, 1));
        if (motor_system_biped_input_test_flag(other->object_flags, 22)) {
            biped->biped.pathfinding_instanced_geometry_index = -1;
            biped->biped.pathfinding_object_index = contact->m_other_object_index;
            biped->biped.pathfinding_structure_index = contact->m_structure_bsp_index;
            biped->biped.pathfinding_surface_index = -1;
            biped->biped.pathfinding_bsp_reference = -1;
            biped->biped.pathfinding_sector.clear();
        }
    }
    biped->biped.ground_point = contact->m_position;
    biped->biped.ground_normal = contact->m_normal;
    normalize3d_with_default(&biped->biped.ground_normal, &k_real_math_identity_matrix_storage.up);
    biped->biped.m_flags.set(_biped_flag_3, true);
    biped->biped.position = biped->biped.ground_point;
}
