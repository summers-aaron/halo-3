// hkinternal/hkGeomConvexHullTester.obj: from the records 0x6b5040 0x6b5158 0x6b5298 0x6b53a0 0x6b5648
// @compiler xdk5632
// @flags /TP /O2 /Oc
#include "phys/phys_base_types.h"
#include <vectorintrinsics.h>
#include "phys/phys_math.h"
#include "phys/phys_base.h"







#include "phys/phys_geom_edge.h"

#include "phys/phys_thread_memory.h"


#include "phys/phys_geom_hull.h"

#include "phys/phys_hull_tolerances.h"

#include "phys/phys_hull_builder.h"


#include "phys/phys_hull_tester.h"

// @address 0x6b5040 ?find_same_edges@phys_hull_tester@@SA?AVphys_bool@@ABVplane_and_points@phys_hull_builder@@0AAV2@11@Z
phys_bool phys_hull_tester::find_same_edges(const phys_hull_builder::plane_and_points &plane_points1,
    const phys_hull_builder::plane_and_points &plane_points2, phys_bool &found_edge01, phys_bool &found_edge02, phys_bool &found_edge12)
{
    int plane1_vertex0 = plane_points1.m_hull_edges[0]->m_edge_vertex;
    int plane2_vertex0 = plane_points2.m_hull_edges[0]->m_edge_vertex;
    int plane1_vertex1 = plane_points1.m_hull_edges[1]->m_edge_vertex;
    int plane1_vertex2 = plane_points1.m_hull_edges[2]->m_edge_vertex;
    int plane2_vertex1 = plane_points2.m_hull_edges[1]->m_edge_vertex;
    int plane2_vertex2 = plane_points2.m_hull_edges[2]->m_edge_vertex;
    phys_bool result = true;
    if (plane1_vertex0 == plane2_vertex0)
    {
        if (plane1_vertex1 == plane2_vertex1 || plane1_vertex1 == plane2_vertex2)
        {
            if (found_edge01) result = false;
            found_edge01 = true;
        }
        if (plane1_vertex2 == plane2_vertex1 || plane1_vertex2 == plane2_vertex2)
        {
            if (found_edge02) result = false;
            found_edge02 = true;
        }
    }
    if (plane1_vertex0 == plane2_vertex1)
    {
        if (plane1_vertex1 == plane2_vertex2)
        {
            if (found_edge01) result = false;
            found_edge01 = true;
        }
        if (plane1_vertex2 == plane2_vertex2)
        {
            if (found_edge02) result = false;
            found_edge02 = true;
        }
    }
    if ((plane1_vertex1 == plane2_vertex0 && (plane1_vertex2 == plane2_vertex1 || plane1_vertex2 == plane2_vertex2)) ||
        (plane1_vertex1 == plane2_vertex1 && plane1_vertex2 == plane2_vertex2))
    {
        if (found_edge12) result = false;
        found_edge12 = true;
    }
    return result;
}

// @address 0x6b5158 ?check_plane_equations@phys_hull_tester@@SA?AVphys_bool@@AAV?$phys_array@Vphys_vec4@@@@00M@Z
phys_bool phys_hull_tester::check_plane_equations(
    phys_array<phys_vec4> &initial_verts, phys_array<phys_vec4> &used_vertices, phys_array<phys_vec4> &hull_plane_equations, float coplanar_eps)
{
    phys_bool result = true;
    for (int i = 0; result && i < hull_plane_equations.getSize(); i++)
    {
        for (int j = 0; result && j < initial_verts.getSize(); j++)
        {
            float distance = hull_plane_equations[i].dot4_xyz1(initial_verts[j]);
            result = result && distance < coplanar_eps;
        }
        for (int k = 0; result && k < used_vertices.getSize(); k++)
        {
            result = result && hull_plane_equations[i].dot4_xyz1(used_vertices[k]) < coplanar_eps;
        }
    }
    return result;
}


// @address 0x6b5298 ?is_valid_planar_hull@phys_hull_tester@@SA?AVphys_bool@@AAV?$phys_array@Vphys_vec4@@@@AAVphys_geom_hull@@00AAV?$phys_array@Vplane_and_points@phys_hull_builder@@@@M@Z
class phys_bool phys_hull_tester::is_valid_planar_hull(class phys_array<class phys_vec4> &p0, class phys_geom_hull &p1,
    class phys_array<class phys_vec4> &p2, class phys_array<class phys_vec4> &p3,
    class phys_array<class phys_hull_builder::plane_and_points> &p4, float p5)
{
    phys_bool result = true;
    result = static_cast<bool>(check_plane_equations(p0, p2, p3, p5));
    for (int i = 0; i < p4.m_size; i++)
    {
        phys_bool e0 = false, e1 = false, e2 = false;
        for (int j = 0; j < p4.m_size; j++)
        {
            if (j != i)
                find_same_edges(p4.m_data[i], p4.m_data[j], e0, e1, e2);
        }
        result = result && e0 && e1 && e2;
    }
    return result;
}

