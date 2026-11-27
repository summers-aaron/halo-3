#ifndef INCLUDE_PHYS_ISLAND_H
#define INCLUDE_PHYS_ISLAND_H

#include "phys/phys_dynamics.h"
#include "phys/phys_constraint_owner.h"
#include "phys/phys_pair_agent_track.h"

class phys_union_find;
class phys_continuous_simulation;
class phys_world_op_util;
class phys_world_callback_util;

class phys_island : public phys_constraint_owner {
public:
    PHYS_DECLARE_ALLOCATOR(PHYS_MEM_ISLAND);

    inline const phys_array<phys_entity *> &get_entities() const { return m_island_entities; }
    inline const phys_array<phys_action *> &get_actions() const { return m_actions; }
    inline phys_bool is_body_active() const { return m_active; }
    inline phys_bool body_is_fixed() const { return m_storage_idx == 0xffff; }
    inline phys_world *get_action_world() { return m_world; }
    inline int get_island_storage_index() { return m_storage_idx; }
    __forceinline int get_integration_mem_usage()
    {
        int fixed_part_size = (m_constraint_info.m_solver_temp_count << 2) + 8;
        int entity_span_bytes = (m_island_entities.getSize() << 7) + 144;
        return entity_span_bytes + fixed_part_size + m_constraint_info.m_schemas_size + 16;
    }
    virtual void gather_memory_stats(phys_statistics_collector *collector) const;
    phys_bool should_sleep_legacy(const phys_step_info &step_info);
    void mark_for_write();
    inline void unmark_for_write() { m_mt_lock.unmark_for_write(); }
    inline void mark_entities_read_only() const {}
    inline void unmark_entities_read_only() const {}
    phys_multi_thread_lock &get_multithread_lock() const { return m_mt_lock; }
    void is_valid_aabb();
    void merge_constraint_info(phys_island &other);

protected:
    friend class phys_world;
    friend class phys_world_op_util;
    friend class phys_world_callback_util;
    friend class phys_simulation;
    friend class phys_continuous_simulation;

    phys_island(phys_world *world);
    virtual ~phys_island();
    void add_entity_internal(phys_entity *entity);
    void remove_entity_internal(phys_entity *entity);
    virtual void add_constraint_to_locked_island(phys_constraint *constraint);
    virtual void remove_constraint_locked_island(phys_constraint *constraint);
    virtual void add_callback_request(phys_constraint *constraint, int request);
    void add_world_action(phys_action *action);
    void remove_action(phys_action *action);
    phys_bool is_fully_connected(phys_union_find &union_find_checker);

public:
    phys_world *m_world;
    int m_constraint_count;
    unsigned short m_storage_idx;
    unsigned short m_dirty_list_idx;
    unsigned char m_split_check_frames;
    unsigned char m_hf_sleep_counter;
    unsigned char m_low_freq_sleep_counter;
    // The console allocates bit-fields from the most significant bit and hkWorld.cpp writes these bytes whole
    // (m_state & 63 | 64 sets m_splitCheckRequested; m_activeFlags & 15 | 80 sets m_isInActiveIslandsArray and
    // m_active). Hosts that allocate from the least significant bit declare the fields in reverse so every flag
    // keeps its console bit.
    union {
        struct {
#if defined(_XBOX) && !defined(__clang__)
            bool m_split_check_requested : 2;
            bool m_sparse_enabled : 2;
            bool m_action_cleanup_needed : 2;
            bool m_allow_island_locking : 2;
#else
            bool m_allow_island_locking : 2;
            bool m_action_cleanup_needed : 2;
            bool m_sparse_enabled : 2;
            bool m_split_check_requested : 2;
#endif
        };
        // Byte access preserves combined flag updates in native SDK callers.
        unsigned char m_state;
    };
    union {
        unsigned char m_active_flags;
        struct {
#if defined(_XBOX) && !defined(__clang__)
            bool m_in_active_islands : 2;
            bool m_active : 2;
            bool m_in_integrate_job : 2;
#else
            bool : 2;
            bool m_in_integrate_job : 2;
            bool m_active : 2;
            bool m_in_active_islands : 2;
#endif
        };

        struct {
#if defined(_XBOX) && !defined(__clang__)
            unsigned char m_in_active_islands_bits : 2;
            unsigned char m_island_active_bits : 2;
            unsigned char m_in_integrate_job_bits : 2;
#else
            unsigned char : 2;
            unsigned char m_in_integrate_job_bits : 2;
            unsigned char m_island_active_bits : 2;
            unsigned char m_in_active_islands_bits : 2;
#endif
        };
    };

protected:
    mutable phys_multi_thread_lock m_mt_lock;

public:
    float m_since_high_freq_check;
    float m_since_low_freq_check;
    phys_array<phys_action *> m_actions;
    float m_sleep_time;
    phys_inline_array<phys_entity *, 1> m_island_entities;
    phys_pair_agent_track m_agent_track;
};

typedef char phys_island_size[(sizeof(void *) != 4 || ((sizeof(phys_island) == 0x70) )) ? 1 : -1];
typedef char phys_island_world_offset[(sizeof(void *) != 4 || (offsetof(phys_island, m_world) == 0x18)) ? 1 : -1];
typedef char phys_island_entities_offset[(sizeof(void *) != 4 || (offsetof(phys_island, m_island_entities) == 0x4c)) ? 1 : -1];
typedef char phys_island_track_offset[(sizeof(void *) != 4 || (offsetof(phys_island, m_agent_track) == 0x5c)) ? 1 : -1];

typedef char phys_island_state_offset[(sizeof(void *) != 4 || (offsetof(phys_island, m_state) == 0x27)) ? 1 : -1];
typedef char phys_island_active_flags_offset[(sizeof(void *) != 4 || (offsetof(phys_island, m_active_flags) == 0x28)) ? 1 : -1];

#endif
