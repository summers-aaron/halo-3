#ifndef INCLUDE_PHYS_WORLD_OP_UTIL_H
#define INCLUDE_PHYS_WORLD_OP_UTIL_H

#include "phys/phys_dynamics.h"

struct phys_pair_agent_track;

class phys_world_op_util {
public:
    enum callback_firing { OP_CALLBACKS_SUPPRESS, OP_CALLBACKS_FIRE };
    // March 2007 (0x834d58d0).
    static __forceinline int estimate_island_size(int solver_entity_count, int solver_constraint_count) {
        return phys_math::max_of_two(solver_entity_count, solver_constraint_count);
    }
    static void split_island(phys_island *cur_island, phys_world *world,
        phys_array<phys_island *> &new_islands_out, phys_array<phys_entity *> *old_entities_out);
    static void mark_island_active(phys_world *world, phys_island *island);
    static void validate_world(phys_world *world);
    static void add_entity_island(phys_world *, phys_entity *, phys_entity_activation);
    static void update_entity_broadphase(phys_world *, phys_entity *);
    static void mark_island_inactive_mt(phys_world *, phys_island *);
    static void set_rigid_body_motion_type(phys_rigid_body *, phys_motion::motion_kind, phys_entity_activation, phys_entity_filter_update_mode);
    static void detach_actions_fixed_island(phys_world *, phys_entity *, phys_array<phys_action *> &);
    static void detach_actions_dynamic_island(phys_world *, phys_entity *, phys_array<phys_action *> &);
    static void remove_entity_from_island(phys_world *, phys_entity *);
    static void add_entity_broadphase(phys_world *, phys_entity *);
    static void remove_entity_from_broadphase(phys_world *, phys_entity *);
    static void add_volume_broadphase(phys_world *, phys_volume *);
    static void remove_volume_from_broadphase(phys_world *, phys_volume *);
    static void remove_island(phys_world *world, phys_island *island);
    static void merge_islands(phys_world *world, phys_entity *listener_entity_a, phys_entity *listener_entity_b);
    static void split_islands(phys_world *world);
    static void split_island(phys_world *world, phys_island *island);
    static void deactivate_island_internal(phys_world *world, phys_island *island);
    static void cleanup_dirty_islands(phys_world *world);
    static void mark_island_inactive(phys_world *world, phys_island *island);
    static void remove_island_from_dirty_list(phys_world *world, phys_island *island);
    static void add_constraint_to_locked_island(phys_world *world, phys_constraint *constraint);
    static phys_constraint *add_constraint_now(phys_world *world, phys_constraint *constraint, callback_firing fire_pending_callbacks);
    static void remove_constraint_now(phys_world *world, phys_constraint *constraint, callback_firing fire_pending_callbacks);
    static void remove_constraint_locked_island(phys_world *world, phys_constraint *constraint);
private:
    static void detach_fixed_partner_agents(phys_pair_agent_track &track_to_scan,
        phys_entity *entity, phys_pair_agent_track &removed_agents, phys_motion::motion_kind new_motion_type);
    static void detach_constraints(phys_entity *, phys_array<phys_constraint *> &);
    static void replace_motion_object(phys_rigid_body *body, phys_motion::motion_kind new_motion_kind,
        phys_bool new_state_needs_inertia, phys_bool old_state_needs_inertia, phys_world *world);
public:
    static void add_actions_to_entity_island(phys_world *, phys_entity *, phys_array<phys_action *> &);
    static void activate_island_internal(phys_world *world, phys_island *island);
    static phys_island *merge_two_islands(phys_world *world, phys_island *island_a, phys_island *island_b);
    // March 0x834d5a68: merge only two non-fixed entities in different islands.
    static __forceinline void merge_islands_if_needed(phys_entity *listener_entity_a, phys_entity *listener_entity_b) {
        if (!listener_entity_a->body_is_fixed() && !listener_entity_b->body_is_fixed() &&
            listener_entity_a->get_island() != listener_entity_b->get_island()) {
            merge_islands(listener_entity_a->get_action_world(), listener_entity_a, listener_entity_b);
        }
    }
};

#endif
