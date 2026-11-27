// @address 0x16f100 ?render_decorators@c_structure_renderer@@SAXXZ
// @address 0x16f0a8 ?rasterizer_set_packed_blend_state@@YAXPAUD3DDevice@@KUblend_state_value@@@Z
// @address 0x88e354 g_dynamic_render_target_indices
// graphicslibxbox/render_player_view.obj: from the records 0x147098 0x148240 0x152610 0x1528d0 0x152d40 0x153208 0x153830 0x153ac8 0x153b90 0x153c40 0x162838 0x16ad08 0x16bce0 0x16e730 0x16eb28 0x16edb0 0x170090 0x170730 0x175fb0 0x1764e0 0x177218 0x1787a8 0x1788b0 0x178b68 0x178d90 0x179140 0x1798d0 0x17bd18 0x17be18 0x17c010 0x17c680 0x17f490 0x17f7c8 0x17fdd0 0x17fe30 0x180018 0x180130 0x180eb0 0x44cc78 0x44ccb8 0x44cd20 0x44cdc0 0x44ce98 0x44d0b8 0x44d1f0 0x44d200 0x44d248 0x44d308 0x44d370 0x44d3c8 0x44d4b0 0x44d548 0x44d5e8 0x44d600 0x44f9d8
// @address 0x73042c g_interface_aspect_ratio_scaling
// @address 0x81da20 g_rasterizer_last_rectangles
// @address 0x8c46e4 g_rasterizer_distortion_visible
// @address 0x8c482e g_render_method_submit_cache_bypass
// @address 0x8c482f g_render_method_submit_cached_state_enabled
// @flags /TP /O2 /Oc
// @address 0x8c4694 g_rasterizer_device
// @address 0x81dab8 g_rasterizer_alpha_blend_mode
#include "rasterizer/rasterizer_device_state.h"
#include "game/player_control_globals.h"
#include "rasterizer/rasterizer_surface_globals.h"
// @address 0x72cd3a g_rasterizer_using_albedo_sampler
// @address 0x832a88 g_rasterizer_surface_groups
#include "game/game_engine_waypoint_constants.h"
#include "camera/observer_globals_types.h"
#include "camera/saved_film_director_storage.h"
#include "camera/director_storage_types.h"
#include "render/render_transparency_globals.h"
#include "rasterizer/rasterizer_shader_gpr_globals.h"
#include "rasterizer/rasterizer_tiling_globals.h"
#include "visibility/visible_items_types.h"
#include "visibility/visible_items.h"
#include "objects/light_globals.h"
#include "render/render_texture_camera_globals.h"
#include "render/render_object_storage.h"
#include "render/render_simple_lights.h"
#include "render/render_lights.h"
#include "gpu_particle/gpu_particle_globals.h"
#include "cseries/long_sort.h"
#include "memory/tls_data_arrays.h"
#include "rasterizer/rasterizer_device_globals.h"
#include "render/render_objects_globals.h"
struct s_data_array;

#include "cutscene/cinematic_globals_types.h"
#include "game/game_time_runtime.h"

#include "render/render_player_view_globals.h"
class c_player_view;



#include "rasterizer/rasterizer_main_globals.h"
#include "rasterizer/render_target_types.h"
#include "rasterizer/rasterizer_viewport_state.h"
#include "render/render_camera_types.h"
#include "objects/object_reference_types.h"
#include "math/real_math_types.h"
#include "memory/data_types.h"
#include "objects/object_placement.h"
#include "rasterizer/rasterizer_video_mode_types.h"
#include "render/render_player_view_observer_flags_types.h"
#include "render/render_visibility_collection_globals.h"
// @address 0x153830 ?get_player_render_camera_orientation@c_player_view@@SAXPAUreal_matrix4x3@@@Z
// @address 0x153b90 render_player_view_compute_pixel_scale
// @address 0x17fdd0 render_player_view_initialize_window_bounds
// @address 0x44cc78 ?disown_vertex_shader_constants@c_rasterizer@@SAXJJ@Z
// @address 0x44ccb8 render_player_view_compute_exposure_multiplier
// @address 0x44d1f0 ?__tls_set_g_render_texture_camera_globals_allocator@@YAXPAX@Z
// @address 0x44d308 render_texture_camera_set_position_object
// @address 0x167948 ?set_sampler_texture@c_rasterizer@@SAXJABVc_rasterizer_texture_ref@@@Z
#include "render/structure_renderer.h"
class c_object_renderer;
#include "render/object_renderer.h"

#include "cseries/cseries.h"
#include "memory/data.h"
#include "tag_files/tag_types.h"
#include "cache/restricted_memory.h"
#include "render_methods/render_method_definitions.h"
#include "rasterizer/rasterizer_occlusion_query_globals.h"
#include "scenario/scenario_tag.h"
#include "game/game_globals.h"
#define XVIDEO_MODE XDK_XVIDEO_MODE
#include <xtl.h>
#include "rasterizer/rasterizer_tiling_types.h"
#include "cache/restricted_memory_regions.h"
#include "game/game.h"
#include "objects/objects.h"
#undef XVIDEO_MODE
#include "rasterizer/rasterizer_occlusion_queries.h"
#include "cache/cache_files.h"
#include "render/render_visibility_collection.h"
#include "scenario/scenario.h"
#include "effects/effects_particle_system_entry_types.h"
#include "rasterizer/occlusion_query_types.h"
#include "effects/effects_particle_system_list_types.h"
#include "main/main_render.h"
#include "render/render_cameras.h"
#include "effects/particle_system.h"
#include "rasterizer/rasterizer_states.h"
#include "render/render_structure.h"
#include "render/render_structure_globals_types.h"
#include "render/views/render_player_view.h"
#include "sky_atm/atmosphere.h"
#include "render/render_lens_flares.h"
#include "render/views/render_view.h"
#include "bink/bink_playback.h"
#include "cutscene/cinematics.h"
#include "effects/effects.h"
#include "interface/chud/chud.h"
#include "interface/user_interface_controller.h"
#include "objects/lights.h"
#include "rasterizer/rasterizer_main.h"
#include "rasterizer/xenon/rasterizer_xenon_main.h"
#include "render/render_objects.h"
#include "render/render_transparents.h"
#include "render_methods/render_method_submit.h"
#include "visibility/visibility_render_objects.h"
#include "camera/observer.h"
#include "interface/interface_constants.h"
#include "render/render.h"
#include "sky_atm/atmosphere_fog_interface.h"
#include "math/real_math_functions.h"
#include "render/render_player_view_dependencies.h"
#include "game/player_mapping_types.h"
#include <math.h>
#include <string.h>

#include "math/matrix_math_types.h"
typedef real_vector3d vector3;


#include "rasterizer/rasterizer_render_globals_types.h"


#include "render/camera_fx_globals_types.h"
// @address 0x8c4704 g_postprocess_exposure_offset
float g_postprocess_exposure_offset;



#include "render/views/player_view_types.h"
#include "render/views/lightmap_shadows_view.h"



class c_rasterizer;
#include "rasterizer/rasterizer_vertex_definitions.h"

#include "rasterizer/rasterizer_textures_cache.h"
#include "rasterizer/rasterizer.h"
#include "rasterizer/rasterizer_sampler_texture_calls.h"





struct s_observer_result;
extern "C" {

// @address 0x416f8 k_default_particle_block_view
// @rdata 0x820416f8..0x82041768
const s_observer_result k_default_particle_block_view = { 0 };
// @rdata-end
// @address 0x7f1a10 g_particle_system_lists
// @address 0x8c43b0 g_particle_render_camera
const s_observer_result *g_particle_render_camera;

}

// @address 0x11df6a4 g_render_texture_camera_globals
// @address 0x11df894 g_player_view_camera_settings
extern "C" {
__declspec(thread) s_render_texture_camera_globals *g_render_texture_camera_globals;
__declspec(thread) render_player_view_camera_settings *g_player_view_camera_settings;
}

#include "visibility/visibility_collection.h"
#include "visibility/visibility_globals.h"
#include "visibility/visibility_region_types.h"






#include "math/real_math.h"
#include "math/point_math.h"
#include "cubemaps/cubemap_sample.h"
#include "render/render_texture_camera_allocator.h"
#include "cache/restricted_memory_layout.h"
#include "rasterizer/render_surface_types.h"
#include "rasterizer/rasterizer_render_target_types.h"
#include "render/render_object_globals_types.h"
#include "effects/decal_definition_types.h"
#include "effects/decal_system_types.h"
#include "render/camera_fx_values.h"
#include "../../splitscreen_view.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "structures/structure_bsp_tag.h"
#include "main/main_render_types.h"
#include "rasterizer/xenon/rasterizer_xenon_memory.h"
#include "sky_atm/weighted_atmosphere_parameters.h"
#include "sky_atm/sky_atm_parameters_types.h"
#include "render/render_texture_camera_types.h"
#include "objects/object_datum.h"
#include "render/views/occlusion_view.h"
#include "camera/director_globals_types.h"
#include "interface/interface_constants_dependencies.h"
#include "render/render_view.h"
#include "rasterizer/rasterizer_dynamic_render_target_types.h"
#include "render/render_texture_camera.h"
#include "render/views/simple_view_types.h"
#include "render/views/texture_camera_view.h"
#include "units/unit_types.h"
#include "render/render_lightmap_shadows_globals.h"
#include "game/player_control_types.h"
#include "rasterizer/rasterizer_render_targets.h"
#include "render/screen_postprocess.h"
#include "render/render_player_view.h"
#include "camera/camera_types.h"
#include "game/game_globals_storage.h"
#include "render/render_objects_submission_globals.h"
#include "memory/datum_index_inlines.h"
#include "memory/datum_index.h"
struct s_transparent_sort_workspace {
    unsigned short pivot;
    unsigned short front[1024];
    unsigned short back[1024];
    unsigned short front_count, back_count;
    unsigned short after_front[1024];
    unsigned short after_back[1024];
    unsigned short after_front_count, after_back_count;
};
typedef char render_view_zoom_offset[(offsetof(s_player_control_globals, states) + offsetof(s_player_control_state, action) + offsetof(control,
    zoom) == 0x3ea) ? 1 : -1];

// @address 0x152610 render_player_view_update_particle_systems
extern "C" void render_player_view_update_particle_systems(int particle_user)
{
    int visibility_bucket;
    s_effects_particle_system_list *bucket_list;
    bool is_first_person;
    int entry_handle;
    const s_observer_result *observer_camera;
    unsigned char valid_user;
    c_particle_system *current_system;
    int filtered_user;
    struct { long visibility[4]; c_particle_system *system; s_data_iterator iterator; } iteration_state;
    for (long list_index = 0; list_index < 4; list_index++)
        g_particle_system_lists[list_index].count = 0;
    valid_user = 0;
    if (particle_user != -1 && g_director_globals->slots[particle_user].perspective == 0)
        valid_user = 1;
    filtered_user = valid_user ? particle_user : -1;
    iteration_state.iterator.absolute_index = -1;
    iteration_state.iterator.index = -1;
    iteration_state.iterator.data = (s_data_array *)g_particle_system_data_array;
    current_system = (c_particle_system *)data_iterator_next(&iteration_state.iterator);
    iteration_state.system = current_system;
    while (current_system != 0) {
        visibility_bucket = particle_system_compute_visibility(current_system, filtered_user, (bool *)iteration_state.visibility);
        if (visibility_bucket < 4) {
            bucket_list = &g_particle_system_lists[visibility_bucket];
            entry_handle = iteration_state.iterator.index;
            is_first_person = *(bool *)iteration_state.visibility;
            bucket_list->entries[bucket_list->count].particle_system_index = entry_handle;
            bucket_list->entries[bucket_list->count].first_person = is_first_person;
            ++bucket_list->count;
        }
        current_system = (c_particle_system *)data_iterator_next(&iteration_state.iterator);
        iteration_state.system = current_system;
    }
    observer_camera = observer_try_and_get_camera(particle_user);
    g_particle_render_camera = observer_camera;
    if (observer_camera == 0)
        g_particle_render_camera = &k_default_particle_block_view;
    return;
}


void c_player_view::get_player_render_camera_orientation(real_matrix4x3 *camera)
{
    const render_camera *view_camera = c_player_view::get_current()->get_render_camera();
    camera->position = view_camera->position;
    camera->forward = view_camera->forward;
    camera->up = view_camera->up;
    cross_product3d(&camera->up, &camera->forward, &camera->left);
    camera->scale = 1.0f;
}





// match: this caller retains its pointer view of the player argument.


// @address 0x153ac8 ?static_lighting_setup_internals@c_player_view@@KAXJ@Z
void c_player_view::static_lighting_setup_internals(long user_index)
{
    long light_count = light_data->first_unallocated;
    real_matrix4x3 camera;
    get_player_render_camera_orientation(&camera);
    float scale = render_player_view_compute_pixel_scale() * 2.0f;
    for (long absolute_index = 0; absolute_index < light_count; absolute_index++)
    {
        if (lights_game_globals->enabled_lights[absolute_index >> 5] & (1 << (absolute_index & 31)))
        {
            long light_index = datum_absolute_index_to_index(light_data, absolute_index);
            light_update_fade_intensity(light_index, user_index, &camera, scale);
        }
    }
}

