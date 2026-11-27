// hkdynamics/hkConstraintSolverSetup.obj: from the records 0x65e4d0 0x65e4e0 0x65e610 0x65e618 0x65e658 0x65e720 0x65e848 0x65e9e0 0x65eaa8 0x65ebd0 0x65ee60 0x65ef10 0x65f7e8
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x65e4d0 ?phys_solver_setup_integrate2@@YAHABUphys_solver_info@@ABVphys_step_info@@PBVphys_vel_accum@@PBQAVphys_entity@@H@Z
// @address 0x65e610 ?shutdown_solver@phys_solver_setup@@SAXAAUphys_solver_resources@@@Z
// @address 0x65e618 ?has_room_for_accumulators@phys_solver_setup@@SA?AVphys_bool@@AAUphys_solver_resources@@PAPAVphys_entity@@H@Z
// @address 0x65e658 ?sub_solve@phys_solver_setup@@SAXAAUphys_solver_resources@@W4solver_mode@1@@Z
// @address 0x65eaa8 ?integrate_one_step@phys_solver_setup@@SAXABUphys_solver_info@@ABVphys_step_info@@PBVphys_vel_accum@@PBQAVphys_entity@@H@Z
// @address 0x65e9e0 ?has_room_for_schemas@phys_solver_setup@@SA?AVphys_bool@@AAUphys_solver_resources@@PAPAVphys_constraint@@H@Z
// @address 0x65e848 ?add_accumulators_internal@phys_solver_setup@@SAXAAUphys_solver_resources@@PBQAVphys_entity@@H@Z
// @address 0x65e4e0 ?init_solver_state@phys_solver_setup@@SAXAAVphys_step_info@@AAUphys_solver_info@@AAVphys_constraint_query_in@@PADHAAUphys_solver_resources@@@Z
#include <vectorintrinsics.h>
#include <ppcintrinsics.h>
#include "phys/phys_simple_contact_data.h"
#include "phys/phys_build_jacobian_from_atoms.h"
#include "phys/phys_solver_export.h"
#include "phys/phys_math.h"
#include "phys/phys_thread_memory.h"
#include "phys/phys_island.h"
#include "phys/phys_solver.h"
#include "phys/phys_solver_functions.h"
#include "phys/phys_rigid_motion_util.h"

#include "phys/phys_base_types.h"
#include "phys/phys_base.h"
#include "phys/phys_schema.h"
#include "phys/phys_dynamics.h"
#include "phys/phys_collide.h"
#include "phys/phys_atoms.h"
#include "phys/phys_constraint.h"
#include "phys/phys_constraint_data.h"
#include "phys/phys_vel_accum.h"
struct phys_constraint_internal;

class phys_constraint;
#include "phys/phys_constraint_query_in.h"
#include "phys/phys_constraint_query_out.h"

#include "phys/phys_constraint_solver_setup.h"

#include "phys/phys_header_schema.h"
#include "phys/phys_step_info.h"

#include "phys/phys_solver_build_jacobian.h"
#include "phys/phys_solver_elem_temp.h"
#include "phys/phys_math_prefetch.h"
template <typename T> class phys_array;

int phys_solver_setup_integrate2(const phys_solver_info &solver_info, const phys_step_info &step_info,
    const phys_vel_accum *accum_array, phys_entity *const *entities, int solver_entity_count)
{
    const phys_vel_accum *accum_cursor = accum_array + 1;
    for (int i = 0; i < solver_entity_count; i++, accum_cursor++) {
    }
    int inactive_frame_count = phys_rigid_motion_apply_accums(solver_info, step_info, accum_array + 1,
        reinterpret_cast<phys_motion *const *>(entities), solver_entity_count, (int)offsetof(phys_entity, m_motion));
    return inactive_frame_count;
}

void phys_solver_setup::shutdown_solver(phys_solver_resources &resources)
{
}

