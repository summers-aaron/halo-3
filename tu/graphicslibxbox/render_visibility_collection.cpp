// graphicslibxbox/render_visibility_collection.obj: from the records 0x153f38 0x15a4e0 0x15a898 0x15a908 0x15abe8 0x15ace8 0x15add8 0x17d318
// @compiler xdk6130
#include "objects/object_render_state_tls.h"
#include "render/render_visibility_collection_imports.h"
#include "rasterizer/rasterizer_main_runtime_globals.h"
#include "visibility/visibility_globals.h"
#include "render/render.h"
// @address 0x7f4310 g_object_marker_globals
// @flags /TP /O2 /Oc
#include "cutscene/cinematic_new_globals_types.h"
#include "visibility/visible_items_types.h"
#include "visibility/visible_items.h"
#include "cseries/cseries.h"
#include "tag_files/tag_groups.h"
#include "multithreading/thread_tls.h"
#include "objects/object_header.h"
#include "units/unit_types.h"
#include "objects/object_definitions.h"
#include "models/model_types.h"
#include "models/render_model_definitions.h"
#include "items/equipment_definitions.h"
#include "objects/object_globals.h"
#include <string.h>
#include <stdlib.h>
#include "objects/object_reference_types.h"
#include "math/real_math_types.h"
#include "math/matrix_math_types.h"
#include "items/equipment.h"
#include "models/model_skinning.h"
#include "render/render_objects_static_lighting.h"
#include "render/render_visibility_collection.h"
#include "render/render_visibility_collection_globals.h"
#include "visibility/visibility_render_objects.h"
#include "visibility/visibility_render_objects_lod.h"
#include "render/render_visibility.h"
#include "visibility/visibility_projection_types.h"
#include "memory/data_types.h"
#include "visibility/visibility_collection_types.h"
#include "visibility/visibility_collection.h"
#include "render/render_objects_visible_hierarchy.h"
#include "objects/object_marker_globals_types.h"
#include "objects/scenery.h"
#include "render/render_visibility_cached_state_types.h"
#include "render/render_object_info.h"
#include "memory/data_typed_access.h"
#include "memory/datum_index.h"
#include "cseries/bit_operations.h"
struct render_camera;
struct render_projection;
struct s_cluster_reference;
struct visibility_projection;
// @address 0x149720 ?object_get_cached_render_state@@YAJJ@Z
struct render_camera;
struct render_projection;


struct s_data_array;

typedef char visibility_offset_check_157[(offsetof(unit_datum, unit.active_camouflage) == 0x358) ? 1 : -1];
typedef char visibility_offset_check_158[(offsetof(unit_datum, unit.equipment_object_index) == 0x280) ? 1 : -1];
typedef char visibility_offset_check_159[(offsetof(s_equipment_definition, equipment_types) + 7 * sizeof(s_tag_block) == 0x218) ? 1 : -1];
typedef char visibility_offset_check_160[(offsetof(render_model_definition, render_geometry.meshes) == 0x68) ? 1 : -1];

static __forceinline bool visibility_object_flag_test(unsigned long flags)
{
    return ((flags >> 15) & 1) != 0;
}


static const float k_visibility_zero = 0.0f;
static const float k_visibility_one = 1.0f;
static const float k_visibility_epsilon = 0.0001f;

static __forceinline float render_visibility_collection_saturate(float value)
{
    return MIN(MAX(value, k_visibility_zero), k_visibility_one);
}

// @address 0x113cec0 render_visibility_collection_globals
s_render_visibility_collection_globals render_visibility_collection_globals;

static __forceinline float render_visibility_collection_equipment_fraction(long equipment_index)
{
    float fraction = k_visibility_zero;
    object_datum *equipment = object_get(equipment_index);
    const s_equipment_definition *definition =
        TAG_GET('eqip', const s_equipment_definition, equipment->definition_index);
    if (definition->equipment_types[7].count != 0)
        fraction = equipment_active_fraction(equipment_index);
    return fraction;
}

static __forceinline bool render_visibility_collection_group_contains(const s_cinematics_new_group *group, long object_index)
{
    bool found = false;
    for (long index = 0; index < group->count; index++)
    {
        if (group->object_indices[index] == object_index)
        {
            found = true;
            break;
        }
    }
    return found;
}

