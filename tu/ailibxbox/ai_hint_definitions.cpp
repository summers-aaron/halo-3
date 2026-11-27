// ailibxbox/ai_hint_definitions.obj: from the records 0xcdf30 0xcf038 0xd0668 0xd06f0 0xd07b8 0x528858 0x5289d8 0x528c78 0x528e58 0x528f90 0x529100 0x5291e0 0x529508 0x529978 0x529b80 0x529d40 0x529f10 0x52a068 0x52a0f8 0x52a1e8
// @flags /TP /O2
#include "math/matrix_math_types.h"
#include "memory/tls_globals.h"
#include "objects/object_header.h"
#include "math/real_math_types.h"
#include "math/real_math.h"
#include "memory/data_types.h"
#include "ai/sector_ref.h"
#include "objects/object_placement.h"
#include "ai/actor_datum.h"
#include "cseries/cseries.h"
#include <float.h>
#include <math.h>
#include <string.h>
#include "tag_files/tag_groups.h"
#include "objects/objects.h"
#include "ai/ai_hint_definitions.h"
#include "ai/ai_reference_frame.h"
#include "ai/sector.h"
#include "items/projectiles.h"
#include "units/unit_animation.h"
#include "ai/character_definitions.h"
#include "ai/character_properties_types.h"
#include "objects/object_datum.h"
#include "ai/special_movement_types.h"
#include "game/game_globals.h"
#include "ai/ai_globals_definition.h"
#include "ai/ai_point3d.h"
#include "scenario/scenario_tag.h"
#include "ai/pathfinding_data.h"
#include "ai/pathfinding_geometry.h"
#include "scenario/scenario.h"
#include "scenario/scenario_access.h"
#include "units/unit_types.h"
#include "devices/machine_datum.h"
#include "objects/object_runtime_globals_types.h"
#include "math/real_math_constants.h"
#include "objects/object_runtime_tls.h"
#include "ai/ai_hint_jump.h"
#include "ai/sector_geometry.h"
#include "physics/collision_types.h"
#include "memory/data.h"
// @address 0xd0668 ?ai_hint_get_vault_markers@@YAFJPAUobject_marker@@FF@Z
// @address 0xd06f0 ?ai_hint_get_mount_markers@@YAFJPAUobject_marker@@FF@Z
// @address 0xd07b8 ?ai_hint_get_hoist_markers@@YAFJPAUobject_marker@@FF@Z
enum e_action_hoist;

struct s_jump_seam_jump_index {
  short jump_index;
  short pad2;
};
struct s_jump_seam {
  short seam_index;
  short pad2;
  float length;
  union { s_tag_block jumps; struct { long jump_index_count; TAG_POINTER(s_jump_seam_jump_index) jump_indices; TAG_POINTER(void) jump_indices_definition; }; };
};
// @rdata 0x820242d4 k_vault_score_numerator
static const float k_vault_score_numerator = 3.0f;
// @rdata 0x820242d8 k_vault_score_scale
static const float k_vault_score_scale = 2.0f;
// @rdata 0x82021694 k_vault_initial_score
static const float k_vault_initial_score = FLT_MAX;
// @rdata 0x8202251c k_vault_score_zero
static const float k_vault_score_zero = 0.0f;
// @rdata 0x820221fc k_vault_score_one
static const float k_vault_score_one = 1.0f;
// @rdata 0x8202215c k_vault_marker_vertical_threshold
static const float k_vault_marker_vertical_threshold = 0.95f;

struct pathfinding_hint_data
{
    long type;
    hint_jump_data jump;
};

static __forceinline const sector_link *hint_get(pathfinding_data *scenario, long index)
{
  return &((sector_link *)scenario->links.address)[index];
}
static __forceinline const pathfinding_hint *volume_get(pathfinding_data *scenario, long index)
{
  return &((pathfinding_hint *)scenario->hints.address)[index];
}
static __forceinline bool object_name_traversable(long name_index)
{
  bool result = true;
  if (name_index >= 0 && name_index < global_scenario->machines.count) {
    s_object_globals *list = tls_object_runtime_globals();
    if (list->first_name_index[7] != -1) {
      long object_index = list->object_indices[list->first_name_index[7] + name_index];
      if (object_index != -1) {
        s_machine_datum *object = (s_machine_datum *)object_get_and_verify_type(object_index, NONE);
#if defined(_XBOX) && !defined(__clang__)
        // match: 0xd07b8: the float loads through the constant-pool base register the function already holds
        if (((1 << object->object_type) & 0x80) && object->device_position <= (&k_real_zero_storage)[553])
#else
        if (((1 << object->object_type) & 0x80) && object->device_position <= 0.5f)
#endif
          result = object->power > k_real_zero_storage && !object->machine_flags.test(_machine_flag_0);
      }
    }
  }
  return result;
}
static __forceinline unsigned char ai_hint_get_vault_height(short vault_height, float *lower, float *upper)
{
    bool valid = false;
    if (!(vault_height < 0) && !(vault_height >= 2) && global_game_globals != 0 && global_game_globals->ai_globals.count() > 0) {
        s_ai_globals_definition *ai_globals = TAG_BLOCK_GET_ELEMENT(&global_game_globals->ai_globals, 0, s_ai_globals_definition);
        valid = 1;
        *lower = ai_globals->vault_heights[vault_height].lower;
        *upper = ai_globals->vault_heights[vault_height].upper;
    }
    return valid;
}

