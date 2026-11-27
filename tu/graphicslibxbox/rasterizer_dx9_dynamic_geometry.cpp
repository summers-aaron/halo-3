// graphicslibxbox/rasterizer_dx9_dynamic_geometry.obj: from the records 0x16df18 0x183fa0 0x1859b8 0x18f488 0x18f648 0x18fdb8 0x18ff60 0x190048 0x190190 0x1902a0 0x1921a8 0x2a4d68 0x2a4e30 0x2a4f70 0x2a5340 0x2a5560 0x2a55e0 0x2a5658 0x44bc20 0x455ee8 0x4590a0 0x459200 0x4592c0 0x4595b0 0x459950 0x459a50 0x459b48 0x459d70 0x459e40
// @address 0x73042c g_interface_aspect_ratio_scaling
// @address 0x737664 g_rasterizer_particle_submission_state
// @address 0x7c6178 g_rasterizer_sampler_filter_modes
// @address 0x81dab0 g_rasterizer_current_z_buffer_mode
// @address 0x8c4320 g_rasterizer_tiling_alignment
// @address 0x8c4330 g_rasterizer_tiling_globals
// @address 0x948f78 main_globals
// @address 0x10eb500 g_rasterizer_player_views
// @address 0x195e88 rasterizer_device_set_sampler_filter_modes
// @flags /TP /O2 /Oc
// @split_bss
// @sparse_bss 0x828c4318..0x828c46d0

extern "C" {
// @address 0x7445e0 g_rasterizer_tiling_alpha
float g_rasterizer_tiling_alpha = 1.9500000476837158f;
// @address 0x8c46e4 g_rasterizer_distortion_visible
bool g_rasterizer_distortion_visible;
}
// match: 0x1902a0: a literal drops the lfs for the negative scale and changes the tile-normalization fadds to fsubs.
// @address 0xabc k_rasterizer_screen_x_offset_scale
// @address 0x81da20 g_rasterizer_last_rectangles

#include "rasterizer/dx9/rasterizer_dx9_dynamic_geometry.h"
#include "rasterizer/dx9/rasterizer_dx9_dynamic_geometry_globals.h"

#if !defined(_XBOX) || defined(__clang__)
const float k_rasterizer_screen_x_offset_scale = -2.0f;
#endif
#include "render/render_water_globals.h"
#include "rasterizer/rasterizer_render_target_types.h"
#include "rasterizer/rasterizer_shader_definitions.h"
#include "rasterizer/rasterizer_tiling_types.h"
#include "main/main_flag_state.h"
#include "main/game_tick_time_samples.h"
#include "bitmaps/bitmap_group_definitions.h"
#include "rasterizer/rasterizer_depth_bias_globals.h"
#include "rasterizer/rasterizer_shader_gpr_types.h"
#include "rasterizer/rasterizer_screenshot_calls.h"
#include "rasterizer/rasterizer_viewport_state.h"
#include "main/main_screenshot.h"
#include "game/game_globals.h"
#include "rasterizer/rasterizer_main_globals.h"
#include <math.h>
#include "cseries/cseries.h"
#include "tag_files/tag_groups.h"
#include "math/real_math_types.h"
#include "tag_files/tag_types.h"
#include <xtl.h>
#include "rasterizer/rasterizer_resource_definitions.h"
#include <xgraphics.h>
#include "cseries/crt_xdk.h"
#include <ppcintrinsics.h>
#include "effects/player_effects.h"
#include "interface/interface_constants.h"
#include "interface/chud/chud_draw.h"
#include "rasterizer/rasterizer_render_targets.h"
#include "render/screen_postprocess_xenon.h"
#include "render/screen_postprocess_xenon_globals.h"
#include "render/screen_postprocess.h"
#include "cache/cache_files.h"
#include "scenario/scenario.h"
#include "rasterizer/rasterizer_render_globals_types.h"
#include "rasterizer/rasterizer_vertex_screen_types.h"
#include "bitmaps/bitmap_group.h"
#include "rasterizer/rasterizer_states.h"
#include "bitmaps/bitmap_data.h"
#include "physics/rasterizer_vertex_transparent_types.h"
#include "rasterizer/rasterizer_vertex_definitions.h"
#include "render_methods/render_method_definitions.h"
#include "rasterizer/rasterizer.h"
#include "rasterizer/rasterizer_hdr_targets.h"
#include "rasterizer/rasterizer_global_definitions.h"
#include "rasterizer/rasterizer_textures_cache.h"
#include "render/views/player_view_types.h"
#include "rasterizer/dx9/rasterizer_screenshot_types.h"
#include "rasterizer/dx9/rasterizer_dynamic_screen_geometry_types.h"
#include "rasterizer/dx9/rasterizer_dynamic_screen_calls.h"
#include "rasterizer/rasterizer_quad.h"
#include "render/water_renderer.h"
#if defined(_XBOX) && !defined(__clang__)
#include "gpu_particle/contrail_layout_types.h"
#else
#include "saved_games/game_state_procs_externs.h"
#endif

struct rasterizer_vertex_debug
{
    real_point3d position;
    unsigned long color;
};

struct s_motion_blur_shader_constants
{
    real_vector4d blur_scale;
    real_vector4d blur_parameters;
    real_vector4d screen_center;
    int taps[4];
};

struct D3DDevice;
struct D3DIndexBuffer;
struct D3DSurface;
struct D3DVertexDeclaration;
struct rasterizer_vertex_world;
struct c_rasterizer_vertex_shader;
struct c_rasterizer_pixel_shader;

// @address 0x16df18 ?draw_fullscreen_quad@c_rasterizer@@SAXHH@Z
void c_rasterizer::draw_fullscreen_quad(int width, int height)
{
    float x_offset = (float)(-1.0 / width);
    float y_offset = (float)(1.0 / height);

    rasterizer_vertex_screen vertices[3];
    vertices[0].x = -1.0f + x_offset;
    vertices[0].y = 1.0f + y_offset;
    vertices[0].u = 0.0f;
    vertices[0].v = 0.0f;
    vertices[0].color = 0xffffffff;
    vertices[1].x = 1.0f + x_offset;
    vertices[1].y = 1.0f + y_offset;
    vertices[1].u = 1.0f;
    vertices[1].v = 0.0f;
    vertices[1].color = 0xffffffff;
    vertices[2].x = -1.0f + x_offset;
    vertices[2].y = -1.0f + y_offset;
    vertices[2].u = 0.0f;
    vertices[2].v = 1.0f;
    vertices[2].color = 0xffffffff;

    D3DDevice_SetRenderState_CullMode(g_device, 0);
    D3DDevice_SetIndices(g_device, 0);
    D3DDevice_DrawVerticesUP(g_device, (D3DPRIMITIVETYPE)8, 3, vertices, 20);
    D3DDevice_SetRenderState_CullMode(g_device, 2);
}

// @address 0x183fa0 ?draw_worldspace_polygon@c_rasterizer@@SAXPBUrasterizer_vertex_world@@J@Z
void c_rasterizer::draw_worldspace_polygon(const rasterizer_vertex_world *vertices, long vertex_count)
{
    // match: 0x183fa0: value range assumption guides branch lowering
    __assume(vertex_count == 4);
    D3DDevice_SetRenderState_CullMode(g_device, 0);
    D3DDevice_SetIndices(g_device, 0);
    D3DDevice_DrawVerticesUP(g_device, (D3DPRIMITIVETYPE)5, vertex_count, vertices, 32);
    D3DDevice_SetRenderState_CullMode(g_device, 2);
}

// @address 0x1859b8 ?draw_fullscreen_quad_with_texture_xform@c_rasterizer@@SAXHHPBTreal_rectangle2d@@@Z
void c_rasterizer::draw_fullscreen_quad_with_texture_xform(int width, int height, const real_rectangle2d *rectangle)
{
    float x_offset = (float)(-1.0 / width);
    float y_offset = (float)(1.0 / height);

    rasterizer_vertex_screen vertices[3];
    vertices[0].x = -1.0f + x_offset;
    vertices[0].y = 1.0f + y_offset;
    vertices[0].u = rectangle->x0;
    vertices[0].v = rectangle->y0;
    vertices[0].color = 0xffffffff;
    vertices[1].x = 1.0f + x_offset;
    vertices[1].y = 1.0f + y_offset;
    vertices[1].u = rectangle->x1;
    vertices[1].v = rectangle->y0;
    vertices[1].color = 0xffffffff;
    vertices[2].x = -1.0f + x_offset;
    vertices[2].y = -1.0f + y_offset;
    vertices[2].u = rectangle->x0;
    vertices[2].v = rectangle->y1;
    vertices[2].color = 0xffffffff;

    D3DDevice_SetRenderState_CullMode(g_device, 0);
    D3DDevice_SetIndices(g_device, 0);
    D3DDevice_DrawVerticesUP(g_device, (D3DPRIMITIVETYPE)8, 3, vertices, 20);
    D3DDevice_SetRenderState_CullMode(g_device, 2);
}

