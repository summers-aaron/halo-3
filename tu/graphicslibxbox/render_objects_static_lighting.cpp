// graphicslibxbox/render_objects_static_lighting.obj: from the records 0xd1468 0xf0608 0x149590 0x149720 0x149a00 0x149a20 0x159710 0x159b10 0x159f78 0x15a168 0x15a430 0x15b500 0x15be20 0x15beb0 0x15c608 0x15ca08 0x1634d0 0x16aa78 0x1706b0 0x17c5b8 0x23d820 0x23d9f8 0x44cbd0 0x44eb80 0x44eb98 0x44ec60 0x44f448 0x44f740 0x44f7d0 0x51e398 0x57f068 0x5804a0 0x5805b0 0x580628
// @address 0xd1468 ?arccosine@@YAMM@Z
// @address 0x11df70c g_director_globals
// @address 0x7f4310 g_object_marker_globals
// @address 0x8c46dc ?x_current_player_view@c_player_view@@1PAV1@A
// @address 0x9e7ea0 g_visibility_globals
// @address 0x11df7dc g_static_lighting_effect_attachments
// @address 0x11df878 g_object_message_queue
// @address 0x11df8c0 g_cinematic_light_globals
// @compiler xdk6130
// @address 0x11df710 g_cached_object_render_states
// @flags /TP /O2 /Oc
// @address 0xf0608 ?interpolate_scalar@@YAXPAMMM@Z
// @address 0x149720 ?object_get_cached_render_state@@YAJJ@Z
// @address 0x15a430 ?render_object_is_first_person@@YA_NJJ@Z
// @address 0x15be20 ?interpolate_value_perceptual@@YAMMMM@Z
// @address 0x15beb0 ?render_object_update_change_colors@@YAXJ_N@Z
// @address 0x1706b0 render_object_initialize_setup
// @address 0x17c5b8 object_get_cached_render_lighting
// @address 0x23d820 render_static_lighting_accumulate_sample
// @address 0x44cbd0 xmmatrix_determinant
// @address 0x44eb80 render_objects_set_cached_base_address
// @address 0x44f740 ?render_object_invalidate_render_state@@YAXJ@Z
// @address 0x321420 ?add_message@?$t_message_queue@Us_object_render_thread_message@@$0IAA@@@QAAXPBUs_object_render_thread_message@@@Z
// @address 0x1ec830 ?data_set_new_base_address@@YAXPAPAUs_data_array@@PAU1@@Z
// @address 0xaae70 ?datum_new@@YAJPAUs_data_array@@@Z
// @address 0xac748 ?object_try_and_get_and_verify_type@@YAPAXJK@Z
// @address 0x3174d0 ?reset_object_cached_render_state@@YAXJ@Z

#include "render/render_static_lighting_storage.h"
#include "camera/saved_film_director_storage.h"
#include "objects/cinematic_light_globals.h"
#include "main/main_loop_globals.h"
#include "game/players_globals_types.h"
#include "objects/object_render_state_tls.h"
#include "math/real_constant_pool.h"
#include "render/render_transparency_globals.h"
#include "visibility/visible_items_types.h"
#include "visibility/visible_items.h"
#include "render/render_objects_globals.h"
#include "ai/ai_dialogue_globals.h"
#include "camera/camera_fx_settings.h"
#include "objects/object_reference_types.h"
#include "math/real_math_globals.h"
#include "math/random_math.h"
#include "math/real_math.h"
#include "math/matrix_math_types.h"
#include "memory/data_types.h"
#include "memory/data.h"
#include "memory/datum_index_inlines.h"
#include "tag_files/tag_types.h"
#include "objects/object_datum.h"
#include "memory/static_arrays.h"
#include "cseries/cseries.h"
#include <ppcintrinsics.h>
#include <vectorintrinsics.h>
#include <string.h>
#include "render/views/player_view_types.h"
#include "objects/object_definitions.h"
#include "visibility/visibility_collection.h"
#include "visibility/visibility_globals.h"
#include <math.h>
#include "cache/restricted_memory_regions.h"
#include "cache/restricted_memory_regions_inline.h"
#include "game/game.h"
#include "scenario/scenario_tag.h"
#include "scenario/cluster_reference_helpers.h"
#include "render/render_camera_types.h"
#include "cache/restricted_memory.h"
#include "memory/tls_globals.h"
#if !defined(_XBOX) || defined(__clang__)
#include "../../port/sdk/runtime/port_config.h"
#endif
#include "cache/restricted_memory_registration.h"
#include "objects/objects.h"
#include "objects/object_header.h"
#include "simulation/simulation.h"
#include "geometry/geometry_sampling.h"
#include "render/render.h"
#include "render/render_objects_static_lighting.h"
#include "scenario/scenario_definitions.h"
#include "objects/lights.h"
#include "objects/object_lights.h"
#include "render/render_visibility_collection.h"
#include "render/views/render_player_view.h"
#include "units/vehicles.h"
#include "cache/restricted_memory_layout.h"
#include "geometry/geometry_sampling_types.h"
#include "game/game_time_types.h"
#include "effects/transparent_sort_layer_types.h"
#include "render/render_objects_static_lighting_hierarchy_levels_types.h"
#include "render/cinematic_light_entry_types.h"
#include "render/render_objects_static_lighting_setup_types.h"
#include "scenario/scenario.h"
#include "cache/cache_files.h"
#include "math/function_definitions.h"
#include "effects/effect_node_attachment_types.h"
#include "models/model_types.h"
#include "cutscene/cinematic_globals_types.h"
#include "game/player_mapping_types.h"
#include "models/render_model_definitions.h"
#include "render/render_transparents.h"
#include "render/render_objects.h"
#include "render/render_objects_static_lighting_imports.h"
#include "objects/object_marker_access.h"
#include "saved_games/saved_film_globals.h"
#include "scenario/scenario_lightmap_types.h"
#include "camera/director_globals_types.h"
#include "units/biped_types.h"
#include "render/render_visibility_cached_state_types.h"
#include "render/render_lighting_types.h"
#include "cubemaps/cubemap_sample.h"
#include "render/shader_extern_info_types.h"
#include "objects/object_render_thread_message.h"
#include "multithreading/message_queue.h"
#include "math/xenon_matrix_types.h"
#include "rasterizer/xenon/xenon_matrix.h"
#include "effects/screen_effect_definition_types.h"
#include "scenario/scenario_lightmap_lighting_types.h"
#include "effects/effect_lightprobes.h"
#include "render/lighting_interface.h"
#include "objects/light_cinematic_types.h"
#include "render/camera_fx_globals_types.h"
#include "render/render_camera_projection.h"
#include "render/render_lightmap_shadows_globals.h"
#include "game/game_tuning_types.h"
#include "units/unit_types.h"
#include "camera/camera_types.h"
#include "visibility/visibility_projections_and_volumes.h"
#include "objects/object_marker_globals_types.h"
#include "units/vehicle_types.h"
#include "cseries/cseries_wrapped.h"
#include <stdlib.h>
#include "units/giant_definitions.h"
#include "memory/datum_index.h"
#include "math/random_math_range.h"
#include "objects/scenery.h"
#include "render/render_first_person_globals_types.h"

// @address 0x7c61e4 g_render_object_time_fade_scale
extern "C" {
float g_render_object_time_fade_scale = 3.125f;
}

// @address 0x825fb0 render_object_globals
s_render_object_first_person_globals render_object_globals;

class c_camera_fx_values;
struct s_effect_lightprobe_result;
struct s_render_object_info;
struct s_geometry_sampling_probe;
struct render_objects_static_lighting_definition { long count; TAG_POINTER(float) values; };

struct s_transparent_sample_table
{
    real_point2d points[9];
    float radians_per_degree;
    float epsilon;
};

typedef char static_lighting_scenery_probe_offset[(offsetof(s_lightmap_scenery_probe, probe) == 8) ? 1 : -1];
typedef char static_lighting_machine_probe_offset[(offsetof(s_lightmap_machine_sample, probe) == 12) ? 1 : -1];
typedef char static_lighting_machine_probe_count_offset[(offsetof(s_lightmap_machine_probe, probe_count) == 32) ? 1 :
    -1];
typedef char static_lighting_machine_size[(sizeof(s_lightmap_machine_probe) == 44) ? 1 : -1];

// A type 2 object inherits its first-person ownership from its parent.

// match: 0x44f448: inline keeps interpolate_vector3d at 756 bytes; out of line it links 152 bytes short
#if defined(_XBOX) && !defined(__clang__)
inline void interpolate_scalar(float *value, float target, float rate)
#else
void interpolate_scalar(float *value, float target, float rate)
#endif
{
    float difference = target - *value;
    *value += PIN(difference, -rate, rate);
}

bool render_object_is_first_person(long player_index, long object_index)
{
    bool is_first_person = false;
    long index = object_index;
    if (index != -1 && player_index != -1) {
        if (g_director_globals->slots[player_index].perspective == 0) {
            s_object_header *headers = (s_object_header *)object_header_data->data;
            long resolved_object_index = index;
            if (headers[datum_index_to_absolute_index(index)].object_type == 2)
                resolved_object_index = headers[datum_index_to_absolute_index(index)].datum->parent_object_index;
            index = resolved_object_index;
            if (index != -1 && headers[datum_index_to_absolute_index(index)].object_type == 0) {
                if (index == player_mapping_globals->unit_indices[player_index])
                    is_first_person = true;
            }
        }
    }
    return is_first_person;
}

// @address 0x44a218 ?initialize_default_shader_extern_info@@YAXPAUs_shader_extern_info@@@Z

// @address 0xde710 ?object_get_function_value_simple@@YAMJJJ@Z

static __forceinline s_visibility_cached_object_render_state *render_objects_static_lighting_cached_get(long index)
{
    return &((s_visibility_cached_object_render_state
        *)g_cached_object_render_states->data)[datum_index_to_absolute_index(index)];
}

