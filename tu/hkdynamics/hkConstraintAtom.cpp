// hkdynamics/hkConstraintAtom.obj: from the records 0x65e058 0x65e068 0x65e080 0x65e118 0x65e120 0x65e160 0x65e240 0x65e288 0x65e2d0 0x65e328
// @flags /TP /O2
// @address 0x65e058 ?phys_bridge_atom_call_data@@YAXPAVphys_constraint_data@@ABVphys_constraint_query_in@@AAVphys_constraint_query_out@@@Z
// @address 0x65e160 ?collision_response_end_callback@phys_soft_contact_modifier_atom@@QAAXABVphys_contact_point@@MAAUphys_simple_constraint_init_input@@AAUphys_body_vel@@12@Z
// @address 0x65e2d0 ?add_modifier_info@phys_modifier_atom@@QBAHAAUphys_constraint_info@@@Z
// @address 0x65e068 ?init@phys_bridge_atom@@QAAXPAVphys_constraint_data@@@Z
// @address 0x65e118 ?collision_response_end_callback@phys_mass_changer_atom@@QAAXABVphys_contact_point@@MAAUphys_simple_constraint_init_input@@AAUphys_body_vel@@12@Z
// @address 0x65e120 ?collision_response_begin_callback@phys_soft_contact_modifier_atom@@QAAXABVphys_contact_point@@AAUphys_simple_constraint_init_input@@AAUphys_body_vel@@12@Z
#include "phys/phys_simple_constraint_util.h"
#include <ppcintrinsics.h>
#include <vectorintrinsics.h>
#include "phys/phys_math.h"
#include "phys/phys_collide.h"

#include "phys/phys_atoms.h"
#include "phys/phys_atom.h"
class phys_constraint_data;
struct phys_atom;
// @address 0x7ceb30 g_phys_soft_contact_body_vels



void phys_bridge_atom_call_data(phys_constraint_data *m_constraint_data, const phys_constraint_query_in &in, phys_constraint_query_out &out)
{
    m_constraint_data->build_jacobian(in, out);
}

// 0x65e068
void phys_bridge_atom::init(phys_constraint_data *data)
{
    m_constraint_data = data;
    m_build_jacobian_func = &phys_bridge_atom_call_data;
}

// @address 0x65e080 ?collision_response_begin_callback@phys_mass_changer_atom@@QAAXABVphys_contact_point@@AAUphys_simple_constraint_init_input@@AAUphys_body_vel@@12@Z
void phys_mass_changer_atom::collision_response_begin_callback(const phys_contact_point &, phys_simple_constraint_init_input &agent_body_a,
    phys_body_vel &, phys_simple_constraint_init_input &agent_body_b, phys_body_vel &)
{
    float factor_a = m_factor_a;
    float factor_b = m_factor_b;
    agent_body_a.m_inv_mass *= factor_a;
    agent_body_a.m_inv_inertia.mul(phys_simd_scalar(factor_a));
    agent_body_b.m_inv_mass *= factor_b;
    agent_body_b.m_inv_inertia.mul(phys_simd_scalar(factor_b));
}

// 0x65e118
void phys_mass_changer_atom::collision_response_end_callback(
    const phys_contact_point &, float, phys_simple_constraint_init_input &, phys_body_vel &, phys_simple_constraint_init_input &, phys_body_vel &)
{
}

// 0x65e120
void phys_soft_contact_modifier_atom::collision_response_begin_callback(const phys_contact_point &, phys_simple_constraint_init_input &,
    phys_body_vel &contact_velocity_a, phys_simple_constraint_init_input &, phys_body_vel &velocity_b)
{
    phys_body_vel *saved_body_vels = g_phys_soft_contact_body_vels;
    saved_body_vels[0] = contact_velocity_a;
    saved_body_vels[1] = velocity_b;
}

void phys_soft_contact_modifier_atom::collision_response_end_callback(const phys_contact_point &, float, phys_simple_constraint_init_input &,
    phys_body_vel &contact_velocity_a, phys_simple_constraint_init_input &, phys_body_vel &velocity_b)
{
    float blend = m_query_tau;
    {
        phys_simd_scalar tau(blend);
        contact_velocity_a.m_lin_parts.set_lerp4(g_phys_soft_contact_body_vels[0].m_lin_parts, contact_velocity_a.m_lin_parts, tau);
    }
    {
        phys_simd_scalar tau(blend);
        contact_velocity_a.m_ang_jacobian.set_lerp4(g_phys_soft_contact_body_vels[0].m_ang_jacobian, contact_velocity_a.m_ang_jacobian, tau);
    }
    {
        phys_simd_scalar tau(blend);
        velocity_b.m_lin_parts.set_lerp4(g_phys_soft_contact_body_vels[1].m_lin_parts, velocity_b.m_lin_parts, tau);
    }
    {
        phys_simd_scalar tau(blend);
        velocity_b.m_ang_jacobian.set_lerp4(g_phys_soft_contact_body_vels[1].m_ang_jacobian, velocity_b.m_ang_jacobian, tau);
    }
}


// @address 0x65e240 ?collision_response_begin_callback@phys_moving_surface_atom@@QAAXABVphys_contact_point@@AAUphys_simple_constraint_init_input@@AAUphys_body_vel@@12@Z
void phys_moving_surface_atom::collision_response_begin_callback(const phys_contact_point &contact, phys_simple_constraint_init_input &,
    phys_body_vel &, phys_simple_constraint_init_input &, phys_body_vel &velocity_b)
{
    phys_vec4 velocity = get_atom_velocity();
    phys_simd_scalar normal_vel = velocity.dot3(contact.get_contact_normal());
    phys_vec4 contact_projected_vel;
    contact_projected_vel.set_mul4(normal_vel, velocity);
    velocity.sub4(contact_projected_vel);
    velocity_b.m_lin_parts.add4(velocity);
}

// @address 0x65e288 ?collision_response_end_callback@phys_moving_surface_atom@@QAAXABVphys_contact_point@@MAAUphys_simple_constraint_init_input@@AAUphys_body_vel@@12@Z
void phys_moving_surface_atom::collision_response_end_callback(const phys_contact_point &contact, float,
    phys_simple_constraint_init_input &, phys_body_vel &, phys_simple_constraint_init_input &, phys_body_vel &velocity_b)
{
    phys_vec4 velocity = get_atom_velocity();
    phys_simd_scalar normal_vel = velocity.dot3(contact.get_contact_normal());
    phys_vec4 contact_projected_vel;
    contact_projected_vel.set_mul4(normal_vel, velocity);
    velocity.sub4(contact_projected_vel);
    velocity_b.m_lin_parts.sub4(velocity);
}

int phys_modifier_atom::add_modifier_info(phys_constraint_info &info) const
{
    int result = 0;
    switch (m_type) {
    case 23:
    case 25:
        result = 2;
        break;
    case 24:
    case 26:
        info.m_schemas_size += 80;
        break;
    }
    return result;
}

// @address 0x65e328 ?add_all_modifier_info@phys_modifier_atom@@SAHPAU1@AAUphys_constraint_info@@@Z
int phys_modifier_atom::add_all_modifier_info(phys_modifier_atom *modifier, phys_constraint_info &info)
{
    int atom_callback_req = 0;
    phys_atom *atom = (phys_atom *)modifier;
    while (1) {
        if (!atom->is_modifier_type())
            break;
        phys_modifier_atom *modifier_atom = (phys_modifier_atom *)atom;
        atom_callback_req |= modifier_atom->add_modifier_info(info);
        atom = modifier_atom->m_child_shape;
    }
    return atom_callback_req;
}
