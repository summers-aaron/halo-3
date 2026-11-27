// hkdynamics/hkSimpleConstraintContactMgr.obj: from the records 0x655fc0 0x655fc8 0x656008 0x656020 0x656038 0x656098 0x6560b8 0x656218 0x6562e0 0x6563e8 0x6564e0 0x656500 0x656538 0x656558 0x656938 0x656940 0x656aa8 0x656b78 0x656c38 0x656cb8 0x656d18 0x656da8 0x656e30 0x656e98
// @flags /TP /O2 /Oc
// @address 0x655fc0 ?get_chain_constraint@phys_simple_contact_manager@@UAAPAVphys_constraint@@XZ
// @address 0x655fc8 ?reserve_contact_points_impl@phys_simple_contact_manager@@UAA?AW4phys_result@@H@Z
// @address 0x656008 ?toi_response_begin_callback@phys_simple_contact_manager@@UAAXABVphys_contact_point@@AAUphys_simple_constraint_init_input@@AAUphys_body_vel@@12@Z
// @address 0x656020 ?toi_response_end_callback@phys_simple_contact_manager@@UAAXABVphys_contact_point@@MAAUphys_simple_constraint_init_input@@AAUphys_body_vel@@12@Z
// @address 0x656038 ?get_contact_point_id@phys_contact_confirmed_event@@QBAGXZ
// @address 0x656098 ?get_contact_manager@phys_contact_confirmed_event@@QBAPBVphys_dynamics_contact_manager@@XZ
// @address 0x6560b8 ?add_toi_impl@phys_simple_contact_manager@@UAA?AW4toi_verdict@phys_contact_manager@@ABVphys_collision_proxy@@0ABUphys_process_collision_input@@AAUphys_process_collision_output@@MAAVphys_contact_point@@PBVphys_simplex_cache@@AAMAAVphys_contact_point_material@@@Z
// @address 0x656218 ?remove_toi_impl@phys_simple_contact_manager@@UAAXABVphys_collider@@0AAVphys_constraint_owner@@AAVphys_contact_point_material@@@Z
// @address 0x6562e0 ?process_toi@phys_simple_contact_manager@@UAAXAAUphys_toi_event@@MAAV?$phys_array@PAVphys_entity@@@@@Z
// @address 0x656538 ??0report_contact_factory@phys_simple_contact_manager@@QAA@PAVphys_world@@@Z
// @address 0x656938 ?get_all_contact_point_ids@phys_simple_contact_manager@@UBAXAAV?$phys_array@G@@@Z
// @address 0x656aa8 ?gather_memory_stats@phys_simple_contact_manager@@MBAXPAVphys_statistics_collector@@@Z
// @address 0x656b78 ??0phys_simple_contact_manager@@QAA@PAVphys_world@@PAVphys_rigid_body@@1@Z
// @address 0x656c38 ??1phys_simple_contact_manager@@UAA@XZ
// @address 0x656cb8 ?create_contact_manager@report_contact_factory@phys_simple_contact_manager@@UAAPAVphys_contact_manager@@ABVphys_collider@@0ABUphys_collision_input@@@Z
// @address 0x656d18 ?get_all_used_ids@phys_contact_point_id_manager@@QBAXAAV?$phys_array@G@@@Z
// @address 0x656da8 ??1phys_simple_contact_data@@UAA@XZ
#include "phys/phys_base_types.h"
// @address 0x656e30 ??_Gphys_simple_contact_data@@UAAPAXI@Z
// @address 0x656e98 ??_Gphys_simple_contact_manager@@UAAPAXI@Z
#include <ppcintrinsics.h>
#include <stdlib.h>
#include <math.h>
#include "phys/phys_math.h"
#include "phys/phys_simple_contact_atom_util.h"
#include "phys/phys_object.h"
#pragma intrinsic(_rotl)
#include "phys/phys_dynamics_contact_manager.h"
#include "phys/phys_collision_listener.h"
#include "phys/phys_dispatcher.h"
#include "phys/phys_atoms.h"
#include "phys/phys_constraint.h"
#include "phys/phys_constraint_owner.h"
#include "phys/phys_contact_point_props.h"
#include "phys/phys_contact_point_id_manager.h"
#include "phys/phys_simple_contact_data.h"
#include "phys/phys_statistics_collector.h"
#include "phys/phys_world_op_util.h"
#include "phys/phys_toi_point_added_event.h"

