// hkdynamics/hkWorldCinfo.obj: from the records 0x63c720 0x63c738 0x63c890 0x63ca48 0x63ca90 0x63cad0 0x63cb10
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "phys/phys_dynamics.h"
#include "phys/phys_type_desc.h"
// @address 0x60a380 ?free_chunk@phys_thread_memory@@QAAXPAXHW4phys_memory_class@@@Z
// @address 0x5bec98 ?gather_memory_stats@phys_ref_object@@UBAXPAVphys_statistics_collector@@@Z
// @address 0x63c720 ?destroy_loaded_world_desc@@YAXPAX@Z
// @address 0x63ca48 ?set_broadphase_world_size@phys_world_desc@@QAAXM@Z
// @address 0x63ca90 ??0phys_world_desc@@QAA@Vphys_fixup_tag@@@Z
#include <vectorintrinsics.h>

void destroy_loaded_world_desc(void *object)
{
    static_cast<phys_world_desc *>(object)->~phys_world_desc();
}

// @address 0x63c738 ?setup_solver_info@phys_world_desc@@QAAXW4solver_kind@1@@Z
void phys_world_desc::setup_solver_info(solver_kind solver_preset)
{
    switch (solver_preset) {
    case SOLVER_PRESET_2ITER_SOFT:
        m_solver_tau = 0.3f;
        m_solver_damp = 0.9f;
        m_solver_iterations = 2;
        break;
    case SOLVER_PRESET_2ITER_MEDIUM:
        m_solver_tau = 0.6f;
        m_solver_damp = 1.0f;
        m_solver_iterations = 2;
        break;
    case SOLVER_PRESET_2ITER_HARD:
        m_solver_tau = 0.9f;
        m_solver_damp = 1.1f;
        m_solver_iterations = 2;
        break;
    case SOLVER_PRESET_4ITER_SOFT:
        m_solver_tau = 0.3f;
        m_solver_damp = 0.9f;
        m_solver_iterations = 4;
        break;
    case SOLVER_PRESET_4ITER_MEDIUM:
        m_solver_tau = 0.6f;
        m_solver_damp = 1.0f;
        m_solver_iterations = 4;
        break;
    case SOLVER_PRESET_4ITER_HARD:
        m_solver_tau = 0.9f;
        m_solver_damp = 1.1f;
        m_solver_iterations = 4;
        break;
    case SOLVER_PRESET_8ITER_SOFT:
        m_solver_tau = 0.3f;
        m_solver_damp = 0.9f;
        m_solver_iterations = 8;
        break;
    case SOLVER_PRESET_8ITER_MEDIUM:
        m_solver_tau = 0.6f;
        m_solver_damp = 1.0f;
        m_solver_iterations = 8;
        break;
    case SOLVER_PRESET_8ITER_HARD:
        m_solver_tau = 0.9f;
        m_solver_damp = 1.1f;
        m_solver_iterations = 8;
        break;
    default:
        break;
    }
}

void phys_world_desc::set_broadphase_world_size(float world_extent)
{
    m_broadphase_bounds.m_min_limit.flags_set_all(-0.5f * world_extent);
    m_broadphase_bounds.m_max_limit.flags_set_all(0.5f * world_extent);
}

__forceinline phys_world_desc::phys_world_desc(phys_fixup_tag flag)
{
    if (flag.m_fixup_finishing && m_contact_resting_vel == 0.0f)
        m_contact_resting_vel = 3.40282e38f;
}

// @address 0x63cad0 ?fixup_loaded_world_desc@@YAXPAX@Z
void fixup_loaded_world_desc(void *object)
{
    void * storage = object;
    phys_fixup_tag flag;
    flag.m_fixup_finishing = 1;
    if (storage)
        new (storage) phys_world_desc(flag);
}

// @address 0x63cb10 ?get_world_desc_vtable@@YAPBXXZ
const void *get_world_desc_vtable()
{
    return phys_type_desc::get_vtable<phys_world_desc>();
}


// @address 0x63c890 ??0phys_world_desc@@QAA@XZ
phys_world_desc::phys_world_desc()
{
    m_gravity.set(0.0f, -9.8f, 0.0f);
    m_islands_enabled = true;
    m_broadphase_query_size = 1024;
    m_broadphase_bounds.m_min_limit.set(-500.0f, -500.0f, -500.0f);
    m_broadphase_bounds.m_max_limit.set(500.0f, 500.0f, 500.0f);
    m_collision_filter = 0;
    m_convex_list_filter = 0;
    m_broadphase_marker_count = 0;
    m_toi_event_queue_size = 250;
    m_solver_tau = 0.6f;
    m_solver_damp = 1.0f;
    m_contact_resting_vel = 3.40282e38f;
    m_solver_iterations = 4;
    m_solver_microsteps = 1;
    m_coherent_solver_order = false;
    m_convex_edge_snap_threshold = 0.524f;
    m_concave_edge_snap_threshold = 0.698f;
    m_toi_weld_reject = false;
    m_collision_tolerance = 0.1f;
    m_border_behaviour = BROADPHASE_BORDER_ASSERT;
    m_toi_response_rotate_normal = 0.2f;
    m_sleep_ref_distance = 0.02f;
    m_expected_max_lin_vel = 200.0f;
    m_expected_min_psi_dt = 1.0f / 30.0f;
    m_iter_cast_early_out_dist = 0.01f;
    m_legacy_edge_weld = false;
    m_iter_cast_max_iters = 20;
    m_sleep_enabled = true;
    m_wake_on_body_move = true;
    m_want_legacy_sleep = false;
    m_hf_sleep_period = 0.1f;
    m_low_freq_sleep_period = 1.0f;
    m_min_island_size = 64;
    m_frame_marker_psi_snap = .0001f;
    m_inactive_frame_select0 = 0;
    m_inactive_frame_select1 = 0;
    m_integrate_sleep_counter = 0;
    m_contact_point_gen = CONTACT_POINT_REJECT_MANY;
    m_simulation_type = SIMULATION_TYPE_CONTINUOUS;
    m_memory_watchdog = 0;
    m_single_thread_actions = true;
}
