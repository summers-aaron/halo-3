// graphicslibxbox/render_view.obj: from the records 0x148798 0x153e00 0x17e710 0x17e848 0x18f918 0x191318 0x449278 0x449280 0x4493e8 0x449470 0x44dd68 0x44ded8
// @flags /TP /O2 /Oc
#include "camera/observer_globals_types.h"
#include "rasterizer/rasterizer_viewport_state.h"
#include "render/render_camera_types.h"
#include "objects/object_reference_types.h"
#include "cseries/crt_xdk.h"
#include "math/real_math_types.h"
#include <string.h>
#include <ppcintrinsics.h>
#include "scenario/scenario.h"
#include "rasterizer/rasterizer_main.h"
#include "render/render_cameras.h"
#include "interface/user_interface.h"
#include "render/views/render_view.h"
#include "math/matrix_math.h"
#include "rasterizer/rasterizer_video_mode_types.h"
#include "cache/cache_files.h"
#include "rasterizer/rasterizer_render_globals_types.h"
#include "rasterizer/rasterizer_render_targets.h"
#include "rasterizer/rasterizer_globals.h"
#include "render/render_water_globals.h"
#include "scenario/scenario_tag.h"
#include "tag_files/tag_types.h"
#include "rasterizer/rasterizer.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "structures/structure_bsp_tag.h"
#include "scenario/scenario_access.h"
#include "cubemaps/cubemap_sample.h"
#include "render/render_view.h"
#include "render/views/world_view.h"
#include "render/views/texture_camera_view.h"
#include "render/views/occlusion_view.h"
#include "render/views/simple_view_types.h"
#include "render/camera_fx_settings_types.h"
#include "render/camera_fx_values.h"
#include "math/real_math.h"
#include "render/ui_view.h"
#include "cache/restricted_memory_regions_externs.h"
#include "render/camera_fx_settings.h"
#include "render/render_player_view_camera_settings.h"
#include "render/views/patchy_fog.h"
#include "render/views/world_view_types.h"
#include "render/views/lightmap_shadows_view.h"
#include "render/render_player_view_observer_flags_types.h"
#include "math/real_math_trigonometry.h"
// @address 0x7cff08 ?global_scenario@@3PAUscenario@@A
// @address 0x7cff0c ?global_tag_instances@@3PBUcache_file_tag_instance@@B
// @address 0x8a1ecc ?g_active_structure_bsp_mask@@3KA
// @address 0x9bb88 ?scenario_leaf_index_from_point@@YAJPBTreal_point3d@@PAJ@Z
// @address 0x9e788 ?scenario_cluster_reference_from_leaf_index@@YA?AUs_cluster_reference@@JJ@Z
// @address 0x17c680 ?render_submit_visibility@c_occlusion_view@@UAAXXZ
// @address 0x45ca80 ?render_setup@c_lights_view@@UAAXXZ
// @address 0x164540 ?render@c_player_view@@UAAXXZ
// @address 0x81da20 g_rasterizer_last_rectangles

// @address 0x73a1ac ?g_view_stack_top@c_view@@0JA
long c_view::g_view_stack_top = -1;

// @address 0x81dc04 g_view_stack
extern "C" { c_view *g_view_stack[4]; }

// @address 0x8c46dc ?x_current_player_view@c_player_view@@1PAV1@A
c_player_view *c_player_view::x_current_player_view;

struct s_observer_result;


template<typename T> static __forceinline const T &render_view_max(const T &a, const T &b)
{ return a > b ? a : b; }
template<typename T> static __forceinline const T &render_view_min(const T &a, const T &b)
{ return a < b ? a : b; }

__forceinline c_world_view::c_world_view()
{
    starting_cluster_invalid = 0;
    starting_cluster = invalid_cluster_reference_get();
}

