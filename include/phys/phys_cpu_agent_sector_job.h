#ifndef INCLUDE_PHYS_CPU_AGENT_SECTOR_JOB_H
#define INCLUDE_PHYS_CPU_AGENT_SECTOR_JOB_H
#include "phys/phys_job_queue.h"
struct phys_mt_thread_state;
class phys_island;
phys_job_queue::job_status phys_cpu_agent_sector_job(phys_mt_thread_state &, phys_job_queue &, phys_job_queue::job_queue_entry &);
#endif
