// graphicslibxbox/render_sprite.obj: from the records 0x4679b8
// @flags /TP /O2 /Oc
#include "render/render_camera_types.h"
#include <math.h>
#include "bitmaps/bitmap_group.h"
#include "math/real_math.h"
#include "math/matrix_math.h"
#include "tag_files/tag_groups.h"
#include "render/render_sprite.h"
#include "physics/rasterizer_vertex_transparent_types.h"
#include "bitmaps/bitmap_data.h"
#include "bitmaps/bitmap_group_definitions.h"
#include "render/render_view.h"
#include "render/views/player_view_types.h"
#include "rasterizer/rasterizer_textures_cache.h"

struct D3DDevice;

#include "rasterizer/rasterizer.h"
#include "rasterizer/rasterizer_device_states.h"
// @address 0x195e88 rasterizer_device_set_sampler_filter_modes
// @address 0x7c6178 g_rasterizer_sampler_filter_modes
// @address 0x8c46dc ?x_current_player_view@c_player_view@@1PAV1@A

struct bitmap_group_sprite
{
    short bitmap_index;
    short flags;
    long alignment_flags;
    float left;
    float right;
    float top;
    float bottom;
    real_point2d registration_point;
};

enum
{
    _sprite_input_in_view_space_bit = 0,
    _sprite_mirror_horizontal_bit = 1,
    _sprite_mirror_vertical_bit = 2,
};

enum
{
    _sprite_orientation_screen_facing = 0,
    _sprite_orientation_parallel,
    _sprite_orientation_perpendicular,
};


// match: 0x4679b8: inline sampler update preserves the device-state call sequence.
static __forceinline void render_sprite_set_sampler_filter_mode(long sampler, c_rasterizer::e_sampler_filter_mode mode)
{
    if (g_rasterizer_sampler_filter_modes[sampler] != mode)
    {
        if (VALID_INDEX(sampler, NUMBEROF(g_rasterizer_sampler_filter_modes)))
            g_rasterizer_sampler_filter_modes[sampler] = mode;
        if ((unsigned long)mode <= 7)
            rasterizer_device_set_sampler_filter_modes(c_rasterizer::get_device(), sampler, mode, mode);
    }
}



// match: 0x4679b8: inline transform preserves vertex-loop code generation.
static __forceinline real_point3d *render_sprite_matrix4x3_transform_point(const real_matrix4x3 *matrix, const real_point3d *point, real_point3d *result)
{
    float x = point->x;
    float y = point->y;
    float z = point->z;
    x *= matrix->scale;
    y *= matrix->scale;
    z *= matrix->scale;
    result->x = x * matrix->rotation.forward.i + y * matrix->rotation.left.i + z * matrix->rotation.up.i + matrix->position.x;
    result->y = x * matrix->rotation.forward.j + y * matrix->rotation.left.j + z * matrix->rotation.up.j + matrix->position.y;
    result->z = x * matrix->rotation.forward.k + y * matrix->rotation.left.k + z * matrix->rotation.up.k + matrix->position.z;
    return result;
}

static __forceinline unsigned long render_sprite_real_a_rgb_color_to_pixel32(float alpha, const real_rgb_color *color)
{
    unsigned long pixel = 0;
    float scale = 255.0f;

    pixel = ((long)(alpha * scale) << 24) | ((long)(color->red * scale) << 16) | ((long)(color->green * scale) << 8) | (long)(color->blue * scale);
    return pixel;
}

static __forceinline unsigned long render_sprite_real_argb_color_to_pixel32(const real_argb_color *color)
{
    unsigned long pixel = 0;
    pixel = render_sprite_real_a_rgb_color_to_pixel32(color->alpha, &color->rgb);
    return pixel;
}

