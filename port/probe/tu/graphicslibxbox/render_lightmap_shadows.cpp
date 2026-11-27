// graphicslibxbox/render_lightmap_shadows.obj: from the records 0x153280 0x17c0f8 0x17c170 0x17d6f8 0x17ed60 0x45d798 0x45d8d8 0x45db78 0x17f180
#include "render/render_lightmap_shadows_globals.h"
#include "visibility/visibility_globals.h"
#include "effects/effect_render_globals.h"
#include "rasterizer/rasterizer_state_globals.h"
#include "rasterizer/rasterizer_tiling_types.h"
#include "rasterizer/rasterizer_tiling_globals.h"
#include "memory/gameplay_tls_globals.h"
#include "memory/tls_globals.h"
#include "visibility/visible_items_types.h"
#include "visibility/visible_items.h"
#include "render/render_object_storage.h"
#include "render/render_view_stack_globals.h"
#include "sky_atm/weighted_atmosphere_parameters.h"
#include "render/render_object_globals_types.h"
#include "rasterizer/rasterizer_main_globals.h"
#include "rasterizer/rasterizer_viewport_state.h"
#include "render/render_lightmap_shadows_calls.h"
#include "render/render_camera_types.h"
#include <xtl.h>
#include <xboxmath.h>
#include <new>
#include "objects/object_datum.h"
#include "objects/object_header_access.h"
#include "objects/object_definitions.h"
#include "game/player_mapping_types.h"
#include "cutscene/cinematic_globals_types.h"
#include "cutscene/cinematics.h"
#include "models/model_types.h"
#include "objects/objects.h"
#include "objects/object_header.h"
#include "camera/camera_scripting.h"
#include "objects/lights.h"
#include "render/render_mesh.h"
#include "render/render_visibility_collection.h"
#include "visibility/visibility_projections_and_volumes.h"
#include "render/render_cameras.h"
#include "render/render_objects_static_lighting.h"
#include "render/render_lens_flares.h"
#include "rasterizer/rasterizer_states.h"
#include "render/render_lightmap_shadows.h"
#include "render/render_structure.h"
#include "sky_atm/atmosphere.h"
#include "visibility/visibility_projection_types.h"
#include "sky_atm/real_linear_rgb_color_types.h"
#include "objects/light_intermediate_geometry_types.h"
#include "objects/light_geometry_types.h"
#include "render/shader_reference_types.h"
#include "cache/cache_files.h"
#include "rasterizer/rasterizer_render_globals_types.h"
#include "render/render.h"
#include "render/render_occlusions.h"
#include "math/real_math.h"
#include "math/matrix_math.h"
#include "memory/data.h"
#include "memory/datum_index.h"
#include "render/render_lighting_types.h"
#include "memory/tls_data_arrays.h"
#include <ppcintrinsics.h>
#include <vectorintrinsics.h>
#include <string.h>
#include "tag_files/tag_types.h"
#include "rasterizer/rasterizer.h"
#include "rasterizer/rasterizer_hdr_targets.h"
#include "render/views/lightmap_shadows_view.h"
#include "visibility/visibility_collection.h"
#include "render/render_first_person_globals_types.h"
#include "camera/director_globals_types.h"
#include "render/structure_renderer.h"
#include "render/object_renderer.h"
#include "sky_atm/atmosphere_fog_interface.h"
#include "render/render_view.h"
#include "rasterizer/rasterizer_textures_cache.h"
#include "render/views/texture_camera_view.h"
#include "render/render_structure_globals_types.h"
#include "rasterizer/xenon/xenon_matrix.h"
#include "render/render_camera_projection.h"

struct s_render_object_first_person_globals;
struct object_datum;
struct render_lighting;
struct D3DDevice;
struct s_shadow_bounds_vertices
{
    short vertices[56][4];
    float following_constants[2];
};
// @address 0x11df71c g_effect_render_globals
// @address 0x8368d0 g_player_render_tuning
// @flags /TP /O2 /J /Oc
// @address 0x45d798 ?render_lightmap_shadow_calculate_radius@@YAXJPBTreal_point3d@@PBTreal_vector3d@@PAM2_N3@Z


typedef char render_lightmap_shadows_canonical_layout_1[(sizeof(void *) != 4 || ((sizeof(c_view) == 0x294))) ? 1 : -1];
typedef char render_lightmap_shadows_canonical_layout_2[
    (sizeof(void *) != 4 || ((offsetof(c_lightmap_shadows_view, m_object_bounding_box) == 0x298))) ? 1 : -1];

static __forceinline bool render_lightmap_shadows_rectangle_intersects(const real_rectangle2d *a, const real_rectangle2d *b)
{
    return a->x0 <= b->x1 && a->x1 >= b->x0 && a->y0 <= b->y1 && a->y1 >= b->y0;
}


void render_lightmap_shadow_calculate_radius(long object_index, const real_point3d *position, const real_vector3d *forward,
    float *frustum_radius, float *projected_radius, bool a, bool b)
{
    if (object_index != -1)
    {
        object_datum *object = object_get(object_index);
        real_vector3d to_object;
        real_vector3d along;
        float projected = 0.0f;
        float frustum = 0.0f;

        vector_from_points3d(position, &object->bounding_center, &to_object);
        float dot = dot_product3d(&to_object, forward);
        scale_vector3d(forward, dot, &along);
        subtract_vectors3d(&to_object, &along, &to_object);
        frustum = magnitude3d(&to_object);
        projected = magnitude3d(&along);

        if (frustum + object->bounding_radius > *frustum_radius)
            *frustum_radius = frustum + object->bounding_radius;
        if (projected + object->bounding_radius > *projected_radius)
            *projected_radius = projected + object->bounding_radius;

        if (b && object->next_object_index != -1)
            render_lightmap_shadow_calculate_radius(object->next_object_index, position, forward, frustum_radius, projected_radius, a, b);
        if (a && object->first_child_object_index != -1)
            render_lightmap_shadow_calculate_radius(object->first_child_object_index, position, forward, frustum_radius, projected_radius, a, true);
    }
}


// @address 0x17c0f8 ?compute_visibility@c_lightmap_shadows_view@@QAAXJJ@Z
void c_lightmap_shadows_view::compute_visibility(long object_index, long light_index)
{
    light_geometry geometry;
    if (object_shadow_visible(object_index, &m_object_bounding_box, &geometry))
        render_visibility_add_object_to_items(object_index, g_visibility_globals.collection_input->option1, false, true);
}


#ifndef ABS
#define ABS(x) ((x) >= 0.0f ? (x) : -(x))
#endif


// @address 0x17c170 ?object_shadow_visible@c_lightmap_shadows_view@@SA_NJPAUs_oriented_bounding_box@@PAUlight_geometry@@@Z
bool c_lightmap_shadows_view::object_shadow_visible(long object_index, s_oriented_bounding_box *obb, light_geometry *geometry)
{
    bool offset_origin = true;
    s_oriented_bounding_box default_obb;
    light_geometry default_geometry;
    if (!obb)
        obb = &default_obb;
    if (!geometry)
        geometry = &default_geometry;

    const render_lighting *lighting = object_get_cached_render_lighting(object_index);
    real_vector3d light_direction = lighting ? lighting->down : *global_down3d;
    if (!valid_real_vector3d(&light_direction))
        light_direction = *global_down3d;

    real_point3d center;
    float radius;
    object_get_bounding_sphere(object_index, &center, &radius);
    float projected_radius = MAX(radius, 0.0002f);
    float frustum_radius = projected_radius;
    render_lightmap_shadow_calculate_radius(object_index, &center, &light_direction, &frustum_radius, &projected_radius, true, false);

    real_point3d origin = center;
    real_vector3d forward = light_direction;
    if (normalize3d(&forward) == 0.0f)
        forward = *global_up3d;

    geometry->type = 1;
    geometry->field_of_view = 0.0055555557f;
    geometry->far_distance = 1.0f;
    float width = MAX(ABS(forward.i), ABS(forward.j));
    geometry->near_width = 2.0f * projected_radius + width;
    geometry->far_width = geometry->near_width + projected_radius;
    if (offset_origin) {
        point_from_line3d(&origin, &forward, -projected_radius, &origin);
    } else {
        geometry->near_width -= projected_radius;
        geometry->far_width -= projected_radius;
    }
    geometry->near_distance = 2.0f * frustum_radius;

    real_vector3d up;
    if (fabsf(dot_product3d(global_left3d, &forward)) < 0.9f)
        cross_product3d(&forward, global_left3d, &up);
    else
        cross_product3d(&forward, global_backward3d, &up);
    normalize3d(&up);
    if (sqrtf(magnitude_squared3d(&forward)) <= 0.995f
        || sqrtf(magnitude_squared3d(&up)) <= 0.995f
        || !valid_real_vector3d_axes2(&forward, &up))
    {
        forward = *global_down3d;
        up = *global_forward3d;
    }

    light_build_intermediate_geometry(&origin, &origin, &forward, &up, 1.0f, -1, geometry);

    obb->extents[0] = geometry->far_width / 2.0f;
    obb->extents[1] = geometry->intermediate.frustum.near_extent.x / 2.0f;
    obb->extents[2] = geometry->intermediate.frustum.near_extent.y / 2.0f;
    obb->matrix.forward = forward;
    obb->matrix.up = up;
    cross_product3d(&up, &forward, &obb->matrix.left);
    point_from_line3d(&origin, &obb->matrix.forward, obb->extents[0], &obb->matrix.position);
    obb->matrix.scale = k_real_one;
    return obb_visible(obb);
}