// 0x149720
long object_get_cached_render_state(long object_index)
{
    long result = -1;
    s_data_array *cached_states = g_cached_object_render_states;
    if (cached_states != 0)
    {
        do
        {
            int index_offset = (datum_index_to_absolute_index(object_index)) << 2;
            char *render_data = (char *)g_object_render_data;
            long cached_index = *(long *)(render_data + index_offset);
            bool newly_allocated = false;

            if (cached_index != -1 && ((s_visibility_cached_object_render_state
                *)cached_states->data)[datum_index_to_absolute_index(cached_index)].object_index == object_index)
            {
                result = cached_index;
            }
            else
            {
                result = datum_new(cached_states);
                if (result == -1)
                {
                    s_data_array *states = g_cached_object_render_states;
                    int max_diff = (int)0x80000000;
                    float rand = _real_random_range(get_local_random_seed_address(), 0.0f, 1.0f);
                    long start_index = (long)(rand * (float)(states->maximum_count - 32));
                    long next_index = ::data_next_index(states, start_index);
                    if (next_index != -1)
                    {
                        do
                        {
                            s_visibility_cached_object_render_state *entry = &((s_visibility_cached_object_render_state
                                *)g_cached_object_render_states->data)[datum_index_to_absolute_index(next_index)];
                            long candidate = c_render_globals::get_frame_index() - entry->update_frame;
                            candidate = (long)((double)candidate >= 0.0 ? (double)candidate : 1000.0);
                            if (candidate > max_diff)
                            {
                                max_diff = candidate;
                                result = next_index;
                                if (candidate > 1000)
                                    break;
                            }
                            next_index = ::data_next_index(g_cached_object_render_states, next_index);
                        } while (next_index != -1);
                    }

                    if (result == -1)
                        break;
                    {
                        s_visibility_cached_object_render_state *evicted = &((s_visibility_cached_object_render_state
                            *)g_cached_object_render_states->data)[datum_index_to_absolute_index(result)];
                        long old_object_index = evicted->object_index;
                        if (old_object_index != -1)
                            reset_object_cached_render_state(old_object_index);
                    }
                }

                newly_allocated = true;
                if (result == -1)
                    break;
            }

            s_visibility_cached_object_render_state *state = &((s_visibility_cached_object_render_state
                *)g_cached_object_render_states->data)[datum_index_to_absolute_index(result)];
            if (newly_allocated)
            {
                state->object_index = object_index;
                state->sample_frame = -1;
                state->update_frame = -1;
                state->sample_valid = false;
                state->valid = 0;

                initialize_default_shader_extern_info(&state->shader_info);
                state->shader_info.object_index = object_index;
                state->shader_info.function_value_simple_callback = object_get_function_value_simple;

                state->cubemap_sample.set_defaults();

                state->lod = -1;

                object_datum *datum = ((s_object_header
                    *)object_header_data->data)[(datum_index_to_absolute_index(object_index))].datum;
                int region_size = datum->region_information.size;
                char *region_base = (char *)datum + datum->region_information.offset;
                if ((unsigned char)((datum->flags >> 11) & 1))
                    region_size -= 16;

                int copy_size = region_size / 10;
                char *source = region_base + (copy_size << 3);
                char *dest = state->region_data[0];
                for (unsigned int i = 4; i != 0; --i)
                {
                    memcpy(dest, source, copy_size);
                    dest += 16;
                }

                *(long *)(render_data + index_offset) = result;
                render_object_update_change_colors(object_index, true);
            }
        } while (false);
    }

    return result;
}

// @address 0x1d6bb0 ?restricted_region_free_member@@YAXJJ@Z

// Allocator of the cached object render states (region 2, layout 2, thread-local data array).

// @address 0x149a00 ?deallocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_cached_object_render_states_allocator@@YAXPAX@Z@@UAAXPAX@Z
template <>
void t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_cached_object_render_states_allocator>::deallocate(void *memory)
{
    g_cached_object_render_states = 0;
    restricted_region_free_member(2, m_member_index);
}

// @address 0x7cff0c ?global_tag_instances@@3PBUcache_file_tag_instance@@B

// @address 0x7cff08 ?global_scenario@@3PAUscenario@@A

// @address 0x149a20 render_object_compute_light_scale
extern "C" void render_object_compute_light_scale(long object_index, const float *unused_a, const float *unused_b,
    const float *unused_c, float *result)
{
    *result = -1.0f;
    long definition_index = global_scenario->global_lighting.index;
    if (definition_index != -1) {
        render_objects_static_lighting_definition *definition =
            (render_objects_static_lighting_definition
                *)global_tag_instances[datum_index_to_absolute_index(definition_index)].definition;
        if (object_index >= 0 && object_index < definition->count) {
            float value = definition->values[object_index];
            if (value > 0.0f) {
                c_player_view *view =
                    &c_player_view::x_global_player_views[g_visibility_globals.collection_input->option1];
                if (view != 0) {
                    bool in_progress = false;
                    s_cinematic_globals_definition *cinematic = cinematic_globals;
                    if (cinematic != 0)
                        in_progress = cinematic->in_progress;
                    float exposure;
                    if (in_progress)
                        exposure = render_player_view_compute_exposure(&view->camera_settings->values);
                    else
                        exposure = view->exposure;
                    *result = sqrtf(value / exposure);
                }
            }
        }
    }
}

static __forceinline unsigned long render_objects_static_lighting_real_rgb_color_to_pixel32(const real_rgb_color *color)
{
    unsigned long red = (long)(color->red * 255.0f);
    unsigned long green = (long)(color->green * 255.0f);
    unsigned long blue = (long)(color->blue * 255.0f);
    return ((((0xff00 | red) << 8) | green) << 8) | blue;
}

float interpolate_value_perceptual(float from, float to, float fraction)
{
    float from_log = (float)log(from);
    float to_log = (float)log(to);
    float blended;
    if (fabs(from_log - to_log) < 5.0f)
        blended = (1.0f - fraction) * from_log + to_log * fraction;
    else
        blended = (to_log + from_log) * 0.5f;
    return (float)exp(blended);
}

void render_object_update_change_colors(long object_index, bool force)
{
    if (!restricted_region_locked_for_current_thread(2))
    {
        s_object_render_thread_message message;
        message.object_index = object_index;
        message.type = 2;
        message.unknown = force != false;
        g_object_message_queue->add_message(&message);
    }
    else
    {
        object_datum *object = (object_datum *)object_try_and_get_and_verify_type(object_index, 0xffffffff);
        if (object)
        {
            s_object_definition *definition = (s_object_definition
                *)global_tag_instances[datum_index_to_absolute_index(object->definition_index)].definition;
            long render_state_index = ((long *)g_object_render_data)[datum_index_to_absolute_index(object_index)];
            if (render_state_index != -1 && (force || (definition->runtime_flags & 1)))
            {
                s_visibility_cached_object_render_state *state = &((s_visibility_cached_object_render_state
                    *)g_cached_object_render_states->data)[datum_index_to_absolute_index(render_state_index)];
                state->shader_info.change_color_count = 0;
                for (long i = 0; i < 4; ++i)
                {
                    real_rgb_color color;
                    if (object_get_change_color(object_index, i + 1, &color))
                    {
                        ((unsigned long *)&state->shader_info.change_colors)[i] =
                            render_objects_static_lighting_real_rgb_color_to_pixel32(&color);
                        state->shader_info.change_color_count++;
                    }
                }
            }
        }
    }
}

void render_object_initialize_setup(s_render_objects_static_lighting_setup *setup)
{
    setup->flags = 0;
    setup->object_index = -1;
    setup->render_data = 0;
    setup->shader_extern_info = 0;
    setup->scenery_instance_index = -1;
    setup->level_of_detail = 1;
    setup->region_count = 0;
    memset(setup->region_permutations, 0, sizeof(setup->region_permutations));
    memset(setup->region_zsort, 0, sizeof(setup->region_zsort));
    s_shader_extern_info *info = setup->shader_extern_info;
    setup->definition_index = -1;
    setup->skinning_matrix_count = 0;
    setup->skinning_memory_designator = -1;
    if (info)
        initialize_default_shader_extern_info(info);
}

extern "C" const render_lighting *object_get_cached_render_lighting(long object_index)
{
    object_datum *object = object_get(object_index);
    const render_lighting *lighting = 0;
    if (object->parent_object_index != -1)
        lighting = object_get_cached_render_lighting(inl_object_get_ultimate_parent(object_index));
    else
    {
        long state_index = object_get_cached_render_state(object_index);
        if (state_index != -1)
        {
            s_visibility_cached_object_render_state *state = render_objects_static_lighting_cached_get(state_index);
            if (state->valid)
                lighting = &state->shader_info.lighting;
        }
    }
    return lighting;
}

XMVECTOR xmmatrix_determinant(XMMATRIX M)
{
    XMVECTOR V0, V1, V2, V3, V4, V5;
    XMVECTOR P0, P1, P2, R, S;
    XMVECTOR Result;
    XMVECTOR Zero, SignMask;

    V0 = __vpermwi(M.r[2], 0x40);
    V1 = __vpermwi(M.r[3], 0xA5);
    V2 = __vpermwi(M.r[2], 0x40);
    V3 = __vpermwi(M.r[3], 0xFE);
    V4 = __vpermwi(M.r[2], 0xA5);
    V5 = __vpermwi(M.r[3], 0xFE);

    P0 = __vmulfp(V0, V1);
    P1 = __vmulfp(V2, V3);
    P2 = __vmulfp(V4, V5);

    V0 = __vpermwi(M.r[2], 0xA5);
    V1 = __vpermwi(M.r[3], 0x40);
    V2 = __vpermwi(M.r[2], 0xFE);
    V3 = __vpermwi(M.r[3], 0x40);
    V4 = __vpermwi(M.r[2], 0xFE);
    V5 = __vpermwi(M.r[3], 0xA5);

    P0 = __vnmsubfp(V0, V1, P0);
    P1 = __vnmsubfp(V2, V3, P1);
    P2 = __vnmsubfp(V4, V5, P2);

    V0 = __vpermwi(M.r[1], 0xFE);
    V1 = __vpermwi(M.r[1], 0xA5);
    V2 = __vpermwi(M.r[1], 0x40);

    R = __vmulfp(V0, P0);
    SignMask = __vspltisw(-1);
    Zero = __vspltisw(0);
    R = __vnmsubfp(V1, P1, R);
    SignMask = __vslw(SignMask, SignMask);
    SignMask = __vmrghw(Zero, SignMask);
    R = __vmaddfp(V2, P2, R);
    S = __vxor(M.r[0], SignMask);

    Result = __vmsum4fp(S, R);

    return Result;
}

void render_objects_set_cached_base_address(void *object)
{
    data_set_new_base_address(&g_cached_object_render_states, (s_data_array *)object);
}

