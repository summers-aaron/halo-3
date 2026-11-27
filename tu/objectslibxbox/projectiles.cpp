// objectslibxbox/projectiles.obj: from the records 0xc09b8 0xc0aa8 0xc0b78 0xf35b8 0xf3c60 0xf3f40 0xfbb18 0xfcd70 0x1b6f48 0x33e888 0x33ec10 0x33ed38 0x33f130 0x3400d8 0x340400 0x340750 0x340ab8 0x340b20 0x340df0 0x340e38 0x342588 0x342640 0x342748 0x3427a8 0x3429e8 0x342d80 0x342ea0 0x3430b0 0x3432e0 0x343c08 0x343e00 0x344048 0x344320 0x344408 0x344468 0x344510 0x344840 0x344910 0x344ea0 0x345060 0x35a378 0x7092c0
// @flags /TP /O2
#include "physics/physics_constants_tls.h"
#include "math/real_math_constant_pool.h"
#include "simulation/simulation_queue_event_types.h"
#include "physics/collision_types.h"
#include "math/matrix_math_types.h"
#include "math/matrix_math_functions.h"
#include "math/real_math_types.h"
#include "math/real_math.h"
#include "math/point_math.h"
#include "cseries/cseries_wrapped.h"
#include "memory/data_types.h"
#include "memory/static_arrays.h"
#include "tag_files/tag_types.h"
#include "game/game_time_types.h"
#include "objects/object_reference_types.h"
#include "objects/object_datum.h"
#include "objects/object_placement.h"
#include "objects/object_iterator.h"
#include "memory/data.h"
#include "memory/datum_index.h"
#include "memory/tls_globals.h"
#include "game/player_types.h"
#include "ai/actor_globals.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <new>
#include "game/players.h"
#include "objects/objects.h"
#include "scenario/scenario.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "sound/game_sound.h"
#include "units/units.h"
#include "ai/actor_combat.h"
#include "ai/ai.h"
#include "effects/effects.h"
#include "game/game_time.h"
#include "items/projectiles.h"
#include "math/periodic_functions.h"
#include "math/random_math.h"
#include "objects/damage.h"
#include "objects/widgets/widgets.h"
#include "physics/breakable_surfaces.h"
#include "objects/object_types.h"
#include "objects/object_definitions.h"
#include "physics/collisions.h"
#include "physics/havok_utilities.h"
#include "physics/water_physics.h"
#include "physics/water_physics_types.h"
#include "game/game_globals.h"
#include "game/materials_definitions.h"
#include "phys/phys_dynamics.h"
#include "cache/cache_files.h"
#include "networking/network_configuration.h"
#include "simulation/game_interface/simulation_game_events.h"
#include "physics/havok_component.h"
#include "effects/effect_vector_types.h"
#include "effects/effect_deterministic_types.h"
#include "game/player_mapping_types.h"
#include "sound/game_sound_types.h"
#include "models/model_target_types.h"
#include "models/model_types.h"
#include "units/unit_types.h"
#include "effects/material_effects.h"
#include "objects/damage_data.h"
#include "game/material_constants.h"
#include "items/projectile_definitions.h"
#include "items/equipment_definitions.h"
#include "items/weapon_definitions.h"
#include "ai/weapon_datum_types.h"
#include "game/game_globals_storage.h"
#include "game/game_simulation_types.h"
#include "ai/ai_math_constants.h"
#include "physics/collision_test_flags.h"
#include "items/projectile_flags.h"
#include "items/projectile_types.h"
#include "items/projectile_material_response_definitions.h"
#include "effects/effect_data.h"
#include "math/real_constant_pool.h"
#include "physics/physics_constants_globals.h"
#include "networking/network_configuration_types.h"
#include "effects/effect_creation_flags.h"
#include "effects/effect_creation_data.h"
#include "cseries/crt_xdk.h"
#include "tag_files/tag_index.h"
#include "effects/effect_creation.h"
#include "objects/object_early_mover_queries.h"
#include "items/projectile_translation.h"
#include "phys/phys_math.h"
#include "physics/havok_component_class.h"
#include "items/projectile_collision_queries.h"
#include "models/model_damage_info.h"
#include "tag_files/tag_groups.h"
#include "effects/material_effect_enums.h"
#include "game/game_time_inline.h"
#include "math/random_math_range.h"
#include "objects/object_header_access.h"
#include "tag_files/string_ids.h"

// match: 0x33f130: canonical object-prefix adoption changes native c2 operand ranks.
struct s_projectile_object_datum : object_datum, _projectile_datum {
    using _projectile_datum::flags;
};

struct s_projectile_supercombine_payload { long definition; };

struct s_projectile_sound_data {
    s_sound_location sound;
    s_location location;
};

struct s_projectile_brute_grenade_definition {
    float angular_velocity_minimum;
    float angular_velocity_maximum;
    float spin;
    float air_damping;
    float air_alignment_scale;
    float air_alignment_strength;
    float air_alignment_power;
    float ground_plane_radius;
    float ground_alignment_strength;
    float ground_alignment_scale;
    float ground_alignment_power;
    float ground_damping;
};

struct detonate_collision_result : collision_result {
    __forceinline detonate_collision_result() {}
};

struct translate_attachment {
    unsigned char type;
    long effect_index;
};

struct translate_collision_result : collision_result {
    __forceinline translate_collision_result() {}
};

struct conical_distribution {
    short angular_count;
    short radial_count;
    float exponent;
    float maximum_angle;
};


typedef char projectile_layout_check_0[(sizeof(s_projectile_object_datum) == 0x1f8) ? 1 : -1];
typedef char projectile_layout_check_1[(sizeof(void *) != 4 || ((offsetof(s_projectile_object_datum, hit_normal) == 0x168))) ? 1 : -1];
typedef char projectile_layout_check_2[(offsetof(projectile_definition, guided1) == 0x23c) ? 1 : -1];
typedef char dp20_layout_1[(sizeof(void *) != 4 || ((offsetof(s_projectile_object_datum, flags) == 0x160))) ? 1 : -1];
typedef char dp20_layout_2[(offsetof(projectile_definition, detonation_damage.index) == 0x164) ? 1 : -1];
typedef char dp20_layout_3[(offsetof(projectile_definition, super_impact_damage.index) == 0x1b8) ? 1 : -1];
typedef char dp20_layout_4[(offsetof(projectile_definition, attached_impact_damage.index) == 0x220) ? 1 : -1];

// @address 0x32938 k_real_constants_minus_one


#if defined(_XBOX) && !defined(__clang__)
#define k_real_zero AI_REAL_CONSTANTS[-553]
#define k_real_one AI_REAL_CONSTANTS[-753]
#else
#define k_real_zero 0.0f
#define k_real_one 1.0f
#endif
#define k_real_half AI_REAL_CONSTANTS[0]

static __forceinline short game_player_count(void)
{
  game_globals_storage *globals = game_globals;
  return globals->options.game_mode == 1 ? globals->options.campaign_difficulty : 1;
}

float projectile_definition_get_velocity_multiplier(long tag_index)
{
  float result = k_real_one;
  projectile_definition *definition = (projectile_definition *)tag_get(0, tag_index);
  if (definition->single_player_velocity > k_real_zero || definition->multiplayer_velocity > k_real_zero) {
    float single_player_velocity = definition->single_player_velocity > k_real_zero ? definition->single_player_velocity : k_real_one;
    float multiplayer_velocity = definition->multiplayer_velocity > k_real_zero ? definition->multiplayer_velocity : k_real_one;
    short player_count = game_player_count();
    if (player_count <= 1)
      result = single_player_velocity;
    else if (player_count >= 3)
      result = multiplayer_velocity;
    else
      result = single_player_velocity + (multiplayer_velocity - single_player_velocity) * (float)(player_count - 1) * k_real_half;
  }
  return result;
}

collision_result &collision_result::operator=(const collision_result &other)
{
    type = other.type;
    fraction = other.fraction;
    point = other.point;
    start_location = other.start_location;
    location = other.location;
    bsp = other.bsp;
    global_material = other.global_material;
    plane = other.plane;
    geometry_index = other.geometry_index;
    instance_index = other.instance_index;
    object_index = other.object_index;
    region_index = other.region_index;
    node_index = other.node_index;
    bsp_reference = other.bsp_reference;
    structure_bsp_index = other.structure_bsp_index;
    leaf_index = other.leaf_index;
    surface_index = other.surface_index;
    plane_index = other.plane_index;
    flags = other.flags;
    breakable_surface_bit = other.breakable_surface_bit;
    material_index = other.material_index;
    breakable_surface = other.breakable_surface;
    return *this;
}


// match: 0x33f130: nested object_get changes indexed-load operand ranks.
// match: 0x340e38: object_header.h declarations change collision dot-product operand ranks.
static __forceinline s_projectile_object_datum *projectile_object_get_datum(long index)
{
    return (s_projectile_object_datum *)((s_object_header *)object_header_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(index)].datum;
}
// @address 0x344320 ?projectile_count_attached_to_object_for_supercombine_recursive@@YAJJJPAJ@Z
long projectile_count_attached_to_object_for_supercombine_recursive(long object_index, long definition_index, long *first_object_index)
{
    int count = 0;
    int child_index = projectile_object_get_datum(object_index)->first_child_object_index;

    while (child_index != -1) {
        s_projectile_object_datum *child = projectile_object_get_datum(child_index);
        if (child->definition_index == definition_index && child->damage_owner.player_index != -1) {
            bool hidden = (child->flags >> 6) & 1;
            if (!hidden) {
                count++;
                if (first_object_index && *first_object_index == -1)
                    *first_object_index = child_index;
            }
        }
        if (child->first_child_object_index != -1 && !((1 << child->object_type) & _object_mask_biped))
            count += projectile_count_attached_to_object_for_supercombine_recursive(child_index, definition_index, first_object_index);
        child_index = child->next_object_index;
    }
    return count;
}

// @address 0xfbb18 projectile_count_attached_to_object_for_supercombine
extern "C" long projectile_count_attached_to_object_for_supercombine(long index, long definition, long *projectile, long *object)
{
    long result = 0;
    s_projectile_object_datum *datum = projectile_object_get_datum(index);
    long projectile_index = -1;
    while (datum->parent != -1 && !((1 << datum->object_type) & 1)) {
        index = datum->parent;
        datum = projectile_object_get_datum(index);
    }
    result = projectile_count_attached_to_object_for_supercombine_recursive(index, definition, &projectile_index);
    if (projectile) *projectile = projectile_index;
    if (object) *object = index;
    return result;
}

// @address 0x344408 ?projectile_handle_parent_destroyed@@YA_NJ@Z
bool projectile_handle_parent_destroyed(long object_index)
{
    s_projectile_object_datum *object = projectile_object_get_datum(object_index);
    object->scale_b = 1.0f;
    object->scale_a = 1.0f;
    object->flags.set(_projectile_perception_attached_bit, false);
    object_detach(object_index);
    return true;
}


static __forceinline void *projectile_tag_block_element(const s_tag_block *block,long index,long size) { return (char *)block->address+index*size; }
static __forceinline s_global_material_definition *projectile_global_get_material(c_global_material_type material)
{
    s_global_material_definition *result=0;
    if(material.m_index!=-1) {
        if(material.m_index>=0 && material.m_index<((s_game_globals *)global_game_globals)->materials.count())
            result=(s_global_material_definition *)((s_game_globals *)global_game_globals)->materials.address + material.m_index;
    }
    return result;
}
// @address 0x344468 ?projectile_definition_find_material_response@@YAPBUs_projectile_material_response_definition@@PAUprojectile_definition@@Vc_global_material_type@@@Z
struct s_projectile_material_response_definition const *projectile_definition_find_material_response(
    struct projectile_definition *definition, class c_global_material_type material_type)
{
    const s_projectile_material_response_definition *result=0;
    long count=definition->responses.count;
    c_global_material_type current=material_type;
    while(!result && projectile_global_get_material(current)) {
        s_global_material_definition *global_material=projectile_global_get_material(current);
        for(long i=0;i<count;i++) {
            s_projectile_material_response_definition *response = (s_projectile_material_response_definition *)projectile_tag_block_element(
                &definition->responses, i, 64);
            if(response->material==current) {result=response; break;}
        }
        current=global_material->parent_material_type;
    }
    if(!result) result=&default_projectile_material_response;
    return result;
}


// @address 0x33ec10 projectile_get_target_position
extern "C" void projectile_get_target_position(const s_projectile_targeting_data *target, real_point3d *position)
{
    long object_index = target->object_index;
    s_projectile_object_datum *object = projectile_object_get_datum(object_index);
    unit_get_body_position(object_index, position);
    if (target->model_target_index != -1) {
        long model_index =
            ((const s_object_definition *)tag_get(0, object->definition_index))
                ->model.index;
        if (model_index != -1) {
            const s_model_definition *model = (const s_model_definition *)tag_get(0, model_index);
            if (model->targets.count > target->model_target_index) {
                object_marker markers[2];
                short marker_count = internal_object_get_markers_by_string_id(
                    object_index,
                    ((const s_model_target *)model->targets.address)[target->model_target_index].marker_name,
                    markers,
                    2,
                    false);
                if (marker_count > 1) {
                    position->x = (markers[0].matrix.position.x + markers[1].matrix.position.x) * 0.5f;
                    position->y = (markers[0].matrix.position.y + markers[1].matrix.position.y) * 0.5f;
                    position->z = (markers[0].matrix.position.z + markers[1].matrix.position.z) * 0.5f;
                    return;
                }
                if (marker_count > 0) {
                    *position = markers[0].matrix.position;
                }
            }
        }
    }
}

// @address 0x340ab8 projectile_estimate_time_to_target
extern "C" float projectile_estimate_time_to_target(long tag_index, float value)
{
    float scale = projectile_definition_get_velocity_multiplier(tag_index) *
        ((const projectile_definition *)tag_get(0, tag_index))->guided1;
    float result = 0.0f;
    if (scale > 0.0f)
        result = value / scale;
    return result;
}

// @address 0x340df0 projectile_clear_object_references
extern "C" void projectile_clear_object_references(long projectile_index, long object_index)
{
    s_projectile_object_datum *projectile = projectile_object_get_datum(projectile_index);
    if (projectile->target_object_index == object_index)
    {
        projectile->target_object_index = -1;
        projectile->target_node_index = -1;
    }
    if (projectile->ignore_object_index == object_index)
        projectile->ignore_object_index = -1;
}

// @address 0xc0b78 ?projectile_aim_linear@@YA_NMPBTreal_point3d@@0PATreal_vector3d@@PAM22@Z
bool projectile_aim_linear(float speed, const real_point3d *origin, const real_point3d *target, real_vector3d *result_aim_vector,
    float *result_speed, float *result_time, float *result_distance)
{
    real_vector3d aim_vector;
    vector_from_points3d(origin, target, &aim_vector);
    float distance = normalize3d(&aim_vector);
    if (distance == 0.0f)
        aim_vector = k_real_math_identity_matrix_storage.up;
    float time;
    if (speed > 0.0001f)
        time = distance / speed;
    else
        time = 0.0f;
    *result_aim_vector = aim_vector;
    if (result_distance)
        *result_distance = distance;
    if (result_speed)
        *result_speed = speed;
    if (result_time)
        *result_time = time;
    return true;
}

#define projectiles_max(a,b) ((a)>(b)?(a):(b))
#define projectiles_min(a,b) ((a)<(b)?(a):(b))
static __forceinline bool projectile_flag_test(unsigned long flags, long bit)
{
    bool result = (flags >> bit) & 1;
    return result;
}

// @address 0xf3c60 ?projectile_compute_function_value@@YA_NJJJPAMPA_N1@Z
bool projectile_compute_function_value(long object_index, long function, long unused, float *result, bool *active, bool *unused_out)
{
    s_projectile_object_datum *projectile = projectile_object_get_datum(object_index);
    const projectile_definition *definition =
        (const projectile_definition *)tag_get(0, projectile->definition_index);
    float value = 1.0f;
    bool handled = true;
    switch (function) {
    case _string_id_time_remaining:
        value = projectile->scale_a;
        break;
    case _string_id_range_remaining:
        value = definition->lifetime != 0.0f ? projectile->time / definition->lifetime : 0.0f;
        break;
    case _string_id_tracer:
        value = projectile_flag_test((unsigned long)projectile->flags, _projectile_tracer_bit) ? 1.0f : 0.0f;
        break;
    case _string_id_acceleration_range:
        if (projectile_flag_test((unsigned long)projectile->flags, _projectile_disable_deceleration_bit)) {
            value = 1.0f;
        } else if (definition->guided1 != 0.0f && definition->guided2 != 0.0f) {
            float guided = (projectile->time - definition->start) * definition->scale;
            value = projectiles_min(projectiles_max(guided, 0.0f), 1.0f);
        } else {
            value = 1.0f;
        }
        break;
    case _string_id_projectile_attach:
        value = projectile_flag_test((unsigned long)projectile->flags, _projectile_perception_attached_bit) ? 1.0f : 0.0f;
        break;
    default:
        handled = false;
        break;
    }
    if (handled) {
        *result = value;
        *active = value > 0.0f;
    }
    return handled;
}

static __forceinline float magnitude_squared2d_from_3d(const real_vector3d *vector)
{
    return vector->i * vector->i + vector->j * vector->j;
}
static __forceinline float magnitude2d_from_3d(const real_vector3d *vector)
{
    return square_root(magnitude_squared2d_from_3d(vector));
}