// @address 0x45d8d8 ?obb_visible@@YA_NPAUs_oriented_bounding_box@@@Z
bool obb_visible(s_oriented_bounding_box *obb)
{
    s_visibility_input *input = g_visibility_globals.collection_input;
    real_rectangle3d rect;
    rect.x0 = -obb->extents[0];
    rect.x1 = obb->extents[0];
    rect.y0 = -obb->extents[1];
    rect.y1 = obb->extents[1];
    rect.z0 = -obb->extents[2];
    rect.z1 = obb->extents[2];
    real_point3d edges[12][2];
    rectangle3d_build_edges(&rect, 12, edges);
    real_point3d transformed[12][2];
    for (long i = 0; i < 12; i++)
    {
        inl_matrix4x3_transform_point(&obb->matrix, &edges[i][0], &transformed[i][0]);
        inl_matrix4x3_transform_point(&obb->matrix, &edges[i][1], &transformed[i][1]);
    }
    bool visible = visibility_project_connected_points_clipped(
        &input->projection, input->projection.near_clip, &transformed[0][0], 24, 0, 0, 0, &rect, true, false, false);
    return visible & render_lightmap_shadows_rectangle_intersects(&input->projection.volume.rectangle, (const real_rectangle2d *)&rect);
}

extern "C" {
// @address 0x7374fc g_first_person_object_index
long g_first_person_object_index = -1;
// @address 0x737500 g_first_person_physics_object_index
long g_first_person_physics_object_index = -1;
// @address 0x72cb02 g_motion_blur_enabled
bool g_motion_blur_enabled = true;
}


// @address 0x153280 ?setup_camera@c_lightmap_shadows_view@@SAXJ@Z
void c_lightmap_shadows_view::setup_camera(long user_index)
{
    s_render_object_first_person_globals *globals = &render_object_globals;
    globals->object_index = -1;
    globals->model_count = 0;
    long camera_object = scripted_camera_get_first_person_camera_object();
    globals->object_index = camera_object;
    if (camera_object == -1 && user_index != -1 && tls_director_globals()->slots[user_index].perspective == 0)
    {
        long player = tls_player_mapping_globals()->unit_indices[user_index];
        globals->object_index = player;
        globals->model_count = render_camera_build_first_person_models(user_index, player, 4, globals->models);
    }

    g_first_person_object_index = -1;
    g_motion_blur_enabled = g_effect_render_globals->render_unknown0;
    if (user_index != -1 && ((1UL << tls_director_globals()->slots[user_index].perspective) & 3) != 0)
    {
        long unit_index = tls_player_mapping_globals()->unit_indices[user_index];
        if (unit_index != -1)
            g_first_person_object_index = inl_object_get_ultimate_parent(unit_index);
    }

    g_first_person_physics_object_index = -1;
    if (g_first_person_object_index != -1)
    {
        object_datum *object = object_get(g_first_person_object_index);
        long local_physics_object_index = -1;
        if ((unsigned char)((object->physics_flags >> 1) & 1) || (unsigned char)((object->physics_flags >> 2) & 1))
            local_physics_object_index = object->local_physics_object_index;
        g_first_person_physics_object_index = local_physics_object_index;
        if (g_first_person_physics_object_index != -1 &&
            (unsigned char)((object_get(g_first_person_physics_object_index)->physics_flags >> 9) & 1))
            g_first_person_physics_object_index = -1;
    }
}


// @rdata 0x820242d8
static const float render_lightmap_shadows_two = 2.0f;

static __forceinline float render_lightmap_shadows_square(float value)
{
    return value * value;
}

static __forceinline real_vector3d *render_lightmap_shadows_fog_vector(const real_point3d *eye, const real_point3d *point, real_vector3d *direction)
{
    direction->i = point->x - eye->x;
    direction->k = point->z - eye->z;
    direction->j = point->y - eye->y;
    return direction;
}

static __forceinline void render_lightmap_shadows_set_scaled_color(real_linear_rgb_color *result, float scale, const real_rgb_color *color)
{
    result->red = color->red * scale;
    result->green = color->green * scale;
    result->blue = color->blue * scale;
}

static __forceinline void render_lightmap_shadows_add_colors(const real_linear_rgb_color *a, const real_linear_rgb_color *b, real_linear_rgb_color *result)
{
    result->red = a->red + b->red;
    result->green = a->green + b->green;
    result->blue = a->blue + b->blue;
}

static __forceinline void render_lightmap_shadows_add_scattering_coefficients(
    const real_rgb_color *a, const real_rgb_color *b, real_vector3d *result)
{
    result->i = b->red + a->red;
    result->j = a->green + b->green;
    result->k = a->blue + b->blue;
}

static __forceinline void render_lightmap_shadows_multiply_colors(const real_linear_rgb_color *a, const real_linear_rgb_color *b, real_linear_rgb_color *result)
{
    result->red = a->red * b->red;
    result->green = a->green * b->green;
    result->blue = a->blue * b->blue;
}

static __forceinline void render_lightmap_shadows_multiply_colors(const real_rgb_color *a, const real_linear_rgb_color *b, real_linear_rgb_color *result)
{
    result->red = a->red * b->red;
    result->green = a->green * b->green;
    result->blue = a->blue * b->blue;
}