// @address 0x18fdb8 ?draw_screen_quad_with_texture_transform@c_rasterizer@@SAXHHPBTreal_rectangle2d@@0@Z
void c_rasterizer::draw_screen_quad_with_texture_transform(int width, int height, const real_rectangle2d *rectangle, const real_rectangle2d *rectangle2)
{
    float x_offset = (float)(-1.0 / width);
    float y_offset = (float)(1.0 / height);

    rasterizer_vertex_screen vertices[3];
    vertices[0].x = rectangle->x0 * 2.0f - 1.0f + x_offset;
    vertices[0].y = y_offset - rectangle->y0 * 2.0f + 1.0f;
    vertices[0].u = rectangle2->x0;
    vertices[0].v = rectangle2->y0;
    vertices[0].color = 0xffffffff;
    vertices[1].x = rectangle->x1 * 2.0f - 1.0f + x_offset;
    vertices[1].y = y_offset - rectangle->y0 * 2.0f + 1.0f;
    vertices[1].u = rectangle2->x1;
    vertices[1].v = rectangle2->y0;
    vertices[1].color = 0xffffffff;
    vertices[2].x = rectangle->x0 * 2.0f - 1.0f + x_offset;
    vertices[2].y = y_offset - rectangle->y1 * 2.0f + 1.0f;
    vertices[2].u = rectangle2->x0;
    vertices[2].v = rectangle2->y1;
    vertices[2].color = 0xffffffff;

    D3DDevice_SetRenderState_CullMode(g_device, 0);
    D3DDevice_SetIndices(g_device, 0);
    D3DDevice_DrawVerticesUP(g_device, (D3DPRIMITIVETYPE)8, 3, vertices, 20);
    D3DDevice_SetRenderState_CullMode(g_device, 2);
}

typedef char rasterizer_player_view_index_offset[(sizeof(void *) != 4 || ((offsetof(c_player_view, view_index) == 0x29c0))) ? 1 : -1];

// The hardware-format slot packs with the wrapper frame: inlining the bitmap local
// into rasterizer_quad_screenspace grows its frame 0x70 -> 0x80, so the wrapper stays.
static __forceinline bool rasterizer_dx9_dynamic_geometry_get_bitmap_hardware_format(
    long bitmap_group_index, long bitmap_index, c_rasterizer_texture_ref *texture_ref)
{
    const bitmap_data *bitmap;
    return ::bitmap_group_get_xenon_hardware_format(bitmap_group_index, bitmap_index, texture_ref, &bitmap, 0);
}

// @address 0x18ff60 ?rasterizer_quad_screenspace@@YAXAAY03$$CBTpoint2d@@KPBUs_tag_reference@@F_N@Z
void rasterizer_quad_screenspace(const point2d (&points)[4], unsigned long color, const s_tag_reference *, short, bool)
{
    const s_tag_reference *bitmap_reference = NULL;
    short bitmap_index = 0;
    c_rasterizer_texture_ref texture_ref;
    if (!bitmap_reference)
    {
        s_game_globals *globals = ((s_game_globals *)global_game_globals);
        if (globals)
        {
            c_rasterizer_globals *rasterizer_globals = globals->rasterizer_globals_ref.index != NONE
                ? (c_rasterizer_globals *)tag_get('rasg', globals->rasterizer_globals_ref.index)
                : NULL;
            bitmap_reference = rasterizer_globals->get_default_texture_ref(0);
        }
    }
    if (bitmap_reference)
    {
        bitmap_group *bitmap_group = (struct bitmap_group *)tag_get('bitm', bitmap_reference->index);
        long index = MIN(MAX(bitmap_index, 0), bitmap_group->bitmaps.count - 1);
        if (rasterizer_dx9_dynamic_geometry_get_bitmap_hardware_format(bitmap_reference->index, index, &texture_ref))
        {
            rasterizer_quad_screenspace_explicit(points, color, texture_ref, false, (c_rasterizer::e_alpha_blend_mode)3);
        }
    }
}

// @address 0x190048 ?rasterizer_quad_screenspace_explicit@@YAXAAY03$$CBTpoint2d@@KVc_rasterizer_texture_ref@@_NW4e_alpha_blend_mode@c_rasterizer@@@Z
void rasterizer_quad_screenspace_explicit(const point2d (&points)[4], unsigned long color, c_rasterizer_texture_ref texture_ref,
    bool, c_rasterizer::e_alpha_blend_mode alpha_blend_mode)
{
    rasterizer_dynamic_screen_geometry_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));
    float texture_coordinates[4][2] = { { 0.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f } };
    rasterizer_vertex_screen vertices[4];
    for (unsigned long i = 0; i < 4; i++)
    {
        vertices[i].u = texture_coordinates[i][0];
        vertices[i].v = texture_coordinates[i][1];
        vertices[i].x = (float)points[i].x;
        vertices[i].y = (float)points[i].y;
        vertices[i].color = color;
    }
    calculate_aspect_ratio_scaling();
    real_vector2d scale = g_interface_aspect_ratio_scaling;
    rasterizer_scale_screen_vertices(vertices, 4, &scale);
    parameters.scale1.i = parameters.scale1.j = 1.0f;
    parameters.scale0.i = parameters.scale0.j = 1.0f;
    parameters.textures[0] = texture_ref;
    parameters.explicit_shader_index = NONE;
    parameters.alpha_blend_mode = (short)alpha_blend_mode;
    parameters.point_sampled = false;
    rasterizer_draw_dynamic_screen_quad(&parameters, vertices);
}

extern "C" {

// match: 0x1921a8: retain one sparse static BSS layout for the shared tiling and GPR base.
static __declspec(align(16)) unsigned char g_rasterizer_tiling_alignment[0x10];
static __declspec(align(16)) volatile s_rasterizer_tiling_state g_rasterizer_tiling_globals;
const void *rasterizer_dx9_dynamic_geometry_bss_padding_anchor = g_rasterizer_tiling_alignment;

}

// @address 0x2a5560 rasterizer_tiling_initialize
extern "C" unsigned char rasterizer_tiling_initialize(short width, short height)
{
    volatile s_rasterizer_tiling_state *tiling = (s_rasterizer_tiling_state *)&g_rasterizer_tiling_globals;
    tiling->disabled = false;
    tiling->width = width;
    tiling->height = height;
    tiling->pass_count = 1;
    tiling->spare_after_enabled = false;
    tiling->include_ui = true;
    tiling->end_callback = 0;
    tiling->pad_to_tile_grid = false;
    tiling->fence_destination = 0;
    tiling->render_bloom = c_screen_postprocess::x_editable_settings.bloom_mode != 0;
    tiling->render_flares = tiling->render_bloom;
    tiling->skip_postprocess = false;
    tiling->tiling_initialized = true;
    tiling->composite_tiles = true;
    tiling->use_half_size_tiles = false;
    tiling->reserved_tiling_flag = false;
    tiling->screenshot_delay = 0;
    return 1;
}

// @address 0x2a55e0 ?rasterizer_tiling_get_scale@@YAXPAM0@Z
void rasterizer_tiling_get_scale(float *x_scale, float *y_scale)
{
    const volatile s_rasterizer_tiling_state *tiling = (const s_rasterizer_tiling_state *)&g_rasterizer_tiling_globals;
    *x_scale = 1.0f;
    *y_scale = 1.0f;
    if (tiling->enabled)
    {
        *x_scale = (float)(tiling->width / c_rasterizer::render_globals.width);
        *y_scale = (float)(tiling->height / c_rasterizer::render_globals.height);
    }
}

void c_rasterizer::draw_fullscreen_quad()
{
    rasterizer_vertex_screen vertices[3];
    vertices[0].x = -1.0f; vertices[0].y = 1.0f; vertices[0].u = 0.0f; vertices[0].v = 0.0f; vertices[0].color = 0xffffffff;
    vertices[1].x = 1.0f; vertices[1].y = 1.0f; vertices[1].u = 1.0f; vertices[1].v = 0.0f; vertices[1].color = 0xffffffff;
    vertices[2].x = -1.0f; vertices[2].y = -1.0f; vertices[2].u = 0.0f; vertices[2].v = 1.0f; vertices[2].color = 0xffffffff;
    D3DDevice_SetRenderState_CullMode(g_device, 0);
    D3DDevice_SetIndices(g_device, 0);
    D3DDevice_DrawVerticesUP(g_device, (D3DPRIMITIVETYPE)8, 3, vertices, 20);
    D3DDevice_SetRenderState_CullMode(g_device, 2);
}

static __forceinline void rasterizer_dx9_dynamic_geometry_set_z_buffer_off()
{
    float scale = 1.0f;
    union { float as_float; unsigned long as_dword; } slope_scale_bias = { g_rasterizer_depth_bias };
    union { float as_float; unsigned long as_dword; } depth_bias = { g_rasterizer_depth_bias };

    if (g_rasterizer_use_floating_point_z_buffer)
        scale = -1.0f;

    if (g_rasterizer_current_z_buffer_mode != 2 || g_rasterizer_use_floating_point_z_buffer != g_rasterizer_current_z_buffer_floating_point)
    {
        g_rasterizer_current_z_buffer_mode = 2;
        g_rasterizer_current_z_buffer_floating_point = g_rasterizer_use_floating_point_z_buffer;
        D3DDevice_SetRenderState_ZEnable(c_rasterizer::get_device(), 0);
        slope_scale_bias.as_float *= scale;
        depth_bias.as_float *= scale;
        D3DDevice_SetRenderState_SlopeScaleDepthBias(c_rasterizer::get_device(), slope_scale_bias.as_dword);
        D3DDevice_SetRenderState_DepthBias(c_rasterizer::get_device(), depth_bias.as_dword);
    }
}

