// hkconstraintsolver/hkSolver.obj: from the records 0x66ce30 0x66ce38 0x66ce40 0x66ced0 0x66d270 0x66da48 0x66db30 0x66e040 0x66e730 0x670858 0x672fd0
// @compiler xdk4314
// @flags /TP /O2 /Oc /QVMXReserve
// @address 0x66ce30 phys_solve_upload
// @address 0x66ce38 ?phys_debug_print_accums@@YAXHABUphys_solver_info@@PAVphys_vel_accum@@1@Z
// @address 0x66ced0 phys_solver_stiff_spring_chain
// @address 0x66d270 phys_solver_ball_socket_chain
// @address 0x66e040 phys_solver_powered_chain
// @address 0x66e730 phys_solve_step_jacobians
#define PHYS_DISABLE_VEC4_CONSTRUCTORS
#include "phys/phys_base_types.h"

#include <vectorintrinsics.h>
#include <math.h>
#include <ppcintrinsics.h>
#include "phys/phys_powered_chain_solver_util.h"
#include "phys/phys_solver.h"
#include "phys/phys_cfm_param.h"
#include "phys/phys_1d_schemas.h"
#include "phys/phys_1d_lin_motor_schema.h"
#include "phys/phys_solver_velocity.h"
#include "phys/phys_solver_elem_temp.h"
#include "phys/phys_vel_accum_offset.h"
#include "phys/phys_chain_motor_solver_info.h"
#include "phys/phys_dynamics.h"
#include "phys/phys_header_schema.h"
#include "phys/phys_contact_schema_types.h"
#include "phys/phys_math.h"
#include "phys/phys_chain_solver_info.h"
#include "phys/phys_chain_schema_inl.h"
#include "phys/phys_solver_check.h"

struct solver_step_temp
{
    phys_padded<int> m_current_step;
    phys_padded<float> m_integrate_factor;
};

extern "C" void phys_solve_upload(void)
{
}

void phys_debug_print_accums(int accumulator_count, const phys_solver_info& solver_info,
    phys_vel_accum* accum_array, phys_vel_accum* accums_end)
{
}

// @address 0x82017350 phys_solver_max_vel
static const __declspec(align(16)) float phys_solver_max_vel[4] = { 1e6f, 1e6f, 1e6f, 1e6f };

// match: helpers establish callee global symbol IDs (positiveMask, identity, invMask, maxVel) for loop preheader hoisting
static __forceinline void touch_xform_identity() { (void)&phys_transform_identity[0]; }
static __forceinline void touch_inv_mask_table() { (void)&phys_vec4_cmp::s_invMaskFromBits[0]; }
static __forceinline const phys_vec4& solver_max_vel_get()
{
    return *static_cast<const phys_vec4*>(static_cast<const void*>(&phys_solver_max_vel));
}


// @address 0x66ce40 ?update_sum_velocities@@YAXABUphys_solver_info@@ABVphys_vec4@@_N1PAVphys_vel_accum@@@Z
void update_sum_velocities(const phys_solver_info& info, const phys_vec4& vel_field, bool is_last_step,
    const phys_vec4& factor, phys_vel_accum*vel_accum)
{
    __vector4 scale = factor.m_simd_quad;
    __vector4 f = __vspltw(info.m_integrate_vel_factor.m_simd_quad, 0);
    __vector4 sum_lin = vel_accum->m_scratch_0.m_simd_quad;
    __vector4 sum_ang_vel = vel_accum->m_scratch_1.m_simd_quad;
    __vector4 lin_vel_delta = __vsubfp(vel_accum->m_lin_vel.m_simd_quad, sum_lin);
    __vector4 ang_vel_delta = __vsubfp(vel_accum->m_ang_vel.m_simd_quad, sum_ang_vel);
    sum_lin = __vaddfp(__vmulfp(f, lin_vel_delta), sum_lin);
    sum_ang_vel = __vaddfp(sum_ang_vel, __vmulfp(f, ang_vel_delta));
    if (!is_last_step)
    {
        lin_vel_delta = __vaddfp(lin_vel_delta, sum_lin);
        ang_vel_delta = __vaddfp(ang_vel_delta, sum_ang_vel);
        lin_vel_delta = __vaddfp(lin_vel_delta, vel_field.m_simd_quad);
    }
    else
    {
        sum_lin = __vmulfp(scale, sum_lin);
        sum_ang_vel = __vmulfp(scale, sum_ang_vel);
    }
    vel_accum->m_scratch_0.m_simd_quad = sum_lin;
    vel_accum->m_scratch_1.m_simd_quad = sum_ang_vel;
    vel_accum->m_lin_vel.m_simd_quad = lin_vel_delta;
    vel_accum->m_ang_vel.m_simd_quad = ang_vel_delta;
}

static __forceinline void update_sum_vels(const phys_solver_info& info, const phys_vec4& vel_field,
    bool is_last_step, const phys_vec4& factor, phys_vel_accum* vel_accum)
{
    __vector4 vel_field_quad = vel_field.m_simd_quad;
    __vector4 f = __vspltw(info.m_integrate_vel_factor.m_simd_quad, 0);
    __vector4 sum_lin = vel_accum->m_scratch_0.m_simd_quad;
    __vector4 sum_ang_vel = vel_accum->m_scratch_1.m_simd_quad;
    __vector4 lin_vel_delta = __vsubfp(vel_accum->m_lin_vel.m_simd_quad, sum_lin);
    __vector4 ang_vel_delta = __vsubfp(vel_accum->m_ang_vel.m_simd_quad, sum_ang_vel);
    sum_lin = __vaddfp(__vmulfp(f, lin_vel_delta), sum_lin);
    sum_ang_vel = __vaddfp(sum_ang_vel, __vmulfp(f, ang_vel_delta));
    if (!is_last_step)
    {
        lin_vel_delta = __vaddfp(lin_vel_delta, sum_lin);
        ang_vel_delta = __vaddfp(ang_vel_delta, sum_ang_vel);
        lin_vel_delta = __vaddfp(lin_vel_delta, vel_field_quad);
    }
    else
    {
        sum_lin = __vmulfp(factor.m_simd_quad, sum_lin);
        sum_ang_vel = __vmulfp(factor.m_simd_quad, sum_ang_vel);
    }
    vel_accum->m_scratch_0.m_simd_quad = sum_lin;
    vel_accum->m_scratch_1.m_simd_quad = sum_ang_vel;
    vel_accum->m_lin_vel.m_simd_quad = lin_vel_delta;
    vel_accum->m_ang_vel.m_simd_quad = ang_vel_delta;
}

namespace phys_solver
{
    static __forceinline void apply_vel_field(const phys_solver_info& info, const phys_vec4& vel_field,
        phys_vel_accum* accum_array, phys_vel_accum* accums_end)
    {
        if (accums_end == 0)
        {
            accums_end = (phys_vel_accum*)-1;
        }
        while (accum_array < accums_end)
        {
            switch (accum_array->m_type)
            {
            case phys_vel_accum::PHYS_VEL_ACCUM_KEYFRAMED_BODY:
                accum_array->get_sum_lin_vel().set_zero4();
                accum_array->get_sum_ang_vel().set_zero4();
                accum_array++;
                break;
            case phys_vel_accum::PHYS_RIGID_BODY:
                do
                {
                    accum_array->get_sum_lin_vel().set_zero4();
                    accum_array->get_sum_ang_vel().set_zero4();
                    accum_array->m_lin_vel.add4(vel_field);
                    accum_array++;
                }
                while (accum_array->m_type == phys_vel_accum::PHYS_RIGID_BODY && accum_array < accums_end);
                break;
            case phys_vel_accum::PHYS_NO_GRAVITY_RIGID_BODY:
                return;
            default:
                __debugbreak();
            }
        }
    }

    static __forceinline void integrate_vel_accum(const phys_solver_info& info, phys_vel_accum* vel_accum)
    {
        const phys_solver_info::sleep_info& accum_sleep_info = info.m_deactivation_info[vel_accum->m_deactivation_class];

        phys_vec4 abs_lin_vel; abs_lin_vel.set_abs4(vel_accum->m_lin_vel);
        phys_vec4 abs_ang_vel; abs_ang_vel.set_abs4(vel_accum->m_ang_vel);

        phys_vec4 rel_vel; rel_vel.set_mul4(accum_sleep_info.m_inv_lin_vel_threshold, abs_lin_vel);
        rel_vel.add_scaled4(accum_sleep_info.m_ang_vel_threshold_inv, abs_ang_vel);

        phys_vec4 one; one.set_all3(info.m_one_const);

        // match: 0x66da48: establish callee global symbol IDs (positiveMask, identity, invMask, maxVel) for loop preheader hoisting
        touch_xform_identity();
        touch_inv_mask_table();

        {
            const phys_vec4& max_vel_vec = solver_max_vel_get();
            int mask = abs_lin_vel.compare_less4(max_vel_vec).get_mask() & abs_ang_vel.compare_less4(max_vel_vec).get_mask();
            mask &= vel_accum->m_lin_vel.compare_equal4(vel_accum->m_lin_vel).get_mask() & vel_accum->m_ang_vel.compare_equal4(vel_accum->m_ang_vel).get_mask();
            if ((mask & phys_vec4_cmp::CMP_MASK_XYZ) != phys_vec4_cmp::CMP_MASK_XYZ)
            {
                vel_accum->m_lin_vel = phys_transform::get_identity().get_column(0);
                vel_accum->m_ang_vel = phys_transform::get_identity().get_column(0);
            }
        }

        if (rel_vel.compare_less_equal4(one).all_are_set(phys_vec4_cmp::CMP_MASK_XYZ))
        {
            phys_vec4 sleep_vel_threshold; sleep_vel_threshold.set_all3(accum_sleep_info.m_rel_sleep_vel_threshold);
            if (rel_vel.compare_less_equal4(sleep_vel_threshold).all_are_set(phys_vec4_cmp::CMP_MASK_XYZ))
            {
                if (vel_accum->m_deactivation_counter > 0)
                {
                    vel_accum->m_deactivation_counter--;
                }
                vel_accum->m_ang_vel.set_zero4();
                vel_accum->m_lin_vel.set_zero4();
                return;
            }
            else
            {
                vel_accum->m_ang_vel.mul4(accum_sleep_info.m_slow_object_vel_mul);
                vel_accum->m_lin_vel.mul4(accum_sleep_info.m_slow_object_vel_mul);
            }
        }
        vel_accum->m_deactivation_counter = accum_sleep_info.m_steps_to_sleep;
    }

