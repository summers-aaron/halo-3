// hkdynamics/hkCpuIntegrateMotionJob.obj: from the records 0x6603b0
// @flags /TP /O1
// @address 0x6603b0 ?phys_cpu_integrate_motion_job@@YA?AW4job_status@phys_job_queue@@AAUphys_mt_thread_state@@AAV2@AAUjob_queue_entry@2@W4job_wait_mode@2@@Z
#include "phys/phys_cpu_integrate_motion_job.h"

#include "phys/phys_job_queue.h"
#include "phys/phys_dynamics.h"
#include "phys/phys_rigid_motion_util.h"
#include "phys/phys_vel_accum.h"
#include "phys/phys_job_queue_util.h"
#include "phys/phys_dynamics_jobs.h"
#include "phys/phys_island.h"

phys_job_queue::job_status phys_cpu_integrate_motion_job(phys_mt_thread_state &thread, phys_job_queue &task_queue,
    phys_job_queue::job_queue_entry &entry, phys_job_queue::job_wait_mode job_wait_status)
{
    phys_integrate_motion_job *src_job = (phys_integrate_motion_job *)&entry;
    phys_world *world = thread.m_world;
    phys_vel_accum *accum_array = src_job->m_task_header->m_task_accums_base + src_job->m_first_entity_idx + 1;
    phys_motion *const *motion_list = reinterpret_cast<phys_motion *const *>(src_job->m_task_header->m_all_entities) + src_job->m_first_entity_idx;
    int result = phys_rigid_motion_apply_accums(world->m_dynamics_step_info.m_solver_info, world->m_dynamics_step_info.m_step_info,
        accum_array, motion_list, src_job->m_entity_count, (int)offsetof(phys_entity, m_motion));
    src_job->m_inactive_frame_count = result;
    return task_queue.finish_job_get_next(phys_job_queue::WORKER_THREAD_CPU, &entry, entry, 0, job_wait_status);
}
