#ifndef INCLUDE_PHYS_JOB_QUEUE_UTIL_H
#define INCLUDE_PHYS_JOB_QUEUE_UTIL_H

#include "phys/phys_job_queue.h"
#include "phys/phys_collision_input.h"
#include "phys/phys_constraint_query_in.h"
#include "phys/phys_thread_memory.h"

class phys_world;
class phys_multithreaded_simulation;

struct phys_mt_thread_state {
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_DYNAMICS, phys_mt_thread_state);
    phys_mt_thread_state(phys_world *world, phys_multithreaded_simulation *simulation, phys_job_queue::worker_thread_kind type);

    phys_world *m_world;
    phys_multithreaded_simulation *m_world_simulation;
    phys_job_queue::worker_thread_kind m_thread_type;
    __declspec(align(16)) phys_process_collision_input m_collision_input;
    phys_constraint_query_in m_constraint_query_in;
};

typedef char phys_mt_thread_size[(sizeof(void *) != 4 || ((sizeof(phys_mt_thread_state) == 176) )) ? 1 : -1];
typedef char phys_mt_thread_type_offset[(sizeof(void *) != 4 || ((offsetof(phys_mt_thread_state, m_thread_type) == 8) )) ? 1 : -1];
typedef char phys_mt_thread_collision_offset[(sizeof(void *) != 4 || ((offsetof(phys_mt_thread_state, m_collision_input) == 16) )) ? 1 : -1];
typedef char phys_mt_thread_query_offset[(sizeof(void *) != 4 || ((offsetof(phys_mt_thread_state, m_constraint_query_in) == 80) )) ? 1 : -1];

class phys_job_queue_util {
public:
    static phys_job_queue::job_pop_result pop_dynamics_job(phys_job_queue &, phys_job_queue::job_queue_dynamic_data *,
        phys_job_queue::job_queue_entry &, phys_job_queue::job_queue_entry &);
    static phys_job_queue::job_creation_status finish_dynamics_job(phys_job_queue &, phys_job_queue::job_queue_dynamic_data *,
        const phys_job_queue::job_queue_entry &, phys_job_queue::job_queue_entry_input &);
};

#endif
