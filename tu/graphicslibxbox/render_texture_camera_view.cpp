// graphicslibxbox/render_texture_camera_view.obj: from the records 0x45db10 0x4664e8 0x466650 0x466760 0x466828 0x466870 0x466e58
// @flags /TP /O2 /Oc
#include "render/camera_fx_globals_types.h"
#include "camera/observer_globals_types.h"
#include "render/render_object_storage.h"
#include "render/render_simple_lights.h"
#include "render/render_texture_camera_calls.h"
#include "memory/tls_globals.h"
#include "rasterizer/rasterizer_viewport_state.h"
#include "rasterizer/rasterizer_main_globals.h"
#include "render/render_camera_types.h"
#include "objects/object_reference_types.h"
#include "math/real_math.h"
#include <string.h>
#include "objects/objects.h"
#include "render/render.h"
#include "render/render_visibility_collection.h"
#include "render/render_cameras.h"
#include "render/render_structure.h"
#include "render/render_structure_lighting.h"
#include "rasterizer/rasterizer_states.h"
#include "render/views/render_player_view.h"
#include "render/views/render_view.h"
#include "render/render_objects.h"
#include "sky_atm/atmosphere.h"
#include "sky_atm/atmosphere_fog_interface.h"
#include "render/render_transparents.h"
#include "render/render_lights.h"
#include "visibility/visibility_collection.h"
#include "render/render_structure_globals_types.h"
#include "render/render_object_globals_types.h"
#include "render/render_texture_camera_view_globals.h"
// @address 0x8368d0 g_player_render_tuning
#include "rasterizer/dx9/rasterizer_dx9_device.h"
#include "rasterizer/rasterizer_render_targets.h"
#include "render_methods/render_method_submit_globals.h"




#include "rasterizer/rasterizer_textures_cache.h"
#include "rasterizer/rasterizer.h"
struct s_observer_result;

enum e_render_texture_camera_mode
{
    _render_texture_camera_mode_default,
    _render_texture_camera_mode_object
};

enum e_render_texture_camera_target_type
{
    _render_texture_camera_target_object,
    _render_texture_camera_target_point
};

#include "render/render_texture_camera_globals.h"



#include "render/render_view.h"

#include "render/views/player_view_types.h"

#include "render/views/texture_camera_view.h"




typedef char texture_camera_offset_check_129[(sizeof(void *) != 4 || ((offsetof(c_texture_camera_view, lights_view) == 0xbe0))) ? 1 : -1];
typedef char texture_camera_offset_check_130[(sizeof(void *) != 4 || ((offsetof(c_texture_camera_view, user_index) == 0x29b4))) ? 1 : -1];
typedef char texture_camera_offset_check_131[(sizeof(void *) != 4 || ((offsetof(c_texture_camera_view, m_texture) == 0x29e0))) ? 1 : -1];
typedef char texture_camera_offset_check_132[(sizeof(void *) != 4 || ((offsetof(c_texture_camera_view, m_flags) == 0x2a04))) ? 1 : -1];
typedef char texture_camera_offset_check_133[(sizeof(void *) != 4 || ((offsetof(c_texture_camera_view, m_callback) == 0x2a0c))) ? 1 : -1];

#include "render/render_lightmap_shadows_globals.h"
#include "render/object_renderer.h"
#include "render/structure_renderer.h"
#include "render/camera_fx_globals.h"
#include "sky_atm/weighted_atmosphere_parameters.h"

static __forceinline long render_texture_camera_get_render_mode()
{
    return tls_render_texture_camera_globals()->render_mode;
}

static __forceinline long render_texture_camera_get_target_object_index()
{
    long object_index = -1;
    if (tls_render_texture_camera_globals()->target_type == _render_texture_camera_target_object)
    {
        long target_object_index = tls_render_texture_camera_globals()->target_object_index;
        if (object_try_and_get_and_verify_type(target_object_index, 0xffffffff))
            object_index = target_object_index;
    }
    return object_index;
}