// @address 0x153e00 ?get_starting_cluster@c_world_view@@QAAXPAUs_cluster_reference@@@Z
// match: 0x153e00: pointer __restrict qualifier prevents alias reload
void c_world_view::get_starting_cluster(s_cluster_reference *__restrict starting_cluster)
{
    long bsp_index;
    long leaf_index = scenario_leaf_index_from_point(&this->render_camera_state.position, &bsp_index);
    s_cluster_reference value;
    if (leaf_index != -1)
        value = scenario_cluster_reference_from_leaf_index(bsp_index, leaf_index);
    else
        *(unsigned short *)&value = (unsigned short)-1;
    *starting_cluster = value;
    if (starting_cluster->bsp_index != -1)
    {
        this->starting_cluster = *starting_cluster;
        this->starting_cluster_invalid = 0;
    }
    else
    {
        signed char previous_bsp_index = this->starting_cluster.bsp_index;
        this->starting_cluster_invalid = 1;
        long selected_index;
        if (previous_bsp_index != -1 && (g_active_structure_bsp_mask & (1 << previous_bsp_index)) != 0)
            selected_index = previous_bsp_index;
        else
            selected_index = global_structure_bsp_first_active_index_get();
        signed char selected_bsp = (signed char)selected_index;
        starting_cluster->bsp_index = selected_bsp;

        scenario *scenario = global_scenario;
        long zero = 0;
        scenario_structure_bsp_reference *entries = (scenario_structure_bsp_reference *)scenario->structure_bsps.address;
        scenario_structure_bsp_reference *entry = &entries[selected_bsp];
        bsp_index = this->starting_cluster.cluster_index;
        const structure_bsp *bsp = (const structure_bsp *)global_tag_instances[(unsigned short)entry->structure_bsp.index].definition;
        long upper = bsp->clusters.count - 1;
        starting_cluster->cluster_index = (unsigned char)render_view_min(render_view_max(bsp_index, zero), upper);
    }
}

// @address 0x148950 ?render_camera_build_viewport_frustum_bounds@@YA_NPBUrender_camera@@PATreal_rectangle2d@@@Z

// @address 0x449278 ?render_setup@c_occlusion_view@@UAAXXZ
void c_occlusion_view::render_setup()
{
    render_setup_internal();
}

// @address 0x4493e8 ?setup_camera@c_fullscreen_view@@QAAXPBUs_observer_result@@@Z
void c_fullscreen_view::setup_camera(const s_observer_result *observer)
{
    real_rectangle2d viewport_bounds;
    memset(&this->rasterizer_camera, 0, sizeof(render_camera));
    ::render_view_compute_fullscreen_bounds(&this->rasterizer_camera);
    render_view_initialize_camera(&this->rasterizer_camera, 0);
    render_camera_build_viewport_frustum_bounds(&this->rasterizer_camera, &viewport_bounds);
    render_camera_build_projection(&this->rasterizer_camera, &viewport_bounds, &this->rasterizer_projection, 0.0f);
    memcpy(&this->render_camera_state, &this->rasterizer_camera, sizeof(render_camera));
    memcpy(&this->render_projection_state, &this->rasterizer_projection, sizeof(render_projection));
}

// @address 0x449470 ?render_blank_frame@c_fullscreen_view@@SAXPBTreal_rgb_color@@@Z
void c_fullscreen_view::render_blank_frame(const real_rgb_color *color)
{
    float clear_color[4];
    c_rasterizer::set_render_target(0, (c_rasterizer::e_surface)1);
    c_rasterizer::set_depth_stencil_surface((c_rasterizer::e_surface)0);
    float alpha_multiplier = c_rasterizer::get_render_target_alpha_multiplier(0);
    float scale = 1.0f / alpha_multiplier;
    clear_color[0] = color->red * scale;
    clear_color[1] = color->green * scale;
    clear_color[2] = color->blue * scale;
    clear_color[3] = 1.0f;
    c_rasterizer::clearf(15, (const real_vector4d *)clear_color, 1.0f, 1);
}

// @address 0x58e518 _blkmov

