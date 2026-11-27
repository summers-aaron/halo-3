// hkconstraintsolver/hkPoweredChainSolverUtil.obj: from the records 0x664a88 0x664b38 0x664b98 0x664bf8 0x664f20 0x665238 0x665378 0x665450 0x665638 0x665bb8 0x666190 0x666388 0x6664d8
// @compiler xdk4314
// @flags /TP /O2 /Oc /QVMXReserve
#include "phys/phys_base_types.h"
// @address 0x664b38 phys_powered_chain_disable_next_motor
// @address 0x664b98 phys_powered_chain_set_impulse_vel
// @address 0x666388 phys_powered_chain_build_jacobian
#include <vectorintrinsics.h>
#include <ppcintrinsics.h>
#include <math.h>
#include "phys/phys_powered_chain_solver_util.h"
#include "phys/phys_chain_schema_types.h"
#include "phys/phys_mat6.h"
#include "phys/phys_math.h"
#include "phys/phys_vec8.h"
#include "phys/phys_cfm_param.h"
#include "phys/phys_vel_accum.h"
#include "phys/phys_vel_accum_offset.h"
#include "phys/phys_jacobian_element.h"

#include "phys/phys_constraint_query_in.h"
#include "phys/phys_constraint_query_out.h"
#include "phys/phys_chain_solver_info.h"
#include <string.h>
#include "phys/phys_mat6_copy_inl.h"

// match: 0x664f20, 0x665238, 0x665638: build the scalar splat as one expression; the shared constructor assigns twice.
static __forceinline phys_simd_scalar phys_powered_chain_util_simd_scalar(const float &x) { return phys_simd_scalar(__vspltw(__lvlx(&x, 0), 0)); }





 extern "C" void phys_powered_chain_disable_next_motor(const unsigned char *motor_bits, phys_mat6 *matrix_row)
{
    if (!*motor_bits)
        return;
    int motor_bit = 0;
    __vector4 *next_row_vectors = &matrix_row->m_blocks[1][1].m_column0.m_simd_quad;
    for (; motor_bit < 6; motor_bit += 2, ++next_row_vectors) {
        if ((*motor_bits >> (unsigned char)motor_bit) & 3) {
            __vector4 *cur_row_vectors = next_row_vectors - 6;
            *cur_row_vectors = __vxor(*cur_row_vectors, *cur_row_vectors);
            *next_row_vectors = __vxor(*next_row_vectors, *next_row_vectors);
        }
    }
}

extern "C" void phys_powered_chain_set_impulse_vel(
    int row_idx, int axis, const phys_3d_ang_motor_solver_info *motor_infos, phys_vec8 *outputs)
{
    const phys_3d_ang_motor_solver_info *info = &motor_infos[row_idx];
    phys_vec8 *output = &outputs[row_idx];
    int axis_motor_state = (info->m_chain_broken >> (axis * 2)) & 3;
    float velocity;
    if (axis_motor_state == 1)
        velocity = info->m_motor_infos[axis].m_max_substep_impulse;
    else
        velocity = info->m_motor_infos[axis].m_min_substep_impulse;
    output->m_ang_half(axis) = velocity;
}
extern "C" void phys_powered_chain_build_jacobian(const phys_powered_chain_jacobian_params &params, const phys_constraint_query_in &in,
    phys_constraint_query_out &out)
{
    int n = params.m_constraint_count;
    float damping = params.m_chain_damping;
    phys_powered_chain_schema *s = static_cast<phys_powered_chain_schema *>(out.m_jacobian_schemas.val());
    float tau = params.m_chain_tau;
    unsigned char *status_write_ptr = params.m_child_status_writeback_buf;
    phys_vel_accum_offset *src = params.m_vel_accums;
    int cnt = n + 1;
    phys_3d_ang_motor_solver_info *mat = params.m_motors_state;
    s->m_type = phys_schema::SCHEMA_KIND_POWERED_CHAIN;
    s->m_constraint_count = (unsigned short)n;
    s->m_query_tau = tau;
    s->m_child_status_write_back = status_write_ptr;
    s->m_solver_damping = damping;
    phys_vel_accum_offset *dst = reinterpret_cast<phys_vel_accum_offset *>(phys_add_byte_offset(s, (unsigned short)n * 0x4f0 + 64));
    s->m_schema_size = ((unsigned short)n * 0x534 + 83) & -16;
    if (cnt > 0) { unsigned c = cnt; do { *dst++ = *src++; } while (--c); }
    phys_3d_ang_motor_solver_info *motor_desc = reinterpret_cast<phys_3d_ang_motor_solver_info *>(phys_add_byte_offset(s, s->m_constraint_count * 0x4f4 + 68));
    if (n > 0) { unsigned c = n; do { *motor_desc++ = *mat++; } while (--c); }
    s->m_max_torque_hysteresis = params.m_max_torque_hyst;
    s->m_child_status_write_back = params.m_child_status_writeback_buf;
    phys_powered_chain_build_lu_matrix(params.m_constraint_count, params.m_cfm_param, params.m_motors_state,
        phys_powered_chain_schema::get_lin_jacobians(out.m_jacobian_schemas.val()),
        phys_powered_chain_schema::get_ang_jacobians(out.m_jacobian_schemas.val(), n),
        reinterpret_cast<phys_vel_accum_offset *>(phys_add_byte_offset(s, s->m_constraint_count * 0x4f0 + 64)), params.m_accums_base,
        reinterpret_cast<phys_chain_mat6_triple *>(phys_add_byte_offset(s, s->m_constraint_count * 240 + 32)), n * 960);
    out.m_jacobian_schemas = phys_add_byte_offset(out.m_jacobian_schemas.val(), s->m_schema_size);
}