float render_player_view_compute_pixel_scale(void)
{
    float result = (float)(c_rasterizer::render_globals.width /
                             (c_player_view::get_current()->rasterizer_camera.window_pixel_bounds.x1 -
                                 c_player_view::get_current()->rasterizer_camera.window_pixel_bounds.x0)
                         + c_rasterizer::render_globals.height /
                             (c_player_view::get_current()->rasterizer_camera.window_pixel_bounds.y1 -
                                 c_player_view::get_current()->rasterizer_camera.window_pixel_bounds.y0))
                 * c_player_view::get_current()->rasterizer_camera.field_of_view_scale * 0.5f;
    if (c_player_view::get_current()->rasterizer_camera.frustum_adjustment) {
        result = (c_player_view::get_current()->rasterizer_camera.frustum_scale.y
            + c_player_view::get_current()->rasterizer_camera.frustum_scale.x) * result * 0.5f;
    }
    if (result <= 0.001f) {
        return 0.001f;
    }
    return result;
}

// @address 0x153c40 ?compute_visibility@c_player_view@@UAAXXZ

static __forceinline void render_player_view_game_clusters_set(s_game_cluster_bit_vectors *v, s_cluster_reference r, bool value)
{
    v->bit_vectors[r.bsp_index].set(r.cluster_index, value);
}
static __forceinline void render_player_view_add_visible_clusters_to_frame_visible_clusters() {
    s_visibility_region *region = (s_visibility_region *)g_visibility_globals.collection_input;
    s_game_cluster_bit_vectors *bits = &render_visibility_collection_globals.visible_clusters;
    for (long i = 0; i < region->cluster_count; i++) {
        const visibility_cluster *cluster = &region->clusters[i];
        render_player_view_game_clusters_set(bits, cluster->cluster_reference, true);
    }
}

void c_player_view::compute_visibility()
{
    {
        s_cluster_reference starting_cluster = invalid_cluster_reference_get();
        c_visible_items::clear_all();
        c_visible_items::push_marker();
        get_starting_cluster(&starting_cluster);
        render_visibility_camera_collection_compute(get_render_camera(), starting_cluster, get_render_projection(), view_index, user_index, false);
    }
    render_player_view_add_visible_clusters_to_frame_visible_clusters();
}



// @address 0x16eb28 render_player_view_setup_albedo_targets
void render_player_view_setup_albedo_targets(bool unused)
{
    c_rasterizer::set_depth_stencil_surface((c_rasterizer::e_surface)3);
    c_rasterizer::set_render_target(0, (c_rasterizer::e_surface)17);
    c_rasterizer::set_render_target(1, (c_rasterizer::e_surface)19);
    c_rasterizer::set_render_target(2, (c_rasterizer::e_surface)0);
    c_rasterizer::set_render_target(3, (c_rasterizer::e_surface)0);

    unsigned long clear_target = 16;
    if (rasterizer_tiling_get_state()->enabled) clear_target = 17;
#ifdef HALO3_64BIT
    // Decorators replace only stencil bit 5. Clear the previous frame's object and
    // first-person bits too, before the scope reads them as heat from surface 54.
    clear_target |= 32;
#endif
    c_rasterizer::refresh_tiling_render_targets();
    c_rasterizer::set_viewport(g_rasterizer_last_rectangles.viewport, 0.0f, 1.0f);
    c_rasterizer::set_scissor_rect(&g_rasterizer_last_rectangles.scissor);
    c_rasterizer::clear(clear_target, 0x8d1b3c8d, 1.0f, 0);
    c_rasterizer::clear(2, 0x8080ff, 1.0f, 0);
}

// @address 0x16edb0 render_player_view_set_exposure_constants
void render_player_view_set_exposure_constants(float exposure, float multiplier, float stops, bool disable)
{
    real_vector4d constants;
    constants.n[0] = exposure;
    constants.n[1] = (float)pow(2.0, (double)stops);
    constants.n[2] = c_rasterizer::get_render_target_alpha_multiplier(1);
    constants.n[3] = c_rasterizer::get_render_target_alpha_multiplier(0);
    g_rasterizer_device->SetVertexShaderConstantF(232, (const float *)&constants, 1);
    g_rasterizer_device->SetPixelShaderConstantF(0, (const float *)&constants, 1);

    real_vector4d scale;
    scale.n[0] = multiplier;
    scale.n[1] = exposure * multiplier;
    scale.n[3] = 0.0f;
    scale.n[2] = 0.0f;
    g_rasterizer_device->SetVertexShaderConstantF(239, (const float *)&scale, 1);
    g_rasterizer_device->SetPixelShaderConstantF(12, (const float *)&scale, 1);

    int flags[2];
    flags[0] = !disable && c_rasterizer::g_LDR_render_mode == 2 && !c_rasterizer::g_HDR_alpha_blend;
    flags[1] = !disable && c_rasterizer::g_HDR_render_mode == 2 && !c_rasterizer::g_HDR_alpha_blend;
    g_rasterizer_device->SetPixelShaderConstantB(14, flags, 2);
}


// @address 0x16bce0 render_player_view_compute_exposure
float render_player_view_compute_exposure(c_camera_fx_values *camera_fx_values)
{
    float blend = camera_fx_globals->current_b;
    double power = pow(2.0,
        (double)((1.0f - blend) * camera_fx_values->current + camera_fx_globals->current_a * blend + camera_fx_globals->exposure_offset +
            camera_fx_values->exposure_boost + g_postprocess_exposure_offset));
    float scale = (float)power;
    return c_screen_postprocess::x_editable_settings.exposure * scale * 0.66943294f;
}

// @address 0x179140 ?setup_targets_static_lighting@c_rasterizer@@SAXMM_NM0@Z
void c_rasterizer::setup_targets_static_lighting(float exposure, float multiplier, bool hdr_alpha_blend, float stops, bool clear_targets)
{
    stops = c_camera_fx_values::g_HDR_target_stops;
    c_rasterizer::set_depth_stencil_surface((c_rasterizer::e_surface)3);
    c_rasterizer::g_HDR_alpha_blend = 0;
    c_rasterizer::set_render_target(0, (c_rasterizer::e_surface)13);
    if (hdr_alpha_blend) {
        c_rasterizer::g_HDR_alpha_blend = 0;
        c_rasterizer::set_render_target(1, (c_rasterizer::e_surface)14);
    } else {
        c_rasterizer::set_render_target(1, (c_rasterizer::e_surface)0);
    }
    c_rasterizer::set_render_target(2, (c_rasterizer::e_surface)0);
    c_rasterizer::set_render_target(3, (c_rasterizer::e_surface)0);
    c_rasterizer::refresh_tiling_render_targets();
    float maximum_z = 1.0f;
    c_rasterizer::set_viewport(g_rasterizer_last_rectangles.viewport, 0.0f, maximum_z);
    c_rasterizer::set_scissor_rect(&g_rasterizer_last_rectangles.scissor);
    if (clear_targets)
        c_rasterizer::clear(2, 1, maximum_z, 0);
    render_player_view_set_exposure_constants(exposure, multiplier, stops, false);
}

// 0x17fdd0
extern "C" {
// @address 0x8c4843 g_active_camo_capture_pending
// @address 0x8c4848 g_active_camo_resolve_pending
// @address 0x86531c g_active_camo_texture_bounds
// @address 0x86532c g_active_camo_resolve_bounds
}
extern "C" void render_player_view_initialize_window_bounds(const rectangle2d *window, const rectangle2d *render_bounds)
{
    if (g_active_camo_capture_pending) {
        g_active_camo_resolve_bounds = *window;
        g_active_camo_resolve_pending = true;
        g_active_camo_capture_pending = false;
        g_active_camo_texture_bounds.x0 = 0.0f;
        g_active_camo_texture_bounds.x1 = 0.0f;
        g_active_camo_texture_bounds.y0 = 1.0f;
        g_active_camo_texture_bounds.y1 = 1.0f;
    }
}

// @address 0x180018 render_player_view_find_transparent_boundary
extern "C" void render_player_view_find_transparent_boundary(void *buffer, unsigned short first, unsigned short last, unsigned short *boundary)
{
    unsigned short *indices = (unsigned short *)buffer;
    unsigned short enabled[1024];
    unsigned short disabled[1024];
    unsigned short disabled_count = 0;
    unsigned short enabled_count = 0;
    for (unsigned short i = first; i < last; ++i) {
        unsigned short value = indices[i];
        if (c_transparency_renderer::transparents.m_storage[value].has_plane)
            enabled[enabled_count++] = value;
        else
            disabled[disabled_count++] = value;
    }
    for (unsigned short i = 0; i < enabled_count; ++i)
        indices[first + i] = enabled[i];
    unsigned short enabled_end = (unsigned short)(first + enabled_count);
    *boundary = enabled_end;
    for (unsigned short i = 0; i < disabled_count; ++i)
        indices[enabled_end + i] = disabled[i];
}

void c_rasterizer::disown_vertex_shader_constants(long start_register, long count)
{
    unsigned int first = (unsigned int)start_register >> 2;
    unsigned int last = ((unsigned int)start_register + count - 1) >> 2;
    unsigned __int64 mask = (unsigned __int64)((__int64)0x8000000000000000i64 >> (last - first)) >> first;
    g_device->m_Pending.m_Mask[0] |= mask;
}

float render_player_view_compute_exposure_multiplier(c_camera_fx_values *camera_fx_values)
{
    float blend = camera_fx_globals->current_b;
    float exposure = (1.0f - blend) * camera_fx_values->current + camera_fx_globals->current_a * blend + camera_fx_globals->exposure_offset;
    return (float)pow(2.0, (double)(camera_fx_values->exposure_result - exposure));
}

void __tls_set_g_render_texture_camera_globals_allocator(void *allocator)
{
    g_render_texture_camera_globals = (s_render_texture_camera_globals *)allocator;
}


// @address 0x44d200 ?render_texture_camera_initialize@@YAXXZ
void render_texture_camera_initialize(void)
{
    g_render_texture_camera_globals = (s_render_texture_camera_globals *)g_render_texture_camera_globals_allocator.reserve_memory(
        "render texture globals", 0, sizeof(s_render_texture_camera_globals), 0);
}

// @address 0x5bec98 ?render_texture_camera_dispose@@YAXXZ
void render_texture_camera_dispose(void)
{
}


void render_texture_camera_set_position_object(long object_index, long marker_index)
{
    if (object_try_and_get_and_verify_type(object_index, (unsigned long)-1)) {
        g_render_texture_camera_globals->position_type = 0;
        g_render_texture_camera_globals->object_index = object_index;
        g_render_texture_camera_globals->marker_index = marker_index;
        g_render_texture_camera_globals->position_offset_point.x = 0.0f;
        g_render_texture_camera_globals->position_offset_point.y = 0.0f;
        g_render_texture_camera_globals->position_offset_point.z = 0.0f;
    }
}

static const float render_player_view_radians_per_degree = 0.017453292f;

// @address 0x44d370 render_texture_camera_set_field_of_view
void render_texture_camera_set_field_of_view(float field_of_view_degrees)
{
    g_render_texture_camera_globals->field_of_view_type = 0;
    float clamped = 1.0f;
    if (field_of_view_degrees > clamped) {
        clamped = 179.9f;
        if (field_of_view_degrees < clamped) {
            g_render_texture_camera_globals->field_of_view = field_of_view_degrees * render_player_view_radians_per_degree;
            return;
        }
    }
    g_render_texture_camera_globals->field_of_view = clamped * render_player_view_radians_per_degree;
}

static __forceinline void render_player_view_texture_camera_off()
{
    g_render_texture_camera_globals->enabled = false;
    g_render_texture_camera_globals->dynamic_lights = false;
}

// @address 0x44d3c8 ?render_texture_camera_set_object_marker@@YAXJJM@Z
void render_texture_camera_set_object_marker(long object_index, long marker_name, float field_of_view_degrees)
{
    object_marker marker;
    if (object_index != -1 &&
        object_try_and_get_and_verify_type(object_index, (unsigned long)-1) &&
        internal_object_get_markers_by_string_id(object_index, marker_name, &marker, 1, false))
    {
        g_render_texture_camera_globals->enabled = true;
        g_render_texture_camera_globals->position_type = 0;
        g_render_texture_camera_globals->target_type = 1;
        g_render_texture_camera_globals->orientation_type = 1;
        g_render_texture_camera_globals->reserved = 0;
        g_render_texture_camera_globals->object_index = object_index;
        g_render_texture_camera_globals->marker_index = marker_name;
        g_render_texture_camera_globals->position_offset_point.x = 0.0f;
        g_render_texture_camera_globals->position_offset_point.y = 0.0f;
        g_render_texture_camera_globals->position_offset_point.z = 0.0f;
        render_texture_camera_set_field_of_view(field_of_view_degrees);
    }
    else
    {
        render_player_view_texture_camera_off();
    }
}

