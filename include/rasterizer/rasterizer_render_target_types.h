#ifndef RASTERIZER_RASTERIZER_RENDER_TARGET_TYPES_H
#define RASTERIZER_RASTERIZER_RENDER_TARGET_TYPES_H

#include <stddef.h>
#include "rasterizer/rasterizer.h"

#include "rasterizer/render_surface_types.h"
class c_render_surface_group {
public:
    long release();
    c_render_surface *get_specialized_surface(c_rasterizer::e_surface surface);
    long create(const s_surface_group_description &, c_rasterizer::e_surface, D3DSurface *);
    s_surface_group_description description;
    long count;
    c_render_surface *surfaces;
    static c_render_surface g_surface_buffer[224];
};

typedef char render_surface_group_count_offset[(offsetof(c_render_surface_group, count) == 0x48) ? 1 : -1];
typedef char render_surface_group_surfaces_offset[(sizeof(void *) != 4 || ((offsetof(c_render_surface_group, surfaces) == 0x4c))) ? 1 : -1];

#endif
