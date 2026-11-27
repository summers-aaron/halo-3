#ifndef RASTERIZER_RASTERIZER_DX9_DYNAMIC_GEOMETRY_H
#define RASTERIZER_RASTERIZER_DX9_DYNAMIC_GEOMETRY_H

// Functions defined in tu/graphicslibxbox/rasterizer_dx9_dynamic_geometry.cpp that other units call.

union point2d;
struct s_tag_reference;

void rasterizer_quad_screenspace(const point2d (&points)[4], unsigned long color, const s_tag_reference *bitmap_reference,
    short bitmap_index, bool unknown);

#endif // RASTERIZER_RASTERIZER_DX9_DYNAMIC_GEOMETRY_H
