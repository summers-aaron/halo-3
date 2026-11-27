#ifndef INCLUDE_PHYS_CONSTRAINT_SOLVER_SETUP_H
#define INCLUDE_PHYS_CONSTRAINT_SOLVER_SETUP_H

#include "phys/phys_base.h"
#include "phys/phys_vel_accum.h"
#include "phys/phys_solver_elem_temp.h"
#include "phys/phys_schema.h"
#include "phys/phys_thread_memory.h"

class phys_island;
class phys_entity;
class phys_constraint_query_in;
class phys_constraint_query_out;
class phys_constraint;
struct phys_constraint_internal;
struct phys_atom;
struct phys_build_jacobian_task;
struct phys_solve_constraint_batch_task;

struct phys_build_jacobian_task_set {
    phys_build_jacobian_task_set()
        : m_build_jacobian_tasks(0), m_jacobian_task_count(0), m_callback_constraints(0), m_callback_constraint_count(0) {}
    phys_build_jacobian_task *m_build_jacobian_tasks;
    int m_jacobian_task_count;
    const phys_constraint_internal **m_callback_constraints;
    int m_callback_constraint_count;
};

struct phys_solve_jacobians_tasks {
    phys_solve_jacobians_tasks() : m_first_solve_task(0), m_first_batch_size(0) {}
    phys_solve_constraint_batch_task *m_first_solve_task;
    unsigned short m_first_batch_size;
};

struct phys_build_jacobian_task_header {
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_CONSTRAINT, phys_build_jacobian_task_header);
    phys_build_jacobian_task_header() : m_object_ref_count(1), m_buffer(0), m_task_buf_size(0) {}
    __declspec(align(16)) mutable int m_open_job_count;
    int m_broadphase_jobs_pending;
    int m_island_should_sleep;
    int m_object_ref_count;
    char *m_buffer;
    signed char m_export_done;
    signed char m_split_check_requested;
    int m_task_buf_size;
    int m_buffer_capacity;
    phys_vel_accum *m_task_accums_base;
    phys_vel_accum *m_accums_end;
    int m_gravity_job_count;
    int m_integrate_vel_job_count;
    phys_schema *m_schema_base;
    phys_solver_elem_temp *m_solver_temp_base;
    int m_solver_result_count;
    int m_solver_temp_count;
    const phys_constraint_query_in *m_constraint_query_in;
    phys_entity *const *m_all_entities;
    unsigned short m_all_entity_count;
    unsigned short m_entity_capacity;
    phys_bool m_solve_single_thread;
    phys_bool m_solve_single_thread_ppu;
    phys_array<phys_island *> m_new_split_islands;
    phys_build_jacobian_task_set m_build_tasks;
    phys_solve_jacobians_tasks m_solve_tasks;
};

struct phys_build_jacobian_task {
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_CONSTRAINT, phys_build_jacobian_task);
    enum { ATOM_INFO_MAX = 144 };
    struct atom_info {
        const phys_atom *m_data_atoms;
        phys_constraint *m_instance;
        void *m_solver_runtime;
        const phys_transform *m_transform_a;
        const phys_transform *m_transform_b;
        unsigned short m_atoms_size;
        unsigned short m_runtime_size;
        unsigned short m_accum_idx_a;
        unsigned short m_accum_idx_b;
    };
    phys_build_jacobian_task() { m_atom_info_count = 0; m_next_task_schemas = 0; m_next = 0; }
    __declspec(align(128)) phys_build_jacobian_task *m_next;
    phys_build_jacobian_task_header *m_task_header;
    const phys_vel_accum *m_vel_accums;
    phys_schema *m_schema_list;
    phys_schema *m_next_task_schemas;
    int m_atom_info_count;
    atom_info m_atom_infos[ATOM_INFO_MAX];
};

struct phys_solve_constraint_batch_task {
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_CONSTRAINT, phys_solve_constraint_batch_task);
    phys_solve_constraint_batch_task() {
        m_next = 0;
        m_last_task_in_batch = false;
        m_next_batch_size = 0;
        m_next_batch_first_task = 0;
    }
    __declspec(align(16)) phys_solve_constraint_batch_task *m_next;
    phys_build_jacobian_task_header *m_task_header;
    phys_vel_accum *m_vel_accums;
    const phys_schema *m_schema_list;
    phys_solver_elem_temp *m_solver_elem_temp;
    int m_schema_buffer_size;
    int m_solver_temp_buffer_size;
    phys_bool m_last_task_in_batch;
    unsigned short m_next_batch_size;
    phys_solve_constraint_batch_task *m_next_batch_first_task;
};

