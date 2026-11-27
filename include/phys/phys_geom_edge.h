#ifndef INCLUDE_PHYS_GEOM_EDGE_H
#define INCLUDE_PHYS_GEOM_EDGE_H

#include "phys/phys_math.h"

class phys_geom_edge {
public:
    // March 0x83590910 leaves m_info uninitialized.
    __forceinline phys_geom_edge() {}
    __forceinline const phys_vec4 *get_vertex(const phys_vec4 *base) const { return base + m_edge_vertex; }
    __forceinline phys_geom_edge *get_mirror_edge(phys_geom_edge *base) const { return base + m_mirror_edge; }
    __forceinline phys_geom_edge *get_next_edge(phys_geom_edge *base) const { return base + m_next; }

    unsigned short m_edge_vertex;
    unsigned short m_mirror_edge;
    unsigned short m_next;
    unsigned short m_bvh_info;
};

typedef unsigned short phys_geom_edge_idx;
typedef char havok_size_hkGeomEdge[(sizeof(phys_geom_edge) == 8) ? 1 : -1];

#endif
