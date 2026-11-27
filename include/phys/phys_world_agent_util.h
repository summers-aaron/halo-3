#ifndef INCLUDE_PHYS_WORLD_AGENT_UTIL_H
#define INCLUDE_PHYS_WORLD_AGENT_UTIL_H

class phys_linked_collider;
class phys_entity;
class phys_island;
struct phys_pair_agent_entry;
struct phys_process_collision_input;
struct phys_collision_input;

class phys_world_agent_util {
public:
    static phys_pair_agent_entry *add_pair_agent(phys_linked_collider *pair_collider_a, phys_linked_collider *pair_collider_b,
        const phys_process_collision_input &input);
    static void remove_agent(phys_pair_agent_entry *pair_agent_entry);
    static void remove_agent_and_toi_events(phys_pair_agent_entry *pair_agent_entry);
    static phys_island *island_from_agent_entry(phys_pair_agent_entry *entry,
        phys_island *candidate_island_a, phys_island *candidate_island_b);
    static void refilter_entity_compound(phys_entity *entity, phys_collision_input &input);
    static void invalidate_sweep_info(phys_entity *entity, phys_collision_input &input);
    static void warp_time(phys_island *island, float agent_old_time, float agent_new_time, phys_collision_input &input);
};

#endif