// @address 0x664a88 phys_powered_chain_disable_this_motor
extern "C" void phys_powered_chain_disable_this_motor(phys_3d_ang_motor_solver_info *info, phys_bool flag, phys_mat6 *a, phys_mat6 *b)
{
    if (!info->m_chain_broken)
        return;
    for (int i = 0; i < 3; i++) {
        if (info->get_motor_solver_state(i)) {
            a->m_blocks[1][0](i, 0) = 0.0f;
            a->m_blocks[1][0](i, 1) = 0.0f;
            a->m_blocks[1][0](i, 2) = 0.0f;
            a->m_blocks[1][1](i, 0) = 0.0f;
            a->m_blocks[1][1](i, 1) = 0.0f;
            a->m_blocks[1][1](i, 2) = 0.0f;
            a->m_blocks[1][1](i, i) = 1.0f;
            if (flag) {
                b->m_blocks[1][0](i, 0) = 0.0f;
                b->m_blocks[1][0](i, 1) = 0.0f;
                b->m_blocks[1][0](i, 2) = 0.0f;
                b->m_blocks[1][1](i, 0) = 0.0f;
                b->m_blocks[1][1](i, 1) = 0.0f;
                b->m_blocks[1][1](i, 2) = 0.0f;
            }
        }
    }
}

// @address 0x6664d8 ??0phys_mat6@@QAA@ABV0@@Z

static __forceinline float phys_powered_chain_util_max2(float x, float y)
{
    return (float)__fsel(x - y, x, y);
}
// @address 0x664bf8 phys_powered_chain_lu_row
extern "C" void phys_powered_chain_lu_row(phys_bool not_last_row,
    phys_chain_mat6_triple &triple, phys_3d_ang_motor_solver_info *motor_states, phys_mat6 &prev_offdiag_mat, phys_mat6 *&prev_upper_triple)
{
    phys_mat6 diag_mat(triple.m_mat_diag);
    phys_mat6 next_offdiag_mat(triple.m_mat_next_offdiag);
    phys_powered_chain_disable_this_motor(motor_states, not_last_row, &diag_mat, &next_offdiag_mat);
    phys_powered_chain_disable_next_motor(&motor_states->m_chain_broken, &prev_offdiag_mat);
    phys_mat6_set_transpose(triple.m_lower, prev_offdiag_mat);
    phys_mat6 tmp;
    phys_mat6_set_mul(tmp, triple.m_lower, *prev_upper_triple);
    triple.m_diagonal_inv = diag_mat;
    phys_mat6_sub(triple.m_diagonal_inv, tmp);
    for (int i = 0; i < 3; i++) {
        float &d0 = triple.m_diagonal_inv.m_blocks[0][0](i, i);
        float &d1 = triple.m_diagonal_inv.m_blocks[1][1](i, i);
        d0 = phys_powered_chain_util_max2(d0, 1.1920929e-07f);
        d1 = phys_powered_chain_util_max2(d1, 1.1920929e-07f);
    }
    phys_mat6 diag_copy;
    diag_copy = triple.m_diagonal_inv;
    phys_mat6_set_invert(triple.m_diagonal_inv, diag_copy);
    phys_mat6_set_mul(triple.m_upper, triple.m_diagonal_inv, next_offdiag_mat);
    prev_offdiag_mat = triple.m_mat_next_offdiag;
    prev_upper_triple = &triple.m_upper;
}

