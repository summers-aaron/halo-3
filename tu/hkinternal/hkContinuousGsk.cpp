// hkinternal/hkContinuousGsk.obj: from the records 0x69b478 0x69b558 0x69b678 0x69b7e0 0x69b920 0x69ba50 0x69bba0 0x69bd10 0x69bfe8 0x69cbb0
// @flags /TP /O2 /Oc
// @compiler xdk5632
// match: 0x69cbb0: hkVector4 arrays require trivial construction.
#define PHYS_DISABLE_VEC4_CONSTRUCTORS
#include <xtl.h>
#include <math.h>
#include <ppcintrinsics.h>
#include <vectorintrinsics.h>
#include "phys/phys_simplex.h"
#include "phys/phys_continuous_simplex.h"
#include "phys/phys_thread_memory.h"
#include "phys/phys_collision_proxy.h"
#include "phys/phys_math.h"
#include "phys/phys_collide.h"
#include "phys/phys_collision_input.h"
#include "phys/phys_simplex_types.h"
#include "phys/phys_swept_transform_util.h"
#include "phys/phys_vec4_util.h"


#include "phys/phys_base_types.h"
#include "phys/phys_step_info.h"
#include "phys/phys_object.h"
#include "phys/phys_constraint_owner.h"
#include "phys/phys_simplex_status.h"
struct phys_4d_simplex_tolerances
{
    phys_padded<float> m_toi_separation;
    phys_padded<float> m_min_separation;
    phys_padded<float> m_min_safe_delta_rt;
    phys_padded<float> m_min_toi_dt;
    phys_padded<float> m_toi_accuracy;
};
struct phys_4d_simplex_vertex_output
{
    __forceinline phys_4d_simplex_vertex_output() : m_toi_rt(1.0f) {}
    phys_padded<float> m_toi_rt;
    phys_vec4 m_colliding_point;
};
struct phys_4d_simplex_vertex_input
{
    phys_padded<const phys_motion_state *> m_motion_a;
    phys_padded<const phys_motion_state *> m_motion_b;
    phys_padded<phys_vec4 *> m_vertices_a;
    phys_padded<int> m_hull_vertex_count;
    phys_padded<int> m_allocated_vertex_count;
    phys_padded<float> m_neg_w;
    phys_padded<float> m_max_accel_bound;
    phys_padded<float> m_inv_max_accel;
    phys_vec4 m_plane_normal;
    phys_vec4 m_plane_point;
    phys_padded<const phys_step_info *> m_step_info;
    phys_padded<float> m_worst_approach_delta;
    phys_vec4 m_dir_50;
    phys_vec4 m_dirs_60[2];
    phys_padded<float> m_start_rt;
};
typedef char phys_4d_simplex_dir_offset[(sizeof(void *) != 4 || (((unsigned int)&((phys_4d_simplex_vertex_input *)0)->m_dir_50 == 0x50) )) ? 1 : -1];
struct phys_4d_simplex_collide_mats
{
    phys_transform m_world_from_a;
    phys_transform m_world_from_b;
    phys_transform m_a_to_b;
    phys_vec4 m_normal;
    phys_vec4 m_origin;
    phys_vec4 m_dir_e0;
    phys_vec4 m_dirs_f0[2];
    phys_vec4 m_com_a;
    phys_vec4 m_com_b;
};
// @address 0x69b478 ?phys_4d_simplex_worst_projected_vel@@YAXPBVphys_motion_state@@0ABVphys_vec4@@1PBV2@AAM3@Z
void phys_4d_simplex_worst_projected_vel(const phys_motion_state *motion_state_a, const phys_motion_state *motion_state_b, const phys_vec4 &normal,
    const phys_vec4 &keyframe_lin_vel, const phys_vec4 *ang_velocity, float &lin_projection, float &ang_projection)
{
    phys_vec4 toi_cross_a;
    toi_cross_a.set_cross(ang_velocity[0], normal);
    phys_vec4 toi_cross_b;
    toi_cross_b.set_cross(ang_velocity[1], normal);
    float pulley_len_a = toi_cross_a.length_3d();
    float proj_a = motion_state_a->m_object_radius * pulley_len_a;
    float pulley_len_b = toi_cross_b.length_3d();
    float proj_b = motion_state_b->m_object_radius * pulley_len_b;
    ang_projection = proj_b + proj_a;
    lin_projection = keyframe_lin_vel.dot3(normal);
}
// @address 0x69b558 ?phys_4d_simplex_point_vel@@YAMPBVphys_motion_state@@0ABVphys_vec4@@11PBV2@M@Z
__forceinline float phys_4d_simplex_point_vel(const phys_motion_state *motion_state_a, const phys_motion_state *motion_state_b, const phys_vec4 &point,
    const phys_vec4 &normal, const phys_vec4 &keyframe_lin_vel, const phys_vec4 *ang_velocity, float time)
{
    phys_vec4 center_a;
    phys_swept_transform_util::compute_center_of_mass_at(*motion_state_a, time, center_a);
    phys_vec4 center_b;
    phys_swept_transform_util::compute_center_of_mass_at(*motion_state_b, time, center_b);
    phys_vec4 arm_a;
    arm_a.set_sub4(point, center_a);
    phys_vec4 arm_b;
    arm_b.set_sub4(point, center_b);
    phys_vec4 velocity;
    velocity.set_neg4(keyframe_lin_vel);
    phys_vec4 toi_cross_a;
    toi_cross_a.set_cross(ang_velocity[0], arm_a);
    velocity.add4(toi_cross_a);
    phys_vec4 toi_cross_b;
    toi_cross_b.set_cross(ang_velocity[1], arm_b);
    velocity.sub4(toi_cross_b);
    return velocity.dot3(normal);
}
// @address 0x69b678 ?phys_4d_simplex_point_vel_ang_err@@YAMPBVphys_motion_state@@0ABVphys_vec4@@11PBV2@M@Z
float phys_4d_simplex_point_vel_ang_err(const phys_motion_state *motion_state_a, const phys_motion_state *motion_state_b, const phys_vec4 &point,
    const phys_vec4 &normal, const phys_vec4 &keyframe_lin_vel, const phys_vec4 *ang_velocity, float time)
{
    phys_vec4 velocity;
    velocity.set_neg4(keyframe_lin_vel);
    float contact_projected_vel = velocity.dot3(normal);
    {
        phys_vec4 center_a;
        phys_swept_transform_util::compute_center_of_mass_at(*motion_state_a, time, center_a);
        phys_vec4 arm;
        arm.set_sub4(point, center_a);
        phys_vec4 cross;
        cross.set_cross(ang_velocity[0], arm);
        float error = cross.dot3(normal);
        if (error <= 0.0f)
        {
            contact_projected_vel += error * 1.2f;
        }
        else
        {
            const float scale = 1.0f / 1.2f;
            contact_projected_vel += error * scale;
        }
    }
    {
        phys_vec4 center_b;
        phys_swept_transform_util::compute_center_of_mass_at(*motion_state_b, time, center_b);
        phys_vec4 arm;
        arm.set_sub4(point, center_b);
        phys_vec4 cross;
        cross.set_cross(ang_velocity[1], arm);
        float error = cross.dot3(normal);
        if (error >= 0.0f)
        {
            contact_projected_vel -= error * 1.2f;
        }
        else
        {
            contact_projected_vel -= error * (1.0f / 1.2f);
        }
    }
    return contact_projected_vel;
}
// @address 0x69b7e0 ?phys_4d_simplex_collide_init_matrices@@YAXABUphys_4d_simplex_vertex_input@@MAAUphys_4d_simplex_collide_mats@@@Z
void phys_4d_simplex_collide_init_matrices(const phys_4d_simplex_vertex_input &input, float time, phys_4d_simplex_collide_mats &m)
{
    const phys_step_info *step_info = input.m_step_info;
    float body_delta_time = time * step_info->m_delta_time;
    phys_swept_transform_util::lerp_swept_add_on(input.m_motion_a->get_swept_transform(), step_info->m_step_start_time, body_delta_time, m.m_world_from_a);
    phys_swept_transform_util::lerp_swept_add_on(input.m_motion_b->get_swept_transform(), step_info->m_step_start_time, body_delta_time, m.m_world_from_b);
    m.m_a_to_b.set_mul_inverse_mul(m.m_world_from_a, m.m_world_from_b);
    m.m_normal.vec_set_rotated_dir(m.m_a_to_b.get_rotation(), input.m_plane_normal);
    m.m_origin.vec_set_transformed_pos(m.m_a_to_b, input.m_plane_point);
    m.m_normal(3) = -input.m_neg_w;
    m.m_com_a = input.m_motion_a->get_swept_transform().m_center_of_mass_local;
    phys_vec4_util::rotate_inverse_points(m.m_world_from_a.get_rotation(), input.m_dirs_60, 2, m.m_dirs_f0);
    m.m_com_b.vec_set_transformed_pos(m.m_a_to_b, input.m_motion_b->get_swept_transform().m_center_of_mass_local);
    m.m_dir_e0.vec_set_rotated_inverse_dir(m.m_world_from_a.get_rotation(), input.m_dir_50);
}
// @address 0x69bba0 ?phys_4d_simplex_point_plane_final@@YAMABUphys_4d_simplex_vertex_input@@ABUphys_4d_simplex_tolerances@@ABVphys_vec4@@MMMMMAAUphys_4d_simplex_vertex_output@@@Z
float phys_4d_simplex_point_plane_final(struct phys_4d_simplex_vertex_input const &p0, struct phys_4d_simplex_tolerances const &p1,
    class phys_vec4 const &p2, float p3, float p4, float p5, float p6, float p7, struct phys_4d_simplex_vertex_output &p8)
{
    float &lo = p3;
    float &hi = p4;
    float sep_lo = p5;
    float sep_hi = p6;
    float separation_vec = p1.m_toi_separation;
    float float_epsilon = p1.m_toi_accuracy * 0.25f;
    float best = lo;

