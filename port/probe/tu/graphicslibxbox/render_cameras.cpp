// graphicslibxbox/render_cameras.obj: from the records 0x148468 0x148608 0x148950 0x148bc8 0x148c88 0x148ff8 0x1534c0 0x1570a8 0x167268
// @flags /TP /O2 /Oc
#include "render/render_camera_types.h"
#include "render/render_camera_projection.h"
// @address 0x148950 ?render_camera_build_viewport_frustum_bounds@@YA_NPBUrender_camera@@PATreal_rectangle2d@@@Z
// @address 0x4470d0 ?rasterizer_main_get_video_mode@@YAPAUXVIDEO_MODE@@XZ

#include "math/real_math.h"
#include "math/matrix_math.h"
#include "objects/object_header.h"
#include "game/game_globals_storage.h"
#include "tag_files/tag_groups.h"
#include <math.h>
#include <string.h>
#include "interface/first_person_weapons.h"
#include "interface/first_person_weapon_data_types.h"
#include "rasterizer/rasterizer_main.h"
#include "render/render_cameras.h"
#include "render/render_first_person_model_types.h"
#include "../../splitscreen_view.h"
#include "../../../sdk/runtime/port_config.h"
#include "rasterizer/rasterizer_render_globals_types.h"
#include "models/render_model_definitions.h"
#include "objects/object_definitions.h"
#include "models/model_types.h"
#include "cutscene/cinematic_globals_types.h"
#include "animations/animation_id.h"
#include "game/game_allegiance_data.h"
#include "interface/interface_globals.h"
extern "C" {
// 32 view configs: index 0 selects standard aspect, index 1 widescreen.
// @address 0x730bb8 k_interface_splitscreen_view_configs
const view_config k_interface_splitscreen_view_configs[2][4][4] = {
    {
        { { 0.0f, 0.0f, 1.0f, 1.0f, 0 }, { 0.0f, 0.0f, 0.0f, 0.0f, 0 },
            { 0.0f, 0.0f, 0.0f, 0.0f, 0 }, { 0.0f, 0.0f, 0.0f, 0.0f, 0 } },
        { { 0.0f, 0.0f, 1.0f, 0.5f, 1 }, { 0.0f, 0.5f, 1.0f, 1.0f, 1 },
            { 0.0f, 0.0f, 0.0f, 0.0f, 0 }, { 0.0f, 0.0f, 0.0f, 0.0f, 0 } },
        { { 0.0f, 0.0f, 1.0f, 0.5f, 1 }, { 0.0f, 0.5f, 0.5f, 1.0f, 2 },
            { 0.5f, 0.5f, 1.0f, 1.0f, 2 }, { 0.0f, 0.0f, 0.0f, 0.0f, 0 } },
        { { 0.0f, 0.0f, 0.5f, 0.5f, 2 }, { 0.5f, 0.0f, 1.0f, 0.5f, 2 },
            { 0.0f, 0.5f, 0.5f, 1.0f, 2 }, { 0.5f, 0.5f, 1.0f, 1.0f, 2 } }
    },
    {
        { { 0.0f, 0.0f, 1.0f, 1.0f, 0 }, { 0.0f, 0.0f, 0.0f, 0.0f, 0 },
            { 0.0f, 0.0f, 0.0f, 0.0f, 0 }, { 0.0f, 0.0f, 0.0f, 0.0f, 0 } },
        { { 0.125f, 0.0f, 0.875f, 0.5f, 3 }, { 0.125f, 0.5f, 0.875f, 1.0f, 3 },
            { 0.0f, 0.0f, 0.0f, 0.0f, 0 }, { 0.0f, 0.0f, 0.0f, 0.0f, 0 } },
        { { 0.125f, 0.0f, 0.875f, 0.5f, 3 }, { 0.0f, 0.5f, 0.5f, 1.0f, 2 },
            { 0.5f, 0.5f, 1.0f, 1.0f, 2 }, { 0.0f, 0.0f, 0.0f, 0.0f, 0 } },
        { { 0.0f, 0.0f, 0.5f, 0.5f, 2 }, { 0.5f, 0.0f, 1.0f, 0.5f, 2 },
            { 0.0f, 0.5f, 0.5f, 1.0f, 2 }, { 0.5f, 0.5f, 1.0f, 1.0f, 2 } }
    }
};
}
// @address 0x25888 k_observer_default_aspect_ratio