// the parameter where a ray from point along vector meets the plane, 0 when parallel
// @address 0x1b6f48 ?vector_intersect_plane3d@@YAMPBTreal_point3d@@PBTreal_vector3d@@PBUreal_plane3d@@@Z
#if defined(_XBOX) && !defined(__clang__)
inline float vector_intersect_plane3d(const real_point3d *point, const real_vector3d *vector, const real_plane3d *plane)
#else
float vector_intersect_plane3d(const real_point3d *point, const real_vector3d *vector, const real_plane3d *plane)
#endif
{
    float rate = vector->i * plane->normal.i + vector->j * plane->normal.j + vector->k * plane->normal.k;
    float result = 0.0f;
    if (rate != 0.0f) {
        float distance = point->x * plane->normal.i + point->y * plane->normal.j + point->z * plane->normal.k - plane->distance;
        result = -(distance / rate);
    }
    return result;
}

// @address 0x342748 ?projectile_calculate_maximum_damage_distance@@YAXJ@Z
void projectile_calculate_maximum_damage_distance(long projectile_index)
{
    s_projectile_object_datum *projectile = projectile_object_get_datum(projectile_index);
    const projectile_definition *definition =
        (const projectile_definition *)tag_get(0, projectile->definition_index);
    if ((unsigned char)((projectile->object_flags >> 3) & 1) != 0)
        projectile->maximum_damage_distance = definition->maximum_damage_distance_flagged;
    else
        projectile->maximum_damage_distance = definition->maximum_damage_distance_default;
}

// @address 0x345060 ?projectile_hit_normal_get@@YAPBTreal_vector3d@@JPAT1@@Z
const real_vector3d *projectile_hit_normal_get(long projectile_index, real_vector3d *hit_normal)
{
    s_projectile_object_datum *projectile = projectile_object_get_datum(projectile_index);
    if (projectile->parent == -1) {
        *hit_normal = projectile->hit_normal;
    } else {
        real_matrix4x3 world_matrix;
        real_matrix4x3 *node_matrix;
        if (projectile->parent_node_index == -1) {
            node_matrix = object_get_world_matrix(projectile->parent, &world_matrix);
        } else {
            s_projectile_object_datum *parent = projectile_object_get_datum(projectile->parent);
            node_matrix = (real_matrix4x3 *)((char *)parent + parent->node_matrices_offset + projectile->parent_node_index * sizeof(real_matrix4x3));
        }
        float normal_i = projectile->hit_normal.i;
        float normal_j = projectile->hit_normal.j;
        float normal_k = projectile->hit_normal.k;
        hit_normal->i = node_matrix->forward.i * normal_i + (node_matrix->up.i * normal_k + node_matrix->left.i * normal_j);
        hit_normal->j = node_matrix->forward.j * normal_i + (node_matrix->up.j * normal_k + node_matrix->left.j * normal_j);
        hit_normal->k = node_matrix->forward.k * normal_i + (node_matrix->up.k * normal_k + node_matrix->left.k * normal_j);
    }
    return hit_normal;
}

// 0x342640

// @address 0x342640 ?projectile_adjust_for_angular_velocity_change@@YAXJ@Z
void projectile_adjust_for_angular_velocity_change(long projectile_index)
{
    s_projectile_object_datum * projectile = projectile_object_get_datum(projectile_index);
    real_vector3d axis = projectile->angular_velocity;
    float angular_speed = normalize3d(&axis);
    if (angular_speed > 0.0001f) {
        float angle = angular_speed * game_tick_length();
        projectile->flags.set(_projectile_rotating_bit, true);
        projectile->rotation_axis = axis;
        projectile->rotation_sine = (float)sin(angle);
        projectile->rotation_cosine = (float)cos(angle);
    } else {
        projectile->flags.set(_projectile_rotating_bit, false);
        projectile->rotation_sine = 0.0f;
        projectile->rotation_cosine = 1.0f;
    }
}


static __forceinline long projectile_supercombine_object_type(long index)
{
  return object_header_get(index)->object_type;
}
static __forceinline bool projectile_supercombine_flags_test(unsigned long flags, long bit)
{
  return (bool)((flags >> bit) & 1);
}
static __forceinline bool projectile_supercombine_game_is_campaign()
{
  return game_globals->options.game_mode == 1;
}
static __forceinline bool projectile_supercombine_game_is_predicted()
{
  bool result = false;
  if (game_globals->options.game_simulation == _game_simulation_distributed_client)
    result = true;
  return result;
}
static __forceinline void projectile_supercombine_damage_owner_get(long index, s_damage_owner *owner)
{
  *owner = projectile_object_get_datum(index)->damage_owner;
}
static __forceinline void projectile_supercombine_set_action(long index, short action)
{
  s_projectile_object_datum *projectile = projectile_object_get_datum(index);
  if (action > projectile->action)
    projectile->action = (signed char)action;
}
static __forceinline void projectile_supercombine_request(long index)
{
  s_projectile_object_datum *projectile = projectile_object_get_datum(index);
  s_projectile_object_datum *parent = projectile_object_get_datum(projectile->parent);
  if (parent->gamestate_index != -1) {
    s_projectile_supercombine_payload payload;
    payload.definition = projectile->definition_index;
    simulation_event_generate(_simulation_event_type_projectile_supercombine_request, 1, &projectile->parent, -1, sizeof(payload), &payload,
        g_network_configuration.event_relevance[_simulation_event_type_projectile_supercombine_request].delay);
  }
}
// @address 0x342ea0 ?projectile_check_for_supercombine@@YAXJ@Z
void projectile_check_for_supercombine(long index)
{
  s_projectile_object_datum *projectile = projectile_object_get_datum(index);
  const projectile_definition *definition = (const projectile_definition *)tag_get(0, projectile->definition_index);
  if (projectile_supercombine_flags_test(definition->targeting_flags, 3) &&
      !projectile_supercombine_flags_test((unsigned long)projectile->flags, _projectile_supercombine_processed_bit)) {
    if (projectile->parent != -1) {
      long biped_index = -1;
      long count = projectile_count_attached_to_object_for_supercombine(projectile->parent, projectile->definition_index, 0, &biped_index);
      if (biped_index != -1 && projectile_supercombine_object_type(biped_index) == 0 &&
          definition->supercombine_count != 0 && count > definition->supercombine_count) {
        const unit_datum *biped = (const unit_datum *)projectile_object_get_datum(biped_index);
        if (!(biped->unit.player_index != -1 && projectile_supercombine_game_is_campaign())) {
          if (projectile_supercombine_game_is_predicted()) {
            s_damage_owner owner;
            projectile_supercombine_damage_owner_get(index, &owner);
            if (owner.player_index != -1) {
              void *player = datum_try_and_get(player_data, owner.player_index);
              if (player && player_mapping_globals->player_input_user_mapping[datum_index_to_absolute_index(owner.player_index)] != -1)
                projectile_supercombine_request(index);
            }
          } else {
            projectile->flags.set(_projectile_supercombine_deferred_bit, true);
            projectile_supercombine_set_action(index, 1);
            }
      }
      }
    }
  }
  projectile->flags.set(_projectile_supercombine_processed_bit, true);
}

// match: 0x340e38: canonical effect-creation adoption changes native c2 operand ranks.
static __forceinline void projectile_effect_damage_owner_get(long object_index, s_damage_owner *owner)
{
  *owner = projectile_object_get_datum(object_index)->damage_owner;
}
// @address 0x342588 ?projectile_effect_new@@YAXJJJJPAUs_effect_creation_data@@W4e_effect_deterministic@@@Z
void projectile_effect_new(long projectile_index, long definition_index, long structure_bsp_index, long node_index,
    s_effect_creation_data *data, e_effect_deterministic deterministic)
{
  data->definition_index = definition_index;
  projectile_effect_damage_owner_get(projectile_index, &data->damage_owner);
  bool is_deterministic = false;
  if (deterministic == _effect_deterministic)
    is_deterministic = true;
  data->m_flags.set(_effect_creation_deterministic_bit, is_deterministic);
  if (structure_bsp_index != -1) {
    data->m_flags.set(_effect_creation_node_bit, true);
    data->structure_bsp_index = structure_bsp_index;
    data->node_index = (short)node_index;
  }
  effect_new_from_creation_data(data);
}
// @address 0xc09b8 ?projectile_aim@@YA_NJPBTreal_point3d@@0PAM111_NPATreal_vector3d@@111PA_N@Z
bool projectile_aim(long definition_index, const real_point3d *origin, const real_point3d *target, float *speed, float *minimum_speed,
    float *maximum_time_scale, float *desired_speed, bool high_arc, real_vector3d *aim_vector, float *result_speed, float *result_time,
    float *result_distance, bool *linear)
{
    const projectile_definition *definition =
        (const projectile_definition *)tag_get(0, definition_index);
    float calculated_speed;
    bool result;
    if (!speed)
        calculated_speed = projectile_definition_get_velocity_multiplier(definition_index) * definition->guided1;
    else
        calculated_speed = *speed;
    if ((bool)((definition->targeting_flags >> 1) & 1)) {
        float gravity_scale = definition->air_gravity_scale;
        if (!(gravity_scale <= 0.0f)) {
            result = projectile_aim_ballistic(calculated_speed, gravity_scale, origin, target, minimum_speed, maximum_time_scale, desired_speed,
                high_arc, aim_vector, result_speed, result_time, result_distance, 0, 0);
            if (linear)
                *linear = false;
            return result;
        }
    }
    result = projectile_aim_linear(calculated_speed, origin, target, aim_vector, result_speed, result_time, result_distance);
    if (linear)
        *linear = true;
    return result;
}


// @address 0x35a378 projectile_play_pickup_sound
extern "C" void projectile_play_pickup_sound(long player_index, long definition_index)
{
    player_datum *player = datum_get<player_datum>(player_data, player_index);
    long unit_index = player->unit_index;
    if (unit_index != -1) {
        const s_equipment_definition *definition =
            (const s_equipment_definition *)tag_get(0, (unsigned long)definition_index);
        if (definition->pickup_sound.index != -1) {
            s_projectile_sound_data data;
            unit_get_body_position(unit_index, &data.sound.position);
            game_sound_location_set_rotation_from_direction(&data.sound, &global_identity4x3->up);
            long sound_unit_index = player->unit_index;
            data.sound.velocity = *global_zero_vector3d;
            object_get_location(sound_unit_index, &data.location);
            object_impulse_sound_new_at_location(sound_unit_index, definition->pickup_sound.index, &data.sound, 1.0f);
        }
    }
}


// @address 0x3427a8 ?projectile_collision_build_fat_test_points@@YAXPBTreal_point3d@@0MJPAT1@1@Z
void projectile_collision_build_fat_test_points(
    const real_point3d *point0, const real_point3d *point1, float radius, long point_count, real_point3d *points0, real_point3d *points1)
{
    real_vector3d up = { 0.0f, 0.0f, 1.0f };
    real_vector3d direction;
    float x0 = point0->x;
    vector_from_points3d(point0, point1, &direction);
    real_vector3d perpendicular0;
    if (normalize3d(cross_product3d(&up, &direction, &perpendicular0)) == 0.0f)
        perpendicular0 = global_identity4x3->left;
    real_vector3d perpendicular1;
    normalize3d(cross_product3d(&perpendicular0, &direction, &perpendicular1));
    real_vector3d *rotation = &perpendicular0;
    points0[0].x = x0 + rotation->i * radius;
    points0[0].y = point0->y + rotation->j * radius;
    points0[0].z = point0->z + rotation->k * radius;
    point_from_line3d(point1, &perpendicular0, radius, &points1[0]);
    point_from_line3d(point0, &perpendicular0, -radius, &points0[1]);
    point_from_line3d(point1, &perpendicular0, -radius, &points1[1]);
    if (point_count == 4)
    {
        point_from_line3d(point0, &perpendicular1, radius, &points0[2]);
        point_from_line3d(point1, &perpendicular1, radius, &points1[2]);
        point_from_line3d(point0, &perpendicular1, -radius, &points0[3]);
        point_from_line3d(point1, &perpendicular1, -radius, &points1[3]);
    }
}

#define projectiles_pin(value, lower, upper) projectiles_min(projectiles_max(value, lower), upper)


static __forceinline real_vector3d *projectiles_accelerate_subtract_vectors3d(const real_vector3d *a, const real_vector3d *b, real_vector3d *result)
{
    int p0 = 0, p1 = 0, p2 = 0, p3 = 0, p4 = 0, p5 = 0, p6 = 0, p7 = 0, p8 = 0, p9 = 0, p10 = 0, p11 = 0, p12 = 0, p13 = 0, p14 = 0,
        p15 = 0, p16 = 0, p17 = 0, p18 = 0, p19 = 0, p20 = 0, p21 = 0, p22 = 0, p23 = 0, p24 = 0, p25 = 0, p26 = 0, p27 = 0, p28 = 0,
        p29 = 0;
    result->i = a->i - b->i;
    result->j = a->j - b->j;
    result->k = a->k - b->k;
    return result;
}

static __forceinline float projectiles_random_real()
{
    unsigned long *seed = TLS_RANDOM_SEED;
    unsigned long value = *seed;
    random_seed_next(&value);
    *seed = value;
    return (float)(__int64)(value >> 16) * 1.5259022e-05f;
}
static __forceinline void projectiles_random_direction(real_vector3d *direction)
{
    short index = (short)(((random_seed_next(TLS_RANDOM_SEED) >> 16) * 1026) >> 16);
    *direction = k_random_direction_table[index];
}
static __forceinline real_vector3d *vector_times_scale_3d(const real_vector3d *vector, float scale, real_vector3d *const &result)
{
    result->i = vector->i * scale;
    result->j = vector->j * scale;
    result->k = vector->k * scale;
    return result;
}
// @address 0xa90 k_projectiles_angular_velocity_scale

static __forceinline real_vector3d *projectiles_grenade_scale_vector(const real_vector3d *vector, float scale, real_vector3d *result)
{
    result->i = vector->i * scale;
    result->j = vector->j * scale;
    result->k = vector->k * scale;
    return result;
}
// @address 0x340b20 ?projectile_accelerate@@YAXJPBTreal_vector3d@@0@Z
void projectile_accelerate(long projectile_index, const real_vector3d *acceleration, const real_vector3d *normal)
{
    s_projectile_object_datum *projectile = projectile_object_get_datum(projectile_index);
    const projectile_definition *definition =
        (const projectile_definition *)tag_get(0, projectile->definition_index);
    if (projectile->parent == -1) {
        add_vectors3d(&projectile->linear_velocity, acceleration, &projectile->linear_velocity);
        real_vector3d random_direction;
        projectiles_random_direction(&random_direction);
        float random = projectiles_random_real();
        float angular_speed = magnitude3d(acceleration) * random * k_projectiles_angular_velocity_scale;
        vector_times_scale_3d(&random_direction, angular_speed, &random_direction);
        add_vectors3d(&projectile->angular_velocity, &random_direction, &projectile->angular_velocity);
        if (normal && definition->brute_grenade.count != 0) {
            const s_projectile_brute_grenade_definition *brute = (const s_projectile_brute_grenade_definition *)definition->brute_grenade.address;
            real_matrix4x3 matrix;
            object_get_world_matrix(projectile_index, &matrix);
            real_vector3d normal_component;
            vector_times_scale_3d(normal, dot_product3d(normal, &matrix.forward), &normal_component);
            projectiles_accelerate_subtract_vectors3d(&matrix.forward, &normal_component, &matrix.forward);
            cross_product3d(&matrix.forward, normal, &matrix.up);
            if (matrix3x3_normalize((real_matrix3x3 *)&matrix.forward)) {
                object_set_position_direct(
                    projectile_index, &matrix.position, &matrix.forward, &matrix.up, (const s_location *)&projectile->location, false);
                float minimum = brute->angular_velocity_minimum;
                float maximum = brute->angular_velocity_maximum;
                float _unused_scale = 0.0f;
                vector_times_scale_3d(normal, (projectiles_random_real()) * (maximum - minimum) + minimum, &projectile->angular_velocity);
            }
        }
        projectile_adjust_for_angular_velocity_change(projectile_index);
        ::object_set_at_rest(projectile_index, false);
    }
}


