// objectslibxbox/multiplayer_game_objects.obj: from the records 0x2ed458 0x351830 0x351b40 0x351c40 0x351d30 0x351e38 0x352080 0x352288
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "memory/data_types.h"
#include "memory/tls_globals.h"
#include "tag_files/tag_groups.h"
#include "objects/object_header.h"
#include "math/matrix_math_types.h"
#include "math/real_math_types.h"
#include "math/real_math.h"
#include "tag_files/tag_types.h"
#include "game/player_types.h"
#include "game/game_globals.h"
#include "objects/object_datum.h"
#include "objects/object_placement.h"
#include "game/map_variant.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "game/game_engine.h"
#include "objects/objects.h"
#include "units/units.h"
#include "math/collisions_simple.h"
#include "objects/multiplayer_game_objects.h"
#include "math/matrix_math.h"
#include "scenario/scenario.h"
#include "objects/multiplayer_object_types.h"
#include "game/game_engine_vip_variant.h"
#include "game/game_engine_vip_globals.h"
#include "game/game_engine_settings_definitions.h"
#include "game/multiplayer_runtime_globals.h"
#include "objects/object_definitions.h"
#include "game/player_access.h"
// @address 0x2ed458 ?multiplayer_object_calculate_boundary_geometry@@YAXPAUs_multiplayer_object_boundary_geometry_data@@J@Z
// @address 0x351c40 ?multiplayer_get_spawn_object_position_and_facing_vectors@@YAXJPATreal_point3d@@PATreal_vector3d@@1@Z
// @address 0x351d30 ?point_in_multiplayer_object_boundary@@YA_NPBTreal_point3d@@PBUs_multiplayer_object_boundary_geometry_data@@@Z
// @address 0x351e38 ?player_in_multiplayer_object_boundary@@YA_NJPBUs_multiplayer_object_boundary_geometry_data@@@Z
// @address 0x352080 ?multiplayer_object_calculate_boundary_geometry_from_properties@@YAXJPBUs_multiplayer_object_properties@@PAUs_multiplayer_object_boundary_geometry_data@@_N@Z
// @address 0x352288 ?multiplayer_object_calculate_box_boundary_geometry@@YAXPBUs_multiplayer_object_properties@@PAUs_multiplayer_object_boundary_geometry_data@@@Z

template<typename t_enum, typename t_storage, long k_minimum, long k_maximum>
static __forceinline bool multiplayer_game_objects_enum_in_range(const c_enum<t_enum, t_storage, k_minimum, k_maximum> &value)
{
    bool result = false;
    if (value.m_storage >= k_minimum && value.m_storage < k_maximum)
        result = true;
    return result;
}

template<typename t_enum, typename t_storage, long k_count>
static __forceinline bool multiplayer_game_objects_flags_valid(const c_flags_no_init<t_enum, t_storage, k_count> &flags)
{
    unsigned long mask = (1UL << k_count) - 1;
    return ((unsigned long)flags.get_unsafe() & ~mask) == 0;
}

// @rdata 0x8202490c
static const float multiplayer_game_objects_boundary_size_epsilon = 0.0001f;

void multiplayer_object_calculate_boundary_geometry(s_multiplayer_object_boundary_geometry_data *out_geometry_data, long target_object_index)
{
    if (target_object_index != -1)
    {
        s_multiplayer_object_properties local;
        long definition_index = -1;
        local.flags.clear();
        local.passability.clear();
        local.game_engine_flags.clear();
        if (game_engine_running() && global_game_globals != 0 && global_game_globals->multiplayer_globals_index != -1)
        {
            const s_multiplayer_globals_definition *multiplayer_globals
                = TAG_GET('mulg', const s_multiplayer_globals_definition, global_game_globals->multiplayer_globals_index);
            if (multiplayer_globals != 0)
            {
                const s_multiplayer_runtime_globals_definition *runtime
                    = (const s_multiplayer_runtime_globals_definition *)multiplayer_globals->runtime.address;
                definition_index = runtime->vip_influence_area.index;
            }
        }

        const s_object_definition *boundary_definition = TAG_GET('obje', const s_object_definition, definition_index);
        const s_multiplayer_object_properties_definition *geometry_definition
            = (const s_multiplayer_object_properties_definition *)boundary_definition->multiplayer_object.address;
        local.initialize_from_object_definition_and_scenario_placement(definition_index, geometry_definition, 0);
        float value = (float)(short)g_game_engine_vip_variant->m_influence_radius;
        local.box_length = value;
        local.box_width = value;
        multiplayer_object_calculate_boundary_geometry_from_properties(target_object_index, &local, out_geometry_data, false);
    }
}

