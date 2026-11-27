#ifndef INCLUDE_PHYS_CHAIN_MOTOR_SOLVER_INFO_H
#define INCLUDE_PHYS_CHAIN_MOTOR_SOLVER_INFO_H

#include <stddef.h>

// Chain solver record used by hkPoweredChainSolverUtil, hkSolver and hkSolverExport.
// This is distinct from hk1dMotorSolverInfo; see field-disputes.tsv, hk1dMotorSolverInfo.
struct motor_1d_solver_info {
    float m_max_substep_impulse;
    float m_min_substep_impulse;
    float m_velocity;
    float m_query_tau;
    float m_solver_damping;
};

struct phys_3d_ang_motor_solver_info {
    unsigned char m_chain_broken;
    motor_1d_solver_info m_motor_infos[3];

    // March 0x8351dff8 / 0x8351e808, solve/hkSolverInfo.h:194,200.
    __forceinline int get_motor_solver_state(int idx) const {
        return (m_chain_broken >> (2 * idx)) & 3;
    }
    __forceinline void set_chain_state(int idx, int value) {
        const int shifted_value = (value & 3) << (2 * idx);
        const int shifted_mask = 3 << (2 * idx);
        m_chain_broken &= static_cast<unsigned char>(~shifted_mask);
        m_chain_broken |= static_cast<unsigned char>(shifted_value);
    }
};

typedef char phys_chain_motor_solver_info_size[(sizeof(motor_1d_solver_info) == 20) ? 1 : -1];
typedef char phys_3d_ang_motor_info_size[(sizeof(phys_3d_ang_motor_solver_info) == 64) ? 1 : -1];
typedef char phys_3d_ang_motor_info_motors_offset[(offsetof(phys_3d_ang_motor_solver_info, m_motor_infos) == 4) ? 1 : -1];

#endif // INCLUDE_HAVOK_HK_CHAIN_MOTOR_SOLVER_INFO_H
