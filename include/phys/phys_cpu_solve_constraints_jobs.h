#ifndef INCLUDE_PHYS_CPU_SOLVE_CONSTRAINTS_JOBS_H
#define INCLUDE_PHYS_CPU_SOLVE_CONSTRAINTS_JOBS_H
#include "phys/phys_job_queue.h"
struct phys_mt_thread_state;
class phys_island;
phys_job_queue::job_status phys_cpu_solve_apply_gravity_job(phys_mt_thread_state &, phys_job_queue &,
    phys_job_queue::job_queue_entry &, phys_job_queue::job_wait_mode);
phys_job_queue::job_status phys_cpu_solve_constraint_batch_job(phys_mt_thread_state &, phys_job_queue &,
    phys_job_queue::job_queue_entry &, phys_job_queue::job_wait_mode);
phys_job_queue::job_status phys_cpu_solve_integrate_vels_job(phys_mt_thread_state &, phys_job_queue &,
    phys_job_queue::job_queue_entry &, phys_job_queue::job_wait_mode);
phys_job_queue::job_status phys_cpu_solve_export_results_job(phys_mt_thread_state &, phys_job_queue &,
    phys_job_queue::job_queue_entry &, phys_job_queue::job_wait_mode);
phys_job_queue::job_status phys_cpu_solve_constraints_job(phys_mt_thread_state &, phys_job_queue &, phys_job_queue::job_queue_entry &,
    phys_job_queue::job_wait_mode);
#endif
