#ifndef INCLUDE_PHYS_GEOM_HULL_H
#define INCLUDE_PHYS_GEOM_HULL_H

#include <stddef.h>

#include "phys/phys_base.h"
#include "phys/phys_geom_edge.h"

class phys_geom_hull {
public:
    phys_geom_hull();
    void init_from_vertex(int vertex);
    void init_from_edge(int vertex0, int vertex1);
    void init_from_triangle(int vertex0, int vertex1, int vertex2);
    phys_bool is_valid_topology();
    void visit_next_and_mirror_edges(phys_geom_edge *edge);

    phys_vec4 *m_vertex_base;
    phys_inline_array<phys_geom_edge, 128> m_hull_edges;
};

typedef char havok_offset_hkGeomHull_edges[(sizeof(void *) != 4 || offsetof(phys_geom_hull, m_hull_edges) == 4) ? 1 : -1];
typedef char havok_size_hkGeomHull[(sizeof(void *) != 4 || ((sizeof(phys_geom_hull) == 0x410) )) ? 1 : -1];

#endif