// @address 0x351830 ?initialize_from_object_definition_and_scenario_placement@s_multiplayer_object_properties@@QAAXJPBUs_multiplayer_object_properties_definition@@PBUs_scenario_multiplayer_object_properties@@@Z
void s_multiplayer_object_properties::initialize_from_object_definition_and_scenario_placement(long definition_index,
    const s_multiplayer_object_properties_definition *definition,
    const s_scenario_multiplayer_object_properties *placement)
{
    flags = definition->flags;
    object_type = definition->object_type;
    boundary_shape = definition->boundary_shape;
    remapping_policy = _multiplayer_object_remapping_policy_none;
    teleporter_channel = _teleporter_channel_default;
    passability = definition->passability;
    symmetry = _game_engine_symmetric_placement_default;
    game_engine_flags = definition->game_engine_flags;
    if (game_engine_flags.is_empty())
        game_engine_flags.set_unsafe(0x3ff);
    team_designator.m_storage = 8;
    spawn_sequence = -1;
    minimum_count = 0;
    maximum_count = -1;
    spawn_timer_mode = definition->spawn_timer_mode;
    spawn_time = definition->spawn_time;
    abandon_time = definition->abandon_time;
    box_width = definition->box_width;
    box_length = definition->box_length;
    box_height = definition->box_height;
    box_top = definition->box_top;
    boundary_size = definition->boundary_size;
    boundary_rotation = 0.0f;
    boundary_inner_radius = definition->boundary_inner_radius;
    boundary_outer_radius = definition->boundary_outer_radius;
    boundary_center_marker = definition->boundary_center_marker;
    spawn_marker_name = definition->spawn_marker_name;
    boundary_effect = definition->boundary_effect.index;
    boundary_material = definition->boundary_material;
    if (placement)
    {
        flags.set(_multiplayer_object_spawn_at_start, placement->spawn_flags.test(_multiplayer_object_placement_spawn_at_start));
        flags.set(_multiplayer_object_spawn_on_disturbance, placement->spawn_flags.test(_multiplayer_object_placement_spawn_on_disturbance));
        boundary_shape = multiplayer_game_objects_enum_in_range(placement->boundary_shape) &&
            placement->boundary_shape != _multiplayer_object_boundary_shape_unused
            ? placement->boundary_shape : boundary_shape;
        remapping_policy = placement->remapping_policy;
        teleporter_channel = placement->teleporter_channel;
        symmetry = placement->symmetry;
        const bool &placement_flags_valid = multiplayer_game_objects_flags_valid(placement->game_engine_flags);
        if (placement_flags_valid && !placement->game_engine_flags.is_empty())
            game_engine_flags = placement->game_engine_flags;
        team_designator = placement->team_designator;
        spawn_sequence = placement->spawn_sequence;
        minimum_count = placement->minimum_count;
        maximum_count = placement->maximum_count;
        if (placement->spawn_time > 0)
            spawn_time = placement->spawn_time;
        if (placement->abandon_time != 0)
            abandon_time = placement->abandon_time;
        if (fabsf(placement->box_width) > 0.001f)
            box_width = placement->box_width;
        if (fabsf(placement->box_length) > 0.001f)
            box_length = placement->box_length;
        if (fabsf(placement->box_height) > 0.001f)
            box_height = placement->box_height;
        if (fabsf(placement->box_top) > 0.001f)
            box_top = placement->box_top;
        if (fabsf(placement->boundary_size) > multiplayer_game_objects_boundary_size_epsilon)
            boundary_size = placement->boundary_size;
    }
    standard_shader = definition->standard_shader.index;
    opaque_shader = definition->opaque_shader.index;
    switch (boundary_shape)
    {
    case 1:
        standard_shader = definition->sphere_standard_shader.index != -1 ? definition->sphere_standard_shader.index : standard_shader;
        opaque_shader = definition->sphere_opaque_shader.index != -1 ? definition->sphere_opaque_shader.index : opaque_shader;
        break;
    case 2:
        standard_shader = definition->cylinder_standard_shader.index != -1 ? definition->cylinder_standard_shader.index : standard_shader;
        opaque_shader = definition->cylinder_opaque_shader.index != -1 ? definition->cylinder_opaque_shader.index : opaque_shader;
        break;
    case 3:
        standard_shader = definition->box_standard_shader.index != -1 ? definition->box_standard_shader.index : standard_shader;
        opaque_shader = definition->box_opaque_shader.index != -1 ? definition->box_opaque_shader.index : opaque_shader;
        break;
    }
    validate_and_report(definition_index);
}

