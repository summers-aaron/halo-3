// hkdynamics/hkDynamicsJobs.obj: from the records 0x664138
// @compiler xdk6130
// @flags /TP /O2 /Oc

#include "phys/phys_island.h"
#include "phys/phys_job_queue.h"

#include "phys/phys_dynamics.h"









#include "phys/phys_dynamics_jobs.h"
#include "phys/phys_constraint_solver_setup.h"




// @address 0x664138 ?pop_job_task@phys_broadphase_job@@QAA?AW4job_pop_result@phys_job_queue@@AAV?$phys_array@PAVphys_island@@@@AAV1@@Z
phys_job_queue::job_pop_result phys_broadphase_job::pop_job_task(phys_array<phys_island *> &islands, phys_broadphase_job &out)
{
    out.m_sim_island = islands[this->m_island_idx];
    phys_build_jacobian_task_header *task_header = this->m_task_header;
    if (task_header != 0 && task_header->m_new_split_islands.getSize() != 0)
    {
        int new_island_count = task_header->m_new_split_islands.getSize();
        task_header->m_object_ref_count += new_island_count;
        this->m_island_idx = (unsigned short)islands.getSize();
        this->m_island_count = (unsigned short)new_island_count;
        phys_array<phys_island *> &header_split_islands = task_header->m_new_split_islands;
        phys_array<phys_island *> &world_active_islands = out.m_sim_island->m_world->m_active_islands;
        for (int i = 0; i < new_island_count; i++)
        {
            phys_island *island = header_split_islands[i];
            island->m_storage_idx = (unsigned short)world_active_islands.getSize();
            world_active_islands.array_push_back(island);
        }
        header_split_islands.clear_and_deallocate();
        phys_free_chunk<phys_entity *>(const_cast<phys_entity **>(out.m_task_header->m_all_entities), task_header->m_entity_capacity, PHYS_MEM_BASE);
        task_header->m_all_entities = 0;
        return phys_job_queue::JOB_QUEUE_KEEP_ENTRY;
    }
    if (this->m_island_count > 1)
    {
        this->m_island_idx += 1;
        this->m_island_count -= 1;
        out.m_island_count = 1;
        return phys_job_queue::JOB_QUEUE_KEEP_ENTRY;
    }
    return phys_job_queue::JOB_QUEUE_POP_ENTRY;
}
