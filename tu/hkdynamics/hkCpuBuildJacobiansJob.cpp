// hkdynamics/hkCpuBuildJacobiansJob.obj: from the records 0x660708 0x660850
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "phys/phys_cpu_build_jacobians_job.h"
#include "phys/phys_base_types.h"

#include <ppcintrinsics.h>
#include "phys/phys_job_queue.h"



#include "phys/phys_atoms.h"
#include "phys/phys_build_jacobian_from_atoms.h"

#include "phys/phys_vel_accum.h"

#include "phys/phys_constraint_query_in.h"

#include "phys/phys_constraint_query_out.h"

#include "phys/phys_job_queue_util.h"

#include "phys/phys_dynamics_jobs.h"

namespace {
struct phys_build_jacobians_task_input {
    phys_build_jacobian_task::atom_info *m_atom_infos;
    int m_atom_info_count;
    const phys_vel_accum *m_vel_accums;
    char *m_schema_list;
    char *m_next_task_schemas;
    void *m_next_task;
    phys_bool m_finish_schemas_goto;
};
}

#include "phys/phys_header_schema.h"


#include "phys/phys_solver_build_jacobian.h"

// @address 0x660708 ?phys_cpu_build_jacobians_from_task@@YAXABUphys_build_jacobians_task_input@?A0xe40593d7@@AAVphys_constraint_query_in@@@Z
__forceinline void phys_cpu_build_jacobians_from_task(const phys_build_jacobians_task_input &input, phys_constraint_query_in &solver_query_in)
{
    phys_constraint_query_out query_out;
    query_out.m_jacobian_schemas = reinterpret_cast<phys_schema *>(input.m_schema_list);

    const phys_build_jacobian_task::atom_info *atom_infos = input.m_atom_infos;
    int atom_info_cnt = input.m_atom_info_count;

    for (int a = 0; a < atom_info_cnt; )
    {
        const phys_build_jacobian_task::atom_info &jacobian_atom_info = atom_infos[a];
        a++;
        if (a < atom_info_cnt)
        {
            const phys_build_jacobian_task::atom_info &next = atom_infos[a];
            char *p = (char *)next.m_data_atoms;
            __dcbt(0, p);
            __dcbt(128, p);
            __dcbt(0, next.m_solver_runtime);
            __dcbt(0, phys_add_byte_offset(query_out.m_jacobian_schemas.val(), 512));
        }
        {
            solver_query_in.m_transform_a = jacobian_atom_info.m_transform_a;
            solver_query_in.m_transform_b = jacobian_atom_info.m_transform_b;
            solver_query_in.m_body_a = input.m_vel_accums + jacobian_atom_info.m_accum_idx_a;
            solver_query_in.m_body_b = input.m_vel_accums + jacobian_atom_info.m_accum_idx_b;
            solver_query_in.m_accum_a_index = jacobian_atom_info.m_accum_idx_a;
            solver_query_in.m_accum_b_index = jacobian_atom_info.m_accum_idx_b;
            solver_query_in.m_query_constraint = jacobian_atom_info.m_instance;
            solver_query_in.m_constraint_runtime = jacobian_atom_info.m_solver_runtime;
            solver_query_in.m_runtime_in_main_memory = jacobian_atom_info.m_solver_runtime;
        }
        {
            phys_solver_build_jacobian_atoms(jacobian_atom_info.m_data_atoms, jacobian_atom_info.m_atoms_size, solver_query_in, query_out);
        }
    }
    {
        if (!input.m_finish_schemas_goto)
        {
            phys_schema *end_schema = query_out.m_jacobian_schemas;
            *(reinterpret_cast<int *>(end_schema)) = 0;
            query_out.m_jacobian_schemas = phys_add_byte_offset(end_schema, 16);
        }
        else
        {
            int branch_offset = phys_get_byte_offset_int(query_out.m_jacobian_schemas.val(), input.m_next_task_schemas);
            if (branch_offset > 0)
            {
                phys_goto_schema *goto_schema = reinterpret_cast<phys_goto_schema *>(query_out.m_jacobian_schemas.val());
                goto_schema->init_goto_offset(branch_offset);
                query_out.m_jacobian_schemas = goto_schema + 1;
            }
        }
    }
}

// @address 0x660850 ?phys_cpu_build_jacobians_job@@YA?AW4job_status@phys_job_queue@@AAUphys_mt_thread_state@@AAV2@AAUjob_queue_entry@2@W4job_wait_mode@2@@Z
phys_job_queue::job_status phys_cpu_build_jacobians_job(phys_mt_thread_state &tl, phys_job_queue &shared_job_queue,
    phys_job_queue::job_queue_entry &solver_job_in_out, phys_job_queue::job_wait_mode queue_wait_status)
{
    const phys_build_jacobians_job &src_job = reinterpret_cast<phys_build_jacobians_job &>(solver_job_in_out);

    phys_constraint_query_in &solver_query_in = tl.m_constraint_query_in;

    phys_build_jacobians_task_input input;
    {
        phys_build_jacobian_task *task = src_job.m_build_jacobian_task;

        input.m_atom_infos = task->m_atom_infos;
        input.m_atom_info_count = task->m_atom_info_count;
        input.m_vel_accums = task->m_vel_accums;
        input.m_schema_list = reinterpret_cast<char *>(task->m_schema_list);
        input.m_next_task_schemas = reinterpret_cast<char *>(task->m_next_task_schemas);
        input.m_next_task = task->m_next;
        input.m_finish_schemas_goto = src_job.m_finish_schemas_goto && task->m_next;
    }

    phys_cpu_build_jacobians_from_task(input, solver_query_in);

    return shared_job_queue.finish_job_get_next(phys_job_queue::WORKER_THREAD_CPU, &solver_job_in_out, solver_job_in_out, 0, queue_wait_status);
}
