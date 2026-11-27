#ifndef INCLUDE_PHYS_DEFAULT_TOI_RESOURCE_MANAGER_H
#define INCLUDE_PHYS_DEFAULT_TOI_RESOURCE_MANAGER_H

#include "phys/phys_toi_resource_manager.h"

struct phys_toi_resources {
    int m_min_process_priority;
    int m_max_entities;
    int m_max_active_entities;
    int m_max_constraints;
    int m_toi_solver_iters;
    int m_forced_toi_final_iters;
    char *m_scratch_buffer;
    int m_scratch_buffer_size;

    __forceinline phys_toi_resources() {
        m_min_process_priority = 2;
        m_max_entities = 1000;
        m_max_active_entities = 1000;
        m_max_constraints = 1000;
        m_toi_solver_iters = 4;
        m_forced_toi_final_iters = 4;
        m_scratch_buffer = 0;
        m_scratch_buffer_size = 0;
    }
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char havok_size_hkToiResources[(sizeof(void *) != 4 || (sizeof(phys_toi_resources) == 32)) ? 1 : -1];
#endif

class phys_default_toi_resource_manager : public phys_toi_resource_manager {
public:
    PHYS_DECLARE_ALLOCATOR(PHYS_MEM_BASE_CLASS);
    phys_default_toi_resource_manager();
    virtual ~phys_default_toi_resource_manager();
    virtual phys_result begin_toi_setup_resources(const phys_toi_event &event,
        const phys_array<phys_toi_event> &other_events, phys_toi_resources &resources_out);
    virtual phys_toi_resource_response on_toi_unsolvable(phys_array<constraint_violation_info> &constraint_violations);
    virtual phys_toi_resource_response on_resources_depleted();
    virtual void end_toi_and_free_resources(const phys_toi_event &event,
        const phys_array<phys_toi_event> &other_events, const phys_toi_resources &resources);
    int m_default_scratchpad_size;
private:
    phys_bool should_handle_toi(const phys_toi_event &event);
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char phys_default_toi_resource_manager_size[(sizeof(void *) != 4 || (sizeof(phys_default_toi_resource_manager) == 12)) ? 1 : -1];
#endif

#endif