// @address 0x153f38 render_visibility_camera_collection_compute
extern "C" void render_visibility_camera_collection_compute(
    const render_camera *camera, s_cluster_reference cluster, const render_projection *projection, long a, long b, bool flag)
{
    visibility_projection visibility;
    ::render_visibility_build_projection(
        camera, projection, cluster, &visibility);
    ((c_visibility_collection *)&g_visibility_globals.collection_input)
        ->compute(_collection_type_0, flag ? 256 : 0, &visibility, 1, &visibility.basis_to_world.position, 1024.0f, 0, cluster, a, b);
}

// @address 0x15a908 render_visibility_compute_object_state
extern "C" void render_visibility_compute_object_state(long object_index, s_render_object_info *info)
{
    object_header_datum *header = object_header_get(object_index);
    object_datum *object = OBJECT_GET(object_index, object_datum, _object_mask_all);
    s_object_definition *definition = TAG_GET('obje', s_object_definition, object->definition_index);
    if (info->shader_extern_info == 0)
        return;
    if (((1 << header->object_type) & 0x1003) != 0)
    {
        info->shader_extern_info->camouflage = (unsigned char)(
            PIN(((unit_datum *)object)->unit.active_camouflage, k_visibility_zero, k_visibility_one) * 255.0f);
        float shield_fraction = render_visibility_collection_saturate((object->shield_vitality - k_visibility_one) * 0.2f);
        info->shader_extern_info->shield = (unsigned char)(shield_fraction * 255.0f);
        float damage_fraction = render_visibility_collection_saturate(object->current_shield_damage);
        info->shader_extern_info->shield_damage = (unsigned char)(damage_fraction * 255.0f);
        const long &equipment_index = OBJECT_GET(object_index, unit_datum, _object_mask_unit)->unit.equipment_object_index;
        if (equipment_index != -1)
        {
            float active_fraction = render_visibility_collection_equipment_fraction(equipment_index);
            if (active_fraction > k_visibility_epsilon)
            {
                info->shader_extern_info->shield = MAX(info->shader_extern_info->shield, 127);
            }
        }
        info->shader_extern_info->override_material = 0;
    }
    else
    {
        info->shader_extern_info->camouflage = 0;
        info->shader_extern_info->shield = 0;
        info->shader_extern_info->shield_damage = 0;
        info->shader_extern_info->override_material = 0;
    }
    info->shader_extern_info->bounding_sphere.i = object->bounding_center.x;
    info->shader_extern_info->bounding_sphere.j = object->bounding_center.y;
    info->shader_extern_info->bounding_sphere.k = object->bounding_center.z;
    info->shader_extern_info->bounding_sphere.w = object->bounding_radius;
    if (visibility_object_flag_test(object->flags))
    {
        info->cinematic_clip_plane_mask = 0UL;
        long group_count = cinematic_new_globals->group_count;
        s_cinematics_new_group *group = &cinematic_new_globals->groups[0];
        for (long group_index = 0; group_index < group_count; ++group_index)
        {
            bool found = render_visibility_collection_group_contains(group, object_index);
            if (found)
                info->cinematic_clip_plane_mask |= 1UL << group_index;
            group++;
        }
    }
    else
    {
        info->cinematic_clip_plane_mask = 0UL;
    }
    long model_tag_index = definition->model.index;
    if (model_tag_index != -1)
    {
        s_model_definition *model_definition = TAG_GET('hlmt', s_model_definition, model_tag_index);
        if ((model_definition->flags & 1) != 0)
            info->shader_extern_info->camouflage = 255;
    }
}

// @address 0x15a898 render_visibility_get_cached_lod
extern "C" char *render_visibility_get_cached_lod(long object_index)
{
    char *result = 0;
    long index = object_get_cached_render_state(object_index);
    if (index != -1)
        result = &datum_get_typed<s_visibility_cached_object_render_state>(g_cached_object_render_states, index)->lod;
    return result;
}

static __forceinline real_matrix4x3 *render_visibility_collection_get_node_matrices(
    long object_index, long *count)
{
    object_datum *object = object_get(object_index);
    short offset = object->node_matrices.offset;
    *count = (unsigned long)object->node_matrices.size / sizeof(real_matrix4x3);
    return (real_matrix4x3 *)((char *)object + offset);
}