static __forceinline void render_objects_static_lighting_clear_all_object_render_data()
{
    for (long i = 0; i < g_object_render_data_count; ++i) ((long *)g_object_render_data)[i] = -1;
}
// @address 0x44eb98 ?render_objects_reset_render_state@@YAXJ@Z
void render_objects_reset_render_state(long unused)
{
    render_objects_static_lighting_clear_all_object_render_data();
    for (long index = ::data_next_index(g_cached_object_render_states, -1); index != -1;
        index = inl_data_next_index(g_cached_object_render_states, index))
    {
        s_visibility_cached_object_render_state *state = &((s_visibility_cached_object_render_state
            *)g_cached_object_render_states->data)[datum_index_to_absolute_index(index)];
        state->object_index = -1;
    }
}

void render_object_invalidate_render_state(long object_index)
{
    if (g_cached_object_render_states == 0)
    {
        s_object_render_thread_message message;
        message.object_index = object_index;
        message.type = 1;
        message.unknown = 0;
        g_object_message_queue->add_message(&message);
    }
    else
    {
        long render_state_index = object_get_cached_render_state(object_index);
        if (render_state_index != -1)
            render_objects_static_lighting_cached_get(render_state_index)->sample_valid = false;
    }
}

// @address 0x44f7d0 ?allocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_cached_object_render_states_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template <>
void *t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_cached_object_render_states_allocator>::allocate(unsigned int size, const char *name)
{
    int member_index = g_restricted_regions[2].add_member(name, "", size, 0, render_objects_set_cached_base_address, 0,
        0);
    unsigned int offset = g_restricted_regions[2].get_member_offset(member_index);
    data_set_new_base_address(&g_cached_object_render_states, (s_data_array *)(g_restricted_address[2] + offset));
    m_member_index = member_index;
    m_thread_id = g_registered_thread_index;
    return g_restricted_address[2] + g_restricted_regions[2].get_member_offset(member_index);
}

// @address 0x51e398 ?__tls_set_g_cached_object_render_states_allocator@@YAXPAX@Z
void __tls_set_g_cached_object_render_states_allocator(void *object)
{
    data_set_new_base_address((s_data_array **)&g_ai_dialogue_records, (s_data_array *)object);
}

// Accumulate one lighting sample: the blend is clamped to [0, 1], the first
// eight channels take the larger of the stored and the scaled incoming value,
// the next three blend against (1 - blend), and the last three again take the
// larger value.
extern "C" void render_static_lighting_accumulate_sample(float *out, const float *in, float t)
{
    t = MIN(MAX(t, 0.0f), 1.0f);
    out[0] = out[0] > in[0] * t ? out[0] : in[0] * t;
    out[1] = out[1] > in[1] * t ? out[1] : in[1] * t;
    out[2] = out[2] > in[2] * t ? out[2] : in[2] * t;
    out[3] = out[3] > in[3] * t ? out[3] : in[3] * t;
    out[4] = out[4] > in[4] * t ? out[4] : in[4] * t;
    out[5] = out[5] > in[5] * t ? out[5] : in[5] * t;
    out[6] = out[6] > in[6] * t ? out[6] : in[6] * t;
    out[7] = out[7] > in[7] * t ? out[7] : in[7] * t;
    out[8] = out[8] > (1.0f - t) + (in[8] * t) ? (1.0f - t) + (in[8] * t) : out[8];
    out[9] = out[9] > (1.0f - t) + (in[9] * t) ? (1.0f - t) + (in[9] * t) : out[9];
    out[10] = out[10] > (1.0f - t) + (in[10] * t) ? (1.0f - t) + (in[10] * t) : out[10];
    out[11] = out[11] > in[11] * t ? out[11] : in[11] * t;
    out[12] = out[12] > in[12] * t ? out[12] : in[12] * t;
    out[13] = out[13] > in[13] * t ? out[13] : in[13] * t;
}

// @rdata 0x82024c00..0x82024c50
static const s_transparent_sample_table k_sample_table = {
    {{0.0f, 0.0f}, {-0.5f, 0.5f}, {0.5f, 0.5f}, {-0.5f, -0.5f}, {0.5f, -0.5f}, {-1.0f, 0.0f}, {1.0f, 0.0f}, {0.0f,
        1.0f}, {0.0f, -1.0f}},
    0.017453292f, 0.0001f};
// @rdata-end


static __forceinline real_plane3d *render_objects_static_lighting_copy_plane(const real_plane3d *plane, real_plane3d
    *result)
{
    *result = *plane;
    return result;
}

// @address 0x1634d0 render_objects_add_transparent_element
// match: 0x1634d0: noalias declaration preserves caller live registers
extern "C" __declspec(noalias) bool render_objects_add_transparent_element(const real_point3d *point, const
    real_plane3d *plane,
    float bias, e_transparent_sort_layer sort_layer, void (*callback)(const void *, long),
    const void *context, long user, float radius)
{
    real_matrix4x3 camera;
    float scale_x = 0.0f; float scale_y = 0.0f;

    bool result = false;
    int count = c_transparency_renderer::m_total_transparent_count;
    if (count < 1024)
    {
        int next_count = count + 1;
        c_transparency_renderer::transparent_sorted_order.m_order[count] = (short)count;
        s_transparent_types *entry = &c_transparency_renderer::transparents.m_storage[count];
        c_transparency_renderer::transparent_sorted_order.m_count =
            (short)MAX((int)(unsigned short)c_transparency_renderer::transparent_sorted_order.m_count, next_count);

        entry->has_plane = (plane != 0 && radius > 0.0001f && sqrtf(plane->n.j * plane->n.j + plane->n.i * plane->n.i +
            plane->n.k * plane->n.k) > 0.5f);

        c_player_view::get_player_render_camera_orientation(&camera);
        float cam_pos_z = camera.position.z;
        float cam_pos_y = camera.position.y;
        float cam_pos_x = camera.position.x;
        float cam_fwd_k = camera.forward.k;
        float cam_fwd_j = camera.forward.j;
        float cam_fwd_i = camera.forward.i; float cam_up_k = camera.up.k;

        if (entry->has_plane)
        {
            if (!(plane->n.j * cam_pos_y + plane->n.i * cam_pos_x + plane->n.k * cam_pos_z - plane->d >= 0.0f))
            {
                entry->plane.n.i = -plane->n.i;
                entry->plane.n.j = -plane->n.j;
                entry->plane.n.k = -plane->n.k;
                entry->plane.d = -plane->d;
            }
            else
            {
                render_objects_static_lighting_copy_plane(plane, &entry->plane);
            }

            float distance;
            {
                real_vector3d offset;
                offset.i = point->x - cam_pos_x;
                offset.j = point->y - cam_pos_y;
                offset.k = point->z - cam_pos_z;
                distance = normalize3d(&offset);
            }

            float clamped_dot = PIN(-dot_product3d(&camera.forward, &entry->plane.n), 0.0f, 1.0f);
            entry->radius = (clamped_dot * (radius * radius)) / distance;

            {
            real_vector3d basis1;
            real_vector3d basis2;
            float dot_up = plane->n.j * camera.up.j + plane->n.i * camera.up.i + plane->n.k * cam_up_k;
            if (fabsf(dot_up) < 0.9f)
            {
                basis1.i = camera.up.j * plane->n.k - camera.up.k * plane->n.j;
                basis1.j = camera.up.k * plane->n.i - camera.up.i * plane->n.k;
                basis1.k = camera.up.i * plane->n.j - camera.up.j * plane->n.i;
            }
            else
            {
                basis1.i = camera.left.j * plane->n.k - camera.left.k * plane->n.j;
                basis1.j = camera.left.k * plane->n.i - camera.left.i * plane->n.k;
                basis1.k = camera.left.i * plane->n.j - camera.left.j * plane->n.i;
            }

            basis2.i = plane->n.j * basis1.k - plane->n.k * basis1.j;
            basis2.k = plane->n.i * basis1.j - plane->n.j * basis1.i;
            basis2.j = plane->n.k * basis1.i - plane->n.i * basis1.k;

            normalize3d(&basis1);
            normalize3d(&basis2);

            for (unsigned short i = 0; i < 9; ++i)
            {
                real_point3d *sample_point = &entry->sample_points[i];
                float scaled[3];
                scale_x = k_sample_table.points[i].x * radius;
                scaled[0] = basis1.i * scale_x;
                scaled[1] = basis1.j * scale_x;
                scaled[2] = basis1.k * scale_x;
                sample_point->x = point->x + scaled[0];
                sample_point->y = point->y + scaled[1];
                sample_point->z = point->z + scaled[2];
                scale_y = k_sample_table.points[i].y * radius;
                scaled[0] = basis2.i * scale_y;
                scaled[1] = basis2.j * scale_y;
                scaled[2] = basis2.k * scale_y;
                sample_point->x = sample_point->x + scaled[0];
                sample_point->y = sample_point->y + scaled[1];
                sample_point->z = sample_point->z + scaled[2];
            }
            }

        }
        else
        {
            entry->radius = 0.0f;
        }

        entry->position = *point;
        float to_x = point->x - cam_pos_x; float to_y = point->y - cam_pos_y; float to_z = point->z - cam_pos_z;
        c_transparency_renderer::m_total_transparent_count = next_count;
        entry->sort_layer = sort_layer;
        entry->render_callback = callback;
        entry->render_data = context;
        entry->user_data = user;
        entry->sort_key = -(to_x * cam_fwd_i + (to_y * cam_fwd_j + to_z * cam_fwd_k)) - bias;

        result = true;
    }

    return result;
}

// @address 0x23d9f8 render_objects_compute_effect_weight
extern "C" float render_objects_compute_effect_weight(s_screen_effect_response *params, float input, float angle, float weight)
{
    if ((params->flags & 2) == 0 && input > params->input_limit)
        return 0.0f;
    float half = 0.5f;
    float input_fraction = input / params->input_limit;
    float first = params->input_function.definition.evaluate_scalar(PIN(input_fraction, 0.0f, 1.0f), half);
    float scaled;
    if (params->divisor <= k_real_epsilon)
        scaled = weight;
    else
    {
        scaled = weight / params->divisor;
        scaled = MIN(MAX(scaled, 0.0f), 1.0f);
    }
    float combined = params->scale_function.definition.evaluate_scalar(scaled, half) * first;
    float angle_fraction = angle * 0.31830987f;
    return params->angle_function.definition.evaluate_scalar(PIN(angle_fraction, 0.0f, 1.0f), half) * combined;
}
// match: noalias declaration preserves caller live registers

