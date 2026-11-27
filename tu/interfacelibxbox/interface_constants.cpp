#include "interface/interface_constants_dependencies.h"
// interfacelibxbox/interface_constants.obj: from the records 0x18e730 0x18e800 0x1896f8 0x488ab0 0x488b10 0x488c80
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x8c46dc ?x_current_player_view@c_player_view@@1PAV1@A
// @address 0x730434 g_interface_authoring_to_target_dimensions_scaling
// @address 0x73042c g_interface_aspect_ratio_scaling
#include "math/real_math_types.h"
#include "interface/gui_widget_types.h"
#include "interface/user_interface.h"
#include "interface/interface_constants.h"
#include "render/views/player_view_types.h"
#include "rasterizer/rasterizer.h"
#include "rasterizer/rasterizer_main.h"
#include "rasterizer/rasterizer_video_mode_types.h"
#include "rasterizer/rasterizer_render_globals_types.h"
#include "interface/user_interface_shared_globals.h"

extern "C" {
// @address 0x7c5f4c k_authored_ui_dimensions
point2d k_authored_ui_dimensions[2] = { { 1152, 640 }, { 640, 480 } };
// @address 0x119a540 g_interface_display_width
long g_interface_display_width;
// @address 0x119a544 g_interface_display_height
long g_interface_display_height;
// @address 0x119a548 g_interface_render_width
long g_interface_render_width;
// @address 0x119a54c g_interface_render_height
long g_interface_render_height;
// @address 0x119a550 g_interface_authoring_target_width
int g_interface_authoring_target_width;
// @address 0x119a554 g_interface_authoring_target_height
int g_interface_authoring_target_height;
}

// 0x18e730
void interface_get_current_window_settings(rectangle2d *bounds, rectangle2d *, rectangle2d *scaled_bounds, rectangle2d *)
{
    const render_camera *camera = c_player_view::get_current()->get_rasterizer_camera();
    if (bounds)
        *bounds = camera->window_pixel_bounds;
    if (scaled_bounds)
    {
        calculate_aspect_ratio_scaling();
        real_vector2d inverse_scale = g_interface_aspect_ratio_scaling;
        inverse_scale.i = 1.0f / g_interface_aspect_ratio_scaling.i;
        inverse_scale.j = 1.0f / inverse_scale.j;
        *scaled_bounds = camera->window_pixel_bounds;
        interface_scale_bounds(scaled_bounds, &inverse_scale);
    }
}

// @address 0x18e800 interface_scale_bounds
extern "C" void interface_scale_bounds(rectangle2d *bounds, const real_vector2d *scale)
{
    float scale_x = scale->i;
    float scale_y = scale->j;
    short right = (short)((float)bounds->x1 * scale_x);
    short top = (short)((float)bounds->y0 * scale_y);
    short bottom = (short)((float)bounds->y1 * scale_y);
    bounds->x0 = (short)((float)bounds->x0 * scale_x);
    bounds->x1 = right;
    bounds->y0 = top;
    bounds->y1 = bottom;
}

real_vector2d interface_get_authoring_aspect_conversion_scaling(e_authored_ui_aspect_ratio aspect_ratio)
{
    real_vector2d scaling = { 1.0f, 1.0f };
    const s_user_interface_shared_globals *globals =
        (const s_user_interface_shared_globals *)((char *)::user_interface_shared_tag_globals_try_and_get());
    if (globals)
        scaling = globals->authoring_aspect_conversion_scaling[aspect_ratio];
    return scaling;
}

e_authored_ui_aspect_ratio interface_get_closest_authored_ui_aspect_ratio(void)
{
    e_authored_ui_aspect_ratio result = _authored_ui_aspect_ratio_four_by_three;
    short width;
    short height;
    if (rasterizer_main_get_video_mode()->fIsWideScreen != 0)
    {
        width = (short)c_rasterizer::render_globals.width;
        height = (short)c_rasterizer::render_globals.height;
    }
    else
    {
        width = 640;
        height = 480;
    }
    if (width == k_authored_ui_dimensions[1].x && height == k_authored_ui_dimensions[1].y)
        result = _authored_ui_aspect_ratio_sixteen_by_nine;
    return result;
}