void phys_solver_setup::init_solver_state(phys_step_info &step_info, phys_solver_info &solver_info,
    phys_constraint_query_in &solver_query_in, char *solver_buffer_start, int solver_buffer_size, phys_solver_resources &resources)
{
    resources.m_step_info = &step_info;
    resources.m_solver_info = &solver_info;
    resources.m_query_in = &solver_query_in;
    solver_buffer_size = solver_buffer_size - solver_buffer_size % 16;
    char *buf_end = solver_buffer_start + solver_buffer_size;
    const float fraction = 0.6f;
    char *p = reinterpret_cast<char *>((reinterpret_cast<size_t>(solver_buffer_start) + 15) & ~size_t(15));
    resources.m_vel_accums = reinterpret_cast<phys_vel_accum *>(p);
    p = reinterpret_cast<char *>((reinterpret_cast<size_t>(p) + (solver_buffer_size * 10) / 64) & ~size_t(15));
    resources.m_accums_end = reinterpret_cast<phys_vel_accum *>(p);
    resources.m_accums_backup = reinterpret_cast<phys_vec4 *>(p);
    p = reinterpret_cast<char *>((reinterpret_cast<size_t>(p) + (solver_buffer_size << 2) / 64) & ~size_t(15));
    resources.m_accums_backup_end = reinterpret_cast<phys_vec4 *>(p);
    resources.m_schema_list[0].m_begin = p;
    p = reinterpret_cast<char *>((reinterpret_cast<size_t>(p) +
        (unsigned)((float)(solver_buffer_size * 40) * fraction / 64.0f)) & ~size_t(15));
    resources.m_schema_list[0].m_end = p;
    resources.m_schema_list[1].m_begin = p;
    p = reinterpret_cast<char *>((reinterpret_cast<size_t>(p) +
        (unsigned)((float)(solver_buffer_size * 40) * (1.0f - fraction) / 64.0f)) & ~size_t(15));
    resources.m_schema_list[1].m_end = p;
    resources.m_setup_ptr_64 = p;
    resources.solver_results_limit = buf_end;
    resources.m_accums_current = resources.m_vel_accums;
    resources.solver_results_cur = resources.m_setup_ptr_64;
    resources.m_setup_ptr_76 = resources.m_setup_ptr_64;
    resources.m_schema_list[0].m_current = resources.m_schema_list[0].m_begin;
    resources.m_schema_list[0].m_last_processed = resources.m_schema_list[0].m_current;
    resources.m_schema_list[1].m_current = resources.m_schema_list[1].m_begin;
    resources.m_schema_list[1].m_last_processed = resources.m_schema_list[1].m_current;
}

void phys_solver_setup::sub_solve(phys_solver_resources &resources, solver_mode mode)
{
    if (mode == SOLVER_PASS_MODE_A) {
        resources.m_schema_list[0].m_last_processed = resources.m_schema_list[0].m_begin;
        resources.m_schema_list[1].m_last_processed = resources.m_schema_list[1].m_begin;
    }
    for (char *cursor = resources.m_setup_ptr_76; cursor < resources.solver_results_cur; cursor += 4) {
        *reinterpret_cast<float *>(cursor) = 0.0f;
    }
    resources.m_setup_ptr_76 = resources.solver_results_cur;
    char *rebase_ptr = resources.m_schema_list[0].m_last_processed;
    if (rebase_ptr != resources.m_schema_list[0].m_current) {
        phys_solve_step_jacobians(*resources.m_solver_info, reinterpret_cast<const phys_schema *>(rebase_ptr),
            resources.m_vel_accums, reinterpret_cast<phys_solver_elem_temp *>(resources.m_setup_ptr_64));
    }
    rebase_ptr = resources.m_schema_list[1].m_last_processed;
    if (rebase_ptr != resources.m_schema_list[1].m_current) {
        phys_solve_step_jacobians(*resources.m_solver_info, reinterpret_cast<const phys_schema *>(rebase_ptr),
            resources.m_vel_accums, reinterpret_cast<phys_solver_elem_temp *>(resources.m_setup_ptr_64));
    }
    resources.m_schema_list[0].m_last_processed = resources.m_schema_list[0].m_current;
    resources.m_schema_list[1].m_last_processed = resources.m_schema_list[1].m_current;
}