// @address 0x459950 ?draw_textured_screen_triangle_list@c_rasterizer@@SAXPBUrasterizer_vertex_screen@@J@Z
void c_rasterizer::draw_textured_screen_triangle_list(const rasterizer_vertex_screen *vertices, long triangle_count)
{
    D3DDevice_SetRenderState_CullMode(g_device, 0);
    rasterizer_dx9_dynamic_geometry_set_z_buffer_off();
    D3DDevice_SetIndices(g_device, 0);
    D3DDevice_DrawVerticesUP(g_device, (D3DPRIMITIVETYPE)4, triangle_count * 3, vertices, 20);
    D3DDevice_SetRenderState_CullMode(g_device, 2);
}

// @address 0x459a50 ?draw_textured_transparent_polygon@c_rasterizer@@SA_NJ@Z
bool c_rasterizer::draw_textured_transparent_polygon(long explicit_shader)
{
    bool result = false;
    s_game_globals *globals = ((s_game_globals *)global_game_globals);
    if (globals)
    {
        c_rasterizer_globals *rasterizer_globals = globals->rasterizer_globals_ref.index != NONE
            ? (c_rasterizer_globals *)tag_get('rasg', globals->rasterizer_globals_ref.index)
            : NULL;
        const c_rasterizer_globals::s_explicit_shader *shader = rasterizer_globals->get_explicit_shaders(1);
        const s_tag_reference *vertex_shader_ref = &shader->vertex;
        const s_tag_reference *pixel_shader_ref = &shader->pixel;

        if (pixel_shader_ref->index != NONE && vertex_shader_ref->index != NONE)
        {
            D3DDevice_SetRenderState_CullMode(g_device, 0);
            D3DDevice_SetVertexDeclaration(g_device, rasterizer_get_vertex_declaration(_vertex_type_debug, _transfer_vector_vertex_none, _entry_point_default));
            c_rasterizer::set_vertex_shader(c_rasterizer_vertex_shader::get(vertex_shader_ref->index), _vertex_type_debug,
                _transfer_vector_vertex_none, _entry_point_default);
            c_rasterizer::set_pixel_shader(c_rasterizer_pixel_shader::get(pixel_shader_ref->index), _entry_point_default);
            result = true;
            D3DDevice_SetRenderState_CullMode(g_device, 2);
        }
    }
    return result;
}

#if defined(_XBOX) && !defined(__clang__)
extern "C" {
// @address 0x81da28 g_rasterizer_viewport_bounds
rectangle2d g_rasterizer_viewport_bounds;
}
#endif

// @address 0x459b48 ?draw_debug_line2d@c_rasterizer@@SAXABTreal_point3d@@0KK@Z
void c_rasterizer::draw_debug_line2d(const real_point3d &point0, const real_point3d &point1, unsigned long color0, unsigned long color1)
{
    rasterizer_vertex_debug vertices[2];
    const real_point3d *points[2] = { &point0, &point1 };
    unsigned long colors[2] = { color0, color1 };
    for (long vertex_index = 0; vertex_index < 2; vertex_index++)
    {
        rasterizer_vertex_debug *vertex = &vertices[vertex_index];
        vertex->position.x = points[vertex_index]->x;
        vertex->position.y = points[vertex_index]->y;
        vertex->position.z = 0.0f;
        vertex->color = colors[vertex_index];
    }

    const rectangle2d *bounds = &g_rasterizer_viewport_bounds;
    float width = (float)(bounds->x1 - bounds->x0);
    float height = (float)(bounds->y1 - bounds->y0);
    vertices[0].position.x = vertices[0].position.x / width * 2.0f - 1.0f - (float)bounds->x0 / width * 2.0f;
    vertices[0].position.y = (float)bounds->y0 / height * 2.0f - (vertices[0].position.y / height * 2.0f - 1.0f);
    vertices[1].position.x = vertices[1].position.x / width * 2.0f - 1.0f - (float)bounds->x0 / width * 2.0f;
    vertices[1].position.y = (float)bounds->y0 / height * 2.0f - (vertices[1].position.y / height * 2.0f - 1.0f);

    if (draw_textured_transparent_polygon(1))
    {
        D3DDevice_SetRenderState_CullMode(g_device, 0);
        rasterizer_dx9_dynamic_geometry_set_z_buffer_off();
        D3DDevice_SetIndices(g_device, 0);
        D3DDevice_DrawVerticesUP(g_device, (D3DPRIMITIVETYPE)2, 2, vertices, 16);
        D3DDevice_SetRenderState_CullMode(g_device, 2);
    }
}

// @address 0x459d70 ?draw_debug_linestrip2d@c_rasterizer@@SAXPBTpoint2d@@JK@Z
void c_rasterizer::draw_debug_linestrip2d(const point2d *points, long, unsigned long color)
{
    long segment_index = 3;
    do
    {
        real_point3d line[2];
        line[0].x = (float)points->x;
        line[0].y = (float)points->y;
        line[0].z = 0.0f;
        points++;
        line[1].x = (float)points->x;
        line[1].y = (float)points->y;
        line[1].z = 0.0f;
        draw_debug_line2d(line[0], line[1], color, color);
    } while (--segment_index >= 0);
}

// @address 0x459e40 ?draw_textured_transparent_quad@c_rasterizer@@SAXPBUrasterizer_vertex_transparent@@W4e_alpha_blend_mode@1@@Z
void c_rasterizer::draw_textured_transparent_quad(const rasterizer_vertex_transparent *vertices, e_alpha_blend_mode alpha_blend_mode)
{
    s_game_globals *globals = ((s_game_globals *)global_game_globals);
    c_rasterizer_globals *rasterizer_globals = globals->rasterizer_globals_ref.index != NONE
        ? (c_rasterizer_globals *)tag_get('rasg', globals->rasterizer_globals_ref.index)
        : NULL;
    const s_tag_reference *vertex_shader_ref = &rasterizer_globals->get_explicit_shaders(20)->vertex;
    const s_tag_reference *pixel_shader_ref = &rasterizer_globals->get_explicit_shaders(20)->pixel;

    if (pixel_shader_ref->index != NONE && vertex_shader_ref->index != NONE)
    {
                if (g_current_alpha_blend_mode != _alpha_blend_alpha_blend)
        {
            g_current_alpha_blend_mode = _alpha_blend_alpha_blend;
            rasterizer_set_device_alpha_blend_mode(g_device, _alpha_blend_alpha_blend);
        }
        c_rasterizer::set_z_buffer_mode(_z_buffer_mode_read_only);
        D3DDevice_SetRenderState_CullMode(g_device, 0);
        D3DDevice_SetVertexDeclaration(
            g_device, rasterizer_get_vertex_declaration(_vertex_type_transparent, _transfer_vector_vertex_none, _entry_point_default));
        c_rasterizer::set_vertex_shader(c_rasterizer_vertex_shader::get(vertex_shader_ref->index), _vertex_type_transparent,
            _transfer_vector_vertex_none, _entry_point_default);
        c_rasterizer::set_pixel_shader(c_rasterizer_pixel_shader::get(pixel_shader_ref->index), _entry_point_default);
        D3DDevice_SetIndices(g_device, 0);
        D3DDevice_DrawVerticesUP(g_device, (D3DPRIMITIVETYPE)6, 4, vertices, 24);
        D3DDevice_SetRenderState_CullMode(g_device, 2);
    }
}

// @address 0x18f648 ?draw_textured_screen_quad@c_rasterizer@@SAXPBUrasterizer_vertex_screen@@_N@Z
void c_rasterizer::draw_textured_screen_quad(const rasterizer_vertex_screen *vertices, bool unused)
{
    D3DDevice_SetRenderState_CullMode(g_device, 0);
    rasterizer_dx9_dynamic_geometry_set_z_buffer_off();
    D3DDevice_SetVertexDeclaration(g_device, rasterizer_get_vertex_declaration(_vertex_type_screen, _transfer_vector_vertex_none, _entry_point_default));
    D3DDevice_SetIndices(g_device, 0);
    D3DDevice_DrawVerticesUP(g_device, (D3DPRIMITIVETYPE)5, 4, vertices, 20);
    c_rasterizer::set_z_buffer_mode(_z_buffer_mode_read_only);
    D3DDevice_SetRenderState_CullMode(g_device, 2);
}

// @address 0x4590a0 rasterizer_draw_screen_grid
// Fill a 10x10 grid of textured screen-space triangles.
extern "C" void rasterizer_draw_screen_grid()
{
    rasterizer_vertex_screen vertices[1500];
    for (long y = 0; y < 10; y++)
    {
        float texture_top = (float)y * 0.1f;
        float texture_bottom = (float)(y + 1) * 0.1f;
        for (long x = 0; x < 10; x++)
        {
            float u0 = (float)x * 0.1f;
            float u1 = (float)(x + 1) * 0.1f;
            rasterizer_vertex_screen *v = &vertices[(y * 10 + x) * 3];
            v[0].u = u0;
            v[0].v = texture_top;
            v[1].u = u1;
            v[1].v = texture_top;
            v[2].u = u0;
            v[2].v = texture_bottom;
            for (long i = 0; i < 3; i++)
            {
                v[i].color = 0xffffffff;
                v[i].x = v[i].u * 2.0f - 1.0f;
                v[i].y = 1.0f - v[i].v * 2.0f;
            }
        }
    }
    D3DDevice_SetRenderState_CullMode(c_rasterizer::get_device(), 0);
    D3DDevice_SetIndices(c_rasterizer::get_device(), 0);
    D3DDevice_DrawVerticesUP(c_rasterizer::get_device(), (D3DPRIMITIVETYPE)8, 300, vertices, 20);
    D3DDevice_SetRenderState_CullMode(c_rasterizer::get_device(), 2);
}

