#ifndef RASTERIZER_IMPLICIT_GEOMETRY_TYPES_H
#define RASTERIZER_IMPLICIT_GEOMETRY_TYPES_H

#include "tag_files/tag_types.h"
#include "math/matrix_math_types.h"
#include <stddef.h>

struct s_circle_strip_vertex
{
    float x, y, z;
    float u, v;
};

struct s_implicit_geometry_primitive
{
    long type;
    long render_method_index;
    real_argb_color color;
    real_matrix4x3 transform;
    float parameters[4];
};

struct s_implicit_geometry_globals
{
    int count;
    s_implicit_geometry_primitive primitives[64];
    s_circle_strip_vertex circle_strip[128];
};

typedef char implicit_geometry_primitive_size[(sizeof(s_implicit_geometry_primitive) == 0x5c) ? 1 : -1];
typedef char implicit_geometry_primitive_centroid_offset[(offsetof(s_implicit_geometry_primitive, transform.position) == 0x40) ? 1 : -1];
typedef char implicit_geometry_primitives_offset[(offsetof(s_implicit_geometry_globals, primitives) == 4) ? 1 : -1];
typedef char implicit_geometry_circle_strip_offset[(offsetof(s_implicit_geometry_globals, circle_strip) == 0x1704) ? 1 : -1];
typedef char implicit_geometry_globals_size[(sizeof(s_implicit_geometry_globals) == 0x2104) ? 1 : -1];

struct rasterizer_implicit_geometry_binding
{
    unsigned short destination;
    unsigned char source;
    unsigned char flags;
};
struct rasterizer_implicit_geometry_entry
{
    unsigned short textures;
    unsigned short vertex_constants;
    char spare_after_vertex_constants[4];
    unsigned short pixel_constants;
    unsigned short integer_constants;
    unsigned short boolean_constants;
    unsigned short samplers;
    unsigned short vertex_externs;
    unsigned short spare_after_vertex_externs;
    unsigned short pixel_externs;
    char spare_after_pixel_externs[6];
};
struct rasterizer_implicit_geometry_template
{
    s_tag_reference vertex_shader;
    s_tag_reference pixel_shader;
    unsigned long entry_point_mask;
    s_tag_block entry_points;
    s_tag_block entries;
    s_tag_block bindings;
};

typedef char implicit_geometry_entry_size[(sizeof(rasterizer_implicit_geometry_entry) == 0x1c) ? 1 : -1];
typedef char implicit_geometry_samplers_offset[(offsetof(rasterizer_implicit_geometry_entry, samplers) == 0xe) ? 1 : -1];
typedef char implicit_geometry_vertex_externs_offset[(offsetof(rasterizer_implicit_geometry_entry, vertex_externs) == 0x10) ? 1 : -1];
typedef char implicit_geometry_pixel_externs_offset[(offsetof(rasterizer_implicit_geometry_entry, pixel_externs) == 0x14) ? 1 : -1];

#endif
