#ifndef RENDER_METHODS_RENDER_METHOD_DEFINITIONS_H
#define RENDER_METHODS_RENDER_METHOD_DEFINITIONS_H

#include <stddef.h>

#include "math/real_math_types.h"
#include "tag_files/tag_types.h"
#include "math/function_definitions.h"

struct s_render_method_texture
{
    s_tag_reference bitmap;
    unsigned short index;
    unsigned char address_mode;
    unsigned char filter_mode;
    unsigned char type;
    unsigned char transform;
    unsigned short functions;
};

struct s_render_method_animated_parameter
{
    int type;
    int input_name;
    int range_name;
    float time_period;
    c_function_definition function;
};

typedef char render_method_texture_size[(sizeof(s_render_method_texture) == 0x18) ? 1 : -1];
typedef char render_method_animated_parameter_size[(sizeof(s_render_method_animated_parameter) == 0x24) ? 1 : -1];

// The render method entry points, in the order of the shaders' ENTRY_POINT_* numbers (source\rasterizer\hlsl\entry.fx).
// Names: March render_method_definitions.obj entry-point strings (active_camo .. water_shading); its location is a guess.
enum e_entry_point
{
    _entry_point_default = 0,
    _entry_point_albedo,
    _entry_point_static_default,
    _entry_point_static_per_pixel,
    _entry_point_static_per_vertex,
    _entry_point_static_sh,
    _entry_point_static_prt_ambient,
    _entry_point_static_prt_linear,
    _entry_point_static_prt_quadratic,
    _entry_point_dynamic_light,
    _entry_point_shadow_generate,
    _entry_point_shadow_apply,
    _entry_point_active_camo,
    _entry_point_lightmap_debug_mode,
    _entry_point_static_per_vertex_color,
    _entry_point_water_tessellation,
    _entry_point_water_shading,
    _entry_point_dynamic_light_cinematic,
    k_number_of_entry_points
};

struct s_rm_constant_route { unsigned short register_index; unsigned char parameter_index, constant_index; };
struct s_tag_block_index { unsigned short value; };
struct s_rm_postprocess_pass { s_tag_block_index textures; s_tag_block_index vertex_constants; s_tag_block_index pixel_constants; };

struct s_render_method_postprocess_definition
{
    enum e_runtime_queryable_property
    {
        _runtime_queryable_property_diffuse_weights,
        _runtime_queryable_property_diffuse_map,
        _runtime_queryable_property_diffuse_map_green,
        _runtime_queryable_property_diffuse_map_blue,
        _runtime_queryable_property_diffuse_color,
        _runtime_queryable_property_diffuse_map_alpha
    };
    __forceinline bool query_sprite_bitmap(const s_render_method_texture **reference) const;
    bool sample_texture(e_runtime_queryable_property property, real_point2d point, long sampler, real_argb_color *color) const;
    bool query_texture_transform(e_runtime_queryable_property property, real_vector4d *transform, long *bitmap_index, long *mipmap) const;
    s_tag_reference template_reference;
    s_tag_block textures;
    c_typed_tag_block<real_vector4d> real_constants;
    s_tag_block integer_values;
    unsigned long boolean_values;
    s_tag_block entry_points;
    c_typed_tag_block<s_rm_postprocess_pass> passes;
    c_typed_tag_block<s_rm_constant_route> routing_info;
    c_typed_tag_block<s_render_method_animated_parameter> animations;
    long alpha_blend_mode;
    long flags;
    unsigned long : 32;
    short query_constants[6];
};

typedef char render_method_postprocess_size[(sizeof(s_render_method_postprocess_definition) == 0x80) ? 1 : -1];
typedef char render_method_flags_offset_check[(offsetof(s_render_method_postprocess_definition, flags) == 0x6c) ? 1 : -1];
typedef char render_method_alpha_blend_offset[(offsetof(s_render_method_postprocess_definition, alpha_blend_mode) == 0x68) ? 1 : -1];

#endif // RENDER_METHODS_RENDER_METHOD_DEFINITIONS_H
