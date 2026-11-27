#ifndef INCLUDE_PHYS_CPU_INTEGRATE_JOB_H
#define INCLUDE_PHYS_CPU_INTEGRATE_JOB_H
#include "phys/phys_job_queue.h"
struct phys_mt_thread_state;
class phys_island;
struct phys_build_jacobian_task_header;
struct phys_build_jacobian_task_set;
struct phys_solve_jacobians_tasks;
phys_job_queue::job_status run_integrate_job(phys_mt_thread_state &, phys_job_queue &, phys_job_queue::job_queue_entry &);
phys_bool can_solve_single_threaded(const phys_build_jacobian_task_header &);
// match: 0x660f28: retain the accepted noalias contract when moving the task-collection declaration.
__declspec(noalias) void create_jacobian_task_set(phys_island *, phys_bool,
    phys_build_jacobian_task_header *, phys_build_jacobian_task_set *, phys_solve_jacobians_tasks *);
#endif
