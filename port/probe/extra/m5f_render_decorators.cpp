// tmp/probe/extra (GAME-CHANGES F48): c_structure_renderer::render_decorators (retail 0x8216f100) and its two
// helpers (0x8216f8c0 wind, 0x8216fb18 sun), not in source: prebuilt.tsv lists all three as "between library
// functions" (LTCG placed them among d3d9 state.obj's routines). Decompiled from the retail listing (recomp oracle
// sub_8216F100, sub_8216F8C0, sub_8216FB18). The direct device constant stores of the listing are the inlined
// c_rasterizer::set_vertex_shader_constant / set_pixel_shader_constant (VS c240-c249, PS c13).
//
// Every visible cluster's decorator groups (structure_cluster.decorator_groups, 48 bytes each) that are within the
// decorator set's draw distance and in the visibility region are drawn as one auto-indexed, instanced draw: the
// set's render model supplies the vertices (stream from its geometry), the BSP's decorator instance buffer the
// per-instance data (at the group's offset), the vertex shader (explicit shader per render type) expands
// index_count * instance_count vertices.
#include <math.h>
#include <xtl.h>
#include "math/real_math_types.h"
#include "rasterizer/rasterizer.h"
#include "rasterizer/rasterizer_main_runtime_globals.h"
#include "rasterizer/rasterizer_vertex_buffer.h"
#include "rasterizer/rasterizer_vertex_types.h"
#include "rasterizer/rasterizer_sampler_filter_state.h"
#include "rasterizer/rasterizer_global_definitions.h"
#include "rasterizer/rasterizer_primitives.h"
#include "render/structure_renderer.h"
#include "render/views/player_view_types.h"
#include "render/views/render_player_view.h"
#include "render/render_objects_globals.h"
#include "render_methods/render_method_data.h"
#include "render_methods/render_method_submit.h"
#include "scenario/scenario.h"
#include "scenario/scenario_tag.h"
#include "scenario/scenario_definitions.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "scenario/wind_definitions.h"
#include "structures/structure_bsp_tag.h"
#include "tag_files/tag_groups.h"
#include "models/render_model_definitions.h"
#include "geometry/render_geometry_types.h"
#include "geometry/render_geometry_mesh_types.h"
#include "geometry/render_geometry_parts.h"
#include "geometry/geometry_definitions.h"
#include "game/game_globals.h"
#include "game/game_globals_storage.h"
#include "bitmaps/bitmap_group.h"
#include "visibility/visibility_globals.h"
#include "visibility/visibility_collection.h"
#include "visibility/visibility_region_types.h"
#include "visibility/visibility_collection_objects_imports.h"
#include "visibility/visibility_fade.h"
#include "render/render_lightmap_shadows_globals.h"
#include "sky_atm/atmosphere_external.h"
#include "sky_atm/real_linear_rgb_color_types.h"
#include "runtime/port_config.h"

extern s_wind_values g_wind_values;
extern "C" void d3d9_set_vertex_shader_bounds(struct s_d3d9_state_bounds *bounds);
extern "C" void rasterizer_set_alpha_test_enabled(unsigned char enable, int);

// A decorator group of a structure cluster (structure_cluster.decorator_groups).
struct s_decorator_group
{
    unsigned short instance_count;
    unsigned char decorator_set_index;     // structure_bsp.decorator_sets
    unsigned char instance_buffer_index;   // vertex buffer of structure_bsp.decorator_instance_buffer
    unsigned long instance_buffer_offset;  // bytes
    union {
        real_vector4d instance_constants[2]; // VS c240, c241 (the first's w is the bounding radius)
        struct { char unused8[28]; real_point3d bounding_center; }; // from the second constant's w on
    };
};
typedef char decorator_group_size[(sizeof(s_decorator_group) == 48) ? 1 : -1];

// The decorator set tag ('dctr') as far as the renderer reads it.
struct s_decorator_set_definition
{
    s_tag_reference render_model;
    char unused10[16];
    s_tag_reference texture;
    unsigned char render_flags;            // bit 0: two-sided (no culling)
    unsigned char render_shader;           // k_decorator_explicit_shaders
    unsigned char unused32[2];
    real_vector4d vertex_constants_246;
    real_vector4d vertex_constants_249;
    real_vector4d pixel_constants_13;
    float lod_fade_start;                  // full opacity up to here (pixel-scaled)
    float lod_fade_end;                    // 30% at the end; beyond it (plus the group radius) the group is skipped
    float early_cull;                      // the cull distance is scaled by 1 - early_cull
};
typedef char decorator_set_definition_layout[(offsetof(s_decorator_set_definition, lod_fade_start) == 100) ? 1 : -1];

