// blamlibxbox/main_screenshot.obj: from the records 0x2a3928 0x2a3c60 0x2a40a8 0x2a5d70
// @flags /TP /O2 /Oc
#include "render/render_view_stack_globals.h"
// @compiler xdk6130
#include "rasterizer/rasterizer_screenshot.h"
#include <xtl.h>
#include <xgraphics.h>
#include "cseries/cseries.h"
#include "math/real_math.h"
#include <math.h>
#include <string.h>
#include <ppcintrinsics.h>
#include "effects/player_effects.h"
#include "interface/chud/chud.h"
#include "interface/interface_constants.h"
#include "bink/bink_playback.h"
#include "bink/bink_playback_types.h"
#include "render/render_view.h"
#include "cutscene/cinematics.h"
#include "rasterizer/dx9/rasterizer_dx9_dynamic_geometry.h"
#include "render/render_cameras.h"
#include "render/views/render_view.h"
#include "render/views/render_player_view.h"
#include "render/views/player_view_types.h"
#include "rasterizer/rasterizer_render_globals_types.h"
#include "rasterizer/rasterizer.h"
#include "render/screen_postprocess.h"
#include "render/depth_of_field_types.h"
#include "render/depth_of_field_globals.h"
#include "render/ui_view.h"
#include "render/screen_postprocess_settings.h"
#include "render/screen_postprocess_xenon.h"
#include "interface/user_interface_window_manager.h"
#include "rasterizer/rasterizer_hue_saturation.h"

#include "rasterizer/rasterizer_tiling_types.h"
#include "rasterizer/rasterizer_tiling_globals.h"
#include "main/main_screenshot_globals.h"
#include "main/main_screenshot.h"
#include "render/render_camera_types.h"
#include "render/screen_postprocess_blur.h"
// match: 0x2a40a8: retain full-register distortion argument; the owner bool declaration adds narrowing.
#include "main/main_screenshot_composite.h"

static __forceinline void main_screenshot_set_memexport_stream(s_gpu_memexport_stream_constant *stream, void *buffer, long buffer_size)
{
    ULONG_PTR address = (ULONG_PTR)buffer;
    stream->address = (unsigned long)(((((address >> 20) + 0x200) & 0x1000) + (address & 0x1ffffffc)) >> 2 | 0x40000000);
    stream->format = 0x4b000000;
    stream->flags = (stream->flags & 0xc0f8) | 0x4b000000 | 0x602;
    stream->size = ((buffer_size / 4) & 0x7fffff) | 0x4b000000;
}

// @address 0x2a5d70 main_screenshot_start_capture
extern "C" unsigned char main_screenshot_start_capture(long width, long height, unsigned long texture_width, unsigned long texture_height,
    void *buffer, long buffer_size, long volatile *fence_destination)
{
    volatile s_rasterizer_tiling_state *tiling = rasterizer_tiling_get_state();
    unsigned char result = false;
    unsigned char started;
    if (tiling->enabled)
        started = false;
    else
        started = rasterizer_tiling_initialize((short)width, (short)height);
    if (started)
    {
        tiling->skip_postprocess = false;
        tiling->end_callback = rasterizer_export_screenshot_tile;
        tiling->pad_to_tile_grid = true;
        tiling->fence_destination = fence_destination;
        tiling->use_half_size_tiles = true;
        memset(buffer, 0, buffer_size);
        XGSetTextureHeader(texture_width, texture_height, 1, 4, D3DFMT_LIN_A8R8G8B8, D3DPOOL_DEFAULT, 0, 0, 0, (D3DTexture *)&tiling->texture, 0, 0);
        XGOffsetBaseTextureAddress((D3DBaseTexture *)&tiling->texture, buffer, buffer);
        main_screenshot_set_memexport_stream((s_gpu_memexport_stream_constant *)&tiling->export_stream, buffer, buffer_size);
        __lwsync();
        tiling->screenshot_active = true;
        result = true;
    }
    return result;
}

