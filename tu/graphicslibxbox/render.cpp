// graphicslibxbox/render.obj: from the records 0x152328 0x152520 0x16fd28 0x449a48 0x449be8 0x449c10 0x449d28 0x449d80 0x44a030 0x44a218 0x44a2c0 0x44a3b8 0x4508d0 0x450ac8 0x450bc0 0x70a100 0x70c8f8
#include "rasterizer/rasterizer_state_globals.h"
#include "math/real_math_constants.h"
#include "rasterizer/xenon/rasterizer_xenon_submit.h"
#include "objects/object_render_state_tls.h"
#include "render/render_transparency_globals.h"
#include "render/render_object_storage.h"
#include "text/font_id_types.h"
#include "rasterizer/rasterizer_states.h"
#include "rasterizer/rasterizer_resource_definitions.h"
#include "rasterizer/rasterizer_main_globals.h"
#include "render/render_camera_types.h"
#include "math/matrix_math_types.h"
#include "memory/data_types.h"
#include "objects/object_reference_types.h"
#include <xtl.h>
#include "multithreading/thread_tls.h"
#include "cseries/crt_xdk.h"
#include "math/real_math.h"
#include <ppcintrinsics.h>
#include "cache/restricted_memory_regions.h"
#include "render/render.h"
#include "text/draw_string.h"
#include "text/font_cache_lock.h"
#include "render/render_objects.h"
#include "memory/data.h"
#include "render/render_structure.h"
#include "render/render_transparents.h"
#include "geometry/geometry_sampling.h"
#include "render/render_text_style_types.h"
#include "cseries/global_real_argb_colors.h"
#include "interface/chud/utf32_types.h"
#include "render/render_objects_static_lighting.h"
#include "memory/physical_memory_map.h"
#include "render/render_object_globals_types.h"
#include "cache/restricted_memory_layout.h"
#include "math/real_math_types.h"
#include "render/render_globals.h"
#include "interface/user_interface_window_manager.h"
#include "cache/restricted_memory.h"
#include "render/shader_extern_info_types.h"
#include "text/font_character_types.h"
#include "text/draw_string_types.h"
#include "text/rasterizer_draw_string_types.h"
#include "text/font_cache_types.h"
#include "rasterizer/rasterizer_text.h"
#include "main/main_render_loading_types.h"
#include "math/real_constant_pool.h"
#include "rasterizer/rasterizer.h"
#include "render/screen_postprocess.h"
#include "objects/object_render_thread_message.h"
#include "multithreading/message_queue.h"
#include "render/render_structure_globals_types.h"
#include "render/render_first_person_globals_types.h"
#include "render/views/player_view_types.h"
#include "render/render_objects_globals.h"
#include "render/render_visibility_cached_state_types.h"

struct s_render_object_first_person_globals;
class c_font_cache;
struct s_font_header;
struct D3DDevice;
struct s_resolve_short_rectangle;
struct D3DPixelShader;
struct D3DVertexShader;
// @address 0x16fe60 D3DDevice_SetRenderState_AlphaFunc
// @address 0x7c6178 g_rasterizer_sampler_filter_modes
// @address 0x195e88 rasterizer_device_set_sampler_filter_modes
// @flags /TP /O2 /Oc
// @address 0x44a2c0 ?render_draw_styled_string@@YAXPBUs_render_text_style@@PAVc_draw_string@@PAVc_font_cache@@@Z
// @address 0x1ec160 ?data_new@@YAPAUs_data_array@@PBDJJJPAVc_allocation_base@@@Z
// @address 0x191810 ?set_shadow_color@c_draw_string@@QAAXPBTreal_argb_color@@@Z
// @address 0x2902f8 ?set_color_deprecated@c_draw_string@@QAAXPBTreal_argb_color@@_N@Z
// @address 0x2905e8 ?draw_more@c_draw_string@@QAA_NPAVc_font_cache@@PBD@Z
// @address 0x9b308 ?normalize3d@@YAMPATreal_vector3d@@@Z
// @address 0x80608 GetTickCount
// @address 0x152520 ?combine_projection_and_view_matrix@@YAXPBUreal_matrix4x3@@QAY03$$CBMQAM@Z

// @address 0x8c446f g_rasterizer_alpha_to_mask_enabled
extern "C" { bool g_rasterizer_alpha_to_mask_enabled; }

