#ifndef INCLUDE_PHYS_DEFAULT_MAINTENANCE_MANAGER_H
#define INCLUDE_PHYS_DEFAULT_MAINTENANCE_MANAGER_H
#include "phys/phys_world_maintenance_manager.h"
extern "C" { extern const float k_world_time_inv_numerator[]; }
class phys_default_maintenance_manager : public phys_world_maintenance_manager {
public:
    PHYS_DECLARE_ALLOCATOR(PHYS_MEM_BASE_CLASS);
    phys_default_maintenance_manager();
    virtual void init(phys_world *world);
    virtual void perform_maintenance(phys_world *world, phys_step_info &step_info);
    virtual void perform_maintenance_no_split(phys_world *world, phys_step_info &step_info);
private:
    void reset_world_time(phys_world *world, phys_step_info &step_info);
    void mark_islands_sleep_legacy(phys_world *world, phys_step_info &step_info);
protected:
    float m_min_allowed_time;
    float m_max_allowed_time;
};
typedef char phys_default_maintenance_size[(sizeof(void *) != 4 || ((sizeof(phys_default_maintenance_manager) == 16) )) ? 1 : -1];
#endif
