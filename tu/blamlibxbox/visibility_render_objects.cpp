// blamlibxbox/visibility_render_objects.obj: from the records 0x1702a8 0x2ffbb8 0x2ffce0 0x2ffdc0 0x2ffec8 0x2fffc8
#include "game/players_globals.h"
#include "game/player_control_types.h"
#include "render/views/player_view_types.h"
#include "game/player_control_globals.h"
#include "objects/object_render_state_tls.h"
#include "tag_files/tag_types.h"
#include "cutscene/cinematic_globals_types.h"
#include <string.h>
#include "objects/objects.h"
#include "objects/object_material_types.h"
#include "units/units.h"
#include "cseries/cseries.h"
#include "models/model_skinning.h"
#include "render/render_objects_static_lighting.h"
#include "visibility/visibility_render_objects.h"
#include "render/render_visibility_collection.h"
#include "render/render_objects_static_lighting_setup_types.h"
#include "cache/cache_files.h"
#include "models/model_skinning_types.h"
#include "models/render_model_definitions.h"
#include "models/model_types.h"
#include "models/model_variant_types.h"
#include "cache/render_model_region_types.h"
#include "render/shader_extern_info_types.h"
#include "render/render_visibility_cached_state_types.h"
#include "game/player_mapping_types.h"
#include "game/player_types.h"
#include "memory/tls_globals.h"
#include "visibility/visibility_render_objects_globals.h"
#include "objects/object_datum.h"
#include "memory/data_types.h"
#include "render/render_object_info.h"
#include "render/render_lightmap_shadows_globals.h"
#include "camera/saved_film_director_storage.h"
#include "math/matrix_math.h"
#include "render/render_first_person_globals_types.h"
#include "visibility/visible_items_types.h"
#include "render/render_lighting_types.h"
#include "visibility/visible_items.h"
#include "render/object_renderer.h"
#include "memory/data.h"
#include "cseries/bit_test.h"
#include "tag_files/tag_groups.h"
#if !defined(_XBOX) || defined(__clang__)
#include "../../port/sdk/runtime/port_config.h"
#endif
struct s_cluster_reference;
// @flags /TP /O2 /Oc
// @address 0x2ffec8 ?visibility_render_objects_geometry_in_cache@@YA_NJJPBDPADJ_NPAEPA_N@Z

struct object_region_information;

struct s_user_globals;