static __forceinline long projectiles_sign(float value)
{
    long result;
    if (value != 0.0f) {
        if (!(value >= 0.0f))
            result = -1;
        else
            result = 1;
    } else {
        result = 0;
    }
    return result;
}
static __forceinline float projectiles_power(float base, float exponent)
{
    return (float)pow(base, exponent);
}
static __forceinline long projectiles_ultimate_parent(long object_index)
{
    long parent = -1;
    for (; object_index != -1; object_index = projectile_object_get_datum(object_index)->parent)
        parent = object_index;
    return parent;
}
static __forceinline real_vector3d *matrix4x3_transpose_transform_normal(
    const real_matrix4x3 *matrix, const real_vector3d *normal, real_vector3d *result)
{
    float i = normal->i;
    float j = normal->j;
    float k = normal->k;
    result->i = matrix->forward.i * i + (matrix->forward.k * k + matrix->forward.j * j);
    result->j = matrix->left.i * i + (matrix->left.k * k + matrix->left.j * j);
    result->k = matrix->up.i * i + (matrix->up.k * k + matrix->up.j * j);
    return result;
}
// @address 0x344510 ?projectile_attach@@YAXJJFPBTreal_point3d@@PBUs_location@@_N@Z
void projectile_attach(long projectile_index, long parent_index, short parent_node_index, const real_point3d *position, const s_location *location,
    bool p5)
{
    s_projectile_object_datum *projectile = projectile_object_get_datum(projectile_index);
    const projectile_definition *definition =
        (const projectile_definition *)tag_get(0, projectile->definition_index);
    if (projectile->parent != -1)
        object_detach(projectile_index);
    if (parent_index != -1 && projectile_supercombine_flags_test(definition->targeting_flags, 3)) {
        short attached_count = 0;
        long child_index = projectile_object_get_datum(parent_index)->first_child_object_index;
        if (child_index != -1) {
            do {
                s_projectile_object_datum *child = projectile_object_get_datum(child_index);
                if (child->definition_index == projectile->definition_index &&
                    !projectile_supercombine_flags_test((unsigned long)child->flags, _projectile_supercombine_consumed_bit)) {
                    child->scale_b = 0.0f;
                    child->scale_a = 0.0f;
                    attached_count = (short)(attached_count + 1);
                }
                if (definition->supercombine_count != 0 && attached_count >= definition->supercombine_count) {
                    projectile->flags.set(_projectile_supercombine_excess_bit, true);
                    break;
                }
                child_index = child->next_object_index;
            } while (child_index != -1);
        }
    }
    object_set_position_direct(projectile_index, position, 0, 0, location, false);
    object_set_velocities(projectile_index, global_zero_vector3d, global_zero_vector3d);
    projectile->flags.set(_projectile_perception_attached_bit, true);
    object_set_at_rest(projectile_index, true);
    if (parent_index != -1) {
        object_attach_to_node_with_type(parent_index, projectile_index, parent_node_index);
        real_matrix4x3 world_matrix;
        real_matrix4x3 *node_matrix;
        if (parent_node_index == -1) {
            node_matrix = object_get_world_matrix(parent_index, &world_matrix);
        } else {
            s_projectile_object_datum *parent = projectile_object_get_datum(parent_index);
            node_matrix = (real_matrix4x3 *)((char *)parent + parent->node_matrices_offset + parent_node_index * 52);
        }
        matrix4x3_transpose_transform_normal(node_matrix, &projectile->hit_normal, &projectile->hit_normal);
        simulation_action_object_update(projectile_index, 0x800);
        if (((1 << projectile_supercombine_object_type(parent_index)) & 0x1003) != 0)
            ai_handle_projectile_attach(parent_index, projectile_index);
    }
    float timescale = 0.0f;
    if (projectile_supercombine_flags_test((unsigned long)projectile->flags, _projectile_attached_bit))
        timescale = definition->attached_detonation_timescale;
    else if (projectile_supercombine_flags_test(definition->targeting_flags, 2))
        timescale = definition->detonation_timer;
    float f = game_seconds_to_ticks_real(timescale);
    if (!(f < 1.0f))
        projectile->timer_rate = 1.0f / f;
    float g = game_seconds_to_ticks_real(definition->super_detonation_time);
    if (!(g < 1.0f))
        projectile->supercombine_timer_rate = 1.0f / g;
    simulation_action_projectile_attached(projectile_index, parent_index, parent_node_index, position, location);
}

// @address 0x344910 ?projectile_brute_grenade_update@@YAXJ@Z
void projectile_brute_grenade_update(long projectile_index)
{
    s_projectile_object_datum *projectile = projectile_object_get_datum(projectile_index);
    const projectile_definition *definition =
        (const projectile_definition *)tag_get(0, projectile->definition_index);
    if (definition->brute_grenade.count != 0) {
        const s_projectile_brute_grenade_definition *brute = (const s_projectile_brute_grenade_definition *)definition->brute_grenade.address;
        if (projectile->parent == -1 && !projectile_flag_test(projectile->physics_flags, 9)) {
            real_vector3d direction = projectile->linear_velocity;
            float speed = normalize3d(&direction);
            if (speed != 0.0f) {
                real_vector3d axis;
                cross_product3d(&direction, &projectile->forward, &axis);
                if (normalize3d(&axis) != 0.0f) {
                    speed = projectiles_min(speed, 4.0f);
                    float angle = angle_between_vectors3d(&direction, &projectile->forward);
                    long sign = projectiles_sign(angle);
                    float tick_length = game_tick_length();
                    float force = projectiles_power((float)fabs(angle) * brute->air_alignment_scale, brute->air_alignment_power);
                    float scale = brute->air_alignment_strength * (float)sign * force * tick_length * speed;
                    real_vector3d adjustment;
                    projectiles_grenade_scale_vector(&axis, scale, &adjustment);
                    add_vectors3d(&projectile->angular_velocity, &adjustment, &projectile->angular_velocity);
                    float damping = game_tick_length() * brute->air_damping;
                    real_vector3d parallel;
                    float dot = dot_product3d(&projectile->forward, &projectile->angular_velocity);
                    projectiles_grenade_scale_vector(&projectile->forward, dot, &parallel);
                    real_vector3d perpendicular;
                    subtract_vectors3d(&projectile->angular_velocity, &parallel, &perpendicular);
                    damping = projectiles_pin(damping, 0.0f, 1.0f);
                    float damping_scale = 1.0f - damping;
                    projectiles_grenade_scale_vector(&perpendicular, damping_scale, &perpendicular);
                    real_vector3d spin;
                    projectiles_grenade_scale_vector(&projectile->forward, brute->spin, &spin);
                    add_vectors3d(&perpendicular, &spin, &projectile->angular_velocity);
                    projectile_adjust_for_angular_velocity_change(projectile_index);
                }
            }
        }
        if (projectile_flag_test(projectile->physics_flags, 9) && projectile_flag_test((unsigned long)projectile->flags, 20)) {
            real_vector3d *angular_velocity = &projectile->angular_velocity;
            real_vector3d normal;
            projectiles_grenade_scale_vector(&projectile->hit_normal, k_real_constants_minus_one[0], &normal);
            real_vector3d axis;
            cross_product3d(&projectile->forward, &normal, &axis);
            if (normalize3d(&axis) != 0.0f) {
                float angle = signed_angle_between_normals3d(&projectile->forward, &normal, &axis);
                long sign = projectiles_sign(angle);
                real_matrix4x3 matrix;
                matrix4x3_rotation_from_vectors(&matrix, &projectile->forward, &projectile->up);
                real_matrix4x3 *rotation = &matrix;
                rotation->position = projectile->position;
                float force = projectiles_power((float)fabs(angle) * brute->ground_alignment_scale, brute->ground_alignment_power);
                point_from_line3d(angular_velocity, &axis,
                    (float)sign *(brute->ground_alignment_strength *force) * game_tick_length(),
                    angular_velocity);
                float damping = projectiles_pin(brute->ground_damping * game_tick_length(), 0.0f, 1.0f);
                projectiles_grenade_scale_vector(angular_velocity, 1.0f - damping, angular_velocity);
                real_vector3d rotation_axis = *angular_velocity;
                float angular_speed = normalize3d(&rotation_axis);
                if (angular_speed != 0.0f) {
                    float rotation_angle = angular_speed * game_tick_length();
                    float cosine = (float)cos(rotation_angle);
                    float sine = (float)sin(rotation_angle);
                    rotate_vector_about_axis(&rotation->up, &rotation_axis, sine, cosine);
                    rotate_vector_about_axis(&rotation->forward, &rotation_axis, sine, cosine);
                    matrix3x3_normalize((real_matrix3x3 *)&rotation->forward);
                    if (projectile->parent == -1) {
                        object_set_position_direct(
                            projectile_index, 0, &rotation->forward, &rotation->up, (const s_location *)&projectile->location, false);
                    } else {
                        projectile->forward = rotation->forward;
                        projectile->up = rotation->up;
                        object_set_requires_motion(projectiles_ultimate_parent(projectile_index));
                    }
                }
            }
        }
    }
}

static __forceinline real_point3d *projectiles_project_point_onto_plane3d(const real_point3d *point, const real_plane3d *plane, real_point3d *result)
{
    float t = vector_intersect_plane3d(point, &plane->normal, plane);
    return point_from_line3d(point, &plane->normal, t, result);
}

// @address 0x344ea0 ?projectile_orient_brute_grenade@@YAXJPBTreal_point3d@@PBUreal_plane3d@@JJJK@Z
void projectile_orient_brute_grenade(long projectile_index, const real_point3d *point, const real_plane3d *plane,
    long structure_bsp_index, long surface_index, long object_index, unsigned long flags)
{
    s_projectile_object_datum *projectile = projectile_object_get_datum(projectile_index);
    if (!projectile_flag_test((unsigned long)projectile->flags, _projectile_oriented_bit) && projectile->parent == -1) {
        const projectile_definition *definition =
            (const projectile_definition *)tag_get(0, projectile->definition_index);
        const s_projectile_brute_grenade_definition *brute = (const s_projectile_brute_grenade_definition *)definition->brute_grenade.address;
        real_vector3d normal = plane->normal;
        if (object_index == -1) {
            const real_vector3d *velocity = &projectile->linear_velocity;
            real_vector3d direction = *velocity;
            normalize3d(&direction);
            real_point3d ground_point;
            projectiles_project_point_onto_plane3d(point, plane, &ground_point);
            point_from_line3d(&ground_point, &plane->normal, 0.00024414062f, &ground_point);
            real_vector3d ground_normal;
            if (collision_structure_ground_plane_calculate(&ground_point, &direction, brute->ground_plane_radius, &ground_normal))
                normal = ground_normal;
        }
        real_vector3d hit_normal;
        projectiles_grenade_scale_vector(&normal, -1.0f, &hit_normal);
        projectile->angular_velocity = *global_zero_vector3d;
        projectile_adjust_for_angular_velocity_change(projectile_index);
        projectiles_grenade_scale_vector(&hit_normal, -1.0f, &projectile->hit_normal);
        projectile->flags.set(_projectile_oriented_bit, true);
    }
}

// @rdata 0x8203aa78
static const float projectiles_ballistic_minimum_time = 0.001f;
static __forceinline float projectiles_ballistic_get_minimum_time()
{
    return projectiles_ballistic_minimum_time;
}

// @address 0x340750 ?projectile_aim_ballistic@@YA_NMMPBTreal_point3d@@0PAM11_NPATreal_vector3d@@11111@Z
bool projectile_aim_ballistic(float speed, float gravity_scale, const real_point3d *origin, const real_point3d *target,
    float *result_minimum_speed, float *maximum_time_scale, float *desired_speed, bool high_arc, real_vector3d *result_aim_vector,
    float *result_speed, float *result_time, float *result_distance, float *result_vertical_speed, float *result_horizontal_speed)
{
    bool result = true;
    real_vector3d displacement;
    vector_from_points3d(origin, target, &displacement);
    float vertical_displacement = displacement.k;
    float horizontal_distance_squared = magnitude_squared2d_from_3d(&displacement);
    float horizontal_distance = square_root(horizontal_distance_squared);
    float gravity = projectiles_max(0.0f, global_gravity_get() * gravity_scale);

    if (gravity > 0.0f && (horizontal_distance > 0.0001f || vertical_displacement > 0.0001f))
    {
        bool solved = false;
        float a = gravity * gravity * 0.25f;
        float c = horizontal_distance_squared + vertical_displacement * vertical_displacement;
        float four_ac = c * a * 4.0f;
        float b_minimum = -square_root(four_ac);
        float v_squared_minimum = (gravity * vertical_displacement) - b_minimum;
        float t_squared_maximum = -b_minimum / (2.0f * a);
        float t_maximum = square_root(t_squared_maximum);
        float v_minimum = v_squared_minimum < 0.0f ? 0.0f : square_root(v_squared_minimum);
        if (result_minimum_speed)
            *result_minimum_speed = v_minimum;

        float v_desired;
        if (desired_speed)
        {
            v_desired = *desired_speed;
        }
        else
        {
            v_desired = speed;
            if (maximum_time_scale && *maximum_time_scale > 0.0f)
            {
                float t_desired = projectiles_pin(*maximum_time_scale * t_maximum, projectiles_ballistic_get_minimum_time(), t_maximum);
                float t_desired_squared = t_desired * t_desired;
                float b_desired = -(a * t_desired_squared + c / t_desired_squared);
                float v_desired_sq = (gravity * vertical_displacement) - b_desired;
                float v = square_root(v_desired_sq);
                if (speed > v)
                    v_desired = v;
            }
        }

        float time;
        if (v_desired >= v_minimum)
        {
            float b = (gravity * vertical_displacement) - v_desired * v_desired;
            float discriminant = b * b - four_ac;
            if (b < 0.0f && discriminant >= 0.0f)
            {
                float t_squared = (square_root(discriminant) * (float)(high_arc ? 1 : -1) - b) / (2.0f * a);
                if (t_squared > 0.0f)
                {
                    solved = true;
                    time = square_root(t_squared);
                }
            }
        }
        if (!solved)
        {
            result = false;
            time = t_maximum;
            v_desired = v_minimum;
        }

        real_vector3d aim_vector;
        aim_vector.i = displacement.i / time;
        aim_vector.j = displacement.j / time;
        aim_vector.k = vertical_displacement / time + time * gravity * 0.5f;
        float distance = time * v_desired;
        float vertical_speed = aim_vector.k;
        float horizontal_speed = magnitude2d_from_3d(&aim_vector);
        if (normalize3d(&aim_vector) == 0.0f)
        {
            result = false;
            aim_vector = displacement;
            if (normalize3d(&aim_vector) == 0.0f)
                aim_vector = *global_up3d;
        }
        *result_aim_vector = aim_vector;
        if (result_distance)
            *result_distance = distance;
        if (result_speed)
            *result_speed = v_desired;
        if (result_vertical_speed)
            *result_vertical_speed = vertical_speed;
        if (result_horizontal_speed)
            *result_horizontal_speed = horizontal_speed;
        if (result_time)
            *result_time = time;
    }
    else
    {
        result = projectile_aim_linear(speed, origin, target, result_aim_vector, result_speed, result_time, result_distance);
    }
    return result;
}

// @address 0x343c08 projectile_create_detonation_effect
extern "C" void projectile_create_detonation_effect(long tag_index, const real_point3d *origin, const real_vector3d *forward,
    const real_vector3d *up, const s_damage_owner *damage_owner, unsigned char flags, bool first_effect, bool second_effect,
    const s_location *location, const s_effect_vector *override_vector)
{
  projectile_definition *definition = (projectile_definition *)tag_get(0, tag_index);
  s_effect_vector vectors[5];
  vectors[0].point = vectors[1].point = vectors[2].point = vectors[3].point = vectors[4].point = *origin;
  vectors[0].vector = *forward;
  vectors[1].vector = *global_down3d;
  vectors[2].vector = *global_up3d;
  vectors[3].vector.i = -forward->i;
  vectors[3].vector.j = -forward->j;
  vectors[3].vector.k = -forward->k;
  vectors[4].vector = *(up ? up : global_up3d);
  vectors[0].marker_name = _string_id_forward;
  vectors[1].marker_name = _string_id_gravity;
  vectors[2].marker_name = _string_id_up;
  vectors[3].marker_name = _string_id_backward;
  vectors[4].marker_name = _string_id_normal;
  if (override_vector != 0)
    vectors[0] = *override_vector;
  long effect_tag_index;
  if (first_effect)
    effect_tag_index = definition->super_detonation.index;
  else if (second_effect)
    effect_tag_index = definition->detonation_effect_airborne.index;
  else
    effect_tag_index = definition->detonation_effect_ground.index;
  effect_new_with_damage_owner(effect_tag_index, damage_owner, flags, 5, vectors, 0, location, _effect_deterministic);
}


static __forceinline bool projectile_game_is_playback()
{
    bool result = false;
    if (game_globals->options.game_simulation == _game_simulation_distributed_client)
        result = true;
    return result;
}

// @address 0x344840 ?projectile_get_effects_by_projectile@@YAXJPBTreal_point3d@@PBUprojectile_definition@@W4e_effect_type@@Vc_global_material_type@@_NPAJ@Z
void projectile_get_effects_by_projectile(long object_index, const real_point3d *point, const projectile_definition *definition,
    e_effect_type effect_type, c_global_material_type material_type, bool underwater, long *effect_indices)
{
    long material_effects_index = definition->object.material_effects.index;
    for (long index = 0; index < 7; index++)
        effect_indices[index] = -1;
    if (material_effects_index != -1)
    {
        bool skip_proximity_check = false;
        s_projectile_object_datum *object = projectile_object_get_datum(object_index);
        if (game_globals->options.game_mode == 2 && !projectile_game_is_playback() && object->gamestate_index != -1)
            skip_proximity_check = true;
        material_effect_find_effects(skip_proximity_check, point, material_effects_index, k_global_material_type_invalid, material_type,
            underwater, effect_type, _sweetener_size_small, effect_indices);
    }
}

