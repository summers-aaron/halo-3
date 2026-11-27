#ifndef INCLUDE_PHYS_PAIR_AGENT_ENTRY_H
#define INCLUDE_PHYS_PAIR_AGENT_ENTRY_H

#include "phys/phys_agent_entry.h"
#include "tag_files/tag_types.h"
#include <stddef.h>

class phys_agent;
class phys_contact_manager;
class phys_linked_collider;

struct phys_pair_agent_entry : phys_agent_entry
{
    // The agent object of an old-style stream agent entry (hkAgentMachineUtil::calcNnStatistics 0x691a68 reports it
    // as the entry's "Agent"); the same slot as hkAgent1nMachineEntry::m_streamAgent.
    TAG_POINTER(phys_agent) m_stream_agent;
    unsigned short m_agent_index_on_collider[2];
    TAG_POINTER(phys_contact_manager) m_contact_manager;
    unsigned char m_quality_idx;
    unsigned char m_force_collide_ppu;
    unsigned char : 8;
    unsigned char : 8;
    TAG_POINTER(phys_linked_collider) m_collider[2];
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char phys_pair_agent_entry_size[(sizeof(phys_pair_agent_entry) == 28) ? 1 : -1];
typedef char phys_pair_entry_contact_manager_offset[(offsetof(phys_pair_agent_entry, m_contact_manager) == 12) ? 1 : -1];
#endif

#endif