// @address 0x44d4b0 render_texture_camera_set_position_and_direction
void render_texture_camera_set_position_and_direction(bool enable, const real_point3d *position, const real_vector3d *forward,
    const real_vector3d *up, float field_of_view)
{
    s_render_texture_camera_globals *texture_camera_globals = g_render_texture_camera_globals;
    real_vector3d *camera_forward = &texture_camera_globals->camera_forward;
    real_vector3d *camera_up = &texture_camera_globals->camera_up;
    // match: 0x44d4b0: the first store goes through the thread-local global to keep the address-register order
    g_render_texture_camera_globals->position_type = 1;
    texture_camera_globals->target_type = 1;
    real_point3d *camera_position = &texture_camera_globals->position;
    texture_camera_globals->orientation_type = 3;
    texture_camera_globals->reserved = 0;
    *camera_position = *position;
    texture_camera_globals->position_offset_point.x = 0.0f;
    texture_camera_globals->position_offset_point.y = 0.0f;
    texture_camera_globals->position_offset_point.z = 0.0f;
    *camera_forward = *forward;
    *camera_up = *up;
    render_texture_camera_set_field_of_view(field_of_view);
}

// Registers a member of field layout 1 (see c_restricted_memory::add_member) and publishes its
// per-thread address. One of a family of identical allocators that differ
// only in the layout index, the update callback and the thread-local pointer.


// @address 0x44d548 ?reserve_memory@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_render_texture_camera_globals_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
template<>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_render_texture_camera_globals_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
  int member_index = g_restricted_regions[3].add_member(name, type, size, 0, __tls_set_g_render_texture_camera_globals_allocator, 0, 0);
  unsigned int offset = g_restricted_regions[3].get_member_offset(member_index);
  void *address = g_restricted_address[3] + offset;
  int owner = g_registered_thread_index;
  m_thread_id = owner;
  m_member_index = member_index;
  g_render_texture_camera_globals = (s_render_texture_camera_globals *)address;
  return g_restricted_address[3] + g_restricted_regions[3].get_member_offset(member_index);
}


extern "C" {

// @address 0x8c46b8 g_antialiasing_mode
}


// @address 0x1d6bb0 ?restricted_region_free_member@@YAXJJ@Z

// @address 0x44d600 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_render_texture_camera_globals_allocator@@YAXPAX@Z@@UAAXPAX@Z
template <>
void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_render_texture_camera_globals_allocator>::deallocate(void *memory)
{
    long value = m_member_index;
    g_render_texture_camera_globals = 0;
    restricted_region_free_member(3, value);
}

// @address 0x44d5e8 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_render_texture_camera_globals_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template <>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_render_texture_camera_globals_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// @address 0x44f9d8 render_texture_slot_get_surface
extern "C" c_render_surface *render_texture_slot_get_surface(const c_render_surface_group *slot, long surface, long resolution, long unused)
{
    long flags = slot->description.flags;
    unsigned int value = 0;
    if (flags & 1)
        value = c_rasterizer::g_screen_resolution;
    if (flags & 2)
        value = (value + (value << 1)) << 1;
    if (flags & 8)
        value = value << 2;
    if (flags & 0x20)
        value = (value << 2) + g_antialiasing_mode;
    return &slot->surfaces[value];
}

// @address 0x162838 ?render_submit_visibility@c_player_view@@UAAXXZ

static __forceinline void render_player_view_structure_reset()
{
    g_render_structure_globals.current_a = 0;
    g_render_structure_globals.current_b = 0;
    g_render_structure_globals.depth = -1;
    g_cluster_sorted_order.m_count = 0;
    g_render_structure_globals.stack_b[5] = -1;
    g_render_structure_globals.cached_definition_index = -1;
    g_render_structure_globals.cached_instance_marker = -1;
}
static __forceinline void render_player_view_structure_push()
{
    ++g_render_structure_globals.depth;
    g_render_structure_globals.cluster_markers[g_render_structure_globals.depth] = g_render_structure_globals.current_a;
    g_render_structure_globals.instance_markers[g_render_structure_globals.depth] = g_render_structure_globals.current_b;
}
static __forceinline void render_object_markers_reset()
{
    s_render_object_globals *g = g_render_object_globals;
    g->current_a = 0;
    g->current_b = 0;
    g->marker = -1;
    g->active = false;
}
static __forceinline void render_player_view_object_push()
{
    s_render_object_globals *g = g_render_object_globals;
    ++g->marker;
    g->stack_a[g->marker] = g->current_a;
    g->stack_b[g->marker] = g->current_b;
}
static __forceinline void render_player_view_object_pop()
{
    s_render_object_globals *g = g_render_object_globals;
    --g->marker;
    g->current_a = g->stack_a[g->marker + 1];
    g->current_b = g->stack_b[g->marker + 1];
}


// @address 0x1764e0 player_view_render_transparent_objects
/* __real@3f800000 (a literal pool owned by another object): 1.0f at [0] */

extern "C" void player_view_render_transparent_objects(bool render_objects, bool render_decals)
{
    ::c_rasterizer::set_z_buffer_mode((c_rasterizer::e_z_buffer_mode)1);
    c_rasterizer::begin_high_quality_blend();
    if (render_objects) {
        int enabled = 1;
        g_rasterizer_device->SetVertexShaderConstantB(2, &enabled, 1);
        real_vector4d color = {1.0f, 1.0f, 1.0f, 1.0f};
        g_rasterizer_device->SetPixelShaderConstantF(32, &color.x, 1);
        real_vector4d transform = {0.0f, 0.0f, 1.0f, 1.0f};
        g_rasterizer_device->SetVertexShaderConstantF(228, &transform.x, 1);
        ::c_object_renderer::render_object_contexts(_entry_point_default, 32);
    }
    if (render_decals) c_decal_system::render_all(c_decal_definition::_pass_default);
}

extern "C" {
// @address 0x8c448c g_rasterizer_resolve_samples
}

// @address 0x1787a8 player_view_resolve_albedo_surfaces
extern "C" bool player_view_resolve_albedo_surfaces(const rectangle2d *rectangle)
{
    if (D3DDevice_GetDeviceState(g_rasterizer_device) & 1) {
        c_rasterizer::resolve_surface_tiled((c_rasterizer::e_surface)17, 0, false, false);
        c_rasterizer::resolve_surface_tiled((c_rasterizer::e_surface)19, 1, false, g_rasterizer_resolve_samples != 0);
    } else {
        c_rasterizer::resolve_surface((c_rasterizer::e_surface)17, 0, rectangle, rectangle->x0, rectangle->y0);
        c_rasterizer::resolve_surface((c_rasterizer::e_surface)19, 1, rectangle, rectangle->x0, rectangle->y0);
    }
    return true;
}

// @address 0x178b68 ?submit_occlusion_tests@c_player_view@@IAAX_N0@Z
void c_player_view::submit_occlusion_tests(bool render_objects, bool render_decals)
{
    if (rasterizer_tiling_get_state()->enabled) return;
    ::c_rasterizer::set_z_buffer_mode((c_rasterizer::e_z_buffer_mode)1);
    c_rasterizer::set_depth_stencil_surface((c_rasterizer::e_surface)3);
    c_rasterizer::set_render_target(0, (c_rasterizer::e_surface)0);
    c_rasterizer::set_render_target(1, (c_rasterizer::e_surface)0);
    c_rasterizer::set_render_target(2, (c_rasterizer::e_surface)0);
    c_rasterizer::set_render_target(3, (c_rasterizer::e_surface)0);
    c_rasterizer::set_viewport(g_rasterizer_last_rectangles.viewport, 0.0f, 1.0f);
    c_rasterizer::set_scissor_rect(&g_rasterizer_last_rectangles.scissor);
    if (render_objects) lens_flares_submit_occlusions(view_index, 0);
    if (render_decals) lens_flares_submit_occlusions(view_index, 1);
}

// the render view state the shadow-map pass and the static lighting pass read


// @address 0x44ce98 player_view_draw_static_lighting_composite
extern "C" void player_view_draw_static_lighting_composite(const c_player_view *params)
{
    if (c_rasterizer::set_explicit_shaders(68, _vertex_type_screen, _transfer_vector_vertex_none, _entry_point_default)) {
        c_rasterizer::set_render_target(0, (c_rasterizer::e_surface)8);
        c_rasterizer::set_render_target(1, (c_rasterizer::e_surface)13);
        c_rasterizer::set_render_target(2, (c_rasterizer::e_surface)14);
        c_rasterizer::set_render_target(3, (c_rasterizer::e_surface)0);
        c_rasterizer::set_depth_stencil_surface((c_rasterizer::e_surface)0);
        D3DDevice_SetRenderState_ColorWriteEnable(g_rasterizer_device, 15);
        D3DDevice_SetRenderState_ColorWriteEnable1(g_rasterizer_device, 15);
        D3DDevice_SetRenderState_ColorWriteEnable2(g_rasterizer_device, 15);
        if (g_rasterizer_alpha_blend_mode != 0) {
            g_rasterizer_alpha_blend_mode = 0;
            D3DDevice_SetRenderState_AlphaBlendEnable(g_rasterizer_device, 0);
        }
        c_rasterizer::end_high_quality_blend();
        ::c_rasterizer::set_z_buffer_mode((c_rasterizer::e_z_buffer_mode)2);
        c_rasterizer::set_aliased_surface_as_texture(0, (c_rasterizer::e_surface)13);
        c_rasterizer::set_sampler_address_mode(0, (c_rasterizer::e_sampler_address_mode)(1), (c_rasterizer::e_sampler_address_mode)(1),
            (c_rasterizer::e_sampler_address_mode)(1));
        c_rasterizer::set_sampler_filter_mode(0, c_rasterizer::_sampler_filter_mode_point);
        c_rasterizer::set_aliased_surface_as_texture(1, (c_rasterizer::e_surface)14);
        c_rasterizer::set_sampler_address_mode(1, (c_rasterizer::e_sampler_address_mode)(1), (c_rasterizer::e_sampler_address_mode)(1),
            (c_rasterizer::e_sampler_address_mode)(1));
        c_rasterizer::set_sampler_filter_mode(1, c_rasterizer::_sampler_filter_mode_point);
        c_rasterizer::set_aliased_surface_as_texture(2, (c_rasterizer::e_surface)8);
        c_rasterizer::set_sampler_address_mode(2, (c_rasterizer::e_sampler_address_mode)(1), (c_rasterizer::e_sampler_address_mode)(1),
            (c_rasterizer::e_sampler_address_mode)(1));
        c_rasterizer::set_sampler_filter_mode(2, c_rasterizer::_sampler_filter_mode_point);
        c_rasterizer::draw_fullscreen_quad();
        // match: 0x44ce98: preserve accepted sampler temporary lifetimes through the canonical ABI adapter.
        rasterizer_set_sampler_texture_reference(0, c_rasterizer_texture_ref());
        rasterizer_set_sampler_texture_reference(1, c_rasterizer_texture_ref());
        rasterizer_set_sampler_texture_reference(2, c_rasterizer_texture_ref());
        c_rasterizer::set_render_target(0, (c_rasterizer::e_surface)0);
        c_rasterizer::set_render_target(1, (c_rasterizer::e_surface)0);
        c_rasterizer::set_render_target(2, (c_rasterizer::e_surface)0);
        c_rasterizer::set_render_target(3, (c_rasterizer::e_surface)0);
        c_rasterizer::set_depth_stencil_surface((c_rasterizer::e_surface)4);
        D3DDevice_FlushHiZStencil(g_rasterizer_device, D3DFHZS_SYNCHRONOUS);
        rasterizer_rebuild_hierarchical_z_buffer();
    }
    bool hdr_alpha_blend = c_screen_postprocess::x_editable_settings.enabled || rasterizer_tiling_get_state()->enabled;
    c_rasterizer::setup_targets_static_lighting(params->exposure, params->exposure_multiplier, hdr_alpha_blend, c_camera_fx_values::g_HDR_target_stops, false);
    ::c_rasterizer::set_z_buffer_mode((c_rasterizer::e_z_buffer_mode)0);
}

struct structure_camera_fx_palette_entry;