// @address 0x8368d0 g_player_render_tuning
static void *visibility_render_objects_reset_memory_get(long handle);
// @address 0x1702a8 visibility_render_objects_add_first_person
extern "C" void visibility_render_objects_add_first_person(long p0, long p1, bool /*p2*/, bool /*p3*/)
{
    s_render_object_first_person_globals *globals = &render_object_globals;
    void *first_person = object_try_and_get_and_verify_type(globals->object_index, (unsigned long)-1);
    bool ready = first_person != 0 && globals->model_count > 0;
    if (!ready)
        return;
    bool ok = true;
    long hierarchy_count = c_visible_items::m_items.hierarchies.count;
    long saved_object_count = c_visible_items::m_items.objects.count;
    if ((unsigned long)hierarchy_count < c_visible_items::m_items.hierarchies.unknown) {
        c_visible_items::m_items.hierarchies.count = (unsigned short)(hierarchy_count + 1);
        if (hierarchy_count != -1) {
            s_object_hierarchy *hierarchy = &c_visible_items::m_items.hierarchies.entries[hierarchy_count];
            long added = 0;
            hierarchy->flags |= 15;
            hierarchy->region_cluster_bitvector_index = -1;
            for (long processed = 0; processed < globals->model_count; processed++) {
                render_first_person_model *instance = &globals->models[processed];
                {
                    render_model_definition *definition = (render_model_definition *)tag_get(0, instance->render_model_index);
                    if (processed < globals->model_count - 1)
                        c_object_renderer::submit_object_attachments(p0, instance->object_index);
                    if (definition->render_geometry.meshes.count > 0) {
                        unsigned long item_index = c_visible_items::m_items.objects.count;
                        if (item_index < c_visible_items::m_items.objects.unknown)
                            c_visible_items::m_items.objects.count = (unsigned short)(item_index + 1);
                        else
#if !defined(_XBOX) || defined(__clang__)
                        {
                            ok = false;
                            break;
                        }
#else
                            item_index = 0xffffffff;
#endif
                        s_render_objects_static_lighting_setup *item = &c_visible_items::m_items.objects.entries[item_index].render_setup;
                        bool model_uses_lighting = false;
                        // match: 0x1702a8: shared datum access changes register allocation at the visible-item allocation.
                        object_datum *object = (object_datum *)((s_object_header *)object_header_data->data)[(unsigned short)instance->object_index].datum;
                        long model_index = ((const s_object_material_definition *)tag_get(0, object->definition_index))->model.index;
                        if (model_index != -1)
                            model_uses_lighting = (((unsigned long)((const s_model_definition *)tag_get(0, 
                                                                         model_index))
                                                            ->flags >>
                                                        1) &
                                1);
                        render_object_initialize_setup(item);
                        long object_index = instance->object_index;
                        long definition_index = instance->render_model_index;
                        item->object_index = object_index;
                        item->definition_index = definition_index;
                        item->region_count = definition->regions.count;
                        item->flags |= 1;
                        if (object->first_widget_index != -1)
                            item->flags |= 2;
                        else
                            item->flags &= ~2;
                        if (model_uses_lighting)
                            item->flags |= 4;
                        else
                            item->flags &= ~4;
                        item->flags = (item->flags & ~0x18) | 0x10;
                        if (processed < globals->model_count - 1)
                            item->flags |= 32;
                        else
                            item->flags &= ~32;
#if defined(_WIN64) || defined(__LP64__)
                        void *regions = definition->regions.address;
#else
                        void *&regions = definition->regions.address;
#endif
                        for (long region_index = 0; region_index < definition->regions.count; region_index++) {
                            BLAM_ASSUME(region_index >= 0);
                            item->region_permutations[region_index] = (char)((const s_render_model_region *)regions)[region_index].permutations->start_index;
                        }
                        long level_of_detail = (short)item->level_of_detail;
                        item->skinning_matrix_count = (unsigned char)render_model_get_skinning_matrix_count(
                            definition_index, level_of_detail, (unsigned char *)item->region_permutations);
                        long skinning_size = 0;
                        long matrix_count = item->skinning_matrix_count;
                        if (matrix_count > 0)
                            skinning_size = ((matrix_count + (matrix_count << 1)) << 4) + 68;
                        long skinning_memory = render_visibility_allocate_reset_memory(0, 1, skinning_size);
                        item->skinning_memory_designator = skinning_memory;
                        if (skinning_memory != -1) {
                            s_model_skinning *skinning = (s_model_skinning *)visibility_render_objects_reset_memory_get(skinning_memory);
                            render_model_build_skinning(object_index, definition_index, instance->nodes, (unsigned char *)item->region_permutations,
                                level_of_detail, true, (long)item->skinning_matrix_count, skinning);
                        } else {
                            ok = false;
                        }
                        s_shader_extern_info *render_state = 0;
                        const long &cached_object_index = instance->object_index;
                        long cached_state = object_get_cached_render_state(cached_object_index);
                        if (cached_state != -1)
                            render_state = &datum_get<s_visibility_cached_object_render_state>(g_cached_object_render_states, cached_state)->shader_info;
#if !defined(_XBOX) || defined(__clang__)
                        else
                        {
                            ok = false;
                            break;
                        }
#endif
                        item->shader_extern_info = render_state;
                        if (render_state != 0) {
                            render_lighting *lighting = object_update_cached_render_lighting(p1, 10000.0f);
                            item->shader_extern_info->lighting = *lighting;
                        }
                        if (item->shader_extern_info != 0)
                            render_visibility_compute_object_state(item->object_index, (s_render_object_info *)item);
                        added++;
                    }
                }
            }
            hierarchy->object_count = (unsigned short)added;
            hierarchy->levels.values[0] = 255;
            hierarchy->levels.values[1] = 255;
            hierarchy->levels.values[2] = 255;
            hierarchy->first_object = c_visible_items::get_objects_starting_index();
        } else {
            ok = false;
        }
    } else {
        ok = false;
    }
    if (!ok) {
        c_visible_items::m_items.hierarchies.count = (unsigned short)hierarchy_count;
        c_visible_items::m_items.objects.count = saved_object_count;
    }
}
// @address 0x2ffce0 ?visibility_render_objects_get_region_zsort_offsets@@YAXJJPAUs_render_object_info@@@Z
void visibility_render_objects_get_region_zsort_offsets(long object_index, long unused, s_render_object_info *info)
{
    const object_datum *object = datum_get<s_object_header>(object_header_data, object_index)->datum;
    const s_object_material_definition *definition = (const s_object_material_definition *)tag_get(0, object->definition_index);
    const s_model_definition *model = (const s_model_definition *)tag_get(0, definition->model.index);
    if (object->variant_index != -1) {
        const s_model_variant *variant = &((const s_model_variant *)model->variants.address)[object->variant_index];
        for (long region_index = 0; region_index < model->regions.count; region_index++) {
            if (variant->region_indices[region_index] != -1)
                info->region_zsort[region_index] = ((const s_model_variant_region *)variant->regions.address)[variant->region_indices[region_index]].zsort;
            else
                info->region_zsort[region_index] = 0;
        }
    } else {
        memset(info->region_zsort, 0, sizeof(info->region_zsort));
    }
}

