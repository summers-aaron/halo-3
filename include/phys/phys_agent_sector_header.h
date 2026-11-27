#ifndef INCLUDE_PHYS_AGENT_SECTOR_HEADER_H
#define INCLUDE_PHYS_AGENT_SECTOR_HEADER_H
#include "phys/phys_constraint_data.h"
#include "phys/phys_physics_command_queue.h"
struct phys_agent_sector_header {
    struct sector_job_info {
        PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_DYNAMICS, sector_job_info);
        phys_constraint_info m_constraint_info;
        phys_fixed_size_command_queue<1024> m_command_queue;
    };
    int m_job_count;
    int m_open_job_count;
    sector_job_info *m_job_info[1];
    static __forceinline int get_sector_allocated_size(int job_count) { return (static_cast<short>(job_count) + 2) * 4; }
    __forceinline sector_job_info *get_job_info(int index) { return m_job_info[static_cast<short>(index)]; }
    static phys_agent_sector_header *allocate(int job_count);
    void deallocate();
};
typedef char phys_agent_job_info_size[(sizeof(phys_agent_sector_header::sector_job_info) == 1056) ? 1 : -1];
typedef char phys_agent_job_commands_offset[(offsetof(phys_agent_sector_header::sector_job_info, m_command_queue) == 16) ? 1 : -1];
#endif