// @address 0x190190 rasterizer_scale_screen_vertices
extern "C" void rasterizer_scale_screen_vertices(rasterizer_vertex_screen *vertices, long vertex_count, const real_vector2d *scale)
{
    while (--vertex_count >= 0)
    {
        vertices[vertex_count].x *= scale->i;
        vertices[vertex_count].y *= scale->j;
    }
}

// @address 0x44bc20 rasterizer_render_water
extern "C" void rasterizer_render_water(bool enabled)
{
    if (g_water_refraction_enabled)
    {
        rasterizer_dx9_dynamic_geometry_set_z_buffer_off();
        D3DDevice_SetRenderState_CullMode(c_rasterizer::get_device(), 0);
        D3DDevice_SetRenderState_ColorWriteEnable(c_rasterizer::get_device(), 15);
        int constant = enabled;
        D3DDevice_SetVertexShaderConstantB(c_rasterizer::get_device(), 104, &constant, 1);
        c_water_renderer::render_cluster_parts(_entry_point_water_tessellation, 3);
    }
}

extern "C" {
// @address 0x119a4e8 g_motion_blur_cache_valid
bool g_motion_blur_cache_valid;
// @address 0x7372fc g_motion_blur_taps
long g_motion_blur_taps = 5;
// @address 0x7374e0 g_motion_blur_expected_dt
float g_motion_blur_expected_dt = 0.033333335071802139f;
// @address 0x7374e4 g_motion_blur_max_x
float g_motion_blur_max_x = 0.013500001281499862f;
// @address 0x7374e8 g_motion_blur_max_y
float g_motion_blur_max_y = 0.024000000208616256f;
// @address 0x7374ec g_motion_blur_scale_x
float g_motion_blur_scale_x = 0.045000001788139343f;
// @address 0x7374f0 g_motion_blur_scale_y
float g_motion_blur_scale_y = 0.079999998211860656f;
// @address 0x7374f4 g_motion_blur_center_falloff
float g_motion_blur_center_falloff = 2.0f;
}

// @address 0x4592c0 rasterizer_set_motion_blur_constants
extern "C" void rasterizer_set_motion_blur_constants()
{
    s_motion_blur_shader_constants constants;
    constants.taps[0] = 0;
    constants.taps[1] = 0;
    constants.taps[2] = 1;
    constants.taps[3] = 0;
    if (!g_motion_blur_cache_valid)
    {
        s_game_globals *globals = ((s_game_globals *)global_game_globals);
        c_rasterizer_globals *rasterizer_globals = globals->rasterizer_globals_ref.index != NONE
            ? (c_rasterizer_globals *)tag_get('rasg', globals->rasterizer_globals_ref.index)
            : NULL;
        long *m_motion_blur_taps = &g_motion_blur_taps;
        *m_motion_blur_taps = rasterizer_globals->m_motion_blur_taps;
        float *m_motion_blur_expected_dt = &g_motion_blur_expected_dt;
        *m_motion_blur_expected_dt = rasterizer_globals->m_motion_blur_expected_dt;
        float *m_motion_blur_center_falloff = &g_motion_blur_center_falloff;
        *m_motion_blur_center_falloff = rasterizer_globals->m_motion_blur_center_falloff;
        float *m_motion_blur_max_x = &g_motion_blur_max_x;
        *m_motion_blur_max_x = rasterizer_globals->m_motion_blur_max_x;
        float *m_motion_blur_max_y = &g_motion_blur_max_y;
        *m_motion_blur_max_y = rasterizer_globals->m_motion_blur_max_y;
        bool *cache_valid = &g_motion_blur_cache_valid;
        *cache_valid = true;
        float *m_motion_blur_scale_x = &g_motion_blur_scale_x;
        *m_motion_blur_scale_x = rasterizer_globals->m_motion_blur_scale_x;
        float *m_motion_blur_scale_y = &g_motion_blur_scale_y;
        *m_motion_blur_scale_y = rasterizer_globals->m_motion_blur_scale_y;
    }
    if (g_motion_blur_taps <= 2)
        g_motion_blur_taps = 2;
    else if (g_motion_blur_taps >= 20)
        g_motion_blur_taps = 20;
    constants.taps[0] = g_motion_blur_taps;

    constants.blur_parameters.i = (float)(unsigned long)constants.taps[0];
    constants.blur_parameters.j = g_main_render_timing_data->maximum;
    constants.blur_parameters.k = g_motion_blur_expected_dt;
    constants.blur_parameters.l = g_motion_blur_center_falloff;
    constants.blur_scale.i = g_motion_blur_max_x;
    constants.blur_scale.j = g_motion_blur_max_y;
    constants.blur_scale.k = g_motion_blur_scale_x;
    constants.blur_scale.l = -g_motion_blur_scale_y;
    {
        real_rgb_color multiplier;
        player_effect_get_screen_tint_multiplier(c_player_view::get_current()->view_index, &multiplier);
        constants.blur_scale.i *= multiplier.red;
        constants.blur_scale.j *= multiplier.red;
        constants.blur_scale.k *= multiplier.green;
        constants.blur_scale.l *= multiplier.green;
        constants.blur_parameters.l *= multiplier.blue;
    }
    constants.blur_parameters.j = constants.blur_parameters.j > 0.0001f ? constants.blur_parameters.k / constants.blur_parameters.j : 1.0f;
    constants.blur_scale.k *= constants.blur_parameters.j;
    constants.blur_scale.l *= constants.blur_parameters.j;
    {
        real_point2d screen_point;
        chud_draw_get_normalized_screen_point(c_player_view::get_current()->view_index, &screen_point);
        constants.screen_center.i = screen_point.x * 2.0f - 1.0f;
        constants.screen_center.j = 1.0f - screen_point.y * 2.0f;
        constants.screen_center.l = 0.0f;
        constants.screen_center.k = 0.0f;
    }
    c_rasterizer::get_device()->SetPixelShaderConstantI(2, constants.taps, 1);
    c_rasterizer::get_device()->SetPixelShaderConstantF(172, &constants.blur_parameters.i, 1);
    c_rasterizer::get_device()->SetPixelShaderConstantF(173, &constants.blur_scale.i, 1);
    c_rasterizer::get_device()->SetPixelShaderConstantF(174, &constants.screen_center.i, 1);
    BOOL enabled = g_rasterizer_distortion_visible;
    c_rasterizer::get_device()->SetPixelShaderConstantB(2, &enabled, 1);
}

static __forceinline c_render_surface *rasterizer_dx9_dynamic_geometry_get_render_surface(long surface)
{
    return g_rasterizer_surface_groups[surface].get_specialized_surface((c_rasterizer::e_surface)surface);
}
static __forceinline long get_render_surface_width_in_pixels(long surface)
{
    c_render_surface *render_surface = rasterizer_dx9_dynamic_geometry_get_render_surface(surface);
    return render_surface->description.width;
}
static __forceinline long get_render_surface_height_in_pixels(long surface)
{
    c_render_surface *render_surface = rasterizer_dx9_dynamic_geometry_get_render_surface(surface);
    return render_surface->description.height;
}
static __forceinline void rasterizer_dx9_dynamic_geometry_set_pixel_shader_constants(long start, long count, const real_vector4d *values)
{
    c_rasterizer::get_device()->SetPixelShaderConstantF(start, &values->i, count);
}

// @address 0x455ee8 rasterizer_draw_fullscreen_pass
extern "C" void rasterizer_draw_fullscreen_pass(long unused, long shader)
{
    if (c_rasterizer::set_explicit_shaders(shader, _vertex_type_screen, _transfer_vector_vertex_none, _entry_point_default))
    {
        c_rasterizer::set_aliased_surface_as_texture(0, (c_rasterizer::e_surface)1);
        c_rasterizer::set_sampler_address_mode(0, (c_rasterizer::e_sampler_address_mode)(1), (c_rasterizer::e_sampler_address_mode)(1),
            (c_rasterizer::e_sampler_address_mode)(1));
        if (g_rasterizer_sampler_filter_modes[0] != 1)
        {
            g_rasterizer_sampler_filter_modes[0] = 1;
            rasterizer_device_set_sampler_filter_modes(c_rasterizer::get_device(), 0, 1, 1);
        }
        rasterizer_set_back_buffer_render_target(0, 1);
        real_vector4d texel;
        texel.i = (float)(1.0 / get_render_surface_width_in_pixels(1));
        texel.j = (float)(1.0 / get_render_surface_height_in_pixels(1));
        texel.k = texel.l = 0.0f;
        rasterizer_dx9_dynamic_geometry_set_pixel_shader_constants(1, 1, &texel);
        real_vector4d color;
        color.j = 1.0f;
        color.k = 1.0f;
        color.l = 1.0f;
        color.i = 1.0f;
        rasterizer_dx9_dynamic_geometry_set_pixel_shader_constants(2, 1, &color);
        c_rasterizer::draw_fullscreen_quad(get_render_surface_width_in_pixels(1), get_render_surface_height_in_pixels(1));
        c_rasterizer::set_sampler_texture(0, c_rasterizer_texture_ref());
    }
}

