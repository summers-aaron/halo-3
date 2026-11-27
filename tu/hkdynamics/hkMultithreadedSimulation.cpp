// hkdynamics/hkMultithreadedSimulation.obj: from the records 0x259d68 0x6529f0 0x652aa8 0x652b60 0x652c38 0x652d10 0x652e18 0x652e28 0x652e38 0x652e48 0x652e68 0x652e70 0x652e78 0x653180 0x6531f0 0x653210 0x653240 0x653380 0x653488 0x653550 0x653778 0x653ba8 0x653d30 0x653e08 0x653ea8 0x653fe8
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x652e18 ?reset_thread_tokens@phys_multithreaded_simulation@@QAAXXZ
// @address 0x652e28 ?get_multithread_config@phys_multithreaded_simulation@@UAAXAAUphys_multithread_config@@@Z
// @address 0x652e38 ?set_multithread_config@phys_multithreaded_simulation@@UAAXABUphys_multithread_config@@@Z
// @address 0x652e48 ?set_active_worker_count@phys_multithreaded_simulation@@QAAXH@Z
// @address 0x652e68 ?assert_no_agent_collision_info@phys_multithreaded_simulation@@UAAXPAUphys_pair_agent_entry@@@Z
// @address 0x652e70 ?assert_no_entity_collision_info@phys_multithreaded_simulation@@UAAXPAPAVphys_entity@@HPAVphys_world@@@Z
// @address 0x6531f0 ?step_process_mt@phys_multithreaded_simulation@@QAAXABW4phys_thread_token@@@Z
// @address 0x653e08 ?step_delta_time@phys_multithreaded_simulation@@UAA?AW4phys_step_result@@M@Z
// @address 0x653fe8 ?get_job_queue@phys_multithreaded_simulation@@UAAPAVphys_job_queue@@XZ
#include "phys/phys_base_types.h"
#include <xtl.h>
#include <vectorintrinsics.h>
#include <ppcintrinsics.h>
#include "physics/havok_memory.h"
#include "phys/phys_math.h"
#include "phys/phys_solver.h"
#include "phys/phys_job_queue_util.h"
#include "phys/phys_dynamics_jobs.h"
#include "phys/phys_single_thread_island_jobs.h"
#include "phys/phys_cpu_integrate_job.h"
#include "phys/phys_cpu_agent_sector_job.h"
#include "phys/phys_cpu_build_accums_job.h"
#include "phys/phys_cpu_build_jacobians_job.h"
#include "phys/phys_cpu_integrate_motion_job.h"
#include "phys/phys_cpu_post_collide_job.h"
#include "phys/phys_cpu_solve_constraints_jobs.h"
#include "phys/phys_cpu_split_island_job.h"
#include "phys/phys_thread_memory.h"
#include "phys/phys_dynamics.h"
#include "phys/phys_island.h"
#include "phys/phys_collision_input.h"
#include "phys/phys_algorithm.h"
#include "phys/phys_pair_agent_machine.h"
#include "phys/phys_critical_section.h"
#include "phys/phys_semaphore.h"
#include "phys/phys_step_info.h"
#include "phys/phys_toi_event.h"
#include "phys/phys_statistics_collector.h"
#include "phys/phys_constraint_owner.h"
#include "phys/phys_pair_agent_entry.h"
#include "phys/phys_typed_broadphase_handle_pair.h"
#include "phys/phys_broadphase_listener.h"
#include "phys/phys_multithread_config.h"
#include "phys/phys_multithreaded_simulation.h"
#include "phys/phys_world_agent_util.h"
#include "phys/phys_dispatcher.h"
#include "phys/phys_world_maintenance_manager.h"
#include "phys/phys_world_op_util.h"
#include "phys/phys_world_callback_util.h"
#include "phys/phys_world_op_queue_types.h"
#include "phys/phys_broadphase.h"
#include "phys/phys_typed_broadphase_dispatcher.h"
#include "phys/phys_multithreaded_simulation_sort.h"
#include "phys/phys_memory.h"
#include "phys/phys_cpu_process_agent.h"
// @address 0x259d68 ?get@free_block_list@phys_thread_memory@@QAAPAXXZ
// @address 0x652d10 ?pair_less_than@@YA_NABVphys_typed_broadphase_pair@@0@Z
// @address 0x653240 ?add_collision_pair@mt_entity_pair_broadphase_listener@phys_multithreaded_simulation@@UAAXAAVphys_typed_broadphase_pair@@@Z
// @address 0x653380 ?remove_collision_pair@mt_entity_pair_broadphase_listener@phys_multithreaded_simulation@@UAAXAAVphys_typed_broadphase_pair@@@Z
// @address 0x653180 ?get_thread_token@phys_multithreaded_simulation@@QAA?AW4phys_thread_token@@XZ
// @address 0x6529f0 ?add_collision_pair@mt_volume_broadphase_listener@phys_multithreaded_simulation@@UAAXAAVphys_typed_broadphase_pair@@@Z
// @address 0x652aa8 ?remove_collision_pair@mt_volume_broadphase_listener@phys_multithreaded_simulation@@UAAXAAVphys_typed_broadphase_pair@@@Z
// @address 0x652b60 ?add_collision_pair@mt_broadphase_border_listener@phys_multithreaded_simulation@@UAAXAAVphys_typed_broadphase_pair@@@Z
// @address 0x652c38 ?remove_collision_pair@mt_broadphase_border_listener@phys_multithreaded_simulation@@UAAXAAVphys_typed_broadphase_pair@@@Z