// @address 0x15abe8 render_visibility_get_render_model
extern "C" void render_visibility_get_render_model(long object_index, long mode, long *render_model_index,
    // match: 0x15abe8: pointer __restrict qualifier prevents alias reload
    real_matrix4x3 **node_matrices, long *__restrict node_count)
{
    object_datum *object = object_get(object_index);
    s_object_definition *definition =
        TAG_GET('obje', s_object_definition, object->definition_index);
    long model_index = definition->model.index;
    if (model_index != -1)
    {
        s_model_definition *model =
            TAG_GET('hlmt', s_model_definition, model_index);
        bool has_impostor = false;
        long selected_model_index = model->skinning_render_model_index;
        *node_matrices = render_visibility_collection_get_node_matrices(object_index, node_count);
        if (selected_model_index != -1)
            has_impostor = true;
        if ((signed char)mode != 1 && has_impostor)
        {
            *node_count = 1;
        }
        else
        {
            selected_model_index = model->render_model_index;
        }
        if (selected_model_index != -1)
        {
            render_model_definition *render_model =
                TAG_GET('mode', render_model_definition, selected_model_index);
            if (render_model->render_geometry.meshes.count > 0)
            {
                *render_model_index = selected_model_index;
                return;
            }
        }
    }
    *render_model_index = -1;
}

// @address 0x15ace8 render_visibility_get_shader_state
extern "C" char *render_visibility_get_shader_state(long object_index)
{
    char *result = 0;
    long index = object_get_cached_render_state(object_index);
    if (index != -1)
        result = reinterpret_cast<char *>(
            &datum_get_typed<s_visibility_cached_object_render_state>(g_cached_object_render_states, index)->shader_info);
    return result;
}

// match: 0x15add8: size-opts emit subf.+beq for the rem!=0 check instead of subf+cmpwi+beq
#pragma optimize("s", on)
// @address 0x15add8 render_visibility_allocate_reset_memory
extern "C" int render_visibility_allocate_reset_memory(int a, int b, int size)
{
    int result = -1;
    int aligned_size = size;
    int rem = size % 4;
    if (rem != 0)
    {
        aligned_size += 4 - rem;
    }

    rasterizer_main_memory_globals *memory = rasterizer_memory_get_globals();
    int current_offset = memory->current_offset;
    int new_offset = current_offset + aligned_size;
    if (new_offset <= 0x29000)
    {
        result = current_offset & 0x0fffffff;
        memory->current_offset = new_offset;
        if (new_offset > memory->high_watermark)
        {
            memory->high_watermark = new_offset;
        }
    }
    return result;
}
// match: 0x17d318: restore default opts; without it add_object_to_items uses clrlwi. and a different float const
#pragma optimize("", on)

struct s_render_object_info;
struct s_model_skinning;
struct render_lighting;

