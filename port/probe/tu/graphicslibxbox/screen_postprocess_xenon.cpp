// graphicslibxbox/screen_postprocess_xenon.obj: from the records 0x182a38 0x184010 0x184298 0x184910 0x185600 0x185928 0x185b38 0x185d90 0x186058 0x186130 0x18f9d8 0x18fbb0 0x2a4750 0x2a4900 0x2a4a80 0x44a7f0 0x4560d8 0x4562c0 0x456368 0x456548 0x456670 0x456778
// @flags /TP /O2 /Oc
#define NOD3D
#include "memory/datum_index.h"
#include "render/camera_fx_globals_types.h"
#include "rasterizer/rasterizer_viewport_state.h"
#include "render/render_camera_types.h"
#include "math/real_math_types.h"
#include "render/render_lightmap_shadows_globals.h"
#include <string.h>
#include <xtl.h>
#include <d3d9.h>
#include "render/camera_fx_globals.h"
#include "main/main_screenshot_globals.h"
#include "render/screen_postprocess_xenon.h"
#include "rasterizer/xenon/rasterizer_xenon_submit.h"
#include "render/screen_postprocess_xenon_globals.h"
#include "main/main_render.h"
#include "render/shader_reference_types.h"
#include "cache/cache_files.h"
#include "rasterizer/rasterizer_hue_saturation.h"
#include "scenario/scenario.h"
#include "game/game_globals.h"
#include "rasterizer/rasterizer_tag_instance_types.h"
#include "rasterizer/rasterizer_textures_cache.h"
#include "rasterizer/rasterizer.h"
#include "render/screen_postprocess.h"
#include "rasterizer/rasterizer_global_definitions.h"
#include <math.h>
#include "rasterizer/rasterizer_render_target_types.h"
#include "render/camera_fx_values.h"
#include "render/views/player_view_types.h"
#include "render/screen_postprocess_settings.h"
#include "rasterizer/rasterizer_render_globals_types.h"
#include "render/depth_of_field_types.h"
#include "cseries/cseries.h"
#include "rasterizer/dx9/rasterizer_screenshot_types.h"
#include "math/real_math.h"
#include "math/matrix_math_types.h"
#include "main/main_render_types.h"
#include "math/xenon_matrix_types.h"
#include "rasterizer/xenon/xenon_matrix.h"
#include "cutscene/cinematic_globals_types.h"
#include "tag_files/tag_groups.h"
#include "interface/chud/chud.h"

struct s_range_source {
    long flags;
    float min;
    float max;
    float scale;
    float value;
};

struct s_postprocess_color_settings {
    float pad0;
    float hue_start;
    float hue_end;
    float saturation_end;
    float saturation_start;
    float brightness;
    float exposure_offset;
    float exposure_bias;
    real_rgb_color filter;
    real_rgb_color offset;
};

// @address 0x8c46dc ?x_current_player_view@c_player_view@@1PAV1@A
// @address 0x7e7cb0 g_water_composite_shading_constants
// @address 0x16fd28 rasterizer_set_alpha_test_enabled
// @address 0x7c6178 g_rasterizer_sampler_filter_modes
// @address 0x195e88 rasterizer_device_set_sampler_filter_modes
// @address 0x221fc k_postprocess_real_constants
// @address 0x8c4704 g_postprocess_exposure_offset
// @address 0x8368d0 g_player_render_tuning
// @address 0x8c46e4 g_rasterizer_distortion_visible
struct s_game_globals;
struct cache_file_tag_instance;

// @address 0x73762c ?x_editable_settings@c_screen_postprocess@@2Us_settings@1@A
c_screen_postprocess::s_settings c_screen_postprocess::x_editable_settings =
{
    true, 0, 1, 3, 0, 1, 0, false, 0, 1.0f, true, true, 1.0f, 0, 0
};

// @address 0x7374f8 g_motion_blur_max_view_count
long g_motion_blur_max_view_count = 1;

extern "C" {
// @address 0x10f5d30 g_main_render_default_postprocess_settings
// @address 0x70a1a8 ??__Eg_main_render_default_postprocess_settings@@YAXXZ
c_screen_postprocess::s_settings g_main_render_default_postprocess_settings = c_screen_postprocess::x_editable_settings;
}


// @address 0x185928 ?set_surface_as_texture_default_black@@YAXJW4e_surface@c_rasterizer@@@Z
void set_surface_as_texture_default_black(long sampler_index, c_rasterizer::e_surface surface)
{
    if (surface != c_rasterizer::_surface_none) {
        c_rasterizer::set_aliased_surface_as_texture(sampler_index, surface);
        return;
    }
    const c_rasterizer_globals *globals;
    long index = global_game_globals->rasterizer_globals_ref.index;
    if (index != -1)
        globals = (const c_rasterizer_globals *)global_tag_instances[datum_index_to_absolute_index(index)].definition;
    else
        globals = 0;
    c_rasterizer::set_sampler_texture(sampler_index, globals->get_default_texture_rasterizer_texture_ref(5));
}


// @address 0x4560d8 ?calculate_gaussian_kernel@@YAXPAMJMM@Z
void calculate_gaussian_kernel(float *kernel, long count, float mean, float sigma)
{
    if (sigma < 0.0001f) {
        memset(kernel, 0, count * sizeof(float));
        long index = (long)mean;
        index = (index > 0 ? index : 0) < count - 1 ? (index > 0 ? index : 0) : count - 1;
        kernel[index] = 1.0f;
    } else {
        float factor = (float)(1.0 / (sigma * sqrt(6.283185307179586)));
        float inverse_variance = 1.0f / (2.0f * (sigma * sigma));
        float sum = 0.0f;
        for (long index = 0; index < count; index++) {
            float distance = (float)index - mean;
            float weight = factor * (float)exp((double)(-(distance * distance * inverse_variance)));
            kernel[index] = weight;
            sum += weight;
        }
        float normalization = 1.0f / sum;
        for (long index = 0; index < count; index++)
            kernel[index] *= normalization;
    }
}


// @address 0x4562c0 ?calculate_taps_from_kernel@@YAXPAMPATreal_vector4d@@J_NM@Z
void calculate_taps_from_kernel(float *kernel, real_vector4d *taps, long count, bool horizontal, float offset)
{
    // match: 0x4562c0: value range assumption guides branch lowering
    __assume(count == 5);
    for (long i = 0; i < count; i++) {
        long kernel_index = i * 2;
        float weight = kernel[kernel_index] + kernel[kernel_index + 1];
        float position = weight > 0.0f ? (float)kernel_index - 4.0f - kernel[kernel_index] / weight : 0.5f;
        taps[i].i = horizontal ? position : offset;
        taps[i].j = horizontal ? offset : position;
        taps[i].k = weight;
        taps[i].l = 0.0f;
    }
}


// @address 0x184910 ?setup_rasterizer_for_postprocess@c_screen_postprocess@@SAX_N@Z
void c_screen_postprocess::setup_rasterizer_for_postprocess(bool p0)
{
    c_rasterizer::clear_sampler_textures(0xffff);
    if (p0) {
        c_rasterizer::set_render_target(0, c_rasterizer::_surface_none);
        c_rasterizer::set_render_target(1, c_rasterizer::_surface_none);
        c_rasterizer::set_render_target(2, c_rasterizer::_surface_none);
        c_rasterizer::set_render_target(3, c_rasterizer::_surface_none);
        c_rasterizer::set_depth_stencil_surface(c_rasterizer::_surface_none);
    }
    if (c_rasterizer::get_current_alpha_blend_mode() != c_rasterizer::_alpha_blend_opaque) {
        c_rasterizer::set_current_alpha_blend_mode(c_rasterizer::_alpha_blend_opaque);
        D3DDevice_SetRenderState_AlphaBlendEnable(c_rasterizer::get_device(), 0);
    }
    D3DDevice_SetRenderState_CullMode(c_rasterizer::get_device(), 0);
    c_rasterizer::set_scissor_rect(0);
    c_rasterizer::set_z_buffer_mode(c_rasterizer::_z_buffer_mode_off);
    D3DDevice_SetRenderState_ColorWriteEnable(c_rasterizer::get_device(), 15);
    rasterizer_set_alpha_test_enabled(0, 0);
}


