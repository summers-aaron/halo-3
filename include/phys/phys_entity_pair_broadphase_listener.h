#ifndef INCLUDE_PHYS_ENTITY_PAIR_BROADPHASE_LISTENER_H
#define INCLUDE_PHYS_ENTITY_PAIR_BROADPHASE_LISTENER_H
#include "phys/phys_broadphase_listener.h"
class phys_world;
class phys_entity_pair_broadphase_listener : public phys_broadphase_listener {
public:
    __forceinline virtual ~phys_entity_pair_broadphase_listener() {}
    phys_entity_pair_broadphase_listener(phys_world *world);
    virtual void add_collision_pair(phys_typed_broadphase_pair &pair);
    virtual void remove_collision_pair(phys_typed_broadphase_pair &pair);
    phys_world *m_world;
};
#endif