// @address 0x44dd68 ?set@c_camera_fx_values@@QAAXPBVc_camera_fx_settings@@@Z

void c_camera_fx_values::set(const c_camera_fx_settings *settings)
{
    this->set_exposure(settings->exposure.value);
    this->index0 = -1;
    this->surface = -1;
    this->previous = 0.0f;
    this->exposure_sensitivity = settings->exposure_sensitivity.value;
    this->white_point = settings->white_point.value;
    this->bloom_intensity = settings->bloom_intensity.value;
    this->bloom_threshold = settings->bloom_threshold.value;
    this->bloom_brightness = settings->bloom_brightness.value;
    this->color_balance = settings->color_balance.color;
    this->color_contrast = settings->color_contrast.color;
    this->color_saturation = settings->color_saturation.color;
    this->parameter144 = settings->parameter144.value;
    this->parameter176 = settings->parameter176.value;
    this->parameter192 = settings->parameter192.value;
}

// @address 0x9b308 ?normalize3d@@YAMPATreal_vector3d@@@Z
// @address 0xf25a0 ?matrix4x3_inverse@@YAXPBUreal_matrix4x3@@PAU1@@Z


// @address 0x17e710 ?setup_camera@c_lightmap_shadows_view@@QAAXJJ@Z
void c_lightmap_shadows_view::setup_camera(long width, long height)
{
    short pixel_width = (short)width;
    short pixel_height = (short)height;
    render_camera *camera = &rasterizer_camera;
    rectangle2d &title_bounds = camera->fullscreen_title_safe_pixel_bounds;
    title_bounds.x1 = pixel_width;
    camera->fullscreen_pixel_bounds.x1 = pixel_width;
    title_bounds.y1 = pixel_height;
    title_bounds.x0 = 0;
    camera->fullscreen_pixel_bounds.x0 = 0;
    title_bounds.y0 = 0;
    short *render_y = camera->fullscreen_pixel_bounds.n + 2;
    render_y[-2] = 0;
    render_y[0] = pixel_height;
    point_from_line3d(&m_object_bounding_box.matrix.position, &m_object_bounding_box.matrix.forward,
        -2.0f * m_object_bounding_box.half_extents.i, &camera->position);
    const real_vector3d *forward = &m_object_bounding_box.matrix.forward;
    camera->forward = *forward;
    const real_vector3d *up = &m_object_bounding_box.matrix.up;
    camera->up = *up;
    camera->near_clip = m_object_bounding_box.half_extents.i;
    camera->far_clip = m_object_bounding_box.half_extents.i * 2.0f + camera->near_clip;
    camera->mirrored = false;
    camera->frustum_adjustment = false;
    camera->vertical_field_of_view = 0.0055555557f;
    camera->field_of_view_scale = 1.0f;
    render_camera_build_orthogonal_projection(&m_object_bounding_box, &camera->fullscreen_pixel_bounds, &rasterizer_projection);
    memcpy(&render_camera_state, camera, sizeof(render_camera_state));
    memcpy(&render_projection_state, &rasterizer_projection, sizeof(render_projection_state));
}

// match: 0x17e848: the shared cross_product3d swaps one fmadds pair in this function.
static __forceinline real_vector3d *render_view_cross_product3d(const real_vector3d *a, const real_vector3d *b, real_vector3d *result)
{
    result->i = a->j * b->k - a->k * b->j;
    result->j = a->k * b->i - a->i * b->k;
    result->k = a->i * b->j - a->j * b->i;
    return result;
}