    static __forceinline void integrate_vels(const phys_solver_info& info, const phys_vec4& vel_field, bool is_last_step,
        const phys_vec4& factor, phys_vel_accum* accum_array, phys_vel_accum* accums_end)
    {
        if (accums_end == 0)
        {
            accums_end = (phys_vel_accum*)-1;
        }
        while (accum_array < accums_end)
        {
            switch (accum_array->m_type)
            {
            case phys_vel_accum::PHYS_VEL_ACCUM_KEYFRAMED_BODY:
                {
                    const phys_vec4& zero = phys_vec4::get_zero();
                    update_sum_vels(info, zero, is_last_step, factor, accum_array);
                }
                accum_array++;
                break;
            case phys_vel_accum::PHYS_RIGID_BODY:
                do
                {
                    integrate_vel_accum(info, accum_array);
                    update_sum_vels(info, vel_field, is_last_step, factor, accum_array);
                    accum_array++;
                }
                while (accum_array->m_type == phys_vel_accum::PHYS_RIGID_BODY && accum_array < accums_end);
                break;
            case phys_vel_accum::PHYS_NO_GRAVITY_RIGID_BODY:
                return;
            default:
                break;
            }
        }
    }
}

// @address 0x66da48 phys_solve_apply_gravity_steps
extern "C" int phys_solve_apply_gravity_steps(const phys_solver_info& info,
    phys_vel_accum* accum_array, phys_vel_accum* accums_end)
{
    phys_vec4 gravity = info.m_gravity_per_substep;
    phys_solver::apply_vel_field(info, gravity, accum_array, accums_end);
    phys_debug_print_accums(0, info, accum_array, accums_end);
    return 1;
}

// @address 0x66db30 phys_solve_integrate_vel_steps
extern "C" int phys_solve_integrate_vel_steps(const phys_solver_info& info, int cur_solver_step,
    phys_vel_accum* accum_array, phys_vel_accum* accums_end, phys_bool& out_solve_finished)
{
    phys_debug_print_accums(cur_solver_step + 1, info, accum_array, accums_end);
    phys_vec4 gravity = info.m_gravity_per_substep;
    phys_vec4 factor; factor.set_all3(info.m_inv_steps * info.m_inv_integrate_vel_factor(0));
    bool is_last_step = false;
    out_solve_finished = false;
    if (cur_solver_step >= info.m_step_count - 1)
    {
        is_last_step = true;
        out_solve_finished = true;
    }
    phys_solver::integrate_vels(info, gravity, is_last_step, factor, accum_array, accums_end);
    if (cur_solver_step >= info.m_step_count - 1)
    {
        phys_debug_print_accums(99, info, accum_array, accums_end);
    }
    return 1;
}






namespace phys_solver
{

    static __forceinline void store_vel_accums(phys_vel_accum& a, phys_vel_accum& b)
    {
        a.m_ang_vel.m_simd_quad = __VMXGetReg(87);
        b.m_ang_vel.m_simd_quad = __VMXGetReg(90);
        a.m_lin_vel.m_simd_quad = __VMXGetReg(86);
        b.m_lin_vel.m_simd_quad = __VMXGetReg(89);
    }

    static __forceinline void pop_vel_accums()
    {
    }

    static __forceinline void load_jacobian(const phys_1lin2ang_jacobian& lin_ang_jac)
    {
        __VMXSetReg(78, lin_ang_jac.m_lin_part_0.m_simd_quad);
        __VMXSetReg(79, lin_ang_jac.m_ang_jacobian[0].m_simd_quad);
        __VMXSetReg(80, lin_ang_jac.m_ang_jacobian[1].m_simd_quad);
    }

    static __forceinline phys_simd_scalar lin_dv0_user_tau_custom_rhs(const phys_1lin2ang_jacobian& lin_ang_jac,
        const phys_vel_accum& mA, const phys_vel_accum& mB, const phys_solver_info& info,
        float tau, float damping, float rhs)
    {
        load_jacobian(lin_ang_jac);
        phys_vec4 body_vel_a; body_vel_a.m_simd_quad = __vsubfp(__VMXGetReg(86), mA.m_scratch_0.m_simd_quad);
        phys_vec4 ang_a; ang_a.m_simd_quad = __vsubfp(__VMXGetReg(87), mA.m_scratch_1.m_simd_quad);
        phys_vec4 body_vel_b; body_vel_b.m_simd_quad = __vsubfp(__VMXGetReg(89), mB.m_scratch_0.m_simd_quad);
        phys_vec4 ang_b; ang_b.m_simd_quad = __vsubfp(__VMXGetReg(90), mB.m_scratch_1.m_simd_quad);

        phys_vec4 lin_part; lin_part.set_sub4(body_vel_a, body_vel_b);
        phys_vec4 ang_sq0; ang_sq0.set_mul4(ang_a, lin_ang_jac.m_ang_jacobian[0]);
        phys_vec4 ang_sq1; ang_sq1.set_mul4(ang_b, lin_ang_jac.m_ang_jacobian[1]);
        lin_part.mul4(lin_ang_jac.m_lin_part_0);
        ang_sq0.add4(ang_sq1);
        phys_vec4 dv; dv.set_add4(ang_sq0, lin_part);

        phys_vec4 sum_lin_a = mA.m_scratch_0;
        phys_vec4 sum_ang_a = mA.m_scratch_1;
        phys_vec4 sum_lin_b = mB.m_scratch_0;
        phys_vec4 sum_ang_b = mB.m_scratch_1;
        phys_vec4 sum_lin_diff; sum_lin_diff.set_sub4(sum_lin_a, sum_lin_b);
        phys_vec4 scaled_ang_a; scaled_ang_a.set_mul4(sum_ang_a, lin_ang_jac.m_ang_jacobian[0]);
        phys_vec4 scaled_ang_b; scaled_ang_b.set_mul4(sum_ang_b, lin_ang_jac.m_ang_jacobian[1]);
        sum_lin_diff.mul4(lin_ang_jac.m_lin_part_0);
        scaled_ang_a.add4(scaled_ang_b);
        phys_vec4 sum_dv; sum_dv.set_add4(scaled_ang_a, sum_lin_diff);

        sum_dv.mul4(phys_simd_scalar(info.m_inv_integrate_vel_factor(0) * tau));
        dv.mul4(phys_simd_scalar(damping));
        phys_vec4 x; x.set_add4(sum_dv, dv);
        float v = x.horizontal_add3();
        float r = rhs * tau - v;
        return phys_simd_scalar(r);
    }

    static __forceinline void apply_impulse(const phys_simd_scalar& impulse, const phys_1lin2ang_jacobian& lin_ang_jac,
        phys_vel_accum& mA, phys_vel_accum& mB, phys_solver_elem_temp& temp)
    {
        __vector4 applied = __lvewx(&temp.m_applied_impulse, 0);
        __vector4 splat_a = __vspltw(__VMXGetReg(88), 3);
        __vector4 splat_b = __vspltw(__VMXGetReg(91), 3);
        __vector4 ang_dir_a = __vmulfp(lin_ang_jac.m_ang_jacobian[0].m_simd_quad, __VMXGetReg(88));
        __vector4 ang_dir_b = __vmulfp(lin_ang_jac.m_ang_jacobian[1].m_simd_quad, __VMXGetReg(91));
        __vector4 lin_dir_a = __vmulfp(lin_ang_jac.m_lin_part_0.m_simd_quad, splat_a);
        __vector4 lin_dir_b = __vmulfp(lin_ang_jac.m_lin_part_0.m_simd_quad, splat_b);
        __vector4 sum = __vaddfp(applied, impulse.get_quad());
        __VMXSetReg(87, __vmaddfp(impulse.get_quad(), ang_dir_a, __VMXGetReg(87)));
        __VMXSetReg(90, __vmaddfp(impulse.get_quad(), ang_dir_b, __VMXGetReg(90)));
        __VMXSetReg(86, __vmaddfp(impulse.get_quad(), lin_dir_a, __VMXGetReg(86)));
        __VMXSetReg(89, __vnmsubfp(impulse.get_quad(), lin_dir_b, __VMXGetReg(89)));
        __stvewx(sum, &temp.m_applied_impulse, 0);
    }
}



// @address 0x66ced0
extern "C" void phys_solver_stiff_spring_chain(const phys_solver_info& info, const void* step_temp,
    phys_vel_accum* accum_array, phys_stiff_spring_chain_schema* jacobian_schema, phys_solver_elem_temp* temp)
{
    int solver_constraint_count = jacobian_schema->m_constraint_count;
    phys_1lin2ang_jacobian* jacobian_list = phys_stiff_spring_chain_schema::get_jacobians(reinterpret_cast<phys_schema *>(jacobian_schema));
    phys_vel_accum_offset* accum_offsets = jacobian_schema->get_accumulator_offsets_base();
    phys_chain_triple* matrix = jacobian_schema->get_matrix_buffer();
    float* schema_temp_buf = jacobian_schema->get_temp_buffer() + 1;
    schema_temp_buf[-1] = 0.0f;

    for (int i = 0; i < solver_constraint_count; i++)
    {
        phys_vel_accum& accum_a = accum_offsets[i].get_accumulator(accum_array);
        phys_vel_accum& accum_b = accum_offsets[i + 1].get_accumulator(accum_array);
        phys_solver::load_vel_accums(accum_a, accum_b);
        float dv = phys_solver::lin_dv0_user_tau_custom_rhs(jacobian_list[i], accum_a, accum_b, info, jacobian_schema->m_query_tau,
            jacobian_schema->m_solver_damping, jacobian_list[i].get_rhs());
        schema_temp_buf[i] = (dv - matrix[i].m_lower * schema_temp_buf[i - 1]) / matrix[i].m_diagonal_inv;
        phys_solver::pop_vel_accums();
    }

    float impulse = 0.0f;
    for (int i = solver_constraint_count - 1; i >= 0; i--)
    {
        impulse = schema_temp_buf[i] - matrix[i].m_upper * impulse;
        // match: 0x66ced0: pointer __restrict qualifier prevents alias reload
        phys_vel_accum* __restrict accum_a = &accum_offsets[i].get_accumulator(accum_array);
        // match: 0x66ced0: pointer __restrict qualifier prevents alias reload
        phys_vel_accum* __restrict accum_b = &accum_offsets[i + 1].get_accumulator(accum_array);
        phys_solver::load_vel_accums(*accum_a, *accum_b);
        phys_solver::apply_impulse(phys_simd_scalar(impulse), jacobian_list[i], *accum_a, *accum_b, temp[i]);
        phys_solver::store_vel_accums(*accum_a, *accum_b);
    }
}

