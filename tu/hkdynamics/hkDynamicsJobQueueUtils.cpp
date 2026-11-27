// hkdynamics/hkDynamicsJobQueueUtils.obj: from the records 0x662348 0x662350 0x662730 0x662790 0x662888 0x662950 0x662a38 0x662a90 0x663190 0x663288
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "phys/phys_base_types.h"
#include "physics/havok_memory.h"
#include "phys/phys_thread_memory.h"
#include "phys/phys_base.h"
#include "phys/phys_step_info.h"
#include "phys/phys_collision_input.h"
#include "phys/phys_dynamics.h"
#include "phys/phys_dynamics_jobs.h"
#include "phys/phys_job_queue_util.h"
#include "phys/phys_agent_sector_header.h"
#include "phys/phys_constraint_query_in.h"
#include "phys/phys_dispatcher.h"
#include "phys/phys_constraint_solver_setup.h"
#include "phys/phys_multithreaded_simulation.h"
#include "phys/phys_island.h"
#include "phys/phys_vel_accum.h"
#include "phys/phys_constraint_query_in_world.h"
// @address 0x663190 ?set_capacity@?$phys_queue@Ujob_queue_entry@phys_job_queue@@@@QAAXH@Z

template <class T> class phys_array;
struct phys_dma_utils {
    static __forceinline int increment_i32_main_mem(int *variable, int increment, int dma_group_id = 14)
    {
        int queue_new_value = *variable + increment;
        *variable = queue_new_value;
        return queue_new_value;
    }
    static __forceinline void set_i32_main_mem(int *variable, int value, int dma_group_id = 14)
    {
        *variable = value;
    }
    static __forceinline void set_i8_main_mem(signed char *dst_main_mem, signed char value, int dma_group_id = 14)
    {
        *dst_main_mem = value;
    }
};
// @address 0x662348 ?upload_open_jobs@@YAXPBUphys_build_jacobian_task_header@@0@Z
void upload_open_jobs(const phys_build_jacobian_task_header *header, const phys_build_jacobian_task_header *original_header)
{
}
// @address 0x662350 ?pop_dynamics_job@phys_job_queue_util@@SA?AW4job_pop_result@phys_job_queue@@AAV3@PAUjob_queue_dynamic_data@3@AAUjob_queue_entry@3@2@Z
phys_job_queue::job_pop_result phys_job_queue_util::pop_dynamics_job(
    phys_job_queue &queue, phys_job_queue::job_queue_dynamic_data *data, phys_job_queue::job_queue_entry &job_in, phys_job_queue::job_queue_entry &next_job_out)
{
    phys_dynamics_job &src_job = reinterpret_cast<phys_dynamics_job &>(job_in);
    phys_string::mem_copy16(&next_job_out, &job_in, sizeof(phys_job_queue::job_queue_entry) >> 4);
    phys_world *world = reinterpret_cast<phys_world *>(queue.m_user_data);
    const phys_array<phys_island *> &islands = world->m_active_islands;
    switch (src_job.m_job_type) {
    case SIM_JOB_INTEGRATE: {
        phys_integrate_job &queued_job = static_cast<phys_integrate_job &>(src_job);
        phys_integrate_job &out = reinterpret_cast<phys_integrate_job &>(next_job_out);
        return queued_job.pop_job_task(islands, out);
    }
    case SIM_JOB_BUILD_ACCUMS: {
        phys_build_accums_job &queued_job = static_cast<phys_build_accums_job &>(src_job);
        phys_build_accums_job &out = reinterpret_cast<phys_build_accums_job &>(next_job_out);
        return queued_job.pop_job_task(islands, out);
    }
    case SIM_JOB_BUILD_JACOBIANS: {
        phys_build_jacobians_job &queued_job = static_cast<phys_build_jacobians_job &>(src_job);
        phys_build_jacobians_job &out = reinterpret_cast<phys_build_jacobians_job &>(next_job_out);
        return queued_job.pop_job_task(islands, out);
    }
    case SIM_JOB_BROADPHASE: {
        phys_broadphase_job &queued_job = static_cast<phys_broadphase_job &>(src_job);
        phys_broadphase_job &out = reinterpret_cast<phys_broadphase_job &>(next_job_out);
        return queued_job.pop_job_task(const_cast<phys_array<phys_island *> &>(islands), out);
    }
    case SIM_JOB_SPLIT_ISLAND: {
        phys_split_island_job &queued_job = static_cast<phys_split_island_job &>(src_job);
        phys_split_island_job &out = reinterpret_cast<phys_split_island_job &>(next_job_out);
        return queued_job.pop_job_task(islands, out);
    }
    case SIM_JOB_INTEGRATE_MOTION: {
        phys_integrate_motion_job &queued_job = static_cast<phys_integrate_motion_job &>(src_job);
        phys_integrate_motion_job &out = reinterpret_cast<phys_integrate_motion_job &>(next_job_out);
        return queued_job.pop_job_task(islands, out);
    }
    case SIM_JOB_SOLVE_APPLY_GRAVITY: {
        phys_solve_apply_gravity_job &queued_job = static_cast<phys_solve_apply_gravity_job &>(src_job);
        phys_solve_apply_gravity_job &out = reinterpret_cast<phys_solve_apply_gravity_job &>(next_job_out);
        return queued_job.pop_job_task(islands, out);
    }
    case SIM_JOB_SOLVE_CONSTRAINT_BATCH: {
        phys_solve_constraint_batch_job &queued_job = static_cast<phys_solve_constraint_batch_job &>(src_job);
        phys_solve_constraint_batch_job &out = reinterpret_cast<phys_solve_constraint_batch_job &>(next_job_out);
        return queued_job.pop_job_task(islands, out);
    }
    case SIM_JOB_SOLVE_INTEGRATE_VELS: {
        phys_solve_integrate_vel_job &queued_job = static_cast<phys_solve_integrate_vel_job &>(src_job);
        phys_solve_integrate_vel_job &out = reinterpret_cast<phys_solve_integrate_vel_job &>(next_job_out);
        return queued_job.pop_job_task(islands, out);
    }
    case SIM_JOB_SOLVE_EXPORT_RESULTS: {
        phys_solve_export_results_job &queued_job = static_cast<phys_solve_export_results_job &>(src_job);
        phys_solve_export_results_job &out = reinterpret_cast<phys_solve_export_results_job &>(next_job_out);
        return queued_job.pop_job_task(islands, out);
    }
    case SIM_JOB_JACOBIAN_SETUP_CB:
    case SIM_JOB_SOLVE_CONSTRAINTS:
    case SIM_JOB_POST_COLLIDE: {
        phys_dynamics_job &queued_job = static_cast<phys_dynamics_job &>(src_job);
        phys_dynamics_job &out = reinterpret_cast<phys_dynamics_job &>(next_job_out);
        return queued_job.pop_dynamics_job_task(islands, out);
    }
    case SIM_JOB_AGENT_SECTOR: {
        phys_agent_sector_job &queued_job = static_cast<phys_agent_sector_job &>(src_job);
        phys_agent_sector_job &out = reinterpret_cast<phys_agent_sector_job &>(next_job_out);
        return queued_job.pop_job_task(islands, out);
    }
    default:
        break;
    }
    return phys_job_queue::JOB_QUEUE_POP_ENTRY;
}
// @address 0x662730 ?spawn_build_jacobian_jobs@@YAXABVphys_dynamics_job@@ABUphys_build_jacobian_task_header@@AAVphys_job_queue@@PAUjob_queue_dynamic_data@3@AAUjob_queue_entry_input@3@@Z
void spawn_build_jacobian_jobs(const phys_dynamics_job &src_job, const phys_build_jacobian_task_header &header,
    phys_job_queue &queue, phys_job_queue::job_queue_dynamic_data *data, phys_job_queue::job_queue_entry_input &input)
{
    new (&input.m_queue_job) phys_build_jacobians_job(src_job, header.m_build_tasks.m_build_jacobian_tasks, header);
    header.m_open_job_count = header.m_build_tasks.m_jacobian_task_count;
    input.m_job_priority = 0;
    input.m_job_thread_type = 0;
}
// @address 0x662790 ?allocate@phys_agent_sector_header@@SAPAU1@H@Z
phys_agent_sector_header *phys_agent_sector_header::allocate(int job_count)
{
    int size = get_sector_allocated_size(job_count);
    phys_agent_sector_header *header = (phys_agent_sector_header *)phys_thread_memory::get_singleton_instance().allocate_chunk(size, PHYS_MEM_DYNAMICS);
    header->m_open_job_count = job_count;
    header->m_job_count = job_count;
    sector_job_info **job_infos = header->m_job_info;
    for (int i = 0; i < job_count; i++)
        job_infos[i] = new sector_job_info();
    return header;
}
// @address 0x662888 ??0phys_mt_thread_state@@QAA@PAVphys_world@@PAVphys_multithreaded_simulation@@W4worker_thread_kind@phys_job_queue@@@Z
phys_mt_thread_state::phys_mt_thread_state(phys_world *world, phys_multithreaded_simulation *simulation,
    phys_job_queue::worker_thread_kind type)
{
    m_world = world;
    m_world_simulation = simulation;
    m_thread_type = type;
    m_collision_input = *world->m_collision_input;
    m_constraint_query_in.set(*world);
}
// @address 0x662950 ?deallocate@phys_agent_sector_header@@QAAXXZ
void phys_agent_sector_header::deallocate()
{
    sector_job_info **job_infos = m_job_info;
    for (int i = 0; i < m_job_count; i++) {
        sector_job_info *info = job_infos[i];
        if (info != 0)
            delete info;
    }
    int size = get_sector_allocated_size(m_job_count);
    phys_thread_memory &memory = phys_thread_memory::get_singleton_instance();
    memory.free_chunk(this, size, PHYS_MEM_DYNAMICS);
}
// @address 0x662a38 ?spawn_split_island_job@@YAXPBVphys_dynamics_job@@ABUphys_build_jacobian_task_header@@AAVphys_job_queue@@PAUjob_queue_dynamic_data@3@@Z
__forceinline void spawn_split_island_job(const phys_dynamics_job *src_job, const phys_build_jacobian_task_header &header,
    phys_job_queue &queue, phys_job_queue::job_queue_dynamic_data *data)
{
    if (header.m_split_check_requested) {
        phys_split_island_job split_job(*src_job, SIM_JOB_SPLIT_ISLAND);
        data->m_job_queue[0].enqueue_front(split_job.as_job_queue_entry());
    }
}
// @address 0x662a90 ?finish_dynamics_job@phys_job_queue_util@@SA?AW4job_creation_status@phys_job_queue@@AAV3@PAUjob_queue_dynamic_data@3@ABUjob_queue_entry@3@AAUjob_queue_entry_input@3@@Z
phys_job_queue::job_creation_status phys_job_queue_util::finish_dynamics_job(
    phys_job_queue &queue, phys_job_queue::job_queue_dynamic_data *data, const phys_job_queue::job_queue_entry &job_in,
        phys_job_queue::job_queue_entry_input &new_job_created)
{
    int jobs_to_finish = 1;
    const phys_dynamics_job &src_job = reinterpret_cast<const phys_dynamics_job &>(job_in);
    phys_build_jacobian_task_header *local_task_header = src_job.m_task_header;
    switch (src_job.m_job_type) {
    case SIM_JOB_SPAWN_JACOBIAN_TASKS:
    case SIM_JOB_BUILD_ACCUMS: {
        const phys_build_accums_job &build_accum_job = reinterpret_cast<const phys_build_accums_job &>(src_job);
        local_task_header->m_open_job_count--;
        if (local_task_header->m_open_job_count == 0) {
            int ppu_elem_count = local_task_header->m_build_tasks.m_callback_constraint_count;
            if (ppu_elem_count > 0) {
                new (&new_job_created.m_queue_job) phys_fire_jacobian_setup_callback(build_accum_job);
                new_job_created.m_job_priority = 0;
                new_job_created.m_job_thread_type = 0;
            } else {
                spawn_build_jacobian_jobs(build_accum_job, *local_task_header, queue, data, new_job_created);
            }
            goto job_was_created;
        }
        break;
    }
    case SIM_JOB_JACOBIAN_SETUP_CB: {
        const phys_fire_jacobian_setup_callback &setup_callback_job = reinterpret_cast<const phys_fire_jacobian_setup_callback &>(src_job);
        spawn_build_jacobian_jobs(setup_callback_job, *setup_callback_job.m_task_header, queue, data, new_job_created);
        return phys_job_queue::JOB_ENQUEUED;
    }
    case SIM_JOB_BUILD_JACOBIANS: {
        local_task_header->m_open_job_count--;
        const phys_build_jacobians_job &build_jacobians_job = reinterpret_cast<const phys_build_jacobians_job &>(src_job);
        if (local_task_header->m_open_job_count == 0) {
            phys_dynamics_job *new_dynamics_job;
            if (local_task_header->m_solve_single_thread) {
                new_dynamics_job = new (&new_job_created.m_queue_job) phys_solve_constraints_job(build_jacobians_job, *local_task_header);
                new_job_created.m_job_priority = 0;
                new_job_created.m_job_thread_type = 0;
            } else {
                new_dynamics_job = new (&new_job_created.m_queue_job) phys_solve_apply_gravity_job(src_job, *local_task_header);
                new_job_created.m_job_priority = 0;
                new_job_created.m_job_thread_type = 0;
                local_task_header->m_open_job_count = local_task_header->m_gravity_job_count;
            }
            spawn_split_island_job(new_dynamics_job, *local_task_header, queue, data);
            goto job_was_created;
        }
        break;
    }
    case SIM_JOB_SOLVE_CONSTRAINTS: {
        phys_dma_utils::set_i8_main_mem(&src_job.m_task_header->m_export_done, char(1));
        return phys_job_queue::JOB_NOT_CREATED;
    }
    case SIM_JOB_SOLVE_APPLY_GRAVITY: {
        local_task_header->m_open_job_count--;
        if (local_task_header->m_open_job_count == 0) {
            const phys_solve_apply_gravity_job &apply_gravity_job = reinterpret_cast<const phys_solve_apply_gravity_job &>(src_job);
            new (&new_job_created.m_queue_job) phys_solve_constraint_batch_job(apply_gravity_job, *local_task_header);
            new_job_created.m_job_priority = 0;
            new_job_created.m_job_thread_type = 0;
            local_task_header->m_open_job_count = local_task_header->m_solve_tasks.m_first_batch_size;
            upload_open_jobs(local_task_header, src_job.m_task_header);
            return phys_job_queue::JOB_ENQUEUED;
        }
        break;
    }
    case SIM_JOB_SOLVE_CONSTRAINT_BATCH: {
        const phys_solve_constraint_batch_job &solve_batch_job = reinterpret_cast<const phys_solve_constraint_batch_job &>(src_job);
        phys_job_queue::job_creation_status status = phys_job_queue::JOB_NOT_CREATED;
        local_task_header->m_open_job_count--;
        if (local_task_header->m_open_job_count == 0) {
            const unsigned int next_batch_size = solve_batch_job.m_solve_batch_task->m_next_batch_size;
            int new_job_open_count;
            if (next_batch_size != 0) {
                new (&new_job_created.m_queue_job) phys_solve_constraint_batch_job(solve_batch_job);
                new_job_open_count = next_batch_size;
            } else {
                if (solve_batch_job.m_solver_micro_step + 1 < solve_batch_job.m_solver_micro_steps) {
                    phys_solve_constraint_batch_job *new_batch_job = new (&new_job_created.m_queue_job) phys_solve_constraint_batch_job(solve_batch_job);
                    new_batch_job->m_solver_micro_step++;
                    new_batch_job->m_solve_batch_task = local_task_header->m_solve_tasks.m_first_solve_task;
                    new_job_open_count = local_task_header->m_solve_tasks.m_first_batch_size;
                } else {
                    new (&new_job_created.m_queue_job) phys_solve_integrate_vel_job(solve_batch_job, *local_task_header);
                    new_job_open_count = local_task_header->m_integrate_vel_job_count;
                }
            }
            new_job_created.m_job_priority = 0;
            new_job_created.m_job_thread_type = 0;
            local_task_header->m_open_job_count = new_job_open_count;
            status = phys_job_queue::JOB_ENQUEUED;
        }
        upload_open_jobs(local_task_header, src_job.m_task_header);
        return status;
    }
    case SIM_JOB_SOLVE_INTEGRATE_VELS: {
        const phys_solve_integrate_vel_job &integrate_vel_job = reinterpret_cast<const phys_solve_integrate_vel_job &>(src_job);
        local_task_header->m_open_job_count--;
        if (local_task_header->m_open_job_count == 0) {
            int new_job_open_count;
            if (!integrate_vel_job.m_solving_finished) {
                new (&new_job_created.m_queue_job) phys_solve_constraint_batch_job(integrate_vel_job, *local_task_header);
                new_job_open_count = local_task_header->m_solve_tasks.m_first_batch_size;
            } else {
                new (&new_job_created.m_queue_job) phys_integrate_motion_job(integrate_vel_job, *local_task_header);
                new_job_open_count = local_task_header->m_build_tasks.m_jacobian_task_count;
            }
            new_job_created.m_job_priority = 0;
            new_job_created.m_job_thread_type = 0;
            local_task_header->m_open_job_count = new_job_open_count;
            goto job_was_created;
        }
        break;
    }
    case SIM_JOB_SOLVE_EXPORT_RESULTS: {
        const phys_solve_export_results_job &export_results_job = reinterpret_cast<const phys_solve_export_results_job &>(src_job);
        phys_build_jacobian_task_header *main_mem_task_header = export_results_job.m_task_header;
        local_task_header->m_open_job_count--;
        if (local_task_header->m_open_job_count == 0) {
            phys_dma_utils::set_i8_main_mem(&main_mem_task_header->m_export_done, char(1));
            local_task_header = 0;
            return phys_job_queue::JOB_NOT_CREATED;
        }
        break;
    }
    case SIM_JOB_INTEGRATE_MOTION: {
        const phys_integrate_motion_job &integrate_motion_job = reinterpret_cast<const phys_integrate_motion_job &>(src_job);
        if (integrate_motion_job.m_inactive_frame_count <= 5)
            phys_dma_utils::set_i32_main_mem(&integrate_motion_job.m_task_header->m_island_should_sleep, 0);
        jobs_to_finish = 1 + (unsigned(integrate_motion_job.m_entity_count - 1) / phys_integrate_motion_job::JOB_ACCUM_CAPACITY);
        // Continue with the shared broadphase completion count.
    }
    case SIM_JOB_SPLIT_ISLAND: {
        const phys_split_island_job &split_island_job = reinterpret_cast<const phys_split_island_job &>(src_job);
        int open_job_count = phys_dma_utils::increment_i32_main_mem(&split_island_job.m_task_header->m_broadphase_jobs_pending, -jobs_to_finish);
        if (open_job_count == 0) {
            new (&new_job_created.m_queue_job) phys_broadphase_job(split_island_job, split_island_job.m_task_header);
            new_job_created.m_job_priority = 0;
            new_job_created.m_job_thread_type = 1;
            return phys_job_queue::JOB_ENQUEUED;
        }
        return phys_job_queue::JOB_NOT_CREATED;
    }
    case SIM_JOB_AGENT_SECTOR: {
        const phys_agent_sector_job &agent_sector_job = reinterpret_cast<const phys_agent_sector_job &>(src_job);
        if (!agent_sector_job.m_header)
            return phys_job_queue::JOB_NOT_CREATED;
        int open_job_count = phys_dma_utils::increment_i32_main_mem(&agent_sector_job.m_header->m_open_job_count, -1);
        if (open_job_count == 0) {
            new (&new_job_created.m_queue_job) phys_post_collide_job(agent_sector_job);
            new_job_created.m_job_priority = 0;
            new_job_created.m_job_thread_type = 0;
            return phys_job_queue::JOB_ENQUEUED;
        }
        return phys_job_queue::JOB_NOT_CREATED;
    }
    default:
        break;
    }
    upload_open_jobs(local_task_header, src_job.m_task_header);
    return phys_job_queue::JOB_NOT_CREATED;
job_was_created:
    upload_open_jobs(local_task_header, src_job.m_task_header);
    return phys_job_queue::JOB_ENQUEUED;
}
template <> void phys_queue<phys_job_queue::job_queue_entry>::set_capacity(int wanted_capacity)
{
    int capacity = m_small_capacity;
    if (!(capacity >= wanted_capacity)) {
        if (!((capacity << 1) < wanted_capacity))
            wanted_capacity = capacity << 1;
        phys_job_queue::job_queue_entry *grown_storage = 
            (phys_job_queue::job_queue_entry *)phys_thread_memory::get_singleton_instance().allocate_chunk(wanted_capacity << 7, PHYS_MEM_ARRAY);
        phys_job_queue::job_queue_entry *prev_storage = m_data;
        if (prev_storage != 0) {
            if (m_used_count != 0) {
                int tail = m_tail;
                int head = m_head;
                if (tail <= head) {
                    int head_span_bytes = (m_small_capacity - head) << 7;
                    phys_string::mem_copy(grown_storage, prev_storage + head, head_span_bytes);
                    phys_string::mem_copy((char *)grown_storage + head_span_bytes, m_data, tail << 7);
                } else {
                    phys_string::mem_copy(grown_storage, prev_storage + head, m_used_count << 7);
                }
            }
            int size = m_used_count;
            m_head = 0;
            m_tail = size;
        }
        release_memory();
        m_data = grown_storage;
        m_small_capacity = wanted_capacity;
    }
}

// @address 0x663288 ?enqueue_front@?$phys_queue@Ujob_queue_entry@phys_job_queue@@@@QAAXABUjob_queue_entry@phys_job_queue@@@Z
template <> void phys_queue<struct phys_job_queue::job_queue_entry>::enqueue_front(struct phys_job_queue::job_queue_entry const &p0)
{
    if (m_used_count >= m_small_capacity) {
        if (m_small_capacity == 0)
            set_capacity(8);
        else
            set_capacity(m_small_capacity * 2);
    }
    if (m_head == 0)
        m_head = m_small_capacity;
    m_data[--m_head] = p0;
    m_used_count++;
}