union rectangle2d;
union rectangle2d;
#include "rasterizer/rasterizer.h"
struct render_camera;
#include "rasterizer/rasterizer_video_mode_types.h"


#include "interface/chud_draw_globals.h"
#include "interface/chud/chud_draw_types.h"
#include "render/views/player_view_types.h"

struct render_camera_projection
{
    real_rectangle2d bounds;
    float width;
    float height;
    float half_width;
    float half_height;
    float center_x;
    float center_y;
    float scale_x;
    float scale_y;
    union {
        struct { float left, right, bottom, top; };
        real_rectangle2d frustum;
    };
};



static __forceinline void get_fullscreen_render_pixel_bounds(rectangle2d *bounds) {
 bounds->y0 = 0;
 bounds->x0 = 0;
 bounds->x1 = (short)c_rasterizer::render_globals.width;
 bounds->y1 = (short)c_rasterizer::render_globals.height;
}
// @address 0x148468 ?render_view_compute_fullscreen_bounds@@YAXPAUrender_camera@@@Z
void render_view_compute_fullscreen_bounds(struct render_camera * camera) {
 get_fullscreen_render_pixel_bounds(&camera->fullscreen_pixel_bounds);
 ::c_rasterizer::get_fullscreen_render_title_safe_pixel_bounds(&camera->fullscreen_title_safe_pixel_bounds);
 ::c_rasterizer::get_display_pixel_bounds(&camera->display_pixel_bounds);
 camera->window_pixel_bounds = camera->fullscreen_pixel_bounds;
 camera->window_title_safe_pixel_bounds = camera->fullscreen_title_safe_pixel_bounds;
}

// match: 0x148950: __restrict on camera hoists the frustum_adjustment load above the frustum_bounds stores (sinks below + recolors r8->r11 without it)
bool render_camera_build_viewport_frustum_bounds(const render_camera * __restrict camera, real_rectangle2d *frustum_bounds)
{
    rectangle2d window_bounds = camera->window_pixel_bounds;
    rectangle2d title_safe_bounds = camera->window_title_safe_pixel_bounds;
    real_point2d scale;
    scale.x = (float)camera->display_pixel_bounds.x1 / (float)camera->fullscreen_pixel_bounds.x1;
    scale.y = (float)camera->display_pixel_bounds.y1 / (float)camera->fullscreen_pixel_bounds.y1;
    rectangle2d_scale_around_origin(&window_bounds, &scale);
    rectangle2d_scale_around_origin(&title_safe_bounds, &scale);

    float aspect_ratio = (float)(window_bounds.y1 - window_bounds.y0) / (float)(window_bounds.x1 - window_bounds.x0);
    float center_x = (float)(title_safe_bounds.x0 + title_safe_bounds.x1) / 2.0f;
    float center_y = (float)(title_safe_bounds.y0 + title_safe_bounds.y1) / 2.0f;
    float scale_y = 2.0f / (float)(window_bounds.y1 - window_bounds.y0);
    float scale_x = scale_y * aspect_ratio;
    frustum_bounds->x0 = scale_x * ((float)window_bounds.x0 - center_x);
    frustum_bounds->x1 = scale_x * ((float)window_bounds.x1 - center_x);
    frustum_bounds->y0 = scale_y * (center_y - (float)window_bounds.y1);
    frustum_bounds->y1 = scale_y * (center_y - (float)window_bounds.y0);
    if (camera->frustum_adjustment)
    {
        float width = camera->frustum_scale.x * (frustum_bounds->x1 - frustum_bounds->x0);
        float height = camera->frustum_scale.y * (frustum_bounds->y1 - frustum_bounds->y0);
        float adjusted_center_x = frustum_bounds->x0 + (0.5f + 0.5f * camera->frustum_center.x) * (frustum_bounds->x1 - frustum_bounds->x0);
        float adjusted_center_y = frustum_bounds->y0 + (0.5f + 0.5f * camera->frustum_center.y) * (frustum_bounds->y1 - frustum_bounds->y0);
        frustum_bounds->x0 = adjusted_center_x - 0.5f * width;
        frustum_bounds->x1 = adjusted_center_x + 0.5f * width;
        frustum_bounds->y0 = adjusted_center_y - 0.5f * height;
        frustum_bounds->y1 = adjusted_center_y + 0.5f * height;
    }
    return true;
}