// @address 0x66d270
extern "C" void phys_solver_ball_socket_chain(const phys_solver_info& info, const void* step_temp,
    phys_vel_accum* accum_array, phys_schema* schema_in, phys_solver_elem_temp* temp)
{
    phys_vel_accum* accum_base = accum_array;
    phys_ball_socket_chain_schema* jacobian_schema = reinterpret_cast<phys_ball_socket_chain_schema*>(schema_in);
    int solver_constraint_count = jacobian_schema->m_constraint_count;
    phys_1lin2ang_jacobian* lin_ang_jac = phys_ball_socket_chain_schema::get_jacobians(schema_in);
    phys_vel_accum_offset* accum_offsets = jacobian_schema->get_accumulator_offsets_base();
    phys_chain_mat_triple* matrix = jacobian_schema->get_matrix_buffer();
    phys_vec4* schema_temp_buf = jacobian_schema->get_temp_buffer() + 1;
    schema_temp_buf[-1].set_zero4();

    for (int i = 0; i < solver_constraint_count; lin_ang_jac += 3, i++)
    {
        // match: 0x66d270: pointer __restrict qualifier prevents alias reload
        phys_vel_accum* __restrict accum_a = &accum_offsets[i].get_accumulator(accum_base);
        // match: 0x66d270: pointer __restrict qualifier prevents alias reload
        phys_vel_accum* __restrict accum_b = &accum_offsets[i + 1].get_accumulator(accum_base);
        phys_solver::load_vel_accums(*accum_a, *accum_b);
        phys_vec4 dv;
        dv(0) = phys_solver::lin_dv0_user_tau_custom_rhs(lin_ang_jac[0], *accum_a, *accum_b, info, jacobian_schema->m_query_tau,
            jacobian_schema->m_solver_damping, lin_ang_jac[0].get_rhs());
        dv(1) = phys_solver::lin_dv0_user_tau_custom_rhs(lin_ang_jac[1], *accum_a, *accum_b, info, jacobian_schema->m_query_tau,
            jacobian_schema->m_solver_damping, lin_ang_jac[1].get_rhs());
        dv(2) = phys_solver::lin_dv0_user_tau_custom_rhs(lin_ang_jac[2], *accum_a, *accum_b, info, jacobian_schema->m_query_tau,
            jacobian_schema->m_solver_damping, lin_ang_jac[2].get_rhs());
        dv(3) = 0.0f;
        phys_solver::pop_vel_accums();
        phys_vec4 tmp;
        tmp.set_mul_mat3_vec3(*reinterpret_cast<const phys_mat3 *>(matrix[i].m_lower), schema_temp_buf[i - 1]);
        tmp.set_sub4(dv, tmp);
        schema_temp_buf[i].set_mul_mat3_vec3(*reinterpret_cast<const phys_mat3 *>(matrix[i].m_diagonal_inv), tmp);
    }

    phys_vec4 impulse;
    impulse.set_zero4();
    lin_ang_jac = phys_ball_socket_chain_schema::get_jacobians(schema_in);
    for (int i = solver_constraint_count - 1; i >= 0; i--)
    {
        // match: 0x66d270: pointer __restrict qualifier prevents alias reload
        phys_vel_accum* __restrict accum_a = &accum_offsets[i].get_accumulator(accum_array);
        // match: 0x66d270: pointer __restrict qualifier prevents alias reload
        phys_vel_accum* __restrict accum_b = &accum_offsets[i + 1].get_accumulator(accum_array);
        int j = i * 3;
        phys_vec4 tmp;
        tmp.set_mul_mat3_vec3(*reinterpret_cast<const phys_mat3 *>(matrix[i].m_upper), impulse);
        impulse.set_sub4(schema_temp_buf[i], tmp);
        phys_solver::load_vel_accums(*accum_a, *accum_b);
        phys_solver::apply_impulse(impulse.get_simd_at(0), lin_ang_jac[j], *accum_a, *accum_b, temp[j]);
        phys_solver::apply_impulse(impulse.get_simd_at(1), lin_ang_jac[j + 1], *accum_a, *accum_b, temp[j + 1]);
        phys_solver::apply_impulse(impulse.get_simd_at(2), lin_ang_jac[j + 2], *accum_a, *accum_b, temp[j + 2]);
        phys_solver::store_vel_accums(*accum_a, *accum_b);
    }
}







namespace phys_solver
{
    static __forceinline void apply_ang_impulse(const phys_simd_scalar& impulse, const phys_2ang_jacobian& lin_ang_jac,
        phys_vel_accum& mA, phys_vel_accum& mB, phys_solver_elem_temp& temp)
    {
        __vector4 applied = __lvewx(&temp.m_applied_impulse, 0);
        __vector4 ang_dir_a = __vmulfp(lin_ang_jac.m_ang_jacobian[0].m_simd_quad, __VMXGetReg(88));
        __vector4 ang_dir_b = __vmulfp(lin_ang_jac.m_ang_jacobian[1].m_simd_quad, __VMXGetReg(91));
        __vector4 sum = __vaddfp(applied, impulse.get_quad());
        __VMXSetReg(87, __vmaddfp(impulse.get_quad(), ang_dir_a, __VMXGetReg(87)));
        __VMXSetReg(90, __vmaddfp(impulse.get_quad(), ang_dir_b, __VMXGetReg(90)));
        __stvewx(sum, &temp.m_applied_impulse, 0);
    }
}


// @address 0x66e040
extern "C" void phys_solver_powered_chain(const phys_solver_info& solver_info, const solver_step_temp& step_temp,
    phys_vel_accum* accum_array, phys_powered_chain_schema* jacobian_schema, phys_solver_elem_temp* elem_temp)
{
    phys_chain_solver_info info(solver_info);
    info.m_constraint_count = jacobian_schema->m_constraint_count;
    info.m_schema_tau = jacobian_schema->m_query_tau;
    info.m_schema_damping = jacobian_schema->m_solver_damping;
    info.m_lin_ang_jacs = phys_powered_chain_schema::get_lin_jacobians(jacobian_schema);
    info.m_ang_jacs = phys_powered_chain_schema::get_ang_jacobians(jacobian_schema, jacobian_schema->m_constraint_count);
    info.m_vel_accum_offsets = jacobian_schema->get_accumulator_offsets_base();
    info.m_accums_base = accum_array;
    info.m_chain_triples = jacobian_schema->get_matrix_buffer();
    info.m_motors_state = jacobian_schema->get_child_constraint_status_base();
    info.m_g_temp_buffer = jacobian_schema->get_temp_buffer();
    info.m_vel_buffer = jacobian_schema->get_velocity_buffer();

    {
        phys_2ang_jacobian* lin_ang_jac = info.m_ang_jacs;
        for (int i = 0; i < info.m_constraint_count; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                float rhs = lin_ang_jac->get_ang_rhs();
                const motor_1d_solver_info& motor_solver_info = info.m_motors_state[i].m_motor_infos[j];
                rhs += motor_solver_info.m_velocity * step_temp.m_integrate_factor;
                lin_ang_jac->set_ang_rhs(rhs);
                lin_ang_jac++;
            }
        }
    }

    phys_powered_chain_calc_velocities(info, info.m_vel_buffer);

    for (int i = 0; i < info.m_constraint_count; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (info.m_motors_state[i].get_motor_solver_state(j) == 2)
            {
                phys_vec8& v = info.m_vel_buffer[i];
                v.m_ang_half(j) = 0.0f;
            }
            else if (info.m_motors_state[i].get_motor_solver_state(j) != 0)
            {
                phys_powered_chain_set_impulse_vel(i, j, info.m_motors_state, info.m_vel_buffer);
            }
        }
    }

    while (true)
    {
        phys_powered_chain_solve_vec_g(info, info.m_g_temp_buffer);
        int modified_child_constraint_idx = -1;
        int modified_coord_idx = -1;
        float impulse = 0.0f;
        phys_powered_chain_enable_motors(info, modified_child_constraint_idx, modified_coord_idx, impulse, info.m_motors_state);
        if (modified_child_constraint_idx >= 0)
        {
            phys_powered_chain_update_lu(modified_child_constraint_idx, info.m_constraint_count, info.m_motors_state, info.m_chain_triples);
            phys_powered_chain_restore_vel(modified_child_constraint_idx, modified_coord_idx, info, info.m_vel_buffer);
            continue;
        }
        else
        {
            break;
        }
    }

    while (true)
    {
        phys_powered_chain_solve_vec_g(info, info.m_g_temp_buffer);
        int modified_child_constraint_idx = -1;
        int modified_coord_idx = -1;
        float impulse = 0.0f;
        phys_powered_chain_disable_motors(info, modified_child_constraint_idx, modified_coord_idx, impulse, info.m_motors_state);
        if (modified_child_constraint_idx >= 0)
        {
            phys_powered_chain_update_lu(modified_child_constraint_idx, info.m_constraint_count, info.m_motors_state, info.m_chain_triples);
            phys_powered_chain_set_impulse_vel(
                modified_child_constraint_idx, modified_coord_idx, info.m_motors_state, info.m_vel_buffer);
            continue;
        }
        else
        {
            break;
        }
    }

    phys_vec8* velocities = info.m_g_temp_buffer + 1;
    phys_vec8 tmp;
    tmp.set_zero8();
    for (int i = info.m_constraint_count - 1; i >= 0; i--)
    {
        int row3_idx = i * 3;
        int row6_idx = i * 6;
        phys_vec8 chain_scratch2;
        chain_scratch2.set_mul_mat6(info.m_chain_triples[i].m_upper, tmp);
        tmp.set_sub8(velocities[i], chain_scratch2);
        // match: 0x66e040: pointer __restrict qualifier prevents alias reload
        phys_vel_accum* __restrict mA = &info.m_vel_accum_offsets[i].get_accumulator(accum_array);
        // match: 0x66e040: pointer __restrict qualifier prevents alias reload
        phys_vel_accum* __restrict mB = &info.m_vel_accum_offsets[i + 1].get_accumulator(accum_array);
        phys_solver::load_vel_accums(*mA, *mB);
        phys_solver::apply_impulse(tmp.m_lin_block.get_simd_at(0), info.m_lin_ang_jacs[row3_idx + 0], *mA, *mB, elem_temp[row6_idx + 0]);
        phys_solver::apply_impulse(tmp.m_lin_block.get_simd_at(1), info.m_lin_ang_jacs[row3_idx + 1], *mA, *mB, elem_temp[row6_idx + 1]);
        phys_solver::apply_impulse(tmp.m_lin_block.get_simd_at(2), info.m_lin_ang_jacs[row3_idx + 2], *mA, *mB, elem_temp[row6_idx + 2]);
        phys_solver::apply_ang_impulse(tmp.m_ang_half.get_simd_at(0), info.m_ang_jacs[row3_idx + 0], *mA, *mB, elem_temp[row6_idx + 3]);
        phys_solver::apply_ang_impulse(tmp.m_ang_half.get_simd_at(1), info.m_ang_jacs[row3_idx + 1], *mA, *mB, elem_temp[row6_idx + 4]);
        phys_solver::apply_ang_impulse(tmp.m_ang_half.get_simd_at(2), info.m_ang_jacs[row3_idx + 2], *mA, *mB, elem_temp[row6_idx + 5]);
        phys_solver::store_vel_accums(*mA, *mB);
    }
}



