#ifndef RASTERIZER_RASTERIZER_VERTEX_DECLARATION_TABLE_H
#define RASTERIZER_RASTERIZER_VERTEX_DECLARATION_TABLE_H

#include <xtl.h>
#include "rasterizer/rasterizer_vertex_declaration_types.h"

extern "C" {
extern D3DVertexDeclaration *g_rasterizer_vertex_declarations[22][4][4];
extern const XMVECTORI k_dhenn3_packing_constant;
}

#endif