// @address 0x44f448 interpolate_vector3d
extern "C" void interpolate_vector3d(real_vector3d *value, const real_vector3d *target, float rate)
{
    const float epsilon = 0.0001f;
    const float one = 1.0f;
    float magnitude = PIN(magnitude3d(value), 0.0001f, 10.0f);
    float target_magnitude = PIN(magnitude3d(target), 0.0001f, 10.0f);
    real_vector3d direction;
    scale_vector3d(value, one / magnitude, &direction);
    real_vector3d target_direction;
    scale_vector3d(target, one / target_magnitude, &target_direction);
    float alignment = dot_product3d(&target_direction, &direction);
    if (!(fabsf(alignment) >= 0.9999f) && !(magnitude <= epsilon) && !(target_magnitude <= epsilon))
    {
        float angle = arccosine(alignment);
        float rotation = angle * rate;
        real_vector3d axis;
        cross_product3d(&direction, &target_direction, &axis);
        normalize3d(&axis);
        float cosine = (float)cos(rotation);
        float sine = (float)sin(rotation);
        rotate_vector_about_axis(&direction, &axis, sine, cosine);
        interpolate_scalar(&magnitude, target_magnitude, rate);
        scale_vector3d(&direction, magnitude, value);
    }
    else
    {
        interpolate_scalar(&value->i, target->i, rate);
        interpolate_scalar(&value->j, target->j, rate);
        interpolate_scalar(&value->k, target->k, rate);
    }
}

// @address 0x5804a0 render_objects_decode_lightprobe
extern "C" void render_objects_decode_lightprobe(const s_lightmap_half_rgb_probe *probe,
    // match: 0x5804a0: pointer __restrict qualifier prevents alias reload
    const real_point3d *__restrict point, s_geometry_sample *result)
{
    result->diffuse.i = 0.0f;
    result->diffuse.j = 0.0f;
    result->diffuse.k = 0.0f;
    result->normal = *global_up3d;
    result->valid = false;
    result->position = *point;
    result->dominant_light_intensity = 10.0f;
    result->dominant_light_direction.i = ::half_to_real(probe->dominant_light_direction[0]);
    result->dominant_light_direction.j = ::half_to_real(probe->dominant_light_direction[1]);
    result->dominant_light_direction.k = ::half_to_real(probe->dominant_light_direction[2]);
    result->dominant_light_color.red = ::half_to_real(probe->dominant_light_intensity[0]);
    result->dominant_light_color.green = ::half_to_real(probe->dominant_light_intensity[1]);
    result->dominant_light_color.blue = ::half_to_real(probe->dominant_light_intensity[2]);
    for (long i = 0; i < 9; ++i)
    {
        result->red[i] = ::half_to_real(probe->channel_coefficients[0][i]);
        result->green[i] = ::half_to_real(probe->channel_coefficients[1][i]);
        result->blue[i] = ::half_to_real(probe->channel_coefficients[2][i]);
    }
}
// Callee returns 0/1; declared long so the caller keeps a plain copy (no bool narrow).

// @address 0x5805b0 render_objects_sample_effect_lightprobe
extern "C" long render_objects_sample_effect_lightprobe(long p0, long object_index, const real_point3d *p2,
    s_effect_lightprobe_result *result)
{
    real_point3d point = ((s_object_header
        *)object_header_data->data)[datum_index_to_absolute_index(object_index)].datum->bounding_center;
    long ok = effect_sample_lightprobes(&point, result);
    return (0 != ok) << 1;
}

// @address 0x580628 render_objects_get_sky_lightprobe
extern "C" long render_objects_get_sky_lightprobe(long object_index, s_geometry_sample *info)
{
    long result = 2;
    s_location location;
    char message[256];
    float intensity;
    object_get_location(object_index, &location);
    const s_cluster_reference *cluster = &location.cluster_reference;
    long cluster_index = cluster->cluster_index;
    long bsp_index = cluster->bsp_index;
    if (cluster_index != -1 && bsp_index != -1)
    {
        scenario *scenario_definition = global_scenario;
        long sky_index = scenario_zone_set_structure_bsp_cluster_visible_sky_index_get(
            scenario_definition, global_zone_set_index, bsp_index, cluster_index);
        const scenario *scenario_view = scenario_definition;
        if (sky_index != -1 && sky_index >= 0 && sky_index < scenario_view->skies.count)
        {
            long object_tag_index = ((const scenario_sky_reference
                *)scenario_view->skies.address)[sky_index].object.index;
            if (object_tag_index != -1)
            {
                const s_object_definition *object_definition =
                    (const s_object_definition
                        *)global_tag_instances[datum_index_to_absolute_index(object_tag_index)].definition;
                long model_index = object_definition->model.index;
                if (model_index != -1)
                {
                    const s_model_definition *model =
                        (const s_model_definition
                            *)global_tag_instances[datum_index_to_absolute_index(model_index)].definition;
                    long render_model_index = model->render_model_index;
                    if (render_model_index != -1)
                    {
                        const render_model_definition *render_model =
                            (const render_model_definition
                                *)global_tag_instances[datum_index_to_absolute_index(render_model_index)].definition;
                        memcpy(info->red, (const long *)render_model->ravi_red, 9 * sizeof(long));
                        memcpy(info->green, (const long *)render_model->ravi_green, 9 * sizeof(long));
                        memcpy(info->blue, (const long *)render_model->ravi_blue, 9 * sizeof(long));
                        calculate_dominant_light_from_lightprobe(
                            render_model->ravi_red, render_model->ravi_green, render_model->ravi_blue,
                            &info->dominant_light_direction, &info->dominant_light_color, &intensity);
                        info->normal = global_identity4x3->up;
                        info->dominant_light_intensity = intensity;
                        info->diffuse.i = 0.0f;
                        info->diffuse.j = 0.0f;
                        info->diffuse.k = 0.0f;
                        result = 0;
                        csnzprintf(message, sizeof(message), "Sb%d", object_index);
                    }
                }
            }
        }
    }
    if (result != 0 && c_visible_items::m_items.skies.count != 0
        && lighting_interface_get_visible_sky_lighting((long *)info->red, (long *)info->green, (long *)info->blue,
            &info->dominant_light_direction, &info->dominant_light_color) != 0)
    {
        info->normal = global_identity4x3->up;
        info->dominant_light_intensity = 10.0f;
        info->diffuse.i = 1.0f;
        info->diffuse.j = 1.0f;
        info->diffuse.k = 1.0f;
        result = 0;
        csnzprintf(message, sizeof(message), "Sf%d", object_index);
    }
    return result;
}

// @address 0x15c608 ?update@c_dynamic_cubemap_sample@@QAA_NPBTreal_point3d@@M@Z
bool c_dynamic_cubemap_sample::update(const real_point3d *current_position, float /*blend*/)
{
    bool result = false;
    if (m_blend_factor >= 1.0f)
    {
        s_cluster_reference reference = scenario_cluster_reference_from_point(current_position);
        if (cluster_reference_valid(&reference))
            search_for_cubemap_sample_in_cluster(reference, current_position, 0.1f);
    }
    return result;
}

// @address 0x15b500 render_objects_interpolate_lightprobe
extern "C" bool render_objects_interpolate_lightprobe(
    s_geometry_sample *sample, const s_geometry_sample *target, float rate, long unused)
{
    bool result = false;
    float error = 0.0f;
    float r_sum0 = 0.0f;
    float g_sum0 = 0.0f;
    float b_sum0 = 0.0f;
    float r_sum1 = 0.0f;
    float g_sum1 = 0.0f;
    float b_sum1 = 0.0f;
    for (unsigned long i = 0; i < 9; ++i)
    {
        r_sum0 = (r_sum0 + (sample->red[i] * sample->red[i]));
        g_sum0 = (g_sum0 + (sample->green[i] * sample->green[i]));
        b_sum0 = (b_sum0 + (sample->blue[i] * sample->blue[i]));
        r_sum1 = (r_sum1 + (target->red[i] * target->red[i]));
        g_sum1 = (g_sum1 + (target->green[i] * target->green[i]));
        b_sum1 = (b_sum1 + (target->blue[i] * target->blue[i]));
    }
    float epsilon = 0.00001f;
    real_rgb_color sample_magnitude;
    real_rgb_color target_magnitude;
    real_rgb_color magnitude;
    sample_magnitude.red = sqrt(r_sum0);
    sample_magnitude.green = sqrt(g_sum0);
    sample_magnitude.blue = sqrt(b_sum0);
    target_magnitude.red = sqrt(r_sum1);
    target_magnitude.green = sqrt(g_sum1);
    target_magnitude.blue = sqrt(b_sum1);

    sample_magnitude.red = MAX(sample_magnitude.red, epsilon);
    sample_magnitude.green = MAX(sample_magnitude.green, epsilon);
    sample_magnitude.blue = MAX(sample_magnitude.blue, epsilon);
    target_magnitude.red = MAX(target_magnitude.red, epsilon);
    target_magnitude.green = MAX(target_magnitude.green, epsilon);
    target_magnitude.blue = MAX(target_magnitude.blue, epsilon);

    magnitude.red = interpolate_value_perceptual(sample_magnitude.red, target_magnitude.red, rate);
    magnitude.green = interpolate_value_perceptual(sample_magnitude.green, target_magnitude.green, rate);
    magnitude.blue = interpolate_value_perceptual(sample_magnitude.blue, target_magnitude.blue, rate);

    float inverse_rate = 1.0f - rate;
    for (unsigned long i = 0; i < 9; ++i)
    {
        sample->red[i] = ((((sample->red[i] / sample_magnitude.red) * inverse_rate) +
                                          ((target->red[i] / target_magnitude.red) * rate)) *
            magnitude.red);
        sample->green[i] = ((((sample->green[i] / sample_magnitude.green) * inverse_rate) +
                                          ((target->green[i] / target_magnitude.green) * rate)) *
            magnitude.green);
        sample->blue[i] = ((((sample->blue[i] / sample_magnitude.blue) * inverse_rate) +
                                          ((target->blue[i] / target_magnitude.blue) * rate)) *
            magnitude.blue);
    }

    sample->dominant_light_intensity = (inverse_rate * sample->dominant_light_intensity) +
        (target->dominant_light_intensity * rate);
    sample->dominant_light_color.red = inverse_rate * sample->dominant_light_color.red +
        target->dominant_light_color.red * rate;
    sample->dominant_light_color.green = inverse_rate * sample->dominant_light_color.green +
        target->dominant_light_color.green * rate;
    sample->dominant_light_color.blue = inverse_rate * sample->dominant_light_color.blue +
        target->dominant_light_color.blue * rate;

    interpolate_vector3d(&sample->dominant_light_direction, &target->dominant_light_direction, rate);

    for (unsigned long i = 0; i < 9; ++i)
    {
        error = (error + (fabs((sample->red[i] - target->red[i]))));
        error = (error + (fabs((sample->green[i] - target->green[i]))));
        error = (error + (fabs((sample->blue[i] - target->blue[i]))));
    }
    error = (error + fabs(sample->dominant_light_color.red - target->dominant_light_color.red));
    error += fabs(sample->dominant_light_color.green - target->dominant_light_color.green);
    error += fabs(sample->dominant_light_color.blue - target->dominant_light_color.blue);
    error += fabs(sample->dominant_light_direction.i - target->dominant_light_direction.i);
    error += fabs(sample->dominant_light_direction.j - target->dominant_light_direction.j);
    error += fabs(sample->dominant_light_direction.k - target->dominant_light_direction.k);

    if (error < (target_magnitude.red + target_magnitude.green + target_magnitude.blue) * 0.05f)
        result = true;

    return result;
}