// @address 0x148240 ?setup_camera@c_player_view@@QAAXJJJJPBUs_observer_result@@_N@Z
void c_player_view::setup_camera(long user, long count, long index, long controller, const s_observer_result *observer, bool unused)
{
    render_camera *camera = &rasterizer_camera;
    render_view_compute_fullscreen_bounds(camera);
    render_view_compute_window_bounds(user, count, &camera->window_pixel_bounds, &camera->window_title_safe_pixel_bounds);
    short x = camera->window_pixel_bounds.x0;
    short y = camera->window_pixel_bounds.y0;
    camera->window_offset.x = x;
    camera->window_offset.y = y;
    camera->window_pixel_bounds.x0 = 0;
    camera->window_pixel_bounds.y0 = 0;
    camera->window_pixel_bounds.x1 -= x;
    camera->window_pixel_bounds.y1 -= y;
    camera->window_title_safe_pixel_bounds.x1 -= x;
    camera->window_title_safe_pixel_bounds.x0 -= x;
    camera->window_title_safe_pixel_bounds.y1 -= y;
    camera->window_title_safe_pixel_bounds.y0 -= y;
    bool widescreen = rasterizer_main_get_video_mode()->fIsWideScreen != 0;
    antialiasing_mode = splitscreen_view_config(widescreen, count, user).extra;
    window_type = 0;
    switch (count)
    {
    case 2: {
        long type;
        if (antialiasing_mode == 3)
            type = user + 61;
        else
            type = user + 59;
        window_type = type;
        break;
    }
    case 3:
        if (user == 0)
        {
            if (antialiasing_mode == 3)
                window_type = 61;
            else
                window_type = 59;
        }
        else
            window_type = user + 63;
        break;
    case 4:
        window_type = user + 63;
        break;
    }
    render_view_initialize_camera(camera, observer);
    if (observer)
    {
        const render_player_view_observer_flags *const &source = &observer->flags;
        render_player_view_observer_flags *const &destination = &observer_flags;
        *destination = *source;
    }
    else
        observer_flags.values[0] = 0;
    real_rectangle2d bounds;
    render_camera_build_viewport_frustum_bounds(camera, &bounds);
    render_projection *projection = &rasterizer_projection;
    render_camera_build_projection(camera, &bounds, projection, 0.0f);
    memcpy(&render_camera_state, camera, sizeof(*camera));
    memcpy(&render_projection_state, projection, sizeof(*projection));
    user_index = user;
    window_count = count;
    window_index = index;
    view_index = controller;
    controller_index = user_interface_controller_get_rumble_enabled(view_index);
    camera_settings = &g_player_view_camera_settings[user];
}
// @address 0x16ad08 ?setup_camera_fx_parameters@c_player_view@@QAAXM@Z
void c_player_view::setup_camera_fx_parameters(float value)
{
    struct scenario *scenario = global_scenario;
    const cache_file_tag_instance *instances = global_tag_instances;
    const c_camera_fx_settings *scenario_settings = 0;
    if (scenario && scenario->camera_fx.index != -1)
        scenario_settings = (const c_camera_fx_settings *)instances[DATUM_INDEX_TO_ABSOLUTE_INDEX(scenario->camera_fx.index)].definition;
    s_cluster_reference cluster;
    this->get_starting_cluster(&cluster);
    const structure_bsp *structure = (const structure_bsp *)instances[DATUM_INDEX_TO_ABSOLUTE_INDEX(
        ((scenario_structure_bsp_reference *)scenario->structure_bsps.address)[cluster.bsp_index].structure_bsp.index)].definition;
    short palette_index = ((structure_cluster *)structure->clusters.address)[cluster.cluster_index].camera_fx_index;
    const structure_camera_fx_palette_entry *palette = 0;
    if (palette_index != -1)
        palette = (const structure_camera_fx_palette_entry *)((const structure_camera_fx_palette_entry *)structure->camera_fx_palette.address + palette_index);
    const c_camera_fx_settings *game_settings = 0;
    if (global_game_globals && global_game_globals->camera_fx.index != -1)
        game_settings = (const c_camera_fx_settings *)instances[DATUM_INDEX_TO_ABSOLUTE_INDEX(
            global_game_globals->camera_fx.index)].definition;
    this->camera_settings->values.update(
        (const c_camera_fx_settings *)this->camera_settings, palette, scenario_settings, game_settings, value);
    float range = render_player_view_compute_exposure_multiplier(&this->camera_settings->values);
    this->lights_view.exposure = camera_fx_globals->exposure_scale * range;
    this->exposure = render_player_view_compute_exposure(&this->camera_settings->values);
    this->exposure_multiplier = render_player_view_compute_exposure_multiplier(&this->camera_settings->values);
}




// @address 0x44cd20 player_view_handle_occlusion_callback
extern "C" void player_view_handle_occlusion_callback(long)
{
    s_main_render_view_layout layout;
    main_render_get_view_layout(&layout);
    bool should_set_z_buffer = !c_rasterizer::get_is_tiling_enabled() && layout.view_count == 1;
    c_transparency_renderer::render(should_set_z_buffer);
    long height = g_rasterizer_surface_groups[52].get_specialized_surface((c_rasterizer::e_surface)52)->description.height;
    c_render_surface *surface = g_rasterizer_surface_groups[52].get_specialized_surface((c_rasterizer::e_surface)52);
    if (g_chud_text_render_target_dirty)
        chud_draw_composite(surface->description.width, height);
}


extern "C" {
// @address 0x8c4850 g_transparent_sort_partition_count
long g_transparent_sort_partition_count;

}
static __forceinline bool render_player_view_transparent_after(unsigned short a, unsigned short b)
{
    long forward = render_transparents_compute_side_votes(a, b);
    bool result;
    if (forward == 9) result = true;
    else if (forward == -9) result = false;
    else {
        long reverse = render_transparents_compute_side_votes(b, a);
        if (reverse == 9) result = false;
        else if (reverse == -9) result = true;
        else {
            long difference = forward - reverse;
            if (difference) result = difference > 0;
            else result = forward > 0;
        }
    }
    return result;
}
static __forceinline float render_player_view_plane_distance(const real_point3d *point, const real_plane3d *plane)
{
    return point->x * plane->normal.i + point->y * plane->normal.j + point->z * plane->normal.k - plane->distance;
}
// @address 0x180130 render_player_view_sort_transparents_recursive
extern "C" void render_player_view_sort_transparents_recursive(void *buffer, unsigned short first, unsigned short last,
    unsigned short boundary, void *scratch, unsigned short original_boundary, unsigned short depth)
{
    if ((unsigned short)(last - first) > 1 && (unsigned short)(boundary - first) != 0) {
        unsigned short *indices = (unsigned short *)buffer;
        s_transparent_sort_workspace *workspace = (s_transparent_sort_workspace *)scratch;
        unsigned short pivot_index = (unsigned short)-1;
        float furthest = -1000.0f;
        for (unsigned short i = first; i < boundary; ++i) {
            const s_transparent_types *const &entry = &c_transparency_renderer::transparents.m_storage[indices[i]];
            float distance = entry->radius;
            if (!(distance <= furthest)) {
                furthest = distance;
                pivot_index = i;
            }
        }
        unsigned short pivot = indices[pivot_index];
        workspace->pivot = pivot;
        workspace->front_count = 0;
        workspace->back_count = 0;
        workspace->after_front_count = 0;
        workspace->after_back_count = 0;
        for (unsigned short i = first; i < boundary; ++i) {
            if (i != pivot_index) {
                unsigned short value = indices[i];
                if (render_player_view_transparent_after(value, workspace->pivot)) {
                    workspace->after_front[workspace->after_front_count] = value;
                    ++workspace->after_front_count;
                } else {
                    workspace->front[workspace->front_count] = value;
                    ++workspace->front_count;
                }
            }
        }
        const s_transparent_types *pivot_entry = &c_transparency_renderer::transparents.m_storage[workspace->pivot];
        for (unsigned short i = boundary; i < last; ++i) {
            unsigned short value = indices[i];
            if (!(render_player_view_plane_distance(&c_transparency_renderer::transparents.m_storage[value].position, &pivot_entry->plane) < 0.0f)) {
                workspace->after_back[workspace->after_back_count] = value;
                ++workspace->after_back_count;
            } else {
                workspace->back[workspace->back_count] = value;
                ++workspace->back_count;
            }
        }
        unsigned short write = first;
        for (unsigned short i = 0; i < workspace->front_count; ++i) indices[write++] = workspace->front[i];
        for (unsigned short i = 0; i < workspace->back_count; ++i) indices[write++] = workspace->back[i];
        indices[write++] = workspace->pivot;
        for (unsigned short i = 0; i < workspace->after_front_count; ++i) indices[write++] = workspace->after_front[i];
        for (unsigned short i = 0; i < workspace->after_back_count; ++i) indices[write++] = workspace->after_back[i];
        unsigned short front_boundary = first + workspace->front_count;
        unsigned short front_end = front_boundary + workspace->back_count;
        unsigned short after_first = front_end + 1;
        unsigned short after_boundary = after_first + workspace->after_front_count;
        unsigned short after_end = after_boundary + workspace->after_back_count;
        if (front_end - first > 1)
            render_player_view_sort_transparents_recursive(buffer, first, front_end, front_boundary, scratch, original_boundary, (unsigned short)(depth + 1));
        if (after_end - after_first > 1)
            render_player_view_sort_transparents_recursive(
                buffer, after_first, after_end, after_boundary, scratch, original_boundary, (unsigned short)(depth + 1));
        ++g_transparent_sort_partition_count;
    }
}

extern "C" {

// @address 0x8c484c g_transparent_sort_mode
unsigned short g_transparent_sort_mode;

}
// @address 0x17fe30 render_player_view_sort_transparents
extern "C" void render_player_view_sort_transparents()
{
    unsigned short buckets[4];
    unsigned short boundary;
    unsigned short indices[1024];
    s_transparent_sort_workspace workspace;
    c_sorter<s_transparent_types, 1024> &order = c_transparency_renderer::transparent_sorted_order;
    long first = c_transparency_renderer::m_markers[c_transparency_renderer::m_current_marker_index].first_transparent_index;
    unsigned long count = c_transparency_renderer::m_total_transparent_count;
    order.sort((bool (*)(short, short, const void *))render_transparents_test_sort_order, first);
    if (g_transparent_sort_mode == 0) {
        buckets[0] = first;
        buckets[1] = first;
        buckets[2] = first;
        buckets[3] = first;
        for (long i = first; (unsigned long)i < count; ++i) {
            unsigned short value = order.get_order_entry(i);
            indices[i] = value;
            buckets[c_transparency_renderer::transparents.m_storage[value].sort_layer] = (unsigned short)(i + 1);
        }
        if (buckets[1] == first) buckets[1] = buckets[0];
        if (buckets[2] == first) buckets[2] = buckets[1];
        if (buckets[3] == first) buckets[3] = buckets[2];
        for (long bucket = 0; bucket < 3; ++bucket) {
            unsigned short low = buckets[bucket];
            unsigned short high = buckets[bucket + 1];
            if (high - low >= 2) {
                boundary = (unsigned short)-1;
                render_player_view_find_transparent_boundary(indices, low, high, &boundary);
                unsigned short enabled_end = boundary;
                g_transparent_sort_partition_count = 0;
                render_player_view_sort_transparents_recursive(indices, low, high, enabled_end, &workspace, enabled_end, 0);
            }
        }
        for (long i = first; (unsigned long)i < count; ++i) {
            order.set_order_entry(i, indices[i]);
        }
    }
}

// @address 0x2588c k_widescreen_aspect_ratio
// @address 0x44d248 ?render_texture_camera_initialize_for_new_map@@YAXXZ
void render_texture_camera_initialize_for_new_map()
{
    memset(g_render_texture_camera_globals, 0, sizeof(*g_render_texture_camera_globals));
    g_render_texture_camera_globals->enabled = false;
    g_render_texture_camera_globals->dynamic_lights = false;
    g_render_texture_camera_globals->render_lights = false;
    g_render_texture_camera_globals->render_mode = 0;
    g_render_texture_camera_globals->position_type = 1;
    g_render_texture_camera_globals->target_type = 1;
    g_render_texture_camera_globals->orientation_type = 0;
    g_render_texture_camera_globals->reserved = 0;
    g_render_texture_camera_globals->field_of_view_type = 1;
    set_real_point3d(&g_render_texture_camera_globals->position, 0.0f, 0.0f, 0.0f);
    set_real_point3d(&g_render_texture_camera_globals->position_offset_point, 0.0f, 0.0f, 0.0f);
    set_real_point3d(&g_render_texture_camera_globals->target, 1.0f, 0.0f, 0.0f);
    g_render_texture_camera_globals->field_of_view = 1.0f;
    g_render_texture_camera_globals->aspect_ratio = k_widescreen_aspect_ratio;
    g_render_texture_camera_globals->width = 384;
    g_render_texture_camera_globals->height = 384;
}

// @address 0x5bec98 ?render_texture_camera_dispose_from_old_map@@YAXXZ
void render_texture_camera_dispose_from_old_map(void)
{
}

// @address 0x73a1a8 ?m_fov_scale@c_first_person_view@@2MA
float c_first_person_view::m_fov_scale = 0.88f;

// @address 0x7c61e0 ?k_first_person_far_clip_scale@@3MA
float k_first_person_far_clip_scale = 3.2f;


// @address 0x170730 render_player_view_setup_first_person_camera
extern "C" void render_player_view_setup_first_person_camera(c_first_person_view *first_person_view, bool scale_far)
{
    render_camera *camera = &first_person_view->rasterizer_camera;
    memcpy(camera, first_person_view->original_camera, sizeof(render_camera));
    float width = (float)(camera->window_pixel_bounds.y1 - camera->window_pixel_bounds.y0);
#if !defined(_XBOX) || defined(__clang__)
    // PC (port/GAME-CHANGES.md, "Custom resolutions"): 1.8 is the 360's full screen, 1152x640: a view wider than the
    // screen is a split-screen one (2 or 3 players), whose weapon is drawn with less field of view. At a custom
    // resolution the screen is wider than 1.8 (21:9) or narrower (16:9), so the test is against the screen itself.
    const float split_aspect_ratio = (float)c_rasterizer::render_globals.width / (float)c_rasterizer::render_globals.height;
#else
    const float split_aspect_ratio = 1.8f;
#endif
    if (width > 0.0001f)
    {
        if ((float)(camera->window_pixel_bounds.x1 - camera->window_pixel_bounds.x0) / width > split_aspect_ratio)
        {
            if (camera->window_pixel_bounds.y0 > 0)
                camera->vertical_field_of_view *= 0.7f;
            else
                camera->vertical_field_of_view *= 0.73f;
        }
        else
            camera->vertical_field_of_view *= c_first_person_view::m_fov_scale;
    }
    else
        camera->vertical_field_of_view *= c_first_person_view::m_fov_scale;
    float chud_scale = MAX(chud_get_cinematic_fov_scale(), 0.0001f);
    camera->vertical_field_of_view /= chud_scale;
    if (scale_far)
    {
        float *far_clip = &camera->near_clip;
        *far_clip *= k_first_person_far_clip_scale;
    }
    real_rectangle2d bounds;
    render_camera_build_viewport_frustum_bounds((const render_camera *)camera, &bounds);
    render_projection *projection = &first_person_view->render_projection_state;
    render_camera_build_projection((const render_camera *)camera, &bounds, projection, 0.0f);
    render_setup_window((const render_camera *)camera, projection);
}