static __forceinline bool bitmap_group_get_bitmap_hardware_format(long bitmap_group_index, long bitmap_index, c_rasterizer_texture_ref *out_hardware_format)
{
    // match: 0x4679b8: non-null output preserves the build_sprite schedule
    __assume(out_hardware_format != NULL);
    // match: 0x4679b8: valid group index preserves the build_sprite schedule
    __assume(bitmap_group_index != NONE);
    const bitmap_group *group = TAG_GET('bitm', const bitmap_group, bitmap_group_index);
    // match: 0x4679b8: non-null group preserves the build_sprite schedule
    __assume(group != NULL);
    // match: 0x4679b8: known interop flag preserves the build_sprite schedule
    __assume(TEST_BIT(group->flags, bitmap_group::_using_interop_resource_bit));
    const bitmap_data *bitmap;
    bool result = bitmap_group_get_xenon_hardware_format(bitmap_group_index, bitmap_index, out_hardware_format, &bitmap, NULL) != 0;
    return result;
}



static void build_sprite_transform_input(unsigned long flags, const real_point3d *untransformed_origin, const real_vector3d *untransformed_direction,
    real_point3d *transformed_origin, real_vector3d *transformed_direction, real_vector3d *world_direction)
{
    // match: 0x4679b8: non-null origin input settles link-time schedule ties
    __assume(untransformed_origin != NULL);
    // match: 0x4679b8: non-null transformed origin settles link-time schedule ties
    __assume(transformed_origin != NULL);
    // match: 0x4679b8: non-null direction outputs settle link-time schedule ties
    __assume(transformed_direction != NULL);
    const render_projection *projection = c_player_view::get_current()->get_rasterizer_projection();
    if (TEST_BIT(flags, _sprite_input_in_view_space_bit))
    {
        *transformed_origin = *untransformed_origin;
        if (untransformed_direction)
            *transformed_direction = *untransformed_direction;
        if (untransformed_direction)
            matrix4x3_inverse_transform_normal(&projection->world_to_view, untransformed_direction, world_direction);
    }
    else
    {
        matrix4x3_transform_point(&projection->world_to_view, untransformed_origin, transformed_origin);
        if (untransformed_direction)
            matrix4x3_transform_normal(&projection->world_to_view, untransformed_direction, transformed_direction);
        if (untransformed_direction)
            *world_direction = *untransformed_direction;
    }
}

static void build_sprite_basis(short orientation, unsigned long flags, const real_point3d *origin, const real_vector3d *direction, real_matrix4x3 *basis)
{
    // match: 0x4679b8: non-null origin settles link-time schedule ties
    __assume(origin != NULL);
    const render_camera *camera = c_player_view::get_current()->get_rasterizer_camera();
    if (orientation == _sprite_orientation_screen_facing)
    {
        basis->forward.i = 1.0f;
        basis->forward.j = 0.0f;
        basis->forward.k = 0.0f;
        basis->left.i = 0.0f;
        basis->left.j = 1.0f;
        basis->left.k = 0.0f;
    }
    else if (orientation == _sprite_orientation_parallel)
    {
        basis->forward = *direction;
        normalize3d(&basis->forward);
        const real_vector3d *forward = &basis->forward;
        real_vector3d *left = &basis->left;
        float k = origin->x * forward->j - origin->y * forward->i;
        float j = origin->z * forward->i - origin->x * forward->k;
        left->i = origin->y * forward->k - origin->z * forward->j;
        left->j = j;
        left->k = k;
        normalize3d(left);
    }
    else if (orientation == _sprite_orientation_perpendicular)
    {
        const real_vector3d *axis = &camera->up;
        float dot = dot_product3d(direction, axis);
        if (dot * dot > 0.99f * magnitude_squared3d(direction))
            axis = &camera->forward;
        cross_product3d(direction, axis, &basis->forward);
        normalize3d(&basis->forward);
        basis->left = basis->forward;
        basis->up = *direction;
        normalize3d(&basis->up);
        rotate_vector_about_axis(&basis->left, &basis->up, -1.0f, 0.0f);
    }
    else
    {
        // match: 0x4679b8: unreachable hint omits the orientation range check
        __assume(0);
    }
}

