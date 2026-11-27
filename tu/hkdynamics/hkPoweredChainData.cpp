// hkdynamics/hkPoweredChainData.obj: from the records 0x64e3a0 0x64e3b8 0x64e3c0 0x64e460 0x64e4b8 0x64e4f0 0x64e538 0x64e580 0x64e680 0x64e6d0 0x64e778 0x64e7e0 0x64e8f8 0x64e910 0x64e940 0x64f118 0x64f120
// @flags /TP /O2 /Oc
// @compiler xdk6130
#include "phys/phys_algorithm.h"
// @address 0x64e3a0 ?destroy_loaded_powered_chain_data@@YAXPAX@Z
// @address 0x64e3b8 ?get_type_id@phys_powered_chain_data@@UBAHXZ
// @address 0x64e460 ?get_constraint_info@phys_powered_chain_data@@UBAXAAUconstraint_info@phys_constraint_data@@@Z
// @address 0x64e4b8 ?get_runtime_info@phys_powered_chain_data@@UBAXVphys_bool@@AAUruntime_info@phys_constraint_data@@@Z
// @address 0x64e4f0 ?enable_motor@phys_powered_chain_data@@SAXPAVphys_constraint_chain@@HH@Z
// @address 0x64e538 ?disable_chain_motor@phys_powered_chain_data@@SAXPAVphys_constraint_chain@@HH@Z
// @address 0x64e580 ?add_info_in_body_space@phys_powered_chain_data@@QAAXABVphys_vec4@@0ABVphys_quat@@PAVphys_motor@@22@Z
// @address 0x64e680 ?phys_1d_ang_vel_motor_commit_into_info@@YAXAAVphys_1d_motor_info@@ABVphys_constraint_query_in@@AAVphys_2ang_jacobian@@PAUphys_1d_motor_solver_info@@@Z
// @address 0x64e778 ??0phys_powered_chain_data@@QAA@Vphys_fixup_tag@@@Z
// @address 0x64e8f8 ?fixup_loaded_powered_chain_data@@YAXPAX@Z
// @address 0x64e910 ?get_powered_chain_data_vtable@@YAPBXXZ
// @address 0x64f120 ??_Gphys_powered_chain_data@@UAAPAXI@Z
#include <vectorintrinsics.h>
#include "physics/havok_halo_external.h"
#include "phys/phys_chain_schemas.h"
#include "phys/phys_math.h"
#include "phys/phys_1d_motor_solver_info.h"
#include "phys/phys_quat.h"
#include "phys/phys_jacobian_element.h"
#include "phys/phys_powered_chain_solver_util.h"
#include "phys/phys_1d_bilateral_info.h"

#include "phys/phys_constraint_chain.h"
#include "phys/phys_atoms.h"
#include "phys/phys_atom.h"
#include "phys/phys_simple_constraint_util.h"
#include "phys/phys_motor_constraint_info.h"
#include "phys/phys_chain_motor_solver_info.h"
#include "phys/phys_powered_chain_jacobian.h"
#include "phys/phys_powered_chain_data.h"
#include "phys/phys_constraint_query_in.h"
#include "phys/phys_constraint_query_out.h"
#include "phys/phys_vel_accum_offset.h"
#include "phys/phys_type_desc.h"

extern "C" {
// @address 0x7ceb30 g_phys_soft_contact_body_vels
phys_body_vel g_phys_soft_contact_body_vels[2];
}