struct phys_simple_contact_atom;
#include "phys/phys_thread_memory.h"
#include "phys/phys_dynamics_contact_manager.h"
#include "phys/phys_collide.h"
#include "phys/phys_collision_input.h"
#include "phys/phys_constraint_query_in.h"
#include "phys/phys_contact_point.h"
#include "phys/phys_constraint_query_out.h"

#include "phys/phys_dynamics.h"
#include "phys/phys_world_callback_util.h"
#include "phys/phys_memory_class.h"
#include "phys/phys_entity_callback_util.h"
#include "phys/phys_simple_constraint_util.h"
class phys_report_contact_manager;
// @address 0x655f40 ?cleanup@phys_contact_manager@@UAAXXZ

#include "phys/phys_toi_event.h"
#include "phys/phys_manifold_point_added_event.h"
#include "phys/phys_simple_constraint_contact_manager.h"
#include "phys/phys_collision_listener.h"

static __forceinline void fire_entity_contact_added(phys_world *world, phys_entity *agent_body_a, phys_entity *agent_body_b, phys_contact_added_event &event)
{
    phys_world_callback_util::fire_entity_contact_added(world, event);
    phys_entity_callback_util::fire_entity_contact_added(agent_body_a, event);
    phys_entity_callback_util::fire_entity_contact_added(agent_body_b, event);
}
static __forceinline phys_rigid_body *rigid_body_of(const phys_collider &collidable)
{
    if (collidable.get_type_id() == 1) return static_cast<phys_rigid_body *>(collidable.get_collider_owner());
    else return 0;
}
static __forceinline void fire_entity_contact_removed(phys_world *world, phys_entity *agent_body_a, phys_entity *agent_body_b,
    phys_contact_removed_event &event)
{
    phys_world_callback_util::fire_entity_contact_removed(world, event);
    phys_entity_callback_util::fire_entity_contact_removed(agent_body_a, event);
    phys_entity_callback_util::fire_entity_contact_removed(agent_body_b, event);
}
static __forceinline void notify_contact_point_confirmed(phys_entity *body, phys_contact_confirmed_event &event)
{
    phys_entity_callback_util::notify_contact_point_confirmed(body, event);
}

class phys_constraint;
class phys_solver_results;
// @address 0x5bec98 ?build_jacobian@phys_constraint_data@@UAAXABVphys_constraint_query_in@@AAVphys_constraint_query_out@@@Z
// @address 0x64cda0 ?needs_jacobian_callback@phys_constraint_data@@UAA?AVphys_bool@@XZ
// @address 0x5bec98 ?build_jacobian_callback@phys_constraint_data@@UAAXABVphys_constraint_query_in@@@Z

class phys_solver_results;

// @address 0x42d298 ?get_type_id@phys_simple_contact_manager@@UAE?AW4Type@phys_contact_manager@@XZ
phys_contact_manager::Type phys_simple_contact_manager::get_type_id()
{
    return CONTACT_MANAGER_SIMPLE_CONSTRAINT;
}

phys_constraint *phys_simple_contact_manager::get_chain_constraint()
{
    return &m_contact_constraint;
}

phys_result phys_simple_contact_manager::reserve_contact_points_impl(int contact_reserve_count)
{
    int atom_contact_count = m_contact_data.m_contact_atom->m_contact_point_count;
    if (atom_contact_count + contact_reserve_count + m_reserved_contact_points > 126) {
        return PHYS_FAILURE;
    }
    m_reserved_contact_points = static_cast<unsigned char>(m_reserved_contact_points + contact_reserve_count);
    return PHYS_SUCCESS;
}

void phys_simple_contact_manager::toi_response_begin_callback(const phys_contact_point &point, phys_simple_constraint_init_input &body_input_a,
    phys_body_vel &contact_velocity_a, phys_simple_constraint_init_input &body_input_b, phys_body_vel &velocity_b)
{
    m_contact_data.collision_response_begin_callback(point, body_input_a, contact_velocity_a, body_input_b, velocity_b);
}

void phys_simple_contact_manager::toi_response_end_callback(const phys_contact_point &point, float impulse,
    phys_simple_constraint_init_input &body_input_a, phys_body_vel &contact_velocity_a,
        phys_simple_constraint_init_input &body_input_b, phys_body_vel &velocity_b)
{
    m_contact_data.collision_response_end_callback(point, impulse, body_input_a, contact_velocity_a, body_input_b, velocity_b);
}

