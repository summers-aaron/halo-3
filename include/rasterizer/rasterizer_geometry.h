#ifndef RASTERIZER_RASTERIZER_GEOMETRY_H
#define RASTERIZER_RASTERIZER_GEOMETRY_H

#include "cache/geometry_globals.h"

class c_rasterizer_index_buffer;
struct s_render_geometry;

extern "C" c_rasterizer_index_buffer *render_geometry_get_index_buffer(const s_render_geometry *geometry, long entry_index);

#endif