// @address 0x57f068 render_objects_apply_cinematic_lighting
extern "C" void render_objects_apply_cinematic_lighting(long object_index, render_lighting *lighting)
{
    s_cinematic_light_globals *cinematic = g_cinematic_light_globals;
    long object_entry_index = lights_find_cinematic_object_entry(cinematic, object_index, false);
    if (object_entry_index != -1)
    {
        lighting->unknown = object_entry_index;
        s_cinematic_object_entry *entry = &cinematic->object_entries[object_entry_index];
        real_vector3d *negated_key_color = &entry->lighting.down;
        memcpy(negated_key_color, lighting, sizeof(*lighting));
        s_geometry_sample *sample = &entry->lighting.sample;
        float scale = entry->scale;
        float k = camera_fx_globals->exposure_scale;
        float k2 = k;
        for (long i = 0; i < 9; i++)
        {
            sample->red[i] = sample->red[i] * scale + entry->values0[i] * k;
            sample->green[i] = sample->green[i] * scale + entry->values1[i] * k;
            sample->blue[i] = sample->blue[i] * scale + entry->values2[i] * k2;
        }
        real_vector3d *key_color = &sample->dominant_light_direction;
        calculate_dominant_light_from_lightprobe(
            sample->red, sample->green, sample->blue, key_color, &sample->dominant_light_color,
                &sample->dominant_light_intensity);
        negated_key_color->i = -key_color->i;
        negated_key_color->j = -key_color->j;
        negated_key_color->k = -key_color->k;
    }
}

// Fallback static lighting used when no cached render state exists (536 bytes).

extern "C" {
// @address 0x11db0e8 g_fallback_render_lighting
render_lighting g_fallback_render_lighting;
}

// @address 0x149590 ?object_update_cached_render_lighting@@YAPAUrender_lighting@@JM@Z
render_lighting *object_update_cached_render_lighting(long object_index, float value)
{
    long ultimate_parent_index = -1;
    object_datum *object = object_get(object_index);
    long index = object_index;
    if (object_index != -1)
    {
        do
        {
            ultimate_parent_index = index;
            index = object_get(index)->parent_object_index;
        } while (index != -1);
    }
    long cached_index = object_get_cached_render_state(ultimate_parent_index);
    render_lighting *result = 0;
    if (cached_index != -1)
    {
        s_visibility_cached_object_render_state *parent_state = render_objects_static_lighting_cached_get(cached_index);
        if (ultimate_parent_index == object_index)
        {
            render_objects_refresh_lighting(cached_index, object_index, value);
            result = &parent_state->shader_info.lighting;
        }
        else
        {
            long object_cached_index = object_get_cached_render_state(object_index);
            if (object_cached_index != -1)
            {
                s_visibility_cached_object_render_state *object_state =
                    render_objects_static_lighting_cached_get(object_cached_index);
                object_state->shader_info.lighting = parent_state->shader_info.lighting;
                result = &object_state->shader_info.lighting;
                object_state->target_lighting = parent_state->target_lighting;
            }
        }
    }
    if (!result)
    {
        result = &g_fallback_render_lighting;
        render_objects_sample_lighting(ultimate_parent_index, 1, &g_fallback_render_lighting.sample, 0);
        g_fallback_render_lighting.cubemap.update(&object->bounding_center, 0.1f);
    }
    if ((object->flags_bytes[BIG_ENDIAN_VIEW_INDEX(0, 4)] & 1) != 0)
        render_objects_apply_cinematic_lighting(object_index, result);
    else if (result)
        result->unknown = -1;
    if (result)
        render_object_compute_light_scale(
            ((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_index)].object_type,
            result->sample.red,
            result->sample.green,
            result->sample.blue,
            &result->sample.light_scale);
    return result;
}

// @address 0x8368d0 g_player_render_tuning
extern "C" {
// @address 0x730e50 g_shadow_fade_distance_scale
// @address 0x737510 g_game_tuning_globals

}
static __forceinline void *render_objects_static_lighting_fade_tag_get(long index)
{
    // match: 0x737510: value range assumption guides branch lowering
    __assume(index != -1);
    return global_tag_instances[datum_index_to_absolute_index(index)].definition;
}
static __forceinline long render_objects_static_lighting_render_model_get(const object_datum *object)
{
    const s_object_definition *definition =
        (const s_object_definition *)render_objects_static_lighting_fade_tag_get(object->definition_index);
    return definition->model.index;
}
static __forceinline bool render_objects_static_lighting_fade_cinematic()
{
    bool result = false;
    if (cinematic_globals)
        result = cinematic_globals->in_progress;
    return result;
}
static __forceinline float render_objects_static_lighting_screen_fade(float projected_radius, float zero, float one)
{
    return PIN((projected_radius - 30.0f) / 6.0f, zero, one);
}

static __forceinline bool render_objects_static_lighting_fade_flag_test(unsigned long flags, long bit)
{
    return (flags & (1UL << bit)) != 0;
}
static __forceinline unsigned long render_objects_static_lighting_fade_split_view(long count) { return count >= 4 || 1
    < count; }

#if !defined(_XBOX) || defined(__clang__)
extern "C" int __cdecl port_config_splitscreen_full_quality(void);
#endif
// @address 0x159b10 render_object_compute_fade
// match: 0x159b10: pointer __restrict qualifier prevents alias reload
extern "C" void render_object_compute_fade(long object_index, float distance, unsigned char *__restrict fade)
{
    fade[0] = 0;
    fade[2] = 0;
    fade[1] = 0;
    s_object_header *header = &((s_object_header
        *)object_header_data->data)[datum_index_to_absolute_index(object_index)];
    object_datum *object = header->datum;
    long object_type = header->object_type;
    if (render_objects_static_lighting_fade_flag_test(object->flags, 19)) {
        fade[2] = 255;
        fade[1] = 255;
        fade[0] = 255;
        return;
    }
    long model_index = render_objects_static_lighting_render_model_get(object);
    if (model_index == -1)
        return;
    s_model_fade_parameters parameters = ((s_model_definition
        *)render_objects_static_lighting_fade_tag_get(model_index))->fade;
    long parent_index = inl_object_get_ultimate_parent(object_index);
    if (parent_index != object_index) {
        long parent_model_index =
            render_objects_static_lighting_render_model_get(object_get(parent_index));
        if (parent_model_index != -1)
            parameters = ((s_model_definition *)render_objects_static_lighting_fade_tag_get(parent_model_index))->fade;
    }
    if (parameters.fade_start_distance > 0.0f && parameters.disappear_distance - parameters.fade_start_distance >
        0.0001f) {
        float scale;
        if (!render_objects_static_lighting_fade_cinematic() && !players_globals->suppressed &&
            player_mapping_globals->local_player_count > 1 &&
#if !defined(_XBOX) || defined(__clang__)
            !port_config_splitscreen_full_quality() &&
#endif
                render_objects_static_lighting_fade_split_view(player_mapping_globals->local_player_count))
            {
            scale = g_game_tuning_globals.values[object_type] * g_player_render_tuning.distance_scale;
            scale = PIN(scale, 0.001f, 1.0f);
        } else {
            scale = PIN(g_player_render_tuning.distance_scale, 0.001f, 1.0f);
        }
        parameters.disappear_distance *= scale;
        parameters.fade_start_distance *= scale;
        parameters.lod_distance *= scale;
        parameters.lod_distance = MIN(parameters.lod_distance, parameters.disappear_distance);
    }
    float fade_end = parameters.disappear_distance;
    float fade_start = parameters.fade_start_distance;
    float lod_distance = parameters.lod_distance;
    float bounding_radius = object->bounding_radius;
    float radius_distance = bounding_radius * 7.0f;
    float shadow_fade = 1.0f;
    float visibility_fade = 1.0f;
    float lod_fade = 1.0f;
    float shadow_distance = g_shadow_fade_distance_scale * distance;
    if ((shadow_distance < parameters.shadow_fade_start_distance + radius_distance) == false) {
        shadow_fade = 0.0f;
    } else if (shadow_distance > parameters.shadow_fade_start_distance) {
        shadow_fade = 1.0f - (shadow_distance - parameters.shadow_fade_start_distance) / radius_distance;
        shadow_fade = PIN(shadow_fade, 0.0f, 1.0f);
        if ((shadow_fade > 0.03f) == false)
            shadow_fade = 0.0f;
    }
#if !defined(_XBOX) || defined(__clang__)
    // Extend the object fades after computing the shadow alpha at the real distance.
    const float draw_distance_scale = port_config_draw_distance();
    distance /= draw_distance_scale;
#endif
    float fade_range;
    if (fade_end > 0.0f && (fade_range = fade_end - fade_start) > 0.0001f) {
        if ((distance < fade_end) == false)
            visibility_fade = 0.0f;
        else if (distance > fade_start) {
            visibility_fade = 1.0f - (distance - fade_start) / fade_range;
            visibility_fade = PIN(visibility_fade, 0.0f, 1.0f);
        }
    } else {
        float projected_radius = render_camera_compute_projected_diameter(
            &c_player_view::get_current()->render_projection_state, &object->bounding_center, bounding_radius);
#if !defined(_XBOX) || defined(__clang__)
        projected_radius *= draw_distance_scale;
#endif
        visibility_fade = render_objects_static_lighting_screen_fade(projected_radius, 0.0f, 1.0f);
    }
    float near_distance = lod_distance;
    if (!(near_distance > 0.0f))
        near_distance = radius_distance;
    if (near_distance > 0.0f) {
        if ((distance < near_distance) == false)
            lod_fade = 0.0f;
        else if (distance > near_distance * 0.7f) {
            lod_fade = (near_distance - distance) / (near_distance * 0.3f);
            lod_fade = PIN(lod_fade, 0.0f, 1.0f);
        } else {
            lod_fade = 1.0f;
        }
    }
    unsigned char visibility = (unsigned char)(visibility_fade * 255.0f);
    unsigned char shadow = (unsigned char)(shadow_fade * 255.0f);
    unsigned char lod = (unsigned char)(lod_fade * 255.0f);
    fade[0] = visibility;
    fade[2] = shadow;
    fade[1] = lod;
}
// Object fade snapshots use the canonical game_time_globals_definition field offsets.