unsigned short phys_contact_confirmed_event::get_contact_point_id() const
{
    if (is_toi_event()) {
        return 0xffff;
    } else {
        const phys_simple_contact_data *data = m_contact_data;
        int contact_idx = m_contact_point - data->m_contact_atom->get_contact_points();
        int sphere_contact_id = data->m_id_manager.array_index_of(contact_idx);
        return static_cast<unsigned short>(sphere_contact_id);
    }
}

namespace {
}


const phys_dynamics_contact_manager *phys_contact_confirmed_event::get_contact_manager() const
{
    if (is_toi_event()) {
        return 0;
    } else {
        int offset = 20;
        const phys_simple_contact_manager *manager = reinterpret_cast<const phys_simple_contact_manager *>(
            reinterpret_cast<const char *>(phys_add_byte_offset_const(m_contact_data, -offset)));
        return reinterpret_cast<const phys_dynamics_contact_manager *>(manager);
    }
}

phys_contact_manager::toi_verdict phys_simple_contact_manager::add_toi_impl(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_process_collision_input &input, phys_process_collision_output &output, float time, phys_contact_point &point, const phys_simplex_cache *cache,
    float &velocity, phys_contact_point_material &material)
{
    phys_entity *listener_entity_a = static_cast<phys_entity *>(agent_body_a.get_root_collider()->get_collider_owner());
    phys_entity *listener_entity_b = static_cast<phys_entity *>(agent_body_b.get_root_collider()->get_collider_owner());
    phys_material &entity_material_a = listener_entity_a->get_body_material();
    phys_material &entity_material_b = listener_entity_b->get_body_material();
    material.set_friction_coeff(phys_material::combine_friction(entity_material_a.get_friction(), entity_material_b.get_friction()));
    material.set_restitution(phys_material::combine_restitution(entity_material_a.get_restitution(), entity_material_b.get_restitution()));
    phys_toi_point_added_event event(this, &input, &output, &agent_body_a, &agent_body_b, &point, cache, &material, time, velocity);
    fire_entity_contact_added(m_world, listener_entity_a, listener_entity_b, event);
    velocity = event.m_projected_vel;
    return static_cast<phys_contact_manager::toi_verdict>(event.m_status);
}

void phys_simple_contact_manager::remove_toi_impl(
    const phys_collider &pair_collider_a, const phys_collider &pair_collider_b, phys_constraint_owner &owner, phys_contact_point_material &material)
{
    phys_rigid_body *agent_body_a = rigid_body_of(pair_collider_a);
    phys_rigid_body *agent_body_b = rigid_body_of(pair_collider_b);
    phys_world *world = agent_body_a->get_action_world();
    phys_contact_removed_event event(0xffff, this, &owner, &material, agent_body_a, agent_body_b);
    fire_entity_contact_removed(world, agent_body_a, agent_body_b, event);
}

void phys_simple_contact_manager::process_toi(phys_toi_event &hit_toi, float velocity, phys_array<phys_entity *> &entities)
{
    phys_world *world = hit_toi.m_island_entities[0]->get_action_world();
    phys_contact_confirmed_event event(phys_contact_added_event::COLLISION_EVENT_TOI, &hit_toi.m_island_entities[0]->m_collider,
        &hit_toi.m_island_entities[1]->m_collider, 0, &hit_toi.m_contact_point, &hit_toi.m_properties, velocity, hit_toi.m_separating_vel);
    phys_world_callback_util::notify_contact_point_confirmed(world, event);
    notify_contact_point_confirmed(hit_toi.m_island_entities[0], event);
    notify_contact_point_confirmed(hit_toi.m_island_entities[1], event);
    velocity = event.m_rotate_normal;
    phys_entity *agent_body_a = hit_toi.m_island_entities[0];
    phys_entity *agent_body_b = hit_toi.m_island_entities[1];
    if (agent_body_a->is_fixed_or_keyframed() && agent_body_b->is_fixed_or_keyframed()) return;
    else phys_toi_simple_collision_response(world, hit_toi, velocity, entities);
}

