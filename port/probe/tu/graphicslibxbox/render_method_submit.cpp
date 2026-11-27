// graphicslibxbox/render_method_submit.obj: from the records 0x16ff90 0x170020 0x1711d0 0x1712d0 0x1713f0 0x171508 0x1717a0 0x1718e0 0x171968 0x171f30 0x1722f0 0x172b88 0x172fa8 0x173228 0x1732d8 0x173350 0x173558 0x174830 0x174950 0x174a28 0x174ac0 0x174bf0 0x17a320 0x17a4d8 0x185ff0 0x44e008 0x457368 0x4574c0 0x4579c8 0x457ba0 0x458438 0x45a5f0 0x48d2e0 0x48d3c0
#include "rasterizer/rasterizer_main_globals.h"
#include "rasterizer/rasterizer_device_states.h"
#include "render/views/player_view_types.h"
#include "rasterizer/rasterizer_implicit_geometry.h"
#include "geometry/geometry_definitions.h"
#include "rasterizer/xenon/rasterizer_xenon_submit.h"
#include "rasterizer/rasterizer_state_globals.h"
#include "rasterizer/rasterizer_device_globals.h"
#include "cache/cache_file_tag_resource_runtime_interface.h"
#include "cache/geometry_storage.h"
#include "render/render_player_view_globals.h"
#include "render/render_texture_camera_view_globals.h"
#include "render_methods/render_method_submit_globals.h"
#include "render/render_objects.h"
#include "render_methods/render_method_data.h"
#include "scenario/cluster_reference_helpers.h"
#include "math/real_math_types.h"
#include "memory/datum_index.h"
#include "tag_files/tag_types.h"
#include "game/game_globals_storage.h"
#include <xtl.h>
#include "rasterizer/rasterizer_primitives.h"
#include "multithreading/thread_tls.h"
#include <string.h>
#include "game/game_time.h"
#include "render_methods/render_method_submit.h"
#include "render_methods/render_method_submit_geometry.h"
#include "render_methods/render_method_submit_dependencies.h"
#include "scenario/scenario_interpolators.h"
#include "render/render_mesh.h"
#include "interface/c_gui_widget.h"
#include "interface/gui_animation_transform.h"
#include "interface/gui_widget_keyframes.h"
#include "rasterizer/rasterizer_resource_definitions.h"
#include "rasterizer/rasterizer_states.h"
#include "render/views/render_player_view.h"
#include "game/players.h"
#include "render/render_structure.h"
#include "sky_atm/atmosphere.h"
#include "geometry/geometry_sampling_types.h"
#include "rasterizer/xenon/rasterizer_xenon_memory_request_types.h"
#include "render/shader_reference_types.h"
#include "rasterizer/rasterizer_tag_instance_types.h"
#include "scenario/scenario_interpolator_types.h"
#include "cache/cache_file_globals.h"
#include "scenario/scenario.h"
#include "rasterizer/global_shader_parameter_types.h"
#include "rasterizer/rasterizer_render_globals_types.h"
#include "math/function_definitions.h"
#include "rasterizer/rasterizer_vertex_buffer.h"
#include "bitmaps/bitmap_data.h"
#include "rasterizer/rasterizer_vertex_types.h"
#include "render_methods/render_method_definitions.h"
#include "scenario/scenario_tag.h"
#include "bitmaps/bitmap_group.h"
#include "bitmaps/bitmap_group_definitions.h"
#include "render/render_camera_types.h"
#include "rasterizer/rasterizer_global_definitions.h"
#include "rasterizer/rasterizer_implicit_geometry_types.h"
#include "geometry/geometry_material_types.h"
#include "rasterizer/rasterizer_textures_cache.h"
#include "rasterizer/rasterizer.h"
#include "cubemaps/cubemap_sample.h"
#include "render/shader_extern_info_types.h"
#include "geometry/render_geometry_parts.h"
#include "rasterizer/xenon/rasterizer_xenon_memory_types.h"
#include "rasterizer/xenon/rasterizer_xenon_memory.h"
#include "rasterizer/rasterizer_compiled_shader_types.h"
#include "render_methods/render_method_shader_definition_types.h"
#include "rasterizer/rasterizer_vertex_definitions.h"
#include "rasterizer/rasterizer_shader_definitions.h"
#include "render_methods/render_method.h"
#include "rasterizer/rasterizer_stencil.h"
#include "rasterizer/rasterizer_index_buffer.h"
#include "render/render_geometry.h"
#include "geometry/render_geometry_types.h"
#include "geometry/render_geometry_mesh_types.h"
#include "game/game_globals.h"
#include "scenario/scenario_access.h"
#include "scenario/scenario_interpolator_definition.h"
#include "sky_atm/atmosphere_fog_interface.h"
#include "memory/data.h"
#include "render/render_texture_camera_types.h"
#include "rasterizer/rasterizer_dynamic_render_target_types.h"
#include "rasterizer/rasterizer_render_target_types.h"
#include "render/render_view.h"
#include "render/render.h"
#include "scenario/scenario_lightmap_types.h"
#include "render/render_structure_globals_types.h"
#include "rasterizer/rasterizer_render_targets.h"
#include "memory/tls_globals.h"
#include "objects/emblem_info.h"
#include "objects/emblems.h"
#include "rasterizer/rasterizer_shader_tag_types.h"
#include "cseries/pointer_utilities.h"
#include "rasterizer/xenon/d3d_vertex_shader.h"
struct s_rm_category_dependency { long category; s_tag_block shaders; };
struct s_render_method_submit_declaration
{
    long position;
    long reserved0;
    long secondary;
    long reserved1;
    long lighting;
};
struct s_destination_info { short constant_index; short register_index; };

namespace
{
// Scope tag-backed plans to the frame that acquired them. Map changes can reuse
// the tag allocation, so check the loaded cache and the template datum as well.
struct s_binding_plan_epoch
{
    const cache_file_tag_instance *tags;
    long checksum;
    long frame;
    unsigned __int64 generation;
};

s_binding_plan_epoch g_binding_plan_epoch;

struct s_submitted_shader_state
{
    const void *shader_template;
    long template_index;
    unsigned short template_salt;
    e_transfer_vector_vertex_types transfer_type;
    unsigned __int64 generation;
};

s_submitted_shader_state g_submitted_shader_state;

static __forceinline unsigned __int64 render_method_submit_plan_generation()
{
    const long frame = c_render_globals::get_frame_index();
    const long checksum = cache_files_get_header()->checksum;
    if (g_binding_plan_epoch.tags != global_tag_instances || g_binding_plan_epoch.checksum != checksum ||
        g_binding_plan_epoch.frame != frame)
    {
        g_binding_plan_epoch.tags = global_tag_instances;
        g_binding_plan_epoch.checksum = checksum;
        g_binding_plan_epoch.frame = frame;
        ++g_binding_plan_epoch.generation;
    }
    return g_binding_plan_epoch.generation;
}

enum e_binding_range
{
    _binding_texture,
    _binding_pixel_constant,
    _binding_integer_constant,
    _binding_vertex_constant,
    _binding_boolean_constant,
    _binding_extern_sampler,
    _binding_pixel_extern,
    _binding_vertex_extern,
    k_binding_range_count
};

struct s_binding_route
{
    unsigned short destination;
    unsigned char source;
    union
    {
        unsigned char sampler_offset;
        signed char extern_type;
    };
};

struct s_binding_range
{
    const s_binding_route *first;
    const s_binding_route *last;

    const s_binding_route *begin() const { return first; }
    const s_binding_route *end() const { return last; }
};

struct s_render_method_binding_plan
{
    const rasterizer_implicit_geometry_template *shader_template;
    const void *entry_points;
    const void *entries;
    const void *bindings;
    long template_index;
    long entry_point;
    unsigned __int64 generation;
    unsigned short template_salt;
    s_binding_range ranges[k_binding_range_count];
    // Each packed range has six count bits.
    s_binding_route routes[k_binding_range_count * 63];
};

constexpr unsigned int k_binding_plan_count = 128;
s_render_method_binding_plan g_binding_plans[k_binding_plan_count];
s_render_method_binding_plan *g_last_binding_plan;

static const s_render_method_binding_plan &render_method_submit_get_binding_plan(
    const rasterizer_implicit_geometry_template *shader_template, long template_index, long entry_point)
{
    const auto template_slot = DATUM_INDEX_TO_ABSOLUTE_INDEX(template_index);
    const auto salt = template_index != NONE ? global_tag_instances[template_slot].salt : 0;
    const auto generation = render_method_submit_plan_generation();
    const auto *entry_points = (const unsigned short *)shader_template->entry_points.address;
    const auto *entries = (const rasterizer_implicit_geometry_entry *)shader_template->entries.address;
    const auto *bindings = (const rasterizer_implicit_geometry_binding *)shader_template->bindings.address;
    const auto matches = [&](const s_render_method_binding_plan &plan)
    {
        return plan.shader_template == shader_template && plan.template_index == template_index && plan.entry_point == entry_point &&
            plan.generation == generation && plan.template_salt == salt && plan.entry_points == entry_points &&
            plan.entries == entries && plan.bindings == bindings;
    };
    const auto *last = g_last_binding_plan;
    if (last && matches(*last))
        return *last;

    const unsigned int bucket = (template_slot ^ (template_slot >> 7) ^ (entry_point * 37)) & (k_binding_plan_count - 1);
    auto &plan = g_binding_plans[bucket];
    g_last_binding_plan = &plan;
    if (&plan != last && matches(plan))
        return plan;

    const auto &entry = entries[entry_points[entry_point] & 0x3ff];
    const unsigned short packed_ranges[k_binding_range_count] =
    {
        entry.textures, entry.pixel_constants, entry.integer_constants, entry.vertex_constants, entry.boolean_constants,
        entry.samplers, entry.pixel_externs, entry.vertex_externs
    };

    auto *output = plan.routes;
    for (unsigned int range = 0; range < k_binding_range_count; ++range)
    {
        const unsigned short packed = packed_ranges[range];
        const unsigned int count = packed >> 10;
        plan.ranges[range] = { output, output + count };
        for (unsigned int index = 0; index < count; ++index)
        {
            const auto &binding = bindings[(packed & 0x3ff) + index];
            output->destination = binding.destination;
            output->source = binding.source;
            if (range >= _binding_extern_sampler)
                output->extern_type = binding.source < k_number_of_render_method_externs
                    ? k_global_shader_parameters[binding.source].type : -1;
            else
                output->sampler_offset = range == _binding_texture && (binding.flags & 1) ? 20 : 0;
            ++output;
        }
    }
    plan.shader_template = shader_template;
    plan.entry_points = entry_points;
    plan.entries = entries;
    plan.bindings = bindings;
    plan.template_index = template_index;
    plan.template_salt = salt;
    plan.entry_point = entry_point;
    plan.generation = generation;
    return plan;
}
}