void destroy_loaded_powered_chain_data(void *storage)
{
    static_cast<phys_powered_chain_data *>(storage)->~phys_powered_chain_data();
}
int phys_powered_chain_data::get_type_id() const
{
    return 102;
}
void phys_powered_chain_data::get_constraint_info(phys_constraint_data::constraint_info &info) const
{
    info.m_data_atoms = const_cast<phys_atom *>(m_data_atoms.get_atoms());
    info.m_all_atoms_size = m_data_atoms.get_size_of_all_atoms();
    info.clear();
    info.add_header_size();
    int solver_constraint_count = m_chain_infos.getSize();
    int schemas_size = solver_constraint_count * 3 * 48 + 32 + solver_constraint_count * 3 * 32 + solver_constraint_count * 960
        + (solver_constraint_count + 1) * 32 + solver_constraint_count * 32 + (solver_constraint_count + 1) * 4 + solver_constraint_count * 64;
    schemas_size = (schemas_size + 15) & ~15;
    int solver_result_count = solver_constraint_count * 6;
    info.add(schemas_size, solver_result_count, solver_result_count);
}
void phys_powered_chain_data::get_runtime_info(phys_bool want_runtime, phys_constraint_data::runtime_info &info) const
{
    int solver_constraint_count = m_chain_infos.getSize();
    const int results_per_constraint = 6;
    info.m_solver_result_count = solver_constraint_count * results_per_constraint;
    info.m_external_runtime_size = info.m_solver_result_count * sizeof(solver_result)
        + ((solver_constraint_count + 3) & ~3) + solver_constraint_count * 16;
}
void phys_powered_chain_data::enable_motor(phys_constraint_chain *instance, int constraint_idx, int chain_motor_index)
{
    phys_powered_chain_data *data = static_cast<phys_powered_chain_data *>(instance->get_chain_data());
    unsigned char *constraint_statuses = data->get_constraint_flags(instance->get_runtime());
    unsigned char mask = static_cast<unsigned char>(~(3 << (chain_motor_index * 2)));
    unsigned char status = static_cast<unsigned char>(0 << (chain_motor_index * 2));
    constraint_statuses[constraint_idx] &= mask;
    constraint_statuses[constraint_idx] |= status;
}
void phys_powered_chain_data::disable_chain_motor(phys_constraint_chain *instance, int constraint_idx, int chain_motor_index)
{
    phys_powered_chain_data *data = static_cast<phys_powered_chain_data *>(instance->get_chain_data());
    unsigned char *constraint_statuses = data->get_constraint_flags(instance->get_runtime());
    unsigned char mask = static_cast<unsigned char>(~(3 << (chain_motor_index * 2)));
    unsigned char status = static_cast<unsigned char>(2 << (chain_motor_index * 2));
    constraint_statuses[constraint_idx] &= mask;
    constraint_statuses[constraint_idx] |= status;
}
void phys_powered_chain_data::add_info_in_body_space(const phys_vec4 &joint_pivot_a, const phys_vec4 &joint_pivot_b, const phys_quat &a_from_c,
    phys_motor *first_motor, phys_motor *second_motor, phys_motor *third_motor)
{
    constraint_info &info = m_chain_infos.expand_one();
    info.m_pivot_in_a = joint_pivot_a;
    info.m_pivot_in_b = joint_pivot_b;
    info.m_a_to_c = a_from_c;
    info.m_axis_motors[0] = first_motor;
    first_motor->add_object_reference();
    info.m_axis_motors[1] = second_motor;
    second_motor->add_object_reference();
    info.m_axis_motors[2] = third_motor;
    third_motor->add_object_reference();
    info.m_swap_bodies = false;
    info.m_b_to_c = phys_quat::get_identity();
}
void phys_1d_ang_vel_motor_commit_into_info(phys_1d_motor_info &info, const phys_constraint_query_in &in,
    phys_2ang_jacobian &jacobian_elem, phys_1d_motor_solver_info *motor)
{
    motor->m_min_impulse = info.m_max_force * in.m_micro_step_dt.val();
    motor->m_max_impulse = info.m_min_force * in.m_micro_step_dt.val();
    motor->m_query_tau = info.m_target_vel;
    motor->m_solver_damping = info.m_query_tau;
    motor->m_rhs_factor = info.m_solver_damping;
    jacobian_elem.set_ang_rhs(in.m_substep_inv_dt * info.m_target_pos);
}
phys_powered_chain_data::phys_powered_chain_data(phys_fixup_tag flag)
    : phys_chain_data(flag), m_data_atoms(flag), m_chain_infos(flag)
{
    m_data_atoms.m_bridge_atom.init(this);
}
void fixup_loaded_powered_chain_data(void *storage)
{
    phys_fixup_tag flag;
    flag.m_fixup_finishing = 1;
    if (storage) {
        new (storage) phys_powered_chain_data(flag);
    }
}
const void *get_powered_chain_data_vtable()
{
    union { char storage[sizeof(phys_powered_chain_data)]; const void *pointer; } buffer;
    phys_fixup_tag flag;
    flag.m_fixup_finishing = 0;
    new (buffer.storage) phys_powered_chain_data(flag);
    return buffer.pointer;
}