// @address 0x15a4e0 render_visibility_update_object
extern "C" bool render_visibility_update_object(long object_index, float distance, float lighting_distance,
                                     unsigned long view_index, s_render_object_info *info)
{
    signed char previous_lod = 1;
    signed char original_lod = -1;
    long previous_lod_argument;
    bool success;
    {
        info->shader_extern_info = 0;
        if (view_index <= 3)
#if !defined(_XBOX) || defined(__clang__)
        {
            const char *cached_lod = render_visibility_get_cached_lod(object_index);
            if (!cached_lod)
                return false;
            previous_lod = *cached_lod;
        }
#else
            previous_lod = *render_visibility_get_cached_lod(object_index);
#endif
        // match: 0x15a4e0: zero-bit rotate preserves 32-bit GPR value
        previous_lod_argument = _rotl(previous_lod, 0);
        success = visibility_render_objects_compute_lod_and_zsort(
            object_index, distance, view_index, info, previous_lod_argument, &original_lod);
    }
    if (success)
    {
        info->level_of_detail = (signed char)original_lod;
        long absolute_object_index = datum_index_to_absolute_index(object_index);
        short instance_index = -1;
        object_header_datum *header = object_header_get(object_index);
        if (header->object_type == 6)
            instance_index = (short)((s_scenery_object *)header->datum)->instance_index;
        info->scenery_instance_index = instance_index;
        real_matrix4x3 *node_matrices;
        long node_count;
        render_visibility_get_render_model(object_index, (signed char)info->level_of_detail,
            &info->definition_index, &node_matrices, &node_count);
        if (info->definition_index != -1)
        {
            info->shader_extern_info = (s_shader_extern_info *)render_visibility_get_shader_state(object_index);
#if !defined(_XBOX) || defined(__clang__)
            if (!info->shader_extern_info)
                return false;
#endif
            if (info->shader_extern_info)
                render_visibility_compute_object_state(object_index, info);

            bool reuse_skinning = previous_lod != -1 && info->level_of_detail == previous_lod && info->shader_extern_info &&
                info->shader_extern_info->skinning_offset != -1 &&
                info->shader_extern_info->frame_index == (c_render_globals::get_frame_index() & 0x3fffffff);
            bool changed;
            success = visibility_render_objects_update_geometry_cache(object_index, info->definition_index, view_index,
                                  &info->level_of_detail, previous_lod_argument,
                                  info->mesh_indices, (int *)&info->mesh_count, &changed);
            reuse_skinning &= !changed;
            bool lod_changed = ((signed char)original_lod - info->level_of_detail) != 0;
            if (reuse_skinning && lod_changed)
                reuse_skinning = false;
            if (success)
            {
                long matrix_count = render_model_get_skinning_matrix_count(info->definition_index, info->level_of_detail, info->mesh_indices);
                info->skinning_memory_designator = -1;
                info->skinning_matrix_count = (unsigned char)matrix_count;
                if (!reuse_skinning)
                {
                    long allocation_size = 0;
                    if (matrix_count > 0)
                        allocation_size = matrix_count * 48 + 68;
                    info->skinning_memory_designator = render_visibility_allocate_reset_memory(0, 1, allocation_size);
                    if (info->skinning_memory_designator != -1)
                    {
                        if (info->shader_extern_info && !changed && !lod_changed)
                        {
                            info->shader_extern_info->frame_index = c_render_globals::get_frame_index() & 0x3fffffff;
                            info->shader_extern_info->skinning_offset = info->skinning_memory_designator;
                        }
                        s_model_skinning *skinning = 0;
                        if (info->skinning_memory_designator != -1 && !(info->skinning_memory_designator & (long)0x80000000))
                            skinning = (s_model_skinning *)(rasterizer_memory_get_globals()->buffer + (info->skinning_memory_designator & 0x0fffffff));
                        render_model_build_skinning(object_index, info->definition_index, node_matrices,
                                                    info->mesh_indices, info->level_of_detail, false, matrix_count, skinning);
                    }
                    else
                        success = false;
                }
                else
                    info->skinning_memory_designator = info->shader_extern_info->skinning_offset;

                if (!reuse_skinning && success)
                {
                    if (info->shader_extern_info)
                    {
                        info->shader_extern_info->lighting =
                            *object_update_cached_render_lighting(object_index, lighting_distance);
                    }
                    render_object_update_change_colors(object_index, false);
                }
                if (success)
                {
                    long cache_index = g_object_render_data[absolute_object_index];
                    if (cache_index != -1)
                    {
                        s_visibility_cached_object_render_state *entry =
                            datum_get_typed<s_visibility_cached_object_render_state>(g_cached_object_render_states, cache_index);
                        if (entry->object_index == object_index)
                            entry->frame_index = c_render_globals::get_frame_index();
                    }
                }
            }
        }
        else
            success = false;
    }
    return success;
}

static __forceinline s_object_marker_globals *render_visibility_collection_get_object_marker_globals()
{
    return &g_object_marker_globals[get_current_thread_index() - 1];
}

static __forceinline void render_visibility_collection_object_marker_begin()
{
    s_object_marker_globals *globals = render_visibility_collection_get_object_marker_globals();
    globals->active = true;
    globals->mark++;
}

static __forceinline void render_visibility_collection_object_marker_end()
{
    render_visibility_collection_get_object_marker_globals()->active = false;
}

// @address 0x17d318
long render_visibility_add_object_to_items(long object_index, long view_index, bool lit, bool shadow)
{
    real_point3d center;
    float radius;
    object_datum *object = object_get(object_index);
    center = object->bounding_center;
    radius = object->bounding_radius;
    render_visibility_collection_object_marker_begin();

    render_objects_static_lighting_hierarchy_levels levels;
    levels.values[0] = 255;
    levels.values[1] = 255;
    levels.values[2] = shadow ? 255 : 0;
    levels.values[3] = 255;
    long hierarchy_index = render_objects_add_visible_hierarchy(object_index, &center, radius, view_index, lit, shadow, true, NONE,
        levels, false, false, NONE);
    if (hierarchy_index != NONE)
    {
        s_object_hierarchy *hierarchy = &c_visible_items::m_items.hierarchies.entries[hierarchy_index];
        hierarchy->region_cluster_bitvector_index = 0xffff;
        SET_BIT(hierarchy->flags, 3, true);
        SET_BIT(hierarchy->flags, 2, true);
        SET_BIT(hierarchy->flags, 0, lit);
        SET_BIT(hierarchy->flags, 1, shadow);
    }
    render_visibility_collection_object_marker_end();
    return hierarchy_index;
}