static inline phys_volume *get_volume(phys_collider *c) { return static_cast<phys_volume *>(c->get_collider_owner()); }

// @address 0x6a2fa0 ?advance_time@phys_continuous_simulation@@UAA?AW4phys_step_result@@XZ

void phys_multithreaded_simulation::mt_volume_broadphase_listener::add_collision_pair(phys_typed_broadphase_pair &pair)
{
    if (pair.get_element_a()->get_type_id() == 2) {
        phys_collider *a = (phys_collider *)pair.get_element_a()->get_collider_owner();
        phys_collider *b = (phys_collider *)pair.get_element_b()->get_collider_owner();
        get_volume(a)->insert_overlapping_collider(b);
    }
    if (pair.get_element_b()->get_type_id() == 2) {
        phys_collider *a = (phys_collider *)pair.get_element_a()->get_collider_owner();
        phys_collider *b = (phys_collider *)pair.get_element_b()->get_collider_owner();
        get_volume(b)->insert_overlapping_collider(a);
    }
}

void phys_multithreaded_simulation::mt_volume_broadphase_listener::remove_collision_pair(phys_typed_broadphase_pair &pair)
{
    if (pair.get_element_a()->get_type_id() == 2) {
        phys_collider *a = (phys_collider *)pair.get_element_a()->get_collider_owner();
        phys_collider *b = (phys_collider *)pair.get_element_b()->get_collider_owner();
        get_volume(a)->erase_overlapping_collider(b);
    }
    if (pair.get_element_b()->get_type_id() == 2) {
        phys_collider *a = (phys_collider *)pair.get_element_a()->get_collider_owner();
        phys_collider *b = (phys_collider *)pair.get_element_b()->get_collider_owner();
        get_volume(b)->erase_overlapping_collider(a);
    }
}

void phys_multithreaded_simulation::mt_broadphase_border_listener::add_collision_pair(phys_typed_broadphase_pair &pair)
{
    signed char ta = pair.get_element_a()->get_type_id();
    if (ta == 3 && pair.get_element_b()->get_type_id() == 3) return;
    if (ta == 3) {
        phys_collider *a = (phys_collider *)pair.get_element_a()->get_collider_owner();
        phys_collider *b = (phys_collider *)pair.get_element_b()->get_collider_owner();
        get_volume(a)->insert_overlapping_collider(b);
    }
    if (pair.get_element_b()->get_type_id() == 3) {
        phys_collider *a = (phys_collider *)pair.get_element_a()->get_collider_owner();
        phys_collider *b = (phys_collider *)pair.get_element_b()->get_collider_owner();
        get_volume(b)->insert_overlapping_collider(a);
    }
}