// @address 0x16fd28 rasterizer_set_alpha_test_enabled
void rasterizer_set_alpha_test_enabled(unsigned char enable, int)
{
    if (enable)
    {
        if (!g_decorator_albedo_active && c_rasterizer::get_current_alpha_blend_mode() != c_rasterizer::_alpha_blend_opaque)
        {
            D3DDevice *device = c_rasterizer::get_device();
            c_rasterizer::set_current_alpha_blend_mode(c_rasterizer::_alpha_blend_opaque);
            D3DDevice_SetRenderState_AlphaBlendEnable(device, FALSE);
        }

        if (g_rasterizer_alpha_to_mask_enabled)
        {
            D3DDevice_SetRenderState_AlphaToMaskEnable(c_rasterizer::get_device(), TRUE);
            D3DDevice_SetRenderState_AlphaToMaskOffsets(c_rasterizer::get_device(), 0xaa);
            return;
        }
        else
        {
            D3DDevice_SetRenderState_AlphaTestEnable(c_rasterizer::get_device(), TRUE);
            D3DDevice_SetRenderState_AlphaFunc(c_rasterizer::get_device(), D3DCMP_GREATEREQUAL);
            D3DDevice_SetRenderState_AlphaRef(c_rasterizer::get_device(), 128);
            return;
        }
    }
    else if (g_rasterizer_alpha_to_mask_enabled)
    {
        D3DDevice_SetRenderState_AlphaToMaskEnable(c_rasterizer::get_device(), FALSE);
        return;
    }
    else
    {
        D3DDevice_SetRenderState_AlphaTestEnable(c_rasterizer::get_device(), FALSE);
        return;
    }
}


// @address 0x11df878 g_object_message_queue
// @address 0x10eb500 g_rasterizer_player_views
// @address 0x3ac90 global_real_argb_colors
// @address 0x8c46e0 ?m_frame_time@c_render_globals@@0MA
float c_render_globals::m_frame_time;
// @address 0x8c46e6 ?m_depth_fade_active@c_render_globals@@0_NA
bool c_render_globals::m_depth_fade_active;
// @address 0x73a188 ?m_frame_index@c_render_globals@@0KA
unsigned long c_render_globals::m_frame_index = 1;
// @address 0x8c46e7 ?m_skinning_constants_owned@c_render_globals@@0_NA
bool c_render_globals::m_skinning_constants_owned;
// @address 0x8c46bf ?m_distortion_active@c_render_globals@@0_NA
bool c_render_globals::m_distortion_active;

void combine_projection_and_view_matrix(const real_matrix4x3 *view, const float (* const projection)[4], float * const out)
{
    const float *view_elements = &view->rotation.m[0][0];
    const float *projection_elements = &projection[0][0];
    float scale = 1.0f;
    int output_index = 0;
    for (int projection_column = 0; projection_column < 4; projection_column++)
    {
        for (int view_row = 0; view_row < 4; view_row++)
        {
            out[output_index] = scale *
                (projection_elements[projection_column] * view_elements[view_row * 3] +
                    projection_elements[projection_column + 4] * view_elements[view_row * 3 + 1] +
                    projection_elements[projection_column + 8] * view_elements[view_row * 3 + 2]);
            output_index = output_index + 1;
        }
        out[output_index - 1] = out[output_index - 1] + scale * projection_elements[projection_column + 12];
    }
}

void *g_restricted_render_memory;
// @address 0x70a100 ??__Eg_restricted_render_memory_section@@YAXXZ
// @address 0x70c8f8 ??__Fg_restricted_render_memory_section@@YAXXZ
// @address 0x7bba18 ?g_restricted_render_memory_section@@3Vc_restricted_section@@A
c_restricted_section g_restricted_render_memory_section;
// @address 0x1ec580 ?data_delete_all@@YAXPAUs_data_array@@@Z
// @address 0x449a48 ?render_initialize@@YAXXZ
void render_initialize()
{
    g_restricted_render_memory=::_physical_memory_malloc((memory_stage)1, (char const *)(0), 0x80000, 0, (char const *)(0), -1);
    g_restricted_render_memory_section.initialize(g_restricted_render_memory,0x80000,32);
    g_restricted_regions[4].initialize(4,&g_restricted_render_memory_section,31,0);
    ::restricted_region_lock_primary(4);
    c_render_globals::set_frame_index(0);
    c_render_globals::set_frame_time(0.0f);
    g_cached_object_render_states=data_new("cached object render states",384,sizeof(s_visibility_cached_object_render_state),0,(c_allocation_base *)&g_cached_object_render_states_allocator);
    render_object_globals.object_index=-1;
    g_render_object_globals =
        (s_render_object_globals *)g_render_object_globals_allocator.reserve_memory(
            "render object", "globals", 0x3c040, 0);
}
// @address 0x449be8 ?render_initialize_for_new_map@@YAXXZ
void render_initialize_for_new_map()
{
    c_render_globals::set_frame_time(0.0f);
    data_make_valid(g_cached_object_render_states);
}

