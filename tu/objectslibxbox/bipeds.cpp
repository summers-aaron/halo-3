// objectslibxbox/bipeds.obj: from the records 0x9a938 0x9afa8 0x9d3c8 0xa1698 0xae678 0xb1cb0 0xca7f0 0xe5950 0xe6100 0xe6238 0xe6260 0xe6368 0xe6800 0xe68e8 0xea3a0 0xee690 0xf0500 0x101e28 0x1084b8 0x1098a0 0x109ad8 0x10a580 0x10ab90 0x10ac98 0x10da18 0x10dff8 0x10e458 0x10ebd8 0x110ee8 0x121058 0x122330 0x15c0a8 0x1c1520 0x247e50 0x250320 0x345180 0x3451d0 0x3451d8 0x345248 0x345300 0x3454e0 0x3456f8 0x345970 0x3459e8 0x345c58 0x345d50 0x345e80 0x345fc8 0x346220 0x3463c0 0x346428 0x346538 0x346660 0x346ba8 0x346e00 0x347080 0x347268 0x347328 0x347590 0x347670 0x3476e8 0x347760 0x3477e8 0x3479c0 0x347d88 0x347e50 0x347f68 0x347fc8 0x3481d0 0x35bcd0
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "physics/physics_constants_tls.h"
#include "physics/character_physics_mode_melee.h"
#include "math/real_math_constants.h"
#include "units/bipeds.h"
#include "units/unit_animation_types.h"
#include "ai/actor_type_flood_types.h"
#include "physics/havok_group_types.h"
#include "units/weapon_addition_method_types.h"
#include "math/real_math_globals.h"
#include "math/matrix_math_functions.h"
#include "math/real_math_functions.h"
#include "units/biped_globals.h"
#include "objects/object_reference_types.h"
#include "units/biped_seat_impulse_types.h"
#include "game/game_globals_storage.h"
#include "math/matrix_math_types.h"
#include "math/real_math_types.h"
#include "math/point_math.h"
#include "math/real_math_limit.h"
#include "animations/animation_id.h"
#include "objects/object_datum.h"
#include "objects/object_hierarchy.h"
#include "objects/object_placement.h"
#include "units/motor_enums.h"
#include "units/motor_program_state.h"
#include "tag_files/tag_types.h"
#include "tag_files/string_ids.h"
#include "units/motor_request.h"
#include "phys/phys_object.h"
#include "scenario/scenario_tag.h"
#include "cseries/cseries.h"
#include <math.h>
#include <string.h>
#include <ppcintrinsics.h>
#include "game/game_globals.h"
#include "ai/sector_ref.h"
#include "motor/motor_animation.h"
#include "objects/objects.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "units/units.h"
#include "ai/ai.h"
#include "ai/event_handling.h"
#include "cutscene/recorded_animations.h"
#include "math/periodic_functions.h"
#include "models/models.h"
#include "motor/motor_system.h"
#include "motor/motor_system_biped.h"
#include "motor/mover.h"
#include "objects/damage.h"
#include "objects/damage_owner.h"
#include "physics/havok.h"
#include "physics/havok_component.h"
#include "physics/havok_halo_external.h"
#include "physics/havok_utilities.h"
#include "scenario/scenario_kill_trigger_volumes.h"
#include "units/unit_animation.h"
#include "motor/action_system.h"
#include "physics/water_physics.h"
#include "sound/game_sound.h"
#include "ai/character_definitions.h"
#include "animations/animation_manager.h"
#include "animations/animation_manager_mixing_board_inline.h"
#include "animations/mixing_board/mixing_board_slider.h"
#include "motor/motor_programs.h"
#include "scenario/scenario.h"
#include "objects/object_early_movers.h"
#include "units/biped_types.h"
#include "units/biped_definitions.h"
#include "physics/havok_math.h"
#include "physics/havok_constants_types.h"
#include "motor/motor_program_definition.h"
#include "models/render_model_definitions.h"
#include "animations/mixing_board/channels/auxilary_channel.h"
#include "game/player_types.h"
#include "units/vehicle_datum.h"
#include "ai/actor_datum.h"
#include "ai/actor_globals.h"
#include "memory/tls_globals.h"
#include "structures/structure_runtime_resources.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "ai/sector_geometry.h"
#include "motor/contact_point_types.h"
#include "units/biped_melee_target_parameters_types.h"
#include "units/slider_movement_pattern_types.h"
#include "units/scenario_unit_datum_types.h"
#include "cache/cache_files.h"
#include "physics/collisions.h"
#include "units/slider_target_position_types.h"
#include "camera/observer_proximity.h"
#include "animations/mixing_board/mixing_board_definition.h"
#include "animations/mixing_board/mixing_board_slider_types.h"
#include "animations/animation_playback_flags.h"
#include "animations/mixing_board/channels/aiming_channel.h"
#include "physics/havok_ray_test_types.h"
#include "memory/data_types.h"
#include "memory/data_typed_access.h"
#include "memory/datum_index.h"
#include "memory/tls_data_arrays.h"
#include "game/game_time_types.h"
#include "items/weapon_definitions.h"
#include "physics/character_physics_capsule_types.h"
#include "game/materials.h"
#include "units/unit_definitions.h"
#include "structures/structure_bsp_tag.h"
#include "math/real_math.h"
#include "objects/object_header.h"
#include "physics/character_physics_mode_dead.h"
#include "physics/character_physics_mode_sentinel.h"
#include "physics/character_physics_mode_ground_datum.h"
#include "camera/following_camera.h"
#include "phys/phys_base.h"
#include "phys/phys_constraint.h"
#include "phys/phys_dynamics.h"
#include "physics/havok_component_class.h"
#include "physics/physics_model_constraint_types.h"
#include "phys/phys_collide.h"
#include "math/real_constant_pool.h"
#include "units/vehicle_definitions.h"
#include "physics/physics_constants_types.h"
#include "physics/physics_constants_globals.h"
#include "phys/phys_ragdoll_data.h"
#include "phys/phys_hinge_data.h"
#include "phys/phys_limited_hinge_data.h"
#include "phys/phys_ball_socket_data.h"
#include "phys/phys_stiff_spring_data.h"
#include "phys/phys_prismatic_data.h"
#include "physics/runtime_node_to_havok_component_rigid_body_index_mapping.h"
#include "units/unit_animation_storage_types.h"
#include "math/matrix_math_constants.h"
#include "physics/collision_types.h"
#include "physics/collision_test_flags.h"
#include "physics/collision_vector_query.h"
#include "multithreading/thread_tls.h"
#include "game/game_time_inlines.h"
#include "game/game_time_inline.h"
#include "animations/ik_point_iterator.h"
#include "animations/animation_search_path_iterator.h"
#include "models/model_types.h"
#include "animations/animation_event_data.h"
#include "effects/material_effect_types.h"
#include "game/material_constants.h"
#include "effects/material_effects.h"
#include "effects/effects.h"
#include "render/water_renderer.h"
#include "units/unit_weapon_pickup_result.h"
#include "objects/damage_data.h"
#include "game/falling_damage_types.h"
#include "objects/object_soft_ceiling.h"
#include "objects/damage_inline.h"
#include "tag_files/tag_groups.h"
#include "units/unit_inventory_inline.h"
#include "objects/object_type_access.h"
#include "objects/object_animation_manager_inline.h"
#include "objects/object_node_matrices_inline.h"
#include "motor/mover_inline.h"
#include "scenario/scenario_access.h"
#include "scenario/cluster_reference_helpers.h"
#include "physics/havok_component_datum_access.h"
#include "game/game_simulation_predicates.h"
#include "scenario/scenario_objects.h"

enum e_biped_default_movement_type { _biped_default_movement_none, _biped_default_movement_ground, _biped_default_movement_flying };

struct s_bipeds_footstep_water_interaction { real_vector3d velocity; real_vector3d direction; };

struct s_planar_rotation
{
    float cosine;
    float sine;
};

// @address 0x79dbb8 global_lean_turn_angle
float global_lean_turn_angle = 0.52359879f;
// @address 0x79dbb4 global_lean_angle
float global_lean_angle = 0.34906584f;
// @address 0x73d02c global_lean_pivot_offset
float global_lean_pivot_offset = 1.3f;
// @address 0x73ddf4 g_mixing_board_component_sizes
long g_mixing_board_component_sizes[5] = { 12, 52, 24, 52, 92 };

// @address 0x265bc k_biped_aiming_vector_minimum_magnitude
// @address 0xae678 ?normalize2d@@YAMPATreal_vector2d@@@Z
// @address 0xe6100 ?biped_should_remain_active@@YA_NJ@Z
// @address 0xe6260 biped_update_aiming_channel
// @address 0xe6368 ?export_real@?$t_short_fixed@$0N@@@QBAMXZ
// @address 0xea3a0 ?biped_compute_function_value@@YA_NJJJPAMPA_N1@Z
// @address 0x1084b8 ?biped_calculate_invisible_crouched_uber_melee_z_offset@@YA_NJPAM@Z
// @address 0x10e458 ?unit_animation_allows_vehicle_ik@@YA_NJ@Z
// @address 0x10ebd8 biped_get_camera_velocities
// @address 0x15c0a8 ?unit_animation_eye_tracking_in_progress@@YA_NJ@Z
// @address 0x3463c0 biped_get_scale
// @address 0x347268 ?biped_get_current_weapon@@YAJJ@Z
// @address 0x347d88 ?biped_get_melee_target_physics_pill@@YAXJPATreal_point3d@@PAM1@Z
// @address 0x347e50 biped_is_melee_target_in_range
// @address 0x347f68 weapon_is_energy_blade
// @address 0x345180 ?biped_dispose_from_old_structure_bsp@@YAXJ@Z
// @address 0x3451d8 ?biped_adjust_placement@@YAXPAUobject_placement_data@@@Z
// @address 0x110ee8 ?biped_handle_deleted_object@@YAXJJ@Z
// @address 0x247e50 ?get_constraint_rigid_bodies@c_havok_component@@QAAXJPAJ0@Z
// @address 0x1098a0 ?biped_get_physics_model_node_matrix_z_offset@@YAMJ@Z
// @address 0xe6238 ?biped_post_update@@YA_NJ@Z
// @address 0x345c58 ?biped_approximate_surface_index@@YA_NJJJPATreal_point3d@@PAFPAJ22PAK@Z
// @address 0x1c1520 ??1phys_single_shape_container@@UAA@XZ


class phys_world_object;


// @address 0x3aa90 k_real_math_origin3d

/* ---- object and tag data ---- */


typedef char bipeds_weapon_reload_interrupt_offset[(offsetof(weapon_definition, reload_interrupt_mode) == 0x406) ? 1 : -1];


typedef char bipeds_bsp_instance_count_offset[(offsetof(structure_bsp, instanced_geometry_instances.count) == 0x1b0) ? 1 : -1];


/* ---- vehicle ik animation (0x10e458) ---- */


/* ---- character physics (c_character_physics_component at biped + 0x4c0) ---- */


static __forceinline float bipeds_ragdoll_z_offset_fraction(c_character_physics_mode_dead_datum *datum)
{
    float fraction = 0.0f;
    if (datum->m_ragdoll_start_time != NONE)
    {
        long elapsed_ticks = game_time_get() - datum->m_ragdoll_start_time;
        float elapsed_seconds = game_ticks_to_seconds((float)elapsed_ticks);
        float t = elapsed_seconds / 0.15f;
        fraction = MIN(MAX(1.0f - t, 0.0f), 1.0f);
    }
    return fraction;
}

static __forceinline void bipeds_set_death_info(c_character_physics_mode_dead_datum *datum, long node_index, const real_vector3d *vector)
{
    datum->m_death_node_index = node_index;
    datum->m_death_time = game_time_get();
    datum->m_death_node_acceleration = *vector;
}


static __forceinline long bipeds_get_impulse_name(const c_channel_base *object) { return object->m_definition_index; }


class c_auxilary_channel;
template<long k_bits> class t_short_fixed;
// @address 0x79d7dc k_mixing_board_slider_targets
static __forceinline bool bipeds_weapon_is_valid(const c_animation_manager *object)
{
    return object->m_current_goal.weapon_class != NONE && object->m_current_goal.weapon_type != NONE;
}


static __forceinline float bipeds_biped_calculate_uber_melee_z_offset_per_tick()
{
    return game_tick_length() * 0.15f;
}

// 0xe6100
bool biped_should_remain_active(long biped_index)
{
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, 1);
    bool result = false;
    if (motor_system_is_relaxed(biped_index) == 0)
    {
        if (biped->biped.physics.get_mode() == c_character_physics_component::_mode_melee)
        {
            result = true;
        }
        else if (biped->biped.physics.get_mode() != c_character_physics_component::_mode_dead)
        {
            if (fabsf(magnitude_squared3d(&biped->unit.throttle) - 0.0f) >= 0.0001f || biped->biped.physics.is_sentinel_mode() ||
                motor_system_in_airborne_state(biped_index))
                result = true;
        }
    }
    return result;
}

// 0xe6260
void biped_update_aiming_channel(long biped_index)
{
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, NONE);
    c_animation_manager *animation = (c_animation_manager *)((char *)biped + biped->object.animation.offset);
    const c_mixing_board_definition *definition = 0;
    if (animation->m_mixing_board_type <= 2)
        definition = &g_mixing_board_definition_list[animation->m_mixing_board_type];
    c_aiming_channel *channel = (c_aiming_channel *)((char *)animation + ((c_mixing_board_definition::get_component_size(3) + 44) << 2)
        + definition->component_types[3].offset);
    if (channel->valid())
    {
        real_euler_angles2d angles = *global_zero_angles2d;
        if (biped->biped.aiming_vector_magnitude > 0.0f)
        {
            euler_angles2d_from_vector3d(&angles, &biped->biped.aiming_vector);
        }
        channel->set_yaw(angles.yaw);
        channel->set_pitch(angles.pitch);
    }
}

// 0xe6238
bool biped_post_update(long biped_index)
{
    biped_update_aiming_channel(biped_index);
    return false;
}

// 0xe6368
template float t_short_fixed<13>::export_real() const;

// 0xea3a0
bool biped_compute_function_value(long biped_index, long function_type, long unused_argument, float *value_out, bool *positive_out, bool *deterministic)
{
    bool handled = false;
    float zero = 0.0f;
    float one = 1.0f;
    float value = zero;
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, NONE);
    biped_definition *definition = (biped_definition *)tag_get('bipd', biped->object.definition_index);
    if (function_type <= 482)
    {
        if (function_type != 482)
        {
            if (function_type != 186)
            {
                if (function_type == 476)
                {
                    if (biped->biped.physics.get_mode() == c_character_physics_component::_mode_flying)
                        value = one;
                    handled = true;
                }
            }
            else
            {
                bool stunned = false;
                if (biped->object.object_type == 0)
                    stunned = (biped->biped.stun_ticks <= 0 ? 0 : -1) & 1;
                if ((unsigned int)stunned != 0)
                {
                    value = one;
                    handled = true;
                }
            }
        }
        else
        {
            value = (float)(double)(signed char)biped->biped.combat_status * 0.1f;
            handled = true;
        }
    }
    else if (function_type == 570)
    {
        float speed = definition->physics.flying.maximum_velocity;
        if (speed > zero)
        {
            value = sqrt(biped->object.linear_velocity.k * biped->object.linear_velocity.k + (biped->object.linear_velocity.i *
                biped->object.linear_velocity.i + biped->object.linear_velocity.j * biped->object.linear_velocity.j)) / speed;
            handled = true;
        }
    }
    if (handled)
    {
        *value_out = MIN(MAX(value, zero), one);
        *positive_out = value > zero;
    }
    return handled;
}

// 0x1084b8
bool biped_calculate_invisible_crouched_uber_melee_z_offset(long biped_index, float *z_offset)
{
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, 1);
    c_character_physics_component *physics = &biped->biped.physics;
    bool result = false;
    long offset = 0;
    if (physics->get_mode() == c_character_physics_component::_mode_melee)
        offset = physics->get_mode_melee()->get_exit_invisible_crouched_uber_melee_offset();
    if (offset != 0)
    {
        *z_offset = (float)offset * bipeds_biped_calculate_uber_melee_z_offset_per_tick();
        result = true;
    }
    return result;
}


// 0x15c0a8
bool unit_animation_eye_tracking_in_progress(long unit_index)
{
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 0x2003);
    unit_definition *definition = (unit_definition *)tag_get('unit', unit->object.definition_index);
    bool result = false;
    if (!(bool)((definition->flags >> 11) & 1) && ((c_animation_manager *)object_header_block_get(object_get(unit_index),
        &unit->object.animation))->valid()
        && ((unit->unit.control_flags & 0x8000) != 0 || (bool)((unit->unit.unit_flags >> 23) & 1))
        && (unit->unit.left_eye_marker != NONE || unit->unit.right_eye_marker != NONE))
    {
        result = true;
    }
    return result;
}


// @address 0x3459e8 ?biped_get_movement_ground_direction@@YA_NJPATreal_point3d@@PATreal_vector3d@@@Z

static __forceinline c_character_physics_component::e_mode bipeds_mover_get_physics_mode(long object_index)
{
    biped_datum *object = (biped_datum *)object_get_and_verify_type(object_index, 0x3003);
    c_character_physics_component::e_mode mode = c_character_physics_component::_mode_none;
    if (object->object.object_type == 0)
        mode = object->biped.physics.get_mode();
    return mode;
}

bool biped_get_movement_ground_direction(long biped_index, real_point3d *point, real_vector3d *vector)
{
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, 1);
    c_character_physics_component::e_mode mode = bipeds_mover_get_physics_mode(biped_index);
    long motor_program = mover_get_motor_program(biped_index);
    bool result = false;
    if (motor_program == 5)
    {
        scale_vector3d(&biped->object.forward, -1.0f, vector);
        result = true;
    }
    else if (motor_program == 7)
    {
        scale_vector3d(&biped->object.up, -1.0f, vector);
        point_from_line3d(point, &biped->object.up, 0.05f, point);
        result = true;
    }
    else if (mode == c_character_physics_component::_mode_sentinel)
    {
        if (action_executing(biped_index, _action_hoist))
        {
            object_get_origin(biped_index, point);
            point_from_line3d(point, &biped->object.forward, 0.05f, point);
            *vector = *global_down3d;
            result = true;
        }
        else
        {
            c_character_physics_mode_sentinel_datum *sentinel = biped->biped.physics.get_mode_sentinel();
            if (sentinel != 0)
            {
                const real_point3d *sentinel_position = sentinel->get_sentinel_physics_position();
                *point = *sentinel_position;
                *vector = *global_down3d;
                result = true;
            }
        }
    }
    if (!result)
    {
        if ((bool)((biped->biped.flags >> 3) & 1))
        {
            scale_vector3d(&biped->biped.ground_normal, -1.0f, vector);
            *point = biped->biped.ground_point;
        }
        else
        {
            *vector = *global_down3d;
        }
        point_from_line3d(point, global_up3d, 0.05f, point);
        result = true;
    }
    return result;
}

// @address 0x345d50 ?biped_verify_surface_reference@@YA_NFJJJJ@Z
bool biped_verify_surface_reference(short structure_bsp_index, long surface_index, long cluster_index, long object_index, long unused)
{
    bool valid = false;
    if (object_index != NONE)
    {
        valid = object_try_and_get_and_verify_type(object_index, NONE) != 0;
    }
    else if (structure_bsp_index >= 0 && structure_bsp_index < global_scenario_get()->structure_bsps.count &&
        global_structure_bsp_is_active(structure_bsp_index))
    {
        structure_bsp *structure_bsp = global_structure_bsp_get(structure_bsp_index);
        if (cluster_index != NONE)
        {
            valid = cluster_index >= 0 && cluster_index < structure_bsp->instanced_geometry_instances.count;
        }
        else
        {
            s_structure_bsp_resource *resource = &structure_bsp->resource;
            s_structure_bsp_resources *resources = resource->use_resource_items ?
                resource->collision_resource_interface.get_resources() : (s_structure_bsp_resources *)resource->raw_resources.address;
            collision_bsp *collision_bsp = &resources->collision_bsp[0];
            valid = surface_index >= 0 && surface_index < collision_bsp->surfaces.count;
        }
    }
    return valid;
}

