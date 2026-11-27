#ifndef INCLUDE_PHYS_DYNAMICS_JOBS_H
#define INCLUDE_PHYS_DYNAMICS_JOBS_H

#include "phys/phys_job_queue.h"
#include "phys/phys_constraint_solver_setup.h"
#include "phys/phys_step_info.h"

struct phys_mt_thread_state;
struct phys_agent_sector_header;
struct phys_pair_agent_sector;

enum phys_dynamics_job_type {
    SIM_JOB_INTEGRATE = 0,
    SIM_JOB_BUILD_ACCUMS,
    SIM_JOB_SPAWN_JACOBIAN_TASKS,
    SIM_JOB_JACOBIAN_SETUP_CB,
    SIM_JOB_BUILD_JACOBIANS,
    SIM_JOB_SPLIT_ISLAND,
    SIM_JOB_SOLVE_CONSTRAINTS,
    SIM_JOB_SOLVE_APPLY_GRAVITY,
    SIM_JOB_SOLVE_CONSTRAINT_BATCH,
    SIM_JOB_SOLVE_INTEGRATE_VELS,
    SIM_JOB_SOLVE_EXPORT_RESULTS,
    SIM_JOB_INTEGRATE_MOTION,
    SIM_JOB_BROADPHASE,
    SIM_JOB_AGENT_SECTOR,
    SIM_JOB_POST_COLLIDE
};

class phys_dynamics_job {
public:
    enum no_job { JOB_NO_SOURCE };
    phys_dynamics_job(phys_dynamics_job_type type, const phys_dynamics_job &src_job)
        : m_job_type(type), m_island_idx(src_job.m_island_idx), m_sim_island(src_job.m_sim_island), m_task_header(src_job.m_task_header) {}
    phys_dynamics_job(phys_dynamics_job_type type, no_job)
        : m_job_type(type), m_island_idx(0xffff), m_sim_island(0), m_task_header(0) {}
    static __forceinline void *operator new(size_t, void *storage) { return storage; }
    __forceinline const phys_job_queue::job_queue_entry &as_job_queue_entry() const {
        return reinterpret_cast<const phys_job_queue::job_queue_entry &>(*this);
    }
    phys_job_queue::job_pop_result pop_dynamics_job_task(const phys_array<phys_island *> &islands, phys_dynamics_job &out);
    __declspec(align(16)) phys_enum<phys_dynamics_job_type, unsigned short> m_job_type;
    unsigned short m_island_idx;
    phys_island *m_sim_island;
    phys_build_jacobian_task_header *m_task_header;
};

class phys_integrate_job : public phys_dynamics_job {
public:
    phys_integrate_job(int island_count) : phys_dynamics_job(SIM_JOB_INTEGRATE, JOB_NO_SOURCE) {
        m_island_idx = 0;
        m_island_count = island_count;
    }
    __forceinline phys_job_queue::job_pop_result pop_job_task(const phys_array<phys_island *> &islands, phys_integrate_job &out) {
        out.m_sim_island = islands[m_island_idx];
        if (m_island_count > 1) {
            m_island_idx++;
            m_island_count--;
            out.m_island_count = 1;
            return phys_job_queue::JOB_QUEUE_KEEP_ENTRY;
        }
        return phys_job_queue::JOB_QUEUE_POP_ENTRY;
    }
    int m_island_count;
};

class phys_build_accums_job : public phys_dynamics_job {
public:
    enum { JOB_ACCUM_CAPACITY = 64 };
    phys_entity *const *m_island_entity_array;
    unsigned short m_first_entity_idx;
    unsigned short m_entity_count;
    __forceinline phys_job_queue::job_pop_result pop_job_task(const phys_array<phys_island *> &islands, phys_build_accums_job &out) {
        out.m_sim_island = islands[m_island_idx];
        if (m_entity_count > JOB_ACCUM_CAPACITY) {
            m_entity_count -= JOB_ACCUM_CAPACITY;
            m_first_entity_idx += JOB_ACCUM_CAPACITY;
            out.m_entity_count = JOB_ACCUM_CAPACITY;
            return phys_job_queue::JOB_QUEUE_KEEP_ENTRY;
        }
        return phys_job_queue::JOB_QUEUE_POP_ENTRY;
    }
protected:
    phys_build_accums_job(const phys_dynamics_job &src_job, phys_dynamics_job_type type = SIM_JOB_BUILD_ACCUMS)
        : phys_dynamics_job(type, src_job) {}
};