static __forceinline c_render_surface *screen_postprocess_xenon_get_render_surface(long s)
{
    return g_rasterizer_surface_groups[s].get_specialized_surface((c_rasterizer::e_surface)s);
}
static __forceinline long get_surface_width_inline(long s)
{
    c_render_surface *surface = screen_postprocess_xenon_get_render_surface(s);
    return surface->description.width;
}
static __forceinline long get_surface_height_inline(long s)
{
    c_render_surface *surface = screen_postprocess_xenon_get_render_surface(s);
    return surface->description.height;
}
static __forceinline void set_pixel_shader_constant_inline(long start, long count, const real_vector4d *values)
{
    c_rasterizer::get_device()->SetPixelShaderConstantF(start, values->n, count);
}
static __forceinline void set_sampler_filter_mode_inline(long sampler, long mode)
{
    if (g_rasterizer_sampler_filter_modes[0] != mode) {
        g_rasterizer_sampler_filter_modes[0] = mode;
        if ((unsigned long)mode <= 7)
            rasterizer_device_set_sampler_filter_modes(c_rasterizer::get_device(), sampler, mode, mode);
    }
}

// @rdata 0x8204ae90..0x8204aee0
static const float k_postprocess_gaussian_blur_constants[20] =
{
    0.5f, -4.1f, 0.01953125f, 0.0f,
    0.5f, -2.3f, 0.234375f, 0.0f,
    0.5f, -0.5f, 0.4921875f, 0.0f,
    0.5f, 1.3f, 0.234375f, 0.0f,
    0.5f, 3.1f, 0.01953125f, 0.0f
};
// @rdata-end
// @address 0x185d90 ?copy@c_screen_postprocess@@SAXJW4e_surface@c_rasterizer@@0W4e_sampler_filter_mode@3@W4e_sampler_address_mode@3@MMMMPATreal_rectangle2d@@@Z
void c_screen_postprocess::copy(long shader, c_rasterizer::e_surface source, c_rasterizer::e_surface dest,
    c_rasterizer::e_sampler_filter_mode filter, c_rasterizer::e_sampler_address_mode address, float x, float y, float z, float w,
    real_rectangle2d *rect)
{
    c_rasterizer::set_aliased_surface_as_texture(0, (c_rasterizer::e_surface)source);
    c_rasterizer::set_sampler_address_mode(0, (c_rasterizer::e_sampler_address_mode)(address),
        (c_rasterizer::e_sampler_address_mode)(address), (c_rasterizer::e_sampler_address_mode)(address));
    set_sampler_filter_mode_inline(0, filter);
    c_rasterizer::set_render_target(0, (c_rasterizer::e_surface)dest);
    c_rasterizer::set_render_target(1, (c_rasterizer::e_surface)0);
    if (c_rasterizer::set_explicit_shaders(shader, _vertex_type_screen, _transfer_vector_vertex_none, _entry_point_default)) {
        real_vector4d texel;
        texel.i = (float)(1.0 / get_surface_width_inline(source));
        texel.j = (float)(1.0 / get_surface_height_inline(source));
        texel.k = texel.l = 0.0f;
        set_pixel_shader_constant_inline(1, 1, &texel);
        real_vector4d color = {x, y, z, w};
        set_pixel_shader_constant_inline(2, 1, &color);
        if (rect)
            c_rasterizer::draw_fullscreen_quad_with_texture_xform(
                get_surface_width_inline(dest), get_surface_height_inline(dest), rect);
        else
            c_rasterizer::draw_fullscreen_quad(get_surface_width_inline(dest),
                get_surface_height_inline(dest));
        c_rasterizer::set_sampler_texture(0, c_rasterizer_texture_ref());
    }
}
// @address 0x186058 ?gaussian_blur_fixed@c_screen_postprocess@@SAXW4e_surface@c_rasterizer@@0MMMM@Z
void c_screen_postprocess::gaussian_blur_fixed(c_rasterizer::e_surface surface, c_rasterizer::e_surface surface2, float x, float y,
    float z, float)
{
    c_screen_postprocess::copy(11, (c_rasterizer::e_surface)(surface), (c_rasterizer::e_surface)(surface2),
        (c_rasterizer::e_sampler_filter_mode)(2), (c_rasterizer::e_sampler_address_mode)(1), 1.0f, 1.0f, 1.0f, 1.0f, 0);
    c_rasterizer::resolve_surface(surface2, 0, 0, 0, 0);
    D3DDevice_SetPixelShaderConstantFN(c_rasterizer::get_device(), 3, k_postprocess_gaussian_blur_constants, 5,
        ((unsigned __int64)3) << 62);
    c_screen_postprocess::copy(34, (c_rasterizer::e_surface)(surface2), (c_rasterizer::e_surface)(surface),
        (c_rasterizer::e_sampler_filter_mode)(2), (c_rasterizer::e_sampler_address_mode)(1), x, y, z, 1.0f, 0);
}
// @address 0x18fbb0 ?blit@c_screen_postprocess@@SAXJW4e_surface@c_rasterizer@@0W4e_sampler_filter_mode@3@W4e_sampler_address_mode@3@MMMMPATreal_rectangle2d@@3@Z
void c_screen_postprocess::blit(long entry_index, c_rasterizer::e_surface source, c_rasterizer::e_surface dest,
    c_rasterizer::e_sampler_filter_mode filter, c_rasterizer::e_sampler_address_mode address, float x, float y, float z, float w,
    real_rectangle2d *source_texture_rect, real_rectangle2d *dest_texture_rect)
{
    c_rasterizer::set_aliased_surface_as_texture(0, source);
    c_rasterizer::set_sampler_address_mode(0, c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp,
        c_rasterizer::_sampler_address_clamp);
    set_sampler_filter_mode_inline(0, 1);
    c_rasterizer::set_render_target(0, (c_rasterizer::e_surface)1);
    if (c_rasterizer::set_explicit_shaders(entry_index, _vertex_type_screen, _transfer_vector_vertex_none, _entry_point_default)) {
        real_vector4d texel;
        texel.i = (float)(1.0 / get_surface_width_inline(source));
        texel.j = (float)(1.0 / get_surface_height_inline(source));
        texel.k = texel.l = 0.0f;
        set_pixel_shader_constant_inline(1, 1, &texel);
        real_vector4d color = {x, y, z, w};
        set_pixel_shader_constant_inline(2, 1, &color);
        c_rasterizer::draw_screen_quad_with_texture_transform(get_surface_width_inline(1),
            get_surface_height_inline(1), dest_texture_rect, source_texture_rect);
        c_rasterizer::set_sampler_texture(0, c_rasterizer_texture_ref());
    }
}
// @address 0x456368 ?gaussian_blur@c_screen_postprocess@@SAXW4e_surface@c_rasterizer@@0MM@Z
void c_screen_postprocess::gaussian_blur(c_rasterizer::e_surface surface, c_rasterizer::e_surface surface2, float sigma_x, float sigma_y)
{
    float variance_x = sigma_x * sigma_x;
    float variance_y = sigma_y * sigma_y;
    float kernel[10];
    real_vector4d taps[5];
    if (variance_x > 1.0f)
        calculate_gaussian_kernel(kernel, 10, 4.5f, sqrtf(variance_x - 0.25f));
    else {
        calculate_gaussian_kernel(kernel + 1, 9, 4.0f, sigma_x);
        kernel[0] = 0.0f;
    }
    calculate_taps_from_kernel(kernel, taps, 5, true, variance_y > 1.0f ? 0.5f : 0.0f);
    D3DDevice_SetPixelShaderConstantFN(c_rasterizer::get_device(), 3, (const float *)(taps), 5, ((unsigned __int64)3) << 62);
    c_screen_postprocess::copy(34, (c_rasterizer::e_surface)(surface), (c_rasterizer::e_surface)(surface2),
        (c_rasterizer::e_sampler_filter_mode)(2), (c_rasterizer::e_sampler_address_mode)(1), 1.0f, 1.0f, 1.0f, 1.0f, 0);
    c_rasterizer::resolve_surface(surface2, 0, 0, 0, 0);
    if (variance_y != variance_x) {
        if (variance_y > 1.0f)
            calculate_gaussian_kernel(kernel, 10, 4.5f, sqrtf(variance_y - 0.25f));
        else {
            calculate_gaussian_kernel(kernel + 1, 9, 4.0f, sigma_y);
            kernel[0] = 0.0f;
        }
    }
    calculate_taps_from_kernel(kernel, taps, 5, false, variance_x > 1.0f ? 0.5f : 0.0f);
    D3DDevice_SetPixelShaderConstantFN(c_rasterizer::get_device(), 3, (const float *)(taps), 5, ((unsigned __int64)3) << 62);
    c_screen_postprocess::copy(34, (c_rasterizer::e_surface)(surface2), (c_rasterizer::e_surface)(surface),
        (c_rasterizer::e_sampler_filter_mode)(2), (c_rasterizer::e_sampler_address_mode)(1), 1.0f, 1.0f, 1.0f, 1.0f, 0);
}
// @address 0x456548 screen_postprocess_apply_large_blur
extern "C" void screen_postprocess_apply_large_blur(float horizontal_sigma, float vertical_sigma)
{
    float horizontal_variance;
    float vertical_variance;
    float intensity;
    float next_vertical;
    float remaining_horizontal;
    float horizontal_applied;
    float pass_variance;
    float vertical_applied;
    float horizontal_pass;
    float vertical_pass;
    pass_variance = 0;
    horizontal_variance = (horizontal_sigma * horizontal_sigma);
    vertical_variance = (vertical_sigma * vertical_sigma);
#if defined(_XBOX) && !defined(__clang__)
    // match: 0x456548: 0.0f (+0x320), 2.5f (+0xa10) and 1.0f load off one 0x820221fc base held in r31
    // across the blur calls; spelled literals take a lis each and save one more fpr
    horizontal_applied = k_postprocess_real_constants[200];
    pass_variance = k_postprocess_real_constants[644];
#else
    horizontal_applied = 0.0f;
    pass_variance = 2.5f;
#endif
    vertical_applied = horizontal_applied;
    if (!((horizontal_variance <= pass_variance))) {
#if defined(_XBOX) && !defined(__clang__)
        intensity = k_postprocess_real_constants[0];
#else
        intensity = 1.0f;
#endif
        do {
            next_vertical = (vertical_applied + pass_variance);
            if ((next_vertical >= vertical_variance))
                break;
            c_screen_postprocess::gaussian_blur_fixed((c_rasterizer::e_surface)50, (c_rasterizer::e_surface)50, intensity, intensity,
                intensity, intensity);
            c_rasterizer::resolve_surface((c_rasterizer::e_surface)50, 0, 0, 0, 0);
#if defined(_XBOX) && !defined(__clang__)
            pass_variance = k_postprocess_real_constants[644];
#else
            pass_variance = 2.5f;
#endif
            horizontal_applied = (horizontal_applied + pass_variance);
            vertical_applied = next_vertical;
        } while (((horizontal_applied + pass_variance) < horizontal_variance));
    }
    while (true) {
        if (!((horizontal_applied < horizontal_variance))) {
            if ((vertical_applied >= vertical_variance))
                break;
        }
        remaining_horizontal = (horizontal_variance - horizontal_applied);
        if (!((remaining_horizontal >= pass_variance))) {
            horizontal_pass = remaining_horizontal;
        } else {
            horizontal_pass = pass_variance;
        }
        vertical_pass = (vertical_variance - vertical_applied);
        if (!((vertical_pass < pass_variance))) {
            vertical_pass = pass_variance;
        }
        c_screen_postprocess::gaussian_blur((c_rasterizer::e_surface)50, (c_rasterizer::e_surface)50, sqrtf(horizontal_pass),
            sqrtf(vertical_pass));
        c_rasterizer::resolve_surface((c_rasterizer::e_surface)50, 0, 0, 0, 0);
#if defined(_XBOX) && !defined(__clang__)
        pass_variance = k_postprocess_real_constants[644];
#else
        pass_variance = 2.5f;
#endif
        horizontal_applied = (horizontal_pass + horizontal_applied);
        vertical_applied = (vertical_pass + vertical_applied);
    }
    return;
}