phys_bool phys_solver_setup::has_room_for_accumulators(phys_solver_resources &resources,
    phys_entity **entities, int solver_entity_count)
{
    int stream_bytes_left = phys_get_byte_offset(resources.m_accums_current, resources.m_accums_end);
    if (resources.m_accums_current == resources.m_vel_accums) {
        ++solver_entity_count;
    }
    stream_bytes_left -= 16;
    if (solver_entity_count * static_cast<int>(sizeof(phys_vel_accum)) > stream_bytes_left) {
        return phys_bool(false);
    } else {
        return phys_bool(true);
    }
}

void phys_solver_setup::add_accumulators_internal(phys_solver_resources &resources,
    phys_entity *const *entities, int solver_entity_count)
{
    if (solver_entity_count == 0) {
        return;
    }
    phys_vel_accum *accumulator = resources.m_accums_current;
    if (accumulator == resources.m_vel_accums) {
        accumulator->make_fixed_accumulator();
        resources.m_accums_current = accumulator + 1;
        phys_vec4 *accum_backup = resources.m_accums_backup;
        accum_backup[1].set_zero4();
        accum_backup[0].set_zero4();
    }
    for (unsigned i = 0; i < static_cast<unsigned>(solver_entity_count); ++i) {
        entities[i]->m_solver_data = phys_get_byte_offset(resources.m_vel_accums, resources.m_accums_current) + i * sizeof(phys_vel_accum);
    }
    phys_vel_accum *first = resources.m_accums_current;
    phys_vec4 *accum_backup = resources.m_accums_backup + (entities[0]->m_solver_data / sizeof(phys_vel_accum)) * 2;
    resources.m_accums_current = phys_rigid_motion_build_accums(*resources.m_step_info,
        reinterpret_cast<phys_motion *const *>(entities), solver_entity_count, (int)offsetof(phys_entity, m_motion), first);
    for (unsigned i = 0; i < static_cast<unsigned>(solver_entity_count); ++i) {
        accum_backup[0] = first->get_sum_lin_vel();
        accum_backup[1] = first->get_sum_ang_vel();
        first->get_sum_lin_vel().set_zero4();
        first->get_sum_ang_vel().set_zero4();
        first = first + 1;
        accum_backup = accum_backup + 2;
    }
    resources.m_accums_current->m_type = phys_vel_accum::PHYS_NO_GRAVITY_RIGID_BODY;
}

phys_bool phys_solver_setup::has_room_for_schemas(
    phys_solver_resources &resources, phys_constraint **constraints, int solver_constraint_count)
{
    char *schema_ptrs[2];
    schema_ptrs[0] = resources.m_schema_list[0].m_current;
    schema_ptrs[1] = resources.m_schema_list[1].m_current;
    char *solver_results_cur = resources.solver_results_cur;
    phys_constraint **end = constraints + solver_constraint_count;
    for (phys_constraint **it = constraints; it < end; it++) {
        phys_constraint *constraint = *it;
        int schema_idx = constraint->get_priority() >= 4 ? 1 : 0;
        phys_constraint_internal *internal = constraint->m_constraint_internal;
        solver_results_cur += internal->m_solver_temp_count << 2;
        schema_ptrs[schema_idx] += internal->m_schemas_size;
    }
    bool memory_ok = !(solver_results_cur > resources.solver_results_limit) && !(schema_ptrs[0] + 4 > resources.m_schema_list[0].m_end) &&
        (schema_ptrs[1] + 4 <= resources.m_schema_list[1].m_end);
    return phys_bool(memory_ok);
}