// @address 0x64f118 phys_constraint_get_runtime
extern "C" phys_constraint_runtime *phys_constraint_get_runtime(phys_constraint_internal *internal)
{
    return internal->m_solver_runtime;
}
// @address 0x64e6d0 ??0phys_powered_chain_data@@QAA@XZ
phys_powered_chain_data::phys_powered_chain_data() : phys_chain_data(), m_chain_infos()
{
    m_query_tau = 0.6f;
    m_solver_damping = 1.0f;
    m_cfm_lin_add = 1.1920929e-08f;
    m_cfm_lin_mul = 1.0f;
    m_cfm_ang_add = 1.1920929e-08f;
    m_cfm_ang_mul = 1.0f;
    m_max_error_dist = 0.1f;
    m_data_atoms.m_bridge_atom.init(this);
}
// @address 0x64e3c0 ?estimate_angle_to_local@@YAXABVphys_quat@@0AAVphys_vec4@@@Z
void estimate_angle_to_local(const phys_quat &from, const phys_quat &to, phys_vec4 &angle_out)
{
    angle_out.set_cross(from.get_imag(), to.get_imag());
    angle_out.add_scaled4(to.m_quat_vec.get_simd_at(3), from.get_imag());
    angle_out.sub_mul4(from.m_quat_vec.get_simd_at(3), to.get_imag());
    angle_out.add4(angle_out);
    if (float(to.get_imag().dot4_simd(from.get_imag())) < 0.0f)
        angle_out.set_neg4(angle_out);
}
// @address 0x64e7e0 ??1phys_powered_chain_data@@UAA@XZ
phys_powered_chain_data::~phys_powered_chain_data()
{
    for (int i = 0; i < m_chain_infos.getSize(); i++) {
        for (int m = 0; m < 3; m++) {
            if (m_chain_infos[i].m_axis_motors[m])
                m_chain_infos[i].m_axis_motors[m]->object_remove_ref();
        }
    }
}
// @address 0x64e940 ?build_jacobian@phys_powered_chain_data@@UAAXABVphys_constraint_query_in@@AAVphys_constraint_query_out@@@Z
void phys_powered_chain_data::build_jacobian(const phys_constraint_query_in &unused_query_in, phys_constraint_query_out &out)
{
    phys_begin_constraints(unused_query_in, out, unused_query_in.m_constraint_runtime, sizeof(phys_solver_results));

    phys_constraint_query_in new_query_in = unused_query_in;
    new_query_in.m_query_constraint = 0;
    new_query_in.m_constraint_runtime = 0;

    phys_inline_array<phys_vel_accum_offset, 32> accum_array;
    {
        const phys_vel_accum *base_accum;
        phys_constraint_chain *chain_instance;
        {
            chain_instance = static_cast<phys_constraint_chain *>(unused_query_in.m_query_constraint.val());
            const phys_entity *chain_entity_a = chain_instance->get_entity_a();
            base_accum = phys_add_byte_offset_const(unused_query_in.m_body_a.val(), -int(chain_entity_a->m_solver_data));
        }

        const phys_array<phys_entity *> &entities = chain_instance->m_chained_entities;
        int solver_constraint_count = entities.getSize() - 1;

        phys_inline_array<phys_3d_ang_motor_solver_info, 32> motor_states;
        motor_states.set_array_size(solver_constraint_count);

        phys_entity *rB = entities[0];
        phys_motion *cB = rB->get_motion();
        new_query_in.m_body_b = phys_add_byte_offset_const(base_accum, rB->m_solver_data);
        accum_array.push_back_unchecked(phys_vel_accum_offset(base_accum, new_query_in.m_body_b));
        new_query_in.m_transform_b = &cB->get_transform();

        new_query_in.m_rhs_factor = new_query_in.m_substep_inv_dt;
        new_query_in.m_virt_mass_factor = 1.0f;

        phys_1lin2ang_jacobian *const lin_jacobians_base = phys_powered_chain_get_lin_jacobians(out.m_jacobian_schemas.val());
        phys_2ang_jacobian *const ang_jacobians_base = phys_powered_chain_get_ang_jacobians(out.m_jacobian_schemas.val(), solver_constraint_count);
        phys_1lin2ang_jacobian *lin_jacobians = lin_jacobians_base;
        phys_2ang_jacobian *ang_jacobians = ang_jacobians_base;

        for (int i = 0; i < solver_constraint_count; i++) {
            new_query_in.m_body_a = new_query_in.m_body_b;
            new_query_in.m_transform_a = new_query_in.m_transform_b;

            rB = entities[i + 1];
            cB = rB->get_motion();
            new_query_in.m_body_b = phys_add_byte_offset_const(base_accum, rB->m_solver_data);
            accum_array.array_push_back(phys_vel_accum_offset(base_accum, new_query_in.m_body_b));
            new_query_in.m_transform_b = &cB->get_transform();

            phys_vec4 pos_a;
            pos_a.set_transformed_pos(*new_query_in.m_transform_a, m_chain_infos[i].m_pivot_in_a);
            phys_vec4 pos_b;
            pos_b.set_transformed_pos(*new_query_in.m_transform_b, m_chain_infos[i].m_pivot_in_b);

            phys_load_vel_accums(&new_query_in);
            phys_stable_ball_socket_jacobian(pos_a, pos_b, m_max_error_dist, new_query_in, lin_jacobians);
            lin_jacobians += 3;

            {
                phys_quat rot_a;
                rot_a.set(new_query_in.m_transform_a->get_rotation());
                phys_quat rot_b;
                rot_b.set(new_query_in.m_transform_b->get_rotation());

                if (m_chain_infos[i].m_swap_bodies)
                    phys_algorithm::swap(rot_a, rot_b);

                {
                    phys_quat world_from_chain;
                    world_from_chain.set_mul(rot_b, m_chain_infos[i].m_b_to_c);
                    rot_b = world_from_chain;
                }

                const phys_quat &a_from_c = m_chain_infos[i].m_a_to_c;

                float *old_target_floats = &get_motor_runtime_quats(unused_query_in.m_constraint_runtime)[i * 4];
                phys_quat old_target_frame;
                old_target_frame.m_quat_vec.set(old_target_floats[0], old_target_floats[1], old_target_floats[2], old_target_floats[3]);

                if (float(old_target_frame.get_imag().length_sq_4d()) == 0.0f)
                    old_target_frame = a_from_c;

                phys_quat target_world_nb;
                target_world_nb.set_mul(rot_a, a_from_c);
                phys_quat target_world_ob;
                target_world_ob.set_mul(rot_a, old_target_frame);
                old_target_floats[0] = a_from_c.m_quat_vec(0);
                old_target_floats[1] = a_from_c.m_quat_vec(1);
                old_target_floats[2] = a_from_c.m_quat_vec(2);
                old_target_floats[3] = a_from_c.m_quat_vec(3);

                phys_vec4 delta_target;
                estimate_angle_to_local(rot_b, target_world_nb, delta_target);
                phys_vec4 position_error;
                estimate_angle_to_local(rot_b, target_world_ob, position_error);
                delta_target.sub4(position_error);

                if (m_chain_infos[i].m_swap_bodies) {
                    delta_target.set_neg3(delta_target);
                    position_error.set_neg3(position_error);
                }

                phys_rotation constraint_space;
                constraint_space.set(rot_b);

                for (int j = 0; j < 3; j++) {
                    const phys_vec4 constrained_dof_ws = constraint_space.get_column(j);
                    phys_solver_results *solver_results = get_solver_results(chain_instance->get_runtime());
                    {
                        phys_motor_input motor_in;
                        phys_1d_ang_vel_motor_begin_jacobian(constrained_dof_ws, new_query_in, ang_jacobians, motor_in);
                        motor_in.m_step_info = &new_query_in;
                        motor_in.m_last_results = solver_results[i * 6 + 3 + j];
                        motor_in.m_delta_target = delta_target(j);
                        motor_in.m_pos_error = position_error(j);

                        phys_motor_output motor_out;
                        phys_compute_motor_data(m_chain_infos[i].m_axis_motors[j], &motor_in, &motor_out);

                        phys_1d_motor_solver_info *solver_info = static_cast<phys_1d_motor_solver_info *>(
                            static_cast<void *>(&motor_states[i].m_motor_infos[j]));
                        phys_1d_ang_vel_motor_commit_into_info(motor_out, new_query_in, *ang_jacobians, solver_info);
                        ang_jacobians++;
                    }
                }
            }
        }

        {
            unsigned char *const child_status_flags = get_constraint_flags(chain_instance->get_runtime());
            for (int c = 0; c < solver_constraint_count; c++)
                motor_states[c].m_chain_broken = child_status_flags[c];

            phys_powered_chain_jacobian_params params;
            params.m_constraint_count = solver_constraint_count;
            params.m_chain_tau = m_query_tau;
            params.m_chain_damping = m_solver_damping;
            params.m_cfm_param.m_lin_add = m_cfm_lin_add;
            params.m_cfm_param.m_lin_mul = m_cfm_lin_mul;
            params.m_cfm_param.m_ang_add = m_cfm_ang_add;
            params.m_cfm_param.m_ang_mul = m_cfm_ang_mul;
            params.m_vel_accums = accum_array.begin();
            params.m_accums_base = base_accum;
            params.m_motors_state = motor_states.begin();
            params.m_max_torque_hyst = 0.0f;
            params.m_child_status_writeback_buf = child_status_flags;
            params.m_jacobians_end = ang_jacobians;

            phys_powered_chain_build_jacobian(params, unused_query_in, out);
        }
    }
}

const phys_type_desc phys_powered_chain_data_type_desc = phys_type_desc("hkPoweredChainData", fixup_loaded_powered_chain_data,
    destroy_loaded_powered_chain_data, get_powered_chain_data_vtable());