// 0x345c58

bool biped_approximate_surface_index(long biped_index, long a, long b, real_point3d *surface_point, short *bsp_index, long *surface_index,
    long *reference_index, long *material_index, unsigned long *flags)
{
    // match: 0x345c58: pointer __restrict qualifier prevents alias reload
    biped_datum *__restrict biped = (biped_datum *)object_get_and_verify_type(biped_index, 1);
    real_point3d position = biped->biped.position;
    real_vector3d direction;
    bool found = false;
    if (biped_get_movement_ground_direction(biped_index, &position, &direction))
    {
        found = biped_find_surface(biped_index, a, b, 2.0f, &position, &direction, surface_point, 0, bsp_index, surface_index,
            reference_index, material_index, flags);
    }
    if (!found)
    {
        *bsp_index = -1;
        *surface_index = -1;
        *material_index = -1;
        *flags = -1;
        *reference_index = -1;
    }
    return found;
}

static __forceinline void bipeds_biped_reset_pathfinding_location(long biped_index);
// @address 0xb1cb0 ?biped_find_pathfinding_location@@YAXJPAFPAJPAVc_sector_ref@@PATreal_point3d@@1PAK@Z

void biped_find_pathfinding_location(
    // match: 0xb1cb0: pointer __restrict qualifier prevents alias reload
    long biped_index, short *, long *, c_sector_ref *__restrict sector_ref, real_point3d *point, long *object_index, unsigned long *flags)
{
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, 1);
    if (biped->biped.physics.get_mode() == c_character_physics_component::_mode_flying)
    {
        biped->biped.pathfinding_structure_index = NONE;
        biped->biped.pathfinding_surface_index = NONE;
        biped->biped.pathfinding_sector.clear();
        biped->biped.pathfinding_instanced_geometry_index = NONE;
        biped->biped.pathfinding_object_index = NONE;
        biped->biped.pathfinding_bsp_reference = NONE;
        object_get_origin(biped_index, point);
    }
    else
    {
        if (biped->biped.pathfinding_surface_index == NONE)
        {
            long game_time = game_time_get();
            if (game_time > biped->biped.pathfinding_time)
            {
                biped->biped.pathfinding_time = game_time;
                real_point3d surface_point = biped->biped.position;
                if (biped->biped.pathfinding_object_index != NONE &&
                    object_try_and_get_and_verify_type(biped->biped.pathfinding_object_index, NONE) == 0)
                    biped->biped.pathfinding_object_index = NONE;
                biped_approximate_surface_index(biped_index, biped->biped.pathfinding_object_index,
                    biped->biped.pathfinding_instanced_geometry_index,
                    &surface_point, &biped->biped.pathfinding_structure_index, &biped->biped.pathfinding_surface_index,
                    &biped->biped.pathfinding_instanced_geometry_index, &biped->biped.pathfinding_object_index,
                        &biped->biped.pathfinding_bsp_reference);
                biped->biped.pathfinding_sector.clear();
                if (biped->biped.pathfinding_surface_index != NONE)
                    biped->biped.position = surface_point;
            }
        }
        else if (!biped_verify_surface_reference(biped->biped.pathfinding_structure_index, biped->biped.pathfinding_surface_index,
                     biped->biped.pathfinding_instanced_geometry_index, biped->biped.pathfinding_object_index,
                         biped->biped.pathfinding_bsp_reference))
        {
            bipeds_biped_reset_pathfinding_location(biped_index);
        }
        bool update_sector = biped->biped.pathfinding_sector == NONE;
        if (update_sector)
        {
            if (biped->biped.pathfinding_surface_index != NONE)
                biped->biped.pathfinding_sector = collision_surface_get_sector(biped->biped.pathfinding_structure_index,
                    biped->biped.pathfinding_surface_index, biped->biped.pathfinding_instanced_geometry_index,
                        biped->biped.pathfinding_object_index,
                    biped->biped.pathfinding_bsp_reference, &biped->biped.position);
            else
            {
                biped->biped.pathfinding_sector.clear();
            }
        }
    }
    if (point)
        *point = biped->biped.position;
    if (object_index)
        *object_index = biped->biped.pathfinding_object_index;
    if (flags)
        *flags = biped->biped.pathfinding_bsp_reference;
    if (sector_ref)
    {
        sector_ref->structure_bsp_index = biped->biped.pathfinding_sector.structure_bsp_index;
        sector_ref->sector_index = biped->biped.pathfinding_sector.sector_index;
    }
}


// @address 0x346220 biped_start_melee_dash
extern "C" void biped_start_melee_dash(long biped_index, long target_unit_index, long unknown, bool forced)
{
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, 1);
    c_character_physics_component *physics = &biped->biped.physics;
    if (physics->get_mode() != c_character_physics_component::_mode_melee && target_unit_index != NONE && target_unit_index != biped_index)
    {
        s_object_header *target_header = object_header_get(target_unit_index);
        if (((1 << target_header->object_type) & 1) != 0)
        {
            biped_datum *target = (biped_datum *)target_header->datum;
            if (forced || biped_can_melee_target_unit(biped_index, target_unit_index))
            {
                real_vector3d velocity;
                object_get_localized_velocities(biped_index, &velocity, 0, 0, 0);
                limit3d(&velocity, 1.5f);
                physics->set_mode(c_character_physics_component::_mode_melee);
                bool unit_flag = (biped->unit.unit_flags >> 22) & 1;
                physics->get_mode_melee()->setup_dash(
                    forced, target->unit.player_index != NONE, !unit_flag, target_unit_index, unknown, &biped->unit.aiming_vector, &velocity);
            }
        }
    }
}


// 0x3463c0
float biped_get_scale(long object_index)
{
    biped_datum *object = (biped_datum *)object_get_and_verify_type(object_index, 1);
    biped_definition *definition = (biped_definition *)tag_get(0, object->object.definition_index);
    float scale = k_real_one;
    if (definition->physics.ground.gravity_scale > k_real_zero)
        scale = definition->physics.ground.gravity_scale;
    if (object->biped.scale > k_real_zero)
        scale *= object->biped.scale;
    return scale;
}

// 0x347268
long biped_get_current_weapon(long unit_index)
{
    long weapon_index = NONE;
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 1);
    if (unit->object.parent_object_index != NONE)
    {
        unit_datum *parent = (unit_datum *)object_get_and_verify_type(unit->object.parent_object_index, NONE);
        if (((1 << parent->object.object_type) & 0x1003) != 0)
        {
            if (parent->unit.gunner_object_index == unit_index)
                weapon_index = unit_get_active_primary_weapon(unit->object.parent_object_index, 0);
        }
    }
    if (weapon_index == NONE)
        weapon_index = unit_inventory_get_weapon_or_none(
            unit_index, ((unit_datum *)object_get_and_verify_type(unit_index, 0x1003))->unit.current_weapon_index);
    return weapon_index;
}

// 0x347d88


void biped_get_melee_target_physics_pill(long biped_index, real_point3d *position, float *height, float *offset)
{
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(biped_index, 1);
    ::biped_get_physics_pill_from_definition(biped_index, position, (float *)(height), (float *)(offset));
    object_marker marker;
    if (internal_object_get_markers_by_string_id(biped_index, 158, &marker, 1, false) == 1 ||
        internal_object_get_markers_by_string_id(biped_index, 157, &marker, 1, false) == 1)
    {
        position->x = marker.matrix.position.x;
        position->y = marker.matrix.position.y;
        if (unit->object.parent_object_index != NONE)
        {
            position->z = marker.matrix.position.z - *height;
        }
        else
        {
            *height = marker.matrix.position.z - position->z;
        }
    }
}

bool biped_is_melee_target_in_range(const s_biped_melee_target_parameters *movement, bool sprinting, bool unarmed)
{
    float speed = movement->distance <= 0.0f ? 0.0f : movement->distance;
    bool result = false;
    if (speed < (sprinting ? 1.6f : 1.05f) && (movement->airborne != 0 || movement->ground_angles.pitch >
        -0.52359879f))
    {
        float yaw_far = unarmed ? 1.04719758f : 0.87266463f;
        float yaw_near = unarmed ? 1.04719758f : 0.34906584f;
        float pitch_limit = unarmed ? 1.39626336f : 0.52359879f;
        float scaled_speed = speed * 0.952381f;
        float yaw_limit = (1.0f - scaled_speed) * yaw_far + scaled_speed * yaw_near;
        result = movement->target_angles.yaw >= -yaw_limit && movement->target_angles.yaw <= yaw_limit && movement->target_angles.pitch
            >= -pitch_limit && movement->target_angles.pitch <= pitch_limit;
    }
    return result;
}

// 0x347f68

bool weapon_is_energy_blade(long weapon_index)
{
    bool result = false;
    if (weapon_index != NONE)
    {
        object_datum *weapon = (object_datum *)object_get_and_verify_type(weapon_index, 4);
        weapon_definition *definition = (weapon_definition *)tag_get(2003132784, weapon->definition_index);
        result = definition->reload_interrupt_mode - 6 == 0;
    }
    return result;
}


// @address 0x10ab90 ?biped_get_physics_pill_from_definition@@YAXJPATreal_point3d@@PAM1@Z
void biped_get_physics_pill_from_definition(long _sym_p0, union real_point3d * position, float * height, float * offset)
{
    unit_datum *object = (unit_datum *)object_get_and_verify_type(((unsigned int)_sym_p0), 1);
    biped_definition *definition = (biped_definition *)tag_get(0, object->object.definition_index);
    object_get_origin(((unsigned int)_sym_p0), position);
    if (!(definition->physics.flags & 1))
        position->z += definition->physics.radius;
    bool fixed = (definition->physics.flags >> 1) & 1;
    if (fixed) {
        *height = k_real_zero;
        *offset = definition->physics.radius;
        return;
    }
    bool raised = (object->unit.unit_flags >> 22) & 1;
    float scale = raised ? k_real_one : k_real_zero;
    *height = definition->physics.height_standing + (definition->physics.height_crouching - definition->physics.height_standing) *
        scale - definition->physics.radius * k_real_two;
    *height = *height > k_real_zero ? *height : k_real_zero;
    *offset = definition->physics.radius;
}

// 0x345180
static __forceinline void bipeds_biped_reset_pathfinding_location(long biped_index)
{
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, 1);
    biped->biped.pathfinding_structure_index = NONE;
    biped->biped.pathfinding_surface_index = NONE;
    biped->biped.pathfinding_sector.clear();
    biped->biped.pathfinding_instanced_geometry_index = NONE;
    biped->biped.pathfinding_object_index = NONE;
    biped->biped.pathfinding_bsp_reference = NONE;
    biped->biped.pathfinding_time = NONE;
}
void biped_dispose_from_old_structure_bsp(long biped_index)
{
    bipeds_biped_reset_pathfinding_location(biped_index);
}

// @address 0x3451d0 ?biped_place@@YAXJPAUs_scenario_biped@@@Z
void biped_place(long biped_index, s_scenario_biped *scenario_biped)
{
    unit_place(biped_index, &scenario_biped->unit);
}

// 0x3451d8

void biped_adjust_placement(object_placement_data *data)
{
    biped_definition *definition = (biped_definition *)tag_get('bipd', data->definition_index);
    if ((bool)((definition->physics.flags >> 0) & 1) && !(bool)((definition->physics.flags >> 4) & 1))
        point_from_line3d(&data->position, &data->up, definition->physics.radius, &data->position);
}


// 0x10e458
bool unit_animation_allows_vehicle_ik(long unit_index)
{
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 0x2003);
    bool result = false;
    if (unit->object.parent_object_index != NONE && unit->unit.parent_seat_index != NONE)
    {
        c_animation_manager *animation = (c_animation_manager *)object_header_block_get(object_get(unit_index), &unit->object.animation);
        long state = animation->m_current_goal.state;
        result = true;
        if (mover_is_in_entering_state(unit_index))
            result = false;
        else switch (state)
        {
            case _string_id_surprise_front: case _string_id_surprise_back: case _string_id_resurrect_front: case _string_id_resurrect_back:
            case _string_id_airborne_dead: case _string_id_landing_dead: case _string_id_exit: case _string_id_h_ping:
            case _string_id_ai_impulse: case _string_id_board: case _string_id_ejection:
            case _string_id_shake_fist: case _string_id_cheer: case _string_id_taunt: case _string_id_brace: case _string_id_point:
            case _string_id_hold: case _string_id_wave: case _string_id_advance: case _string_id_fallback:
                result = false;
                break;
        }
        if (unit_animation_allows_playback_adjustment(unit_index, _animation_playback_complete_bit))
            result = false;
    }
    return result;
}

// 0x10ebd8
extern "C" int biped_get_camera_velocities(long biped_index, real_vector3d *velocity, real_vector3d *camera_velocity)
{
    int result = 0;
    float tick_length = game_tick_length();
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, NONE);
    if (tick_length > 0.0001f)
    {
        if (camera_velocity != 0)
        {
            real_point3d point_a;
            real_point3d point_b;
            following_camera_deterministic((const real_matrix4x3 *)&biped->biped.camera_a, (real_euler_angles3d *)&point_a);
            following_camera_deterministic((const real_matrix4x3 *)&biped->biped.camera_b, (real_euler_angles3d *)&point_b);
            float inv_tick_length = 1.0f / tick_length;
            camera_velocity->i = (point_a.x - point_b.x) * inv_tick_length;
            camera_velocity->j = (point_a.y - point_b.y) * inv_tick_length;
            camera_velocity->k = (point_a.z - point_b.z) * inv_tick_length;
        }
        if (velocity != 0)
        {
            float d[3];
            d[0] = biped->biped.camera_a.position.x - biped->biped.camera_b.position.x;
            d[1] = biped->biped.camera_a.position.y - biped->biped.camera_b.position.y;
            float inv_tick_length = 1.0f / tick_length;
            d[2] = biped->biped.camera_a.position.z - biped->biped.camera_b.position.z;
            velocity->i = inv_tick_length * d[0];
            velocity->j = inv_tick_length * d[1];
            velocity->k = d[2] * inv_tick_length;
        }
        result = 1;
    }
    return result;
}

// 0x110ee8
void biped_handle_deleted_object(long biped_index, long object_index)
{
    c_character_physics_component *physics = &((biped_datum *)object_get_and_verify_type(biped_index, 1))->biped.physics;
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, 1);
    if (physics->get_mode() == c_character_physics_component::_mode_melee && physics->get_mode_melee()->get_target_unit_index() == object_index)
        physics->get_mode_melee()->clear_target_unit_index();
    if (biped->biped.pathfinding_object_index == object_index)
        bipeds_biped_reset_pathfinding_location(biped_index);
}

// 0x247e50
class phys_entity;
class phys_rigid_body;
class phys_constraint_data;
class phys_shape;

static __forceinline short bipeds_position_controlling_rigid_body_index(const c_havok_component *component)
{
    long index = component->m_object_postion_controlling_rigid_body_index;
    return (short)((index >= 0 && index < component->m_rigid_bodies.getSize()) ? index : -1);
}
__forceinline unsigned char *c_havok_component::get_stretched_counter(long constraint_index)
{
    return (unsigned char *)&m_component_constraints[constraint_index].m_header.pad3;
}
__forceinline void c_havok_component::c_constraint::get_connected_rigid_bodies(phys_rigid_body **a, phys_rigid_body **b)
{
    *a = m_contact_constraint->get_rigid_body_a();
    *b = m_contact_constraint->get_rigid_body_b();
}
void c_havok_component::get_constraint_rigid_bodies(long constraint_index, long *rigid_body_a_index, long *rigid_body_b_index)
{
    phys_rigid_body *rigid_body_a = 0;
    phys_rigid_body *rigid_body_b = 0;
    m_component_constraints[constraint_index].get_connected_rigid_bodies(&rigid_body_a, &rigid_body_b);
    *rigid_body_a_index = ::havok_entity_get_havok_component_rigid_body_index(rigid_body_a);
    if (::havok_entity_get_havok_component_index(rigid_body_b) != NONE)
        *rigid_body_b_index = ::havok_entity_get_havok_component_rigid_body_index(rigid_body_b);
    else
        *rigid_body_b_index = NONE;
}


phys_single_shape_container::~phys_single_shape_container()
{
    m_single_child_shape->object_remove_ref();
}

// 0x1098a0
float biped_get_physics_model_node_matrix_z_offset(long biped_index)
{
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, 1);
    float z_offset = 0.0f;
    if (motor_system_is_relaxed(biped_index) == 1 && biped->biped.physics.get_mode() == c_character_physics_component::_mode_dead)
    {
        float ragdoll_fraction = bipeds_ragdoll_z_offset_fraction(biped->biped.physics.get_mode_dead());
        z_offset = -ragdoll_fraction * 0.125f;
    }
    return z_offset;
}


// @address 0x9afa8 ?biped_compute_lean@@YAXJPATreal_point3d@@PATreal_vector3d@@1@Z
void biped_compute_lean(long biped_index, real_point3d *camera_position, real_vector3d *camera_forward, real_vector3d *camera_up)
{
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, 1);
    float lean_input = biped->biped.lean_input;
    long sign;
    if (lean_input != 0.0f)
    {
        if (lean_input < 0.0f)
            sign = -1;
        else
            sign = 1;
    }
    else
    {
        sign = 0;
    }
    float function_value = transition_function_evaluate(1, fabs(lean_input)) * (float)sign;
    float turn = global_lean_turn_angle * function_value;
    float lean = global_lean_angle * function_value;
    float return_angle = lean * -0.6f;
    float cosine_turn = (float)cos(turn);
    float sine_turn = (float)sin(turn);
// match: 0x9afa8: keeps the three rotation calls at 0x9b084, 0x9b0dc, and 0x9b134.
#pragma inline_depth(0)
    rotate_vector_about_axis(camera_forward, camera_up, sine_turn, cosine_turn);
    real_point3d pivot_point;
    pivot_point.x = camera_position->x - camera_up->i * global_lean_pivot_offset;
    pivot_point.y = camera_position->y - camera_up->j * global_lean_pivot_offset;
    pivot_point.z = camera_position->z - camera_up->k * global_lean_pivot_offset;
    float cosine_lean = (float)cos((double)lean);
    float sine_lean = (float)sin((double)lean);
    rotate_vector_about_axis(camera_up, camera_forward, sine_lean, cosine_lean);
    point_from_line3d(&pivot_point, camera_up, global_lean_pivot_offset, camera_position);
    rotate_vector_about_axis(camera_up, camera_forward, (float)sin((double)return_angle), (float)cos((double)return_angle));
// match: 0x9a938: leaving inlining disabled adds a call at 0x9ac30 before the camera marker is initialized.
#pragma inline_depth()
}


#if defined(_XBOX) && !defined(__clang__)
inline float normalize2d(real_vector2d *vector)
#else
float normalize2d(real_vector2d *vector)
#endif
{
    float length = magnitude2d(vector);
    if (fabsf(length) >= 0.0001f)
    {
        scale_vector2d(vector, 1.0f / length, vector);
        return length;
    }
    return 0.0f;
}