// @address 0x17e848 ?render_camera_build_orthogonal_projection@@YAXPBUs_oriented_bounding_box@@PBTrectangle2d@@PAUrender_projection@@@Z
void render_camera_build_orthogonal_projection(
    const s_oriented_bounding_box *bounds, const rectangle2d *window_bounds, render_projection *projection)
{
    real_vector3d forward = bounds->matrix.forward;
    real_vector3d left;
    real_vector3d up;
    render_view_cross_product3d(&forward, &bounds->matrix.up, &left);
    render_view_cross_product3d(&left, &forward, &up);
    normalize3d(&left);
    normalize3d(&up);
    normalize3d(&forward);

    projection->view_to_world.forward = left;
    projection->view_to_world.left = up;
    projection->view_to_world.up = forward;
    point_from_line3d(&bounds->matrix.position, &bounds->matrix.forward, -bounds->half_extents.i, &projection->view_to_world.position);
    projection->view_to_world.scale = 1.0f;
    matrix4x3_inverse(&projection->view_to_world, &projection->world_to_view);

    float window_width = window_bounds->x1 - window_bounds->x0;
    projection->world_to_screen_size.i = window_width / (2.0f * bounds->half_extents.j);
    projection->world_to_screen_size.j = (float)(window_bounds->y1 - window_bounds->y0) / (2.0f * bounds->half_extents.k);
    projection->frustum.y0 = -1.0f;
    projection->frustum.x0 = -1.0f;
    projection->frustum.y1 = 1.0f;
    projection->frustum.x1 = 1.0f;

    float z_near = 0.0f;
    float z_far = bounds->half_extents.i * 2.0f;
    memset(projection->projection_matrix, 0, sizeof(projection->projection_matrix));
    projection->projection_matrix[0][0] = 1.0f / bounds->half_extents.j;
    projection->projection_matrix[3][0] = (float)(-1.0 / (window_bounds->x1 - window_bounds->x0));
    projection->projection_matrix[1][1] = 1.0f / bounds->half_extents.k;
    projection->projection_matrix[3][1] = (float)(1.0 / (window_bounds->y1 - window_bounds->y0));
    float z_range = z_far - z_near;
    projection->projection_matrix[2][2] = 1.0f / z_range;
    projection->projection_matrix[3][2] = -z_near / (z_far - z_near);
    projection->projection_matrix[3][3] = 1.0f;
}

// @address 0x191318 ?render@c_ui_view@@UAAXXZ
void c_ui_view::render()
{
    rectangle2d window_bounds;
    int widescreen = rasterizer_main_get_video_mode()->fIsWideScreen;
    if (widescreen)
    {
        window_bounds.y0 = 0;
        window_bounds.x0 = 0;
        window_bounds.x1 = (short)c_rasterizer::render_globals.width;
        window_bounds.y1 = (short)c_rasterizer::render_globals.height;
    }
    else
    {
        window_bounds.y0 = 0;
        window_bounds.x0 = 0;
        window_bounds.x1 = 640;
        window_bounds.y1 = 480;
    }
    user_interface_render_windows(-1, -1, 4, &window_bounds, m_render_target, m_is_screenshot);
}

// @address 0x44ded8 render_view_reset_camera_fx
extern "C" void render_view_reset_camera_fx()
{
    unsigned int users = 4;
    render_player_view_camera_settings *settings = (render_player_view_camera_settings *)restricted_region2_address();
    // match: 0x44ded8: a separate value cursor keeps two induction registers.
    char *values = (char *)&settings->values;
    do
    {
        camera_fx_settings_initialize(&settings->settings, true);
        ((c_camera_fx_values *)values)->set(&settings->settings);
        ++settings;
        values += sizeof(*settings);
    } while (--users != 0);
}

__forceinline c_patchy_fog::c_patchy_fog()
    {
        m_rotation = 0.0f;
        m_near_distance = 0.0f;
        m_sheet_distance = 0.0f;
        m_sheet_index = 0;
        memset(m_sheet_u, 0, sizeof(m_sheet_u));
        memset(m_sheet_v, 0, sizeof(m_sheet_v));
        m_data.position = *global_origin3d;
        m_data.forward = *global_zero_vector3d;
        m_data.left = *global_zero_vector3d;
    }

__forceinline c_first_person_view::c_first_person_view() {}