void c_player_view::render_submit_visibility()
{
    bool visible = true;
    c_transparency_renderer::reset();
    c_transparency_renderer::push_marker();
    render_player_view_structure_reset(); render_player_view_structure_push();
    ::c_structure_renderer::submit_visibility(visible);
    render_object_markers_reset(); render_player_view_object_push();
    ::c_object_renderer::submit_visibility(visible);
}

extern "C" {


// @address 0x8c482d g_decorator_albedo_active


// @address 0x10f5d70 g_rasterizer_xenon_memory

}

// @address 0x1798d0 player_view_render_opaque_objects
extern "C" void player_view_render_opaque_objects(long p1)
{
    c_rasterizer::set_z_buffer_mode(g_rasterizer_using_albedo_sampler ? c_rasterizer::_z_buffer_mode_read_only : c_rasterizer::_z_buffer_mode_normal);
    rasterizer_set_global_shader_parameter(1);
    rasterizer_set_global_shader_parameter(2);
    if (g_render_hiz_stencil_enabled) {
        if (!g_decorator_albedo_active && g_rasterizer_alpha_blend_mode != 0) {
            g_rasterizer_alpha_blend_mode = 0;
            D3DDevice_SetRenderState_AlphaBlendEnable(g_rasterizer_device, 0);
        }
        render_objects_set_hiz_stencil_state(&g_rasterizer_xenon_memory);
    }
    D3DDevice_SetRenderState_CullMode(g_rasterizer_device, 2);
    D3DDevice_SetRenderState_ColorWriteEnable(g_rasterizer_device, 15);
    g_render_method_submit_cached_state_enabled = g_render_method_submit_cache_bypass == 0;
    c_object_renderer::render_object_contexts(_entry_point_static_prt_quadratic, p1);
    render_method_submit_reset_cached_state();
    c_object_renderer::render_object_widgets(_entry_point_static_sh);
    c_object_renderer::render_unattached_widgets(_entry_point_static_sh);
}

// @address 0x17f7c8 player_view_render_first_person_albedo
extern "C" void player_view_render_first_person_albedo(c_first_person_view *first_person_view, long view_index)
{
    player_view_render_opaque_objects(0x100);
    render_player_view_setup_first_person_camera(first_person_view, false);
    player_view_render_opaque_objects(0x200);
}



// @address 0x17bd18 render_player_view_set_atmosphere_index
extern "C" void render_player_view_set_atmosphere_index(long atmosphere_index)
{
    if (atmosphere_constants_index != atmosphere_index) {
        long sky_index = global_scenario->sky_parameters.index;
        if (sky_index != -1) {
            const s_sky_atm_parameters_definition *definition =
                (const s_sky_atm_parameters_definition *)tag_get('skya', sky_index);
            long count = definition->atmosphere_settings.count;
            if (count > 0 && atmosphere_index >= 0 && atmosphere_index < count) {
                const c_atmosphere_setting *setting = &((const c_atmosphere_setting *)definition->atmosphere_settings.address)[atmosphere_index];
                if (setting) {
                    atmosphere_constants_index = atmosphere_index;
                    s_weighted_atmosphere_parameters parameters = { false };
                    s_cluster_reference cluster;
                    ((c_player_view *)c_player_view::get_current())->get_starting_cluster(&cluster);
                    render_objects_accumulate_atmosphere(cluster, setting, &parameters, 1.0f);
                    c_atmosphere_fog_interface::set_constant(&parameters, true, true);
                    return;
                }
            }
        }
        c_atmosphere_fog_interface::disable_atmosphere();
    }
}

// the camera a render texture is drawn from




// NOTE: the shared distance3d helper reorders the subtract/multiply chain at the call below
// (verified by standalone asm diff); this local spelling matches the shipped image, so keep it.
static __forceinline float render_player_view_distance3d(const real_point3d *a, const real_point3d *b)
{
    float dx = a->x - b->x;
    float dz = a->z - b->z;
    float dy = a->y - b->y;
    return sqrtf(dy * dy + (dz * dz + dx * dx));
}


// @address 0x152d40 render_texture_camera_try_and_get
extern "C" bool render_texture_camera_try_and_get(long camera_type, s_render_texture_camera *camera, c_player_view *player_view)
{
    s_render_texture_camera_globals *globals = g_render_texture_camera_globals;
    const short width = 384;
    const short height = 384;
    camera->width = width;
    camera->height = height;
    camera->vertical_field_of_view = globals->field_of_view;
    bool result;
    switch (camera_type) {
    case 1: {
        result = globals->enabled;
        if (result) {
            object_datum *position_object = NULL;
            bool position_marker_valid = false;
            const real_point3d *target_point = NULL;
            object_datum *target_object = NULL;
            bool target_marker_valid = false;
            object_marker position_marker;
            object_marker target_marker;

            switch (globals->position_type) {
            case 0: {
                long object_index = globals->object_index;
                position_object = (object_datum *)object_try_and_get_and_verify_type(object_index, (unsigned long)-1);
                if (position_object) {
                    if (internal_object_get_markers_by_string_id(object_index, globals->marker_index, &position_marker, 1, false)) {
                        position_marker_valid = true;
                        add_points3d(&globals->position_offset_point, &position_marker.matrix.position, &camera->position);
                    } else {
                        add_points3d(&position_object->bounding_center, &globals->position_offset_point, &camera->position);
                    }
                } else {
                    render_player_view_texture_camera_off();
                    result = false;
                }
                break;
            }
            case 1:
                add_points3d(&globals->position, &globals->position_offset_point, &camera->position);
                break;
            }

            switch (globals->target_type) {
            case 0: {
                long object_index = globals->target_object_index;
                target_object = (object_datum *)object_try_and_get_and_verify_type(object_index, (unsigned long)-1);
                if (target_object) {
                    if (internal_object_get_markers_by_string_id(object_index, globals->target_marker_name, &target_marker, 1, false)) {
                        target_point = &target_marker.matrix.position;
                        target_marker_valid = true;
                    } else {
                        target_point = &target_object->bounding_center;
                    }
                } else {
                    render_player_view_texture_camera_off();
                    result = false;
                }
                break;
            }
            case 1:
                target_point = &globals->target;
                break;
            }

            if (result) {
                bool look_at_target = false;
                switch (globals->orientation_type) {
                case 0:
                    look_at_target = true;
                    break;
                case 1:
                    if (position_marker_valid) {
                        camera->forward = position_marker.matrix.forward;
                        camera->up = position_marker.matrix.up;
                    } else {
                        look_at_target = true;
                    }
                    break;
                case 2:
                    if (target_marker_valid) {
                        camera->forward = target_marker.matrix.forward;
                        camera->up = target_marker.matrix.up;
                    } else {
                        look_at_target = true;
                    }
                    break;
                case 3:
                    camera->forward = globals->camera_forward;
                    camera->up = globals->camera_up;
                    break;
                case 4:
                    if (position_object) {
                        camera->forward = position_object->forward;
                        camera->up = position_object->up;
                    } else {
                        look_at_target = true;
                    }
                    break;
                }

                if (look_at_target) {
                    if (target_point)
                        vector_from_points3d(&camera->position, target_point, &camera->forward);
                    vector3 forward = camera->forward;
                    normalize3d(&forward);
                    if (!target_point || !valid_real_normal3d(&forward)) {
                        forward.i = 1.0f;
                        forward.j = 0.0f;
                        forward.k = 0.0f;
                    }
                    vector3 left;
                    real_vector3d_build_axes_from_vectors(&forward, global_up3d, &camera->forward, &left, &camera->up);
                }

                switch (globals->field_of_view_type) {
                case 0:
                    camera->vertical_field_of_view = globals->field_of_view;
                    break;
                case 1:
                    if (target_point && target_object) {
                        float radius = target_object->bounding_radius * globals->field_of_view;
                        float distance = render_player_view_distance3d(&camera->position, &target_object->bounding_center);
                        camera->vertical_field_of_view = (float)atan2(radius, MAX(distance, 0.01f)) * 2.0f;
                    } else {
                        camera->vertical_field_of_view = 1.5707964f;
                    }
                    break;
                }
            }
        }
        break;
    }
    case 2:
        if (globals->enabled) {
            result = false;
            break;
        }
        result = false;
        break;
    default:
        result = false;
        break;
    }
    camera->horizontal_field_of_view = camera->vertical_field_of_view;
    return result;
}


// @address 0x17c680 ?render_submit_visibility@c_occlusion_view@@UAAXXZ
void c_occlusion_view::render_submit_visibility()
{
    ::c_structure_renderer::submit_visibility(0);
    ::c_object_renderer::submit_visibility(6);
}

inline void c_lightmap_shadows_view::render_submit_visibility()
{
    ::c_structure_renderer::submit_visibility(0);
    ::c_object_renderer::submit_visibility(6);
}


// @address 0x72ce7a ?render_lightmap_shadows_enabled@@3_NA
bool render_lightmap_shadows_enabled = true;
// @address 0x17be18 player_view_render_lightmap_shadows
extern "C" void player_view_render_lightmap_shadows(c_player_view *p0)
{
    bool use_second = c_screen_postprocess::x_editable_settings.enabled || (rasterizer_tiling_get_state()->enabled != 0);
    rasterizer_main_setup_hdr_render_targets(use_second, true);
    if (render_lightmap_shadows_enabled)
    {
        long r = p0->view_index;
        long unit_index = -1;
        s_controllers *g = g_director_globals;
        if (g->slots[r].perspective == 0)
        {
            long first_person_unit_index = -1;
            if (r != -1)
                first_person_unit_index = player_mapping_globals->unit_indices[r];
            unit_index = first_person_unit_index;
            if (first_person_unit_index != -1)
                lightmap_shadows_view_render_object(&p0->lightmap_shadows_view, first_person_unit_index, p0->user_index);
        }
        p0->lightmap_shadows_view.render(unit_index);
    }
    D3DDevice_FlushHiZStencil(g_rasterizer_device, static_cast<D3DFHZS_FLUSHTYPE>(0));
}

// @address 0x180eb0 ?render_patchy_fog@c_player_view@@IAAXXZ
void c_player_view::render_patchy_fog()
{
    const render_camera *camera = get_rasterizer_camera();
    const render_projection *projection = get_rasterizer_projection();
    patchy_fog.render_patchy_fog(&camera->position, &camera->forward, &camera->up, &camera->window_pixel_bounds,
        &camera->window_pixel_bounds, &projection->world_to_view, camera->near_clip, projection->projection_matrix);
    lights_view.submit_simple_light_draw_list_to_shader();
}



// @address 0x44d0b8 ?submit_attachments@c_player_view@@IAAXXZ
void c_player_view::submit_attachments()
{
    long view = this->view_index;
    long iterator;
    s_object_visibility *item = c_visible_items::m_items.objects.list_iterator_new(&iterator, c_visible_items::get_objects_starting_index());
    while (item != 0)
    {
        ::c_object_renderer::submit_object_attachments(view, item->render_setup.object_index);
        item = c_visible_items::m_items.objects.list_iterator_next(&iterator);
    }
    long object_count = c_visible_items::m_items.objects_to_render.count;
    for (long i = c_visible_items::get_light_starting_index(); i < object_count; ++i)
    {
        long object = c_visible_items::m_items.objects_to_render.entries[i];
        if (object != -1)
            light_render_lens_flare(object, g_visibility_globals.collection_input->option0);
    }
}

extern "C" {
// @address 0x72cd3f g_cinematic_letterbox_enabled
// match: existing callers retain distinct point and vector views.


}

static __forceinline rectangle2d render_player_view_get_scissor_bounds(const render_camera *camera)
{
    rectangle2d scissor_bounds = camera->window_pixel_bounds;
    if (inl_cinematic_in_progress() && g_cinematic_letterbox_enabled)
    {
        {
            rectangle2d top_bar, bottom_bar;
            cinematic_compute_letterbox_bars(&scissor_bounds, &top_bar, &bottom_bar);
        }
        calculate_aspect_ratio_scaling();
        real_vector2d scale = g_interface_aspect_ratio_scaling;
        interface_scale_bounds(&scissor_bounds, &scale);
    }
    return scissor_bounds;
}

// @address 0x44cdc0 ?render_setup@c_player_view@@UAAXXZ
void c_player_view::render_setup()
{
    c_rasterizer::begin(get_rasterizer_camera()->window_pixel_bounds, render_player_view_get_scissor_bounds(get_rasterizer_camera()));
    render_setup_window(get_rasterizer_camera(), get_rasterizer_projection());
    render_player_view_update_particle_systems(view_index);
}

struct c_view;


extern "C" {

// @address 0x81dc04 g_view_stack

}




class c_texture_camera_view;

extern "C" {

// @address 0x8c46cc g_rasterizer_max_ps_gprs



// @address 0x8c4874 g_frame_texture_flags
// @address 0x049f90 k_reflection_view_vftable

}