static __forceinline real_vector2d *bipeds_perpendicular2d(const real_vector2d *vector, real_vector2d *result)
{
    result->i = -vector->j;
    result->j = vector->i;
    return result;
}

// @address 0xf0500 ?biped_build_2d_camera_frame@@YAXPBTreal_vector3d@@0PATreal_vector2d@@1@Z
inline void biped_build_2d_camera_frame(const real_vector3d *forward, const real_vector3d *up, real_vector2d *forward2d, real_vector2d *left2d)
{
    memcpy(forward2d, forward, sizeof(real_vector2d));
    if (normalize2d(forward2d) == 0.0f)
    {
        memcpy(forward2d, up, sizeof(real_vector2d));
        if (forward->k > 0.0f)
            negate_vector2d(forward2d, forward2d);
        if (normalize2d(forward2d) == 0.0f)
            memcpy(forward2d, &k_real_math_identity_matrix_storage.forward, sizeof(real_vector2d));
    }
    bipeds_perpendicular2d(forward2d, left2d);
}


// @address 0x3456f8 biped_adjust_camera_position
extern "C" void biped_adjust_camera_position(long biped_index, real_point3d *camera_position, const real_vector3d *forward, const real_vector3d *up)
{
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, 1);
    biped_definition *definition = (biped_definition *)tag_get('bipd', biped->object.definition_index);
    float weight = 0.0f;
    if (definition->camera_angle_min < definition->camera_angle_max)
    {
        float range = definition->camera_angle_max - definition->camera_angle_min;
        float z = forward->k;
        float angle;
        if (!(z > -1.0f))
            angle = 1.5707964f;
        else if (z < 0.0f)
            angle = 1.5707964f - (float)acos((double)-z);
        else
            angle = 0.0f;
        float t = (angle - definition->camera_angle_min) / range;
        t = PIN(t, 0.0f, 1.0f);
        weight = transition_function_evaluate(5, t);
    }
    if (!(weight <= 0.0f) && TEST_BIT(biped->biped.flags_bytes[BIG_ENDIAN_VIEW_INDEX(0, 2)], 0))
        {
            real_vector2d forward2d;
            real_vector2d left2d;
            biped_build_2d_camera_frame(forward, up, &forward2d, &left2d);
            biped_datum *biped_after = (biped_datum *)object_get_and_verify_type(biped_index, 1);
            real_matrix4x3 *node_matrices = (real_matrix4x3 *)((char *)biped_after + biped_after->object.node_matrices.offset);
            real_vector3d delta;
            vector_from_points3d(&node_matrices->position, camera_position, &delta);
            real_vector3d offset;
            offset.i = dot_product2d(&delta.xy, &forward2d);
            offset.j = dot_product2d(&delta.xy, &left2d);
            offset.k = delta.k;
            real_vector3d lean = biped->biped.camera_offset;
            if (!(lean.i > definition->camera_min_value))
                lean.i = definition->camera_min_value;
            real_vector3d scaled;
            subtract_vectors3d(&lean, &offset, &scaled);
            scaled.i *= definition->camera_scale_x * weight;
            scaled.j *= definition->camera_scale_y * weight;
            scaled.k *= definition->camera_scale_z * weight;
            point_from_line2d(&camera_position->xy, &forward2d, scaled.i, &camera_position->xy);
            point_from_line2d(&camera_position->xy, &left2d, scaled.j, &camera_position->xy);
            camera_position->z += scaled.k;
        }
    real_vector3d forward_copy = *forward;
    real_vector3d up_copy = *up;
    biped_compute_lean(biped_index, camera_position, &forward_copy, &up_copy);
}

// External names used by single-unit shape checks.
// @address 0xa0538 ?action_executing@@YA_NJW4e_action@@@Z
// @address 0xc1a90 ?euler_angles2d_from_vector3d@@YAPATreal_euler_angles2d@@PAT1@PBTreal_vector3d@@@Z
// @address 0x8a1ecc ?g_active_structure_bsp_mask@@3KA
// @address 0x79d458 ?g_mixing_board_definition_list@@3PAVc_mixing_board_definition@@A
// @address 0x88c60 ?get_resources@c_structure_bsp_resource_interface@@QBAPAUs_structure_bsp_resources@@XZ
// @address 0x7cff08 ?global_scenario@@3PAUscenario@@A
// @address 0xfcf30 ?internal_object_get_markers_by_string_id@@YAFJJPAUobject_marker@@F_N@Z
// @address 0xe8f00 ?motor_system_in_airborne_state@@YA_NJ@Z
// @address 0x9a080 ?object_get_origin@@YAPATreal_point3d@@JPAT1@@Z
// @address 0xac748 ?object_try_and_get_and_verify_type@@YAPAXJK@Z
// @address 0xdbca0 ?set_pitch@c_aiming_channel@@QAAXM@Z
// @address 0xdbb78 ?set_yaw@c_aiming_channel@@QAAXM@Z
// @address 0x9a2c8 ?unit_get_active_primary_weapon@@YAJJPAJ@Z
// @address 0xdc770 ?valid@c_animation_manager@@QBA_NXZ
// @address 0xed230 ?mover_is_in_entering_state@@YA_NJ@Z
// @address 0xdbdc8 ?unit_animation_allows_playback_adjustment@@YA_NJW4e_animation_playback_flags@@@Z
// @address 0x74b714 ?global_up3d@@3PBTreal_vector3d@@B
// @address 0x74b720 ?global_down3d@@3PBTreal_vector3d@@B

// @address 0x347328 ?biped_impulse_vehicle_seat@@YAXJW4e_biped_seat_impulse_type@@@Z
// 0x347328

struct s_body_world;
class c_havok_component;
/* Constant downward axis for the seat impulse. */
static const real_matrix4x3 bipeds_negative_identity4x3 = { 1.0f, { { {-1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, -1.0f} }
    }, {0.0f, 0.0f, 0.0f} };

/* Reuse the object-header base across seat-impulse lookups. */
static __forceinline void *bipeds_object_get_and_verify_type_known(s_object_header *headers, long object_index)
{
    return headers[datum_index_to_absolute_index(object_index)].datum;
}
static __forceinline unsigned char bipeds_object_get_type_known(s_object_header *headers, long object_index)
{
    return headers[datum_index_to_absolute_index(object_index)].object_type;
}
static __forceinline long bipeds_object_get_ultimate_parent_known(s_object_header *headers, long object_index)
{
    long ultimate_parent_index = NONE;
    while (object_index != NONE)
    {
        ultimate_parent_index = object_index;
        object_index = ((object_datum *)bipeds_object_get_and_verify_type_known(headers, object_index))->parent_object_index;
    }
    return ultimate_parent_index;
}

// 0x347328
void biped_impulse_vehicle_seat(long biped_index, e_biped_seat_impulse_type impulse_type)
{
    s_object_header *headers = ((s_object_header *)object_header_data->data);
    object_datum *biped = (object_datum *)bipeds_object_get_and_verify_type_known(headers, biped_index);
    if (biped->parent_object_index != NONE && ((1 << bipeds_object_get_type_known(headers, biped->parent_object_index)) & 2) != 0)
    {
        biped_definition *biped_tag = (biped_definition *)tag_get('bipd', biped->definition_index);
        long vehicle_index = biped->parent_object_index;
        object_datum *vehicle = (object_datum *)bipeds_object_get_and_verify_type_known(headers, vehicle_index);
        if (vehicle->parent_object_index == NONE)
        {
            vehicle_definition *vehicle_definition = (struct vehicle_definition *)tag_get('vehi', vehicle->definition_index);
            float impulse = 0.0f;
            real_point3d origin;
            object_get_origin(biped_index, &origin);
            switch (impulse_type)
            {
            case _biped_seat_impulse_enter:
                impulse = vehicle_definition->seat_enter_impulse;
                break;
            case _biped_seat_impulse_exit:
                impulse = vehicle_definition->seat_exit_impulse;
                break;
            default:
                // match: 0x347328: unreachable hint omits dead branch check
                __assume(0);
            }
            if (impulse > 0.0f && vehicle->havok_component_index != NONE)
            {
                c_havok_component *havok_component = havok_component_get(vehicle->havok_component_index);
                long rigid_body_index = havok_component->calculate_rigid_body_index_from_node_index(biped->parent_node_index);
                if (rigid_body_index != NONE)
                {
                    float minimum_speed = 2.0f;
                    float maximum_speed = 4.0f;
                    float speed_scale = 1.0f;
                    real_vector3d velocity;
                    real_vector3d acceleration_storage;
                    real_vector3d * acceleration = &acceleration_storage;
                    real_vector3d parent_angular_velocity;
                    real_vector3d parent_velocity;
                    ((c_havok_component *)(s_body_world *)havok_component)
                        ->get_rigid_body_linear_velocity_at_point(
                            rigid_body_index, &velocity, &origin);
                    // match: 0x347328: value range assumption guides branch lowering
                    __assume(vehicle_index != NONE);
                    if (::object_get_early_mover_local_space_velocity(bipeds_object_get_ultimate_parent_known(headers, vehicle_index),
                            &origin, &parent_velocity,
                            &parent_angular_velocity, true, false))
                        subtract_vectors3d(&velocity, &parent_velocity, &velocity);
                    float speed = magnitude3d(&velocity);
                    speed_scale = 1.0f - (speed - minimum_speed) / (maximum_speed - minimum_speed);
                    speed_scale = MIN(MAX(speed_scale, 0.0f), 1.0f);
                    scale_vector3d(&bipeds_negative_identity4x3.up,
                        speed_scale *game_tick_length() * impulse *
                            (biped_tag->physics.mass / havok_component->get_rigid_body_mass(rigid_body_index)) *
                            global_gravity_get(),
                        acceleration);
                    havok_component->rigid_body_apply_acceleration_at_point(rigid_body_index, &origin, acceleration);
                }
            }
        }
    }
}


// @address 0x346428 ?biped_bring_ragdoll_inside_world@@YAXJ_N@Z
void biped_bring_ragdoll_inside_world(long biped_index, bool flag)
{
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, 1);
    if ((long)motor_system_is_relaxed(biped_index) == 1 && biped->object.havok_component_index != NONE)
    {
        c_havok_component *component = havok_component_get(biped->object.havok_component_index);
        long rigid_body_index = bipeds_position_controlling_rigid_body_index(component);
        if (component->is_physics_model_havok_component() && rigid_body_index != NONE)
        {
            float minimum_dimension;
            float largest_shape_dimension;
            long convex_shape_count;
            long phantom_shape_count;
            float radius = calculate_havok_shape_bounding_sphere_radius(component->get_rigid_body(rigid_body_index)->get_collider()->get_shape(),
                &minimum_dimension, &largest_shape_dimension, &convex_shape_count, &phantom_shape_count);
            real_point3d center_of_mass;
            (component)->get_rigid_body_center_of_mass(rigid_body_index, &center_of_mass);
            biped_bring_ragdoll_inside_world(biped_index, flag, &center_of_mass, radius);
        }
    }
}


static __forceinline void bipeds_real_point3d_from_hkVector4(real_point3d *point, const phys_vec4 &vector)
{
    float z = vector(2);
    set_real_point3d(point, vector(0), vector(1), z);
}
// @address 0x250320 ?get_constraint_pivots@c_havok_component@@QAAXJPATreal_point3d@@0@Z
#if defined(_XBOX) && !defined(__clang__)
inline void c_havok_component::get_constraint_pivots(long constraint_index, real_point3d *pivot_in_a, real_point3d *pivot_in_b)
#else
void c_havok_component::get_constraint_pivots(long constraint_index, real_point3d *pivot_in_a, real_point3d *pivot_in_b)
#endif
{
    c_constraint *constraint = &m_component_constraints[constraint_index];
    switch (constraint->get_constraint_type()) {
    case _physics_model_constraint_type_ragdoll: {
        phys_ragdoll_data *data = (phys_ragdoll_data *)constraint->get_constraint()->get_chain_data();
        bipeds_real_point3d_from_hkVector4(pivot_in_a, data->m_data_atoms.m_transform_atom.m_transform_a.m_translation);
        bipeds_real_point3d_from_hkVector4(pivot_in_b, data->m_data_atoms.m_transform_atom.m_transform_b.m_translation);
        break;
    }
    case _physics_model_constraint_type_limited_hinge: {
        phys_limited_hinge_data *data = (phys_limited_hinge_data *)constraint->get_constraint()->get_chain_data();
        bipeds_real_point3d_from_hkVector4(pivot_in_a, data->m_data_atoms.m_transform_atom.m_transform_a.m_translation);
        bipeds_real_point3d_from_hkVector4(pivot_in_b, data->m_data_atoms.m_transform_atom.m_transform_b.m_translation);
        break;
    }
    case _physics_model_constraint_type_hinge: {
        phys_hinge_data *data = (phys_hinge_data *)constraint->get_constraint()->get_chain_data();
        bipeds_real_point3d_from_hkVector4(pivot_in_a, data->m_data_atoms.m_transform_atom.m_transform_a.m_translation);
        bipeds_real_point3d_from_hkVector4(pivot_in_b, data->m_data_atoms.m_transform_atom.m_transform_b.m_translation);
        break;
    }
    case _physics_model_constraint_type_stiff_spring: {
        phys_stiff_spring_data *data = (phys_stiff_spring_data *)constraint->get_constraint()->get_chain_data();
        bipeds_real_point3d_from_hkVector4(pivot_in_a, data->m_data_atoms.m_pivots_atom.m_translation_a);
        bipeds_real_point3d_from_hkVector4(pivot_in_b, data->m_data_atoms.m_pivots_atom.m_translation_b);
        break;
    }
    case _physics_model_constraint_type_ball_and_socket: {
        phys_ball_socket_data *data = (phys_ball_socket_data *)constraint->get_constraint()->get_chain_data();
        bipeds_real_point3d_from_hkVector4(pivot_in_a, data->m_data_atoms.m_pivots_atom.m_translation_a);
        bipeds_real_point3d_from_hkVector4(pivot_in_b, data->m_data_atoms.m_pivots_atom.m_translation_b);
        break;
    }
    case _physics_model_constraint_type_prismatic: {
        phys_prismatic_data *data = (phys_prismatic_data *)constraint->get_constraint()->get_chain_data();
        bipeds_real_point3d_from_hkVector4(pivot_in_a, data->m_data_atoms.m_transform_atom.m_transform_a.m_translation);
        bipeds_real_point3d_from_hkVector4(pivot_in_b, data->m_data_atoms.m_transform_atom.m_transform_b.m_translation);
        break;
    }
    default:
        // match: 0x250320: unreachable hint omits dead branch check
        __assume(0);
    }
}
static __forceinline void bipeds_hkTransform_from_matrix4x3(phys_transform *transform, const real_matrix4x3 *matrix)
{
    phys_vec4_from_real_vector3d(&transform->m_rotation.get_column(0), &matrix->forward);
    phys_vec4_from_real_vector3d(&transform->m_rotation.get_column(1), &matrix->left);
    phys_vec4_from_real_vector3d(&transform->m_rotation.get_column(2), &matrix->up);
    phys_vec4_from_real_point3d(&transform->m_translation, &matrix->position);
}


// @address 0x346660 ?biped_bring_rigid_body_to_safe_position@@YA_NJ_NPBTreal_point3d@@MJJJ0@Z
bool biped_bring_rigid_body_to_safe_position(long biped_index, bool check_stretched, const real_point3d *point, float radius,
    long rigid_body_index, long constraint_index, long parent_rigid_body_index, bool parent_safe)
{
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, 1);
    c_havok_component *component = havok_component_get(biped->object.havok_component_index);
    bool result = false;
    bool move = false;
    real_point3d center_of_mass;
    real_matrix4x3 matrix;
    component->get_rigid_body_center_of_mass(rigid_body_index, &center_of_mass);
    component->get_rigid_body_transform(rigid_body_index, &matrix);
    if (check_stretched && constraint_index != NONE)
    {
        long rigid_body_a, rigid_body_b;
        component->get_constraint_rigid_bodies(constraint_index, &rigid_body_a, &rigid_body_b);
        if (rigid_body_a != NONE && rigid_body_b != NONE)
        {
            unsigned char *stretched_counter = component->get_stretched_counter(constraint_index);
            real_point3d pivot_a, pivot_b;
            component->get_constraint_pivots(constraint_index, &pivot_a, &pivot_b);
            real_matrix4x3 matrix_a, matrix_b;
            component->get_rigid_body_transform(rigid_body_a, &matrix_a);
            component->get_rigid_body_transform(rigid_body_b, &matrix_b);
            matrix4x3_transform_point(&matrix_a, &pivot_a, &pivot_a);
            matrix4x3_transform_point(&matrix_b, &pivot_b, &pivot_b);
            if (distance_squared3d(&pivot_a, &pivot_b) > 0.001089f)
            {
                (*stretched_counter)++;
                if (*stretched_counter >= 3)
                {
                    *stretched_counter = 0;
                    move = true;
                }
            }
            if (!(game_time_get() & 1) && *stretched_counter != 0)
                (*stretched_counter)--;
        }
    }
    real_point3d starts[2];
    real_vector3d rays[2];
    long ray_count;
    if (parent_rigid_body_index != NONE)
    {
        real_point3d parent_center;
        component->get_rigid_body_center_of_mass(parent_rigid_body_index, &parent_center);
        real_point3d pivot_a, pivot_b;
        component->get_constraint_pivots(constraint_index, &pivot_a, &pivot_b);
        long rigid_body_a, rigid_body_b;
        component->get_constraint_rigid_bodies(constraint_index, &rigid_body_a, &rigid_body_b);
        real_point3d pivot_world;
        matrix4x3_transform_point(&matrix, rigid_body_a == parent_rigid_body_index ? &pivot_b : &pivot_a, &pivot_world);
        real_vector3d pivot_offset;
        vector_from_points3d(&parent_center, &pivot_world, &pivot_offset);
        starts[0] = parent_center;
        rays[0] = pivot_offset;
        vector_from_points3d(&pivot_world, &center_of_mass, &rays[1]);
        starts[1] = pivot_world;
        ray_count = 2;
    }
    else
    {
        starts[0] = *point;
        vector_from_points3d(point, &center_of_mass, &rays[0]);
        ray_count = 1;
    }
    for (long i = 0; !move && i < ray_count; i++)
    {
        s_havok_ray_test ray;
        ray.start = starts[i];
        ray.vector = rays[i];
        long motor_program = mover_get_motor_program(biped_index);
        c_motor_program_definition *definition = motor_program != NONE ? g_program_definition[motor_program] : 0;
        short group = 0;
        if (definition->relaxation_state)
            group = definition->relaxation_state(biped_index);
        move |= havok_ray_cast_in_world(&ray, group ? 13 : 14, biped->object.havok_component_index, NONE);
    }
    if (move)
    {
        real_vector3d offset;
        vector_from_points3d(&matrix.position, &center_of_mass, &offset);
        real_matrix4x3 new_matrix = matrix;
        new_matrix.position = starts[0];
        new_matrix.position.x -= offset.i;
        new_matrix.position.y -= offset.j;
        new_matrix.position.z -= offset.k;
        phys_transform transform;
        bipeds_hkTransform_from_matrix4x3(&transform, &new_matrix);
        component->set_rigid_body_transform_internal(rigid_body_index, &transform);
        result = true;
    }
    return result;
}
// @address 0x346ba8 ?biped_bring_ragdoll_inside_world_recursive@@YA_NJ_NPBTreal_point3d@@MPBVc_runtime_node_to_havok_component_rigid_body_index_mapping@@PAKJ0@Z

