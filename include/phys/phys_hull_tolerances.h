#ifndef INCLUDE_PHYS_HULL_TOLERANCES_H
#define INCLUDE_PHYS_HULL_TOLERANCES_H

#include "phys/phys_base_types.h"

struct phys_hull_tolerances {
    phys_bool m_post_filter;
    phys_bool m_convert_to_unit_cube;
    phys_bool m_accurate_but_slow;
    float m_edge_weld_tol;
    float m_degenerate_tol;
    float m_coplanar_vertex_tol;
    float m_coplanar_plane_tol;
    float m_coplanar_tol;
    float m_opposite_normal_tol;
    float m_min_proj;
    float m_max_angle;
    float m_plane_eqn_min_len;
    float m_hull_tol;
    float m_hull_tol2;
    float m_hull_tol3;

    __forceinline phys_hull_tolerances()
        : m_post_filter(true), m_edge_weld_tol(2e-5f), m_degenerate_tol(4e-6f),
          m_coplanar_vertex_tol(1e-6f), m_coplanar_plane_tol(1e-5f),
          m_coplanar_tol(0.05f), m_opposite_normal_tol(1e-6f), m_min_proj(1e-6f),
          m_max_angle(1e-8f), m_plane_eqn_min_len(1e-6f), m_hull_tol(10e-5f), m_hull_tol2(10e-6f), m_hull_tol3(2e-5f) {}
};

typedef char havok_size_hkGeomConvexHullTolerances[(sizeof(phys_hull_tolerances) == 0x34) ? 1 : -1];

#endif