// @address 0x4b7a8 k_screen_bitmap_quad_vertices
// @rdata 0x8204b7a8..0x8204b7f8
extern "C" {
const rasterizer_vertex_screen k_screen_bitmap_quad_vertices[4] = {
    { -1.0f, -1.0f, 0.0f, 0.0f, 0xffffffff },
    { -1.0f, 1.0f, 1.0f, 0.0f, 0xffffffff },
    { 1.0f, 1.0f, 1.0f, 1.0f, 0xffffffff },
    { 1.0f, -1.0f, 0.0f, 1.0f, 0xffffffff }
};
}
// @rdata-end

// @address 0x18f488 rasterizer_draw_screen_bitmap
extern "C" void rasterizer_draw_screen_bitmap(
    const rasterizer_vertex_screen *vertices, long bitmap_group_index, long bitmap_index, float amount, float progress, float offset)
{
    vertices = k_screen_bitmap_quad_vertices;
    if (bitmap_group_index != -1)
    {
        c_rasterizer_texture_ref texture;
        real_vector4d tint;
        real_vector4d adjustment;
        const bitmap_data *bitmap;
        unsigned char (*getter)(long, long, c_rasterizer_texture_ref *, const bitmap_data **, __int64 *) = &bitmap_group_get_xenon_hardware_format;
        getter(bitmap_group_index, bitmap_index, &texture, &bitmap, NULL);
        c_rasterizer::set_sampler_texture(0, texture);
        c_rasterizer::set_sampler_address_mode(0, (c_rasterizer::e_sampler_address_mode)(1), (c_rasterizer::e_sampler_address_mode)(1),
            (c_rasterizer::e_sampler_address_mode)(1));
        if (g_rasterizer_sampler_filter_modes[0] != 2)
        {
            g_rasterizer_sampler_filter_modes[0] = 2;
            rasterizer_device_set_sampler_filter_modes(c_rasterizer::get_device(), 0, 2, 2);
        }
        tint.i = tint.j = tint.k = 1.0f;
        tint.l = c_rasterizer::get_render_target_alpha_multiplier(0);
        c_rasterizer::get_device()->SetPixelShaderConstantF(2, &tint.i, 1);
        adjustment.i = 1.0f - progress;
        adjustment.l = 1.0f - progress;
        adjustment.j = offset;
        adjustment.k = amount;
        c_rasterizer::get_device()->SetPixelShaderConstantF(50, &adjustment.i, 1);
        if (c_rasterizer::get_current_alpha_blend_mode() != c_rasterizer::_alpha_blend_additive)
        {
            D3DDevice *device = c_rasterizer::get_device();
            c_rasterizer::set_current_alpha_blend_mode(c_rasterizer::_alpha_blend_additive);
            D3DDevice_SetRenderState_AlphaBlendEnable(device, 1);
            D3DDevice_SetRenderState_BlendOp(device, 0);
            D3DDevice_SetRenderState_SrcBlend(device, 1);
            D3DDevice_SetRenderState_DestBlend(device, 1);
        }
        c_rasterizer::draw_textured_screen_quad(vertices, false);
    }
}

// @address 0x2a4e30 rasterizer_setup_postprocess_pass
extern "C" void rasterizer_setup_postprocess_pass()
{
    c_screen_postprocess::setup_rasterizer_for_postprocess(true);
    real_vector4d constants;
    constants.i = 0.0f;
    constants.j = 0.0f;
    constants.k = 0.0f;
    constants.l = 0.0f;
    c_rasterizer::get_device()->SetPixelShaderConstantF(6, constants.n, 1);
    constants.i = 0.0f;
    c_rasterizer::get_device()->SetPixelShaderConstantF(3, constants.n, 1);
    constants.i = constants.j = 0.0f;
    c_rasterizer::get_device()->SetPixelShaderConstantF(4, constants.n, 1);
    constants.j = 0.0f;
    constants.k = 0.0f;
    c_rasterizer::get_device()->SetPixelShaderConstantF(5, constants.n, 1);
    rasterizer_draw_fullscreen_pass(1, 50);
    c_rasterizer::resolve_surface((c_rasterizer::e_surface)1, 0, 0, 0, 0);
}

class c_camera_fx_values;

// @address 0x2a4d68 rasterizer_render_tiled_grid
// match: 0x2a4d68: pointer __restrict qualifier prevents alias reload
extern "C" void rasterizer_render_tiled_grid(rasterizer_dx9_dynamic_geometry_screenshot_parameters * __restrict parameters)
{
    volatile s_rasterizer_tiling_state *tiling = (s_rasterizer_tiling_state *)&g_rasterizer_tiling_globals;
    bool saved_tiling_flag = tiling->include_ui;
    tiling->include_ui = 0;
    rasterizer_clear_postprocess_target(0);
    long row = 0;
    if (parameters->dimensions[5] > 0)
    {
        long column_count = parameters->dimensions[4];
        do
        {
            long column = 0;
            if (column_count > 0)
            {
                do
                {
                    main_screenshot_setup_tile_camera(parameters, column, row);
                    rasterizer_resolve_accumulation_targets(parameters);
                    screen_postprocess_render_flare(parameters, column, row);
                    column_count = parameters->dimensions[4];
                    column = column + 1;
                } while (column < column_count);
            }
            row = row + 1;
        } while (row < parameters->dimensions[5]);
    }
    tiling->include_ui = saved_tiling_flag;
    c_camera_fx_values *fx_source = parameters->camera_fx;
    c_screen_postprocess::s_settings *settings = &c_screen_postprocess::x_editable_settings;
    bool saved_bloom_flag = settings->debug;
    settings->debug = 0;
    c_screen_postprocess::postprocess_bloom_buffer((c_rasterizer::e_surface)30, *fx_source, c_rasterizer::_multires_full);
    settings->debug = saved_bloom_flag;
}

// @address 0x2a5340 ?rasterizer_psuedo_dynamic_screen_quad_draw@@YAXPBUrasterizer_dynamic_screen_geometry_parameters@@@Z
void rasterizer_psuedo_dynamic_screen_quad_draw(const rasterizer_dynamic_screen_geometry_parameters *parameters)
{
    const rasterizer_dx9_dynamic_geometry_screenshot_parameters *screenshot = (const rasterizer_dx9_dynamic_geometry_screenshot_parameters *)parameters;
    volatile s_rasterizer_tiling_state *tiling = (s_rasterizer_tiling_state *)&g_rasterizer_tiling_globals;
    for (long pass = 0; pass < tiling->pass_count; pass++)
    {
        if (tiling->composite_tiles)
            rasterizer_setup_postprocess_pass();
        for (long row = 0; row < screenshot->dimensions[5]; row++)
        {
            for (long column = 0; column < screenshot->dimensions[4]; column++)
            {
                bool tile_ready = true;
                if (!tiling->disabled)
                {
                    c_rasterizer::set_depth_stencil_surface((c_rasterizer::e_surface)3);
                    tile_ready = main_screenshot_setup_tile_camera(screenshot, column, row);
                }
                if (tile_ready)
                {
                    rasterizer_resolve_accumulation_targets(screenshot);
                }
                else
                {
                    c_rasterizer::g_HDR_alpha_blend = 0;
                    c_rasterizer::g_color_surfaces[0] = (c_rasterizer::e_surface)13;
                    c_render_surface *render_surface0 = rasterizer_dx9_dynamic_geometry_get_render_surface(13);
                    D3DDevice_SetRenderTarget_External(c_rasterizer::get_device(), 0, render_surface0->surface);
                    c_rasterizer::g_HDR_alpha_blend = 0;
                    c_rasterizer::g_color_surfaces[1] = (c_rasterizer::e_surface)14;
                    c_render_surface *render_surface1 = rasterizer_dx9_dynamic_geometry_get_render_surface(14);
                    D3DDevice_SetRenderTarget_External(c_rasterizer::get_device(), 1, render_surface1->surface);
                    c_rasterizer::clear(15, 0, 1.0f, 0);
                    c_rasterizer::g_HDR_alpha_blend = 0;
                    c_rasterizer::resolve_surface((c_rasterizer::e_surface)13, 0, 0, 0, 0);
                    c_rasterizer::g_HDR_alpha_blend = 0;
                    c_rasterizer::resolve_surface((c_rasterizer::e_surface)14, 1, 0, 0, 0);
                }
                main_screenshot_render_tile(screenshot, column, row);
                if (tiling->composite_tiles)
                    rasterizer_composite_tile(screenshot, column, row);
                if (tiling->end_callback != 0)
                    tiling->end_callback(column, row,
                        screenshot->dimensions[2], screenshot->dimensions[3], screenshot->dimensions[0], screenshot->dimensions[1],
                        pass, 9 + (tiling->skip_postprocess == false));
            }
        }
    }
}

