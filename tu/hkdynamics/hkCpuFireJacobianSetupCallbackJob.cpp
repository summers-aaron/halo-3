// hkdynamics/hkCpuFireJacobianSetupCallbackJob.obj: from the records 0x6609e0
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "phys/phys_simple_contact_data.h"
#include "phys/phys_atoms.h"
#include "phys/phys_job_queue_util.h"
#include "phys/phys_dynamics_jobs.h"
#include "phys/phys_dynamics.h"
#include "phys/phys_constraint.h"
#include "phys/phys_thread_memory.h"
#include "phys/phys_single_thread_island_jobs.h"
struct phys_simple_contact_atom;

struct phys_constraint_internal;
#include "phys/phys_world_constraint_util.h"
// @address 0x6609e0 ?run_jacobian_callback_job@phys_single_thread_island_jobs@@SA?AW4job_status@phys_job_queue@@AAUphys_mt_thread_state@@AAV3@AAUjob_queue_entry@3@W4job_wait_mode@3@@Z
phys_job_queue::job_status phys_single_thread_island_jobs::run_jacobian_callback_job(phys_mt_thread_state &tl,
    phys_job_queue &shared_job_queue, phys_job_queue::job_queue_entry &solver_job_in_out, phys_job_queue::job_wait_mode queue_wait_status)
{
    phys_constraint_query_in *in = &tl.m_constraint_query_in;
    phys_fire_jacobian_setup_callback &src_job = reinterpret_cast<phys_fire_jacobian_setup_callback &>(solver_job_in_out);
    phys_build_jacobian_task_header *task = src_job.m_task_header;
    if (task->m_build_tasks.m_callback_constraint_count > 0) {
        int index = 0;
        do {
            const phys_constraint_internal *ci = task->m_build_tasks.m_callback_constraints[index];
            in->m_query_constraint = ci->m_contact_constraint;
            in->m_constraint_runtime = ci->m_solver_runtime;
            in->m_body_a = phys_add_byte_offset(task->m_task_accums_base, ci->m_island_entities[0]->m_solver_data);
            in->m_body_b = phys_add_byte_offset(task->m_task_accums_base, ci->m_island_entities[1]->m_solver_data);
            in->m_transform_a = &ci->m_contact_constraint->get_entity_a()->get_collider()->get_transform();
            in->m_transform_b = &ci->m_contact_constraint->get_entity_b()->get_collider()->get_transform();
            if (ci->m_callback_request & 1) {
                phys_constraint_data *contact_constraint_data = ci->m_contact_constraint->get_chain_data();
                phys_atom *atom = phys_world_constraint_util::get_terminal_atom(ci);
                phys_simple_contact_fire_callbacks((phys_simple_contact_data *)contact_constraint_data, in, (phys_simple_contact_atom *)atom);
            }
            if (ci->m_callback_request & 4) {
                phys_constraint_data *contact_constraint_data = ci->m_contact_constraint->get_chain_data();
                contact_constraint_data->build_jacobian_callback(*in);
            }
            index = index + 1;
        } while (index < task->m_build_tasks.m_callback_constraint_count);
        int count = task->m_build_tasks.m_callback_constraint_count;
        const phys_constraint_internal **callbacks = task->m_build_tasks.m_callback_constraints;
        phys_thread_memory *system_thread_memory = &phys_thread_memory::get_singleton_instance();
        system_thread_memory->free_chunk(callbacks, count * (int)sizeof(void *), static_cast<phys_memory_class>(3));
        task->m_build_tasks.m_callback_constraints = 0;
        task->m_build_tasks.m_callback_constraint_count = 0;
    }
    return shared_job_queue.finish_job_get_next(phys_job_queue::WORKER_THREAD_CPU, &solver_job_in_out, solver_job_in_out, 0, queue_wait_status);
}

