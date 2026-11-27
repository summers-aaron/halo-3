#ifndef INCLUDE_PHYS_SYMMETRIC_AGENT_LINEAR_CAST_H
#define INCLUDE_PHYS_SYMMETRIC_AGENT_LINEAR_CAST_H

// The collectors that swap bodies A and B for a symmetric agent, defined in tu/hkcollide/hkSymmetricAgentLinearCast.cpp.

#include "phys/phys_collectors.h"
#include "phys/phys_collide.h"

class phys_symmetric_flip_collector : public phys_hit_collector {
public:
    // Shares the base collector's allocator.
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_AGENT, phys_hit_collector);
    phys_hit_collector &m_collector;
    phys_symmetric_flip_collector(phys_hit_collector &c) : m_collector(c) {}
    virtual void add_hit(const phys_hit_point &point);
    virtual void add_volume_callback_shape(const phys_collision_proxy &body);
};

class phys_symmetric_flip_cast_collector : public phys_hit_collector {
public:
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_AGENT, phys_symmetric_flip_cast_collector);
    phys_vec4 m_path;
    phys_hit_collector &m_collector;
    phys_symmetric_flip_cast_collector(const phys_vec4 &path, phys_hit_collector &c) : m_path(path), m_collector(c) {}
    virtual void add_hit(const phys_hit_point &point);
};

class phys_symmetric_flip_pair_collector : public phys_proxy_pair_collector {
public:
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_AGENT, phys_symmetric_flip_pair_collector);
    phys_proxy_pair_collector &m_collector;
    phys_symmetric_flip_pair_collector(phys_proxy_pair_collector &c) : m_collector(c) {}
    virtual void add_proxy_pair(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b);
};

#endif // HAVOK_HK_SYMMETRIC_AGENT_LINEAR_CAST_H