// @address 0x221fc k_real_constant_pool
// @address 0x74b898 g_geometry_globals
// @address 0x7c6178 g_rasterizer_sampler_filter_modes
// @address 0x8c46dc ?x_current_player_view@c_player_view@@1PAV1@A
// @address 0x195e88 rasterizer_device_set_sampler_filter_modes
// @address 0x1950c8 D3DDevice_BlockOnFence
// @flags /TP /O2 /Oc
// @address 0x72cb03 g_render_hiz_stencil_enabled
// @address 0x5a3d80 D3DResource_IsBusy
// @address 0x825d90 g_render_method_submit_extern_constant_47
#if !defined(_XBOX) || defined(__clang__)
// PC port only: the 16 bytes at 0x82825d90 are zero in the shipped image (verified); the Xbox build resolves this tag to its linked address.
real_vector4d g_render_method_submit_extern_constant_47 = { 0.0f, 0.0f, 0.0f, 0.0f };
#endif
// @address 0x172570 D3DDevice_RunCommandBuffer
// @address 0x5ab8c0 D3DVertexShader_Bind
// @address 0x5ab218 D3DDevice_EndCommandBuffer
// @address 0x5aada0 D3DDevice_BeginCommandBuffer
// @address 0x5aaba8 D3DCommandBuffer_GetClone
class c_render_method;
// @address 0x174ac0 ?get_compiled_pixel_shader@c_render_method@@QBAPBVc_rasterizer_compiled_pixel_shader@@W4e_entry_point@@@Z
struct s_mesh;
struct s_part;
class c_render_method;
struct s_rasterizer_entry;
struct s_mesh;
// match: 0x1718e0: other callers retain incompatible C-linkage mesh parameter types.
// @address 0x16ff90 ?render_method_submit_set_texture@@YA_NJJJ@Z
bool render_method_submit_set_texture(long sampler, long bitmap_group_index, long bitmap_index)
{
    bool result = false;
    if (bitmap_group_index != -1 && bitmap_index != -1)
    {
        c_rasterizer_texture_ref texture_ref;
        texture_ref.clear();
        const bitmap_data *entry;
        if (::bitmap_group_get_xenon_hardware_format(bitmap_group_index, bitmap_index, &texture_ref, &entry, (__int64 *)(0)))
        {
            c_rasterizer::set_sampler_texture(sampler, texture_ref);
            result = true;
        }
    }
    return result;
}
// @address 0x825fa0 ?m_change_colors@c_render_method_data@@2PAKA
unsigned long c_render_method_data::m_change_colors[4];
// @address 0x8c46eb ?m_valid@c_render_method_data@@2_NA
bool c_render_method_data::m_valid;
// @address 0x8c4708 ?m_cubemap_blend_factor@c_render_method_data@@2MA
float c_render_method_data::m_cubemap_blend_factor;
// @address 0x8c470c ?m_context_interface@c_render_method_data@@2P6AMJJJ@ZA
float (*c_render_method_data::m_context_interface)(long, long, long);
// @address 0x738d1c ?m_lightmap_bsp_index@c_render_method_data@@2JA
long c_render_method_data::m_lightmap_bsp_index = -1;
// @address 0x7bbaa4 ?m_lightmap_cluster_reference@c_render_method_data@@2Us_cluster_reference@@A
s_cluster_reference c_render_method_data::m_lightmap_cluster_reference = { -1, 0xff };
// @address 0x737704 ?m_lightmap_instance_index@c_render_method_data@@2JA
long c_render_method_data::m_lightmap_instance_index = -1;
// @address 0x7bbaa8 ?m_cubemap_dynamic_0_cluster_reference@c_render_method_data@@2Us_cluster_reference@@A
s_cluster_reference c_render_method_data::m_cubemap_dynamic_0_cluster_reference = { -1, 0xff };
// @address 0x73a150 ?m_cubemap_dynamic_0_index@c_render_method_data@@2JA
long c_render_method_data::m_cubemap_dynamic_0_index = -1;
// @address 0x7bbaac ?m_cubemap_dynamic_1_cluster_reference@c_render_method_data@@2Us_cluster_reference@@A
s_cluster_reference c_render_method_data::m_cubemap_dynamic_1_cluster_reference = { -1, 0xff };
// @address 0x73a154 ?m_cubemap_dynamic_1_index@c_render_method_data@@2JA
long c_render_method_data::m_cubemap_dynamic_1_index = -1;
// @address 0x73a158 ?m_context@c_render_method_data@@2JA
long c_render_method_data::m_context = -1;
// @address 0x170020 ?render_method_submit_valid@@YAXXZ
void render_method_submit_valid()
{
    c_render_method_data::m_lightmap_bsp_index = -1;
    c_render_method_data::m_cubemap_blend_factor = 0.0f;
    c_render_method_data::m_lightmap_cluster_reference = invalid_cluster_reference_get();
    c_render_method_data::m_lightmap_instance_index = -1;
    c_render_method_data::m_cubemap_dynamic_0_cluster_reference = invalid_cluster_reference_get();
    c_render_method_data::m_cubemap_dynamic_0_index = -1;
    c_render_method_data::m_cubemap_dynamic_1_cluster_reference = invalid_cluster_reference_get();
    c_render_method_data::m_cubemap_dynamic_1_index = -1;
    c_render_method_data::m_context_interface = 0;
    c_render_method_data::m_context = -1;
}
extern "C" {
// @address 0x73a15c g_render_method_submit_cached_definition_index
long g_render_method_submit_cached_definition_index = -1;
// @address 0x8c482f g_render_method_submit_cached_state_enabled
unsigned char g_render_method_submit_cached_state_enabled;
// @address 0x8c4838 g_render_method_submit_state_dirty
unsigned char g_render_method_submit_state_dirty;
// @address 0x730e6c g_render_method_submit_light_direction
real_vector4d g_render_method_submit_light_direction = { 0.0f, 0.0f, 1.0f, 1.0f };
// @address 0x730e7c g_render_method_submit_light_intensity
real_rgb_color g_render_method_submit_light_intensity = { 1.0f, 1.0f, 1.0f };
// @address 0x119a4a0 g_render_method_submit_cluster_color
real_argb_color g_render_method_submit_cluster_color;
// @address 0x11db06c g_render_method_submit_loop_phase
float g_render_method_submit_loop_phase;
// @address 0x11db070 g_render_method_submit_loop_phase_tick
long g_render_method_submit_loop_phase_tick;
}
// @address 0x25088 k_global_shader_parameters
static __forceinline void render_method_submit_set_data(float cubemap_blend_factor, long structure_bsp_index,
    s_cluster_reference lightmap_cluster_reference, long lightmap_instance_index,
    s_cluster_reference cubemap_dynamic_0_cluster_reference, long cubemap_dynamic_0_index,
    s_cluster_reference cubemap_dynamic_1_cluster_reference, long cubemap_dynamic_1_index, long unused,
    const s_shader_reference *change_colors, long context, float (*context_interface)(long, long, long))
{
    c_render_method_data::m_valid = true;
    c_render_method_data::m_cubemap_blend_factor = cubemap_blend_factor;
    c_render_method_data::m_lightmap_bsp_index = structure_bsp_index;
    c_render_method_data::m_lightmap_cluster_reference = lightmap_cluster_reference;
    c_render_method_data::m_lightmap_instance_index = lightmap_instance_index;
    c_render_method_data::m_cubemap_dynamic_0_cluster_reference = cubemap_dynamic_0_cluster_reference;
    c_render_method_data::m_cubemap_dynamic_0_index = cubemap_dynamic_0_index;
    c_render_method_data::m_cubemap_dynamic_1_cluster_reference = cubemap_dynamic_1_cluster_reference;
    c_render_method_data::m_cubemap_dynamic_1_index = cubemap_dynamic_1_index;
    c_render_method_data::m_context_interface = context_interface;
    c_render_method_data::m_context = context;
    if (change_colors)
        memcpy(c_render_method_data::m_change_colors, change_colors->colors, sizeof(c_render_method_data::m_change_colors));
    if (structure_bsp_index != -1 && (lightmap_cluster_reference.bsp_index != -1 || lightmap_instance_index != -1)
        && structure_bsp_index != g_render_method_submit_cached_definition_index)
    {
        for (long i = 0; i < 49; i++)
        {
            if (k_global_shader_parameters[i].type == 2)
                rasterizer_set_global_shader_parameter(i);
        }
        g_render_method_submit_cached_definition_index = structure_bsp_index;
    }
}
// @address 0x1712d0 render_method_submit_set_structure_data
extern "C" void render_method_submit_set_structure_data(bool valid, long structure_bsp_index,
    s_cluster_reference lightmap_cluster_reference, long lightmap_instance_index,
    s_cluster_reference cubemap_dynamic_0_cluster_reference, long cubemap_dynamic_0_index,
    s_cluster_reference cubemap_dynamic_1_cluster_reference, long cubemap_dynamic_1_index, float cubemap_blend_factor,
    const s_shader_reference *change_colors, long context, float (*context_interface)(long, long, long))
{
    render_method_submit_set_data(cubemap_blend_factor, structure_bsp_index, lightmap_cluster_reference, lightmap_instance_index,
        cubemap_dynamic_0_cluster_reference, cubemap_dynamic_0_index, cubemap_dynamic_1_cluster_reference, cubemap_dynamic_1_index,
        valid, change_colors, context, context_interface);
}
static __forceinline void render_method_submit_set_data_from_extern(float cubemap_blend_factor, long structure_bsp_index,
    s_cluster_reference lightmap_cluster_reference, long lightmap_instance_index,
    s_cluster_reference cubemap_dynamic_0_cluster_reference, long cubemap_dynamic_0_index,
    s_cluster_reference cubemap_dynamic_1_cluster_reference, long cubemap_dynamic_1_index, long unused,
    const s_shader_reference *change_colors, long context, float (*context_interface)(long, long, long))
{
    c_render_method_data::m_valid = true;
    c_render_method_data::m_lightmap_bsp_index = structure_bsp_index;
    c_render_method_data::m_lightmap_cluster_reference = lightmap_cluster_reference;
    c_render_method_data::m_lightmap_instance_index = lightmap_instance_index;
    c_render_method_data::m_cubemap_dynamic_0_cluster_reference = cubemap_dynamic_0_cluster_reference;
    c_render_method_data::m_cubemap_dynamic_1_cluster_reference = cubemap_dynamic_1_cluster_reference;
    c_render_method_data::m_cubemap_blend_factor = cubemap_blend_factor;
    c_render_method_data::m_context_interface = context_interface;
    c_render_method_data::m_cubemap_dynamic_1_index = cubemap_dynamic_1_index;
    c_render_method_data::m_cubemap_dynamic_0_index = cubemap_dynamic_0_index;
    c_render_method_data::m_context = context;
    if (change_colors)
        memcpy(c_render_method_data::m_change_colors, change_colors->colors, sizeof(c_render_method_data::m_change_colors));
    if (structure_bsp_index != -1 && (lightmap_cluster_reference.bsp_index != -1 || lightmap_instance_index != -1)
        && structure_bsp_index != g_render_method_submit_cached_definition_index)
    {
        for (long i = 0; i < 49; i++)
        {
            if (k_global_shader_parameters[i].type == 2)
                rasterizer_set_global_shader_parameter(i);
        }
        g_render_method_submit_cached_definition_index = structure_bsp_index;
    }
}
// @address 0x1711d0 render_method_submit_set_extern_info
extern "C" void render_method_submit_set_extern_info(long structure_bsp_index, void *shader_extern_info)
{
    const s_shader_extern_info *extern_info = (const s_shader_extern_info *)shader_extern_info;
    if (extern_info)
    {
        render_method_submit_set_data_from_extern(extern_info->cubemap_sample.m_blend_factor, structure_bsp_index, invalid_cluster_reference_get(), -1,
            extern_info->cubemap_sample.m_current.cluster_reference, extern_info->cubemap_sample.m_current.next_cubemap_index,
            extern_info->cubemap_sample.m_last.cluster_reference, extern_info->cubemap_sample.m_last.next_cubemap_index, 0,
            &extern_info->change_colors, extern_info->object_index, extern_info->function_value_simple_callback);
    }
    else
    {
        c_render_method_data::m_valid = false;
        render_method_submit_valid();
    }
}
// @address 0x10f5d70 g_rasterizer_xenon_memory
// @address 0x1717a0 rasterizer_xenon_memory_find_entry
s_rasterizer_entry **rasterizer_xenon_memory_find_entry(c_rasterizer_xenon_memory *memory, unsigned __int64 key, unsigned int type)
{
    unsigned int a = (unsigned int)key;
    unsigned int b = (unsigned int)(key >> 32);
    unsigned int c = type;
    a = (a - b - c) ^ (c >> 13);
    b = (b - a - c) ^ (a << 8);
    c = (c - b - a) ^ (b >> 13);
    unsigned int bucket = c % 12287;
    s_rasterizer_entry *entry = g_rasterizer_xenon_memory.hash_table[bucket];
    if (entry != 0)
    {
        int probe_step = 0;
        int collisions = g_rasterizer_xenon_memory.collision_count;
        while (((unsigned __int64)entry->key_high << 32) + entry->key_low != key || (entry->flags >> 26) != type)
        {
            collisions = collisions + 1;
            unsigned int next_bucket = probe_step + bucket + 1;
            probe_step = probe_step + 2;
            g_rasterizer_xenon_memory.collision_count = collisions;
            bucket = next_bucket % 12287;
            if (probe_step <= 20)
            {
                entry = g_rasterizer_xenon_memory.hash_table[bucket];
                if (entry == 0)
                    break;
            }
            else
            {
                g_rasterizer_xenon_memory.collision_overflow_count = g_rasterizer_xenon_memory.collision_overflow_count + 1;
                return 0;
            }
        }
    }
    return &g_rasterizer_xenon_memory.hash_table[bucket];
}
// @address 0x1718e0 render_method_submit_test_geometry_keys
bool render_method_submit_test_geometry_keys(const unsigned __int64 *keys, int count)
{
    const c_tag_resource_cache_file_access_cache *access_cache =
        g_geometry_globals.manager->m_resource_cache.m_geometry_storage.m_per_thread_access_cache[get_current_thread_index()];
    bool result = true;
    int index = 0;
    while (index < count)
    {
        unsigned __int64 key = *keys;
        result = (unsigned __int64)access_cache->m_resource_pages.begin()[(unsigned int)(key >> 15) & 0x7fff] == key;
        if (!result)
            break;
        ++index;
        ++keys;
    }
    return result;
}
__forceinline void render_method_submit_invalidate_cache()
{
    ++g_binding_plan_epoch.generation;
    c_rasterizer::g_current_alpha_blend_mode = (c_rasterizer::e_alpha_blend_mode)13;
    c_rasterizer::g_current_separate_alpha_blend_mode = (c_rasterizer::e_separate_alpha_blend_mode)5;
    c_rasterizer::g_current_z_buffer_mode = (c_rasterizer::e_z_buffer_mode)8;
    g_rasterizer_stencil_mode = (c_rasterizer::e_stencil_mode)17;
    XMemSet(c_rasterizer::x_last_sampler_textures, 0, sizeof(c_rasterizer::x_last_sampler_textures));
}
// @address 0x81daac g_rasterizer_stencil_mode
// @address 0x172b88 render_method_submit_reset_cached_state
extern "C" void render_method_submit_reset_cached_state()
{
    g_render_method_submit_cached_state_enabled = 0;
    if (g_render_method_submit_state_dirty)
    {
        render_method_submit_invalidate_cache();
        g_render_method_submit_state_dirty = 0;
        g_render_method_submit_cache = k_default_render_method_submit_cache;
    }
}
// @address 0x1713f0 render_method_submit_draw_mesh_part
extern "C" void render_method_submit_draw_mesh_part(unsigned __int64 p0, const s_render_geometry *geometry, const s_mesh *mesh, const s_part *part,
    int mode, unsigned long *selection, const geometry_material *materials)
{
    bool submitted = false;
    if (!selection)
        submitted = render_method_submit_try_submit_cached(&g_rasterizer_xenon_memory, p0, geometry, mesh, part, mode, materials);
    if (!submitted)
    {
        if (g_render_method_submit_state_dirty)
        {
            render_method_submit_invalidate_cache();
            g_render_method_submit_state_dirty = 0;
            g_render_method_submit_cache = k_default_render_method_submit_cache;
        }
        render_mesh_part(geometry, mesh, part, mode, selection, materials);
    }
}
// @address 0x171508 render_method_submit_try_submit_cached
extern "C" bool render_method_submit_try_submit_cached(c_rasterizer_xenon_memory *memory, unsigned __int64 p0, const s_render_geometry *geometry,
    const s_mesh *mesh, const s_part *part, int mode, const geometry_material *materials)
{
    bool submitted = false;
    bool can_submit = false;
    if (g_render_hiz_stencil_enabled)
    {
        if (mode > 0)
        {
            if (mode <= 8)
                can_submit = true;
            else
                can_submit = can_submit;
        }
    }
    if (can_submit)
    {
        s_rasterizer_xenon_memory_request **request = (s_rasterizer_xenon_memory_request **)rasterizer_xenon_memory_find_entry(
            &g_rasterizer_xenon_memory, p0, mode);
        if (request)
        {
            c_render_method *render_method = (c_render_method *)tag_get(0, materials[part->material_index].render_method.index);
            bool low_pressure = g_rasterizer_xenon_memory.submit_count < 20 && g_rasterizer_xenon_memory.m_request_count < 0xc00;
            s_rasterizer_entry *cached = (s_rasterizer_entry *)*request;
            if (cached)
            {
                __dcbt(cached->key_index * 8, g_rasterizer_xenon_memory.m_entries);
                unsigned int flags = cached->flags;
                if ((flags & 0x3800000) == 0x1000000)
                {
#if defined(_XBOX) && !defined(__clang__)
                    // match: 0x171508: reading the whole key_range word makes cl reload key_index (lhz 0x10(r6)) here
                    // instead of reusing the halfword the __dcbt above loaded; the shift only holds on the big-endian console
                    unsigned short key_index = (unsigned short)(cached->key_range >> 16);
#else
                    unsigned short key_index = cached->key_index;
#endif
                    if (!render_method_submit_test_geometry_keys(
                            &g_rasterizer_xenon_memory.m_entries[key_index].key, cached->key_count))
                    {
                        cached->flags = (flags & ~0x3800000) | 0x1800000;
                        ++g_rasterizer_xenon_memory.command_buffer_invalidation_count;
                    }
                }
            }
            else if (low_pressure)
                g_rasterizer_xenon_memory.new_request(request, p0, mode);

            cached = (s_rasterizer_entry *)*request;
            if (cached)
            {
                unsigned int state = (cached->flags >> 23) & 7;
                switch (state)
                {
                case 2:
                    render_method_submit_set_cached_state(&g_rasterizer_xenon_memory, cached,
                        materials[part->material_index].render_method.index, render_method,
                        mesh, mode);
                    return true;
                case 3:
                    if (D3DResource_IsBusy(cached->buffer) != 0 || !low_pressure)
                        break;
                    // fall through: an idle state-3 entry is rebuilt like a new one
                case 0:
                    if (render_method_submit_build_cached_entry(&g_rasterizer_xenon_memory, (s_rasterizer_entry *)*request, render_method,
                            materials[part->material_index].render_method.index, geometry, mesh, part, mode))
                    {
                        render_method_submit_set_cached_state(&g_rasterizer_xenon_memory, (s_rasterizer_entry *)*request,
                            materials[part->material_index].render_method.index, render_method,
                            mesh, mode);
                        submitted = true;
                    }
                    break;
                }
            }
        }
    }
    return submitted;
}
// @address 0x1732d8 render_method_submit_set_device_texture
extern "C" void render_method_submit_set_device_texture(D3DDevice *device, long sampler, c_rasterizer_texture_ref texture_ref)
{
    if (texture_ref.get_datum_ref() != -1 && texture_ref.valid())
    {
        D3DBaseTexture *texture = texture_ref.get_d3d_texture();
        device->SetTexture(sampler, texture);
    }
    else
    {
        device->SetTexture(sampler, 0);
    }
}
// @address 0x173228 render_method_submit_set_bitmap_texture
extern "C" bool render_method_submit_set_bitmap_texture(D3DDevice *device, s_rasterizer_xenon_memory_entry *memory_entry, long sampler,
    long tag_index, long bitmap_index)
{
    bool bound = false;
    if (tag_index != -1 && bitmap_index != -1)
    {
        c_rasterizer_texture_ref texture_ref;
        texture_ref.clear();
        const bitmap_data *entry;
        if (::bitmap_group_get_xenon_hardware_format(tag_index, bitmap_index, &texture_ref, &entry, (__int64 *)&memory_entry->key))
        {
            render_method_submit_set_device_texture(device, sampler, texture_ref);
            bound = true;
        }
    }
    if (!bound)
        D3DDevice_SetTexture(device, sampler, 0, (1ui64 << 63) >> (unsigned int)(sampler + 32));
    return bound;
}
// @address 0x174830 ?render_method_submit@@YA_NJPBVc_render_method@@W4e_vertex_type@@W4e_transfer_vector_vertex_types@@W4e_entry_point@@_N@Z
bool render_method_submit(long index, class c_render_method const *render_method, enum e_vertex_type vertex_type,
    enum e_transfer_vector_vertex_types transfer_type, enum e_entry_point entry_point, bool force)
{
    s_render_method_submit_cache *cache = &g_render_method_submit_cache;
    bool changed = !cache->valid ||
                   render_method != cache->render_method ||
                   entry_point != cache->entry_point ||
                   vertex_type != cache->vertex_type ||
                   transfer_type != g_submitted_shader_state.transfer_type ||
                   g_submitted_shader_state.generation != render_method_submit_plan_generation();
    if (!changed && render_method)
    {
        const long template_index = render_method->get_postprocess_definition()->template_reference.index;
        changed = template_index != g_submitted_shader_state.template_index;
        if (!changed && template_index != NONE)
        {
            const auto &tag = global_tag_instances[DATUM_INDEX_TO_ABSOLUTE_INDEX(template_index)];
            changed = g_submitted_shader_state.shader_template != tag.definition ||
                      g_submitted_shader_state.template_salt != tag.salt;
        }
    }
    do
    {
        if (changed || force)
        {
            if (!render_method_submit_set_shader_state(render_method, vertex_type, transfer_type, entry_point, force))
            {
                g_render_method_submit_cache = k_default_render_method_submit_cache;
                break;
            }
            if (!force)
            {
                cache->render_method = render_method;
                cache->entry_point = entry_point;
                cache->vertex_type = vertex_type;
                const long template_index = render_method->get_postprocess_definition()->template_reference.index;
                const auto &tag = global_tag_instances[DATUM_INDEX_TO_ABSOLUTE_INDEX(template_index)];
                g_submitted_shader_state = { tag.definition, template_index, tag.salt, transfer_type, g_binding_plan_epoch.generation };
            }
            else if (changed)
            {
                // A pixel-only submit cannot satisfy a later full bind for this tuple.
                *cache = k_default_render_method_submit_cache;
            }
        }
        if (!render_method_submit_update_animated_state(index, render_method, entry_point, force, 0))
            break;
        return true;
    } while (0);
    return false;
}
// @address 0x174a28 ?get_compiled_vertex_shader@c_render_method@@QBAPBVc_rasterizer_compiled_vertex_shader@@W4e_vertex_type@@W4e_entry_point@@@Z
const c_rasterizer_compiled_vertex_shader *c_render_method::get_compiled_vertex_shader(e_vertex_type vertex_type, e_entry_point entry_point) const
{
    const s_render_method_shader_definition *definition = (const s_render_method_shader_definition *)tag_get(0, this->definition.index);
    const s_rasterizer_global_vertex_shader_definition *shared = (const s_rasterizer_global_vertex_shader_definition *)tag_get(0,
        definition->shared_vertex_shader.index);
    const s_tag_block *vertex_entry = (const s_tag_block *)tag_block_get_element_with_size(&shared->vertex_types, vertex_type, sizeof(s_tag_block));
    const s_rasterizer_global_shader_entry_point *global_entry = (const s_rasterizer_global_shader_entry_point
        *)tag_block_get_element_with_size(vertex_entry, entry_point, sizeof(s_rasterizer_global_shader_entry_point));
    if (global_entry->shader_index != -1)
        return (const c_rasterizer_compiled_vertex_shader *)tag_block_get_element_with_size(&shared->shaders, global_entry->shader_index,
            sizeof(c_rasterizer_compiled_vertex_shader));
    else
    {
        const s_render_method_postprocess_definition *postprocess = get_postprocess_definition();
        const rasterizer_implicit_geometry_template *shader_template = (const rasterizer_implicit_geometry_template *)tag_get(0,
            postprocess->template_reference.index);
        const c_rasterizer_vertex_shader *shader = (const c_rasterizer_vertex_shader *)tag_get(0, shader_template->vertex_shader.index);
        return shader->get_compiled_shader(vertex_type, entry_point, 0);
    }
}
const c_rasterizer_compiled_pixel_shader *c_render_method::get_compiled_pixel_shader(e_entry_point entry_point) const
{
    const s_render_method_shader_definition *definition = (const s_render_method_shader_definition *)tag_get(0, this->definition.index);
    long shader_index = -1;
    const s_rasterizer_global_pixel_shader_definition *shared = (const s_rasterizer_global_pixel_shader_definition *)tag_get(0,
        definition->shared_pixel_shader.index);
    const s_rasterizer_global_shader_entry_point *global_entry_point = (const s_rasterizer_global_shader_entry_point
        *)tag_block_get_element_with_size(&shared->entry_points, entry_point, sizeof(s_rasterizer_global_shader_entry_point));
    if (global_entry_point->category_dependency.count != 0)
    {
        const s_rm_category_dependency *dependency = (const s_rm_category_dependency
            *)tag_block_get_element_with_size(&global_entry_point->category_dependency, 0, sizeof(s_rm_category_dependency));
        const long *option = (const long *)tag_block_get_element_with_size(&dependency->shaders,
            (short)this->get_selected_option((short)dependency->category), sizeof(long));
        shader_index = *option;
    }
    else if (global_entry_point->shader_index != -1)
        shader_index = global_entry_point->shader_index;
    if (shader_index == -1)
    {
        const s_render_method_postprocess_definition *postprocess = this->get_postprocess_definition();
        const rasterizer_implicit_geometry_template *shader_template = (const rasterizer_implicit_geometry_template *)tag_get(0,
            postprocess->template_reference.index);
        const c_rasterizer_pixel_shader *shader = (const c_rasterizer_pixel_shader *)tag_get(0, shader_template->pixel_shader.index);
        return shader->get_compiled_shader(entry_point, 0);
    }
    else
        return (const c_rasterizer_compiled_pixel_shader *)tag_block_get_element_with_size(&shared->shaders, shader_index,
            sizeof(c_rasterizer_compiled_pixel_shader));
}
// @address 0x185ff0 ?get_default_texture_rasterizer_texture_ref@c_rasterizer_globals@@QBA?BVc_rasterizer_texture_ref@@J@Z
#if defined(_XBOX) && !defined(__clang__)
inline const c_rasterizer_texture_ref c_rasterizer_globals::get_default_texture_rasterizer_texture_ref(long index) const
#else
const c_rasterizer_texture_ref c_rasterizer_globals::get_default_texture_rasterizer_texture_ref(long index) const
#endif
{
    c_rasterizer_texture_ref result;
    const s_rasterizer_default_texture *default_texture = (const s_rasterizer_default_texture
        *)tag_block_get_element_with_size(&m_default_textures_refs, index, sizeof(s_rasterizer_default_texture));
    const bitmap_data *entry;
    ::bitmap_group_get_xenon_hardware_format(default_texture->bitmap.index, 0, &result, &entry, (__int64 *)(0));
    return result;
}
namespace
{
struct s_shader_plan
{
    const c_render_method *render_method;
    const rasterizer_implicit_geometry_template *shader_template;
    const s_render_method_postprocess_definition *postprocess;
    const void *options;
    unsigned __int64 generation;
    long definition_index;
    long template_index;
    e_vertex_type vertex_type;
    e_transfer_vector_vertex_types transfer_type;
    e_entry_point entry_point;
    unsigned short template_salt;
    bool pixel_only;
    const c_rasterizer_compiled_vertex_shader *vertex_shader;
    const c_rasterizer_compiled_pixel_shader *pixel_shader;
};

constexpr unsigned int k_shader_plan_count = 256;
s_shader_plan g_shader_plans[k_shader_plan_count];
s_shader_plan *g_last_shader_plan;

static const s_shader_plan &render_method_submit_get_shader_plan(const c_render_method *render_method,
    const rasterizer_implicit_geometry_template *shader_template, e_vertex_type vertex_type,
    e_transfer_vector_vertex_types transfer_type, e_entry_point entry_point, bool pixel_only)
{
    const auto generation = render_method_submit_plan_generation();
    const auto *postprocess = render_method->get_postprocess_definition();
    const long template_index = postprocess ? postprocess->template_reference.index : -1;
    const unsigned short salt = template_index != NONE
        ? global_tag_instances[DATUM_INDEX_TO_ABSOLUTE_INDEX(template_index)].salt : 0;
    const auto matches = [&](const s_shader_plan &plan)
    {
        return plan.render_method == render_method && plan.vertex_type == vertex_type && plan.transfer_type == transfer_type &&
            plan.entry_point == entry_point && plan.pixel_only == pixel_only && plan.generation == generation &&
            plan.shader_template == shader_template && plan.postprocess == postprocess && plan.options == render_method->options.address &&
            plan.definition_index == render_method->definition.index && plan.template_index == template_index && plan.template_salt == salt;
    };
    const auto *last = g_last_shader_plan;
    if (last && matches(*last))
        return *last;

    const auto material_address = (unsigned __int64)render_method;
    const unsigned int bucket = ((material_address >> 2) ^ (material_address >> 10) ^
        (vertex_type * 17 + transfer_type * 31 + entry_point * 37 + pixel_only * 43)) & (k_shader_plan_count - 1);
    auto &plan = g_shader_plans[bucket];
    g_last_shader_plan = &plan;
    if (&plan != last && matches(plan))
        return plan;

    const auto *vertex_shader = pixel_only ? nullptr : render_method->get_compiled_vertex_shader(vertex_type, entry_point);
    const auto *pixel_shader = render_method->get_compiled_pixel_shader(entry_point);
    plan = { render_method, shader_template, postprocess, render_method->options.address, generation,
        render_method->definition.index, template_index, vertex_type, transfer_type, entry_point, salt,
        pixel_only, vertex_shader, pixel_shader };
    return plan;
}
}

