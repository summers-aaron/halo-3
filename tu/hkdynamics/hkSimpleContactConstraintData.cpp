// hkdynamics/hkSimpleContactConstraintData.obj: from the records 0x1060b8 0x663340 0x663350 0x663360 0x663368 0x663378 0x663380 0x663510 0x6635d8 0x6636b8 0x663820 0x663b30 0x663be8
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x1060b8 ?alloc_contact_id@phys_contact_point_id_manager@@QAAHH@Z
// @address 0x663340 ?get_constraint_info@phys_simple_contact_data@@UBAXAAUconstraint_info@phys_constraint_data@@@Z
// @address 0x663350 ?get_runtime_info@phys_simple_contact_data@@UBAXVphys_bool@@AAUruntime_info@phys_constraint_data@@@Z
// @address 0x663360 ?get_solver_results@phys_simple_contact_data@@UAAPAVphys_solver_results@@PAX@Z
// @address 0x663368 ?is_valid_aabb@phys_simple_contact_data@@UBA?AVphys_bool@@XZ
// @address 0x663378 ?get_type_id@phys_simple_contact_data@@UBAHXZ
// @address 0x663b30 ??0phys_simple_contact_data@@QAA@PAVphys_constraint@@@Z
// @address 0x663be8 ??0phys_simple_contact_data@@QAA@XZ
#include "phys/phys_base.h"
#include <vectorintrinsics.h>
#include "phys/phys_simple_contact_data.h"
#include "phys/phys_simple_collision_response.h"
#include "phys/phys_simple_constraint_util.h"
#include "phys/phys_collide.h"
#include "phys/phys_dynamics.h"
#include "phys/phys_atoms.h"
#include "phys/phys_constraint_owner.h"
#include "phys/phys_contact_point_props.h"
#include "phys/phys_collision_input.h"
struct phys_contact_confirmed_event;
#include "phys/phys_world_callback_util.h"
struct phys_constraint_internal;






#include "phys/phys_simple_contact_atom_util.h"
#include "phys/phys_constraint_info.h"
#include "phys/phys_contact_point.h"
#include "phys/phys_world_constraint_util.h"
#include "phys/phys_vel_accum.h"
#include "phys/phys_constraint_query_in.h"
#include "phys/phys_collision_listener.h"
#include "phys/phys_entity_callback_util.h"
class phys_solver_results;

int phys_contact_point_id_manager::alloc_contact_id(int value)
{
    for (int i = m_values.getSize() - 1; i >= 0; --i) {
        if (m_values[i] == 255) {
            m_values[i] = static_cast<unsigned char>(value);
            return i;
        } else {
            continue;
        }
    }
    unsigned char queue_new_value = static_cast<unsigned char>(value);
    m_values.array_push_back(queue_new_value);
    return m_values.getSize() - 1;
}

void phys_simple_contact_data::get_constraint_info(phys_constraint_data::constraint_info &info) const
{
    phys_constraint_data::gather_constraint_info(m_contact_atom, m_contact_atom->m_all_atoms_size, info);
}

void phys_simple_contact_data::get_runtime_info(phys_bool want_runtime, phys_constraint_data::runtime_info &info) const
{
    info.m_external_runtime_size = 0;
    info.m_solver_result_count = 0;
}

phys_solver_results *phys_simple_contact_data::get_solver_results(void *runtime)
{
    return 0;
}

phys_bool phys_simple_contact_data::is_valid_aabb() const
{
    return phys_bool(true);
}

int phys_simple_contact_data::get_type_id() const
{
    return 11;
}

phys_simple_contact_data::phys_simple_contact_data(phys_constraint *constraint)
    : m_contact_constraint(constraint)
{
    m_contact_atom = phys_simple_contact_atom_util::allocate_contact_atom(1);
    m_contact_atom->m_bvh_info.init();
    m_atom_size = m_contact_atom->m_all_atoms_size;
}