typedef char phys_build_task_header_size[(sizeof(void *) != 4 || ((sizeof(phys_build_jacobian_task_header) == 128) )) ? 1 : -1];
typedef char phys_build_task_header_accums[(sizeof(void *) != 4 || ((offsetof(phys_build_jacobian_task_header, m_task_accums_base) == 32) )) ? 1 : -1];
typedef char phys_build_task_header_entities[(sizeof(void *) != 4 || ((offsetof(phys_build_jacobian_task_header, m_all_entities) == 68) )) ? 1 : -1];
typedef char phys_build_task_header_split_islands[(sizeof(void *) != 4 || ((offsetof(phys_build_jacobian_task_header, m_new_split_islands) == 80) )) ? 1 : -1];
typedef char phys_build_task_header_tasks[(sizeof(void *) != 4 || ((offsetof(phys_build_jacobian_task_header, m_build_tasks) == 92) )) ? 1 : -1];
typedef char phys_build_task_header_solve_tasks[(sizeof(void *) != 4 || ((offsetof(phys_build_jacobian_task_header, m_solve_tasks) == 108) )) ? 1 : -1];
typedef char phys_build_task_atom_size[(sizeof(void *) != 4 || ((sizeof(phys_build_jacobian_task::atom_info) == 28) )) ? 1 : -1];
typedef char phys_build_task_atoms[(sizeof(void *) != 4 || ((offsetof(phys_build_jacobian_task, m_atom_infos) == 24) )) ? 1 : -1];
typedef char phys_build_task_size[(sizeof(void *) != 4 || ((sizeof(phys_build_jacobian_task) == 4096) )) ? 1 : -1];
typedef char phys_solve_task_size[(sizeof(void *) != 4 || ((sizeof(phys_solve_constraint_batch_task) == 48) )) ? 1 : -1];

struct phys_solver_info;
class phys_step_info;
struct phys_constraint_schema_info {
    phys_constraint *m_contact_constraint;
    phys_schema *m_cur_schema;
    float m_allowed_penetration_depth;
};

struct phys_solver_resources {
    struct schema_state {
        char *m_begin;
        char *m_end;
        char *m_current;
        char *m_last_processed;
    };
    const phys_step_info *m_step_info;
    const phys_solver_info *m_solver_info;
    phys_constraint_query_in *m_query_in;
    phys_vel_accum *m_vel_accums;
    phys_vel_accum *m_accums_end;
    phys_vel_accum *m_accums_current;
    phys_vec4 *m_accums_backup;
    phys_vec4 *m_accums_backup_end;
    schema_state m_schema_list[2];
    char *m_setup_ptr_64;
    char *solver_results_limit;
    char *solver_results_cur;
    char *m_setup_ptr_76;
};

class phys_solver_setup {
public:
    enum solver_mode { SOLVER_PASS_MODE_A = 0, SOLVER_PASS_MODE_B = 1 };
    static void init_solver_state(phys_step_info &step_info, phys_solver_info &solver_info, phys_constraint_query_in &solver_query_in,
        char *solver_buffer_start, int solver_buffer_size, phys_solver_resources &resources);
    static void sub_solve(phys_solver_resources &resources, solver_mode mode);
    static void shutdown_solver(phys_solver_resources &resources);
    static phys_bool has_room_for_accumulators(phys_solver_resources &resources, phys_entity **entities, int solver_entity_count);
    static void add_accumulators_internal(phys_solver_resources &resources, phys_entity *const *entities, int solver_entity_count);
    static phys_bool has_room_for_schemas(phys_solver_resources &resources, phys_constraint **constraints, int solver_constraint_count);
    static void integrate_one_step(const phys_solver_info &solver_info, const phys_step_info &step_info,
        const phys_vel_accum *accum_array, phys_entity *const *entities, int solver_entity_count);
    // match: 0x65e720: retain the accepted noalias contract when moving the solver-setup declaration.
    static __declspec(noalias) int compute_solve_buffer_offsets(class phys_island &p0, char *p1, int p2, struct phys_build_jacobian_task_header &p3);
    static int compute_solver_buffer_size(class phys_island &p0);
    static void add_schemas_internal(phys_solver_resources &s, phys_constraint **constraints, int solver_constraint_count,
        phys_array<struct phys_constraint_schema_info> &constraint_status);
    static int solve_constraints(const phys_step_info &step_info, const phys_solver_info &solver_info,
        phys_constraint_query_in &constraint_query_in, class phys_island &island, phys_entity *const *bodies, int solver_body_count);
private:
    static __forceinline void build_vel_accums(const phys_step_info &info, phys_entity *const *bodies_in, int solver_entity_count, phys_vel_accum *accum_out);
    static __forceinline void build_jacobian_element(
        const phys_constraint_internal *c, phys_constraint_query_in &in, phys_vel_accum *base_accum, phys_constraint_query_out &out);
    static __forceinline void build_jacobian_elements(phys_constraint_query_in &in, phys_entity *const *bodies, int solver_body_count,
        phys_vel_accum *base_accum, phys_schema *schema_out, phys_schema *schema_out_end, phys_schema *schema_out_b);
};

#endif
