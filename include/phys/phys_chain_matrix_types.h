#ifndef INCLUDE_PHYS_CHAIN_MATRIX_TYPES_H
#define INCLUDE_PHYS_CHAIN_MATRIX_TYPES_H

#include "phys/phys_mat6_types.h"

struct phys_chain_triple {
    float m_lower;
    float m_diagonal_inv;
    float m_upper;
};

struct phys_chain_mat_triple {
    phys_vec4 m_lower[3];
    phys_vec4 m_diagonal_inv[3];
    phys_vec4 m_upper[3];
};

struct phys_chain_mat6_triple {
    phys_mat6 m_lower;
    phys_mat6 m_diagonal_inv;
    phys_mat6 m_upper;
    phys_mat6 m_mat_diag;
    phys_mat6 m_mat_next_offdiag;
};

typedef char phys_chain_triple_size[(sizeof(phys_chain_triple) == 12) ? 1 : -1];
typedef char phys_chain_mat_triple_size[(sizeof(phys_chain_mat_triple) == 144) ? 1 : -1];
typedef char phys_chain_mat6_triple_size[(sizeof(phys_chain_mat6_triple) == 960) ? 1 : -1];
typedef char phys_chain_mat6_diagonal_offset[(offsetof(phys_chain_mat6_triple, m_diagonal_inv) == 192) ? 1 : -1];
typedef char phys_chain_mat6_upper_offset[(offsetof(phys_chain_mat6_triple, m_upper) == 384) ? 1 : -1];
typedef char phys_chain_mat6_mat_diagonal_offset[(offsetof(phys_chain_mat6_triple, m_mat_diag) == 576) ? 1 : -1];
typedef char phys_chain_mat6_next_offset[(offsetof(phys_chain_mat6_triple, m_mat_next_offdiag) == 768) ? 1 : -1];

#endif
