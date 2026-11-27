#ifndef INCLUDE_PHYS_COLLISION_INPUT_H
#define INCLUDE_PHYS_COLLISION_INPUT_H
#include "phys/phys_collide.h"
#include "phys/phys_step_info.h"
class phys_dispatcher;
class phys_convex_list_filter;
struct phys_agent_config;
struct phys_world_dynamics_step_info;

struct phys_collision_input {
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_AGENT, phys_collision_input);
    __forceinline float get_tolerance() const { return m_box_tolerance; }
    __forceinline void set_tolerance(float tolerance) { m_box_tolerance = tolerance; }
    phys_padded<phys_dispatcher *> m_agent_dispatcher;
    phys_padded<float> m_box_tolerance;
    phys_padded<const phys_compound_shape_filter *> m_compound_filter;
    phys_padded<const phys_convex_list_filter *> m_convex_list_filter;
    mutable phys_bool m_create_pred_agents;
};


struct phys_process_collision_input : public phys_collision_input {
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_AGENT, phys_process_collision_input);
    phys_step_info m_step_info;
    mutable phys_padded<phys_collision_quality_info *> m_quality_info;
    phys_world_dynamics_step_info *m_dynamics_info;
    bool m_legacy_edge_weld;
    phys_agent_config *m_agent_config;
};

struct phys_linear_cast_collision_input : public phys_collision_input {
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_AGENT, phys_linear_cast_collision_input);
    __forceinline phys_linear_cast_collision_input() : m_max_extra_penetration(1.192092896e-7f) {}
    __forceinline void set(const phys_process_collision_input &input) {
        phys_collision_input &collision_input = *this;
        collision_input = input;
        m_agent_config = input.m_agent_config;
    }
    __forceinline void set_path_and_tolerance(const phys_vec4 &path, float tolerance) {
        m_path = path;
        m_cached_path_length = path.length_3d();
        m_box_tolerance = tolerance;
    }
    phys_vec4 m_path;
    float m_max_extra_penetration;
    float m_cached_path_length;
    phys_agent_config *m_agent_config;
};
typedef char phys_collide_size_collision_input[(sizeof(void *) != 4 || ((sizeof(phys_collision_input) == 0x14) )) ? 1 : -1];
typedef char phys_collide_size_process_input[(sizeof(void *) != 4 || ((sizeof(phys_process_collision_input) == 0x40) )) ? 1 : -1];
typedef char phys_collide_size_linear_input[(sizeof(void *) != 4 || ((sizeof(phys_linear_cast_collision_input) == 0x40) )) ? 1 : -1];

#endif
