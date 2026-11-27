#ifndef RENDER_RENDER_GEOMETRY_H
#define RENDER_RENDER_GEOMETRY_H

class c_rasterizer_index_buffer;
struct s_render_geometry;

extern "C" const c_rasterizer_index_buffer *render_geometry_get_index_buffer(const s_render_geometry *geometry, long entry_index);

#endif // RENDER_RENDER_GEOMETRY_H