static __forceinline unsigned __int64 bipeds_recursive_find_property_value(const phys_world_object *object, unsigned long id)
{
    return object->get_world_object_property(id).m_data;
}
static __forceinline long bipeds_recursive_havok_entity_get_havok_component_rigid_body_index(const phys_world_object *object)
{
    return (long)bipeds_recursive_find_property_value(object, 0x2002);
}
static __forceinline long bipeds_recursive_havok_entity_get_havok_component_index(const phys_world_object *object)
{
    long value = (long)bipeds_recursive_find_property_value(object, 0x2001);
    return value == 0 ? -1 : value;
}
static __forceinline void bipeds_recursive_get_constraint_rigid_bodies(c_havok_component *component, long index, long *a, long *b)
{
    phys_constraint *instance = component->get_constraint(index)->get_constraint();
    const phys_world_object *body_a = instance->get_rigid_body_a();
    const phys_world_object *body_b = instance->get_rigid_body_b();
    *a = bipeds_recursive_havok_entity_get_havok_component_rigid_body_index(body_a);
    if (bipeds_recursive_havok_entity_get_havok_component_index(body_b) != -1)
        *b = bipeds_recursive_havok_entity_get_havok_component_rigid_body_index(body_b);
    else
        *b = -1;
}
bool biped_bring_ragdoll_inside_world_recursive(long biped_index, bool flag, const real_point3d *position, float radius,
    const c_runtime_node_to_havok_component_rigid_body_index_mapping *mapping, unsigned long *fixed, long parent, bool safe)
{
 object_datum *biped=(object_datum *)object_get_and_verify_type(biped_index, _object_mask_biped);
 c_havok_component *component=havok_component_get(biped->havok_component_index);
 bool result=true;
 for(long constraint_index=0;constraint_index<component->get_constraint_count();constraint_index++){
  long next=-1;
  long a,b;
  bipeds_recursive_get_constraint_rigid_bodies(component,constraint_index,&a,&b);
  if(a==parent) next=b;
  if(b==parent) next=a;
  if(next!=-1) { unsigned long bit=1<<(next&31); if(!(fixed[next>>5]&bit)){
   bool child_safe=biped_bring_rigid_body_to_safe_position(biped_index,flag,position,radius,next,constraint_index,parent,safe);
   fixed[next>>5]|=bit;
   biped_bring_ragdoll_inside_world_recursive(biped_index,flag,position,radius,mapping,fixed,next,child_safe);
  } }
 }
 return result;
}

// @address 0x2b1610 ?set_mode@c_character_physics_component@@QAAXW4e_mode@1@@Z
// @address 0x318a80 ?object_set_position_direct@@YAXJPBTreal_point3d@@PBTreal_vector3d@@1PBUs_location@@_N@Z
// @address 0x9b308 ?normalize3d@@YAMPATreal_vector3d@@@Z

// @address 0x3476e8 ?biped_enter_dead_physics@@YAXJ@Z
void biped_enter_dead_physics(long biped_index)
{
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, 1);
    biped->biped.physics.set_mode(c_character_physics_component::_mode_dead);
    if (motor_system_in_airborne_state(biped_index))
        biped->biped.physics.get_mode_dead()->notify_off_ground();
}


// @address 0x347590 ?biped_enter_posture_physics@@YAXJPBTreal_point3d@@PBTreal_vector3d@@@Z
void biped_enter_posture_physics(long biped_index, const real_point3d *position, const real_vector3d *forward)
{
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, 1);
    s_location *location = &biped->object.location;
    real_vector3d planar_forward;
    if (forward)
    {
        planar_forward = *forward;
        planar_forward.k = 0.0f;
        if (normalize3d(&planar_forward) > 0.0f)
            forward = &planar_forward;
        else
            forward = 0;
    }
    biped->biped.physics.set_mode(c_character_physics_component::_mode_sentinel);
    if (forward)
        object_set_position_direct(biped_index, position, forward, &k_real_math_identity_matrix_storage.up, location, false);
    else
        object_set_position_direct(biped_index, position, 0, 0, location, false);
}

// @address 0x347670 ?biped_enter_climbing_physics@@YAXJPBTreal_point3d@@@Z
void biped_enter_climbing_physics(long biped_index, const real_point3d *position)
{
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, 1);
    biped->biped.physics.set_mode(c_character_physics_component::_mode_sentinel_b);
    object_set_position_direct(biped_index, position, 0, 0, 0, false);
}


static __forceinline e_biped_default_movement_type bipeds_biped_get_default_movement_type(long biped_index)
{
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, 1);
    e_biped_default_movement_type type = _biped_default_movement_none;
    if (!(bool)((biped->object.damage_flags >> 2) & 1))
    {
        biped_definition *definition = (biped_definition *)tag_get('bipd', biped->object.definition_index);
        type = (bool)((definition->physics.flags >> 4) & 1) ? _biped_default_movement_flying : _biped_default_movement_ground;
    }
    return type;
}

// @address 0x347760 ?biped_enter_default_physics@@YAXJ@Z
void biped_enter_default_physics(long biped_index)
{
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, 1);
    switch (bipeds_biped_get_default_movement_type(biped_index))
    {
    case _biped_default_movement_ground:
        biped->biped.physics.set_mode(c_character_physics_component::_mode_ground);
        break;
    case _biped_default_movement_flying:
        biped->biped.physics.set_mode(c_character_physics_component::_mode_flying);
        break;
    default:
        biped_enter_dead_physics(biped_index);
        break;
    }
}

// @address 0x2554f8 ?rigid_body_calculate_fixed_position_along_z@c_havok_component@@QAA_NJW4e_havok_group@@PBVphys_shape@@MJPAJ@Z


// @address 0xee690 ?biped_exit_invisible_crouched_uber_melee@@YAXJ@Z
void biped_exit_invisible_crouched_uber_melee(long biped_index)
{
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, 1);
    // match: 0xee690: pointer __restrict qualifier prevents alias reload
    biped_definition *__restrict definition = (biped_definition *)tag_get('bipd', biped->object.definition_index);
    bool succeeded = false;
    if (biped->biped.physics.get_mode() == c_character_physics_component::_mode_melee)
    {
        c_character_physics_mode_melee_datum *melee = biped->biped.physics.get_mode_melee();
        float z_offset;
        if (biped_calculate_invisible_crouched_uber_melee_z_offset(biped_index, &z_offset) && biped->object.havok_component_index != NONE)
        {
            c_havok_component *component = havok_component_get(biped->object.havok_component_index);
            long rigid_body_index = bipeds_position_controlling_rigid_body_index(component);
            if (rigid_body_index != NONE)
            {
                long ending_iteration_index;
                const phys_shape *shape = (const phys_shape *)&((s_character_physics_capsule *)definition->physics.pills.address)->shape;
                if (component->rigid_body_calculate_fixed_position_along_z(rigid_body_index, _havok_group_biped, shape,
                        bipeds_biped_calculate_uber_melee_z_offset_per_tick(), melee->get_exit_invisible_crouched_uber_melee_offset(),
                        &ending_iteration_index))
                {
                    melee->exit_invisible_crouched_uber_melee(ending_iteration_index);
                    ::havok_object_rebuild(biped_index, 0);
                    biped->unit.crouch_amount = 0.0f;
                    succeeded = true;
                }
            }
        }
    }
    if (!succeeded)
        biped->unit.unit_flags |= 1 << 22;
}

// @address 0x36d508 ?set_program@c_motor_program_state@@QAA_NJW4e_motor_program@@@Z

// @address 0x74b6f4 ?global_zero_vector2d@@3PBTreal_vector2d@@B
// @address 0x74b700 ?global_origin3d@@3PBTreal_point3d@@B
// @address 0x74b708 ?global_zero_vector3d@@3PBTreal_vector3d@@B
// @address 0x3454e0 ?biped_reset@@YAXJ@Z
void biped_reset(long biped_index)
{
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, 1);
    biped_definition *definition = (biped_definition *)tag_get('bipd', biped->object.definition_index);
    biped->unit.crouch_amount = 0.0f;
    biped->biped.lean_input = 0.0f;
    biped->biped.flags = 0;
    biped->biped.last_airborne_speech_time = NONE;
    biped->biped.bump_object_index = NONE;
    biped->biped.bump_ticks = 0;
    biped->biped.jump_control_ticks = 0;
    biped->biped.stun_ticks = 0;
    biped->biped.falling_danger_cooldown = 0;
    object_get_origin(biped_index, &biped->biped.position);
    bipeds_biped_reset_pathfinding_location(biped_index);
    biped->biped.physics.initialize(biped_index);
    biped_enter_default_physics(biped_index);
    if (!(bool)((biped->object.damage_flags >> 2) & 1))
    {
        c_motor_program_state *motor = (c_motor_program_state *)((char *)biped + biped->unit.motor_program.offset);
        if ((bool)((definition->physics.flags >> 4) & 1))
            motor->set_program(biped_index, _motor_program_flying);
        else
            motor->set_program(biped_index, _motor_program_ground);
    }
    biped->biped.following_camera_offset = *global_origin3d;
    biped->biped.previous_ground_normal = *global_up3d;
    biped->biped.moving_turn_scale = 1.0f;
    biped->biped.moving_turn_remaining_angle = 0.0f;
    biped->biped.moving_turn_position = *global_origin3d;
    biped->biped.velocity = *global_zero_vector2d;
    real_matrix4x3 matrix;
    ::object_get_world_matrix(biped_index, &matrix);
    memcpy(&biped->biped.camera_a, &matrix, sizeof(real_matrix4x3));
    memcpy(&biped->biped.camera_b, &matrix, sizeof(real_matrix4x3));
    memcpy(&biped->biped.camera_e, &matrix, sizeof(real_matrix4x3));
    memcpy(&biped->biped.camera_c, &matrix, sizeof(real_matrix4x3));
    memcpy(&biped->biped.camera_d, &matrix, sizeof(real_matrix4x3));
    biped->biped.landing_root_offset = 0.0f;
    ((biped_datum *)object_get_and_verify_type(biped_index, 1))->biped.scale = 1.0f;
}


// @address 0x345248 ?biped_new@@YA_NJPAUobject_placement_data@@PA_N@Z
bool biped_new(long biped_index, object_placement_data *placement, bool *out_of_memory)
{
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, 1);
    biped_reset(biped_index);
    biped->biped.swarm_actor_index = NONE;
    biped->biped.squad_index = NONE;
    biped->biped.combat_status = 0;
    if (game_simulation::game_is_distributed())
    {
        if (!::object_header_block_allocate(biped_index, 0x540, 40, 2))
            *out_of_memory = true;
    }
    return true;
}

// @address 0x74b70c ?global_forward3d@@3PBTreal_vector3d@@B

// @address 0x345e80 ?biped_fix_transform@@YAXJPATreal_point3d@@PATreal_vector3d@@1@Z
void biped_fix_transform(long biped_index, real_point3d *position, real_vector3d *forward, real_vector3d *up)
{
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, 1);
    biped_definition *definition = (biped_definition *)tag_get('bipd', biped->object.definition_index);
    if (biped->object.parent_object_index == NONE && !(bool)((definition->physics.flags >> 3) & 1) &&
        mover_get_motor_program(biped_index) != 7 &&
        mover_get_motor_program(biped_index) != 8 && biped->biped.physics.get_mode() != c_character_physics_component::_mode_dead)
    {
        if (up != 0 && up->k != 1.0f)
            *up = *global_up3d;
        if (forward != 0 && forward->k != 0.0f)
        {
            forward->k = 0.0f;
            if (normalize3d(forward) == 0.0f)
                *forward = *global_forward3d;
        }
    }
}

// @address 0x9cb80 ?object_get_velocities@@YAXJPATreal_vector3d@@0@Z
// @address 0x33e620 ?motor_system_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
/* a TU-local copy of the identity up vector lets c2 see the values */
static const real_vector3d bipeds_global_up3d = { 0.0f, 0.0f, 1.0f };


// @address 0x347080 ?biped_died@@YAXJJPBTreal_vector3d@@_N1@Z
void biped_died(long biped_index, long killer_index, const real_vector3d *damage_vector, bool alternate, bool unused)
{
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, 1);
    if (biped->biped.physics.get_mode() != c_character_physics_component::_mode_dead)
    {
        if (biped->object.parent_object_index == NONE && motor_system_in_airborne_state(biped_index))
        {
            union { real_vector3d velocity; real_point3d position; };
            object_get_velocities(biped_index, &velocity, 0);
            if (dot_product3d(&velocity, &bipeds_global_up3d) > 0.001f)
            {
                biped_definition *definition = (biped_definition *)tag_get('bipd', biped->object.definition_index);
                float distance = definition->physics.height_crouching - definition->physics.radius * 2.0f;
                if (distance > 0.0f)
                {
                    float limit = g_havok_constants.minimum_radius * 2.0f;
                    float offset = distance < limit ? distance : limit;
                    object_get_origin(biped_index, &position);
                    position.z += offset;
                    object_set_position_direct(biped_index, &position, 0, 0, 0, false);
                }
            }
        }
    }
    if (mover_get_motor_program(biped_index) != 2)
    {
        c_motor_request request;
        request.setup_die(_action_none, alternate);
        motor_system_submit(biped_index, &request);
    }
    if (biped->biped.physics.get_mode() == c_character_physics_component::_mode_dead)
    {
        if (damage_vector != 0 && killer_index != NONE)
            bipeds_set_death_info(biped->biped.physics.get_mode_dead(), killer_index, damage_vector);
    }
    ((biped_datum *)object_get_and_verify_type(biped_index, 1))->biped.scale = 1.0f;
}

// @address 0x121058 ?biped_locate_auxilary_animations@@YAXJ@Z
void biped_locate_auxilary_animations(long biped_index)
{
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, 1);
    c_animation_manager *animation = (c_animation_manager *)((char *)biped + biped->object.animation.offset);
    s_unit_animation_storage *root_node = (s_unit_animation_storage *)((char *)biped + biped->unit.animation_storage.offset);
    if (animation)
    {
        long weapon_class = animation->m_current_goal.weapon_class;
        long weapon_type = animation->m_current_goal.weapon_type;
        long secondary_weapon_type = root_node->weapon_type;
        if (weapon_class == 486)
        {
            animation->setup_auxilary_channel(animation->get_mixing_board_auxilary_channel(5), weapon_class, weapon_type, 641, 1024);
            animation->setup_auxilary_channel(animation->get_mixing_board_auxilary_channel(6), weapon_class, secondary_weapon_type, 641, 2048);
            animation->setup_auxilary_channel(animation->get_mixing_board_auxilary_channel(3), weapon_class, weapon_type, 485, 1024);
            animation->setup_auxilary_channel(animation->get_mixing_board_auxilary_channel(4), weapon_class, secondary_weapon_type, 485, 2048);
        }
        else
        {
            animation->setup_auxilary_channel(animation->get_mixing_board_auxilary_channel(5), weapon_class, weapon_type, 641, 1024);
            animation->setup_auxilary_channel(animation->get_mixing_board_auxilary_channel(6), weapon_class, weapon_type, 641, 2048);
            c_auxilary_channel *channel3 = animation->get_mixing_board_auxilary_channel(3);
            channel3->clear();
            channel3->m_frame_position = 0.0f;
            c_auxilary_channel *channel4 = animation->get_mixing_board_auxilary_channel(4);
            channel4->clear();
            channel4->m_frame_position = 0.0f;
        }
        animation->setup_aiming_channel(animation->get_mixing_board_aiming_channel(4), weapon_class, weapon_type, 1183, 0);
    }
    unit_animation_locate_auxilary_animations(biped_index);
}


static __forceinline float bipeds_melee_plane_distance(const real_vector3d *normal, const real_point3d *point, float distance)
{
    return normal->i*point->x + normal->j*point->y + normal->k*point->z - distance;
}

static __forceinline void bipeds_melee_zero_angles(real_euler_angles2d *angles)
{
    angles->yaw = angles->pitch = 0.0f;
}

static __forceinline float bipeds_melee_plane_camera_dot(const real_vector3d *normal, const real_point3d *point)
{
    return point->x * normal->i + normal->j * point->y + normal->k * point->z;
}
static __forceinline real_plane3d *bipeds_melee_plane_from_point_and_normal(real_plane3d *plane, const real_point3d *point, const
    real_vector3d *normal)
{
    plane->normal = *normal;
    plane->distance = bipeds_melee_plane_camera_dot(normal, point);
    return plane;
}
static __forceinline float bipeds_melee_distance_to_plane(const real_plane3d *plane, const real_point3d *point)
{
    return bipeds_melee_plane_distance(&plane->normal, point, plane->distance);
}
// @address 0x3479c0 ?biped_calculate_melee_target_parameters@@YAXJJPBTreal_vector3d@@PAUs_biped_melee_target_parameters@@@Z
void biped_calculate_melee_target_parameters(
    long biped_index, long target_index, const real_vector3d *aiming_vector, s_biped_melee_target_parameters *parameters)
{
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, 1);
    real_point3d source_position, target_position, camera_position;
    float source_height, source_radius, target_height, target_radius;
    real_vector3d target_velocity;
    biped_get_melee_target_physics_pill(biped_index, &source_position, &source_height, &source_radius);
    biped_get_melee_target_physics_pill(target_index, &target_position, &target_height, &target_radius);
    object_get_velocities(target_index, &target_velocity, 0);
    unit_get_camera_position(biped_index, &camera_position);
    parameters->airborne =
        (motor_system_in_airborne_state(biped_index) || biped->biped.physics.get_mode() != c_character_physics_component::_mode_ground) ? true
                                                                                                                                  : false;
    bipeds_melee_zero_angles(&parameters->ground_angles);
    euler_angles2d_from_vector3d(&parameters->aiming_angles, aiming_vector);
    bipeds_melee_zero_angles(&parameters->target_angles);
    parameters->source_position = source_position;
    parameters->source_position.z -= source_radius;
    parameters->target_position = target_position;
    parameters->target_position.z -= target_radius;
    float combined_radius = source_radius + target_radius;
    parameters->target_velocity = target_velocity;
    parameters->target_radius = target_radius;
    if (!parameters->airborne)
    {
        const real_vector3d *ground_normal = biped->biped.physics.get_mode_ground()->get_ground_plane();
        real_vector3d projected, forward;
        float projection = dot_product3d(ground_normal, &biped->object.forward);
        scale_vector3d(ground_normal, projection, &projected);
        subtract_vectors3d(&biped->object.forward, &projected, &forward);
        float forward_length = normalize3d(&forward);
        if (forward_length != 0.0f)
        {
            real_vector3d direction;
            vector_from_points3d(&parameters->source_position, &parameters->target_position, &direction);
            float direction_length = normalize3d(&direction);
            if (direction_length != 0.0f)
            {
                real_matrix4x3 rotation;
                real_vector3d local_direction;
                matrix4x3_rotation_from_vectors(&rotation, &forward, ground_normal);
                matrix3x3_inverse_transform_vector(&rotation.rotation, &direction, &local_direction);
                euler_angles2d_from_vector3d(&parameters->ground_angles, &local_direction);
            }
        }
    }
    real_matrix3x3 aiming_rotation;
    aiming_rotation.forward = *aiming_vector;
    generate_up_vector3d(&aiming_rotation.forward, &aiming_rotation.up);
    cross_product3d(&aiming_rotation.up, &aiming_rotation.forward, &aiming_rotation.left);
    real_point3d target_top;
    set_real_point3d(&target_top, target_position.x, target_position.y, target_position.z + target_height);
    real_vector3d plane_normal;
    generate_up_vector3d(aiming_vector, &plane_normal);
    real_plane3d plane;
    bipeds_melee_plane_from_point_and_normal(&plane, &camera_position, &plane_normal);
    float bottom_distance = bipeds_melee_distance_to_plane(&plane, &target_position);
    float top_distance = bipeds_melee_distance_to_plane(&plane, &target_top);
    float fraction;
    if (top_distance < 0.0001f)
        fraction = 1.0f;
    else if (bottom_distance > rasterizer_states_mode_7_depth_bias)
        fraction = 0.0f;
    else
        { fraction = -bottom_distance; fraction /= top_distance - bottom_distance; }
    real_point3d target_point = target_position;
    target_point.z += fraction*target_height;
    real_vector3d target_direction;
    vector_from_points3d(&camera_position, &target_point, &target_direction);
    float target_distance = normalize3d(&target_direction);
    parameters->distance = target_distance;
    if (parameters->distance != 0.0f)
    {
        real_vector3d local_direction;
        matrix3x3_inverse_transform_vector(&aiming_rotation, &target_direction, &local_direction);
        euler_angles2d_from_vector3d(&parameters->target_angles, &local_direction);
    }
    parameters->distance = MAX(parameters->distance - combined_radius, 0.0f);
}