// @address 0x8c46e5 g_render_water_flag
extern "C" {
bool g_render_water_flag;
}

// @address 0x2a3928 main_screenshot_compute_tile_layout
extern "C" bool main_screenshot_compute_tile_layout(s_screenshot_options *options)
{
    bool result = false;
    volatile s_rasterizer_tiling_state *tiling = rasterizer_tiling_get_state();
    long width = tiling->width;
    long height = tiling->height;
    bool depth_of_field = g_depth_of_field_game_globals->enabled;
    options->width = width;
    options->height = height;
    options->depth_of_field = depth_of_field;
    do
    {
        if (!depth_of_field && !g_render_water_flag && !rasterizer_tiling_get_state()->use_half_size_tiles)
        {
            options->tile_width = c_rasterizer::render_globals.width;
            options->tile_height = c_rasterizer::render_globals.height;
        }
        else
        {
            float ratio_x = (float)width / (float)c_rasterizer::render_globals.width;
            float ratio_y = (float)height / (float)c_rasterizer::render_globals.height;
            long x = (long)(float)ceil(ratio_x * 20.0f);
            long y = (long)(float)ceil(ratio_y * 20.0f);
            if (x * 4 > c_rasterizer::render_globals.width || y * 4 > c_rasterizer::render_globals.height)
                break;
            options->tile_width = (long)(float)ceil((float)c_rasterizer::render_globals.width * 0.5f);
            options->tile_height = (long)(float)ceil((float)c_rasterizer::render_globals.height * 0.5f);
        }
        options->tile_width &= -16;
        options->tile_height &= -16;
        options->tile_count_x = (long)(float)ceil((float)options->width / (float)options->tile_width);
        options->tile_count_y = (long)(float)ceil((float)options->height / (float)options->tile_height);
        while ((options->tile_width - 16) * options->tile_count_x >= options->width)
            options->tile_width -= 16;
        while ((options->tile_height - 16) * options->tile_count_y >= options->height)
            options->tile_height -= 16;
        if (rasterizer_tiling_get_state()->pad_to_tile_grid)
        {
            options->width = options->tile_width * options->tile_count_x;
            options->height = options->tile_height * options->tile_count_y;
        }
        result = true;
    } while (false);
    return result;
}

extern "C" {
// @address 0x737504 g_tiled_render_x_scale
float g_tiled_render_x_scale = 1.0f;
// @address 0x737508 g_tiled_render_y_scale
float g_tiled_render_y_scale = 1.0f;

// @address 0x8c4830 g_tiled_render_x_offset
float g_tiled_render_x_offset;
// @address 0x8c4834 g_tiled_render_y_offset
float g_tiled_render_y_offset;

}

// match: 0x2a3c60: keeps memcpy(camera, camera, ...) as a bl memcpy; the intrinsic drops the self-copy call
#pragma function(memcpy)
// @address 0x2a3c60 main_screenshot_setup_tile_camera
// match: 0x2a3c60: __restrict hoists options size loads above the first memcpy (frame 0xd0, not 0xc0)
extern "C" bool main_screenshot_setup_tile_camera(s_screenshot_options *__restrict options, long tile_x, long tile_y)
{
    c_view *view = options->view;
    render_camera *camera = view->get_rasterizer_camera_modifiable();
    memcpy(&view->render_camera_state, camera, sizeof(*camera));
    camera->frustum_adjustment = true;
    camera->frustum_scale.x = (float)c_rasterizer::render_globals.width / (float)options->width;
    camera->frustum_scale.y = (float)c_rasterizer::render_globals.height / (float)options->height;
    camera->frustum_center.x = (((float)tile_x + 0.5f) * (float)options->tile_width / (float)options->width) * 2.0f - 1.0f;
    camera->frustum_center.y = -((((float)tile_y + 0.5f) * (float)options->tile_height / (float)options->height) * 2.0f - 1.0f);
    memcpy(camera, camera, sizeof(*camera));
    {
        real_rectangle2d bounds;
        render_camera_build_viewport_frustum_bounds(camera, &bounds);
        render_camera_build_projection(camera, &bounds, view->get_rasterizer_projection_modifiable(), 0.0f);
    }
    memcpy(&options->view->render_projection_state, options->view->get_rasterizer_projection(), sizeof(render_projection));
    g_tiled_render_x_scale = (float)options->width / (float)c_rasterizer::render_globals.width;
    g_tiled_render_y_scale = (float)options->height / (float)c_rasterizer::render_globals.height;
    g_tiled_render_x_offset = -((float)(options->tile_width * tile_x) - (float)(c_rasterizer::render_globals.width - options->tile_width) * 0.5f);
    g_tiled_render_y_offset = -((float)(options->tile_height * tile_y) - (float)(c_rasterizer::render_globals.height - options->tile_height) * 0.5f);
    return true;
}