// @address 0x449760 ?render_initialize_for_new_structure_bsp@@YAXK@Z
void render_initialize_for_new_structure_bsp(unsigned long structure_bsp_mask)
{
    c_structure_renderer::reset();
}

// @address 0x5bec98 ?render_dispose_from_old_structure_bsp@@YAXK@Z
void render_dispose_from_old_structure_bsp(unsigned long structure_bsp_mask)
{
}

// @address 0x449d28 ?render_dispose@@YAXXZ
void render_dispose()
{
    g_cached_object_render_states = 0;
    restricted_region_unlock_primary(4);
    ::restricted_region_destroy(4);
    if (g_restricted_render_memory)
    {
        g_restricted_render_memory = 0;
    }
}

// @address 0x44a218 ?initialize_default_shader_extern_info@@YAXPAUs_shader_extern_info@@@Z
void initialize_default_shader_extern_info(s_shader_extern_info *info)
{
    memset(info,0,sizeof(s_shader_extern_info));
    info->object_index=-1;
    info->skinning_offset=-1;
    info->lightprobe_sample=c_geometry_sampler::m_default_lightprobe_sample;
    const real_vector3d up={0.0f,0.0f,1.0f};
    negate_vector3d(&up,&info->down);
    info->instance_index=-1;
    info->cubemap_sample.set_defaults();
}


// @address 0x44a2c0 ?render_draw_styled_string@@YAXPBUs_render_text_style@@PAVc_draw_string@@PAVc_font_cache@@@Z
void render_draw_styled_string(const s_render_text_style *style, c_draw_string *draw, c_font_cache *font_cache)
{
    const real_rgb_color *color = style->color;
    if (!color)
    {
        color = &global_real_argb_colors[_real_argb_white].rgb;
    }
    real_argb_color color_argb;
    color_argb.alpha = 1.0f;
    color_argb.rgb = *color;
    if (style->shadow_color)
    {
        real_argb_color shadow_argb;
        shadow_argb.alpha = 1.0f;
        shadow_argb.rgb = *style->shadow_color;
        draw->set_shadow_color(&shadow_argb);
    }
    draw->set_color_deprecated(&color_argb, true);
    draw->set_scale(style->scale);
    draw->draw(font_cache, "\r\n");
    if (style->string)
    {
        draw->draw_more(font_cache, "\r\n");
        draw->draw_more(font_cache, style->string);
    }
}
// @address 0x1148 ??_7c_simple_font_draw_string@@6B@
// @address 0x191968 ?draw@c_simple_font_draw_string@@UAA_NPAVc_font_cache@@PBD@Z
// @address 0x4556f0 ?draw@c_simple_font_draw_string@@UAA_NPAVc_font_cache@@PB_W@Z
extern "C" {
// @address 0x10b0 k_font_cache_vtable
}

// @address 0x44a3b8 render_draw_pregame_text
extern "C" void render_draw_pregame_text(const s_render_text_style *style, bool simple)
{
    if ((unsigned char)simple != 0)
    {
        c_simple_font_draw_string draw;
        render_draw_styled_string(style, &draw, NULL);
        return;
    }
    c_rasterizer_draw_string draw;
    c_font_cache cache;
    draw.m_font_id = _terminal_font;
    draw.m_font_header = font_get_header(0);
    render_draw_styled_string(style, &draw, &cache);
}


extern "C" {
// @address 0x242d4 k_real_constant_pool_three
}
// @address 0x450ac8 render_is_loading_complete
extern "C" bool render_is_loading_complete(void)
{
    bool result = false;
    unsigned int now = GetTickCount();
    if (now - g_main_render_loading_globals.last_time > 5000)
    {
        result = true;
    }
    else if (g_main_render_loading_globals.initialized)
    {
#if defined(_XBOX) && !defined(__clang__)
        // match: 0x450ac8: shared constant-pool base register; r9 base kept live for the 3.0 lfs
        if (g_main_render_loading_globals.smoothed_progress >= k_real_constant_pool_three[2234])
#else
        if (g_main_render_loading_globals.smoothed_progress >= 0.9f)
#endif
        {
            int completion_time = g_main_render_loading_globals.completion_time;
            if (completion_time == -1)
            {
                g_main_render_loading_globals.completion_time = now;
                g_window_manager.m_fade_in = 1;
            }
            else
            {
#if defined(_XBOX) && !defined(__clang__)
                // match: 0x450ac8: shared constant-pool base register; lfs 0(r9) reuses the 0.9 base
                result = (float)(double)(unsigned int)(now - completion_time) * k_real_one_thousandth_storage > k_real_constant_pool_three[0];
#else
                result = (float)(double)(unsigned int)(now - completion_time) * k_real_one_thousandth_storage > 3.0f;
#endif
            }
        }
    }
    else
    {
        result = true;
    }
    return result;
}

