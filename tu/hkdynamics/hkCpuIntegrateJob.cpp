// hkdynamics/hkCpuIntegrateJob.obj: from the records 0x660bb0 0x660bc0 0x660f28 0x661f30
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x660bb0 ?can_solve_single_threaded@@YA?AVphys_bool@@ABUphys_build_jacobian_task_header@@@Z
// @address 0x660bc0 ?append_new_task@@YAXPBUphys_build_jacobian_task_header@@PAUphys_build_jacobian_task_set@@PAUphys_solve_jacobians_tasks@@AAPAUphys_build_jacobian_task@@AAPAUphys_solve_constraint_batch_task@@44AAPAVphys_schema@@AAPAUphys_solver_elem_temp@@AAV?$phys_local_array@Uphys_constraint_internal_info@?A0xc725f199@@@@AAHHAAPAGVphys_bool@@V9@@Z
#include <ppcintrinsics.h>
#include "cseries/pointer_utilities.h"
#include "physics/havok_memory.h"
#include "phys/phys_base_types.h"
#include "phys/phys_math.h"
#include "phys/phys_schema.h"
#include "phys/phys_step_info.h"
#include "phys/phys_action.h"
struct phys_build_jacobian_task_header;
#include "phys/phys_base.h"
#include "phys/phys_memory.h"
#include "phys/phys_constraint_data.h"
#include "phys/phys_constraint.h"
#include "phys/phys_dynamics.h"
#include "phys/phys_local_buffer.h"
#include "phys/phys_island.h"
#include "phys/phys_solver_elem_temp.h"
#include "phys/phys_constraint_solver_setup.h"
#include "phys/phys_cpu_integrate_job.h"
#include "phys/phys_dynamics_jobs.h"
#include "phys/phys_job_queue_util.h"
class phys_constraint;
namespace {
struct phys_constraint_internal_info {
    __declspec(align(8)) const phys_constraint_internal *m_constraint_internal;
    unsigned short m_entity_a_idx;
    unsigned short m_entity_b_idx;
};
}
#include "phys/phys_local_array.h"
#include "phys/phys_ptr_map_base.h"
#include "phys/phys_math_prefetch.h"
struct phys_build_jacobian_task;
#include "phys/phys_vel_accum.h"
#include "phys/phys_rigid_motion_util.h"
#include "phys/phys_multithread_config.h"
#include "phys/phys_multithreaded_simulation.h"
#include "phys/phys_constraint_query_in.h"
#include "phys/phys_cpu_split_island_job.h"
struct phys_solve_constraint_batch_task;
phys_bool can_solve_single_threaded(const phys_build_jacobian_task_header &)
{
    return false;
}
// @address 0x660bc0
__forceinline void append_new_task(const phys_build_jacobian_task_header *task_header,
    phys_build_jacobian_task_set *out_build_tasks,
    phys_solve_jacobians_tasks *out_solve_tasks,
    phys_build_jacobian_task *&prev_build_task,
    phys_solve_constraint_batch_task *&prev_solve_task,
    phys_solve_constraint_batch_task *&prev_batch_first_task,
    phys_solve_constraint_batch_task *&cur_batch_first_task,
    phys_schema *&schema_list,
    phys_solver_elem_temp *&elem_temp,
    phys_local_array<phys_constraint_internal_info> &this_task_constraints,
    int &last_task_batch_idx,
    int cur_batch_idx,
    unsigned short *&batch_size_ptr,
    phys_bool ppu_only_build_list,
    phys_bool gen_solve_tasks)
{
    phys_schema *const schema_start = schema_list;
    phys_solver_elem_temp *const elem_temp_start = elem_temp;
    phys_build_jacobian_task *build_task = new phys_build_jacobian_task;
    if (prev_build_task == 0) {
        if (!ppu_only_build_list) {
            out_build_tasks->m_build_jacobian_tasks = build_task;
            out_build_tasks->m_jacobian_task_count = 0;
        }
    } else {
        prev_build_task->m_next = build_task;
        prev_build_task->m_next_task_schemas = schema_list;
    }
    out_build_tasks->m_jacobian_task_count = out_build_tasks->m_jacobian_task_count + 1;
    build_task->m_task_header = const_cast<phys_build_jacobian_task_header *>(task_header);
    build_task->m_atom_info_count = this_task_constraints.getSize();
    build_task->m_vel_accums = task_header->m_task_accums_base;
    build_task->m_schema_list = schema_start;
    int solver_constraint_count = this_task_constraints.getSize();
    for (int c = 0; c < solver_constraint_count; c++) {
        phys_constraint_internal_info &info = this_task_constraints[c];
        const phys_constraint_internal &internal = *info.m_constraint_internal;
        phys_build_jacobian_task::atom_info &jacobian_atom_info = build_task->m_atom_infos[c];
        jacobian_atom_info.m_data_atoms = internal.get_atoms();
        jacobian_atom_info.m_atoms_size = internal.get_atoms_size();
        jacobian_atom_info.m_instance = internal.m_contact_constraint;
        jacobian_atom_info.m_solver_runtime = internal.m_solver_runtime;
        jacobian_atom_info.m_runtime_size = internal.m_runtime_size;
        int integrate_body_index_a = info.m_entity_a_idx;
        int integrate_body_index_b = info.m_entity_b_idx;
        jacobian_atom_info.m_accum_idx_a = (unsigned short)(integrate_body_index_a + 1);
        jacobian_atom_info.m_accum_idx_b = (unsigned short)(integrate_body_index_b + 1);
        jacobian_atom_info.m_transform_a = &internal.m_island_entities[0]->get_motion()->get_motion_state()->get_transform();
        jacobian_atom_info.m_transform_b = &internal.m_island_entities[1]->get_motion()->get_motion_state()->get_transform();
        schema_list = schema_list + internal.m_schemas_size;
        elem_temp = elem_temp + internal.m_solver_temp_count;
    }
    this_task_constraints.clear();
    schema_list = schema_list + 16;
    build_task->m_next_task_schemas = schema_list;
    elem_temp = (phys_solver_elem_temp *)align_pointer(elem_temp, 4);
    prev_build_task = build_task;
    if (gen_solve_tasks) {
        phys_solve_constraint_batch_task *solve_task = new phys_solve_constraint_batch_task();
        if (prev_solve_task != 0)
            prev_solve_task->m_next = solve_task;
        if (cur_batch_idx != last_task_batch_idx) {
            if (cur_batch_idx == 0)
                out_solve_tasks->m_first_solve_task = solve_task;
            last_task_batch_idx = cur_batch_idx;
            if (prev_solve_task != 0) {
                phys_solve_constraint_batch_task *batch_task = prev_batch_first_task;
                unsigned short batch_size = *batch_size_ptr;
                while (batch_task != 0 && batch_task != cur_batch_first_task) {
                    batch_task->m_next_batch_size = batch_size;
                    batch_task->m_next_batch_first_task = cur_batch_first_task;
                    batch_task = batch_task->m_next;
                }
                prev_solve_task->m_last_task_in_batch = true;
                batch_size_ptr = &prev_solve_task->m_next_batch_size;
            }
            prev_batch_first_task = cur_batch_first_task;
            cur_batch_first_task = solve_task;
        }
        *batch_size_ptr = *batch_size_ptr + 1;
        solve_task->m_task_header = const_cast<phys_build_jacobian_task_header *>(task_header);
        solve_task->m_vel_accums = task_header->m_task_accums_base;
        solve_task->m_schema_list = schema_start;
        solve_task->m_solver_elem_temp = elem_temp_start;
        solve_task->m_schema_buffer_size = (int)((char *)schema_list - (char *)schema_start);
        solve_task->m_solver_temp_buffer_size = (int)((char *)elem_temp - (char *)elem_temp_start);
        prev_solve_task = solve_task;
    }
}
static __forceinline int phys_cpu_integrate_job_max2(int x, int y)
{
    return x > y ? x : y;
}
static __forceinline bool phys_world_op_island_can_be_sparse(phys_world *world, int island_size)
{
    return island_size < world->m_min_island_size;
}
// @address 0x660f28 ?create_jacobian_task_set@@YAXPAVphys_island@@Vphys_bool@@PAUphys_build_jacobian_task_header@@PAUphys_build_jacobian_task_set@@PAUphys_solve_jacobians_tasks@@@Z
void create_jacobian_task_set(phys_island *island, phys_bool force_coherent_order,
    phys_build_jacobian_task_header *task_header, phys_build_jacobian_task_set *out_build_tasks,
    phys_solve_jacobians_tasks *out_solve_tasks)
{
    phys_entity *const *const bodies = island->get_entities().begin();
    int solver_body_count = island->get_entities().getSize();
    phys_entity *const *const bodies_end = bodies + solver_body_count;
    phys_local_array<phys_constraint_internal_info> normal_priority_constraints(island->m_constraint_count);
    phys_local_array<phys_constraint_internal_info> high_priority_constraints(island->m_constraint_count);
    phys_local_array<phys_constraint_internal_info> callback_constraints(island->m_constraint_count);
    int all_normal_priority = 1;
    int map_capacity_po2 = 64;
    {
        while (map_capacity_po2 < solver_body_count) {
            map_capacity_po2 *= 2;
        }
        map_capacity_po2 *= 4;
        phys_local_buffer<phys_ulong> ptr_map_storage(map_capacity_po2);
        phys_ptr_map_base<phys_ulong> entity_ptr_to_idx(ptr_map_storage.begin(), map_capacity_po2 * sizeof(phys_ulong));
        {
            for (int bi = 0; bi < solver_body_count; bi++) {
                entity_ptr_to_idx.insert((phys_ulong)bodies[bi], (phys_ulong)bi);
            }
        }
        phys_entity *const *b = bodies;
        unsigned int accum_offset = sizeof(phys_vel_accum);
        int bi;
        for (b = bodies, bi = 0; b < bodies_end; b++, bi++) {
            phys_entity *body = *b;
            if (b + 4 < bodies_end) {
                const void *p = b[2]->get_constraint_masters().begin();
                int size = b[2]->get_constraint_masters().getSize() * sizeof(phys_constraint_internal);
                do {
                    phys_math::force_prefetch<256>(p);
                    p = phys_add_byte_offset_const(p, 256);
                    size -= 256;
                } while (size > 0);
                phys_math::prefetch_128(&(b[4]->m_solver_data));
            }
            {
                if (accum_offset != body->m_solver_data) {
                    body->m_solver_data = accum_offset;
                }
                accum_offset += sizeof(phys_vel_accum);
            }
            const phys_constraint_internal *c = body->get_constraint_masters().begin();
            const phys_constraint_internal *constraints_end = body->get_constraint_masters().end();
            for (; c < constraints_end; c++) {
                phys_constraint_internal_info info;
                {
                    info.m_constraint_internal = c;
                    unsigned short *entity_indices = &info.m_entity_a_idx;
                    const int slave_side = 1 - c->m_master_body;
                    entity_indices[c->m_master_body] = (unsigned short)bi;
                    entity_indices[slave_side] = (unsigned short)entity_ptr_to_idx.get_with_default((phys_ulong)c->m_island_entities[slave_side], 0xffff);
                }
                all_normal_priority &= c->m_normal_type;
                if (c->m_callback_request & 3) {
                    callback_constraints.push_back_unchecked(info);
                }
                if (c->m_priority < phys_constraint::CONSTRAINT_PRIORITY_TOI_HIGHER) {
                    normal_priority_constraints.push_back_unchecked(info);
                } else {
                    high_priority_constraints.push_back_unchecked(info);
                }
            }
        }
    }
    task_header->m_solve_single_thread_ppu = (0 == all_normal_priority);
    task_header->m_solve_single_thread = task_header->m_solve_single_thread_ppu || can_solve_single_threaded(*task_header);
    phys_bool gen_solve_tasks = !task_header->m_solve_single_thread;
    phys_schema *schema_list = task_header->m_schema_base;
    phys_solver_elem_temp *elem_temp = task_header->m_solver_temp_base;
    int cur_batch_idx = -1;
    int last_task_batch_idx = -1;
    unsigned short *batch_size_ptr = &out_solve_tasks->m_first_batch_size;
    *batch_size_ptr = 0;
    phys_build_jacobian_task *prev_build_task = 0;
    phys_solve_constraint_batch_task *prev_solve_task = 0;
    phys_solve_constraint_batch_task *prev_batch_first_task = 0;
    phys_solve_constraint_batch_task *cur_batch_first_task = 0;
    phys_local_array<phys_constraint_internal_info> next_task_constraints(phys_build_jacobian_task::ATOM_INFO_MAX);
    phys_local_buffer<int> entity_groups(solver_body_count);
    for (int priority_pass = 0; priority_pass < 2; priority_pass++) {
        phys_bool ppu_only_build_list = false;
        phys_array<phys_constraint_internal_info> *source_constraints;
        switch (priority_pass) {
        case 0:
            source_constraints = &normal_priority_constraints;
            prev_build_task = 0;
            break;
        default:
            source_constraints = &high_priority_constraints;
            break;
        }
        while (source_constraints->getSize()) {
            for (int i = 0; i < solver_body_count; entity_groups[i++] = 0) {
            }
            int cur_group_id = 1;
            cur_batch_idx++;
            phys_constraint_internal_info *c = source_constraints->begin();
            phys_constraint_internal_info *const constraints_end = source_constraints->end();
            phys_constraint_internal_info *next_left_out = source_constraints->begin();
            for (; c < constraints_end; c++) {
                const phys_constraint_internal_info &info = *c;
                int fixed_entity_group = 0;
                int &entity_a_group = (info.m_entity_a_idx == 0xffff) ? fixed_entity_group : entity_groups[info.m_entity_a_idx];
                int &entity_b_group = (info.m_entity_b_idx == 0xffff) ? fixed_entity_group : entity_groups[info.m_entity_b_idx];
                if (next_task_constraints.getSize() == phys_build_jacobian_task::ATOM_INFO_MAX) {
                    append_new_task(task_header, out_build_tasks, out_solve_tasks, prev_build_task, prev_solve_task, prev_batch_first_task,
                        cur_batch_first_task, schema_list, elem_temp, next_task_constraints, last_task_batch_idx, cur_batch_idx,
                        batch_size_ptr, ppu_only_build_list, gen_solve_tasks);
                    cur_group_id += gen_solve_tasks;
                }
                if (force_coherent_order && cur_group_id > 1) {
                    for (; c < constraints_end; c++) {
                        *(next_left_out++) = *c;
                    }
                    break;
                } else {
                    if ((entity_a_group != 0 && entity_a_group != cur_group_id) || (entity_b_group != 0 && entity_b_group != cur_group_id)) {
                        *(next_left_out++) = *c;
                    } else {
                        next_task_constraints.push_back_unchecked(*c);
                        entity_a_group = cur_group_id;
                        entity_b_group = cur_group_id;
                    }
                }
            }
            if (next_task_constraints.getSize()) {
                append_new_task(task_header, out_build_tasks, out_solve_tasks, prev_build_task, prev_solve_task, prev_batch_first_task,
                    cur_batch_first_task, schema_list, elem_temp, next_task_constraints, last_task_batch_idx, cur_batch_idx,
                    batch_size_ptr, ppu_only_build_list, gen_solve_tasks);
                cur_group_id++;
            }
            const int new_source_count = int(next_left_out - source_constraints->begin());
            source_constraints->set_size_unchecked(new_source_count);
        }
    }
    if (gen_solve_tasks) {
        {
            phys_solve_constraint_batch_task *prev_batch_task = prev_batch_first_task;
            unsigned short batch_size = *batch_size_ptr;
            while (prev_batch_task && prev_batch_task != cur_batch_first_task) {
                prev_batch_task->m_next_batch_size = batch_size;
                prev_batch_task->m_next_batch_first_task = cur_batch_first_task;
                prev_batch_task = prev_batch_task->m_next;
            }
            prev_solve_task->m_last_task_in_batch = true;
        }
        const unsigned int accumulator_count = 1 + task_header->m_all_entity_count;
        task_header->m_accums_end = task_header->m_task_accums_base + accumulator_count;
        task_header->m_gravity_job_count = 1 + (accumulator_count - 1) / 64;
        task_header->m_integrate_vel_job_count = 1 + (accumulator_count - 1) / 64;
    }
    {
        out_build_tasks->m_callback_constraint_count = 0;
        out_build_tasks->m_callback_constraints = 0;
        int callback_constraint_cnt = callback_constraints.getSize();
        if (callback_constraint_cnt) {
            const phys_constraint_internal **cc = phys_alloc_chunk<const phys_constraint_internal *>(callback_constraint_cnt, PHYS_MEM_CONSTRAINT_SOLVER);
            out_build_tasks->m_callback_constraint_count = callback_constraint_cnt;
            out_build_tasks->m_callback_constraints = cc;
            for (int i = 0; i < callback_constraint_cnt; i++) {
                cc[i] = callback_constraints[i].m_constraint_internal;
            }
        }
    }
}
// @address 0x661f30 ?run_integrate_job@@YA?AW4job_status@phys_job_queue@@AAUphys_mt_thread_state@@AAV2@AAUjob_queue_entry@2@@Z
phys_job_queue::job_status run_integrate_job(phys_mt_thread_state &tl, phys_job_queue &shared_job_queue, phys_job_queue::job_queue_entry &agent_next_job_out)
{
    const phys_integrate_job &src_job = reinterpret_cast<phys_integrate_job &>(agent_next_job_out);
    phys_island *island = src_job.m_sim_island;
    const phys_world_dynamics_step_info &step_info = tl.m_world->m_dynamics_step_info;
    if (!tl.m_world->m_single_thread_actions) {
        phys_array<phys_action *> &actions = island->m_actions;
        for (int j = 0; j < actions.getSize(); j++) {
            phys_action *action = actions[j];
            if (action) {
                action->apply_action(step_info.m_step_info);
            }
        }
    }
    phys_build_jacobian_task_header *task_header = new phys_build_jacobian_task_header;
    phys_entity *const *entities = island->get_entities().begin();
    unsigned int solver_entity_count = island->get_entities().getSize();
    task_header->m_all_entity_count = (unsigned short)solver_entity_count;
    task_header->m_all_entities = entities;
    task_header->m_entity_capacity = (unsigned short)island->get_entities().get_sector_capacity();
    task_header->m_broadphase_jobs_pending = 0;
    task_header->m_island_should_sleep = 1;
    task_header->m_buffer = 0;
    task_header->m_task_buf_size = 0;
    task_header->m_build_tasks.m_build_jacobian_tasks = 0;
    int split_requested = 0;
    {
        island->m_split_check_frames++;
        if (!island->m_sparse_enabled) {
            if (island->m_split_check_requested && ((island->m_split_check_frames & 0x7) == 0) && tl.m_world->m_want_islands) {
                split_requested = 1;
            }
        } else {
            const int island_size = phys_cpu_integrate_job_max2(island->m_island_entities.getSize(), island->m_constraint_count);
            const phys_bool can_island_be_sparse = phys_world_op_island_can_be_sparse(tl.m_world, island_size);
            if (!can_island_be_sparse) {
                island->m_sparse_enabled = false;
                if (phys_bool((island->m_split_check_frames & 0x3) == 0)) {
                    split_requested = 1;
                }
            } else {
                if ((island->m_split_check_frames & 0x7) == 0) {
                    split_requested = 1;
                }
            }
        }
    }
    if (island->m_constraint_info.m_schemas_size == 0) {
        int inactive_frame_count = phys_rigid_motion_force_step(step_info.m_solver_info, step_info.m_step_info,
            step_info.m_solver_info.m_gravity_per_step, (phys_motion *const *)island->m_island_entities.begin(), island->m_island_entities.getSize(),
            (int)offsetof(phys_entity, m_motion));
        if (inactive_frame_count <= 5) {
            task_header->m_island_should_sleep = 0;
        }
    } else {
        unsigned int est_constraint_count = phys_cpu_integrate_job_max2(island->m_constraint_count, island->get_entities().getSize());
        if (est_constraint_count < tl.m_world_simulation->m_mt_config.m_max_serial_constraints) {
        {
            int inactive_frame_count = phys_solver_setup::solve_constraints(
                step_info.m_step_info, step_info.m_solver_info, tl.m_constraint_query_in, *island, entities, solver_entity_count);
            if (inactive_frame_count <= 5) {
                task_header->m_island_should_sleep = 0;
            }
        }
        } else {
        {
            int solver_buf_size = phys_solver_setup::compute_solver_buffer_size(*island);
            char *solver_buf = (char *)phys_memory::get_singleton_instance().allocate_runtime_block(solver_buf_size, PHYS_MEM_CONSTRAINT);
            phys_solver_setup::compute_solve_buffer_offsets(*island, solver_buf, solver_buf_size, *task_header);
            task_header->m_solver_result_count = island->m_constraint_info.m_solver_result_count;
            task_header->m_solver_temp_count = island->m_constraint_info.m_solver_temp_count;
            task_header->m_constraint_query_in = &tl.m_constraint_query_in;
            task_header->m_export_done = 0;
            int integrate_job_count = 1 + ((solver_entity_count - 1) / 64);
            task_header->m_broadphase_jobs_pending = split_requested + integrate_job_count;
            task_header->m_split_check_requested = (signed char)split_requested;
            phys_build_accums_job &j = reinterpret_cast<phys_build_accums_job &>(agent_next_job_out);
            {
                j.m_job_type = SIM_JOB_BUILD_ACCUMS;
                j.m_task_header = task_header;
                j.m_island_entity_array = entities;
                j.m_first_entity_idx = 0;
                j.m_entity_count = (unsigned short)solver_entity_count;
            }
            task_header->m_open_job_count = 1 + 1 + ((solver_entity_count - 1) / 64);
            shared_job_queue.add_job(agent_next_job_out, phys_job_queue::JOB_PRIORITY_HIGH, phys_job_queue::JOB_KIND_HOST);
        }
        phys_build_jacobian_task_set out_build_tasks;
        phys_solve_jacobians_tasks out_solve_tasks;
        {
            create_jacobian_task_set(island, step_info.m_solver_info.m_coherent_solver_order,
                task_header, &out_build_tasks, &out_solve_tasks);
        }
        task_header->m_solve_tasks = out_solve_tasks;
        task_header->m_build_tasks = out_build_tasks;
        phys_create_jacobian_tasks_job create_tasks_job(src_job, task_header);
        phys_job_queue::job_status status = shared_job_queue.finish_job_get_next(
            tl.m_thread_type, (phys_job_queue::job_queue_entry *)&create_tasks_job, agent_next_job_out, &tl, phys_job_queue::JOB_WAIT_FOR_NEXT);
        return status;
        }
    }
        if (split_requested) {
            phys_cpu_split_island_job_impl(island, task_header->m_new_split_islands);
        }
        {
            task_header->m_export_done = 1;
            new (&agent_next_job_out) phys_broadphase_job(src_job, task_header);
            shared_job_queue.add_job_get_next(phys_job_queue::JOB_KIND_HOST_BROADPHASE, phys_job_queue::JOB_PRIORITY_HIGH, tl.m_thread_type, agent_next_job_out,
                phys_job_queue::JOB_WAIT_FOR_NEXT);
        }
        return phys_job_queue::JOB_QUEUE_HAS_WORK;
}