void phys_solver_setup::integrate_one_step(const phys_solver_info &solver_info, const phys_step_info &step_info,
    const phys_vel_accum *accum_array, phys_entity *const *entities, int solver_entity_count)
{
    for (int i = 0; i < solver_entity_count; ++i) {
        phys_entity *entity = entities[i];
        phys_motion *motion = entity->get_motion();
        phys_vel_accum accumulator;
        const phys_vel_accum *source = phys_add_byte_offset_const(accum_array, entity->m_solver_data);
        accumulator = *source;
        accumulator.get_sum_lin_vel() = accumulator.m_lin_vel;
        accumulator.get_sum_ang_vel() = accumulator.m_ang_vel;
        phys_bool deactivated(false);
        unsigned char sleep_counter = motion->m_integrate_sleep_counter;
        motion->m_integrate_sleep_counter = 255;
        phys_rigid_motion_apply_accums(solver_info, step_info, &accumulator, &motion, 1, 0);
        motion->m_integrate_sleep_counter = sleep_counter;
    }
}


// @address 0x65f7e8 ??4phys_vel_accum@@QAAAAV0@ABV0@@Z
phys_vel_accum &phys_vel_accum::operator=(const phys_vel_accum &other)
{
    m_type = other.m_type;
    m_mat_is_identity = other.m_mat_is_identity;
    m_deactivation_counter = other.m_deactivation_counter;
    m_deactivation_class = other.m_deactivation_class;
    m_lin_vel = other.m_lin_vel;
    m_ang_vel = other.m_ang_vel;
    m_inv_masses = other.m_inv_masses;
    m_scratch_0 = other.m_scratch_0;
    m_scratch_1 = other.m_scratch_1;
    m_scratch_2 = other.m_scratch_2;
    m_scratch_3 = other.m_scratch_3;
    return *this;
}