static __forceinline void render_player_view_set_texture_camera_gprs_allocation()
{
    if (g_rasterizer_gpr_allocation != 3)
    {
        (*rasterizer_shader_gpr_get_globals())[0] = 128 - g_rasterizer_max_ps_gprs;
        (*rasterizer_shader_gpr_get_globals())[1] = g_rasterizer_max_ps_gprs;
        g_rasterizer_device->SetShaderGPRAllocation(0, (*rasterizer_shader_gpr_get_globals())[0], (*rasterizer_shader_gpr_get_globals())[1]);
        g_rasterizer_gpr_allocation = 3;
    }
}

static __forceinline bool render_player_view_target_allocated(long target_type)
{
    return g_dynamic_render_target_indices[target_type] != -1;
}

static __forceinline long render_player_view_get_target_index_from_type(long target_type)
{
    return g_dynamic_render_target_indices[target_type];
}

static __forceinline s_dynamic_render_target *render_player_view_get_dynamic_target(long target_index)
{
    return &((s_dynamic_render_target *)g_dynamic_render_targets->data)[datum_index_to_absolute_index(target_index)];
}

static __forceinline long render_player_view_get_frame_texture_target(long target_type, long width, long height, char *name)
{
    long target_index = g_dynamic_render_target_indices[target_type];
    return target_index != -1
        ? target_index
        : c_dynamic_render_targets::allocate_target((e_render_target_type)target_type, width, height, -1, c_render_globals::get_frame_index(), name);
}

static __forceinline void render_player_view_get_render_target_bounds(rectangle2d *bounds)
{
    bounds->y0 = 0;
    bounds->x0 = 0;
    bounds->x1 = (short)c_rasterizer::render_globals.width;
    bounds->y1 = (short)c_rasterizer::render_globals.height;
}

static __forceinline bool render_player_view_texture_camera_rendered()
{
    return (g_frame_texture_flags >> 1) & 1;
}

static __forceinline void render_player_view_set_texture_camera_rendered()
{
    g_frame_texture_flags |= FLAG(1);
}



// @address 0x1528d0 ?create_frame_textures@c_player_view@@QAAXJ@Z
void c_player_view::create_frame_textures(long view_index)
{
    render_player_view_set_texture_camera_gprs_allocation();
    for (long target_type = _render_target_type_texture_camera; target_type <= _render_target_type_last_frame_texture; target_type++)
    {
        s_render_texture_camera_globals *globals = g_render_texture_camera_globals;
        if (globals->enabled && globals->dynamic_lights && target_type == _render_target_type_texture_camera)
        {
            long target_index = render_player_view_get_frame_texture_target(target_type, 384, 384, "texture camera");
            if (target_index != -1)
            {
                rectangle2d bounds;
                render_player_view_get_render_target_bounds(&bounds);
                s_dynamic_render_target *target = render_player_view_get_dynamic_target(target_index);
                c_reflection_view reflection_view;
                c_view::begin((c_view *)&reflection_view);
                c_rasterizer::begin(bounds, bounds);
                render_setup_window(get_rasterizer_camera(), get_rasterizer_projection());
                g_rasterizer_device->SetRenderTarget(0, target->accumulation_surface);
                rasterizer_set_depth_stencil_surface_direct(target->depth_surface);
                c_rasterizer::set_render_target(1, c_rasterizer::_surface_none);
                c_rasterizer::set_render_target(2, c_rasterizer::_surface_none);
                c_rasterizer::set_render_target(3, c_rasterizer::_surface_none);
                bink_playback_render_rect(&bounds);
                c_rasterizer_texture_ref texture = target->accumulation_texture;
                c_rasterizer::resolve_entire_surface(texture, 0);
                c_view::end();
            }
        }
        else
        {
            s_render_texture_camera camera;
            if (!render_texture_camera_try_and_get(target_type, &camera, this) || render_player_view_texture_camera_rendered())
                continue;
            long target_index = render_player_view_get_frame_texture_target(target_type, camera.width, camera.height, camera.name);
            if (target_index == -1)
                continue;
            s_observer_result observer;
            observer.position = camera.position;
            observer.forward = camera.forward;
            observer.up = camera.up;
            observer.horizontal_field_of_view = camera.horizontal_field_of_view;
            observer.vertical_field_of_view = camera.vertical_field_of_view;
            observer.field_of_view_scale = 1.0f;
            s_dynamic_render_target *target = render_player_view_get_dynamic_target(target_index);
            c_texture_camera_view texture_camera_view;
            texture_camera_view.set_render_flags((g_render_texture_camera_globals->render_lights ? 0x200 : 0) | 188);
            texture_camera_view.set_render_target(target->accumulation_texture, target->albedo_texture, target->normal_texture, -1, (c_rasterizer::e_surface)3);
            texture_camera_view.set_render_target(target->accumulation_surface, target->albedo_surface, target->normal_surface, -1, target->depth_surface);
            texture_camera_view.setup_camera(&observer, -1);
            c_player_view *previous_player_view = c_player_view::get_current();
            c_player_view::set_current(&texture_camera_view);
            c_view::begin(&texture_camera_view);
            texture_camera_view.render_texture_camera(view_index);
            c_view::end();
            c_player_view::set_current(previous_player_view);
            render_player_view_set_texture_camera_rendered();
        }
    }
}

// @address 0x147098 player_view_render_frame
extern "C" void player_view_render_frame(c_player_view *frame, long p1)
{
    c_view *view = (c_view *)frame;
    c_player_view::set_current(frame);
    c_view::begin(view);
    g_occlusion_query_globals.active = 0;
    frame->create_frame_textures(p1);
    player_view_setup_lighting(p1, frame->view_index);
    view->compute_visibility();
    view->render_submit_visibility();
    view->render();
    c_view::end();
    c_player_view::set_current(0);
}

extern "C" {
// @address 0x8c48bc g_scene_texture_flags
// @address 0x8c48c0 g_uses_scene_texture
unsigned long g_uses_scene_texture;
// @address 0x8c48c4 g_scene_texture_flags_per_view
unsigned long g_scene_texture_flags_per_view[4];
// @address 0x8c48d4 g_last_lighting_view_index
long g_last_lighting_view_index;
}

// @address 0x153208 player_view_setup_lighting
extern "C" void player_view_setup_lighting(long view_index, long user_index)
{
    c_lightmap_shadows_view::setup_camera(user_index);
    c_player_view::static_lighting_setup_internals(view_index);
    g_scene_texture_flags_per_view[g_last_lighting_view_index] = g_scene_texture_flags;
    g_scene_texture_flags = 0;
    g_last_lighting_view_index = view_index;
    g_uses_scene_texture = g_scene_texture_flags_per_view[view_index];
}







// @address 0x17c010 lightmap_shadows_view_render_object
extern "C" void lightmap_shadows_view_render_object(c_lightmap_shadows_view *view, long object_index, long user_index)
{
    if (TEST_BIT(g_player_render_tuning.flags, 2))
        return;

    real one = 1.0f;
    real fade = one;
    real shadow_fade = ((unit_datum *)((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_index)].datum)->unit.active_camouflage;
    if (shadow_fade >= 0.0f && shadow_fade <= one)
        fade = one - shadow_fade;

    c_visible_items::push_marker();
    view->compute_visibility(object_index, -1);
    if (object_index != -1)
        c_visibility_collection::postprocess_filter_isolate_shadowing_object(object_index, false);
    view->submit_visibility_and_render(fade, one);
    c_rasterizer::clear_sampler_textures(0xffff);
    c_visible_items::pop_marker();
}



// @address 0x175fb0 player_view_render_pass
// match: preserve this caller's unsigned visibility mask.


extern "C" void player_view_render_pass(unsigned int p0, unsigned int p1)
{
    unsigned short count = (unsigned short)c_transparency_renderer::m_total_transparent_count;
    long index = ++c_transparency_renderer::m_current_marker_index;
    render_player_view_object_push();
    c_transparency_renderer::m_markers[index].first_transparent_index = count;
    c_transparency_renderer::transparent_sorted_order.set_count(count);
    c_visible_items::push_marker();
    render_objects_update_visible_subparts(p1);
    c_object_renderer::submit_visibility(p0 == 2);
    if (p0 >= 1)
    {
        if (p0 != 1)
        {
            if (p0 < 3)
            {
                render_player_view_sort_transparents();
                c_transparency_renderer::render(1);
            }
        }
        else
            player_view_render_opaque_objects(1);
    }
    else
        c_object_renderer::render_albedo(1);
    c_visible_items::pop_marker();
    c_transparency_renderer::pop_marker();
    render_player_view_object_pop();
}

// --- 0x170090 support types ---






extern "C" {
// @address 0x8c46ea g_render_first_person_active
unsigned char g_render_first_person_active;


}

// @address 0x170090 ?render_first_person@c_player_view@@IAAXXZ
void c_player_view::render_first_person()
{
    short zoom = -1;
    if (view_index != -1)
        zoom = player_control_globals->states[view_index].action.zoom;
    if (zoom == -1)
    {
        g_render_first_person_active = true;
        c_visible_items::push_marker();
        render_player_view_object_push();

        first_person_view.original_camera = &rasterizer_camera;
        first_person_view.render_camera_state = render_camera_state;
        first_person_view.rasterizer_camera = rasterizer_camera;

        long user = view_index;
        s_controllers *g = g_director_globals;
        if (user != -1 && g->slots[user].perspective == 0)
        {
            long unit = player_mapping_globals->unit_indices[user];
            if (unit != -1)
                visibility_render_objects_add_first_person(user, unit, true, true);
        }
        c_object_renderer::submit_visibility(8);
        c_view::begin(&first_person_view);
        c_first_person_view *first_person = &first_person_view;
        render_player_view_setup_first_person_camera(first_person, true);
        c_object_renderer::render_albedo(0x500);
        render_player_view_setup_first_person_camera(first_person, false);
        c_object_renderer::render_albedo(0x600);
        c_view::end();
        c_visible_items::pop_marker();
        g_render_first_person_active = false;
        render_player_view_object_pop();
    }
}

// 0x177218

// @address 0x177218 player_view_resolve_depth_of_field_source

#if !defined(_XBOX) || defined(__clang__)
// PC (port/GAME-CHANGES.md, "Custom resolutions"): the window's depth-of-field source (the scope screens' scene
// texture), retail's arithmetic as player_view_resolve_depth_of_field_source had it inline: the quarter-size centre of
// a width x height window around its title-safe centre (window pixels), cropped to the scene texture's 9:5.
static void player_view_depth_of_field_source_bounds(int width, int height, real center_x, real center_y,
    rectangle2d *resolve_bounds)
{
    real aspect_ratio = (real)width / (real)height;
    resolve_bounds->x0 = (short)(center_x - (real)(width / 4));
    resolve_bounds->x1 = (short)(center_x + (real)(width / 4));
    resolve_bounds->y0 = (short)(center_y - (real)(height / 4));
    resolve_bounds->y1 = (short)(center_y + (real)(height / 4));

    int resolve_width = resolve_bounds->x1 - resolve_bounds->x0;
    int resolve_height = resolve_bounds->y1 - resolve_bounds->y0;
    if (aspect_ratio > 1.801f) {
        int crop = (resolve_width - (int)floorf((real)resolve_height * 1.8f + 0.5f)) / 2;
        resolve_bounds->x0 += crop;
        resolve_bounds->x1 -= crop;
    } else if (aspect_ratio < 1.7989999f) {
        int crop = (resolve_height - (int)floorf((real)resolve_width * 0.5555556f + 0.5f)) / 2;
        resolve_bounds->y0 += crop;
        resolve_bounds->y1 -= crop;
    }
}

// The sizes c_render_surface_group::create gives the scene texture (55: the region) and the depth it is resolved through
// (53, 54: the region aligned to 32 pixels) for the full-screen view, the largest. At 1152x640 they are retail's 288x160
// and 320x192. The region in the depth's 4x view is half the window's (g_antialiasing_mode 0, the only one Halo 3 uses).
extern "C" void player_view_depth_of_field_source_extents(const rectangle2d *window_bounds, const rectangle2d *title_safe_bounds,
    long *resolve_width, long *resolve_height, long *width, long *height)
{
    rectangle2d bounds;
    player_view_depth_of_field_source_bounds(window_bounds->x1 - window_bounds->x0, window_bounds->y1 - window_bounds->y0,
        (real)(title_safe_bounds->x1 - title_safe_bounds->x0) * 0.5f + (real)title_safe_bounds->x0 - (real)window_bounds->x0,
        (real)(title_safe_bounds->y1 - title_safe_bounds->y0) * 0.5f + (real)title_safe_bounds->y0 - (real)window_bounds->y0,
        &bounds);
    const long x0 = bounds.x0 / 2, y0 = bounds.y0 / 2, x1 = bounds.x1 / 2, y1 = bounds.y1 / 2;
    *width = x1 - x0;
    *height = y1 - y0;
    *resolve_width = ((x1 + 31) & ~31) - (x0 & ~31);
    *resolve_height = ((y1 + 31) & ~31) - (y0 & ~31);
}
#endif

