#ifndef INCLUDE_PHYS_CONTACT_POINT_PROPS_H
#define INCLUDE_PHYS_CONTACT_POINT_PROPS_H

#include "phys/phys_constraint_data.h"
#include "phys/phys_collide.h"

class phys_contact_point_props : public phys_solver_results, public phys_contact_point_material {
public:
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_CONSTRAINT_SOLVER, phys_contact_point_props);
    float m_internal_data_a;
};

typedef char phys_contact_point_props_size[(sizeof(void *) != 4 || ((sizeof(phys_contact_point_props) == 0x14))) ? 1 : -1];
typedef char phys_contact_point_props_internal_offset[(sizeof(void *) != 4 || ((offsetof(phys_contact_point_props, m_internal_data_a) == 0x10))) ? 1 : -1];

#endif