// @address 0x2ffdc0 ?visibility_render_objects_update_geometry_cache@@YA_NHIIPAFHPAEPAHPA_N@Z
bool visibility_render_objects_update_geometry_cache(int target_object_index, unsigned int render_model_index,
    unsigned int render_view_index, short *object_lod, int previous_lod_value, unsigned char *region_mask, int *region_total, bool *geometry_changed)
{
    char cached_lod_byte;
    const render_model_definition *geometry_model_definition;
    long cached_render_state_index;
    char *cached_permutations;
    char *region_permutations;
    char *region_zsort;

    geometry_model_definition = (const render_model_definition *)tag_get(0, render_model_index);
    ::object_get_region_information(target_object_index, (long *)(region_total), &region_permutations,
        &region_zsort, 0);
    if ((*object_lod != 1) && (*object_lod == 0)) {
        *region_total = 1;
    }
    cached_permutations = 0;
    cached_render_state_index = object_get_cached_render_state(target_object_index);
    if ((int)cached_render_state_index != -1) {
        cached_permutations = datum_get<s_visibility_cached_object_render_state>(
            g_cached_object_render_states, cached_render_state_index)->region_data[0];
    }
#if !defined(_XBOX) || defined(__clang__)
    else
    {
        *geometry_changed = true;
        return false;
    }
#endif
    visibility_render_objects_geometry_in_cache(render_model_index, *region_total, region_permutations,
        cached_permutations, *object_lod, 1, region_mask, geometry_changed);
    cached_lod_byte = *object_lod;
    if (3 >= render_view_index) {
        *render_visibility_get_cached_lod(target_object_index) = (char)cached_lod_byte;
    }
    return 0 < geometry_model_definition->render_geometry.meshes.count;
}

bool visibility_render_objects_geometry_in_cache(long index, long region_count, const char *permutations, char *permutations_out,
    long level_of_detail, bool first_person, unsigned char *values_out, bool *any_out)
{
    *any_out = false;
    const render_model_definition *model = (const render_model_definition *)global_tag_instances[(unsigned short)index].definition;
    unsigned short valid_mask = 0;
    for (long region_index = 0; region_index < region_count; region_index++) {
        long permutation_index = (unsigned char)permutations[region_index];
        if (permutation_index != -1 && permutation_index < ((const s_render_model_region *)model->regions.address)[region_index].permutation_count)
            valid_mask |= 1 << region_index;
    }
    for (long region_index = 0; region_index < region_count; region_index++) {
        if (valid_mask & (1 << region_index)) {
            const s_render_model_region *region = &((const s_render_model_region *)model->regions.address)[region_index];
            values_out[region_index] = (unsigned char)region->permutations[permutations[region_index]].start_index;
            permutations_out[region_index] = permutations[region_index];
        } else {
            values_out[region_index] = 255;
        }
    }
    return true;
}

