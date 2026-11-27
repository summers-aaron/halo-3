#ifndef INCLUDE_PHYS_BVH_AGENT_SPECIALIZATIONS_H
#define INCLUDE_PHYS_BVH_AGENT_SPECIALIZATIONS_H
// Include after declaring the owning agent and its symmetric templates.
template <>
void phys_symmetric_agent_linear_cast<phys_bvh_agent>::static_penetrations(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_proxy_pair_collector &collector);
template <>
void phys_symmetric_agent_linear_cast<phys_bvh_agent>::static_closest_points(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_hit_collector &collector);
template <>
void phys_symmetric_agent_linear_cast<phys_bvh_agent>::static_sweep_cast(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_linear_cast_collision_input &input, phys_hit_collector &collector, phys_hit_collector *start_collector);
template <>
void phys_symmetric_agent_linear_cast<phys_bvh_agent>::get_closest_points(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_hit_collector &collector);
template <>
void phys_symmetric_agent<phys_bvh_agent>::sweep_cast(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_linear_cast_collision_input &input, phys_hit_collector &collector, phys_hit_collector *start_collector);
#endif
