#ifndef INCLUDE_PHYS_PAIR_AGENT_SECTOR_H
#define INCLUDE_PHYS_PAIR_AGENT_SECTOR_H

#include "phys/phys_thread_memory.h"

struct phys_pair_agent_entry;

struct phys_pair_agent_sector {
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_AGENT, phys_pair_agent_sector);
    unsigned char m_data[512];
    __forceinline phys_pair_agent_entry *get_track_begin() { return reinterpret_cast<phys_pair_agent_entry *>(m_data); }
    __forceinline phys_pair_agent_entry *get_track_end() { return reinterpret_cast<phys_pair_agent_entry *>(m_data + 512); }
};

typedef char phys_pair_agent_sector_size[(sizeof(phys_pair_agent_sector) == 512) ? 1 : -1];

#endif