// Scale interface bounds for the Xenon scaler.
// @address 0x148bc8 rectangle2d_scale_around_origin
// match: 0x148bc8: __restrict on scale pins the scale.x/scale.y loads above the bounds stores (short-convert chain recolors + reorders without it)
extern "C" void rectangle2d_scale_around_origin(rectangle2d *bounds, const real_point2d * __restrict scale)
{
    bounds->x0 = (short)((float)bounds->x0 * scale->x);
    bounds->y0 = (short)((float)bounds->y0 * scale->y);
    bounds->x1 = (short)((float)bounds->x1 * scale->x);
    bounds->y1 = (short)((float)bounds->y1 * scale->y);
}

// @address 0x1570a8 render_camera_compute_projected_diameter
float render_camera_compute_projected_diameter(const render_projection *projection, const real_point3d *center, float radius)
{
    float depth = projection->world_to_view.forward.k * center->x +
        (projection->world_to_view.up.k * center->z + projection->world_to_view.left.k * center->y) +
        projection->world_to_view.position.z;
    float distance = depth < 0.0f ? -depth : depth;
    if (distance <= 0.1f) distance = 0.1f;
    float scale = projection->world_to_screen_size.j;
    int enabled = 0;
    if (cinematic_globals != 0) enabled = cinematic_globals->in_progress;
    if ((unsigned int)(enabled & 255) != 0 && rasterizer_main_get_video_mode()->fIsWideScreen == 0) scale = scale * 1.333f;
    return scale / distance * radius * 2.0f;
}

static __forceinline void render_cameras_calculate_safe_bounds(const rectangle2d *bounds, const rectangle2d *safe, rectangle2d *clipped)
{
    *clipped = *bounds;
    if (bounds->x0 < safe->x0) clipped->x0 = safe->x0;
    if (bounds->x1 > safe->x1) clipped->x1 = safe->x1;
    if (bounds->y0 < safe->y0) clipped->y0 = safe->y0;
    if (bounds->y1 > safe->y1) clipped->y1 = safe->y1;
}
extern "C" unsigned int port_vk_render_resolution_scale(void);

const view_config &splitscreen_view_config(bool widescreen, long count, long screen)
{
    if (widescreen && ((count == 2 && screen < 2) || (count == 3 && screen == 0)) &&
        (c_rasterizer::render_globals.width != 1152 || c_rasterizer::render_globals.height != 640 ||
            port_vk_render_resolution_scale() > 1))
    {
        // The standard-aspect row also selects the full-width, half-height intermediate render target.
        return k_interface_splitscreen_view_configs[0][count - 1][screen];
    }
    return k_interface_splitscreen_view_configs[widescreen][count - 1][screen];
}