class phys_create_jacobian_tasks_job : public phys_build_accums_job {
public:
    phys_create_jacobian_tasks_job(const phys_dynamics_job &src_job, phys_build_jacobian_task_header *header)
        : phys_build_accums_job(src_job, SIM_JOB_SPAWN_JACOBIAN_TASKS) { m_task_header = header; }
};

class phys_fire_jacobian_setup_callback : public phys_dynamics_job {
public:
    phys_fire_jacobian_setup_callback(const phys_build_accums_job &src_job)
        : phys_dynamics_job(SIM_JOB_JACOBIAN_SETUP_CB, src_job) {}
};

class phys_build_jacobians_job : public phys_dynamics_job {
public:
    phys_build_jacobians_job(const phys_dynamics_job &src_job, phys_build_jacobian_task *task, const phys_build_jacobian_task_header &header)
        : phys_dynamics_job(SIM_JOB_BUILD_JACOBIANS, src_job) {
        m_constraint_query_in = header.m_constraint_query_in;
        m_finish_schemas_goto = header.m_solve_single_thread;
        m_build_jacobian_task = task;
    }
    phys_build_jacobian_task *m_build_jacobian_task;
    const phys_constraint_query_in *m_constraint_query_in;
    phys_bool m_finish_schemas_goto;
    __forceinline phys_job_queue::job_pop_result pop_job_task(const phys_array<phys_island *> &islands, phys_build_jacobians_job &out) {
        out.m_sim_island = islands[m_island_idx];
        if (m_build_jacobian_task->m_next) {
            m_build_jacobian_task = m_build_jacobian_task->m_next;
            return phys_job_queue::JOB_QUEUE_KEEP_ENTRY;
        }
        return phys_job_queue::JOB_QUEUE_POP_ENTRY;
    }
};

class phys_split_island_job : public phys_dynamics_job {
public:
    phys_split_island_job(const phys_dynamics_job &src_job, phys_dynamics_job_type type = SIM_JOB_SPLIT_ISLAND)
        : phys_dynamics_job(type, src_job) {}
    __forceinline phys_job_queue::job_pop_result pop_job_task(const phys_array<phys_island *> &islands, phys_split_island_job &out) {
        out.m_sim_island = islands[m_island_idx];
        return phys_job_queue::JOB_QUEUE_POP_ENTRY;
    }
};

class phys_broadphase_job : public phys_dynamics_job {
public:
    phys_broadphase_job(const phys_dynamics_job &src_job, phys_build_jacobian_task_header *header)
        : phys_dynamics_job(SIM_JOB_BROADPHASE, src_job) { m_task_header = header; m_island_count = 1; }
    phys_job_queue::job_pop_result pop_job_task(phys_array<phys_island *> &islands, phys_broadphase_job &out);
    unsigned short m_island_count;
};

class phys_solve_apply_gravity_job : public phys_dynamics_job {
public:
    enum { GRAVITY_JOB_ACCUM_MAX = 64 };
    phys_solve_apply_gravity_job(const phys_dynamics_job &src_job, const phys_build_jacobian_task_header &header)
        : phys_dynamics_job(SIM_JOB_SOLVE_APPLY_GRAVITY, src_job) {
        m_vel_accums = header.m_task_accums_base;
        m_accums_end = header.m_accums_end;
    }
    __forceinline phys_job_queue::job_pop_result pop_job_task(const phys_array<phys_island *> &islands, phys_solve_apply_gravity_job &out) {
        out.m_sim_island = islands[m_island_idx];
        if (m_vel_accums + GRAVITY_JOB_ACCUM_MAX < m_accums_end) {
            m_vel_accums = m_vel_accums + GRAVITY_JOB_ACCUM_MAX;
            out.m_accums_end = m_vel_accums;
            return phys_job_queue::JOB_QUEUE_KEEP_ENTRY;
        }
        return phys_job_queue::JOB_QUEUE_POP_ENTRY;
    }
    phys_vel_accum *m_vel_accums;
    phys_vel_accum *m_accums_end;
};