static __forceinline float render_objects_static_lighting_fade_scale(float seconds)
{
    float fraction = PIN(seconds * 0.05f, 0.0f, 1.0f);
    return fraction * g_render_object_time_fade_scale + (1.0f - fraction);
}
// @rdata 0x820242d8
static const float k_render_objects_static_lighting_alpha_minimum = 2.0f;
// @address 0x159710 ?render_object_calculate_alphas@@YAXJPBTreal_point3d@@MJ_N1PAUrender_objects_static_lighting_hierarchy_levels@@@Z
void render_object_calculate_alphas(long object_index, const real_point3d *position, float, long object_type, bool
    time_fade,
    bool cached_alpha, render_objects_static_lighting_hierarchy_levels *alphas)
{
    unsigned char fade[4];
    fade[0] = 0;
    fade[2] = 0;
    fade[1] = 0;
    c_player_view *view = c_player_view::get_current();
    if (view != 0)
    {
        const render_camera *camera = view->get_rasterizer_camera();
        // match: 0x159710: the direct component differences preserve the component load schedule
        real_vector3d distance_vector;
        distance_vector.i = position->x - camera->position.x;
        distance_vector.j = position->y - camera->position.y;
        distance_vector.k = position->z - camera->position.z;
        float distance = sqrtf(distance_vector.i * distance_vector.i + distance_vector.j * distance_vector.j +
            distance_vector.k * distance_vector.k) * camera->field_of_view_scale;
        if (time_fade)
        {
            s_object_header *headers = (s_object_header *)object_header_data->data;
            object_datum *object = (object_datum *)headers[datum_index_to_absolute_index(object_index)].datum;
            long tick = game_time_globals->time - object->last_motion_time;
            distance = distance * render_objects_static_lighting_fade_scale((float)tick *
                game_time_globals->tick_length);
        }
        render_object_compute_fade(object_index, distance, &fade[0]);
        if (!cached_alpha)
        {
            fade[2] = 0;
        }
        else
        {
            if (fade[2] != 0)
            {
                long cached_index = object_get_cached_render_state(object_index);
                if (cached_index != -1)
                {
                    float brightness = PIN(
                        datum_get<s_visibility_cached_object_render_state>(g_cached_object_render_states, cached_index)
                            ->shader_info.lightprobe_sample.dominant_light_intensity,
                        k_render_objects_static_lighting_alpha_minimum, 8.0f);
                    fade[2] = (unsigned char)(((brightness - k_render_objects_static_lighting_alpha_minimum) *
                        (float)fade[2]) * 0.16666667f);
                }
            }
        }
    }
    else
    {
        fade[2] = 255;
        fade[1] = 255;
        fade[0] = 255;
    }
    if (cached_alpha && time_fade)
    {
        bool found = false;
        for (long index = 0; index < 4; ++index)
        {
            c_director *entry = g_director_globals->directors[index].get_director();
            if (entry->get_camera()->get_target() == object_index)
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            s_object_header *headers = (s_object_header *)object_header_data->data;
            unit_datum *object = (unit_datum *)headers[datum_index_to_absolute_index(object_index)].datum;
            long tick = game_time_globals->time - object->unit.last_active_time;
            float spawn_fade = 1.0f - ((float)tick * game_time_globals->tick_length -
                k_render_objects_static_lighting_alpha_minimum);
            float spawn_scale = PIN(spawn_fade, 0.0f, 1.0f);
            fade[2] = (unsigned char)((float)fade[2] * spawn_scale);
        }
    }
    if (object_type == 0)
    {
        s_object_header *headers = (s_object_header *)object_header_data->data;
        unit_datum *object = (unit_datum *)headers[datum_index_to_absolute_index(object_index)].datum;
        float factor = object->unit.active_camouflage;
        if (factor >= 0.0f && factor <= 1.0f)
        {
            fade[2] = (unsigned char)((1.0f - factor) * (float)fade[2]);
        }
    }
    if (fade[2] != 0 && ((1 << object_type) & 0x1003) != 0)
    {
        float function_value = 0.0f;
        bool active = false;
        bool extra;
        if (unit_compute_function_value(object_index, 142, -1, &function_value, &active, &extra))
        {
            fade[2] = (unsigned char)((1.0f - function_value) * (float)fade[2]);
        }
    }
    *(long *)alphas = *(long *)fade;
}

static __forceinline void render_objects_static_lighting_initialize_screen_effect(
    render_objects_screen_effect_settings *values, const float &zero, const float &one)
{
    values->channels[0] = zero;
    values->channels[1] = zero;
    values->channels[2] = zero;
    values->channels[3] = zero;
    values->channels[4] = zero;
    values->channels[5] = zero;
    values->channels[6] = zero;
    values->channels[7] = zero;
    values->color_filter.red = one;
    values->color_filter.green = one;
    values->color_filter.blue = one;
    real_rgb_color *color_floor = &values->color_floor;
    color_floor->red = zero;
    color_floor->green = zero;
    color_floor->blue = zero;
}

// @address 0x16aa78 render_objects_compute_screen_effect
extern "C" void render_objects_compute_screen_effect(const real_point3d *position, const real_vector3d *forward,
    render_objects_screen_effect_settings *values, long player_index)
{
    const float &zero = 0.0f;
    const float &one = 1.0f;
    scenario *scenario_definition = (scenario *)global_scenario;
    // match: 0x16aa78: value range assumption guides branch lowering
    __assume(scenario_definition != 0);
    render_objects_static_lighting_initialize_screen_effect(values, zero, one);
    long default_index = scenario_definition->screen_effect.index;
    if (default_index != -1)
    {
        render_objects_static_lighting_screen_effect_definition *definition =
            (render_objects_static_lighting_screen_effect_definition
                *)global_tag_instances[datum_index_to_absolute_index(default_index)].definition;
        if (definition->count > 0)
            render_static_lighting_accumulate_sample((float *)values, definition->effects[0].values, one);
    }

    for (long index = data_next_index(g_static_lighting_effect_attachments, -1); index != -1; index =
        inl_data_next_index(g_static_lighting_effect_attachments, index))
    {
        effects_node_attachment *attachment = &((effects_node_attachment
            *)g_static_lighting_effect_attachments->data)[datum_index_to_absolute_index(index)];
        // match: 0x16aa78: value range assumption guides branch lowering
        __assume(attachment != 0);
        real_vector3d displacement;
        displacement.k = attachment->world_point.z - position->z;
        displacement.i = attachment->world_point.x - position->x;
        displacement.j = attachment->world_point.y - position->y;
        float distance = sqrtf(displacement.k * displacement.k + (displacement.j * displacement.j + displacement.i *
            displacement.i));
        float angle = angle_between_vectors3d(&displacement, forward);
        render_objects_static_lighting_screen_effect_definition *definition =
            (render_objects_static_lighting_screen_effect_definition
                *)global_tag_instances[datum_index_to_absolute_index(attachment->definition_index)].definition;
        for (long effect_index = 0; effect_index < definition->count; ++effect_index)
        {
            render_objects_static_lighting_screen_effect *effect = &definition->effects[effect_index];
            if (effect->response.divisor <= zero || attachment->age <= effect->response.divisor)
            {
                bool apply = true;
                unsigned long flags = effect->response.flags;
                if ((flags & 4) != 0 || (flags & 8) != 0)
                {
                    long object_index = effects_node_attachment_try_and_get_object(attachment);
                    if (object_index != -1)
                    {
                        bool first_person = render_object_is_first_person(player_index, object_index);
                        if (((flags & 4) != 0 && !first_person) || ((flags & 8) != 0 && first_person))
                            apply = false;
                    }
                    else
                        apply = false;
                }
                if (apply)
                {
                    float weight = render_objects_compute_effect_weight(&effect->response, distance, angle,
                        attachment->age);
                    if (weight > zero)
                        render_static_lighting_accumulate_sample((float *)values, effect->values, weight);
                }
            }
        }
    }
}

static __forceinline long render_objects_static_lighting_allocate_visible_object()
{
    long index = c_visible_items::m_items.objects.count;
    if ((unsigned long)index < c_visible_items::m_items.objects.unknown)
    {
        ++c_visible_items::m_items.objects.count;
        return index;
    }
    return -1;
}

static __forceinline bool render_objects_static_lighting_test_visible_object(const __vector4 *planes, const
    object_datum *object)
{
    real_point3d center = object->bounding_center;
    return visibility_volume_test_sphere_planes(planes, &center, object->bounding_radius) != 0;
}

static __forceinline const __vector4 *render_objects_static_lighting_get_active_planes(const
    s_visibility_input *input)
{
    const __vector4 *planes = 0;
    if (input->projection_count > 0)
        planes = input->projection.volume.vector_planes;
    return planes;
}