// @address 0x148608 ?render_view_compute_window_bounds@@YAXJJPATrectangle2d@@0@Z
void render_view_compute_window_bounds(long screen, long arrangement, rectangle2d *bounds, rectangle2d *clipped)
{
    rectangle2d fullscreen_bounds;
    get_fullscreen_render_pixel_bounds(&fullscreen_bounds);
    bool widescreen = rasterizer_main_get_video_mode()->fIsWideScreen != 0;
    const view_config *view = &splitscreen_view_config(widescreen, arrangement, screen);
    long width = fullscreen_bounds.x1 - fullscreen_bounds.x0;
    long height = fullscreen_bounds.y1 - fullscreen_bounds.y0;
    bounds->x0 = (short)(view->x0 * width);
    bounds->x1 = (short)(view->x1 * width);
    bounds->y0 = (short)(view->y0 * height);
    bounds->y1 = (short)(view->y1 * height);
    if (arrangement > 1 && port_config_splitscreen_symmetric_hud())
    {
        // Round 2.5% of the viewport width and 3.5% of its height once for each pair of edges.
        const int dx = ((bounds->x1 - bounds->x0) + 20) / 40;
        const int dy = (7 * (bounds->y1 - bounds->y0) + 100) / 200;
        clipped->x0 = bounds->x0 + dx;
        clipped->x1 = bounds->x1 - dx;
        clipped->y0 = bounds->y0 + dy;
        clipped->y1 = bounds->y1 - dy;
        return;
    }
    rectangle2d safe_bounds;
    rasterizer_get_view_title_safe_pixel_bounds(&safe_bounds, widescreen && view->extra == 1);
    render_cameras_calculate_safe_bounds(bounds, &safe_bounds, clipped);
}

// @address 0x8977a0 g_chud_draw_globals
// @rdata 0x8202490c
static const float k_render_cameras_normalize_epsilon = 0.0001f;
static __forceinline void render_cameras_normalize3d(real_vector3d *vector)
{
    float length = sqrtf(vector->i * vector->i + vector->j * vector->j + vector->k * vector->k);
    if (!(fabsf(length) < k_render_cameras_normalize_epsilon))
    {
        float inverse = 1.0f / length;
        vector->i *= inverse;
        vector->j *= inverse;
        vector->k *= inverse;
    }
}
// @address 0x167268 ??0c_curved_hud_test@@QAA@XZ
c_curved_hud_test::c_curved_hud_test()
{
    memset(this, 0, sizeof(*this));
    m_horizontal_angle = 0.6981317f;
    m_aspect_ratio = 1.5f;
    m_distance = 0.5f;
    if (g_chud_draw_globals.curvature_info)
    {
        float amount = g_chud_draw_globals.curvature_amount;
        m_horizontal_angle = (float)((1.0 - amount) * 0.017453292f + g_chud_draw_globals.curvature_info->field_of_view * amount);
        m_aspect_ratio = g_chud_draw_globals.curvature_info->aspect_ratio;
        m_distance = (float)(g_chud_draw_globals.curvature_info->depth_offset * amount - (1.0 - amount) * 0.9f);
    }
    m_corner_tangents.i = (float)tan(m_aspect_ratio * m_horizontal_angle);
    m_corner_tangents.j = (float)tan(m_horizontal_angle);
    set_real_vector3d(&m_shader_constants[0].xyz, -m_corner_tangents.i, m_corner_tangents.j, 1.0f);
    set_real_vector3d(&m_shader_constants[1].xyz, m_corner_tangents.i, m_corner_tangents.j, 1.0f);
    set_real_vector3d(&m_shader_constants[2].xyz, m_corner_tangents.i, -m_corner_tangents.j, 1.0f);
    set_real_vector3d(&m_shader_constants[3].xyz, -m_corner_tangents.i, -m_corner_tangents.j, 1.0f);
    for (long i = 0; i < 4; i++)
        render_cameras_normalize3d(&m_shader_constants[i].xyz);
    m_projection_scale.i = (m_shader_constants[1].k + m_distance) / m_shader_constants[1].i;
    m_projection_scale.j = (m_shader_constants[1].k + m_distance) / m_shader_constants[1].j;
    c_player_view *view = c_player_view::get_current();
    m_origin.x = (float)(g_chud_draw_globals.pixel_bounds.x0 - view->rasterizer_camera.window_pixel_bounds.x0);
    m_origin.y = (float)(g_chud_draw_globals.pixel_bounds.y0 - view->rasterizer_camera.window_pixel_bounds.y0);
    m_half_size.i = (float)(g_chud_draw_globals.pixel_bounds.x1 - g_chud_draw_globals.pixel_bounds.x0) * 0.5f;
    m_half_size.j = (float)(g_chud_draw_globals.pixel_bounds.y1 - g_chud_draw_globals.pixel_bounds.y0) * 0.5f;
    m_radians_per_pixel = angle_between_vectors3d(&m_shader_constants[0].xyz, &m_shader_constants[1].xyz) / g_chud_draw_globals.width;
}

