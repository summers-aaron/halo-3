#ifndef INCLUDE_PHYS_CPU_SPLIT_ISLAND_JOB_H
#define INCLUDE_PHYS_CPU_SPLIT_ISLAND_JOB_H
#include "phys/phys_job_queue.h"
struct phys_mt_thread_state;
class phys_island;
template <typename T> class phys_array;
phys_job_queue::job_status phys_cpu_split_island_job(phys_mt_thread_state &, phys_job_queue &, phys_job_queue::job_queue_entry &);
void phys_cpu_split_island_job_impl(phys_island *, phys_array<phys_island *> &);
#endif