void phys_multithreaded_simulation::mt_broadphase_border_listener::remove_collision_pair(phys_typed_broadphase_pair &pair)
{
    signed char ta = pair.get_element_a()->get_type_id();
    if (ta == 3 && pair.get_element_b()->get_type_id() == 3) return;
    if (ta == 3) {
        phys_collider *a = (phys_collider *)pair.get_element_a()->get_collider_owner();
        phys_collider *b = (phys_collider *)pair.get_element_b()->get_collider_owner();
        get_volume(a)->erase_overlapping_collider(b);
    }
    if (pair.get_element_b()->get_type_id() == 3) {
        phys_collider *a = (phys_collider *)pair.get_element_a()->get_collider_owner();
        phys_collider *b = (phys_collider *)pair.get_element_b()->get_collider_owner();
        get_volume(b)->erase_overlapping_collider(a);
    }
}

namespace {
}

// 0x652d10
bool pair_less_than(const phys_typed_broadphase_pair &a, const phys_typed_broadphase_pair &b)
{
    const phys_collider *collider_a0 = static_cast<const phys_collider *>(a.get_element_a()->get_collider_owner());
    const phys_collider *collider_a1 = static_cast<const phys_collider *>(a.get_element_b()->get_collider_owner());
    const phys_collider *collider_b0 = static_cast<const phys_collider *>(b.get_element_a()->get_collider_owner());
    const phys_collider *collider_b1 = static_cast<const phys_collider *>(b.get_element_b()->get_collider_owner());
    int uid_a0 = phys_get_rigid_body(collider_a0)->get_uid();
    int uid_a1 = phys_get_rigid_body(collider_a1)->get_uid();
    int uid_b0 = phys_get_rigid_body(collider_b0)->get_uid();
    int uid_b1 = phys_get_rigid_body(collider_b1)->get_uid();
    if (uid_a0 < uid_b0) return true;
    if (uid_a0 == uid_b0) return uid_a1 < uid_b1;
    return false;
}

// 0x652e18
void phys_multithreaded_simulation::reset_thread_tokens()
{
    m_thread_token_count = 0;
}

// 0x652e28
void phys_multithreaded_simulation::get_multithread_config(phys_multithread_config &config)
{
    config = m_mt_config;
}

// 0x652e38
void phys_multithreaded_simulation::set_multithread_config(const phys_multithread_config &config)
{
    m_mt_config = config;
}

// 0x652e48
void phys_multithreaded_simulation::set_active_worker_count(int active_worker_count)
{
    if (active_worker_count == 0) {
        m_job_queue.set_job_query_rules(phys_job_queue::JOB_QUERY_RULES_CPU);
    } else {
        m_job_queue.set_job_query_rules(phys_job_queue::JOB_QUERY_RULES_COPROC);
    }
}

// 0x652e68
void phys_multithreaded_simulation::assert_no_agent_collision_info(phys_pair_agent_entry *pair_agent_entry)
{
}

// 0x652e70
void phys_multithreaded_simulation::assert_no_entity_collision_info(phys_entity **entities, int solver_entity_count, phys_world *world)
{
}

