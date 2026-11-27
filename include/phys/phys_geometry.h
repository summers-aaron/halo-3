#ifndef INCLUDE_PHYS_GEOMETRY_H
#define INCLUDE_PHYS_GEOMETRY_H

#include "phys/phys_base.h"

struct phys_geometry {
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_COLLISION_INFO, phys_geometry);
    struct geometry_triangle {
        PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_BASE_CLASS, phys_geometry::geometry_triangle);
        int m_a;
        int m_b;
        int m_c;
        void set(int a, int b, int c) { m_a = a; m_b = b; m_c = c; }
    };
    phys_array<phys_vec4> m_vertices;
    phys_array<geometry_triangle> m_triangle_list;
};

#endif // INCLUDE_HAVOK_HK_GEOMETRY_H