bool ai_link_hint_traversable(pathfinding_data *scenario, long trigger_index)
{
  bool result = true;
  const sector_link *hint = hint_get(scenario, trigger_index);
  if (hint->hint_index != -1) {
    const pathfinding_hint *volume = volume_get(scenario, hint->hint_index);
    if (volume->type == 10)
      result = object_name_traversable(volume->jump.data[1]);
  }
  return result;
}

short ai_hint_get_vault_markers(long object_index, object_marker *markers, short maximum_marker_count, short flags)
{
    short marker_count = 0;
    if (flags & 1)
    {
        marker_count = internal_object_get_markers_by_string_id(object_index, 0x1c3, markers, 32, false);
    }
    if (marker_count < 32 && (flags & 2))
    {
        marker_count += internal_object_get_markers_by_string_id(object_index, 0x1c4, &markers[marker_count],
            32 - marker_count, false);
    }
    return marker_count;
}

short ai_hint_get_mount_markers(long object_index, object_marker *markers, short maximum_marker_count, short flags)
{
    short marker_count = 0;
    if (flags & 0x20)
    {
        marker_count = internal_object_get_markers_by_string_id(object_index, 0x1c5, markers, 32, false);
    }
    if (marker_count < 32)
    {
        if (flags & 0x40)
        {
            marker_count += internal_object_get_markers_by_string_id(object_index, 0x1c6, &markers[marker_count],
                32 - marker_count, false);
        }
        if (marker_count < 32 && (flags & 0x80))
        {
            marker_count += internal_object_get_markers_by_string_id(object_index, 0x1c7, &markers[marker_count],
                32 - marker_count, false);
        }
    }
    return marker_count;
}

short ai_hint_get_hoist_markers(long object_index, object_marker *markers, short maximum_marker_count, short flags)
{
    short marker_count = 0;
    if (flags & 0x800)
    {
        marker_count = internal_object_get_markers_by_string_id(object_index, 0x1c8, markers, 32, false);
    }
    if (marker_count < 32 && (flags & 0x1000))
    {
        marker_count += internal_object_get_markers_by_string_id(object_index, 0x1c9, &markers[marker_count],
            32 - marker_count, false);
    }
    return marker_count;
}

bool ai_hint_vault_range_valid(const real_point3d *start, const real_point3d *end, const special_movement *movement)
{
    bool result = false;
    int mask = movement->movement_hints & 3;
    short vault_height;
    float lower;
    float upper;
    for (vault_height = 0; vault_height < 2; vault_height++) {
        if (((1 << vault_height) & mask) != 0 && ai_hint_get_vault_height(vault_height, &lower, &upper)) {
            float height = end->z - start->z;
            result = !(height < lower) && (height <= upper);
            if (result) break;
        }
    }
    return result;
}