// @address 0x15a168 render_objects_add_visible_object
extern "C" long render_objects_add_visible_object(long object_index, unsigned long view_index, float distance, float
    lighting_distance,
                            unsigned short *object_count, bool test_visibility, bool exclude_first_person, long
                                player_index)
{
    long result = -1;
    if (!exclude_first_person || !render_object_is_first_person(player_index, object_index))
    {
        result = render_objects_static_lighting_allocate_visible_object();
        if (result != -1)
        {
            s_render_objects_static_lighting_setup *info =
                &c_visible_items::m_items.objects.entries[result].render_setup;
            object_datum *object = object_get(object_index);
            bool visible = true;
            bool model_flag1 = false;
            bool model_flag2 = false;
            s_object_definition *definition =
                (s_object_definition
                    *)global_tag_instances[datum_index_to_absolute_index(object->definition_index)].definition;
            if (definition->model.index != -1)
            {
                s_model_definition *model =
                    (s_model_definition
                        *)global_tag_instances[datum_index_to_absolute_index(definition->model.index)].definition;
                model_flag1 = (model->flags & 2) != 0;
                model_flag2 = (model->flags & 4) != 0;
            }
            if (test_visibility)
            {
                s_visibility_input *input =
                    (s_visibility_input
                        *)g_visibility_globals.collection_input;
                if (input->projection_count > 0)
                {
                    visible =
                        render_objects_static_lighting_test_visible_object(render_objects_static_lighting_get_active_planes(input), object);
                }
            }
            render_object_initialize_setup(info);
            info->object_index = object_index;
            if (visible)
                info->flags |= 1;
            else
                info->flags &= ~1;
            if (object->first_widget_index != -1)
                info->flags |= 2;
            else
                info->flags &= ~2;
            if (model_flag1)
                info->flags |= 4;
            else
                info->flags &= ~4;
            if (model_flag2)
                info->flags |= 8;
            else
                info->flags &= ~8;
            if (!render_visibility_update_object(object_index, distance, lighting_distance, view_index,
                             (s_render_object_info *)info))
                --c_visible_items::m_items.objects.count;
            else
                ++*object_count;
            for (long child_index = object->first_child_object_index; child_index != -1;
                 child_index = object_get(child_index)->next_object_index)
            {
                render_objects_add_visible_object(child_index, view_index, distance, lighting_distance, object_count,
                             test_visibility, exclude_first_person, player_index);
            }
        }
    }
    return result;
}

static __forceinline s_object_marker_globals *render_objects_static_lighting_object_marker_globals_get()
{
    return &g_object_marker_globals[g_registered_thread_index - 1];
}

static __forceinline bool render_objects_static_lighting_object_mark(long object_index)
{
    s_object_marker_globals *markers = render_objects_static_lighting_object_marker_globals_get();
    long mark = markers->mark;
    long previous = markers->object_marks[datum_index_to_absolute_index(object_index)];
    markers->object_marks[datum_index_to_absolute_index(object_index)] = mark;
    return previous != mark;
}

static __forceinline void render_objects_static_lighting_object_mark_set_data(long object_index, long data)
{
    render_objects_static_lighting_object_marker_globals_get()->mark_data[datum_index_to_absolute_index(object_index)]
        = data;
}

static __forceinline long render_objects_static_lighting_allocate_visible_hierarchy()
{
    long index = c_visible_items::m_items.hierarchies.count;
    if ((unsigned long)index < c_visible_items::m_items.hierarchies.unknown)
    {
        ++c_visible_items::m_items.hierarchies.count;
        return index;
    }
    return -1;
}

// @address 0x159f78 render_objects_add_visible_hierarchy
extern "C" long render_objects_add_visible_hierarchy(long object_index, const real_point3d *origin, float
    distance_scale,
                            unsigned long view_index, bool lit, bool shadow, bool fully_contained, long,
                            render_objects_static_lighting_hierarchy_levels levels, bool compute_distances,
                            bool exclude_first_person, long player_index)
{
    long hierarchy_index = render_objects_static_lighting_allocate_visible_hierarchy();
    if (hierarchy_index != -1)
    {
        s_object_hierarchy *hierarchy = &c_visible_items::m_items.hierarchies.entries[hierarchy_index];
        if (render_objects_static_lighting_object_mark(object_index))
            render_objects_static_lighting_object_mark_set_data(object_index, hierarchy_index);

        hierarchy->flags = (lit ? 1 : 0) | (shadow ? 2 : 0) | (fully_contained ? 4 : 0);
        hierarchy->region_cluster_bitvector_index = -1;
        hierarchy->object_count = 0;
        hierarchy->levels = levels;

        object_datum *object = object_get(object_index);
        bool test_visibility = object->object_identifier.get_source() == 4;
        float distance = 0.0f;
        float lighting_distance = 1280.0f;
        if (compute_distances)
        {
            c_player_view *view = c_player_view::get_current();
            if (view)
            {
                lighting_distance = render_camera_compute_projected_diameter(&view->render_projection_state, origin,
                                                 object->scale * distance_scale);
                if (view->get_render_camera() != 0)
                {
                    real_vector3d vector;
                    vector.j = 0.0f;
                    vector_from_points3d(origin, &view->get_render_camera()->position, &vector);
                    distance = magnitude3d(&vector);
                }
            }
        }
        hierarchy->first_object = render_objects_add_visible_object(object_index, view_index, distance,
            lighting_distance,
                                               &hierarchy->object_count, test_visibility, exclude_first_person,
                                                   player_index);
    }
    return hierarchy_index;
}

static __forceinline s_object_header *render_objects_static_lighting_sample_header_in_array(s_object_header *headers,
    long object_index)
{
    return &headers[datum_index_to_absolute_index(object_index)];
}

static __forceinline s_object_header *render_objects_static_lighting_sample_header(long object_index)
{
    return render_objects_static_lighting_sample_header_in_array((s_object_header *)object_header_data->data,
        object_index);
}

static __forceinline bool render_objects_static_lighting_sample_model_flag(
    long model_index, unsigned long flag, const cache_file_tag_instance *tag_instances)
{
    return model_index != -1 &&
        (((const s_model_definition *)tag_instances[datum_index_to_absolute_index(model_index)].definition)->flags &
            flag) != 0;
}

static __forceinline bool render_objects_static_lighting_sample_sky(
    long (*const &getter)(long, s_geometry_sample *), long object_index, s_geometry_sample *sample)
{
    long status = getter(object_index,  sample);
    return _geometry_sample_result_sampled == status;
}

static __forceinline void render_objects_static_lighting_sample_compressed(
    const s_lightmap_half_rgb_probe *probe, long object_index, s_geometry_sample *sample)
{
    real_point3d point = object_get(object_index)->bounding_center;
    render_objects_decode_lightprobe(probe, &point,  sample);
}

// @address 0x15ca08 render_objects_sample_lighting
extern "C" bool render_objects_sample_lighting(long object_index, bool flag, s_geometry_sample *sample, long unused)
{
    object_datum *object = object_get(object_index);
    s_object_header *header = render_objects_static_lighting_sample_header(object_index);
    const s_object_definition *definition =
        (const s_object_definition
            *)global_tag_instances[datum_index_to_absolute_index(object->definition_index)].definition;
    long model_index = definition->model.index;
    bool sky_only = render_objects_static_lighting_sample_model_flag(model_index, 8, global_tag_instances);
    // match: 0x15ca08: const reference binding preserves temporary lifetime
    const unsigned long &effects_only_value = (unsigned
        char)render_objects_static_lighting_sample_model_flag(model_index, 32, global_tag_instances);
    // match: 0x15ca08: const reference binding preserves temporary lifetime
    const unsigned char &effects_only = (unsigned char)effects_only_value;
    if (sky_only)
        return render_objects_static_lighting_sample_sky(render_objects_get_sky_lightprobe, object_index, sample);

    bool sampled = false;
    const scenario *scenario_definition =
        (const scenario *)global_scenario;
    if (effects_only && render_objects_sample_effect_lightprobe(0, object_index, &object->bounding_center,
        (s_effect_lightprobe_result *)sample) == 0)
        sampled = true;
    const object_datum *lighting = (const object_datum *)object;
    if (!sampled && lighting->cinematic_object_index != -1 && scenario_definition->lightmap.index != -1)
    {
        if (lighting->cinematic_object_index == 30000)
        {
            if (render_objects_sample_effect_lightprobe(0, object_index, &object->bounding_center,
                (s_effect_lightprobe_result *)sample) == 0)
                sampled = true;
        }
        else if (lighting->cinematic_object_index >= 0)
        {
            if (geometry_sampling_get_lightmap_probe(&object->bounding_center, lighting->cinematic_object_index,
                (s_geometry_sampling_probe *)sample) == 0)
                sampled = true;
        }
    }
    if (!sampled && lighting->cinematic_light_index != -1 && scenario_definition->lightmap.index != -1)
    {
        s_object_header *sample_header = render_objects_static_lighting_sample_header(object_index);
        const s_lightmap_half_rgb_probe *probe;
        short object_type = sample_header->object_type;
        if (object_type == 6)
        {
            if (!(header->flags & 16))
            {
                const s_scenario_lightmap_definition *lightmap =
                    (const s_scenario_lightmap_definition
                        *)global_tag_instances[datum_index_to_absolute_index(scenario_definition->lightmap.index)].definition;
                probe = &((const s_lightmap_scenery_probe
                    *)lightmap->scenery_light_probes.address)[lighting->cinematic_light_index].probe;
                render_objects_static_lighting_sample_compressed(probe, object_index, sample);
                sampled = true;
            }
        }
        else if (object_type == 7)
        {
            const s_scenario_lightmap_definition *lightmap =
                (const s_scenario_lightmap_definition
                    *)global_tag_instances[datum_index_to_absolute_index(scenario_definition->lightmap.index)].definition;
            const s_lightmap_machine_probe *machine = &((const s_lightmap_machine_probe
                *)lightmap->machine_light_probes.address)[lighting->cinematic_light_index];
            if (!(header->flags & 16) && machine->probe_count > 0)
            {
                probe = &machine->probes[0].probe;
                render_objects_static_lighting_sample_compressed(probe, object_index, sample);
                sampled = true;
            }
        }
    }
    if (!sampled)
    {
        long flags = 0;
        long needs_fallback = false;
        bool vertical_only = false;
        if (definition->object_type == 1)
        {
            if ((1 << vehicle_get_type(object_index)) & 0x94)
                vertical_only = true;
        }
        else if (definition->object_type == 0)
        {
            if (((const biped_datum
                *)object_get(object_index))->biped.physics.get_mode() == 2)
                vertical_only = true;
        }
        else if (definition->object_type == 11)
        {
            const object_datum *giant = object_get(object_index);
            const s_giant_definition *giant_definition =
                (const s_giant_definition
                    *)global_tag_instances[datum_index_to_absolute_index(giant->definition_index)].definition;
            typedef char light_toggle_name_offset[(offsetof(s_giant_definition, integrated_light_toggle.name) == 0x110) ? 1 : -1];
            unsigned long light_toggle_name_bits;
            memcpy(&light_toggle_name_bits, &giant_definition->integrated_light_toggle.name, sizeof(light_toggle_name_bits));
            if ((bool)((light_toggle_name_bits >> 4) & 1))
                vertical_only = true;
        }
        if ((bool)((object->flags >> 13) & 1))
            flags |= 1;
        if ((bool)((definition->flags >> 1) & 1))
            flags |= 1;
        bool unlit = false;
        long result = lights_distant_lighting_at_point_new(flags, object_index, object_index, flag,
            &object->bounding_center, sample, 0, vertical_only);
        if (result == 1 || result == 2 || result == 4 || result == 3)
        {
            needs_fallback = true;
            if (result == 3)
                unlit = true;
        }
        if ((unsigned char)needs_fallback && !vertical_only && (flags & 1))
        {
            flags |= 1;
            result = lights_distant_lighting_at_point_new(flags, object_index, object_index, flag,
                &object->bounding_center, sample, 0, vertical_only);
            needs_fallback = result == 1 || result == 2 || result == 4 || result == 3;
            if (result == 3)
                unlit = true;
        }
        if ((unsigned char)needs_fallback && result != 4 && !(unlit && (unsigned char)unused))
        {
            result = render_objects_sample_effect_lightprobe(flags, object_index, &object->bounding_center,
                (s_effect_lightprobe_result *)sample);
            needs_fallback = result == 1 || result == 2 || result == 4 || result == 3;
        }
        if ((unsigned char)needs_fallback && !(unlit && (unsigned char)unused))
        {
            if (render_objects_get_sky_lightprobe(object_index,  sample) == 0)
                needs_fallback = false;
        }
        return !(unsigned char)needs_fallback;
    }
    return true;
}