// @address 0x17ed60 render_lightmap_shadows_compute_scattering
// match: 0x17ed60: pointer __restrict qualifier prevents alias reload
extern "C" void render_lightmap_shadows_compute_scattering(const s_weighted_atmosphere_parameters *__restrict parameters,
    const real_point3d *point, const real_point3d *eye, float distance_bias, real_linear_rgb_color *transmittance,
    real_linear_rgb_color *inscattering)
{
    if (g_effect_render_globals->camera_relative && parameters->valid)
    {
        real_vector3d direction;
        render_lightmap_shadows_fog_vector(eye, point, &direction);
        float distance = normalize3d(&direction);
        real_vector3d sun_direction = parameters->sun_direction;
        normalize3d_with_default(&sun_direction, global_down3d);
        float cosine = -dot_product3d(&sun_direction, &direction);
        float fog_distance = MAX(parameters->sun_intensity + distance + distance_bias, 0.0f);
        fog_distance = MIN(fog_distance, parameters->distance_bias);
        const real_rgb_color *mie_scattering = &parameters->mie_scattering;
        float point_height = MAX(point->z - parameters->rayleigh_height, 0.0f);
        float eye_height = MAX(eye->z - parameters->rayleigh_height, 0.0f);
        const float *rayleigh_density_scale = &parameters->fog_intensity;
        float height_difference = point_height - eye_height;
        float rayleigh_integral;
        float mie_integral;
        if (height_difference * height_difference > 0.001f)
        {
            float point_density = (float)exp(-(point_height / *rayleigh_density_scale));
            float eye_density = (float)exp(-(eye_height / *rayleigh_density_scale));
            rayleigh_integral = -((point_density - eye_density) * *rayleigh_density_scale / height_difference * fog_distance);
            point_density = (float)exp(-(point_height / parameters->mie_height));
            eye_density = (float)exp(-(eye_height / parameters->mie_height));
            mie_integral = -((point_density - eye_density) / height_difference * parameters->mie_height * fog_distance);
        }
        else
        {
            rayleigh_integral = (float)exp(-(point_height / *rayleigh_density_scale)) * fog_distance;
            mie_integral = (float)exp(-(point_height / parameters->mie_height)) * fog_distance;
        }
        const real_rgb_color *rayleigh_scattering = &parameters->rayleigh_scattering;
        const float mie_red = (float)mie_scattering->red;
        // match: 0x17ed60: const reference binding preserves temporary lifetime
        const float &rayleigh_red = (float)rayleigh_scattering->red;
        transmittance->red = (float)exp(-(mie_red * rayleigh_integral + rayleigh_red * mie_integral));
        transmittance->green = (float)exp(-(rayleigh_scattering->green * mie_integral + mie_scattering->green * rayleigh_integral));
        transmittance->blue = (float)exp(-(rayleigh_scattering->blue * mie_integral + mie_scattering->blue * rayleigh_integral));
        real_linear_rgb_color scattering_fraction;
        scattering_fraction.red = 1.0f - transmittance->red;
        scattering_fraction.green = 1.0f - transmittance->green;
        scattering_fraction.blue = 1.0f - transmittance->blue;
        float rayleigh_phase = render_lightmap_shadows_square(cosine) + 1.0f;
        float anisotropy = parameters->mie_anisotropy;
        const float &square_anisotropy = parameters->mie_anisotropy;
        float anisotropy_squared = render_lightmap_shadows_square(square_anisotropy);
        float inverse_mie_distance = 1.0f / square_root(anisotropy + 1.0f - anisotropy * cosine * render_lightmap_shadows_two);
        float mie_phase = (1.0f - anisotropy_squared) * (inverse_mie_distance * inverse_mie_distance * inverse_mie_distance);
        real_linear_rgb_color rayleigh_color;
        real_linear_rgb_color mie_color;
        real_linear_rgb_color scattered;
        render_lightmap_shadows_set_scaled_color(&rayleigh_color, rayleigh_phase, &parameters->fog_color);
        render_lightmap_shadows_set_scaled_color(&mie_color, mie_phase, &parameters->mie_color);
        const real_rgb_color *sun_color = &parameters->sun_color;
        render_lightmap_shadows_add_colors(&rayleigh_color, &mie_color, &rayleigh_color);
        render_lightmap_shadows_multiply_colors(&rayleigh_color, &scattering_fraction, &rayleigh_color);
        render_lightmap_shadows_multiply_colors(sun_color, &rayleigh_color, &scattered);
        real_vector3d scattering;
        render_lightmap_shadows_add_scattering_coefficients(&parameters->rayleigh_scattering, &parameters->mie_scattering, &scattering);
        if (!(magnitude_squared3d(&scattering) < 0.0001f))
        {
            inscattering->red = scattered.red / scattering.i;
            inscattering->green = scattered.green / scattering.j;
            inscattering->blue = scattered.blue / scattering.k;
        }
        else
        {
            inscattering->red = 0.0f;
            inscattering->green = 0.0f;
            inscattering->blue = 0.0f;
        }
    }
    else
    {
        transmittance->red = transmittance->green = transmittance->blue = 1.0f;
        inscattering->blue = 0.0f;
        inscattering->red = 0.0f;
        inscattering->green = 0.0f;
    }
}


extern "C" {
// @address 0x81dc04 g_view_stack
// @address 0x8c484e g_render_static_lighting_active
// @address 0x8c483a g_debug_shadow_bounds_solid
bool g_debug_shadow_bounds_solid;
// @address 0x8c483b g_debug_shadow_histencil
bool g_debug_shadow_histencil;
// @address 0x81da28 g_rasterizer_viewport_bounds
#if defined(__clang__)
#include "rasterizer/rasterizer_viewport_state.h"
#define g_rasterizer_viewport_bounds (g_rasterizer_last_rectangles.viewport)
#endif

}

static __forceinline real_vector4d *render_lightmap_shadows_set_vector4d(real_vector4d *vector, float i, float j, float k, float l)
{
    vector->i = i;
    vector->j = j;
    vector->k = k;
    vector->l = l;
    return vector;
}
static __forceinline void render_lightmap_shadows_pack_capsule_center(
    real_vector4d *center, float *z, float *radius, float center_x, float center_y, float center_z, float capsule_radius)
{
    center->i = center_x;
    center->j = center_y;
    *z = center_z;
    *radius = capsule_radius;
}
static __forceinline void render_lightmap_shadows_pack_capsule_axis(real_vector4d *output, float i, float j, float k, float half_length)
{
    float *axis_j = &output->j;
    output->i = i;
    *axis_j = j;
    output->k = k;
    output->l = half_length;
}
static __forceinline void render_lightmap_shadows_alpha_blend(long mode)
{
    if (c_rasterizer::get_current_alpha_blend_mode() != mode)
    {
        c_rasterizer::set_current_alpha_blend_mode(static_cast<c_rasterizer::e_alpha_blend_mode>(mode));
        rasterizer_set_device_alpha_blend_mode(c_rasterizer::get_device(), mode);
    }
}
// @rdata 0x82024d20..0x82024ef0
static const float render_lightmap_shadows_bounds_prefix[2] = { 2.0f, 1.0f };
static const s_shadow_bounds_vertices render_lightmap_shadows_bounds_data =
{
    {
        { 32767, 32767, 13573, 0 },
        { 32767, 13573, 32767, 0 },
        { 32767, 13573, -32767, 0 },
        { 32767, 32767, -13573, 0 },
        { 32767, 13573, 32767, 0 },
        { 32767, -13573, 32767, 0 },
        { 32767, -13573, -32767, 0 },
        { 32767, 13573, -32767, 0 },
        { 32767, -13573, 32767, 0 },
        { 32767, -32767, 13573, 0 },
        { 32767, -32767, -13573, 0 },
        { 32767, -13573, -32767, 0 },
        { -21845, 32767, 13573, 0 },
        { -21845, 32767, -13573, 0 },
        { -32767, 13573, -32767, 0 },
        { -32767, 13573, 32767, 0 },
        { -32767, 13573, 32767, 0 },
        { -32767, 13573, -32767, 0 },
        { -32767, -13573, -32767, 0 },
        { -32767, -13573, 32767, 0 },
        { -32767, -13573, 32767, 0 },
        { -32767, -13573, -32767, 0 },
        { -21845, -32767, -13573, 0 },
        { -21845, -32767, 13573, 0 },
        { 32767, 32767, 13573, 0 },
        { 32767, 32767, -13573, 0 },
        { -21845, 32767, -13573, 0 },
        { -21845, 32767, 13573, 0 },
        { 32767, -32767, 13573, 0 },
        { -21845, -32767, 13573, 0 },
        { -21845, -32767, -13573, 0 },
        { 32767, -32767, -13573, 0 },
        { 32767, 13573, 32767, 0 },
        { -32767, 13573, 32767, 0 },
        { -32767, -13573, 32767, 0 },
        { 32767, -13573, 32767, 0 },
        { 32767, 13573, -32767, 0 },
        { 32767, -13573, -32767, 0 },
        { -32767, -13573, -32767, 0 },
        { -32767, 13573, -32767, 0 },
        { 32767, 13573, 32767, 0 },
        { 32767, 32767, 13573, 0 },
        { -21845, 32767, 13573, 0 },
        { -32767, 13573, 32767, 0 },
        { 32767, -13573, -32767, 0 },
        { 32767, -32767, -13573, 0 },
        { -21845, -32767, -13573, 0 },
        { -32767, -13573, -32767, 0 },
        { 32767, 32767, -13573, 0 },
        { 32767, 13573, -32767, 0 },
        { -32767, 13573, -32767, 0 },
        { -21845, 32767, -13573, 0 },
        { 32767, -13573, 32767, 0 },
        { -32767, -13573, 32767, 0 },
        { -21845, -32767, 13573, 0 },
        { 32767, -32767, 13573, 0 }
    },
    { 0.01745329238474369f, 0.0001f }
};
// @rdata-end
static __forceinline void render_lightmap_shadows_draw_bounds()
{
    (void)&render_lightmap_shadows_bounds_prefix;
    c_rasterizer::draw_primitive_up((c_rasterizer_index_buffer::e_primitive_type)6, 14, render_lightmap_shadows_bounds_data.vertices, 8);
}
static __forceinline void render_lightmap_shadows_color_write(unsigned long mask)
{
    D3DDevice_SetRenderState_ColorWriteEnable(c_rasterizer::get_device(), mask);
    D3DDevice_SetRenderState_ColorWriteEnable1(c_rasterizer::get_device(), mask);
}
static __forceinline void render_lightmap_shadows_push_markers()
{
    ++g_render_structure_globals.depth;
    g_render_structure_globals.cluster_markers[g_render_structure_globals.depth] = g_render_structure_globals.current_a;
    g_render_structure_globals.instance_markers[g_render_structure_globals.depth] = g_render_structure_globals.current_b;
    s_render_object_globals *objects = g_render_object_globals;
    long *marker_depth = &objects->marker_depth;
    ++*marker_depth;
    objects->marker_object_counts[*marker_depth] = objects->object_count;
    objects->marker_mesh_part_counts[*marker_depth] = objects->mesh_part_count;
}