// @address 0x665378 phys_powered_chain_solve_vec_g
extern "C" {
// @address 0x665378 phys_powered_chain_solve_vec_g
void phys_powered_chain_solve_vec_g(const phys_chain_solver_info &info, phys_vec8 *p1)
{
    phys_vec8 *g = p1 + 1;
    long i = 0;
    g[-1].set_zero8();
    if (info.m_constraint_count > 0) {
        do {
            phys_vec8 tmp;
            phys_mat6 *ma = &info.m_chain_triples[i].m_lower;
            phys_vec8 *rhs = info.m_vel_buffer.val() + i;
            phys_mat6_set_mul_vec(tmp, *ma, g[i - 1]);
            tmp.m_lin_block.set_sub4(rhs->m_lin_block, tmp.m_lin_block);
            tmp.m_ang_half.set_sub4(rhs->m_ang_half, tmp.m_ang_half);
            phys_mat6_set_mul_vec(g[i], info.m_chain_triples[i].m_diagonal_inv, tmp);
            i++;
        } while (i < info.m_constraint_count);
    }
}
}


// @address 0x665450 phys_powered_chain_disable_motors
extern "C" void phys_powered_chain_disable_motors(const phys_chain_solver_info &info, int &modified_child_constraint_idx,
    int &modified_coord_idx, float &impulse, phys_3d_ang_motor_solver_info *motor_states)
{
    phys_vec8 *velocities = info.m_g_temp_buffer + 1;
    float max_excess = 0.0f;
    int child_idx = -1;
    int coord_idx = -1;
    phys_vec8 tmp;
    tmp.set_zero8();
    for (int i = info.m_constraint_count - 1; i >= 0; i--) {
        phys_vec8 chain_scratch2;
        phys_mat6_set_mul_vec(chain_scratch2, info.m_chain_triples[i].m_upper, tmp);
        tmp.set_sub8(velocities[i], chain_scratch2);
        for (int k = 0; k < 3; k++) {
            if (info.m_motors_state[i].get_motor_solver_state(k) == 0) {
                float velocity = tmp.m_ang_half(k);
                float a = -velocity + info.m_motors_state[i].m_motor_infos[k].m_min_substep_impulse * 1.05f;
                float excess = phys_powered_chain_util_max2(velocity - info.m_motors_state[i].m_motor_infos[k].m_max_substep_impulse * 1.05f, a);
                if (excess > max_excess) {
                    max_excess = excess;
                    child_idx = i;
                    coord_idx = k;
                    impulse = velocity;
                }
            }
        }
    }
    if (child_idx >= 0) {
        float max_substep_impulse = info.m_motors_state[child_idx].m_motor_infos[coord_idx].m_max_substep_impulse;
        int state = (impulse > max_substep_impulse) ? 1 : 3;
        info.m_motors_state[child_idx].set_chain_state(coord_idx, state);
        modified_child_constraint_idx = child_idx;
        modified_coord_idx = coord_idx;
    }
}


static __forceinline void phys_powered_chain_diag_at_row(int row, const phys_cfm_param &constraint_force_mix, phys_1lin2ang_jacobian *j,
    phys_2ang_jacobian *ang_jac, const phys_vel_accum_offset *va, const phys_vel_accum *accums_base, phys_mat6 &diag_mat)
{
    const float &lin_multiplier = constraint_force_mix.m_lin_mul;
    const int row_cnt = 3;
    const int base = row * row_cnt;
    for (int r = 0; r < 3; r++) {
        for (int c = r; c < 3; c++) {
            const phys_vel_accum &mA = va[row].get_accumulator(accums_base);
            const phys_vel_accum &mB = va[row + 1].get_accumulator(accums_base);
            float &lin_part = diag_mat.m_blocks[0][0](r, c);
            lin_part = j[base + r].get_off_diag_same_bodies(mA, mB, j[base + c]);
            diag_mat.m_blocks[0][0](c, r) = lin_part;
            const float ang_nondiag = ang_jac[base + r].get_off_diag_same_bodies(mA, mB, ang_jac[base + c]);
            diag_mat.m_blocks[1][1](r, c) = ang_nondiag;
            diag_mat.m_blocks[1][1](c, r) = ang_nondiag;
        }
        diag_mat.m_blocks[0][0](r, r) *= lin_multiplier;
        diag_mat.m_blocks[1][1](r, r) *= constraint_force_mix.m_ang_mul;
        diag_mat.m_blocks[0][0](r, r) += constraint_force_mix.m_lin_add;
        diag_mat.m_blocks[1][1](r, r) += constraint_force_mix.m_ang_add;
    }
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 3; c++) {
            diag_mat.m_blocks[1][0](r, c) = ang_jac[base + r].get_off_diag_same_bodies(
                va[row].get_accumulator(accums_base), va[row + 1].get_accumulator(accums_base), j[base + c].as_2ang_jacobian());
        }
    }
    diag_mat.m_blocks[0][1].set_transpose_of(diag_mat.m_blocks[1][0]);
}