namespace phys_solver
{
    static __forceinline void load_fixed_regs()
    {
        __vector4 one = __vcuxwfp(__vspltisw(1), 0);
        __VMXSetReg(93, one);
        __VMXSetReg(94, __vspltisw(0));
        __vector4 mask = __vsubfp(__VMXGetReg(94), __VMXGetReg(93));
        mask = __vrlimi(mask, __VMXGetReg(93), 1, 0);
        __VMXSetReg(92, mask);
    }

    static __forceinline void store_vels(phys_vel_accum& a, phys_vel_accum& b)
    {
        a.m_lin_vel.m_simd_quad = __VMXGetReg(86);
        a.m_ang_vel.m_simd_quad = __VMXGetReg(87);
        b.m_lin_vel.m_simd_quad = __VMXGetReg(89);
        b.m_ang_vel.m_simd_quad = __VMXGetReg(90);
    }

    static __forceinline void prefetch_vel_accums(phys_vel_accum& a, phys_vel_accum& b)
    {
        __dcbt(0, &a);
        __dcbt(0, &b);
    }

    static __forceinline void store_delayed_result(phys_solver_elem_temp* delayed)
    {
    }

    static __forceinline void load_jacobian_ptr(const phys_1lin2ang_jacobian* lin_ang_jac)
    {
        __VMXSetReg(81, lin_ang_jac[1].m_lin_part_0.m_simd_quad);
        __VMXSetReg(82, lin_ang_jac[1].m_ang_jacobian[0].m_simd_quad);
        __VMXSetReg(83, lin_ang_jac[1].m_ang_jacobian[1].m_simd_quad);
    }

    static __forceinline void load_ang_jacobian(const phys_2ang_jacobian& lin_ang_jac)
    {
        __VMXSetReg(84, lin_ang_jac.m_ang_jacobian[0].m_simd_quad);
        __VMXSetReg(85, lin_ang_jac.m_ang_jacobian[1].m_simd_quad);
    }

    static __forceinline phys_simd_scalar get_lin_dv0(const phys_1lin2ang_jacobian& lin_ang_jac, const phys_vel_accum& mA,
        const phys_vel_accum& mB, const phys_solver_info& info)
    {
        __vector4 lin_part = __vsubfp(__VMXGetReg(86), __VMXGetReg(89));
        phys_simd_scalar res;
        res.m_simd_scalar = __vmulfp(__VMXGetReg(87), __VMXGetReg(79));
        __vector4 ang_sq1 = __vmulfp(__VMXGetReg(90), __VMXGetReg(80));
        res.m_simd_scalar = __vaddfp(ang_sq1, res.m_simd_scalar);
        res.m_simd_scalar = __vmaddfp(lin_part, __VMXGetReg(78), res.m_simd_scalar);
        res.m_simd_scalar = __vrlimi(res.m_simd_scalar, __VMXGetReg(78), 1, 0);
        res.m_simd_scalar = __vmsum4fp(__VMXGetReg(92), res.m_simd_scalar);
        return res;
    }

    static __forceinline __vector4 lin_dv1_get(const phys_1lin2ang_jacobian& lin_ang_jac, const phys_vel_accum& mA,
        const phys_vel_accum& mB, const phys_solver_info& info)
    {
        __vector4 ang_nondiag = __vmulfp(__VMXGetReg(87), __VMXGetReg(82));
        ang_nondiag = __vmaddfp(__VMXGetReg(90), __VMXGetReg(83), ang_nondiag);
        __vector4 sum = __vmaddfp(__vsubfp(__VMXGetReg(86), __VMXGetReg(89)), __VMXGetReg(81), ang_nondiag);
        sum = __vrlimi(sum, __VMXGetReg(81), 1, 0);
        return __vmsum4fp(__VMXGetReg(92), sum);
    }

    static __forceinline __vector4 get_ang_dv0_no_load(const phys_2ang_jacobian& lin_ang_jac, const phys_vel_accum& mA,
        const phys_vel_accum& mB, const phys_solver_info& info)
    {
        __vector4 sum = __vmulfp(__VMXGetReg(87), __VMXGetReg(84));
        sum = __vmaddfp(__VMXGetReg(90), __VMXGetReg(85), sum);
        sum = __vrlimi(sum, __VMXGetReg(85), 1, 0);
        return __vmsum4fp(sum, __VMXGetReg(92));
    }

    static __forceinline phys_simd_scalar get_ang_dv0(const phys_2ang_jacobian& lin_ang_jac, const phys_vel_accum& mA,
        const phys_vel_accum& mB, const phys_solver_info& info)
    {
        load_ang_jacobian(lin_ang_jac);
        phys_simd_scalar res;
        res.get_quad() = get_ang_dv0_no_load(lin_ang_jac, mA, mB, info);
        return res;
    }

    static __forceinline phys_simd_scalar get_ang_dv0_user_tau_custom_rhs(const phys_2ang_jacobian& lin_ang_jac, const phys_vel_accum& mA,
        const phys_vel_accum& mB, const phys_solver_info& info, float tau, float damping, float rhs)
    {
        __VMXSetReg(84, lin_ang_jac.m_ang_jacobian[0].m_simd_quad);
        __VMXSetReg(85, lin_ang_jac.m_ang_jacobian[1].m_simd_quad);
        __vector4 chain_sum_ang_a = mA.m_scratch_1.m_simd_quad;
        __vector4 chain_sum_ang_b = mB.m_scratch_1.m_simd_quad;
        __vector4 ang_a = __vsubfp(__VMXGetReg(87), chain_sum_ang_a);
        __vector4 ang_b = __vsubfp(__VMXGetReg(90), chain_sum_ang_b);
        __vector4 tau_splat = __vspltw(__lvlx(&tau, 0), 0);
        __vector4 f = __vspltw(info.m_inv_integrate_vel_factor.m_simd_quad, 0);
        __vector4 tf = __vmulfp(tau_splat, f);
        __vector4 damping_splat = __vspltw(__lvlx(&damping, 0), 0);
        __vector4 rhs_splat = __vspltw(__lvlx(&rhs, 0), 0);
        __vector4 d0 = __vmulfp(ang_a, __VMXGetReg(84));
        __vector4 d1 = __vmulfp(ang_b, __VMXGetReg(85));
        __vector4 sum0 = __vmulfp(chain_sum_ang_a, __VMXGetReg(84));
        __vector4 sum1 = __vmulfp(chain_sum_ang_b, __VMXGetReg(85));
        __vector4 s = __vaddfp(sum0, sum1);
        __vector4 d = __vaddfp(d0, d1);
        s = __vmulfp(s, tf);
        d = __vmulfp(d, damping_splat);
        __vector4 rr = __vmulfp(rhs_splat, tau_splat);
        __vector4 x = __vaddfp(s, d);
        x = __vrlimi(x, rr, 1, 0);
        phys_simd_scalar res;
        res.get_quad() = __vmsum4fp(__VMXGetReg(92), x);
        return res;
    }

    static __forceinline phys_simd_scalar get_ang_dv0_user_tau(const phys_2ang_jacobian& lin_ang_jac, const phys_vel_accum& mA,
        const phys_vel_accum& mB, const phys_solver_info& info, float tau, float damping)
    {
        __vector4 chain_sum_ang_a = mA.m_scratch_1.m_simd_quad;
        __vector4 chain_sum_ang_b = mB.m_scratch_1.m_simd_quad;
        __vector4 ang_a = __vsubfp(__VMXGetReg(87), chain_sum_ang_a);
        __vector4 ang_b = __vsubfp(__VMXGetReg(90), chain_sum_ang_b);
        __VMXSetReg(84, lin_ang_jac.m_ang_jacobian[0].m_simd_quad);
        __VMXSetReg(85, lin_ang_jac.m_ang_jacobian[1].m_simd_quad);
        __vector4 tau_splat = __vspltw(__lvlx(&tau, 0), 0);
        __vector4 f = __vspltw(info.m_inv_integrate_vel_factor.m_simd_quad, 0);
        __vector4 tf = __vmulfp(tau_splat, f);
        __vector4 rr = __vmulfp(__VMXGetReg(85), tau_splat);
        __vector4 damping_splat = __vspltw(__lvlx(&damping, 0), 0);
        __vector4 d0 = __vmulfp(ang_a, __VMXGetReg(84));
        __vector4 d1 = __vmulfp(ang_b, __VMXGetReg(85));
        __vector4 d = __vaddfp(d0, d1);
        __vector4 sum1 = __vmulfp(chain_sum_ang_b, __VMXGetReg(85));
        __vector4 sum0 = __vmulfp(chain_sum_ang_a, __VMXGetReg(84));
        __vector4 s = __vaddfp(sum0, sum1);
        s = __vmulfp(s, tf);
        d = __vmulfp(d, damping_splat);
        __vector4 x = __vaddfp(s, d);
        x = __vrlimi(x, rr, 1, 0);
        phys_simd_scalar res;
        res.get_quad() = __vmsum4fp(__VMXGetReg(92), x);
        return res;
    }

    static __forceinline void apply_impulse0(const phys_simd_scalar& impulse, const phys_1lin2ang_jacobian& lin_ang_jac,
        phys_vel_accum& mA, phys_vel_accum& mB, phys_solver_elem_temp& temp)
    {
        __vector4 applied = __lvewx(&temp.m_applied_impulse, 0);
        __vector4 splat_a = __vspltw(__VMXGetReg(88), 3);
        __vector4 splat_b = __vspltw(__VMXGetReg(91), 3);
        __vector4 ang_dir_a = __vmulfp(__VMXGetReg(79), __VMXGetReg(88));
        __vector4 ang_dir_b = __vmulfp(__VMXGetReg(80), __VMXGetReg(91));
        __vector4 lin_dir_a = __vmulfp(__VMXGetReg(78), splat_a);
        __vector4 lin_dir_b = __vmulfp(__VMXGetReg(78), splat_b);
        __vector4 sum = __vaddfp(applied, impulse.get_quad());
        __VMXSetReg(87, __vmaddfp(impulse.get_quad(), ang_dir_a, __VMXGetReg(87)));
        __VMXSetReg(90, __vmaddfp(impulse.get_quad(), ang_dir_b, __VMXGetReg(90)));
        __VMXSetReg(86, __vmaddfp(impulse.get_quad(), lin_dir_a, __VMXGetReg(86)));
        __VMXSetReg(89, __vnmsubfp(impulse.get_quad(), lin_dir_b, __VMXGetReg(89)));
        __stvewx(sum, &temp.m_applied_impulse, 0);
    }