static __forceinline void render_lightmap_shadows_set_pixel_constants(long index, long count, const real_vector4d *constants)
{
    c_rasterizer::set_pixel_shader_constant(index, count, constants);
}
static __forceinline void render_lightmap_shadows_set_integer_constants(long index, long count, const long *constants)
{
    D3DDevice_SetPixelShaderConstantI(c_rasterizer::get_device(), index, (const int *)constants, count);
}

static __forceinline float render_lightmap_shadows_distance3d(const real_point3d *a, const real_point3d *b)
{
    real_vector3d relative;
    relative.j = 0.0f;
    relative.k = 0.0f;
    vector_from_points3d(a, b, &relative);
    return magnitude3d(&relative);
}

__forceinline void c_rasterizer::inl_set_vertex_shader_constant(long start_register, long vector4f_count, const real_vector4d *constant_data)
{
    // match: 0x45db78: non-null device assumption pins constant-upload scheduling
    // match: 0x17d6f8: non-null device assumption pins constant-upload scheduling
    __assume(get_device() != 0);
    get_device()->SetVertexShaderConstantF(start_register, (const float *)constant_data, vector4f_count);
}

// @address 0x45db78 ?submit_visibility_and_render@c_lightmap_shadows_view@@QAAXMMJJ@Z
void c_lightmap_shadows_view::submit_visibility_and_render(float shadow_fade, float resolution_scale, long object_index, long model_index)
{
    c_lightmap_shadows_view *view = this;
    long shadow_size = (long)((float)c_rasterizer::render_globals.shadow_size * resolution_scale);
#if !defined(_XBOX) || defined(__clang__)
    shadow_size = PIN(shadow_size, 16, c_rasterizer::render_globals.shadow_size);
#else
    shadow_size = PIN(shadow_size, 16, 512);
#endif
    ceil((float)(shadow_size + 1) * 0.125f);
    view->setup_camera(shadow_size, shadow_size);
    view->render_setup_internal();
    render_lightmap_shadows_push_markers();
    view->render_submit_visibility();
    c_view::begin(view);
    c_rasterizer::setup_targets_dynamic_light();
    c_view::end();
    rasterizer_main_setup_hdr_render_targets(true, true);
    render_lightmap_shadows_color_write(7);
    if (c_rasterizer::set_explicit_shaders(52, _vertex_type_tiny_position, _transfer_vector_vertex_none, _entry_point_default))
    {
        render_mesh_submit_rigid_matrix(&view->m_object_bounding_box.matrix);
        real_vector4d bounds[2];
        render_lightmap_shadows_set_vector4d(&bounds[0], view->m_object_bounding_box.half_extents.i * 2.0f,
            view->m_object_bounding_box.half_extents.j * 2.0f, view->m_object_bounding_box.half_extents.k * 2.0f, 1.0f);
        render_lightmap_shadows_set_vector4d(&bounds[1], 0.0f, 0.0f, 0.0f, 0.0f);
        c_rasterizer::inl_set_vertex_shader_constant(12, 2, bounds);
        if (!g_debug_shadow_bounds_solid)
        {
            g_render_method_submit_cache = k_default_render_method_submit_cache;
            D3DDevice_SetPixelShader(c_rasterizer::get_device(), 0);
            if (c_rasterizer::get_current_alpha_blend_mode() != c_rasterizer::_alpha_blend_opaque)
            {
                c_rasterizer::set_current_alpha_blend_mode(c_rasterizer::_alpha_blend_opaque);
                D3DDevice_SetRenderState_AlphaBlendEnable(c_rasterizer::get_device(), 0);
            }
        }
        else
        {
            render_lightmap_shadows_alpha_blend(1);
            real_vector4d color = { 0.3f, 0.3f, 0.1f, 1.0f };
            render_lightmap_shadows_set_pixel_constants(1, 1, &color);
        }
        c_rasterizer::set_z_buffer_mode(c_rasterizer::_z_buffer_mode_read_only);
        D3DDevice_SetRenderState_CullMode(c_rasterizer::get_device(), 6);
        c_rasterizer::set_stencil_mode(c_rasterizer::_stencil_mode_shadow_apply);
        render_lightmap_shadows_draw_bounds();
        D3DDevice_FlushHiZStencil(c_rasterizer::get_device(), (D3DFHZS_FLUSHTYPE)0);
        if (g_debug_shadow_bounds_solid)
        {
            render_lightmap_shadows_alpha_blend(1);
            real_vector4d color = { 0.1f, 0.2f, 0.1f, 1.0f };
            render_lightmap_shadows_set_pixel_constants(1, 1, &color);
        }
        D3DDevice_SetRenderState_CullMode(c_rasterizer::get_device(), 2);
        c_rasterizer::set_stencil_mode(c_rasterizer::_stencil_mode_shadow_generate);
        render_lightmap_shadows_draw_bounds();
    }
    c_rasterizer::set_z_buffer_mode(c_rasterizer::_z_buffer_mode_read_only);
    render_lightmap_shadows_color_write(0);
    c_rasterizer::set_stencil_mode(c_rasterizer::_stencil_mode_shadow_read);
    bool saved_shadow_mode = g_render_static_lighting_active;
    g_render_static_lighting_active = true;
    c_object_renderer::render_static_lighting(c_rasterizer::_z_buffer_mode_read_only);
    g_render_static_lighting_active = saved_shadow_mode;
    c_rasterizer::set_z_buffer_mode(c_rasterizer::_z_buffer_mode_read_only);
    render_lightmap_shadows_color_write(7);
    const real_matrix4x3 *bounds_matrix = &view->m_object_bounding_box.matrix;
    render_mesh_submit_rigid_matrix(bounds_matrix);
    real_vector4d bounds[2];
    const c_lightmap_shadows_view *bounds_view = view;
    const c_lightmap_shadows_view *const *bounds_reference = &bounds_view;
    render_lightmap_shadows_set_vector4d(&bounds[0], (*bounds_reference)->m_object_bounding_box.half_extents.i * 1.25f,
        (*bounds_reference)->m_object_bounding_box.half_extents.j * 1.25f, (*bounds_reference)->m_object_bounding_box.half_extents.k * 1.25f, 1.0f);
    render_lightmap_shadows_set_vector4d(&bounds[1], 0.0f, 0.0f, 0.0f, 0.0f);
    c_rasterizer::set_vertex_shader_constant(12, 2, bounds);
    c_rasterizer::set_stencil_mode((c_rasterizer::e_stencil_mode)(g_debug_shadow_histencil ? 5 : 6));
    real_vector4d fade;
    render_lightmap_shadows_set_vector4d(&fade, shadow_fade, shadow_fade, shadow_fade, shadow_fade);
    c_rasterizer::set_pixel_shader_constant(11, 1, &fade);
    if (c_rasterizer::set_explicit_shaders(53, _vertex_type_tiny_position, _transfer_vector_vertex_none, _entry_point_albedo))
    {
        c_rasterizer::set_z_buffer_mode(c_rasterizer::_z_buffer_mode_off);
        D3DDevice_SetRenderState_CullMode(c_rasterizer::get_device(), 6);
        render_lightmap_shadows_alpha_blend(10);
        if (atmosphere_constants_index != -2)
        {
            atmosphere_constants_index = -2;
            c_atmosphere_fog_interface::set_constant(&c_atmosphere_fog_interface::m_default_parameters, true, true);
        }
        const render_projection *player_projection = c_player_view::get_current()->get_rasterizer_projection();
        XMMATRIX inverse_projection = XMMatrixIdentity();
        inverse_projection._11 = -1.0f / player_projection->projection_matrix[0][0];
        inverse_projection._12 = 0.0f;
        inverse_projection._13 = player_projection->projection_matrix[2][0] * inverse_projection._11;
        inverse_projection._14 = player_projection->projection_matrix[3][0] * inverse_projection._11;
        inverse_projection._21 = 0.0f;
        inverse_projection._22 = -1.0f / player_projection->projection_matrix[1][1];
        inverse_projection._23 = player_projection->projection_matrix[2][1] * inverse_projection._22;
        inverse_projection._24 = player_projection->projection_matrix[3][1] * inverse_projection._22;
        XMMATRIX view_to_world = XMMatrixIdentity();
        view_to_world._11 = player_projection->view_to_world.forward.i;
        view_to_world._12 = player_projection->view_to_world.left.i;
        view_to_world._13 = player_projection->view_to_world.up.i;
        view_to_world._14 = player_projection->view_to_world.position.x;
        view_to_world._21 = player_projection->view_to_world.forward.j;
        view_to_world._22 = player_projection->view_to_world.left.j;
        view_to_world._23 = player_projection->view_to_world.up.j;
        view_to_world._24 = player_projection->view_to_world.position.y;
        view_to_world._31 = player_projection->view_to_world.forward.k;
        view_to_world._32 = player_projection->view_to_world.left.k;
        view_to_world._33 = player_projection->view_to_world.up.k;
        view_to_world._34 = player_projection->view_to_world.position.z;
        XMMATRIX shadow_transform = XMMatrixMultiply(view_to_world, inverse_projection);
        const render_projection *const &shadow_projection = view->get_rasterizer_projection();
        XMMATRIX world_to_shadow = XMMatrixIdentity();
        world_to_shadow._11 = shadow_projection->world_to_view.forward.i;
        world_to_shadow._12 = shadow_projection->world_to_view.left.i;
        world_to_shadow._13 = shadow_projection->world_to_view.up.i;
        world_to_shadow._14 = shadow_projection->world_to_view.position.x;
        world_to_shadow._21 = shadow_projection->world_to_view.forward.j;
        world_to_shadow._22 = shadow_projection->world_to_view.left.j;
        world_to_shadow._23 = shadow_projection->world_to_view.up.j;
        world_to_shadow._24 = shadow_projection->world_to_view.position.y;
        world_to_shadow._31 = shadow_projection->world_to_view.forward.k;
        world_to_shadow._32 = shadow_projection->world_to_view.left.k;
        world_to_shadow._33 = shadow_projection->world_to_view.up.k;
        world_to_shadow._34 = shadow_projection->world_to_view.position.z;
        shadow_transform = XMMatrixMultiply(world_to_shadow, shadow_transform);
        XMMATRIX shadow_project = XMMatrixIdentity();
        memcpy(&shadow_project, shadow_projection->projection_matrix, sizeof(shadow_project));
        shadow_project = XMMatrixTranspose(shadow_project);
        shadow_project._14 = 0.0f;
        shadow_project._24 = 0.0f;
        shadow_transform = XMMatrixMultiply(shadow_project, shadow_transform);
        XMMATRIX texture_transform = XMMatrixIdentity();
        float scale = (float)shadow_size / (float)c_rasterizer::render_globals.shadow_size;
        texture_transform._11 = scale * 0.5f;
        texture_transform._14 = scale * 0.5f;
        texture_transform._22 = scale * -0.5f;
        texture_transform._24 = scale * 0.5f;
        shadow_transform = XMMatrixMultiply(texture_transform, shadow_transform);
        render_lightmap_shadows_set_pixel_constants(1, 3, (const real_vector4d *)&shadow_transform);
        real_vector4d direction;
        direction.i = -shadow_projection->view_to_world.up.i;
        direction.j = -shadow_projection->view_to_world.up.j;
        direction.k = -shadow_projection->view_to_world.up.k;
        direction.l = 0.0f;
        c_rasterizer::set_pixel_shader_constant(10, 1, &direction);
        // match: 0x45db78: address-taken shadow_project keeps its frame home; without it r18 is not saved and shadow_size moves 96->112
        if (false) c_rasterizer::set_pixel_shader_constant(11, 4, (const real_vector4d *)&shadow_project);
        const render_camera *camera = c_player_view::get_current()->get_rasterizer_camera();
        real_rectangle2d frustum;
        render_camera_build_viewport_frustum_bounds(camera, &frustum);
        render_projection projection;
        render_camera_build_projection(camera, &frustum, &projection, 0.0f);
        XMMATRIX view_projection;
        float *combined_matrix = &view_projection._11;
        combine_projection_and_view_matrix(&projection.world_to_view, projection.projection_matrix, combined_matrix);
        XMVECTOR determinant = xmmatrix_determinant(view_projection);
        XMMATRIX inverse_view_projection;
        XMMATRIX inverse_storage;
        XMMATRIX *inverse_result = new (&inverse_storage) XMMATRIX(XMMatrixInverse(&determinant, view_projection));
        inverse_view_projection = *inverse_result;
        c_rasterizer::set_pixel_shader_constant(100, 4, (const real_vector4d *)&inverse_view_projection);
        const rectangle2d *viewport = &g_rasterizer_viewport_bounds;
        real_vector4d pixel_size;
        pixel_size.i = 1.0f / (float)(viewport->x1 - viewport->x0);
        pixel_size.j = 1.0f / (float)(viewport->y1 - viewport->y0);
        pixel_size.k = pixel_size.i * 0.5f;
        pixel_size.w = pixel_size.j * 0.5f;
        c_rasterizer::set_pixel_shader_constant(7, 1, &pixel_size);
        real_vector4d screen_scale;
        screen_scale.i = 2.0f / (float)(viewport->x1 - viewport->x0);
        screen_scale.j = -2.0f / (float)(viewport->y1 - viewport->y0);
        screen_scale.k = ((float)viewport->x0 * -2.0f) / (float)(viewport->x1 - viewport->x0) - 1.0f;
        screen_scale.w = ((float)viewport->y0 * 2.0f) / (float)(viewport->y1 - viewport->y0) + 1.0f;
        c_rasterizer::set_pixel_shader_constant(8, 1, &screen_scale);
        float pixel_scale = (2.0f / player_projection->projection_matrix[1][1]) / (float)c_rasterizer::render_globals.height;
        const render_projection *fog_projection = view->get_rasterizer_projection();
        real_point3d center;
        point_from_line3d(&fog_projection->view_to_world.position, &fog_projection->view_to_world.up, view->m_object_bounding_box.half_extents.k, &center);
        float radius = sqrtf(magnitude_squared3d(&view->m_object_bounding_box.half_extents));
        float distance = render_lightmap_shadows_distance3d(&center, &player_projection->view_to_world.position);
        real_linear_rgb_color transmittance, near_inscattering, far_inscattering;
        render_lightmap_shadows_compute_scattering(&c_atmosphere_fog_interface::m_default_parameters, &player_projection->view_to_world.position, &center,
            -radius, &transmittance, &near_inscattering);
        render_lightmap_shadows_compute_scattering(&c_atmosphere_fog_interface::m_default_parameters, &player_projection->view_to_world.position, &center,
            radius, &transmittance, &far_inscattering);
        real_vector4d fog_constants[2];
        float near_distance = distance - radius;
        float near_surface_distance = MAX(near_distance, 0.0f);
        float inverse_distance = distance + radius;
        real_linear_rgb_color inscattering_delta;
        inscattering_delta.red = far_inscattering.red - near_inscattering.red;
        inscattering_delta.green = far_inscattering.green - near_inscattering.green;
        inscattering_delta.blue = far_inscattering.blue - near_inscattering.blue;
        float distance_span = inverse_distance - near_surface_distance;
        inverse_distance = 1.0f / MAX(distance_span, 0.0001f);
        fog_constants[0].i = inscattering_delta.red * inverse_distance;
        fog_constants[0].j = inscattering_delta.green * inverse_distance;
        fog_constants[0].k = inscattering_delta.blue * inverse_distance;
        near_distance = -MAX(near_distance, 0.0f);
        fog_constants[1].i = fog_constants[0].i * near_distance + near_inscattering.red;
        fog_constants[1].j = fog_constants[0].j * near_distance + near_inscattering.green;
        fog_constants[1].k = fog_constants[0].k * near_distance + near_inscattering.blue;
        c_rasterizer::set_pixel_shader_constant(4, 2, fog_constants);
        real_vector4d depth_constants;
        depth_constants.i = -1.0f / player_projection->projection_matrix[3][2];
        depth_constants.j = player_projection->projection_matrix[2][2] * depth_constants.i;
        depth_constants.k = (float)sqrt(2.0) / (float)shadow_size;
        depth_constants.w = -pixel_scale;
        c_rasterizer::set_pixel_shader_constant(9, 1, &depth_constants);
        long capsule_count = 0;
        real_vector4d capsules[20][2];
        const s_model_definition *model =
            (const s_model_definition *)global_tag_instances[datum_index_to_absolute_index(model_index)].definition;
        if (model->render_model_index != -1)
        {
            for (long index = 0; index < model->shadow_capsules.count; ++index)
            {
                const s_model_shadow_capsule *capsule =
                    &static_cast<const s_model_shadow_capsule *>(model->shadow_capsules.address)[index];
                real_point3d start, end;
                real_vector3d axis;
                (void)&axis;
                bool end_valid = object_try_and_get_marker_point(object_index, capsule->end_marker_name, capsule->end_marker_index, &end);
                bool start_valid = object_try_and_get_marker_point(object_index, capsule->start_marker_name, capsule->start_marker_index, &start);
                if ((start_valid & end_valid) && (unsigned long)capsule_count < 20)
                {
                    vector_from_points3d(&end, &start, &axis);
                    real_point3d midpoint;
                    midpoint.x = (start.x + end.x) * 0.5f;
                    midpoint.y = (start.y + end.y) * 0.5f;
                    midpoint.z = (start.z + end.z) * 0.5f;
                    float half_length = normalize3d(&axis) * 0.5f;
                    render_lightmap_shadows_pack_capsule_center(&capsules[capsule_count][0],
                        &capsules[capsule_count][0].k, &capsules[capsule_count][0].l,
                        midpoint.x, midpoint.y, midpoint.z, capsule->radius);
                    render_lightmap_shadows_pack_capsule_axis(&capsules[capsule_count][1], axis.i, axis.j, axis.k, half_length);
                    ++capsule_count;
                }
            }
            if (capsule_count > 0)
            {
                long capsule_control[4];
                capsule_control[0] = capsule_count;
                capsule_control[1] = 0;
                capsule_control[2] = 2;
                render_lightmap_shadows_set_integer_constants(1, 1, capsule_control);
                c_rasterizer::set_pixel_shader_constant(114, 40, &capsules[0][0]);
            }
        }
        c_rasterizer::set_aliased_surface_as_texture(0, (c_rasterizer::e_surface)3);
        c_rasterizer::set_sampler_address_mode(
            0, c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp);
        c_rasterizer::set_sampler_filter_mode(0, c_rasterizer::_sampler_filter_mode_point);
        c_rasterizer::set_aliased_surface_as_texture(2, (c_rasterizer::e_surface)19);
        c_rasterizer::set_sampler_address_mode(
            2, c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp);
        c_rasterizer::set_sampler_filter_mode(2, c_rasterizer::_sampler_filter_mode_point);
        render_lightmap_shadows_draw_bounds();
    }
    D3DDevice_SetRenderState_ClipPlaneEnable(c_rasterizer::get_device(), 0);
    c_rasterizer::set_stencil_mode(c_rasterizer::_stencil_mode_off);
    c_rasterizer::set_z_buffer_mode(c_rasterizer::_z_buffer_mode_read_only);
    render_lightmap_shadows_alpha_blend(0);
    render_lightmap_shadows_color_write(15);
    c_structure_renderer::pop_marker();
    c_object_renderer::pop_marker();
}