// @address 0x8c4700 g_postprocess_frame_index
long g_postprocess_frame_index;


static __forceinline void screen_postprocess_xenon_set_sampler_filter_mode_1(long mode)
{
    if (g_rasterizer_sampler_filter_modes[1] != mode) {
        g_rasterizer_sampler_filter_modes[1] = mode;
        if ((unsigned long)mode <= 7)
            rasterizer_device_set_sampler_filter_modes(c_rasterizer::get_device(), 1, mode, mode);
    }
}
// @address 0x185b38 screen_postprocess_prepare_bloom_surfaces
extern "C" long screen_postprocess_prepare_bloom_surfaces(long source, c_camera_fx_values *fx, long bloom, long blur)
{
    long result = 0;
    if (c_screen_postprocess::x_editable_settings.debug && c_screen_postprocess::x_editable_settings.display == 1)
        result = source;
    else {
        c_screen_postprocess::copy(49, (c_rasterizer::e_surface)(source), (c_rasterizer::e_surface)(bloom),
            (c_rasterizer::e_sampler_filter_mode)(1), (c_rasterizer::e_sampler_address_mode)(1), 1.0f, 1.0f, 1.0f, 1.0f, 0);
        c_rasterizer::resolve_surface((c_rasterizer::e_surface)bloom, 0, 0, 0, 0);
        if (c_screen_postprocess::x_editable_settings.debug && c_screen_postprocess::x_editable_settings.display == 2)
            result = bloom;
        else {
            c_screen_postprocess::copy(49, (c_rasterizer::e_surface)(bloom), (c_rasterizer::e_surface)(blur),
                (c_rasterizer::e_sampler_filter_mode)(1), (c_rasterizer::e_sampler_address_mode)(1), 1.0f, 1.0f, 1.0f, 1.0f, 0);
            c_rasterizer::resolve_surface((c_rasterizer::e_surface)blur, 0, 0, 0, 0);
            if (c_screen_postprocess::x_editable_settings.debug && c_screen_postprocess::x_editable_settings.display > 2)
                result = blur;
            else {
                fx->surface = fx->index0;
                fx->index0 = g_postprocess_frame_index;
                g_postprocess_frame_index++;
                if (g_postprocess_frame_index >= 8)
                    g_postprocess_frame_index = 0;
                c_rasterizer_globals *globals = global_game_globals->rasterizer_globals_ref.index != -1
                    ? (c_rasterizer_globals *)tag_get('rasg', global_game_globals->rasterizer_globals_ref.index)
                    : 0;
                c_rasterizer_texture_ref texture;
                if (globals->m_default_textures_refs.count() > 7)
                    texture = globals->get_default_texture_rasterizer_texture_ref(7);
                else
                    texture = globals->get_default_texture_rasterizer_texture_ref(0);
                c_rasterizer::set_sampler_texture(1, texture);
                c_rasterizer::set_sampler_address_mode(
                    1, c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp);
                screen_postprocess_xenon_set_sampler_filter_mode_1(2);
                c_screen_postprocess::copy(35, (c_rasterizer::e_surface)(blur), (c_rasterizer::e_surface)(fx->index0 + 42),
                    (c_rasterizer::e_sampler_filter_mode)(2), (c_rasterizer::e_sampler_address_mode)(1), fx->exposure_sensitivity,
                    1.0f, 1.0f, 1.0f, 0);
                c_rasterizer::resolve_surface((c_rasterizer::e_surface)(fx->index0 + 42), 0, 0, 0, 0);
            }
        }
    }
    return result;
}
// @address 0x185600 ?apply_binary_op_ex@c_screen_postprocess@@SAXJW4e_surface@c_rasterizer@@00W4e_sampler_filter_mode@3@W4e_sampler_address_mode@3@MMMMPATreal_rectangle2d@@@Z
void c_screen_postprocess::apply_binary_op_ex(long entry_index, c_rasterizer::e_surface source, c_rasterizer::e_surface source2,
    c_rasterizer::e_surface dest, c_rasterizer::e_sampler_filter_mode filter, c_rasterizer::e_sampler_address_mode address, float x,
    float y, float z, float w, real_rectangle2d *rect)
{
    set_surface_as_texture_default_black(0, source);
    c_rasterizer::set_sampler_address_mode(0, (c_rasterizer::e_sampler_address_mode)(address),
        (c_rasterizer::e_sampler_address_mode)(address), (c_rasterizer::e_sampler_address_mode)(address));
    set_sampler_filter_mode_inline(0, filter);
    set_surface_as_texture_default_black(1, source2);
    c_rasterizer::set_sampler_address_mode(1, (c_rasterizer::e_sampler_address_mode)(address),
        (c_rasterizer::e_sampler_address_mode)(address), (c_rasterizer::e_sampler_address_mode)(address));
    screen_postprocess_xenon_set_sampler_filter_mode_1(filter);
    c_rasterizer::set_render_target(0, dest);
    if (c_rasterizer::set_explicit_shaders(entry_index, _vertex_type_screen, _transfer_vector_vertex_none, _entry_point_default)) {
        real_vector4d texel;
        texel.i = (float)(1.0 / get_surface_width_inline(source));
        texel.j = (float)(1.0 / get_surface_height_inline(source));
        texel.k = texel.l = 0.0f;
        set_pixel_shader_constant_inline(1, 1, &texel);
        real_vector4d color = {x, y, z, w};
        set_pixel_shader_constant_inline(2, 1, &color);
        if (rect)
            c_rasterizer::draw_fullscreen_quad_with_texture_xform(
                get_surface_width_inline(dest), get_surface_height_inline(dest), rect);
        else
            c_rasterizer::draw_fullscreen_quad(get_surface_width_inline(dest),
                get_surface_height_inline(dest));
        c_rasterizer::set_sampler_texture(0, c_rasterizer_texture_ref());
        c_rasterizer::set_sampler_texture(1, c_rasterizer_texture_ref());
    }
}
// @address 0x456670 ?blur_display@c_screen_postprocess@@SA?AW4e_surface@c_rasterizer@@MM@Z
c_rasterizer::e_surface c_screen_postprocess::blur_display(float sigma_x, float sigma_y)
{
    if (c_rasterizer::get_render_target(0) == (c_rasterizer::e_surface)1) {
        long surface = 20;
        if (c_player_view::get_current() != 0) {
            long view_surface = c_player_view::get_current()->window_type;
            if (view_surface != 0)
                surface = view_surface;
        }
        c_rasterizer::resolve_surface((c_rasterizer::e_surface)surface, 0, 0, 0, 0);
#if !defined(_XBOX) || defined(__clang__)
        // On Xbox 360, 2_10_10_10_FLOAT's 2-bit UNORM alpha is not scaled by resolve_exp_bias (-5),
        // whereas our float16 resolve_scale scales all 4 channels by 2^-5 (crushing UI alpha to ~6/255).
        // Seed surface 51's alpha to 1.0 and mask alpha writes during the initial downsample so every
        // subsequent normalized gaussian_blur pass preserves alpha=1.0 on both Vulkan and D3D12.
        c_rasterizer::set_render_target(0, (c_rasterizer::e_surface)51);
        c_rasterizer::get_device()->Clear(0, 0, D3DCLEAR_TARGET, 0xffffffff, 1.0f, 0);
        DWORD saved_cwe = 0xf;
        c_rasterizer::get_device()->GetRenderState(D3DRS_COLORWRITEENABLE, &saved_cwe);
        c_rasterizer::get_device()->SetRenderState(D3DRS_COLORWRITEENABLE, 0x7);
#endif
        c_screen_postprocess::copy(39, (c_rasterizer::e_surface)(surface), (c_rasterizer::e_surface)(51),
            (c_rasterizer::e_sampler_filter_mode)(2), (c_rasterizer::e_sampler_address_mode)(1), 1.0f, 1.0f, 1.0f, 1.0f, 0);
#if !defined(_XBOX) || defined(__clang__)
        c_rasterizer::get_device()->SetRenderState(D3DRS_COLORWRITEENABLE, saved_cwe);
#endif
        c_rasterizer::resolve_surface((c_rasterizer::e_surface)51, 0, 0, 0, 0);
        c_screen_postprocess::gaussian_blur((c_rasterizer::e_surface)51, (c_rasterizer::e_surface)51, sigma_x, sigma_y);
        c_rasterizer::resolve_surface((c_rasterizer::e_surface)51, 0, 0, 0, 0);
#if !defined(_XBOX) || defined(__clang__)
        // Custom resolutions enlarge surface 51 (480x270 at 1080p, 960x540 at 4K vs 288x160 native);
        // extra kernel_5 passes keep the screen-space blur radius matching retail Xbox 360.
        const long w51 = get_surface_width_inline((c_rasterizer::e_surface)51);
        const long extra_passes = w51 >= 800 ? 2 : (w51 >= 440 ? 1 : 0);
        for (long p = 0; p < extra_passes; ++p) {
            c_screen_postprocess::gaussian_blur((c_rasterizer::e_surface)51, (c_rasterizer::e_surface)51, sigma_x, sigma_y);
            c_rasterizer::resolve_surface((c_rasterizer::e_surface)51, 0, 0, 0, 0);
        }
#endif
    }
    return (c_rasterizer::e_surface)51;
}
// @address 0x2a4900 screen_postprocess_downsample_surface
extern "C" void screen_postprocess_downsample_surface(float *width_scale, float *height_scale, float width, float height, long surface)
{
    float width_limit = width * 1.5f;
    long width_active = *width_scale > width_limit;
    float height_limit = height * 1.5f;
    long height_active = *height_scale > height_limit;
    for (;;) {
        if (!width_active && !height_active)
            return;
        float w;
        if (height_active != 0)
            w = -0.5f;
        else
            w = 0.0f;
        float z;
        if (width_active != 0)
            z = -0.5f;
        else
            z = 0.0f;
        float y;
        if (height_active != 0)
            y = 2.0f;
        else
            y = 1.0f;
        float x;
        if (width_active != 0)
            x = 2.0f;
        else
            x = 1.0f;
        c_screen_postprocess::copy(29, (c_rasterizer::e_surface)(surface), (c_rasterizer::e_surface)(surface),
            (c_rasterizer::e_sampler_filter_mode)(2), (c_rasterizer::e_sampler_address_mode)(3), x, y, z, w, 0);
        c_rasterizer::resolve_surface((c_rasterizer::e_surface)surface, 0, 0, 0, 0);
        float width_factor;
        if (width_active != 0)
            width_factor = 0.5f;
        else
            width_factor = 1.0f;
        float scaled_width = *width_scale * width_factor;
        *width_scale = scaled_width;
        float height_factor;
        if (height_active != 0)
            height_factor = 0.5f;
        else
            height_factor = 1.0f;
        float scaled_height = *height_scale * height_factor;
        *height_scale = scaled_height;
        width_active = scaled_width > width_limit;
        height_active = scaled_height > height_limit;
    }
}

