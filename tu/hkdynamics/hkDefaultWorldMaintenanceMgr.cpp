// hkdynamics/hkDefaultWorldMaintenanceMgr.obj: from the records 0x65a858 0x65a870 0x65a8a0 0x65a908 0x65a958 0x65aac0
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x65a858 ?init@phys_default_maintenance_manager@@UAAXPAVphys_world@@@Z
// @address 0x65a870 ??0phys_default_maintenance_manager@@QAA@XZ
// @address 0x65a8a0 ?perform_maintenance@phys_default_maintenance_manager@@UAAXPAVphys_world@@AAVphys_step_info@@@Z
// @address 0x65a908 ?perform_maintenance_no_split@phys_default_maintenance_manager@@UAAXPAVphys_world@@AAVphys_step_info@@@Z
// @address 0x65aac0 ?mark_islands_sleep_legacy@phys_default_maintenance_manager@@AAAXPAVphys_world@@AAVphys_step_info@@@Z
#include "phys/phys_dynamics.h"



#include "phys/phys_collision_input.h"
#include "phys/phys_island.h"


#include "phys/phys_simulation.h"
#include "phys/phys_world_op_util.h"
// match: 0x65a958: external storage preserves hkDefaultWorldMaintenanceMgr::resetWorldTime code; owner: __real@3f800000
// @address 0xafc k_world_time_inv_numerator
#include "phys/phys_world_agent_util.h"
#include "phys/phys_default_maintenance_manager.h"

phys_default_maintenance_manager::phys_default_maintenance_manager()
{
    m_min_allowed_time = 33.0f;
    m_max_allowed_time = 63.0f;
}

void phys_default_maintenance_manager::init(phys_world *world)
{
    world->m_world_simulation->set_current_time(m_min_allowed_time);
    world->m_world_simulation->set_current_psi_time(m_min_allowed_time);
}

void phys_default_maintenance_manager::perform_maintenance(phys_world *world, phys_step_info &step_info)
{
    phys_world_op_util::split_islands(world);
    if (step_info.m_step_start_time >= m_max_allowed_time)
        reset_world_time(world, step_info);
    if (world->m_want_legacy_sleep)
        mark_islands_sleep_legacy(world, step_info);
}

void phys_default_maintenance_manager::perform_maintenance_no_split(phys_world *world, phys_step_info &step_info)
{
    if (step_info.m_step_start_time >= m_max_allowed_time)
        reset_world_time(world, step_info);
    if (world->m_want_legacy_sleep)
        mark_islands_sleep_legacy(world, step_info);
}

// @address 0x65a958 ?reset_world_time@phys_default_maintenance_manager@@AAAXPAVphys_world@@AAVphys_step_info@@@Z
void phys_default_maintenance_manager::reset_world_time(phys_world *world, phys_step_info &step_info)
{

    float step_delta_time = step_info.m_end_time - step_info.m_step_start_time;

    phys_step_info &new_step_info = world->m_dynamics_step_info.m_step_info;

    {
        new_step_info.m_step_start_time = m_min_allowed_time;
        new_step_info.m_end_time = m_min_allowed_time + step_delta_time;
        new_step_info.m_delta_time = step_delta_time;
        new_step_info.m_inv_dt = k_world_time_inv_numerator[0] / new_step_info.m_delta_time;
    }

    float warp_delta_time = new_step_info.m_step_start_time - step_info.m_step_start_time;

    {
        world->m_world_simulation->set_current_time(world->m_world_simulation->get_current_time() + warp_delta_time);
        world->m_world_simulation->set_current_psi_time(new_step_info.m_step_start_time);
    }

    if (world->m_world_simulation->get_simulate_until_time() != -1.0f)
    {
        world->m_world_simulation->set_simulate_until_time(world->m_world_simulation->get_simulate_until_time() + warp_delta_time);
    }

    const phys_array<phys_island *> &islands = world->get_active_islands();
    {
        for (int i = 0; i < islands.getSize(); i++)
        {
            phys_island *island = islands[i];

            for (int e = 0; e < island->m_island_entities.getSize(); e++)
            {
                phys_rigid_body *body = static_cast<phys_rigid_body *>(island->m_island_entities[e]);
                phys_motion_state *ms = body->get_rigid_motion()->get_motion_state();
                ms->get_swept_transform().m_center_of_mass0(3) += warp_delta_time;
            }

            phys_world_agent_util::warp_time(island, step_info.m_end_time, new_step_info.m_end_time, *world->m_collision_input);
        }
    }

    world->m_world_simulation->warp_time(warp_delta_time);

    step_info = new_step_info;
    world->m_collision_input->m_step_info = new_step_info;
}

void phys_default_maintenance_manager::mark_islands_sleep_legacy(phys_world *world, phys_step_info &step_info)
{
    world->m_min_island_size = 0;
    const phys_array<phys_island *> &world_active_islands = world->get_active_islands();
    for (int i = world_active_islands.getSize() - 1; i >= 0; --i)
    {
        phys_island *active_island = world_active_islands[i];
        if (active_island->should_sleep_legacy(step_info))
            phys_world_op_util::mark_island_inactive(world, active_island);
    }
}