    static __forceinline void apply_impulse1(const phys_simd_scalar& impulse, const phys_1lin2ang_jacobian& lin_ang_jac,
        phys_vel_accum& mA, phys_vel_accum& mB, phys_solver_elem_temp& temp)
    {
        __vector4 applied = __lvewx(&temp.m_applied_impulse, 0);
        __vector4 splat_a = __vspltw(__VMXGetReg(88), 3);
        __vector4 splat_b = __vspltw(__VMXGetReg(91), 3);
        __vector4 ang_dir_a = __vmulfp(__VMXGetReg(82), __VMXGetReg(88));
        __vector4 ang_dir_b = __vmulfp(__VMXGetReg(83), __VMXGetReg(91));
        __vector4 lin_dir_a = __vmulfp(__VMXGetReg(81), splat_a);
        __vector4 lin_dir_b = __vmulfp(__VMXGetReg(81), splat_b);
        __vector4 sum = __vaddfp(applied, impulse.get_quad());
        __VMXSetReg(87, __vmaddfp(impulse.get_quad(), ang_dir_a, __VMXGetReg(87)));
        __VMXSetReg(90, __vmaddfp(impulse.get_quad(), ang_dir_b, __VMXGetReg(90)));
        __VMXSetReg(86, __vmaddfp(impulse.get_quad(), lin_dir_a, __VMXGetReg(86)));
        __VMXSetReg(89, __vnmsubfp(impulse.get_quad(), lin_dir_b, __VMXGetReg(89)));
        __stvewx(sum, &temp.m_applied_impulse, 0);
    }

    static __forceinline void apply_impulse_p1(const phys_simd_scalar& impulse, const phys_1lin2ang_jacobian& lin_ang_jac,
        phys_vel_accum& mA, phys_vel_accum& mB, phys_solver_elem_temp& temp)
    {
        const __vector4 impulse_value = impulse.get_quad();
        __vector4 applied = __lvewx(&temp.m_applied_impulse, 0);
        __vector4 splat_a = __vspltw(__VMXGetReg(88), 3);
        __vector4 splat_b = __vspltw(__VMXGetReg(91), 3);
        __vector4 ang_dir_a = __vmulfp(__VMXGetReg(82), __VMXGetReg(88));
        __vector4 lin_dir_a = __vmulfp(__VMXGetReg(81), splat_a);
        __vector4 ang_dir_b = __vmulfp(__VMXGetReg(83), __VMXGetReg(91));
        __vector4 sum = __vaddfp(applied, impulse_value);
        __vector4 lin_dir_b = __vmulfp(__VMXGetReg(81), splat_b);
        __VMXSetReg(87, __vmaddfp(impulse_value, ang_dir_a, __VMXGetReg(87)));
        __VMXSetReg(90, __vmaddfp(impulse_value, ang_dir_b, __VMXGetReg(90)));
        __VMXSetReg(86, __vmaddfp(impulse.get_quad(), lin_dir_a, __VMXGetReg(86)));
        __VMXSetReg(89, __vnmsubfp(impulse.get_quad(), lin_dir_b, __VMXGetReg(89)));
        __stvewx(sum, &temp.m_applied_impulse, 0);
    }

    static __forceinline void apply_impulse01(const phys_simd_scalar& impulse_a, const phys_simd_scalar& impulse_b, const phys_1lin2ang_jacobian* lin_ang_jac,
        phys_vel_accum& mA, phys_vel_accum& mB, phys_solver_elem_temp* temp)
    {
        apply_impulse0(impulse_a, lin_ang_jac[0], mA, mB, temp[0]);
        apply_impulse_p1(impulse_b, lin_ang_jac[1], mA, mB, temp[1]);
    }

    static __forceinline void apply_ang_impulse0(const phys_simd_scalar& impulse, const phys_2ang_jacobian& lin_ang_jac,
        phys_vel_accum& mA, phys_vel_accum& mB, phys_solver_elem_temp& temp)
    {
        __vector4 applied = __lvewx(&temp.m_applied_impulse, 0);
        __vector4 ang_dir_a = __vmulfp(__VMXGetReg(84), __VMXGetReg(88));
        __vector4 ang_dir_b = __vmulfp(__VMXGetReg(85), __VMXGetReg(91));
        __vector4 sum = __vaddfp(applied, impulse.get_quad());
        __VMXSetReg(87, __vmaddfp(impulse.get_quad(), ang_dir_a, __VMXGetReg(87)));
        __VMXSetReg(90, __vmaddfp(impulse.get_quad(), ang_dir_b, __VMXGetReg(90)));
        __stvewx(sum, &temp.m_applied_impulse, 0);
    }

    static __forceinline void get_dv01(const phys_1lin2ang_jacobian* lin_ang_jac, const phys_vel_accum& mA,
        const phys_vel_accum& mB, const phys_solver_info& info, phys_vec4& out)
    {
        load_jacobian(lin_ang_jac[0]);
        load_jacobian_ptr(lin_ang_jac);
        __vector4 r0 = get_lin_dv0(lin_ang_jac[0], mA, mB, info).get_quad();
        __vector4 r1 = lin_dv1_get(lin_ang_jac[1], mA, mB, info);
        __stvewx(r0, &out(0), 0);
        __stvewx(r1, &out(1), 0);
    }

    static __forceinline void get_dv012(const phys_triple_bilateral_ang_jacobian* jacobian, const phys_vel_accum& mA,
        const phys_vel_accum& mB, const phys_solver_info& info, phys_vec4& out)
    {
        load_jacobian(jacobian->m_jac_0);
        __vector4 r0 = get_lin_dv0(jacobian->m_jac_0, mA, mB, info).get_quad();
        __VMXSetReg(81, jacobian->m_jac_1.m_lin_part_0.m_simd_quad);
        __VMXSetReg(82, jacobian->m_jac_1.m_ang_jacobian[0].m_simd_quad);
        __VMXSetReg(83, jacobian->m_jac_1.m_ang_jacobian[1].m_simd_quad);
        __VMXSetReg(84, jacobian->m_jac_2.m_ang_jacobian[0].m_simd_quad);
        __VMXSetReg(85, jacobian->m_jac_2.m_ang_jacobian[1].m_simd_quad);
        __vector4 lin_delta1 = __vsubfp(__VMXGetReg(86), __VMXGetReg(89));
        __vector4 a1 = __vmulfp(__VMXGetReg(87), __VMXGetReg(82));
        __vector4 b1 = __vmulfp(__VMXGetReg(90), __VMXGetReg(83));
        __vector4 s1 = __vaddfp(b1, a1);
        s1 = __vmaddfp(lin_delta1, __VMXGetReg(81), s1);
        s1 = __vrlimi(s1, __VMXGetReg(81), 1, 0);
        __vector4 r1 = __vmsum4fp(__VMXGetReg(92), s1);
        __vector4 r2 = get_ang_dv0_no_load(jacobian->m_jac_2, mA, mB, info);
        __stvewx(r0, &out(0), 0);
        __stvewx(r1, &out(1), 0);
        __stvewx(r2, &out(2), 0);
    }

    static __forceinline void solve_single_contact(const phys_solver_info& info, const solver_step_temp& t,
        const phys_single_contact_schema& jacobian_schema, phys_vel_accum& mA, phys_vel_accum& mB,
        phys_solver_elem_temp* temp, phys_solver_elem_temp*& delayed)
    {
        load_jacobian(jacobian_schema.m_contact_jac);
        phys_simd_scalar dv = get_lin_dv0(jacobian_schema.m_contact_jac, mA, mB, info);
        if (float(dv) < 0.0f)
        {
            return;
        }
        phys_simd_scalar x = dv * jacobian_schema.m_contact_jac.get_inv_jac_diag_simd();
        apply_impulse0(x, jacobian_schema.m_contact_jac, mA, mB, temp[0]);
    }

    static __forceinline void solve_pair_contact(const phys_solver_info& info, const phys_pair_contact_schema& jacobian_schema,
        phys_vel_accum& mA, phys_vel_accum& mB, phys_solver_elem_temp* temp, phys_solver_elem_temp*& delayed)
    {
        phys_vec4 dv;
        get_dv01(&jacobian_schema.m_contact_jac.m_jac_0, mA, mB, info, dv);
        float dv_0 = dv(0);
        float dv_1 = dv(1);
        float x0 = jacobian_schema.m_contact_jac.m_jac_0.get_user_value() * dv_0 + jacobian_schema.m_inv_jac01 * dv_1;
        float x1 = jacobian_schema.m_contact_jac.m_jac_1.get_user_value() * dv_1 + jacobian_schema.m_inv_jac01 * dv_0;
        if (x0 > 0.0f)
        {
            if (x1 > 0.0f)
            {
                apply_impulse01(phys_simd_scalar(x0), phys_simd_scalar(x1), &jacobian_schema.m_contact_jac.m_jac_0, mA, mB, temp);
            }
            else
            {
            only_first:
                float y0 = jacobian_schema.m_contact_jac.m_jac_0.get_inv_jac_diag() * dv_0;
                if (y0 > 0.0f)
                {
                    apply_impulse0(phys_simd_scalar(y0), jacobian_schema.m_contact_jac.m_jac_0, mA, mB, temp[0]);
                }
            }
        }
        else
        {
            float y1 = jacobian_schema.m_contact_jac.m_jac_1.get_inv_jac_diag() * dv_1;
            if (y1 <= 0.0f)
            {
                goto only_first;
            }
            apply_impulse1(phys_simd_scalar(y1), jacobian_schema.m_contact_jac.m_jac_1, mA, mB, temp[1]);
        }
    }

