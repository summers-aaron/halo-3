// hkinternal/hkGskManifoldUtil.obj: from the records 0x698900 0x698b08 0x698d28 0x698f38 0x699120 0x6991a0 0x699210 0x6993a0 0x699510 0x699f08 0x69a0c8 0x69a160 0x69ac20 0x69afb0
// @flags /TP /O2 /Oc
// @compiler xdk5632
// @address 0x699120 phys_simplex_manifold_cleanup
#include <xtl.h>
#include <vectorintrinsics.h>
#include "phys/phys_simplex_manifold_util.h"
#include "phys/phys_collide.h"
#include "phys/phys_simplex_manifold.h"
#include "phys/phys_collision_proxy.h"
#include "phys/phys_collision_input.h"
#include "phys/phys_constraint_owner.h"
#include "phys/phys_contact_point.h"
#include "phys/phys_quad_constants.h"

typedef phys_simplex_manifold::manifold_contact phys_simplex_manifold_point;

struct phys_simplex_manifold_convert_out {
    __vector4 m_position;
    __vector4 separation_quad;
    __vector4 m_normal;
};

extern "C" void phys_simplex_manifold_cleanup(phys_simplex_manifold *box_manifold, phys_contact_manager *contact_manager,
    phys_constraint_owner &manifold_constraint_owner)
{
    for (unsigned int point_index = 0; point_index < box_manifold->m_contact_point_count; point_index++) {
        if (box_manifold->m_process_contact_points[point_index].m_handle_id != 0xffff)
            contact_manager->contact_remove_point(box_manifold->m_process_contact_points[point_index].m_handle_id,
                manifold_constraint_owner);
    }
    box_manifold->m_contact_point_count = 0;
    box_manifold->m_vert_count_a = 0;
    box_manifold->m_vert_count_b = 0;
}


// @address 0x698f38 phys_simplex_manifold_point_exists_resort
extern "C" unsigned char phys_simplex_manifold_point_exists_resort(phys_simplex_manifold &box_manifold,
    const phys_simplex_cache &manifold_new_point)
{
    const unsigned int atom_contact_count = box_manifold.m_contact_point_count;
    for (unsigned int i = 0; i < atom_contact_count; i++) {
        const phys_simplex_manifold_point &cp = box_manifold.m_process_contact_points[i];
        if (manifold_new_point.m_dim_a == cp.m_dim_a && manifold_new_point.m_dim_b == cp.m_dim_b) {
            const int simplex_dim = manifold_new_point.m_dim_b + manifold_new_point.m_dim_a;
            if (manifold_new_point.m_vertices[0] == box_manifold.get_vertex_id(cp, 0)
                && manifold_new_point.m_vertices[1] == box_manifold.get_vertex_id(cp, 1)) {
                if (simplex_dim > 2 && manifold_new_point.m_vertices[2] != box_manifold.get_vertex_id(cp, 2))
                    continue;
                if (simplex_dim > 3 && manifold_new_point.m_vertices[3] != box_manifold.get_vertex_id(cp, 3))
                    continue;
                if (i != 0) {
                    phys_simplex_manifold_point tmp = box_manifold.m_process_contact_points[i];
                    box_manifold.m_process_contact_points[i] = box_manifold.m_process_contact_points[0];
                    box_manifold.m_process_contact_points[0] = tmp;
                }
                return 1;
            }
        }
    }
    if (manifold_new_point.m_dim_a == 1 || manifold_new_point.m_dim_b == 1) {
        for (unsigned int i = 0; i < atom_contact_count; i++) {
            phys_simplex_manifold_point &cp = box_manifold.m_process_contact_points[i];
            if ((manifold_new_point.m_dim_a == 1 && cp.m_dim_a == 1
                    && box_manifold.get_vertex_id(cp, 0) == manifold_new_point.m_vertices[0])
                || (manifold_new_point.m_dim_b == 1 && cp.m_dim_b == 1
                    && box_manifold.get_vertex_id(cp, cp.m_dim_a) == manifold_new_point.m_vertices[manifold_new_point.m_dim_a])) {
                cp.m_dim_a = cp.m_dim_b = 0;
                break;
            }
        }
    }
    return 0;
}

// @address 0x6991a0 ?phys_simplex_manifold_convert_point_point@@YAHABUphys_simplex_manifold_work@@AAUmanifold_contact@phys_simplex_manifold@@AAUphys_simplex_manifold_convert_out@@@Z
__forceinline int phys_simplex_manifold_convert_point_point(const phys_simplex_manifold_work &work,
    phys_simplex_manifold::manifold_contact &cp, phys_simplex_manifold_convert_out &out)
{
    const __vector4 &pA = work.get_vertex_a(cp.m_vert_ids[0]).get_quad();
    const __vector4 &pB = work.get_vertex_b(cp.m_vert_ids[1]).get_quad();
    __vector4 separation = __vsubfp(pA, pB);
    float distance_sq;
    __stvewx(__vmsum3fp(separation, separation), &distance_sq, 0);
    if (distance_sq > work.m_radius_sum_sq)
        return 1;
    out.separation_quad = separation;
    out.m_normal = work.m_master_normal.get_quad();
    out.m_position = pA;
    return 0;
}

