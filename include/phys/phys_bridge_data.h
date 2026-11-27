#ifndef INCLUDE_PHYS_BRIDGE_DATA_H
#define INCLUDE_PHYS_BRIDGE_DATA_H

#include <stddef.h>
#include "phys/phys_atoms.h"
#include "phys/phys_constraint_data.h"

class phys_bridge_data : public phys_constraint_data {
public:
    PHYS_DECLARE_ALLOCATOR(PHYS_MEM_CONSTRAINT);

    phys_bridge_atoms m_data_atoms;
    phys_constraint_data *m_constraint_data;
    unsigned short m_schemas_size;
    unsigned short m_solver_result_count;
};

typedef char phys_bridge_data_atoms_offset[(
    sizeof(void *) != 4 || (offsetof(phys_bridge_data, m_data_atoms) == 12)) ? 1 : -1];
typedef char phys_bridge_data_ptr_offset[(
    sizeof(void *) != 4 || (offsetof(phys_bridge_data, m_constraint_data) == 24)) ? 1 : -1];
typedef char phys_bridge_data_size[(
    sizeof(void *) != 4 || (sizeof(phys_bridge_data) == 32)) ? 1 : -1];

#endif // HAVOK_HK_BRIDGE_CONSTRAINT_DATA_H
