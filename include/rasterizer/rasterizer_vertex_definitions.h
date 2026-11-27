#ifndef RASTERIZER_RASTERIZER_VERTEX_DEFINITIONS_H
#define RASTERIZER_RASTERIZER_VERTEX_DEFINITIONS_H

// The vertex declaration (register mappings up to a terminator whose attribute is -1), the declaration
// table and attribute usages owned by tu/graphicslibxbox/rasterizer_vertex_definitions.cpp, and the
// functions defined there that other units call.

#include "rasterizer/rasterizer_vertex_types.h"

extern "C" {
// One declaration per c_rasterizer_vertex_buffer::e_vertex_buffer_declaration; the GPU effect units
// fill their own entries at startup.
extern rasterizer_vertex_declaration g_rasterizer_vertex_buffer_declarations[];
// The D3DDECLUSAGE of each vertex attribute.
extern const unsigned long k_vertex_semantic_usages[];
}

unsigned char rasterizer_vertex_get_stride_from_declaration(const rasterizer_vertex_declaration *declaration);

// Stride padding sizes (in bytes) the GPU units fill with one padding register when
// their vertex elements do not fill the stride.
enum e_vertex_declaration_padding
{
    _vertex_declaration_padding_none = 0,
    _vertex_declaration_padding_four = 4,
    _vertex_declaration_padding_eight = 8,
    _vertex_declaration_padding_twelve = 12
};

#endif // RASTERIZER_RASTERIZER_VERTEX_DEFINITIONS_H
