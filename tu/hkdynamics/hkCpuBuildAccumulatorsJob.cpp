// hkdynamics/hkCpuBuildAccumulatorsJob.obj: from the records 0x660b40
// @compiler xdk6130
// @flags /TP /O1
// @address 0x660b40 ?phys_cpu_build_accumulators_job@@YA?AW4job_status@phys_job_queue@@AAUphys_mt_thread_state@@AAV2@AAUjob_queue_entry@2@@Z
#include "phys/phys_cpu_build_accums_job.h"
#include "phys/phys_job_queue.h"
#include "phys/phys_job_queue_util.h"
#include "phys/phys_vel_accum.h"
#include "phys/phys_rigid_motion_util.h"
#include "phys/phys_dynamics_jobs.h"
#include "phys/phys_island.h"
#include "phys/phys_dynamics.h"
phys_job_queue::job_status phys_cpu_build_accumulators_job(phys_mt_thread_state &thread, phys_job_queue &queue, phys_job_queue::job_queue_entry &entry)
{
    phys_build_accums_job &src_job = reinterpret_cast<phys_build_accums_job &>(entry);
    phys_motion *const *motion_list = reinterpret_cast<phys_motion *const *>(src_job.m_island_entity_array) + src_job.m_first_entity_idx;
    int motion_count = src_job.m_entity_count;
    int motion_stride = (int)offsetof(phys_entity, m_motion);
    phys_vel_accum *accum_array = src_job.m_task_header->m_task_accums_base + 1 + src_job.m_first_entity_idx;
    phys_rigid_motion_build_accums(
        thread.m_collision_input.m_step_info, motion_list, motion_count, motion_stride, accum_array);
    return queue.finish_job_get_next(thread.m_thread_type, &entry, entry, &thread, phys_job_queue::JOB_WAIT_FOR_NEXT);
}

