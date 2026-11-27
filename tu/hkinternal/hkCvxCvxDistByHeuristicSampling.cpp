// hkinternal/hkCvxCvxDistByHeuristicSampling.obj: from the records 0x6b4be0 0x6b4d18
// @flags /TP /O2 /Oc
// @compiler xdk5632
#include <xtl.h>
#include "phys/phys_base.h"
#include "phys/phys_collide.h"
#include <vectorintrinsics.h>
#include "phys/phys_convex_dist_sampling.h"
#include "phys/phys_simplex_out.h"
#include "phys/phys_vec4_util.h"
extern "C" {
}
struct phys_convex_dist_sample {
    phys_hull_vertex m_point_a;
    phys_hull_vertex m_point_b;
    phys_vec4 m_normal;
};
// @address 0x6b4be0 ?phys_convex_dist_sample_step@@YAXPBVphys_convex_shape@@0ABVphys_transform@@ABVphys_vec4@@AAUphys_convex_dist_sample@@@Z
void phys_convex_dist_sample_step(const phys_convex_shape *penetration_shape_a, const phys_convex_shape *penetration_shape_b, const phys_transform &a_from_b,
    const phys_vec4 &direction, phys_convex_dist_sample &sample_out)
{
    phys_hull_vertex capsule_vertex_a;
    {
        phys_vec4 neg_dir;
        neg_dir.set_neg4(direction);
        penetration_shape_a->get_support_vertex_impl(neg_dir, capsule_vertex_a);
    }
    phys_hull_vertex capsule_vertex_b;
    {
        phys_vec4 dir_b;
        dir_b.set_rotated_inverse_dir(a_from_b.m_rotation, direction);
        penetration_shape_b->get_support_vertex_impl(dir_b, capsule_vertex_b);
    }
    phys_vec4 point_b_in_a;
    point_b_in_a.set_transformed_pos(a_from_b, capsule_vertex_b);
    phys_vec4 delta;
    delta.set_sub4(capsule_vertex_a, point_b_in_a);
    float distance;
    __stvewx(__vmsum3fp(delta.m_simd_quad, direction.m_simd_quad), &distance, 0);
    if (distance > sample_out.m_normal(3))
    {
        sample_out.m_normal = direction;
        sample_out.m_point_a = capsule_vertex_a;
        sample_out.m_point_b = capsule_vertex_b;
        sample_out.m_normal(3) = distance;
    }
}
// @address 0x6b4d18 phys_convex_dist_by_sampling
extern "C" void phys_convex_dist_by_sampling(const phys_convex_shape *penetration_shape_a,
    const phys_convex_shape *penetration_shape_b, const phys_transform &a_from_b,
    phys_hull_vertex *point_on_a_out, phys_hull_vertex *point_on_b_out, phys_simplex_out &out)
{
    phys_convex_dist_sample sample;
    sample.m_normal(3) = -3.40282e+38f;
    const phys_vec4 *axis = &phys_transform::get_identity().m_rotation.get_column(0);
    for (int i = 0; i < 3; i++)
    {
        phys_vec4 dir = *axis;
        axis++;
        phys_convex_dist_sample_step(penetration_shape_a, penetration_shape_b, a_from_b, dir, sample);
        dir.set_neg4(dir);
        phys_convex_dist_sample_step(penetration_shape_a, penetration_shape_b, a_from_b, dir, sample);
    }
    float float_epsilon = 1.0f;
    while (float_epsilon > 0.001f)
    {
        float_epsilon *= 0.99f;
        phys_vec4 perp;
        phys_vec4_util::compute_perpendicular_vec(sample.m_normal, perp);
        perp.normalize_xyz();
        phys_vec4 cross;
        cross.set_cross(sample.m_normal, perp);
        perp.mul4(phys_simd_scalar(float_epsilon));
        cross.mul4(phys_simd_scalar(float_epsilon));
        phys_vec4 d0; d0.set_add4(sample.m_normal, perp);
        phys_vec4 d1; d1.set_sub4(sample.m_normal, perp);
        phys_vec4 d2; d2.set_add4(sample.m_normal, cross);
        phys_vec4 d3; d3.set_sub4(sample.m_normal, cross);
        phys_simd_scalar inv = d0.inv_length_3d();
        d0.mul4(inv);
        d1.mul4(inv);
        d2.mul4(inv);
        d3.mul4(inv);
        phys_convex_dist_sample_step(penetration_shape_a, penetration_shape_b, a_from_b, d0, sample);
        phys_convex_dist_sample_step(penetration_shape_a, penetration_shape_b, a_from_b, d1, sample);
        phys_convex_dist_sample_step(penetration_shape_a, penetration_shape_b, a_from_b, d2, sample);
        phys_convex_dist_sample_step(penetration_shape_a, penetration_shape_b, a_from_b, d3, sample);
        float_epsilon *= 0.5f;
    }
    *point_on_a_out = sample.m_point_a;
    *point_on_b_out = sample.m_point_b;
    out.m_distance = sample.m_normal(3);
    out.m_normal_in_a = sample.m_normal;
    out.m_point_on_a = sample.m_point_a;
}