extern "C" {
// @address 0x4b7f8 k_lightmap_shadows_quality_shader_indices
#if !defined(_XBOX) || defined(__clang__)
const long k_lightmap_shadows_quality_shader_indices[4] = { 0x4c, 0x4f, 0x4d, 0x4e };
#endif
}
static __forceinline void render_lightmap_shadows_push_structure_marker()
{
    g_render_structure_globals.depth++;
    g_render_structure_globals.cluster_markers[g_render_structure_globals.depth] = g_render_structure_globals.current_a;
    g_render_structure_globals.instance_markers[g_render_structure_globals.depth] = g_render_structure_globals.current_b;
}
static __forceinline void render_lightmap_shadows_push_object_marker()
{
    s_render_object_globals *globals = g_render_object_globals;
    long *depth = &globals->marker_depth;
    (*depth)++;
    globals->marker_object_counts[*depth] = globals->object_count;
    globals->marker_mesh_part_counts[*depth] = globals->mesh_part_count;
}
static __forceinline void render_lightmap_shadows_set_alpha_blend(long mode)
{
    if (c_rasterizer::get_current_alpha_blend_mode() != mode)
    {
        c_rasterizer::set_current_alpha_blend_mode(static_cast<c_rasterizer::e_alpha_blend_mode>(mode));
        rasterizer_set_device_alpha_blend_mode(c_rasterizer::get_device(), mode);
    }
}