// @address 0x652e78 ?process_next_job@phys_multithreaded_simulation@@QAAXW4worker_thread_kind@phys_job_queue@@@Z
void phys_multithreaded_simulation::process_next_job(phys_job_queue::worker_thread_kind queue_thread_type)
{
    phys_job_queue::job_queue_entry src_job;
    if (m_job_queue.fetch_next_job(queue_thread_type, src_job, phys_job_queue::JOB_WAIT_FOR_NEXT) == phys_job_queue::JOB_QUEUE_HAS_WORK) {
        phys_job_queue::job_status queue_job_status = phys_job_queue::JOB_QUEUE_HAS_WORK;
        phys_mt_thread_state *tl = new phys_mt_thread_state(m_world, this, queue_thread_type);
        while (queue_job_status == phys_job_queue::JOB_QUEUE_HAS_WORK) {
            phys_dynamics_job &dynamics_job = reinterpret_cast<phys_dynamics_job &>(src_job);
            switch (dynamics_job.m_job_type) {
            case SIM_JOB_INTEGRATE:
                queue_job_status = run_integrate_job(*tl, m_job_queue, src_job);
                continue;
            case SIM_JOB_BUILD_ACCUMS:
                queue_job_status = phys_cpu_build_accumulators_job(*tl, m_job_queue, src_job);
                continue;
            case SIM_JOB_JACOBIAN_SETUP_CB:
                queue_job_status = phys_single_thread_island_jobs::run_jacobian_callback_job(*tl, m_job_queue, src_job, phys_job_queue::JOB_WAIT_FOR_NEXT);
                continue;
            case SIM_JOB_BUILD_JACOBIANS:
                queue_job_status = phys_cpu_build_jacobians_job(*tl, m_job_queue, src_job, phys_job_queue::JOB_WAIT_FOR_NEXT);
                continue;
            case SIM_JOB_SOLVE_CONSTRAINTS:
                queue_job_status = phys_cpu_solve_constraints_job(*tl, m_job_queue, src_job, phys_job_queue::JOB_WAIT_FOR_NEXT);
                continue;
            case SIM_JOB_SOLVE_APPLY_GRAVITY:
                queue_job_status = phys_cpu_solve_apply_gravity_job(*tl, m_job_queue, src_job, phys_job_queue::JOB_WAIT_FOR_NEXT);
                continue;
            case SIM_JOB_SOLVE_CONSTRAINT_BATCH:
                queue_job_status = phys_cpu_solve_constraint_batch_job(*tl, m_job_queue, src_job, phys_job_queue::JOB_WAIT_FOR_NEXT);
                continue;
            case SIM_JOB_SOLVE_INTEGRATE_VELS:
                queue_job_status = phys_cpu_solve_integrate_vels_job(*tl, m_job_queue, src_job, phys_job_queue::JOB_WAIT_FOR_NEXT);
                continue;
            case SIM_JOB_SOLVE_EXPORT_RESULTS:
                queue_job_status = phys_cpu_solve_export_results_job(*tl, m_job_queue, src_job, phys_job_queue::JOB_WAIT_FOR_NEXT);
                continue;
            case SIM_JOB_INTEGRATE_MOTION:
                queue_job_status = phys_cpu_integrate_motion_job(*tl, m_job_queue, src_job, phys_job_queue::JOB_WAIT_FOR_NEXT);
                continue;
            case SIM_JOB_SPLIT_ISLAND:
                queue_job_status = phys_cpu_split_island_job(*tl, m_job_queue, src_job);
                continue;
            case SIM_JOB_BROADPHASE:
                queue_job_status = phys_single_thread_island_jobs::run_broadphase_job(*tl, m_job_queue, src_job);
                continue;
            case SIM_JOB_AGENT_SECTOR:
                queue_job_status = phys_cpu_agent_sector_job(*tl, m_job_queue, src_job);
                continue;
            case SIM_JOB_POST_COLLIDE:
                if (phys_cpu_post_collide_job(*tl, m_job_queue, src_job) == phys_job_queue::JOB_QUEUE_HAS_WORK)
                    continue;
                break;
            default:
                continue;
            }
            break;
        }
        delete tl;
    }
}

// 0x6531f0
void phys_multithreaded_simulation::step_process_mt(const phys_thread_token &thread_token)
{
    if (thread_token == PHYS_THREAD_TOKEN_CPU) {
        process_next_job(phys_job_queue::WORKER_THREAD_PRIMARY);
    } else {
        process_next_job(phys_job_queue::WORKER_THREAD_CPU);
    }
}