static __forceinline void phys_powered_chain_next_off_diag(int row, phys_1lin2ang_jacobian *j,
    phys_2ang_jacobian *ang_jac, const phys_vel_accum_offset *va, const phys_vel_accum *accums_base, phys_mat6 &next_offdiag_mat)
{
    const int row_cnt = 3;
    const int base = row * row_cnt;
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 3; c++) {
            const phys_vel_accum &mB = va[row + 1].get_accumulator(accums_base);
            next_offdiag_mat.m_blocks[0][0](r, c) = j[base + r].get_off_diag_shared_body(mB, j[base + row_cnt + c]);
            next_offdiag_mat.m_blocks[1][1](r, c) = ang_jac[base + r].get_off_diag_shared_body(mB, ang_jac[base + row_cnt + c]);
            next_offdiag_mat.m_blocks[0][1](r, c) = j[base + r].as_2ang_jacobian().get_off_diag_shared_body(
                mB, ang_jac[base + row_cnt + c]);
            next_offdiag_mat.m_blocks[1][0](r, c) = ang_jac[base + r].get_off_diag_shared_body(
                mB, j[base + row_cnt + c].as_2ang_jacobian());
        }
    }
}

// @address 0x665bb8 phys_powered_chain_build_lu_matrix
extern "C" void phys_powered_chain_build_lu_matrix(int solver_constraint_count, const phys_cfm_param &constraint_force_mix,
    // match: 0x665bb8: pointer __restrict qualifier prevents alias reload
    phys_3d_ang_motor_solver_info *motor_states, phys_1lin2ang_jacobian *__restrict j, phys_2ang_jacobian *__restrict ang_jac,
    phys_vel_accum_offset *va, const phys_vel_accum *accums_base, phys_chain_mat6_triple *chain_triples, int solver_buffer_size)
{
    const phys_cfm_param &row_cfm = constraint_force_mix;
    phys_mat6 prev_offdiag_mat;
    prev_offdiag_mat.set_zero();
    phys_mat6 *prev_upper_triple = &prev_offdiag_mat;
    int i;
    for (i = 0; i < solver_constraint_count; i++) {
        phys_powered_chain_diag_at_row(i, row_cfm, j, ang_jac, va, accums_base, chain_triples[i].m_mat_diag);
        if (i < solver_constraint_count - 1)
            phys_powered_chain_next_off_diag(i, j, ang_jac, va, accums_base, chain_triples[i].m_mat_next_offdiag);
        else
            chain_triples[i].m_mat_next_offdiag.set_zero();
    }
    for (i = 0; i < solver_constraint_count; i++) {
        phys_powered_chain_lu_row(
            phys_bool(0 != (solver_constraint_count - 1 - i)), chain_triples[i], &motor_states[i], prev_offdiag_mat, prev_upper_triple);
    }
}