// @address 0x2a4750 screen_postprocess_blit_scaled_surface
extern "C" void screen_postprocess_blit_scaled_surface(long surface, long surface2, long width, long height, long shader)
{
    if (width == c_rasterizer::render_globals.width && height == c_rasterizer::render_globals.height && surface == surface2)
        return;
    float scale_x = (float)(c_rasterizer::render_globals.width - width) * 0.5f / (float)c_rasterizer::render_globals.width;
    float scale_y = (float)(c_rasterizer::render_globals.height - height) * 0.5f / (float)c_rasterizer::render_globals.height;
    real_vector4d tex_xform;
    tex_xform.i = scale_x;
    tex_xform.j = scale_y;
    tex_xform.k = 1.0f - scale_x;
    tex_xform.l = 1.0f - scale_y;
    c_rasterizer::get_device()->SetPixelShaderConstantF(4, tex_xform.n, 1);
    real_vector4d white;
    white.l = 0.0f;
    white.k = 0.0f;
    white.j = 1.0f;
    white.i = 1.0f;
    c_rasterizer::get_device()->SetPixelShaderConstantF(3, white.n, 1);
    c_screen_postprocess::copy(shader, (c_rasterizer::e_surface)(surface), (c_rasterizer::e_surface)(surface2),
        (c_rasterizer::e_sampler_filter_mode)(1), (c_rasterizer::e_sampler_address_mode)(1), 1.0f, 1.0f, 1.0f, 1.0f, 0);
    c_rasterizer::resolve_surface((c_rasterizer::e_surface)surface2, 0, 0, 0, 0);
}
// @address 0x456778 ?postprocess_bloom_buffer@c_screen_postprocess@@SA?AW4e_surface@c_rasterizer@@W423@AAVc_camera_fx_values@@W4e_multires@3@@Z
c_rasterizer::e_surface c_screen_postprocess::postprocess_bloom_buffer(c_rasterizer::e_surface source, c_camera_fx_values &fx_values,
    c_rasterizer::e_multires multires)
{
    c_camera_fx_values *fx = &fx_values;
    int result = 0;
    int bloom = multires + 34;
    int blur = multires + 38;
    result = screen_postprocess_prepare_bloom_surfaces(source, (c_camera_fx_values *)fx, bloom, blur);
    if (c_screen_postprocess::x_editable_settings.debug && result)
        return (c_rasterizer::e_surface)result;
    float intensity;
    if (c_screen_postprocess::x_editable_settings.bloom_mode)
        intensity = fx->bloom_brightness;
    else
        intensity = 0.0f;
    c_screen_postprocess::gaussian_blur_fixed((c_rasterizer::e_surface)blur, (c_rasterizer::e_surface)blur,
        fx->color_balance.red * intensity,
        fx->color_balance.green * intensity, fx->color_balance.blue * intensity, 1.0f);
    c_rasterizer::resolve_surface((c_rasterizer::e_surface)blur, 0, 0, 0, 0);
    if (c_screen_postprocess::x_editable_settings.debug && c_screen_postprocess::x_editable_settings.bloom_mode == 5)
        return (c_rasterizer::e_surface)blur;
    c_screen_postprocess::apply_binary_op_ex(66, (c_rasterizer::e_surface)bloom, (c_rasterizer::e_surface)blur,
        (c_rasterizer::e_surface)bloom, c_rasterizer::_sampler_filter_mode_bilinear, c_rasterizer::_sampler_address_clamp,
        fx->color_contrast.red * intensity, fx->color_contrast.green * intensity, fx->color_contrast.blue * intensity, 1.0f, 0);
    c_rasterizer::resolve_surface((c_rasterizer::e_surface)bloom, 0, 0, 0, 0);
    if (!(c_screen_postprocess::x_editable_settings.debug && c_screen_postprocess::x_editable_settings.bloom_mode == 6))
    {
        c_screen_postprocess::gaussian_blur_fixed((c_rasterizer::e_surface)bloom, (c_rasterizer::e_surface)bloom, 1.0f, 1.0f, 1.0f, 1.0f);
        c_rasterizer::resolve_surface((c_rasterizer::e_surface)bloom, 0, 0, 0, 0);
        if (!(c_screen_postprocess::x_editable_settings.debug && c_screen_postprocess::x_editable_settings.bloom_mode == 2))
        {
            c_screen_postprocess::apply_binary_op_ex(10, (c_rasterizer::e_surface)source, (c_rasterizer::e_surface)bloom,
                (c_rasterizer::e_surface)source, c_rasterizer::_sampler_filter_mode_bilinear, c_rasterizer::_sampler_address_clamp,
                fx->color_saturation.red * intensity, fx->color_saturation.green * intensity, fx->color_saturation.blue * intensity, 1.0f, 0);
            c_rasterizer::resolve_surface((c_rasterizer::e_surface)source, 0, 0, 0, 0);
            c_screen_postprocess::gaussian_blur_fixed((c_rasterizer::e_surface)source, (c_rasterizer::e_surface)source, 1.0f, 1.0f, 1.0f, 1.0f);
            c_rasterizer::resolve_surface((c_rasterizer::e_surface)source, 0, 0, 0, 0);
            if (c_screen_postprocess::x_editable_settings.debug && c_screen_postprocess::x_editable_settings.bloom_mode > 1
                && c_screen_postprocess::x_editable_settings.bloom_mode != 7)
                return source;
            return (c_rasterizer::e_surface)result;
        }
    }
    return (c_rasterizer::e_surface)bloom;
}
// @address 0x44a7f0 screen_postprocess_compute_focus_range
extern "C" bool screen_postprocess_compute_focus_range(const s_depth_of_field *defaults, const s_range_source *source, float *center,
    // match: 0x44a7f0: pointer __restrict qualifier prevents alias reload
    float *inv_scale, float *half_width, float *__restrict value)
{
    bool has_source = source != NULL && (source->flags & 1);
    if (has_source) {
        *center = (source->min + source->max) * 0.005f;
        double raw_scale = source->scale * 0.01f;
        double scale = raw_scale > 0.001 ? raw_scale : 0.001;
        *inv_scale = (float)(1.0 / scale);
        *half_width = (float)fabs(source->max - source->min) * 0.005f;
        *value = source->value * 10.124f;
    } else {
        *center = (defaults->g + defaults->h) * 0.5f;
        *inv_scale = defaults->i;
        *half_width = (defaults->h - defaults->g) * 0.5f;
        *value = defaults->focus_distance;
    }
    unsigned char enabled = defaults->enabled;
    return enabled != 0 || has_source;
}