phys_simple_contact_data::phys_simple_contact_data()
    : m_contact_constraint(0)
{
    m_contact_atom = phys_simple_contact_atom_util::allocate_contact_atom(1);
    m_contact_atom->m_bvh_info.init();
    m_atom_size = m_contact_atom->m_all_atoms_size;
}

// @address 0x663510 ?collision_response_begin_callback@phys_simple_contact_data@@UAAXABVphys_contact_point@@AAUphys_simple_constraint_init_input@@AAUphys_body_vel@@12@Z
void phys_simple_contact_data::collision_response_begin_callback(const phys_contact_point &cp, phys_simple_constraint_init_input &init_input_a,
    phys_body_vel &body_vel_a, phys_simple_constraint_init_input &init_input_b, phys_body_vel &body_vel_b)
{
    phys_constraint *instance = m_contact_constraint;
    if (!instance->m_constraint_modifiers)
        return;
    phys_atom *atom = instance->m_constraint_modifiers;
    while (atom->is_modifier_type()) {
        switch (atom->get_type_id()) {
        case 24: {
            phys_mass_changer_atom *c = reinterpret_cast<phys_mass_changer_atom *>(atom);
            c->collision_response_begin_callback(cp, init_input_a, body_vel_a, init_input_b, body_vel_b);
            break;
        }
        case 23: {
            phys_soft_contact_modifier_atom *c = reinterpret_cast<phys_soft_contact_modifier_atom *>(atom);
            c->collision_response_begin_callback(cp, init_input_a, body_vel_a, init_input_b, body_vel_b);
            break;
        }
        case 26: {
            phys_moving_surface_atom *c = reinterpret_cast<phys_moving_surface_atom *>(atom);
            c->collision_response_begin_callback(cp, init_input_a, body_vel_a, init_input_b, body_vel_b);
            break;
        }
        }
        atom = static_cast<phys_modifier_atom *>(atom)->m_child_shape;
    }
}


// @address 0x6635d8 ?collision_response_end_callback@phys_simple_contact_data@@UAAXABVphys_contact_point@@MAAUphys_simple_constraint_init_input@@AAUphys_body_vel@@12@Z
void phys_simple_contact_data::collision_response_end_callback(class phys_contact_point const &p0, float p1,
    struct phys_simple_constraint_init_input &p2, struct phys_body_vel &p3, struct phys_simple_constraint_init_input &p4,
    struct phys_body_vel &p5)
{
    phys_constraint *instance = m_contact_constraint;
    if (!instance->m_constraint_modifiers)
        return;
    phys_atom *atom = instance->m_constraint_modifiers;
    while (atom->is_modifier_type()) {
        switch (atom->get_type_id()) {
        case 23: {
            phys_soft_contact_modifier_atom *c = reinterpret_cast<phys_soft_contact_modifier_atom *>(atom);
            c->collision_response_end_callback(p0, p1, p2, p3, p4, p5);
            break;
        }
        case 24: {
            phys_mass_changer_atom *c = reinterpret_cast<phys_mass_changer_atom *>(atom);
            c->collision_response_end_callback(p0, p1, p2, p3, p4, p5);
            break;
        }
        case 26: {
            phys_moving_surface_atom *c = reinterpret_cast<phys_moving_surface_atom *>(atom);
            c->collision_response_end_callback(p0, p1, p2, p3, p4, p5);
            break;
        }
        }
        atom = reinterpret_cast<phys_modifier_atom *>(atom)->m_child_shape;
    }
}

class phys_constraint;
struct phys_atom;

