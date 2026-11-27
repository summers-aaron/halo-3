#ifndef INCLUDE_PHYS_CHAIN_SCHEMA_INL_H
#define INCLUDE_PHYS_CHAIN_SCHEMA_INL_H

#include "phys/phys_chain_schema_types.h"
#include "phys/phys_chain_matrix_types.h"
#include "phys/phys_vel_accum_offset.h"

__forceinline phys_chain_triple *phys_stiff_spring_chain_schema::get_matrix_buffer()
{
    return reinterpret_cast<phys_chain_triple *>(get_jacobians(this) + m_constraint_count);
}

__forceinline float *phys_stiff_spring_chain_schema::get_temp_buffer()
{
    return reinterpret_cast<float *>(get_matrix_buffer() + m_constraint_count);
}

__forceinline phys_vel_accum_offset *phys_stiff_spring_chain_schema::get_accumulator_offsets_base()
{
    return reinterpret_cast<phys_vel_accum_offset *>(get_temp_buffer() + 1 + m_constraint_count);
}

__forceinline phys_chain_mat_triple *phys_ball_socket_chain_schema::get_matrix_buffer()
{
    return reinterpret_cast<phys_chain_mat_triple *>(phys_add_byte_offset(get_jacobians(this), m_constraint_count * 3 * sizeof(phys_1lin2ang_jacobian)));
}

__forceinline phys_vec4 *phys_ball_socket_chain_schema::get_temp_buffer()
{
    return reinterpret_cast<phys_vec4 *>(phys_add_byte_offset(get_matrix_buffer(), m_constraint_count * sizeof(phys_chain_mat_triple)));
}

__forceinline phys_vel_accum_offset *phys_ball_socket_chain_schema::get_accumulator_offsets_base()
{
    return reinterpret_cast<phys_vel_accum_offset *>(phys_add_byte_offset(get_temp_buffer(), (m_constraint_count + 1) * sizeof(phys_vec4)));
}

__forceinline phys_chain_mat6_triple *phys_powered_chain_schema::get_matrix_buffer()
{
    return reinterpret_cast<phys_chain_mat6_triple *>(
        phys_add_byte_offset(get_ang_jacobians(this, m_constraint_count), m_constraint_count * 3 * sizeof(phys_2ang_jacobian)));
}

__forceinline phys_vec8 *phys_powered_chain_schema::get_temp_buffer()
{
    return reinterpret_cast<phys_vec8 *>(phys_add_byte_offset(get_matrix_buffer(), m_constraint_count * sizeof(phys_chain_mat6_triple)));
}

__forceinline phys_vec8 *phys_powered_chain_schema::get_velocity_buffer()
{
    return get_temp_buffer() + 1 + m_constraint_count;
}

__forceinline phys_vel_accum_offset *phys_powered_chain_schema::get_accumulator_offsets_base()
{
    return reinterpret_cast<phys_vel_accum_offset *>(get_velocity_buffer() + m_constraint_count);
}

__forceinline phys_3d_ang_motor_solver_info *phys_powered_chain_schema::get_child_constraint_status_base()
{
    return reinterpret_cast<phys_3d_ang_motor_solver_info *>(get_accumulator_offsets_base() + m_constraint_count + 1);
}

#endif