class phys_solve_integrate_vel_job;
class phys_solve_constraint_batch_job : public phys_dynamics_job {
public:
    phys_solve_constraint_batch_job(const phys_solve_apply_gravity_job &src_job, const phys_build_jacobian_task_header &header)
        : phys_dynamics_job(SIM_JOB_SOLVE_CONSTRAINT_BATCH, src_job) {
        m_solve_batch_task = header.m_solve_tasks.m_first_solve_task;
        m_solver_step = 0;
        m_solver_micro_step = 0;
    }
    phys_solve_constraint_batch_job(const phys_solve_constraint_batch_job &src_job)
        : phys_dynamics_job(SIM_JOB_SOLVE_CONSTRAINT_BATCH, src_job) {
        *this = src_job;
        m_solve_batch_task = src_job.m_solve_batch_task->m_next_batch_first_task;
    }
    phys_solve_constraint_batch_job(const phys_solve_integrate_vel_job &src_job, const phys_build_jacobian_task_header &header);
    __forceinline phys_job_queue::job_pop_result pop_job_task(const phys_array<phys_island *> &islands, phys_solve_constraint_batch_job &out) {
        out.m_sim_island = islands[m_island_idx];
        if (!m_solve_batch_task->m_last_task_in_batch) {
            m_solve_batch_task = m_solve_batch_task->m_next;
            return phys_job_queue::JOB_QUEUE_KEEP_ENTRY;
        }
        return phys_job_queue::JOB_QUEUE_POP_ENTRY;
    }
    unsigned int m_solver_step;
    unsigned int m_solver_micro_steps;
    unsigned int m_solver_micro_step;
    phys_solve_constraint_batch_task *m_solve_batch_task;
};

class phys_solve_integrate_vel_job : public phys_dynamics_job {
public:
    enum { INTEGRATE_JOB_ACCUM_MAX = 64 };
    phys_solve_integrate_vel_job(const phys_solve_constraint_batch_job &src_job, const phys_build_jacobian_task_header &header)
        : phys_dynamics_job(SIM_JOB_SOLVE_INTEGRATE_VELS, src_job) {
        m_vel_accums = header.m_task_accums_base;
        m_accums_end = header.m_accums_end;
        m_solver_step = src_job.m_solver_step;
        m_solving_finished = false;
    }
    __forceinline phys_job_queue::job_pop_result pop_job_task(const phys_array<phys_island *> &islands, phys_solve_integrate_vel_job &out) {
        out.m_sim_island = islands[m_island_idx];
        if (m_vel_accums + INTEGRATE_JOB_ACCUM_MAX < m_accums_end) {
            m_vel_accums = m_vel_accums + INTEGRATE_JOB_ACCUM_MAX;
            out.m_accums_end = m_vel_accums;
            return phys_job_queue::JOB_QUEUE_KEEP_ENTRY;
        }
        return phys_job_queue::JOB_QUEUE_POP_ENTRY;
    }
    phys_vel_accum *m_vel_accums;
    phys_vel_accum *m_accums_end;
    unsigned int m_solver_step;
    phys_bool m_solving_finished;
};

