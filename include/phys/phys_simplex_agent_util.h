#ifndef INCLUDE_PHYS_SIMPLEX_AGENT_UTIL_H
#define INCLUDE_PHYS_SIMPLEX_AGENT_UTIL_H

// Functions defined in tu/hkinternal/hkGskAgentUtil.cpp that other units call.

struct phys_narrow_agent_input;
struct phys_agent_entry;
class phys_simplex_cache;
struct phys_simplex_manifold;
struct phys_process_collision_output;
class phys_vec4;

void phys_simplex_util_process_no_sweep(const phys_narrow_agent_input &input, phys_agent_entry *entry, void *agent_data, phys_simplex_cache &simplex_cache,
    phys_simplex_manifold &simplex_manifold, phys_vec4 &simplex_separating_normal, phys_process_collision_output &output);

#endif // HAVOK_HK_GSK_AGENT_UTIL_H