extern "C" {
}
// @address 0x699210 ?phys_simplex_manifold_convert_point_edge@@YAHABUphys_simplex_manifold_work@@AAUmanifold_contact@phys_simplex_manifold@@AAUphys_simplex_manifold_convert_out@@@Z
__forceinline int phys_simplex_manifold_convert_point_edge(const phys_simplex_manifold_work &work, phys_simplex_manifold_point &cp,
    phys_simplex_manifold_convert_out &out)
{
    const __vector4 &capsule_vertex_a = work.get_vertex_a(cp.m_vert_ids[0]).get_quad();
    const __vector4 &capsule_vertex_b = work.get_vertex_b(cp.m_vert_ids[1]).get_quad();
    const __vector4 &vertex_c = work.get_vertex_b(cp.m_vert_ids[2]).get_quad();

    out.m_position = capsule_vertex_a;
    out.m_normal = work.m_master_normal.get_quad();

    __vector4 cb = __vsubfp(vertex_c, capsule_vertex_b);
    __vector4 ca = __vsubfp(vertex_c, capsule_vertex_a);
    __vector4 ab = __vsubfp(capsule_vertex_a, capsule_vertex_b);

    __vector4 lhs_one_tmp = __vpermwi(cb, 0x63);
    __vector4 rhs_reg_op = __vpermwi(ca, 0x87);
    __vector4 swiz_prod_lo = __vmulfp(lhs_one_tmp, rhs_reg_op);
    lhs_one_tmp = __vpermwi(cb, 0x87);
    rhs_reg_op = __vpermwi(ca, 0x63);
    __vector4 vec_mul_out = __vmulfp(lhs_one_tmp, rhs_reg_op);
    __vector4 cross = __vsubfp(swiz_prod_lo, vec_mul_out);

    phys_vec4 plane;
    phys_simd_scalar d0(__vmsum3fp(ca, cb));
    plane(0) = d0;
    phys_simd_scalar d1(__vmsum3fp(ab, cb));
    plane(1) = d1;
    phys_simd_scalar d2(__vmsum3fp(cross, cross));
    plane(2) = d2;
    plane(3) = 0.0f;
    phys_simd_scalar d3(__vmsum3fp(cb, cb));
    float edge_len_sq = d3;

    __vector4 plane_quad = plane.m_simd_quad;
    __vector4 negative = __vcmpgtfp(__vspltisw(0), plane_quad);
    unsigned int cr6_bits;
    __vcmpequwR(__vand(negative, phys_vec4_cmp::s_maskFromBits[phys_vec4_cmp::CMP_MASK_XY]), __vspltisw(0), &cr6_bits);
    int any_negative = ~cr6_bits & 0x80;

    if (plane(3) > work.m_radius_sum_sq * edge_len_sq || any_negative)
        return 1;

    __vector4 x = __vspltw(plane_quad, 0);
    __vector4 sum = __vaddfp(x, __vspltw(plane_quad, 1));
    __vector4 one = phys_simd_vec_1111;
    __vector4 r = __vrefp(sum);
    __vector4 q = __vspltw(__vmulfp(x, __vmaddfp(__vnmsubfp(r, sum, one), r, r)), 0);
    out.separation_quad = __vsubfp(capsule_vertex_a, __vmaddfp(q, vertex_c, __vmulfp(__vsubfp(one, q), capsule_vertex_b)));
    return 0;
}