    int i = 0;
    do {
        if (!(sep_lo > separation_vec)) break;

        float box_denominator = sep_hi - sep_lo;
        if (box_denominator > -float_epsilon) break;

        float t = (separation_vec - sep_lo) / box_denominator;
        if ((i & 1) == 0) {
            t = phys_math::max_of_two(t, 0.1f);
            t = phys_math::min_of_two(t, 0.9f);
        }

        best = t * hi + (1.0f - t) * lo;

        phys_4d_simplex_collide_mats collide_mats;
        phys_4d_simplex_collide_init_matrices(p0, best, collide_mats);

        phys_vec4 diff;
        diff.set_sub4(p2, collide_mats.m_origin);
        float dist = collide_mats.m_normal.dot4_xyz1(diff);

        separation_vec = p1.m_toi_separation;
        if (phys_math::equal(dist, separation_vec, float_epsilon)) break;

        if (!(dist >= separation_vec)) {
            hi = best;
            sep_hi = dist;
        } else {
            lo = best;
            sep_lo = dist;
        }
        ++i;
    } while (i < 10);

    if (!(best >= p8.m_toi_rt)) {
        p8.m_toi_rt = best;
        p8.m_colliding_point = p2;
    }
    return p8.m_toi_rt;
}

// @address 0x69b920 ?phys_4d_simplex_separating_normal@@YAXABVphys_transform@@0PBVphys_convex_shape@@1AAVphys_simplex@@AAVphys_vec4@@3@Z
__forceinline void phys_4d_simplex_separating_normal(class phys_transform const &p0, class phys_transform const &p1,
    class phys_convex_shape const *p2, class phys_convex_shape const *p3, class phys_simplex &p4, class phys_vec4 &p5, class phys_vec4 &p6)
{
    phys_transform transform;
    transform.set_mul_inverse_mul(p0, p1);

    phys_vec4 closest;
    p4.get_closest_feature(p2, p3, transform, closest);

    p5.m_simd_quad = __vmaddfp(p0.get_rotation().get_column(2).m_simd_quad, __vspltw(closest.m_simd_quad, 2),
                          __vmaddfp(p0.get_rotation().get_column(1).m_simd_quad, __vspltw(closest.m_simd_quad, 1),
                                    __vmulfp(p0.get_rotation().get_column(0).m_simd_quad, __vspltw(closest.m_simd_quad, 0))));