/* ---- biped vehicle speech (dev name biped_vehicle_speech) ---- */
// real_plane3d: defined this way by 2 accepted units (tu/blamlibxbox/matrix_math.cpp tu/objectslibxbox/projectiles.cpp)


// collision_result: canonical union of the two accepted views
// (tu/objectslibxbox/projectiles.cpp: field_ names, word/short helpers, operator=;
//  tu/blamlibxbox/collisions.cpp: named surface/plane/material members, plane at +0x24).
// size 0x60, align 4
      // 16 bytes
    // 12 bytes
   // 4 bytes


static __forceinline bool biped_find_surface(long object_index, float distance, const real_vector3d *direction, collision_result *collision)
{
    real_point3d origin;
    real_vector3d scaled;
    object_get_origin(object_index, &origin);
    point_from_line3d(&origin, direction, -0.4f, &origin);
    scale_vector3d(direction, distance, &scaled);
    return collision_test_vector(k_biped_ground_collision_flags, false, &origin,
        &scaled, NONE, NONE, NONE, collision);
}

// @address 0xca7f0 ?biped_find_surface@@YA_NJJJMPBTreal_point3d@@PBTreal_vector3d@@PAT1@PAT2@PAFPAJ55PAK@Z
bool biped_find_surface(long biped_index, long a, long b, float radius, const real_point3d *position, const real_vector3d *direction,
    real_point3d *surface_point, real_vector3d *surface_direction, short *bsp_index, long *surface_index, long *reference_index,
    long *material_index, unsigned long *flags)
{
    collision_result collision;
    real_point3d origin;
    real_vector3d scaled;
    collision.global_material.m_index = NONE;
    if (position)
        origin = *position;
    else
        object_get_origin(biped_index, &origin);
    scaled.i = direction->i * radius;
    scaled.j = direction->j * radius;
    scaled.k = direction->k * radius;
    origin.x -= direction->i * k_real_two_fifths_storage[0];
    origin.y -= direction->j * k_real_two_fifths_storage[0];
    origin.z -= direction->k * k_real_two_fifths_storage[0];
    s_collision_test_flags test_flags = k_biped_ground_collision_flags;
    bool hit;
    if (a != NONE && object_test_vector(a, test_flags, &origin, &scaled, NONE, NONE, &collision))
        hit = true;
    else
        hit = collision_test_vector(test_flags, false, &origin, &scaled, NONE, NONE, NONE, &collision);
    if (hit)
    {
        long material = collision.object_index;
        if (bsp_index)
        {
            if (collision.structure_bsp_index == NONE && collision.type == 4 && material != NONE)
                *bsp_index = object_header_get(material)->cluster_reference.bsp_index;
            else
                *bsp_index = (short)collision.structure_bsp_index;
        }
        if (surface_index)
            *surface_index = collision.surface_index;
        if (material_index)
            *material_index = material;
        if (flags)
        {
            long flag_values = collision.bsp_reference;
            if (material == NONE)
                flag_values = NONE;
            *flags = flag_values;
        }
        if (surface_point)
            *surface_point = collision.point;
        if (surface_direction)
            *surface_direction = collision.plane.normal;
        if (reference_index)
            *reference_index = collision.instance_index;
    }
    return hit;
}

// @address 0x346e00 ?biped_vehicle_speech@@YAXJJ@Z
void biped_vehicle_speech(long biped_index, long vehicle_index)
{
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, 1);
    c_animation_manager *manager = (c_animation_manager *)((char *)biped + biped->object.animation.offset);
    vehicle_datum *vehicle = (vehicle_datum *)object_get_and_verify_type(vehicle_index, 2);
    unit_definition *definition = (unit_definition *)tag_get('vehi', vehicle->object.definition_index);
    if ((bool)((definition->flags >> 6) & 1) && biped->unit.actor_index != NONE && manager->get_state_name() != 183
        && game_time_get() - biped->unit.last_weapon_control_time > game_seconds_integer_to_ticks(4)
        && vehicle->vehicle.airborne_ticks > game_seconds_integer_to_ticks(1))
    {
        long now = game_time_get();
        if (biped->biped.last_airborne_speech_time == NONE || game_seconds_to_ticks_real(0.5f) +
            (float)biped->biped.last_airborne_speech_time < (float)now)
        {
            biped->biped.last_airborne_speech_time = now;
            collision_result collision1;
            const real_vector3d down = { 0.0f, 0.0f, -1.0f };
            if (!biped_find_surface(biped_index, 8.0f, &down, &collision1))
            {
                real_vector3d predicted;
                scale_vector3d(&vehicle->object.linear_velocity, 2.0f, &predicted);
                predicted.k -= 2.0f * g_physics_constants->gravity;
                if (normalize3d(&predicted) > 0.0f)
                {
                    collision_result collision2;
                    biped_find_surface(biped_index, 8.0f, &predicted, &collision2);
                }
            }
        }
    }
}

/* ---- mode ik (0x3481d0) ---- */


static __forceinline const c_model_animation_graph *bipeds_animation_graph_get(const c_animation_manager *animation)
{
    return (const c_model_animation_graph *)tag_get('jmad', animation->m_graph_tag_index);
}
static __forceinline long bipeds_animation_graph_node_count(const c_model_animation_graph *graph)
{
    return graph ? graph->get_node_count() : 0;
}
static __forceinline long bipeds_animation_graph_flags_size(const c_model_animation_graph *graph)
{
    long node_count = 0;
    if (graph)
        node_count = graph->get_node_count();
    return ((node_count + 31) >> 5) * sizeof(unsigned long);
}


// @address 0x347fc8 biped_process_ik
extern "C" void biped_process_ik(long object_index, long marker_name, long weapon_name, bool mode_ik, float weight,
    const c_animation_manager *animation, ik_point_iterator *iterator, long node_count, real_matrix4x3 *node_matrices)
{
    object_marker marker;
    object_marker target_marker;
    if (object_index != NONE && marker_name != NONE && weight > 0.0f)
    {
        const c_model_animation_graph *graph = bipeds_animation_graph_get(animation);
        if (graph && graph->get_node_count() != 0)
        {
            const biped_datum *object = (const biped_datum *)object_get_and_verify_type(object_index, NONE);
            const biped_definition *object_definition = (const biped_definition *)tag_get(0, object->object.definition_index);
            const s_model_definition *model = (const s_model_definition *)tag_get('hlmt', object_definition->unit.model.index);
            unsigned long processed_nodes[8];
            memset(processed_nodes, 0, bipeds_animation_graph_flags_size(bipeds_animation_graph_get(animation)));
            bool found;
            do
            {
                if (mode_ik)
                {
                    const c_model_animation_graph *current_graph = bipeds_animation_graph_get(animation);
                    iterator->mode_name = animation->m_current_goal.mode;
                    iterator->weapon_class_name = animation->m_current_goal.weapon_class;
                    found = current_graph->find_next_weapon_ik_point(iterator);
                }
                else
                {
                    const c_model_animation_graph *current_graph = bipeds_animation_graph_get(animation);
                    iterator->mode_name = animation->m_current_goal.mode;
                    iterator->weapon_class_name = animation->m_current_goal.weapon_class;
                    found = current_graph->find_next_mode_ik_point(iterator);
                }
                if (found && iterator->attach_to_marker != NONE && iterator->attach_to_marker != 0 && iterator->marker != NONE &&
                    iterator->marker != 0)
                {
                    if (internal_object_get_markers_by_string_id(object_index, iterator->marker, &marker, 1, false) != 0)
                    {
                        short node_index = marker.node_index;
                        unsigned long bit = 1 << (node_index & 31);
                        unsigned long *word = &processed_nodes[node_index >> 5];
                        unsigned long bits = *word;
                        if ((bits & bit) == 0 &&
                            internal_object_get_markers_by_string_id(marker_name, iterator->attach_to_marker, &target_marker, 1, false) !=
                                0)
                        {
                            biped_apply_inverse_kinematics(model->render_model_index, node_index, &marker.node_to_marker,
                                &target_marker.matrix, weight, node_count, node_matrices);
                            *word = bits | bit;
                        }
                    }
                }
            } while (found);
        }
    }
}

// @address 0x3481d0 ?process_mode_ik@@YAXJJJPAUreal_matrix4x3@@@Z
void process_mode_ik(long object_index, long marker_name, long node_count, real_matrix4x3 *node_matrices)
{
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(object_index, 0x2003);
    c_animation_manager *animation = (c_animation_manager *)object_header_block_get(object_get(object_index), &unit->object.animation);
    if (animation->valid() && marker_name != NONE)
    {
        t_short_fixed<13> slider = animation->get_slider_value_from_chain(5, _mixing_board_parent_slider_regular);
        bool active = slider > 0;
        float weight = slider.export_real();
        if (active)
        {
            ik_point_iterator iterator;
            biped_process_ik(object_index, marker_name, NONE, true, weight, animation, &iterator, node_count, node_matrices);
        }
    }
}

// @address 0x345970 ?biped_fix_position@@YA_NJJPBTreal_point3d@@0PAT1@MM_N2@Z
bool biped_fix_position(long biped_index, long ignore_object_index, const real_point3d *position_a,
    const real_point3d *position_b, real_point3d *position_c, float unknown_float, float scale, bool h, bool use_physics_pill)
{
    if (use_physics_pill)
    {
        real_point3d pill_position;
        float pill_height;
        float pill_offset;
        biped_get_physics_pill_from_definition(biped_index, &pill_position, &pill_height, &pill_offset);
        scale = pill_offset * scale;
    }
    return unit_fix_position(biped_index, ignore_object_index, position_a, position_b, position_c, 0.0f, scale, h);
}
// @address 0x345fc8 ?biped_exit_seat_end@@YAXJJ@Z
void biped_exit_seat_end(long biped_index, long seat_index)
{
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, 1);
    bool fixed = false;
    biped->object.forward.k = 0.0f;
    if (normalize3d(&biped->object.forward) == 0.0f)
        biped->object.forward = *global_forward3d;
    biped->object.up = *global_up3d;
    for (long attempt = seat_index == NONE; !fixed && attempt < 2; attempt++)
    {
        long ignore_object_index;
        if (attempt == 0)
            ignore_object_index = seat_index;
        else
            ignore_object_index = NONE;
        real_point3d pill_position;
        float pill_height;
        float pill_radius;
        ::biped_get_physics_pill_from_definition(biped_index, &pill_position, (float *)(&pill_height), (float *)(&pill_radius));
        fixed = unit_fix_position(biped_index, ignore_object_index, NULL, NULL, NULL, 0.0f, pill_radius * 2.0f, true);
        if (!fixed && seat_index != NONE)
        {
            object_datum *seat = (object_datum *)object_get_and_verify_type(seat_index, _object_mask_all);
            float seat_radius = seat->bounding_radius;
            real_point3d seat_center = seat->bounding_center;
            fixed = biped_fix_position(biped_index, ignore_object_index, NULL, &seat_center, NULL, 0.0f, seat_radius, true, false);
        }
    }
    if (!fixed && seat_index != NONE)
    {
        real_vector3d offset;
        {
            real_point3d origin;
            real_point3d center;
            object_get_origin(biped_index, &origin);
            object_get_center_of_mass(biped_index, &center);
            offset.i = origin.x - center.x;
            offset.j = origin.y - center.y;
            offset.k = origin.z - center.z;
        }
        {
            real_point3d seat_center;
            real_point3d position;
            object_get_center_of_mass(seat_index, &seat_center);
            position.x = seat_center.x + offset.i;
            position.y = seat_center.y + offset.j;
            position.z = seat_center.z + offset.k + 0.001f;
            object_set_position_direct(biped_index, &position, NULL, NULL, NULL, false);
        }
    }
}

static __forceinline bool bipeds_unit_is_sprinting(long unit_index)
{
    return false;
}

// @address 0x101e28 ?biped_handle_footfall_events@@YAXJPBUs_animation_event_data@@@Z

void biped_handle_footfall_events(long biped_index, const s_animation_event_data *animation_event_data)
{
    if (animation_event_data->event_type == 0 && object_has_animation_manager(biped_index))
    {
        biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, 1);
        bool stationary = magnitude_squared3d(&biped->object.linear_velocity) <= 2.25f;
        float volume = 1.0f;
        long footstep_volume = (long)(volume * 127.0f);
        biped->biped.footstep_volume = (signed char)MIN(MAX(footstep_volume, 0), 127);
        e_effect_type effect_type;
        if (stationary)
            effect_type = _effect_type_walk;
        else if (bipeds_unit_is_sprinting(biped_index))
            effect_type = _effect_type_biped_sprint;
        else
            effect_type = _effect_type_run;
        switch (animation_event_data->frame_event_type)
        {
        case _animation_frame_event_type_left_foot:
            biped_make_footstep(biped_index, effect_type, _contact_point_left_foot, volume);
            break;
        case _animation_frame_event_type_right_foot:
            biped_make_footstep(biped_index, effect_type, _contact_point_right_foot, volume);
            break;
        case _animation_frame_event_type_both_feet_shuffle:
            biped_make_footstep(biped_index, _effect_type_biped_shuffle, _contact_point_body, volume);
            break;
        case _animation_frame_event_type_body_impact:
            biped_make_footstep(biped_index, _effect_type_biped_jump_land_hard, _contact_point_body, volume);
            break;
        }
    }
}
// @address 0x3477e8 ?biped_calculate_better_melee_target@@YAJJJJ@Z
long biped_calculate_better_melee_target(long unit_index, long target_index, long current_target)
{
    long result = NONE;
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 1);
    bool energy_blade = weapon_is_energy_blade(unit_inventory_get_weapon_or_none(unit_index, unit->unit.current_weapon_index));
    if (current_target == NONE)
    {
        s_biped_melee_target_parameters parameters;
        biped_calculate_melee_target_parameters(unit_index, target_index, &unit->unit.aiming_vector, &parameters);
        if (biped_is_melee_target_in_range((const s_biped_melee_target_parameters *)&parameters, energy_blade, false))
            result = target_index;
    }
    else
    {
        s_biped_melee_target_parameters parameters_a, parameters_b;
        biped_calculate_melee_target_parameters(unit_index, target_index, &unit->unit.aiming_vector, &parameters_a);
        biped_calculate_melee_target_parameters(unit_index, current_target, &unit->unit.aiming_vector, &parameters_b);
        bool in_range_a = biped_is_melee_target_in_range((const s_biped_melee_target_parameters *)&parameters_a, energy_blade, false);
        bool in_range_b = biped_is_melee_target_in_range((const s_biped_melee_target_parameters *)&parameters_b, energy_blade, false);
        if (in_range_a && in_range_b)
        {
            if (parameters_a.distance < 0.525f && parameters_b.distance < 0.525f)
                result = (parameters_a.target_angles.yaw < 0.0f ? -parameters_a.target_angles.yaw : parameters_a.target_angles.yaw) <
                        (parameters_b.target_angles.yaw < 0.0f ? -parameters_b.target_angles.yaw : parameters_b.target_angles.yaw)
                    ? target_index
                    : current_target;
            else
                result = parameters_a.distance < parameters_b.distance ? target_index : current_target;
        }
        else if (in_range_a && !in_range_b)
            result = target_index;
        else if (!in_range_a && in_range_b)
            result = current_target;
    }
    return result;
}

// @rdata 0x82022dc0
static const float bipeds_bump_half = 0.5f;


static __forceinline long bipeds_bump_seconds_to_ticks_round(float seconds)
{
    float ticks = (float)tls_game_time_globals()->ticks_per_second * seconds; return (long)(ticks + bipeds_bump_half);
}

static __forceinline void bipeds_ai_handle_bump(long biped_index, long object_index, const real_vector3d *normal)
{
    if (tls_actor_globals()->enabled)
    {
        long responsible_index = ai_get_unit_responsible_for_damage(object_index, false);
        if (responsible_index != NONE)
        {
            biped_datum *other = (biped_datum *)object_get_and_verify_type(responsible_index, 0x1003);
            biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, 0x1003);
            if (biped->unit.actor_index != NONE)
                actor_handle_bump(biped->unit.actor_index, responsible_index);
            if (other->unit.actor_index != NONE)
                actor_handle_bump(other->unit.actor_index, biped_index);
        }
    }
}

// @address 0x10da18 ?biped_bumped_object@@YAXJJPBTreal_vector3d@@@Z
void biped_bumped_object(long biped_index, long object_index, const real_vector3d *normal)
{
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, 1);
    if ((signed char)biped->biped.bump_ticks < 0)
    {
        if (object_index == NONE)
        {
            ++biped->biped.bump_ticks;
            return;
        }
        biped->biped.bump_ticks = -bipeds_bump_seconds_to_ticks_round(bipeds_bump_half);
    }
    if (object_index != NONE && object_try_and_get_and_verify_type(object_index, 0xffffffff))
    {
        s_object_header *header = object_header_get(object_index);
        if (((1 << header->object_type) & 1) && ((biped_datum *)header->datum)->biped.physics.get_mode() ==
            c_character_physics_component::_mode_melee)
            biped->biped.flags |= 0x4000;
        if ((unsigned char)biped->biped.bump_ticks < 128)
        {
            bipeds_ai_handle_bump(biped_index, object_index, normal);
            if (biped->unit.player_index != NONE || recorded_animation_controlling_unit(biped_index))
            {
                if (biped->biped.bump_object_index != object_index)
                {
                    biped->biped.bump_object_index = object_index;
                    biped->biped.bump_ticks = 0;
                    return;
                }
                ++biped->biped.bump_ticks;
                if (!(tls_game_time_globals()->tick_length * (float)(signed char)biped->biped.bump_ticks <= 0.1f))
                    biped->biped.bump_ticks = -bipeds_bump_seconds_to_ticks_round(bipeds_bump_half);
            }
        }
    }
}

// @address 0xe6800 biped_update_melee_aiming
extern "C" unsigned char biped_update_melee_aiming(long p0, long p1)
{
    bool result = false;
    biped_datum *biped = (biped_datum *)object_get(p0);
    if (biped_update_melee_turning(p0, &biped->unit.aiming_vector))
    {
        // match: 0xe6800: retain the four point-sized aggregate copies and their integer temporaries.
        biped->unit.facing_vector = biped->unit.aiming_vector;
        biped->unit.desired_aiming_vector = biped->unit.aiming_vector;
        biped->unit.desired_looking_vector = biped->unit.aiming_vector;
        biped->unit.looking_vector = biped->unit.aiming_vector;
        simulation_action_object_update(p0, 0x10000);
        biped->biped.flags |= 1;
        result = true;
    }
    return result;
}