// @address 0x450bc0 render_is_load_recently_complete
bool render_is_load_recently_complete(void)
{
    long now = GetTickCount();
    bool result = false;
    if (g_main_render_loading_globals.completion_time != -1)
    {
        result = (float)(int)(now - g_main_render_loading_globals.completion_time) * 0.001f <= 3.0f;
    }
    return result;
}

extern "C" {
// @address 0x836908 g_rasterizer_shared_shader_cache

}
// @address 0x4508d0 render_initialize_loading_screen
extern "C" void render_initialize_loading_screen(void)
{
    g_main_render_loading_globals.initialized = true;
    g_main_render_loading_globals.started = false;
    g_main_render_loading_globals.owner_thread = get_current_thread_index();
    g_main_render_loading_globals.completion_time = -1;
    float *cursor = g_main_render_loading_globals.clear_values;
    unsigned int remaining = 0x800;
    do {
        *cursor = 1000.0f;
        cursor++;
        remaining--;
    } while (remaining != 0);
    memset(g_main_render_loading_globals.history, 0, sizeof(g_main_render_loading_globals.history));
    g_main_render_loading_globals.history_index = 0;
    c_screen_postprocess::setup_rasterizer_for_postprocess(false);
    void *declaration = rasterizer_get_vertex_declaration(_vertex_type_screen, _transfer_vector_vertex_none, _entry_point_default);
    D3DDevice_SetVertexDeclaration(c_rasterizer::get_device(), (D3DVertexDeclaration *)declaration);
    D3DDevice_SetPixelShader(c_rasterizer::get_device(), g_rasterizer_shared_shader_cache.pixel_shaders[7]);
    D3DDevice_SetVertexShader(c_rasterizer::get_device(), g_rasterizer_shared_shader_cache.vertex_shaders[7]);
    D3DDevice_SetRenderState_ColorWriteEnable(c_rasterizer::get_device(), 15);
    rasterizer_set_alpha_test_enabled(0, 0);
    if (c_rasterizer::g_current_separate_alpha_blend_mode != c_rasterizer::_separate_alpha_blend_off)
    {
        c_rasterizer::g_current_separate_alpha_blend_mode = c_rasterizer::_separate_alpha_blend_off;
        D3DDevice_SetRenderState_SeparateAlphaBlendEnable(c_rasterizer::get_device(), 0);
    }
    c_rasterizer::set_stencil_mode(c_rasterizer::_stencil_mode_off);
    D3DDevice_SetRenderState_FillMode(c_rasterizer::get_device(), 0);
    D3DDevice_SetRenderState_ClipPlaneEnable(c_rasterizer::get_device(), 0);
    c_rasterizer::set_aliased_surface_as_texture(0, (c_rasterizer::e_surface)1);
    c_rasterizer::set_sampler_address_mode(0, c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp);
    if (g_rasterizer_sampler_filter_modes[0] != 1)
    {
        g_rasterizer_sampler_filter_modes[0] = 1;
        rasterizer_device_set_sampler_filter_modes(c_rasterizer::get_device(), 0, 1, 1);
    }
    rasterizer_set_back_buffer_render_target(0, 1);
    float shader_constants[6];
    shader_constants[3] = 1.0f;
    shader_constants[2] = 1.0f;
    shader_constants[1] = 1.0f;
    shader_constants[0] = 1.0f;
    ::c_rasterizer::set_pixel_shader_constant(2, 1, (real_vector4d const *)shader_constants);
    long surface_width = c_rasterizer::get_surface_width((c_rasterizer::e_surface)1);
    long surface_height = c_rasterizer::get_surface_height((c_rasterizer::e_surface)1);
    c_rasterizer::draw_fullscreen_quad(surface_height, surface_width);
    c_rasterizer::set_render_target(0, (c_rasterizer::e_surface)67);
    c_rasterizer::resolve_surface((c_rasterizer::e_surface)67, 0, 0, 0, 0);
}

