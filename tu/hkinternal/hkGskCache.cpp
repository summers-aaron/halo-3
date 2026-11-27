// hkinternal/hkGskCache.obj: from the records 0x6876b8 0x6878b0
// @flags /TP /O2 /Oc
// match: 0x6878b0: retain the trivial hkVector4 construction used by the native unit.
#define PHYS_DISABLE_VEC4_CONSTRUCTORS
// @compiler xdk5632
#include <vectorintrinsics.h>
#include "phys/phys_math.h"
#include "phys/phys_collide.h"
#include "phys/phys_triangle_shape.h"
#include "phys/phys_vec4_util.h"


// @address 0x6876b8 ?init@phys_simplex_cache@@QAAXPBVphys_convex_shape@@0ABVphys_transform@@@Z
void phys_simplex_cache::init(const phys_convex_shape *penetration_shape_a, const phys_convex_shape *penetration_shape_b, const phys_transform &transform)
{
    phys_vertex_id ids[4];
    ids[0] = 0;
    phys_hull_vertex capsule_vertex_a[1];
    phys_hull_vertex capsule_vertex_b[1];
    penetration_shape_a->vertex_ids_to_vertices_impl(ids, 1, capsule_vertex_a);
    penetration_shape_b->vertex_ids_to_vertices_impl(ids, 1, capsule_vertex_b);

    __vector4 b = capsule_vertex_b[0].m_simd_quad;
    __vector4 pB = __vaddfp(__vmaddfp(transform.m_rotation.m_column2.m_simd_quad, __vspltw(b, 2),
                                __vmaddfp(transform.m_rotation.m_column1.m_simd_quad, __vspltw(b, 1),
                                    __vmulfp(transform.m_rotation.m_column0.m_simd_quad, __vspltw(b, 0)))),
        transform.m_translation.m_simd_quad);

    phys_vec4 diff;
    diff.m_simd_quad = __vsubfp(pB, capsule_vertex_a[0].m_simd_quad);
    phys_hull_vertex support_a[1];
    penetration_shape_a->get_support_vertex_impl(diff, support_a[0]);

    diff.m_simd_quad = __vsubfp(pB, support_a[0].m_simd_quad);
    __vector4 zero_w_quad = phys_simd_vec_0000;
    {
        phys_vec4 neg_dir;
        neg_dir.m_simd_quad = __vmrghw(__vmrghw(__vmsum3fp(transform.m_rotation.m_column0.m_simd_quad, diff.m_simd_quad),
                                            __vmsum3fp(transform.m_rotation.m_column2.m_simd_quad, diff.m_simd_quad)),
                                   __vmrghw(__vmsum3fp(transform.m_rotation.m_column1.m_simd_quad, diff.m_simd_quad), zero_w_quad));
        neg_dir.set_neg4(neg_dir);
        phys_hull_vertex support_b[1];
        penetration_shape_b->get_support_vertex_impl(neg_dir, support_b[0]);
        m_vertices[1] = (unsigned short)support_b[0].get_packed_id();
    }
    m_vertices[0] = (unsigned short)support_a[0].get_packed_id();
    m_vertices[2] = 0xabcd;
    m_vertices[3] = 0xabcd;
    m_dim_a = 1;
    m_dim_b = 1;

    int v1 = penetration_shape_a->get_collision_sphere_count_impl();
    int r3 = penetration_shape_b->get_collision_sphere_count_impl();
    if (v1 > 3) v1 = 255;
    m_max_dim_a = (unsigned char)v1;

    if (r3 <= 3) {
        m_max_dim_b = (unsigned char)r3;
        return;
    }
    m_max_dim_b = 255;
}

// @address 0x6878b0 ?init_triangle@phys_simplex_cache@@QAAXPBVphys_convex_shape@@PBVphys_triangle_shape@@ABVphys_transform@@@Z
void phys_simplex_cache::init_triangle(const phys_convex_shape *penetration_shape_a, const phys_triangle_shape *penetration_shape_b,
    const phys_transform &b_to_a_transform)
{
    if (penetration_shape_b->is_extruded())
    {
        init(penetration_shape_a, penetration_shape_b, b_to_a_transform);
        return;
    }

    m_vertices[1] = 0;
    m_vertices[2] = 1;
    m_vertices[3] = 2;

    phys_vec4 verts[3];
    phys_vec4_util::transform_points(b_to_a_transform, penetration_shape_b->get_vertices(), 3, verts);

    phys_vec4 e0; e0.set_sub4(verts[1], verts[0]);
    phys_vec4 e1; e1.set_sub4(verts[2], verts[1]);
    phys_vec4 normal;
    normal.set_cross(e0, e1);

    phys_vertex_id id = 0;
    phys_hull_vertex capsule_vertex_a;
    penetration_shape_a->vertex_ids_to_vertices_impl(&id, 1, &capsule_vertex_a);

    phys_vec4 diff; diff.set_sub4(capsule_vertex_a, verts[0]);
    if (float(diff.dot3(normal)) < 0.0f)
    {
        normal.set_neg4(normal);
    }

    phys_hull_vertex support_a;
    penetration_shape_a->get_support_vertex_impl(normal, support_a);
    m_vertices[0] = (unsigned short)support_a.get_packed_id();
    m_dim_a = 1;
    m_dim_b = 3;

    int n = penetration_shape_a->get_collision_sphere_count_impl();
    m_max_dim_a = (unsigned char)((n > 3) ? 255 : n);
    m_max_dim_b = 3;
}