class phys_solve_export_results_job : public phys_split_island_job {
public:
    phys_solve_export_results_job(const phys_dynamics_job &src_job, const phys_build_jacobian_task_header &header,
        phys_dynamics_job_type type = SIM_JOB_SOLVE_EXPORT_RESULTS)
        : phys_split_island_job(src_job, type), m_solve_batch_task(header.m_solve_tasks.m_first_solve_task) {}
    phys_solve_constraint_batch_task *m_solve_batch_task;
    __forceinline phys_job_queue::job_pop_result pop_job_task(const phys_array<phys_island *> &islands, phys_solve_export_results_job &out) {
        out.m_sim_island = islands[m_island_idx];
        if (m_solve_batch_task->m_next) {
            m_solve_batch_task = m_solve_batch_task->m_next;
            return phys_job_queue::JOB_QUEUE_KEEP_ENTRY;
        }
        return phys_job_queue::JOB_QUEUE_POP_ENTRY;
    }
};

class phys_integrate_motion_job : public phys_solve_export_results_job {
public:
    enum { JOB_ACCUM_CAPACITY = 64 };
    phys_integrate_motion_job(const phys_dynamics_job &src_job, const phys_build_jacobian_task_header &header)
        : phys_solve_export_results_job(src_job, header, SIM_JOB_INTEGRATE_MOTION) {
        m_first_entity_idx = 0;
        m_entity_count = header.m_all_entity_count;
        m_buffer = header.m_buffer;
    }
    unsigned short m_entity_count;
    void *m_buffer;
    unsigned short m_first_entity_idx;
    int m_inactive_frame_count;
    __forceinline phys_job_queue::job_pop_result pop_job_task(const phys_array<phys_island *> &islands, phys_integrate_motion_job &out) {
        out.m_sim_island = islands[m_island_idx];
        if (m_entity_count > JOB_ACCUM_CAPACITY) {
            m_entity_count -= JOB_ACCUM_CAPACITY;
            m_first_entity_idx += JOB_ACCUM_CAPACITY;
            out.m_entity_count = JOB_ACCUM_CAPACITY;
            return phys_job_queue::JOB_QUEUE_KEEP_ENTRY;
        }
        if (m_solve_batch_task) {
            m_job_type = SIM_JOB_SOLVE_EXPORT_RESULTS;
            return phys_job_queue::JOB_QUEUE_KEEP_ENTRY;
        }
        return phys_job_queue::JOB_QUEUE_POP_ENTRY;
    }
};

class phys_solve_constraints_job : public phys_dynamics_job {
public:
    phys_solve_constraints_job(const phys_dynamics_job &src_job, const phys_build_jacobian_task_header &header)
        : phys_dynamics_job(SIM_JOB_SOLVE_CONSTRAINTS, src_job) {
        m_buffer = header.m_buffer;
        m_accums_offset = phys_get_byte_offset_int(m_buffer, header.m_task_accums_base);
        m_schema_offset = phys_get_byte_offset_int(m_buffer, header.m_schema_base);
        m_solver_temp_offset = phys_get_byte_offset_int(m_buffer, header.m_solver_temp_base);
        m_task_buf_size = header.m_task_buf_size;
        m_solver_result_count = header.m_solver_result_count;
        m_solver_temp_count = header.m_solver_temp_count;
    }
    void *m_buffer;
    int m_task_buf_size;
    unsigned long m_accums_offset;
    unsigned long m_schema_offset;
    unsigned long m_solver_temp_offset;
    int m_solver_result_count;
    int m_solver_temp_count;
};

class phys_agent_sector_job;
class phys_post_collide_job : public phys_dynamics_job {
public:
    phys_post_collide_job(const phys_agent_sector_job &src_job);
    phys_agent_sector_header *m_header;
};