    static __forceinline void solve_3d_friction(const phys_solver_info& info, const phys_3d_friction_schema& jacobian_schema,
        phys_vel_accum& mA, phys_vel_accum& mB, phys_solver_elem_temp* temp, phys_solver_elem_temp*& delayed)
    {
        phys_vec4 dv;
        get_dv012(&jacobian_schema.m_contact_jac, mA, mB, info, dv);
        float dv_0 = dv(0);
        float dv_1 = dv(1);
        float dv_2 = dv(2);
        float x2 = jacobian_schema.m_contact_jac.m_jac_2.get_inv_jac_diag() * dv_2;
        float x0 = jacobian_schema.m_contact_jac.m_jac_0.get_user_value() * dv_0 + jacobian_schema.m_inv_jac01 * dv_1;
        float x1 = jacobian_schema.m_contact_jac.m_jac_1.get_user_value() * dv_1 + jacobian_schema.m_inv_jac01 * dv_0;
        float max_friction_sq = jacobian_schema.m_max_friction_force * jacobian_schema.m_max_friction_force;
        float sq = x0 * x0 + x1 * x1 + x2 * x2;
        if (sq > max_friction_sq)
        {
            float f = jacobian_schema.m_max_friction_force * (1.0f / __fsqrts(sq));
            x0 *= f;
            x1 *= f;
            x2 *= f;
            temp[3].m_applied_impulse = f;
        }
        else
        {
            temp[3].m_applied_impulse = 1.0f;
        }
        x2 *= jacobian_schema.m_friction_radius;
        apply_impulse01(phys_simd_scalar(x0), phys_simd_scalar(x1), &jacobian_schema.m_contact_jac.m_jac_0, mA, mB, temp);
        apply_ang_impulse0(phys_simd_scalar(x2), jacobian_schema.m_contact_jac.m_jac_2, mA, mB, temp[2]);
    }

    static __forceinline void solve_2d_friction(const phys_solver_info& info, const phys_2d_friction_schema& jacobian_schema,
        phys_vel_accum& mA, phys_vel_accum& mB, phys_solver_elem_temp* temp, phys_solver_elem_temp*& delayed)
    {
        phys_vec4 dv;
        get_dv01(&jacobian_schema.m_contact_jac.m_jac_0, mA, mB, info, dv);
        float dv_0 = dv(0);
        float dv_1 = dv(1);
        float motor_max_force = jacobian_schema.m_max_friction_force;
        float max_friction_sq = motor_max_force * motor_max_force;
        float x0 = jacobian_schema.m_contact_jac.m_jac_0.get_user_value() * dv_0 + jacobian_schema.m_inv_jac01 * dv_1;
        float x1 = jacobian_schema.m_contact_jac.m_jac_1.get_user_value() * dv_1 + jacobian_schema.m_inv_jac01 * dv_0;
        float sq = x0 * x0 + x1 * x1;
        if (sq > max_friction_sq)
        {
            float f = __fsqrts(max_friction_sq / sq);
            x0 *= f;
            x1 *= f;
            temp[2].m_applied_impulse = f;
        }
        else
        {
            temp[2].m_applied_impulse = 1.0f;
        }
        apply_impulse01(phys_simd_scalar(x0), phys_simd_scalar(x1), &jacobian_schema.m_contact_jac.m_jac_0, mA, mB, temp);
    }

    static __forceinline void solve_1d_bilateral(const phys_solver_info& info, const solver_step_temp& t,
        const phys_1d_bilateral_schema& jacobian_schema, phys_vel_accum& mA, phys_vel_accum& mB,
        phys_solver_elem_temp* temp, phys_solver_elem_temp*& delayed)
    {
        load_jacobian(jacobian_schema.m_contact_jac);
        phys_simd_scalar dv = get_lin_dv0(jacobian_schema.m_contact_jac, mA, mB, info);
        phys_simd_scalar x = dv * jacobian_schema.m_contact_jac.get_inv_jac_diag_simd();
        apply_impulse0(x, jacobian_schema.m_contact_jac, mA, mB, temp[0]);
    }

    static __forceinline void solve_1d_bilateral_user_tau(const phys_solver_info& info, const solver_step_temp& t,
        const phys_1d_bilateral_user_tau_schema& jacobian_schema, phys_vel_accum& mA, phys_vel_accum& mB,
        phys_solver_elem_temp* temp, phys_solver_elem_temp*& delayed)
    {
        phys_simd_scalar dv = lin_dv0_user_tau_custom_rhs(jacobian_schema.m_contact_jac, mA, mB, info, jacobian_schema.m_query_tau,
            jacobian_schema.m_solver_damping, jacobian_schema.m_contact_jac.get_rhs());
        phys_simd_scalar x = dv * jacobian_schema.m_contact_jac.get_inv_jac_diag_simd();
        apply_impulse0(x, jacobian_schema.m_contact_jac, mA, mB, temp[0]);
    }

    static __forceinline void solve_1d_lin_limits(const phys_solver_info& info, const solver_step_temp& t,
        const phys_1d_lin_limits_schema& jacobian_schema, phys_vel_accum& mA, phys_vel_accum& mB,
        phys_solver_elem_temp* temp, phys_solver_elem_temp*& delayed)
    {
        load_jacobian(jacobian_schema.m_contact_jac);
        float dv = get_lin_dv0(jacobian_schema.m_contact_jac, mA, mB, info);
        float x = dv - jacobian_schema.m_min_limit;
        if (x > 0.0f)
        {
            x *= jacobian_schema.m_contact_jac.get_inv_jac_diag();
            apply_impulse0(phys_simd_scalar(x), jacobian_schema.m_contact_jac, mA, mB, temp[0]);
        }
        float y = dv - jacobian_schema.m_max_limit;
        if (y < 0.0f)
        {
            y *= jacobian_schema.m_contact_jac.get_inv_jac_diag();
            apply_impulse0(phys_simd_scalar(y), jacobian_schema.m_contact_jac, mA, mB, temp[0]);
        }
    }

    static __forceinline void solve_1d_friction(const phys_solver_info& info, const solver_step_temp& t,
        const phys_1d_friction_schema& jacobian_schema, phys_vel_accum& mA, phys_vel_accum& mB,
        phys_solver_elem_temp* temp, phys_solver_elem_temp*& delayed)
    {
        load_jacobian(jacobian_schema.m_contact_jac);
        float dv = get_lin_dv0(jacobian_schema.m_contact_jac, mA, mB, info);
        float x = jacobian_schema.m_contact_jac.get_inv_jac_diag() * dv;
        float abs_x = fabsf(x);
        if (abs_x > jacobian_schema.m_max_friction_force)
        {
            float f = jacobian_schema.m_max_friction_force / abs_x;
            temp[1].m_applied_impulse = f;
            x *= f;
        }
        else
        {
            temp[1].m_applied_impulse = 1.0f;
        }
        apply_impulse0(phys_simd_scalar(x), jacobian_schema.m_contact_jac, mA, mB, temp[0]);
    }

    static __forceinline void solve_1d_lin_motor(const phys_solver_info& info, const solver_step_temp& t,
        const phys_1d_lin_motor_schema& jacobian_schema, phys_vel_accum& mA, phys_vel_accum& mB,
        phys_solver_elem_temp* temp, phys_solver_elem_temp*& delayed)
    {
        const phys_1d_motor_solver_info* motor = &jacobian_schema.m_schema_motor;
        temp[2].m_applied_impulse = temp[2].m_applied_impulse + motor->m_query_tau * t.m_integrate_factor;
        float rhs = jacobian_schema.m_contact_jac.get_rhs() + temp[2].m_applied_impulse;
        float dv = lin_dv0_user_tau_custom_rhs(jacobian_schema.m_contact_jac, mA, mB, info, motor->m_solver_damping, motor->m_rhs_factor, rhs);
        float x = dv * jacobian_schema.m_contact_jac.get_inv_jac_diag();
        if (x > motor->m_min_impulse)
        {
            temp[1].m_applied_impulse = motor->m_min_impulse / x;
            x = motor->m_min_impulse;
        }
        else if (x < motor->m_max_impulse)
        {
            temp[1].m_applied_impulse = motor->m_max_impulse / x;
            x = motor->m_max_impulse;
        }
        else
        {
            temp[1].m_applied_impulse = 1.0f;
        }
        apply_impulse0(phys_simd_scalar(x), jacobian_schema.m_contact_jac, mA, mB, temp[0]);
    }

    static __forceinline void solve_pulley(const phys_solver_info& info, const phys_1d_pulley_schema& jacobian_schema,
        phys_vel_accum& mA, phys_vel_accum& mB, phys_solver_elem_temp* temp, phys_solver_elem_temp*& delayed)
    {
        phys_vec4 lin_a; lin_a.m_simd_quad = __vmulfp(__VMXGetReg(86), jacobian_schema.m_contact_jac.m_lin_parts[0].m_simd_quad);
        phys_vec4 lin_b; lin_b.m_simd_quad = __vmulfp(__VMXGetReg(89), jacobian_schema.m_contact_jac.m_lin_parts[1].m_simd_quad);
        phys_vec4 lin_part; lin_part.set_sub4(lin_a, lin_b);
        phys_vec4 ang_a; ang_a.m_simd_quad = __vmulfp(__VMXGetReg(87), jacobian_schema.m_contact_jac.m_ang_jacobian[0].m_simd_quad);
        phys_vec4 ang_b; ang_b.m_simd_quad = __vmulfp(__VMXGetReg(90), jacobian_schema.m_contact_jac.m_ang_jacobian[1].m_simd_quad);
        ang_a.add4(ang_b);
        ang_a.add4(lin_part);
        phys_simd_scalar sum = ang_a.horizontal_add3();
        phys_simd_scalar dv;
        dv.m_simd_scalar = __vspltw(jacobian_schema.m_contact_jac.m_lin_parts[0].m_simd_quad, 3);
        dv.m_simd_scalar = __vsubfp(dv.m_simd_scalar, sum.m_simd_scalar);
        __vector4 splat_a = __vspltw(__VMXGetReg(88), 3);
        phys_simd_scalar x = jacobian_schema.m_contact_jac.m_ang_jacobian[0].get_simd_at(3); x.m_simd_scalar = __vmulfp(dv.m_simd_scalar, x.m_simd_scalar);
        __vector4 applied = __lvewx(&temp[0].m_applied_impulse, 0);
        __vector4 splat_b = __vspltw(__VMXGetReg(91), 3);
        __vector4 lin_dir_a = __vmulfp(jacobian_schema.m_contact_jac.m_lin_parts[0].m_simd_quad, splat_a);
        __VMXSetReg(87, __vmaddfp(x.get_quad(), __vmulfp(jacobian_schema.m_contact_jac.m_ang_jacobian[0].m_simd_quad, __VMXGetReg(88)), __VMXGetReg(87)));
        __vector4 applied_sum = __vaddfp(applied, x.get_quad());
        __vector4 ang_dir_b = __vmulfp(jacobian_schema.m_contact_jac.m_ang_jacobian[1].m_simd_quad, __VMXGetReg(91));
        __vector4 lin_dir_b = __vmulfp(jacobian_schema.m_contact_jac.m_lin_parts[1].m_simd_quad, splat_b);
        __VMXSetReg(90, __vmaddfp(x.get_quad(), ang_dir_b, __VMXGetReg(90)));
        __VMXSetReg(86, __vmaddfp(x.get_quad(), lin_dir_a, __VMXGetReg(86)));
        __VMXSetReg(89, __vnmsubfp(x.get_quad(), lin_dir_b, __VMXGetReg(89)));
        __stvewx(applied_sum, &temp[0].m_applied_impulse, 0);
    }