// @address 0x653210 ?less_island_ptr@@YA?AVphys_bool@@PBVphys_island@@0@Z
static phys_bool less_island_ptr(const phys_island *a, const phys_island *b)
{
    return a->m_island_entities[0]->get_uid() < b->m_island_entities[0]->get_uid();
}

namespace {
static __forceinline void step_process_mt(phys_multithreaded_simulation *simulation, const phys_thread_token &thread_token)
{
    if (thread_token == PHYS_THREAD_TOKEN_CPU) {
        simulation->process_next_job(phys_job_queue::WORKER_THREAD_PRIMARY);
    } else {
        simulation->process_next_job(phys_job_queue::WORKER_THREAD_CPU);
    }
}
}

extern "C" {
}
extern "C" {
}
namespace {
static __forceinline void push_pair(phys_array<phys_typed_broadphase_pair> &array, const phys_typed_broadphase_pair &pair)
{
    if (array.m_size == (array.m_capacity_and_flags & 0x3fffffff)) phys_array_util::grow_array_storage(&array, sizeof(pair));
    array.m_data[array.m_size] = pair;
    ++array.m_size;
}
static __forceinline phys_pair_agent_entry *add_agent_helper(phys_linked_collider *a, phys_linked_collider *b, phys_process_collision_input *input)
{
    phys_collider_quality quality_a = a->get_quality_type();
    phys_collider_quality quality_b = b->get_quality_type();
    signed char quality_index = input->m_agent_dispatcher->get_collision_quality_idx(quality_a, quality_b);
    if (quality_index == 0) {
        return 0;
    }
    phys_collision_quality_info *quality_info = input->m_agent_dispatcher->get_collision_quality_info(quality_index);
    input->m_create_pred_agents = quality_info->m_use_ccd;
    return phys_world_agent_util::add_pair_agent(a, b, *input);
}
}

// 0x653240
void phys_multithreaded_simulation::mt_entity_pair_broadphase_listener::add_collision_pair(phys_typed_broadphase_pair &pair)
{
    phys_linked_collider *pair_collider_a = static_cast<phys_linked_collider *>(pair.get_element_a()->get_collider_owner());
    phys_linked_collider *pair_collider_b = static_cast<phys_linked_collider *>(pair.get_element_b()->get_collider_owner());
    phys_entity *listener_entity_a = static_cast<phys_entity *>(pair_collider_a->get_collider_owner());
    phys_entity *listener_entity_b = static_cast<phys_entity *>(pair_collider_b->get_collider_owner());
    phys_multithreaded_simulation *simulation = m_world_simulation;
    if (simulation->m_collect_collision_pairs && !listener_entity_a->body_is_fixed() && !listener_entity_b->body_is_fixed() &&
        listener_entity_a->get_island() != listener_entity_b->get_island()) {
        push_pair(simulation->m_added_pairs, pair);
    } else {
        add_agent_helper(pair_collider_a, pair_collider_b, simulation->m_world->get_collision_input());
    }
}