static __forceinline int phys_manifold_util_get_mask(__vector4 mask)
{
    __vector4 one_vec4 = { 1.0f, 1.0f, 1.0f, 1.0f };
    __vector4 bit_select = { 8.0f, 4.0f, 2.0f, 1.0f };
    __vector4 result = mask;
    result = __vand(result, one_vec4);
    result = __vmsum4fp(result, bit_select);
    result = __vctsxs(result, 0);
    return (int)result.u[0];
}
static __forceinline __vector4 phys_manifold_util_cross(__vector4 a, __vector4 b)
{
    __vector4 lhs_one_tmp = __vpermwi(a, 0x63);
    __vector4 rhs_reg_op = __vpermwi(b, 0x87);
    __vector4 swiz_prod_lo = __vmulfp(lhs_one_tmp, rhs_reg_op);
    lhs_one_tmp = __vpermwi(a, 0x87);
    rhs_reg_op = __vpermwi(b, 0x63);
    __vector4 vec_mul_out = __vmulfp(lhs_one_tmp, rhs_reg_op);
    return __vsubfp(swiz_prod_lo, vec_mul_out);
}
static __forceinline phys_simd_scalar phys_manifold_util_get_simd_at(const __vector4 &v, int i)
{
    phys_simd_scalar res;
    switch (i) {
    case 0: res.m_simd_scalar = __vspltw(v, 0); return res;
    case 1: res.m_simd_scalar = __vspltw(v, 1); return res;
    case 2: res.m_simd_scalar = __vspltw(v, 2); return res;
    default: res.m_simd_scalar = __vspltw(v, 3); return res;
    }
}
static __forceinline void phys_manifold_util_set_lerp4(__vector4 &dst, const __vector4 &a, const __vector4 &b, const phys_simd_scalar &t)
{
    __vector4 x = __vspltw(t.m_simd_scalar, 0);
    __vector4 s = __vsubfp(phys_simd_vec_1111, x);
    __vector4 temp = __vmulfp(s, a);
    dst = __vmaddfp(x, b, temp);
}
// @address 0xf00 g_phys_quad_sign_mask
static __forceinline void phys_manifold_util_normalize3(__vector4 &v)
{
    __vector4 temp = __vmsum3fp(v, v);
    __vector4 scale;
    scale.v[0] = 1.0f / __fsqrts(temp.v[0]);
    scale = __vspltw(scale, 0);
    v = __vmulfp(v, scale);
}
// @address 0x698900 ?phys_simplex_manifold_convert_point_face@@YAHABUphys_simplex_manifold_work@@AAUmanifold_contact@phys_simplex_manifold@@AAUphys_simplex_manifold_convert_out@@@Z
__forceinline int phys_simplex_manifold_convert_point_face(const phys_simplex_manifold_work &work, phys_simplex_manifold_point &cp,
    phys_simplex_manifold_convert_out &out)
{
    const __vector4 &capsule_vertex_a = work.get_vertex_a(cp.m_vert_ids[0]).get_quad();
    const __vector4 &vertex_b0 = work.get_vertex_b(cp.m_vert_ids[1]).get_quad();
    const __vector4 &vertex_b1 = work.get_vertex_b(cp.m_vert_ids[2]).get_quad();
    const __vector4 &vertex_b2 = work.get_vertex_b(cp.m_vert_ids[3]).get_quad();

    out.m_position = capsule_vertex_a;
    out.separation_quad = __vsubfp(capsule_vertex_a, vertex_b0);

    __vector4 e0 = __vsubfp(vertex_b2, vertex_b1);
    __vector4 e1 = __vsubfp(vertex_b0, vertex_b2);
    __vector4 e2 = __vsubfp(vertex_b1, vertex_b0);

    __vector4 normal = phys_manifold_util_cross(e0, e1);
    phys_manifold_util_normalize3(normal);
    __vector4 diff_a_b1 = __vsubfp(capsule_vertex_a, vertex_b1);
    __vector4 diff_a_b2 = __vsubfp(capsule_vertex_a, vertex_b2);
    __vector4 diff_a_b0 = __vsubfp(capsule_vertex_a, vertex_b0);

    phys_vec4 dot_products;
    phys_simd_scalar d0(__vmsum3fp(phys_manifold_util_cross(diff_a_b1, e0), normal));
    dot_products(0) = d0;
    phys_simd_scalar d1(__vmsum3fp(phys_manifold_util_cross(diff_a_b2, e1), normal));
    dot_products(1) = d1;
    phys_simd_scalar d2(__vmsum3fp(phys_manifold_util_cross(diff_a_b0, e2), normal));
    dot_products(2) = d2;
    phys_simd_scalar d3(__vmsum3fp(work.m_master_normal.get_quad(), normal));
    dot_products(3) = d3;

    out.m_normal = normal;
    int mask = phys_manifold_util_get_mask(__vcmpgtfp(__vspltisw(0), dot_products.m_simd_quad));
    if (mask & 1)
        out.m_normal = __vxor(normal, g_phys_quad_sign_mask);
    return ~mask & 14;
}

// @address 0x698b08 ?phys_simplex_manifold_convert_face_point@@YAHABUphys_simplex_manifold_work@@AAUmanifold_contact@phys_simplex_manifold@@AAUphys_simplex_manifold_convert_out@@@Z
__forceinline int phys_simplex_manifold_convert_face_point(const phys_simplex_manifold_work &work, phys_simplex_manifold_point &cp,
    phys_simplex_manifold_convert_out &out)
{
    const __vector4 &vertex_a0 = work.get_vertex_a(cp.m_vert_ids[0]).get_quad();
    const __vector4 &vertex_a1 = work.get_vertex_a(cp.m_vert_ids[1]).get_quad();
    const __vector4 &vertex_a2 = work.get_vertex_a(cp.m_vert_ids[2]).get_quad();
    const __vector4 &capsule_vertex_b = work.get_vertex_b(cp.m_vert_ids[3]).get_quad();

    out.m_position = capsule_vertex_b;
    out.separation_quad = __vsubfp(vertex_a0, capsule_vertex_b);

    __vector4 e0 = __vsubfp(vertex_a2, vertex_a1);
    __vector4 e1 = __vsubfp(vertex_a0, vertex_a2);
    __vector4 e2 = __vsubfp(vertex_a1, vertex_a0);

    __vector4 normal = phys_manifold_util_cross(e0, e1);
    phys_manifold_util_normalize3(normal);
    __vector4 diff_b_a1 = __vsubfp(capsule_vertex_b, vertex_a1);
    __vector4 diff_b_a2 = __vsubfp(capsule_vertex_b, vertex_a2);
    __vector4 diff_b_a0 = __vsubfp(capsule_vertex_b, vertex_a0);

    phys_vec4 dot_products;
    phys_simd_scalar d0(__vmsum3fp(phys_manifold_util_cross(diff_b_a1, e0), normal));
    dot_products(0) = d0;
    phys_simd_scalar d1(__vmsum3fp(phys_manifold_util_cross(diff_b_a2, e1), normal));
    dot_products(1) = d1;
    phys_simd_scalar d2(__vmsum3fp(phys_manifold_util_cross(diff_b_a0, e2), normal));
    dot_products(2) = d2;
    phys_simd_scalar d3(__vmsum3fp(work.m_master_normal.get_quad(), normal));
    dot_products(3) = d3;

    __vector4 dist = __vmsum3fp(out.separation_quad, normal);
    out.m_position = __vmaddfp(normal, dist, capsule_vertex_b);
    out.m_normal = normal;
    int mask = phys_manifold_util_get_mask(__vcmpgtfp(__vspltisw(0), dot_products.m_simd_quad));
    if (mask & 1)
        out.m_normal = __vxor(normal, g_phys_quad_sign_mask);
    return ~mask & 14;
}

