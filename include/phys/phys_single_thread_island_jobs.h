#ifndef INCLUDE_PHYS_SINGLE_THREAD_ISLAND_JOBS_H
#define INCLUDE_PHYS_SINGLE_THREAD_ISLAND_JOBS_H
#include "phys/phys_job_queue.h"
struct phys_mt_thread_state;
class phys_single_thread_island_jobs {
public:
    static phys_job_queue::job_status run_broadphase_job(phys_mt_thread_state &, phys_job_queue &, phys_job_queue::job_queue_entry &);
    static phys_job_queue::job_status run_jacobian_callback_job(phys_mt_thread_state &, phys_job_queue &,
        phys_job_queue::job_queue_entry &, phys_job_queue::job_wait_mode);
};
#endif
