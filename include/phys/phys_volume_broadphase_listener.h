#ifndef INCLUDE_PHYS_VOLUME_BROADPHASE_LISTENER_H
#define INCLUDE_PHYS_VOLUME_BROADPHASE_LISTENER_H

#include "phys/phys_base.h"
#include "phys/phys_broadphase_listener.h"

class phys_volume_broadphase_listener : public phys_broadphase_listener {
public:
    phys_volume_broadphase_listener() {}
    virtual void add_collision_pair(phys_typed_broadphase_pair &pair);
    virtual void remove_collision_pair(phys_typed_broadphase_pair &pair);
};

#endif
