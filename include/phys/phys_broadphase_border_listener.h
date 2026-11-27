#ifndef INCLUDE_PHYS_BROADPHASE_BORDER_LISTENER_H
#define INCLUDE_PHYS_BROADPHASE_BORDER_LISTENER_H

#include "phys/phys_base.h"
#include "phys/phys_broadphase_listener.h"

class phys_broadphase_border_listener : public phys_broadphase_listener {
public:
    phys_broadphase_border_listener() {}
    virtual void add_collision_pair(phys_typed_broadphase_pair &pair);
    virtual void remove_collision_pair(phys_typed_broadphase_pair &pair);
};

#endif