static __forceinline phys_contact_point &phys_simple_contact_get_point(phys_simple_contact_data &data, int id)
{
    const int index = data.m_id_manager.m_values[id];
    return data.m_contact_atom->get_contact_points()[index];
}
// @address 0x6564e0 ?get_process_contact_point@phys_simple_contact_manager@@UAAPAVphys_contact_point@@G@Z
phys_contact_point *phys_simple_contact_manager::get_process_contact_point(unsigned short id)
{
    return &phys_simple_contact_get_point(m_contact_data, id);
}

// @address 0x656500 ?get_contact_point_props@phys_simple_contact_manager@@UAAPAVphys_contact_point_props@@G@Z
phys_contact_point_props *phys_simple_contact_manager::get_contact_point_props(unsigned short index)
{
    phys_simple_contact_data *data = &m_contact_data;
    phys_simple_contact_atom *atom = data->m_contact_atom;
    unsigned char property_idx = data->m_id_manager.m_values[index];
    return atom->get_contact_point_props() + property_idx;
}


phys_simple_contact_manager::report_contact_factory::report_contact_factory(phys_world *world) : m_world(world)
{
}

void phys_simple_contact_manager::get_all_contact_point_ids(phys_array<unsigned short> &ids) const
{
    m_contact_data.m_id_manager.get_all_used_ids(ids);
}

void phys_simple_contact_manager::gather_memory_stats(phys_statistics_collector *collector) const
{
    collector->begin_stats_object(0, phys_statistics_collector::stat_category_runtime, this);
    collector->add_chunk_stats("ContactAtom", phys_statistics_collector::stat_category_runtime, m_contact_data.m_contact_atom,
        m_contact_data.m_contact_atom->m_all_atoms_size, 0);
    if ((m_contact_data.m_id_manager.m_values.m_capacity_and_flags & (1 << 31)) == 0) {
        collector->add_chunk_stats("ContactIds", phys_statistics_collector::stat_category_runtime,
            m_contact_data.m_id_manager.m_values.m_data, m_contact_data.m_id_manager.m_values.m_size,
            m_contact_data.m_id_manager.m_values.m_capacity_and_flags & 0x3fffffff);
    }
    collector->end_object();
}
phys_simple_contact_manager::phys_simple_contact_manager(phys_world *world, phys_rigid_body *agent_body_a, phys_rigid_body *agent_body_b)
    : phys_dynamics_contact_manager(CONTACT_MANAGER_SIMPLE_CONSTRAINT), m_contact_data(&m_contact_constraint),
        m_contact_constraint(phys_constraint::PRIORITY_PSI)
{
    m_contact_constraint.m_data = &m_contact_data;
    m_contact_constraint.m_island_entities[0] = agent_body_a;
    m_contact_constraint.m_island_entities[1] = agent_body_b;
    m_world = world;
    m_flags = 0;
    m_reserved_contact_points = 0;
    m_contact_data.m_mem_size_and_flags = 0;
    m_contact_constraint.m_mem_size_and_flags = 0;
    phys_dispatcher *dispatcher = world->get_dispatcher();
    signed char index = dispatcher->get_collision_quality_idx(
        static_cast<phys_collider_quality>(agent_body_a->m_collider.m_broadphase_handle.m_quality_type),
        static_cast<phys_collider_quality>(agent_body_b->m_collider.m_broadphase_handle.m_quality_type));
    m_contact_constraint.set_constraint_priority(
        static_cast<phys_constraint::constraint_priority>(dispatcher->get_collision_quality_info(index)->m_constraint_priority));
}

phys_simple_contact_manager::~phys_simple_contact_manager()
{
    if (m_contact_data.m_contact_atom->m_contact_point_count != 0) {
        phys_world_op_util::remove_constraint_now(m_world, &m_contact_constraint, phys_world_op_util::OP_CALLBACKS_FIRE);
    }
    m_contact_constraint.m_island_entities[0] = 0;
    m_contact_constraint.m_island_entities[1] = 0;
    m_contact_constraint.m_data = 0;
}

phys_contact_manager *phys_simple_contact_manager::report_contact_factory::create_contact_manager(const phys_collider &a,
    const phys_collider &b, const phys_collision_input &input)
{
    phys_rigid_body *agent_body_a = static_cast<phys_rigid_body *>(a.get_collider_owner());
    phys_rigid_body *agent_body_b = static_cast<phys_rigid_body *>(b.get_collider_owner());
    phys_thread_memory *thread = phys_thread_memory::get_instance_ptr();
    void *memory = ((phys_thread_memory *)thread)->allocate_chunk(sizeof(phys_simple_contact_manager), (phys_memory_class)32);
    reinterpret_cast<phys_simple_contact_manager *>(memory)->m_mem_size_and_flags = sizeof(phys_simple_contact_manager);
    return reinterpret_cast<phys_contact_manager *>(new (memory) phys_simple_contact_manager(m_world, agent_body_a, agent_body_b));
}