    static __forceinline void solve_1d_ang(const phys_solver_info& info, const solver_step_temp& t,
        const phys_1d_ang_schema& jacobian_schema, phys_vel_accum& mA, phys_vel_accum& mB,
        phys_solver_elem_temp* temp, phys_solver_elem_temp*& delayed)
    {
        phys_simd_scalar dv = get_ang_dv0(jacobian_schema.m_contact_jac, mA, mB, info);
        phys_simd_scalar x = dv * jacobian_schema.m_contact_jac.get_inv_jac_diag_simd();
        apply_ang_impulse0(x, jacobian_schema.m_contact_jac, mA, mB, temp[0]);
    }

    static __forceinline void solve_1d_ang_limits(const phys_solver_info& info, const solver_step_temp& t,
        const phys_1d_ang_limits_schema& jacobian_schema, phys_vel_accum& mA, phys_vel_accum& mB,
        phys_solver_elem_temp* temp, phys_solver_elem_temp*& delayed)
    {
        float dv = get_ang_dv0_user_tau(jacobian_schema.m_contact_jac, mA, mB, info, jacobian_schema.m_query_tau, info.m_solver_damping);
        float x = dv - jacobian_schema.m_min_limit;
        if (x > 0.0f)
        {
            x = jacobian_schema.m_contact_jac.get_inv_jac_diag() * x;
            apply_ang_impulse0(phys_simd_scalar(x), jacobian_schema.m_contact_jac, mA, mB, temp[0]);
        }
        else
        {
            float y = dv - jacobian_schema.m_max_limit;
            if (y < 0.0f)
            {
                y = jacobian_schema.m_contact_jac.get_inv_jac_diag() * y;
                apply_ang_impulse0(phys_simd_scalar(y), jacobian_schema.m_contact_jac, mA, mB, temp[0]);
            }
        }
    }

    static __forceinline void solve_1d_ang_friction(const phys_solver_info& info, const solver_step_temp& t,
        const phys_1d_ang_friction_schema& jacobian_schema, phys_vel_accum& mA, phys_vel_accum& mB,
        phys_solver_elem_temp* temp, phys_solver_elem_temp*& delayed)
    {
        float dv = get_ang_dv0(jacobian_schema.m_contact_jac, mA, mB, info);
        float x = jacobian_schema.m_contact_jac.get_inv_jac_diag() * dv;
        float abs_x = fabsf(x);
        if (abs_x > jacobian_schema.m_max_friction_force)
        {
            float f = jacobian_schema.m_max_friction_force / abs_x;
            temp[1].m_applied_impulse = f;
            x *= f;
        }
        else
        {
            temp[1].m_applied_impulse = 1.0f;
        }
        apply_ang_impulse0(phys_simd_scalar(x), jacobian_schema.m_contact_jac, mA, mB, temp[0]);
    }

    static __forceinline void solve_1d_ang_motor(const phys_solver_info& info, const solver_step_temp& t,
        const phys_1d_ang_motor_schema& jacobian_schema, phys_vel_accum& mA, phys_vel_accum& mB,
        phys_solver_elem_temp* temp, phys_solver_elem_temp*& delayed)
    {
        const phys_1d_motor_solver_info* motor = &jacobian_schema.m_schema_motor;
        temp[2].m_applied_impulse = temp[2].m_applied_impulse + motor->m_query_tau * t.m_integrate_factor;
        float rhs = jacobian_schema.m_contact_jac.get_ang_rhs() + temp[2].m_applied_impulse;
        float dv = get_ang_dv0_user_tau_custom_rhs(jacobian_schema.m_contact_jac, mA, mB, info, motor->m_solver_damping, motor->m_rhs_factor, rhs);
        float x = jacobian_schema.m_contact_jac.get_inv_jac_diag() * dv;
        if (x > motor->m_min_impulse)
        {
            temp[1].m_applied_impulse = motor->m_min_impulse / x;
            x = motor->m_min_impulse;
        }
        else if (x < motor->m_max_impulse)
        {
            temp[1].m_applied_impulse = motor->m_max_impulse / x;
            x = motor->m_max_impulse;
        }
        else
        {
            temp[1].m_applied_impulse = 1.0f;
        }
        apply_ang_impulse0(phys_simd_scalar(x), jacobian_schema.m_contact_jac, mA, mB, temp[0]);
    }

    static __forceinline const phys_header_schema* get_solver_header(const phys_schema* schema)
    {
        return static_cast<const phys_header_schema*>(schema);
    }