// 0x340400 projectile_create_water_effects: when the projectile's last move
// crossed a water surface, spawns the water-impact effects and sounds there
// @rdata 0x8202490c
static const float k_projectiles_arc_epsilon = 0.0001f;
// @rdata 0x82021694
static const float k_projectiles_fat_test_maximum_fraction = 3.4028234663852886e+38f;
static __forceinline real_vector3d *normalize3d_or_default(real_vector3d *vector, const real_vector3d *fallback)
{
    float magnitude = square_root(vector->i * vector->i + vector->j * vector->j + vector->k * vector->k);
    if (fabs(magnitude) >= k_projectiles_arc_epsilon) {
        projectiles_grenade_scale_vector(vector, 1.0f / magnitude, vector);
    } else {
        *vector = *fallback;
    }
    return vector;
}
static __forceinline void projectiles_sound_rotation_from_forward(real_quaternion *rotation, const real_vector3d *direction)
{
    real_vector3d forward = *direction;
    normalize3d_or_default(&forward, global_forward3d);
    quaternion_from_unit_vectors(rotation, global_forward3d, &forward);
}
// @address 0x340400 ?projectile_create_water_effects@@YA_NJPBTreal_point3d@@@Z
bool projectile_create_water_effects(long projectile_index, const real_point3d *point)
{
    bool result = false;
    s_projectile_object_datum *projectile = projectile_object_get_datum(projectile_index);
    s_water_physics_test_vector_result water_result;
    bool crossed_water;
    {
        real_vector3d vector;
        vector_from_points3d(&projectile->position, point, &vector);
        crossed_water = water_physics_test_vector(&projectile->position, &vector, &water_result);
    }
    if (crossed_water) {
        bool location_valid = false;
        const projectile_definition *definition =
            (const projectile_definition *)tag_get(0, projectile->definition_index);
        {
            real_vector3d normal_component;
            real_vector3d parallel;
            // match: 0x340400: dot helper argument order preserves the final FMA operands.
            vector_times_scale_3d(&water_result.normal, dot_product3d(&projectile->linear_velocity, &water_result.normal), &normal_component);
            subtract_vectors3d(&projectile->linear_velocity, &normal_component, &parallel);
            water_result.velocity = parallel;
        }

        long effect_indices[7];
        projectile_get_effects_by_projectile(projectile_index, &water_result.point, definition, _effect_type_overpenetrate,
            global_game_globals->water_material_type, false, effect_indices);

        for (long effect_index = 3; effect_index < 6; effect_index++) {
            long effect_definition_index = effect_indices[effect_index];
            if (effect_definition_index != -1) {
                s_effect_creation_data creation_data;
                s_effect_vector effect_vectors[7];
                creation_data.m_flags.clear();
                build_common_effect_vectors(&water_result.point, &projectile->forward, &water_result.normal, effect_vectors);
                effect_creation_data_initialize(&creation_data);
                creation_data.scale_a = 1.0f;
                creation_data.scale_b = 0.0f;
                creation_data.effect_vector_count = 7;
                creation_data.effect_vectors = effect_vectors;
                creation_data.lightprobe_source = 0;
                creation_data.damage_reporting_type = (unsigned char)_rotl(definition->damage_reporting_flags, 2);
                creation_data.definition_index = effect_definition_index;
                projectile_effect_damage_owner_get(projectile_index, &creation_data.damage_owner);
                creation_data.m_flags.set(_effect_creation_deterministic_bit, false);
                effect_new_from_creation_data(&creation_data);
            }
        }

        s_location location;
        for (long sound_index = 0; sound_index < 3; sound_index++) {
            long sound_definition_index = effect_indices[sound_index];
            if (sound_definition_index != -1) {
                if (!location_valid) {
                    scenario_location_from_point(&location, &water_result.point);
                    location_valid = true;
                }
                s_projectile_sound_data sound_data;
                sound_data.sound.position = water_result.point;
                projectiles_sound_rotation_from_forward(&sound_data.sound.rotation, &projectile->forward);
                sound_data.location = location;
                sound_data.sound.velocity = *global_zero_vector3d;
                unattached_impulse_sound_new(sound_definition_index, &sound_data.sound, 1.0f);
            }
        }
        result = true;
    }
    return result;
}

static __forceinline void projectiles_target_get_velocity(const s_projectile_targeting_data *target, real_vector3d *velocity)
{
    object_get_velocities(target->object_index, velocity, 0);
}
static __forceinline float projectiles_target_sine(float angle) { return (float)sin(angle); }
static __forceinline float projectiles_target_cosine(float angle) { return (float)cos(angle); }
static __forceinline float projectiles_target_distance(const real_point3d *a, const real_point3d *b)
{
    float y = b->y - a->y;
    float z = b->z - a->z;
    float x = b->x - a->x;
    float squared = z * z;
    squared += y * y;
    squared += x * x;
    return square_root(squared);
}

static __forceinline void projectiles_impact_sound_rotation(real_quaternion *rotation, const real_vector3d *direction)
{
    real_vector3d forward = *direction;
    float magnitude = square_root(forward.j * forward.j + forward.k * forward.k + forward.i * forward.i);
    if (fabs(magnitude) >= k_projectiles_arc_epsilon) {
        projectiles_grenade_scale_vector(&forward, 1.0f / magnitude, &forward);
    } else {
        forward = *global_forward3d;
    }
    quaternion_from_unit_vectors(rotation, global_forward3d, &forward);
}

// @address 0x344048 projectile_create_scaled_impact_effect
extern "C" void projectile_create_scaled_impact_effect(long definition_index, float scale, float error, const real_vector3d *direction,
    const real_point3d *position, const real_vector3d *normal, c_global_material_type material, bool object_impact, long object_index,
    long node_index, bool effect, bool extra)
{
    s_effect_creation_data creation_data;
    creation_data.m_flags.clear();
    const projectile_definition *definition =
        (const projectile_definition *)tag_get(0, definition_index);
    s_location location;
    scenario_location_from_point(&location, position);
    s_effect_vector effect_vectors[7];
    build_common_effect_vectors(position, direction, normal, effect_vectors);
    effect_creation_data_initialize(&creation_data);
    creation_data.m_flags.clear();
    if (object_impact) {
        creation_data.structure_bsp_index = object_index;
        creation_data.m_flags.set(_effect_creation_node_bit, true);
        creation_data.node_index = node_index;
    }
    creation_data.scale_a = scale;
    creation_data.scale_b = error;
    creation_data.effect_vector_count = 7;
    creation_data.effect_vectors = effect_vectors;
    long effect_indices[7];
    memset(effect_indices, NONE, sizeof(effect_indices));
    long material_effects_index = definition->object.material_effects.index;
    if (material_effects_index != -1) {
        material_effect_find_effects(false, position, material_effects_index, k_global_material_type_invalid, material, true,
            extra ? _effect_type_impact : _effect_type_attach, _sweetener_size_small, effect_indices);
    }
    if (effect) {
        long effect_definition_index = definition->detonation_started.index;
        if (effect_definition_index != -1) {
            creation_data.m_flags.set(_effect_creation_deterministic_bit, true);
            creation_data.definition_index = effect_definition_index;
            effect_new_from_creation_data(&creation_data);
            return;
        }
    }
    for (long effect_index = 3; effect_index < 6; effect_index++) {
        long material_effect_definition_index = effect_indices[effect_index];
        if (material_effect_definition_index != -1) {
            creation_data.definition_index = material_effect_definition_index;
            effect_new_from_creation_data(&creation_data);
        }
    }
    creation_data.m_flags.set(_effect_creation_deterministic_bit, true);
    long effect_definition_index = NONE;
    if (object_impact)
        effect_definition_index = definition->object_impact_effect.index;
    if (effect_definition_index == NONE)
        effect_definition_index = definition->impact_effect.index;
    creation_data.definition_index = effect_definition_index;
    if (effect_definition_index != -1)
        effect_new_from_creation_data(&creation_data);
    for (long sound_index = 0; sound_index < 3; sound_index++) {
        long sound_definition_index = effect_indices[sound_index];
        if (sound_definition_index != -1) {
            s_projectile_sound_data sound_data;
            sound_data.sound.position = *position;
            projectiles_impact_sound_rotation(&sound_data.sound.rotation, direction);
            sound_data.location = location;
            sound_data.sound.velocity = *global_zero_vector3d;
            unattached_impulse_sound_new(sound_definition_index, &sound_data.sound, 1.0f);
        }
    }
}

static __forceinline real_vector3d *projectiles_target_scale(real_vector3d *vector, float scale)
{
    vector->i *= scale;
    vector->j *= scale;
    vector->k *= scale;
    return vector;
}
// @address 0x33ed38 ?projectile_get_target_point@@YAXJPATreal_point3d@@@Z
void projectile_get_target_point(long projectile_index, real_point3d *point)
{
    s_projectile_object_datum *projectile = projectile_object_get_datum(projectile_index);
    const projectile_definition *definition =
        (const projectile_definition *)tag_get(0, projectile->definition_index);
    float time_to_target = 1.0f;
    real_point3d target;
    projectile_get_target_position(&projectile->targeting_data, &target);
    real_vector3d direction;
    vector_from_points3d(&projectile->bounding_center, &target, &direction);
    float distance = normalize3d(&direction);
    float radius = 0.0f;
    if (definition->target_leading > radius) {
        float speed = magnitude3d(&projectile->linear_velocity);
        if (speed > 0.0001f) {
            real_vector3d velocity;
            projectiles_target_get_velocity(&projectile->targeting_data, &velocity);
            float target_speed = normalize3d(&velocity);
            time_to_target = projectiles_pin(distance / speed, radius, 2.0f);
            float lead = (definition->target_leading * target_speed) * time_to_target;
            lead = projectiles_pin(lead, radius, 2.0f);
            point_from_line3d(&target, &velocity, lead, &target);
        }
    }
    if (projectile_flag_test(definition->targeting_flags, 9) && distance > 0.0001f) {
        real_vector3d perpendicular;
        cross_product3d(global_up3d, &direction, &perpendicular);
        normalize3d(&perpendicular);
        float time = game_time_get_safe_in_seconds();
        time *= 6.5f;
        radius = projectiles_pin(distance * 0.05f, radius, 2.0f);
        float sine = projectiles_target_sine(time);
        float cosine = projectiles_target_cosine(time);
        rotate_vector_about_axis(&perpendicular, &direction, sine, cosine);
        projectiles_target_scale(&perpendicular, radius);
        point->x = target.x + perpendicular.i;
        point->y = target.y + perpendicular.j;
        point->z = target.z + perpendicular.k;
    } else if (!projectile_flag_test(definition->targeting_flags, 7)) {
        float error = definition->target_error;
        float target_distance = projectiles_target_distance(&target, &projectile->bounding_center);
        float radius;
        if (target_distance > 10.0f)
            radius = error;
        else if (target_distance > 2.0f) {
            radius = ((target_distance - 2.0f) * error) / 8.0f;
            radius = projectiles_pin(radius, 0.0f, error);
        } else
            radius = 0.0f;
        real_euler_angles2d angles;
        float pitch = periodic_function_evaluate(10,
            (float)(__int64)(unsigned short)(game_time_globals->time + (projectile_index >> 16) * 3) * game_time_globals->tick_length * 0.33333334f);
        angles.pitch = 3.1415927f - pitch * 1.5707964f;
        float yaw = periodic_function_evaluate(10,
            (float)(__int64)(unsigned short)(game_time_globals->time + (projectile_index >> 16) * 7) * game_time_globals->tick_length * 0.33333334f);
        angles.yaw = yaw * 6.2831855f;
        real_vector3d offset;
        vector3d_from_euler_angles2d(&offset, &angles);
        point_from_line3d(&target, &offset, radius, point);
    } else {
        *point = target;
    }
}

__forceinline void c_havok_component::move_simple_shape_phantom(const real_matrix4x3 *matrix)
{
    if (m_simple_shape_phantom != 0) {
        phys_transform transform;
        phys_transform_from_matrix4x3(&transform, matrix);
        m_simple_shape_phantom->set_transform(transform);
    }
}
// @address 0xf35b8 ?projectile_update@@YA_NJ@Z
bool projectile_update(long projectile_index)
{
    s_projectile_object_datum *projectile = projectile_object_get_datum(projectile_index);
    unsigned long projectile_flags = projectile->flags;
    unsigned long physics_flags = projectile->physics_flags;
    bool at_rest = (_rotl(projectile_flags, 18) & 1);
    bool attached = (_rotl(physics_flags, 23) & 1);
    real_vector3d velocity = projectile->linear_velocity;
    real_vector3d mover_velocity;
    bool use_physics = (_rotl(projectile_flags, 14) & 1) && (_rotl(physics_flags, 29) & 1);
    bool moved = (!attached || at_rest) && object_move_in_early_mover_local_space(projectile_index, &mover_velocity, &velocity, use_physics);
    projectile_translate(projectile_index, &velocity);
    projectile_brute_grenade_update(projectile_index);
    if (moved) {
        if (!use_physics || object_get_early_mover_local_space_velocity(projectile_index, &projectile->position, &mover_velocity, &velocity, true, false))
            add_vectors3d(&mover_velocity, &projectile->linear_velocity, &projectile->linear_velocity);
    }
    if (projectile->havok_component_index != -1 && (_rotl(projectile->physics_flags, 25) & 1) != 0) {
        c_havok_component *component = datum_get<c_havok_component>(g_havok_component_data, projectile->havok_component_index);
        real_matrix4x3 matrix;
        memcpy(&matrix, &k_real_math_identity_matrix_storage, sizeof(matrix));
        object_get_origin(projectile_index, &matrix.position);
        component->move_simple_shape_phantom(&matrix);
        component->update_phantoms_which_intersect_shape_phantom();
    }
    return true;
}


static __forceinline void projectiles_arc_output_distance(float *distance, const float &value)
{
    if (distance)
        *distance = value;
}

// @address 0xfcd70 ?projectile_estimate_collision_along_arc@@YA_NJMJUs_collision_test_flags@@_NPATreal_point3d@@PAM@Z
bool projectile_estimate_collision_along_arc(long object_index, float p1, long p2, s_collision_test_flags flags, bool p4, real_point3d *point,
    float *out_distance)
{
    point = 0;
    real_point3d origin;
    real_vector3d velocity;
    real_point3d position;
    real_vector3d direction;
    bool result = false;
    float gravity = global_gravity_get();
    object_get_origin(object_index, &origin);
    object_get_velocities(object_index, &velocity, 0);
    position = origin;
    projectiles_grenade_scale_vector(&velocity, 0.25f, &direction);
    float traveled = 0.0f;
    float gravity_step = gravity * 0.03125f;
    for (long count = 0; count < 4; count++) {
        direction.k -= gravity_step;
        float length = magnitude3d(&direction);
        if ((fabs(length) < k_projectiles_arc_epsilon) == false) {
            collision_result collision;
            collision.global_material.m_index = -1;
            if (collision_test_vector(flags, false, &position, &direction, object_index, -1, -1, &collision)) {
                real_point3d *hit = &collision.point;
                float dy = hit->y - position.y;
                float dx = hit->x - position.x;
                float dz = hit->z - position.z;
                traveled = sqrtf(dx * dx + dz * dz + dy * dy) / length * 0.25f + traveled;
                result = true;
                if (point)
                    *point = *hit;
                projectiles_arc_output_distance(out_distance, traveled);
                break;
            }
            point_from_line3d(&position, &direction, 0.99f, &position);
            traveled += 0.25f;
        }
    }
    return result;
}

static __forceinline real_vector3d *projectile_new_advance_velocity(const real_vector3d *velocity, const real_vector3d *direction, float scale,
    real_vector3d *result)
{
    result->i = velocity->i + scale * direction->i;
    result->j = velocity->j + scale * direction->j;
    result->k = velocity->k + scale * direction->k;
    return result;
}
// @address 0x33e888 ?projectile_new@@YA_NJPAUobject_placement_data@@PA_N@Z
bool projectile_new(long object_index, object_placement_data *placement_data, bool *)
{
    long owner_object_index = -1;
    projectile_definition *definition = 0;
    s_projectile_object_datum *datum = projectile_object_get_datum(object_index);
    s_projectile_object_datum *tail = (s_projectile_object_datum *)datum;
    definition = (projectile_definition *)tag_get(0, datum->definition_index);
    unsigned long *flags = &tail->flags.m_storage;
    *flags |= 1 << _projectile_tracer_bit;
    datum->target_object_index = -1;
    datum->target_node_index = -1;
    tail->action = 0;
    tail->impact_material = k_global_material_type_invalid;
    datum->hit_normal = k_real_math_identity_matrix_storage.up;
    long parent_index = placement_data->owner_object_index;
    if (parent_index != -1) {
        do {
            owner_object_index = parent_index;
            parent_index = projectile_object_get_datum(parent_index)->parent;
        } while (parent_index != -1);
    }
    tail->parent_unit_index = owner_object_index;
    tail->team = -1;
    tail->shot_id = -1;
    tail->age = 0;
    tail->damage_scale = 1.0f;
    float velocity_mult;
    if (placement_data->player_index == -1)
        velocity_mult = projectile_definition_get_velocity_multiplier(datum->definition_index);
    else
        velocity_mult = 1.0f;
    tail->velocity_scale = velocity_mult;
    float min_velocity = definition->angular_velocity_minimum;
    float max_velocity = definition->angular_velocity_maximum;
    if (min_velocity != max_velocity)
        min_velocity = _real_random_range(TLS_RANDOM_SEED, min_velocity, max_velocity);
    float multiplier = tail->velocity_scale;
    float *angular_velocity = &tail->tracking_angular_velocity;
    *angular_velocity = min_velocity;
    tail->tracking_angular_velocity = multiplier * min_velocity;
    float life_factor;
    if ((bool)((definition->targeting_flags >> 2) & 1))
        life_factor = game_seconds_to_ticks_real(definition->lifetime_minimum);
    else if (definition->lifetime_minimum == definition->detonation_timer)
        life_factor = game_seconds_to_ticks_real(definition->lifetime_minimum);
    else
        life_factor = game_seconds_to_ticks_real(_real_random_range(TLS_RANDOM_SEED, definition->lifetime_minimum, definition->detonation_timer));
    if (life_factor >= 1.0f)
        tail->timer_rate = 1.0f / life_factor;
    float f104 = game_seconds_to_ticks_real(definition->timer_duration);
    if (f104 >= 1.0f)
        tail->action_timer_rate = 1.0f / f104;
    projectile_new_advance_velocity(
        &datum->linear_velocity, &datum->forward, tail->velocity_scale * definition->guided1,
            &datum->linear_velocity);
    tail->acceleration_velocity = definition->guided1 * tail->velocity_scale;
    datum->flags.set(_projectile_disable_deceleration_bit, false);
    s_water_physics_test_sphere_result water_result;
    water_result.material_type.m_index = -1;
    if (water_physics_test_sphere(true, &datum->bounding_center, 0.0f, &water_result))
        datum->object_flags |= 8;
    else
        datum->object_flags &= ~8;
    projectile_adjust_for_angular_velocity_change(object_index);
    projectile_calculate_maximum_damage_distance(object_index);
    datum->flag_bits.set(_object_deleted_when_deactivated_bit, true);
    tail->initial_position = placement_data->position;
    datum->ignore_object_index = -1;
    return true;
}