bool render_method_submit_bind_shaders(const c_render_method *render_method,
    const rasterizer_implicit_geometry_template *shader_template, e_vertex_type vertex_type,
    e_transfer_vector_vertex_types transfer_type, e_entry_point entry_point, bool force)
{
    const auto &plan = render_method_submit_get_shader_plan(render_method, shader_template, vertex_type, transfer_type, entry_point, force);
    if (force)
    {
        if (!plan.pixel_shader)
            return false;
        D3DDevice_SetPixelShader(g_rasterizer_device, plan.pixel_shader->get_d3d_shader());
        return true;
    }

    bool vertex_ok = false;
    if (plan.vertex_shader)
    {
        D3DVertexShader *d3d_vertex_shader = plan.vertex_shader->get_d3d_shader();
        D3DVertexDeclaration *declaration = rasterizer_get_vertex_declaration(plan.vertex_type, plan.transfer_type, plan.entry_point);
        D3DDevice_SetVertexDeclaration(c_rasterizer::get_device(), declaration);
        if (d3d_vertex_shader)
        {
            D3DDevice_SetVertexShader(c_rasterizer::get_device(), d3d_vertex_shader);
            vertex_ok = true;
        }
    }
    if (!plan.pixel_shader)
        return false;
    D3DDevice_SetPixelShader(c_rasterizer::get_device(), plan.pixel_shader->get_d3d_shader());
    return vertex_ok;
}