// @address 0x466760 ?compute_visibility@c_texture_camera_view@@UAAXJ@Z
void c_texture_camera_view::compute_visibility(long user_index)
{
    c_texture_camera_view *view = this;
    if (render_texture_camera_get_render_mode() == _render_texture_camera_mode_object)
    {
        long object_index = render_texture_camera_get_target_object_index();
        if (object_index != -1)
            render_visibility_add_object_to_items(object_index, user_index, true, true);
    }
    else
    {
        s_cluster_reference starting_cluster;
        unsigned short invalid_cluster = 0xffff;
        memcpy(&starting_cluster, &invalid_cluster, sizeof(starting_cluster));
        view->get_starting_cluster(&starting_cluster);
        render_visibility_camera_collection_compute(view->get_render_camera(), starting_cluster, view->get_render_projection(),
            view->m_view_index, user_index, (view->m_flags & 0x400) != 0);
    }
}

static __forceinline void render_texture_camera_view_begin(rectangle2d rectangle, rectangle2d rectangle2)
{
    c_rasterizer::set_viewport(rectangle, 0.0f, 1.0f);
    g_rasterizer_last_rectangles.viewport = rectangle;
    c_rasterizer::set_scissor_rect(&rectangle2);
    g_rasterizer_last_rectangles.scissor = rectangle2;
}
// @address 0x45db10 ?render_setup_internal@c_lightmap_shadows_view@@QBAXXZ
void c_lightmap_shadows_view::render_setup_internal() const
{
    render_texture_camera_view_begin(get_rasterizer_camera()->fullscreen_pixel_bounds, get_rasterizer_camera()->fullscreen_pixel_bounds);
    render_setup_window(get_rasterizer_camera(), get_rasterizer_projection());
}

struct render_structure_globals;
static __forceinline void render_texture_camera_structure_reset()
{
    g_render_structure_globals.current_a = 0;
    g_render_structure_globals.current_b = 0;
    g_render_structure_globals.depth = -1;
    g_cluster_sorted_order.m_count = 0;
    g_render_structure_globals.stack_b[5] = -1;
    g_render_structure_globals.cached_definition_index = -1;
    g_render_structure_globals.cached_instance_marker = -1;
}
static __forceinline void render_texture_camera_structure_push()
{
    ++g_render_structure_globals.depth;
    g_render_structure_globals.cluster_markers[g_render_structure_globals.depth] =
        g_render_structure_globals.current_a;
    g_render_structure_globals.instance_markers[g_render_structure_globals.depth] =
        g_render_structure_globals.current_b;
}
static __forceinline void render_texture_camera_structure_pop()
{
    --g_render_structure_globals.depth;
    g_render_structure_globals.current_a =
        g_render_structure_globals.cluster_markers[g_render_structure_globals.depth + 1];
    g_cluster_sorted_order.m_count = (unsigned short)g_render_structure_globals.current_a;
    g_render_structure_globals.current_b =
        g_render_structure_globals.instance_markers[g_render_structure_globals.depth + 1];
}
static __forceinline void render_texture_camera_object_reset()
{
    s_render_object_globals *g = g_render_object_globals;
    g->current_a = 0;
    g->current_b = 0;
    g->marker = -1;
    g->active = false;
}
static __forceinline void render_texture_camera_object_push()
{
    s_render_object_globals *g = g_render_object_globals;
    ++g->marker;
    g->stack_a[g->marker] = g->current_a;
    g->stack_b[g->marker] = g->current_b;
}
static __forceinline void render_texture_camera_object_pop()
{
    s_render_object_globals *g = g_render_object_globals;
    --g->marker;
    g->current_a = g->stack_a[g->marker + 1];
    g->current_b = g->stack_b[g->marker + 1];
}


// @address 0x466e58 texture_camera_view_render_texture_camera
extern "C" void texture_camera_view_render_texture_camera(c_texture_camera_view *view, long view_index)
{
    player_view_setup_lighting(view_index, view->view_index);
    c_visible_items::clear_all();
    c_transparency_renderer::m_current_marker_index = -1;
    c_transparency_renderer::m_total_transparent_count = 0;
    render_texture_camera_object_reset();
    render_texture_camera_structure_reset();
    c_transparency_renderer::transparent_sorted_order.clear();
    c_visible_items::push_marker();
    render_texture_camera_object_push();
    c_transparency_renderer::push_marker();
    render_texture_camera_structure_push();
    view->compute_visibility(view_index);
    view->render_submit_visibility();
    view->render();
    render_texture_camera_structure_pop();
    c_transparency_renderer::pop_marker();
    render_texture_camera_object_pop();
    c_visible_items::pop_marker();
}

// ===== 0x466870 c_texture_camera_view::render =====
struct D3DDevice;
struct D3DSurface;


