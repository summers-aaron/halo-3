#ifndef INCLUDE_PHYS_PAIR_AGENT_TRACK_H
#define INCLUDE_PHYS_PAIR_AGENT_TRACK_H

#include "phys/phys_base.h"

struct phys_pair_agent_sector;

struct phys_pair_agent_track {
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_COLLISION_INFO, phys_pair_agent_track);

    // March 0x834d5be8 initializes the sector array before this store.
    phys_pair_agent_track() { m_last_sector_bytes = 512; }
    int get_sector_size(int track_sector_index) const {
        if (track_sector_index + 1 == m_track_sectors.getSize()) {
            return m_last_sector_bytes;
        }
        return 512;
    }

    unsigned int m_last_sector_bytes;
    phys_inline_array<phys_pair_agent_sector *, 1> m_track_sectors;
};

typedef char phys_pair_agent_track_size[(sizeof(void *) != 4 || ((sizeof(phys_pair_agent_track) == 0x14) )) ? 1 : -1];
typedef char phys_pair_agent_track_sectors_offset[(sizeof(void *) != 4 || offsetof(phys_pair_agent_track, m_track_sectors) == 4) ? 1 : -1];

#endif
