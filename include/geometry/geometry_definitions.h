#ifndef GEOMETRY_GEOMETRY_DEFINITIONS_H
#define GEOMETRY_GEOMETRY_DEFINITIONS_H

class c_rasterizer_vertex_buffer;
struct s_render_geometry;

c_rasterizer_vertex_buffer *render_geometry_get_vertex_buffer(const s_render_geometry *geometry, long vertex_buffer_index);

#endif