// @address 0x174950 ?set_pixel_and_vertex_shaders@@YA_NPBVc_render_method@@W4e_vertex_type@@W4e_transfer_vector_vertex_types@@W4e_entry_point@@@Z
bool set_pixel_and_vertex_shaders(const c_render_method *render_method, e_vertex_type vertex_type,
    e_transfer_vector_vertex_types transfer_type, e_entry_point entry_point)
{
    const auto *postprocess = render_method->get_postprocess_definition();
    const auto *shader_template = postprocess && postprocess->template_reference.index != -1
        ? (const rasterizer_implicit_geometry_template *)tag_get(0, postprocess->template_reference.index) : nullptr;
    return render_method_submit_bind_shaders(render_method, shader_template, vertex_type, transfer_type, entry_point, false);
}
// @address 0x8368cc g_command_buffer_device
static __forceinline void render_method_submit_stream(const s_render_geometry *geometry, long buffer_index, long stream, unsigned __int64 frequency)
{
    const c_rasterizer_vertex_buffer *buffer = (const c_rasterizer_vertex_buffer *)render_geometry_get_vertex_buffer(geometry, buffer_index);
    D3DDevice_SetStreamSource(g_command_buffer_device, stream, buffer->m_d3d_buffer, 0, buffer->m_vertex_stride, frequency);
}
// @address 0x457368 render_method_submit_set_mesh_streams
extern "C" unsigned char render_method_submit_set_mesh_streams(c_rasterizer_xenon_memory *memory, s_rasterizer_xenon_memory_request *request,
    const s_render_geometry *owner, const s_mesh *part, long *index_type)
{
    bool result = false;
    s_rasterizer_xenon_memory_entry *entry = g_rasterizer_xenon_memory.new_entry(request);
    if (entry)
    {
        const c_tag_resource_cache_file_access_cache *cache =
            g_geometry_globals.manager->m_resource_cache.m_geometry_storage.m_per_thread_access_cache[get_current_thread_index()];
        entry->key = cache->m_resource_pages.begin()[datum_index_to_absolute_index(owner->api_resource_handle)];
        const c_rasterizer_index_buffer *indices = render_geometry_get_index_buffer(owner, (unsigned short)part->index_buffer_indices[0]);
        D3DDevice_SetIndices(g_command_buffer_device, indices->get_d3d_buffer());
        unsigned long vertex_buffer = (unsigned short)part->vertex_buffer_indices[0];
        *index_type = indices->get_primitive_type();
        render_method_submit_stream(owner, vertex_buffer, 0, 1);
        unsigned long lighting = (unsigned short)part->vertex_buffer_indices[2];
        if (lighting == 0xffff)
        {
            lighting = (unsigned short)part->vertex_buffer_indices[1];
            if (lighting != 0xffff)
                render_method_submit_stream(owner, lighting, 4, 2);
        }
        else
            render_method_submit_stream(owner, lighting, 4, 2);
        if ((unsigned short)part->vertex_buffer_indices[3] != 0xffff)
            render_method_submit_stream(owner, (unsigned short)part->vertex_buffer_indices[3], 2, 1);
        result = true;
    }
    return result;
}
// @address 0x45a5f0 render_method_submit_build_declaration
extern "C" void render_method_submit_build_declaration(long position, long secondary, long vertex_type, s_render_method_submit_declaration *declaration)
{
    declaration->position = 0;
    declaration->position = rasterizer_get_declaration_size(0, position);
    switch (vertex_type)
    {
    case _vertex_type_particle_model:
    case _vertex_type_chud_simple:
        declaration->lighting = rasterizer_get_declaration_size(1, 2);
        declaration->secondary = 0;
        break;
    case _vertex_type_flat_world:
        declaration->lighting = rasterizer_get_declaration_size(1, 3);
        declaration->secondary = 0;
        break;
    case _vertex_type_chud_fancy:
        declaration->lighting = rasterizer_get_declaration_size(1, 1);
        declaration->secondary = 0;
        break;
    case _vertex_type_flat_skinned:
    case _vertex_type_screen:
    case _vertex_type_debug:
    case _vertex_type_light_volume:
        declaration->lighting = 0;
        declaration->secondary = rasterizer_get_declaration_size(2, secondary);
        break;
    }
}
// @address 0x48d2e0 gui_widget_animate_rotation_origin
extern "C" void gui_widget_animate_rotation_origin(s_animation_transform *transform, float time, bool combine,
    const s_gui_texture_offset_keyframe *start, const s_gui_texture_offset_keyframe *end, const c_function_definition *function)
{
    if (start->transition_type == 1)
        function = start->custom_function;
    const float zero = 0.0f;
    float progress = zero;
    if (function->m_data.address != 0 && function->m_data.size > 0)
        progress = function->map_to_output_range_legacy(function->evaluate_legacy(time, 1.0f));
    transform->texture_offset.x =
        (end->offset.x - start->offset.x) * progress + start->offset.x + (combine ? transform->texture_offset.x : zero);
    transform->texture_offset.y =
        (end->offset.y - start->offset.y) * progress + start->offset.y + (combine ? transform->texture_offset.y : zero);
}
// @address 0x48d3c0 gui_widget_animate_bitmap
extern "C" void gui_widget_animate_bitmap(s_animation_transform *transform, float time, bool combine,
    const s_gui_bitmap_keyframe *start, const s_gui_bitmap_keyframe *end, const c_function_definition *function)
{
    if (start->transition_type == 1)
        function = start->custom_function;
    float progress = 0.0f;
    if (function->m_data.address != 0 && function->m_data.size > 0)
        progress = function->map_to_output_range_legacy(function->evaluate_legacy(time, 1.0f));
    transform->bitmap_sequence_index =
        (short)((long)((float)(end->sequence_index - start->sequence_index) * progress + 0.5f) + start->sequence_index);
    transform->bitmap_frame_index =
        (short)((long)((float)(end->frame_index - start->frame_index) * progress + 0.5f) + start->frame_index);
}
// @address 0x73a1c8 k_d3d_texture_address_modes
// @address 0x17a320 render_method_submit_set_lightmap_texture
extern "C" void render_method_submit_set_lightmap_texture(long sampler, long bitmap_index, long structure_bsp_index, long bitmap_flags)
{
    bool has_texture = false;
    if (bitmap_index != -1 && structure_bsp_index != -1 && structure_bsp_is_active(structure_bsp_index))
    {
        const scenario_structure_bsp_reference *reference =
            &((const scenario_structure_bsp_reference *)((scenario *)global_scenario)->structure_bsps.address)[structure_bsp_index];
        if (reference->structure_bsp.index != -1)
        {
            int lightmap_index = reference->decorator_model.index;
            if (lightmap_index != -1 && bitmap_index >= 0)
            {
                const bitmap_group *definition = (const bitmap_group *)tag_get(0, lightmap_index);
                if (bitmap_index < definition->bitmaps.count &&
                    render_method_submit_set_texture(sampler, lightmap_index, bitmap_index))
                {
                    has_texture = true;
                }
            }
        }
    }
    if (!has_texture)
    {
        int rasterizer_index = global_game_globals->rasterizer_globals_ref.index;
        const c_rasterizer_globals *definition;
        if (rasterizer_index != -1)
            definition = (const c_rasterizer_globals *)tag_get(0, rasterizer_index);
        else
            definition = 0;
        c_rasterizer::set_sampler_texture(sampler, ((const c_rasterizer_globals *)definition)->get_default_texture_rasterizer_texture_ref(2));
    }
    c_rasterizer::set_sampler_address_mode(sampler, c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp,
        c_rasterizer::_sampler_address_clamp);
    if (g_rasterizer_sampler_filter_modes[sampler] != 0)
    {
        if (sampler >= 0 && (unsigned int)sampler < 26)
            g_rasterizer_sampler_filter_modes[sampler] = 0;
        rasterizer_device_set_sampler_filter_modes(c_rasterizer::get_device(), sampler, 0, 0);
    }
    D3DDevice *device = c_rasterizer::get_device();
    (c_rasterizer::get_device())->m_Pending.m_Mask[1] |= 1ui64 << 60;
    device->m_Constants.PixelShaderF[15].x = device->m_Constants.PixelShaderF[15].y =
        device->m_Constants.PixelShaderF[15].z = device->m_Constants.PixelShaderF[15].w = c_render_method_data::m_cubemap_blend_factor;
}
// @address 0x17a4d8 render_method_submit_set_default_texture
extern "C" void render_method_submit_set_default_texture(D3DDevice *d3d_device, s_rasterizer_xenon_memory_entry *memory_entry,
    int extern_index, int default_sampler)
{
    if (d3d_device != 0)
    {
        D3DDevice_SetSamplerState(d3d_device, default_sampler, D3DSAMP_ADDRESSU, k_d3d_texture_address_modes[1]);
        D3DDevice_SetSamplerState(d3d_device, default_sampler, D3DSAMP_ADDRESSV, k_d3d_texture_address_modes[1]);
        D3DDevice_SetSamplerState(d3d_device, default_sampler, D3DSAMP_ADDRESSW, k_d3d_texture_address_modes[1]);
        rasterizer_device_set_sampler_filter_modes(d3d_device, default_sampler, 2, 2);
    }
    else
    {
        c_rasterizer::set_sampler_address_mode(default_sampler, c_rasterizer::_sampler_address_clamp,
            c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp);
        if (g_rasterizer_sampler_filter_modes[default_sampler] != 2)
        {
            if (default_sampler >= 0 && (unsigned int)default_sampler < 26)
                g_rasterizer_sampler_filter_modes[default_sampler] = 2;
            rasterizer_device_set_sampler_filter_modes(c_rasterizer::get_device(), default_sampler, 2, 2);
        }
    }
    int rasterizer_index = global_game_globals->rasterizer_globals_ref.index;
    const c_rasterizer_globals *rasterizer_definition;
    if (rasterizer_index != -1)
        rasterizer_definition = (const c_rasterizer_globals *)tag_get(0, rasterizer_index);
    else
        rasterizer_definition = 0;
    int default_texture_index;
    const s_tag_reference *material_textures = (const s_tag_reference *)((const c_rasterizer_globals *)rasterizer_definition)->m_material_textures_refs.address;
    if (extern_index != 24)
    {
        if (extern_index != 25)
        {
            default_texture_index = material_textures[1].index;
        }
        else
        {
            default_texture_index = material_textures[2].index;
        }
    }
    else
        default_texture_index = material_textures[0].index;
    if (d3d_device != 0)
    {
        render_method_submit_set_bitmap_texture(d3d_device, memory_entry, default_sampler, default_texture_index, 0);
        return;
    }
    render_method_submit_set_texture(default_sampler, default_texture_index, 0);
}
// @address 0x172fa8 ?evaluate_function@@YAMJPBUs_render_method_animated_parameter@@@Z
float evaluate_function(long p0, const s_render_method_animated_parameter *ap)
{
#if defined(_XBOX) && !defined(__clang__)
    // match: 0x172fa8: shared constant-pool base register; one base feeds the 0.0 range and 1.0 period lfs
    const float *pool_base = k_real_constant_pool;
    float fv2 = pool_base[200];
#else
    float fv2 = 0.0f;
#endif
    float fv4;
    int t = ap->input_name;
    int is_692 = (t - 692 == 0);
    int is_range = (unsigned char)(t == 693 || t == 694 || t == 695 || t == 696);
    if (c_render_method_data::m_context_interface && (t != 0 || ap->range_name != 0)
        && (unsigned char)is_692 == 0 && (unsigned char)is_range == 0)
    {
        fv4 = c_render_method_data::m_context_interface(c_render_method_data::m_context, t, p0);
        fv2 = c_render_method_data::m_context_interface(c_render_method_data::m_context, ap->range_name, p0);
    }
    else
    {
#if defined(_XBOX) && !defined(__clang__)
        // match: 0x172fa8: shared constant-pool base register; the 1.0 lfs reuses the pool base
        float inv = pool_base[0] / ap->time_period;
#else
        float inv = 1.0f / ap->time_period;
#endif
        fv4 = game_time_get_safe_in_seconds() * inv;
        if ((unsigned char)is_692 != 0)
        {
            fv4 = (game_time_get_safe_in_seconds() + (float)c_render_method_data::m_context) * inv;
        }
        else if ((unsigned char)is_range != 0)
        {
            scenario *sc = ((scenario *)global_scenario);
            int idx = t - 693;
            unsigned int id = 0;
            if (sc->interpolators.count > idx)
                id = ((s_scenario_interpolator_definition *)sc->interpolators.address)[idx].name;
            s_scenario_interpolator_state *st = scenario_get_interpolator_state(id, 0);
            int flag = 0;
            if (st != 0)
                flag = st->flags & 1;
            if ((unsigned char)flag != 0)
            {
                fv4 = hs_interpolator_get_current_value(id, true);
            }
            else
            {
                unsigned int local_index = (unsigned int)-1;
                s_scenario_interpolator_state *st2 = scenario_get_interpolator_state(id, (long *)&local_index);
                float v = fv2;
                if (st2 != 0)
                {
                    float start = st2->start;
                    v = scenario_interpolator_evaluate_at(local_index, start);
                }
                fv4 = v;
            }
        }
        if ((unsigned char)is_range == 0 && *static_cast<const unsigned char *>(ap->function.m_data.address) != 3)
            fv4 = (float)(fv4 - __fcfid(__fctidz(fv4)));
    }
    return ap->function.evaluate_legacy(fv4, fv2);
}
static void render_method_submit_apply_externs(const s_render_method_binding_plan &plan, long type, bool force,
    long *last_sampler_textures)
{
    for (const auto &route : plan.ranges[_binding_extern_sampler])
    {
        if (type == route.extern_type)
        {
            if (g_render_method_submit_state_dirty)
                last_sampler_textures[route.destination] = -1;
            render_method_submit_set_extern_sampler(0, 0, route.source, route.destination);
        }
    }
    for (const auto &route : plan.ranges[_binding_pixel_extern])
    {
        if (force || type == route.extern_type)
        {
            real_vector4d value;
            render_method_submit_get_extern_constant(route.source, &value);
            c_rasterizer::get_device()->SetPixelShaderConstantF(route.destination, value.n, 1);
        }
    }
    for (const auto &route : plan.ranges[_binding_vertex_extern])
    {
        if (force || type == route.extern_type)
        {
            real_vector4d value;
            render_method_submit_get_extern_constant(route.source, &value);
            c_rasterizer::get_device()->SetVertexShaderConstantF(route.destination, value.n, 1);
        }
    }
}