extern "C" {
// @address 0x72cafe g_render_camera_cinematic_widescreen_scale_enabled
bool g_render_camera_cinematic_widescreen_scale_enabled = true;
// @address 0x58840 k_render_camera_cinematic_widescreen_scale
const float k_render_camera_cinematic_widescreen_scale = 0.5625f;
}
static __forceinline float render_cameras_half_field_of_view(const render_camera *camera)
{
    return camera->vertical_field_of_view * 0.5f;
}
static __forceinline bool render_cameras_cinematic_active()
{
    bool result = false;
    if (cinematic_globals != 0)
        result = cinematic_globals->in_progress;
    return result;
}
// @rdata 0x8202490c
static const float render_cameras_projection_epsilon = 0.0001f;
// match: 0x148ff8: aggregate helper retains the tangent operand rank.
static __forceinline render_cameras_projection_factors render_cameras_compute_projection_factors(
    const render_camera_projection *projection, float half_field_of_view)
{
    render_cameras_projection_factors factors;
    factors.tangent = (float)tan(half_field_of_view);
    factors.aspect = projection->width / projection->height;
    return factors;
}
// match: 0x148ff8: aggregate helper retains the vertical-scale operand rank.
static __forceinline render_cameras_vertical_factors render_cameras_compute_vertical_factors(
    const render_camera_projection *projection, short display_height)
{
    render_cameras_vertical_factors factors;
    factors.lower = projection->bounds.y0;
    factors.upper = projection->bounds.y1;
    factors.scale = projection->height / (float)display_height;
    return factors;
}