// @address 0x2a4f70 rasterizer_composite_tile
extern "C" void rasterizer_composite_tile(const void *parameters, long column, long row)
{
    const rasterizer_dx9_dynamic_geometry_screenshot_parameters *screenshot = (const rasterizer_dx9_dynamic_geometry_screenshot_parameters *)parameters;
    const rasterizer_dx9_dynamic_geometry_screenshot_parameters *const *screenshot_reference = &screenshot;
    c_screen_postprocess::setup_rasterizer_for_postprocess(true);
    long source = 9 + !g_rasterizer_tiling_globals.skip_postprocess;
    real_vector4d tint;
    tint.i = tint.j = tint.k = g_rasterizer_tiling_globals.skip_postprocess ? 0.0f : 1.0f;
    tint.l = g_rasterizer_tiling_alpha;
    c_rasterizer::get_device()->SetPixelShaderConstantF(5, tint.n, 1);
    screen_postprocess_blit_scaled_surface(source, 11, screenshot->dimensions[2], screenshot->dimensions[3], 38);

    float width = (float)(*screenshot_reference)->dimensions[2];
    float height = (float)(*screenshot_reference)->dimensions[3];
    float scaled_width = (float)(*screenshot_reference)->dimensions[2] / (float)(*screenshot_reference)->dimensions[0]
        * (float)c_rasterizer::render_globals.width;
    float scaled_height = (float)(*screenshot_reference)->dimensions[3] / (float)(*screenshot_reference)->dimensions[1]
        * (float)c_rasterizer::render_globals.height;
    screen_postprocess_downsample_surface(&width, &height, scaled_width, scaled_height, 11);

    real_vector4d texture_transform;
    texture_transform.i = texture_transform.j = 0.0f;
    texture_transform.k = 1.0f;
    texture_transform.l = 1.0f;
    c_rasterizer::get_device()->SetPixelShaderConstantF(4, texture_transform.n, 1);
    real_vector4d color;
    color.j = 1.0f;
    color.k = color.l = 0.0f;
    color.i = 1.0f;
    c_rasterizer::get_device()->SetPixelShaderConstantF(3, color.n, 1);
    rasterizer_draw_fullscreen_pass(1, 40);
    if (c_rasterizer::get_current_alpha_blend_mode() != c_rasterizer::_alpha_blend_additive)
    {
        D3DDevice *device = c_rasterizer::get_device();
        c_rasterizer::set_current_alpha_blend_mode(c_rasterizer::_alpha_blend_additive);
        D3DDevice_SetRenderState_AlphaBlendEnable(device, 1);
        D3DDevice_SetRenderState_BlendOp(device, 0);
        D3DDevice_SetRenderState_SrcBlend(device, 1);
        D3DDevice_SetRenderState_DestBlend(device, 1);
    }

    float width_scale = width / scaled_width;
    float height_scale = height / scaled_height;
    real_vector4d offset;
    offset.i = 0.5f - ((float)column + 0.5f) * (float)(*screenshot_reference)->dimensions[2] * width_scale / (float)(*screenshot_reference)->dimensions[0];
    offset.j = 0.5f - ((float)row + 0.5f) * (float)(*screenshot_reference)->dimensions[3] * height_scale / (float)(*screenshot_reference)->dimensions[1];
    offset.k = offset.l = 0.0f;
    c_rasterizer::set_pixel_shader_constant(3, 1, &offset);
    c_screen_postprocess::apply_binary_op_ex(30, (c_rasterizer::e_surface)11, (c_rasterizer::e_surface)0, (c_rasterizer::e_surface)1,
        (c_rasterizer::e_sampler_filter_mode)2, (c_rasterizer::e_sampler_address_mode)3, width_scale, 0.0f, 0.0f, height_scale, NULL);
    c_rasterizer::resolve_surface((c_rasterizer::e_surface)1, 0, NULL, 0, 0);
    if (c_rasterizer::get_current_alpha_blend_mode() != c_rasterizer::_alpha_blend_opaque)
    {
        c_rasterizer::set_current_alpha_blend_mode(c_rasterizer::_alpha_blend_opaque);
        rasterizer_set_device_alpha_blend_mode(c_rasterizer::get_device(), 0);
    }
}

static __forceinline short rasterizer_dx9_dynamic_geometry_clamp_tile(short value, unsigned long extent)
{
    return (short)(((unsigned long)(value > 0 ? value : 0) < extent) ? (value > 0 ? value : 0) : extent);
}

static __forceinline real_vector2d rasterizer_dx9_dynamic_geometry_upper_corner(float x_scale, float y_scale)
{
    real_vector2d result;
    result.i = (x_scale + 1.0f) * 0.5f;
    result.j = (y_scale + 1.0f) * 0.5f;
    return result;
}

// @address 0x2a5658 rasterizer_export_screenshot_tile
extern "C" void rasterizer_export_screenshot_tile(
    long tile_x, long tile_y, long tile_width, long tile_height, long image_width, long image_height, long unused, long surface)
{
    rasterizer_vertex_screen vertices[3];
    c_screen_postprocess::setup_rasterizer_for_postprocess(true);
    long surface_width = get_render_surface_width_in_pixels(surface);
    long surface_height = get_render_surface_height_in_pixels(surface);
    D3DTexture *texture = (D3DTexture *)&g_rasterizer_tiling_globals.texture;
    XGTEXTURE_DESC description;
    XGGetTextureDesc(texture, 0, &description);
    real_vector4d constant_one;
    real_vector4d constant_tile;
    real_vector4d constant_transform;
    real_vector4d constant_export;
    constant_one.n[2] = 1.0f;
    constant_one.n[1] = 1.0f;
    constant_one.n[3] = 1.0f;
    constant_one.n[0] = 1.0f;
    c_rasterizer::get_device()->SetPixelShaderConstantF(5, constant_one.n, 1);
    screen_postprocess_blit_scaled_surface(surface, 10, tile_width, tile_height, 38);
    float width_scale = (float)tile_width;
    float height_scale = (float)tile_height;
    screen_postprocess_downsample_surface(&width_scale, &height_scale,
        (float)tile_width / (float)image_width * (float)description.Width,
        (float)tile_height / (float)image_height * (float)description.Height, 10);
    D3DDevice *device = c_rasterizer::get_device();
    device->BeginExport(0, texture, 1);
    if (c_rasterizer::set_explicit_shaders(83, _vertex_type_screen, _transfer_vector_vertex_none, _entry_point_default))
    {
        c_rasterizer::set_aliased_surface_as_texture(0, (c_rasterizer::e_surface)surface);
        c_rasterizer::set_sampler_address_mode(
            0, (c_rasterizer::e_sampler_address_mode)3, (c_rasterizer::e_sampler_address_mode)3, (c_rasterizer::e_sampler_address_mode)3);
        if (g_rasterizer_sampler_filter_modes[0] != 2)
        {
            g_rasterizer_sampler_filter_modes[0] = 2;
            rasterizer_device_set_sampler_filter_modes(c_rasterizer::get_device(), 0, 2, 2);
        }
        D3DDevice_SetTexture(c_rasterizer::get_device(), 1, texture, 0x40000000);
        c_rasterizer::set_render_target(0, (c_rasterizer::e_surface)10);
        c_rasterizer::set_render_target(1, (c_rasterizer::e_surface)0);
        real_rectangle2d normalized;
        normalized.x0 = (float)(tile_x * tile_width) / (float)image_width;
        normalized.x1 = (float)((tile_x + 1) * tile_width) / (float)image_width;
        normalized.y0 = (float)(tile_y * tile_height) / (float)image_height;
        normalized.y1 = (float)((tile_y + 1) * tile_height) / (float)image_height;
        float x0 = normalized.x0 * (float)description.Width;
        float x1 = normalized.x1 * (float)description.Width;
        float y0 = normalized.y0 * (float)description.Height;
        float y1 = normalized.y1 * (float)description.Height;
        rectangle2d pixel_bounds;
        pixel_bounds.x0 = (short)((float)floor(x0) - 2.0f);
        pixel_bounds.x1 = (short)((float)ceil(x1) + 2.0f);
        pixel_bounds.y0 = (short)((float)floor(y0) - 2.0f);
        pixel_bounds.y1 = (short)((float)ceil(y1) + 2.0f);
        pixel_bounds.x0 = rasterizer_dx9_dynamic_geometry_clamp_tile(pixel_bounds.x0, description.Width);
        pixel_bounds.x1 = rasterizer_dx9_dynamic_geometry_clamp_tile(pixel_bounds.x1, description.Width);
        pixel_bounds.y0 = rasterizer_dx9_dynamic_geometry_clamp_tile(pixel_bounds.y0, description.Height);
        pixel_bounds.y1 = rasterizer_dx9_dynamic_geometry_clamp_tile(pixel_bounds.y1, description.Height);
        float scale_x = width_scale / (float)surface_width;
        float scale_y = height_scale / (float)surface_height;
        constant_tile.n[0] = constant_tile.n[1] = 1.0f;
        constant_tile.n[2] = (float)pixel_bounds.x0;
        constant_tile.n[3] = (float)pixel_bounds.y0;
        c_rasterizer::get_device()->SetPixelShaderConstantF(2, constant_tile.n, 1);
        float offset_x = 0.5f - scale_x * 0.5f;
        float offset_y = 0.5f - scale_y * 0.5f;
        real_vector2d upper_corner = rasterizer_dx9_dynamic_geometry_upper_corner(scale_x, scale_y);
        constant_transform.n[0] = (upper_corner.i - offset_x) / (x1 - x0);
        constant_transform.n[1] = (upper_corner.j - offset_y) / (y1 - y0);
        constant_transform.n[2] = offset_x - (x0 - 0.5f) * constant_transform.n[0];
        constant_transform.n[3] = offset_y - (y0 - 0.5f) * constant_transform.n[1];
        c_rasterizer::get_device()->SetPixelShaderConstantF(3, constant_transform.n, 1);
        constant_export.n[0] = (float)(description.RowPitch / (description.BitsPerPixel >> 3));
        constant_export.n[1] = (float)description.Height * constant_export.n[0];
        constant_export.n[2] = g_rasterizer_tiling_alpha;
        constant_export.n[3] = 0.0f;
        c_rasterizer::get_device()->SetPixelShaderConstantF(4, constant_export.n, 1);
        device->SetPixelShaderConstantF(5, (const float *)&g_rasterizer_tiling_globals.export_stream, 1);
        vertices[0].x = -1.0f;
        vertices[0].y = 1.0f;
        vertices[0].u = 0.0f;
        vertices[0].v = 0.0f;
        vertices[1].x = (float)(pixel_bounds.x1 - pixel_bounds.x0) * 2.0f / (float)surface_width - 1.0f;
        vertices[1].y = 1.0f;
        vertices[1].u = 1.0f;
        vertices[1].v = 0.0f;
        vertices[2].x = -1.0f;
        vertices[2].y = 1.0f - (float)(pixel_bounds.y1 - pixel_bounds.y0) * 2.0f / (float)surface_height;
        vertices[2].u = 0.0f;
        vertices[2].v = 1.0f;
        D3DDevice_SetRenderState_CullMode(c_rasterizer::get_device(), 0);
        D3DDevice_SetIndices(device, 0);
        D3DDevice_DrawVerticesUP(device, (D3DPRIMITIVETYPE)8, 3, vertices, 20);
        D3DDevice_SetRenderState_CullMode(c_rasterizer::get_device(), 2);
    }
    device->EndExport(0, texture, 0);
}

