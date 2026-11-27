#ifndef INCLUDE_PHYS_MULTITHREADED_SIMULATION_H
#define INCLUDE_PHYS_MULTITHREADED_SIMULATION_H

#include "phys/phys_continuous_simulation.h"
#include "phys/phys_broadphase_listener.h"
#include "phys/phys_typed_broadphase_handle_pair.h"
#include "phys/phys_critical_section.h"
#include "phys/phys_job_queue.h"
#include "phys/phys_multithread_config.h"
#include "phys/phys_semaphore.h"

#include "phys/phys_thread_token.h"

class phys_multithreaded_simulation : public phys_continuous_simulation {
public:
    PHYS_DECLARE_ALLOCATOR(PHYS_MEM_BASE_CLASS);
    phys_multithreaded_simulation(phys_world *world);
    virtual ~phys_multithreaded_simulation();
    virtual phys_job_queue *get_job_queue();
    virtual void get_multithread_config(phys_multithread_config &config);
    virtual void set_multithread_config(const phys_multithread_config &config);
    virtual void assert_no_agent_collision_info(phys_pair_agent_entry *pair_agent_entry);
    virtual void assert_no_entity_collision_info(phys_entity **entities, int solver_entity_count, phys_world *world);
    virtual phys_step_result step_delta_time(float body_delta_time);
    class mt_entity_pair_broadphase_listener : public phys_broadphase_listener {
    public:
        __forceinline mt_entity_pair_broadphase_listener() : m_world_simulation(0) {}
        virtual void add_collision_pair(phys_typed_broadphase_pair &pair);
        virtual void remove_collision_pair(phys_typed_broadphase_pair &pair);
        __forceinline virtual ~mt_entity_pair_broadphase_listener() {}
        phys_multithreaded_simulation *m_world_simulation;
    };
    class mt_volume_broadphase_listener : public phys_broadphase_listener {
    public:
        __forceinline mt_volume_broadphase_listener() : m_display_lock(0) {}
        virtual void add_collision_pair(phys_typed_broadphase_pair &pair);
        virtual void remove_collision_pair(phys_typed_broadphase_pair &pair);
        __forceinline virtual ~mt_volume_broadphase_listener() {}
        phys_critical_section *m_display_lock;
    };
    class mt_broadphase_border_listener : public phys_broadphase_listener {
    public:
        __forceinline mt_broadphase_border_listener() : m_display_lock(0) {}
        virtual void add_collision_pair(phys_typed_broadphase_pair &pair);
        virtual void remove_collision_pair(phys_typed_broadphase_pair &pair);
        __forceinline virtual ~mt_broadphase_border_listener() {}
        phys_critical_section *m_display_lock;
    };
    void reset_thread_tokens();
    phys_thread_token get_thread_token();
    void set_active_worker_count(int active_worker_count);
    void step_process_mt(const phys_thread_token &thread_token);
    void process_next_job(phys_job_queue::worker_thread_kind queue_thread_type);

    mt_entity_pair_broadphase_listener m_mt_entity_pair_listener;
    mt_volume_broadphase_listener m_mt_volume_listener;
    mt_broadphase_border_listener m_border_listener;
    phys_bool m_collect_collision_pairs;
    phys_array<phys_typed_broadphase_pair> m_added_pairs;
    phys_critical_section m_added_pairs_lock;
    phys_array<phys_typed_broadphase_pair> m_removed_pairs;
    phys_critical_section m_removed_pairs_lock;
    phys_multithread_config m_mt_config;
    int m_thread_token_count;
    phys_semaphore m_step_semaphore;
    int m_step_active_islands;
    int m_step_inactive_islands;
    __declspec(align(64)) phys_critical_section m_step_lock;
    __declspec(align(64)) phys_job_queue m_job_queue;
    __declspec(align(64)) phys_critical_section m_toi_queue_lock;
    __declspec(align(64)) phys_critical_section m_token_lock;
public:
    void step_begin_st(float body_delta_time);
    void step_end_st();
};
#if defined(_M_PPCBE) || defined(_XBOX)
typedef char phys_multithreaded_simulation_size[(sizeof(void *) != 4 || (sizeof(phys_multithreaded_simulation) == 1024)) ? 1 : -1];
typedef char phys_multithreaded_simulation_config_offset[(sizeof(void *) != 4 || (offsetof(phys_multithreaded_simulation, m_mt_config) == 248)) ? 1 : -1];
#endif

#endif
