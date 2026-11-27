#ifndef INCLUDE_PHYS_NARROW_AGENT_H
#define INCLUDE_PHYS_NARROW_AGENT_H

class phys_collision_proxy;
class phys_contact_manager;
class phys_constraint_owner;
class phys_dispatcher;
class phys_vec4;
struct phys_agent_entry;
struct phys_narrow_agent_input;
struct phys_narrow_agent_process_input;
struct phys_collision_input;
struct phys_process_collision_output;

namespace phys_narrow_agent {
    enum agent_stream_command {
        AGENT_STREAM_NULL = 0,
        AGENT_STREAM_END = 1,
        AGENT_STREAM_CALL = 2,
        AGENT_STREAM_CALL_FLIPPED = 3,
        AGENT_STREAM_CALL_SWEEP = 4,
        AGENT_STREAM_CALL_SWEEP_FLIPPED = 5,
        AGENT_STREAM_CALL_AGENT = 6
    };
    typedef int narrow_agent_kind;
    enum narrow_agent_symmetry { NARROW_AGENT_SYMMETRIC = 0, NARROW_AGENT_ASYMMETRIC = 1, NARROW_AGENT_ASYMMETRIC_FLIPPED = 2 };
    typedef void *(*agent_create_fn)(const phys_narrow_agent_input &, phys_agent_entry *, void *);
    typedef void (*agent_destroy_fn)(phys_agent_entry *, void *, phys_contact_manager *, phys_constraint_owner &, phys_dispatcher *);
    typedef void *(*agent_cleanup_fn)(phys_agent_entry *, void *, phys_contact_manager *, phys_constraint_owner &);
    typedef void (*agent_remove_point_fn)(phys_agent_entry *, void *, unsigned short);
    typedef void (*agent_commit_potential_fn)(phys_agent_entry *, void *, unsigned short);
    typedef void (*agent_create_zombie_fn)(phys_agent_entry *, void *, unsigned short);
    typedef void (*agent_update_filter_fn)(phys_agent_entry *, void *, phys_collision_proxy &, phys_collision_proxy &,
        const phys_collision_input &, phys_contact_manager *, phys_constraint_owner &);
    typedef void (*agent_invalidate_sweep_fn)(phys_agent_entry *, void *, phys_collision_input &);
    typedef void (*agent_warp_time_fn)(phys_agent_entry *, void *, float, float, phys_collision_input &);
    typedef void (*agent_sep_normal_fn)(const phys_narrow_agent_input &, void *, phys_vec4 &);
    typedef void *(*agent_process_fn)(const phys_narrow_agent_process_input &, phys_agent_entry *, void *, phys_vec4 *, phys_process_collision_output &);
}

#endif