// @address 0x2a4a80 screen_postprocess_render_flare
extern "C" void screen_postprocess_render_flare(const rasterizer_dx9_dynamic_geometry_screenshot_parameters *view, long x, long y)
{
    c_screen_postprocess::setup_rasterizer_for_postprocess(false);
    c_rasterizer::g_HDR_alpha_blend = false;
    screen_postprocess_blit_scaled_surface(14, 14, view->dimensions[2], view->dimensions[3], 40);
    c_rasterizer::g_HDR_alpha_blend = false;
    c_screen_postprocess::apply_binary_op_ex(5, (c_rasterizer::e_surface)0, (c_rasterizer::e_surface)14,
        (c_rasterizer::e_surface)21, c_rasterizer::_sampler_filter_mode_point, c_rasterizer::_sampler_address_clamp,
        view->camera_fx->bloom_intensity, view->camera_fx->bloom_threshold, 1.0f, 1.0f, 0);
    c_rasterizer::resolve_surface((c_rasterizer::e_surface)21, 0, 0, 0, 0);
    float width_scale = (float)view->dimensions[2] * 0.25f;
    float height_scale = (float)view->dimensions[3] * 0.25f;
    float target_width = ((float)view->dimensions[2] / (float)view->dimensions[0]) * (float)c_rasterizer::render_globals.width * 0.25f;
    float target_height = ((float)view->dimensions[3] / (float)view->dimensions[1]) * (float)c_rasterizer::render_globals.height * 0.25f;
    screen_postprocess_downsample_surface(&width_scale, &height_scale, target_width, target_height, 21);
    float width_ratio = width_scale / target_width;
    float height_ratio = height_scale / target_height;
    real_vector4d offset;
    offset.i = 0.5f - (((float)x + 0.5f) * (float)view->dimensions[2]) * width_ratio / (float)view->dimensions[0];
    offset.j = 0.5f - (((float)y + 0.5f) * (float)view->dimensions[3]) * height_ratio / (float)view->dimensions[1];
    offset.k = 0.0f;
    offset.l = 0.0f;
    set_pixel_shader_constant_inline(3, 1, &offset);
    c_screen_postprocess::apply_binary_op_ex(30, (c_rasterizer::e_surface)21, (c_rasterizer::e_surface)30,
        (c_rasterizer::e_surface)30, c_rasterizer::_sampler_filter_mode_bilinear, c_rasterizer::_sampler_address_border,
        width_ratio, 0.0f, 0.0f, height_ratio, 0);
    c_rasterizer::resolve_surface((c_rasterizer::e_surface)30, 0, 0, 0, 0);
}


static __forceinline bool screen_postprocess_xenon_get_is_tiling_bracket_active() { return (D3DDevice_GetDeviceState(c_rasterizer::get_device()) & 1) != 0; }
static __forceinline c_rasterizer::e_surface screen_postprocess_xenon_get_surface_accum(long surface)
{
    c_rasterizer::g_HDR_alpha_blend = false;
    return (c_rasterizer::e_surface)surface;
}
// @address 0x184010 ?resolve_frame_buffers@c_screen_postprocess@@SAXPBTrectangle2d@@@Z
void c_screen_postprocess::resolve_frame_buffers(const rectangle2d *rectangle)
{
    if (rectangle) {
        if (screen_postprocess_xenon_get_is_tiling_bracket_active())
            c_rasterizer::resolve_surface_tiled(screen_postprocess_xenon_get_surface_accum(13), 0, false, false);
        else
            c_rasterizer::resolve_surface(screen_postprocess_xenon_get_surface_accum(13), 0, rectangle, rectangle->x0, rectangle->y0);
    } else {
        if (screen_postprocess_xenon_get_is_tiling_bracket_active())
            c_rasterizer::resolve_surface_tiled(screen_postprocess_xenon_get_surface_accum(13), 0, false, false);
        else
            c_rasterizer::resolve_surface(screen_postprocess_xenon_get_surface_accum(13), 0, 0, 0, 0);
    }
    if (c_screen_postprocess::x_editable_settings.resolve_hdr) {
        if (rectangle) {
            if (screen_postprocess_xenon_get_is_tiling_bracket_active())
                c_rasterizer::resolve_surface_tiled(screen_postprocess_xenon_get_surface_accum(14), 1, false, false);
            else
                c_rasterizer::resolve_surface(screen_postprocess_xenon_get_surface_accum(14), 1, rectangle, rectangle->x0, rectangle->y0);
        } else {
            if (screen_postprocess_xenon_get_is_tiling_bracket_active())
                c_rasterizer::resolve_surface_tiled(screen_postprocess_xenon_get_surface_accum(14), 1, false, false);
            else
                c_rasterizer::resolve_surface(screen_postprocess_xenon_get_surface_accum(14), 1, 0, 0, 0);
        }
    }
    c_rasterizer::set_viewport(g_rasterizer_last_rectangles.viewport, 0.0f, 1.0f);
    c_rasterizer::set_scissor_rect(&g_rasterizer_last_rectangles.scissor);
}