    static __forceinline int step_jacobian(const phys_solver_info& info, solver_step_temp& t, const phys_schema* current_schema,
        phys_vel_accum* accum_array, phys_solver_elem_temp* elem_temp_in)
    {
        const phys_schema* jacobian_schema = current_schema;
        phys_solver_elem_temp* temp = elem_temp_in;
        phys_vel_accum* mA;
        phys_vel_accum* mB;
        phys_solver_elem_temp* delayed = 0;
        goto first_header;

        while (true)
        {
            current_schema = jacobian_schema;
            const phys_schema::schema_kind type = phys_schema::get_type_id(current_schema);
            switch (type)
            {
            case phys_schema::SCHEMA_KIND_SHIFT_RESULTS_PTR:
                {
                    const phys_header_schema* s = reinterpret_cast<const phys_header_schema*>(jacobian_schema);
                    jacobian_schema = reinterpret_cast<const phys_schema*>(s + 1);
                }
                break;

            case phys_schema::SCHEMA_KIND_SINGLE_CONTACT:
            case phys_schema::SCHEMA_KIND_SINGLE_CONTACT_CHECKED:
                while (true)
                {
                    const phys_single_contact_schema* s = reinterpret_cast<const phys_single_contact_schema*>(jacobian_schema);
                    solve_single_contact(info, t, *s, *mA, *mB, temp, delayed);
                    temp += phys_schema::get_solver_temp_count(jacobian_schema);
                    const phys_schema::schema_kind next_schema_type = phys_schema::get_type_id(reinterpret_cast<const phys_schema*>(s + 1));
                    jacobian_schema = reinterpret_cast<const phys_schema*>(s + 1);
                    if (next_schema_type != phys_schema::SCHEMA_KIND_SINGLE_CONTACT)
                    {
                        break;
                    }
                }
                break;

            case phys_schema::SCHEMA_KIND_PAIR_CONTACT:
            case phys_schema::SCHEMA_KIND_PAIR_CONTACT_CHECKED:
                {
                    const phys_pair_contact_schema* s = reinterpret_cast<const phys_pair_contact_schema*>(jacobian_schema);
                    solve_pair_contact(info, *s, *mA, *mB, temp, delayed);
                    temp += 2;
                    jacobian_schema = reinterpret_cast<const phys_schema*>(s + 1);
                }
                if (phys_schema::get_type_id(jacobian_schema) != phys_schema::SCHEMA_KIND_PAIR_CONTACT)
                {
                    continue;
                }
                {
                    const phys_pair_contact_schema* s = reinterpret_cast<const phys_pair_contact_schema*>(jacobian_schema);
                    solve_pair_contact(info, *s, *mA, *mB, temp, delayed);
                    temp += 2;
                    jacobian_schema = reinterpret_cast<const phys_schema*>(s + 1);
                }
                if (phys_schema::get_type_id(jacobian_schema) != phys_schema::SCHEMA_KIND_3D_FRICTION)
                {
                    break;
                }

            case phys_schema::SCHEMA_KIND_3D_FRICTION:
                {
                    const phys_3d_friction_schema* s = static_cast<const phys_3d_friction_schema*>(jacobian_schema);
                    solve_3d_friction(info, *s, *mA, *mB, temp, delayed);
                    temp += 4;
                    const phys_schema::schema_kind next_schema_type = phys_schema::get_type_id(s + 1);
                    jacobian_schema = s + 1;
                    if (next_schema_type != phys_schema::SCHEMA_KIND_HEADER)
                    {
                        break;
                    }
                    // Skip the 2D solve and fall through to the following header.
                    current_schema = 0;
                }

            case phys_schema::SCHEMA_KIND_2D_FRICTION:
                if (current_schema)
                {
                    const phys_2d_friction_schema* s = static_cast<const phys_2d_friction_schema*>(current_schema);
                    solve_2d_friction(info, *s, *mA, *mB, temp, delayed);
                    temp += 3;
                    const phys_schema::schema_kind next_schema_type = phys_schema::get_type_id(s + 1);
                    jacobian_schema = s + 1;
                    if (next_schema_type != phys_schema::SCHEMA_KIND_HEADER)
                    {
                        break;
                    }
                }

            case phys_schema::SCHEMA_KIND_HEADER:
                current_schema = jacobian_schema;
                prefetch_vel_accums(*get_solver_header(current_schema)->get_body_a(accum_array),
                    *get_solver_header(current_schema)->get_body_b(accum_array));
                store_vels(*mA, *mB);
            first_header:
                current_schema = jacobian_schema;
                mA = get_solver_header(current_schema)->get_body_a(accum_array);
                mB = get_solver_header(current_schema)->get_body_b(accum_array);
                load_vel_accums(*mA, *mB);
                jacobian_schema = get_solver_header(current_schema) + 1;
                break;

#define PHYS_STEP_SIMPLE(step_case, step_label, TYPE, step_solve_fn) \
            case phys_schema::step_case: \
            step_label: \
                { \
                    const TYPE* s = reinterpret_cast<const TYPE*>(jacobian_schema); \
                    step_solve_fn(info, t, *s, *mA, *mB, temp, delayed); \
                    temp += phys_schema::get_solver_temp_count(jacobian_schema); \
                    const phys_schema::schema_kind next_schema_type = phys_schema::get_type_id(reinterpret_cast<const phys_schema*>(s + 1)); \
                    jacobian_schema = reinterpret_cast<const phys_schema*>(s + 1); \
                    if (next_schema_type == phys_schema::step_case) \
                    { \
                        goto step_label; \
                    } \
                } \
                break;

            PHYS_STEP_SIMPLE(SCHEMA_KIND_1D_ANG_FRICTION, ang_friction, phys_1d_ang_friction_schema, solve_1d_ang_friction)
            PHYS_STEP_SIMPLE(SCHEMA_KIND_1D_ANG_LIMITS, ang_limits, phys_1d_ang_limits_schema, solve_1d_ang_limits)
            PHYS_STEP_SIMPLE(SCHEMA_KIND_1D_ANG_MOTOR, ang_motor, phys_1d_ang_motor_schema, solve_1d_ang_motor)
            PHYS_STEP_SIMPLE(SCHEMA_KIND_1D_ANG, angular, phys_1d_ang_schema, solve_1d_ang)
            PHYS_STEP_SIMPLE(SCHEMA_KIND_1D_BILATERAL, bilateral_step, phys_1d_bilateral_schema, solve_1d_bilateral)
            PHYS_STEP_SIMPLE(SCHEMA_KIND_1D_BILATERAL_USER_TAU, bilateral_user_tau, phys_1d_bilateral_user_tau_schema, solve_1d_bilateral_user_tau)
            PHYS_STEP_SIMPLE(SCHEMA_KIND_1D_FRICTION, friction, phys_1d_friction_schema, solve_1d_friction)
            PHYS_STEP_SIMPLE(SCHEMA_KIND_1D_LIN_LIMITS, lin_limits, phys_1d_lin_limits_schema, solve_1d_lin_limits)
            PHYS_STEP_SIMPLE(SCHEMA_KIND_1D_LIN_MOTOR, lin_motor, phys_1d_lin_motor_schema, solve_1d_lin_motor)

            case phys_schema::SCHEMA_KIND_1D_PULLEY:
                {
                    const phys_1d_pulley_schema* s = reinterpret_cast<const phys_1d_pulley_schema*>(jacobian_schema);
                    solve_pulley(info, *s, *mA, *mB, temp, delayed);
                    temp += 1;
                    jacobian_schema = reinterpret_cast<const phys_schema*>(s + 1);
                }
                break;

            case phys_schema::SCHEMA_KIND_SET_MASS:
                {
                    const phys_set_mass_schema* s = reinterpret_cast<const phys_set_mass_schema*>(jacobian_schema);
                    store_vels(*mA, *mB);
                    mA->m_inv_masses = s->m_inv_mass_a;
                    mB->m_inv_masses = s->m_inv_mass_b;
                    jacobian_schema = reinterpret_cast<const phys_schema*>(s + 1);
                    load_vel_accums(*mA, *mB);
                }
                break;

            case phys_schema::SCHEMA_KIND_ADD_VEL:
                {
                    const phys_add_vel_schema* s = reinterpret_cast<const phys_add_vel_schema*>(jacobian_schema);
                    store_vels(*mA, *mB);
                    phys_vel_accum* bodies[2];
                    bodies[0] = mA;
                    bodies[1] = mB;
                    phys_vel_accum* body = bodies[s->m_body_index];
                    phys_simd_scalar sr(info.m_integrate_vel_factor(0) * float(int(t.m_current_step)));
                    phys_vec4 x;
                    x.set_mul4(sr, s->m_velocity);
                    body->m_lin_vel.add4(s->m_velocity);
                    body->m_lin_vel.add4(x);
                    body->get_sum_lin_vel().add4(x);
                    jacobian_schema = reinterpret_cast<const phys_schema*>(s + 1);
                    load_vel_accums(*mA, *mB);
                }
                break;

            case phys_schema::SCHEMA_KIND_GOTO:
                {
                    const phys_goto_schema* s = reinterpret_cast<const phys_goto_schema*>(jacobian_schema);
                    jacobian_schema = reinterpret_cast<const phys_schema*>(reinterpret_cast<const char*>(jacobian_schema) + s->m_offset);
                }
                break;

            case phys_schema::SCHEMA_KIND_END:
                store_delayed_result(delayed);
                store_vels(*mA, *mB);
                return 0;

            case phys_schema::SCHEMA_KIND_STIFF_SPRING_CHAIN:
                {
                    store_vels(*mA, *mB);
                    const phys_stiff_spring_chain_schema* s = reinterpret_cast<const phys_stiff_spring_chain_schema*>(jacobian_schema);
                    phys_solver_stiff_spring_chain(info, &t, accum_array, const_cast<phys_stiff_spring_chain_schema*>(s), temp);
                    temp += s->m_constraint_count;
                    jacobian_schema = reinterpret_cast<const phys_schema *>(
                        reinterpret_cast<const char *>(jacobian_schema) + *reinterpret_cast<const int *>(reinterpret_cast<const char *>(s) + 4));
                    load_fixed_regs();
                    load_vel_accums(*mA, *mB);
                }
                break;

            case phys_schema::SCHEMA_KIND_BALL_SOCKET_CHAIN:
                {
                    store_vels(*mA, *mB);
                    const phys_ball_socket_chain_schema* s = reinterpret_cast<const phys_ball_socket_chain_schema*>(jacobian_schema);
                    phys_solver_ball_socket_chain(info, &t, accum_array,
                        reinterpret_cast<phys_schema *>(const_cast<phys_ball_socket_chain_schema *>(s)), temp);
                    temp += s->m_constraint_count * 3;
                    jacobian_schema = reinterpret_cast<const phys_schema *>(
                        reinterpret_cast<const char *>(jacobian_schema) + *reinterpret_cast<const int *>(reinterpret_cast<const char *>(s) + 4));
                    load_fixed_regs();
                    load_vel_accums(*mA, *mB);
                }
                break;

            case phys_schema::SCHEMA_KIND_POWERED_CHAIN:
                {
                    store_vels(*mA, *mB);
                    const phys_powered_chain_schema* s = reinterpret_cast<const phys_powered_chain_schema*>(jacobian_schema);
                    phys_solver_powered_chain(info, t, accum_array, const_cast<phys_powered_chain_schema*>(s), temp);
                    temp += s->m_constraint_count * 6;
                    jacobian_schema = reinterpret_cast<const phys_schema *>(
                        reinterpret_cast<const char *>(jacobian_schema) + *reinterpret_cast<const int *>(reinterpret_cast<const char *>(s) + 16));
                    load_fixed_regs();
                    load_vel_accums(*mA, *mB);
                }
                break;

            default:
                __debugbreak();
                return 0;
            }
        }
    }
}

// @address 0x66e730
extern "C" void phys_solve_step_jacobians(const phys_solver_info& info, const phys_schema* schema_list,
    phys_vel_accum* accum_array, phys_solver_elem_temp* temp)
{
    // match: 0x66e730: this register read keeps f12 first in the 3D friction multiply at 0x66f080.
    __VMXGetReg(94);
    phys_solver::load_fixed_regs();
    solver_step_temp t;
    t.m_integrate_factor = 0.0f;
    t.m_current_step = 0;
    phys_solver::step_jacobian(info, t, schema_list, accum_array, temp);
}

// ?HK_flyingcolors_solver@@3VhkBool@@A (owned by hkSolver.obj, .bss).
phys_bool PHYS_FLYINGCOLORS_SOLVER;

extern "C" {
// @address 0x7ceb74 g_phys_solver_check_count
int g_phys_solver_check_count;
}

namespace phys_solver
{
    // match: 0x670858: the trailing arguments keep f13 first in both coupled multiplies of the second contact pair.
    static __forceinline void solve_constraints(const phys_solver_info& info, const phys_schema* schema_list,
        phys_vel_accum* accum_array, phys_solver_elem_temp* temp, int, int)
    {
        phys_solver::load_fixed_regs();
        phys_vec4 gravity = info.m_gravity_per_substep;
        phys_solver::apply_vel_field(info, gravity, accum_array, 0);
        solver_step_temp step_temp;
        phys_vec4 factor; factor.set_all3(info.m_inv_steps * info.m_inv_integrate_vel_factor(0));
        for (int i = 0; i < info.m_step_count; i++)
        {
            step_temp.m_current_step = i;
            step_temp.m_integrate_factor = 1.0f;
            int m = info.m_micro_step_count;
            do
            {
                phys_debug_print_accums(i, info, accum_array, 0);
                phys_solver::step_jacobian(info, step_temp, schema_list, accum_array, temp);
                step_temp.m_integrate_factor = 0.0f;
            }
            while (--m > 0);
            bool is_last_step = false;
            if (i == info.m_step_count - 1)
            {
                is_last_step = true;
                phys_debug_print_accums(98, info, accum_array, 0);
            }
            phys_solver::integrate_vels(info, gravity, is_last_step, factor, accum_array, 0);
            if (i == info.m_step_count - 1)
            {
                phys_debug_print_accums(99, info, accum_array, 0);
            }
        }
    }
}

// @address 0x670858 phys_solve_constraints
extern "C" int phys_solve_constraints(const phys_solver_info& info, const phys_schema* schema_list,
    phys_vel_accum* accum_array, phys_solver_elem_temp* temp)
{
    if (!PHYS_FLYINGCOLORS_SOLVER)
    {
        PHYS_FLYINGCOLORS_SOLVER = reinterpret_cast<bool (*)(int)>(process_flying_colors)(0);
        if (!PHYS_FLYINGCOLORS_SOLVER)
            return 0;
    }
    phys_solver::solve_constraints(info, schema_list, accum_array, temp, 0, 0);
    return 1;
}

// @address 0x672fd0 phys_solve_constraints_steps
extern "C" int phys_solve_constraints_steps(const phys_solver_info& info, int cur_solver_step, int cur_micro_step,
    const phys_schema* schema_list, phys_vel_accum* accum_array, phys_solver_elem_temp* temp)
{
    if (!PHYS_FLYINGCOLORS_SOLVER)
    {
        PHYS_FLYINGCOLORS_SOLVER = reinterpret_cast<bool (*)(int)>(process_flying_colors)(0);
        if (!PHYS_FLYINGCOLORS_SOLVER)
            return 0;
    }
    phys_solver::load_fixed_regs();
    solver_step_temp step_temp;
    step_temp.m_current_step = cur_solver_step;
    float integrate_factor = (cur_micro_step != 0) ? 0.0f : 1.0f;
    step_temp.m_integrate_factor = integrate_factor;
    phys_solver::step_jacobian(info, step_temp, schema_list, accum_array, temp);
    return 1;
}

