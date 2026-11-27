#ifndef INCLUDE_PHYS_CHAIN_SCHEMA_TYPES_H
#define INCLUDE_PHYS_CHAIN_SCHEMA_TYPES_H

#include "phys/phys_schema.h"
#include "tag_files/tag_types.h"
#include "phys/phys_jacobian_element.h"

struct phys_chain_triple;
struct phys_chain_mat_triple;
struct phys_chain_mat6_triple;
struct phys_3d_ang_motor_solver_info;
class phys_vec8;
class phys_vel_accum_offset;

class __declspec(align(16)) phys_stiff_spring_chain_schema : public phys_schema {
public:
#if defined(_XBOX) && !defined(__clang__)
    unsigned short m_constraint_count;
    unsigned char : 8;
    phys_schema::joint_schema_kind m_type;
#else
    phys_schema::joint_schema_kind m_type;
    unsigned short m_constraint_count;
#endif
    int m_schema_size;
    float m_query_tau;
    float m_solver_damping;

    static __forceinline phys_1lin2ang_jacobian *get_jacobians(phys_schema *jacobian_schema) {
        return reinterpret_cast<phys_1lin2ang_jacobian *>(reinterpret_cast<phys_stiff_spring_chain_schema *>(jacobian_schema) + 1);
    }
    phys_chain_triple *get_matrix_buffer();
    float *get_temp_buffer();
    phys_vel_accum_offset *get_accumulator_offsets_base();
};

class __declspec(align(16)) phys_ball_socket_chain_schema : public phys_schema {
public:
#if defined(_XBOX) && !defined(__clang__)
    unsigned short m_constraint_count;
    unsigned char : 8;
    phys_schema::joint_schema_kind m_type;
#else
    phys_schema::joint_schema_kind m_type;
    unsigned short m_constraint_count;
#endif
    int m_schema_size;
    float m_query_tau;
    float m_solver_damping;

    static __forceinline phys_1lin2ang_jacobian *get_jacobians(phys_schema *jacobian_schema) {
        return reinterpret_cast<phys_1lin2ang_jacobian *>(reinterpret_cast<phys_ball_socket_chain_schema *>(jacobian_schema) + 1);
    }
    phys_chain_mat_triple *get_matrix_buffer();
    phys_vec4 *get_temp_buffer();
    phys_vel_accum_offset *get_accumulator_offsets_base();
};

class __declspec(align(16)) phys_powered_chain_schema : public phys_schema {
public:
#if defined(_XBOX) && !defined(__clang__)
    unsigned short : 16;
    unsigned char : 8;
    phys_schema::joint_schema_kind m_type;
#else
    phys_schema::joint_schema_kind m_type;
    unsigned char : 8;
    unsigned short : 16;
#endif
    unsigned short m_constraint_count;
    TAG_POINTER(unsigned char) m_child_status_write_back;
    unsigned int : 32;
    int m_schema_size;
    float m_max_torque_hysteresis;
    float m_query_tau;
    float m_solver_damping;

    static __forceinline phys_1lin2ang_jacobian *get_lin_jacobians(phys_schema *jacobian_schema) {
        return reinterpret_cast<phys_1lin2ang_jacobian *>(reinterpret_cast<phys_powered_chain_schema *>(jacobian_schema) + 1);
    }
    static __forceinline phys_2ang_jacobian *get_ang_jacobians(phys_schema *jacobian_schema, int solver_constraint_count) {
        return reinterpret_cast<phys_2ang_jacobian *>(get_lin_jacobians(jacobian_schema) + solver_constraint_count * 3);
    }
    phys_chain_mat6_triple *get_matrix_buffer();
    phys_vec8 *get_temp_buffer();
    phys_vec8 *get_velocity_buffer();
    phys_vel_accum_offset *get_accumulator_offsets_base();
    phys_3d_ang_motor_solver_info *get_child_constraint_status_base();
    void *get_track_end();
    void init_powered_chain(int solver_constraint_count, phys_vel_accum_offset *accum_offsets,
        float tau, float damping, phys_3d_ang_motor_solver_info *motors, unsigned char *status_write_back);
};

// Buffer accessors need the chain matrix and motor records; their owners still carry private definitions.
typedef char phys_stiff_spring_chain_schema_size[(sizeof(phys_stiff_spring_chain_schema) == 16) ? 1 : -1];
typedef char phys_ball_socket_chain_schema_size[(sizeof(phys_ball_socket_chain_schema) == 16) ? 1 : -1];
typedef char phys_powered_chain_schema_size[(sizeof(void *) != 4 || ((sizeof(phys_powered_chain_schema) == 32) )) ? 1 : -1];
typedef char phys_stiff_spring_chain_schema_type_offset[(offsetof(phys_stiff_spring_chain_schema, m_type) == PHYS_SCHEMA_TAG_OFFSET) ? 1 : -1];
typedef char phys_ball_socket_chain_kind_offset[(offsetof(phys_ball_socket_chain_schema, m_type) == PHYS_SCHEMA_TAG_OFFSET) ? 1 : -1];
typedef char phys_powered_chain_schema_type_offset[(offsetof(phys_powered_chain_schema, m_type) == PHYS_SCHEMA_TAG_OFFSET) ? 1 : -1];
typedef char phys_powered_chain_schema_count_offset[(offsetof(phys_powered_chain_schema, m_constraint_count) == 4) ? 1 : -1];
typedef char phys_powered_chain_schema_status_offset[(offsetof(phys_powered_chain_schema, m_child_status_write_back) == 8) ? 1 : -1];
typedef char phys_powered_chain_schema_size_offset[(sizeof(void *) != 4 || ((offsetof(phys_powered_chain_schema, m_schema_size) == 16) )) ? 1 : -1];
typedef char phys_powered_chain_schema_hysteresis_offset[(sizeof(void *) != 4
    || ((offsetof(phys_powered_chain_schema, m_max_torque_hysteresis) == 20) )) ? 1 : -1];
typedef char phys_powered_chain_schema_tau_offset[(sizeof(void *) != 4 || ((offsetof(phys_powered_chain_schema, m_query_tau) == 24) )) ? 1 : -1];


#endif
