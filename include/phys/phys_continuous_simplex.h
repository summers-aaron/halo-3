#ifndef INCLUDE_PHYS_CONTINUOUS_SIMPLEX_H
#define INCLUDE_PHYS_CONTINUOUS_SIMPLEX_H

// Functions defined in tu/hkinternal/hkContinuousGsk.cpp that other units call.

struct phys_narrow_agent_process_input;
class phys_simplex_cache;
struct phys_process_collision_output;
class phys_vec4;

extern "C" void phys_4d_simplex_collide_toi(const phys_narrow_agent_process_input &narrow_input, const float allowed_penetration_depth,
    const float min_separation,
    const float toi_separation, phys_simplex_cache &simplex_cache, phys_vec4 &simplex_separating_normal, phys_process_collision_output &output);

#endif // HAVOK_HK_CONTINUOUS_GSK_H