// @address 0x737504 g_tiled_render_x_scale
// @address 0x737508 g_tiled_render_y_scale
// @address 0x8c4830 g_tiled_render_x_offset
// @address 0x8c4834 g_tiled_render_y_offset

static __forceinline void rasterizer_dx9_dynamic_geometry_screen_filter(long sampler, long mode)
{
    if (g_rasterizer_sampler_filter_modes[sampler] != mode)
    {
        g_rasterizer_sampler_filter_modes[sampler] = mode;
        if ((unsigned long)mode <= 7)
            rasterizer_device_set_sampler_filter_modes(c_rasterizer::get_device(), sampler, mode, mode);
    }
}
static __forceinline void rasterizer_dx9_dynamic_geometry_screen_normalize(float inverse_width, float inverse_height,
    rasterizer_vertex_screen *vertices, long count, float x_offset, float y_offset)
{
    float x_scale = inverse_width * 2.0f;
    float y_scale = inverse_height * 2.0f;
    while (--count >= 0)
    {
        vertices[count].x = (((vertices[count].x * g_tiled_render_x_scale + g_tiled_render_x_offset) - 0.5f) * x_scale - 1.0f) + x_offset;
        vertices[count].y = (y_offset - (((vertices[count].y * g_tiled_render_y_scale + g_tiled_render_y_offset) - 0.5f) * y_scale - 1.0f));
    }
}

// @address 0x1902a0 rasterizer_draw_dynamic_screen_quad
extern "C" void rasterizer_draw_dynamic_screen_quad(const rasterizer_dynamic_screen_geometry_parameters *parameters, rasterizer_vertex_screen *vertices)
{
    s_game_globals *globals = ((s_game_globals *)global_game_globals);
    c_rasterizer_globals *rasterizer_globals = globals->rasterizer_globals_ref.index != NONE
        ? (c_rasterizer_globals *)tag_get('rasg', globals->rasterizer_globals_ref.index) : NULL;
    const s_tag_reference *vertex_reference = &rasterizer_globals->get_explicit_shaders(
        parameters->explicit_shader_index == NONE ? 2 : parameters->explicit_shader_index)->vertex;
    const s_tag_reference *pixel_reference = &rasterizer_globals->get_explicit_shaders(
        parameters->explicit_shader_index == NONE ? 2 : parameters->explicit_shader_index)->pixel;
    const c_rasterizer_vertex_shader *vertex_shader = vertex_reference->index != NONE ? c_rasterizer_vertex_shader::get(vertex_reference->index) : NULL;
    const c_rasterizer_pixel_shader *const &pixel_shader = pixel_reference->index != NONE ? c_rasterizer_pixel_shader::get(pixel_reference->index) : NULL;
    // match: 0x1902a0: testing pixel_shader directly sends the missing-index branch to the exit and drops li r31,0 at 0x19035c.
    if ((pixel_shader + (long)_rotl(_rotl(0, 16), 16)) && vertex_shader)
    {
        if (parameters->textures[0].valid())
        {
            c_rasterizer::set_vertex_shader(vertex_shader, _vertex_type_screen, _transfer_vector_vertex_none, _entry_point_default);
            c_rasterizer::set_pixel_shader(pixel_shader, _entry_point_default);
            c_rasterizer::set_sampler_texture(0, parameters->textures[0]);
            c_rasterizer::e_sampler_address_mode address0 = (c_rasterizer::e_sampler_address_mode)!parameters->texture_address[0];
            c_rasterizer::set_sampler_address_mode(0, address0, address0, address0);
            rasterizer_dx9_dynamic_geometry_screen_filter(0, !parameters->point_sampled + 1);
            if (parameters->textures[1].valid())
            {
                c_rasterizer::set_sampler_texture(1, parameters->textures[1]);
                c_rasterizer::e_sampler_address_mode address1 = (c_rasterizer::e_sampler_address_mode)!parameters->texture_address[1];
                c_rasterizer::set_sampler_address_mode(1, address1, address1, address1);
                rasterizer_dx9_dynamic_geometry_screen_filter(1, !parameters->point_sampled + 1);
            }
            real_vector4d color;
            color.i = color.j = color.k = 1.0f;
            color.l = c_rasterizer::get_render_target_alpha_multiplier(0);
            D3DDevice_SetPixelShaderConstantF(c_rasterizer::get_device(), 2, &color.i, 1);
            long alpha_blend_mode = parameters->alpha_blend_mode;
            if (alpha_blend_mode != c_rasterizer::get_current_alpha_blend_mode())
            {
                c_rasterizer::set_current_alpha_blend_mode(static_cast<c_rasterizer::e_alpha_blend_mode>(alpha_blend_mode));
                rasterizer_set_device_alpha_blend_mode(c_rasterizer::get_device(), alpha_blend_mode);
            }
            rectangle2d bounds;
            if (c_player_view::get_current())
                bounds = c_player_view::get_current()->get_rasterizer_camera()->window_pixel_bounds;
            else
            {
                bounds.x0 = bounds.y0 = 0;
                bounds.x1 = (short)c_rasterizer::render_globals.width;
                bounds.y1 = (short)c_rasterizer::render_globals.height;
            }
            float inverse_width = 1.0f / (float)(bounds.x1 - bounds.x0);
            float inverse_height = 1.0f / (float)(bounds.y1 - bounds.y0);
            rasterizer_dx9_dynamic_geometry_screen_normalize(inverse_width, inverse_height,
                vertices, 4,
                (float)bounds.x0 * inverse_width * k_rasterizer_screen_x_offset_scale,
                (float)bounds.y0 * inverse_height * 2.0f);
            c_rasterizer::draw_textured_screen_quad(vertices, false);
        }
    }
}

