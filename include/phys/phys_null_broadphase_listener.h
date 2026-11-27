#ifndef INCLUDE_PHYS_NULL_BROADPHASE_LISTENER_H
#define INCLUDE_PHYS_NULL_BROADPHASE_LISTENER_H

#include "phys/phys_broadphase_listener.h"

class phys_null_broadphase_listener : public phys_broadphase_listener {
public:
    __forceinline phys_null_broadphase_listener() {}
    virtual __forceinline ~phys_null_broadphase_listener() {}
    virtual void add_collision_pair(phys_typed_broadphase_pair &pair);
    virtual void remove_collision_pair(phys_typed_broadphase_pair &pair);
};

#endif