// @address 0x17d6f8 ?submit_visibility_and_render@c_lightmap_shadows_view@@QAAXMM@Z
void c_lightmap_shadows_view::submit_visibility_and_render(float shadow_fade, float scale)
{
    real_vector4d front_color, back_color;
    long quality;
    if (inl_cinematic_in_progress() || rasterizer_tiling_get_state()->enabled)
        quality = 3;
    else if (scale >= 1.0f)
        quality = 2;
    else if (scale > 0.18f)
        quality = 1;
    else
        quality = 0;
    long size = (long)((float)c_rasterizer::render_globals.shadow_size * scale);
#if !defined(_XBOX) || defined(__clang__)
    size = PIN(size, 16, c_rasterizer::render_globals.shadow_size);
    long resolve_size = (long)((float)ceil((float)(size + 1) * 0.125f) * 8.0f);
    resolve_size = PIN(resolve_size, 16, c_rasterizer::render_globals.shadow_size);
#else
    size = PIN(size, 16, 512);
    long resolve_size = (long)((float)ceil((float)(size + 1) * 0.125f) * 8.0f);
    resolve_size = PIN(resolve_size, 16, 512);
#endif
    setup_camera(size, size);
    render_setup_internal();
    render_lightmap_shadows_push_structure_marker();
    render_lightmap_shadows_push_object_marker();
    render_submit_visibility();
    D3DDevice_FlushHiZStencil(c_rasterizer::get_device(), (D3DFHZS_FLUSHTYPE)1);
    c_view::begin(this);
    render_occlusions_setup_depth_target(1, resolve_size, resolve_size);
    c_object_renderer::render_static_lighting(c_rasterizer::_z_buffer_mode_decal);
    rectangle2d bounds;
    bounds.x0 = bounds.y0 = 0;
    bounds.x1 = bounds.y1 = (short)resolve_size;
    c_rasterizer::resolve_surface((c_rasterizer::e_surface)2, 4, &bounds, 0, 0);
    c_rasterizer::setup_targets_dynamic_light();
    c_view::end();
    rasterizer_main_setup_hdr_render_targets(true, true);
    D3DDevice_SetRenderState_ColorWriteEnable(c_rasterizer::get_device(), 7);
    D3DDevice_SetRenderState_ColorWriteEnable1(c_rasterizer::get_device(), 7);
    if (c_rasterizer::set_explicit_shaders(52, _vertex_type_tiny_position, _transfer_vector_vertex_none, _entry_point_default))
    {
        render_mesh_submit_rigid_matrix(&m_object_bounding_box.matrix);
        real_vector4d constants[2];
        render_lightmap_shadows_set_vector4d(&constants[0], m_object_bounding_box.half_extents.i, m_object_bounding_box.half_extents.j,
            m_object_bounding_box.half_extents.k, 1.0f);
        render_lightmap_shadows_set_vector4d(&constants[1], 0.0f, 0.0f, 0.0f, 0.0f);
        c_rasterizer::inl_set_vertex_shader_constant(12, 2, constants);
        if (!g_debug_shadow_bounds_solid)
        {
            g_render_method_submit_cache = k_default_render_method_submit_cache;
            D3DDevice_SetPixelShader(c_rasterizer::get_device(), 0);
            if (c_rasterizer::get_current_alpha_blend_mode() != c_rasterizer::_alpha_blend_opaque)
            {
                c_rasterizer::set_current_alpha_blend_mode(c_rasterizer::_alpha_blend_opaque);
                D3DDevice_SetRenderState_AlphaBlendEnable(c_rasterizer::get_device(), 0);
            }
        }
        else
        {
            render_lightmap_shadows_set_alpha_blend(1);
            render_lightmap_shadows_set_vector4d(&front_color, 0.3f, 0.3f, 0.1f, 1.0f);
            c_rasterizer::set_pixel_shader_constant(1, 1, &front_color);
        }
        c_rasterizer::set_z_buffer_mode(c_rasterizer::_z_buffer_mode_read_only);
        D3DDevice_SetRenderState_CullMode(c_rasterizer::get_device(), 6);
        c_rasterizer::set_stencil_mode(c_rasterizer::_stencil_mode_shadow_apply);
        render_lightmap_shadows_draw_bounds();
        D3DDevice_FlushHiZStencil(c_rasterizer::get_device(), (D3DFHZS_FLUSHTYPE)0);
        if (g_debug_shadow_bounds_solid)
        {
            render_lightmap_shadows_set_alpha_blend(1);
            render_lightmap_shadows_set_vector4d(&back_color, 0.1f, 0.2f, 0.1f, 1.0f);
            c_rasterizer::set_pixel_shader_constant(1, 1, &back_color);
        }
        D3DDevice_SetRenderState_CullMode(c_rasterizer::get_device(), 2);
        c_rasterizer::set_stencil_mode(c_rasterizer::_stencil_mode_shadow_generate);
        render_lightmap_shadows_draw_bounds();
    }
    c_rasterizer::set_stencil_mode((c_rasterizer::e_stencil_mode)(g_debug_shadow_histencil ? 5 : 6));
    D3DDevice_FlushHiZStencil(c_rasterizer::get_device(), (D3DFHZS_FLUSHTYPE)1);
    real_vector4d fade;
    render_lightmap_shadows_set_vector4d(&fade, shadow_fade, shadow_fade, shadow_fade, shadow_fade);
    c_rasterizer::set_pixel_shader_constant(11, 1, &fade);
    if (c_rasterizer::set_explicit_shaders(k_lightmap_shadows_quality_shader_indices[quality], _vertex_type_tiny_position,
            _transfer_vector_vertex_none, _entry_point_default))
    {
        c_rasterizer::set_z_buffer_mode(c_rasterizer::_z_buffer_mode_off);
        D3DDevice_SetRenderState_CullMode(c_rasterizer::get_device(), 6);
        render_lightmap_shadows_set_alpha_blend(10);
        if (atmosphere_constants_index != -2)
        {
            atmosphere_constants_index = -2;
            c_atmosphere_fog_interface::set_constant(&c_atmosphere_fog_interface::m_default_parameters, true, true);
        }
        const render_projection *player_projection = c_player_view::get_current()->get_rasterizer_projection();
        XMMATRIX inverse_projection = XMMatrixIdentity();
        inverse_projection._11 = -1.0f / player_projection->projection_matrix[0][0];
        inverse_projection._12 = 0.0f;
        inverse_projection._13 = player_projection->projection_matrix[2][0] * inverse_projection._11;
        inverse_projection._14 = player_projection->projection_matrix[3][0] * inverse_projection._11;
        inverse_projection._21 = 0.0f;
        inverse_projection._22 = -1.0f / player_projection->projection_matrix[1][1];
        inverse_projection._23 = player_projection->projection_matrix[2][1] * inverse_projection._22;
        inverse_projection._24 = player_projection->projection_matrix[3][1] * inverse_projection._22;
        XMMATRIX inverse_view = XMMatrixIdentity();
        inverse_view._11 = player_projection->view_to_world.rotation.forward.i;
        inverse_view._12 = player_projection->view_to_world.rotation.left.i;
        inverse_view._13 = player_projection->view_to_world.rotation.up.i;
        inverse_view._14 = player_projection->view_to_world.position.x;
        inverse_view._21 = player_projection->view_to_world.rotation.forward.j;
        inverse_view._22 = player_projection->view_to_world.rotation.left.j;
        inverse_view._23 = player_projection->view_to_world.rotation.up.j;
        inverse_view._24 = player_projection->view_to_world.position.y;
        inverse_view._31 = player_projection->view_to_world.rotation.forward.k;
        inverse_view._32 = player_projection->view_to_world.rotation.left.k;
        inverse_view._33 = player_projection->view_to_world.rotation.up.k;
        inverse_view._34 = player_projection->view_to_world.position.z;
        XMMATRIX screen_to_shadow = XMMatrixMultiply(inverse_view, inverse_projection);
        const render_projection *const &shadow_projection = &rasterizer_projection;
        XMMATRIX shadow_view = XMMatrixIdentity();
        shadow_view._11 = shadow_projection->world_to_view.rotation.forward.i;
        shadow_view._12 = shadow_projection->world_to_view.rotation.left.i;
        shadow_view._13 = shadow_projection->world_to_view.rotation.up.i;
        shadow_view._14 = shadow_projection->world_to_view.position.x;
        shadow_view._21 = shadow_projection->world_to_view.rotation.forward.j;
        shadow_view._22 = shadow_projection->world_to_view.rotation.left.j;
        shadow_view._23 = shadow_projection->world_to_view.rotation.up.j;
        shadow_view._24 = shadow_projection->world_to_view.position.y;
        shadow_view._31 = shadow_projection->world_to_view.rotation.forward.k;
        shadow_view._32 = shadow_projection->world_to_view.rotation.left.k;
        shadow_view._33 = shadow_projection->world_to_view.rotation.up.k;
        shadow_view._34 = shadow_projection->world_to_view.position.z;
        screen_to_shadow = XMMatrixMultiply(shadow_view, screen_to_shadow);
        XMMATRIX shadow_matrix = XMMatrixIdentity();
        memcpy(&shadow_matrix, shadow_projection->projection_matrix, sizeof(shadow_matrix));
        shadow_matrix = XMMatrixTranspose(shadow_matrix);
        shadow_matrix._14 = 0.0f;
        shadow_matrix._24 = 0.0f;
        screen_to_shadow = XMMatrixMultiply(shadow_matrix, screen_to_shadow);
        XMMATRIX texture_transform = XMMatrixIdentity();
        float texture_scale = (float)size / (float)c_rasterizer::render_globals.shadow_size;
        texture_transform._11 = 0.5f * texture_scale;
        texture_transform._14 = 0.5f * texture_scale;
        texture_transform._22 = -0.5f * texture_scale;
        texture_transform._24 = 0.5f * texture_scale;
        screen_to_shadow = XMMatrixMultiply(texture_transform, screen_to_shadow);
        c_rasterizer::set_pixel_shader_constant(1, 3, (const real_vector4d *)&screen_to_shadow);
        real_vector4d direction;
        direction.i = -shadow_projection->view_to_world.rotation.up.i;
        direction.j = -shadow_projection->view_to_world.rotation.up.j;
        direction.k = -shadow_projection->view_to_world.rotation.up.k;
        direction.l = 0.0f;
        c_rasterizer::set_pixel_shader_constant(10, 1, &direction);
        // match: 0x17d6f8: address-taken shadow_matrix keeps its frame home; without it texture_transform moves 112->96
        if (false) c_rasterizer::set_pixel_shader_constant(11, 4, (const real_vector4d *)&shadow_matrix);
        real_vector4d texel;
        texel.i = 1.0f / (float)(g_rasterizer_viewport_bounds.x1 - g_rasterizer_viewport_bounds.x0);
        texel.j = 1.0f / (float)(g_rasterizer_viewport_bounds.y1 - g_rasterizer_viewport_bounds.y0);
        texel.k = texel.i * 0.5f;
        texel.l = texel.j * 0.5f;
        c_rasterizer::set_pixel_shader_constant(7, 1, &texel);
        const rectangle2d *viewport = &g_rasterizer_viewport_bounds;
        real_vector4d viewport_transform;
        viewport_transform.i = 2.0f / (float)(viewport->x1 - viewport->x0);
        viewport_transform.j = -2.0f / (float)(viewport->y1 - viewport->y0);
        viewport_transform.k = (float)viewport->x0 * -2.0f / (float)(viewport->x1 - viewport->x0) - 1.0f;
        viewport_transform.l = (float)viewport->y0 * 2.0f / (float)(viewport->y1 - viewport->y0) + 1.0f;
        c_rasterizer::set_pixel_shader_constant(8, 1, &viewport_transform);
        const real_point3d *eye = &player_projection->view_to_world.position;
        float pixel_size = 2.0f / player_projection->projection_matrix[1][1] / (float)c_rasterizer::render_globals.height;
        const render_projection *fog_projection = &rasterizer_projection;
        real_point3d center;
        point_from_line3d(&fog_projection->view_to_world.position, &fog_projection->view_to_world.rotation.up, m_object_bounding_box.half_extents.k, &center);
        float radius = sqrtf(magnitude_squared3d(&m_object_bounding_box.half_extents));
        float distance = sqrtf(distance_squared3d(&center, eye));
        real_linear_rgb_color transmittance, near_inscattering, far_inscattering;
        render_lightmap_shadows_compute_scattering(&c_atmosphere_fog_interface::m_default_parameters,
            eye, &center, -radius, &transmittance, &near_inscattering);
        render_lightmap_shadows_compute_scattering(&c_atmosphere_fog_interface::m_default_parameters,
            eye, &center, radius, &transmittance, &far_inscattering);
        float near_distance = MAX(distance - radius, 0.0f);
        real_linear_rgb_color inscattering_difference;
        inscattering_difference.green = 0.0f;
        inscattering_difference.blue = 0.0f;
        float inverse_distance = 0.0f;
        inscattering_difference.red = far_inscattering.red - near_inscattering.red;
        inscattering_difference.green = far_inscattering.green - near_inscattering.green;
        inscattering_difference.blue = far_inscattering.blue - near_inscattering.blue;
        inverse_distance = 1.0f / MAX(distance + radius - near_distance, 0.0001f);
        real_vector4d fog[2];
        fog[0].j = 0.0f;
        fog[0].k = 0.0f;
        float near_offset = 0.0f;
        fog[0].i = inscattering_difference.red * inverse_distance;
        fog[0].j = inscattering_difference.green * inverse_distance;
        fog[0].k = inscattering_difference.blue * inverse_distance;
        near_offset = -MAX(distance - radius, 0.0f);
        fog[1].i = fog[0].i * near_offset + near_inscattering.red;
        fog[1].j = fog[0].j * near_offset + near_inscattering.green;
        fog[1].k = fog[0].k * near_offset + near_inscattering.blue;
        c_rasterizer::set_pixel_shader_constant(4, 2, fog);
        real_vector4d depth;
        depth.i = -1.0f / player_projection->projection_matrix[3][2];
        depth.j = player_projection->projection_matrix[2][2] * depth.i;
        depth.k = (float)sqrt(2.0) / (float)size;
        depth.l = -pixel_size;
        c_rasterizer::set_pixel_shader_constant(9, 1, &depth);
        c_rasterizer::set_aliased_surface_as_texture(1, (c_rasterizer::e_surface)2);
        c_rasterizer::set_sampler_address_mode(
            1, c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp);
        c_rasterizer::set_sampler_filter_mode(1, c_rasterizer::_sampler_filter_mode_point);
        c_rasterizer::set_aliased_surface_as_texture(0, (c_rasterizer::e_surface)3);
        c_rasterizer::set_sampler_address_mode(
            0, c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp);
        c_rasterizer::set_sampler_filter_mode(0, c_rasterizer::_sampler_filter_mode_point);
        c_rasterizer::set_aliased_surface_as_texture(2, (c_rasterizer::e_surface)19);
        c_rasterizer::set_sampler_address_mode(
            2, c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp);
        c_rasterizer::set_sampler_filter_mode(2, c_rasterizer::_sampler_filter_mode_point);
        render_lightmap_shadows_draw_bounds();
    }
    D3DDevice_SetRenderState_ClipPlaneEnable(c_rasterizer::get_device(), 0);
    c_rasterizer::set_stencil_mode(c_rasterizer::_stencil_mode_off);
    c_rasterizer::set_z_buffer_mode(c_rasterizer::_z_buffer_mode_read_only);
    render_lightmap_shadows_set_alpha_blend(0);
    D3DDevice_SetRenderState_ColorWriteEnable(c_rasterizer::get_device(), 15);
    D3DDevice_SetRenderState_ColorWriteEnable1(c_rasterizer::get_device(), 15);
    c_structure_renderer::pop_marker();
    c_object_renderer::pop_marker();
}