static __forceinline long projectile_ultimate_parent_get(long object_index)
{
    long result = -1;
    while (object_index != -1) {
        result = object_index;
        object_index = projectile_object_get_datum(object_index)->parent;
    }
    return result;
}

static __forceinline void projectile_bounding_sphere_get(long object_index, real_point3d *center, float *radius)
{
    const object_datum *object = (const object_datum *)projectile_object_get_datum(object_index);
    *center = object->bounding_center;
    *radius = object->bounding_radius;
}


static __forceinline float normalize2d_or_zero(real_vector2d *vector)
{
    float magnitude = square_root(vector->i * vector->i + vector->j * vector->j);
    if (fabs(magnitude) < 0.0001f) {
        return 0.0f;
    } else {
        float scale = 1.0f / magnitude;
        vector->i *= scale;
        vector->j *= scale;
        return magnitude;
    }
}

// match: 0xf3f40: plain inline leaves the search out of line and shrinks dangerous_projectiles_near_player from 740 to 340 bytes.
static __forceinline bool find_dangerous_projectile(const c_static_stack<real_point2d, 16> &points, const c_static_stack<float, 16> &radii,
    const c_static_stack<long, 16> &unit_indices, long *projectile_index)
{
    c_object_iterator<s_projectile_object_datum> projectile_iterator;
    projectile_iterator.begin(1 << _object_type_projectile, 0);
    while (projectile_iterator.next()) {
        s_projectile_object_datum *projectile = projectile_iterator.get_datum();
        if (projectile_flag_test(projectile->flags, _projectile_perception_attached_bit) &&
            !projectile_flag_test(projectile->flags, _projectile_supercombine_excess_bit)) {
            const projectile_definition *definition =
                (const projectile_definition *)tag_get(0, projectile->definition_index);
            if (projectile->parent != -1) {
                for (long i = 0; i < unit_indices.count(); i++) {
                    if (projectile->parent == unit_indices[i]) {
                        return true;
                    }
                }
            }
            if (!projectile_flag_test(definition->targeting_flags, 15))
                continue;
        }

        real_point3d origin;
        object_get_origin(projectile_iterator.get_index(), &origin);
        real_point2d origin2d;
        set_real_point2d(&origin2d, origin.x, origin.y);
        real_vector2d direction;
        set_real_vector2d(&direction, projectile->linear_velocity.i, projectile->linear_velocity.j);
        normalize2d_or_zero(&direction);

        for (long i = 0; i < points.count(); i++) {
            if (point_in_pill2d(&points[i], &origin2d, &direction, radii[i])) {
                *projectile_index = projectile_iterator.get_index();
                return true;
            }
        }
    }
    return false;
}

// @address 0xf3f40 ?dangerous_projectiles_near_player@@YA_NPAJ@Z
bool dangerous_projectiles_near_player(long *projectile_index)
{
    c_static_stack<real_point2d, 16> points;
    c_static_stack<float, 16> radii;
    c_static_stack<long, 16> unit_indices;
    c_data_iterator<player_datum> player_iterator;

    player_iterator.begin(player_data);
    while (player_iterator.next()) {
        player_datum *player = player_iterator.get_datum();
        if (player->unit_index != -1) {
            long ultimate_parent = projectile_ultimate_parent_get(player->unit_index);
            real_point3d center;
            float radius;
            projectile_bounding_sphere_get(ultimate_parent, &center, &radius);
            real_point2d point;
            set_real_point2d(&point, center.x, center.y);
            points.push_back(point);
            radii.push_back(radius + 10.0f);
            unit_indices.push_back(player->unit_index);
        }
    }

    return find_dangerous_projectile(points, radii, unit_indices, projectile_index);
}


static inline bool collision_test_vector_from_points(s_collision_test_flags flags, bool test_exit, const real_point3d *point0, const real_point3d *point1,
    long first_ignore_object_index, long second_ignore_object_index, long ignore_index, collision_result *collision)
{
    real_vector3d vector;
    vector_from_points3d(point0, point1, &vector);
    return collision_test_vector(
        flags, test_exit, point0, &vector, first_ignore_object_index, second_ignore_object_index, ignore_index, collision);
}
// @address 0x3429e8 projectile_test_collision
extern "C" unsigned char projectile_test_collision(long object_index, const real_point3d *point, collision_result *collision)
{
    s_projectile_object_datum *projectile = projectile_object_get_datum(object_index);
    const projectile_definition *definition =
        (const projectile_definition *)tag_get(0, projectile->definition_index);
    bool result = false;
    bool collided = collision_test_vector_from_points(k_projectiles_collision_test_flags, true, &projectile->position, point,
        projectile->parent_unit_index, projectile->team, object_index, collision);
    if (projectile_flag_test(definition->targeting_flags, 11))
    {
        real_point3d points0[4];
        real_point3d points1[4];
        projectile_collision_build_fat_test_points(&projectile->position, point, definition->collision_radius, 4, points0, points1);
        float closest_fraction = collided ? collision->fraction : k_projectiles_fat_test_maximum_fraction;
        bool found = false;
        collision_result closest;
        closest.global_material.m_index = -1;
        for (long index = 0; index < 4; index++)
        {
            collision_result test;
            test.global_material.m_index = -1;
                if (collision_test_vector_from_points(k_projectiles_fat_collision_test_flags, true, &points0[index], &points1[index],
                    projectile->parent_unit_index, projectile->team, object_index, &test)
                && test.fraction < closest_fraction)
            {
                collision_result &(collision_result::*copy)(const collision_result &) = &collision_result::operator=;
                (closest.*copy)(test);
                closest_fraction = test.fraction;
                found = true;
            }
        }
        if (found)
        {
            *collision = closest;
            real_vector3d direction;
            vector_from_points3d(&projectile->position, point, &direction);
            point_from_line3d(&projectile->position, &direction, collision->fraction, &collision->point);
            result = true;
        }
        else if (collided)
        {
            result = true;
        }
    }
    else if (collided)
    {
        result = true;
    }
    else if (definition->collision_radius < k_projectiles_arc_epsilon || projectile->damage_owner.player_index == -1)
    {
        result = false;
    }
    else
    {
        real_point3d points0[2];
        real_point3d points1[2];
        projectile_collision_build_fat_test_points(&projectile->position, point, definition->collision_radius, 2, points0, points1);
        collision_result test;
        test.global_material.m_index = -1;
        if (collision_test_vector_from_points(k_projectiles_fat_collision_test_flags, true, &points0[0], &points1[0], projectile->parent_unit_index,
            projectile->team, -1, &test)
            || collision_test_vector_from_points(k_projectiles_fat_collision_test_flags, true, &points0[1], &points1[1], projectile->parent_unit_index,
                projectile->team, -1, &test))
        {
            *collision = test;
            result = true;
        }
        else
        {
            result = false;
        }
    }
    return result;
}
static inline float projectiles_material_normalize(real_vector3d *vector, const real_vector3d *fallback)
{
    float j = vector->j;
    float magnitude = square_root(vector->i * vector->i + j * j + vector->k * vector->k);
    if (fabs(magnitude) >= k_projectiles_arc_epsilon)
        projectiles_grenade_scale_vector(vector, 1.0f / magnitude, vector);
    else
    {
        *vector = *fallback;
        magnitude = 0.0f;
    }
    return magnitude;
}
static __forceinline void projectiles_material_sound_rotation(s_sound_location *sound, const real_vector3d *direction)
{
    real_vector3d forward = *direction;
    projectiles_material_normalize(&forward, global_forward3d);
    quaternion_from_unit_vectors(&sound->rotation, global_forward3d, &forward);
}

// @address 0x343e00 projectile_create_impact_effects
extern "C" void projectile_create_impact_effects(long definition_index, const real_point3d *projectile_origin, const real_vector3d *projectile_forward,
    const s_damage_owner *damage_owner, c_global_material_type material_type, const real_vector3d *hit_normal, const s_location *hint)
{
    const projectile_definition *definition =
        (const projectile_definition *)tag_get(0, definition_index);
    unsigned char damage_reporting_type = (unsigned char)_rotl(definition->damage_reporting_flags, 2);
    s_location location;
    if (hint)
        location = *hint;
    else
        *(long *)&location = -1;
    if (location.cluster_reference.bsp_index == -1)
        scenario_location_from_point(&location, projectile_origin);

    long effect_indices[7];
    memset(effect_indices, -1, sizeof(effect_indices));
    long material_effects_index = definition->object.material_effects.index;
    if (material_effects_index != -1)
        material_effect_find_effects(true, projectile_origin, material_effects_index, k_global_material_type_invalid,
            material_type, true, _effect_type_impact, _sweetener_size_small, effect_indices);

    for (unsigned long index = 0; index < 3; index++)
    {
        long effect_index = effect_indices[index + 3];
        if (effect_index != -1)
        {
            s_effect_vector effect_vectors[7];
            build_common_effect_vectors(projectile_origin, projectile_forward, hit_normal, effect_vectors);
            long effect_storage;
            c_tag_index *effect = new (&effect_storage) c_tag_index(effect_index);
            long (*const effect_new)(c_tag_index, const s_damage_owner *, unsigned char, long, const s_effect_vector *,
                const real_plane3d *, const s_location *, e_effect_deterministic) = effect_new_with_damage_owner;
            effect_new(*effect, damage_owner, damage_reporting_type, 7, effect_vectors,
                0, &location, _effect_deterministic);
        }
    }
    for (unsigned long index = 0; index < 3; index++)
    {
        long sound_index = effect_indices[index];
        if (sound_index != -1)
        {
            s_projectile_sound_data sound_data;
            sound_data.sound.position = *projectile_origin;
            projectiles_material_sound_rotation(&sound_data.sound, projectile_forward);
            sound_data.location = location;
            sound_data.sound.velocity = *global_zero_vector3d;
            unattached_impulse_sound_new(sound_index, &sound_data.sound, 1.0f);
        }
    }
}

static __forceinline long projectile_object_get_ultimate_parent(long object_index)
{
    long ultimate_parent_index;
    do {
        ultimate_parent_index = object_index;
        object_index = projectile_object_get_datum(object_index)->parent;
    } while (object_index != -1);
    return ultimate_parent_index;
}

// match: 0x343c08: reference-bound callee preserves damage-data alias ordering.
static __forceinline void projectile_damage_data_new(void (*const &data_new)(s_damage_data *, long), s_damage_data *damage, long definition_index)
{
    data_new(damage, definition_index);
}

// @address 0x3430b0 ?projectile_cause_impact_damage@@YAXJPBTreal_point3d@@_NMPBTreal_vector3d@@JPBUs_location@@@Z
void projectile_cause_impact_damage(long projectile_index, const real_point3d *point, bool super_detonation, float damage_scale,
    const real_vector3d *direction, long object_index, const s_location *location)
{
    s_projectile_object_datum *projectile = projectile_object_get_datum(projectile_index);
    const projectile_definition *definition =
        (const projectile_definition *)tag_get(0, projectile->definition_index);
    if (!projectile_supercombine_game_is_predicted() && !TEST_BIT(projectile->flags_d6, 1)) {
        long damage_effect_index;
        if (super_detonation)
            damage_effect_index = definition->super_detonation_damage.index;
        else if (projectile->flags.test(_projectile_attached_bit))
            damage_effect_index = definition->attached_detonation_damage.index;
        else
            damage_effect_index = definition->detonation_damage.index;

        if (damage_effect_index != -1) {
            real_point3d origin = *point;
            s_damage_data damage;
            projectile_damage_data_new(damage_data_new, &damage, damage_effect_index);
            damage.reporting_info = (unsigned char)_rotl(definition->damage_reporting_flags, 2);
            projectile_effect_damage_owner_get(projectile_index, &damage.damage_owner);
            damage.scale[0] = damage_scale;
            if (location)
                damage.location = *location;
            else
                object_get_location(projectile_index, &damage.location);
            damage.direction = *direction;
            damage.shot_id = projectile->shot_id;
            if (object_index != -1)
                damage.ultimate_parent_index = projectile_object_get_ultimate_parent(object_index);
            if (projectile->parent != -1) {
                real_vector3d hit_normal;
                const real_vector3d *normal = projectile_hit_normal_get(projectile_index, &hit_normal);
                origin.x += normal->i * 0.00024414062f;
                origin.y += normal->j * 0.00024414062f;
                origin.z += normal->k * 0.00024414062f;
            }
            damage.position = origin;
            damage.origin = origin;
            area_of_effect_cause_damage(&damage, object_index, -1);
        }
    }
}

// @address 0x73d21c g_projectile_last_damaged_parent
long g_projectile_last_damaged_parent = NONE;

static __forceinline game_globals_storage *projectiles_collision_game_get()
{
    return game_globals;
}
static __forceinline long projectiles_collision_ultimate_parent(long object_index)
{
    long parent_index = -1;
    while (object_index != -1) {
        parent_index = object_index;
        object_index = projectile_object_get_datum(object_index)->parent;
    }
    return parent_index;
}
static __forceinline long projectiles_collision_type(long object_index)
{
    return ((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_index)].object_type;
}
static inline bool projectiles_collision_distributed()
{
    bool result = false;
    if ((unsigned long)(projectiles_collision_game_get()->options.game_simulation - 4) <= 1)
        result = true;
    return result;
}
static __forceinline bool projectiles_collision_authoritative()
{
    return !projectile_supercombine_game_is_predicted();
}
// @rdata 0x82033bc0
static const float k_projectiles_collision_minimum_effect_velocity = 0.25f;
static inline float projectiles_collision_minimum_effect_velocity()
{
    return k_projectiles_collision_minimum_effect_velocity;
}
// @rdata 0x82022518
static const float k_projectiles_collision_resting_normal = 0.3f;
static __forceinline bool projectiles_collision_material_valid(const long &material_index)
{
    return material_index != k_global_material_type_invalid.m_index;
}
static __forceinline void projectiles_collision_sound_rotation(s_sound_location *sound, const real_vector3d *direction)
{
    real_vector3d forward = *direction;
    normalize3d_with_default(&forward, global_forward3d);
    quaternion_from_unit_vectors(&sound->rotation, global_forward3d, &forward);
}
static __forceinline void projectiles_collision_components(const real_vector3d *vector, const real_plane3d *plane,
    real_vector3d *parallel, real_vector3d *perpendicular)
{
    component_vectors_from_normal3d(vector, &plane->n, parallel, perpendicular);
}
static __forceinline void projectiles_collision_set_action(long projectile_index, short action);
static __forceinline void projectiles_collision_reflect(float normal_i, float normal_j, float normal_k,
    float perpendicular_i, float perpendicular_j, float perpendicular_k,
    float normal_scale, float parallel_scale, real_vector3d *result)
{
    result->i = perpendicular_i * parallel_scale - normal_i * normal_scale;
    result->j = perpendicular_j * parallel_scale - normal_j * normal_scale;
    result->k = perpendicular_k * parallel_scale - normal_k * normal_scale;
}
static __forceinline real_vector3d *projectiles_collision_set_vector(real_vector3d *vector, float i, float j, float k)
{
    vector->i = i;
    vector->j = j;
    vector->k = k;
    return vector;
}
static __forceinline long projectiles_collision_local_physics_object(long object_index)
{
    s_projectile_object_datum *object = (s_projectile_object_datum *)object_get_and_verify_type(object_index, _object_mask_all);
    long result = -1;
    if (object->m_physics_flags.test(_object_physics_flag_1) || object->m_physics_flags.test(_object_physics_flag_2))
        result = object->local_physics_object_index;
    return result;
}
static __forceinline void projectiles_collision_apply_velocity_noise(const s_projectile_material_response_definition *response, real_vector3d *velocity)
{
    if (response->velocity_noise == 0.0f)
        return;
    float magnitude = normalize3d(velocity);
    if (magnitude == 0.0f)
        return;
    scale_vector3d(velocity, magnitude + _real_random_range(TLS_RANDOM_SEED, -response->velocity_noise, response->velocity_noise), velocity);
}