// @address 0x2fffc8 visibility_render_objects_compute_lod
int visibility_render_objects_compute_lod(long object_index, int view_index, const s_model_fade_parameters *fade, float distance, signed char previous_lod)
{
    float scaled_distance = distance;
    int cinematic_in_progress;
    cinematic_in_progress = 0;
    s_cinematic_globals_definition *cinematic = tls_cinematic_globals();
    if (cinematic != 0) {
        cinematic_in_progress = cinematic->in_progress;
    }
    float lod_distance;
    if ((unsigned int)(cinematic_in_progress & 255) == 0) {
        if (tls_players_globals()->suppressed == 0) {
            lod_distance = g_player_render_tuning.lod_distance_scale;
            if (lod_distance > 0.0001f) {
                scaled_distance = distance / lod_distance;
            }
        }
    }
    float zoom_magnification;
    zoom_magnification = 1.0f;
    int current_view_index;
    current_view_index = c_player_view::get_current()->view_index;
    int unit_index;
    if (current_view_index != -1) {
        long player_index = tls_player_mapping_globals()->player_indices[current_view_index];
        if (player_index != -1) {
            player_datum *player = datum_get<player_datum>(player_data, player_index);
            if (player != 0) {
                unit_index = player->unit_index;
                if (unit_index != -1) {
                    unit_index = unit_get_aiming_unit_index(unit_index);
                    if (unit_index != -1) {
                        zoom_magnification = ::unit_get_zoom_magnification(unit_index,
                            (unsigned short)(player_control_globals)->states[current_view_index].action.zoom);
                    }
                }
            }
        }
    }
    float fade_distance = fade->shadow_fade_start_distance * zoom_magnification;
    int result = 1;
    if (fade->render_model_index != -1) {
        cinematic = tls_cinematic_globals();
        cinematic_in_progress = 0;
        if (cinematic != 0) {
            cinematic_in_progress = cinematic->in_progress;
        }
        if ((unsigned int)(cinematic_in_progress & 255) == 0) {
            lod_distance = scaled_distance;
            if (fade_distance != 0.0f) {
#if !defined(_XBOX) || defined(__clang__)
                // This tag field also controls shadows, so scale only the local LOD threshold.
                fade_distance *= port_config_draw_distance();
#endif
                if (previous_lod == 0) {
                    lod_distance = scaled_distance + 0.5f;
                } else if (previous_lod == 1) {
                    lod_distance = scaled_distance - 0.5f;
                }
                if (lod_distance > fade_distance) {
                    result = 0;
                } else {
                    result = 1;
                }
            }
        }
    }
    return result;
}


// @address 0x2ffbb8 visibility_render_objects_compute_lod_and_zsort
extern "C" bool visibility_render_objects_compute_lod_and_zsort(long object_index, float distance, long view_index,
    s_render_object_info *info, signed char previous_lod, signed char *lod)
{
    bool result = false;
    bool visible = false;
    if (!object_is_hidden(object_index))
        visible = true;
    if (visible)
    {
        const object_datum *object = (const object_datum *)datum_get<s_object_header>(object_header_data, object_index)->datum;
        const s_object_material_definition *definition = (const s_object_material_definition *)tag_get(0, object->definition_index);
        if (definition->model.index != -1)
        {
            const s_model_definition *model = (const s_model_definition *)tag_get(0, definition->model.index);
            if (!test_bit(object->flags, 19))
            {
                if (test_bit(object->damage_flags, 2))
                    distance *= 1.7f;
                signed char new_lod = (signed char)visibility_render_objects_compute_lod(object_index, view_index, &model->fade, distance, previous_lod);
                *lod = new_lod;
                result = new_lod != -1;
                *lod = PIN(new_lod, 0, 1);
            }
            visibility_render_objects_get_region_zsort_offsets(object_index, definition->model.index, info);
        }
    }
    return result;
}

static rasterizer_main_memory_globals rasterizer_memory_globals;
rasterizer_main_memory_globals *rasterizer_memory_get_globals()
{
    return &rasterizer_memory_globals;
}
static __forceinline void *visibility_render_objects_reset_memory_get(long handle)
{
    void *result = 0;
    if ((handle & (long)0x80000000) == 0)
        result = rasterizer_memory_globals.buffer + (handle & 0xfffffff);
    return result;
}

typedef char visibility_view_index_offset[(sizeof(void *) != 4 || ((offsetof(c_player_view, view_index) == 10688))) ? 1 : -1];
typedef char visibility_zoom_offset[(
    (offsetof(s_player_control_globals, states) + offsetof(s_player_control_state, action) +
        offsetof(control, zoom) == 1002)) ? 1 : -1];
