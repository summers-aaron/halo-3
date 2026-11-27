// hkvisualize/hkDisplaySphere.obj: from the records 0x6bbd38 0x6bbd40 0x6bbdf0 0x6bc0c8
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x6bbd38 ?get_wireframe_geometry@phys_display_sphere@@UAAXAAV?$phys_array@Vphys_vec4@@@@@Z
// @address 0x6bbd40 ??0phys_display_sphere@@QAA@ABVphys_sphere@@HH@Z
// @address 0x6bbdf0 ?subdivide_sphere_recursive@@YAXAAUphys_geometry@@HMM@Z
// @address 0x6bc0c8 ?build_display_geometry@phys_display_sphere@@UAAXXZ
#define PHYS_CONFIG_SIMD 1
#include "physics/havok_memory.h"

#include "phys/phys_display_sphere.h"
phys_display_sphere::phys_display_sphere(const phys_sphere& sphere, int x_res, int y_res)
:  phys_display_geometry(PHYS_DISPLAY_SPHERE) ,
   m_display_sphere(sphere), m_x_res(x_res), m_y_res(y_res)
{

}

void subdivide_sphere_recursive( phys_geometry& geometry, int tri_idx, const phys_real radius, phys_real recursions_left )
{
    if( recursions_left == 0 )
    {
        return;
    }

    int vert_count = geometry.m_vertices.getSize();
    int tri_count = geometry.m_triangle_list.getSize();
    geometry.m_vertices.expand_by( 3 );
    geometry.m_triangle_list.expand_by( 3 );

    phys_geometry::geometry_triangle& t = geometry.m_triangle_list[ tri_idx ];

    {
        phys_vec4& v1 = geometry.m_vertices[ t.m_a ];
        phys_vec4& v2 = geometry.m_vertices[ t.m_b ];
        phys_vec4& corner_v3 = geometry.m_vertices[ t.m_c ];
        phys_vec4& mid_v12 = geometry.m_vertices[ vert_count ];
        phys_vec4& mid_v23 = geometry.m_vertices[ vert_count+1 ];
        phys_vec4& mid_v31 = geometry.m_vertices[ vert_count+2 ];
        mid_v12.set_add4( v1, v2 ); mid_v12.normalize_xyz(); mid_v12.mul4( radius );
        mid_v23.set_add4( v2, corner_v3 ); mid_v23.normalize_xyz(); mid_v23.mul4( radius );
        mid_v31.set_add4( corner_v3, v1 ); mid_v31.normalize_xyz(); mid_v31.mul4( radius );
    }

    {
        phys_geometry::geometry_triangle& t1 = geometry.m_triangle_list[ tri_count ];
        phys_geometry::geometry_triangle& t2 = geometry.m_triangle_list[ tri_count+1 ];
        phys_geometry::geometry_triangle& t3 = geometry.m_triangle_list[ tri_count+2 ];
        t1.m_a = t.m_a;	t1.m_b = vert_count;	t1.m_c = vert_count+2;
        t2.m_a = vert_count;	t2.m_b = t.m_b;	t2.m_c = vert_count+1;
        t3.m_a = vert_count+2;	t3.m_b = vert_count+1;	t3.m_c = t.m_c;
        t.m_a = vert_count;		t.m_b = vert_count+1;	t.m_c = vert_count+2;
    }

    {
        subdivide_sphere_recursive( geometry, tri_count, radius, recursions_left-1 );
        subdivide_sphere_recursive( geometry, tri_count+1, radius, recursions_left-1 );
        subdivide_sphere_recursive( geometry, tri_count+2, radius, recursions_left-1 );
        subdivide_sphere_recursive( geometry, tri_idx, radius, recursions_left-1 );
    }
}

void phys_display_sphere::build_display_geometry()
{
    m_geometry = new phys_geometry;
    phys_real r = m_display_sphere.get_convex_radius();

    {
        phys_real x = r * 0.525731112119133606f;
        phys_real z = r * 0.850650808352039932f;
        int i;

        phys_array<phys_vec4>& v = m_geometry->m_vertices;
        v.set_array_size( 12 );
        i=0;
        v[i++].set( -x, 0.0f, z );
        v[i++].set( x, 0.0, z );
        v[i++].set( -x, 0.0, -z );
        v[i++].set( x, 0.0, -z );
        v[i++].set( 0.0, z, x );
        v[i++].set( 0.0, z, -x );
        v[i++].set( 0.0, -z, x );
        v[i++].set( 0.0, -z, -x );
        v[i++].set( z, x, 0.0 );
        v[i++].set( -z, x, 0.0 );
        v[i++].set( z, -x, 0.0 );
        v[i++].set( -z, -x, 0.0 );

        phys_array<phys_geometry::geometry_triangle>& t = m_geometry->m_triangle_list;
        t.set_array_size( 20 );
        i=0;
        t[i++].set( 1,4,0 );
        t[i++].set( 4,9,0 );
        t[i++].set( 4,5,9 );
        t[i++].set( 8,5,4 );
        t[i++].set( 1,8,4 );
        t[i++].set( 1,10,8 );
        t[i++].set( 10,3,8 );
        t[i++].set( 8,3,5 );
        t[i++].set( 3,2,5 );
        t[i++].set( 3,7,2 );
        t[i++].set( 3,10,7 );
        t[i++].set( 10,6,7 );
        t[i++].set( 6,11,7 );
        t[i++].set( 6,0,11 );
        t[i++].set( 6,1,0 );
        t[i++].set( 10,1,6 );
        t[i++].set( 11,0,9 );
        t[i++].set( 2,11,9 );
        t[i++].set( 5,2,9 );
        t[i++].set( 11,2,7 );
    }

    for( int i=0; i<20; ++i )
    {
        subdivide_sphere_recursive( *m_geometry, i, r, 2 );
    }
}

void phys_display_sphere::get_wireframe_geometry(phys_array<phys_vec4>& lines)
{

}