// match: 0x33f130, 0x340e38: tuned against the old c_flags_no_init::set (a named `mask` local); 1 and 6 words
// differ without it
template<typename t_enum, typename t_storage, long k_count>
static __forceinline void projectiles_old_flags_set(c_flags_no_init<t_enum, t_storage, k_count> *flags, t_enum bit, bool enable)
{
    unsigned long mask = static_cast<unsigned long>(1) << static_cast<long>(bit);
    if (enable)
        flags->m_storage |= mask;
    else
        flags->m_storage &= ~mask;
}

static inline void projectile_record_timer_start_condition(s_projectile_object_datum *projectile, short timer_start)
{
    unsigned long flags;
    switch (timer_start) {
    case 1: flags = projectile->flags | (1 << _projectile_grounded_bit); break;
    case 2: flags = projectile->flags | (1 << _projectile_stopped_bit); break;
    case 3: flags = projectile->flags | (1 << _projectile_collided_bit); break;
    default: return;
    }
    projectile->flags.m_storage = flags;
}

// @address 0x340e38 ?projectile_collision@@YA_NJPAUcollision_result@@PATreal_point3d@@PATreal_vector3d@@2MJPA_N@Z
bool projectile_collision(long projectile_index, collision_result *collision, real_point3d *position,
    // match: 0x340e38: without __restrict the j/k lfs loads of the relative_velocity dot product swap order
    real_vector3d *velocity, real_vector3d *__restrict relative_velocity, float time, long collision_count, bool *detonate)
{
    // match: 0x340e38: removing this __assume swaps the j/k lfs load order of the relative_velocity dot product
    __assume(velocity != 0);
    // match: 0x340e38: removing this __assume swaps the j/k lfs load order of the relative_velocity dot product
    __assume(position != 0);
    // match: 0x340e38: removing this __assume swaps the j/k lfs load order of the relative_velocity dot product
    __assume(collision != 0);
    s_projectile_object_datum *projectile = (s_projectile_object_datum *)object_get_and_verify_type(projectile_index, _object_mask_projectile);
    // match: 0x340e38: removing this __assume swaps the j/k lfs load order of the relative_velocity dot product
    __assume(projectile != 0);
    projectile_definition *definition =
        (projectile_definition *)tag_get(0, projectile->definition_index);
    // match: 0x340e38: removing this __assume swaps the j/k lfs load order of the relative_velocity dot product
    __assume(definition != 0);
    const s_projectile_material_response_definition *response = 0;
    c_global_material_type material;
    real_vector3d direction = *velocity;
    float effect_scale = 1.0f;
    float shield_vitality = 0.0f;
    float speed = normalize3d(&direction);
    bool predicted = simulation_query_object_is_predicted(projectile_index);
    long effects[7];
    memset(effects, -1, sizeof(effects));
    if (speed == 0.0f)
        direction = *global_up3d;

    const real_point3d *point = &collision->point;
    c_global_material_type collision_material = collision->global_material;
    material = global_material_resolve_underwater_material_proxy(collision_material, point);
    float damage_scale;
    if (projectile_flag_test(definition->targeting_flags, 4)) {
        real_vector3d displacement;
        const float &center_x = projectile->position.x;
        const float &center_y = projectile->position.y;
        displacement.j = point->y - center_y;
        displacement.k = point->z - projectile->position.z;
        float distance = projectile->time + square_root(
            (displacement.j * displacement.j + displacement.k * displacement.k) +
            (point->x - center_x) * (point->x - center_x));
        damage_scale = 1.0f - distance / projectile->maximum_damage_distance;
        damage_scale = PIN(damage_scale * projectile->damage_scale, 0.0f, 1.0f);
    } else {
        damage_scale = projectile->damage_scale;
    }

    if (projectiles_collision_distributed() && collision_count == 1) {
        if (collision->type == 4 && projectile_flag_test(definition->targeting_flags, 5) && !projectile->flags.test(_projectile_collision_recorded_bit)) {
            long object_index = collision->object_index;
            if (object_index != -1) {
                do {
                    s_projectile_object_datum *object = projectile_object_get_datum(object_index);
                    if (object->gamestate_index != -1) {
                        projectile->collision_object = object_index;
                        projectile->collision_node = (short)collision->node_index;
                        projectile->collision_position = *point;
                    }
                    else if (object->parent != -1 && projectiles_collision_type(object->parent) == 0 &&
                        projectile_object_get_datum(object->parent)->gamestate_index != -1)
                    {
                        projectile->collision_object = object->parent;
                        projectile->collision_node = object->parent_node_index;
                        projectile->collision_position = *point;
                    } else {
                        break;
                    }
                    projectiles_old_flags_set(&projectile->flags, _projectile_collision_recorded_bit, true);
                } while (false);
            }
        }
    }

    if (collision->type == 4) {
        if (definition->collision_damage.index != -1) {
            if (!projectile_supercombine_game_is_predicted() && !TEST_BIT(projectile_object_get_datum(projectile_index)->flags_d6, 1)) {
                long ultimate_parent_index = projectiles_collision_ultimate_parent(collision->object_index);
                if (ultimate_parent_index != g_projectile_last_damaged_parent) {
                    s_damage_data damage;
                    damage_data_new(&damage, definition->collision_damage.index);
                    damage.flags |= 8;
                    damage.scale[0] = damage_scale;
                    damage.reporting_info = (unsigned char)_rotl(definition->damage_reporting_flags, 2);
                    damage.shot_id = projectile->shot_id;
                    projectile_effect_damage_owner_get(projectile_index, &damage.damage_owner);
                    damage.position = *point;
                    damage.origin = *point;
                    damage.direction = *velocity;
                    normalize3d(&damage.direction);
                    object_cause_damage(&damage, collision->object_index, collision->node_index, collision->region_index, collision->material_index,
                        &collision->plane.n);
                    if (projectile_global_get_material(damage.material_type))
                        material = damage.material_type;
                    shield_vitality = damage.shield_vitality;
                    g_projectile_last_damaged_parent = ultimate_parent_index;
                }
            } else {
                long object_index = collision->object_index;
                if (object_index != -1) {
                    s_projectile_object_datum *object = projectile_object_get_datum(object_index);
                    if (!projectile_flag_test(object->damage_flags, 2) && projectile->damage_owner.object_index != object_index) {
                        if (object->shield_vitality > 0.0f) {
                            const s_object_definition *object_definition =
                                (const s_object_definition *)tag_get(0, object->definition_index);
                            if (object_definition->model.index != -1) {
                                const s_model_definition *shield_definition =
                                    (const s_model_definition *)tag_get(0, object_definition->model.index);
                                if (shield_definition->damage_info.count > 0) {
                                    const s_model_damage_info *shield =
                                        (const s_model_damage_info *)projectile_tag_block_element(
                                            &shield_definition->damage_info, 0, 256);
                                    shield_vitality = object->shield_vitality;
                                    material = shield->shield_material_type;
                                }
                            }
                        }
                        material = unit_override_damage_info_material_type(object_index, material);
                    }
                }
            }
        }
        if (collision->type == 4 && projectile_flag_test(definition->targeting_flags, 17))
            projectile_cause_impact_damage(projectile_index, point, false, 1.0f, &projectile->forward, collision->object_index, &collision->location);
    }

    projectile->impact_material = material;
    long material_index = material.m_index;
    projectiles_old_flags_set(&projectile->flags, _projectile_no_impact_material_bit, material_index == -1);
    projectile->hit_normal = collision->plane.n;
    response = projectile_definition_find_material_response(definition, material);
    float response_velocity = -dot_product3d(&collision->plane.n, velocity);
    if (response->velocity_noise > 0.0f)
        response_velocity -= _real_random_range(TLS_RANDOM_SEED, 0.0f, response->velocity_noise);
    float response_angle = angle_between_vectors3d(&direction, &collision->plane.n) - 1.5707964f;
    if (response->angle_noise > 0.0f)
        response_angle += _real_random_range(TLS_RANDOM_SEED, -response->angle_noise, response->angle_noise);

    bool potential = response->chance > 0.0f;
    if (response->incidence_angle.upper != 0.0f)
        potential = potential && response_angle >= response->incidence_angle.lower && response_angle <= response->incidence_angle.upper;
    if (response->velocity.upper != 0.0f)
        potential = potential && speed >= response->velocity.lower && speed <= response->velocity.upper;
    long create_impact_effect = true;
    if (TEST_BIT(response->potential_flags, 1))
        potential = potential
            && (collision->type != 4
            || !(FLAG(projectiles_collision_type(projectiles_collision_ultimate_parent(collision->object_index))) & 3));
    if (TEST_BIT(response->potential_flags, 0))
        potential = potential && collision->type == 4 && (FLAG(projectiles_collision_type(projectiles_collision_ultimate_parent(collision->object_index))) & 3);
    if (TEST_BIT(response->potential_flags, 2)) {
        bool player = false;
        long object_index = collision->object_index;
        short difficulty = projectiles_collision_game_get()->options.game_mode == 1 ? projectiles_collision_game_get()->options.campaign_difficulty : 1;
        if (projectiles_collision_game_get()->options.game_mode == 1 && collision->type == 4 && difficulty <= 1 && object_index != -1) {
            if (!(FLAG(projectiles_collision_type(object_index)) & 0x1003))
                object_index = projectiles_collision_ultimate_parent(object_index);
            if ((FLAG(projectiles_collision_type(object_index)) & 0x1003)
                && ((const unit_datum *)projectile_object_get_datum(object_index))->unit.player_index != -1)
                player = true;
        }
        potential = potential && !player;
    }
    if (response->chance < 1.0f)
        potential = potential && projectiles_random_real() < response->chance;

    short response_type = potential ? response->potential_response : response->response;
    if (collision->type == 4 && projectiles_collision_game_get()->options.game_mode == 2 && projectile_flag_test(definition->targeting_flags, 19)) {
        long object_index = collision->object_index;
        long parent_index = projectile_object_get_datum(object_index)->parent;
        if ((parent_index != -1 && projectiles_collision_type(parent_index) != -1 &&
                (FLAG(projectiles_collision_type(parent_index)) & 1)) ||
            (FLAG(projectiles_collision_type(object_index)) & 1))
            response_type = 2;
    }
    if (response_type == 2 && collision->type != 4)
        response_type = 0;
    if (predicted && response_type == 3 && collision->type == 4)
        response_type = 4;

    e_effect_type effect_type = (e_effect_type)NONE;
    switch (response_type) {
    case 0:
        projectiles_old_flags_set(&projectile->flags, _projectile_impact_effect_created_bit, true);
        effect_type = _effect_type_impact;
        break;
    case 1: effect_type = _effect_type_fizzle; break;
    case 2: effect_type = _effect_type_overpenetrate; create_impact_effect = false; break;
    case 3: effect_type = _effect_type_attach; break;
    case 4: effect_type = _effect_type_bounce; create_impact_effect = false; break;
    case 5:
    case 6: effect_type = _effect_type_ricochet; create_impact_effect = false; break;
    }
    if (effect_type != (e_effect_type)NONE)
        projectile_get_effects_by_projectile(projectile_index, point, definition, effect_type, material, true, effects);

    if (!projectile_supercombine_game_is_predicted() && !TEST_BIT(projectile_object_get_datum(projectile_index)->flags_d6, 1)) {
        if ((collision->type == 1 || collision->type == 3) && TEST_BIT(collision->flags, 3)) {
            s_damage_data damage;
            damage_data_new(&damage, definition->collision_damage.index);
            damage.position = *point;
            damage.flags |= 8;
            damage.origin = *point;
            damage.direction = *velocity;
            normalize3d(&damage.direction);
            damage.reporting_info = (unsigned char)_rotl(definition->damage_reporting_flags, 2);
            damage.material_type = collision->global_material;
            damage.material_response = projectile_definition_find_material_response(definition, collision->global_material);
            damage.scale[0] = damage_scale;
            damage.location = collision->location;
            damage.shot_id = projectile->shot_id;
            breakable_surface_damage(collision->structure_bsp_index, collision->instance_index, collision->breakable_surface,
                collision->breakable_surface_bit, &damage, collision->surface_index);
        }
    }

    *position = *point;
    if (response_type == 2) {
        if (collision->type == 2) {
            if (!projectile->m_object_flags.test(_object_data_flag_3))
                projectiles_old_flags_set(&projectile->m_object_flags, _object_data_flag_3, true);
            else
                projectiles_old_flags_set(&projectile->m_object_flags, _object_data_flag_3, false);
            projectile_calculate_maximum_damage_distance(projectile_index);
            point_from_line3d(position, &collision->plane.n, -projectiles_ballistic_minimum_time, position);
        } else if (collision->type == 4) {
            scale_vector3d(velocity, 1.0f - response->initial_friction, velocity);
            projectile->team = collision->object_index;
        } else if (definition->detonation_timer != 0.0f) {
            response_type = 3;
            projectiles_old_flags_set(&projectile->flags, _projectile_grounded_bit, true);
            projectiles_old_flags_set(&projectile->flags, _projectile_collided_bit, true);
            projectiles_old_flags_set(&projectile->flags, _projectile_stopped_bit, true);
        } else {
            response_type = 0;
        }
    }
    if (response_type == 5)
        projectile->damage_scale = 0.0f;
    if (response_type == 4 || response_type == 5) {
        real_vector3d normal_component;
        real_vector3d perpendicular_component;
        projectiles_collision_components(relative_velocity, &collision->plane, &normal_component, &perpendicular_component);
        projectiles_collision_reflect(normal_component.i, normal_component.j, normal_component.k,
            perpendicular_component.i, perpendicular_component.j, perpendicular_component.k,
            1.0f - response->perpendicular_friction, 1.0f - response->parallel_friction, &perpendicular_component);
        real_vector3d difference;
        subtract_vectors3d(&perpendicular_component, relative_velocity, &difference);
        add_vectors3d(velocity, &difference, velocity);
        *relative_velocity = perpendicular_component;
        projectile->collision_time = 0.0f;
        projectile->parent_unit_index = -1;
        projectile->collision_count++;
    } else if (response_type != 2 && response_type != 0) {
        *velocity = *global_zero_vector3d;
    }
    if (response->angle_noise > 0.0f)
        _random_vector_in_cone3d(
            TLS_RANDOM_SEED, "response-new-angle-noise", "..\\items\\projectiles.cpp", 0x93c, velocity, 0.0f, response->angle_noise, velocity);
    projectiles_collision_apply_velocity_noise(response, velocity);
    float velocity_squared = magnitude_squared3d(velocity);
    if (response_type != 3 && magnitude_squared3d(velocity) < definition->minimum_velocity * definition->minimum_velocity) {
        projectiles_collision_set_action(projectile_index, 1);
        *detonate = true;
    }
    if (velocity_squared < 0.09f && (collision->plane.n.k > k_projectiles_collision_resting_normal || projectile->collision_count == 3)) {
        long physics_object_index = projectiles_collision_local_physics_object(projectile_index);
        projectiles_old_flags_set(&projectile->flags, _projectile_stopped_bit, true);
        projectiles_collision_set_vector(velocity, 0.0f, 0.0f, 0.0f);
        if (collision->object_index != -1 && collision->object_index != projectile_index) {
            if (definition->brute_grenade.count != 0)
                projectile_orient_brute_grenade(projectile_index, point, &collision->plane, collision->structure_bsp_index, collision->instance_index,
                    collision->object_index, collision->bsp_reference);
            projectile_attach(projectile_index, collision->object_index, collision->node_index, point, &collision->location, false);
            if (definition->brute_grenade.count == 0 && physics_object_index != -1 && collision->object_index == physics_object_index)
                projectiles_old_flags_set(&projectile->flags, _projectile_at_rest_bit, true);
        } else {
            object_set_at_rest(projectile_index, true);
        }
        if (definition->detonation_timer == 0.0f) {
            projectiles_collision_set_action(projectile_index, 1);
        } else {
            projectile_record_timer_start_condition(projectile, definition->timer_start);
        }
    }
    switch (response->scale_effects_by) {
    case 0: effect_scale = damage_scale; break;
    case 1:
        effect_scale = response_angle * 0.63661975f;
        effect_scale = PIN(effect_scale, 0.0f, 1.0f);
        break;
    }
    {
        s_effect_creation_data creation_data;
        creation_data.m_flags.clear();
        real_vector3d effect_direction;
        effect_direction.j = direction.j;
        effect_direction = direction;
        bool authoritative = !simulation_query_object_is_predicted(projectile_index);
        bool impact = response_type == 0;
        bool attached = response_type == 3;
        bool detonation = !projectile->flags.test(_projectile_timer_started_bit) && (projectile->flags.test(_projectile_stopped_bit) || response_type == 3);
        real_vector3d normal = collision->plane.n;
        normalize3d_with_default(&effect_direction, global_down3d);
        normalize3d(&normal);
        s_effect_vector effect_vectors[7];
        build_common_effect_vectors(point, &effect_direction, &normal, effect_vectors);
        effect_scale = PIN(effect_scale, 0.0f, 1.0f);
        shield_vitality = PIN(shield_vitality, 0.0f, 1.0f);
        effect_creation_data_initialize(&creation_data);
        creation_data.scale_a = effect_scale;
        creation_data.scale_b = shield_vitality;
        creation_data.effect_vector_count = 7;
        creation_data.effect_vectors = effect_vectors;
        creation_data.lightprobe_source = collision;
        creation_data.damage_reporting_type = (unsigned char)_rotl(definition->damage_reporting_flags, 2);
        if (projectiles_collision_authoritative() || TEST_BIT(projectile_object_get_datum(projectile_index)->flags_d6, 1) ||
            (projectile_supercombine_game_is_predicted() && (authoritative || (!impact && !attached)))) {
            if (response_velocity > projectiles_collision_minimum_effect_velocity()) {
                bool effect_created = false;
                for (long index = 3; index < 6; index++) {
                    long effect_index = effects[index];
                    if (effect_index != -1) {
                        projectile_effect_new(projectile_index, effect_index, collision->object_index, (short)collision->node_index,
                            &creation_data, _effect_non_deterministic);
                        effect_created = true;
                    }
                }
                if (projectiles_collision_material_valid(material_index) &&
                    ((unsigned char)create_impact_effect || !projectile_flag_test(definition->targeting_flags, 18)))
                {
                    long effect_index = -1;
                    if (collision->object_index != -1)
                        effect_index = definition->object_impact_effect.index;
                    if (effect_index == -1)
                        effect_index = definition->impact_effect.index;
                    if (effect_index != -1) {
                        projectile_effect_new(projectile_index, effect_index, collision->object_index, (short)collision->node_index,
                            &creation_data, _effect_deterministic);
                        effect_created = true;
                    }
                }
                for (long index = 0; index < 3; index++) {
                    long sound_index = effects[index];
                    if (sound_index != -1) {
                        s_projectile_sound_data sound;
                        sound.sound.position = *point;
                        projectiles_collision_sound_rotation(&sound.sound, &direction);
                        sound.location = collision->location;
                        sound.sound.velocity = *global_zero_vector3d;
                        unattached_impulse_sound_new(sound_index, &sound.sound, 1.0f);
                        effect_created = true;
                    }
                }
                if (detonation)
                    effect_created = true;
                if (effect_created && !projectile_supercombine_game_is_predicted() && projectile->gamestate_index != -1 && (impact || attached))
                    simulation_action_projectile_impact(detonation, projectile_index, effect_scale, shield_vitality, &effect_direction, collision, impact);
            }
            if (definition->detonation_started.index != -1 && !projectile->flags.test(_projectile_timer_started_bit) &&
                (projectile->flags.test(_projectile_stopped_bit) || response_type == 3))
                projectile_effect_new(projectile_index, definition->detonation_started.index, collision->object_index, (short)collision->node_index,
                    &creation_data, _effect_deterministic);
        }
    }

    bool result = false;
    switch (response_type) {
    case 6: result = true; projectiles_collision_set_action(projectile_index, 2); break;
    case 1: projectiles_collision_set_action(projectile_index, 2); break;
    case 0: projectiles_collision_set_action(projectile_index, 1); break;
    case 3:
        if (collision->type != 4 || projectile->parent != collision->object_index) {
            if (definition->brute_grenade.count != 0)
                projectile_orient_brute_grenade(projectile_index, point, &collision->plane, collision->structure_bsp_index, collision->instance_index,
                    collision->object_index, collision->bsp_reference);
            projectile_attach(projectile_index, collision->type == 4 ? collision->object_index : -1, collision->node_index, position,
                &collision->location, false);
        }
        break;
    case 4:
    case 5: result = true; break;
    }
    return result;
}

