// hkconstraintsolver/hkSolverExport.obj: from the records 0x675e48 0x675e50 0x675e60 0x675e78
// @compiler xdk4314
// @flags /TP /O2 /Oc
// @address 0x675e48 ?phys_solver_export_rhs@@YAXMPAVphys_solver_results@@@Z
// @address 0x675e50 ?phys_solver_export_result64@@YAXMMPAVphys_solver_results@@PAVphys_dma_sparse_writer@@@Z
// @address 0x675e60 ?phys_solver_export_result128@@YAXMMMMPAVphys_solver_results@@PAVphys_dma_sparse_writer@@@Z
// @address 0x675e78 phys_export_impulses_and_rhs
#define PHYS_DISABLE_VEC4_CONSTRUCTORS


#include <ppcintrinsics.h>
#include <vectorintrinsics.h>
#include "phys/phys_solver_export.h"
#include "phys/phys_constraint_data.h"
#include "phys/phys_dynamics.h"
#include "phys/phys_schema.h"
#include "phys/phys_vec8.h"
#include "phys/phys_chain_motor_solver_info.h"
#include "phys/phys_1d_schemas.h"
#include "phys/phys_solver_elem_temp.h"
#include "phys/phys_vel_accum_offset.h"
#include "phys/phys_header_schema.h"
#include "phys/phys_shift_solver_results_schema.h"
#include "phys/phys_contact_schema_types.h"
#include "phys/phys_chain_schema_inl.h"

typedef unsigned char phys_u8;
typedef unsigned short phys_u16;
typedef signed char phys_i8;

class phys_dma_sparse_writer;
void phys_solver_export_rhs(float rhs, phys_solver_results *result)
{
    result->m_internal_solver_data = rhs;
}
void phys_solver_export_result64(float impulse, float rhs, phys_solver_results *result, phys_dma_sparse_writer *writer)
{
    result->m_applied_impulse = impulse;
    result->m_internal_solver_data = rhs;
}
void phys_solver_export_result128(float impulse_a, float rhs_a, float impulse_b, float rhs_b, phys_solver_results *results, phys_dma_sparse_writer *writer)
{
    results[0].m_applied_impulse = impulse_a;
    results[0].m_internal_solver_data = rhs_a;
    results[1].m_applied_impulse = impulse_b;
    results[1].m_internal_solver_data = rhs_b;
}

static __forceinline void phys_solver_write_rhs(float rhs, phys_solver_results *result)
{
    result->m_internal_solver_data = rhs;
}
static __forceinline void phys_solver_write_result64(float impulse, float rhs, phys_solver_results *result, phys_dma_sparse_writer *writer)
{
    result->m_applied_impulse = impulse;
    result->m_internal_solver_data = rhs;
}
static __forceinline void phys_solver_write_result128(
    float impulse_a, float rhs_a, float impulse_b, float rhs_b, phys_solver_results *results, phys_dma_sparse_writer *writer)
{
    results[0].m_applied_impulse = impulse_a;
    results[0].m_internal_solver_data = rhs_a;
    results[1].m_applied_impulse = impulse_b;
    results[1].m_internal_solver_data = rhs_b;
}







namespace phys_solver_export
{
    static __forceinline phys_real integrate_rhs(
        const phys_vel_accum &mA, const phys_vel_accum &mB, const phys_1lin2ang_jacobian *lin_ang_jac, phys_real tau_factor, phys_real vel_factor)
    {
        const phys_vec4 &lin_a = mA.get_sum_lin_vel();
        const phys_vec4 &lin_b = mB.get_sum_lin_vel();
        const phys_vec4 &ang_a = mA.get_sum_ang_vel();
        const phys_vec4 &ang_b = mB.get_sum_ang_vel();
        phys_vec4 ang_sq0; ang_sq0.set_mul4(ang_a, lin_ang_jac->m_ang_jacobian[0]);
        phys_vec4 lin_part; lin_part.set_sub4(lin_a, lin_b);
        phys_vec4 ang_sq1; ang_sq1.set_mul4(ang_b, lin_ang_jac->m_ang_jacobian[1]);
        lin_part.mul4(lin_ang_jac->m_lin_part_0);
        ang_sq0.add4(ang_sq1);
        ang_sq0.add4(lin_part);
        phys_real vel = ang_sq0.horizontal_add3();
        phys_real rhs = lin_ang_jac->m_lin_part_0(3) * tau_factor;
        phys_real v = vel * vel_factor;
        return rhs - v;
    }