// @address 0x1722f0 render_method_submit_update_externs
void render_method_submit_update_externs(const c_render_method *render_method, long entry_point, long type, unsigned char force)
{
    const s_render_method_postprocess_definition *postprocess = render_method->get_postprocess_definition();
    long template_index = postprocess->template_reference.index;
    const auto *shader_template = (const rasterizer_implicit_geometry_template *)tag_get(0, template_index);
    render_method_submit_apply_externs(render_method_submit_get_binding_plan(shader_template, template_index, entry_point),
        type, force, c_rasterizer::x_last_sampler_textures);
}

void render_method_submit_apply_bindings(const s_render_method_postprocess_definition *postprocess,
    const rasterizer_implicit_geometry_template *shader_template, long entry_point, long *last_sampler_textures)
{
    const auto &plan = render_method_submit_get_binding_plan(shader_template, postprocess->template_reference.index, entry_point);
    const auto *textures = (const s_render_method_texture *)postprocess->textures.address;
    const auto *values = (const real_vector4d *)postprocess->real_constants.address;
    const long *integer_values = (const long *)postprocess->integer_values.address;
    unsigned long boolean_values = postprocess->boolean_values;
    render_method_submit_apply_externs(plan, 1, false, last_sampler_textures);
    for (const auto &binding : plan.ranges[_binding_texture])
    {
        long sampler = binding.destination + binding.sampler_offset;
        const s_render_method_texture *texture = &textures[binding.source];
        long address_mode = texture->address_mode & 15;
        c_rasterizer::set_sampler_address_mode(sampler, (c_rasterizer::e_sampler_address_mode)address_mode,
            (c_rasterizer::e_sampler_address_mode)(texture->address_mode >> 4),
            (c_rasterizer::e_sampler_address_mode)address_mode);
        c_rasterizer::set_sampler_filter_mode(sampler, static_cast<c_rasterizer::e_sampler_filter_mode>(texture->filter_mode));
        if (!render_method_submit_set_dynamic_texture(0, sampler, texture->type))
        {
            long bitmap_index = texture->bitmap.index;
            if (bitmap_index != -1)
            {
                const bitmap_group *bitmap =
                    (const bitmap_group *)tag_get('bitm', bitmap_index);
                long frame = texture->index > 0 ? texture->index : 0;
                long maximum = bitmap->bitmaps.count - 1;
                render_method_submit_set_texture(
                    sampler, bitmap_index, frame < maximum ? (texture->index > 0 ? texture->index : 0) : maximum);
            }
            else if (texture->type)
            {
                long globals_index = global_game_globals->rasterizer_globals_ref.index;
                const c_rasterizer_globals *globals = globals_index != -1
                    ? (const c_rasterizer_globals *)tag_get('rasg', globals_index)
                    : 0;
                c_rasterizer::set_sampler_texture(sampler, globals->get_default_texture_rasterizer_texture_ref(0));
            }
        }
    }
    for (const auto &binding : plan.ranges[_binding_pixel_constant])
    {
        c_rasterizer::set_pixel_shader_constant(binding.destination, 1, &values[binding.source]);
    }
    for (const auto &binding : plan.ranges[_binding_integer_constant])
    {
        int value[4];
        value[0] = integer_values[binding.source];
        value[1] = 0;
        value[2] = 1;
        value[3] = 0;
        D3DDevice_SetPixelShaderConstantI(g_rasterizer_device, binding.destination, value, 1);
    }
    for (const auto &binding : plan.ranges[_binding_vertex_constant])
    {
        c_rasterizer::set_vertex_shader_constant(binding.destination, 1, &values[binding.source]);
    }
    for (const auto &binding : plan.ranges[_binding_boolean_constant])
    {
        int boolean_constant = (boolean_values & (1 << binding.source)) != 0;
        D3DDevice_SetPixelShaderConstantB(g_rasterizer_device, binding.destination, &boolean_constant, 1);
    }
}

