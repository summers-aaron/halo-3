// hkinternal/hkDefaultToiResourceMgr.obj: from the records 0x6b3da0 0x6b3db0 0x6b3db8 0x6b3dc0 0x6b3dd0 0x6b3df0 0x6b3ea8
// @compiler xdk3424
// @flags /TP /O2 /Oc
// @address 0x6b3da0 ?should_handle_toi@phys_default_toi_resource_manager@@AAA?AVphys_bool@@ABUphys_toi_event@@@Z
// @address 0x6b3db0 ?on_resources_depleted@phys_default_toi_resource_manager@@UAA?AW4phys_toi_resource_response@@XZ
// @address 0x6b3db8 ?on_toi_unsolvable@phys_default_toi_resource_manager@@UAA?AW4phys_toi_resource_response@@AAV?$phys_array@Uconstraint_violation_info@phys_toi_resource_manager@@@@@Z
// @address 0x6b3dc0 ??1phys_default_toi_resource_manager@@UAA@XZ
// @address 0x6b3dd0 ??0phys_default_toi_resource_manager@@QAA@XZ
#include "phys/phys_base_types.h"
#include "phys/phys_thread_memory.h"
#include "phys/phys_default_toi_resource_manager.h"

phys_bool phys_default_toi_resource_manager::should_handle_toi(const phys_toi_event &event)
{
    return phys_bool(true);
}

phys_toi_resource_response phys_default_toi_resource_manager::on_resources_depleted(void)
{
    return PHYS_TOI_RESOURCE_MANAGER_RESPONSE_NO_EXPAND_CONTINUE;
}

phys_toi_resource_response phys_default_toi_resource_manager::on_toi_unsolvable(
    phys_array<phys_toi_resource_manager::constraint_violation_info> &constraint_violations)
{
    return PHYS_TOI_RESOURCE_MANAGER_RESPONSE_CONTINUE;
}

phys_default_toi_resource_manager::~phys_default_toi_resource_manager()
{
}

phys_default_toi_resource_manager::phys_default_toi_resource_manager()
{
    m_object_ref_count = 1;
    m_default_scratchpad_size = 0x20000;
}

// @address 0x6b3df0 ?begin_toi_setup_resources@phys_default_toi_resource_manager@@UAA?AW4phys_result@@ABUphys_toi_event@@ABV?$phys_array@Uphys_toi_event@@@@AAUphys_toi_resources@@@Z
phys_result phys_default_toi_resource_manager::begin_toi_setup_resources(const phys_toi_event &, const phys_array<phys_toi_event> &, phys_toi_resources &out)
{
    int scratchpad_size = m_default_scratchpad_size;
    out.m_scratch_buffer_size = scratchpad_size;
    out.m_scratch_buffer = phys_alloc_stack<char>(scratchpad_size);
    out.m_max_active_entities = 1000;
    out.m_toi_solver_iters = 3;
    out.m_forced_toi_final_iters = 4;
    out.m_max_constraints = 1000;
    out.m_max_entities = 1000;
    out.m_min_process_priority = 2;
    return PHYS_SUCCESS;
}

// @address 0x6b3ea8 ?end_toi_and_free_resources@phys_default_toi_resource_manager@@UAAXABUphys_toi_event@@ABV?$phys_array@Uphys_toi_event@@@@ABUphys_toi_resources@@@Z
void phys_default_toi_resource_manager::end_toi_and_free_resources(const phys_toi_event &, const phys_array<phys_toi_event> &, const phys_toi_resources &in)
{
    phys_free_stack(in.m_scratch_buffer);
}