static __forceinline void screen_postprocess_xenon_composite_sampler_mode(long index, long mode)
{
    long *modes = g_rasterizer_sampler_filter_modes;
    if (modes[index] != mode) {
        modes[index] = mode;
        rasterizer_device_set_sampler_filter_modes(c_rasterizer::get_device(), index, mode, mode);
    }
}
// @address 0x186130 screen_postprocess_render_composite
extern "C" bool screen_postprocess_render_composite(long shader, c_rasterizer::e_surface target, c_rasterizer::e_surface source,
    float brightness, bool distortion, long unused, bool depth_effect, float near_depth, float far_depth,
    const rectangle2d *scissor, const real_rectangle2d *bounds, const s_range_source *range)
{
    c_rasterizer::set_render_target(0, target);
    if (c_screen_postprocess::x_editable_settings.composite_mask != 1)
        shader = 75;
    long globals_index = global_game_globals->rasterizer_globals_ref.index;
    c_rasterizer_globals *globals;
    if (globals_index != -1)
        globals = (c_rasterizer_globals *)global_tag_instances[datum_index_to_absolute_index(globals_index)].definition;
    else
        globals = 0;
    if (c_rasterizer::set_explicit_shaders(shader, _vertex_type_screen, _transfer_vector_vertex_none, _entry_point_default)) {
    c_rasterizer::set_scissor_rect(scissor);
    if (!c_screen_postprocess::x_editable_settings.primary) {
        screen_postprocess_xenon_composite_sampler_mode(0, 1);
        screen_postprocess_xenon_composite_sampler_mode(1, 1);
    } else {
        screen_postprocess_xenon_composite_sampler_mode(0, 2);
        screen_postprocess_xenon_composite_sampler_mode(1, 2);
    }
    if (c_screen_postprocess::x_editable_settings.composite_mask & 1)
        c_rasterizer::set_aliased_surface_as_texture(0, screen_postprocess_xenon_get_surface_accum(13));
    else
        c_rasterizer::set_sampler_texture(0, globals->get_default_texture_rasterizer_texture_ref(5));
    if (c_screen_postprocess::x_editable_settings.composite_mask & 2)
        c_rasterizer::set_aliased_surface_as_texture(1, screen_postprocess_xenon_get_surface_accum(14));
    else
        c_rasterizer::set_sampler_texture(1, globals->get_default_texture_rasterizer_texture_ref(5));
    set_surface_as_texture_default_black(2, source);
    c_rasterizer::set_sampler_address_mode(2, c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp,
        c_rasterizer::_sampler_address_clamp);
    screen_postprocess_xenon_composite_sampler_mode(2, 2);
    real_vector4d texel;
    texel.i = (float)(1.0 / get_surface_width_inline(26));
    texel.j = (float)(1.0 / get_surface_height_inline(26));
    texel.l = 0.0f;
    texel.k = 0.0f;
    if (g_chud_text_render_target_dirty) {
        c_rasterizer::set_aliased_surface_as_texture(5, (c_rasterizer::e_surface)58);
        set_pixel_shader_constant_inline(4, 1, &g_water_composite_shading_constants);
    }
    float amount;
    real_vector4d depth_parameters;
    real_vector4d focus;
    if (depth_effect && screen_postprocess_compute_focus_range(g_depth_of_field_game_globals, range, &depth_parameters.k,
        &depth_parameters.l, &focus.i, &amount)) {
        c_rasterizer::set_aliased_surface_as_texture(3, (c_rasterizer::e_surface)3);
        c_rasterizer::set_sampler_address_mode(
            3, c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp);
        screen_postprocess_xenon_composite_sampler_mode(3, 1);
        c_rasterizer::set_aliased_surface_as_texture(4, (c_rasterizer::e_surface)50);
        c_rasterizer::set_sampler_address_mode(
            4, c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp);
        screen_postprocess_xenon_composite_sampler_mode(4, 2);
        c_render_surface *depth = screen_postprocess_xenon_get_render_surface(3);
        if (depth->description.resolve_format == 0x1a220197) {
            depth_parameters.j = (far_depth - near_depth) / (far_depth * near_depth);
            depth_parameters.i = 1.0f / far_depth;
        } else {
            depth_parameters.j = -(far_depth - near_depth) / (far_depth * near_depth);
            depth_parameters.i = 1.0f / near_depth;
        }
        set_pixel_shader_constant_inline(6, 1, &depth_parameters);
        focus.j = sqrtf(amount / sqrtf(2.5) < 1.0f ? amount / sqrtf(2.5) : 1.0f);
        focus.l = 0.0f;
        focus.k = 0.0f;
        set_pixel_shader_constant_inline(7, 1, &focus);
        texel.k = (float)(1.0 / get_surface_width_inline(13));
        texel.l = (float)(1.0 / get_surface_height_inline(13));
    }
    set_pixel_shader_constant_inline(1, 1, &texel);
    real_vector4d color = {1.0f, 1.0f, brightness, 0.0f};
    set_pixel_shader_constant_inline(2, 1, &color);
    real_vector4d distortion_parameters;
    if (distortion) {
        float scale = 1.4938016f / c_screen_postprocess::x_editable_settings.exposure;
        distortion_parameters.i = 1.4938016f / scale;
        distortion_parameters.j = scale * 1.0041494f;
        distortion_parameters.k = (scale * scale) * 0.0f;
        distortion_parameters.l = ((scale * scale) * scale) * -0.15f;
    } else {
        distortion_parameters.i = 65535.0f;
        distortion_parameters.j = 1.4938016f / c_screen_postprocess::x_editable_settings.exposure;
        distortion_parameters.k = 0.0f;
        distortion_parameters.l = 0.0f;
    }
    set_pixel_shader_constant_inline(3, 1, &distortion_parameters);
    real_vector4d transform = {1.0f, 1.0f, 0.0f, 0.0f};
    if (bounds) {
        transform.i = 1.0f / (bounds->x1 - bounds->x0);
        transform.j = 1.0f / (bounds->y1 - bounds->y0);
        transform.k = -(bounds->x0 * transform.i);
        transform.l = -(bounds->y0 * transform.j);
    }
    set_pixel_shader_constant_inline(5, 1, &transform);
    c_rasterizer::draw_fullscreen_quad(get_surface_width_inline(target),
        get_surface_height_inline(target));
    c_rasterizer::set_sampler_texture(0, c_rasterizer_texture_ref());
    c_rasterizer::set_sampler_texture(1, c_rasterizer_texture_ref());
    c_rasterizer::set_sampler_texture(2, c_rasterizer_texture_ref());
    c_rasterizer::set_sampler_texture(3, c_rasterizer_texture_ref());
    c_rasterizer::set_sampler_texture(4, c_rasterizer_texture_ref());
    g_render_method_submit_cache = k_default_render_method_submit_cache;
    D3DDevice_SetPixelShader(c_rasterizer::get_device(), 0);
    g_render_method_submit_cache = k_default_render_method_submit_cache;
    D3DDevice_SetVertexShader(c_rasterizer::get_device(), 0);
    D3DDevice_SetRenderState_ScissorTestEnable(c_rasterizer::get_device(), 0);
    return true;
    }
    return false;
}


