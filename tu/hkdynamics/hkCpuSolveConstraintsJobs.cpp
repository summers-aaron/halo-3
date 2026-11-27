// hkdynamics/hkCpuSolveConstraintsJobs.obj: from the records 0x660430 0x660480 0x660538 0x6605a0 0x6605f8
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x660430 ?phys_cpu_solve_apply_gravity_job@@YA?AW4job_status@phys_job_queue@@AAUphys_mt_thread_state@@AAV2@AAUjob_queue_entry@2@W4job_wait_mode@2@@Z
// @address 0x660480 ?phys_cpu_solve_constraint_batch_job@@YA?AW4job_status@phys_job_queue@@AAUphys_mt_thread_state@@AAV2@AAUjob_queue_entry@2@W4job_wait_mode@2@@Z
// @address 0x660538 ?phys_cpu_solve_integrate_vels_job@@YA?AW4job_status@phys_job_queue@@AAUphys_mt_thread_state@@AAV2@AAUjob_queue_entry@2@W4job_wait_mode@2@@Z
// @address 0x6605a0 ?phys_cpu_solve_export_results_job@@YA?AW4job_status@phys_job_queue@@AAUphys_mt_thread_state@@AAV2@AAUjob_queue_entry@2@W4job_wait_mode@2@@Z
// @address 0x6605f8 ?phys_cpu_solve_constraints_job@@YA?AW4job_status@phys_job_queue@@AAUphys_mt_thread_state@@AAV2@AAUjob_queue_entry@2@W4job_wait_mode@2@@Z
#include "phys/phys_cpu_solve_constraints_jobs.h"
#include "phys/phys_dynamics.h"
#include "phys/phys_job_queue.h"

#include "phys/phys_job_queue_util.h"
#include "phys/phys_jacobian_element.h"
#include "phys/phys_dynamics_jobs.h"
#include "phys/phys_solver_export.h"
#include "phys/phys_solver.h"
#include "phys/phys_solver_functions.h"
#include "phys/phys_island.h"
#include "phys/phys_vel_accum.h"
namespace {
static __forceinline void mem_set16(void *destination, const void *value, int count)
{
    const unsigned int *source = static_cast<const unsigned int *>(value);
    unsigned int *target = static_cast<unsigned int *>(destination);
    for (int i = 0; i < count; ++i) {
        target[0] = source[0];
        target[1] = source[1];
        target[2] = source[2];
        target[3] = source[3];
        target += 4;
    }
}
}

phys_job_queue::job_status phys_cpu_solve_apply_gravity_job(phys_mt_thread_state &thread, phys_job_queue &task_queue,
    phys_job_queue::job_queue_entry &entry, phys_job_queue::job_wait_mode job_wait_status)
{
    phys_solve_apply_gravity_job &src_job = reinterpret_cast<phys_solve_apply_gravity_job &>(entry);
    phys_world *world = thread.m_world;
    phys_solve_apply_gravity_steps(world->m_dynamics_step_info.m_solver_info, src_job.m_vel_accums, src_job.m_accums_end);
    return task_queue.finish_job_get_next(phys_job_queue::WORKER_THREAD_CPU, &entry, entry, 0, job_wait_status);
}

phys_job_queue::job_status phys_cpu_solve_constraint_batch_job(phys_mt_thread_state &thread, phys_job_queue &task_queue,
    phys_job_queue::job_queue_entry &entry, phys_job_queue::job_wait_mode job_wait_status)
{
    phys_solve_constraint_batch_job &src_job = reinterpret_cast<phys_solve_constraint_batch_job &>(entry);
    phys_vel_accum *accum_array = src_job.m_solve_batch_task->m_vel_accums;
    const phys_schema *schema_list = src_job.m_solve_batch_task->m_schema_list;
    phys_solver_elem_temp *solver_temp = src_job.m_solve_batch_task->m_solver_elem_temp;
    src_job.m_solver_micro_steps = (thread.m_world)->m_dynamics_step_info.m_solver_info.m_micro_step_count;
    if (src_job.m_solver_step == 0) {
        unsigned int solver_temp_bytes = (src_job.m_solve_batch_task->m_solver_temp_buffer_size + 15) & ~15;
        __declspec(align(16)) unsigned int zero[4] = { 0, 0, 0, 0 };
        mem_set16(solver_temp, zero, solver_temp_bytes >> 4);
    }
    phys_world *world = thread.m_world;
    phys_solve_constraints_steps(world->m_dynamics_step_info.m_solver_info, src_job.m_solver_step, src_job.m_solver_micro_step,
        schema_list, accum_array, solver_temp);
    return task_queue.finish_job_get_next(phys_job_queue::WORKER_THREAD_CPU, &entry, entry, 0, job_wait_status);
}