// @address 0x466828 ?render_submit_visibility@c_texture_camera_view@@UAAXXZ
void c_texture_camera_view::render_submit_visibility()
{
    bool visible = (m_flags & 0x80) != 0;
    ::c_structure_renderer::submit_visibility(visible);
    ::c_object_renderer::submit_visibility(visible ? 3 : 0);
}










extern "C" {
// @address 0x8c484b g_texture_camera_render_active
unsigned char g_texture_camera_render_active;
// @address 0x73a15c g_render_method_submit_cached_definition_index
// @address 0x81dc04 g_view_stack

// @address 0x730e44 g_texture_camera_exposure
float g_texture_camera_exposure = 1.0f;
// @address 0x730e48 g_texture_camera_exposure_multiplier
float g_texture_camera_exposure_multiplier = 1.0f;
}
// @address 0x7bb5c8 g_albedo_buffer_source_texture
c_rasterizer_texture_ref g_albedo_buffer_source_texture;
// @address 0x7bb5cc g_normal_buffer_source_texture
c_rasterizer_texture_ref g_normal_buffer_source_texture;

// @rdata 0x820221fc
static const float k_render_texture_camera_one = 1.0f;
// @rdata 0x8202251c
static const float k_render_texture_camera_zero = 0.0f;

static __forceinline void render_texture_camera_build_simple_lights(c_lights_view *lights)
{
    render_lights_iterate_lights(lights, lights->user_index, 0, g_player_render_tuning.simple_lighting_pass, 0, 1, lights->exposure,
        render_lights_add_simple_light);
}

static __forceinline void render_texture_camera_restore_viewport_and_scissor()
{
    c_rasterizer::set_viewport(g_rasterizer_last_rectangles.viewport, k_render_texture_camera_zero, k_render_texture_camera_one);
    c_rasterizer::set_scissor_rect(&g_rasterizer_last_rectangles.scissor);
}

static __forceinline void render_texture_camera_clear_simple_lights(c_lights_view *lights, long user_index)
{
    lights->simple_light_count = 0;
    memset(lights->simple_light_data, 0, sizeof(lights->simple_light_data));
    lights->user_index = user_index;
}

static __forceinline void render_texture_camera_submit_simple_lights(c_lights_view *lights)
{
    render_lights_submit_simple_light_draw_list_to_shader(lights->simple_light_data, lights->simple_light_count);
}

static __forceinline void render_texture_camera_set_albedo_buffer_source_texture(c_rasterizer_texture_ref albedo, c_rasterizer_texture_ref normal)
{
    g_albedo_buffer_source_texture = albedo;
    g_normal_buffer_source_texture = normal;
}

static __forceinline void render_texture_camera_render_lights(c_lights_view *lights, long user_index,
    long window_index, D3DSurface *surface_a, D3DSurface *surface_b, D3DSurface *surface_c)
{
    lights->render_target1 = surface_b;
    lights->render_target0 = surface_a;
    lights->user_index = user_index;
    lights->depth_stencil = surface_c;
    const s_player_tuning &settings = g_player_render_tuning;
    render_lights_iterate_lights(
        lights, lights->user_index, window_index, settings.lighting_pass, 1, 0, lights->exposure, (void (*)(int, int, float,
        void *))render_lights_render_light);
}