    phys_vec4 support;
    p4.get_closest_point_a_in_a(closest, support);

    support.m_simd_quad = __vnmsubfp(closest.m_simd_quad, __vspltw(closest.m_simd_quad, 3), support.m_simd_quad);
    phys_vec4 delta;
    delta.set_sub4(support, transform.m_translation);
    p6.set_rotated_inverse_dir(transform.m_rotation, delta);

    p5(3) = closest(3) - p2->get_convex_radius() - p3->get_convex_radius();
}
// @address 0x69ba50 ?phys_4d_simplex_coll_input_init@@YAXABUphys_narrow_agent_process_input@@AAUphys_4d_simplex_vertex_input@@@Z
void phys_4d_simplex_coll_input_init(const phys_narrow_agent_process_input &input, phys_4d_simplex_vertex_input &out)
{
    const phys_convex_shape *penetration_shape_a = static_cast<const phys_convex_shape *>(input.m_body_a->get_shape());
    const phys_convex_shape *penetration_shape_b = static_cast<const phys_convex_shape *>(input.m_body_b->get_shape());
    const phys_motion_state *motion_state_a = input.m_body_a->get_motion_state();
    const phys_motion_state *motion_state_b = input.m_body_b->get_motion_state();

    out.m_motion_a = motion_state_a;
    out.m_motion_b = motion_state_b;
    out.m_neg_w = penetration_shape_a->get_convex_radius() + penetration_shape_b->get_convex_radius();
    out.m_dir_50 = input.m_lin_sweep_info;
    out.m_step_info = &input.m_input->m_step_info;

    int child_sphere_count = penetration_shape_a->get_collision_sphere_count_impl();
    out.m_allocated_vertex_count = child_sphere_count;
    out.m_hull_vertex_count = child_sphere_count;
    phys_vec4 *spheres = phys_alloc_stack<phys_vec4>(child_sphere_count);
    out.m_vertices_a = spheres;
    penetration_shape_a->get_collision_spheres_impl((phys_sphere *)spheres);

    out.m_max_accel_bound = motion_state_a->m_object_radius * out.m_dirs_60[0](3) * out.m_dirs_60[0](3) +
        motion_state_b->m_object_radius * out.m_dirs_60[1](3) * out.m_dirs_60[1](3);
    out.m_inv_max_accel = 1.0f / (out.m_max_accel_bound + 1.1920929e-07f);
}
namespace phys_algorithm
{
template <typename T> __forceinline void swap_16_bytes(T &x, T &y)
{
    __declspec(align(16)) T t = x;
    x = y;
    y = t;
}
}
// @address 0x69bd10 phys_4d_simplex_points_vs_plane
extern "C" void phys_4d_simplex_points_vs_plane(const phys_4d_simplex_vertex_input &input, const phys_4d_simplex_tolerances &hull_tol,
    phys_4d_simplex_vertex_output &out)
{
    float hit_toi = 1.0f;
    float current_rt = input.m_start_rt;
    float last_rt = current_rt;
    int hull_vertex_count = input.m_hull_vertex_count;
    float next_rt;
    int i;
    phys_vec4 *vertex;
    while (hull_vertex_count)
    {
        phys_4d_simplex_collide_mats m;
        phys_4d_simplex_collide_init_matrices(input, current_rt, m);
        vertex = input.m_vertices_a;
        phys_vec4 *dest = vertex;
        float delta_rt = hit_toi - current_rt;
        float worst_case = input.m_worst_approach_delta;
        float max_approach = delta_rt * worst_case;
        next_rt = hit_toi;
        i = hull_vertex_count;
        hull_vertex_count = 0;
        for (i--; i >= 0; vertex++, i--)
        {
            phys_vec4 rel_vec;
            rel_vec.set_sub4(*vertex, m.m_origin);
            float dist = m.m_normal.dot4_xyz1(rel_vec);
            if (dist > max_approach + hull_tol.m_toi_separation)
            {
                continue;
            }
            phys_vec4 arm_a;
            arm_a.set_sub4(*vertex, m.m_com_a);
            phys_vec4 arm_b;
            arm_b.set_sub4(*vertex, m.m_com_b);
            phys_vec4 toi_cross_a;
            toi_cross_a.set_cross(m.m_dirs_f0[0], arm_a);
            phys_vec4 toi_cross_b;
            toi_cross_b.set_cross(m.m_dirs_f0[1], arm_b);
            phys_vec4 rel_vel;
            rel_vel.set_sub4(toi_cross_a, m.m_dir_e0);
            rel_vel.sub4(toi_cross_b);
            float vel = rel_vel.dot3(m.m_normal);
            float dist_to_toi = dist - hull_tol.m_toi_separation;
            float toi_projected_dist = dist_to_toi + vel * delta_rt;
            float max_accel_dist = 0.5f * delta_rt * delta_rt * input.m_max_accel_bound;
            if (toi_projected_dist > max_accel_dist)
            {
                continue;
            }
            if (dist < hull_tol.m_toi_separation)
            {
                if (current_rt == input.m_start_rt)
                {
                    out.m_toi_rt = current_rt;
                    out.m_colliding_point = *vertex;
                    hit_toi = current_rt;
                }
                else
                {
                    float vertex_dist = (*vertex)(3);
                    hit_toi = phys_4d_simplex_point_plane_final(input, hull_tol, *vertex, last_rt, current_rt, vertex_dist, dist, vel, out);
                }
                continue;
            }
            phys_algorithm::swap_16_bytes(*vertex, *dest);
            (*dest)(3) = dist;
            dest++;
            hull_vertex_count++;
            float dist_to_min = dist - hull_tol.m_min_separation;
            float toi_to_min = hull_tol.m_toi_separation - hull_tol.m_min_separation;
            float t2_min = 2.0f * dist_to_min * input.m_inv_max_accel;
            if (t2_min > delta_rt * delta_rt)
            {
                continue;
            }
            float t2_toi = 2.0f * toi_to_min * input.m_inv_max_accel;
            float t_min = phys_math::sqrt(t2_min);
            if (vel >= 0.0f)
            {
                t_min += vel * input.m_inv_max_accel * 2.0f;
            }
            else
            {
                float t_stop = -vel * input.m_inv_max_accel;
                t_min = phys_math::max_of_two(t_stop, t_min);
            }
            if (t_min > delta_rt)
            {
                continue;
            }
            float t = t_min + phys_math::sqrt(t2_toi);
            t = phys_math::max_of_two(t, hull_tol.m_min_safe_delta_rt.val());
            next_rt = phys_math::min_of_two(next_rt, current_rt + t);
        }
        if (current_rt > hit_toi)
        {
            break;
        }
        if (current_rt == next_rt && current_rt >= hit_toi)
        {
            break;
        }
        last_rt = current_rt;
        current_rt = next_rt;
        next_rt = hit_toi;
    }
}
extern "C"
{
__forceinline void phys_simplex_recalc_contact(phys_simplex &gsk, const phys_vec4 &master_normal_a, phys_vec4 &point_b_in_a_out, phys_vec4 &support_out)
{
    phys_simplex_recalc_contact_internal(gsk.m_vertices_a, gsk.m_vertices_b_in_a, gsk.m_dim_a, gsk.m_dim_b, master_normal_a, point_b_in_a_out, support_out);
}
}
static __forceinline void phys_continuous_simplex_worst_projected_vel(const phys_motion_state *motion_state_a, const phys_motion_state *motion_state_b,
    const phys_vec4 &normal, const phys_vec4 &keyframe_lin_vel, const phys_vec4 *ang_velocity, float &lin_projection,
    float &ang_projection)
{
    phys_vec4 toi_cross_a;
    toi_cross_a.set_cross(ang_velocity[0], normal);
    phys_vec4 toi_cross_b;
    toi_cross_b.set_cross(ang_velocity[1], normal);
    float proj_a = static_cast<float>(toi_cross_a.length_3d()) * motion_state_a->m_object_radius;
    float proj_b = static_cast<float>(toi_cross_b.length_3d()) * motion_state_b->m_object_radius;
    ang_projection = proj_a + proj_b;
    lin_projection = keyframe_lin_vel.dot3(normal);
}
static __forceinline void phys_continuous_simplex_coll_input_init(const phys_narrow_agent_process_input &input, phys_4d_simplex_vertex_input &out)
{
    float max_accel = 0.0f;
    const phys_convex_shape *penetration_shape_a = static_cast<const phys_convex_shape *>(input.m_body_a->get_shape());
    const phys_convex_shape *penetration_shape_b = static_cast<const phys_convex_shape *>(input.m_body_b->get_shape());
    const phys_motion_state *motion_state_a = input.m_body_a->get_motion_state();
    const phys_motion_state *motion_state_b = input.m_body_b->get_motion_state();

    out.m_motion_a = motion_state_a;
    out.m_motion_b = motion_state_b;
    out.m_neg_w = penetration_shape_a->get_convex_radius() + penetration_shape_b->get_convex_radius();
    out.m_dir_50 = input.m_lin_sweep_info;
    out.m_step_info = &input.m_input->m_step_info;

    int child_sphere_count = penetration_shape_a->get_collision_sphere_count_impl();
    out.m_allocated_vertex_count = child_sphere_count;
    out.m_hull_vertex_count = child_sphere_count;
    phys_vec4 *spheres = phys_alloc_stack<phys_vec4>(child_sphere_count);
    out.m_vertices_a = spheres;
    penetration_shape_a->get_collision_spheres_impl((phys_sphere *)spheres);

    max_accel += motion_state_a->m_object_radius * out.m_dirs_60[0](3) * out.m_dirs_60[0](3);
    float max_accel_b = motion_state_b->m_object_radius * out.m_dirs_60[1](3) * out.m_dirs_60[1](3);
    max_accel += max_accel_b;
    out.m_max_accel_bound = max_accel;
    out.m_inv_max_accel = 1.0f / (out.m_max_accel_bound + 1.1920929e-07f);
}
static __forceinline void phys_continuous_simplex_toi_final(const phys_narrow_agent_process_input &input,
    phys_4d_simplex_tolerances &hull_tol, phys_simplex &gsk,
    int need_closest_feature, phys_vec4 sep_normal_ws, const phys_vec4 *ang_velocity, float rt_lo, float rt_hi, float sep_at_rt0,
    float sep_at_rt1, phys_process_collision_output &output)
{
    float interp_rt = rt_lo;
    float separation_vec = sep_at_rt0;
    phys_transform world_from_a;
    phys_transform world_from_b;
    phys_transform a_from_b;
    const phys_convex_shape *penetration_shape_a = (const phys_convex_shape *)input.m_body_a->get_shape();
    const phys_convex_shape *penetration_shape_b = (const phys_convex_shape *)input.m_body_b->get_shape();
    float capsule_radius_sum = penetration_shape_a->get_convex_radius() + penetration_shape_b->get_convex_radius();
    float inv_slope = (sep_at_rt1 - sep_at_rt0) / (rt_hi - rt_lo);
    phys_vec4 simplex_separating_normal;
    phys_vec4 simplex_point_a;

    for (int i = 0; i < 10; i++)
    {
        float sep_delta = sep_at_rt1 - sep_at_rt0;
        if (sep_delta > -hull_tol.m_toi_accuracy)
        {
            rt_hi = rt_lo;
            sep_delta = 0.5f;
        }
        phys_bool is_final;
        if (phys_math::equal(sep_at_rt1, hull_tol.m_toi_separation, hull_tol.m_toi_accuracy))
        {
            is_final = true;
            need_closest_feature = 0;
            interp_rt = rt_hi;
        }
        else
        {
            is_final = false;
            float t = (hull_tol.m_toi_separation - sep_at_rt0) / sep_delta;
            t = phys_math::max_of_two(t, 0.1f);
            t = phys_math::min_of_two(t, 0.9f);
            interp_rt = rt_lo * (1.0f - t) + rt_hi * t;
        }
        {
            const phys_step_info &step_info = input.m_input->m_step_info;
            float body_delta_time = interp_rt * step_info.m_delta_time;
            phys_swept_transform_util::lerp_swept_add_on((input.m_body_a->get_motion_state())->get_swept_transform(),
                step_info.m_step_start_time, body_delta_time, world_from_a);
            phys_swept_transform_util::lerp_swept_add_on((input.m_body_b->get_motion_state())->get_swept_transform(),
                step_info.m_step_start_time, body_delta_time, world_from_b);
            a_from_b.set_mul_inverse_mul(world_from_a, world_from_b);
        }
        if (need_closest_feature)
        {
            const phys_convex_shape *sA = (const phys_convex_shape *)input.m_body_a->get_shape();
            const phys_convex_shape *sB = (const phys_convex_shape *)input.m_body_b->get_shape();
            gsk.get_closest_feature(sA, sB, a_from_b, simplex_separating_normal);
            gsk.get_closest_point_a_in_a(simplex_separating_normal, simplex_point_a);
        }
        else
        {
            phys_vec4_util::transform_points(a_from_b, gsk.m_vertices_b_in_b, gsk.m_dim_b, gsk.m_vertices_b_in_a);
            phys_vec4 master_normal_a;
            master_normal_a.set_rotated_inverse_dir(world_from_a.get_rotation(), sep_normal_ws);
            phys_simplex_recalc_contact(gsk, master_normal_a, simplex_point_a, simplex_separating_normal);
        }
        separation_vec = simplex_separating_normal(3) - capsule_radius_sum;
        if (phys_math::equal(separation_vec, hull_tol.m_toi_separation, hull_tol.m_toi_accuracy) || is_final)
        {
            break;
        }
        if (rt_lo == rt_hi)
        {
            break;
        }
        if (separation_vec < hull_tol.m_toi_separation)
        {
            rt_hi = interp_rt;
            sep_at_rt1 = separation_vec;
        }
        else
        {
            rt_lo = interp_rt;
            sep_at_rt0 = separation_vec;
        }
    }

    const phys_step_info &step_info = input.m_input->m_step_info;
    float delta_rt = phys_math::max_of_two(interp_rt * step_info.m_delta_time.val(), hull_tol.m_min_toi_dt.val());
    float hit_toi = step_info.m_step_start_time + delta_rt;
    if (hit_toi >= output.m_collision_toi.m_time)
    {
        return;
    }
    if (hit_toi >= step_info.m_end_time - hull_tol.m_min_toi_dt)
    {
        return;
    }

    const phys_convex_shape *convex_a = (const phys_convex_shape *)input.m_body_a->get_shape();
    phys_contact_point contact;
    phys_vec4 point_a_ws;
    point_a_ws.vec_set_transformed_pos(world_from_a, simplex_point_a);
    contact.get_separating_normal().vec_set_rotated_dir(world_from_a.get_rotation(), simplex_separating_normal);
    phys_simd_scalar dist = -convex_a->get_convex_radius() - separation_vec;
    contact.get_contact_position().set_add_mul4(point_a_ws, contact.get_separating_normal(), dist);
    contact.contact_set_distance(separation_vec);
    int simplex_weld_result = phys_convex_shape::WELD_ACCEPT_UNMODIFIED;
    phys_vec4 unwelded_normal = contact.get_separating_normal();
    unsigned char feature_point_count;
    unsigned short feature_points[3];
    if (input.m_body_b->get_shape()->get_type_id() == 5)
    {
        feature_point_count = (unsigned char)gsk.m_dim_b;
        feature_points[0] = (unsigned short)gsk.m_vertices_b_in_b[0].get_packed_id();
        feature_points[1] = (gsk.m_dim_b < 2) ? 0xffff : (unsigned short)gsk.m_vertices_b_in_b[1].get_packed_id();
        feature_points[2] = (gsk.m_dim_b < 3) ? 0xffff : (unsigned short)gsk.m_vertices_b_in_b[2].get_packed_id();
        const phys_convex_shape *weld_convex = (const phys_convex_shape *)input.m_body_b->get_shape();
        simplex_weld_result = weld_convex->weld_contact_point(feature_points, feature_point_count, contact.get_contact_position(),
            &world_from_b, convex_a, &world_from_a, unwelded_normal);
        contact.set_separating_normal(unwelded_normal);
        if (simplex_weld_result == phys_convex_shape::WELD_REJECT_POINT)
        {
            if (input.m_input->m_quality_info->m_toi_weld_reject)
            {
                return;
            }
        }
    }

    const phys_motion_state *motion_state_a = input.m_body_a->get_motion_state();
    const phys_motion_state *motion_state_b = input.m_body_b->get_motion_state();
    float contact_projected_vel = phys_4d_simplex_point_vel(
        motion_state_a, motion_state_b, contact.get_contact_position(), contact.get_contact_normal(), input.m_lin_sweep_info, ang_velocity, hit_toi);
    if (simplex_weld_result == phys_convex_shape::WELD_ACCEPT_MODIFIED && contact_projected_vel > -1.1920929e-07f)
    {
        phys_4d_simplex_vertex_input collide_input;
        collide_input.m_start_rt = interp_rt;
        phys_swept_transform_util::compute_ang_sweep_info(
            *motion_state_a, *motion_state_b, input.m_input->m_step_info.m_delta_time, &collide_input.m_dirs_60[0], &collide_input.m_dirs_60[1]);
        float lin_projection;
        float ang_projection;
        phys_continuous_simplex_worst_projected_vel(
            motion_state_a, motion_state_b, contact.get_contact_normal(), input.m_lin_sweep_info, collide_input.m_dirs_60, lin_projection, ang_projection);
        collide_input.m_worst_approach_delta = lin_projection + ang_projection;
        phys_continuous_simplex_coll_input_init(input, collide_input);
        collide_input.m_plane_normal.vec_set_rotated_inverse_dir(world_from_b.get_rotation(), contact.get_contact_normal());
        collide_input.m_plane_point = gsk.m_vertices_b_in_b[0];
        phys_4d_simplex_vertex_output collide_output;
        phys_4d_simplex_points_vs_plane(collide_input, hull_tol, collide_output);
        phys_free_stack(collide_input.m_vertices_a.val());
        if (collide_output.m_toi_rt < 1.0f)
        {
            phys_vec4 manifold_new_point;
            manifold_new_point.vec_set_transformed_pos(world_from_a, collide_output.m_colliding_point);
            float new_point_vel = phys_4d_simplex_point_vel(motion_state_a, motion_state_b, manifold_new_point,
                contact.get_contact_normal(), input.m_lin_sweep_info, ang_velocity, hit_toi);
            new_point_vel = contact_projected_vel;
            contact.get_contact_position() = manifold_new_point;
        }
    }

    if (inv_slope < contact_projected_vel)
    {
        if (inv_slope > contact_projected_vel * 1.2f)
        {
            contact_projected_vel = inv_slope;
        }
        else
        {
            contact_projected_vel = phys_4d_simplex_point_vel_ang_err(input.m_body_a->get_motion_state(),
                input.m_body_b->get_motion_state(), contact.get_contact_position(), contact.get_contact_normal(),
                    input.m_lin_sweep_info, ang_velocity, hit_toi);
            contact_projected_vel = phys_math::min_of_two(0.0f, contact_projected_vel);
        }
    }
    contact_projected_vel *= step_info.m_inv_dt;

    phys_contact_point_material16 material;
    material.set_user_data(0);
    phys_simplex_cache16 cache;
    gsk.exit_and_export_cache(cache);
    phys_contact_manager::toi_verdict toi_accept = input.m_contact_manager->add_toi(
        *input.m_body_a, *input.m_body_b, *input.m_input, output, hit_toi, contact, &cache, contact_projected_vel, material);
    if (toi_accept == phys_contact_manager::TOI_EVENT_ACCEPT)
    {
        if (output.has_toi())
        {
            input.m_contact_manager->remove_toi(*input.m_body_a->get_root_collider(), *input.m_body_b->get_root_collider(),
                *output.m_constraint_owner, output.m_collision_toi.m_properties);
        }
        output.m_collision_toi.m_separating_speed = contact_projected_vel;
        output.m_collision_toi.m_contact_point = contact;
        output.m_collision_toi.m_time = hit_toi;
        output.m_collision_toi.m_properties.set(material);
        output.m_collision_toi.m_simplex_cache.set(cache);
    }
}
// @address 0x69bfe8 ?phys_4d_simplex_toi_final@@YAXABUphys_narrow_agent_process_input@@AAUphys_4d_simplex_tolerances@@AAVphys_simplex@@HABVphys_vec4@@PBV4@MMMMAAUphys_process_collision_output@@@Z
void phys_4d_simplex_toi_final(const phys_narrow_agent_process_input &input, phys_4d_simplex_tolerances &hull_tol, phys_simplex &gsk, int need_closest_feature,
    const phys_vec4 &sep_normal_ws, const phys_vec4 *ang_velocity, float rt_lo, float rt_hi, float sep_at_rt0, float sep_at_rt1,
    phys_process_collision_output &output)
{
    float interp_rt = rt_lo;
    float separation_vec = sep_at_rt0;
    phys_transform world_from_a;
    phys_transform world_from_b;
    phys_transform a_from_b;
    const phys_convex_shape *penetration_shape_a = (const phys_convex_shape *)input.m_body_a->get_shape();
    const phys_convex_shape *penetration_shape_b = (const phys_convex_shape *)input.m_body_b->get_shape();
    float capsule_radius_sum = penetration_shape_a->get_convex_radius() + penetration_shape_b->get_convex_radius();
    float inv_slope = (sep_at_rt1 - sep_at_rt0) / (rt_hi - rt_lo);
    phys_vec4 simplex_separating_normal;
    phys_vec4 simplex_point_a;

    for (int i = 0; i < 10; i++)
    {
        float sep_delta = sep_at_rt1 - sep_at_rt0;
        if (sep_delta > -hull_tol.m_toi_accuracy)
        {
            rt_hi = rt_lo;
            sep_delta = 0.5f;
        }
        phys_bool is_final;
        if (phys_math::equal(sep_at_rt1, hull_tol.m_toi_separation, hull_tol.m_toi_accuracy))
        {
            is_final = true;
            need_closest_feature = 0;
            interp_rt = rt_hi;
        }
        else
        {
            is_final = false;
            float t = (hull_tol.m_toi_separation - sep_at_rt0) / sep_delta;
            t = phys_math::max_of_two(t, 0.1f);
            t = phys_math::min_of_two(t, 0.9f);
            interp_rt = rt_lo * (1.0f - t) + rt_hi * t;
        }
        {
            const phys_step_info &step_info = input.m_input->m_step_info;
            float body_delta_time = interp_rt * step_info.m_delta_time;
            phys_swept_transform_util::lerp_swept_add_on((input.m_body_a->get_motion_state())->get_swept_transform(),
                step_info.m_step_start_time, body_delta_time, world_from_a);
            phys_swept_transform_util::lerp_swept_add_on((input.m_body_b->get_motion_state())->get_swept_transform(),
                step_info.m_step_start_time, body_delta_time, world_from_b);
            a_from_b.set_mul_inverse_mul(world_from_a, world_from_b);
        }
        if (need_closest_feature)
        {
            const phys_convex_shape *sA = (const phys_convex_shape *)input.m_body_a->get_shape();
            const phys_convex_shape *sB = (const phys_convex_shape *)input.m_body_b->get_shape();
            gsk.get_closest_feature(sA, sB, a_from_b, simplex_separating_normal);
            gsk.get_closest_point_a_in_a(simplex_separating_normal, simplex_point_a);
        }
        else
        {
            phys_vec4_util::transform_points(a_from_b, gsk.m_vertices_b_in_b, gsk.m_dim_b, gsk.m_vertices_b_in_a);
            phys_vec4 master_normal_a;
            master_normal_a.set_rotated_inverse_dir(world_from_a.get_rotation(), sep_normal_ws);
            phys_simplex_recalc_contact(gsk, master_normal_a, simplex_point_a, simplex_separating_normal);
        }
        separation_vec = simplex_separating_normal(3) - capsule_radius_sum;
        if (phys_math::equal(separation_vec, hull_tol.m_toi_separation, hull_tol.m_toi_accuracy) || is_final)
        {
            break;
        }
        if (rt_lo == rt_hi)
        {
            break;
        }
        if (separation_vec < hull_tol.m_toi_separation)
        {
            rt_hi = interp_rt;
            sep_at_rt1 = separation_vec;
        }
        else
        {
            rt_lo = interp_rt;
            sep_at_rt0 = separation_vec;
        }
    }

    const phys_step_info &step_info = input.m_input->m_step_info;
    float delta_rt = phys_math::max_of_two(interp_rt * step_info.m_delta_time.val(), hull_tol.m_min_toi_dt.val());
    float hit_toi = step_info.m_step_start_time + delta_rt;
    if (hit_toi >= output.m_collision_toi.m_time)
    {
        return;
    }
    if (hit_toi >= step_info.m_end_time - hull_tol.m_min_toi_dt)
    {
        return;
    }

    const phys_convex_shape *convex_a = (const phys_convex_shape *)input.m_body_a->get_shape();
    phys_contact_point contact;
    phys_vec4 point_a_ws;
    point_a_ws.vec_set_transformed_pos(world_from_a, simplex_point_a);
    contact.get_separating_normal().vec_set_rotated_dir(world_from_a.get_rotation(), simplex_separating_normal);
    phys_simd_scalar dist = -convex_a->get_convex_radius() - separation_vec;
    contact.get_contact_position().set_add_mul4(point_a_ws, contact.get_separating_normal(), dist);
    contact.contact_set_distance(separation_vec);
    int simplex_weld_result = phys_convex_shape::WELD_ACCEPT_UNMODIFIED;
    phys_vec4 unwelded_normal = contact.get_separating_normal();
    unsigned char feature_point_count;
    unsigned short feature_points[3];
    if (input.m_body_b->get_shape()->get_type_id() == 5)
    {
        feature_point_count = (unsigned char)gsk.m_dim_b;
        feature_points[0] = (unsigned short)gsk.m_vertices_b_in_b[0].get_packed_id();
        feature_points[1] = (gsk.m_dim_b < 2) ? 0xffff : (unsigned short)gsk.m_vertices_b_in_b[1].get_packed_id();
        feature_points[2] = (gsk.m_dim_b < 3) ? 0xffff : (unsigned short)gsk.m_vertices_b_in_b[2].get_packed_id();
        const phys_convex_shape *weld_convex = (const phys_convex_shape *)input.m_body_b->get_shape();
        simplex_weld_result = weld_convex->weld_contact_point(feature_points, feature_point_count, contact.get_contact_position(),
            &world_from_b, convex_a, &world_from_a, unwelded_normal);
        contact.set_separating_normal(unwelded_normal);
        if (simplex_weld_result == phys_convex_shape::WELD_REJECT_POINT)
        {
            if (input.m_input->m_quality_info->m_toi_weld_reject)
            {
                return;
            }
        }
    }

    const phys_motion_state *motion_state_a = input.m_body_a->get_motion_state();
    const phys_motion_state *motion_state_b = input.m_body_b->get_motion_state();
    float contact_projected_vel = phys_4d_simplex_point_vel(
        motion_state_a, motion_state_b, contact.get_contact_position(), contact.get_contact_normal(), input.m_lin_sweep_info, ang_velocity, hit_toi);
    if (simplex_weld_result == phys_convex_shape::WELD_ACCEPT_MODIFIED && contact_projected_vel > -1.1920929e-07f)
    {
        phys_4d_simplex_vertex_input collide_input;
        collide_input.m_start_rt = interp_rt;
        phys_swept_transform_util::compute_ang_sweep_info(
            *motion_state_a, *motion_state_b, input.m_input->m_step_info.m_delta_time, &collide_input.m_dirs_60[0], &collide_input.m_dirs_60[1]);
        float lin_projection;
        float ang_projection;
        phys_continuous_simplex_worst_projected_vel(
            motion_state_a, motion_state_b, contact.get_contact_normal(), input.m_lin_sweep_info, collide_input.m_dirs_60, lin_projection, ang_projection);
        collide_input.m_worst_approach_delta = lin_projection + ang_projection;
        phys_continuous_simplex_coll_input_init(input, collide_input);
        collide_input.m_plane_normal.vec_set_rotated_inverse_dir(world_from_b.get_rotation(), contact.get_contact_normal());
        collide_input.m_plane_point = gsk.m_vertices_b_in_b[0];
        phys_4d_simplex_vertex_output collide_output;
        phys_4d_simplex_points_vs_plane(collide_input, hull_tol, collide_output);
        phys_free_stack(collide_input.m_vertices_a.val());
        if (collide_output.m_toi_rt < 1.0f)
        {
            phys_vec4 manifold_new_point;
            manifold_new_point.vec_set_transformed_pos(world_from_a, collide_output.m_colliding_point);
            float new_point_vel = phys_4d_simplex_point_vel(motion_state_a, motion_state_b, manifold_new_point,
                contact.get_contact_normal(), input.m_lin_sweep_info, ang_velocity, hit_toi);
            new_point_vel = contact_projected_vel;
            contact.get_contact_position() = manifold_new_point;
        }
    }

    if (inv_slope < contact_projected_vel)
    {
        if (inv_slope > contact_projected_vel * 1.2f)
        {
            contact_projected_vel = inv_slope;
        }
        else
        {
            contact_projected_vel = phys_4d_simplex_point_vel_ang_err(input.m_body_a->get_motion_state(),
                input.m_body_b->get_motion_state(), contact.get_contact_position(), contact.get_contact_normal(),
                    input.m_lin_sweep_info, ang_velocity, hit_toi);
            contact_projected_vel = phys_math::min_of_two(0.0f, contact_projected_vel);
        }
    }
    contact_projected_vel *= step_info.m_inv_dt;

    phys_contact_point_material16 material;
    material.set_user_data(0);
    phys_simplex_cache16 cache;
    gsk.exit_and_export_cache(cache);
    phys_contact_manager::toi_verdict toi_accept = input.m_contact_manager->add_toi(
        *input.m_body_a, *input.m_body_b, *input.m_input, output, hit_toi, contact, &cache, contact_projected_vel, material);
    if (toi_accept == phys_contact_manager::TOI_EVENT_ACCEPT)
    {
        if (output.has_toi())
        {
            input.m_contact_manager->remove_toi(*input.m_body_a->get_root_collider(), *input.m_body_b->get_root_collider(),
                *output.m_constraint_owner, output.m_collision_toi.m_properties);
        }
        output.m_collision_toi.m_separating_speed = contact_projected_vel;
        output.m_collision_toi.m_contact_point = contact;
        output.m_collision_toi.m_time = hit_toi;
        output.m_collision_toi.m_properties.set(material);
        output.m_collision_toi.m_simplex_cache.set(cache);
    }
}
// @address 0x69cbb0 phys_4d_simplex_collide_toi
extern "C" void phys_4d_simplex_collide_toi(const phys_narrow_agent_process_input &narrow_input, const float allowed_penetration_depth,
    const float min_separation,
    const float toi_separation, phys_simplex_cache &simplex_cache, phys_vec4 &simplex_separating_normal, phys_process_collision_output &output)
{
    phys_4d_simplex_tolerances hull_tol;
    const phys_collision_quality_info *qi = narrow_input.m_input->m_quality_info.val();
    hull_tol.m_min_separation = min_separation;
    hull_tol.m_toi_separation = toi_separation;
    hull_tol.m_toi_accuracy = qi->m_toi_accuracy * allowed_penetration_depth;
    hull_tol.m_min_safe_delta_rt = phys_math::max_of_two(qi->m_min_safe_dt * allowed_penetration_depth, qi->m_min_abs_safe_dt) *
        narrow_input.m_input->m_step_info.m_inv_dt;
    hull_tol.m_min_toi_dt = qi->m_min_toi_dt;

    const phys_convex_shape *penetration_shape_a = static_cast<const phys_convex_shape *>(narrow_input.m_body_a->get_shape());
    const phys_convex_shape *penetration_shape_b = static_cast<const phys_convex_shape *>(narrow_input.m_body_b->get_shape());
    const phys_motion_state *motion_state_a = narrow_input.m_body_a->get_motion_state();
    const phys_motion_state *motion_state_b = narrow_input.m_body_b->get_motion_state();

    phys_4d_simplex_vertex_input collide_input;
    collide_input.m_vertices_a = 0;
    phys_bool plane_normal_valid = false;
    phys_bool collide_points_failed = false;
    phys_swept_transform_util::compute_ang_sweep_info(*motion_state_a, *motion_state_b, narrow_input.m_input->m_step_info.m_delta_time,
        &collide_input.m_dirs_60[0], &collide_input.m_dirs_60[1]);

    int simplex_iter_count = 0;
    phys_simplex gsk;
    gsk.init(penetration_shape_a, penetration_shape_b, simplex_cache);

    phys_vec4 last_normal;
    last_normal.set_zero4();
    float last_sep = simplex_separating_normal(3);
    int need_closest_feature = 1;
    float current_rt = 0.0f;
    const float end_rt = 1.0f;
    float last_rt = 0.0f;

    while (1)
    {
        float distance = simplex_separating_normal(3);
        if (distance <= hull_tol.m_toi_separation + hull_tol.m_toi_accuracy)
        {
            phys_continuous_simplex_toi_final(narrow_input, hull_tol, gsk, need_closest_feature, last_normal, &collide_input.m_dirs_60[0], last_rt,
                current_rt, last_sep, distance, output);
            break;
        }
        float delta_rt = end_rt - current_rt;
        if (delta_rt <= 0.0f)
        {
            break;
        }
        float lin_projection;
        float ang_projection;
        phys_continuous_simplex_worst_projected_vel(
            motion_state_a, motion_state_b, simplex_separating_normal, narrow_input.m_lin_sweep_info, &collide_input.m_dirs_60[0],
                lin_projection, ang_projection);
        float worst_approach_delta = lin_projection + ang_projection;
        float dist_at_end = simplex_separating_normal(3) - worst_approach_delta * delta_rt;
        if (worst_approach_delta <= 0.0f || dist_at_end > hull_tol.m_min_separation)
        {
            break;
        }
        float allowed_dist = simplex_separating_normal(3) - hull_tol.m_min_separation;
        float safe_delta_rt = allowed_dist / worst_approach_delta;
        if (safe_delta_rt < 0.2f && !collide_points_failed && plane_normal_valid && lin_projection * 1.0f < ang_projection)
        {
            if (!collide_input.m_vertices_a)
            {
                phys_continuous_simplex_coll_input_init(narrow_input, collide_input);
            }
            collide_input.m_start_rt = current_rt;
            collide_input.m_worst_approach_delta = worst_approach_delta;
            phys_4d_simplex_vertex_output collide_output;
            phys_4d_simplex_points_vs_plane(collide_input, hull_tol, collide_output);
            float delta_rtoi = collide_output.m_toi_rt - current_rt;
            if (delta_rtoi < 2.0f * safe_delta_rt)
            {
                collide_points_failed = true;
            }
            safe_delta_rt = phys_math::max_of_two(safe_delta_rt, delta_rtoi);
        }
        if (current_rt + safe_delta_rt >= end_rt)
        {
            break;
        }
        last_rt = current_rt;
        safe_delta_rt = phys_math::max_of_two(hull_tol.m_min_safe_delta_rt.val(), safe_delta_rt);
        current_rt += safe_delta_rt;
        current_rt = phys_math::min_of_two(current_rt, end_rt);
        last_normal = simplex_separating_normal;
        last_sep = simplex_separating_normal(3);
        {
            const phys_step_info &step_info = narrow_input.m_input->m_step_info;
            float t = current_rt * step_info.m_delta_time;
            phys_transform world_from_a;
            phys_swept_transform_util::lerp_swept_add_on(motion_state_a->get_swept_transform(), step_info.m_step_start_time, t, world_from_a);
            phys_transform world_from_b;
            phys_swept_transform_util::lerp_swept_add_on(motion_state_b->get_swept_transform(), step_info.m_step_start_time, t, world_from_b);
            simplex_iter_count++;
            phys_4d_simplex_separating_normal(world_from_a, world_from_b, penetration_shape_a, penetration_shape_b, gsk,
                simplex_separating_normal, collide_input.m_plane_point);
            need_closest_feature = gsk.m_feature_changed;
            plane_normal_valid = true;
            collide_input.m_plane_normal.set_rotated_inverse_dir(world_from_b.get_rotation(), simplex_separating_normal);
        }
    }
    if (collide_input.m_vertices_a)
    {
        phys_free_stack(collide_input.m_vertices_a.val());
    }
    gsk.refresh_simplex_cache(simplex_cache);
}