static __forceinline void projectiles_collision_set_action(long projectile_index, short action)
{
    s_projectile_object_datum *projectile = (s_projectile_object_datum *)object_get_and_verify_type(projectile_index, _object_mask_projectile);
    if (action > projectile->action)
        projectile->action = (signed char)action;
}
// @address 0x42d7c k_projectiles_grenade_test_directions
// @rdata 0x82042d7c..0x82042d88
static const real_vector3d k_projectiles_grenade_test_directions[] = { { 0.0f, 0.0f, -1.0f } };
// @rdata-end


static __forceinline bool projectiles_detonate_game_is_predicted()
{
    bool result = false;
    if (game_globals->options.game_simulation == _game_simulation_distributed_client)
        result = true;
    return result;
}

static __forceinline bool projectiles_detonate_game_is_server()
{
    bool result = true;
    signed char simulation = game_globals->options.game_simulation;
    if (simulation == _game_simulation_synchronous_client || simulation == _game_simulation_distributed_client)
        result = false;
    return result;
}

static __forceinline void projectiles_detonate_owner_get(long object_index, s_damage_owner *owner)
{
    // match: 0x3432e0: the non-null owner fixes the fmuls at +0x5b4 as fr12,fr31 (otherwise fr31,fr12)
    __assume(owner != 0);
    *owner = ((const object_datum *)projectile_object_get_datum(object_index))->damage_owner;
}

// @address 0x3432e0 ?projectile_detonate@@YAXJJ_NM@Z
void projectile_detonate(long projectile_index, long object_index, bool force, float scale)
{
    s_projectile_object_datum *projectile = projectile_object_get_datum(projectile_index);
    if (!projectile->flags.test(_projectile_detonated_bit)) {
        const projectile_definition *definition =
            (const projectile_definition *)tag_get(0, projectile->definition_index);
        bool super_detonation = false;
        long damage_object_index = -1;
        float damage_scale = projectile->damage_scale;
        bool air_detonation = true;
        s_location location;
        object_get_location(projectile_index, &location);
        projectile->flags.set(_projectile_detonated_bit, true);
        if (!projectiles_detonate_game_is_predicted()
            && projectile_flag_test(definition->targeting_flags, 3)
            && !projectile->flags.test(_projectile_supercombine_consumed_bit)
            && projectile->flags.test(_projectile_supercombine_deferred_bit)
            && projectile->parent != -1) {
            s_projectile_object_datum *parent = projectile_object_get_datum(projectile->parent);
            long count = 0;
            long detonation_count = 0;
            for (long child_index = parent->first_child_object_index; child_index != -1;) {
                s_projectile_object_datum *child = projectile_object_get_datum(child_index);
                if (child->definition_index == projectile->definition_index
                    && !projectile_object_get_datum(child_index)->flags.test(_projectile_supercombine_consumed_bit))
                    count++;
                child_index = child->next_object_index;
            }
            if (projectile->flags.test(_projectile_supercombine_use_actual_count_bit))
                detonation_count = count;
            else
                detonation_count = definition->supercombine_count;
            real_point3d center = *global_origin3d;
            for (long child_index = parent->first_child_object_index; child_index != -1;) {
                s_projectile_object_datum *child = projectile_object_get_datum(child_index);
                if (child->definition_index == projectile->definition_index
                    && !projectile_object_get_datum(child_index)->flags.test(_projectile_supercombine_consumed_bit)) {
                    s_projectile_object_datum *child_projectile = projectile_object_get_datum(child_index);
                    if (count <= definition->supercombine_count) {
                        child_projectile->flags.set(_projectile_supercombine_consumed_bit, true);
                        child_projectile->flags.set(_projectile_detonated_bit, true);
                        child_projectile->scale_a = 1.0f;
                        child_projectile->scale_b = 1.0f;
                        real_point3d child_origin;
                        object_get_origin(child_index, &child_origin);
                        add_points3d(&child_origin, &center, &center);
                    } else {
                        float detonation_time;
                        (void)&detonation_time;
                        float detonation_random = projectiles_random_real();
                        detonation_time = child_projectile->scale_a;
                        child_projectile->scale_a = detonation_time * detonation_random;
                        detonation_random = projectiles_random_real();
                        detonation_time = child_projectile->scale_b;
                        child_projectile->scale_b = detonation_time * detonation_random;
                    }
                    count--;
                }
                child_index = child->next_object_index;
            }
            scale_point3d(&center, 1.0f / (float)detonation_count, &center);
            damage_object_index = projectile->parent;
            object_detach(projectile_index);
            {
                real_point3d old_center = projectile->position;
                object_set_position_direct(projectile_index, &center, 0, 0, 0, false);
                object_force_inside_bsp(projectile_index, &old_center, projectile->parent);
            }
            super_detonation = true;
        }

        {
        real_point3d origin;
        real_vector3d forward;
        real_vector3d up;
        object_get_origin(projectile_index, &origin);
        object_get_orientation(projectile_index, &forward, &up);
        if (definition->detonation_sound.index != -1) {
            s_projectile_sound_data sound_data;
            sound_data.sound.position = origin;
            game_sound_location_set_rotation_from_direction(&sound_data.sound, &forward);
            sound_data.location = location;
            sound_data.sound.velocity = *global_zero_vector3d;
            unattached_impulse_sound_new(definition->detonation_sound.index, &sound_data.sound, 1.0f);
        }

        if (!projectiles_detonate_game_is_predicted()
            && !TEST_BIT(projectile_object_get_datum(projectile_index)->flags_d6, 1)) {
            long damage_effect_index;
            if (projectile->flags.test(_projectile_attached_bit))
                damage_effect_index = definition->attached_impact_damage.index;
            else if (super_detonation)
                damage_effect_index = definition->super_impact_damage.index;
            else
                damage_effect_index = definition->impact_damage.index;
            long target_index = super_detonation ? damage_object_index : projectile->parent;
            if (target_index != -1 && damage_effect_index != -1) {
                s_damage_data damage;
                projectile_damage_data_new(damage_data_new, &damage, damage_effect_index);
                damage.scale[0] = damage_scale;
                damage.reporting_info = (unsigned char)(_rotl(definition->damage_reporting_flags, 2) | 2);
                damage.flags |= 0x1008;
                object_get_orientation(projectile_index, &damage.direction, 0);
                object_get_origin(projectile_index, &damage.origin);
                damage.position = damage.origin;
                damage.shot_id = projectile->shot_id;
                projectile_effect_damage_owner_get(projectile_index, &damage.damage_owner);
                object_cause_damage(&damage, target_index, -1, -1, -1, 0);
            }
        }

        c_global_material_type material = projectile->impact_material;
        float proximity = definition->detonation_proximity;
        unsigned long event_flags = 0;
        long effect_index = -1;
        if (projectile->parent != -1) {
            air_detonation = (bool)((~definition->targeting_flags >> 13) & 1);
        } else if (projectile->parent == -1 && !projectile_flag_test(projectile->physics_flags, 9)) {
            material = k_global_material_type_invalid;
            if (proximity != 0.0f) {
                for (unsigned long index = 0; index < 1; index++) {
                    detonate_collision_result collision;
                    real_vector3d vector;
                    projectiles_grenade_scale_vector(&k_projectiles_grenade_test_directions[index], proximity, &vector);
                    if (collision_test_vector(
                            k_projectiles_collision_test_flags, true, &origin, &vector, projectile_index, -1, -1, &collision))
                    {
                        material.m_index = collision.global_material.m_index;
                        air_detonation = false;
                        break;
                    }
                }
            }
        } else {
            air_detonation = false;
        }
        if (super_detonation)
            effect_index = definition->super_detonation.index;
        else if (air_detonation)
            effect_index = definition->detonation_effect_airborne.index;
        else
            effect_index = definition->detonation_effect_ground.index;

        s_damage_owner damage_owner;
        projectiles_detonate_owner_get(projectile_index, &damage_owner);
        if (material.m_index != -1 && !projectile->flags.test(_projectile_impact_effect_created_bit)) {
            projectile->flags.set(_projectile_impact_effect_created_bit, true);
            event_flags |= 4;
            real_vector3d hit_normal;
            projectile_hit_normal_get(projectile_index, &hit_normal);
            normalize3d(&hit_normal);
            projectile_create_impact_effects(projectile->definition_index, &origin, &forward, &damage_owner,
                material, &hit_normal, &location);
        }
        if (effect_index != -1 && !projectile->flags.test(_projectile_no_impact_material_bit)) {
            const s_effect_vector *override_vector = 0;
            s_effect_vector effect_vector;
            if (super_detonation)
                event_flags |= 2;
            else
                event_flags |= 1;
            if (definition->detonation_offset.count) {
                const float *offset_definition = (const float *)definition->detonation_offset.address;
                detonate_collision_result collision;
                real_vector3d direction = projectile->linear_velocity;
                normalize3d_with_default(&direction, global_down3d);
                real_point3d point;
                real_vector3d displacement;
                projectiles_grenade_scale_vector(&direction, -1.0f * *offset_definition, &displacement);
                point_from_line3d(&origin, &displacement, 1.0f, &point);
                if (collision_test_line(k_projectiles_collision_test_flags, &origin, &point, projectile->parent_unit_index, projectile_index, &collision))
                    point = collision.point;
                override_vector = &effect_vector;
                effect_vector.vector = direction;
                effect_vector.point = point;
                effect_vector.marker_name = _string_id_forward;
            }
            real_vector3d hit_normal;
            unsigned char reporting_info = (unsigned char)_rotl(definition->damage_reporting_flags, 2);
            projectile_create_detonation_effect(projectile->definition_index, &origin, &forward, projectile_hit_normal_get(projectile_index, &hit_normal),
                &damage_owner, reporting_info,
                super_detonation, air_detonation, &location, override_vector);
        }
        if (definition->detonation_sound.index != -1)
            event_flags |= 8;
        if (projectiles_detonate_game_is_server() && event_flags) {
            if (air_detonation)
                event_flags |= 16;
            real_vector3d hit_normal;
            simulation_action_projectile_detonate(projectile_index, event_flags, &material,
                projectile_hit_normal_get(projectile_index, &hit_normal), (const s_projectile_hit *)&location, TEST_BIT(event_flags, 1));
        }
        projectile_cause_impact_damage(projectile_index, &origin, super_detonation, damage_scale, &forward, object_index, 0);
        widgets_new(projectile_index);
        if (definition->ai_radius > 0.0f && ai_globals->enabled)
            ai_sound_new_at_position(projectile_index, &origin, 3, definition->ai_volume);
        }
    }
}

// @rdata 0x82022518
static const float projectiles_translate_minimum_ground_normal = 0.3f;
static __forceinline void *projectiles_translate_block_get(const void *object, const s_object_block_reference *reference)
{
    // match: 0x33f130: const reference binding preserves temporary lifetime
    const void *const &block = (char *)object + reference->offset;
    return const_cast<void *>(block);
}
static __forceinline translate_attachment *projectiles_translate_get_attachments(const object_datum *object, long *count)
{
    translate_attachment *result = (translate_attachment *)projectiles_translate_block_get(object, &object->attached_lights);
    *count = (unsigned long)object->attached_lights.size / sizeof(translate_attachment);
    return result;
}
static __forceinline float projectiles_translate_magnitude3d(const real_vector3d *vector)
{
    return square_root(magnitude_squared3d(vector));
}
static __forceinline float projectiles_translate_tracking_dot(const real_vector3d *direction, const real_vector3d *velocity, float *alignment)
{
    *alignment = direction->i * velocity->i + direction->j * velocity->j + direction->k * velocity->k;
    return *alignment;
}
static __forceinline float projectiles_translate_lifetime(const projectile_definition *definition)
{
    float result = definition->lifetime;
    return result;
}
static __forceinline float projectiles_translate_detonation_timer(const projectile_definition *definition)
{
    float result = definition->detonation_timer;
    return result;
}
static __forceinline real_vector3d *projectiles_translate_orientation_cross(const real_vector3d *up, const real_vector3d *forward, real_vector3d *left)
{
    float forward_j = forward->j;
    float up_k = up->k;
    float i = up->j * forward->k - up_k * forward_j;
    float k = up->i * forward_j - up->j * forward->i;
    float j = up_k * forward->i - up->i * forward->k;
    left->i = i;
    left->j = j;
    left->k = k;
    return left;
}

static __forceinline void projectiles_translate_set_action(long projectile_index, short action)
{
    s_projectile_object_datum *projectile = projectile_object_get_datum(projectile_index);
    if (action > projectile->action)
        projectile->action = (signed char)action;
}
static __forceinline bool projectiles_translate_target_valid(const s_projectile_targeting_data *target)
{
    unsigned long type_mask = 0xffffffff;
    bool result = target->object_index != -1 && object_try_and_get_and_verify_type(target->object_index, type_mask) != 0;
    return result;
}
static __forceinline unsigned char projectiles_translate_test_collision(
    long projectile_index, const real_point3d *position, collision_result *collision, bool &tested)
{
    tested = true;
    return projectile_test_collision(projectile_index, position, collision);
}
static __forceinline long projectiles_translate_attachment_effect_index(const translate_attachment *attachment)
{
    return attachment->effect_index;
}
static __forceinline bool projectiles_translate_is_predicted(long object_index)
{
    bool result = false;
    if (projectiles_detonate_game_is_predicted())
        result = projectile_object_get_datum(object_index)->gamestate_index != -1;
    return result;
}
static __forceinline void *projectiles_translate_effect_try_and_get(long effect_index)
{
    void *result = 0;
    const s_data_array *effects = effect_data;
    if (effect_index != -1) {
        long absolute_index = effect_index & 0xffff;
        if (absolute_index < effects->first_unallocated) {
            s_datum_header *datum = (s_datum_header *)(effects->data + absolute_index * effects->size);
            if (datum->identifier != 0 && datum->identifier == effect_index >> 16)
                result = datum;
        }
    }
    return result;
}
static __forceinline void projectiles_translate_attachment_lens_flare(const translate_attachment *attachment)
{
    if (attachment->type != 2)
        return;
    long effect_index = attachment->effect_index;
    if (!projectiles_translate_effect_try_and_get(effect_index))
        return;
    effect_generate_lens_flares(effect_index, false);
}
static __forceinline void projectiles_translate_attachment_lens_flares(unsigned long count, translate_attachment *attachment)
{
    do {
        projectiles_translate_attachment_lens_flare(attachment);
        --count;
        ++attachment;
    } while (count != 0);
}