// @address 0x1142940 g_window_manager
// @address 0x81dc04 g_view_stack

static __forceinline bool main_screenshot_bink_playback_in_progress()
{
    return bink_globals.active && bink_globals.enabled;
}

// match: 0x2a40a8: precise mode is required by fenv_access(on) below (it does not compile without it)
#pragma float_control(precise, on, push)
// match: 0x2a40a8: MAX evaluates __fsqrts twice (compare, then result), not one fsqrts + fmr
#pragma fenv_access(on)
static __forceinline float main_screenshot_blur_sigma()
{
    float blur = g_depth_of_field_game_globals->focus_distance;
    float square = blur * blur;
    float bias = 2.5f;
    float minimum = 0.004f;
    return MAX(__fsqrts(square - bias), minimum);
}
// match: 0x2a40a8: restores default fenv access after main_screenshot_blur_sigma
#pragma fenv_access(off)
// match: 0x2a40a8: restores the float model after main_screenshot_blur_sigma
#pragma float_control(pop)

// @address 0x2a40a8 main_screenshot_render_tile
extern "C" void main_screenshot_render_tile(s_screenshot_options *options, long tile_x, long tile_y)
{
    c_screen_postprocess::setup_rasterizer_for_postprocess(true);
    if (options->depth_of_field)
    {
        c_rasterizer::g_HDR_alpha_blend = false;
        c_screen_postprocess::copy(51, (c_rasterizer::e_surface)13, (c_rasterizer::e_surface)50, (c_rasterizer::e_sampler_filter_mode)2,
            (c_rasterizer::e_sampler_address_mode)1, 1.0f, 1.0f, 1.0f, 1.0f, 0);
        c_rasterizer::resolve_surface((c_rasterizer::e_surface)50, 0, 0, 0, 0);
        float sigma = main_screenshot_blur_sigma() * 0.25f;
        float sigma_y;
        float sigma_x = sigma * (float)options->width / (float)c_rasterizer::render_globals.width;
        sigma_y = sigma * (float)options->height / (float)c_rasterizer::render_globals.height;
#if !defined(_XBOX) || defined(__clang__)
        screen_postprocess_apply_large_blur(sigma_x, sigma_y);
#else
        // match: 0x2a40a8: retains the loads of 50 into r4 and r3 between the stack stores after resolve_surface.
        screen_postprocess_apply_large_blur(50, 50, sigma_x, sigma_y);
#endif
    }
    volatile s_rasterizer_tiling_state *tiling = rasterizer_tiling_get_state();
    c_rasterizer::e_surface target = (c_rasterizer::e_surface)(tiling->skip_postprocess ? 9 : 12);
    real_rectangle2d bounds;
    float render_width = (float)c_rasterizer::render_globals.width;
    float render_height = (float)c_rasterizer::render_globals.height;
    bounds.x0 = -((float)(options->tile_width * tile_x) - (float)(c_rasterizer::render_globals.width - options->tile_width) * 0.5f) / render_width;
    bounds.y0 = -((float)(options->tile_height * tile_y) - (float)(c_rasterizer::render_globals.height - options->tile_height) * 0.5f) / render_height;
    bounds.x1 = bounds.x0 + (float)options->width / render_width;
    bounds.y1 = bounds.y0 + (float)options->height / render_height;
    rasterizer_set_hue_saturation_constants(g_screenshot_hue_saturation, options->hue_end - options->hue_start,
        options->saturation_end - options->saturation_start, options->brightness, &options->filter, &options->offset);
    float exposure_value = options->exposure_offset + 1.0f - options->exposure_bias;
    float minimum_exposure = 0.01f;
    float maximum_exposure = 10.0f;
    float exposure_scale = (exposure_value > minimum_exposure ? exposure_value : minimum_exposure) < maximum_exposure
        ? ((double)exposure_value > (double)minimum_exposure ? exposure_value : minimum_exposure)
        : maximum_exposure;
    real_vector4d exposure_constant;
    exposure_constant.i = exposure_constant.j = exposure_constant.k = exposure_constant.l = exposure_scale;
    c_rasterizer::get_device()->SetPixelShaderConstantF(232, exposure_constant.n, 1);
    long old_composite_mask = c_screen_postprocess::x_editable_settings.composite_mask;
    (void)tiling->disabled;
    c_screen_postprocess::x_editable_settings.composite_mask = 1;
    if (tiling->skip_postprocess)
        c_screen_postprocess::x_editable_settings.composite_mask = 3;
    long distortion = !tiling->skip_postprocess && !tiling->disabled && c_screen_postprocess::x_editable_settings.distortion;
    long shader = options->depth_of_field && !tiling->skip_postprocess ? 41 : 28;
    screen_postprocess_render_composite(shader, target, (c_rasterizer::e_surface)30, 1.0f, distortion, !tiling->skip_postprocess, options->depth_of_field,
        options->view->get_rasterizer_camera()->near_clip, options->view->get_rasterizer_camera()->far_clip, 0, &bounds, 0);
    if (tiling->include_ui && !tiling->skip_postprocess)
    {
        c_player_view::set_current(options->view);
        c_view::begin(options->view);
        c_rasterizer::set_render_target(0, target);
        render_player_view_set_exposure_constants(1.0f, 1.0f, 7.0f, true);
        rectangle2d window_bounds;
        interface_get_current_window_settings(0, 0, &window_bounds, 0);
        c_rasterizer::begin_high_quality_blend();
        c_rasterizer::set_render_target(0, target);
        c_rasterizer::set_render_target(1, c_rasterizer::_surface_none);
        window_manager_render_window(&g_window_manager, _window0, 0, &window_bounds, true);
        chud_draw_screen(0);
        player_effect_render(0);
        c_view::end();
        c_player_view::set_current(0);
        c_ui_view ui_view;
        render_view_initialize_surface(&ui_view, 0, target);
        c_view::begin(&ui_view);
        if (main_screenshot_bink_playback_in_progress())
            bink_playback_render_first_frame();
        cinematic_render(true, true);
        ui_view.m_is_screenshot = true;
        ui_view.c_ui_view::render();
        c_view::end();
    }
    c_rasterizer::resolve_surface(target, 0, 0, 0, 0);
    if (!tiling->skip_postprocess)
    {
        c_screen_postprocess::setup_rasterizer_for_postprocess(false);
        c_screen_postprocess::copy(42, (c_rasterizer::e_surface)target, (c_rasterizer::e_surface)10, (c_rasterizer::e_sampler_filter_mode)1,
            (c_rasterizer::e_sampler_address_mode)1, 0.5128205f, 1.0f, 1.0f, 1.0f, 0);
        c_rasterizer::resolve_surface((c_rasterizer::e_surface)10, 0, 0, 0, 0);
    }
    c_screen_postprocess::x_editable_settings.composite_mask = old_composite_mask;
}


// @address 0x11df81c g_screenshot_hue_saturation