extern "C" {
// @address 0xb84 k_render_camera_width_scale
const float k_render_camera_width_scale = 1280.0f;
// @address 0x588d0 k_render_camera_height_scale
const float k_render_camera_height_scale = 720.0f;
}
// @address 0x148c88 render_camera_build_projection
extern "C" void render_camera_build_projection(const render_camera *camera, const real_rectangle2d *bounds,
    render_projection *projection, float depth)
{
    float near_clip = camera->near_clip;
    float far_clip = camera->far_clip;
    render_camera_projection parameters;
    render_camera_compute_projection_parameters(camera, bounds, &parameters, depth);
    real_vector3d backward;
    real_vector3d left;
    real_vector3d up;
    cross_product3d(&camera->forward, &camera->up, &left);
    cross_product3d(&left, &camera->forward, &up);
    negate_vector3d(&camera->forward, &backward);
    normalize3d(&left);
    normalize3d(&up);
    normalize3d(&backward);
    projection->view_to_world.forward = left;
    projection->view_to_world.left = up;
    projection->view_to_world.up = backward;
    projection->view_to_world.position = camera->position;
    projection->view_to_world.scale = 1.0f;
    matrix4x3_inverse(&projection->view_to_world, &projection->world_to_view);
    float width = (float)(camera->display_pixel_bounds.x1 - camera->display_pixel_bounds.x0);
    float height = (float)(camera->display_pixel_bounds.y1 - camera->display_pixel_bounds.y0);
    float width_scale = 1.0f;
    float height_scale = 1.0f;
    if (width > render_cameras_projection_epsilon)
        width_scale = k_render_camera_width_scale / width;
    if (height > render_cameras_projection_epsilon)
        height_scale = k_render_camera_height_scale / height;
    projection->world_to_screen_size.i = ((parameters.width * parameters.scale_x) * width_scale) * 0.5f;
    projection->world_to_screen_size.j = ((parameters.height * parameters.scale_y) * height_scale) * 0.5f;
    projection->frustum = parameters.frustum;
    real_plane3d plane;
    if (far_clip == 0.0f)
        matrix4x3_transform_plane(&projection->world_to_view, &camera->clip_plane, &plane);
    else {
        plane.n.i = 0.0f;
        plane.n.j = 0.0f;
        plane.n.k = 1.0f;
        plane.d = -far_clip;
    }
    float far_distance = -plane.d / plane.n.k;
    float near_distance = near_clip;
    float depth_scale = near_distance / ((fabsf(plane.n.i / plane.n.k) + fabsf(plane.n.j / plane.n.k) + 1.0f) * (near_distance - far_distance));
    plane.n.i = ((1.0f / plane.n.k) * depth_scale) * plane.n.i;
    plane.n.j = (depth_scale / plane.n.k) * plane.n.j;
    plane.n.k = depth_scale;
    plane.d = -depth_scale * far_distance;
    if (plane.d > 0.0f && far_clip == 0.0f) {
        plane.n.i = -plane.n.i;
        plane.n.j = -plane.n.j;
        plane.n.k = -plane.n.k;
        plane.d = -plane.d;
    }
    memset(projection->projection_matrix, 0, sizeof(projection->projection_matrix));
    projection->projection_matrix[0][0] = parameters.scale_x;
    projection->projection_matrix[0][2] = -plane.n.i;
    projection->projection_matrix[1][1] = parameters.scale_y;
    projection->projection_matrix[1][2] = -plane.n.j;
    projection->projection_matrix[2][0] = -parameters.center_x;
    projection->projection_matrix[2][1] = -parameters.center_y;
    projection->projection_matrix[2][2] = -plane.n.k;
    projection->projection_matrix[2][3] = -1.0f;
    projection->projection_matrix[3][2] = plane.d;
}
// @address 0x148ff8 render_camera_compute_projection_parameters
extern "C" void render_camera_compute_projection_parameters(
    // match: 0x148ff8: __restrict on projection keeps camera bounds loads above projection stores (frame 0xC0->0xB0 + reorder without it)
    const render_camera *camera, const real_rectangle2d *bounds, render_camera_projection *__restrict projection, float aspect_override)
{
    real_point2d scale;
    scale.x = (float)camera->display_pixel_bounds.x1 / (float)camera->fullscreen_pixel_bounds.x1;
    scale.y = (float)camera->display_pixel_bounds.y1 / (float)camera->fullscreen_pixel_bounds.y1;
    projection->width = (float)(camera->window_pixel_bounds.x1 - camera->window_pixel_bounds.x0) * scale.x;
    projection->height = (float)(camera->window_pixel_bounds.y1 - camera->window_pixel_bounds.y0) * scale.y;
    if (!bounds)
    {
        projection->bounds.y1 = 1.0f;
        projection->bounds.x1 = 1.0f;
        projection->bounds.y0 = -1.0f;
        projection->bounds.x0 = -1.0f;
    }
    else
        projection->bounds = *bounds;
    float half_field_of_view = render_cameras_half_field_of_view(camera);
    projection->half_width = (projection->bounds.x1 - projection->bounds.x0) * 0.5f;
    projection->half_height = (projection->bounds.y1 - projection->bounds.y0) * 0.5f;
    projection->center_x = (projection->bounds.x0 + projection->bounds.x1) / projection->half_width * -0.5f;
    projection->center_y = (projection->bounds.y0 + projection->bounds.y1) / projection->half_height * -0.5f;
    const render_cameras_projection_factors &initial =
        render_cameras_compute_projection_factors(projection, half_field_of_view);
    float tangent = initial.tangent;
    float aspect = initial.aspect;
    if (aspect_override == 0.0f)
    {
        float target_aspect = 0.0f;
        target_aspect = rasterizer_main_get_video_aspect_ratio();
        rectangle2d display;
        c_rasterizer::get_display_pixel_bounds(&display);
        float display_aspect = (float)(display.x1 - display.x0) / (float)(display.y1 - display.y0);
        if (fabs(display_aspect - target_aspect) >= render_cameras_projection_epsilon)
            aspect = (aspect / display_aspect) * target_aspect;
    }
    else
        aspect = aspect_override;
    bool cinematic = render_cameras_cinematic_active();
    if (cinematic != 0)
    {
        if (rasterizer_main_get_video_mode()->fIsWideScreen != 0)
        {
            if (g_render_camera_cinematic_widescreen_scale_enabled != 0)
            {
                tangent = (initial.tangent * k_observer_default_aspect_ratio) * k_render_camera_cinematic_widescreen_scale;
            }
        }
        else
        {
            rectangle2d display;
            c_rasterizer::get_display_pixel_bounds(&display);
            const render_cameras_vertical_factors &vertical =
                render_cameras_compute_vertical_factors(projection, display.y1);
            projection->bounds.y0 = vertical.lower * vertical.scale;
            projection->bounds.y1 = vertical.scale * vertical.upper;
        }
    }
    projection->scale_y = 1.0f / (projection->half_height * tangent);
    projection->scale_x = 1.0f / (projection->half_width * aspect * tangent);
    float inverse_y = 1.0f / projection->scale_y;
    float inverse_x = 1.0f / projection->scale_x;
    projection->left = -((projection->center_x + 1.0f) * inverse_x);
    projection->right = -((projection->center_x - 1.0f) * inverse_x);
    projection->bottom = -((projection->center_y + 1.0f) * inverse_y);
    projection->top = -((projection->center_y - 1.0f) * inverse_y);
}


