// hkdynamics/hkSimpleCollisionResponse.obj: from the records 0x6642d0 0x664658
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include <vectorintrinsics.h>
#include <ppcintrinsics.h>
#include <math.h>
#include "phys/phys_math.h"
#include "phys/phys_simple_constraint_util.h"
#include "phys/phys_collide.h"
#include "phys/phys_dynamics.h"
#include "phys/phys_simple_contact_data.h"
#include "phys/phys_simple_collision_response.h"

















class phys_class_desc;



#include "phys/phys_swept_transform_util.h"
#include "phys/phys_vec4_util.h"
#include "phys/phys_contact_point_props.h"
#include "phys/phys_statistics_collector.h"
#include "phys/phys_vel_accum.h"
#include "phys/phys_dynamics_contact_manager.h"








class phys_constraint;


namespace phys_vec4_util {

__forceinline void build_orthonormal_basis(const phys_vec4 &dir, phys_mat3 &out)
{
    out.get_column(0) = dir;
    compute_perpendicular_vec(dir, out.get_column(1));
    out.get_column(1).normalize_xyz();
    out.get_column(2).set_cross(dir, out.get_column(1));
}
}


// @address 0x6642d0 ?solve_single_contact2@phys_simple_collision_response@@SAXPAVphys_simple_contact_data@@ABVphys_contact_point@@ABUphys_simple_collide_params@@PAVphys_rigid_body@@3PAVphys_vel_accum@@4AAUsolve_single_output_ext@1@@Z
void phys_simple_collision_response::solve_single_contact2(phys_simple_contact_data *contact_constraint_data,
    const phys_contact_point &cp, const phys_simple_collide_params &params, phys_rigid_body *contact_rigid_body_a,
    phys_rigid_body *contact_rigid_body_b, phys_vel_accum *agent_body_a, phys_vel_accum *agent_body_b, solve_single_output_ext &output)
{
    phys_body_vel body_vels[2];
    phys_simple_constraint_init_input ina[2];
    {
        phys_vel_accum *body = agent_body_a;
        phys_rigid_body *owner_rigid_body = contact_rigid_body_a;
        for (int i = 0; i < 2; i++) {
            phys_simple_constraint_init_input &in = ina[i];
            phys_body_vel &vel = body_vels[i];
            {
                in.m_inv_mass = body->m_inv_masses(3);
                const phys_vec4 &mass_center = body->get_center_of_mass_in_world();
                in.m_mass_rel_pos.set_sub4(cp.get_contact_position(), mass_center);
                const phys_vec4 &inv_mass_diag = body->m_inv_masses;
                vel.m_lin_parts = body->m_lin_vel;
                if (body->m_mat_is_identity) {
                    vel.m_ang_jacobian = body->m_ang_vel;
                    in.m_inv_inertia.set_diagonal(inv_mass_diag(0), inv_mass_diag(1), inv_mass_diag(2));
                } else {
                    const phys_rotation &t = body->get_core_from_world();
                    vel.m_ang_jacobian.set_rotated_inverse_dir(t, body->m_ang_vel);
                    phys_mat3 x;
                    phys_vec4 d0; d0.set_broadcast3_clobber_w(inv_mass_diag, 0);
                    phys_vec4 d1; d1.set_broadcast3_clobber_w(inv_mass_diag, 1);
                    phys_vec4 d2; d2.set_broadcast3_clobber_w(inv_mass_diag, 2);
                    x.get_column(0).set_mul4(d0, t.get_column(0));
                    x.get_column(1).set_mul4(d1, t.get_column(1));
                    x.get_column(2).set_mul4(d2, t.get_column(2));
                    in.m_inv_inertia.set_mul_inverse(x, t);
                    in.m_inv_inertia.transpose_in_place();
                }
            }
            body = agent_body_b;
            owner_rigid_body = contact_rigid_body_b;
        }
    }

    phys_simple_constraint_info info;
    phys_rotation directions;
    {
        const phys_vec4 &dir = cp.get_contact_normal();
        directions.get_column(0) = dir;
        int min_axis_idx = 0;
        int ok_flag1 = 1;
        int ok_flag2 = 2;
        float a0 = fabsf(dir(0));
        float a1 = fabsf(dir(1));
        float a2 = fabsf(dir(2));
        if (a1 < a0) {
            ok_flag1 = 0;
            min_axis_idx = 1;
            a0 = a1;
        }
        if (a2 < a0) {
            ok_flag2 = min_axis_idx;
            min_axis_idx = 2;
        }
        phys_vec4 &binormal_dir = directions.get_column(1);
        binormal_dir.set_zero4();
        binormal_dir(ok_flag1) = dir(ok_flag2);
        binormal_dir(ok_flag2) = -dir(ok_flag1);
        binormal_dir.normalize_xyz();
        directions.get_column(2).set_cross(dir, binormal_dir);
    }

    contact_constraint_data->collision_response_begin_callback(cp, ina[0], body_vels[0], ina[1], body_vels[1]);
    phys_simple_constraint_init_info(ina[0], ina[1], directions, info);
    output.m_response_impulse = phys_simple_constraint_collide(info, params, body_vels[0], body_vels[1]);
    contact_constraint_data->collision_response_end_callback(cp, output.m_response_impulse, ina[0], body_vels[0], ina[1], body_vels[1]);

    agent_body_a->m_lin_vel = body_vels[0].m_lin_parts;
    contact_rigid_body_a->get_rigid_motion()->m_linear_velocity = body_vels[0].m_lin_parts;
    contact_rigid_body_a->get_rigid_motion()->m_ang_velocity = body_vels[0].m_ang_jacobian;
    agent_body_b->m_lin_vel = body_vels[1].m_lin_parts;
    contact_rigid_body_b->get_rigid_motion()->m_linear_velocity = body_vels[1].m_lin_parts;
    contact_rigid_body_b->get_rigid_motion()->m_ang_velocity = body_vels[1].m_ang_jacobian;

    {
        phys_vel_accum *body = agent_body_a;
        for (int j = 0; j < 2; body = agent_body_b, j++) {
            phys_body_vel &vel = body_vels[j];
            if (body->m_mat_is_identity)
                body->m_ang_vel = vel.m_ang_jacobian;
            else
                body->m_ang_vel.set_rotated_dir(body->get_core_from_world(), vel.m_ang_jacobian);
        }
    }
}