// @address 0x730e4c ?m_current_marker_index@c_transparency_renderer@@0JA
long c_transparency_renderer::m_current_marker_index = -1;
// @address 0x449c10 ?render_dispose_from_old_map@@YAXXZ
void render_dispose_from_old_map()
{
    g_object_message_queue->clear();
    s_data_array *data = g_cached_object_render_states;
    if (data != 0 && data->valid)
    {
        data->valid = false;
        for (long index = 0; index < g_object_render_data_count; ++index)
            ((long *)g_object_render_data)[index] = -1;
    }
    c_transparency_renderer::transparent_sorted_order.clear();
    s_render_object_globals *objects = g_render_object_globals;
    objects->current_a = 0;
    objects->current_b = 0;
    objects->marker = -1;
    objects->active = false;
    c_transparency_renderer::m_current_marker_index = -1;
    c_transparency_renderer::m_total_transparent_count = 0;
    render_structure_globals &structure = g_render_structure_globals;
    structure.current_a = 0;
    structure.current_b = 0;
    structure.depth = -1;
    g_cluster_sorted_order.m_count = 0;
    structure.stack_b[5] = -1;
    structure.cached_definition_index = -1;
    structure.cached_instance_marker = -1;
}

// @address 0x49fa0 k_render_min_field_of_view
const float k_render_min_field_of_view[] = { 0.7592033f };
static __forceinline void render_set_vector4(real_vector4d *out, float x, float y, float z, float w)
{
    out->x = x; out->y = y; out->z = z; out->w = w;
}
static __forceinline void render_cross_product(const real_vector3d *a, const real_vector3d *b, real_vector3d *out)
{
    float i = a->j * b->k - a->k * b->j;
    float j = a->k * b->i - a->i * b->k;
    float k = a->i * b->j - a->j * b->i;
    out->i = i; out->j = j; out->k = k;
}
// @address 0x152328 ?render_setup_window@@YAXPBUrender_camera@@PBUrender_projection@@@Z
void render_setup_window(const render_camera *camera, const render_projection *projection)
{
    real_vector4d position, left, up, forward;
    float matrix[4][4];
    combine_projection_and_view_matrix(&projection->world_to_view, projection->projection_matrix, &matrix[0][0]);
    c_rasterizer::get_device()->SetVertexShaderConstantF(0, ((const real_vector4d *)matrix)->n, 4);
    render_set_vector4(&position, camera->position.x, camera->position.y, camera->position.z, 0.0f);
    render_set_vector4(&forward, camera->forward.i, camera->forward.j, camera->forward.k, 0.0f);
    render_set_vector4(&up, camera->up.i, camera->up.j, camera->up.k, 0.0f);
    render_cross_product(&up.xyz, &forward.xyz, &left.xyz);
    normalize3d(&left.xyz);
    left.w = 0.0f;
    c_rasterizer::get_device()->SetVertexShaderConstantF(4, forward.n, 1);
    c_rasterizer::get_device()->SetVertexShaderConstantF(5, left.n, 1);
    c_rasterizer::get_device()->SetVertexShaderConstantF(6, up.n, 1);
    c_rasterizer::get_device()->SetVertexShaderConstantF(7, position.n, 1);
    c_rasterizer::get_device()->SetPixelShaderConstantF(16, position.n, 1);
}