// @address 0x65ebd0 ?add_schemas_internal@phys_solver_setup@@SAXAAUphys_solver_resources@@PAPAVphys_constraint@@HAAV?$phys_array@Uphys_constraint_schema_info@@@@@Z
void phys_solver_setup::add_schemas_internal(phys_solver_resources &s, phys_constraint **constraints, int solver_constraint_count,
    phys_array<phys_constraint_schema_info> &constraint_status)
{
    phys_constraint_query_in &in = *s.m_query_in;
    phys_constraint_query_out out;

    phys_constraint **constraint_cursor = constraints;
    phys_constraint **constraints_end = constraints + solver_constraint_count;

    for (; constraint_cursor < constraints_end; constraint_cursor++) {
        phys_constraint *instance = constraint_cursor[0];
        int priority_list_id = instance->get_priority() == phys_constraint::CONSTRAINT_PRIORITY_TOI_FORCED;

        phys_entity **entity = &instance->get_internal()->m_island_entities[0];
        phys_motion *motion[2] = { entity[0]->get_motion(), entity[1]->get_motion() };

        phys_vel_accum *accum_a = phys_add_byte_offset(s.m_vel_accums, entity[0]->m_solver_data);
        phys_vel_accum *accum_b = phys_add_byte_offset(s.m_vel_accums, entity[1]->m_solver_data);

        in.m_body_a = accum_a;
        in.m_body_b = accum_b;

        {
            phys_vec4 *backup_a = &s.m_accums_backup[(entity[0]->m_solver_data / sizeof(phys_vel_accum)) * 2];
            phys_vec4 *backup_b = &s.m_accums_backup[(entity[1]->m_solver_data / sizeof(phys_vel_accum)) * 2];
            accum_a->get_sum_lin_vel() = backup_a[0];
            accum_a->get_sum_ang_vel() = backup_a[1];
            accum_b->get_sum_lin_vel() = backup_b[0];
            accum_b->get_sum_ang_vel() = backup_b[1];
        }

        const phys_motion_state *motion_a_state = motion[0]->get_motion_state();
        const phys_motion_state *motion_b_state = motion[1]->get_motion_state();

        in.m_transform_a = &motion_a_state->get_transform();
        in.m_transform_b = &motion_b_state->get_transform();

        in.m_query_constraint = instance;
        in.m_constraint_runtime = instance->get_runtime();

        in.m_accum_a_index = static_cast<unsigned short>(entity[0]->m_solver_data / sizeof(phys_vel_accum));
        in.m_accum_b_index = static_cast<unsigned short>(entity[1]->m_solver_data / sizeof(phys_vel_accum));
        in.m_runtime_in_main_memory = instance->get_runtime();

        {
            phys_constraint_schema_info &info = constraint_status.expand_one();
            info.m_contact_constraint = instance;
            info.m_cur_schema = reinterpret_cast<phys_schema *>(s.m_schema_list[priority_list_id].m_current);
            float allowed_pen_depth_a = entity[0]->get_collider()->m_allowed_penetration_depth;
            float allowed_pen_depth_b = entity[1]->get_collider()->m_allowed_penetration_depth;
            info.m_allowed_penetration_depth = phys_math::min_of_two(allowed_pen_depth_a, allowed_pen_depth_b);

            out.m_jacobian_schemas = reinterpret_cast<phys_schema *>(s.m_schema_list[priority_list_id].m_current);
        }

        if (instance->m_constraint_internal->m_callback_request & phys_atom::ATOM_CALLBACK_NEW_CONTACT) {
            phys_simple_contact_data *simple_contact_data = static_cast<phys_simple_contact_data *>(instance->get_chain_data());
            phys_simple_contact_atom *atom = simple_contact_data->m_contact_atom;
            phys_simple_contact_fire_callbacks(simple_contact_data, &in, atom);
        }

        if (instance->m_constraint_internal->m_callback_request & phys_atom::ATOM_CALLBACK_SETUP) {
            instance->get_chain_data()->build_jacobian_callback(in);
        }

        phys_solver_build_jacobian_atoms(instance->m_constraint_internal->get_atoms(), instance->m_constraint_internal->get_atoms_size(), in, out);

        s.m_schema_list[priority_list_id].m_current = reinterpret_cast<char *>(out.m_jacobian_schemas.val());
        s.solver_results_cur += instance->m_constraint_internal->m_solver_temp_count * 4;

        accum_a->get_sum_lin_vel().set_zero4();
        accum_a->get_sum_ang_vel().set_zero4();
        accum_b->get_sum_lin_vel().set_zero4();
        accum_b->get_sum_ang_vel().set_zero4();
    }

    *reinterpret_cast<int *>(s.m_schema_list[0].m_current) = 0;
    *reinterpret_cast<int *>(s.m_schema_list[1].m_current) = 0;
}
// @address 0x65e720 ?compute_solve_buffer_offsets@phys_solver_setup@@SAHAAVphys_island@@PADHAAUphys_build_jacobian_task_header@@@Z
int phys_solver_setup::compute_solve_buffer_offsets(class phys_island &p0, char *p1, int p2, struct phys_build_jacobian_task_header &p3)
{
    p3.m_buffer = p1;
    int solver_body_count = p0.m_island_entities.m_size;
    {
        p3.m_task_accums_base = reinterpret_cast<phys_vel_accum *>(p1);
        int accum_bytes = sizeof(phys_vel_accum) + 16 + sizeof(phys_vel_accum) * p0.m_island_entities.m_size;
        p1 += accum_bytes;
    }
    int extra_mt_tasks = 0;
    {
        int min_constraints_per_task = 144;
        extra_mt_tasks = (p0.m_constraint_count / min_constraints_per_task) + 1;
        extra_mt_tasks += 1;
        extra_mt_tasks += 1;
        extra_mt_tasks += 1;
        extra_mt_tasks *= 2;
    }
    {
        p3.m_schema_base = reinterpret_cast<phys_schema *>(p1);
        int schema_alloc_bytes = p0.m_constraint_info.m_schemas_size + 16;
        schema_alloc_bytes += 16 * extra_mt_tasks;
        p1 += schema_alloc_bytes;
    }
    {
        int elem_temp_size = p0.m_constraint_info.m_solver_temp_count * 4 + 2 * 4;
        elem_temp_size += extra_mt_tasks * 12;
        elem_temp_size = (elem_temp_size + 15) & ~15;
        p3.m_solver_temp_base = reinterpret_cast<phys_solver_elem_temp *>(p1);
        p1 += elem_temp_size;
    }
    int size = int(p1 - p3.m_buffer);
    p3.m_task_buf_size = size;
    if (p3.m_buffer) {
        phys_vel_accum *accum_base_ptr = p3.m_task_accums_base;
        accum_base_ptr[0].make_fixed_accumulator();
        accum_base_ptr[1 + solver_body_count].m_type = phys_vel_accum::PHYS_NO_GRAVITY_RIGID_BODY;
    }
    return size;
}
// @address 0x65ee60 ?compute_solver_buffer_size@phys_solver_setup@@SAHAAVphys_island@@@Z
int phys_solver_setup::compute_solver_buffer_size(class phys_island &p0)
{
    phys_build_jacobian_task_header task_header;
    return phys_solver_setup::compute_solve_buffer_offsets(p0, 0, 0, task_header);
}
static __forceinline void flush_solver_debug_stream() {}