// 0x653380
void phys_multithreaded_simulation::mt_entity_pair_broadphase_listener::remove_collision_pair(phys_typed_broadphase_pair &pair)
{
    phys_linked_collider *pair_collider_a = static_cast<phys_linked_collider *>(pair.get_element_a()->get_collider_owner());
    phys_linked_collider *pair_collider_b = static_cast<phys_linked_collider *>(pair.get_element_b()->get_collider_owner());
    phys_entity *listener_entity_a = static_cast<phys_entity *>(pair_collider_a->get_collider_owner());
    phys_entity *listener_entity_b = static_cast<phys_entity *>(pair_collider_b->get_collider_owner());
    phys_multithreaded_simulation *simulation = m_world_simulation;
    if (simulation->m_collect_collision_pairs && !listener_entity_a->body_is_fixed() && !listener_entity_b->body_is_fixed() &&
        listener_entity_a->get_island() != listener_entity_b->get_island()) {
        push_pair(simulation->m_removed_pairs, pair);
    } else {
        phys_pair_agent_entry *entry = phys_pair_agent_find(pair_collider_a, pair_collider_b);
        if (entry) phys_world_agent_util::remove_agent(entry);
    }
}
// @address 0x653ba8 ??0phys_multithreaded_simulation@@QAA@PAVphys_world@@@Z
phys_multithreaded_simulation::phys_multithreaded_simulation(phys_world *world)
    : phys_continuous_simulation(world),
      m_added_pairs_lock(4000),
      m_removed_pairs_lock(4000),
      m_step_semaphore(0, 10),
      m_step_lock(4000),
      m_toi_queue_lock(4000),
      m_token_lock(4000)
{
    m_thread_token_count = 0;
    m_collect_collision_pairs = false;

    m_world->get_broadphase()->enable_multithreading(10000);

    m_mt_entity_pair_listener.m_world_simulation = this;
    m_mt_volume_listener.m_display_lock = &m_token_lock;
    world->m_broadphase_dispatcher->set_broadphase_listener(&m_mt_entity_pair_listener, phys_world_object::BROADPHASE_KIND_ENTITY,
        phys_world_object::BROADPHASE_KIND_ENTITY);

    world->m_broadphase_dispatcher->set_broadphase_listener(&m_mt_volume_listener, phys_world_object::BROADPHASE_KIND_VOLUME,
        phys_world_object::BROADPHASE_KIND_ENTITY);
    world->m_broadphase_dispatcher->set_broadphase_listener(&m_mt_volume_listener, phys_world_object::BROADPHASE_KIND_ENTITY,
        phys_world_object::BROADPHASE_KIND_VOLUME);
    world->m_broadphase_dispatcher->set_broadphase_listener(&m_mt_volume_listener, phys_world_object::BROADPHASE_KIND_VOLUME,
        phys_world_object::BROADPHASE_KIND_VOLUME);

    m_border_listener.m_display_lock = &m_token_lock;

    world->m_broadphase_dispatcher->set_broadphase_listener(&m_border_listener, phys_world_object::BROADPHASE_KIND_ENTITY,
        phys_world_object::BROADPHASE_KIND_BORDER);
    world->m_broadphase_dispatcher->set_broadphase_listener(&m_border_listener, phys_world_object::BROADPHASE_KIND_BORDER,
        phys_world_object::BROADPHASE_KIND_ENTITY);

    world->m_broadphase_dispatcher->set_broadphase_listener(&m_border_listener, phys_world_object::BROADPHASE_KIND_VOLUME,
        phys_world_object::BROADPHASE_KIND_BORDER);
    world->m_broadphase_dispatcher->set_broadphase_listener(&m_border_listener, phys_world_object::BROADPHASE_KIND_BORDER,
        phys_world_object::BROADPHASE_KIND_VOLUME);
    world->m_broadphase_dispatcher->set_broadphase_listener(&m_border_listener, phys_world_object::BROADPHASE_KIND_BORDER,
        phys_world_object::BROADPHASE_KIND_BORDER);

    m_job_queue.m_job_pop_func = phys_job_queue_util::pop_dynamics_job;
    m_job_queue.m_finish_job_fn = phys_job_queue_util::finish_dynamics_job;
    m_job_queue.m_user_data = world;
}

// @address 0x653d30 ??1phys_multithreaded_simulation@@UAA@XZ
phys_multithreaded_simulation::~phys_multithreaded_simulation()
{
}

// 0x653e08
phys_step_result phys_multithreaded_simulation::step_delta_time(float body_delta_time)
{
    phys_thread_token thread_token = get_thread_token();
    if (thread_token == PHYS_THREAD_TOKEN_PRIMARY) {
        step_begin_st(body_delta_time);
        m_step_semaphore.release(m_thread_token_count);
    }
    m_step_semaphore.acquire_semaphore();
    ::step_process_mt(this, thread_token);
    if (thread_token == PHYS_THREAD_TOKEN_PRIMARY) {
        step_end_st();
    }
    return PHYS_STEP_RESULT_SUCCESS;
}

