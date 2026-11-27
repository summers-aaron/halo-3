#ifndef RASTERIZER_RASTERIZER_RENDER_GLOBALS_TYPES_H
#define RASTERIZER_RASTERIZER_RENDER_GLOBALS_TYPES_H

// c_rasterizer::render_globals (defined in tu/graphicslibxbox/rasterizer_globals.cpp): the render target size, the
// display size and the shadow map size. Fields past shadow_size are not read by any unit yet.
struct s_rasterizer_render_globals
{
    long width;
    long height;
    long display_width;
    long display_height;
    long shadow_size;
};

#endif // RASTERIZER_RASTERIZER_RENDER_GLOBALS_TYPES_H