static __forceinline s_first_person_weapon_user_data *render_cameras_first_person_get(long user_index)
{
    return &((s_first_person_weapon_user_data *)g_first_person_weapon_data)[user_index];
}

static __forceinline s_first_person_weapon_data *render_cameras_first_person_weapon_get(s_first_person_weapon_user_data *first_person, long slot)
{
    return &first_person->weapons[slot];
}

static __forceinline const real_matrix4x3 *render_cameras_first_person_adjustment(long user_index)
{
    s_first_person_weapon_data *weapon = render_cameras_first_person_weapon_get(render_cameras_first_person_get(user_index), 0);
    const real_matrix4x3 *result = 0;
    if (weapon->hands_node_count != -1 && (weapon->flags & 16))
        result = &weapon->camera_adjustment;
    return result;
}

static __forceinline bool render_cameras_first_person_hidden()
{
    unsigned long skulls = tls_game_globals()->options.game_mode == 1 ? tls_game_globals()->options.secondary_skulls : 0;
    return (skulls & 2) != 0;
}

static __forceinline real_matrix4x3 *render_cameras_object_node_matrices(long object_index, long *node_count)
{
    object_datum *object = object_get(object_index);
    *node_count = object->node_matrices.size / sizeof(real_matrix4x3);
    return (real_matrix4x3 *)object_header_block_get(object_get(object_index), &object->node_matrices);
}

static __forceinline void render_cameras_first_person_copy_model(long object_index, long render_model_index,
    long node_count, const real_matrix4x3 *nodes, render_first_person_model *model)
{
    model->object_index = object_index;
    model->render_model_index = render_model_index;
    model->flags = 0;
    if ((unsigned long)node_count > 64)
        node_count = 64;
    memcpy(model->nodes, nodes, node_count * sizeof(real_matrix4x3));
}