class phys_agent_sector_job : public phys_dynamics_job {
public:
    enum { TASK_SECTOR_COUNT = 16 };
    phys_agent_sector_job(const phys_broadphase_job &src_job, const phys_step_info &step_info,
        phys_pair_agent_sector **sectors, int sector_count, int bytes_in_last_sector)
        : phys_dynamics_job(SIM_JOB_AGENT_SECTOR, src_job) {
        m_island_idx = src_job.m_island_idx;
        m_task_idx = 0;
        m_sector_idx = 0;
        m_track_sectors = sectors;
        m_sector_count = sector_count;
        m_step_info = step_info;
        m_header = 0;
        m_last_sector_bytes = bytes_in_last_sector;
    }
    int m_task_idx;
    int m_sector_idx;
    int m_sector_count;
    phys_pair_agent_sector **m_track_sectors;
    int m_last_sector_bytes;
    int m_task_count;
    phys_agent_sector_header *m_header;
    phys_step_info m_step_info;
    __forceinline phys_job_queue::job_pop_result pop_job_task(const phys_array<phys_island *> &islands, phys_agent_sector_job &out) {
        out.m_sim_island = islands[m_island_idx];
        if (m_sector_count > TASK_SECTOR_COUNT) {
            m_sector_count = m_sector_count - TASK_SECTOR_COUNT;
            m_task_idx += 1;
            m_sector_idx += TASK_SECTOR_COUNT;
            out.m_sector_count = TASK_SECTOR_COUNT;
            out.m_last_sector_bytes = 512;
            return phys_job_queue::JOB_QUEUE_KEEP_ENTRY;
        }
        return phys_job_queue::JOB_QUEUE_POP_ENTRY;
    }
};

__forceinline phys_job_queue::job_pop_result phys_dynamics_job::pop_dynamics_job_task(
    const phys_array<phys_island *> &islands, phys_dynamics_job &out) {
    reinterpret_cast<phys_job_queue::job_queue_entry &>(out) = reinterpret_cast<phys_job_queue::job_queue_entry &>(*this);
    out.m_sim_island = islands[m_island_idx];
    return phys_job_queue::JOB_QUEUE_POP_ENTRY;
}

inline phys_solve_constraint_batch_job::phys_solve_constraint_batch_job(const phys_solve_integrate_vel_job &src_job,
    const phys_build_jacobian_task_header &header) : phys_dynamics_job(SIM_JOB_SOLVE_CONSTRAINT_BATCH, src_job) {
    m_solve_batch_task = header.m_solve_tasks.m_first_solve_task;
    m_solver_step = src_job.m_solver_step + 1;
    m_solver_micro_step = 0;
}

inline phys_post_collide_job::phys_post_collide_job(const phys_agent_sector_job &src_job)
    : phys_dynamics_job(SIM_JOB_POST_COLLIDE, src_job) {
    m_island_idx = src_job.m_island_idx;
    m_header = src_job.m_header;
}

typedef char phys_dynamics_job_size[(sizeof(void *) != 4 || ((sizeof(phys_dynamics_job) == 16) )) ? 1 : -1];
typedef char phys_build_accum_entities[(sizeof(void *) != 4 || ((offsetof(phys_build_accums_job, m_island_entity_array) == 16) )) ? 1 : -1];
typedef char phys_build_jacobians_task[(sizeof(void *) != 4 || ((offsetof(phys_build_jacobians_job, m_build_jacobian_task) == 16) )) ? 1 : -1];
typedef char phys_integrate_motion_count[(sizeof(void *) != 4 || ((offsetof(phys_integrate_motion_job, m_entity_count) == 32) )) ? 1 : -1];
typedef char phys_integrate_motion_index[(sizeof(void *) != 4 || ((offsetof(phys_integrate_motion_job, m_first_entity_idx) == 40) )) ? 1 : -1];
typedef char phys_integrate_motion_result[(sizeof(void *) != 4 || ((offsetof(phys_integrate_motion_job, m_inactive_frame_count) == 44) )) ? 1 : -1];
typedef char phys_agent_sector_job_header[(sizeof(void *) != 4 || ((offsetof(phys_agent_sector_job, m_header) == 40) )) ? 1 : -1];
typedef char phys_agent_sector_job_step_info[(sizeof(void *) != 4 || ((offsetof(phys_agent_sector_job, m_step_info) == 48) )) ? 1 : -1];
typedef char phys_agent_sector_job_size[(sizeof(void *) != 4 || ((sizeof(phys_agent_sector_job) == 64) )) ? 1 : -1];

#endif
