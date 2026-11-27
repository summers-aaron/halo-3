#ifndef INCLUDE_PHYS_CONTACT_MANAGER_FACTORY_H
#define INCLUDE_PHYS_CONTACT_MANAGER_FACTORY_H

#include "phys/phys_object.h"

class phys_collider;
class phys_contact_manager;
struct phys_collision_input;

class phys_contact_manager_factory : public phys_ref_object {
public:
    PHYS_DECLARE_ALLOCATOR(PHYS_MEM_CONTACT);
    virtual phys_contact_manager *create_contact_manager(const phys_collider &a, const phys_collider &b, const phys_collision_input &input) = 0;
};

#endif
