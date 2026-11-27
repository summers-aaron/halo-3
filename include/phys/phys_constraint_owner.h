#ifndef INCLUDE_PHYS_CONSTRAINT_OWNER_H
#define INCLUDE_PHYS_CONSTRAINT_OWNER_H

#include "phys/phys_constraint.h"

class phys_constraint_owner : public phys_ref_object {
public:
    PHYS_DECLARE_ALLOCATOR(PHYS_MEM_DYNAMICS);

    void add_constraint_info(phys_constraint *constraint, phys_constraint_info &delta);
    inline void sub_constraint_info(phys_constraint *constraint, phys_constraint_info &delta) {
        if (constraint->m_constraint_internal) {
            m_constraint_info.sub(delta);
            constraint->m_constraint_internal->sub_constraint_info(delta);
        }
    }

    virtual void add_constraint_to_locked_island(phys_constraint *constraint) {}
    virtual void remove_constraint_locked_island(phys_constraint *constraint) {}
    virtual void add_callback_request(phys_constraint *constraint, int request) {}
    virtual void check_access_rw() {}

    phys_constraint_info m_constraint_info;
};

typedef char phys_constraint_owner_size[(sizeof(void *) != 4 || ((sizeof(phys_constraint_owner) == 0x18) )) ? 1 : -1];
typedef char phys_constraint_owner_info_offset[(sizeof(void *) != 4 || ((offsetof(phys_constraint_owner, m_constraint_info) == 8) )) ? 1 : -1];

#endif