// @address 0x698d28 ?phys_simplex_manifold_convert_edge_edge@@YAHABUphys_simplex_manifold_work@@AAUmanifold_contact@phys_simplex_manifold@@AAUphys_simplex_manifold_convert_out@@@Z
__forceinline int phys_simplex_manifold_convert_edge_edge(const phys_simplex_manifold_work &work, phys_simplex_manifold_point &cp,
    phys_simplex_manifold_convert_out &out)
{
    const __vector4 &vertex_a0 = work.get_vertex_a(cp.m_vert_ids[0]).get_quad();
    const __vector4 &vertex_a1 = work.get_vertex_a(cp.m_vert_ids[1]).get_quad();
    const __vector4 &vertex_b0 = work.get_vertex_b(cp.m_vert_ids[2]).get_quad();
    const __vector4 &vertex_b1 = work.get_vertex_b(cp.m_vert_ids[3]).get_quad();


    __vector4 edge_a = __vsubfp(vertex_a1, vertex_a0);
    __vector4 edge_b = __vsubfp(vertex_b1, vertex_b0);
    __vector4 diff_a0_b0 = __vsubfp(vertex_a0, vertex_b0);
    __vector4 diff_a1_b1 = __vsubfp(vertex_a1, vertex_b1);

    __vector4 normal = phys_manifold_util_cross(edge_a, edge_b);
    __vector4 plane_a = phys_manifold_util_cross(edge_a, normal);
    __vector4 plane_b = phys_manifold_util_cross(edge_b, normal);
    out.m_normal = work.m_master_normal.get_quad();

    phys_vec4 dot_products;
    phys_simd_scalar d0(__vmsum3fp(diff_a0_b0, plane_b));
    dot_products(0) = d0;
    phys_simd_scalar d1(__vmsum3fp(diff_a0_b0, plane_a));
    dot_products(1) = d1;
    phys_simd_scalar d2(__vmsum3fp(diff_a1_b1, plane_b));
    dot_products(2) = d2;
    phys_simd_scalar d3(__vmsum3fp(diff_a1_b1, plane_a));
    dot_products(3) = d3;

    __vector4 dots_quad = dot_products.m_simd_quad;
    int mask = phys_manifold_util_get_mask(__vcmpgtfp(__vspltisw(0), dots_quad));
    if (mask != 12 && mask != 3)
        return 1;

    phys_simd_scalar tA = phys_manifold_util_get_simd_at(dots_quad, 0) / (phys_manifold_util_get_simd_at(dots_quad,
        0) - phys_manifold_util_get_simd_at(dots_quad, 2));
    phys_manifold_util_set_lerp4(out.m_position, vertex_a0, vertex_a1, tA);
    phys_simd_scalar tB = phys_manifold_util_get_simd_at(dots_quad, 1) / (phys_manifold_util_get_simd_at(dots_quad,
        1) - phys_manifold_util_get_simd_at(dots_quad, 3));
    __vector4 capsule_point_b;
    phys_manifold_util_set_lerp4(capsule_point_b, vertex_b0, vertex_b1, tB);
    out.separation_quad = __vsubfp(out.m_position, capsule_point_b);
    return 0;
}