__forceinline c_lightmap_shadows_view::c_lightmap_shadows_view() {}

struct render_player_view_camera_settings;

typedef char render_view_player_size[(sizeof(void *) != 4 || ((sizeof(c_player_view) == 0x29e0))) ? 1 : -1];

// @address 0x449280 ??0c_player_view@@QAA@XZ
c_player_view::c_player_view()
{
    XMemSet(&last_world_to_view, 0, sizeof(last_world_to_view));
    XMemSet(&last_projection_matrix, 0, sizeof(last_projection_matrix));
}

// @address 0x8c46b8 g_antialiasing_mode
static __forceinline void render_view_set_viewport_and_scissor(rectangle2d viewport, rectangle2d scissor)
{
    c_rasterizer::set_viewport(viewport, 0.0f, 1.0f);
    g_rasterizer_last_rectangles.viewport = viewport;
    c_rasterizer::set_scissor_rect(&scissor);
    g_rasterizer_last_rectangles.scissor = scissor;
}
// @address 0x18f918 render_view_initialize_surface
extern "C" void render_view_initialize_surface(c_ui_view *view, long, long surface)
{
    render_camera *camera = &view->rasterizer_camera;
    view->m_render_target = surface;
    render_view_compute_fullscreen_bounds(camera);
    render_view_initialize_camera(camera, 0);
    g_antialiasing_mode = 0;
    render_view_set_viewport_and_scissor(view->rasterizer_camera.fullscreen_pixel_bounds, view->rasterizer_camera.fullscreen_pixel_bounds);
    if (c_rasterizer::get_render_target(0) != view->m_render_target) {
        c_rasterizer::set_render_target(0, (c_rasterizer::e_surface)view->m_render_target);
        c_rasterizer::clear(15, 0, 0.0f, 0);
    }
}

extern "C" {
// @address 0x7be754 k_render_default_horizontal_fov
float k_render_default_horizontal_fov = 70.0f * 0.01745329252f;
// @address 0x73a244 g_render_far_plane
}

static __forceinline float render_view_tangent(float angle)
{
    return (float)tan((double)angle);
}

static __forceinline float render_view_read_z_plane(const float *plane)
{
    return *plane;
}

static __forceinline void render_view_get_z_planes(float *near_plane, float *far_plane)
{
    *near_plane = render_view_read_z_plane(&global_z_near);
    *far_plane = render_view_read_z_plane(&g_render_far_plane);
}

// @address 0x148798 render_view_initialize_camera
extern "C" void render_view_initialize_camera(render_camera *camera, const s_observer_result *observer)
{
    if (observer)
    {
        float field_of_view_scale = 0.785f;
        camera->position = observer->position;
        camera->forward = observer->forward;
        camera->up = observer->up;
        camera->vertical_field_of_view = observer->vertical_field_of_view;
        camera->field_of_view_scale = observer->field_of_view_scale;
        camera->vertical_field_of_view *= field_of_view_scale;
        camera->field_of_view_scale *= field_of_view_scale;
    }
    else
    {
        camera->position = *global_origin3d;
        camera->forward = *global_forward3d;
        camera->up = *global_up3d;
        rectangle2d bounds;
        bounds.y0 = 0;
        bounds.x0 = 0;
        bounds.x1 = (short)c_rasterizer::render_globals.width;
        bounds.y1 = (short)c_rasterizer::render_globals.height;
        float aspect_ratio = (float)bounds.x1 / (float)bounds.y1;
        camera->vertical_field_of_view = 2.0f * arctangent(render_view_tangent(0.5f * k_render_default_horizontal_fov) / aspect_ratio, 1.0f);
        camera->field_of_view_scale = 1.0f;
    }
    camera->mirrored = false;
    render_view_get_z_planes(&camera->near_clip, &camera->far_clip);
    camera->frustum_adjustment = false;
}

c_player_view c_player_view::x_global_player_views[4];

