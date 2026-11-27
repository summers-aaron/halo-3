#ifndef INCLUDE_PHYS_MULTITHREAD_CONFIG_H
#define INCLUDE_PHYS_MULTITHREAD_CONFIG_H

#include "phys/phys_base.h"

struct phys_multithread_config {
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_DYNAMICS, phys_multithread_config);
    phys_multithread_config();
    enum cpu_task_policy { HOST_ACCEPTS_OFFLOAD_TASKS, HOST_REFUSES_OFFLOAD_TASKS };
    unsigned int m_max_serial_constraints;
};

typedef char phys_multithread_config_size[(sizeof(phys_multithread_config) == 4) ? 1 : -1];

#endif