// @address 0x6993a0 ?phys_simplex_manifold_convert_edge_point@@YAHABUphys_simplex_manifold_work@@AAUmanifold_contact@phys_simplex_manifold@@AAUphys_simplex_manifold_convert_out@@@Z
__forceinline int phys_simplex_manifold_convert_edge_point(const phys_simplex_manifold_work &work, phys_simplex_manifold_point &cp,
    phys_simplex_manifold_convert_out &out)
{
    const __vector4 &vertex_a0 = work.get_vertex_a(cp.m_vert_ids[0]).get_quad();
    const __vector4 &vertex_a1 = work.get_vertex_a(cp.m_vert_ids[1]).get_quad();
    const __vector4 &capsule_vertex_b = work.get_vertex_b(cp.m_vert_ids[2]).get_quad();

    out.m_normal = work.m_master_normal.get_quad();

    __vector4 edge = __vsubfp(vertex_a1, vertex_a0);
    __vector4 diff_a1_b = __vsubfp(vertex_a1, capsule_vertex_b);
    __vector4 diff_b_a0 = __vsubfp(capsule_vertex_b, vertex_a0);

    __vector4 lhs_one_tmp = __vpermwi(edge, 0x63);
    __vector4 rhs_reg_op = __vpermwi(diff_a1_b, 0x87);
    __vector4 swiz_prod_lo = __vmulfp(lhs_one_tmp, rhs_reg_op);
    lhs_one_tmp = __vpermwi(edge, 0x87);
    rhs_reg_op = __vpermwi(diff_a1_b, 0x63);
    __vector4 vec_mul_out = __vmulfp(lhs_one_tmp, rhs_reg_op);
    __vector4 cross = __vsubfp(swiz_prod_lo, vec_mul_out);

    phys_vec4 plane;
    phys_simd_scalar d0(__vmsum3fp(diff_a1_b, edge));
    plane(0) = d0;
    phys_simd_scalar d1(__vmsum3fp(diff_b_a0, edge));
    plane(1) = d1;
    phys_simd_scalar d2(__vmsum3fp(cross, cross));
    plane(2) = d2;
    plane(3) = 0.0f;
    phys_simd_scalar d3(__vmsum3fp(edge, edge));
    float edge_len_sq = d3;

    __vector4 negative = __vcmpgtfp(__vspltisw(0), plane.m_simd_quad);
    unsigned int cr6_bits;
    __vcmpequwR(__vand(negative, phys_vec4_cmp::s_maskFromBits[phys_vec4_cmp::CMP_MASK_XY]), __vspltisw(0), &cr6_bits);
    int any_negative = ~cr6_bits & 0x80;

    if (plane(3) > work.m_radius_sum_sq * edge_len_sq || any_negative)
        return 1;

    __vector4 x = __vspltw(plane.m_simd_quad, 0);
    __vector4 sum = __vaddfp(x, __vspltw(plane.m_simd_quad, 1));
    __vector4 one = phys_simd_vec_1111;
    __vector4 r = __vrefp(sum);
    __vector4 q = __vspltw(__vmulfp(x, __vmaddfp(__vnmsubfp(r, sum, one), r, r)), 0);
    __vector4 point = __vmaddfp(q, vertex_a0, __vmulfp(__vsubfp(one, q), vertex_a1));
    out.m_position = point;
    out.separation_quad = __vsubfp(point, capsule_vertex_b);
    return 0;
}

// @address 0x699510 ?phys_simplex_manifold_convert_feature@@YAHABUphys_simplex_manifold_work@@AAUmanifold_contact@phys_simplex_manifold@@AAUphys_simplex_manifold_convert_out@@@Z
__forceinline int phys_simplex_manifold_convert_feature(const phys_simplex_manifold_work &work, phys_simplex_manifold_point &cp,
    phys_simplex_manifold_convert_out &out)
{
    switch (cp.m_dim_a * 4 + cp.m_dim_b) {
    case 0:
        return 1;
    default:
        return 1;
    case 5:
        return phys_simplex_manifold_convert_point_point(work, cp, out);
    case 6:
        return phys_simplex_manifold_convert_point_edge(work, cp, out);
    case 7:
        return phys_simplex_manifold_convert_point_face(work, cp, out);
    case 9:
        return phys_simplex_manifold_convert_edge_point(work, cp, out);
    case 10:
        return phys_simplex_manifold_convert_edge_edge(work, cp, out);
    case 13:
        return phys_simplex_manifold_convert_face_point(work, cp, out);
    }
}

// @address 0x699f08 ?phys_simplex_manifold_compact@@YAXAAUphys_simplex_manifold@@@Z
void phys_simplex_manifold_compact(phys_simplex_manifold &box_manifold)
{
    union {
        unsigned char bytes[16];
        __vector4 quad;
    } used;
#if defined(_XBOX) && !defined(__clang__)
    used.quad = __vxor(used.quad, used.quad);
#else
    used.quad = __vzero();
#endif

    for (unsigned int i = 0; i < box_manifold.m_contact_point_count; i++) {
        const phys_simplex_manifold_point &cp = box_manifold.m_process_contact_points[i];
        int vertex_cnt = cp.m_dim_a + cp.m_dim_b;
        used.bytes[cp.m_vert_ids[0] >> 4] = 1;
        used.bytes[cp.m_vert_ids[1] >> 4] = 1;
        if (vertex_cnt >= 3)
            used.bytes[cp.m_vert_ids[2] >> 4] = 1;
        if (vertex_cnt == 4)
            used.bytes[cp.m_vert_ids[3] >> 4] = 1;
    }

    unsigned int remap[16];
    int i = 0;
    unsigned short *vertex_ids = (unsigned short *)&box_manifold.m_process_contact_points[box_manifold.m_contact_point_count];
    int new_vertex_cnt = 0;
    unsigned int new_vert_offset = 0;
    int total_vertex_cnt = box_manifold.m_vert_count_a + box_manifold.m_vert_count_b;
    for (; i < total_vertex_cnt; i++) {
        remap[i] = new_vert_offset;
        if (used.bytes[i] != 0) {
            vertex_ids[new_vertex_cnt] = vertex_ids[i];
            new_vertex_cnt++;
            new_vert_offset += 16;
        }
    }

    box_manifold.m_vert_count_a = (unsigned char)(remap[box_manifold.m_vert_count_a] >> 4);
    box_manifold.m_vert_count_b = (unsigned char)(new_vertex_cnt - box_manifold.m_vert_count_a);

    for (unsigned int i = 0; i < box_manifold.m_contact_point_count; i++) {
        phys_simplex_manifold_point &cp = box_manifold.m_process_contact_points[i];
        cp.m_vert_ids[0] = (unsigned char)remap[cp.m_vert_ids[0] >> 4];
        cp.m_vert_ids[1] = (unsigned char)remap[cp.m_vert_ids[1] >> 4];
        cp.m_vert_ids[2] = (unsigned char)remap[cp.m_vert_ids[2] >> 4];
        cp.m_vert_ids[3] = (unsigned char)remap[cp.m_vert_ids[3] >> 4];
    }
}