__forceinline void phys_solver_setup::build_vel_accums(
    const phys_step_info &info, phys_entity *const *bodies_in, int solver_entity_count, phys_vel_accum *accum_out)
{
    accum_out[0].make_fixed_accumulator();
    for (int i = 0; i < solver_entity_count; i++) {
        unsigned int accum_offset = (1 + i) * sizeof(phys_vel_accum);
        if (accum_offset != bodies_in[i]->m_solver_data) {
            bodies_in[i]->m_solver_data = accum_offset;
        }
    }
    phys_vel_accum *end = phys_rigid_motion_build_accums(info,
        reinterpret_cast<phys_motion *const *>(bodies_in), solver_entity_count, (int)offsetof(phys_entity, m_motion), accum_out + 1);
    end->m_type = phys_vel_accum::PHYS_NO_GRAVITY_RIGID_BODY;
}

__forceinline void phys_solver_setup::build_jacobian_element(
    const phys_constraint_internal *c, phys_constraint_query_in &in, phys_vel_accum *base_accum, phys_constraint_query_out &out)
{
    phys_math::force_prefetch<256>(c->get_atoms());
    if (c->get_atoms_size() > 256) {
        phys_math::force_prefetch<256>(phys_add_byte_offset(c->get_atoms(), 256));
        if (c->get_atoms_size() > 512) {
            phys_math::force_prefetch<256>(phys_add_byte_offset(c->get_atoms(), 512));
        }
    }

    phys_entity *rA = c->m_island_entities[0];
    phys_entity *rB = c->m_island_entities[1];

    phys_motion *cA = rA->get_motion();
    phys_motion *cB = rB->get_motion();
    in.m_body_a = phys_add_byte_offset(base_accum, rA->m_solver_data);
    in.m_body_b = phys_add_byte_offset(base_accum, rB->m_solver_data);

    in.m_transform_a = &cA->get_transform();
    in.m_transform_b = &cB->get_transform();

    in.m_query_constraint = c->m_contact_constraint;
    in.m_constraint_runtime = c->m_solver_runtime;

    {
        in.m_accum_a_index = static_cast<unsigned short>(rA->m_solver_data / sizeof(phys_vel_accum));
        in.m_accum_b_index = static_cast<unsigned short>(rB->m_solver_data / sizeof(phys_vel_accum));
        in.m_runtime_in_main_memory = in.m_constraint_runtime;
    }

    phys_solver_build_jacobian_atoms(c->get_atoms(), c->get_atoms_size(), in, out);
}