static __forceinline float screen_postprocess_xenon_get_display_exposure_stops(const c_camera_fx_values *fx)
{
    s_camera_fx_globals *globals = camera_fx_globals;
    return (1.0f - globals->current_b) * fx->current + globals->current_a * globals->current_b + globals->exposure_offset +
        fx->exposure_boost + g_postprocess_exposure_offset;
}
// @address 0x184298 screen_postprocess_render_view
extern "C" void screen_postprocess_render_view(c_camera_fx_values *fx, const render_camera *camera,
    const s_postprocess_color_settings *color, long multires, const s_range_source *focus)
{
    c_screen_postprocess::setup_rasterizer_for_postprocess(true);
    c_rasterizer::end_high_quality_blend();
    rasterizer_set_hue_saturation_constants(g_screenshot_hue_saturation, color->hue_end - color->hue_start,
        color->saturation_end - color->saturation_start, color->brightness, &color->filter, &color->offset);
    float exposure_scale = PIN(color->exposure_offset + 1.0f - color->exposure_bias, 0.01f, 10.0f);
    real_vector4d exposure_constant;
    exposure_constant.i = exposure_constant.j = exposure_constant.k = exposure_constant.l = exposure_scale;
    set_pixel_shader_constant_inline(232, 1, &exposure_constant);
    bool depth_of_field = g_depth_of_field_game_globals->enabled || (focus && (focus->flags & 1));
    real_rectangle2d bounds;
    bounds.x0 = 0.0f;
    bounds.x1 = 1.0f;
    bounds.y0 = 0.0f;
    bounds.y1 = 1.0f;
    c_rasterizer::e_surface surface = (c_rasterizer::e_surface)(multires + 30);
    c_rasterizer::set_depth_stencil_surface(c_rasterizer::_surface_none);
    c_rasterizer::e_surface result;
    if (!c_screen_postprocess::x_editable_settings.resolve_hdr)
    {
        result = screen_postprocess_xenon_get_surface_accum(13);
    }
    else if (c_screen_postprocess::x_editable_settings.final_surface_mode != 0)
    {
        c_rasterizer::e_surface debug_surface = c_screen_postprocess::x_editable_settings.final_surface_mode == 1
            ? screen_postprocess_xenon_get_surface_accum(13)
                                                                                     : screen_postprocess_xenon_get_surface_accum(14);
        real_vector4d unit_constant;
        unit_constant.i = unit_constant.j = unit_constant.k = 0.0f;
        unit_constant.l = 1.0f;
        set_pixel_shader_constant_inline(3, 1, &unit_constant);
        set_pixel_shader_constant_inline(4, 1, &unit_constant);
        set_pixel_shader_constant_inline(5, 1, &unit_constant);
        set_pixel_shader_constant_inline(6, 1, &unit_constant);
        c_screen_postprocess::copy(50, (c_rasterizer::e_surface)(debug_surface), (c_rasterizer::e_surface)(debug_surface),
            (c_rasterizer::e_sampler_filter_mode)(1), (c_rasterizer::e_sampler_address_mode)(1), 1.0f, 1.0f, 1.0f, 1.0f, 0);
        c_rasterizer::resolve_surface(debug_surface, 0, 0, 0, 0);
        result = debug_surface;
    }
    else
    {
        float exposure = (float)pow(2.0f, screen_postprocess_xenon_get_display_exposure_stops(fx) * fx->white_point);
        exposure = MAX(exposure, 1.0f);
        exposure = exposure * c_screen_postprocess::x_editable_settings.exposure * 0.66943294f;
        real_vector4d luminance;
        luminance.i = 0.299f;
        luminance.j = 0.587f;
        luminance.k = 0.114f;
        luminance.l = 0.0f;
        set_pixel_shader_constant_inline(3, 1, &luminance);
        if (c_screen_postprocess::x_editable_settings.final_flags & 2)
        {
            c_screen_postprocess::apply_binary_op_ex(48, screen_postprocess_xenon_get_surface_accum(14), c_rasterizer::_surface_none,
                surface, c_rasterizer::_sampler_filter_mode_point, c_rasterizer::_sampler_address_clamp, fx->bloom_intensity *exposure,
                fx->bloom_threshold, 1.0f, 1.0f, &bounds);
        }
        else
        {
            c_rasterizer::set_render_target(0, surface);
            c_rasterizer::clear(15, 0, 0.0f, 0);
        }
        if ((c_screen_postprocess::x_editable_settings.final_flags & 1) && !(g_player_render_tuning.flags & 1))
        {
            c_rasterizer::resolve_surface(surface, 0, 0, 0, 0);
            c_screen_postprocess::apply_binary_op_ex(67, surface, screen_postprocess_xenon_get_surface_accum(13), surface,
                c_rasterizer::_sampler_filter_mode_point, c_rasterizer::_sampler_address_clamp, fx->bloom_intensity *exposure,
                fx->bloom_threshold, 1.0f, 1.0f, &bounds);
        }
        c_rasterizer::resolve_surface(surface, 0, 0, 0, 0);
        if (depth_of_field)
        {
            c_screen_postprocess::copy(51, (c_rasterizer::e_surface)(screen_postprocess_xenon_get_surface_accum(13)),
                (c_rasterizer::e_surface)(50), (c_rasterizer::e_sampler_filter_mode)(2), (c_rasterizer::e_sampler_address_mode)(1),
                1.0f, 1.0f, 1.0f, 1.0f, &bounds);
            c_rasterizer::resolve_surface((c_rasterizer::e_surface)50, 0, 0, 0, 0);
        }
        c_rasterizer::end_high_quality_blend();
        result = (c_rasterizer::e_surface)c_screen_postprocess::postprocess_bloom_buffer(surface, *(c_camera_fx_values *)fx,
            (c_rasterizer::e_multires)multires);
    }
    if (result != c_rasterizer::_surface_none)
    {
        real_rectangle2d destination = bounds;
        if (result != screen_postprocess_xenon_get_surface_accum(13))
        {
            destination.x0 = destination.y0 = 0.0f;
            destination.x1 = destination.y1 = 1.0f;
        }
        float scale = c_screen_postprocess::x_editable_settings.final_scale;
        c_screen_postprocess::blit(c_screen_postprocess::x_editable_settings.final_blend ? 54 : 7, result, (c_rasterizer::e_surface)1,
            c_rasterizer::_sampler_filter_mode_point, c_rasterizer::_sampler_address_clamp, scale, scale, scale, 1.0f, &destination,
            &bounds);
    }
    else
    {
        if (depth_of_field)
        {
            float sigma = depth_of_field_compute_blur_sigma(focus) * 0.25f;
            c_screen_postprocess::gaussian_blur((c_rasterizer::e_surface)50, (c_rasterizer::e_surface)50, sigma, sigma);
            c_rasterizer::resolve_surface((c_rasterizer::e_surface)50, 0, 0, 0, 0);
        }
        long shader = depth_of_field ? 33 : 18;
        if (g_chud_text_render_target_dirty)
            shader = 74;
        float bloom_threshold = fx->bloom_threshold;
        if (bloom_threshold > 0.01f)
            bloom_threshold = 0.01f;
        screen_postprocess_render_composite(shader, (c_rasterizer::e_surface)1, surface, 1.0f / bloom_threshold,
            c_screen_postprocess::x_editable_settings.distortion, 0,
            depth_of_field, camera->near_clip, camera->far_clip, &camera->window_pixel_bounds, &bounds, focus);
    }
}

// @address 0x18f9d8 screen_postprocess_blit_player_view
extern "C" void screen_postprocess_blit_player_view(const c_player_view *view)
{
    c_rasterizer::e_surface surface = (c_rasterizer::e_surface)view->window_type;
    real_rectangle2d source_rect;
    source_rect.x0 = source_rect.y0 = 0.0f;
    source_rect.x1 = source_rect.y1 = 1.0f;
    const render_camera *camera = &view->render_camera_state;
    real_rectangle2d dest_rect;
    dest_rect.x0 = (float)camera->window_offset.x / (float)camera->fullscreen_pixel_bounds.x1;
    dest_rect.x1 = (float)(camera->window_pixel_bounds.x1 + camera->window_offset.x) / (float)camera->fullscreen_pixel_bounds.x1;
    dest_rect.y0 = (float)camera->window_offset.y / (float)camera->fullscreen_pixel_bounds.y1;
    dest_rect.y1 = (float)(camera->window_pixel_bounds.y1 + camera->window_offset.y) / (float)camera->fullscreen_pixel_bounds.y1;
    rasterizer_mark_surface_resolved(screen_postprocess_xenon_get_render_surface(surface));
    c_rasterizer::end_high_quality_blend();
    if (c_rasterizer::get_current_alpha_blend_mode() != c_rasterizer::_alpha_blend_opaque) {
        c_rasterizer::set_current_alpha_blend_mode(c_rasterizer::_alpha_blend_opaque);
        D3DDevice_SetRenderState_AlphaBlendEnable(c_rasterizer::get_device(), 0);
    }
    D3DDevice_SetRenderState_CullMode(c_rasterizer::get_device(), 0);
    c_rasterizer::set_scissor_rect(0);
    c_rasterizer::set_z_buffer_mode(c_rasterizer::_z_buffer_mode_off);
    c_rasterizer::clear_sampler_textures(0xffff);
    c_screen_postprocess::blit(2, surface, (c_rasterizer::e_surface)1, c_rasterizer::_sampler_filter_mode_point,
        c_rasterizer::_sampler_address_clamp, 1.0f, 1.0f, 1.0f, 0.03125f, &source_rect, &dest_rect);
}

