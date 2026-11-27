#ifndef RENDER_METHODS_RENDER_METHOD_SHADER_DEFINITION_TYPES_H
#define RENDER_METHODS_RENDER_METHOD_SHADER_DEFINITION_TYPES_H
#include <stddef.h>
#include "tag_files/tag_types.h"

// Shared tag layout used by shader submission and the contrail shader initializers.
struct s_render_method_shader_definition
{
    unsigned char data_0000[52];
    s_tag_reference shared_pixel_shader;
    s_tag_reference shared_vertex_shader;
};
struct s_global_vertex_shader_entry_point
{
    s_tag_block category_dependency;
    long constant_index;
};
typedef char render_method_shared_vertex_shader_offset[(offsetof(s_render_method_shader_definition, shared_vertex_shader) == 68) ? 1 : -1];
typedef char global_vertex_shader_entry_point_size[(sizeof(s_global_vertex_shader_entry_point) == 16) ? 1 : -1];
#endif