// @address 0x449d80 render_setup_fade_constants
extern "C" void render_setup_fade_constants(long user_index, float near_distance, float far_distance)
{
    const render_camera *camera = &g_rasterizer_player_views[0].render_camera_state;
    real_vector4d position, forward, up;
    real_vector3d left;
    render_set_vector4(&position, camera->position.x, camera->position.y, camera->position.z, 0.0f);
    render_set_vector4(&forward, camera->forward.i, camera->forward.j, camera->forward.k, 0.0f);
    render_set_vector4(&up, camera->up.i, camera->up.j, camera->up.k, 0.0f);
    render_cross_product(&up.xyz, &forward.xyz, &left);
    normalize3d(&left);
    float field_of_view = 
        camera->vertical_field_of_view > k_render_min_field_of_view[0] ? camera->vertical_field_of_view : k_render_min_field_of_view[0];
    float aspect = (float)(camera->fullscreen_pixel_bounds.x1 - camera->fullscreen_pixel_bounds.x0) /
        (float)(camera->fullscreen_pixel_bounds.y1 - camera->fullscreen_pixel_bounds.y0);
    float height = (float)tan((double)(field_of_view / 2.0f)) * far_distance;
    float width = height * aspect;
    float depth = far_distance - near_distance;
    float full_width = 2.0f * width;
    float full_height = 2.0f * height;
    real_vector3d offset = { near_distance, -width, -height };
    real_point3d origin;
    origin.x = position.x + forward.x * offset.i + left.i * offset.j + up.x * offset.k;
    origin.y = position.y + forward.y * offset.i + left.j * offset.j + up.y * offset.k;
    origin.z = position.z + forward.z * offset.i + left.k * offset.j + up.z * offset.k;
    real_vector4d texture_to_world[3], world_to_texture[3];
    render_set_vector4(&texture_to_world[0], forward.x * depth, left.i * full_width, up.x * full_height, origin.x);
    render_set_vector4(&texture_to_world[1], forward.y * depth, left.j * full_width, up.y * full_height, origin.y);
    render_set_vector4(&texture_to_world[2], forward.z * depth, left.k * full_width, up.z * full_height, origin.z);
    float inverse_depth = 1.0f / depth;
    render_set_vector4(&world_to_texture[0], forward.x *inverse_depth, forward.y *inverse_depth, forward.z *inverse_depth,
        -(origin.x *forward.x + origin.y * forward.y + origin.z * forward.z) * inverse_depth);
    float inverse_width = 1.0f / full_width;
    render_set_vector4(&world_to_texture[1], left.i *inverse_width, left.j *inverse_width, left.k *inverse_width,
        -(origin.x *left.i + origin.y * left.j + origin.z * left.k) * inverse_width);
    float inverse_height = 1.0f / full_height;
    render_set_vector4(&world_to_texture[2], up.x *inverse_height, up.y *inverse_height, up.z *inverse_height,
        -(origin.x *up.x + origin.y * up.y + origin.z * up.z) * inverse_height);
    c_rasterizer::get_device()->SetVertexShaderConstantF(26, world_to_texture[0].n, 3);
    c_rasterizer::get_device()->SetVertexShaderConstantF(23, texture_to_world[0].n, 3);
}

// @address 0x44a030 ?render_setup_occlusion_constants@@YAXJ@Z
void render_setup_occlusion_constants(long user_index)
{
    const render_projection *projection = &g_rasterizer_player_views[0].occlusion_projection;
    float matrix[16];
    real_vector4d inverse[3];
    float lengths[3];
    combine_projection_and_view_matrix(&projection->world_to_view, projection->projection_matrix, matrix);
    float *m = matrix;
    m[0] *= 0.5f;
    m[1] *= 0.5f;
    m[2] *= 0.5f;
    m[3] *= 0.5f;
    m[4] *= -0.5f;
    m[5] *= -0.5f;
    m[6] *= -0.5f;
    m[7] *= -0.5f;
    m[3] += 0.5;
    m[7] += 0.5;
    lengths[0] = m[0] * m[0] + m[4] * m[4] + m[8] * m[8];
    lengths[1] = (m[5] * m[5] + m[1] * m[1]) + m[9] * m[9];
    lengths[2] = (m[6] * m[6] + m[2] * m[2]) + m[10] * m[10];
    inverse[0].x = m[0] / lengths[0];
    inverse[0].y = m[4] / lengths[0];
    inverse[0].z = m[8] / lengths[0];
    inverse[0].w = -(m[3] * m[0] + m[7] * m[4] + m[11] * m[8]) / lengths[0];
    inverse[1].x = m[1] / lengths[1];
    inverse[1].y = m[5] / lengths[1];
    inverse[1].z = m[9] / lengths[1];
    inverse[1].w = -((m[7] * m[5] + m[3] * m[1]) + m[11] * m[9]) / lengths[1];
    inverse[2].x = m[2] / lengths[2];
    inverse[2].y = m[6] / lengths[2];
    inverse[2].z = m[10] / lengths[2];
    inverse[2].w = -((m[7] * m[6] + m[3] * m[2]) + m[11] * m[10]) / lengths[2];
    c_rasterizer::get_device()->SetVertexShaderConstantF(32, ((const real_vector4d *)m)->n, 3);
    c_rasterizer::get_device()->SetVertexShaderConstantF(29, inverse[0].n, 3);
}

// @address 0x8c4844 ?m_total_transparent_count@c_transparency_renderer@@0JA
long c_transparency_renderer::m_total_transparent_count;


// @address 0x3aa78 k_real_one_thousandth_storage
const float k_real_one_thousandth_storage = 0.001f;