// @address 0x663380 ?release_contact_point@phys_simple_contact_data@@QAAXAAVphys_constraint_owner@@G@Z
void phys_simple_contact_data::release_contact_point(class phys_constraint_owner &constraint_owner_ref, unsigned short id)
{
    int mid = int(id);

    phys_simple_contact_atom *local_atom = m_contact_atom;

    int removed_point_idx = m_id_manager.get_value_at(mid);
    m_id_manager.release_contact_id(mid);

    phys_constraint_info contact_constraint_info;
    contact_constraint_info.clear();

    const phys_simple_contact_atom *original_atom = m_contact_atom;
    int atom_contact_count = local_atom->m_contact_point_count;
    {
        if (atom_contact_count == 2)
            contact_constraint_info.add(32, 1, 1);

        if (atom_contact_count & 1)
            contact_constraint_info.add(48, 1, 1);
        else
            contact_constraint_info.add(64, 1, 1);

        phys_simple_contact_atom_util::remove_at_and_copy(local_atom, removed_point_idx);
        unsigned char &flags = local_atom->get_contact_point_props()[removed_point_idx].m_flags;
        flags &= ~phys_contact_point_material::CONTACT_FLAG_SOLVER_PATH2;

        m_contact_atom = phys_simple_contact_atom_util::optimize_capacity(m_contact_atom, 1);
        local_atom = m_contact_atom;

        m_id_manager.decrement_values_above(removed_point_idx);
    }

    constraint_owner_ref.sub_constraint_info(m_contact_constraint, contact_constraint_info);

    if (original_atom != m_contact_atom) {
        phys_world_constraint_util::update_moved_atom_parent(m_contact_constraint, original_atom, m_contact_atom, m_contact_atom->m_all_atoms_size);
        m_atom_size = m_contact_atom->m_all_atoms_size;
    }
    local_atom->m_bvh_info.m_flags |= 5;
}

// @address 0x6636b8 ?allocate_contact_point@phys_simple_contact_data@@QAAGAAVphys_constraint_owner@@PAPAVphys_contact_point@@PAPAVphys_contact_point_props@@@Z
unsigned short phys_simple_contact_data::allocate_contact_point(
    class phys_constraint_owner &constraint_owner_ref, class phys_contact_point **contact_point_out, class phys_contact_point_props **contact_props_out)
{
    phys_simple_contact_atom *local_atom = m_contact_atom;
    local_atom->m_bvh_info.m_flags |= 4;
    int new_contact_idx = local_atom->m_contact_point_count;

    const phys_simple_contact_atom *original_atom = m_contact_atom;
    m_contact_atom = phys_simple_contact_atom_util::expand_one(m_contact_atom);
    local_atom = m_contact_atom;

    phys_contact_point *dst_contact_point = &local_atom->get_contact_points()[new_contact_idx];
    phys_contact_point_props *point_props = &local_atom->get_contact_point_props()[new_contact_idx];
    point_props->m_applied_impulse = 0.0f;
    point_props->m_internal_solver_data = 0.0f;

    unsigned char &flags = point_props->m_flags;
    flags = 1;
    if (new_contact_idx > 0 && !(point_props[-1].m_flags & 2))
        flags |= 2;

    *contact_point_out = dst_contact_point;
    *contact_props_out = point_props;

    phys_constraint_info info;
    info.clear();
    int array_new_size = new_contact_idx + 1;
    info.m_max_schema_size = (array_new_size >> 1) * 112 + 16 + (array_new_size & 1) * 48 + 144;
    if (new_contact_idx == 1)
        info.add(32, 1, 1);
    if (array_new_size & 1)
        info.add(48, 1, 1);
    else
        info.add(64, 1, 1);
    constraint_owner_ref.add_constraint_info(m_contact_constraint, info);

