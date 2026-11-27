#ifndef INCLUDE_PHYS_AGENT_CONFIG_H
#define INCLUDE_PHYS_AGENT_CONFIG_H

#include "phys/phys_base.h"

struct phys_agent_config {
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_AGENT, phys_agent_config);
    float m_iter_cast_early_out_dist;
    int m_iter_cast_max_iters;
    __forceinline phys_agent_config()
    {
        m_iter_cast_early_out_dist = 0.01f;
        m_iter_cast_max_iters = 20;
    }
};

typedef char phys_agent_config_size[(sizeof(phys_agent_config) == 8) ? 1 : -1];

#endif