// The scenario structure BSP reference's wind tag (+0x58, after decorator_model).
static __forceinline long decorator_structure_bsp_wind_index(const scenario_structure_bsp_reference *reference)
{
    return reference->wind.index;
}

// 0x8204ba94: explicit shader per decorator render type.
static const long k_decorator_explicit_shaders[6] = { 0x2f, 0x46, 0x47, 0x48, 0x49, 0x52 };

// The sun of the sky lighting the current cluster: VS c247 (direction) / c248 (color), 0x8281dc14.
struct s_decorator_sun
{
    real_vector4d color;
    real_vector4d direction;
};
static s_decorator_sun g_decorator_sun;
static long g_decorator_sun_sky_index = -1; // 0x8273a190

// 0x8216fb18
static void decorator_set_sky_sun(long sky_index)
{
    if (g_decorator_sun_sky_index != sky_index) {
        atmosphere_query_sky_sun_parameters(sky_index, (real_linear_rgb_color *)&g_decorator_sun.color,
            (real_vector3d *)&g_decorator_sun.direction);
        g_decorator_sun_sky_index = sky_index;
    }
    c_rasterizer::set_vertex_shader_constant(247, 1, &g_decorator_sun.direction);
    c_rasterizer::set_vertex_shader_constant(248, 1, &g_decorator_sun.color);
}

// 0x8216f8c0: the wind's gust noise bitmap (else the default texture 10, rasterizer globals) on vertex sampler 20,
// wrapped and filtered, and the wind values' two vectors in VS c243 / c244.
static void decorator_set_wind(const s_wind_definition *wind)
{
    c_rasterizer_texture_ref texture;
    if (wind != 0 && wind->gust_noise_bitmap.index != -1) {
        const bitmap_data *bitmap = 0;
        __int64 unused = 0;
        bitmap_group_get_xenon_hardware_format(wind->gust_noise_bitmap.index, 0, &texture, &bitmap, &unused);
    } else {
        const long globals_index = global_game_globals->rasterizer_globals_ref.index;
        const c_rasterizer_globals *globals = globals_index != -1 ? (const c_rasterizer_globals *)tag_get('rasg', globals_index) : 0;
        texture = globals->get_default_texture_rasterizer_texture_ref(10);
    }
    c_rasterizer::set_sampler_texture(20, texture);
    c_rasterizer::set_sampler_address_mode(20, (c_rasterizer::e_sampler_address_mode)0, (c_rasterizer::e_sampler_address_mode)0,
        (c_rasterizer::e_sampler_address_mode)0);
    if (g_rasterizer_sampler_filter_modes[20] != 2) {
        g_rasterizer_sampler_filter_modes[20] = 2;
        rasterizer_device_set_sampler_filter_modes(g_rasterizer_device, 20, 2, 2);
    }
    c_rasterizer::set_vertex_shader_constant(243, 1, (const real_vector4d *)&g_wind_values.texture_x);
    c_rasterizer::set_vertex_shader_constant(244, 1, (const real_vector4d *)&g_wind_values.velocity_x);
}

// rasterizer_geometry.cpp's k_vertex_stream_from_type[k_vertex_type_from_declaration[declaration]] (retail .rdata:
// 30 declarations; 20 is the decorator instance stream -> type 4 -> stream 1)
static const long k_decorator_declaration_count = 30;
static unsigned long decorator_stream_from_declaration(c_rasterizer_vertex_buffer::e_vertex_buffer_declaration declaration)
{
    static const unsigned long k_vertex_stream_from_type[8] = { 0, 4, 4, 2, 1, 2, 2, 3 };
    static const long k_vertex_type_from_declaration[k_decorator_declaration_count] = {
        0, 0, 0, 0, 1, 2, 1, 0, 3, 3, 3, 3, 0, 0, 2,
        0, 4, 5, 4, 4, 4, 5, 5, 5, 0, 0, 6, 7, 0, 4 };
    return k_vertex_stream_from_type[k_vertex_type_from_declaration[declaration]];
}