// @rdata 0x820221fc
static const float bipeds_ik_one = 1.0f;
// @rdata 0x8202490c
static const float bipeds_ik_epsilon = 0.0001f;
static __forceinline float bipeds_ik_get_one() { return bipeds_ik_one; }
static __forceinline real_point3d *bipeds_points_interpolate3d(const real_point3d *a, const real_point3d *b, float t, real_point3d *result)
{
    float inverse = bipeds_ik_get_one() - t;
    result->x = a->x * inverse + b->x * t;
    result->y = a->y * inverse + b->y * t;
    result->z = a->z * inverse + b->z * t;
    return result;
}
// @address 0x10ac98 biped_apply_inverse_kinematics
extern "C" void biped_apply_inverse_kinematics(long model_index, long node_index, const real_matrix4x3 *basis, const real_matrix4x3 *target,
    float weight, long node_count, real_matrix4x3 *node_matrices)
{
    const render_model_definition *model = (const render_model_definition *)tag_get(0, model_index);
    short parent_node = ((const render_model_node *)model->nodes.address)[node_index].parent_node_index;
    if (parent_node != -1)
    {
        short root_node = ((const render_model_node *)model->nodes.address)[parent_node].parent_node_index;
        if (root_node != -1)
        {
            real_matrix4x3 relative_target;
            real_matrix4x3 inverse_basis;
            real_matrix4x3 inverse_end;
            matrix4x3_inverse(basis, &inverse_basis);
            matrix4x3_compute_product(target, &inverse_basis, &relative_target);
            real_matrix4x3 *end = &node_matrices[node_index];
            matrix4x3_inverse(end, &inverse_end);
            if (fabsf(weight - bipeds_ik_get_one()) >= bipeds_ik_epsilon)
            {
                float inverse = bipeds_ik_get_one() - weight;
                relative_target.position.x = end->position.x * inverse + relative_target.position.x * weight;
                relative_target.position.y = end->position.y * inverse + relative_target.position.y * weight;
                relative_target.position.z = end->position.z * inverse + relative_target.position.z * weight;
            }
            if (root_node == 0)
            {
                memcpy(&inverse_basis, node_matrices, sizeof(inverse_basis));
                matrix4x3_inverse_kinematics_adjust_matrices_new(&relative_target, &inverse_basis, &node_matrices[parent_node], end);
            }
            else
            {
                matrix4x3_inverse_kinematics_adjust_matrices_new(&relative_target, &node_matrices[root_node], &node_matrices[parent_node], end);
            }
            matrix4x3_compute_product(end, &inverse_end, &inverse_basis);
            model_adjust_node_matrices_children_recursive(model, &inverse_basis, node_index, node_count, node_matrices);
        }
    }
}

// @address 0x346538 ?biped_bring_ragdoll_inside_world@@YA_NJ_NPBTreal_point3d@@M@Z
bool biped_bring_ragdoll_inside_world(long biped_index, bool flag, const real_point3d *position, float radius)
{
    c_runtime_node_to_havok_component_rigid_body_index_mapping mapping(biped_index);
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, 1);
    c_havok_component *component = havok_component_get(biped->object.havok_component_index);
    const long k_maximum_rigid_bodies = 32;
    long rigid_body_count = component->get_rigid_body_count();
    bool result = false;
    unsigned long rigid_body_flags[1];
    memset(rigid_body_flags, 0, sizeof(rigid_body_flags));
    if (rigid_body_count >= 1 && rigid_body_count <= k_maximum_rigid_bodies && mapping.get_node_count() > 0)
    {
        long root_rigid_body_index = mapping.get_rigid_body_index(0);
        if (root_rigid_body_index >= 0 && root_rigid_body_index < rigid_body_count)
        {
            bool safe = biped_bring_rigid_body_to_safe_position(biped_index, flag, position, radius, root_rigid_body_index, NONE, NONE, false);
            rigid_body_flags[root_rigid_body_index >> 5] |= 1 << (root_rigid_body_index & 31);
            result = biped_bring_ragdoll_inside_world_recursive(biped_index, flag, position, radius, &mapping, rigid_body_flags,
                root_rigid_body_index, safe);
        }
    }
    return result;
}

// Helpers and externals used by biped_update_camera (0x345300).


// @address 0x345300 ?biped_update_camera@@YAXJ@Z
void biped_update_camera(long biped_index)
{
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, 1);
    if (biped->unit.player_index == NONE)
        return;
    if (motor_animation_resource_busy(biped_index, _motor_resource_2) ||
        motor_animation_resource_busy(biped_index, _motor_resource_3))
        return;

    object_marker marker;
    real_matrix4x3 *node_matrix = object_get_node_matrix(biped_index, 0);
    if (internal_object_get_markers_by_string_id(biped_index, 204, &marker, 1, false) == 0)
        internal_object_get_markers_by_string_id(biped_index, 157, &marker, 1, false);

    real_vector3d delta;
    vector_from_points3d(&node_matrix->position, &marker.matrix.position, &delta);

    real_vector3d aiming_vector = biped->unit.aiming_vector;
    unit_clip_vector_to_aiming_screen_bounds(biped_index, &aiming_vector, false);

    real_vector2d forward2d;
    real_vector2d left2d;
    biped_build_2d_camera_frame(&aiming_vector, &biped->object.up, &forward2d, &left2d);

    real_vector3d offset;
    offset.k = delta.k;
    offset.i = delta.i * forward2d.i + delta.j * forward2d.j;
    offset.j = delta.i * left2d.i + delta.j * left2d.j;

    if (TEST_BIT(biped->biped.flags_bytes[BIG_ENDIAN_VIEW_INDEX(0, 2)], 0))
    {
        interpolate_scalar(&biped->biped.camera_offset.i, offset.i, 0.01f);
        interpolate_scalar(&biped->biped.camera_offset.j, offset.j, 0.01f);
        interpolate_scalar(&biped->biped.camera_offset.k, offset.k, 0.01f);
        return;
    }
    real_vector3d *camera_offset = &biped->biped.camera_offset;
    *camera_offset = offset;
    biped->biped.flags |= 0x100;
}
/* ---- biped footstep effects (0x122330) ---- */

struct s_collision_bsp_cache;

static __forceinline collision_bsp *biped_footstep_global_collision_bsp_get(long index)
{
#ifdef HALO3_64BIT
    return ((TAG_POINTER(collision_bsp) *)(&g_collision_bsp_cache))[get_current_thread_index() * 64 + index];
#else
    return ((collision_bsp **)(&g_collision_bsp_cache))[get_current_thread_index() * 64 + index];
#endif
}

static __forceinline real_plane3d *biped_footstep_plane_from_designator(collision_bsp *bsp, long designator, real_plane3d *result)
{
    real_plane3d *plane = &((real_plane3d *)bsp->planes.address)[designator & 0x7fff];
    if (designator & 0x8000)
    {
        result->normal.i = -plane->normal.i;
        result->normal.j = -plane->normal.j;
        result->normal.k = -plane->normal.k;
        result->distance = -plane->distance;
    }
    else
    {
        *result = *plane;
    }
    return result;
}
static __forceinline bool bipeds_footstep_test_physics_flag(unsigned long flags, long bit)
{
    unsigned char result = (flags >> bit) & 1;
    return result;
}

static __forceinline void bipeds_footstep_material_effect_new(long material_effects_index, long object_index,
    e_effect_type effect_type, e_sweetener_size sweetener_size, c_global_material_type material,
    bool underwater, const real_point3d *position, const real_vector3d *normal,
    const real_vector3d *direction, const s_location *location, float volume,
    const s_bipeds_footstep_water_interaction *interaction, const real_plane3d *surface_plane)
{
    long effects[7];
    material_effect_find_effects(false, position, material_effects_index, k_global_material_type_invalid,
        material, underwater, effect_type, sweetener_size, effects);
    if (effects[6] != NONE && interaction &&
        active_user_close_to_point_not_deterministic(position, 40.0f, _adjust_for_fov_scale_none) && g_restricted_address[1])
        c_water_renderer::game_interation_event_add(effects[6], position, &interaction->direction, &interaction->velocity);
    effect_spawn_impact_effects(effects, position, normal, direction, location, surface_plane);
    game_sound_new_material_effect_sounds(effects, position, normal, direction, (const long *)location, object_index, volume);
}

// @address 0x122330 ?biped_make_footstep@@YAXJW4e_effect_type@@W4e_contact_point@@M@Z
void biped_make_footstep(long biped_index, e_effect_type effect_type, e_contact_point contact_point, float volume)
{
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, 1);
    biped_definition *definition = (biped_definition *)tag_get('bipd', biped->object.definition_index);
    if (biped->biped.physics.get_mode() != c_character_physics_component::_mode_ground)
        return;
    if (definition->unit.material_effects.index == NONE)
        return;

    real_point3d position;
    object_marker marker;
    if (contact_point < definition->contact_points.count &&
        internal_object_get_markers_by_string_id(biped_index, ((long *)definition->contact_points.address)[contact_point], &marker, 1,
            false) != 0)
        position = marker.matrix.position;
    else
    {
        object_get_origin(biped_index, &position);
    }

    if (bipeds_footstep_test_physics_flag(biped->object.physics_flags, 7))
    {
        const c_global_material_type &ground_material = biped->biped.physics.get_mode_ground()->get_ground_material_type();
        real_plane3d surface_plane;
        real_plane3d *surface_plane_ptr = 0;
        float water_height = water_physics_get_depth_at_point(&position);
        if (water_height >= 0.0f)
        {
            volume = water_height;
            volume = PIN(volume, 0.0f, 1.0f);
        }
        if (biped->biped.pathfinding_structure_index != NONE &&
            biped->biped.pathfinding_instanced_geometry_index == NONE &&
            biped->biped.pathfinding_bsp_reference == NONE &&
            biped->biped.pathfinding_surface_index != NONE)
        {
            collision_bsp *bsp = biped_footstep_global_collision_bsp_get(biped->biped.pathfinding_structure_index);
            collision_surface *surface = &((collision_surface *)bsp->surfaces.address)[biped->biped.pathfinding_surface_index];
            surface_plane_ptr = biped_footstep_plane_from_designator(
                biped_footstep_global_collision_bsp_get(biped->biped.pathfinding_structure_index), surface->plane, &surface_plane);
        }
        const s_location *location = &biped->object.location;
        const real_vector3d *ground_plane = biped->biped.physics.get_mode_ground()->get_ground_plane();
        bipeds_footstep_material_effect_new(definition->unit.material_effects.index, biped_index, effect_type,
            (e_sweetener_size)(unsigned char)definition->unit.sweetener_size, ground_material, false, &position,
            global_down3d, ground_plane, location, volume, 0, surface_plane_ptr);
    }
    else
    {
        if (biped->object.parent_object_index != NONE)
            return;
        s_model_definition *model = (s_model_definition *)tag_get('hlmt', definition->unit.model.index);
        if (model->nodes.count <= 0)
            return;

        biped_datum *foot_biped = (biped_datum *)object_get_and_verify_type(biped_index, 1);
        real_matrix4x3 *node_matrices = (real_matrix4x3 *)((char *)foot_biped + foot_biped->object.node_matrices.offset);
        real_point3d foot_position = node_matrices[0].position;
        collision_result collision;
        collision.global_material.m_index = NONE;
        point_from_line3d(&foot_position, global_up3d, 0.1f, &foot_position);

        real_vector3d down;
        scale_vector3d(global_down3d, 1.0f, &down);
        if (!collision_test_vector(k_biped_footstep_collision_flags, true, &foot_position, &down, NONE, NONE, NONE, &collision))
            return;

        const s_location *location = &biped->object.location;
        bipeds_footstep_material_effect_new(definition->unit.material_effects.index, biped_index, effect_type,
            (e_sweetener_size)(unsigned char)definition->unit.sweetener_size, collision.global_material, true, &foot_position,
            global_down3d, global_up3d, location, volume, 0, 0);
    }
}

// @rdata 0x82022dc0 bipeds_turn_half
static const float bipeds_turn_half = 0.5f;
// @rdata 0x82022850 bipeds_turn_height_scale
static const float bipeds_turn_height_scale = 10.0f;
// @rdata 0x8202490c bipeds_turn_epsilon
static const float bipeds_turn_epsilon = 0.0001f;
// @rdata 0x82032938 bipeds_turn_negative_one
static const float bipeds_turn_negative_one = -1.0f;

static __forceinline bool bipeds_turn_definition_flag(unsigned long flags, long bit)
{
    return ((flags >> bit) & 1) != 0;
}


// @address 0x10a580 ?biped_update_moving_turn_constraints@@YAXJ@Z
void biped_update_moving_turn_constraints(long biped_index)
{
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, 1);
    const biped_definition *definition = (const biped_definition *)tag_get('bipd', biped->object.definition_index);
    c_flags_no_init<e_biped_flag, unsigned short, 11> *flags = &biped->biped.m_flags;
    flags->set(_biped_moving_turn_constraints, false);
    if (biped->object.parent_object_index == NONE && biped->biped.physics.get_mode() == c_character_physics_component::_mode_ground
        && !motor_system_in_airborne_state(biped_index) && !bipeds_turn_definition_flag(definition->physics.flags, 3))
    {
        object_marker first_marker;
        object_marker second_marker;
        if (internal_object_get_markers_by_string_id(biped_index, 0x36b, &first_marker, 1, false)
            && internal_object_get_markers_by_string_id(biped_index, 0x36c, &second_marker, 1, false))
        {
            real_matrix4x3 world;
            object_get_world_matrix(biped_index, &world);
            biped_datum *node_biped = (biped_datum *)object_get_and_verify_type(biped_index, 1);
            const real_matrix4x3 *root = (const real_matrix4x3 *)((const char *)node_biped + node_biped->object.node_matrices.offset);
            float height = root->position.z - world.position.z;
            if (height > bipeds_turn_epsilon)
            {
                real_matrix4x3 ground;
                memcpy(&ground, &world, sizeof(ground));
                ground.position.x = root->position.x;
                ground.position.y = root->position.y;
                real_point3d first;
                real_point3d second;
                real_point3d selected;
                matrix4x3_inverse_transform_point(&ground, &first_marker.matrix.position, &first);
                matrix4x3_inverse_transform_point(&ground, &second_marker.matrix.position, &second);
                bool use_second = true;
                float difference = second.z - first.z;
                if (difference > definition->moving_turn_height_threshold
                    || (difference > -definition->moving_turn_height_threshold && fabsf(first.x) < fabsf(second.x)))
                    use_second = false;
                if (use_second)
                    selected = second;
                else
                    selected = first;
                float width = definition->moving_turn_width_scale * height;
                float ratio = MIN(MAX(selected.x / (width * bipeds_turn_half), bipeds_turn_negative_one), 1.0f);
                float turn_scale = 1.0f - ratio * ratio * ratio;
                float attenuation;
                if (use_second)
                {
                    float fraction = second.z * bipeds_turn_height_scale;
                    float delta = 1.0f - PIN(fraction, 0.0f, 1.0f);
                    attenuation = delta * delta;
                }
                else
                {
                    float fraction = first.z * bipeds_turn_height_scale;
                    float delta = 1.0f - PIN(fraction, 0.0f, 1.0f);
                    attenuation = delta * delta;
                }
                biped->biped.moving_turn_scale = MIN(MAX(attenuation * turn_scale, 0.25f), 1.0f);
                if (biped->unit.player_index == NONE)
                {
                    float scale = (1.0f - definition->moving_turn_minimum) * (1.0f - biped->biped.moving_turn_scale) + biped->biped.moving_turn_scale;
                    biped->biped.moving_turn_scale = scale;
                    biped->biped.moving_turn_scale = MIN(MAX(biped->biped.moving_turn_scale, 0.0f), 1.0f);
                }
                else
                    biped->biped.moving_turn_scale = 1.0f;
                const real_point3d *marker = &first_marker.matrix.position;
                if (use_second)
                    marker = &second_marker.matrix.position;
                matrix4x3_inverse_transform_point(&world, marker, &selected);
                selected.z = 0.0f;
                real_vector3d displacement;
                vector_from_points3d(&biped->biped.moving_turn_position, &selected, &displacement);
                point_from_line3d(&biped->biped.moving_turn_position, &displacement,
                    definition->moving_turn_smoothing, &biped->biped.moving_turn_position);
                real_point3d *moving_turn_position = &biped->biped.moving_turn_position;
                moving_turn_position->z = 0.0f;
                flags->set(_biped_moving_turn_constraints, true);
            }
        }
    }
    if (!flags->test(_biped_moving_turn_constraints))
    {
        biped->biped.moving_turn_scale = 1.0f;
        biped->biped.moving_turn_remaining_angle = 0.0f;
        biped->biped.moving_turn_position = *global_origin3d;
    }
}


/* ---- ground fitting (biped_adjust_node_matrices) ---- */


static __forceinline void *bipeds_object_header_block_get_with_count(
    biped_datum *object, const object_header_block_reference *reference, unsigned int element_size, long *count)
{
    *count = reference->size / element_size;
    return (char *)object + reference->offset;
}

static __forceinline float bipeds_matrix3x3_determinant(const real_matrix3x3 *matrix)
{
    return matrix->n[0][0] * matrix->n[1][1] * matrix->n[2][2]
        + matrix->n[0][1] * matrix->n[1][2] * matrix->n[2][0]
        + matrix->n[0][2] * matrix->n[1][0] * matrix->n[2][1]
        - matrix->n[0][0] * matrix->n[1][2] * matrix->n[2][1]
        - matrix->n[0][1] * matrix->n[1][0] * matrix->n[2][2]
        - matrix->n[0][2] * matrix->n[1][1] * matrix->n[2][0];
}



static __forceinline float bipeds_real_cosine(double angle)
{
    return (float)cos(angle);
}