// @address 0x653ea8 ??$quick_sort_recursive@Vphys_typed_broadphase_pair@@P6A_NABV1@0@Z@phys_algorithm@@YAXPAVphys_typed_broadphase_pair@@HHP6A_NABV1@1@Z@Z

// 0x653fe8
phys_job_queue *phys_multithreaded_simulation::get_job_queue()
{
    return &m_job_queue;
}

// 0x653180
phys_thread_token phys_multithreaded_simulation::get_thread_token()
{
    m_step_lock.enter();
    int token = m_thread_token_count;
    m_thread_token_count = token + 1;
    m_step_lock.leave();
    return static_cast<phys_thread_token>(token);
}
// @address 0x653488 ?sort_pair_list@@YAXAAV?$phys_array@Vphys_typed_broadphase_pair@@@@@Z
void sort_pair_list(phys_array<phys_typed_broadphase_pair> &pairs)
{
    phys_typed_broadphase_pair *pair = pairs.m_data;
    const phys_typed_broadphase_pair *pairs_end = pairs.m_data + pairs.m_size;
    while (pair < pairs_end) {
        phys_collider *pair_owner_a = static_cast<phys_collider *>(pair->get_element_a()->get_collider_owner());
        phys_collider *pair_owner_b = static_cast<phys_collider *>(pair->get_element_b()->get_collider_owner());
        int pair_uid_a = phys_get_rigid_body(pair_owner_a)->get_uid();
        int pair_uid_b = phys_get_rigid_body(pair_owner_b)->get_uid();
        if (pair_uid_a > pair_uid_b) {
            phys_algorithm::swap(pair->m_a, pair->m_b);
        }
        pair++;
    }
    phys_algorithm::quick_sort(pairs.m_data, pairs.m_size, pair_less_than);
}

// @address 0x653550 ?step_begin_st@phys_multithreaded_simulation@@QAAXM@Z
void phys_multithreaded_simulation::step_begin_st(float body_delta_time)
{
    m_collect_collision_pairs = true;
    m_world->lock_critical_ops();
    m_world->m_pending_ops->m_defer_island_merges = true;
    m_physics_dt = body_delta_time;
    phys_step_info physics_step_info(m_current_psi_time, m_current_psi_time + body_delta_time);
    m_world->m_dynamics_step_info.m_step_info = physics_step_info;
    m_world->m_collision_input->m_step_info = physics_step_info;
    m_world->m_maintenance_manager->perform_maintenance_no_split(m_world, physics_step_info);
    phys_world_op_util::cleanup_dirty_islands(m_world);
    m_world->check_determinism();
    phys_solve_upload();
    {
        m_world->m_dynamics_step_info.m_step_info = physics_step_info;
        phys_solver_info &solver_info = m_world->m_dynamics_step_info.m_solver_info;
        solver_info.m_delta_time = physics_step_info.m_delta_time * solver_info.m_inv_steps;
        solver_info.m_inv_dt = physics_step_info.m_inv_dt * solver_info.m_step_count;
        solver_info.m_gravity_per_substep.set_mul4(solver_info.m_delta_time, m_world->m_gravity);
        float delta_time_copy = physics_step_info.m_delta_time;
        solver_info.m_gravity_per_step.set_mul4(delta_time_copy, m_world->m_gravity);
    }
    int island_count = m_world->get_active_islands().getSize();
    if (island_count > 0) {
        if (m_world->m_single_thread_actions) {
            --m_world->m_critical_ops_volume_locks;
            apply_actions();
            ++m_world->m_critical_ops_volume_locks;
        }
        phys_integrate_job src_job(island_count);
        m_job_queue.set_queue_capacity(island_count);
        m_job_queue.add_job(*reinterpret_cast<phys_job_queue::job_queue_entry *>(&src_job), phys_job_queue::JOB_PRIORITY_LOW, phys_job_queue::JOB_KIND_HOST);
    }
    m_world->m_dynamics_step_info.m_solver_info.bump_sleep_flags();
    m_step_active_islands = m_world->get_active_islands().getSize();
    m_step_inactive_islands = m_world->get_inactive_islands().getSize();
    m_world->check_determinism();
}