void c_structure_renderer::render_decorators()
{
    const float scale_setting = g_player_render_tuning.f8; // 0x828368d8: the decorator distance scale (F53: one datum)
    if (scale_setting == 0.0f)
        return;
    const float draw_distance_scale = port_config_draw_distance();
    const real_point3d camera = c_player_view::get_current()->render_camera_state.position;

    // the wind of the camera's structure BSP (else the globals' default wind)
    long structure_bsp_index = -1;
    s_cluster_reference camera_cluster;
    const long leaf_index = scenario_leaf_index_from_point(&camera, &structure_bsp_index);
    if (leaf_index != -1)
        camera_cluster = scenario_cluster_reference_from_leaf_index(structure_bsp_index, leaf_index);
    else
        *(short *)&camera_cluster = -1;
    long wind_index;
    if (camera_cluster.bsp_index != -1)
        wind_index = decorator_structure_bsp_wind_index(
            &((const scenario_structure_bsp_reference *)global_scenario->structure_bsps.address)[camera_cluster.bsp_index]);
    else
        wind_index = global_game_globals->default_wind.index;
    const s_wind_definition *wind = 0;
    if (wind_index != -1) {
        wind = (const s_wind_definition *)tag_get('wind', wind_index);
        if (wind != 0)
            wind->update(&g_wind_values);
    }

    c_rasterizer::set_z_buffer_mode((c_rasterizer::e_z_buffer_mode)0);
    D3DDevice_SetRenderState_ColorWriteEnable(g_rasterizer_device, 15);
    decorator_set_wind(wind);
    c_rasterizer::set_stencil_mode((c_rasterizer::e_stencil_mode)9);

    g_decorator_sun.color.i = 1.0f;
    g_decorator_sun.color.j = 1.0f;
    g_decorator_sun.color.k = 1.0f;
    g_decorator_sun.color.l = 0.0f;
    g_decorator_sun.direction.i = 0.0f;
    g_decorator_sun.direction.j = 0.0f;
    g_decorator_sun.direction.k = 1.0f;
    g_decorator_sun.direction.l = 0.0f;
    g_decorator_sun_sky_index = -1;

    const s_visibility_region *region = (const s_visibility_region *)g_visibility_globals.collection_input;
    long last_bsp_index = -1, last_cluster_index = -1;
    for (short visible_index = 0; visible_index < region->cluster_count; visible_index++) {
        const visibility_cluster *visible = &region->clusters[visible_index];
        const long bsp_index = visible->bsp_index;
        const long cluster_index = visible->cluster_index;
        if (bsp_index == last_bsp_index && cluster_index == last_cluster_index)
            continue;
        const scenario_structure_bsp_reference *bsp_reference =
            &((const scenario_structure_bsp_reference *)global_scenario->structure_bsps.address)[bsp_index];
        const structure_bsp *bsp = (const structure_bsp *)tag_get('sbsp', bsp_reference->structure_bsp.index);
        last_bsp_index = bsp_index;
        last_cluster_index = cluster_index;
        const structure_cluster *cluster = &((const structure_cluster *)bsp->clusters.address)[cluster_index];
        if (cluster->decorator_groups.count > 0) {
            const long sky_index = scenario_zone_set_structure_bsp_cluster_visible_sky_index_get(global_scenario,
                global_zone_set_index, bsp_index, cluster_index);
            if (sky_index != -1)
                decorator_set_sky_sun(sky_index);
        }
        for (long group_index = 0; group_index < cluster->decorator_groups.count; group_index++) {
            rasterizer_set_alpha_test_enabled(1, group_index);
            const s_decorator_group *group = &((const s_decorator_group *)cluster->decorator_groups.address)[group_index];
            const s_tag_reference *set_reference = &((const s_tag_reference *)bsp->decorator_sets.address)[group->decorator_set_index];
            if (group->instance_count == 0 || set_reference->index == -1)
                continue;
            const s_decorator_set_definition *set = (const s_decorator_set_definition *)tag_get('dctr', set_reference->index);
            if (set->render_model.index == -1)
                continue;
            const float distance_scale = (scale_setting / render_player_view_compute_pixel_scale()) * draw_distance_scale;
            const float fade_end = set->lod_fade_end * distance_scale;
            const float fade_start = set->lod_fade_start * distance_scale;
            const float radius = group->instance_constants[0].l;
            const float dx = group->bounding_center.x - camera.x;
            const float dy = group->bounding_center.y - camera.y;
            const float dz = group->bounding_center.z - camera.z;
            if (sqrtf(dz * dz + dy * dy + dx * dx) > (1.0f - set->early_cull) * (radius + fade_end))
                continue;
            bool inside;
            if (!visibility_region_test_sphere(region, visible_index, &group->bounding_center, radius, &inside))
                continue;
            c_rasterizer::set_explicit_shaders(k_decorator_explicit_shaders[set->render_shader], _vertex_type_decorator,
                (e_transfer_vector_vertex_types)0, (e_entry_point)0);

            real_vector4d fade;
            fade.i = 0.7f / (fade_start - fade_end);
            fade.j = 1.0f - fade.i * fade_start;
            fade.k = 0.0f;
            fade.l = 0.0f;
            c_rasterizer::set_vertex_shader_constant(245, 1, &fade);
            c_rasterizer::set_vertex_shader_constant(246, 1, &set->vertex_constants_246);
            c_rasterizer::set_vertex_shader_constant(249, 1, &set->vertex_constants_249);
            c_rasterizer::set_pixel_shader_constant(13, 1, &set->pixel_constants_13);
            D3DDevice_SetRenderState_CullMode(g_rasterizer_device, (set->render_flags & 1) ? D3DCULL_NONE : D3DCULL_CW);

            const render_model_definition *model = (const render_model_definition *)tag_get('mode', set->render_model.index);
            c_rasterizer::set_vertex_stream(render_geometry_get_vertex_buffer(&model->render_geometry, 0));
            d3d9_set_vertex_shader_bounds((struct s_d3d9_state_bounds *)model->render_geometry.compression_info.address);
            render_method_submit_set_texture(0, set->texture.index, 0);
            c_rasterizer::set_sampler_address_mode(0, (c_rasterizer::e_sampler_address_mode)0, (c_rasterizer::e_sampler_address_mode)0,
                (c_rasterizer::e_sampler_address_mode)0);
            if (g_rasterizer_sampler_filter_modes[0] != 2) {
                g_rasterizer_sampler_filter_modes[0] = 2;
                rasterizer_device_set_sampler_filter_modes(g_rasterizer_device, 0, 2, 2);
            }
            const c_rasterizer_vertex_buffer *instances =
                render_geometry_get_vertex_buffer(&bsp->decorator_instance_buffer, group->instance_buffer_index);
            // probe guard: no buffer, or a declaration outside the retail table (k_vertex_type_from_declaration has 30
            // entries; the instance buffer is declaration 20, which the old 18-entry bound skipped for every group).
            if (instances->get_d3d_buffer() == 0 ||
                (long)instances->get_declaration() >= k_decorator_declaration_count)
                continue;
            g_rasterizer_device->SetStreamSource(decorator_stream_from_declaration(instances->get_declaration()),
                instances->get_d3d_buffer(), group->instance_buffer_offset, instances->get_vertex_stride());
            c_rasterizer::set_vertex_shader_constant(240, 2, group->instance_constants);

            const s_mesh *mesh = &((const s_mesh *)model->render_geometry.meshes.address)[model->instance_starting_mesh_index];
            const s_subpart *subpart = (const s_subpart *)mesh->subparts.address;
            real_vector4d index_count;
            index_count.i = (float)subpart->index_count;
            index_count.j = 0.0f;
            index_count.k = 0.0f;
            index_count.l = 0.0f;
            c_rasterizer::set_vertex_shader_constant(242, 1, &index_count);
            const unsigned long vertex_count = (unsigned long)subpart->index_count * group->instance_count;
            D3DDevice_SetIndices(g_rasterizer_device, 0);
            D3DDevice_DrawVertices(g_rasterizer_device, g_primitive_types[5], 0, vertex_count);
        }
    }
    c_render_method_data::m_valid = false;
    render_method_submit_valid();
    rasterizer_set_alpha_test_enabled(0, 0);
    c_rasterizer::set_stencil_mode((c_rasterizer::e_stencil_mode)0);
}