phys_job_queue::job_status phys_cpu_solve_integrate_vels_job(phys_mt_thread_state &thread, phys_job_queue &task_queue,
    phys_job_queue::job_queue_entry &entry, phys_job_queue::job_wait_mode job_wait_status)
{
    phys_solve_integrate_vel_job &src_job = reinterpret_cast<phys_solve_integrate_vel_job &>(entry);
    phys_bool finished = false;
    phys_world *world = thread.m_world;
    phys_solve_integrate_vel_steps(world->m_dynamics_step_info.m_solver_info, src_job.m_solver_step,
        src_job.m_vel_accums, src_job.m_accums_end, finished);
    src_job.m_solving_finished = finished;
    return task_queue.finish_job_get_next(phys_job_queue::WORKER_THREAD_CPU, &entry, entry, 0, job_wait_status);
}

phys_job_queue::job_status phys_cpu_solve_export_results_job(phys_mt_thread_state &thread, phys_job_queue &task_queue,
    phys_job_queue::job_queue_entry &entry, phys_job_queue::job_wait_mode job_wait_status)
{
    phys_solve_export_results_job &src_job = reinterpret_cast<phys_solve_export_results_job &>(entry);
    phys_export_impulses_and_rhs(thread.m_world->m_dynamics_step_info.m_solver_info,
        src_job.m_solve_batch_task->m_solver_elem_temp, src_job.m_solve_batch_task->m_schema_list, src_job.m_solve_batch_task->m_vel_accums);
    return task_queue.finish_job_get_next(phys_job_queue::WORKER_THREAD_CPU, &entry, entry, 0, job_wait_status);
}

phys_job_queue::job_status phys_cpu_solve_constraints_job(phys_mt_thread_state &thread, phys_job_queue &task_queue,
    phys_job_queue::job_queue_entry &entry, phys_job_queue::job_wait_mode job_wait_status)
{
    phys_solve_constraints_job &src_job = reinterpret_cast<phys_solve_constraints_job &>(entry);
    phys_vel_accum *accum_array = static_cast<phys_vel_accum *>(phys_add_byte_offset(src_job.m_buffer, src_job.m_accums_offset));
    const phys_schema *schema_list = static_cast<const phys_schema *>(phys_add_byte_offset(src_job.m_buffer, src_job.m_schema_offset));
    phys_solver_elem_temp *solver_temp = static_cast<phys_solver_elem_temp *>(phys_add_byte_offset(src_job.m_buffer, src_job.m_solver_temp_offset));
    unsigned int elem_temp_bytes = (src_job.m_solver_temp_count * sizeof(phys_solver_elem_temp) + 15) & ~15;
    __declspec(align(16)) unsigned int zero[4] = { 0, 0, 0, 0 };
    mem_set16(solver_temp, zero, elem_temp_bytes >> 4);
    phys_world *world = thread.m_world;
    phys_solve_constraints(world->m_dynamics_step_info.m_solver_info, schema_list, accum_array, solver_temp);
    phys_build_jacobian_task_header *task_header = src_job.m_task_header;
    phys_job_queue::job_queue_entry integrate_entry;
    new (&integrate_entry) phys_integrate_motion_job(src_job, *task_header);
    task_queue.add_job(integrate_entry, phys_job_queue::JOB_PRIORITY_HIGH, phys_job_queue::JOB_KIND_HOST);
    phys_export_impulses_and_rhs(thread.m_world->m_dynamics_step_info.m_solver_info,
        task_header->m_solver_temp_base, task_header->m_schema_base, task_header->m_task_accums_base);
    return task_queue.finish_job_get_next(phys_job_queue::WORKER_THREAD_CPU, &entry, entry, 0, job_wait_status);
}