extern "C" {
}

// @address 0x653778 ?step_end_st@phys_multithreaded_simulation@@QAAXXZ
void phys_multithreaded_simulation::step_end_st()
{
    {
        int new_island_count = m_world->get_active_islands().getSize() - m_step_active_islands;
        phys_algorithm::quick_sort(m_world->m_active_islands.begin() + m_step_active_islands, new_island_count, less_island_ptr);
        for (int i = m_step_active_islands; i < m_world->get_active_islands().getSize(); i++) {
            m_world->m_active_islands[i]->m_storage_idx = (unsigned short)i;
        }
    }
    m_world->check_determinism();

    if (m_added_pairs.getSize() + m_removed_pairs.getSize()) {
        phys_typed_broadphase_dispatcher::remove_duplicate_pairs(reinterpret_cast<phys_array<phys_broadphase_handle_pair> &>(m_added_pairs),
            reinterpret_cast<phys_array<phys_broadphase_handle_pair> &>(m_removed_pairs));
        sort_pair_list(m_added_pairs);
        sort_pair_list(m_removed_pairs);
        {
            phys_process_collision_output process_output(0);
            for (int i = 0; i < m_added_pairs.getSize(); ++i) {
                phys_linked_collider *collider_a = static_cast<phys_linked_collider *>(m_added_pairs[i].get_element_a()->get_collider_owner());
                phys_linked_collider *collider_b = static_cast<phys_linked_collider *>(m_added_pairs[i].get_element_b()->get_collider_owner());
                {
                    phys_entity *listener_entity_a = static_cast<phys_entity *>(collider_a->get_collider_owner());
                    phys_entity *listener_entity_b = static_cast<phys_entity *>(collider_b->get_collider_owner());
                    phys_island *island_a = listener_entity_a->get_island();
                    phys_island *island_b = listener_entity_b->get_island();
                    if (island_a != island_b) {
                        island_a = phys_world_op_util::merge_two_islands(m_world, island_a, island_b);
                    }
                    process_output.m_constraint_owner = island_a;
                }
                phys_pair_agent_entry *entry = add_agent_helper(collider_a, collider_b, m_world->get_collision_input());
                if (entry) {
                    phys_cpu_process_agent_helper(entry, *m_world->get_collision_input(), process_output, this);
                }
            }
            m_added_pairs.clear();
        }
        for (int i = 0; i < m_removed_pairs.getSize(); ++i) {
            phys_linked_collider *collider_a = static_cast<phys_linked_collider *>(m_removed_pairs[i].get_element_a()->get_collider_owner());
            phys_linked_collider *collider_b = static_cast<phys_linked_collider *>(m_removed_pairs[i].get_element_b()->get_collider_owner());
            phys_pair_agent_entry *entry = phys_pair_agent_find(collider_a, collider_b);
            if (entry) {
                phys_world_agent_util::remove_agent_and_toi_events(entry);
            }
        }
        m_removed_pairs.clear();
    }

    m_collect_collision_pairs = false;
    m_world->m_pending_ops->m_defer_island_merges = false;
    m_world->unlock_and_flush_ops();
    m_current_psi_time += m_physics_dt;
    if (m_world->m_post_collide_listeners.getSize()) {
        phys_step_info physics_step_info(m_current_psi_time - m_physics_dt, m_current_psi_time);
        phys_world_callback_util::notify_world_post_collide(m_world, physics_step_info);
    }
    m_world->check_determinism();
    advance_time();
}