void phys_contact_point_id_manager::get_all_used_ids(phys_array<unsigned short> &ids) const
{
    for (int i = 0; i < m_values.getSize(); ++i) {
        if (m_values[i] != 255) {
            unsigned short id = static_cast<unsigned short>(i);
            ids.array_push_back(id);
        }
    }
}

phys_simple_contact_data::~phys_simple_contact_data()
{
    if (m_contact_atom != 0) {
        phys_simple_contact_atom_util::free_contact_atom(m_contact_atom);
    }
}
// @address 0x6563e8 ?remove_contact_point_impl@phys_simple_contact_manager@@UAAXGAAVphys_constraint_owner@@@Z
void phys_simple_contact_manager::remove_contact_point_impl(unsigned short heightfield_contact_id, phys_constraint_owner &owner)
{
    phys_simple_contact_data *data = &m_contact_data;
    phys_entity *listener_entity_a = m_contact_constraint.m_island_entities[0];
    phys_entity *listener_entity_b = m_contact_constraint.m_island_entities[1];
    phys_simple_contact_atom *atom = data->m_contact_atom;
    unsigned char index = data->m_id_manager.m_values.m_data[heightfield_contact_id];
    phys_contact_point_props *prop =
        reinterpret_cast<phys_contact_point_props *>(reinterpret_cast<char *>(atom) + 48 + atom->m_reserved_contact_count * 32) + index;
    phys_contact_point_material *material = prop ? prop : 0;
    phys_contact_removed_event event(heightfield_contact_id, this, &owner, material, listener_entity_a, listener_entity_b);
    fire_entity_contact_removed(m_world, listener_entity_a, listener_entity_b, event);
    data->release_contact_point(owner, heightfield_contact_id);
    if (m_contact_data.m_contact_atom->m_contact_point_count == 0) {
        owner.remove_constraint_locked_island(&m_contact_constraint);
    }
}

extern "C" {
}
static __forceinline void phys_fire_contact_process_cb(phys_world *world, phys_entity *agent_body_a, phys_entity *agent_body_b,
    phys_contact_process_event &event)
{
    phys_world_callback_util::notify_contact_process(world, event);
    phys_entity_callback_util::notify_contact_process(agent_body_a, event);
    phys_entity_callback_util::notify_contact_process(agent_body_b, event);
}
template <typename T> static __forceinline T phys_min2(T x, T y) { return x < y ? x : y; }
static __forceinline phys_contact_point_props *contact_props_of(phys_simple_contact_data &data, int id)
{
    phys_simple_contact_atom *atom = data.m_contact_atom;
    unsigned char index = data.m_id_manager.m_values.m_data[id];
    return reinterpret_cast<phys_contact_point_props *>(reinterpret_cast<char *>(atom) + 48 + atom->m_reserved_contact_count * 32) + index;
}
static __forceinline phys_contact_point &contact_point_of(phys_simple_contact_data &data, int id)
{
    return data.m_contact_atom->get_contact_points()[data.m_id_manager.m_values.m_data[id]];
}

// @address 0x656940 ?process_contact_impl@phys_simple_contact_manager@@UAAXABVphys_collider@@0ABUphys_process_collision_input@@AAUphys_process_collision_data@@@Z
void phys_simple_contact_manager::process_contact_impl(
    const phys_collider &a, const phys_collider &b, const phys_process_collision_input &input, phys_process_collision_data &collision_data)
{
    {
        if (m_flags-- == 0) {
            phys_rigid_body *owner_body_a = static_cast<phys_rigid_body *>(a.get_collider_owner());
            phys_rigid_body *owner_body_b = static_cast<phys_rigid_body *>(b.get_collider_owner());
            m_flags = phys_min2(owner_body_a->m_contact_callback_delay, owner_body_b->m_contact_callback_delay);
            {
                phys_contact_process_event event(this, &a, &b, &collision_data);
                phys_process_hit_point *hit_end = collision_data.get_track_end();
                phys_contact_point_props **contact_props_slot = &event.m_contact_point_props[0];
                for (phys_process_hit_point *contact_hit = collision_data.get_first_contact_point(); contact_hit < hit_end; contact_hit++) {
                    *(contact_props_slot++) = contact_props_of(m_contact_data, contact_hit->m_process_contact_id);
                }
                phys_fire_contact_process_cb(m_world, owner_body_a, owner_body_b, event);
            }
        }
    }
    {
        phys_process_hit_point *hit_end = collision_data.get_track_end();
        for (phys_process_hit_point *contact_hit = collision_data.get_first_contact_point(); contact_hit < hit_end; contact_hit++) {
            phys_contact_point &cp = contact_point_of(m_contact_data, contact_hit->m_process_contact_id);
            cp = contact_hit->m_hit_contact;
        }
    }
}