// @address 0x466870 ?render@c_texture_camera_view@@UAAXXZ
void c_texture_camera_view::render()
{
    c_texture_camera_view *view = this;
    c_rasterizer_texture_ref texture;

    g_texture_camera_render_active = 1;
    c_view::begin(this);
    g_render_method_submit_cached_definition_index = -1;
    c_rasterizer::set_render_target(2, (c_rasterizer::e_surface)0);
    c_rasterizer::set_render_target(3, (c_rasterizer::e_surface)0);
    if (view->m_flags & 0x20)
    {
        render_texture_camera_clear_simple_lights(&view->lights_view, 0);
        view->lights_view.exposure = camera_fx_globals->exposure_scale;
        if (view->m_flags & 0x200)
            render_texture_camera_build_simple_lights(&view->lights_view);
        c_rasterizer::end_high_quality_blend();
        D3DDevice_SetRenderTarget_External(c_rasterizer::get_device(), 0, view->m_albedo_surface);
        D3DDevice_SetRenderTarget_External(c_rasterizer::get_device(), 1, view->m_normal_surface);
        rasterizer_set_depth_stencil_surface_direct(view->m_depth_surface);
        c_rasterizer::clear(0x10 | ((view->m_flags & 1) ? 0x0f : 0), 0x80808080, k_render_texture_camera_one, 0);
        if (view->m_flags & 8)
            c_object_renderer::render_albedo(1);
        if (view->m_flags & 4)
            c_structure_renderer::render_albedo();
        if (view->m_flags & 8)
            player_view_render_pass(0, view->user_index);
        player_view_render_transparent_objects((view->m_flags >> 3) & 1, 0);
        D3DDevice_SetRenderTarget_External(c_rasterizer::get_device(), 0, view->m_albedo_surface);
        D3DDevice_SetRenderTarget_External(c_rasterizer::get_device(), 1, view->m_normal_surface);
        render_texture_camera_restore_viewport_and_scissor();
        c_rasterizer::begin_high_quality_blend();
        texture = view->m_albedo_texture;
        c_rasterizer::resolve_entire_surface(texture, 0);
        texture = view->m_normal_texture;
        c_rasterizer::resolve_entire_surface(texture, 1);
        D3DDevice_SetRenderTarget_External(c_rasterizer::get_device(), 0, view->m_render_surface);
        render_texture_camera_set_albedo_buffer_source_texture(view->m_albedo_texture, view->m_normal_texture);
        rasterizer_set_global_shader_parameter(1);
        rasterizer_set_global_shader_parameter(2);
        c_rasterizer::set_render_target(1, (c_rasterizer::e_surface)0);
        if (render_texture_camera_get_render_mode() == _render_texture_camera_mode_object || (view->m_flags & 2))
            c_rasterizer::clear(15, 0, k_render_texture_camera_one, 0);
        render_player_view_set_exposure_constants(g_texture_camera_exposure, g_texture_camera_exposure_multiplier, 7.0f, false);
        c_rasterizer::end_high_quality_blend();
        render_texture_camera_submit_simple_lights(&view->lights_view);
        c_rasterizer::set_using_albedo_sampler(true);
        if (view->m_flags & 8)
            player_view_render_opaque_objects(1);
        if (view->m_flags & 4)
            structure_renderer_render_lighting();
        if (view->m_flags & 8)
            player_view_render_pass(1, view->user_index);
        c_rasterizer::begin_high_quality_blend();
    }
    else if (view->m_flags & 0x40)
    {
        D3DDevice_SetRenderTarget_External(c_rasterizer::get_device(), 0, view->m_render_surface);
        rasterizer_set_depth_stencil_surface_direct(view->m_depth_surface);
        c_rasterizer::clear(31, 0xffffffff, k_render_texture_camera_one, 0);
        if (view->m_flags & 4)
            structure_renderer_render_static_lighting((c_rasterizer::e_z_buffer_mode)3);
        if (view->m_flags & 8)
        {
            c_object_renderer::render_static_lighting((c_rasterizer::e_z_buffer_mode)3);
        }
    }

    if (view->m_flags & 0x200)
        render_texture_camera_render_lights(&view->lights_view, view->view_index, view->user_index, view->m_render_surface, 0, view->m_depth_surface);
    if (view->m_flags & 0x100)
        player_view_render_lightmap_shadows(this);
    if ((view->m_flags & 0x20) && (view->m_flags & 0x80))
    {
        render_texture_camera_submit_simple_lights(&view->lights_view);
        c_rasterizer::set_using_albedo_sampler(false);
        unsigned int window_index = view->user_index;
        unsigned short count = (unsigned short)c_transparency_renderer::m_total_transparent_count;
        long index = ++c_transparency_renderer::m_current_marker_index;
        render_texture_camera_object_push();
        c_transparency_renderer::m_markers[index].first_transparent_index = count;
        c_transparency_renderer::transparent_sorted_order.set_count(count);
        c_visible_items::push_marker();
        render_objects_update_visible_subparts(window_index);
        ::c_object_renderer::submit_visibility(1);
        render_player_view_sort_transparents();
        c_transparency_renderer::render(true);
        c_visible_items::pop_marker();
        c_transparency_renderer::pop_marker();
        render_texture_camera_object_pop();
        render_player_view_sort_transparents();
        c_transparency_renderer::render(true);
        c_rasterizer::set_using_albedo_sampler(true);
    }
    if ((view->m_flags & 0x800) && view->m_callback != 0)
        view->m_callback(view->m_callback_context);

    texture = view->m_texture;
    c_rasterizer::resolve_entire_surface(texture, 0);
    g_albedo_buffer_source_texture.clear();
    g_normal_buffer_source_texture.clear();
    g_render_method_submit_cached_definition_index = -1;
    c_view::end();
    g_texture_camera_render_active = 0;
}


