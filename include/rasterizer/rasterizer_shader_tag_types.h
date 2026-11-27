#ifndef RASTERIZER_RASTERIZER_SHADER_TAG_TYPES_H
#define RASTERIZER_RASTERIZER_SHADER_TAG_TYPES_H

#include <stddef.h>
#include "tag_files/tag_types.h"

struct s_rasterizer_global_shader_entry_point
{
    s_tag_block category_dependency;
    long shader_index;
};
typedef char global_shader_entry_point_size[(sizeof(s_rasterizer_global_shader_entry_point) == 0x10) ? 1 : -1];
typedef char global_shader_entry_point_index_offset[(offsetof(s_rasterizer_global_shader_entry_point, shader_index) == 0xc) ? 1 : -1];

struct s_rasterizer_global_pixel_shader_definition
{
    s_tag_block entry_points;
    unsigned long version;
    s_tag_block shaders;
};
struct s_rasterizer_global_vertex_shader_definition
{
    s_tag_block vertex_types;
    unsigned long version;
    s_tag_block shaders;
};
typedef char global_pixel_shader_definition_size[(sizeof(s_rasterizer_global_pixel_shader_definition) == 0x1c) ? 1 : -1];
typedef char global_vertex_shader_definition_size[(sizeof(s_rasterizer_global_vertex_shader_definition) == 0x1c) ? 1 : -1];
typedef char global_pixel_shader_block_offset[(offsetof(s_rasterizer_global_pixel_shader_definition, shaders) == 0x10) ? 1 : -1];
typedef char global_vertex_shader_block_offset[(offsetof(s_rasterizer_global_vertex_shader_definition, shaders) == 0x10) ? 1 : -1];

#endif