// @address 0x109ad8 ?biped_adjust_node_matrices@@YAXJJPAUreal_matrix4x3@@@Z
void biped_adjust_node_matrices(long biped_index, long node_matrices_count, real_matrix4x3 *node_matrices)
{
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, 1);
    const biped_definition *definition = (const biped_definition *)tag_get('bipd', biped->object.definition_index);
    const c_animation_manager *animation = (const c_animation_manager *)((char *)biped + biped->object.animation.offset);
    biped_update_moving_turn_constraints(biped_index);
    bool grounded = biped->biped.physics.get_mode() == c_character_physics_component::_mode_ground
        && mover_get_motor_program(biped_index) == 0
        && !motor_system_in_airborne_state(biped_index);
    if (biped->object.parent_object_index != NONE)
        return;
    if (!grounded && !(bool)((biped->biped.flags >> 13) & 1))
        return;
    if (bipeds_turn_definition_flag(definition->physics.flags, 3))
        return;

    bool motion_data = animation->current_state_contains_motion_data() && !(bool)((biped->biped.flags >> 13) & 1);
    object_marker markers[2];
    if (!internal_object_get_markers_by_string_id(biped_index, 0x36b, &markers[0], 1, false))
        return;
    if (!internal_object_get_markers_by_string_id(biped_index, 0x36c, &markers[1], 1, false))
        return;

    real_vector3d root_offset = *global_zero_vector3d;
    real_matrix4x3 target_matrices[2];
    XMemCpy(&target_matrices[0], &markers[0].matrix, sizeof(real_matrix4x3));
    XMemCpy(&target_matrices[1], &markers[1].matrix, sizeof(real_matrix4x3));
    bool adjusted = false;

    real_matrix4x3 world_matrix;
    object_get_world_matrix(biped_index, &world_matrix);
    real_point3d root_position = node_matrices->position;
    float root_height = root_position.z - world_matrix.position.z;
    float height = MAX(bipeds_turn_epsilon, root_height);
    if (((biped_datum *)object_get_and_verify_type(biped_index, NONE))->object.node_orientations.offset != NONE)
    {
        long orientation_count;
        const real_orientation *orientations = (const real_orientation *)bipeds_object_header_block_get_with_count(
            biped, &biped->object.node_orientations, sizeof(real_orientation), &orientation_count);
        if (orientations && orientation_count)
            height = MAX(bipeds_turn_epsilon, orientations->translation.z);
    }

    real_vector3d ground_normal;
    if (biped->biped.physics.get_mode() == c_character_physics_component::_mode_ground)
        ground_normal = *biped->biped.physics.get_mode_ground()->get_ground_plane();
    else
        ground_normal = *global_up3d;
    double slope_angle = definition->physics.ground.maximum_slope_angle;
    pin_normal_to_cone3d(&ground_normal, global_up3d, (float)sin(slope_angle), bipeds_real_cosine(slope_angle), &ground_normal);
    real_vector3d normal_delta;
    subtract_vectors3d(&biped->biped.previous_ground_normal, &ground_normal, &normal_delta);
    point_from_line3d(&ground_normal, &normal_delta, definition->ground_normal_dampening, &ground_normal);
    normalize3d(&ground_normal);
    biped->biped.previous_ground_normal = ground_normal;

    real_point3d pill_position;
    float pill_height;
    float pill_radius;
    biped_get_physics_pill_from_definition(biped_index, &pill_position, &pill_height, &pill_radius);
    float contact_offset = (biped->biped.physics.get_mode() == c_character_physics_component::_mode_ground &&
        !biped->biped.physics.get_mode_ground()->ground_contact_offset_disabled())
        ? 0.0f : -g_havok_constants.collision_tolerance;
    real_point3d foot;
    foot.x = pill_position.x + ground_normal.i * (contact_offset - pill_radius);
    foot.y = pill_position.y + ground_normal.j * (contact_offset - pill_radius);
    foot.z = pill_position.z + ground_normal.k * (contact_offset - pill_radius);
    float foot_height = world_matrix.position.z - foot.z;
    float foot_depth = MAX(0.0f, foot_height);

    if (fabsf(ground_normal.k - 1.0f) >= bipeds_turn_epsilon)
    {
        real_vector2d slope;
        float inverse_k = 1.0f / ground_normal.k;
        slope.i = ground_normal.i * inverse_k;
        slope.j = ground_normal.j * inverse_k;
        slope.i = -slope.i;
        slope.j = -slope.j;
        real_vector2d world_delta;
        world_delta.i = world_matrix.position.x - foot.x;
        world_delta.j = world_matrix.position.y - foot.y;
        float world_ground_offset = dot_product2d(&world_delta, &slope);
        real_matrix4x3 *target = target_matrices;
        object_marker *marker = markers;
        for (long marker_index = 0; marker_index < 2; marker_index++, target++, marker++)
        {
            real_vector2d marker_delta;
            vector_from_points2d(&foot.xy, &marker->matrix.position.xy, &marker_delta);
            float ground_z = dot_product2d(&marker_delta, &slope) + foot.z;
            float height_delta = root_position.z - marker->matrix.position.z;
            float weight = PIN(height_delta / height, 0.0f, 1.0f);
            weight = weight * weight;
            float delta_z = ground_z - target->position.z;
            if (delta_z > 0.0f)
                target->position.z = ground_z;
            else
                target->position.z = target->position.z + delta_z * weight;
            target->position.z = (target->position.z > root_position.z) ? root_position.z : target->position.z;
            if (!motion_data)
            {
                real_matrix3x3 axes;
                real_vector3d_build_axes_from_vectors_using_up(&marker->matrix.forward, &ground_normal, &axes.forward, &axes.left, &axes.up);
                real_matrix3x3 inverse;
                float determinant = bipeds_matrix3x3_determinant(&marker->matrix.rotation);
                matrix3x3_inverse(&marker->matrix.rotation, determinant, &inverse);
                real_matrix4x3 rotation;
                matrix3x3_multiply(&inverse, &axes, &rotation.rotation);
                rotation.position = *global_origin3d;
                real_euler_angles3d angles;
                following_camera_deterministic(&rotation, &angles);   // matrix4x3_rotation_to_angles
                angles.yaw = 0.0f;
                angles.roll = PIN(angles.roll, -definition->foot_roll_maximum, definition->foot_roll_maximum);
                angles.pitch = PIN(angles.pitch, -definition->foot_pitch_maximum, definition->foot_pitch_maximum);
                matrix4x3_rotation_from_angles(&rotation, angles.yaw, angles.pitch, angles.roll);
                real_quaternion orientation;
                matrix3x3_rotation_to_quaternion(&rotation.rotation, &orientation);
                quaternions_interpolate(global_identity_quaternion, &orientation, weight, &orientation);
                quaternion_normalize(&orientation);
                matrix3x3_rotation_from_quaternion(&rotation.rotation, &orientation);
                matrix3x3_multiply(&marker->matrix.rotation, &rotation.rotation, &target->rotation);
            }
        }
        adjusted = true;
        if (!motion_data)
        {
            real_vector2d direction;
            direction.i = -ground_normal.i;
            direction.j = -ground_normal.j;
            normalize2d(&direction);
            float lean_scale = -(definition->root_leaning_scale * height);
            root_offset.i = lean_scale * direction.i;
            root_offset.j = lean_scale * direction.j;
            if (fabsf(root_offset.i) >= bipeds_turn_epsilon || fabsf(root_offset.j) >= bipeds_turn_epsilon)
            {
                direction.i = root_offset.i;
                direction.j = root_offset.j;
                float length = normalize2d(&direction);
                real_vector2d target_delta0;
                real_vector2d target_delta1;
                target_delta0.i = target_matrices[0].position.x - world_matrix.position.x;
                target_delta0.j = target_matrices[0].position.y - world_matrix.position.y;
                target_delta1.i = target_matrices[1].position.x - world_matrix.position.x;
                target_delta1.j = target_matrices[1].position.y - world_matrix.position.y;
                float distance0 = direction.i * target_delta0.i + direction.j * target_delta0.j;
                float distance1 = direction.i * target_delta1.i + direction.j * target_delta1.j;
                float distance = MAX(distance0, distance1);
                float distance_squared = distance * distance;
                float scale = MIN(length, distance_squared);
                root_offset.i = direction.i * scale;
                root_offset.j = direction.j * scale;
            }
            float offset0 = target_matrices[0].position.z - markers[0].matrix.position.z;
            float offset1 = target_matrices[1].position.z - markers[1].matrix.position.z;
            real_bounds offset_bounds;
            float lower_height = height;
            offset_bounds.lower = fabsf(lower_height) * 0.5f;
            offset_bounds.upper = fabsf(height) * 0.33f;
            offset_bounds.lower = -offset_bounds.lower;
            offset0 = PIN(offset0, offset_bounds.lower, offset_bounds.upper);
            offset1 = PIN(offset1, offset_bounds.lower, offset_bounds.upper);
            float minimum = (offset0 > offset1) ? offset1 : offset0;
            float maximum = MAX(offset0, offset1);
            minimum = MIN(0.0f, minimum);
            maximum = MAX(maximum, 0.0f);
            float root_z = (maximum + minimum) * 0.5f + world_ground_offset;
            root_z = PIN(root_z, -offset_bounds.upper, offset_bounds.upper);
            float &root_height_offset = root_offset.k;
            target_matrices[0].position.z = markers[0].matrix.position.z + offset0;
            target_matrices[1].position.z = markers[1].matrix.position.z + offset1;
            root_height_offset = root_z;
        }
    }
    else
    {
        float lowered0 = target_matrices[0].position.z - foot_depth;
        target_matrices[0].position.z = MAX(foot.z, lowered0);
        float lowered1 = target_matrices[1].position.z - foot_depth;
        target_matrices[1].position.z = MAX(foot.z, lowered1);
    }

    float limit = definition->root_offset_max_scale * height;
    root_offset.k -= foot_depth;
    if (fabsf(biped->biped.landing_root_offset) >= bipeds_turn_epsilon)
    {
        float adjusted_z = biped->biped.landing_root_offset + root_offset.k;
        root_offset.k = PIN(adjusted_z, -limit, limit);
    }
    scale_point3d(&biped->biped.following_camera_offset, definition->root_offset_dampening, &biped->biped.following_camera_offset);
    point_from_line3d(&biped->biped.following_camera_offset, &root_offset, 1.0f - definition->root_offset_dampening,
        &biped->biped.following_camera_offset);
    scale_point3d(&biped->biped.following_camera_offset, 1.0f - biped->unit.crouch_amount, &biped->biped.following_camera_offset);
    biped->biped.following_camera_offset.z = PIN(biped->biped.following_camera_offset.z, -limit, limit);
    if (fabs(biped->biped.following_camera_offset.x) >= bipeds_turn_epsilon || fabsf(biped->biped.following_camera_offset.y) >=
        bipeds_turn_epsilon ||
        fabsf(biped->biped.following_camera_offset.z) >= bipeds_turn_epsilon)
    {
        adjusted = true;
        for (long node_index = 0; node_index < node_matrices_count; node_index++)
        {
            for (long k = 0; k < 3; k++)
                node_matrices[node_index].position.n[k] += biped->biped.following_camera_offset.n[k];
        }
    }
    memcpy(&biped->biped.camera_c, &target_matrices[0], sizeof(real_matrix4x3));
    memcpy(&biped->biped.camera_d, &target_matrices[1], sizeof(real_matrix4x3));
    if (adjusted)
    {
        const s_model_definition *model = (const s_model_definition *)tag_get('hlmt', definition->unit.model.index);
        biped_apply_inverse_kinematics(model->render_model_index, markers[0].node_index, &markers[0].node_to_marker, &target_matrices[0],
            1.0f, node_matrices_count, node_matrices);
        biped_apply_inverse_kinematics(model->render_model_index, markers[1].node_index, &markers[1].node_to_marker, &target_matrices[1],
            1.0f, node_matrices_count, node_matrices);
    }
}
static __forceinline c_animation_id bipeds_root_velocity_get_animation_id(const c_animation_channel *channel)
{
    return channel->m_animation_id;
}
// @address 0x35bcd0 biped_get_animation_root_velocity
extern "C" unsigned char biped_get_animation_root_velocity(long unit_index, real_vector3d *velocity)
{
    bool result = false;
    if (unit_index != -1 && action_executing(unit_index, _action_custom_animation))
    {
        biped_datum *unit = (biped_datum *)object_get_and_verify_type(unit_index, 1);
        const biped_definition *definition = (const biped_definition *)tag_get(0, unit->object.definition_index);
        const s_model_definition *model =
            (const s_model_definition *)tag_get(0, definition->unit.model.index);
        const render_model_definition *render_model = 0;
        if (model->render_model_index != -1)
            render_model = (const render_model_definition *)tag_get(0, model->render_model_index);
        c_animation_manager *manager = (c_animation_manager *)((char *)unit + unit->object.animation.offset);
        const c_animation_channel *channel = &manager->m_state_channel;
        if (manager->m_transition_channel.valid())
            channel = &manager->m_transition_channel;
        if (channel && bipeds_root_velocity_get_animation_id(channel).valid())
        {
            c_model_animation_graph::get_animation_root_velocity(manager->m_graph_tag_index, channel->m_animation_id,
                channel->m_frame_position * 0.033333335f, channel->m_playback_speed, render_model, velocity);
            real_matrix4x3 matrix;
            matrix4x3_transform_vector(object_get_world_matrix(unit_index, &matrix), velocity, velocity);
            result = 1;
        }
    }
    return result;
}


static __forceinline s_planar_rotation bipeds_rotation_from_angle(float angle)
{
    s_planar_rotation rotation;
    rotation.cosine = (float)cos(angle);
    rotation.sine = (float)sin(angle);
    return rotation;
}
static __forceinline void bipeds_rotate_vector(real_vector3d *vector, const real_vector3d *perpendicular, float sine, float cosine)
{
    vector->i = vector->i * cosine + perpendicular->i * sine;
    vector->j = vector->j * cosine + perpendicular->j * sine;
    vector->k = vector->k * cosine + perpendicular->k * sine;
}
static __forceinline void bipeds_yaw_vectors(real_vector3d *vector, const real_vector3d *axis, float sine, float cosine)
{
    real_vector3d perpendicular;
    cross_product3d(axis, vector, &perpendicular);
    bipeds_rotate_vector(vector, &perpendicular, sine, cosine);
}
static __forceinline void bipeds_pitch_rotate(real_vector3d *vector, const real_vector3d *perpendicular, const float &sine, float cosine)
{
    vector->i = vector->i * cosine + perpendicular->i * sine;
    vector->j = vector->j * cosine + perpendicular->j * sine;
    vector->k = vector->k * cosine + perpendicular->k * sine;
}
static __forceinline void bipeds_pitch_vectors(real_vector3d *forward, real_vector3d *up, float sine, const float &cosine)
{
    real_vector3d negative_forward;
    negative_forward.i = -forward->i;
    negative_forward.j = -forward->j;
    negative_forward.k = -forward->k;
    bipeds_pitch_rotate(forward, up, sine, cosine);
    bipeds_pitch_rotate(up, &negative_forward, sine, cosine);
}
static __forceinline float bipeds_angle_difference(float a, float b)
{
    float difference = a - b;
    if (difference >= biped_half_turn_angle()) difference -= biped_full_turn_angle();
    if (difference <= -biped_half_turn_angle()) difference += biped_full_turn_angle();
    return difference;
}

// @address 0xe68e8 ?biped_update_melee_turning@@YA_NJPATreal_vector3d@@@Z
bool biped_update_melee_turning(long object_index, real_vector3d *aiming_vector)
{
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(object_index, 1);
    bool result = false;
    if (biped->biped.physics.get_mode() == c_character_physics_component::_mode_melee)
    {
        long target_index = biped->biped.physics.get_mode_melee()->get_target_unit_index();
        if (target_index != NONE)
        {
            const real_vector3d *initial_aiming_vector = biped->biped.physics.get_mode_melee()->get_initial_aiming_vector();
            long weapon_index = unit_inventory_get_weapon_or_none(object_index, biped->unit.current_weapon_index);
            bool energy_blade = weapon_is_energy_blade(weapon_index);
            s_biped_melee_target_parameters initial_parameters;
            biped_calculate_melee_target_parameters(object_index, target_index, initial_aiming_vector, &initial_parameters);
            if (biped_is_melee_target_in_range((const s_biped_melee_target_parameters *)&initial_parameters, energy_blade, true))
            {
                s_biped_melee_target_parameters parameters;
                biped_calculate_melee_target_parameters(object_index, target_index, &biped->unit.aiming_vector, &parameters);
                if (biped_is_melee_target_in_range((const s_biped_melee_target_parameters *)&parameters, energy_blade, true))
                {
                    const float &max_turn = game_tick_length() * 9.424777887761593;
                    real_vector3d up;
                    generate_up_vector3d(aiming_vector, &up);
                    float yaw_difference = bipeds_angle_difference(parameters.aiming_angles.yaw, initial_parameters.aiming_angles.yaw);
                    float yaw = parameters.target_angles.yaw;
                    yaw = PIN(yaw, -0.87266463f - yaw_difference, 0.87266463f - yaw_difference);
                    yaw = PIN(yaw, -max_turn, max_turn);
                    bipeds_yaw_vectors(aiming_vector, &up, (float)sin(yaw), (float)cos(yaw));
                    float pitch_difference = parameters.aiming_angles.pitch - initial_parameters.aiming_angles.pitch;
                    float pitch = parameters.target_angles.pitch;
                    pitch = PIN(pitch, -0.2617994f - pitch_difference, 0.43633232f - pitch_difference);
                    pitch = PIN(pitch, -max_turn, max_turn);
                    const s_planar_rotation &rotation = bipeds_rotation_from_angle(pitch);
                    bipeds_pitch_vectors(aiming_vector, &up, rotation.sine, rotation.cosine);
                    result = true;
                }
            }
        }
    }
    return result;
}

// @rdata 0x82022838
static const float bipeds_sight_collision_margin = 0.05f;

static __forceinline bool bipeds_sight_has_variable_up(long unit_index)
{
    biped_datum *object = (biped_datum *)object_get_and_verify_type(unit_index, 0x3003);
    short offset = object->unit.motor_program.offset;
    long program = *(long *)((char *)object + offset);
    bool result = false;
    if (program == 7 || program == 8)
        result = true;
    return result;
}
static __forceinline bool bipeds_sight_suppressed_camera(long unit_index)
{
    c_character_physics_component *physics = &((biped_datum *)object_get_and_verify_type(unit_index, 1))->biped.physics;
    bool result = false;
    if (physics->get_mode() == c_character_physics_component::_mode_melee &&
        physics->get_mode_melee()->has_suppressed_camera())
        result = true;
    return result;
}

static __forceinline float bipeds_sight_height_dot(const real_vector3d *a, const real_vector3d *b)
{
    return a->i * b->i + (a->k * b->k + a->j * b->j);
}
static __forceinline void bipeds_sight_offset_forward(real_point3d *position, const real_vector3d *forward, float distance)
{
    float x = position->x;
    x += forward->i * distance;
    position->x = x;
    float y = position->y;
    y += forward->j * distance;
    position->y = y;
    float z = position->z;
    z += forward->k * distance;
    position->z = z;
}

// @address 0x9a938 biped_estimate_camera_position
extern "C" void biped_estimate_camera_position(long unit_index, short estimate_mode, bool offset_camera,
    const real_point3d *estimated_body_position, const real_vector3d *estimated_up,
    const real_vector3d *desired_facing, const real_vector3d *desired_gun_offset,
    real_point3d *estimated_position)
{
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(unit_index, 1);
    biped_definition *definition = (biped_definition *)tag_get('bipd', biped->object.definition_index);
    float camera_height = 0.0f;
    const real_vector3d *camera_up = 0;
    float crouch_amount = 0.0f;
    if (estimated_up)
        camera_up = estimated_up;
    else if (bipeds_sight_has_variable_up(unit_index))
        camera_up = &biped->object.up;

    if (estimate_mode == 0)
        object_get_origin(unit_index, estimated_position);
    else
        *estimated_position = *estimated_body_position;

    switch (estimate_mode)
    {
    case 2:
    case 5:
        crouch_amount = 1.0f;
        break;
    case 1:
    case 4:
        crouch_amount = 0.0f;
        break;
    case 0:
        if (bipeds_sight_suppressed_camera(unit_index))
            crouch_amount = 0.0f;
        else
            crouch_amount = biped->unit.crouch_amount;
        break;
    case 3:
        break;
    default:
        // match: 0x9a938: unreachable hint omits dead branch check
        __assume(0);
    }

    switch (estimate_mode)
    {
    case 3:
        {
            real_vector3d left = { -desired_facing->j, desired_facing->i, 0.0f };
            bipeds_sight_offset_forward(estimated_position, desired_facing, desired_gun_offset->i);
            point_from_line3d(estimated_position, &left, desired_gun_offset->j, estimated_position);
            estimated_position->z += desired_gun_offset->k;
        }
        break;
    case 4:
    case 5:
        {
            real_point3d aim_position;
            if (unit_get_aim_position(unit_index, &aim_position))
            {
                real_point3d origin;
                real_vector3d forward, up, delta;
                object_get_origin(unit_index, &origin);
                object_get_orientation(unit_index, &forward, &up);
                vector_from_points3d(&origin, &aim_position, &delta);
                camera_height = bipeds_sight_height_dot(&delta, &up);
                camera_height += (definition->standing_camera_height - definition->crouching_camera_height) *
                    (biped->unit.crouch_amount - crouch_amount) * biped->object.scale;
                break;
            }
        }
    case 0:
    case 1:
    case 2:
        camera_height = ((1.0f - crouch_amount) * definition->standing_camera_height +
            definition->crouching_camera_height * crouch_amount) * biped->object.scale;
        break;
    default:
        // match: 0x9a938: unreachable hint omits dead branch check
        __assume(0);
    }
    if (camera_height != 0.0f)
    {
        if (camera_up)
            point_from_line3d(estimated_position, camera_up, camera_height, estimated_position);
        else
            estimated_position->z += camera_height;
    }

    if (offset_camera)
    {
        collision_result collision;
        real_point3d camera_position = *estimated_position;
        real_vector3d forward = biped->unit.aiming_vector;
        real_vector3d up;
        if (camera_up)
        {
            real_vector3d left;
            cross_product3d(camera_up, &forward, &left);
            cross_product3d(&forward, &left, &up);
            if (normalize3d(&up) == 0.0f)
                generate_up_vector3d(&forward, &up);
        }
        else
            generate_up_vector3d(&forward, &up);
        biped_adjust_camera_position(unit_index, &camera_position, &forward, &up);
        real_vector3d direction;
        vector_from_points3d(estimated_position, &camera_position, &direction);
        if (!(normalize3d(&direction) <= 0.0f))
        {
            real_vector3d backwards;
            scale_vector3d(&direction, -0.25f, &backwards);
            real_point3d start;
            if (collision_test_vector(k_biped_camera_collision_flags, false, estimated_position, &backwards, NONE, NONE, NONE, &collision))
                point_from_line3d(estimated_position, &backwards, collision.fraction * 0.9f, &start);
            else
                start = collision.point;
            if (collision_test_vector(k_biped_camera_collision_flags, false, &start, &direction, NONE, NONE, NONE, &collision))
            {
                real_vector3d collision_delta, camera_delta;
                vector_from_points3d(&start, &collision.point, &collision_delta);
                vector_from_points3d(&start, &camera_position, &camera_delta);
                if (!(dot_product3d(&collision_delta, &direction) >=
                    dot_product3d(&camera_delta, &direction) + bipeds_sight_collision_margin))
                {
                    estimated_position->x = collision.point.x - direction.i * bipeds_sight_collision_margin;
                    estimated_position->y = collision.point.y - direction.j * bipeds_sight_collision_margin;
                    estimated_position->z = collision.point.z - direction.k * bipeds_sight_collision_margin;
                    return;
                }
            }
            *estimated_position = camera_position;
        }
    }
}


