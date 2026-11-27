// hkcollide/hkTriangleUtil.obj: from the records 0x706e78 0x706fd0
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x706e78 ?ray_intersect@phys_triangle_util@@SA?AVphys_bool@@AAM00ABVphys_vec4@@1111V2@@Z
// @address 0x706fd0 ?is_degenerate@phys_triangle_util@@SA?AVphys_bool@@ABVphys_vec4@@00M@Z
#include "phys/phys_triangle_util.h"

phys_bool phys_triangle_util::ray_intersect(float &t, float &u, float &v, const phys_vec4 &origin, const phys_vec4 &direction,
    const phys_vec4 &a, const phys_vec4 &b, const phys_vec4 &c, phys_bool cull_backfaces)
{
    phys_vec4 edge_vec_ab;
    edge_vec_ab.set_sub4(b, a);
    phys_vec4 edge_vec_ac;
    edge_vec_ac.set_sub4(c, a);
    phys_vec4 p;
    p.set_cross(direction, edge_vec_ac);
    const float determinant = edge_vec_ab.dot3(p);
    if (cull_backfaces) {
        if (determinant < 0.0001f)
            return false;
    } else {
        if ((determinant > -0.0001f) && (determinant < 0.0001f))
            return false;
    }
    const float tri_inv_det = 1.0f / determinant;
    phys_vec4 offset;
    offset.set_sub4(origin, a);
    u = offset.dot3(p);
    if (u < 0.0f || u > determinant) {
        return false;
    }
    phys_vec4 q;
    q.set_cross(offset, edge_vec_ab);
    v = direction.dot3(q);
    if (v < 0.0f || (u + v) > determinant) {
        return false;
    }
    t = edge_vec_ac.dot3(q);
    t *= tri_inv_det;
    u *= tri_inv_det;
    v *= tri_inv_det;
    return true;
}

phys_bool phys_triangle_util::is_degenerate(const phys_vec4 &a, const phys_vec4 &b, const phys_vec4 &c, float tolerance)
{
    {
        phys_vec4 a_b_delta;
        a_b_delta.set_sub4(a, b);
        phys_vec4 a_c_delta;
        a_c_delta.set_sub4(a, c);
        phys_vec4 cross_vec_a;
        cross_vec_a.set_cross(a_b_delta, a_c_delta);
        phys_vec4 b_a_delta;
        b_a_delta.set_sub4(b, a);
        phys_vec4 b_c_delta;
        b_c_delta.set_sub4(b, c);
        phys_vec4 cross_vec_b;
        cross_vec_b.set_cross(b_a_delta, b_c_delta);
        if ((float(cross_vec_a.length_sq_3d()) < tolerance) || (float(cross_vec_b.length_sq_3d()) < tolerance)) {
            return true;
        }
    }
    {
        phys_vec4 q;
        q.set_sub4(a, b);
        phys_vec4 r;
        r.set_sub4(c, b);
        const float q_len_sq = q.length_sq_3d();
        const float rr = r.length_sq_3d();
        const float q_dot_r = r.dot3(q);
        float qq_times_rr = q_len_sq * rr;
        float q_dot_r_sq = q_dot_r * q_dot_r;
        float determinant = (qq_times_rr - q_dot_r_sq);
        if (determinant == 0.0f) {
            return true;
        }
    }
    return false;
}