    static __forceinline void integrate_rhs_pair(const phys_vel_accum &mA, const phys_vel_accum &mB,
        const phys_1lin2ang_jacobian *lin_ang_jac, phys_real tau_factor, phys_real vel_factor, phys_real &rhs_first, phys_real &rhs_second)
    {
        rhs_first = integrate_rhs(mA, mB, lin_ang_jac, tau_factor, vel_factor);
        rhs_second = integrate_rhs(mA, mB, lin_ang_jac + 1, tau_factor, vel_factor);
    }

    static __forceinline phys_real integrate_rhs_2lin2ang(
        const phys_vel_accum &mA, const phys_vel_accum &mB, const phys_2lin2ang_jacobian *lin_ang_jac, phys_real tau_factor, phys_real vel_factor)
    {
        const phys_vec4 &lin_a = mA.get_sum_lin_vel();
        const phys_vec4 &lin_b = mB.get_sum_lin_vel();
        const phys_vec4 &ang_a = mA.get_sum_ang_vel();
        const phys_vec4 &ang_b = mB.get_sum_ang_vel();
        phys_vec4 ang_sq0; ang_sq0.set_mul4(ang_a, lin_ang_jac->m_ang_jacobian[0]);
        phys_vec4 ang_sq1; ang_sq1.set_mul4(ang_b, lin_ang_jac->m_ang_jacobian[1]);
        phys_vec4 lin_delta0; lin_delta0.set_mul4(lin_a, lin_ang_jac->m_lin_parts[0]);
        phys_vec4 lin_delta1; lin_delta1.set_mul4(lin_b, lin_ang_jac->m_lin_parts[1]);
        ang_sq0.add4(ang_sq1);
        lin_delta0.sub4(lin_delta1);
        ang_sq0.add4(lin_delta0);
        phys_real vel = ang_sq0.horizontal_add3();
        phys_real rhs = lin_ang_jac->get_rhs() * tau_factor;
        phys_real v = vel * vel_factor;
        return rhs - v;
    }

    static __forceinline phys_real integrate_ang_rhs(
        const phys_vel_accum &mA, const phys_vel_accum &mB, const phys_2ang_jacobian *lin_ang_jac, phys_real tau_factor, phys_real vel_factor)
    {
        const phys_vec4 &ang_a = mA.get_sum_ang_vel();
        const phys_vec4 &ang_b = mB.get_sum_ang_vel();
        phys_vec4 ang_sq0; ang_sq0.set_mul4(ang_a, lin_ang_jac->m_ang_jacobian[0]);
        phys_vec4 ang_sq1; ang_sq1.set_mul4(ang_b, lin_ang_jac->m_ang_jacobian[1]);
        ang_sq0.add4(ang_sq1);
        phys_real vel = ang_sq0.horizontal_add3();
        return lin_ang_jac->get_ang_rhs() * tau_factor - vel * vel_factor;
    }

    static __forceinline void phys_solver_export_child_status(phys_powered_chain_schema *jacobian_schema)
    {
        for (int c = 0; c < jacobian_schema->m_constraint_count; c++)
        {
            phys_u8 child_broken = jacobian_schema->get_child_constraint_status_base()[c].m_chain_broken;
            jacobian_schema->m_child_status_write_back[c] = child_broken;
        }
    }