// match: 0x9d3c8: multiplies the midpoint sum by 0.5f with the constant last; the shared helper puts it first.

static __forceinline real_matrix4x3 *bipeds_object_get_node_matrix(long object_index, long node_index)
{
    biped_datum *object = (biped_datum *)object_get_and_verify_type(object_index, NONE);
    return (real_matrix4x3 *)((char *)object + object->object.node_matrices.offset) + node_index;
}


// @address 0x9d3c8 ?biped_get_autoaim_pill@@YAXJPATreal_point3d@@PATreal_vector3d@@PAM@Z
void biped_get_autoaim_pill(long biped_index, union real_point3d *position, union real_vector3d *vector, float *radius)
{
    biped_datum *object = (biped_datum *)object_get_and_verify_type(biped_index, 1);
    biped_definition *definition = (biped_definition *)tag_get('bipd', object->object.definition_index);
    if (definition->autoaim_node_a != NONE && definition->autoaim_node_b != NONE)
    {
        real_matrix4x3 *matrix_a = bipeds_object_get_node_matrix(biped_index, definition->autoaim_node_a);
        real_matrix4x3 *matrix_b = bipeds_object_get_node_matrix(biped_index, definition->autoaim_node_b);
        bool fixed = (definition->physics.flags >> 1) & 1;
        if (fixed)
        {
            real_point3d *pill_origin = position;
            real_point3d *pill_center = pill_origin;
            real_point3d *pill_point = pill_center;
            real_point3d *midpoint_dest = pill_point;
            real_point3d *midpoint_target = midpoint_dest;
            real_point3d *midpoint_origin = midpoint_target;
            midpoint3d(&matrix_a->position, &matrix_b->position, midpoint_origin);
            *vector = *global_zero_vector3d;
        }
        else
        {
            *position = matrix_a->position;
            vector_from_points3d(&matrix_a->position, &matrix_b->position, vector);
        }
        *radius = definition->autoaim_radius;
    }
    else
    {
        float height;
        float offset;
        biped_get_physics_pill_from_definition(biped_index, position, &height, &offset);
        position->z += 0.5f * height;
        scale_vector3d(global_up3d, height * 0.5f, vector);
        *radius = definition->autoaim_radius;
    }
}

// @address 0xa1698 ?biped_can_trade_weapon_with_player@@YA_NJJ@Z
bool biped_can_trade_weapon_with_player(long player_index, long unit_index)
{
    bool result = false;
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 1);
    if (unit->unit.actor_index != NONE && unit->object.parent_object_index == NONE)
    {
        actor_datum *actor = datum_get_typed<actor_datum>(tls_actor_data(), unit->unit.actor_index);
        character_general_properties *general_properties = character_definition_general_properties_get(actor->character_index);
        if (!general_properties || !TEST_BIT(general_properties->flags, 5))
        {
            player_datum *player = datum_get_typed<player_datum>(tls_player_data(), player_index);
            long unit_weapon_index = unit_inventory_get_weapon_or_none(unit_index, unit->unit.current_weapon_index);
            long player_weapon_index = unit_inventory_get_weapon_or_none(
                player->unit_index, ((unit_datum *)object_get_and_verify_type(player->unit_index, 8195))->unit.current_weapon_index);
            if (unit_weapon_index != NONE && player_weapon_index != NONE)
            {
                real_point3d player_head_position;
                real_point3d unit_head_position;
                unit_get_head_position(datum_get_typed<player_datum>(tls_player_data(), player_index)->unit_index, &player_head_position);
                unit_get_head_position(unit_index, &unit_head_position);
                real_vector3d direction;
                direction.i = unit_head_position.x - player_head_position.x;
                direction.j = unit_head_position.y - player_head_position.y;
                direction.k = unit_head_position.z - player_head_position.z;
                float distance = normalize3d(&direction);
                if (distance > 0.0f && distance < 1.0f)
                {
                    real_vector3d aiming_vector = ((unit_datum *)object_get_and_verify_type(player->unit_index, 8195))->unit.aiming_vector;
                    if (dot_product3d(&aiming_vector, &direction) > 0.8f)
                    {
                        bool facing;
                        if (((unit_datum *)object_get_and_verify_type(unit_index, 1))->unit.actor_index != NONE)
                            facing = true;
                        else
                        {
                            real_vector3d forward;
                            object_get_orientation(unit_index, &forward, 0);
                            facing = dot_product3d(&forward, &direction) < 0.0f;
                        }
                        if (facing && motor_animation_busy(unit_index) && !action_executing(unit_index, _action_ai_impulse))
                            facing = false;
                        if (facing)
                        {
                            unit_weapon_pickup_result pickup_result;
                            if (unit_can_pickup_weapon(unit_index, player_weapon_index,
                                _weapon_addition_method_swap_for_primary_weapon, &pickup_result) && pickup_result.backpack[0]
                                && unit_can_pickup_weapon(player->unit_index, unit_weapon_index,
                                    _weapon_addition_method_swap_for_primary_weapon, &pickup_result) && pickup_result.backpack[0])
                                result = true;
                        }
                    }
                }
            }
        }
    }
    return result;
}


static __forceinline bool bipeds_unit_is_dual_wielding(long unit_index)
{
    biped_datum *unit = (biped_datum *)object_get_and_verify_type(unit_index, 8195);
    bool result = false;
    if (unit->unit.current_weapon_index != NONE && unit->unit.secondary_weapon_index != NONE)
        result = true;
    return result;
}

static __forceinline float bipeds_update_vehicle_ik_slider(c_mixing_board_slider *slider, bool enabled, bool interpolate, float
    interpolation_time);

// @address 0x10dff8 ?biped_postprocess_node_matrices@@YAXJJPAUreal_matrix4x3@@@Z
void biped_postprocess_node_matrices(long biped_index, long node_count, real_matrix4x3 *node_matrices)
{
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, 1);
    c_flags_no_init<e_unit_animation_flags, unsigned short, 9> *animation_flags =
        (c_flags_no_init<e_unit_animation_flags, unsigned short, 9> *)((char *)biped + biped->unit.animation_storage.offset);
    c_animation_manager *animation = (c_animation_manager *)((char *)biped + biped->object.animation.offset);
    if (animation->valid())
    {
        bool vehicle_ik = unit_animation_allows_vehicle_ik(biped_index);
        bool weapon_ik = unit_animation_allows_weapon_ik(biped_index);
        bool interpolate = !unit_animation_allows_playback_adjustment(biped_index, _animation_playback_flag_none);
        if (bipeds_unit_is_dual_wielding(biped_index))
            weapon_ik = false;
        c_mixing_board_slider *vehicle_slider = animation->get_mixing_board_slider(6);
        const float interpolation_time = bipeds_update_vehicle_ik_slider(vehicle_slider, vehicle_ik, interpolate,
            ((float)k_standard_interpolation_time));
        c_mixing_board_slider *weapon_slider = animation->get_mixing_board_slider(5);
        if (weapon_ik)
        {
            weapon_slider->move_to_target(_slider_target_position_full, _slider_movement_pattern_full_ease_in, 0.13333334f);
            mixing_board_slider_update(weapon_slider);
        }
        else if (interpolate)
        {
            weapon_slider->move_to_target(_slider_target_position_one, _slider_movement_pattern_ease_in, 0.06666667f);
            mixing_board_slider_update(weapon_slider);
        }
        else
            weapon_slider->snap_to_target(_slider_target_position_one);
        animation_manager_apply_vehicle_ik(biped_index, biped->object.parent_object_index, node_count, node_matrices);
        if (biped->unit.current_weapon_index != NONE && bipeds_weapon_is_valid(animation))
        {
            process_mode_ik(biped_index,
                unit_inventory_get_weapon_or_none(
                    biped_index, ((unit_datum *)object_get_and_verify_type(biped_index, 8195))->unit.current_weapon_index),
                node_count, node_matrices);
            animation_flags->set(_unit_animation_flag_weapon_ik, false);
        }
        c_aiming_channel *aiming_channel = animation->get_mixing_board_aiming_channel(4);
        bool aiming_ik = aiming_channel->valid();
        c_animation_channel *channel = animation->get_mixing_board_animation_channel(2);
        if (channel->valid() && bipeds_get_impulse_name(channel) != 491)
            aiming_ik = false;
        if (animation->current_state_contains_motion_data())
            aiming_ik = false;
        if (biped->biped.aiming_vector_magnitude < k_biped_aiming_vector_minimum_magnitude)
            aiming_ik = false;
        c_mixing_board_slider *aiming_slider = animation->get_mixing_board_slider(7);
        if (aiming_ik)
        {
            aiming_slider->move_to_target(_slider_target_position_full, _slider_movement_pattern_light_ease_in_and_out, interpolation_time);
            return;
        }
        aiming_slider->move_to_target(_slider_target_position_one, _slider_movement_pattern_light_ease_in_and_out, 0.06666667f);
    }
}

static __forceinline float bipeds_update_vehicle_ik_slider(c_mixing_board_slider *slider, bool enabled, bool interpolate, float
    interpolation_time)
{
    if (enabled)
    {
        slider->move_to_target(_slider_target_position_full, _slider_movement_pattern_light_ease_in_and_out, interpolation_time);
        mixing_board_slider_update(slider);
    }
    else if (interpolate)
    {
        slider->move_to_target(_slider_target_position_one, _slider_movement_pattern_light_ease_in_and_out, 0.06666667f);
        mixing_board_slider_update(slider);
    }
    else
        slider->snap_to_target(_slider_target_position_one);
    return interpolation_time;
}


/* ---- biped update (dev name biped_update) ---- */


static __forceinline bool bipeds_biped_update_stun(long biped_index)
{
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, 1);
    bool result = false;
    if (biped->biped.stun_ticks > 0)
    {
        biped->biped.stun_ticks--;
        result = true;
    }
    return result;
}

static __forceinline bool bipeds_actor_handle_kill_volume(long actor_index)
{
    actor_datum *actor = datum_get_typed<actor_datum>(tls_actor_data(), actor_index);
    bool handled = false;
    for (short squad_index = 0; squad_index < 8; squad_index++)
    {
        if (tls_actor_globals()->mission_critical[squad_index].unit_index == actor->unit_index)
        {
            tls_actor_globals()->mission_critical[squad_index].flags |= 2;
            tls_actor_globals()->mission_critical[squad_index].bring_forward_ticks = NONE;
            tls_actor_globals()->mission_critical[squad_index].bring_forward_value = 0.0f;
            handled = true;
            break;
        }
    }
    return handled;
}


template<typename t_type> static __forceinline void bipeds_xchg(t_type *a, t_type *b)
{
    t_type temp = *a;
    *a = *b;
    *b = temp;
}



// match: 0xe5950: direct compare keeps lwzx r31-first order; the shared inline's named local flips 17 operand pairs.
static __forceinline bool bipeds_game_is_predicted()
{
    bool result = false;
    if (tls_game_globals()->options.simulation_type == 4)
        result = true;
    return result;
}

// @address 0xe5950 ?biped_update@@YA_NJ@Z
bool biped_update(long biped_index)
{
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, 1);
    bool result = false;
    if (biped->unit.player_index != NONE)
    {
        if (!unit_vehicle_is_flying(inl_object_get_ultimate_parent(biped_index)))
            object_update_soft_ceiling(biped_index, &datum_get_typed<player_datum>(tls_player_data(), biped->unit.player_index)->position);
    }
    if (biped->unit.actor_index != NONE)
        biped->biped.combat_status = (unsigned char)datum_get_typed<actor_datum>(tls_actor_data(), biped->unit.actor_index)->combat_status;
    if (biped->object.havok_component_index != NONE)
    {
        result = bipeds_biped_update_stun(biped_index);
        long ultimate_parent_index = inl_object_get_ultimate_parent(biped_index);
        if (!(bool)((((biped_datum *)object_get_and_verify_type(ultimate_parent_index, NONE))->object.physics_flags >> 9) & 1))
        {
            bool safe;
            if (scenario_kill_trigger_volumes_test_object(biped_index, &safe) || !safe)
            {
                bool handled = false;
                if (biped->unit.actor_index != NONE)
                    handled = bipeds_actor_handle_kill_volume(biped->unit.actor_index);
                if (!handled)
                {
                    biped->biped.m_flags.set(_biped_kill_volume_damaged, true);
                    if (!bipeds_game_is_predicted())
                    {
                        s_falling_damage_definition *damage_globals = (s_falling_damage_definition *)global_game_globals->falling_damage.address;
                        s_damage_data damage_data;
                        damage_data_new(&damage_data, damage_globals->distance_damage.index);
                        damage_data.reporting_info = 4;
                        damage_data.flags |= 4;
                        damage_compute_damage_owner_from_object_index(ultimate_parent_index, &damage_data.owner);
                        object_cause_damage_simple(&damage_data, biped_index);
                    }
                }
            }
        }
        long root_index = object_get_root_object(biped->object.parent_object_index);
        if (root_index != NONE)
        {
            long root_type = object_header_get(root_index)->object_type;
            if (biped->biped.physics.get_mode() == c_character_physics_component::_mode_melee)
                biped_enter_default_physics(biped_index);
            if (root_type == 1)
            {
                vehicle_datum *vehicle = (vehicle_datum *)object_get_and_verify_type(root_index, 2);
                biped_vehicle_speech(biped_index, root_index);
                if (!bipeds_game_is_predicted() && vehicle->vehicle.upside_down_ticks > 0)
                {
                    long ticks = bipeds_bump_seconds_to_ticks_round(0.55f);
                    if (biped->unit.parent_seat_index != NONE)
                    {
                        object_datum *parent = (object_datum *)object_get_and_verify_type(biped->object.parent_object_index, _object_mask_unit);
                        unit_definition *definition = (unit_definition *)tag_get('vehi', parent->definition_index);
                        unit_seat *seat = &((unit_seat *)definition->seats.address)[biped->unit.parent_seat_index];
                        if (seat->speech_delay > 0.0f)
                            ticks = bipeds_bump_seconds_to_ticks_round(seat->speech_delay);
                    }
                    if (vehicle->vehicle.upside_down_ticks > ticks)
                        action_submit_event(biped_index, 20);
                }
                result = true;
            }
        }
        else
        {
            biped_definition *definition = (biped_definition *)tag_get('bipd', biped->object.definition_index);
            if ((bool)((biped->object.damage_flags >> 2) & 1) ||
                biped->biped.physics.get_mode() != c_character_physics_component::_mode_flying && !(bool)((definition->physics.flags >>
                    3) & 1) &&
                    mover_get_motor_program(biped_index) != 7)
            {
                biped->unit.desired_facing_vector.k = 0.0f;
                if (normalize3d(&biped->unit.desired_facing_vector) == 0.0f)
                    biped->unit.desired_facing_vector = *global_forward3d;
            }
            if (magnitude_squared3d(&biped->unit.throttle) < 0.0001f)
                biped->unit.throttle = *global_zero_vector3d;
            if (biped_should_remain_active(biped_index))
            {
                result = true;
                if ((bool)((biped->object.physics_flags >> 9) & 1))
                    object_set_at_rest(biped_index, false);
            }
            if (TEST_BIT(biped->unit.control_flags, 1))
            {
                if (biped->biped.jump_control_ticks < 127)
                    biped->biped.jump_control_ticks = biped->biped.jump_control_ticks + 1;
            }
            else
            {
                biped->biped.jump_control_ticks = 0;
            }
        }
        biped_update_camera(biped_index);
    }
    c_animation_manager *animation = (c_animation_manager *)((char *)biped + biped->object.animation.offset);
    c_aiming_channel *aiming_channel = animation->get_mixing_board_aiming_channel(4);
    if (aiming_channel->valid())
    {
        real_matrix4x3 world_matrix;
        real_matrix4x3 matrix;
        real_vector3d down;
        real_vector3d aiming_vector;
        const real_vector3d *aiming_source = &biped->biped.aiming_vector;
        ::object_get_world_matrix(biped_index, (::real_matrix4x3 *)&world_matrix);
        memcpy(&matrix, &world_matrix, sizeof(real_matrix4x3));
        object_type_fix_transform_from_physics(biped_index, &matrix);
        bipeds_xchg(&matrix.up, &matrix.forward);
        scale_vector3d(&matrix.up, -1.0f, &matrix.up);
        matrix3x3_inverse_transform_vector(&matrix.rotation, global_down3d, &down);
        add_vectors3d(aiming_source, &down, &aiming_vector);
        normalize3d(&aiming_vector);
        if (aiming_vector.k < 0.0f)
            aiming_vector.k = 0.0f;
        biped->biped.aiming_vector_magnitude = normalize3d(&aiming_vector);
        biped->biped.aiming_vector = aiming_vector;
    }
    else
    {
        biped->biped.aiming_vector_magnitude = 0.0f;
        biped->biped.aiming_vector = *global_zero_vector3d;
    }
    real_matrix4x3 *node_matrices = (real_matrix4x3 *)((char *)biped + biped->object.node_matrices.offset);
    if (node_matrices && (long)(biped->object.node_matrices.size / sizeof(real_matrix4x3)) > 0)
    {
        for (long i = NUMBEROF(biped->biped.node_matrix_history) - 1; i > 0; i--)
            biped->biped.node_matrix_history[i] = biped->biped.node_matrix_history[i - 1];
        biped->biped.node_matrix_history[0] = node_matrices[0];
    }
    return result;
}