// @address 0x69a0c8 phys_simplex_manifold_remove_point
extern "C" void phys_simplex_manifold_remove_point(phys_simplex_manifold *box_manifold, int index)
{
    box_manifold->m_process_contact_points[index] = box_manifold->m_process_contact_points[--box_manifold->m_contact_point_count];

    unsigned int *dst_vertex = (unsigned int *)((char *)&box_manifold->m_process_contact_points[0] + _rotl(box_manifold->m_contact_point_count, 3));
    unsigned int *src_vertex = (unsigned int *)((char *)&box_manifold->m_process_contact_points[1] + _rotl(box_manifold->m_contact_point_count, 3));
    for (int i = (box_manifold->m_vert_count_b + box_manifold->m_vert_count_a - 1) >> 1; i >= 0; i--) {
        *dst_vertex++ = *src_vertex++;
    }

    phys_simplex_manifold_compact(*box_manifold);
}


static __forceinline void phys_simplex_manifold_shift_verts(phys_simplex_manifold &box_manifold, int old_point_cnt, int new_point_cnt)
{
    unsigned int *dst = (unsigned int *)&box_manifold.m_process_contact_points[new_point_cnt];
    unsigned int *src = (unsigned int *)&box_manifold.m_process_contact_points[old_point_cnt];
    int hull_vert_count = box_manifold.m_vert_count_a + box_manifold.m_vert_count_b;
    for (int i = (hull_vert_count - 1) >> 1; i >= 0; i--) {
        *dst++ = *src++;
    }
}

// @address 0x69a160 phys_simplex_manifold_verify_get_points
extern "C" void phys_simplex_manifold_verify_get_points(phys_simplex_manifold &box_manifold, const phys_simplex_manifold_work &work,
    unsigned int first_point_idx, phys_process_collision_output &out, phys_contact_manager *agent_contact_manager)
{
    unsigned int contact_reserve_count = box_manifold.m_contact_point_count;
    for (unsigned int i = first_point_idx; i < box_manifold.m_contact_point_count; i++) {
        phys_process_hit_point *contact_hit = out.m_next_free_contact;
        phys_simplex_manifold_point &cp = box_manifold.m_process_contact_points[i];
        phys_simplex_manifold_convert_out convert_out;
        int result = phys_simplex_manifold_convert_feature(work, cp, convert_out);
        if (result == 0) {
            const __vector4 &normal = convert_out.m_normal;
            phys_simd_scalar dot;
            dot.m_simd_scalar = __vmsum3fp(normal, convert_out.separation_quad);
            float dist = dot.operator float() - work.m_radius_b;
            float neg_dist = -dist;
            dist = dist - work.m_radius_a;
            if (dist < work.m_keep_contact) {
                __vector4 s;
                s = __lvlx(&neg_dist, 0);
                s = __vspltw(s, 0);
                contact_hit->m_hit_contact.m_position.m_simd_quad = __vmaddfp(normal, s, convert_out.m_position);
                contact_hit->m_hit_contact.m_separating_normal.m_simd_quad = normal;
                contact_hit->m_hit_contact.contact_set_distance(dist);
                contact_hit->m_process_contact_id = cp.m_handle_id;
                out.m_next_free_contact = out.m_next_free_contact + 1;
                continue;
            }
        }
        if (cp.m_handle_id != 0xffff)
            agent_contact_manager->contact_remove_point(cp.m_handle_id, *out.m_constraint_owner);
        box_manifold.m_contact_point_count--;
        cp = box_manifold.m_process_contact_points[box_manifold.m_contact_point_count];
        i--;
    }
    if (contact_reserve_count > box_manifold.m_contact_point_count) {
        phys_simplex_manifold_shift_verts(box_manifold, contact_reserve_count, box_manifold.m_contact_point_count);
        phys_simplex_manifold_compact(box_manifold);
    }
}