static void build_sprite_scale(short orientation, unsigned long flags, const real_point3d *origin, const bitmap_data *bitmap, float *scale)
{
    const render_projection *projection = c_player_view::get_current()->get_rasterizer_projection();
    if (orientation == _sprite_orientation_screen_facing && *scale == 0.0f)
        *scale = -origin->z / projection->world_to_screen_size.i;
    *scale *= (float)bitmap->width;
}

// @address 0x4679b8
void build_sprite(long bitmap_tag_index, short orientation, short sequence_index, short sprite_index, const real_point3d *point, const real_vector3d *vector,
    float rotation, float width, unsigned long flags, real_argb_color *color)
{
    // Antenna sprites use a parallel basis without rotation or mirroring.
    orientation = _sprite_orientation_parallel;
    sprite_index = 0;
    rotation = 0.0f;
    flags = 0;

    const bitmap_group *bitmap = TAG_GET('bitm', const bitmap_group, bitmap_tag_index);
    const bitmap_sequence *sequence = TAG_BLOCK_GET_ELEMENT(&bitmap->sequences, sequence_index, const bitmap_sequence);
    const bitmap_group_sprite *sprite = TAG_BLOCK_GET_ELEMENT(&sequence->sprites, sprite_index, const bitmap_group_sprite);
    // match: 0x4679b8: value range assumption guides branch lowering
    __assume(VALID_INDEX(sprite->bitmap_index, bitmap->bitmaps.count));
    const bitmap_data *bitmap_entry = TAG_BLOCK_GET_ELEMENT(&bitmap->bitmaps, sprite->bitmap_index, const bitmap_data);
    const render_projection *projection = c_player_view::get_current()->get_rasterizer_projection();

    float rotation_sine = 0.0f;
    float rotation_cosine = 1.0f;
    if (rotation != 0.0f)
    {
        rotation_sine = sin(rotation);
        rotation_cosine = cos(rotation);
    }

    real_point3d origin;
    real_vector3d direction;
    real_vector3d world_direction;
    real_matrix4x3 basis;
    build_sprite_transform_input(flags, point, vector, &origin, &direction, &world_direction);
    build_sprite_basis(orientation, flags, &origin, &direction, &basis);
    build_sprite_scale(orientation, flags, &origin, bitmap_entry, &width);

    rasterizer_vertex_transparent vertices[4];
    for (short vertex_index = 0; vertex_index < 4; vertex_index++)
    {
        float u = vertex_index < 2 ? sprite->right : sprite->left;
        float v = !TEST_BIT(vertex_index, 0) ? sprite->top : sprite->bottom;
        float x = u - (sprite->left + sprite->registration_point.x);
        float y = (sprite->top + sprite->registration_point.y) - v;
        float temp = x * rotation_sine;
        x = x * rotation_cosine - y * rotation_sine;
        y = y * rotation_cosine + temp;
        if (TEST_BIT(flags, _sprite_mirror_horizontal_bit))
            x = -x;
        if (TEST_BIT(flags, _sprite_mirror_vertical_bit))
            y = -y;

        real_point3d vertex_point;
        vertex_point.x = origin.x + width * (basis.forward.i * x + basis.left.i * y);
        vertex_point.y = origin.y + width * (basis.forward.j * x + basis.left.j * y);
        vertex_point.z = origin.z + width * (basis.forward.k * x + basis.left.k * y);
        render_sprite_matrix4x3_transform_point(&projection->view_to_world, &vertex_point, &vertices[vertex_index].position);
        vertices[vertex_index].u = u;
        vertices[vertex_index].v = v;
        vertices[vertex_index].color = render_sprite_real_argb_color_to_pixel32(color);
    }

    c_rasterizer_texture_ref texture;
    bitmap_group_get_bitmap_hardware_format(bitmap_tag_index, sprite->bitmap_index, &texture);
    c_rasterizer::set_sampler_texture(0, texture);
    c_rasterizer::set_sampler_address_mode(0, c_rasterizer::_sampler_address_clamp);
    render_sprite_set_sampler_filter_mode(0, c_rasterizer::_sampler_filter_mode_bilinear);
    c_rasterizer::draw_textured_transparent_quad(vertices, c_rasterizer::_alpha_blend_alpha_blend);
}