// @address 0x33f130 ?projectile_translate@@YAXJPATreal_vector3d@@@Z
void projectile_translate(long projectile_index, real_vector3d *unswept_velocity)
{
    g_projectile_last_damaged_parent = -1;
    s_projectile_object_datum *projectile = projectile_object_get_datum(projectile_index);
    const projectile_definition *definition =
        (const projectile_definition *)tag_get(0, projectile->definition_index);
    float time_remaining = game_tick_length();
    bool flyby_sound_created = false;
    bool distance_exceeded = false;
    bool detonate = false;
    short collision_count = 0;
    bool water_effect_created = false;
    long collision_object_index = -1;
    real_vector3d average_velocity;
    real_point3d water_point;
    projectile->scale_b += projectile->action_timer_rate;
    projectile->age++;
    bool timer_started;
    switch (definition->timer_start) {
    case 1: timer_started = projectile->flags.test(_projectile_grounded_bit); break;
    case 3: timer_started = projectile->flags.test(_projectile_collided_bit); break;
    case 2: timer_started = projectile->flags.test(_projectile_stopped_bit); break;
    default: timer_started = true; break;
    }
    if (projectile->flags.test(_projectile_timer_started_bit) || projectile->flags.test(_projectile_perception_attached_bit))
        timer_started = true;
    if (timer_started) {
        if (!projectile->flags.test(_projectile_timer_started_bit))
            projectiles_old_flags_set(&projectile->flags, _projectile_timer_started_bit, true);
        projectile->supercombine_timer += projectile->supercombine_timer_rate;
        if (projectile->supercombine_timer >= 1.0f)
            projectile_check_for_supercombine(projectile_index);
        projectile->scale_a += projectile->timer_rate;
        if (projectile->scale_a >= 1.0f)
            projectiles_translate_set_action(projectile_index, 1);
    }
    while (time_remaining > 0.0f &&
        (projectile->action == 0 || (projectile->action == 1 && projectile->action_timer_rate != 0.0f && projectile->scale_b < 1.0f)) &&
        !projectile->flags.test(_projectile_perception_attached_bit) && !projectile_flag_test(projectile->physics_flags, 9) &&
        !projectile->flags.test(_projectile_at_rest_bit) && projectile->parent == -1) {
        float speed = projectiles_translate_magnitude3d(&projectile->linear_velocity);
        real_vector3d velocity;
        velocity.i = 0.0f;
        velocity = projectile->linear_velocity;
        translate_collision_result collision;
        bool tested_collision = false;
        average_velocity = projectile->linear_velocity;
        if (projectiles_translate_target_valid(&projectile->targeting_data) && projectile->tracking_angular_velocity > 0.0f) {
            s_projectile_object_datum *target = projectile_object_get_datum(projectile->target_object_index);
            real_vector3d direction;
            direction.i = 0.0f;
            real_point3d target_point;
            float angular_scale = (projectile->distance - definition->start * projectile->velocity_scale) * definition->scale;
            angular_scale = projectiles_pin(angular_scale, 0.0f, 1.0f);
            float angular_velocity = definition->minimum_angular_velocity +
                (projectile->tracking_angular_velocity - definition->minimum_angular_velocity) * angular_scale;
            if ((1 << target->object_type) & 0x1003) {
                short flags = 12;
                // match: 0x33f130: zero-bit rotate preserves 32-bit GPR value
                if (!projectile_flag_test(_rotl(_rotl(_rotl(definition->targeting_flags, 0), 0), 0), 12))
                    flags |= 1;
                unit_notify_of_tracking_or_locking(projectile->target_object_index, projectile_index, flags);
                if (projectile_object_get_datum(projectile->target_object_index)->target_object_index != -1)
                    angular_velocity *= game_difficulty_get_value(19);
            }
            projectile_get_target_point(projectile_index, &target_point);
            vector_from_points3d(&projectile->position, &target_point, &direction);
            real_vector3d axis;
            cross_product3d(&projectile->linear_velocity, &direction, &axis);
            float alignment;
            if (((bool)((definition->targeting_flags >> 8) & 1) ||
                    projectiles_translate_tracking_dot(&projectile->linear_velocity, &direction, &alignment) > 0.0f) &&
                normalize3d(&axis) > 0.0f)
            {
                float angle = angular_velocity * time_remaining;
                float cosine = (float)cos(angle);
                float sine = (float)sin(angle);
                rotate_vector_about_axis(&velocity, &axis, sine, cosine);
                if ((bool)((definition->targeting_flags >> 6) & 1)) {
                    real_vector3d left;
                    real_vector3d forward;
                    forward.i = 0.0f;
                    forward = direction;
                    real_vector3d up;
                    up.j = 0.0f;
                    up = projectile->up;
                    normalize3d(&forward);
                    cross_product3d(&forward, &up, &left);
                    cross_product3d(&left, &forward, &up);
                    normalize3d(&up);
                    object_set_position_direct(projectile_index, 0, &forward, &up, 0, false);
                }
            }
        }
        if (!projectile->flags.test(_projectile_disable_deceleration_bit)) {
            if (definition->guided1 == 0.0f || definition->guided2 == 0.0f) {
                velocity = projectile->linear_velocity;
                average_velocity = projectile->linear_velocity;
            } else {
                const real_vector3d *acceleration_direction = &projectile->forward;
                float scale = (projectile->distance - definition->start * projectile->velocity_scale) * definition->scale;
                scale = projectiles_pin(scale, 0.0f, 1.0f);
                float desired_speed = (definition->guided1 + (definition->guided2 - definition->guided1) * scale) * projectile->velocity_scale;
                float forward_speed = dot_product3d(acceleration_direction, &velocity);
                float new_speed = desired_speed - projectile->acceleration_velocity + forward_speed;
                if (definition->guided2 > definition->guided1)
                    new_speed = projectiles_min(new_speed, definition->guided2);
                else
                    new_speed = projectiles_max(new_speed, definition->guided2);
                projectile->acceleration_velocity = desired_speed;
                projectile->distance += desired_speed * time_remaining;
                point_from_line3d(&velocity, acceleration_direction, new_speed - forward_speed, &velocity);
                float new_speed_magnitude = projectiles_translate_magnitude3d(&velocity);
                average_velocity.i = (projectile->linear_velocity.i + velocity.i) * 0.5f;
                average_velocity.j = (projectile->linear_velocity.j + velocity.j) * 0.5f;
                average_velocity.k = (projectile->linear_velocity.k + velocity.k) * 0.5f;
                speed = (speed + new_speed_magnitude) * 0.5f;
                if (projectiles_translate_lifetime(definition) == 0.0f && projectiles_translate_detonation_timer(definition) == 0.0f &&
                    projectile->time >= projectile->maximum_damage_distance)
                    projectiles_translate_set_action(projectile_index, 2);
            }
        }
        float gravity;
        if (projectile->m_object_flags.test(_object_data_flag_3))
            gravity = global_gravity_get() * definition->water_gravity_scale;
        else
            gravity = global_gravity_get() * definition->air_gravity_scale;
        gravity *= time_remaining;
        velocity.k -= gravity;
        average_velocity.k -= gravity * 0.5f;
        bool limit_reached = false;
        float time_scale = 1.0f;
        if (definition->lifetime != 0.0f && speed * time_remaining + projectile->time > definition->lifetime) {
            time_scale = speed != 0.0f ? (definition->lifetime - projectile->time) / (speed * time_remaining) : 0.0f;
            distance_exceeded = true;
            limit_reached = true;
        }
        if (definition->maximum_travel_distance != 0.0f && projectile->collision_time + speed * time_remaining > definition->maximum_travel_distance) {
            time_scale = speed != 0.0f ? (definition->maximum_travel_distance - projectile->collision_time) / (speed * time_remaining) : 0.0f;
            distance_exceeded = true;
            limit_reached = true;
        }
        time_scale = projectiles_max(time_scale, 0.0f);
        if (limit_reached && (!(bool)((definition->targeting_flags >> 19) & 1) || time_scale <= 0.0f))
            projectiles_translate_set_action(projectile_index, 1);
        real_point3d new_position;
        point_from_line3d(&projectile->position, &average_velocity, time_scale * time_remaining, &new_position);
        bool generate_lens_flares = false;
        if (collision_count != 10 && projectile->action != 2 &&
            projectiles_translate_test_collision(projectile_index, &new_position, &collision, tested_collision))
        {
                time_remaining *= 1.0f - collision.fraction;
                real_vector3d relative_velocity;
                subtract_vectors3d(&velocity, &projectile->linear_velocity, &relative_velocity);
                point_from_line3d(&projectile->linear_velocity, &relative_velocity, collision.fraction, &velocity);
                projectiles_old_flags_set(&projectile->flags, _projectile_collided_bit, true);
                if (collision.plane.normal.k > projectiles_translate_minimum_ground_normal)
                    projectiles_old_flags_set(&projectile->flags, _projectile_grounded_bit, true);
                if (ai_globals->enabled)
                    actor_handle_projectile_impact(projectile_index, collision.object_index);
                collision_count++;
                generate_lens_flares = projectile_collision(
                    projectile_index, &collision, &new_position, &velocity, unswept_velocity, time_remaining, collision_count, &detonate);
                collision_object_index = collision.object_index;
                if (ai_globals->enabled)
                    ai_sound_new_at_position(projectile_index, &collision.point, 2, definition->impact_ai_volume);
                water_point = collision.point;
        } else {
            if (collision_count == 10)
                projectiles_translate_set_action(projectile_index, 1);
            time_remaining = 0.0f;
            water_point = new_position;
        }
        if (!water_effect_created)
            water_effect_created = projectile_create_water_effects(projectile_index, &water_point);
        if (!projectile->flags.test(_projectile_perception_attached_bit) && tested_collision) {
            real_vector3d forward = projectile->forward;
            real_vector3d up = projectile->up;
            bool rotated = false;
            real_vector3d travel;
            vector_from_points3d(&projectile->position, &new_position, &travel);
            float travel_j = travel.j;
            float travel_k = travel.k;
            float travel_i = travel.i;
            float distance = square_root(travel_i * travel_i + travel_j * travel_j + travel_k * travel_k);
            projectile->time += distance;
            projectile->collision_time += distance;
            if (!flyby_sound_created && definition->flyby_sound.index != -1) {
                for (long user_index = players_first_active_user(); user_index != -1; user_index = players_next_active_user(user_index)) {
                    if (projectile_flyby_sound_try_new(
                            user_index, projectile->damage_owner.player_index, definition->flyby_sound.index, &projectile->position, &travel,
                                &collision.location))
                        flyby_sound_created = true;
                }
            }
            if (projectile_flag_test(definition->targeting_flags, 0) && (velocity.i != 0.0f || velocity.j != 0.0f || velocity.k != 0.0f)) {
                forward = velocity;
                if (normalize3d(&forward) > 0.0f) {
                    real_vector3d left;
                    projectiles_translate_orientation_cross(&up, &forward, &left);
                    cross_product3d(&forward, &left, &up);
                    if (normalize3d(&up) == 0.0f)
                        normalize3d(perpendicular3d(&forward, &up));
                } else {
                    forward = projectile->forward;
                }
                rotate_vector_about_axis(&up, &forward, projectile->rotation_sine, projectile->rotation_cosine);
                rotated = true;
            } else if (projectile->flags.test(_projectile_rotating_bit)) {
                rotate_vector_about_axis(&forward, &projectile->rotation_axis, projectile->rotation_sine, projectile->rotation_cosine);
                rotate_vector_about_axis(&up, &projectile->rotation_axis, projectile->rotation_sine, projectile->rotation_cosine);
                rotated = true;
            }
            if (rotated) {
                normalize3d(&forward);
                point_from_line3d(&up, &forward, -dot_product3d(&forward, &up), &up);
                normalize3d(&up);
            }
            if (projectile->parent == -1) {
                object_set_position_direct(projectile_index, &new_position, &forward, &up, &collision.location, false);
                object_set_velocities(projectile_index, &velocity, 0);
            }
            if (generate_lens_flares) {
                long attachment_count;
                translate_attachment *attachments = projectiles_translate_get_attachments((const object_datum *)projectile, &attachment_count);
                if (attachment_count > 0)
                    projectiles_translate_attachment_lens_flares(attachment_count, attachments);
            }
        }
    }
    switch (projectile->action) {
    case 1:
        if (!projectile->flags.test(_projectile_supercombine_deferred_bit) && !distance_exceeded && !detonate &&
            projectile->action_timer_rate != 0.0f && projectile->scale_b < 1.0f)
            break;
        if (projectiles_translate_is_predicted(projectile_index))
            projectiles_translate_set_action(projectile_index, 2);
        else
            projectile_detonate(projectile_index, collision_object_index, collision_count == 0, time_remaining);
    case 2:
        if (projectile->ignore_object_index != -1) {
            weapon_datum *object = (weapon_datum *)projectile_object_get_datum(projectile->ignore_object_index);
            real_point3d origin;
            object_get_origin(projectile_index, &origin);
            const weapon_definition *effect_definition = (const weapon_definition *)tag_get(0, object->definition_index);
            for (long index = 0; index < effect_definition->barrel_count; ++index)
                effect_set_location_vector(object->barrels[index].firing_effect_index, (const real_vector3d *)&origin);
        }
        if (projectiles_translate_is_predicted(projectile_index))
            object_set_hidden(projectile_index, true);
        else if (!projectile->flags.test(_projectile_distributing_bit))
            object_delete(projectile_index);
        break;
    }
    if ((game_time_globals->time & 3) == 3)
        projectile->collision_count = 0;
    projectiles_old_flags_set(&projectile->flags, _projectile_use_physics_bit, true);
}



static __forceinline float projectiles_initial_normalize3d(real_vector3d *vector)
{
    float length = sqrtf(vector->i * vector->i + vector->j * vector->j + vector->k * vector->k);
    if (fabs(length) >= k_projectiles_arc_epsilon) {
        float scale = 1.0f / length;
        scale_vector3d(vector, scale, vector);
    } else {
        length = 0.0f;
    }
    return length;
}

// @address 0x3400d8
void projectile_initial_update(long projectile_index, bool use_seed, unsigned short supplied_seed,
    // match: 0x3400d8: without __restrict the li/stb that clears *distributed moves below the seed table lwzx loads
    bool *__restrict distributed, unsigned short *__restrict result_seed)
{
    s_projectile_object_datum *projectile = projectile_object_get_datum(projectile_index);
    const projectile_definition *definition =
        (const projectile_definition *)tag_get(0, projectile->definition_index);
    *distributed = false;
    if (projectile_flag_test(definition->targeting_flags, 5)) {
        if (definition->conical_distribution.count) {
            const conical_distribution *distribution = (const conical_distribution *)definition->conical_distribution.address;
            s_projectile_object_datum original;
            memcpy(&original, projectile, sizeof(original));
            real_vector3d direction = projectile->linear_velocity;
            float speed = normalize3d(&direction);
            if (speed > 0.0001f) {
                unsigned short seed = (unsigned short)(random_seed_next(TLS_RANDOM_SEED) >> 16);
                if (use_seed)
                    seed = supplied_seed;
                *result_seed = seed;
                *distributed = true;
                unsigned long local_seed = seed;
                long projectile_number = 0;
                long projectile_count = distribution->angular_count * distribution->radial_count;
                for (long angular_index = 0; angular_index < distribution->angular_count; ++angular_index) {
                    for (long radial_index = 0; radial_index < distribution->radial_count; ++radial_index) {
                        real_vector3d forward;
                        real_vector3d up;
                        _distriubuite_vector_in_cone3d(&local_seed, "conical projectiles", "..\\items\\projectiles.cpp", 1196,
                            &direction, radial_index, distribution->radial_count, angular_index, distribution->angular_count,
                            0.0f, distribution->maximum_angle, distribution->exponent, &forward);
                        projectiles_initial_normalize3d(perpendicular3d(&forward, &up));
                        memcpy(&projectile->flags, &original.flags, sizeof(original) - sizeof(object_datum));
                        if (projectile->parent != -1)
                            object_detach(projectile_index);
                        object_set_position_direct(projectile_index, &original.position, &forward, &up,
                            (const s_location *)&original.location, false);
                        object_set_at_rest(projectile_index, false);
                        scale_vector3d(&projectile->forward, speed, &projectile->linear_velocity);
                        if (projectile_number != projectile_count - 1)
                            projectile->flags.set(_projectile_distributing_bit, true);
                        real_vector3d unswept_velocity = projectile->linear_velocity;
                        projectile_translate(projectile_index, &unswept_velocity);
                        ++projectile_number;
                    }
                }
            }
        } else {
            real_vector3d unswept_velocity = projectile->linear_velocity;
            projectile_translate(projectile_index, &unswept_velocity);
        }
    }
}

// @address 0x7092c0 ??__Edefault_projectile_material_response@@YAXXZ
s_projectile_material_response_definition default_projectile_material_response;