static __forceinline void chain_load_vel_accums(const phys_vel_accum &mA, const phys_vel_accum &mB)
{
    __VMXSetReg(86, mA.m_lin_vel.m_simd_quad);
    __VMXSetReg(87, mA.m_ang_vel.m_simd_quad);
    __VMXSetReg(88, mA.m_inv_masses.m_simd_quad);
    __VMXSetReg(89, mB.m_lin_vel.m_simd_quad);
    __VMXSetReg(90, mB.m_ang_vel.m_simd_quad);
    __VMXSetReg(91, mB.m_inv_masses.m_simd_quad);
}
static __forceinline phys_simd_scalar chain_ang_dv0_user_tau(const phys_2ang_jacobian &lin_ang_jac, const phys_vel_accum &mA,
    const phys_vel_accum &mB, const phys_solver_info &info, float tau, float damping)
{
    __VMXSetReg(84, lin_ang_jac.m_ang_jacobian[0].m_simd_quad);
    __VMXSetReg(85, lin_ang_jac.m_ang_jacobian[1].m_simd_quad);
    __vector4 j0 = __VMXGetReg(84);
    __vector4 ang_a = __VMXGetReg(87);
    __vector4 j1 = __VMXGetReg(85);
    __vector4 chain_sum_ang_a = mA.get_sum_ang_vel().m_simd_quad;
    __vector4 chain_sum_ang_b = mB.get_sum_ang_vel().m_simd_quad;
    phys_simd_scalar t = phys_powered_chain_util_simd_scalar(tau);
    __vector4 vel = __vmaddfp(__vsubfp(ang_a, chain_sum_ang_a), j0, __vmulfp(__vsubfp(__VMXGetReg(90), chain_sum_ang_b), j1));
    __vector4 sum = __vmaddfp(chain_sum_ang_a, __VMXGetReg(84), __vmulfp(chain_sum_ang_b, __VMXGetReg(85)));
    phys_simd_scalar d = phys_powered_chain_util_simd_scalar(damping);
    phys_vec4 factor;
    factor.set_broadcast(info.m_inv_integrate_vel_factor, 0);
    __vector4 scaled_vel = __vmulfp(vel, d.m_simd_scalar);
    __vector4 res = __vmaddfp(sum, __vmulfp(t.m_simd_scalar, factor.m_simd_quad), scaled_vel);
    __vector4 res_with_rhs = __vrlimi(res, __vmulfp(__VMXGetReg(85), t.m_simd_scalar), 1, 0);
    phys_vec4 mask;
    mask.m_simd_quad = __VMXGetReg(92);
    return phys_simd_scalar(__vmsum4fp(mask.m_simd_quad, res_with_rhs));
}
// @address 0x665238 phys_powered_chain_restore_vel
extern "C" void phys_powered_chain_restore_vel(
    int modified_constraint_idx, int modified_coord_idx, const phys_chain_solver_info &info, phys_vec8 *velocities)
{
    const int i = modified_constraint_idx;
    phys_vec8 &velocity = velocities[i];
    const phys_vel_accum &accum_a = info.m_vel_accum_offsets[i].get_accumulator(info.m_accums_base);
    const phys_vel_accum &accum_b = info.m_vel_accum_offsets[i + 1].get_accumulator(info.m_accums_base);
    chain_load_vel_accums(accum_a, accum_b);
    const phys_3d_ang_motor_solver_info &motor = info.m_motors_state[i];
    const int c = modified_coord_idx;
    velocity.m_ang_half(c) = chain_ang_dv0_user_tau(
        info.m_ang_jacs[i * 3 + c], accum_a, accum_b, info.m_solver_info, motor.m_motor_infos[c].m_query_tau, motor.m_motor_infos[c].m_solver_damping);
}
static __forceinline phys_simd_scalar chain_ang_dv0_user_tau_madd(const phys_2ang_jacobian &lin_ang_jac, const phys_vel_accum &mA,
    const phys_vel_accum &mB, const phys_solver_info &info, float tau, float damping)
{
    phys_vec4 factor;
    factor.set_broadcast(info.m_inv_integrate_vel_factor, 0);
    __VMXSetReg(84, lin_ang_jac.m_ang_jacobian[0].m_simd_quad);
    __VMXSetReg(85, lin_ang_jac.m_ang_jacobian[1].m_simd_quad);
    __vector4 chain_sum_ang_a = mA.get_sum_ang_vel().m_simd_quad;
    __vector4 chain_sum_ang_b = mB.get_sum_ang_vel().m_simd_quad;
    __vector4 t = __vspltw(__lvlx(&tau, 0), 0);
    phys_simd_scalar d = phys_powered_chain_util_simd_scalar(damping);
    __vector4 tf = __vmulfp(t, factor.m_simd_quad);
    __vector4 j0 = __VMXGetReg(84);
    __vector4 ang_a = __VMXGetReg(87);
    __vector4 rel_a = __vsubfp(ang_a, chain_sum_ang_a);
    __vector4 sA = __vmulfp(chain_sum_ang_a, __VMXGetReg(84));
    __vector4 rhs = __vmulfp(__VMXGetReg(85), t);
    __vector4 j1 = __VMXGetReg(85);
    __vector4 rel_b = __vsubfp(__VMXGetReg(90), chain_sum_ang_b);
    __vector4 sB = __vmulfp(chain_sum_ang_b, __VMXGetReg(85));
    __vector4 jac_vel_a = __vmulfp(rel_a, j0);
    __vector4 jac_vel_b = __vmulfp(rel_b, j1);
    __vector4 sum = __vaddfp(sA, sB);
    __vector4 vel = __vaddfp(jac_vel_a, jac_vel_b);
    sum = __vmulfp(sum, tf);
    vel = __vmulfp(vel, d.m_simd_scalar);
    __vector4 res = __vaddfp(sum, vel);
    res = __vrlimi(res, rhs, 1, 0);
    return phys_simd_scalar(__vmsum4fp(__VMXGetReg(92), res));
}
// @address 0x665638 phys_powered_chain_enable_motors
extern "C" void phys_powered_chain_enable_motors(const phys_chain_solver_info &info, int &modified_child_constraint_idx,
    int &modified_coord_idx, float &impulse, phys_3d_ang_motor_solver_info *motor_states)
{
    phys_vec8 *velocities = info.m_g_temp_buffer + 1;
    float max_excess = 0.0f;
    int child_idx = -1;
    int coord_idx = -1;
    phys_vec8 prev_prev_scratch;
    prev_prev_scratch.set_zero8();
    phys_vec8 prev_scratch;
    prev_scratch.set_zero8();
    phys_vec8 tmp;
    tmp.set_zero8();
    {
        phys_vec8 chain_scratch2;
        phys_mat6_set_mul_vec(chain_scratch2, info.m_chain_triples[info.m_constraint_count - 1].m_upper, tmp);
        tmp.set_sub8(velocities[info.m_constraint_count - 1], chain_scratch2);
    }
    for (int i = info.m_constraint_count - 1; i >= 0; i--) {
        int prev_index = i - 1;
        prev_prev_scratch = prev_scratch;
        prev_scratch = tmp;
        phys_chain_mat6_triple *triple = &info.m_chain_triples[(short)prev_index];
        if (prev_index >= 0) {
            phys_vec8 chain_scratch2;
            phys_mat6_set_mul_vec(chain_scratch2, triple->m_upper, tmp);
            tmp.set_sub8(velocities[prev_index], chain_scratch2);
        } else {
            tmp.set_zero8();
        }
        phys_mat6 lower_transposed;
        if (prev_index >= 0)
            phys_mat6_set_transpose(lower_transposed, triple->m_mat_next_offdiag);
        else
            lower_transposed.set_to_identity();
        phys_vec8 vel;
        phys_mat6_set_mul_vec(vel, lower_transposed, tmp);
        phys_vec8 t;
        phys_mat6_set_mul_vec(t, triple[1].m_mat_diag, prev_scratch);
        vel.add_vec8(t);
        phys_mat6_set_mul_vec(t, triple[1].m_mat_next_offdiag, prev_prev_scratch);
        vel.add_vec8(t);
        for (int k = 0; k < 3; k++) {
            int state = info.m_motors_state[i].get_motor_solver_state(k);
            if (state == 3 || state == 1) {
                int ci = i;
                const phys_solver_info &solver_info = info.m_solver_info;
                const phys_vel_accum &accum_a = info.m_vel_accum_offsets[ci].get_accumulator(info.m_accums_base);
                const phys_vel_accum &accum_b = info.m_vel_accum_offsets[ci + 1].get_accumulator(info.m_accums_base);
                chain_load_vel_accums(accum_a, accum_b);
                const phys_3d_ang_motor_solver_info &motor = info.m_motors_state[ci];
                float dv = chain_ang_dv0_user_tau_madd(
                    info.m_ang_jacs[ci * 3 + k], accum_a, accum_b, solver_info, motor.m_motor_infos[k].m_query_tau, motor.m_motor_infos[k].m_solver_damping);
                float v = vel.m_ang_half(k);
                if ((state == 3 && v <= dv) || (state == 1 && v >= dv)) {
                    float excess = fabsf(v - dv);
                    if (excess > max_excess) {
                        max_excess = excess;
                        child_idx = ci;
                        coord_idx = k;
                    }
                }
            }
        }
    }
    if (child_idx >= 0) {
        info.m_motors_state[child_idx].set_chain_state(coord_idx, 0);
        modified_child_constraint_idx = child_idx;
        modified_coord_idx = coord_idx;
    }
}
static __forceinline void chain_set_sub4(phys_vec4 &o, const phys_vec4 &a, const phys_vec4 &b) { o.m_simd_quad = __vsubfp(a.m_simd_quad, b.m_simd_quad); }
static __forceinline void chain_set_add4(phys_vec4 &o, const phys_vec4 &a, const phys_vec4 &b) { o.m_simd_quad = __vaddfp(a.m_simd_quad, b.m_simd_quad); }
static __forceinline phys_simd_scalar chain_lin_dv0_user_tau(const phys_1lin2ang_jacobian &lin_ang_jac, const phys_vel_accum &mA,
    const phys_vel_accum &mB, const phys_solver_info &info, float tau, float damping)
{
    __VMXSetReg(78, lin_ang_jac.m_lin_part_0.m_simd_quad);
    __VMXSetReg(79, lin_ang_jac.m_ang_jacobian[0].m_simd_quad);
    __VMXSetReg(80, lin_ang_jac.m_ang_jacobian[1].m_simd_quad);
    phys_vec4 lin_a; lin_a.m_simd_quad = __vsubfp(__VMXGetReg(86), mA.get_sum_lin_vel().m_simd_quad);
    phys_vec4 ang_a; ang_a.m_simd_quad = __vsubfp(__VMXGetReg(87), mA.get_sum_ang_vel().m_simd_quad);
    phys_vec4 lin_b; lin_b.m_simd_quad = __vsubfp(__VMXGetReg(89), mB.get_sum_lin_vel().m_simd_quad);
    phys_vec4 ang_b; ang_b.m_simd_quad = __vsubfp(__VMXGetReg(90), mB.get_sum_ang_vel().m_simd_quad);
    phys_vec4 lin_part; chain_set_sub4(lin_part, lin_a, lin_b);
    phys_vec4 ang_sq0; ang_sq0.set_mul4(ang_a, lin_ang_jac.m_ang_jacobian[0]);
    phys_vec4 ang_sq1; ang_sq1.set_mul4(ang_b, lin_ang_jac.m_ang_jacobian[1]);
    lin_part.mul4(lin_ang_jac.m_lin_part_0);
    ang_sq0.add4(ang_sq1);
    phys_vec4 vel; chain_set_add4(vel, ang_sq0, lin_part);
    phys_vec4 sum_lin_a = mA.get_sum_lin_vel();
    phys_vec4 sum_ang_a = mA.get_sum_ang_vel();
    phys_vec4 sum_lin_b = mB.get_sum_lin_vel();
    phys_vec4 sum_ang_b = mB.get_sum_ang_vel();
    phys_vec4 sum_lin; chain_set_sub4(sum_lin, sum_lin_a, sum_lin_b);
    phys_vec4 ang_sum0; ang_sum0.set_mul4(sum_ang_a, lin_ang_jac.m_ang_jacobian[0]);
    phys_vec4 ang_sum1; ang_sum1.set_mul4(sum_ang_b, lin_ang_jac.m_ang_jacobian[1]);
    sum_lin.mul4(lin_ang_jac.m_lin_part_0);
    ang_sum0.add4(ang_sum1);
    phys_vec4 sum; chain_set_add4(sum, ang_sum0, sum_lin);
    phys_simd_scalar t = phys_powered_chain_util_simd_scalar(tau * info.m_inv_integrate_vel_factor.m_simd_quad.v[0]);
    sum.m_simd_quad = __vmulfp(sum.m_simd_quad, t.m_simd_scalar);
    phys_simd_scalar d = phys_simd_scalar::create(damping);
    vel.m_simd_quad = __vmulfp(vel.m_simd_quad, d.m_simd_scalar);
    phys_vec4 res; chain_set_add4(res, sum, vel);
    phys_simd_scalar dot(__vmsum3fp(res.m_simd_quad, phys_simd_vec_1111));
    return phys_powered_chain_util_simd_scalar(lin_ang_jac.get_rhs() * tau - (float)dot);
}
static __forceinline phys_simd_scalar chain_ang_dv0_user_tau_split(const phys_2ang_jacobian &lin_ang_jac, const phys_vel_accum &mA,
    const phys_vel_accum &mB, const phys_solver_info &info, float tau, float damping)
{
    __VMXSetReg(84, lin_ang_jac.m_ang_jacobian[0].m_simd_quad);
    __VMXSetReg(85, lin_ang_jac.m_ang_jacobian[1].m_simd_quad);
    __vector4 chain_sum_ang_a = mA.get_sum_ang_vel().m_simd_quad;
    __vector4 chain_sum_ang_b = mB.get_sum_ang_vel().m_simd_quad;
    __vector4 t = __vspltw(__lvlx(&tau, 0), 0);
    phys_vec4 factor;
    factor.set_broadcast(info.m_inv_integrate_vel_factor, 0);
    __vector4 tf = __vmulfp(t, factor.m_simd_quad);
    phys_simd_scalar d = phys_powered_chain_util_simd_scalar(damping);
    __vector4 rel_a = __vsubfp(__VMXGetReg(87), chain_sum_ang_a);
    __vector4 rel_b = __vsubfp(__VMXGetReg(90), chain_sum_ang_b);
    __vector4 jac_vel_a = __vmulfp(rel_a, __VMXGetReg(84));
    __vector4 jac_vel_b = __vmulfp(rel_b, __VMXGetReg(85));
    __vector4 vel = __vaddfp(jac_vel_a, jac_vel_b);
    __vector4 rhs = __vmulfp(__VMXGetReg(85), t);
    __vector4 sB = __vmulfp(chain_sum_ang_b, __VMXGetReg(85));
    __vector4 sA = __vmulfp(chain_sum_ang_a, __VMXGetReg(84));
    __vector4 sum = __vaddfp(sA, sB);
    sum = __vmulfp(sum, tf);
    vel = __vmulfp(vel, d.m_simd_scalar);
    __vector4 res = __vaddfp(sum, vel);
    res = __vrlimi(res, rhs, 1, 0);
    return phys_simd_scalar(__vmsum4fp(__VMXGetReg(92), res));
}
// @address 0x664f20 phys_powered_chain_calc_velocities
extern "C" void phys_powered_chain_calc_velocities(const phys_chain_solver_info &info, phys_vec8 *velocities)
{
    phys_1lin2ang_jacobian *j = info.m_lin_ang_jacs;
    phys_2ang_jacobian *ang_jac = info.m_ang_jacs;
    for (int i = 0; i < info.m_constraint_count; i++) {
        phys_vec8 &velocity = velocities[i];
        // match: 0x664f20: pointer __restrict qualifier prevents alias reload
        const phys_vel_accum *__restrict accum_a_ptr = &info.m_vel_accum_offsets[i].get_accumulator(info.m_accums_base);
        // match: 0x664f20: pointer __restrict qualifier prevents alias reload
        const phys_vel_accum *__restrict accum_b_ptr = &info.m_vel_accum_offsets[i + 1].get_accumulator(info.m_accums_base);
        chain_load_vel_accums(*accum_a_ptr, *accum_b_ptr);
        const phys_3d_ang_motor_solver_info &motors = info.m_motors_state[i];
        for (int k = 0; k < 3; k++) {
            velocity.m_lin_block(k) = chain_lin_dv0_user_tau(j[k], *accum_a_ptr, *accum_b_ptr, info.m_solver_info, info.m_schema_tau, info.m_schema_damping);
            velocity.m_ang_half(k) = chain_ang_dv0_user_tau_split(
                ang_jac[k], *accum_a_ptr, *accum_b_ptr, info.m_solver_info, motors.m_motor_infos[k].m_query_tau, motors.m_motor_infos[k].m_solver_damping);
        }
        velocity.m_lin_block(3) = 0.0f;
        velocity.m_ang_half(3) = 0.0f;
        j += 3;
        ang_jac += 3;
    }
}
static inline int phys_chain_max2_int(int x, int y)
{
    return x > y ? x : y;
}

// @address 0x666190 phys_powered_chain_update_lu
extern "C" void phys_powered_chain_update_lu(
    int first_constraint_idx, int solver_constraint_count, phys_3d_ang_motor_solver_info *motor_states, phys_chain_mat6_triple *chain_triples)
{
    int first_row_idx = phys_chain_max2_int(0, first_constraint_idx - 1);
    phys_mat6 prev_offdiag_mat;
    phys_mat6 *prev_upper_triple;
    if (first_row_idx) {
        prev_offdiag_mat = chain_triples[first_row_idx - 1].m_mat_next_offdiag;
        prev_upper_triple = &chain_triples[first_row_idx - 1].m_upper;
    } else {
        prev_offdiag_mat.set_zero();
        prev_upper_triple = &prev_offdiag_mat;
    }
    for (int i = first_row_idx; i < solver_constraint_count; i++) {
        phys_powered_chain_lu_row(
            phys_bool(0 != (solver_constraint_count - 1 - i)), chain_triples[i], &motor_states[i], prev_offdiag_mat, prev_upper_triple);
    }
}