// @address 0x351b40 ?validate_and_report@s_multiplayer_object_properties@@QAAXJ@Z
void s_multiplayer_object_properties::validate_and_report(long definition_index)
{
    const bool &flags_valid = multiplayer_game_objects_flags_valid(flags);
    if (!flags_valid)
        flags.clear();
    if (object_type.m_storage < 0 || object_type.m_storage >= 24)
        object_type = _multiplayer_object_type_none;
    if (boundary_shape.m_storage < 0 || boundary_shape.m_storage >= 4)
        boundary_shape = _multiplayer_object_boundary_shape_unused;
    if (remapping_policy.m_storage < 0 || remapping_policy.m_storage >= 3)
        remapping_policy = _multiplayer_object_remapping_policy_none;
    if (teleporter_channel.m_storage >= 26)
        teleporter_channel = _teleporter_channel_default;
    const bool &passability_valid = multiplayer_game_objects_flags_valid(passability);
    if (!passability_valid)
        passability.clear();
    if (symmetry.m_storage >= 3)
        symmetry = _game_engine_symmetric_placement_default;
    const bool &game_engine_flags_valid = multiplayer_game_objects_flags_valid(game_engine_flags);
    if (!game_engine_flags_valid)
        game_engine_flags.clear();
    if (team_designator.m_storage >= 9)
        team_designator.m_storage = 0;
    if (spawn_timer_mode.m_storage < 0 || spawn_timer_mode.m_storage >= 2)
        spawn_timer_mode = _multiplayer_object_spawn_timer_default;
}

void multiplayer_get_spawn_object_position_and_facing_vectors(long object_index, real_point3d *position, real_vector3d *forward, real_vector3d *up)
{
    object_datum *object = object_get(object_index);
    s_multiplayer_object_properties *properties = (s_multiplayer_object_properties *)((char *)object + object->multiplayer_data_offset);

    bool get_world_matrix = true;
    real_matrix4x3 matrix;
    if (properties->spawn_marker_name != -1 && properties->spawn_marker_name != 0)
    {
        object_marker marker;
        if (internal_object_get_markers_by_string_id(object_index, properties->spawn_marker_name, &marker, 1, true) > 0)
        {
            memcpy(&matrix, &marker.matrix, sizeof(real_matrix4x3));
            get_world_matrix = false;
        }
    }

    if (get_world_matrix)
    {
        ::object_get_world_matrix(object_index, &matrix);
    }

    *position = matrix.position;
    matrix4x3_transform_vector(&matrix, global_forward3d, forward);
    matrix4x3_transform_vector(&matrix, global_up3d, up);
}

static __forceinline bool multiplayer_game_objects_point_in_sphere_boundary(
    const real_point3d *point, const s_multiplayer_object_boundary_geometry_data *boundary)
{
    return collision_point_in_sphere(point, &boundary->matrix.position, boundary->radius);
}

static __forceinline bool multiplayer_game_objects_point_in_cylinder_boundary(
    const real_point3d *point, const s_multiplayer_object_boundary_geometry_data *boundary)
{
    if (collision_point_in_sphere(point, &boundary->matrix.position, boundary->radius))
        return ::collision_point_in_cylinder(point, &boundary->matrix,
            boundary->width, boundary->height + (float)fabs(boundary->neg_height), (float *)(0));
    else
        return false;
}

static __forceinline bool multiplayer_game_objects_point_in_box_boundary(const real_point3d *point, const s_multiplayer_object_boundary_geometry_data *boundary)
{
    if (collision_point_in_sphere(point, &boundary->matrix.position, boundary->radius))
        return ::collision_point_in_box(point, &boundary->matrix, boundary->width,
            boundary->depth, boundary->height + (float)fabs(boundary->neg_height));
    else
        return false;
}

bool point_in_multiplayer_object_boundary(const real_point3d *point, const s_multiplayer_object_boundary_geometry_data *boundary)
{
    bool result = false;
    switch (boundary->type)
    {
    case 1:
        result = multiplayer_game_objects_point_in_sphere_boundary(point, boundary);
        break;
    case 2:
        result = multiplayer_game_objects_point_in_cylinder_boundary(point, boundary);
        break;
    case 3:
        result = multiplayer_game_objects_point_in_box_boundary(point, boundary);
        break;
    }
    return result;
}



bool player_in_multiplayer_object_boundary(long player_index, const s_multiplayer_object_boundary_geometry_data *boundary)
{
    bool result = false;
    player_datum *player = player_get(player_index);
    if (player->unit_index != -1)
    {
        real_point3d points[2];
        object_get_origin(player->unit_index, &points[0]);
        unit_get_body_position(player->unit_index, &points[1]);
        for (unsigned long i = 0; !result && i < 2; i++)
            result = point_in_multiplayer_object_boundary(&points[i], boundary);
    }
    return result;
}

