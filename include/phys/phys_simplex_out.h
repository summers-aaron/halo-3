#ifndef INCLUDE_PHYS_SIMPLEX_OUT_H
#define INCLUDE_PHYS_SIMPLEX_OUT_H

#include <stddef.h>

#include "phys/phys_math.h"

struct phys_simplex_out {
    phys_vec4 m_normal_in_a;
    phys_vec4 m_point_on_a;
    phys_padded<float> m_distance;
};

typedef char havok_size_hkGskOut[(sizeof(phys_simplex_out) == 48) ? 1 : -1];
typedef char havok_offset_hkGskOut_distance[(offsetof(phys_simplex_out, m_distance) == 32) ? 1 : -1];

struct phys_extended_simplex_out : public phys_simplex_out {
    phys_vec4 m_point_b_in_b;
    phys_vec4 m_normal_in_world;
};

typedef char havok_size_hkExtendedGskOut[(sizeof(phys_extended_simplex_out) == 80) ? 1 : -1];
typedef char havok_offset_hkExtendedGskOut_point[(offsetof(phys_extended_simplex_out, m_point_b_in_b) == 48) ? 1 : -1];
typedef char havok_offset_hkExtendedGskOut_normal[(offsetof(phys_extended_simplex_out, m_normal_in_world) == 64) ? 1 : -1];

#endif