// @address 0x528858 ?pathfinding_find_seam_link@@YA_NFJPAFPAJ1@Z
bool pathfinding_find_seam_link(short structure_bsp_index, long marker_index, short * out_structure_bsp_index,
    long * out_element, long * out_value)
{
    pathfinding_data *bsp = (pathfinding_data *)pathfinding_data_get(structure_bsp_index);
    pathfinding_hint *marker = &((pathfinding_hint *)bsp->hints.address)[marker_index];
    *out_structure_bsp_index = -1;
    *out_element = -1;
    *out_value = -1;
    const struct scenario *scenario = global_scenario;
    bool result = false;
    short active_mask = (short)g_active_structure_bsp_mask;
    if (marker->type == 9) {
        for (short i = 0; i < scenario->structure_bsps.count; i++) {
            if (i != structure_bsp_index && (active_mask & (1 << i)) != 0) {
                pathfinding_data *other = (pathfinding_data *)pathfinding_data_get(i);
                if (other) {
                    long group_index = marker->jump.data[0];
                    if (group_index >= 0 && group_index < other->seam_link_indices.count) {
                        s_tag_block *group = &((s_tag_block *)other->seam_link_indices.address)[marker->jump.data[0]];
                        long element_index = marker->jump.data[1];
                        if (element_index >= 0 && element_index < group->count) {
                            long *element = &((long *)group->address)[element_index];
                            if (*element != -1 && *element != 0xffff) {
                                long cluster_index = *element;
                                sector_link *cluster = &((sector_link *)other->links.address)[cluster_index];
                                *out_structure_bsp_index = i;
                                *out_element = cluster_index;
                                short portal_index = cluster->hint_index;
                                if (portal_index != -1) {
                                    pathfinding_hint *portal = &((pathfinding_hint *)other->hints.address)[portal_index];
                                    if (portal->type == 9)
                                        *out_value = portal->jump.data[2];
                                }
                                result = true;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    return result;
}

// @address 0x5289d8 ?pathfinding_find_jump_seam@@YAJPAUpathfinding_data@@FMPAF@Z
long pathfinding_find_jump_seam(pathfinding_data *exclude_data, short seam_index, float distance, short *out_structure_bsp_index)
{
    long jump_index = -1;
    long structure_bsp_count = global_scenario->structure_bsps.count;
    for (short structure_bsp_index = 0; structure_bsp_index < structure_bsp_count; structure_bsp_index++)
    {
        if (structure_bsp_is_active(structure_bsp_index))
        {
            pathfinding_data *data = pathfinding_data_get(structure_bsp_index);
            if (data && data != exclude_data)
            {
                for (short seam = 0; seam < data->seams.count; seam++)
                {
                    s_jump_seam *jump_seam = (s_jump_seam *)tag_block_get_element_with_size(&data->seams, seam,
                        sizeof(s_jump_seam));
                    if (jump_seam->seam_index == seam_index)
                    {
                        if (jump_seam->jump_index_count > 0)
                        {
                            const s_jump_seam_jump_index *first = (const s_jump_seam_jump_index *)tag_block_get_element_with_size(
                                &jump_seam->jumps, 0, sizeof(s_jump_seam_jump_index));
                            if (first->jump_index >= 0 && first->jump_index < data->hints.count)
                            {
                                jump_index = first->jump_index;
                                if (jump_seam->jump_index_count > 1)
                                {
                                    const hint_jump_data *hint = &((const pathfinding_hint *)tag_block_get_element_with_size(
                                                                              &data->hints, first->jump_index,
                                                                                  sizeof(pathfinding_hint)))
                                                                             ->jump;
                                    if (!(hint->flags & 2))
                                    {
                                    real_vector3d direction;
                                    const real_point3d *destination;
                                    const real_point3d *origin;
                                    origin = TAG_BLOCK_GET_ELEMENT(&data->vertices, hint->jump_start_vertex, real_point3d);
                                    destination = TAG_BLOCK_GET_ELEMENT(&data->vertices, hint->jump_end_vertex, real_point3d);
                                    vector_from_points3d(origin, destination, &direction);
                                    if (normalize3d(&direction) > 0.0f)
                                    {
                                        float origin_distance = dot_product3d(origin, &direction);
                                        for (short i = 1; i < jump_seam->jump_index_count; i++)
                                        {
                                            short next = jump_seam->jump_indices[i].jump_index;
                                            if (next >= 0 && next < data->hints.count)
                                            {
                                                const real_point3d *point = TAG_BLOCK_GET_ELEMENT(&data->vertices,
                                                    ((pathfinding_hint *)data->hints.address)[next].jump.jump_start_vertex, real_point3d);
                                                if ((dot_product3d(point, &direction) - origin_distance) / jump_seam->length >= distance)
                                                    break;
                                                jump_index = next;
                                            }
                                        }
                                    }
                                }
                                    }
                                if (out_structure_bsp_index)
                                    *out_structure_bsp_index = structure_bsp_index;
                            }
                        }
                        break;
                    }
                }
                if (jump_index != -1)
                    break;
            }
        }
    }
    return jump_index;
}

static __forceinline float ai_hint_definitions_jump_velocity(short type)
{
  float velocity = 0.0f;
  if (type >= 0 && type < 6 && global_game_globals != 0 && global_game_globals->ai_globals.count() > 0)
    velocity = TAG_BLOCK_GET_ELEMENT(&global_game_globals->ai_globals, 0, s_ai_globals_definition)->jump_velocities[type];
  return velocity;
}

static __forceinline float ai_hint_definitions_max_jump_down_height(short type)
{
  float height = 0.0f;
  if (type >= 0 && type < 6 && global_game_globals != 0 && global_game_globals->ai_globals.count() > 0)
    height = TAG_BLOCK_GET_ELEMENT(&global_game_globals->ai_globals, 0, s_ai_globals_definition)->max_jump_down_heights[type];
  return height;
}


// @address 0x528e58 ?ai_hint_test_jump@@YA_NPATreal_point3d@@000F@Z
bool ai_hint_test_jump(union real_point3d * start, union real_point3d * start2, union real_point3d * end,
    union real_point3d * end2, short type)
{
  // match: 0x528e58: value range assumption guides branch lowering
  __assume(end2 == 0);
  bool result = true;
  float jump_velocity = ai_hint_definitions_jump_velocity(type);
  if (start->z > end->z || (start2 && end2 && start2->z > end2->z))
  {
    float max_down_height = ai_hint_definitions_max_jump_down_height(type);
    float down_height = start->z - end->z;
    if (start2 && end2)
      down_height = down_height > start2->z - end2->z ? down_height : start2->z - end2->z;
    result = !(max_down_height != 0.0f && down_height >= max_down_height);
  }
  if (result)
  {
    real_vector3d aim;
    result = projectile_aim_ballistic(jump_velocity, 1.0f, start, end, 0, 0, 0, true, &aim, 0, 0, 0, 0, 0);
    if (result && start2 && end2)
      result = projectile_aim_ballistic(jump_velocity, 1.0f, start2, end2, 0, 0, 0, true, &aim, 0, 0, 0, 0, 0);
  }
  return result;
}

// @address 0x529f10 ?ai_hint_find_closest_point@@YA_NPAUobject_marker@@PBTreal_point3d@@MPAT2@@Z
bool ai_hint_find_closest_point(object_marker *marker, const real_point3d *point, float radius, real_point3d *closest_point)
{
    if (marker->length > 0.0f)
    {
        real_vector3d vector;
        vector_from_points3d(&marker->matrix.position, point, &vector);
        float t = dot_product3d(&vector, &marker->matrix.up);
        t = PIN(t, 0.0f, marker->length);
        if (marker->length > 2.0f * radius)
            t = PIN(t, radius, marker->length - radius);
        else
            t = PIN(t, 0.0f, marker->length);
        point_from_line3d(&marker->matrix.position, &marker->matrix.up, t, closest_point);
    }
    else
    {
        *closest_point = marker->matrix.position;
    }
    return true;
}

// @address 0x529d40 ai_hint_find_hoist_marker
static __forceinline bool ai_hint_definitions_marker_valid(object_marker *marker)
{
    bool valid = false;
    const real_vector3d up = { 0.0f, 0.0f, 1.0f };
    if (dot_product3d(&marker->matrix.forward, &up) > 0.95f)
        valid = true;
    return valid;
}

extern "C" bool ai_hint_find_hoist_marker(
    // Without it the final *point_ref copy stays in load/store order with y in r10.
    // match: 0x529d40: __restrict on point_ref hoists the copy's y/z loads above the stores (y in r9, z reuses r11).
    long actor_index, long object_index, float radius, object_marker * marker_ref, real_point3d *__restrict point_ref)
{
    bool success = false;
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    character_movement_properties *movement = character_definition_movement_properties_get(actor->character_index);
    if (movement)
    {
        object_marker markers[32];
        short marker_count = 0;
        float best_distance = FLT_MAX;
        short best_marker = -1;
        marker_count = ai_hint_get_hoist_markers(object_index, markers, 32, (short)movement->hoist_flags);
        for (short marker_index = 0; marker_index < marker_count; marker_index++)
        {
            if (ai_hint_definitions_marker_valid(&markers[marker_index]))
            {
                real_vector3d vector;
                scale_vector3d(&markers[marker_index].matrix.up, markers[marker_index].length, &vector);
                float distance = point_to_line_distance_squared3d(&actor->position.body,
                    &markers[marker_index].matrix.position, &vector);
                if (distance < best_distance)
                {
                    best_distance = distance;
                    best_marker = marker_index;
                }
            }
        }
        if (best_marker != -1)
        {
            memcpy(marker_ref, &markers[best_marker], sizeof(object_marker));
            if (point_ref && !ai_hint_find_closest_point(marker_ref, &actor->position.body, radius, point_ref))
                *point_ref = marker_ref->matrix.position;
            success = true;
        }
    }
    return success;
}

// ---------------------------------------------------------------------------
// added from the records: 0xcdf30


// @address 0x528c78 ?pathfinding_find_jump_seam@@YAJPAUpathfinding_data@@PAUhint_jump_data@@PBTreal_point3d@@PAF@Z
long pathfinding_find_jump_seam(pathfinding_data *data, hint_jump_data *jump, const real_point3d *point,
    short *out_structure_bsp_index)
{
    s_jump_seam *seam = (s_jump_seam *)tag_block_get_element_with_size(&data->seams, (unsigned short)jump->data[1],
        sizeof(s_jump_seam));
    float distance = 0.0f;
    if (point && seam->jump_index_count > 0 && seam->length > 0.0001f)
    {
        const real_point3d *origin = TAG_BLOCK_GET_ELEMENT(&data->vertices, jump->jump_start_vertex, real_point3d);
        const real_point3d *destination = TAG_BLOCK_GET_ELEMENT(&data->vertices, jump->jump_end_vertex, real_point3d);
        real_vector3d direction;
        vector_from_points3d(origin, destination, &direction);
        float length = normalize3d(&direction);
        if (length > 0.0f)
        {
            const s_jump_seam_jump_index *first = (const s_jump_seam_jump_index *)tag_block_get_element_with_size(
                &seam->jumps, 0, sizeof(s_jump_seam_jump_index));
            if (first->jump_index >= 0 && first->jump_index < data->hints.count)
            {
                const pathfinding_hint *volume = (const pathfinding_hint *)tag_block_get_element_with_size(&data->hints,
                    first->jump_index, sizeof(pathfinding_hint));
                const real_point3d *hint_point = TAG_BLOCK_GET_ELEMENT(&data->vertices, volume->jump.jump_start_vertex, real_point3d);
                float closest = distance3d(hint_point, origin);
                real_vector3d relative;
                vector_from_points3d(hint_point, point, &relative);
                float along = dot_product3d(&relative, &direction);
                float maximum = closest + length;
                along = PIN(along, closest, maximum);
                distance = along / seam->length;
            }
        }
    }
    return pathfinding_find_jump_seam(data, seam->seam_index, distance, out_structure_bsp_index);
}

// @address 0xcdf30 ai_hint_get_jump_destination_rail
unsigned char ai_hint_get_jump_destination_rail(short bsp, hint_jump_data *jump, const real_point3d *point,
    // match: 0xcdf30: pointer __restrict qualifier prevents alias reload
    real_point3d *__restrict start, real_point3d *__restrict end, c_sector_ref *sector, short *hint)
{
    bool result = false;
    pathfinding_data *data = pathfinding_data_get(bsp);
    long flags = jump->data[2];
    if (flags & 4)
    {
        short destination_bsp = -1;
        long seam = pathfinding_find_jump_seam((pathfinding_data *)data, jump, point, &destination_bsp);
        if (destination_bsp != -1)
        {
            pathfinding_data *destination_data = pathfinding_data_get(destination_bsp);
            hint_jump_data *destination = &((pathfinding_hint *)destination_data->hints.address)[seam].jump;
            if (sector)
                sector->set(destination_bsp, (unsigned short)(destination->data[2] >> 16));
            if (hint)
                *hint = (short)destination->data[3];
            if (start)
                *start = *TAG_BLOCK_GET_ELEMENT(&destination_data->vertices, (unsigned short)destination->data[0], real_point3d);
            if (end)
            {
                if ((destination->data[2] & 2) || (unsigned short)(destination->data[0] >> 16) == 0xffff)
                    *end = *TAG_BLOCK_GET_ELEMENT(&destination_data->vertices, (unsigned short)destination->data[0], real_point3d);
                else
                    *end = *TAG_BLOCK_GET_ELEMENT(&destination_data->vertices, (unsigned short)(destination->data[0] >> 16), real_point3d);
            }
            result = 1;
        }
    }
    else
    {
        if (sector)
            sector->set(bsp, jump->sector_index);
        if (hint)
            *hint = (short)(jump->data[3] >> 16);
        if (start)
            *start = *TAG_BLOCK_GET_ELEMENT(&data->vertices, (unsigned short)jump->data[1], real_point3d);
        if (end)
        {
            if ((flags & 2) || (unsigned short)(jump->data[1] >> 16) == 0xffff)
                *end = *TAG_BLOCK_GET_ELEMENT(&data->vertices, (unsigned short)jump->data[1], real_point3d);
            else
                *end = *TAG_BLOCK_GET_ELEMENT(&data->vertices, (unsigned short)(jump->data[1] >> 16), real_point3d);
        }
        result = 1;
    }
    return result;
}

// ---------------------------------------------------------------------------
// added from the records: 0x528f90

// @address 0x528f90 ?ai_hint_get_best_hoist_height@@YA?AW4e_action_hoist@@JM@Z
enum e_action_hoist ai_hint_get_best_hoist_height(long object_index, float height)
{
    long result = 0;
    long allowed = 0xffff;
    unit_datum *object =
        (unit_datum *)object_get_and_verify_type(object_index, NONE);
    if (((1 << object->object.type) & 0x1003) != 0)
    {
        if (object->unit.actor_index != -1)
        {
            actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), object->unit.actor_index);
            character_movement_properties *movement =
                character_definition_movement_properties_get(actor->character_index);
            if (movement)
                allowed = movement->hoist_flags >> 10;
        }
    }
    short best = -1;
    s_game_globals *globals = global_game_globals;
    if (globals && globals->ai_globals.count() > 0)
    {
        s_ai_globals_definition *ai_globals = TAG_BLOCK_GET_ELEMENT(&globals->ai_globals, 0, s_ai_globals_definition);
        float best_distance = 3.402823466e+38F;
        for (short i = 0; i < 3; i++)
        {
            if ((allowed & (1 << i)) != 0)
            {
                float midpoint = (ai_globals->hoist_heights[i].upper + ai_globals->hoist_heights[i].lower) * 0.5f;
                float distance = fabsf(midpoint - height);
                if (distance < best_distance)
                {
                    best_distance = distance;
                    best = i;
                }
            }
        }
    }
    switch (best)
    {
    case 1: result = 1; break;
    case 2: result = 2; break;
    }
    return (e_action_hoist)result;
}

// ---------------------------------------------------------------------------
// added from the records: 0x529b80

static __forceinline bool ai_hint_definitions_mount_marker_valid(const real_vector3d *up)
{
    bool result = false;
    if ((up->i + up->j) * 0.0f + up->k > 0.95f)
        result = true;
    return result;
}

static __forceinline float ai_hint_definitions_vault_score(const real_vector3d *direction,
    const real_vector3d *up, float length, float three, float two, float one, const float &direction_i)
{
    float dot = (direction->j * up->j + direction->k * up->k) + direction_i * up->i;
    float scaled = fabs(dot) * two;
    float denominator = scaled + one;
    float factor = three / denominator;
    float score = length * factor;
    return score;
}

static __forceinline bool ai_hint_definitions_vault_marker_valid(const real_vector3d *forward,
    float zero, float threshold)
{
    bool valid = false;
    if ((forward->j + forward->i) * zero + forward->k > threshold)
        valid = true;
    return valid;
}

// @address 0x529b80 ai_hint_find_mount_marker
unsigned char ai_hint_find_mount_marker(long actor_index, long object_index, float radius,
    // match: 0x529b80: pointer __restrict qualifier prevents alias reload
    object_marker *marker_ref, real_point3d *__restrict point)
{
    bool result = false;
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    character_movement_properties *movement = character_definition_movement_properties_get(actor->character_index);
    if (movement)
    {
        object_marker markers[32];
        float best_distance = FLT_MAX;
        short best_marker = -1;
        short marker_count = ai_hint_get_mount_markers(object_index, markers, 32, (short)movement->hoist_flags);
        for (short marker_index = 0; marker_index < marker_count; marker_index++)
        {
            if (ai_hint_definitions_mount_marker_valid(&markers[marker_index].matrix.forward))
            {
                float distance = distance_squared3d(&markers[marker_index].matrix.position,
                    &actor->position.body);
                if (distance < best_distance)
                {
                    best_distance = distance;
                    best_marker = marker_index;
                }
            }
        }
        if (best_marker != -1)
        {
            memcpy(marker_ref, &markers[best_marker], sizeof(object_marker));
            if (point && !ai_hint_find_closest_point(marker_ref, &actor->position.body, radius, point))
                *point = marker_ref->matrix.position;
            result = 1;
        }
    }
    return result;
}

// @address 0x529978 ?ai_hint_find_vault_marker@@YA_NJJMPAUobject_marker@@PATreal_point3d@@@Z
bool ai_hint_find_vault_marker(long actor_index, long object_index, float radius,
    object_marker *marker_ref, real_point3d *point_ref)
{
    bool result = false;
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    character_movement_properties *movement =
        character_definition_movement_properties_get(actor->character_index);
    if (movement)
    {
        object_marker markers[32];
        real_point3d closest_point;
        real_point3d best_point;
        float best_score = k_vault_initial_score;
        short best_marker = -1;
        short marker_count = ai_hint_get_vault_markers(object_index, markers, 32,
            (short)movement->hoist_flags);
        short previous_marker_index = 0;
        if (marker_count > 0)
        {
            long marker_index = 0;
            float zero = k_vault_score_zero;
            float one = k_vault_score_one;
            float threshold = k_vault_marker_vertical_threshold;
            float two = k_vault_score_scale;
            float three = k_vault_score_numerator;
            do
            {
                real_vector3d *const &forward = &markers[marker_index].matrix.forward;
                if (ai_hint_definitions_vault_marker_valid(forward, zero, threshold))
                {
                    ai_hint_find_closest_point(&markers[marker_index], &actor->position.body, radius, &closest_point);
                    real_vector3d direction;
                    vector_from_points3d(&closest_point, &actor->position.body, &direction);
                    float length = normalize3d(&direction);
                    real_vector3d *const &left = &markers[marker_index].matrix.left;
                    const float &direction_i = +direction.i;
                    float score = ai_hint_definitions_vault_score(&direction, left,
                        length, three, two, one, direction_i);
                    if (!(score >= best_score))
                    {
                        best_score = score;
                        best_marker = previous_marker_index;
                        best_point = closest_point;
                    }
                }
                previous_marker_index = (short)(marker_index + 1);
                marker_index = previous_marker_index;
            } while (marker_index < marker_count);
        }
        if (best_marker != -1)
        {
            memcpy(marker_ref, &markers[best_marker], sizeof(object_marker));
            if (point_ref)
            {
                *point_ref = best_point;
            }
            result = true;
        }
    }
    return result;
}


// @address 0x52a068 ?ai_hint_find_root_entry_matrix@@YA_NJJJPBTreal_point3d@@PBTreal_vector3d@@PAUreal_matrix4x3@@@Z
bool ai_hint_find_root_entry_matrix(
    long unit_index, long posture, long hint_type, const real_point3d *point, const real_vector3d *vector, real_matrix4x3 *matrix)
{
    bool result = false;
    real_matrix4x3 root_matrix;
    real_matrix4x3 entry_matrix;
    if (unit_animation_build_root_matrix_by_seat_and_state(unit_index, posture, hint_type, false, 3, &root_matrix))
    {
        matrix4x3_rotation_from_vectors(&entry_matrix, vector, global_up3d);
        entry_matrix.position = *point;
        matrix4x3_compute_product(&entry_matrix, &root_matrix, matrix);
        result = true;
    }
    return result;
}

// @address 0x52a0f8 ?ai_hint_find_pill_entry_point@@YA_NJJJJJPBTreal_point3d@@PBTreal_vector3d@@PAT1@PAVc_ai_point3d@@PAT2@PAVc_sector_ref@@@Z
bool ai_hint_find_pill_entry_point(long actor_index, long posture, long hint_type, long seat, long state,
    const real_point3d *point,
    const real_vector3d *vector, real_point3d *point2, c_ai_point3d *ai_point, real_vector3d *vector2, c_sector_ref *sector)
{
    bool result = false;
    real_matrix4x3 root_matrix;
    real_matrix4x3 inverse_matrix;
    real_matrix4x3 pill_matrix;
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    if (ai_hint_find_root_entry_matrix(actor->unit_index, posture, hint_type, point, vector, &root_matrix)
        && unit_animation_build_root_matrix_by_seat_and_state(actor->unit_index, 0x1dc, 12, false, 3, &pill_matrix))
    {
        matrix4x3_inverse(&pill_matrix, &inverse_matrix);
        matrix4x3_compute_product(&root_matrix, &inverse_matrix, &pill_matrix);
        if (point2)
            *point2 = pill_matrix.position;
        if (vector2)
            *vector2 = pill_matrix.forward;
        result = true;
    }
    return result;
}


static __forceinline c_sector_ref ai_hint_definitions_sector_from_point(const real_point3d *point, c_ai_point3d *ai_point)
{
    collision_result collision;
    return pathfinding_find_ground_sector(point, global_up3d, ai_point, &collision, 0.5f);
}

// @address 0x52a1e8 ?ai_hint_find_root_entry_point@@YA_NJJJPBTreal_point3d@@PBTreal_vector3d@@PAT1@PAVc_ai_point3d@@PAT2@PAVc_sector_ref@@@Z
bool ai_hint_find_root_entry_point(long actor_index, long posture, long hint_type, const real_point3d *point,
    const real_vector3d *vector,
    real_point3d *point2, c_ai_point3d *ai_point, real_vector3d *vector2, c_sector_ref *sector)
{
    bool success = false;
    if (actor_index != -1)
    {
        actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
        real_matrix4x3 root_matrix;
        if (ai_hint_find_root_entry_matrix(actor->unit_index, posture, 84, point, vector, &root_matrix))
        {
            if (point2)
                *point2 = root_matrix.position;
            if (ai_point || sector)
            {
                c_sector_ref sector_ref = ai_hint_definitions_sector_from_point(&root_matrix.position, ai_point);
                if (sector)
                    *sector = sector_ref;
            }
            success = true;
        }
    }
    else
    {
        if (point2)
            *point2 = *point;
        if (sector || ai_point)
        {
            c_sector_ref sector_ref = ai_hint_definitions_sector_from_point(point, ai_point);
            if (sector)
                *sector = sector_ref;
        }
        success = true;
    }
    return success;
}

static __forceinline float ai_hint_definitions_rail_dot(const real_vector3d *direction, const real_point3d *point)
{
    return direction->i * point->x + (direction->k * point->z + direction->j * point->y);
}

// @address 0x5291e0 ?ai_hint_get_jump_maneuver_point@@YA_NPAUpathfinding_data@@PAUpathfinding_hint_data@@PBVc_ai_point3d@@Vc_sector_ref@@MPAV3@PAV4@45@Z
bool ai_hint_get_jump_maneuver_point(pathfinding_data *data, pathfinding_hint_data *hint,
    const c_ai_point3d *point, c_sector_ref sector, float radius,
    c_ai_point3d *start_point, c_sector_ref *start_sector,
    c_ai_point3d *end_point, c_sector_ref *end_sector)
{
    bool result = false;
    if (point)
    {
        hint_jump_data *jump = &hint->jump;
        if (hint->jump.vertex1 != -1 && (long)hint->jump.vertex3 != -1)
        {
            real_point3d destination_start;
            real_point3d destination_end;
            c_sector_ref destination_sector;
            short destination_reference_frame;
            if (ai_hint_get_jump_destination_rail(sector.structure_bsp_index, jump, point->get_point(),
                &destination_start, &destination_end, &destination_sector, &destination_reference_frame))
            {
                const real_point3d *start = TAG_BLOCK_GET_ELEMENT(&((pathfinding_data *)data)->vertices, hint->jump.vertex1, real_point3d);
                if (!(hint->jump.flags & 2))
                {
                    const real_point3d *end = TAG_BLOCK_GET_ELEMENT(&((pathfinding_data *)data)->vertices, jump->vertex0, real_point3d);
                    real_vector3d direction;
                    vector_from_points3d(start, end, &direction);
                    float length = normalize3d(&direction);
                    if (length > 0.0f)
                    {
                        real_vector3d destination_direction;
                        vector_from_points3d(&destination_start, &destination_end, &destination_direction);
                        float destination_length = normalize3d(&destination_direction);
                        if (destination_length > 0.0f)
                        {
                            float inverse_length = 1.0f / length;
                            float t = (ai_hint_definitions_rail_dot(&direction, point->get_point())
                                - ai_hint_definitions_rail_dot(&direction, start)) * inverse_length;
                            if (length > 2.0f * radius)
                                t = PIN(t, inverse_length * radius, 1.0f - inverse_length * radius);
                            else
                                t = 0.5f;
                            real_point3d maneuver_point;
                            point_from_line3d(start, &direction, t * length, &maneuver_point);
                            start_point->set(&maneuver_point, point->reference_frame(), sector.structure_bsp_index);
                            point_from_line3d(&destination_start, &destination_direction, t * destination_length,
                                &maneuver_point);
                            end_point->set(&maneuver_point, destination_reference_frame, destination_sector.structure_bsp_index);
                            if (start_sector)
                                *start_sector = sector;
                            if (end_sector)
                                *end_sector = destination_sector;
                            result = true;
                        }
                    }
                }
                if (!result)
                {
                    start_point->set(start, point->reference_frame(), sector.structure_bsp_index);
                    end_point->set(&destination_start, destination_reference_frame, destination_sector.structure_bsp_index);
                    if (start_sector)
                        *start_sector = sector;
                    if (end_sector)
                        *end_sector = destination_sector;
                    result = true;
                }
            }
        }
    }
    return result;
}

struct pathfinding_hint_data;
// @rdata 0x8202b10c
static const float k_ai_hint_maneuver_entry_distance = 0.15f;
// @rdata 0x82022db8
static const float k_ai_hint_maneuver_entry_height = 0.4f;
// @address 0x529508 ?ai_hint_get_maneuver_point@@YA_NJMJPBVc_ai_point3d@@Vc_sector_ref@@PAV1@PAV2@23@Z
bool ai_hint_get_maneuver_point(long actor_index, float radius, long hint_index,
    const c_ai_point3d *point, c_sector_ref sector,
    c_ai_point3d *entry_point, c_sector_ref *entry_sector,
    c_ai_point3d *exit_point, c_sector_ref *exit_sector)
{
    pathfinding_data *data = pathfinding_data_get(sector.structure_bsp_index);
    const pathfinding_hint *hint = (const pathfinding_hint *)volume_get(data, hint_index);
    bool result = false;
    switch (hint->type)
    {
    case 1:
    case 6:
        if (ai_hint_get_jump_maneuver_point(data, (pathfinding_hint_data *)hint, point, sector, radius,
            entry_point, entry_sector, exit_point, exit_sector))
        {
            result = true;
            if (TEST_BIT(hint->fields.flags, 5))
            {
                real_point3d start;
                real_point3d end;
                ai_point_get_position(entry_point, &start);
                ai_point_get_position(exit_point, &end);
                real_vector3d direction;
                vector_from_points3d(&start, &end, &direction);
                direction.k = 0.0f;
                if (normalize3d(&direction) > 0.0f)
                {
                    real_point3d maneuver_point;
                    point_from_line3d(&start, &direction, k_ai_hint_maneuver_entry_distance, &maneuver_point);
                    point_from_line3d(&maneuver_point, global_up3d, k_ai_hint_maneuver_entry_height, &maneuver_point);
                    c_ai_point3d maneuver_ai_point;
                    c_sector_ref maneuver_sector;
                    if (ai_hint_find_root_entry_point(actor_index, 0x117, 84, &maneuver_point, &direction, 0,
                        &maneuver_ai_point, 0, &maneuver_sector))
                    {
                        *entry_point = maneuver_ai_point;
                        *entry_sector = maneuver_sector;
                    }
                }
            }
        }
        break;
    case 5:
        {
            bool success = false;
            const real_point3d *anchor = TAG_BLOCK_GET_ELEMENT(&data->vertices, (unsigned short)hint->jump.data[1], real_point3d);
            real_vector3d direction;
            memcpy(&direction, TAG_BLOCK_GET_ELEMENT(&data->vertices, hint->fields.index2, real_point3d), sizeof(direction));
            scale_vector3d(&direction, -1.0f, &direction);
            if (actor_index != -1)
            {
                actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
                if (actor->unit_index != -1)
                {
                    long hoist = ai_hint_get_best_hoist_height(actor->unit_index,
                        anchor->z - TAG_BLOCK_GET_ELEMENT(&data->vertices, hint->fields.index0, real_point3d)->z);
                    long root_type = -1;
                    switch (hoist)
                    {
                    case 1:
                        root_type = 0x1cd;
                        break;
                    case 2:
                        root_type = 0x1ce;
                        break;
                    }
                    if (root_type != -1)
                        success = ai_hint_find_root_entry_point(actor_index, root_type, 84, anchor, &direction, 0,
                            entry_point, 0, entry_sector);
                    if (!success)
                        success = ai_hint_find_root_entry_point(actor_index, 0x1cc, 84, anchor, &direction, 0,
                            entry_point, 0, entry_sector);
                    if (success)
                    {
                        exit_point->set(anchor, point->reference_frame(), sector.structure_bsp_index);
                        exit_sector->set(sector.structure_bsp_index, (unsigned short)hint->jump.data[0]);
                    }
                }
            }
            if (!success)
            {
                *entry_point = *point;
                *entry_sector = sector;
                exit_point->set(anchor, point->reference_frame(), sector.structure_bsp_index);
                exit_sector->set(sector.structure_bsp_index, (unsigned short)hint->jump.data[0]);
            }
            result = true;
        }
        break;
    }
    return result;
}