static __forceinline void multiplayer_game_objects_calculate_sphere_boundary_geometry(
    const s_multiplayer_object_properties *multiplayer_object_properties, s_multiplayer_object_boundary_geometry_data *boundary_geometry)
{
    boundary_geometry->radius = multiplayer_object_properties->box_width;
}

static __forceinline void multiplayer_game_objects_calculate_cylinder_boundary_geometry(
    const s_multiplayer_object_properties *multiplayer_object_properties, s_multiplayer_object_boundary_geometry_data *boundary_geometry)
{
    real_vector3d up;
    float height = (float)fabs(multiplayer_object_properties->box_height);
    float depth = (float)fabs(multiplayer_object_properties->box_top);
    float total_height = depth + height;
    float offset = height - total_height * 0.5f;
    float radius = multiplayer_object_properties->box_width;

    matrix4x3_transform_vector(&boundary_geometry->matrix, global_up3d, &up);
    point_from_line3d(&boundary_geometry->matrix.position, &up, offset, &boundary_geometry->matrix.position);

    float maximum_height = height > depth ? height : depth;
    boundary_geometry->radius = maximum_height + radius + total_height;
}

// March picks opaque_shader when the last argument is set; the release body always takes standard_shader.
void multiplayer_object_calculate_boundary_geometry_from_properties(long object_index,
    const s_multiplayer_object_properties *multiplayer_object_properties, s_multiplayer_object_boundary_geometry_data *boundary_geometry,
    bool)
{
    memset(boundary_geometry, 0, sizeof(*boundary_geometry));
    boundary_geometry->type = multiplayer_object_properties->boundary_shape;
    boundary_geometry->shader = multiplayer_object_properties->standard_shader;
    boundary_geometry->width = multiplayer_object_properties->box_width;
    boundary_geometry->depth = multiplayer_object_properties->box_length;
    boundary_geometry->height = multiplayer_object_properties->box_height;
    boundary_geometry->neg_height = multiplayer_object_properties->box_top;
    ::object_get_world_matrix(object_index, &boundary_geometry->matrix);

    if (multiplayer_object_properties->boundary_center_marker != -1 && multiplayer_object_properties->boundary_center_marker != 0)
    {
        object_marker marker;
        if (internal_object_get_markers_by_string_id(object_index, multiplayer_object_properties->boundary_center_marker, &marker, 1, true) > 0)
            boundary_geometry->matrix.position = marker.matrix.position;
    }

    boundary_geometry->matrix.scale = 1.0f;
    if ((bool)(_rotl(multiplayer_object_properties->flags, 30) & 1))
    {
        boundary_geometry->matrix.forward = *global_forward3d;
        boundary_geometry->matrix.left = *global_left3d;
        boundary_geometry->matrix.up = *global_up3d;
    }

    switch (multiplayer_object_properties->boundary_shape)
    {
    case 1:
        multiplayer_game_objects_calculate_sphere_boundary_geometry(multiplayer_object_properties, boundary_geometry);
        break;
    case 2:
        multiplayer_game_objects_calculate_cylinder_boundary_geometry(multiplayer_object_properties, boundary_geometry);
        break;
    case 3:
        multiplayer_object_calculate_box_boundary_geometry(multiplayer_object_properties, boundary_geometry);
        break;
    }
}

void multiplayer_object_calculate_box_boundary_geometry(
    const s_multiplayer_object_properties *multiplayer_object_properties, s_multiplayer_object_boundary_geometry_data *boundary_geometry)
{
    real_vector3d up;
    float width = multiplayer_object_properties->box_width;
    float length = multiplayer_object_properties->box_length;
    float height = (float)fabs(multiplayer_object_properties->box_height);
    float depth = (float)fabs(multiplayer_object_properties->box_top);
    float half_extent = (depth + height) * 0.5f;
    float offset = height - half_extent;

    matrix4x3_transform_vector(&boundary_geometry->matrix, global_up3d, &up);
    point_from_line3d(&boundary_geometry->matrix.position, &up, offset, &boundary_geometry->matrix.position);

    float x = (float)fabs(width) * 0.5f + boundary_geometry->matrix.position.x;
    float y = (float)fabs(length) * 0.5f + boundary_geometry->matrix.position.y;
    float z = boundary_geometry->matrix.position.z + half_extent;
    boundary_geometry->radius = sqrtf(x * x + (z * z + y * y)) + (float)fabs(offset);
}