__forceinline void phys_solver_setup::build_jacobian_elements(phys_constraint_query_in &in, phys_entity *const *bodies, int solver_body_count,
    phys_vel_accum *base_accum, phys_schema *schema_out, phys_schema *schema_out_end, phys_schema *schema_out_b)
{
    phys_constraint_query_out out;
    out.m_jacobian_schemas = schema_out;

    phys_entity *const *e = bodies;
    phys_entity *const *entity_end = bodies + solver_body_count;

    phys_inline_array<const phys_constraint_internal *, 256> critical_constraints;

    for (; e < entity_end; e++) {
        const phys_constraint_internal *c = (*e)->get_constraint_masters().begin();
        const phys_constraint_internal *constraints_end = (*e)->get_constraint_masters().end();

        for (; c < constraints_end; c++) {
            if (c->m_callback_request & (phys_atom::ATOM_CALLBACK_SETUP | phys_atom::ATOM_CALLBACK_NEW_CONTACT)) {
                in.m_query_constraint = c->m_contact_constraint;
                in.m_constraint_runtime = c->m_solver_runtime;
                phys_entity *setup_entity_a = c->m_contact_constraint->get_entity_a();
                phys_entity *setup_entity_b = c->m_contact_constraint->get_entity_b();

                in.m_body_a = phys_add_byte_offset(base_accum, setup_entity_a->m_solver_data);
                in.m_body_b = phys_add_byte_offset(base_accum, setup_entity_b->m_solver_data);
                in.m_transform_a = &setup_entity_a->get_collider()->get_transform();
                in.m_transform_b = &setup_entity_b->get_collider()->get_transform();

                if (c->m_callback_request & phys_atom::ATOM_CALLBACK_NEW_CONTACT) {
                    phys_simple_contact_data *simple_contact_data = static_cast<phys_simple_contact_data *>(c->m_contact_constraint->get_chain_data());
                    phys_simple_contact_atom *atom = simple_contact_data->m_contact_atom;
                    phys_simple_contact_fire_callbacks(simple_contact_data, &in, atom);
                }

                if (c->m_callback_request & phys_atom::ATOM_CALLBACK_SETUP) {
                    c->m_contact_constraint->get_chain_data()->build_jacobian_callback(in);
                }
            }

            if (c->m_priority >= phys_constraint::CONSTRAINT_PRIORITY_TOI_HIGHER) {
                critical_constraints.array_push_back(c);
                continue;
            }

            if (out.m_jacobian_schemas >= schema_out_end) {
                phys_goto_schema *sc = reinterpret_cast<phys_goto_schema *>(out.m_jacobian_schemas.val());
                sc->init_goto(schema_out_b);
                out.m_jacobian_schemas = schema_out_b;
                schema_out_end = reinterpret_cast<phys_schema *>(~(unsigned long)0);
            }
            build_jacobian_element(c, in, base_accum, out);
        }
    }

    {
        for (int i = 0; i < critical_constraints.getSize(); i++) {
            const phys_constraint_internal *c = critical_constraints[i];

            if (out.m_jacobian_schemas >= schema_out_end) {
                phys_goto_schema *sc = reinterpret_cast<phys_goto_schema *>(out.m_jacobian_schemas.val());
                sc->init_goto(schema_out_b);
                out.m_jacobian_schemas = schema_out_b;
                schema_out_end = reinterpret_cast<phys_schema *>(~(unsigned long)0);
            }
            build_jacobian_element(c, in, base_accum, out);
        }
    }

    unsigned int *constraint_list_end = reinterpret_cast<unsigned int *>(out.m_jacobian_schemas.val());
    constraint_list_end[0] = 0;
}