extern "C" {
}

// @address 0x466650 ?render_setup@c_texture_camera_view@@UAAXXZ
void c_texture_camera_view::render_setup()
{
    c_texture_camera_view *view = this;
    render_texture_camera_view_begin(view->get_rasterizer_camera()->fullscreen_pixel_bounds, view->get_rasterizer_camera()->fullscreen_pixel_bounds);
    render_setup_window(view->get_rasterizer_camera(), view->get_rasterizer_projection());
    render_player_view_update_particle_systems(view->view_index);
    s_cluster_reference cluster;
    view->get_starting_cluster(&cluster);
    render_objects_update_element_weights(cluster, &view->get_rasterizer_camera()->position);
    atmosphere_constants_index = -1;
    s_weighted_atmosphere_parameters parameters;
    parameters.valid = false;
    memset(&parameters.sun_color, 0, sizeof(parameters) - offsetof(s_weighted_atmosphere_parameters, sun_color));
    c_atmosphere_fog_interface::populate_atmosphere_parameters(cluster, &parameters);
    memcpy(&c_atmosphere_fog_interface::m_default_parameters, &parameters, sizeof(parameters));
    atmosphere_constants_index = -2;
    c_atmosphere_fog_interface::set_constant(&c_atmosphere_fog_interface::m_default_parameters, true, true);
    if (atmosphere_constants_index != -2)
    {
        atmosphere_constants_index = -2;
        c_atmosphere_fog_interface::set_constant(&c_atmosphere_fog_interface::m_default_parameters, true, true);
    }
}

extern "C" {
// @address 0x730e40 g_texture_camera_near_clip
float g_texture_camera_near_clip = 0.001f;
}
// @address 0x4664e8 ?setup_camera@c_texture_camera_view@@QAAXPBUs_observer_result@@J@Z
void c_texture_camera_view::setup_camera(const s_observer_result *observer, long user_index)
{
    render_camera *camera = &rasterizer_camera;
    m_view_index = -1;
    memset(&this->user_index, 0, 7 * sizeof(long));
    long width = 0;
    long height = 0;
    window_count = 1;
    m_texture.get_width_height(0, &width, &height);
    camera->fullscreen_pixel_bounds.x0 = camera->fullscreen_title_safe_pixel_bounds.x0 =
        camera->window_pixel_bounds.x0 = camera->window_title_safe_pixel_bounds.x0 = 0;
    camera->fullscreen_pixel_bounds.y0 = camera->fullscreen_title_safe_pixel_bounds.y0 =
        camera->window_pixel_bounds.y0 = camera->window_title_safe_pixel_bounds.y0 = 0;
    camera->display_pixel_bounds.x0 = 0;
    camera->display_pixel_bounds.y0 = 0;
    camera->display_pixel_bounds.x1 = 430;
    camera->display_pixel_bounds.y1 = 240;
    camera->fullscreen_pixel_bounds.x1 = camera->fullscreen_title_safe_pixel_bounds.x1 =
        camera->window_pixel_bounds.x1 = camera->window_title_safe_pixel_bounds.x1 = (short)width;
    camera->fullscreen_pixel_bounds.y1 = camera->fullscreen_title_safe_pixel_bounds.y1 =
        camera->window_pixel_bounds.y1 = camera->window_title_safe_pixel_bounds.y1 = (short)height;
    render_view_initialize_camera(camera, observer);
    float camera_value = observer->vertical_field_of_view;
    float &field_of_view_output = camera->vertical_field_of_view;
    camera->field_of_view_scale = observer->field_of_view_scale;
    camera->mirrored = false;
    camera->frustum_adjustment = false;
    field_of_view_output = camera_value;
    camera_value = g_texture_camera_near_clip;
    if (!(camera_value > 0.001f))
        camera_value = 0.001f;
    camera->near_clip = camera_value;
    real_rectangle2d bounds;
    render_camera_build_viewport_frustum_bounds(camera, &bounds);
    render_camera_build_projection(camera, &bounds, &rasterizer_projection, tls_render_texture_camera_globals()->projection_depth);
    memcpy(&render_camera_state, camera, sizeof(render_camera));
    memcpy(&render_projection_state, &rasterizer_projection, sizeof(render_projection));
}