// @address 0x656558 ?add_contact_point_impl@phys_simple_contact_manager@@UAAGABVphys_collision_proxy@@0ABUphys_process_collision_input@@AAUphys_process_collision_output@@PBVphys_simplex_cache@@AAVphys_contact_point@@@Z
unsigned short phys_simple_contact_manager::add_contact_point_impl(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_process_collision_input &input, phys_process_collision_output &output, const phys_simplex_cache *cache, phys_contact_point &point)
{
    int atom_contact_count = m_contact_data.m_contact_atom->m_contact_point_count;
    if (m_reserved_contact_points + atom_contact_count <= 126) {
        phys_rigid_body *listener_entity_a = static_cast<phys_rigid_body *>(agent_body_a.get_root_collider()->get_collider_owner());
        phys_rigid_body *listener_entity_b = static_cast<phys_rigid_body *>(agent_body_b.get_root_collider()->get_collider_owner());
        if (atom_contact_count == 0) {
            output.m_constraint_owner->add_constraint_to_locked_island(&m_contact_constraint);
        }
        phys_contact_point_props *point_props;
        unsigned short id;
        float proj_normal_vel;
        {
            phys_contact_point *dst_contact_point;
            id = m_contact_data.allocate_contact_point(*output.m_constraint_owner, &dst_contact_point, &point_props);
            *dst_contact_point = point;
        }
        {
            phys_vec4 body_vel_a;
            listener_entity_a->get_point_velocity(point.get_contact_position(), body_vel_a);
            phys_vec4 body_vel_b;
            listener_entity_b->get_point_velocity(point.get_contact_position(), body_vel_b);
            phys_vec4 delta_vel;
            delta_vel.set_sub4(body_vel_a, body_vel_b);
            proj_normal_vel = delta_vel.dot3(point.get_contact_normal());
        }
        phys_material &entity_material_a = listener_entity_a->get_body_material();
        phys_material &entity_material_b = listener_entity_b->get_body_material();
        point_props->set_friction_coeff(phys_material::combine_friction(entity_material_a.get_friction(), entity_material_b.get_friction()));
        point_props->set_restitution(phys_material::combine_restitution(entity_material_a.get_restitution(), entity_material_b.get_restitution()));
        *reinterpret_cast<float *>(point_props) = 0.0f;
        phys_manifold_point_added_event event(id, this, &input, &output, &agent_body_a, &agent_body_b, &point, cache,
            point_props ? point_props : 0, proj_normal_vel);
        fire_entity_contact_added(m_world, listener_entity_a, listener_entity_b, event);
        if (event.m_status == 1) {
            remove_contact_point_impl(id, *output.m_constraint_owner);
            return 0xffff;
        }
        m_flags = event.m_next_callback_delay;
        if (!(static_cast<float>(point_props->m_contact_restitution) * 0.0078125f != 0.0f) && listener_entity_a->m_collision_listeners.m_size == 0 &&
            listener_entity_b->m_collision_listeners.m_size == 0)
        {
            float inv_mass_sum = listener_entity_a->get_inv_mass() + listener_entity_b->get_inv_mass();
            float mass = 1.0f / (inv_mass_sum + 1e-10f);
            *reinterpret_cast<float *>(point_props) = -0.2f * mass * proj_normal_vel;
            point_props->m_internal_solver_data = 0.0f;
            point_props->m_internal_data_a = point.m_separating_normal(3);
            return id;
        }
        output.m_constraint_owner->add_callback_request(m_contact_data.m_contact_constraint, 1);
        return id;
    }
    return 0xffff;
}

