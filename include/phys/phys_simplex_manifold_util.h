#ifndef INCLUDE_PHYS_SIMPLEX_MANIFOLD_UTIL_H
#define INCLUDE_PHYS_SIMPLEX_MANIFOLD_UTIL_H

// Functions defined in tu/hkinternal/hkGskManifoldUtil.cpp that other units call.

class phys_constraint_owner;
class phys_contact_manager;
struct phys_simplex_manifold;
struct phys_simplex_manifold_work;
struct phys_process_collision_output;

extern "C" void phys_simplex_manifold_cleanup(phys_simplex_manifold *box_manifold, phys_contact_manager *contact_manager,
    phys_constraint_owner &manifold_constraint_owner);
extern "C" void phys_simplex_manifold_remove_point(phys_simplex_manifold *box_manifold, int index);
extern "C" void phys_simplex_manifold_verify_get_points(phys_simplex_manifold &box_manifold, const phys_simplex_manifold_work &work,
    unsigned int first_point_idx, phys_process_collision_output &out, phys_contact_manager *agent_contact_manager);

class phys_simplex_cache;
class phys_collision_proxy;
struct phys_process_collision_input;
class phys_process_hit_point;
extern "C" unsigned char phys_simplex_manifold_point_exists_resort(phys_simplex_manifold &box_manifold, const phys_simplex_cache &manifold_new_point);
extern "C" int phys_simplex_manifold_add_point(phys_simplex_manifold &box_manifold, const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b,
    const phys_process_collision_input &input, phys_process_collision_output &output, const phys_simplex_cache &manifold_new_point,
    phys_process_hit_point *new_hit_in_result_array, phys_process_hit_point *result_point_array,
        phys_contact_manager *pair_contact_manager, int contact_handling);

#endif // HAVOK_HK_GSK_MANIFOLD_UTIL_H