static __forceinline int interface_constants_get_current_display_settings(point2d *dimensions)
{
    int mode = rasterizer_main_get_video_mode()->fIsWideScreen;
    if (mode != 0)
    {
        dimensions->x = (short)c_rasterizer::render_globals.width;
        dimensions->y = (short)c_rasterizer::render_globals.height;
    }
    else
    {
        dimensions->x = 640;
        dimensions->y = 480;
    }
    return mode;
}

static __forceinline bool interface_constants_display_mode_is_widescreen(int mode)
{
    return mode != 0;
}

// 0x488b10
void calculate_aspect_ratio_scaling(void)
{
    if (!interface_constants_display_mode_is_widescreen(rasterizer_main_get_video_mode()->fIsWideScreen))
    {
        point2d render;
        point2d display;

        render.x = (short)c_rasterizer::render_globals.width;
        render.y = (short)c_rasterizer::render_globals.height;
        interface_constants_get_current_display_settings(&display);

        if (display.x != g_interface_display_width || display.y != g_interface_display_height || render.x != g_interface_render_width ||
            render.y != g_interface_render_height)
        {
            g_interface_display_width = display.x;
            g_interface_display_height = display.y;
            g_interface_render_width = render.x;
            g_interface_render_height = render.y;
            if (g_interface_display_width && g_interface_display_height)
            {
                g_interface_aspect_ratio_scaling.i = (float)g_interface_render_width / (float)g_interface_display_width;
                g_interface_aspect_ratio_scaling.j = (float)g_interface_render_height / (float)g_interface_display_height;
            }
            else
            {
                g_interface_aspect_ratio_scaling.i = 1.0f;
                g_interface_aspect_ratio_scaling.j = 1.0f;
            }
        }
    }
}

void calculate_authoring_to_target_dimensions_scaling(void)
{
    if (rasterizer_main_get_video_mode()->fIsWideScreen == 0)
    {
        short tr10;
        short tr9;
        if (rasterizer_main_get_video_mode()->fIsWideScreen != 0)
        {
            tr10 = (short)c_rasterizer::render_globals.width;
            tr9 = (short)c_rasterizer::render_globals.height;
        }
        else
        {
            tr10 = 640;
            tr9 = 480;
        }
        short tr11;
        tr11 = tr10;
        if (tr11 != g_interface_authoring_target_width || tr9 != g_interface_authoring_target_height)
        {
            tr10 = tr9;
            g_interface_authoring_target_width = tr11;
            g_interface_authoring_target_height = tr10;
            float tf0;
            if (tr11 != 0 && tr10 != 0)
            {
                int aspect = interface_get_closest_authored_ui_aspect_ratio();
                float width = (float)g_interface_authoring_target_width;
                float height = (float)g_interface_authoring_target_height;
                point2d authoring = k_authored_ui_dimensions[aspect];
                float tf13;
                tf0 = width / (float)authoring.x;
                tf13 = height / (float)authoring.y;
                if (tf0 > tf13)
                {
                    g_interface_authoring_to_target_dimensions_scaling.i = tf13;
                }
                else
                {
                    g_interface_authoring_to_target_dimensions_scaling.i = tf0;
                }
                if (tf0 > tf13)
                {
                    g_interface_authoring_to_target_dimensions_scaling.j = tf13;
                }
                else
                {
                    g_interface_authoring_to_target_dimensions_scaling.j = tf0;
                }
            }
            else
            {
                tf0 = 1.0f;
                g_interface_authoring_to_target_dimensions_scaling.i = tf0;
                g_interface_authoring_to_target_dimensions_scaling.j = tf0;
            }
        }
    }
    return;
}
