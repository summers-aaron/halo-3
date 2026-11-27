#ifndef INCLUDE_PHYS_STRIDED_VERTICES_H
#define INCLUDE_PHYS_STRIDED_VERTICES_H

struct phys_strided_vertices {
    const float *m_vertices;
    int m_hull_vertex_count;
    int m_vertex_stride_bytes;
};

#endif