// @address 0x4595b0 rasterizer_render_displacement_pass
void rasterizer_render_displacement_pass(const rectangle2d *bounds, s_displacement_motion_blur_transforms transforms, bool motion_blur)
{
    if (rasterizer_is_surface_resolved((c_rasterizer::e_surface)52) || motion_blur)
    {
        if (rasterizer_is_surface_resolved((c_rasterizer::e_surface)52))
            c_rasterizer::set_aliased_surface_as_texture(0, (c_rasterizer::e_surface)52);
        else
            c_rasterizer::set_aliased_surface_as_texture(0, (c_rasterizer::e_surface)52);
        c_rasterizer::set_sampler_address_mode(
            0, (c_rasterizer::e_sampler_address_mode)1, (c_rasterizer::e_sampler_address_mode)1, (c_rasterizer::e_sampler_address_mode)1);
        rasterizer_dx9_dynamic_geometry_screen_filter(0, 2);
        c_rasterizer::g_HDR_alpha_blend = 0;
        c_rasterizer::set_aliased_surface_as_texture(1, (c_rasterizer::e_surface)13);
        c_rasterizer::set_sampler_address_mode(
            1, (c_rasterizer::e_sampler_address_mode)1, (c_rasterizer::e_sampler_address_mode)1, (c_rasterizer::e_sampler_address_mode)1);
        rasterizer_dx9_dynamic_geometry_screen_filter(1, 2);
        if (motion_blur)
        {
            c_rasterizer::set_aliased_surface_as_texture(3, (c_rasterizer::e_surface)3);
            c_rasterizer::set_sampler_address_mode(3, (c_rasterizer::e_sampler_address_mode)1, (c_rasterizer::e_sampler_address_mode)1,
                (c_rasterizer::e_sampler_address_mode)1);
            rasterizer_dx9_dynamic_geometry_screen_filter(3, 1);
            rasterizer_set_motion_blur_constants();
        }
        if (c_rasterizer::get_current_alpha_blend_mode() != c_rasterizer::_alpha_blend_opaque)
        {
            c_rasterizer::set_current_alpha_blend_mode(c_rasterizer::_alpha_blend_opaque);
            D3DDevice_SetRenderState_AlphaBlendEnable(c_rasterizer::get_device(), 0);
        }
        c_rasterizer::set_z_buffer_mode(c_rasterizer::_z_buffer_mode_off);
        rasterizer_main_setup_hdr_render_targets(true, false);

        s_game_globals *globals = ((s_game_globals *)global_game_globals);
        c_rasterizer_globals *rasterizer_globals = globals->rasterizer_globals_ref.index != NONE
            ? (c_rasterizer_globals *)tag_get('rasg', globals->rasterizer_globals_ref.index)
            : NULL;
        const long &explicit_shader_index = motion_blur ? 81 : 37;
        const c_rasterizer_globals::s_explicit_shader *shader = rasterizer_globals->get_explicit_shaders(explicit_shader_index);
        c_rasterizer::set_vertex_shader(
            c_rasterizer_vertex_shader::get(shader->vertex.index), _vertex_type_screen, _transfer_vector_vertex_none, _entry_point_default);
        c_rasterizer::set_pixel_shader(c_rasterizer_pixel_shader::get(shader->pixel.index), _entry_point_default);

        rectangle2d viewport;
        viewport.y0 = 0;
        viewport.x0 = 0;
        viewport.y1 = (short)c_rasterizer::render_globals.height;
        viewport.x1 = (short)c_rasterizer::render_globals.width;
        c_rasterizer::set_viewport(viewport, 0.0f, 1.0f);

        real_vector4d screen_constants = { 0.0f };
        const rectangle2d *const *bounds_reference = &bounds;
        screen_constants.i = 1.0f / (float)((*bounds_reference)->x1 - (*bounds_reference)->x0);
        screen_constants.j = 1.0f / (float)((*bounds_reference)->y1 - (*bounds_reference)->y0);
        rasterizer_tiling_get_scale(&screen_constants.k, &screen_constants.l);
        c_rasterizer::set_pixel_shader_constant(203, 1, &screen_constants);

        real_vector4d texture_constants;
        texture_constants.i = 0.0f;
        texture_constants.j = 0.0f;
        texture_constants.k = 1.0f;
        texture_constants.l = 1.0f;
        c_rasterizer::set_pixel_shader_constant(204, 1, &texture_constants);

        if (motion_blur)
        {
            c_rasterizer::set_pixel_shader_constant(160, 4, transforms.transform0);
            c_rasterizer::set_pixel_shader_constant(164, 4, transforms.transform1);
            c_rasterizer::set_pixel_shader_constant(168, 4, transforms.transform2);
        }
        rasterizer_draw_screen_grid();
        c_rasterizer::set_z_buffer_mode(c_rasterizer::_z_buffer_mode_read_only);
        D3DDevice_SetRenderState_CullMode(c_rasterizer::get_device(), 2);
        c_rasterizer::set_viewport(g_rasterizer_last_rectangles.viewport, 0.0f, 1.0f);
        c_rasterizer::set_scissor_rect(&g_rasterizer_last_rectangles.scissor);
    }
}

struct render_objects_static_lighting_screen_effect_settings;
struct s_screenshot_options;

extern "C" {

// @address 0x72cd3f g_cinematic_letterbox_enabled
bool g_cinematic_letterbox_enabled = true;

// match: 0x1921a8: retain the static GPR anchor spanning the adjacent configured limits.
// @address 0x8c46c0 g_rasterizer_shader_gpr_globals
static __declspec(align(4)) s_rasterizer_shader_gpr_globals g_rasterizer_shader_gpr_globals;

}

// @address 0x1921a8 rasterizer_render_screenshot
extern "C" unsigned char rasterizer_render_screenshot(rasterizer_dx9_dynamic_geometry_screenshot_parameters *parameters,
    c_player_view *player_views, c_camera_fx_values *camera_fx, long flags)
{
    parameters->camera_fx = camera_fx;
    parameters->flags = flags;
    long screenshot_delay = g_rasterizer_tiling_globals.screenshot_delay;
    parameters->view = g_rasterizer_player_views;
    if (screenshot_delay > 0)
    {
        parameters->active = true;
        --g_rasterizer_tiling_globals.screenshot_delay;
    }
    else
    {
        parameters->active = true;
    }
    __lwsync();
    if (!main_globals.secondary_flag && g_rasterizer_tiling_globals.screenshot_delay == 0
        && g_rasterizer_tiling_globals.screenshot_active && !g_rasterizer_tiling_globals.disabled)
    {
        parameters->active = false;
        if (main_screenshot_compute_tile_layout((s_screenshot_options *)parameters))
        {
            bool saved_render_flag = g_cinematic_letterbox_enabled;
            g_cinematic_letterbox_enabled = false;
            g_rasterizer_tiling_globals.enabled = true;
#if defined(_XBOX) && !defined(__clang__)
            if (g_rasterizer_particle_submission_state.gpr_allocation != 0)
#else
            if (g_rasterizer_gpr_allocation != 0)
#endif
            {
                g_rasterizer_shader_gpr_globals[0] = 0;
                g_rasterizer_shader_gpr_globals[1] = 0;
                c_rasterizer::get_device()->SetShaderGPRAllocation(0, 0, 0);
#if defined(_XBOX) && !defined(__clang__)
                g_rasterizer_particle_submission_state.gpr_allocation = 0;
#else
                g_rasterizer_gpr_allocation = 0;
#endif
                unsigned long allocation_flags;
                c_rasterizer::get_device()->GetShaderGPRAllocation(&allocation_flags, &g_rasterizer_shader_gpr_globals[0], &g_rasterizer_shader_gpr_globals[1]);
            }
            render_objects_compute_screen_effect(&parameters->view->get_rasterizer_camera()->position, &parameters->view->get_rasterizer_camera()->forward,
                (render_objects_static_lighting_screen_effect_settings *)parameters->screen_effects, 0);
            if (!g_rasterizer_tiling_globals.render_bloom && !g_rasterizer_tiling_globals.render_flares)
                rasterizer_clear_postprocess_target(0xff000000);
            else
                rasterizer_render_tiled_grid((rasterizer_dx9_dynamic_geometry_screenshot_parameters *)parameters);
            rasterizer_psuedo_dynamic_screen_quad_draw((const rasterizer_dynamic_screen_geometry_parameters *)parameters);
            long volatile *fence_destination = g_rasterizer_tiling_globals.fence_destination;
            if (fence_destination)
            {
                unsigned long fence = c_rasterizer::get_device()->InsertFence();
#if defined(_XBOX) && !defined(__clang__)
                g_rasterizer_particle_submission_state.fence = fence;
#else
                g_game_state_render_fence = fence;
#endif
                long volatile *destination = g_rasterizer_tiling_globals.fence_destination;
                __lwsync();
                InterlockedExchange(destination, fence);
            }
            g_cinematic_letterbox_enabled = saved_render_flag;
            g_tiled_render_x_scale = 1.0f;
            g_tiled_render_y_scale = 1.0f;
            g_tiled_render_x_offset = 0.0f;
            g_tiled_render_y_offset = 0.0f;
        }
        g_rasterizer_tiling_globals.enabled = false;
        g_rasterizer_tiling_globals.screenshot_active = 0;
        g_rasterizer_tiling_globals.spare_after_enabled = false;
        g_rasterizer_tiling_globals.include_ui = true;
        g_rasterizer_tiling_globals.render_bloom = false;
        g_rasterizer_tiling_globals.render_flares = false;
        g_rasterizer_tiling_globals.skip_postprocess = false;
        g_rasterizer_tiling_globals.disabled = false;
        g_rasterizer_tiling_globals.tiling_initialized = false;
        g_rasterizer_tiling_globals.composite_tiles = true;
        g_rasterizer_tiling_globals.use_half_size_tiles = false;
        g_rasterizer_tiling_globals.reserved_tiling_flag = false;
        g_rasterizer_tiling_globals.end_callback = 0;
        g_rasterizer_tiling_globals.pad_to_tile_grid = false;
        g_rasterizer_tiling_globals.fence_destination = 0;
    }
    return parameters->active;
}

// @address 0x8c46bc g_rasterizer_use_floating_point_z_buffer

// match: 0x178d90: an ordinary cross-unit storage accessor keeps the owner's shared BSS base when inlined into callers.
extern "C" volatile s_rasterizer_tiling_state *rasterizer_tiling_get_state()
{
    return &g_rasterizer_tiling_globals;
}

const volatile s_rasterizer_tiling_state_header *rasterizer_tiling_get_state_header()
{
    return &g_rasterizer_tiling_globals;
}

// match: 0x178d90: obtain GPR storage from the same owner as the tiling state to share their compiler anchor.
extern "C" s_rasterizer_shader_gpr_globals *rasterizer_shader_gpr_get_globals()
{
    return &g_rasterizer_shader_gpr_globals;
}
