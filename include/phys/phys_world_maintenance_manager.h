#ifndef INCLUDE_PHYS_WORLD_MAINTENANCE_MANAGER_H
#define INCLUDE_PHYS_WORLD_MAINTENANCE_MANAGER_H
#include "phys/phys_base.h"
class phys_world;
class phys_step_info;
class phys_world_maintenance_manager : public phys_ref_object {
public:
    PHYS_DECLARE_ALLOCATOR(PHYS_MEM_BASE_CLASS);
    virtual void init(phys_world *world) = 0;
    virtual void perform_maintenance(phys_world *world, phys_step_info &step_info) = 0;
    virtual void perform_maintenance_no_split(phys_world *world, phys_step_info &step_info) = 0;
};
#endif