static __forceinline XMMATRIX XMMatrixMultiply(XMMATRIX M1, XMMATRIX M2)
{
    XMVECTOR M00, M01, M02, M03,
             M10, M11, M12, M13,
             M20, M21, M22, M23,
             M30, M31, M32, M33;
    XMVECTOR M00M02, M01M03, M10M12, M11M13,
             M20M22, M21M23, M30M32, M31M33;
    XMMATRIX M2T, P;
    XMMATRIX Result;

    P.r[0] = __vmrghw(M2.r[0], M2.r[2]);
    P.r[1] = __vmrghw(M2.r[1], M2.r[3]);
    P.r[2] = __vmrglw(M2.r[0], M2.r[2]);
    P.r[3] = __vmrglw(M2.r[1], M2.r[3]);

    M2T.r[0] = __vmrghw(P.r[0], P.r[1]);
    M2T.r[2] = __vmrghw(P.r[2], P.r[3]);
    M2T.r[1] = __vmrglw(P.r[0], P.r[1]);
    M2T.r[3] = __vmrglw(P.r[2], P.r[3]);

    M00 = __vmsum4fp(M1.r[0], M2T.r[0]);
    M02 = __vmsum4fp(M1.r[0], M2T.r[2]);
    M01 = __vmsum4fp(M1.r[0], M2T.r[1]);
    M03 = __vmsum4fp(M1.r[0], M2T.r[3]);

    M10 = __vmsum4fp(M1.r[1], M2T.r[0]);
    M12 = __vmsum4fp(M1.r[1], M2T.r[2]);
    M11 = __vmsum4fp(M1.r[1], M2T.r[1]);
    M13 = __vmsum4fp(M1.r[1], M2T.r[3]);

    M20 = __vmsum4fp(M1.r[2], M2T.r[0]);
    M22 = __vmsum4fp(M1.r[2], M2T.r[2]);
    M21 = __vmsum4fp(M1.r[2], M2T.r[1]);
    M23 = __vmsum4fp(M1.r[2], M2T.r[3]);

    M30 = __vmsum4fp(M1.r[3], M2T.r[0]);
    M32 = __vmsum4fp(M1.r[3], M2T.r[2]);
    M31 = __vmsum4fp(M1.r[3], M2T.r[1]);
    M33 = __vmsum4fp(M1.r[3], M2T.r[3]);

    M00M02 = __vmrghw(M00, M02);
    M01M03 = __vmrghw(M01, M03);
    M10M12 = __vmrghw(M10, M12);
    M11M13 = __vmrghw(M11, M13);
    M20M22 = __vmrghw(M20, M22);
    M21M23 = __vmrghw(M21, M23);
    M30M32 = __vmrghw(M30, M32);
    M31M33 = __vmrghw(M31, M33);

    Result.r[0] = __vmrghw(M00M02, M01M03);
    Result.r[1] = __vmrghw(M10M12, M11M13);
    Result.r[2] = __vmrghw(M20M22, M21M23);
    Result.r[3] = __vmrghw(M30M32, M31M33);

    return Result;
}
// @address 0x72cb02 g_motion_blur_enabled
// @address 0x7374f8 g_motion_blur_max_view_count
// @address 0x73a180 g_distortion_occlusion_context
// @address 0x81da28 g_rasterizer_viewport_bounds
#if defined(__clang__)
#include "rasterizer/rasterizer_viewport_state.h"
#define g_rasterizer_viewport_bounds (g_rasterizer_last_rectangles.viewport)
#endif

static __forceinline bool cinematic_in_progress_inline()
{
    bool in_progress = false;
    if (cinematic_globals)
        in_progress = cinematic_globals->in_progress;
    return in_progress;
}
static __forceinline void screen_postprocess_xenon_build_matrices(
    const real_matrix4x3 *world_to_view, const float (*projection)[4], XMMATRIX *view_matrix, XMMATRIX *projection_matrix)
{
    view_matrix->_11 = world_to_view->forward.i;
    view_matrix->_12 = world_to_view->forward.j;
    view_matrix->_13 = world_to_view->forward.k;
    view_matrix->_21 = world_to_view->left.i;
    view_matrix->_22 = world_to_view->left.j;
    view_matrix->_23 = world_to_view->left.k;
    view_matrix->_31 = world_to_view->up.i;
    view_matrix->_32 = world_to_view->up.j;
    view_matrix->_33 = world_to_view->up.k;
    view_matrix->_41 = world_to_view->position.x;
    view_matrix->_42 = world_to_view->position.y;
    view_matrix->_43 = world_to_view->position.z;
    view_matrix->_14 = 0.0f;
    view_matrix->_24 = 0.0f;
    view_matrix->_34 = 0.0f;
    view_matrix->_44 = 1.0f;
    memcpy(projection_matrix, projection, sizeof(*projection_matrix));
}

// @address 0x182a38 ?postprocess_player_view@c_screen_postprocess@@SAXPAVc_player_view@@@Z
void c_screen_postprocess::postprocess_player_view(c_player_view *player_view)
{
    const render_projection *projection = &player_view->rasterizer_projection;
    const render_camera *camera = &player_view->rasterizer_camera;
    s_main_render_view_layout layout;
    main_render_get_view_layout(&layout);
    const unsigned long &motion_blur_enabled = g_motion_blur_enabled;
    bool motion_blur = motion_blur_enabled && (!cinematic_in_progress_inline() || g_effect_render_globals->enabled) &&
        !rasterizer_tiling_get_state()->enabled && layout.view_count <= g_motion_blur_max_view_count;
    if (motion_blur)
    {
        real_vector3d delta;
        delta.j = 0.0f;
        vector_from_points3d(&camera->position, &player_view->last_position, &delta);
        if (dot_product3d(&player_view->last_forward, &camera->forward) < 0.5f ||
            player_view->last_up.i * camera->up.i + player_view->last_up.j * camera->up.j + player_view->last_up.k * camera->up.k < 0.5f ||
            sqrtf(magnitude_squared3d(&delta)) > 5.0f)
        {
            motion_blur = false;
        }
    }
    if (g_rasterizer_distortion_visible || motion_blur)
    {
        const rectangle2d *bounds = &player_view->rasterizer_camera.window_pixel_bounds;
        if (g_distortion_occlusion_context != -1 && !motion_blur)
            c_rasterizer::get_device()->BeginConditionalRendering(g_distortion_occlusion_context);
        XMMATRIX view_matrix, projection_matrix;
        screen_postprocess_xenon_build_matrices(&projection->world_to_view, projection->projection_matrix, &view_matrix,
            &projection_matrix);
        XMMATRIX view_projection = XMMatrixMultiply(view_matrix, projection_matrix);
        screen_postprocess_xenon_build_matrices(&player_view->last_world_to_view, player_view->last_projection_matrix, &view_matrix,
            &projection_matrix);
        XMMATRIX last_view_projection = XMMatrixMultiply(view_matrix, projection_matrix);
        XMVECTOR determinant;
        XMMATRIX inverse_view_projection = XMMatrixInverse(&determinant, view_projection);
        c_rasterizer::resolve_surface(screen_postprocess_xenon_get_surface_accum(13), 0, bounds, bounds->x0, bounds->y0);
        c_rasterizer::set_depth_stencil_surface(c_rasterizer::_surface_none);
#if defined(_XBOX) && !defined(__clang__)
        // match: 0x182a38: the three matrices copy straight from this frame; a packed transforms local adds 192 bytes and
        // moves the saved-register range from r18 to r24
        reinterpret_cast<void (*)(const rectangle2d *, XMMATRIX, XMMATRIX, XMMATRIX, bool)>(rasterizer_render_displacement_pass)(
            bounds, view_projection, last_view_projection, inverse_view_projection, motion_blur);
#else
        s_displacement_motion_blur_transforms transforms;
        memcpy(transforms.transform0, &view_projection, sizeof(transforms.transform0));
        memcpy(transforms.transform1, &last_view_projection, sizeof(transforms.transform1));
        memcpy(transforms.transform2, &inverse_view_projection, sizeof(transforms.transform2));
        rasterizer_render_displacement_pass(bounds, transforms, motion_blur);
#endif
        c_rasterizer::set_viewport(g_rasterizer_viewport_bounds, 0.0f, 1.0f);
        g_rasterizer_distortion_visible = false;
        if (g_distortion_occlusion_context != -1 && !motion_blur)
            c_rasterizer::get_device()->EndConditionalRendering();
    }
    XMemCpy(player_view->last_projection_matrix, projection->projection_matrix, sizeof(player_view->last_projection_matrix));
    XMemCpy(&player_view->last_world_to_view, &projection->world_to_view, sizeof(player_view->last_world_to_view));
    player_view->last_forward = camera->forward;
    player_view->last_up = camera->up;
    player_view->last_position = camera->position;
}
