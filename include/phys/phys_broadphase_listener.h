#ifndef INCLUDE_PHYS_BROADPHASE_LISTENER_H
#define INCLUDE_PHYS_BROADPHASE_LISTENER_H

#include "phys/phys_base.h"

class phys_typed_broadphase_pair;

class phys_broadphase_listener : public phys_ref_object {
public:
    PHYS_DECLARE_ALLOCATOR(PHYS_MEM_BROADPHASE);
    virtual ~phys_broadphase_listener() {}
    virtual void add_collision_pair(phys_typed_broadphase_pair &pair) = 0;
    virtual void remove_collision_pair(phys_typed_broadphase_pair &pair) = 0;
};

#endif