// @rdata 0x8202490c
static const float k_render_objects_static_lighting_refresh_epsilon = 0.0001f;
// @rdata 0x82022dbc
static const float k_render_objects_static_lighting_refresh_cubemap_rate = 0.1f;
// @rdata 0x8203385c
static const float k_render_objects_static_lighting_refresh_rate = 0.2f;

static __forceinline void render_objects_static_lighting_refresh_cubemap(c_dynamic_cubemap_sample *sample, float rate)
{
    sample->m_blend_factor += rate;
    if (sample->m_blend_factor > 1.0f)
        sample->m_blend_factor = 1.0f;
}

static __forceinline float render_objects_static_lighting_refresh_magnitude3d(const real_vector3d *vector)
{
    return sqrtf(vector->i * vector->i + vector->j * vector->j + vector->k * vector->k);
}

static __forceinline float render_objects_static_lighting_refresh_distance3d(const real_point3d *a, const real_point3d
    *b)
{
    real_vector3d delta;
    delta.j = 0.0f;
    delta.k = 0.0f;
    vector_from_points3d(a, b, &delta);
    return render_objects_static_lighting_refresh_magnitude3d(&delta);
}

// @address 0x44ec60 render_objects_refresh_lighting
extern "C" void render_objects_refresh_lighting(long cached_index, long object_index, float value)
{
    s_visibility_cached_object_render_state *state =
        (s_visibility_cached_object_render_state *)render_objects_static_lighting_cached_get(cached_index);
    long frames_since_sample = c_render_globals::get_frame_index() - state->sample_frame;
    object_datum *object = object_get(object_index);
    float radius = object->bounding_radius;
    bool is_sky = false;
    if (((1 << ((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_index)].object_type)
        & 64)
        && (((s_scenery_object *)object)->flags & 4))
        is_sky = true;
    if (is_sky)
    {
        const s_object_definition *definition =
            (const s_object_definition
                *)global_tag_instances[datum_index_to_absolute_index(object->definition_index)].definition;
        const s_model_definition *model =
            (const s_model_definition
                *)global_tag_instances[datum_index_to_absolute_index(definition->model.index)].definition;
        const render_model_definition *render_model =
            (const render_model_definition
                *)global_tag_instances[datum_index_to_absolute_index(model->render_model_index)].definition;
        render_lighting *lighting = &state->shader_info.lighting;
        memcpy(lighting->sample.red, render_model->ravi_red, 9 * sizeof(*lighting->sample.red));
        memcpy(lighting->sample.green, render_model->ravi_green, 9 * sizeof(*lighting->sample.green));
        memcpy(lighting->sample.blue, render_model->ravi_blue, 9 * sizeof(*lighting->sample.blue));
        real_vector3d direction;
        real_rgb_color color;
        float intensity;
        calculate_dominant_light_from_lightprobe(render_model->ravi_red, render_model->ravi_green,
            render_model->ravi_blue, &direction, &color, &intensity);
        lighting->down = direction;
        lighting->sample.dominant_light_direction = direction;
        lighting->sample.dominant_light_color = color;
        lighting->sample.dominant_light_intensity = intensity;
        state->valid = true;
        state->converged = true;
    }
    else if (frames_since_sample)
    {
        long frames_since_update = c_render_globals::get_frame_index() - state->update_frame;
        bool refresh_sample = false;
        bool refresh_lighting = false;
        bool snap = false;
        bool invalid = !state->valid || !state->sample_valid;
        if (frames_since_sample < 0)
            frames_since_sample = 1;
        float distance = render_objects_static_lighting_refresh_distance3d(&state->sample_position,
            &object->bounding_center);
        bool stale;
        if (state->update_frame == -1 || frames_since_update > 1)
            stale = true;
        else
            stale = false;
        if (invalid)
        {
            refresh_sample = true;
            refresh_lighting = true;
            snap = true;
        }
        else
        {
            float distance_threshold = MAX(0.05f, MAX(k_render_objects_static_lighting_refresh_rate /
                MAX((float)frames_since_sample, 1.0f), 0.05f) * radius);
            if (distance > distance_threshold && frames_since_sample > 2)
            {
                refresh_sample = true;
                refresh_lighting = true;
            }
            if (!stale)
            {
                real_vector3d velocity;
                object_get_velocities(object_index, &velocity, 0);
                if (!(bool)((object->flags >> 21) & 1) &&
                    sqrtf((velocity.j * velocity.j + velocity.i * velocity.i) + velocity.k * velocity.k) <
                        k_render_objects_static_lighting_refresh_epsilon)
                {
                    snap = true;
                    refresh_sample = false;
                    refresh_lighting = false;
                    state->converged = true;
                }
            }
        }
        if (!invalid && refresh_sample && state->sample_frame != -1 && frames_since_update < 2)
        {
            if (value > 400.0f)
                refresh_sample = frames_since_sample > 2;
            else if (value > 200.0f)
                refresh_sample = frames_since_sample > 5;
            else if (value > 100.0f)
                refresh_sample = frames_since_sample > 8;
            else if (value > 50.0f)
                refresh_sample = frames_since_sample > 15;
            else if (value > 10.0f)
                refresh_sample = frames_since_sample > 45;
            else
                refresh_sample = frames_since_sample > 60;
        }
        if (refresh_lighting && stale)
            snap = true;
        if (refresh_sample)
        {
            state->shader_info.cubemap_sample.update(&object->bounding_center,
                k_render_objects_static_lighting_refresh_cubemap_rate);
            state->sample_frame = c_render_globals::get_frame_index();
            if (render_objects_sample_lighting(object_index, invalid, &state->target_sample, state->valid))
            {
                state->sample_valid = true;
                state->sample_position = object->bounding_center;
                s_geometry_sample *sample = &state->target_sample;
                state->target_down.i = -sample->dominant_light_direction.i;
                state->target_down.j = -sample->dominant_light_direction.j;
                state->target_down.k = -sample->dominant_light_direction.k;
                if (magnitude3d(&state->target_down) < 0.01f)
                    state->target_down = *global_down3d;
                normalize3d(&state->target_down);
                state->converged = false;
                if (!state->target_sample.valid)
                    snap = true;
            }
        }
        if ((refresh_lighting && state->sample_valid) || !state->converged)
        {
            if (state->valid && !snap)
            {
                render_lighting *lighting = &state->shader_info.lighting;
                render_lighting *target = &state->target_lighting;
                const float minimum_speed = 0.15f;
                const float maximum_speed = 30.0f;
                real_vector3d velocity;
                object_get_velocities(object_index, &velocity, 0);
                float speed = sqrtf((velocity.j * velocity.j + velocity.i * velocity.i) + velocity.k * velocity.k);
                float fraction = (speed - minimum_speed) / (maximum_speed - minimum_speed);
                fraction = PIN(fraction, 0.0f, 1.0f);
                fraction /= (radius > k_render_objects_static_lighting_refresh_epsilon ? radius : 1.0f);
                fraction = PIN(fraction, 0.0f, 1.0f);
                float rate = fraction * k_render_objects_static_lighting_refresh_rate;
                rate *= (simulation_starting_up() ? game_time_globals->rate_scale : 1.0f)
                    * (game_is_cooperative() ? g_saved_film_timer_globals.playback_speed : 1.0f);
                rate = MAX(rate, 0.08f);
                state->converged = render_objects_interpolate_lightprobe(&lighting->sample,
                    &target->sample, MAX(0.0f, MIN(rate, 1.0f)), 0);
                render_objects_static_lighting_refresh_cubemap(&lighting->cubemap, rate * 0.5f);
                interpolate_vector3d(&lighting->down, &target->down, rate *
                    k_render_objects_static_lighting_refresh_cubemap_rate);
            }
            else
            {
                state->shader_info.down = state->target_down;
                state->shader_info.lightprobe_sample = state->target_sample;
                state->shader_info.instance_index = state->instance_index;
                state->converged = true;
            }
            state->valid = true;
        }
        else
        {
            render_objects_static_lighting_refresh_cubemap(&state->shader_info.cubemap_sample, 1.0f);
        }
    }
    state->update_frame = c_render_globals::get_frame_index();
    state->shader_info.down.k = -fabsf(state->shader_info.down.k);
    if (state->shader_info.down.k > rasterizer_states_mode_5_slope_scale_bias)
        state->shader_info.down.k = rasterizer_states_mode_5_slope_scale_bias;
}

typedef t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_cached_object_render_states_allocator>
    t_cached_object_render_states_allocator;
t_cached_object_render_states_allocator g_cached_object_render_states_allocator;

extern "C" {
__declspec(thread) s_data_array *g_cached_object_render_states;
}