extern "C" {
// @address 0x730e50 g_shadow_fade_distance_scale
float g_shadow_fade_distance_scale = 1.0f;
// @address 0x730e58 g_shadow_reduce_distance_scale
float g_shadow_reduce_distance_scale = 1.01f;
}


// @address 0x17f180 ?render@c_lightmap_shadows_view@@QAAXJ@Z
void c_lightmap_shadows_view::render(long ignored_object_index)
{
    long shadow_count = 0;
    if (g_player_render_tuning.maximum_shadow_count > 0)
    {
        long root_object_count = c_visible_items::get_camera_root_objects_count();
        for (long root_index = 0; root_index < root_object_count; root_index++)
        {
            s_object_hierarchy *hierarchy = &c_visible_items::m_items.hierarchies.entries[root_index];
            if (((short)hierarchy->flags & FLAG(1)) != 0 && hierarchy->object_count != 0)
            {
                long object_index = c_visible_items::m_items.objects.entries[hierarchy->first_object].render_setup.object_index;
                object_datum *object = object_get(object_index);
                bool visible = false;
                if (!object_is_hidden(object_index))
                    visible = true;
                if (object_index != ignored_object_index && object->parent_object_index == -1 && visible && hierarchy->levels.values[2] != 0)
                {
                    float scale = 0.0f;
                    if (c_player_view::get_current())
                    {
                        long pixels = (long)render_camera_compute_projected_diameter(
                            (const render_projection *)&c_player_view::get_current()->render_projection_state,
                            &object->bounding_center, object->scale * object->bounding_radius);
#if !defined(_XBOX) || defined(__clang__)
                        scale = (float)pixels * g_player_render_tuning.shadow_resolution_scale / 512.0f;
                        if (scale > 1.0f)
                            scale = 1.0f;
#else
                        long shadow_size = c_rasterizer::render_globals.shadow_size;
                        scale = (float)pixels * g_player_render_tuning.shadow_resolution_scale / (float)shadow_size;
#endif
                    }
                    c_visible_items::push_marker();
                    compute_visibility(object_index, -1);
                    bool rendered = false;
                    const long &definition_index = object_get(object_index)->definition_index;
                    if (definition_index != -1)
                    {
                        const s_object_definition *definition = (const s_object_definition *)
                            global_tag_instances[datum_index_to_absolute_index(definition_index)].definition;
                        if (definition->lightmap_shadow_mode == 3 && definition->model.index != -1)
                        {
                            submit_visibility_and_render((float)hierarchy->levels.values[2] * (1.0f / 255.0f), scale, object_index, definition->model.index);
                            rendered = true;
                        }
                    }
                    if (!rendered)
                        submit_visibility_and_render((float)hierarchy->levels.values[2] * (1.0f / 255.0f), scale);
                    c_visible_items::pop_marker();
                    shadow_count++;
                }
            }
        }
        if (!inl_cinematic_in_progress())
        {
            float reduce = g_shadow_reduce_distance_scale;
            if (shadow_count > g_player_render_tuning.maximum_shadow_count * 2)
                g_shadow_fade_distance_scale *= 1.1f;
            else if (shadow_count > g_player_render_tuning.maximum_shadow_count)
                g_shadow_fade_distance_scale *= reduce;
            else if (shadow_count < g_player_render_tuning.maximum_shadow_count - 1)
                g_shadow_fade_distance_scale = g_shadow_fade_distance_scale * 0.99f + (1.0f - 0.99f);
            if (!(g_shadow_fade_distance_scale > 1.0f))
                g_shadow_fade_distance_scale = 1.0f;
        }
    }
}