// @address 0x664658 ?solve_single_contact@phys_simple_collision_response@@SAXABVphys_contact_point@@MABUphys_simple_collide_params@@PAVphys_motion@@2PAVphys_dynamics_contact_manager@@AAUsolve_single_output@1@@Z
void phys_simple_collision_response::solve_single_contact(const phys_contact_point &cp, float time,
    const phys_simple_collide_params &params, phys_motion *agent_body_a, phys_motion *agent_body_b,
    phys_dynamics_contact_manager *agent_contact_manager, solve_single_output &output)
{
    phys_simple_constraint_init_input init_input_a;
    {
        init_input_a.m_inv_mass = agent_body_a->get_inv_mass();
        phys_vec4 mass_center;
        phys_swept_transform_util::compute_center_of_mass_at(agent_body_a->m_motion_state, time, mass_center);
        init_input_a.m_mass_rel_pos.set_sub4(cp.get_contact_position(), mass_center);
        agent_body_a->get_inv_inertia_world(init_input_a.m_inv_inertia);
    }

    phys_simple_constraint_init_input init_input_b;
    {
        init_input_b.m_inv_mass = agent_body_b->get_inv_mass();
        phys_vec4 mass_center;
        phys_swept_transform_util::compute_center_of_mass_at(agent_body_b->m_motion_state, time, mass_center);
        init_input_b.m_mass_rel_pos.set_sub4(cp.get_contact_position(), mass_center);
        agent_body_b->get_inv_inertia_world(init_input_b.m_inv_inertia);
    }

    phys_body_vel body_vel_a;
    phys_body_vel body_vel_b;
    {
        body_vel_a.m_lin_parts = agent_body_a->get_linear_velocity();
        body_vel_a.m_ang_jacobian = agent_body_a->get_angular_velocity();
        body_vel_b.m_lin_parts = agent_body_b->get_linear_velocity();
        body_vel_b.m_ang_jacobian = agent_body_b->get_angular_velocity();
    }

    phys_body_vel orig_vel_a;
    phys_swept_transform_util::get_atom_velocity(agent_body_a->m_motion_state, orig_vel_a.m_lin_parts, orig_vel_a.m_ang_jacobian);
    phys_body_vel orig_vel_b;
    phys_swept_transform_util::get_atom_velocity(agent_body_b->m_motion_state, orig_vel_b.m_lin_parts, orig_vel_b.m_ang_jacobian);

    phys_simple_constraint_info info;
    phys_rotation directions;
    phys_vec4_util::build_orthonormal_basis(cp.get_contact_normal(), directions);

    agent_contact_manager->toi_response_begin_callback(cp, init_input_a, body_vel_a, init_input_b, body_vel_b);

    phys_simple_constraint_init_info(init_input_a, init_input_b, directions, info);

    output.m_response_impulse = phys_simple_constraint_collide(info, params, body_vel_a, body_vel_b);

    agent_contact_manager->toi_response_end_callback(cp, output.m_response_impulse, init_input_a, body_vel_a, init_input_b, body_vel_b);

    {
        phys_vec4 pA;
        phys_simple_constraint_point_vel(info, orig_vel_a, body_vel_b, pA);
        phys_vec4 pB;
        phys_simple_constraint_point_vel(info, body_vel_a, orig_vel_b, pB);
        output.m_keyframed_vel_a = pA(0);
        output.m_keyframed_vel_b = pB(0);
    }

    {
        agent_body_a->set_linear_velocity(body_vel_a.m_lin_parts);
        agent_body_a->set_angular_velocity(body_vel_a.m_ang_jacobian);
        agent_body_b->set_linear_velocity(body_vel_b.m_lin_parts);
        agent_body_b->set_angular_velocity(body_vel_b.m_ang_jacobian);
    }
}

typedef char phys_vel_accum_size[(sizeof(phys_vel_accum) == 128) ? 1 : -1];
typedef char phys_vel_accum_lin_offset[((unsigned int)&((phys_vel_accum *)0)->m_lin_vel == 16) ? 1 : -1];
