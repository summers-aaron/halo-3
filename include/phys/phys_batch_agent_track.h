#ifndef INCLUDE_PHYS_BATCH_AGENT_TRACK_H
#define INCLUDE_PHYS_BATCH_AGENT_TRACK_H

#include "phys/phys_base.h"

// One 512-byte block of the 1n agent stream: a 16-byte header and the packed agent entries.
struct phys_batch_agent_sector {
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_AGENT, phys_batch_agent_sector);

    enum { SECTOR_BYTE_SIZE = 512, SECTOR_PAYLOAD_SIZE = SECTOR_BYTE_SIZE - 16 };

    unsigned int m_bytes_allocated;
    unsigned int m_track_pad0;
    unsigned int m_track_pad1;
    unsigned int m_track_pad2;
    unsigned char m_data[SECTOR_PAYLOAD_SIZE];

    __forceinline void *get_track_begin() { return &m_data[0]; }
    inline void *get_track_end() { return &m_data[m_bytes_allocated]; }
    inline int get_sector_capacity() { return SECTOR_PAYLOAD_SIZE; }
    inline int get_free_bytes() { return static_cast<int>(SECTOR_PAYLOAD_SIZE - m_bytes_allocated); }
    inline phys_batch_agent_sector() : m_bytes_allocated(0) {}
};

typedef char phys_batch_agent_sector_size[(sizeof(phys_batch_agent_sector) == phys_batch_agent_sector::SECTOR_BYTE_SIZE) ? 1 : -1];

// March 0x834953e0/0x834953b8 construct and destroy this sector array at offset zero.
struct phys_batch_agent_track {
    static __forceinline void *operator new(size_t, void *storage) { return storage; }
    phys_array<phys_batch_agent_sector *> m_track_sectors;
};

typedef char phys_batch_agent_track_size[(sizeof(void *) != 4 || ((sizeof(phys_batch_agent_track) == 12) )) ? 1 : -1];

#endif