// @address 0x65ef10 ?solve_constraints@phys_solver_setup@@SAHABVphys_step_info@@ABUphys_solver_info@@AAVphys_constraint_query_in@@AAVphys_island@@PBQAVphys_entity@@H@Z
int phys_solver_setup::solve_constraints(const phys_step_info &step_info, const phys_solver_info &solver_info,
    phys_constraint_query_in &constraint_query_in, phys_island &island, phys_entity *const *bodies, int solver_body_count)
{
    int inactive_frame_count = 0;

    char *scratch_b = 0;
    int elem_temp_size = island.m_constraint_info.m_solver_temp_count * sizeof(phys_solver_elem_temp) + 2 * sizeof(phys_solver_elem_temp);
    phys_schema *schemas_b = 0;

    int stack_free_bytes = phys_thread_memory::get_singleton_instance().get_stack().get_free_bytes();
    char *scratch_pad = phys_alloc_stack<char>(stack_free_bytes);
    char *buffer = scratch_pad;
    char *buf_end = buffer + stack_free_bytes;

    for (;;) {
        phys_vel_accum *setup_vel_accums;
        {
            setup_vel_accums = reinterpret_cast<phys_vel_accum *>(buffer);
            buffer += sizeof(phys_vel_accum) + 16 + sizeof(phys_vel_accum) * solver_body_count;
        }

        phys_schema *schema_list;
        phys_schema *schemas_end;
        {
            schema_list = reinterpret_cast<phys_schema *>(buffer);
            buffer += island.m_constraint_info.m_schemas_size + 16;
            schemas_end = reinterpret_cast<phys_schema *>(buffer);
        }

        for (;;) {
            phys_solver_elem_temp *elem_temp;
            {
                elem_temp = reinterpret_cast<phys_solver_elem_temp *>(buffer);
                buffer += elem_temp_size;
            }

            if (buffer <= buf_end) {
                build_vel_accums(step_info, bodies, solver_body_count, setup_vel_accums);

                {
                    for (int i = 0; i < island.m_constraint_info.m_solver_temp_count; i++) {
                        elem_temp[i].m_applied_impulse = 0.0f;
                    }
                }

                build_jacobian_elements(constraint_query_in, bodies, solver_body_count, setup_vel_accums, schema_list, schemas_end, schemas_b);

                int solved = phys_solve_constraints(solver_info, schema_list, setup_vel_accums, elem_temp);
                if (solved == 1) {
                    phys_export_impulses_and_rhs(solver_info, elem_temp, schema_list, setup_vel_accums);
                    inactive_frame_count = phys_solver_setup_integrate2(solver_info, step_info, setup_vel_accums, bodies, solver_body_count);
                }

                if (scratch_b) {
                    phys_free_stack(scratch_b);
                }
                phys_free_stack(scratch_pad);

                flush_solver_debug_stream();

                return inactive_frame_count;
            }

            if (reinterpret_cast<char *>(schema_list) >= buf_end) {
                int size = int(buffer - scratch_pad);
                scratch_b = phys_alloc_stack<char>(size);
                buffer = scratch_b;
                buf_end = buffer + size;
                break;
            }

            int unsplittable_schema_bytes = island.m_constraint_info.m_max_schema_size + 16;

            if (reinterpret_cast<char *>(schemas_end) >= buf_end) {
                int min_schema_bytes_in_buf_a = int(buf_end - reinterpret_cast<char *>(schema_list)) - unsplittable_schema_bytes;
                int clipped_bytes = phys_math::max_of_two(min_schema_bytes_in_buf_a, 0);
                int schema_bytes_b = island.m_constraint_info.m_schemas_size + unsplittable_schema_bytes - clipped_bytes;

                int scratch_b_bytes = elem_temp_size + schema_bytes_b;
                scratch_b = phys_alloc_stack<char>(scratch_b_bytes);

                if (min_schema_bytes_in_buf_a < 0) {
                    buffer = scratch_b;
                    buf_end = buffer + scratch_b_bytes;

                    schema_list = reinterpret_cast<phys_schema *>(buffer);
                    buffer += island.m_constraint_info.m_schemas_size + 16;
                    schemas_end = reinterpret_cast<phys_schema *>(buffer);
                    schemas_b = 0;
                } else {
                    schemas_end = reinterpret_cast<phys_schema *>(buf_end - unsplittable_schema_bytes);

                    buffer = scratch_b;
                    buf_end = buffer + scratch_b_bytes;

                    schemas_b = reinterpret_cast<phys_schema *>(buffer);
                    buffer += schema_bytes_b;
                }
            } else {
                int solver_buffer_size = elem_temp_size;
                scratch_b = phys_alloc_stack<char>(solver_buffer_size);
                buffer = scratch_b;
                buf_end = buffer + solver_buffer_size;
            }

        }
    }
}

typedef char phys_vel_accum_size[(sizeof(phys_vel_accum) == 128) ? 1 : -1];
typedef char phys_vel_accum_lin_offset[(offsetof(phys_vel_accum, m_lin_vel) == 16) ? 1 : -1];

