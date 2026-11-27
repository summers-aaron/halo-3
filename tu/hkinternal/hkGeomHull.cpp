// hkinternal/hkGeomHull.obj: from the records 0x6b3500 0x6b3580 0x6b3650 0x6b3a00 0x6b3a70 0x6b3b88
// @compiler xdk5632
// @flags /TP /O2 /Oc
// @address 0x6b3500 ?init_from_vertex@phys_geom_hull@@QAAXH@Z
// @address 0x6b3580 ?init_from_edge@phys_geom_hull@@QAAXHH@Z
#include "phys/phys_geom_hull.h"
void phys_geom_hull::init_from_vertex(int vertex)
{
    m_hull_edges.m_size = 0;
    phys_geom_edge edge;
    edge.m_edge_vertex = (unsigned short)vertex;
    edge.m_mirror_edge = 0;
    edge.m_next = 0;
    m_hull_edges.array_push_back(edge);
}

void phys_geom_hull::init_from_edge(int vertex0, int vertex1)
{
    m_hull_edges.m_size = 0;
    phys_geom_edge first_hull_edge, second_edge;
    first_hull_edge.m_edge_vertex = (unsigned short)vertex0;
    first_hull_edge.m_mirror_edge = 1;
    first_hull_edge.m_next = 1;
    second_edge.m_edge_vertex = (unsigned short)vertex1;
    second_edge.m_mirror_edge = 0;
    second_edge.m_next = 0;
    m_hull_edges.array_push_back(first_hull_edge);
    m_hull_edges.array_push_back(second_edge);
}

// @address 0x6b3a00 ??0phys_geom_hull@@QAA@XZ
phys_geom_hull::phys_geom_hull()
    : m_vertex_base(0)
{
    m_hull_edges.set_array_size(0);
}

// @address 0x6b3b88 ?is_valid_topology@phys_geom_hull@@QAA?AVphys_bool@@XZ
phys_bool phys_geom_hull::is_valid_topology()
{
    phys_geom_edge *edge_base = m_hull_edges.m_data;
    phys_bool result(true);
    for (int i = 0; i < m_hull_edges.m_size; i++)
    {
        phys_bool vertex_in_range;
        int mirror = m_hull_edges.m_data[i].m_mirror_edge;
        vertex_in_range = mirror >= 0 && mirror < m_hull_edges.m_size;
        result = result && vertex_in_range;
        result = result && (m_hull_edges.m_data[mirror].m_mirror_edge == i);
    }
    for (int i = 0; i < m_hull_edges.m_size; i++)
    {
        phys_geom_edge_idx vertex = edge_base[edge_base[edge_base[i].m_mirror_edge].m_next].m_edge_vertex;
        result = result && (vertex == edge_base[i].m_edge_vertex);
    }
    if (m_hull_edges.m_size > 2)
    {
        for (int i = 0; i < m_hull_edges.m_size; i++)
        {
            phys_geom_edge_idx next = edge_base[edge_base[edge_base[i].m_next].m_next].m_next;
            result = result && (i == next);
        }
    }
    for (int i = 0; i < m_hull_edges.m_size; i++)
        m_hull_edges.m_data[i].m_bvh_info = 0;
    visit_next_and_mirror_edges(m_hull_edges.m_data);
    for (int i = 0; i < m_hull_edges.m_size; i++)
        result = result && (m_hull_edges.m_data[i].m_bvh_info == 1);
    return result;
}

// @address 0x6b3650 ?init_from_triangle@phys_geom_hull@@QAAXHHH@Z
void phys_geom_hull::init_from_triangle(int v0, int v1, int v2)
{
    m_hull_edges.m_size = 0;

    phys_vec4 e0 = m_vertex_base[v1];
    e0.sub4(m_vertex_base[v0]);
    float l0 = e0.length_sq_4d();
    phys_vec4 e1 = m_vertex_base[v2];
    e1.sub4(m_vertex_base[v1]);
    float l1 = e1.length_sq_4d();
    phys_vec4 e2 = m_vertex_base[v0];
    e2.sub4(m_vertex_base[v2]);
    float edge2_len_sq = e2.length_sq_4d();

    e0.normalize_xyz();
    e1.normalize_xyz();
    e2.normalize_xyz();

    phys_vec4 d;
    d.set_sub4(e1, e0);
    if (d.length_sq_3d() < 1e-6f)
    {
        float max_edge_len = phys_math::max_of_two(l0, phys_math::max_of_two(l1, edge2_len_sq));
        if (l0 == max_edge_len)
        {
            init_from_edge(v0, v1);
        }
        else if (l1 == max_edge_len)
        {
            init_from_edge(v1, v2);
        }
        else if (edge2_len_sq == max_edge_len)
        {
            init_from_edge(v2, v0);
        }
    }
    else
    {
        phys_geom_edge a;
        phys_geom_edge b;
        phys_geom_edge c;
        phys_geom_edge scratch_edge;
        phys_geom_edge e;
        phys_geom_edge f;
        a.m_edge_vertex = (unsigned short)v0;
        a.m_mirror_edge = 1;
        a.m_next = 2;
        b.m_edge_vertex = (unsigned short)v1;
        b.m_mirror_edge = 0;
        b.m_next = 5;
        c.m_edge_vertex = (unsigned short)v1;
        c.m_mirror_edge = 3;
        c.m_next = 4;
        scratch_edge.m_edge_vertex = (unsigned short)v2;
        scratch_edge.m_mirror_edge = 2;
        scratch_edge.m_next = 1;
        e.m_edge_vertex = (unsigned short)v2;
        e.m_mirror_edge = 5;
        e.m_next = 0;
        f.m_edge_vertex = (unsigned short)v0;
        f.m_mirror_edge = 4;
        f.m_next = 3;
        m_hull_edges.array_push_back(a);
        m_hull_edges.array_push_back(b);
        m_hull_edges.array_push_back(c);
        m_hull_edges.array_push_back(scratch_edge);
        m_hull_edges.array_push_back(e);
        m_hull_edges.array_push_back(f);
    }
}

// @address 0x6b3a70 ?visit_next_and_mirror_edges@phys_geom_hull@@QAAXPAVphys_geom_edge@@@Z
void phys_geom_hull::visit_next_and_mirror_edges(phys_geom_edge *edge)
{
    phys_geom_edge *base = m_hull_edges.begin();
    phys_inline_array<phys_geom_edge *, 1024> pending;
    pending.array_push_back(edge);
    while (pending.getSize() != 0) {
        phys_geom_edge *current = pending[pending.getSize() - 1];
        pending.drop_back();
        phys_geom_edge *next = current;
        do {
            next = next->get_next_edge(base);
            next->m_bvh_info = 1;
        } while (next != current);
        next = current;
        do {
            next = next->get_next_edge(base);
            phys_geom_edge *mirror = next->get_mirror_edge(base);
            if (mirror->m_bvh_info == 0)
                pending.array_push_back(mirror);
        } while (next != current);
    }
}