    static __forceinline void export_impulses_rhs(
        const phys_solver_info &info, const phys_solver_elem_temp *temp, const phys_schema *schema_list, const phys_vel_accum *vel_accums)
    {
        const phys_solver_elem_temp *elem_temp = temp;
        const phys_schema *schema_cursor = schema_list;

        const phys_vel_accum *mA = 0;
        const phys_vel_accum *mB = 0;
        phys_vel_accum temp_accum;

        const phys_real tau_factor = info.m_delta_time * info.m_damp_div_tau;
        const phys_real friction_tau_factor = info.m_delta_time * info.m_damp_div_friction_tau;
        const phys_real vel_factor = info.m_delta_time * phys_real(info.m_step_count);
        const phys_real body_delta_time = info.m_delta_time;

        phys_1lin2ang_jacobian jac_buf[2];
        phys_2ang_jacobian ang_jac_buf;
        phys_2lin2ang_jacobian lin2ang_jac_buf;
        phys_1lin2ang_jacobian *pair_jac_buf = jac_buf;
        phys_1lin2ang_jacobian *jac_buf_ptr = jac_buf;
        phys_2ang_jacobian *ang_jac_buf_ptr = &ang_jac_buf;
        const phys_1lin2ang_jacobian *jac_3d = jac_buf;
        const phys_1lin2ang_jacobian *pair_jac = pair_jac_buf;
        phys_1lin2ang_jacobian *lin_ang_jac = jac_buf_ptr;
        phys_2ang_jacobian *ang_jac_cursor = ang_jac_buf_ptr;
        const phys_2lin2ang_jacobian *pulley_jac = &lin2ang_jac_buf;
        const phys_solver_elem_temp *next_elem_temp = 0;
        int skipped_cnt = 0;

        phys_solver_results *solver_results;
        int solver_result_stride;
        phys_real used_impulse_factor;

        goto header_label;

        while (1)
        {
            switch (phys_schema::get_type_id(schema_cursor))
            {
            case phys_schema::SCHEMA_KIND_HEADER:
header_label:
                {
                    const phys_header_schema *header = static_cast<const phys_header_schema *>(schema_cursor);
                    solver_results = header->m_solver_results_main;
                    solver_result_stride = header->m_solver_result_stride;
                    mA = header->get_body_a(vel_accums);
                    mB = header->get_body_b(vel_accums);
                    schema_cursor = reinterpret_cast<const phys_schema *>(header + 1);
                    if (!solver_results)
                    {
                        while (phys_schema::get_type_id(schema_cursor) > phys_schema::SCHEMA_KIND_SIMPLE_BEGIN &&
                            phys_schema::get_type_id(schema_cursor) < phys_schema::SCHEMA_KIND_SIMPLE_END)
                        {
                            elem_temp += phys_schema::s_schemaNumSolverElemTemps[phys_schema::get_type_id(schema_cursor)];
                            schema_cursor = phys_add_byte_offset_const(schema_cursor, phys_schema::getSize(schema_cursor));
                        }
                    }
                    break;
                }

            case phys_schema::SCHEMA_KIND_GOTO:
                {
                    const phys_goto_schema *s = static_cast<const phys_goto_schema *>(schema_cursor);
                    schema_cursor = phys_add_byte_offset_const(schema_cursor, s->m_offset);
                    break;
                }

            case phys_schema::SCHEMA_KIND_SHIFT_RESULTS_PTR:
                {
                    const phys_shift_solver_results_schema *s = static_cast<const phys_shift_solver_results_schema *>(schema_cursor);
                    solver_results = phys_add_byte_offset(solver_results, solver_result_stride * s->get_solver_result_count());
                    schema_cursor = reinterpret_cast<const phys_schema *>(s + 1);
                    break;
                }

            case phys_schema::SCHEMA_KIND_PAIR_CONTACT:
            case phys_schema::SCHEMA_KIND_PAIR_CONTACT_CHECKED:
                {
                    const phys_2d_friction_schema *s = reinterpret_cast<const phys_2d_friction_schema *>(schema_cursor);
                    pair_jac = &s->m_contact_jac.m_jac_0;
                    schema_cursor = reinterpret_cast<const phys_schema *>(s + 1);
                    phys_real rhs_first, rhs_second;
                    integrate_rhs_pair(*mA, *mB, pair_jac, tau_factor, vel_factor, rhs_first, rhs_second);
                    phys_solver_write_result64(elem_temp[0].m_applied_impulse, rhs_first, solver_results, 0);
                    solver_results = phys_add_byte_offset(solver_results, solver_result_stride);
                    phys_solver_write_result64(elem_temp[1].m_applied_impulse, rhs_second, solver_results, 0);
                    solver_results = phys_add_byte_offset(solver_results, solver_result_stride);
                    elem_temp += 2;
                    break;
                }

            case phys_schema::SCHEMA_KIND_SINGLE_CONTACT:
            case phys_schema::SCHEMA_KIND_SINGLE_CONTACT_CHECKED:
                {
                    const phys_1d_bilateral_schema *s = reinterpret_cast<const phys_1d_bilateral_schema *>(schema_cursor);
                    lin_ang_jac = const_cast<phys_1lin2ang_jacobian *>(reinterpret_cast<const phys_1lin2ang_jacobian *>(s));
                    schema_cursor = phys_add_byte_offset_const(schema_cursor, 48);
                    goto export_single;
                }
            case phys_schema::SCHEMA_KIND_1D_BILATERAL_IMPULSE_CAP:
                {
                    lin_ang_jac = const_cast<phys_1lin2ang_jacobian *>(reinterpret_cast<const phys_1lin2ang_jacobian *>(schema_cursor));
                    schema_cursor = phys_add_byte_offset_const(schema_cursor, 64);
                    goto export_single;
                }
            case phys_schema::SCHEMA_KIND_1D_LIN_LIMITS:
                {
                    lin_ang_jac = const_cast<phys_1lin2ang_jacobian *>(reinterpret_cast<const phys_1lin2ang_jacobian *>(schema_cursor));
                    schema_cursor = phys_add_byte_offset_const(schema_cursor, 64);
                    goto export_single;
                }
            case phys_schema::SCHEMA_KIND_1D_BILATERAL:
                {
                    lin_ang_jac = const_cast<phys_1lin2ang_jacobian *>(reinterpret_cast<const phys_1lin2ang_jacobian *>(schema_cursor));
                    schema_cursor = phys_add_byte_offset_const(schema_cursor, 48);
                }
export_single:
                {
                    phys_real rhs = integrate_rhs(*mA, *mB, lin_ang_jac, tau_factor, vel_factor);
                    phys_solver_write_result64(elem_temp[0].m_applied_impulse, rhs, solver_results, 0);
                    solver_results = phys_add_byte_offset(solver_results, solver_result_stride);
                    elem_temp += 1;
                    break;
                }

            case phys_schema::SCHEMA_KIND_1D_LIN_MOTOR:
                {
                    phys_1lin2ang_jacobian *j = const_cast<phys_1lin2ang_jacobian *>(reinterpret_cast<const phys_1lin2ang_jacobian *>(schema_cursor));
                    lin_ang_jac = j;
                    used_impulse_factor = elem_temp[1].m_used_impulse_factor;
                    phys_real delta_rhs = elem_temp[2].m_delta_rhs;
                    lin_ang_jac->set_rhs(lin_ang_jac->get_rhs() + delta_rhs);
                    next_elem_temp = elem_temp + phys_schema::s_schemaNumSolverElemTemps[phys_schema::get_type_id(schema_cursor)];
                    schema_cursor = phys_add_byte_offset_const(schema_cursor, 80);
                    goto export_friction;
                }
            case phys_schema::SCHEMA_KIND_1D_FRICTION:
                {
                    lin_ang_jac = const_cast<phys_1lin2ang_jacobian *>(reinterpret_cast<const phys_1lin2ang_jacobian *>(schema_cursor));
                    used_impulse_factor = elem_temp[1].m_used_impulse_factor;
                    next_elem_temp = elem_temp + phys_schema::s_schemaNumSolverElemTemps[phys_schema::get_type_id(schema_cursor)];
                    schema_cursor = phys_add_byte_offset_const(schema_cursor, 64);
                }
export_friction:
                {
                    phys_real rhs = used_impulse_factor * integrate_rhs(*mA, *mB, lin_ang_jac, tau_factor, vel_factor);
                    phys_solver_write_result64(elem_temp[0].m_applied_impulse, rhs, solver_results, 0);
                    solver_results = phys_add_byte_offset(solver_results, solver_result_stride);
                    elem_temp = next_elem_temp;
                    next_elem_temp = 0;
                    break;
                }

            case phys_schema::SCHEMA_KIND_1D_ANG:
                {
                    ang_jac_cursor = const_cast<phys_2ang_jacobian *>(reinterpret_cast<const phys_2ang_jacobian *>(schema_cursor));
                    schema_cursor = phys_add_byte_offset_const(schema_cursor, 32);
                    goto export_ang;
                }
            case phys_schema::SCHEMA_KIND_1D_ANG_LIMITS:
                {
                    ang_jac_cursor = const_cast<phys_2ang_jacobian *>(reinterpret_cast<const phys_2ang_jacobian *>(schema_cursor));
                    schema_cursor = phys_add_byte_offset_const(schema_cursor, 48);
                }
export_ang:
                {
                    phys_real rhs = integrate_ang_rhs(*mA, *mB, ang_jac_cursor, body_delta_time, vel_factor);
                    phys_solver_write_result64(elem_temp[0].m_applied_impulse, rhs, solver_results, 0);
                    solver_results = phys_add_byte_offset(solver_results, solver_result_stride);
                    elem_temp += 1;
                    break;
                }

            case phys_schema::SCHEMA_KIND_1D_ANG_MOTOR:
                {
                    phys_2ang_jacobian *j = const_cast<phys_2ang_jacobian *>(reinterpret_cast<const phys_2ang_jacobian *>(schema_cursor));
                    ang_jac_cursor = j;
                    used_impulse_factor = elem_temp[1].m_used_impulse_factor;
                    phys_real delta_rhs = elem_temp[2].m_delta_rhs;
                    ang_jac_cursor->set_ang_rhs(ang_jac_cursor->get_ang_rhs() + delta_rhs);
                    next_elem_temp = elem_temp + phys_schema::s_schemaNumSolverElemTemps[phys_schema::get_type_id(schema_cursor)];
                    schema_cursor = phys_add_byte_offset_const(schema_cursor, 64);
                    goto export_ang_friction;
                }
            case phys_schema::SCHEMA_KIND_1D_ANG_FRICTION:
                {
                    ang_jac_cursor = const_cast<phys_2ang_jacobian *>(reinterpret_cast<const phys_2ang_jacobian *>(schema_cursor));
                    used_impulse_factor = elem_temp[1].m_used_impulse_factor;
                    next_elem_temp = elem_temp + phys_schema::s_schemaNumSolverElemTemps[phys_schema::get_type_id(schema_cursor)];
                    schema_cursor = phys_add_byte_offset_const(schema_cursor, 48);
                }
export_ang_friction:
                {
                    phys_real rhs = used_impulse_factor * integrate_ang_rhs(*mA, *mB, ang_jac_cursor, body_delta_time, vel_factor);
                    phys_solver_write_result64(elem_temp[0].m_applied_impulse, rhs, solver_results, 0);
                    solver_results = phys_add_byte_offset(solver_results, solver_result_stride);
                    elem_temp = next_elem_temp;
                    next_elem_temp = 0;
                    break;
                }

            case phys_schema::SCHEMA_KIND_1D_PULLEY:
                {
                    pulley_jac = reinterpret_cast<const phys_2lin2ang_jacobian *>(schema_cursor);
                    schema_cursor = phys_add_byte_offset_const(schema_cursor, 64);
                    phys_real rhs = integrate_rhs_2lin2ang(*mA, *mB, pulley_jac, tau_factor, vel_factor);
                    phys_solver_write_result64(elem_temp[0].m_applied_impulse, rhs, solver_results, 0);
                    solver_results = phys_add_byte_offset(solver_results, solver_result_stride);
                    elem_temp += 1;
                    break;
                }

            case phys_schema::SCHEMA_KIND_2D_FRICTION:
                {
                    const phys_2d_friction_schema *s = static_cast<const phys_2d_friction_schema *>(schema_cursor);
                    pair_jac = &s->m_contact_jac.m_jac_0;
                    phys_solver_results *sr;
                    s->unpack_solver_results(sr);
                    used_impulse_factor = elem_temp[2].m_used_impulse_factor;
                    schema_cursor = reinterpret_cast<const phys_schema *>(s + 1);
                    phys_real rhs_first, rhs_second;
                    integrate_rhs_pair(*mA, *mB, pair_jac, friction_tau_factor, vel_factor, rhs_first, rhs_second);
                    phys_solver_write_result128(elem_temp[0].m_applied_impulse, rhs_first *used_impulse_factor, elem_temp[1].m_applied_impulse,
                        rhs_second *used_impulse_factor, sr, 0);
                    elem_temp += 3;
                    break;
                }

            case phys_schema::SCHEMA_KIND_3D_FRICTION:
                {
                    const phys_3d_friction_schema *s = static_cast<const phys_3d_friction_schema *>(schema_cursor);
                    jac_3d = &s->m_contact_jac.m_jac_0;
                    phys_solver_results *sr;
                    s->unpack_solver_results(sr);
                    used_impulse_factor = elem_temp[3].m_used_impulse_factor;
                    schema_cursor = reinterpret_cast<const phys_schema *>(s + 1);
                    phys_real rhs_first, rhs_second;
                    integrate_rhs_pair(*mA, *mB, jac_3d, friction_tau_factor, vel_factor, rhs_first, rhs_second);
                    phys_real rhs_third = used_impulse_factor *
                        integrate_ang_rhs(*mA, *mB, reinterpret_cast<const phys_2ang_jacobian *>(jac_3d + 2), friction_tau_factor, vel_factor);
                    phys_solver_write_result128(elem_temp[0].m_applied_impulse, rhs_first *used_impulse_factor, elem_temp[1].m_applied_impulse,
                        rhs_second *used_impulse_factor, sr, 0);
                    phys_solver_write_result64(elem_temp[2].m_applied_impulse, rhs_third * used_impulse_factor, sr + 2, 0);
                    elem_temp += 4;
                    break;
                }

            case phys_schema::SCHEMA_KIND_ADD_VEL:
                {
                    const phys_add_vel_schema *s = static_cast<const phys_add_vel_schema *>(schema_cursor);
                    schema_cursor = reinterpret_cast<const phys_schema *>(s + 1);
                    if (s->m_body_index == 0)
                    {
                        temp_accum = *mA;
                        mA = &temp_accum;
                    }
                    else
                    {
                        temp_accum = *mB;
                        mB = &temp_accum;
                    }
                    phys_vec4 vel; vel = s->m_velocity;
                    temp_accum.m_lin_vel.add4(vel);
                    temp_accum.get_sum_lin_vel().add4(vel);
                    break;
                }

            case phys_schema::SCHEMA_KIND_SET_MASS:
                {
                    schema_cursor = phys_add_byte_offset_const(schema_cursor, 32);
                    break;
                }

            case phys_schema::SCHEMA_KIND_STIFF_SPRING_CHAIN:
                {
                    phys_stiff_spring_chain_schema *s = const_cast<phys_stiff_spring_chain_schema *>(
                        static_cast<const phys_stiff_spring_chain_schema *>(schema_cursor));
                    phys_1lin2ang_jacobian *j = phys_stiff_spring_chain_schema::get_jacobians(s);
                    if (solver_results)
                    {
                        const phys_vel_accum_offset *va = s->get_accumulator_offsets_base();
                        for (int i = 0; i < s->m_constraint_count; i++)
                        {
                            const phys_vel_accum &accum_a = va[i].get_accumulator(vel_accums);
                            const phys_vel_accum &accum_b = va[i + 1].get_accumulator(vel_accums);
                            phys_solver_write_result64(elem_temp[0].m_applied_impulse, integrate_rhs(accum_a, accum_b, j,
                                body_delta_time, vel_factor), solver_results, 0);
                            solver_results = phys_add_byte_offset(solver_results, solver_result_stride);
                            elem_temp += 1;
                            j += 1;
                        }
                    }
                    else
                    {
                        phys_u16 solver_constraint_count = s->m_constraint_count;
                        elem_temp += solver_constraint_count;
                    }
                    schema_cursor = phys_add_byte_offset_const(schema_cursor, s->m_schema_size);
                    break;
                }

            case phys_schema::SCHEMA_KIND_BALL_SOCKET_CHAIN:
                {
                    phys_ball_socket_chain_schema *s = const_cast<phys_ball_socket_chain_schema *>(
                        static_cast<const phys_ball_socket_chain_schema *>(schema_cursor));
                    phys_1lin2ang_jacobian *j = phys_ball_socket_chain_schema::get_jacobians(s);
                    if (solver_results)
                    {
                        const phys_vel_accum_offset *va = s->get_accumulator_offsets_base();
                        for (int i = 0; i < s->m_constraint_count * 3; i++)
                        {
                            const phys_vel_accum &accum_a = va[i].get_accumulator(vel_accums);
                            const phys_vel_accum &accum_b = va[i + 1].get_accumulator(vel_accums);
                            phys_solver_write_result64(elem_temp[0].m_applied_impulse, integrate_rhs(accum_a, accum_b, j,
                                body_delta_time, vel_factor), solver_results, 0);
                            solver_results = phys_add_byte_offset(solver_results, solver_result_stride);
                            elem_temp += 1;
                            j += 1;
                        }
                    }
                    else
                    {
                        elem_temp += s->m_constraint_count * 3;
                    }
                    schema_cursor = phys_add_byte_offset_const(schema_cursor, s->m_schema_size);
                    break;
                }

            case phys_schema::SCHEMA_KIND_POWERED_CHAIN:
                {
                    phys_powered_chain_schema *s = const_cast<phys_powered_chain_schema *>(static_cast<const phys_powered_chain_schema *>(schema_cursor));
                    phys_solver_export_child_status(s);
                    phys_1lin2ang_jacobian *lin_jacs = phys_powered_chain_schema::get_lin_jacobians(s);
                    phys_2ang_jacobian *ang_jac = phys_powered_chain_schema::get_ang_jacobians(s, s->m_constraint_count);
                    if (solver_results)
                    {
                        const phys_vel_accum_offset *va = s->get_accumulator_offsets_base();
                        for (int i = 0; i < s->m_constraint_count; i++)
                        {
                            const phys_vel_accum &accum_a = va[i].get_accumulator(vel_accums);
                            const phys_vel_accum &accum_b = va[i + 1].get_accumulator(vel_accums);
                            for (int k = 0; k < 3; k++)
                            {
                                phys_real lin_rhs = integrate_rhs(accum_a, accum_b, lin_jacs + k, body_delta_time, vel_factor);
                                phys_solver_write_result64(elem_temp[k].m_applied_impulse, lin_rhs, solver_results, 0);
                                phys_real ang_rhs = integrate_ang_rhs(accum_a, accum_b, ang_jac + k, body_delta_time, vel_factor);
                                phys_solver_write_result64(elem_temp[k + 3].m_applied_impulse, ang_rhs, solver_results + k + 3, 0);

                                const phys_3d_ang_motor_solver_info *status = s->get_child_constraint_status_base() + i;
                                if (status->get_motor_solver_state(k) == 3)
                                {
                                    phys_real inv_jac_diag = ang_jac[k].get_inv_jac_diag();
                                    phys_real max_impulse = status->m_motor_infos[k].m_min_substep_impulse / inv_jac_diag;
                                    const phys_real factor = 1.5f;
                                    phys_real limit = max_impulse * info.m_delta_time / status->m_motor_infos[k].m_query_tau * factor;
                                    ang_rhs = phys_math::max_of_two(ang_rhs, limit);
                                    phys_solver_write_rhs(ang_rhs, solver_results + k + 3);
                                }
                                if (status->get_motor_solver_state(k) == 1)
                                {
                                    phys_real inv_jac_diag = ang_jac[k].get_inv_jac_diag();
                                    phys_real max_impulse = status->m_motor_infos[k].m_max_substep_impulse / inv_jac_diag;
                                    const phys_real factor = 1.5f;
                                    phys_real limit = max_impulse * info.m_delta_time / status->m_motor_infos[k].m_query_tau * factor;
                                    ang_rhs = phys_math::min_of_two(ang_rhs, limit);
                                    phys_solver_write_rhs(ang_rhs, solver_results + k + 3);
                                }
                            }
                            solver_results = phys_add_byte_offset(solver_results, solver_result_stride * 6);
                            elem_temp += 6;
                            lin_jacs += 3;
                            ang_jac += 3;
                        }
                    }
                    else
                    {
                        elem_temp += s->m_constraint_count * 6;
                    }
                    schema_cursor = phys_add_byte_offset_const(schema_cursor, s->m_schema_size);
                    break;
                }

            case phys_schema::SCHEMA_KIND_END:
                return;

            default:
                break;
            }
        }
    }
}

// @address 0x675e78
extern "C" void phys_export_impulses_and_rhs(
    const phys_solver_info &i, const phys_solver_elem_temp *temp, const phys_schema *schema_list, const phys_vel_accum *vel_accums)
{
    phys_solver_export::export_impulses_rhs(i, temp, schema_list, vel_accums);
}

typedef char phys_vel_accum_size[(sizeof(phys_vel_accum) == 128) ? 1 : -1];
typedef char phys_vel_accum_lin_offset[((unsigned int)&((phys_vel_accum *)0)->m_lin_vel == 16) ? 1 : -1];