// @address 0x69ac20 ?phys_simplex_manifold_find_redundant_5th@@YAHPAUphys_process_hit_point@@@Z
int phys_simplex_manifold_find_redundant_5th(struct phys_process_hit_point *points)
{
    const int contact_reserve_count = 5;
    int used[5];
    used[0] = 0;
    used[1] = 0;
    used[2] = 0;
    used[3] = 0;
    used[4] = 0;

    float dist[4];
    int idx_a = 0;
    int idx_min = 0;
    float max_first_diag_dist = 0.0f;
    float manifold_min_distance = 3.40282e+38f;
    {
        const phys_vec4 &ref = points[contact_reserve_count - 1].m_hit_contact.get_contact_position();
        for (int i = 0; i < contact_reserve_count - 1; i++) {
            const phys_vec4 &contact_pos = points[i].m_hit_contact.get_contact_position();
            phys_vec4 diff; diff.set_sub4(ref, contact_pos);
            const float d = diff.dot3(diff);
            dist[i] = d;
            if (d > max_first_diag_dist) {
                max_first_diag_dist = d;
                idx_a = i;
            }
            if (d < manifold_min_distance) {
                manifold_min_distance = d;
                idx_min = i;
            }
        }
        used[idx_a] = 1;
    }

    const float last_point_dist = points[contact_reserve_count - 1].m_hit_contact.get_distance();
    const float dw = last_point_dist - points[idx_min].m_hit_contact.get_distance();

    int idx_b = 4;
    {
        max_first_diag_dist *= 1.05f;
        const phys_vec4 &ref = points[idx_a].m_hit_contact.get_contact_position();
        for (int i = 0; i < contact_reserve_count; i++) {
            if (used[i]) {
                continue;
            }
            const phys_vec4 &contact_pos = points[i].m_hit_contact.get_contact_position();
            phys_vec4 diff; diff.set_sub4(ref, contact_pos);
            const float d = diff.dot3(diff);
            if (d > max_first_diag_dist) {
                max_first_diag_dist = d;
                idx_b = i;
            }
        }
        used[idx_b] = 1;
    }

    int idx_c = 0;
    {
        const phys_vec4 *capsule_point_b = &points[idx_b].m_hit_contact.get_contact_position();
        phys_vec4 diag_term; diag_term.set_sub4(points[idx_a].m_hit_contact.get_contact_position(), *capsule_point_b);
        float manifold_max_dist = 0.0f;
        const __vector4 simplex_point_a = points[idx_a].m_hit_contact.get_contact_position().m_simd_quad;
        for (int i = 0; i < contact_reserve_count; i++) {
            if (used[i]) {
                continue;
            }
            const phys_vec4 &contact_pos = points[i].m_hit_contact.get_contact_position();
            phys_vec4 diff; diff.m_simd_quad = __vsubfp(contact_pos.m_simd_quad, simplex_point_a);
            phys_vec4 cross; cross.set_cross(diff, diag_term);
            const float d = cross.dot3(cross);
            if (d > manifold_max_dist) {
                manifold_max_dist = d;
                idx_c = i;
            }
        }
        used[idx_c] = 1;
    }

    int idx_d = 0;
    {
        float manifold_max_dist = 0.0f;
        const phys_vec4 &ref = points[idx_c].m_hit_contact.get_contact_position();
        for (int i = 0; i < contact_reserve_count; i++) {
            if (used[i]) {
                continue;
            }
            const phys_vec4 &contact_pos = points[i].m_hit_contact.get_contact_position();
            phys_vec4 diff; diff.set_sub4(ref, contact_pos);
            const float d = diff.dot3(diff);
            if (d > manifold_max_dist) {
                manifold_max_dist = d;
                idx_d = i;
            }
        }
        used[idx_d] = 1;
    }

    if (!used[4] && dw < 0.0f && manifold_min_distance < dw * dw * 526.0f) {
        int best = 4;
        float best_ratio = 0.030461743f;
        for (int i = 0; i < contact_reserve_count - 1; i++) {
            const float wd = points[i].m_hit_contact.get_distance() - last_point_dist;
            const float alloc_count = wd * wd + 1e-09f;
            if (alloc_count > dist[i] * best_ratio) {
                best = i;
                best_ratio = alloc_count / (dist[i] + 1e-09f);
            }
        }
        return best;
    }

    for (int i = 0; i < contact_reserve_count; i++) {
        if (!used[i]) {
            return i;
        }
    }
    return 0;
}

static __forceinline void phys_simplex_manifold_up_shift_verts(phys_simplex_manifold &box_manifold, int old_point_cnt, int new_point_cnt)
{
    unsigned int *dst = (unsigned int *)&box_manifold.m_process_contact_points[new_point_cnt];
    unsigned int *src = (unsigned int *)&box_manifold.m_process_contact_points[old_point_cnt];
    int hull_vert_count = box_manifold.m_vert_count_a + box_manifold.m_vert_count_b;
    int i = (hull_vert_count - 1) >> 1;
    dst += i;
    src += i;
    for (; i >= 0; i--) {
        *dst-- = *src--;
    }
}