static __forceinline void render_method_submit_add_destination(
    short constant_index, short register_index, s_destination_info *destinations, short *count)
{
    for (long index = 0; index < *count; ++index)
    {
        if (destinations[index].constant_index == constant_index && destinations[index].register_index == register_index)
            return;
    }
    s_destination_info *destination = &destinations[*count];
    ++*count;
    destination->constant_index = constant_index;
    destination->register_index = register_index;
}
// @address 0x173350 update_constants
extern "C" void update_constants(long context, real_vector4d *constants, s_destination_info *destinations,
    short *destination_count, const s_tag_block_index &range, const s_tag_block &routes, const s_tag_block &parameters)
{
    long packed_range = range.value;
    *destination_count = 0;
    for (long index = 0; index < (long)((unsigned long)packed_range >> 10); packed_range = range.value, ++index)
    {
        const s_rm_constant_route *route = &((const s_rm_constant_route *)routes.address)[(packed_range & 0x3ff) + index];
        const s_render_method_animated_parameter *animated = &((const s_render_method_animated_parameter *)parameters.address)[route->parameter_index];
        float value = evaluate_function(context, animated);
        long type = animated->type;
        if (type == 1)
        {
            real_rgb_color color;
            animated->function.map_to_color_range_legacy(value, &color);
            float green = color.green;
            float blue = color.blue;
            constants[route->constant_index].i = color.red;
            constants[route->constant_index].j = green;
            constants[route->constant_index].k = blue;
        }
        else
        {
            value = animated->function.map_to_output_range_legacy(value);
            switch (type)
            {
            case 2:
                constants[route->constant_index].j = value;
            case 0:
            case 3:
                constants[route->constant_index].i = value;
                break;
            case 4:
                constants[route->constant_index].j = value;
                break;
            case 5:
                constants[route->constant_index].k = value;
                break;
            case 6:
            case 8:
                constants[route->constant_index].w = value;
                break;
            }
        }
        render_method_submit_add_destination(route->constant_index, route->register_index, destinations, destination_count);
    }
}
struct s_weighted_atmosphere_parameters;
// @address 0x4aee0 k_mesh_part_type_entry_points
// @address 0x171968 render_method_submit_set_cached_state
extern "C" void render_method_submit_set_cached_state(c_rasterizer_xenon_memory *, s_rasterizer_entry *cached, long context,
    const c_render_method *render_method, const s_mesh *mesh, long entry_point)
{
    c_rasterizer_xenon_memory *memory = &g_rasterizer_xenon_memory;
    long shader_entry_point;
    if (entry_point == 8)
    {
        if (mesh->flags & 1)
            shader_entry_point = 14;
        else if (!memory->alternate_entry_point)
            shader_entry_point = k_mesh_part_type_entry_points[mesh->prt_type];
        else
            shader_entry_point = 5;
    }
    else
        shader_entry_point = entry_point;
    const s_render_method_postprocess_definition *postprocess = render_method->get_postprocess_definition();
    const c_render_method *method = (const c_render_method *)render_method;
    if (shader_entry_point != 1)
    {
        if (method->flags & 2)
            render_player_view_set_atmosphere_index(method->atmosphere_index);
        else if (method->flags & 1)
            c_atmosphere_fog_interface::disable_atmosphere();
        else if (atmosphere_constants_index != -2)
        {
            atmosphere_constants_index = -2;
            c_atmosphere_fog_interface::set_constant(&c_atmosphere_fog_interface::m_default_parameters, true, true);
        }
    }
    if (cached->flags & 0x200000)
        render_method_submit_update_animated_state(context, render_method, shader_entry_point, false, true);
    for (unsigned long index = 0; index < (unsigned char)cached->flags; ++index)
    {
        long constant = ((cached->flags >> 8) & 0x1fff) + index;
        long register_index = memory->stage_registers[constant];
        if (register_index >= 256)
            c_rasterizer::get_device()->SetPixelShaderConstantF(register_index - 256, memory->stage_constants[constant].n, 1);
        else
            c_rasterizer::get_device()->SetVertexShaderConstantF(register_index, memory->stage_constants[constant].n, 1);
    }
    const long template_index = postprocess->template_reference.index;
    const auto *shader_template = (const rasterizer_implicit_geometry_template *)tag_get(0, template_index);
    const auto &plan = render_method_submit_get_binding_plan(shader_template, template_index, entry_point);
    if (cached->flags & 0x400000)
    {
        const auto *textures = (const s_render_method_texture *)postprocess->textures.address;
        for (const auto &route : plan.ranges[_binding_texture])
        {
            const s_render_method_texture *texture = &textures[route.source];
            if (texture->type)
            {
                long sampler = route.destination + route.sampler_offset;
                c_rasterizer::x_last_sampler_textures[sampler] = -1;
                if (!render_method_submit_set_dynamic_texture(0, sampler, texture->type))
                {
                    long bitmap_index = texture->bitmap.index;
                    if (bitmap_index != -1)
                    {
                        const bitmap_group *bitmap = (const bitmap_group *)tag_get(0, bitmap_index);
                        long frame = texture->index > 0 ? texture->index : 0;
                        long maximum = bitmap->bitmaps.count - 1;
                        render_method_submit_set_texture(sampler, bitmap_index, frame < maximum ? (texture->index > 0 ? texture->index : 0) : maximum);
                    }
                    else
                    {
                        long globals_index = global_game_globals->rasterizer_globals_ref.index;
                        const c_rasterizer_globals *globals = globals_index != -1 ? (const c_rasterizer_globals *)tag_get(0, globals_index) : 0;
                        c_rasterizer::set_sampler_texture(sampler, globals->get_default_texture_rasterizer_texture_ref(0));
                    }
                }
            }
        }
    }
    unsigned long booleans = postprocess->boolean_values;
    const long *integers = (const long *)postprocess->integer_values.address;
    for (const auto &route : plan.ranges[_binding_boolean_constant])
    {
        int enabled = (booleans & (1 << route.source)) != 0;
        c_rasterizer::get_device()->SetPixelShaderConstantB(route.destination, &enabled, 1);
    }
    for (const auto &route : plan.ranges[_binding_integer_constant])
    {
        int value[4];
        value[0] = integers[route.source];
        value[1] = 0;
        value[2] = 1;
        value[3] = 0;
        c_rasterizer::get_device()->SetPixelShaderConstantI(route.destination, value, 1);
    }
    if (postprocess->flags & 2)
        rasterizer_set_alpha_test_enabled(1, 0);
    D3DDevice_RunCommandBuffer(c_rasterizer::get_device(), cached->buffer, 0x3fffffff);
#ifdef HALO3_64BIT
    // Replay writes sampler state directly. The next draw may set an inherited sampler
    // before it reaches the usual render-method cache invalidation.
    XMemSet(g_rasterizer_sampler_address_modes, -1, sizeof(g_rasterizer_sampler_address_modes));
    XMemSet(g_rasterizer_sampler_filter_modes, -1, sizeof(g_rasterizer_sampler_filter_modes));
#endif
    if (g_render_method_submit_cached_state_enabled)
        g_render_method_submit_state_dirty = 1;
    else
    {
        render_method_submit_invalidate_cache();
        g_render_method_submit_cache = k_default_render_method_submit_cache;
    }
    if (postprocess->flags & 2)
        rasterizer_set_alpha_test_enabled(0, 0);
}
// @address 0x171f30 render_method_submit_update_animated_state
bool render_method_submit_update_animated_state(long context, const c_render_method *render_method, long entry_point, bool force, bool set_sampler_state)
{
    bool result = false;
    if ((c_render_method_data::m_valid || force) && render_method)
    {
        const s_render_method_postprocess_definition *postprocess = render_method->get_postprocess_definition();
        const rasterizer_implicit_geometry_template *shader_template = (const rasterizer_implicit_geometry_template *)tag_get(0,
            postprocess->template_reference.index);
        if (shader_template->entry_point_mask & (1 << entry_point))
        {
            render_method_submit_apply_externs(
                render_method_submit_get_binding_plan(shader_template, postprocess->template_reference.index, entry_point),
                0, set_sampler_state, c_rasterizer::x_last_sampler_textures);
            if (entry_point < postprocess->entry_points.count)
            {
                const unsigned short *entry_points = (const unsigned short *)postprocess->entry_points.address;
                const s_rm_postprocess_pass *pass = &postprocess->passes[entry_points[entry_point] & 0x3ff];
                for (long index = 0; index < (long)(pass->textures.value >> 10); index++)
                {
                    const s_rm_constant_route *route = &(
                        (const s_rm_constant_route *)postprocess->routing_info.address)[(pass->textures.value & 0x3ff) + index];
                    const s_render_method_animated_parameter *animated = &postprocess->animations[route->parameter_index];
                    const s_render_method_texture *texture = &((const s_render_method_texture *)postprocess->textures.address)[route->constant_index];
                    float value = evaluate_function(context, animated);
                    const bitmap_group *group = (const bitmap_group *)tag_get(0, texture->bitmap.index);
                    long bitmap_index =
                        (long)((float)floor((float)fmod(
                                   (double)animated->function.map_to_output_range_legacy(value),
                                   (double)(float)group->bitmaps.count)) +
                            0.5f);
                    if (set_sampler_state)
                    {
                        long address_mode = texture->address_mode & 0xf;
                        c_rasterizer::set_sampler_address_mode(route->register_index, (c_rasterizer::e_sampler_address_mode)address_mode,
                            (c_rasterizer::e_sampler_address_mode)(texture->address_mode >> 4), (c_rasterizer::e_sampler_address_mode)address_mode);
                        c_rasterizer::set_sampler_filter_mode(route->register_index, static_cast<c_rasterizer::e_sampler_filter_mode>(texture->filter_mode));
                    }
                    c_rasterizer::x_last_sampler_textures[route->register_index] = -1;
                    render_method_submit_set_bitmap_texture(c_rasterizer::get_device(), 0, route->register_index, texture->bitmap.index, bitmap_index);
                }
                if ((pass->vertex_constants.value >> 10) != 0 || (pass->pixel_constants.value >> 10) != 0)
                {
                    real_vector4d constants[64];
                    s_destination_info destinations[64];
                    short destination_count = 0;
                    memcpy(constants, postprocess->real_constants.address, postprocess->real_constants.count() * sizeof(real_vector4d));
                    update_constants(context, constants, destinations, &destination_count, pass->vertex_constants,
                        postprocess->routing_info, postprocess->animations);
                    for (long destination_index = 0; destination_index < destination_count; destination_index++)
                        c_rasterizer::get_device()->SetVertexShaderConstantF(
                            destinations[destination_index].register_index, constants[destinations[destination_index].constant_index].n, 1);
                    update_constants(context, constants, destinations, &destination_count, pass->pixel_constants, postprocess->routing_info,
                        postprocess->animations);
                    for (long destination_index = 0; destination_index < destination_count; destination_index++)
                        c_rasterizer::get_device()->SetPixelShaderConstantF(
                            destinations[destination_index].register_index, constants[destinations[destination_index].constant_index].n, 1);
                }
            }
            result = true;
        }
    }
    return result;
}
static __forceinline void render_method_submit_constant_mask_set(unsigned __int64 *mask, long index)
{
    mask[0] |= (unsigned __int64)1 << (index / 4);
    mask[index / 64 + 1] |= (unsigned __int64)1 << (index % 64);
}
// @address 0x4574c0 render_method_submit_compute_constant_masks
extern "C" void render_method_submit_compute_constant_masks(c_rasterizer_xenon_memory *memory, s_rasterizer_entry *rasterizer_entry,
    const c_render_method *render_method, long entry_point,
    // match: 0x4574c0: pointer __restrict qualifier prevents alias reload
    unsigned __int64 *__restrict pixel_mask, unsigned __int64 *__restrict vertex_mask, unsigned long *__restrict texture_mask)
{
    const s_render_method_postprocess_definition *postprocess = render_method->get_postprocess_definition();
    const long template_index = postprocess->template_reference.index;
    const auto *shader_template = (const rasterizer_implicit_geometry_template *)tag_get(0, template_index);
    const auto &plan = render_method_submit_get_binding_plan(shader_template, template_index, entry_point);
    for (const auto &route : plan.ranges[_binding_pixel_extern])
    {
        if (route.extern_type == 0 || route.extern_type == 1)
        {
            rasterizer_entry->flags |= 0x200000;
            render_method_submit_constant_mask_set(pixel_mask, route.destination);
        }
    }
    for (const auto &route : plan.ranges[_binding_vertex_extern])
    {
        if (route.extern_type == 0 || route.extern_type == 1)
        {
            rasterizer_entry->flags |= 0x200000;
            render_method_submit_constant_mask_set(vertex_mask, route.destination);
        }
    }
    for (const auto &route : plan.ranges[_binding_extern_sampler])
    {
        if (route.extern_type == 0)
            rasterizer_entry->flags |= 0x200000;
    }
    const auto *textures = (const s_render_method_texture *)postprocess->textures.address;
    for (const auto &route : plan.ranges[_binding_texture])
    {
        const long sampler = route.destination + route.sampler_offset;
        if (textures[route.source].type)
        {
            *texture_mask |= 1 << sampler;
            if ((rasterizer_entry->flags & 0x3800000) == 0)
            {
                rasterizer_entry->flags |= 0x400000;
                g_rasterizer_xenon_memory.texture_entry_count++;
            }
        }
    }
    if (c_render_method_data::m_valid && entry_point < postprocess->entry_points.count)
    {
        const s_rm_postprocess_pass *pass = &postprocess->passes[((const unsigned short *)postprocess->entry_points.address)[entry_point] & 0x3ff];
        for (long index = 0; index < (long)(pass->pixel_constants.value >> 10); index++)
        {
            long register_index =
                postprocess->routing_info[(pass->pixel_constants.value & 0x3ff) + index]
                    .register_index;
            rasterizer_entry->flags |= 0x200000;
            render_method_submit_constant_mask_set(pixel_mask, register_index);
        }
        for (long index = 0; index < (long)(pass->vertex_constants.value >> 10); index++)
        {
            long register_index =
                postprocess->routing_info[(pass->vertex_constants.value & 0x3ff) + index]
                    .register_index;
            rasterizer_entry->flags |= 0x200000;
            render_method_submit_constant_mask_set(vertex_mask, register_index);
        }
        for (long index = 0; index < (long)(pass->textures.value >> 10); index++)
        {
            long register_index = postprocess->routing_info[(pass->textures.value & 0x3ff) + index].register_index;
            *texture_mask |= 1 << register_index;
            rasterizer_entry->flags |= 0x200000;
        }
    }
    if (entry_point == 1)
    {
        pixel_mask[0] |= g_rasterizer_xenon_memory.pixel_constant_masks[0].groups;
        vertex_mask[0] |= g_rasterizer_xenon_memory.vertex_constant_mask.groups;
    }
    else
    {
        pixel_mask[0] |= g_rasterizer_xenon_memory.pixel_constant_masks[1].groups;
        vertex_mask[0] |= g_rasterizer_xenon_memory.vertex_constant_mask.groups;
    }
}
static __forceinline s_dynamic_render_target *render_method_submit_dynamic_target(long type)
{
    if (g_dynamic_render_target_indices[type] != -1)
    {
        long index = g_dynamic_render_target_indices[type];
        return datum_get<s_dynamic_render_target>(g_dynamic_render_targets, index);
    }
    return 0;
}
// @address 0x8c4874 g_frame_texture_flags
// @address 0x8c48c0 g_uses_scene_texture
// match: 0x174bf0: canonical owner takes c_player_view *, while this caller retains long.
// @address 0x174bf0 render_method_submit_set_dynamic_texture
extern "C" bool render_method_submit_set_dynamic_texture(D3DDevice *device, long sampler, long texture_type)
{
    bool result = false;
    long target_type = 0;
    switch (texture_type)
    {
    case 1:
        {
            s_render_texture_camera camera;
            if (render_texture_camera_try_and_get(1, &camera, 0))
            {
                target_type = 1;
                g_frame_texture_flags |= 1;
            }
        }
        break;
    case 3:
        target_type = 2;
        break;
    case 4:
        target_type = 3;
        break;
    case 2:
        if (g_uses_scene_texture & 1)
        {
            c_rasterizer::set_aliased_surface_as_texture(sampler, c_rasterizer::_surface_dynamic);
            result = true;
        }
        break;
    }
    if (target_type != 0)
    {
        s_dynamic_render_target *target = render_method_submit_dynamic_target(target_type);
        if (target)
        {
            if (device)
                render_method_submit_set_device_texture(device, sampler, target->accumulation_texture);
            else
                c_rasterizer::set_sampler_texture(sampler, target->accumulation_texture);
            result = true;
        }
    }
    return result;
}
// The former local surface view called +0x44 width and +0x48 height.
// Access the canonical description members at those same offsets.
// @address 0x730e68 g_current_light_gel_index
// @address 0x81dc04 g_view_stack
struct render_structure_globals;
static __forceinline void render_method_submit_filter(long sampler, long filter)
{
    long *cache = g_rasterizer_sampler_filter_modes;
    if (cache[sampler] != filter)
    {
        if (sampler >= 0 && (unsigned long)sampler < 26)
            cache[sampler] = filter;
        rasterizer_device_set_sampler_filter_modes(c_rasterizer::get_device(), sampler, filter, filter);
    }
}
static __forceinline const c_rasterizer_globals *render_method_submit_globals()
{
    long index = global_game_globals->rasterizer_globals_ref.index;
    return index != -1 ? (const c_rasterizer_globals *)tag_get(0, index) : 0;
}
static __forceinline void render_method_submit_vector(float *value, float x, float y, float z, float w)
{
    value[0] = x;
    value[1] = y;
    value[2] = z;
    value[3] = w;
}
static __forceinline const render_camera *render_method_submit_current_camera()
{
    c_view *view = 0;
    if (c_view::get_current_stack_level() >= 0)
        view = g_view_stack[c_view::get_current_stack_level()];
    if (!view)
        view = c_player_view::get_current();
    return view->get_rasterizer_camera();
}
static __forceinline void render_method_submit_depth(const render_camera *camera, float *depth)
{
    const float &near_distance = camera->near_clip;
    float far_distance = camera->far_clip;
    depth[0] = 1.0f / near_distance;
    depth[1] = -((far_distance - near_distance) / (near_distance * far_distance));
    depth[2] = 0.0f;
    depth[3] = 0.0f;
}
// @address 0x173558 render_method_submit_set_extern_sampler
extern "C" void render_method_submit_set_extern_sampler(D3DDevice *device, s_rasterizer_xenon_memory_entry *memory_entry, long index,
    long sampler)
{
    float scene_dimensions[4];
    switch (index)
    {
    case _render_method_extern_texture_global_target_texaccum:
        {
            c_rasterizer::set_sampler_address_mode(sampler, c_rasterizer::_sampler_address_clamp,
                c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp);
            render_method_submit_filter(sampler, 1);
            if (g_albedo_buffer_source_texture.valid())
            {
                c_rasterizer::set_sampler_texture(sampler, g_albedo_buffer_source_texture);
                long width, height;
                c_rasterizer_texture_ref texture = g_albedo_buffer_source_texture;
                texture.get_width_height(0, &width, &height);
                scene_dimensions[0] = (float)width;
                scene_dimensions[1] = (float)height;
            }
            else
            {
                c_rasterizer::e_surface surface = (c_rasterizer::e_surface)17;
                c_rasterizer::set_aliased_surface_as_texture(sampler, surface);
                c_render_surface *render_surface = g_rasterizer_surface_groups[surface].get_specialized_surface(surface);
                scene_dimensions[0] = (float)render_surface->description.width;
                scene_dimensions[1] = (float)g_rasterizer_surface_groups[surface].get_specialized_surface(surface)->description.height;
            }
            c_rasterizer::get_device()->SetPixelShaderConstantF(14, scene_dimensions, 1);
        }
        break;
    case _render_method_extern_texture_global_target_normal:
        {
            c_rasterizer::set_sampler_address_mode(sampler, c_rasterizer::_sampler_address_clamp,
                c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp);
            render_method_submit_filter(sampler, 1);
            if (g_normal_buffer_source_texture.valid())
            {
                c_rasterizer::set_sampler_texture(sampler, g_normal_buffer_source_texture);
                long width, height;
                c_rasterizer_texture_ref texture = g_normal_buffer_source_texture;
                texture.get_width_height(0, &width, &height);
                scene_dimensions[0] = (float)width;
                scene_dimensions[1] = (float)height;
            }
            else
            {
                c_rasterizer::e_surface surface = (c_rasterizer::e_surface)19;
                c_rasterizer::set_aliased_surface_as_texture(sampler, surface);
                c_render_surface *render_surface = g_rasterizer_surface_groups[surface].get_specialized_surface(surface);
                scene_dimensions[0] = (float)render_surface->description.width;
                scene_dimensions[1] = (float)g_rasterizer_surface_groups[surface].get_specialized_surface(surface)->description.height;
            }
            c_rasterizer::get_device()->SetPixelShaderConstantF(14, scene_dimensions, 1);
        }
        break;
    case _render_method_extern_texture_global_target_z:
        {
            c_rasterizer::set_sampler_address_mode(sampler, c_rasterizer::_sampler_address_clamp,
                c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp);
            render_method_submit_filter(sampler, 1);
            c_rasterizer::set_aliased_surface_as_texture(sampler, (c_rasterizer::e_surface)3);
            const render_camera *camera = render_method_submit_current_camera();
            float depth[4];
            render_method_submit_depth(camera, depth);
            c_rasterizer::get_device()->SetPixelShaderConstantF(201, depth, 1);
        }
        break;
    case _render_method_extern_texture_global_target_shadow_buffer1:
        c_rasterizer::set_sampler_address_mode(sampler, c_rasterizer::_sampler_address_clamp,
                c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp);
        render_method_submit_filter(sampler, 2);
        if (rasterizer_is_surface_resolved((c_rasterizer::e_surface)2))
            c_rasterizer::set_aliased_surface_as_texture(sampler, (c_rasterizer::e_surface)2);
        else
            c_rasterizer::set_sampler_texture(sampler, render_method_submit_globals()->get_default_texture_rasterizer_texture_ref(0));
        break;
    case _render_method_extern_texture_global_target_texture_camera:
        c_rasterizer::set_sampler_address_mode(sampler, c_rasterizer::_sampler_address_clamp,
                c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp);
        render_method_submit_filter(sampler, 1);
        break;
    case _render_method_extern_texture_dynamic_light_gel_0:
        {
            bool success = false;
            if (g_current_light_gel_index != -1 && render_method_submit_set_texture(sampler, g_current_light_gel_index, 0))
            {
                c_rasterizer::set_sampler_address_mode(sampler, c_rasterizer::_sampler_address_wrap,
                c_rasterizer::_sampler_address_wrap, c_rasterizer::_sampler_address_wrap);
                render_method_submit_filter(sampler, 0);
                success = true;
            }
            if (!success)
                c_rasterizer::set_sampler_texture(sampler, render_method_submit_globals()->get_default_texture_rasterizer_texture_ref(0));
        }
        break;
    case _render_method_extern_texture_dynamic_environment_map_0:
        {
            render_method_submit_set_lightmap_texture(sampler, c_render_method_data::m_cubemap_dynamic_0_index,
                c_render_method_data::m_cubemap_dynamic_0_cluster_reference.bsp_index,
                c_render_method_data::m_cubemap_dynamic_0_cluster_reference.cluster_index);
        }
        break;
    case _render_method_extern_texture_dynamic_environment_map_1:
        {
            render_method_submit_set_lightmap_texture(sampler, c_render_method_data::m_cubemap_dynamic_1_index,
                c_render_method_data::m_cubemap_dynamic_1_cluster_reference.bsp_index,
                c_render_method_data::m_cubemap_dynamic_1_cluster_reference.cluster_index);
        }
        break;
    case _render_method_extern_texture_lightprobe_texture:
        if (c_render_method_data::m_lightmap_bsp_index != -1)
        {
            s_scenario_lightmap_bsp_data *lightmap = g_render_structure_globals.lightmap_bsp_data[c_render_method_data::m_lightmap_bsp_index];
            if (lightmap && lightmap->coefficients_bitmap.index != -1)
            {
                const bitmap_group *bitmap = (const bitmap_group *)tag_get(0, lightmap->coefficients_bitmap.index);
                if (bitmap->bitmaps.count > 0 && render_method_submit_set_texture(sampler, lightmap->coefficients_bitmap.index, 0))
                {
                    c_rasterizer::set_sampler_address_mode(sampler, c_rasterizer::_sampler_address_clamp,
                c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp);
                    render_method_submit_filter(sampler, 7);
                    int enabled;
                    if ((unsigned char)((const bitmap_data *)bitmap->bitmaps.address)->depth == 8)
                    {
                        enabled = 1;
                        float scale[4];
                        render_method_submit_vector(
                            scale, lightmap->coefficients_map_scale[0].scale, lightmap->coefficients_map_scale[1].scale,
                                lightmap->coefficients_map_scale[2].scale, 0.0f);
                        c_rasterizer::get_device()->SetPixelShaderConstantF(210, scale, 1);
                        render_method_submit_vector(
                            scale, lightmap->coefficients_map_scale[3].scale, lightmap->coefficients_map_scale[4].scale,
                                lightmap->coefficients_map_scale[5].scale, 0.0f);
                        c_rasterizer::get_device()->SetPixelShaderConstantF(211, scale, 1);
                        render_method_submit_vector(
                            scale, lightmap->coefficients_map_scale[6].scale, lightmap->coefficients_map_scale[7].scale,
                                lightmap->coefficients_map_scale[8].scale, 0.0f);
                        c_rasterizer::get_device()->SetPixelShaderConstantF(212, scale, 1);
                    }
                    else
                        enabled = 0;
                    c_rasterizer::get_device()->SetPixelShaderConstantB(10, &enabled, 1);
                }
            }
        }
        break;
    case _render_method_extern_texture_dominant_light_intensity_map:
        if (c_render_method_data::m_lightmap_bsp_index != -1)
        {
            s_scenario_lightmap_bsp_data *lightmap = g_render_structure_globals.lightmap_bsp_data[c_render_method_data::m_lightmap_bsp_index];
            if (lightmap && lightmap->dominant_light_bitmap.index != -1)
            {
                const bitmap_group *bitmap = (const bitmap_group *)tag_get(0, lightmap->dominant_light_bitmap.index);
                if (bitmap->bitmaps.count > 0 && render_method_submit_set_texture(sampler, lightmap->dominant_light_bitmap.index, 0))
                {
                    c_rasterizer::set_sampler_address_mode(sampler, c_rasterizer::_sampler_address_wrap,
                c_rasterizer::_sampler_address_wrap, c_rasterizer::_sampler_address_wrap);
                    render_method_submit_filter(sampler, 7);
                }
            }
        }
        break;
    case _render_method_extern_texture_cook_torrance_cc0236:
    case _render_method_extern_texture_cook_torrance_dd0236:
    case _render_method_extern_texture_cook_torrance_c78d78:
        render_method_submit_set_default_texture(device, memory_entry, index, sampler);
        break;
    case _render_method_extern_active_camo_distortion_texture:
        render_method_submit_set_texture(sampler, render_method_submit_globals()->m_active_camo_distortion_texture.index, 0);
        break;
    case _render_method_extern_scene_ldr_texture:
        {
            c_rasterizer::set_aliased_surface_as_texture(sampler, (c_rasterizer::e_surface)13);
            c_rasterizer::set_sampler_address_mode(sampler, c_rasterizer::_sampler_address_clamp,
                c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp);
            render_method_submit_filter(sampler, 2);
            float dimensions[4];
            c_render_surface *render_surface = g_rasterizer_surface_groups[13].get_specialized_surface((c_rasterizer::e_surface)13);
            dimensions[0] = (float)render_surface->description.width;
            dimensions[1] = (float)g_rasterizer_surface_groups[13].get_specialized_surface((c_rasterizer::e_surface)13)->description.height;
            c_rasterizer::get_device()->SetPixelShaderConstantF(14, dimensions, 1);
        }
        break;
    case _render_method_extern_scene_hdr_texture:
        {
            c_rasterizer::set_aliased_surface_as_texture(sampler, (c_rasterizer::e_surface)14);
            c_rasterizer::set_sampler_address_mode(sampler, c_rasterizer::_sampler_address_clamp,
                c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp);
            render_method_submit_filter(sampler, 2);
            float dimensions[4];
            c_render_surface *render_surface = g_rasterizer_surface_groups[14].get_specialized_surface((c_rasterizer::e_surface)14);
            dimensions[0] = (float)render_surface->description.width;
            dimensions[1] = (float)g_rasterizer_surface_groups[14].get_specialized_surface((c_rasterizer::e_surface)14)->description.height;
            c_rasterizer::get_device()->SetPixelShaderConstantF(14, dimensions, 1);
        }
        break;
    }
}
// @address 0x73a278 g_trilinear_threshold
static __forceinline void render_method_submit_set_address_mode(D3DDevice *device, long sampler, c_rasterizer::e_sampler_address_mode u,
    c_rasterizer::e_sampler_address_mode v, c_rasterizer::e_sampler_address_mode w)
{
    device->SetSamplerState(sampler, D3DSAMP_ADDRESSU, k_d3d_texture_address_modes[u]);
    device->SetSamplerState(sampler, D3DSAMP_ADDRESSV, k_d3d_texture_address_modes[v]);
    device->SetSamplerState(sampler, D3DSAMP_ADDRESSW, k_d3d_texture_address_modes[w]);
}
static __forceinline long render_method_submit_allocate_constant()
{
    if (g_rasterizer_xenon_memory.m_stage < 4096)
        return g_rasterizer_xenon_memory.m_stage++;
    return -1;
}
static __forceinline long render_method_submit_get_frame(const s_render_method_texture *texture)
{
    if (texture->index > 0)
        return texture->index;
    return 0;
}
// @address 0x457ba0 render_method_submit_set_staged_constants
extern "C" unsigned char render_method_submit_set_staged_constants(c_rasterizer_xenon_memory *memory, s_rasterizer_xenon_memory_request *request,
    const c_render_method *render_method,
    const s_render_method_postprocess_definition *postprocess, const rasterizer_implicit_geometry_template *definition,
    // match: 0x457ba0: pointer __restrict qualifier prevents alias reload
    const unsigned __int64 *__restrict vertex_mask, const unsigned __int64 *__restrict pixel_mask,
    const long *texture_mask, long entry_point)
{
    bool result = true;
    const auto &plan = render_method_submit_get_binding_plan(definition, postprocess->template_reference.index, entry_point);
    const s_render_method_texture *textures = (const s_render_method_texture *)postprocess->textures.address;
    const real_vector4d *values = (const real_vector4d *)postprocess->real_constants.address;
    for (const auto &route : plan.ranges[_binding_extern_sampler])
    {
        if (route.extern_type == 1)
        {
            s_rasterizer_xenon_memory_entry *memory_entry = g_rasterizer_xenon_memory.new_entry(request);
            if (memory_entry != 0)
            {
                render_method_submit_set_extern_sampler(g_command_buffer_device, memory_entry, route.source, route.destination);
                long sampler = route.destination;
                if (g_trilinear_threshold <= 3)
                    g_command_buffer_device->m_Constants.TextureFetch[sampler].TriClamp = g_trilinear_threshold;
            }
            else
                result = false;
        }
    }
    for (const auto &route : plan.ranges[_binding_texture])
    {
        long sampler = route.destination;
        const s_render_method_texture *texture = &textures[route.source];
        // The recorded-state mask tests the serialized sampler before remapping it.
        if (((1 << sampler) & *texture_mask) == 0)
        {
            sampler += route.sampler_offset;
            render_method_submit_set_address_mode(g_command_buffer_device, sampler, (c_rasterizer::e_sampler_address_mode)(texture->address_mode & 0xf),
                (c_rasterizer::e_sampler_address_mode)(texture->address_mode >> 4), (c_rasterizer::e_sampler_address_mode)(texture->address_mode & 0xf));
            unsigned long filter = texture->filter_mode;
            if (filter <= 7)
                rasterizer_device_set_sampler_filter_modes(g_command_buffer_device, sampler, filter, filter);
            if (render_method_submit_set_dynamic_texture(g_command_buffer_device, sampler, texture->type) == 0)
            {
                if (texture->bitmap.index != -1)
                {
                    const bitmap_group *bitmap =
                        (const bitmap_group *)global_tag_instances[DATUM_INDEX_TO_ABSOLUTE_INDEX(texture->bitmap.index)].definition;
                    long frame = MIN(render_method_submit_get_frame(texture), bitmap->bitmaps.count - 1);
                    s_rasterizer_xenon_memory_entry *memory_entry = g_rasterizer_xenon_memory.new_entry(request);
                    if (memory_entry != 0)
                        render_method_submit_set_bitmap_texture(g_command_buffer_device, memory_entry, sampler, texture->bitmap.index, frame);
                    else
                        result = false;
                }
                else
                {
                    if (texture->type != 0)
                    {
                        long globals_index = global_game_globals->rasterizer_globals_ref.index;
                        const c_rasterizer_globals *globals;
                        if (globals_index != -1)
                            globals = TAG_GET('rasg', const c_rasterizer_globals, globals_index);
                        else
                            globals = 0;
                        const s_rasterizer_default_texture *default_texture = (const s_rasterizer_default_texture *)globals->m_default_textures_refs.address;
                        c_rasterizer_texture_ref texture_ref;
                        texture_ref.clear();
                        const bitmap_data *bitmap_entry;
                        s_rasterizer_xenon_memory_entry *memory_entry = g_rasterizer_xenon_memory.new_entry(request);
                        if (memory_entry != 0)
                        {
                            ::bitmap_group_get_xenon_hardware_format(default_texture->bitmap.index, 0, &texture_ref,
                                &bitmap_entry, (__int64 *)memory_entry);
                            render_method_submit_set_device_texture(g_command_buffer_device, sampler, texture_ref);
                        }
                        else
                            result = false;
                    }
                }
            }
            if (g_trilinear_threshold <= 3)
                g_command_buffer_device->m_Constants.TextureFetch[sampler].TriClamp = g_trilinear_threshold;
        }
    }
#ifdef HALO3_64BIT
    typedef char rasterizer_submit_entry_flags_offset_64[(offsetof(s_rasterizer_entry, flags) == 16) ? 1 : -1];
    if ((((const s_rasterizer_entry *)request)->flags & 0x3800000u) == 0)
#else
    if (request->shared_type == 0)
#endif
    {
        for (const auto &route : plan.ranges[_binding_pixel_constant])
        {
            long register_index = route.destination;
            if (((1ui64 << ((unsigned __int64)register_index >> 2)) & pixel_mask[0]) != 0)
            {
                if (((1ui64 << (register_index % 64)) & pixel_mask[(register_index / 64) + 1]) == 0)
                {
                    long stage = render_method_submit_allocate_constant();
                    if (stage != -1)
                    {
#ifdef HALO3_64BIT
                        {
                            s_rasterizer_entry *cached_request = (s_rasterizer_entry *)request;
                            cached_request->flags = (cached_request->flags & ~0xffu) | ((cached_request->flags + 1u) & 0xffu);
                        }
#else
                        request->unknown_bits++;
#endif
                        memcpy(g_rasterizer_xenon_memory.stage_data[stage], &values[route.source], sizeof(values[route.source]));
                        g_rasterizer_xenon_memory.stage_keys[stage] = route.destination + 0x100;
                    }
                    else
                        result = false;
                }
            }
            else
            {
                g_command_buffer_device->SetPixelShaderConstantF(route.destination, values[route.source].n, 1);
            }
        }
        for (const auto &route : plan.ranges[_binding_vertex_constant])
        {
            long register_index = route.destination;
            if (((1ui64 << ((unsigned __int64)register_index >> 2)) & vertex_mask[0]) != 0)
            {
                if (((1ui64 << (register_index % 64)) & vertex_mask[(register_index / 64) + 1]) == 0)
                {
                    long stage = render_method_submit_allocate_constant();
                    if (stage != -1)
                    {
#ifdef HALO3_64BIT
                        {
                            s_rasterizer_entry *cached_request = (s_rasterizer_entry *)request;
                            cached_request->flags = (cached_request->flags & ~0xffu) | ((cached_request->flags + 1u) & 0xffu);
                        }
#else
                        request->unknown_bits++;
#endif
                        memcpy(g_rasterizer_xenon_memory.stage_data[stage], &values[route.source], sizeof(values[route.source]));
                        g_rasterizer_xenon_memory.stage_keys[stage] = route.destination;
                    }
                    else
                        result = false;
                }
            }
            else
            {
                g_command_buffer_device->SetVertexShaderConstantF(route.destination, values[route.source].n, 1);
            }
        }
    }
    else
    {
        for (const auto &route : plan.ranges[_binding_pixel_constant])
        {
            long register_index = route.destination;
            if (((1ui64 << ((unsigned __int64)register_index >> 2)) & pixel_mask[0]) == 0)
                g_command_buffer_device->SetPixelShaderConstantF(route.destination, values[route.source].n, 1);
        }
        for (const auto &route : plan.ranges[_binding_vertex_constant])
        {
            long register_index = route.destination;
            if (((1ui64 << ((unsigned __int64)register_index >> 2)) & vertex_mask[0]) == 0)
                g_command_buffer_device->SetVertexShaderConstantF(route.destination, values[route.source].n, 1);
        }
    }
    return result;
}
static __forceinline real_rgb_color render_method_submit_player_color(long index)
{
    real_rgb_color color;
    return *player_profile_get_rgb_color(&color, index);
}
static __forceinline real_vector3d *render_method_submit_cross(const real_vector3d *a, const real_vector3d *b, real_vector3d *result)
{
    float k = a->i * b->j - a->j * b->i;
    float i = a->j * b->k - a->k * b->j;
    float j = a->k * b->i - a->i * b->k;
    result->i = i;
    result->j = j;
    result->k = k;
    return result;
}
// match: 0x44e008: __restrict on value keeps the later bsp_index lbz after these stores
static __forceinline void render_method_submit_constant(float *__restrict value, float x, float y, float z, float w)
{
    value[0] = x;
    value[1] = y;
    value[2] = z;
    value[3] = w;
}
static __forceinline const real_vector4d *render_method_submit_light_direction()
{
    return &g_render_method_submit_light_direction;
}
static __forceinline const real_rgb_color *render_method_submit_light_intensity()
{
    return &g_render_method_submit_light_intensity;
}
static __forceinline void render_method_submit_color(real_vector4d *value, real_rgb_color color, float alpha)
{
    *(real_rgb_color *)value = color;
    value->w = alpha;
}
// @address 0x44e008 render_method_submit_get_extern_constant
extern "C" void render_method_submit_get_extern_constant(long index, real_vector4d *value)
{
    switch (index)
    {
    case _render_method_extern_object_emblem_color_background:
    case _render_method_extern_object_emblem_color_primary:
    case _render_method_extern_object_emblem_color_secondary:
        {
            long color_index;
            float alpha;
            if (tls_game_globals()->options.game_mode == 3)
            {
                switch (index)
                {
                case _render_method_extern_object_emblem_color_background:
                    color_index = g_emblem_render_info.background_color;
                    alpha = (g_emblem_render_info.flags & 1) ? 0.0f : 1.0f;
                    break;
                case _render_method_extern_object_emblem_color_primary:
                    color_index = g_emblem_render_info.primary_color;
                    alpha = (g_emblem_render_info.flags & 2) ? 1.0f : 0.0f;
                    break;
                default:
                    color_index = g_emblem_render_info.secondary_color;
                    alpha = (g_emblem_render_info.flags & 4) ? 1.0f : 0.0f;
                    break;
                }
            }
            else
            {
                long color_function, alpha_function;
                switch (index)
                {
                case _render_method_extern_object_emblem_color_background: color_function = 636; alpha_function = 631; break;
                case _render_method_extern_object_emblem_color_primary: color_function = 634; alpha_function = 632; break;
                default: color_function = 635; alpha_function = 633; break;
                }
                color_index = (long)c_render_method_data::m_context_interface(
                    c_render_method_data::m_context, color_function, -1);
                alpha = c_render_method_data::m_context_interface(c_render_method_data::m_context, alpha_function, -1);
            }
            render_method_submit_color(value, render_method_submit_player_color(color_index), alpha);
        }
        break;
    case _render_method_extern_object_change_color_primary:
    case _render_method_extern_object_change_color_secondary:
    case _render_method_extern_object_change_color_tertiary:
    case _render_method_extern_object_change_color_quaternary:
        {
            unsigned long color = c_render_method_data::m_change_colors[index - 15];
            value->w = 1.0f;
            value->i = (float)((color >> 16) & 255) * 0.003921569f;
            value->j = (float)((color >> 8) & 255) * 0.003921569f;
            value->k = (float)(color & 255) * 0.003921569f;
        }
        break;
    case _render_method_extern_light_dir_0:
    case _render_method_extern_light_dir_1:
    case _render_method_extern_light_dir_2:
    case _render_method_extern_light_dir_3:
        {
            const real_vector4d *direction = render_method_submit_light_direction();
            render_method_submit_constant(value->n, direction->i, direction->j, direction->k, direction->w);
        }
        break;
    case _render_method_extern_light_color_0:
    case _render_method_extern_light_color_1:
    case _render_method_extern_light_color_2:
    case _render_method_extern_light_color_3:
        {
            const real_rgb_color *intensity = render_method_submit_light_intensity();
            render_method_submit_constant(value->n, intensity->red, intensity->green, intensity->blue, 1.0f);
        }
        break;
    case _render_method_extern_flat_envmap_matrix_x:
        {
            render_method_submit_constant(value->n, 1.0f, 0.0f, 0.0f, 0.0f);
            s_render_texture_camera camera;
            if (render_texture_camera_try_and_get(1, &camera, 0))
            {
                real_vector3d right;
                render_method_submit_cross(&camera.forward, &camera.up, &right);
                render_method_submit_constant(value->n, right.i, right.j, right.k, 0.0f);
            }
        }
        break;
    case _render_method_extern_flat_envmap_matrix_y:
        {
            render_method_submit_constant(value->n, 0.0f, 0.0f, 1.0f, 0.0f);
            s_render_texture_camera camera;
            if (render_texture_camera_try_and_get(1, &camera, 0))
                render_method_submit_constant(value->n, -camera.up.n[0], -camera.up.n[1], -camera.up.n[2], 0.0f);
        }
        break;
    case _render_method_extern_flat_envmap_matrix_z:
        {
            render_method_submit_constant(value->n, 0.0f, 1.0f, 0.0f, 0.0f);
            s_render_texture_camera camera;
            if (render_texture_camera_try_and_get(1, &camera, 0))
                render_method_submit_constant(value->n, -camera.forward.n[0], -camera.forward.n[1], -camera.forward.n[2], 0.0f);
        }
        break;
    case _render_method_extern_debug_tint:
        render_method_submit_constant(value->n, 1.0f, 1.0f, 1.0f, 0.0f);
        if (cluster_reference_valid(&c_render_method_data::m_lightmap_cluster_reference))
            render_method_submit_constant(value->n, g_render_method_submit_cluster_color.rgb.red, g_render_method_submit_cluster_color.rgb.green,
                g_render_method_submit_cluster_color.rgb.blue, g_render_method_submit_cluster_color.alpha);
        break;
    case _render_method_extern_screen_constants:
        {
        value->i = 1.0f / (float)c_rasterizer::render_globals.width;
        value->j = 1.0f / (float)c_rasterizer::render_globals.height;
        float field_of_view = c_player_view::get_current()->get_rasterizer_camera()->vertical_field_of_view;
        value->k = (float)c_rasterizer::render_globals.height / ((float)tan(field_of_view * 0.5f) * 2.0f);
        value->w = 0.0f;
        }
        break;
    case _render_method_extern_water_memory_export_address:
        *value = g_render_method_submit_extern_constant_47;
        break;
    case _render_method_extern_tree_animation_timer:
        {
            if (g_render_method_submit_loop_phase_tick != tls_game_time_globals()->time)
            {
                g_render_method_submit_loop_phase_tick = tls_game_time_globals()->time;
                float phase = tls_game_time_globals()->tick_length * (float)g_render_method_submit_loop_phase_tick * 0.6f;
                g_render_method_submit_loop_phase = phase - (float)floor(phase);
            }
            value->i = g_render_method_submit_loop_phase;
        }
        break;
    default:
        value->w = 1.0f;
        value->k = 1.0f;
        value->j = 1.0f;
        value->i = 1.0f;
        break;
    }
}
// @address 0x4579c8 render_method_submit_set_shaders_and_cache
extern "C" unsigned char render_method_submit_set_shaders_and_cache(
    c_rasterizer_xenon_memory *memory, s_rasterizer_entry *request, const c_render_method *render_method, long vertex_type, long transfer_type,
    long entry_point)
{
    unsigned long e0[4];
    int d0[8];
    bool result = false;
    const s_render_method_postprocess_definition *postprocess = (const s_render_method_postprocess_definition *)render_method->postprocess.address;
    long tag_index = postprocess->template_reference.index;
    if (tag_index != -1)
    {
        const rasterizer_implicit_geometry_template *definition = (const rasterizer_implicit_geometry_template *)tag_get(0, tag_index);
        if ((definition->entry_point_mask & (1u << entry_point)) != 0)
        {
            D3DVertexDeclaration *vertex_declaration = rasterizer_get_vertex_declaration(
                (e_vertex_type)vertex_type, (e_transfer_vector_vertex_types)transfer_type, (e_entry_point)entry_point);
            const auto &shader_plan = render_method_submit_get_shader_plan(render_method, definition,
                (e_vertex_type)vertex_type, (e_transfer_vector_vertex_types)transfer_type, (e_entry_point)entry_point, false);
            const auto *vertex_shader = shader_plan.vertex_shader;
            const auto *pixel_shader = shader_plan.pixel_shader;
            if (vertex_shader != 0)
            {
                if (pixel_shader != 0)
                {
                    D3D::CVertexShader *d3d_vertex_shader = D3D::Promote(vertex_shader->get_d3d_shader());
                    D3DPixelShader *d3d_pixel_shader = pixel_shader->get_d3d_shader();
                    render_method_submit_build_declaration(vertex_type, transfer_type, entry_point, (s_render_method_submit_declaration *)d0);
                    if ((d3d_vertex_shader->UCodeMagicAndFlags & 0x40) == 0)
                    {
                        unsigned long fence = d3d_vertex_shader->Fence;
                        if (fence != 0)
                        {
                            D3DDevice_BlockOnFence(fence);
                            d3d_vertex_shader->Fence = 0;
                        }
                        D3DVertexShader_Bind(d3d_vertex_shader, 0, vertex_declaration, (const DWORD *)d0, d3d_pixel_shader);
                    }
                    D3DDevice_SetVertexDeclaration(g_command_buffer_device, 0);
                    D3DDevice_SetVertexShader(g_command_buffer_device, d3d_vertex_shader);
                    D3DDevice_SetPixelShader(g_command_buffer_device, d3d_pixel_shader);
                    if (g_rasterizer_xenon_memory.transparent == 1)
                        rasterizer_set_device_alpha_blend_mode(g_command_buffer_device, postprocess->alpha_blend_mode);
                    unsigned __int64 pixel_mask[5] = {0};
                    unsigned __int64 vertex_mask[5] = {0};
                    e0[0] = 0;
                    render_method_submit_compute_constant_masks(&g_rasterizer_xenon_memory, request, render_method, entry_point,
                        pixel_mask, vertex_mask, (unsigned long *)e0);
                    if (render_method_submit_set_staged_constants(&g_rasterizer_xenon_memory,
                            (s_rasterizer_xenon_memory_request *)request, render_method,
                            (const s_render_method_postprocess_definition *)postprocess, (const rasterizer_implicit_geometry_template *)definition,
                            vertex_mask, pixel_mask, (const long *)e0, entry_point) != 0)
                        result = true;
                }
            }
        }
    }
    return result;
}
// @address 0x458438 render_method_submit_build_cached_entry
extern "C" unsigned char render_method_submit_build_cached_entry(c_rasterizer_xenon_memory *memory, s_rasterizer_entry *cached,
    const c_render_method *render_method, long tag_index, const s_render_geometry *geometry,
    const s_mesh *part, const s_part *range, long mode)
{
    struct s_render_method_submit_sizes
    {
        int physical;
        int runtime;
        long index_type;
    } sizes;
    bool drawn = false;
    bool success = false;
    bool transparent = tag_index != -1 &&
        ((const c_render_method *)tag_get(0, tag_index))
            ->get_postprocess_definition()->alpha_blend_mode != 0;
    D3DTAGCOLLECTION *state;
    if (transparent)
    {
        g_rasterizer_xenon_memory.transparent = 1;
        state = &g_rasterizer_xenon_memory.transparent_state;
    }
    else
    {
        g_rasterizer_xenon_memory.transparent = 0;
        state = &g_rasterizer_xenon_memory.opaque_state;
    }
    D3DDevice_BeginCommandBuffer(g_command_buffer_device, (D3DCommandBuffer *)g_rasterizer_xenon_memory.m_command_buffer,
        g_rasterizer_xenon_memory.m_command_buffer_flags, state,
        &g_rasterizer_xenon_memory.state_a, g_rasterizer_xenon_memory.m_tile_rects, g_rasterizer_xenon_memory.m_tile_count);
    g_rasterizer_xenon_memory.submit_count++;
    g_rasterizer_xenon_memory.command_buffer_build_count++;
    g_rasterizer_xenon_memory.m_shared_entry_count = 0;
    if (render_method_submit_set_mesh_streams(
            &g_rasterizer_xenon_memory, (s_rasterizer_xenon_memory_request *)cached, geometry, part, &sizes.index_type))
    {
        long kind = part->prt_type;
        long entry_point;
        if (mode == 8)
        {
            if (part->flags & 1)
                entry_point = 14;
            else if (!g_rasterizer_xenon_memory.alternate_entry_point)
                entry_point = k_mesh_part_type_entry_points[kind];
            else
                entry_point = 5;
        }
        else
            entry_point = mode;
        if (render_method_submit_set_shaders_and_cache(&g_rasterizer_xenon_memory, cached, render_method, part->vertex_type, kind, entry_point))
        {
            long index_type = sizes.index_type;
            g_command_buffer_device->m_Pending.m_Mask[2] &= ~(1ui64 << 17);
            long count = index_type == 5 ? (long)range->count - 2 : (long)range->count / 3;
            D3DPRIMITIVETYPE primitive = g_primitive_types[index_type];
            D3DDevice_DrawIndexedVertices(g_command_buffer_device, primitive, 0, range->start,
                D3DVERTEXCOUNT(primitive, count));
            drawn = true;
        }
    }
    D3DDevice_EndCommandBuffer(g_command_buffer_device);
    if (drawn)
    {
        D3DCommandBuffer_GetClone((D3DCommandBuffer *)g_rasterizer_xenon_memory.m_command_buffer, 0, 0, (DWORD *)&sizes.runtime, 0, (DWORD *)&sizes.physical);
        bool allocated = true;
        DWORD physical_buffer_address;
        if ((cached->flags & 0x3800000) == 0)
        {
            allocated = false;
            if (g_rasterizer_xenon_memory.runtime_offset + sizes.runtime <= 0xc0000 &&
                g_rasterizer_xenon_memory.physical_offset + sizes.physical <= 0x1c0000)
            {
                cached->buffer = (D3DCommandBuffer *)((char *)g_rasterizer_xenon_memory.m_runtime_buffer + g_rasterizer_xenon_memory.runtime_offset);
                physical_buffer_address = address_from_pointer(
                    static_cast<char *>(g_rasterizer_xenon_memory.m_physical_buffer) + g_rasterizer_xenon_memory.physical_offset);
                g_rasterizer_xenon_memory.runtime_offset = ((sizes.runtime + 15) & ~15) + g_rasterizer_xenon_memory.runtime_offset;
                g_rasterizer_xenon_memory.physical_offset = ((sizes.physical + 31) & ~31) + g_rasterizer_xenon_memory.physical_offset;
                allocated = true;
            }
            else
                physical_buffer_address = sizes.index_type;
        }
        else
            physical_buffer_address = cached->buffer->Identifier;
        if (allocated)
        {
            D3DCommandBuffer_GetClone((D3DCommandBuffer *)g_rasterizer_xenon_memory.m_command_buffer, 0, cached->buffer, (DWORD *)&sizes.runtime,
                pointer_from_address(physical_buffer_address), (DWORD *)&sizes.physical);
            success = true;
            cached->flags = (cached->flags & 0xfc7fffff) | 0x01000000;
            cached->buffer->Identifier = physical_buffer_address;
        }
    }
    if (!success)
    {
        cached->flags = (cached->flags & 0xfc7fffff) | 0x00800000;
        g_rasterizer_xenon_memory.command_buffer_clone_failure_count++;
    }
    return success;
}
// @address 0x73a160 ?k_default_render_method_submit_cache@@3Us_render_method_submit_cache@@A
s_render_method_submit_cache k_default_render_method_submit_cache = { true, NULL, 22, 18 };
// @address 0x10f5d20 ?g_render_method_submit_cache@@3Us_render_method_submit_cache@@A
s_render_method_submit_cache g_render_method_submit_cache = k_default_render_method_submit_cache;
