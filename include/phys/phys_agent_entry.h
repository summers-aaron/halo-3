#ifndef INCLUDE_PHYS_AGENT_ENTRY_H
#define INCLUDE_PHYS_AGENT_ENTRY_H

#include "phys/phys_base_types.h"

struct phys_agent_entry {
    unsigned char m_stream_command;
    unsigned char m_agent_type;
    unsigned char m_contact_point_count;
    unsigned char m_size;
};

// GSK agents use two bytes following the common stream header: cylinder flags at +4
// and the contact-TIM disable flag at +5 (hkPredGskAgent3::create).
struct phys_simplex_agent_entry : phys_agent_entry {
    unsigned char m_user_data;
    unsigned char m_disable_contact_sweeps;
};

typedef char havok_size_hkGskAgentEntry[(sizeof(phys_simplex_agent_entry) == 6) ? 1 : -1];

typedef char havok_size_hkAgentEntry[(sizeof(phys_agent_entry) == 4) ? 1 : -1];

#endif