extern "C" void player_view_resolve_depth_of_field_source(long user_index, const rectangle2d *window_bounds, bool unused)
{
    if (!TEST_BIT(g_uses_scene_texture, 0))
        return;

    int width = window_bounds->x1 - window_bounds->x0;
    int height = window_bounds->y1 - window_bounds->y0;
    rectangle2d resolve_bounds = *window_bounds;
    const c_player_view *player_view = c_player_view::get_current();
#if !defined(_XBOX) || defined(__clang__)
    player_view_depth_of_field_source_bounds(width, height,
        (real)(player_view->rasterizer_camera.window_title_safe_pixel_bounds.x1 -
                  player_view->rasterizer_camera.window_title_safe_pixel_bounds.x0) * 0.5f
            + (real)player_view->rasterizer_camera.window_title_safe_pixel_bounds.x0
            - (real)player_view->rasterizer_camera.window_pixel_bounds.x0,
        (real)(player_view->rasterizer_camera.window_title_safe_pixel_bounds.y1 -
                  player_view->rasterizer_camera.window_title_safe_pixel_bounds.y0) * 0.5f
            + (real)player_view->rasterizer_camera.window_title_safe_pixel_bounds.y0
            - (real)player_view->rasterizer_camera.window_pixel_bounds.y0,
        &resolve_bounds);

    // the copy into the scene texture (55) is the texture's size, through the depth resolve's (53) texels: retail's
    // 288x160 and 1 / 320x192 at 1152x640 (rasterizer_render_targets_screen_size)
    const c_render_surface *scene_texture = g_rasterizer_surface_groups[55].get_specialized_surface((c_rasterizer::e_surface)55);
    const c_render_surface *depth_resolve = g_rasterizer_surface_groups[53].get_specialized_surface((c_rasterizer::e_surface)53);
    const real texel_width = 1.0f / (real)depth_resolve->description.width;
    const real texel_height = 1.0f / (real)depth_resolve->description.height;
    real x_offset = 0.0f;
    real y_offset = 0.0f;
    real x_extent = (real)scene_texture->description.width;
    real y_extent = (real)scene_texture->description.height;
    long depth_surface = 5;
#else
    real aspect_ratio = (real)width / (real)height;
    real center_x = (real)(player_view->rasterizer_camera.window_title_safe_pixel_bounds.x1 -
                             player_view->rasterizer_camera.window_title_safe_pixel_bounds.x0) * 0.5f
        + (real)player_view->rasterizer_camera.window_title_safe_pixel_bounds.x0
        - (real)player_view->rasterizer_camera.window_pixel_bounds.x0;
    real center_y = (real)(player_view->rasterizer_camera.window_title_safe_pixel_bounds.y1 -
                             player_view->rasterizer_camera.window_title_safe_pixel_bounds.y0) * 0.5f
        + (real)player_view->rasterizer_camera.window_title_safe_pixel_bounds.y0
        - (real)player_view->rasterizer_camera.window_pixel_bounds.y0;
    resolve_bounds.x0 = (short)(center_x - (real)(width / 4));
    resolve_bounds.x1 = (short)(center_x + (real)(width / 4));
    resolve_bounds.y0 = (short)(center_y - (real)(height / 4));
    resolve_bounds.y1 = (short)(center_y + (real)(height / 4));

    int resolve_width = resolve_bounds.x1 - resolve_bounds.x0;
    int resolve_height = resolve_bounds.y1 - resolve_bounds.y0;
    if (aspect_ratio > 1.801f) {
        int crop = (resolve_width - (int)floorf((real)resolve_height * 1.8f + 0.5f)) / 2;
        resolve_bounds.x0 += crop;
        resolve_bounds.x1 -= crop;
    } else if (aspect_ratio < 1.7989999f) {
        int crop = (resolve_height - (int)floorf((real)resolve_width * 0.5555556f + 0.5f)) / 2;
        resolve_bounds.y0 += crop;
        resolve_bounds.y1 -= crop;
    }

    real x_offset = 0.0f;
    real y_offset = 0.0f;
    real x_extent = 288.0f;
    real y_extent = 160.0f;
    long depth_surface = 5;
#endif
    switch (g_antialiasing_mode) {
    case 0:
        resolve_bounds.x0 /= 2;
        resolve_bounds.y0 /= 2;
        resolve_bounds.x1 /= 2;
        resolve_bounds.y1 /= 2;
        break;
    case 1:
        resolve_bounds.x0 /= 2;
        x_offset = 0.0f;
#if !defined(_XBOX) || defined(__clang__)
        x_extent = (real)scene_texture->description.width * 0.5f;
#else
        x_extent = 144.0f;
#endif
        resolve_bounds.x1 /= 2;
        break;
    case 3:
        resolve_bounds.x0 /= 2;
        depth_surface = 6;
        x_offset = 0.0f;
#if !defined(_XBOX) || defined(__clang__)
        x_extent = (real)scene_texture->description.width * 0.5f;
#else
        x_extent = 144.0f;
#endif
        resolve_bounds.x1 /= 2;
        break;
    }

    int remainder = resolve_bounds.x0 & 31;
    if (remainder != 0) {
        resolve_bounds.x0 -= remainder;
        x_offset = (real)remainder;
        x_extent += (real)remainder;
    }
    remainder = resolve_bounds.x1 & 31;
    if (remainder != 0)
        resolve_bounds.x1 = resolve_bounds.x1 - remainder + 32;
    remainder = resolve_bounds.y0 & 31;
    if (remainder != 0) {
        resolve_bounds.y0 -= remainder;
        y_offset = (real)remainder;
        y_extent += (real)remainder;
    }
    remainder = resolve_bounds.y1 & 31;
    if (remainder != 0)
        resolve_bounds.y1 = resolve_bounds.y1 - remainder + 32;

    c_rasterizer::set_render_target(0, c_rasterizer::_surface_none);
    c_rasterizer::set_render_target(1, c_rasterizer::_surface_none);
    c_rasterizer::set_depth_stencil_surface((c_rasterizer::e_surface)depth_surface);
    c_rasterizer::resolve_surface((c_rasterizer::e_surface)53, 4, &resolve_bounds, 0, 0);
    c_rasterizer::set_depth_stencil_surface(c_rasterizer::_surface_none);
    c_screen_postprocess::setup_rasterizer_for_postprocess(false);
    c_rasterizer::set_aliased_surface_as_texture(1, (c_rasterizer::e_surface)54);
#if !defined(_XBOX) || defined(__clang__)
    real_rectangle2d texcoords = { x_offset * texel_width, x_extent * texel_width, y_offset * texel_height, y_extent * texel_height };
#else
    real_rectangle2d texcoords = { x_offset * 0.003125f, x_extent * 0.003125f, y_offset * 0.0052083335f, y_extent * 0.0052083335f };
#endif
    c_rasterizer::set_depth_stencil_surface(c_rasterizer::_surface_none);
    c_screen_postprocess::copy(60, (c_rasterizer::e_surface)53, (c_rasterizer::e_surface)55, (c_rasterizer::e_sampler_filter_mode)1,
        (c_rasterizer::e_sampler_address_mode)1, k_game_engine_invalid_dead_point.z, 1000.0f, k_game_engine_invalid_dead_point.z, 1.0f, &texcoords);
    c_rasterizer::resolve_surface((c_rasterizer::e_surface)55, 0, 0, 0, 0);
}

extern "C" {

// @address 0x73a180 g_distortion_occlusion_context
long g_distortion_occlusion_context = -1;
}
static __forceinline void render_player_view_set_distortion_visible(bool visible) { g_rasterizer_distortion_visible = visible; }
static __forceinline bool render_player_view_get_distortion_active()
{
    if (g_chud_text_render_target_dirty) {
        c_render_globals::set_distortion_active(true);
        render_player_view_set_distortion_visible(true);
    }
    return c_render_globals::get_distortion_active();
}
static __forceinline bool render_player_view_get_distortion_visible() { return g_rasterizer_distortion_visible; }

// @address 0x1788b0 player_view_render_distortion
extern "C" void player_view_render_distortion(c_player_view *player_view)
{
    if (!render_player_view_get_distortion_active() || !c_screen_postprocess::x_editable_settings.enabled)
        return;
    c_transparency_renderer::push_marker();
    long window_index = player_view->view_index;
    c_rasterizer::set_z_buffer_mode(c_rasterizer::_z_buffer_mode_read_only);
    s_effects_particle_system_list *list = &g_particle_system_lists[2];
    for (long i = 0; i < list->count; ++i) {
        s_effects_particle_system_entry *entry = &list->entries[i];
        c_particle_system *system = &((c_particle_system *)g_particle_system_data_array->data)[(unsigned short)entry->particle_system_index];
        particle_system_render(system, window_index, entry->first_person, 2);
    }
    if (render_player_view_get_distortion_visible()) {
        s_main_render_view_layout layout;
        rectangle2d bounds;
        long occlusion_result;
        bounds.y0 = player_view->rasterizer_camera.window_pixel_bounds.y0 / 2;
        bounds.x0 = player_view->rasterizer_camera.window_pixel_bounds.x0 / 2;
        bounds.y1 = player_view->rasterizer_camera.window_pixel_bounds.y1 / 2;
        bounds.x1 = player_view->rasterizer_camera.window_pixel_bounds.x1 / 2;
        switch (player_view->antialiasing_mode) {
        case 1:
        case 3:
            bounds.y0 *= 2;
            bounds.y1 *= 2;
            break;
        case 2:
            bounds.x0 *= 2;
            bounds.x1 *= 2;
            bounds.y0 *= 2;
            bounds.y1 *= 2;
            break;
        }
        rasterizer_set_global_shader_parameter(3);
        main_render_get_view_layout(&layout);
        bool special = !c_rasterizer::get_is_tiling_enabled() && layout.view_count == 1;
        rasterizer_setup_distortion_targets(&bounds, special);
        rasterizer_occlusion_submit(_occlusion_query_type_distortion, 6, 0, player_view->user_index, player_view_handle_occlusion_callback);
        rasterizer_occlusions_get_result(_occlusion_query_type_distortion, 0, player_view->user_index, &occlusion_result, &g_distortion_occlusion_context);
        if (g_distortion_occlusion_context != -1)
            D3DDevice_BeginConditionalRendering(g_rasterizer_device, g_distortion_occlusion_context);
        c_rasterizer::resolve_surface((c_rasterizer::e_surface)52, 0, &bounds, bounds.x0, bounds.y0);
        if (g_distortion_occlusion_context != -1)
            D3DDevice_EndConditionalRendering(g_rasterizer_device);
    }
    c_transparency_renderer::pop_marker();
}



void c_player_view::render_lens_flares()
{
    g_rasterizer_using_albedo_sampler = false;
    int enabled = true;
    g_rasterizer_device->SetPixelShaderConstantB(0, &enabled, 1);
    rasterizer_occlusions_retrieve(user_index);
    lens_flares_render(view_index);
    bool albedo = rasterizer_is_surface_resolved((c_rasterizer::e_surface)17);
    g_rasterizer_using_albedo_sampler = albedo;
    int disabled = !albedo;
    g_rasterizer_device->SetPixelShaderConstantB(0, &disabled, 1);
}

long c_player_view::get_player_view_user_index()
{
    return view_index;
}




void render_player_view_build_lighting(
    c_lights_view *lights, long user, long window, D3DSurface *surface0, D3DSurface *surface1, D3DSurface *surface2)
{
    lights->render_target0 = surface0;
    lights->render_target1 = surface1;
    lights->user_index = user;
    lights->depth_stencil = surface2;
    render_lights_iterate_lights(lights, user, window, g_player_render_tuning.lighting_pass, true, false, lights->exposure,
        reinterpret_cast<void (*)(int, int, float, void *)>(&render_lights_render_light));
}


extern "C" {

// @address 0x72cb02 g_motion_blur_enabled

// @address 0x832a68 g_rasterizer_resolved_surface_mask

}

static __forceinline c_director *render_player_view_get_director(long user)
{
    return g_director_globals->directors[user].get_director();
}

static __forceinline bool render_player_view_clear_cooperative_albedo(c_player_view *view)
{
    return game_is_cooperative() && game_globals->options.game_mode == 1 &&
        render_player_view_get_director(view->view_index)->get_type() == 1 &&
        render_player_view_get_director(view->view_index)->get_camera()->get_type() == 2;
}

static __forceinline void render_player_view_set_vertex_shader_constant_owned(long start_register, long count, const float *constants)
{
    D3DVECTOR4 *data = D3DDevice_GpuBeginShaderConstantF4(g_rasterizer_device, false, start_register, count);
    memcpy(data, constants, count * sizeof(D3DVECTOR4));
}

static __forceinline void render_player_view_submit_albedo_lights(c_lights_view *lights, long start_register)
{
    c_lights_view *const *lights_ref = &lights;
    int constants[4];
    constants[0] = (*lights_ref)->simple_light_count;
    constants[1] = 0;
    constants[2] = 5;
    constants[3] = 0;
    g_rasterizer_device->SetVertexShaderConstantI(0, constants, 1);
    if (lights->simple_light_count > 0)
    {
        render_player_view_set_vertex_shader_constant_owned(start_register, 80, lights->simple_light_data);
    }
}