    if (original_atom != m_contact_atom) {
        phys_world_constraint_util::update_moved_atom_parent(m_contact_constraint, original_atom, m_contact_atom, local_atom->m_all_atoms_size);
        m_atom_size = local_atom->m_all_atoms_size;
    }
    return (unsigned short)m_id_manager.alloc_contact_id(new_contact_idx);
}
// @address 0x663820 ?phys_simple_contact_fire_callbacks@@YAXPAVphys_simple_contact_data@@PBVphys_constraint_query_in@@PAUphys_simple_contact_atom@@@Z
void phys_simple_contact_fire_callbacks(
    phys_simple_contact_data *contact_constraint_data, const phys_constraint_query_in *in, phys_simple_contact_atom *atom)
{
    phys_constraint *constraint_inst = in->m_query_constraint;
    phys_rigid_body *agent_body_a = constraint_inst->get_rigid_body_a();
    phys_rigid_body *agent_body_b = constraint_inst->get_rigid_body_b();
    phys_contact_point_props *contact_props = atom->get_contact_point_props();
    phys_contact_point *cp = atom->get_contact_points();
    int contact_point_cnt = atom->m_contact_point_count;

    for (int contact_idx = contact_point_cnt - 1; contact_idx >= 0; cp++, contact_props++, contact_idx--)
    {
        if ((contact_props->m_flags) & 1)
        {
            phys_vec4 rel_pos_a;
            rel_pos_a.set_sub4(cp->get_contact_position(), in->m_body_a->get_center_of_mass_in_world());
            phys_vec4 body_vel_a;
            body_vel_a.set_cross(agent_body_a->get_angular_velocity(), rel_pos_a);
            body_vel_a.add4(agent_body_a->get_linear_velocity());

            phys_vec4 rel_pos_b;
            rel_pos_b.set_sub4(cp->get_contact_position(), in->m_body_b->get_center_of_mass_in_world());
            phys_vec4 body_vel_b;
            body_vel_b.set_cross(agent_body_b->get_angular_velocity(), rel_pos_b);
            body_vel_b.add4(agent_body_b->get_linear_velocity());

            phys_vec4 delta_vel;
            delta_vel.set_sub4(body_vel_a, body_vel_b);
            float proj_normal_vel = delta_vel.dot3(cp->get_contact_normal());

            phys_contact_confirmed_event event(phys_contact_added_event::TYPE_MANIFOLD,
                agent_body_a->get_collider(), agent_body_b->get_collider(), contact_constraint_data, cp,
                contact_props, 0.0f, proj_normal_vel);
            phys_world *world = agent_body_a->get_action_world();
            ::phys_world_callback_util::notify_contact_point_confirmed(world, (*&event));
            phys_entity_callback_util::notify_contact_point_confirmed(agent_body_a, event);
            phys_entity_callback_util::notify_contact_point_confirmed(agent_body_b, event);

            phys_world_dynamics_step_info *collision_env = world->get_collision_input()->m_dynamics_info;
            float restitution = contact_props->get_restitution();
            if (event.m_projected_vel < -collision_env->m_solver_info.m_contact_resting_vel && restitution > 0.3f)
            {
                phys_simple_collide_params params;
                params.m_direction = cp->get_contact_normal();
                params.m_contact_friction = contact_props->get_friction();
                params.m_contact_restitution = restitution;
                params.m_extra_sep_vel = 0.0f;
                params.m_extra_slope = 0.0f;
                params.m_external_sep_vel = event.m_projected_vel;
                phys_simple_collision_response::solve_single_output_ext second_out;
                phys_simple_collision_response::solve_single_contact2(contact_constraint_data, *cp, params, agent_body_a, agent_body_b,
                    const_cast<phys_vel_accum *>(in->m_body_a.val()), const_cast<phys_vel_accum *>(in->m_body_b.val()), second_out);
                contact_props->m_applied_impulse = 0.0f;
                contact_props->m_internal_data_a = cp->get_distance();
            }
            else
            {
                float mass = 1.0f / (agent_body_a->get_inv_mass() + agent_body_b->get_inv_mass() + 1e-10f);
                contact_props->m_applied_impulse = mass * (contact_props->get_restitution() + 1.0f) * event.m_projected_vel;
                contact_props->m_applied_impulse *= -0.2f;
                float error_term = contact_props->get_restitution() * event.m_projected_vel * in->m_substep_dt;
                error_term *= -1.3f;
                contact_props->m_internal_solver_data = error_term;
                contact_props->m_internal_data_a = cp->get_distance() + error_term;
            }
            (contact_props->m_flags) &= ~1;
        }
    }
    constraint_inst->m_constraint_internal->m_callback_request &= ~1;
}