// @address 0x1534c0 render_camera_build_first_person_models
extern "C" long render_camera_build_first_person_models(long user_index, long object_index, long maximum_model_count, render_first_person_model *models)
{
    maximum_model_count = 4;
    s_first_person_weapon_user_data *first_person = render_cameras_first_person_get(user_index);
    bool active = false;
    bool visible = false;
    long model_count = 0;
    for (long slot = 0; slot < 2; ++slot)
    {
        s_first_person_weapon_data *weapon = render_cameras_first_person_weapon_get(first_person, slot);
        active |= (weapon->flags & 2) != 0;
        visible |= (weapon->flags & 1) != 0;
    }
    real_matrix4x3 camera;
    c_player_view::get_player_render_camera_orientation(&camera);
    const real_matrix4x3 *adjustment = render_cameras_first_person_adjustment(user_index);
    if (adjustment)
    {
        real_matrix4x3 inverse;
        matrix4x3_inverse(adjustment, &inverse);
        matrix4x3_compute_product(&camera, &inverse, &camera);
    }
    memcpy(&first_person->camera_matrix, &camera, sizeof(camera));
    if (active && visible && !render_cameras_first_person_hidden() &&
        first_person->unit_index == object_index && first_person->unit_index != -1)
    {
        render_first_person_model *unit_model = 0;
        for (long slot = 0; slot < 2; ++slot)
        {
            s_first_person_weapon_data *weapon = render_cameras_first_person_weapon_get(first_person, slot);
            if ((weapon->flags & 2) && (weapon->flags & 1))
            {
                render_first_person_model *weapon_model = 0;
                if (!unit_model && model_count < maximum_model_count && (weapon->flags & 8) && weapon->object_indices[6] != -1)
                {
                    unit_model = &models[model_count];
                    ++model_count;
                }
                if (unit_model && weapon->object_indices[6] != -1)
                    first_person_weapon_build_model(object_index, weapon->object_indices[6], 1, &camera,
                        weapon->node_matrix_count, weapon->node_matrices, weapon->hands_node_secondary_remapping, unit_model);
                if (model_count < maximum_model_count && (weapon->flags & 4) && weapon->object_indices[4] != -1)
                {
                    weapon_model = &models[model_count];
                    ++model_count;
                    first_person_weapon_build_model(weapon->object_indices[2], weapon->object_indices[4], 1, &camera,
                        -1, weapon->node_matrices, weapon->weapon_node_remapping, weapon_model);
                }
                if (slot == 0 && unit_model && weapon_model)
                    first_person_weapon_apply_weapon_ik(user_index, unit_model, weapon_model);
            }
        }
    }
    if (model_count < maximum_model_count && first_person->object_indices[7] != -1 &&
        first_person->unit_index == object_index && first_person->unit_index != -1)
    {
        long render_model_index = first_person->object_indices[7];
        render_model_definition *render_model = TAG_GET('mode', render_model_definition, render_model_index);
        object_datum *object = object_get(object_index);
        s_object_definition *definition = TAG_GET('obje', s_object_definition, object->definition_index);
        if (definition->model.index != -1)
        {
            s_model_definition *model = TAG_GET('hlmt', s_model_definition, definition->model.index);
            if (model->render_model_index != -1)
            {
                render_model_definition *object_render_model = TAG_GET(
                    'mode', render_model_definition, model->render_model_index);
                long node_count = 0;
                real_matrix4x3 *nodes = render_cameras_object_node_matrices(object_index, &node_count);
                if (render_model->nodes.count == object_render_model->nodes.count &&
                    (unsigned long)render_model->node_list_checksum == (unsigned long)object_render_model->node_list_checksum &&
                    (render_model->dont_draw_over_camera_cosine_angle == 0.0f || camera.forward.k < render_model->dont_draw_over_camera_cosine_angle))
                {
                    render_first_person_model *model = &models[model_count];
                    render_cameras_first_person_copy_model(object_index, render_model_index, node_count, nodes, model);
                    ++model_count;
                }
            }
        }
    }
    return model_count;
}