// @address 0x16e730 ?render_albedo@c_player_view@@IAA_NXZ
bool c_player_view::render_albedo()
{
    bool result = false;
    c_rasterizer::end_high_quality_blend();
    render_player_view_setup_albedo_targets(false);
    if (render_player_view_clear_cooperative_albedo(this))
    {
        c_rasterizer::clear(1, 0x707070, 1.0f, 0);
    }
    c_render_globals::set_skinning_constants_owned(true);
    g_rasterizer_device->m_Pending.m_Mask[0] &= 0xf00000000000007fui64;
    if (g_rasterizer_gpr_allocation != 5)
    {
        (*rasterizer_shader_gpr_get_globals())[0] = 75;
        (*rasterizer_shader_gpr_get_globals())[1] = 53;
        g_rasterizer_device->SetShaderGPRAllocation(0, 75, 53);
        g_rasterizer_gpr_allocation = 5;
    }
    render_player_view_set_exposure_constants(exposure, exposure_multiplier, c_camera_fx_values::g_HDR_target_stops, false);
    render_player_view_submit_albedo_lights(&lights_view, 16);
    bool &decorator_albedo = g_decorator_albedo_active;
    if (g_motion_blur_enabled)
    {
        if (g_rasterizer_alpha_blend_mode != 11)
        {
            blend_state_value blend_state = { 0xe0001 };
            g_rasterizer_alpha_blend_mode = 11;
            g_rasterizer_device->SetRenderState(D3DRS_BLENDFACTOR, 0);
            rasterizer_set_packed_blend_state(g_rasterizer_device, 0, blend_state);
        }
        decorator_albedo = true;
    }
    c_structure_renderer::render_decorators();
    if (g_motion_blur_enabled)
    {
        decorator_albedo = false;
        if (g_rasterizer_alpha_blend_mode != 0)
        {
            g_rasterizer_alpha_blend_mode = 0;
            rasterizer_set_device_alpha_blend_mode(g_rasterizer_device, 0);
        }
    }
    c_rasterizer::set_gprs_allocation((c_rasterizer::e_gpr_allocation)1);
    if (!TEST_BIT(g_uses_scene_texture, 0))
        render_first_person();
    c_object_renderer::render_albedo(1);
    c_structure_renderer::render_albedo();
    player_view_render_pass(0, user_index);
    c_rasterizer::disown_vertex_shader_constants(16, 212);
    c_render_globals::set_skinning_constants_owned(false);
    player_view_render_transparent_objects(true, true);
    if (!(D3DDevice_GetDeviceState(g_rasterizer_device) & 1))
        player_view_resolve_depth_of_field_source(user_index, &get_rasterizer_camera()->window_pixel_bounds, false);
    c_rasterizer::set_depth_stencil_surface((c_rasterizer::e_surface)3);
    c_rasterizer::set_render_target(0, (c_rasterizer::e_surface)17);
    c_rasterizer::set_render_target(1, (c_rasterizer::e_surface)19);
    c_rasterizer::set_render_target(2, (c_rasterizer::e_surface)0);
    c_rasterizer::set_render_target(3, (c_rasterizer::e_surface)0);
    c_rasterizer::set_viewport(g_rasterizer_last_rectangles.viewport, 0.0f, 1.0f);
    c_rasterizer::set_scissor_rect(&g_rasterizer_last_rectangles.scissor);
    if (TEST_BIT(g_uses_scene_texture, 0))
        render_first_person();
    if (D3DDevice_GetDeviceState(g_rasterizer_device) & 1)
    {
        c_rasterizer::resolve_surface_tiled((c_rasterizer::e_surface)3, 4, false, true);
        const c_render_surface *surface =
            render_texture_slot_get_surface(&g_rasterizer_surface_groups[3], 3, c_rasterizer::g_screen_resolution, 0);
        g_rasterizer_resolved_surface_mask[surface->index >> 5] |= 1 << (surface->index & 31);
    }
    else
    {
        const rectangle2d *bounds = &get_rasterizer_camera()->window_pixel_bounds;
        c_rasterizer::resolve_surface((c_rasterizer::e_surface)3, 4, bounds, bounds->x0, bounds->y0);
    }
    c_rasterizer::begin_high_quality_blend();
    if (player_view_resolve_albedo_surfaces(&get_rasterizer_camera()->window_pixel_bounds))
        result = true;
    return result;
}

extern "C" {
// @address 0x8c46c8 g_rasterizer_max_vs_gprs
// @address 0x8c46fc g_depth_render_mode
}

// @address 0x73a174 ?render_debug_depth_render_scale_r@@3MA
float render_debug_depth_render_scale_r = 9.0f;
// @address 0x73a178 ?render_debug_depth_render_scale_g@@3MA
float render_debug_depth_render_scale_g = 10.0f;
// @address 0x73a17c ?render_debug_depth_render_scale_b@@3MA
float render_debug_depth_render_scale_b = 9.0f;

static __forceinline void render_player_view_set_static_lighting_gprs_allocation()
{
    if (g_rasterizer_gpr_allocation != 2)
    {
        (*rasterizer_shader_gpr_get_globals())[0] = g_rasterizer_max_vs_gprs;
        (*rasterizer_shader_gpr_get_globals())[1] = 128 - g_rasterizer_max_vs_gprs;
        g_rasterizer_device->SetShaderGPRAllocation(0, (*rasterizer_shader_gpr_get_globals())[0], (*rasterizer_shader_gpr_get_globals())[1]);
        g_rasterizer_gpr_allocation = 2;
    }
}

static __forceinline void set_alpha_blend_mode_cached(long mode)
{
    if (g_rasterizer_alpha_blend_mode != mode)
    {
        g_rasterizer_alpha_blend_mode = mode;
        if (mode == 11)
        {
            D3DBLENDSTATE blend_state = { 1, 0, 0, 14, 0, 0 };
            g_rasterizer_device->SetRenderState(D3DRS_BLENDFACTOR, 0);
            g_rasterizer_device->SetBlendState(0, blend_state);
        }
        else
            rasterizer_set_device_alpha_blend_mode(g_rasterizer_device, mode);
    }
}

static const long *render_player_view_get_depth_render_address()
{
    return &g_depth_render_mode;
}

// @address 0x178d90 ?render_static_lighting@c_player_view@@IAAXXZ
void c_player_view::render_static_lighting()
{
    real_matrix4x3 camera;
    get_player_render_camera_orientation(&camera);
    bool hdr_alpha_blend = c_screen_postprocess::x_editable_settings.enabled || rasterizer_tiling_get_state()->enabled;
    c_rasterizer::setup_targets_static_lighting(exposure, exposure_multiplier, hdr_alpha_blend, c_camera_fx_values::g_HDR_target_stops, true);
    if (inl_cinematic_in_progress())
        cinematic_clear_letterbox_bars(hdr_alpha_blend);
    lights_view.submit_simple_light_draw_list_to_shader();
    c_rasterizer::end_high_quality_blend();
    render_player_view_set_static_lighting_gprs_allocation();

    long window_index = view_index;
    c_rasterizer::set_z_buffer_mode(c_rasterizer::_z_buffer_mode_normal);
    s_effects_particle_system_list *list = g_particle_system_lists;
    for (long i = 0; i < list->count; ++i)
    {
        s_effects_particle_system_entry *entry = &list->entries[i];
        c_particle_system *system = &((c_particle_system *)g_particle_system_data_array->data)[(unsigned short)entry->particle_system_index];
        particle_system_render(system, window_index, entry->first_person, 0);
    }
    g_rasterizer_device->m_Pending.m_Mask[0] &= 0xf00000000000007fui64;
    c_render_globals::set_skinning_constants_owned(true);

    if (g_depth_render_mode <= 0)
    {
        if (g_motion_blur_enabled)
        {
            set_alpha_blend_mode_cached(11);
            g_decorator_albedo_active = true;
        }
        player_view_render_opaque_objects(1);
        structure_renderer_render_lighting();
        player_view_render_pass(1, user_index);
        if (g_motion_blur_enabled)
        {
            g_decorator_albedo_active = false;
            set_alpha_blend_mode_cached(0);
        }
    }
    if (g_depth_render_mode != 0)
    {
        c_screen_postprocess::setup_rasterizer_for_postprocess(false);
        if (g_depth_render_mode < 0)
            set_alpha_blend_mode_cached(1);
        long shader = abs(g_depth_render_mode) == 1 ? 54 : 59;
        if (c_rasterizer::set_explicit_shaders(shader, _vertex_type_screen, _transfer_vector_vertex_none, _entry_point_default))
        {
            c_rasterizer::set_aliased_surface_as_texture(0, (c_rasterizer::e_surface)3);
            c_rasterizer::set_sampler_address_mode(0, (c_rasterizer::e_sampler_address_mode)1, (c_rasterizer::e_sampler_address_mode)1,
                (c_rasterizer::e_sampler_address_mode)1);
            c_rasterizer::set_sampler_filter_mode(0, c_rasterizer::_sampler_filter_mode_point);
            real_vector4d constants;
            constants.n[0] = (float)pow(2.0, (double)render_debug_depth_render_scale_r);
            constants.n[1] = (float)pow(2.0, (double)render_debug_depth_render_scale_g);
            constants.n[2] = (float)pow(2.0, (double)render_debug_depth_render_scale_b);
            constants.n[3] = 1.0f;
            c_rasterizer::set_pixel_shader_constant(2, 1, &constants);
            c_rasterizer::draw_fullscreen_quad(
                c_rasterizer::get_surface_height((c_rasterizer::e_surface)3), c_rasterizer::get_surface_width((c_rasterizer::e_surface)3));
        }
        set_alpha_blend_mode_cached(0);
    }
    c_rasterizer::disown_vertex_shader_constants(16, 212);
    c_render_globals::set_skinning_constants_owned(false);
    c_decal_system::render_all((c_decal_definition::e_pass)1);
    c_rasterizer::begin_high_quality_blend();
}


static __forceinline void render_player_view_first_person_begin_alpha_blend()
{
    if (g_rasterizer_alpha_blend_mode != 12)
    {
        g_rasterizer_alpha_blend_mode = 12;
        blend_state_value blend_state = { 0xe0001 };
        g_rasterizer_device->SetRenderState(D3DRS_BLENDFACTOR, (DWORD)-1);
        rasterizer_set_packed_blend_state(g_rasterizer_device, 0, blend_state);
    }
}

static __forceinline void render_player_view_first_person_push_objects()
{
    s_render_object_globals *globals = g_render_object_globals;
    long *marker = &globals->marker;
    ++*marker;
    globals->stack_a[*marker] = globals->current_a;
    globals->stack_b[*marker] = globals->current_b;
}

// @address 0x17f490 player_view_render_first_person_effects
extern "C" void player_view_render_first_person_effects(c_player_view *view, unsigned char render_effects)
{
    short zoom = -1;
    if (view->view_index != -1)
        zoom = player_control_globals->states[view->view_index].action.zoom;
    if (zoom != -1)
        return;

    c_rasterizer::set_depth_stencil_surface((c_rasterizer::e_surface)3);
    c_rasterizer::set_z_buffer_mode(c_rasterizer::_z_buffer_mode_normal);
    view->lights_view.submit_simple_light_draw_list_to_shader();

    c_transparency_renderer::push_marker();
    c_visible_items::push_marker();
    render_player_view_first_person_push_objects();

    render_camera *rasterizer_camera = &view->rasterizer_camera;
    view->first_person_view.original_camera = rasterizer_camera;
    view->first_person_view.render_camera_state = view->render_camera_state;
    view->first_person_view.rasterizer_camera = *rasterizer_camera;

    c_view::begin(&view->first_person_view);

    if (!render_effects)
    {
        rasterizer_set_global_shader_parameter(1);
        rasterizer_set_global_shader_parameter(2);
    }
    c_first_person_view *first_person_view = &view->first_person_view;
    render_player_view_setup_first_person_camera(first_person_view, true);

    long user_index = view->view_index;
    s_controllers *director_globals = (s_controllers *)g_director_globals;
    if (user_index != -1 && director_globals->slots[user_index].perspective == 0)
    {
        long player_index = player_mapping_globals->unit_indices[user_index];
        if (player_index != -1)
            visibility_render_objects_add_first_person(user_index, player_index, true, true);
    }
    c_object_renderer::submit_visibility(9);

    if (!render_effects)
    {
        c_rasterizer::set_using_albedo_sampler(true);
        if (g_motion_blur_enabled)
        {
            render_player_view_first_person_begin_alpha_blend();
            g_decorator_albedo_active = 1;
        }
        player_view_render_first_person_albedo(first_person_view, view->view_index);
        if (g_motion_blur_enabled)
        {
            g_decorator_albedo_active = 0;
            set_alpha_blend_mode_cached(0);
        }
    }
    else
    {
        effects_render_pass(view->view_index, 3);
        c_rasterizer::set_using_albedo_sampler(false);
        rasterizer_set_global_shader_parameter(3);
        rasterizer_set_global_shader_parameter(45);
        render_player_view_initialize_window_bounds(&rasterizer_camera->window_pixel_bounds, &rasterizer_camera->fullscreen_pixel_bounds);
        render_player_view_sort_transparents();
        c_transparency_renderer::render(true);
    }

    c_view::end();
    c_visible_items::pop_marker();
    c_object_renderer::pop_marker();
    c_transparency_renderer::pop_marker();
    c_rasterizer::set_using_albedo_sampler(rasterizer_is_surface_resolved((c_rasterizer::e_surface)17));
}

t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_render_texture_camera_globals_allocator> g_render_texture_camera_globals_allocator;