// @address 0x69afb0 phys_simplex_manifold_add_point
extern "C" int phys_simplex_manifold_add_point(phys_simplex_manifold &box_manifold, const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b,
    const phys_process_collision_input &input, phys_process_collision_output &output, const phys_simplex_cache &manifold_new_point,
    phys_process_hit_point *new_hit_in_result_array, phys_process_hit_point *result_point_array,
        phys_contact_manager *pair_contact_manager, int contact_handling)
{
    unsigned short bounding_contact_point_id;
    int result = 4;

    if (box_manifold.m_contact_point_count == 4) {
        int redundant_point_idx = phys_simplex_manifold_find_redundant_5th(result_point_array);
        if (redundant_point_idx == 4) {
            return 5;
        }
        phys_simplex_manifold::manifold_contact &redundant_cp = box_manifold.m_process_contact_points[redundant_point_idx];
        pair_contact_manager->contact_remove_point(redundant_cp.m_handle_id, *output.m_constraint_owner);
        if (redundant_point_idx) {
            redundant_cp = box_manifold.m_process_contact_points[0];
        } else {
            redundant_cp = box_manifold.m_process_contact_points[1];
        }
        phys_simplex_manifold_compact(box_manifold);

        bounding_contact_point_id = pair_contact_manager->add_manifold_contact_point(agent_body_a, agent_body_b, input, output,
            &manifold_new_point, new_hit_in_result_array->m_hit_contact);
        if (bounding_contact_point_id == 0xffff) {
            box_manifold.m_process_contact_points[0] = box_manifold.m_process_contact_points[3];
            box_manifold.m_contact_point_count--;
            phys_simplex_manifold_shift_verts(box_manifold, box_manifold.m_contact_point_count + 1, box_manifold.m_contact_point_count);
            result_point_array[redundant_point_idx] = result_point_array[3];
            return 6;
        }
        new_hit_in_result_array->m_process_contact_id = bounding_contact_point_id;
        result_point_array[redundant_point_idx] = *new_hit_in_result_array;
        result = redundant_point_idx;
    } else {
        if (contact_handling == 0 || manifold_new_point.m_dim_b == 3) {
            bounding_contact_point_id = pair_contact_manager->add_manifold_contact_point(agent_body_a, agent_body_b, input, output,
                &manifold_new_point, new_hit_in_result_array->m_hit_contact);
            if (bounding_contact_point_id == 0xffff) {
                return 5;
            }
        } else {
            bounding_contact_point_id = 0xffff;
        }
        new_hit_in_result_array->m_process_contact_id = bounding_contact_point_id;
        phys_simplex_manifold_up_shift_verts(box_manifold, box_manifold.m_contact_point_count, box_manifold.m_contact_point_count + 1);
        box_manifold.m_process_contact_points[box_manifold.m_contact_point_count] = box_manifold.m_process_contact_points[0];
        box_manifold.m_contact_point_count++;
    }

    phys_simplex_manifold::manifold_contact &cp = box_manifold.m_process_contact_points[0];
    cp.m_dim_a = manifold_new_point.m_dim_a;
    cp.m_dim_b = manifold_new_point.m_dim_b;
    cp.m_all_verts = 0;
    cp.m_handle_id = bounding_contact_point_id;

    unsigned short *vertex_ids = (unsigned short *)&box_manifold.m_process_contact_points[box_manifold.m_contact_point_count];
    int capsule_vertex_offset = 0;

    for (int i = 0; i < manifold_new_point.m_dim_a; i++) {
        unsigned short point_vertex_id = manifold_new_point.m_vertices[i];
        unsigned int j;
        for (j = 0; ; j++) {
            if (j >= box_manifold.m_vert_count_a) {
                for (unsigned int k = box_manifold.m_vert_count_a + box_manifold.m_vert_count_b; k > j; k--) {
                    vertex_ids[k] = vertex_ids[k - 1];
                }
                vertex_ids[j] = point_vertex_id;
                box_manifold.m_vert_count_a++;
                capsule_vertex_offset += 16;
                break;
            }
            if (vertex_ids[j] == point_vertex_id) {
                break;
            }
        }
        cp.m_vert_ids[i] = (unsigned char)(j << 4);
    }

    if (capsule_vertex_offset) {
        phys_simplex_manifold::manifold_contact *p = &box_manifold.m_process_contact_points[1];
        for (unsigned int i = 1; i < box_manifold.m_contact_point_count; p++, i++) {
            for (unsigned int v = p->m_dim_a; v < (unsigned int)(p->m_dim_a + p->m_dim_b); v++) {
                p->m_vert_ids[v] = (unsigned char)(p->m_vert_ids[v] + capsule_vertex_offset);
            }
        }
    }

    for (int i = 0; i < manifold_new_point.m_dim_b; i++) {
        unsigned short point_vertex_id = manifold_new_point.m_vertices[manifold_new_point.m_dim_a + i];
        int end = box_manifold.m_vert_count_a + box_manifold.m_vert_count_b;
        int j;
        for (j = box_manifold.m_vert_count_a; ; j++) {
            if (j >= end) {
                vertex_ids[j] = point_vertex_id;
                box_manifold.m_vert_count_b++;
                break;
            }
            if (vertex_ids[j] == point_vertex_id) {
                break;
            }
        }
        cp.m_vert_ids[manifold_new_point.m_dim_a + i] = (unsigned char)(j << 4);
    }

    return result;
}