// @address 0x6b53a0 ?is_valid_nonplanar_hull@phys_hull_tester@@SA?AVphys_bool@@AAV?$phys_array@Vphys_vec4@@@@AAVphys_geom_hull@@00AAV?$phys_array@Vplane_and_points@phys_hull_builder@@@@M@Z
phys_bool phys_hull_tester::is_valid_nonplanar_hull(phys_array<phys_vec4> &initial_verts, phys_geom_hull &geom_hull, phys_array<phys_vec4> &used_vertices,
    phys_array<phys_vec4> &hull_plane_equations, phys_array<phys_hull_builder::plane_and_points> &tangent_planes, float coplanar_eps)
{
    phys_bool result = true;
    result = result && check_plane_equations(initial_verts, used_vertices, hull_plane_equations, coplanar_eps);
    for (int i = 0; i < tangent_planes.getSize(); i++)
    {
        result = result && tangent_planes[i].m_hull_edges[0]->m_edge_vertex != tangent_planes[i].m_hull_edges[1]->m_edge_vertex &&
            tangent_planes[i].m_hull_edges[1]->m_edge_vertex != tangent_planes[i].m_hull_edges[2]->m_edge_vertex;
        phys_bool found_edge01 = false;
        phys_bool found_edge02 = false;
        phys_bool found_edge12 = false;
        for (int j = 0; j < tangent_planes.getSize(); j++)
        {
            if (j != i)
            {
                result = result &&
                    !(tangent_planes[i].m_hull_edges[0]->m_edge_vertex == tangent_planes[j].m_hull_edges[0]->m_edge_vertex &&
                        tangent_planes[i].m_hull_edges[1]->m_edge_vertex == tangent_planes[j].m_hull_edges[1]->m_edge_vertex &&
                        tangent_planes[i].m_hull_edges[2]->m_edge_vertex == tangent_planes[j].m_hull_edges[2]->m_edge_vertex &&
                        tangent_planes[i].m_hull_plane.approx_equal4(tangent_planes[j].m_hull_plane, 0.001f));
                result = result && find_same_edges(tangent_planes[i], tangent_planes[j], found_edge01, found_edge02, found_edge12);
            }
        }
        result = result && found_edge01 && found_edge02 && found_edge12;
    }
    int hull_edge_count = geom_hull.m_hull_edges.getSize() / 2;
    int hull_face_count = tangent_planes.getSize();
    int hull_vert_count = used_vertices.getSize();
    if (hull_vert_count > 2)
    {
        result = result && (hull_vert_count + hull_face_count - hull_edge_count - 2 == 0);
    }
    return result;
}

// @address 0x6b5648 ?is_valid_hull@phys_hull_tester@@SA?AVphys_bool@@ABUphys_hull_tolerances@@PBVphys_vec4@@HAAVphys_geom_hull@@AAV?$phys_array@Vphys_vec4@@@@@Z
phys_bool phys_hull_tester::is_valid_hull(const phys_hull_tolerances &hull_tolerances, const phys_vec4 *initial_verts,
    int initial_vertex_count, phys_geom_hull &geom_hull, phys_array<phys_vec4> &used_vertices)
{
    phys_bool planar = false;
    phys_array<phys_hull_builder::plane_and_points> tangent_planes;
    phys_array<phys_vec4> hull_plane_equations;
    float coplanar_eps = hull_tolerances.m_coplanar_tol;
    phys_array<phys_vec4> verts;
    for (int i = 0; i < initial_vertex_count; i++)
    {
        verts.array_push_back(initial_verts[i]);
    }
    phys_vec4 used_half_extents, used_center_pos, verts_half_extents, verts_center_pos;
    if (hull_tolerances.m_convert_to_unit_cube)
    {
        phys_hull_builder::convert_to_unit_cube(used_vertices, used_half_extents, used_center_pos);
        phys_hull_builder::convert_to_unit_cube(verts, verts_half_extents, verts_center_pos);
    }
    phys_bool result = true;
    phys_vec4 plane;
    phys_hull_builder::build_plane_equations(hull_tolerances, geom_hull, used_vertices, plane, planar, hull_plane_equations, tangent_planes);
    if (planar)
    {
        result = is_valid_planar_hull(verts, geom_hull, used_vertices, hull_plane_equations, tangent_planes, coplanar_eps);
    }
    else
    {
        result = is_valid_nonplanar_hull(verts, geom_hull, used_vertices, hull_plane_equations, tangent_planes, coplanar_eps);
    }
    if (hull_tolerances.m_convert_to_unit_cube)
    {
        phys_hull_builder::convert_from_unit_cube(used_vertices, used_half_extents, used_center_pos);
        phys_hull_builder::convert_from_unit_cube(verts, verts_half_extents, verts_center_pos);
    }
    return result;
}
