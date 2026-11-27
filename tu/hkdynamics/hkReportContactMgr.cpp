// hkdynamics/hkReportContactMgr.obj: from the records 0x6558f0 0x655928 0x655948 0x655aa0 0x655b60 0x655c28 0x655cc0 0x655d68 0x655d88 0x655ea8 0x655f40 0x655f60
// @flags /TP /O2 /Oc
// @address 0x655aa0 ?add_toi_impl@phys_report_contact_manager@@UAA?AW4toi_verdict@phys_contact_manager@@ABVphys_collision_proxy@@0ABUphys_process_collision_input@@AAUphys_process_collision_output@@MAAVphys_contact_point@@PBVphys_simplex_cache@@AAMAAVphys_contact_point_material@@@Z
// @address 0x655b60 ?remove_toi_impl@phys_report_contact_manager@@UAAXABVphys_collider@@0AAVphys_constraint_owner@@AAVphys_contact_point_material@@@Z
// @address 0x655cc0 ?process_toi@phys_report_contact_manager@@UAAXAAUphys_toi_event@@MAAV?$phys_array@PAVphys_entity@@@@@Z
// @address 0x655d68 ??0report_contact_factory@phys_report_contact_manager@@QAA@PAVphys_world@@@Z
// @address 0x655d88 ?process_contact_impl@phys_report_contact_manager@@UAAXABVphys_collider@@0ABUphys_process_collision_input@@AAUphys_process_collision_data@@@Z
// @address 0x655ea8 ?create_contact_manager@report_contact_factory@phys_report_contact_manager@@UAAPAVphys_contact_manager@@ABVphys_collider@@0ABUphys_collision_input@@@Z
#include "cseries/crt_xdk.h"
#include <vectorintrinsics.h>
#include "phys/phys_dynamics.h"
#include "phys/phys_dynamics_contact_manager.h"
#include "phys/phys_collision_listener.h"
#include "phys/phys_toi_point_added_event.h"
#include "phys/phys_thread_memory.h"
#include "phys/phys_collide.h"
#include "phys/phys_toi_event.h"
#include "phys/phys_world_callback_util.h"
#include "phys/phys_entity_callback_util.h"
#include "phys/phys_manifold_point_added_event.h"
#include "phys/phys_collision_proxy.h"
#include "phys/phys_report_contact_manager.h"
#include "phys/phys_collision_input.h"
#include "phys/phys_constraint_owner.h"
#include "phys/phys_memory_class.h"
#include "phys/phys_simple_constraint_contact_manager.h"
#include "phys/phys_simple_contact_data.h"
#include "phys/phys_statistics_collector.h"
// @address 0x6558f0 ??0phys_report_contact_manager@@QAA@PAVphys_world@@PAVphys_rigid_body@@1@Z
// @address 0x655c28 ?remove_contact_point_impl@phys_report_contact_manager@@UAAXGAAVphys_constraint_owner@@@Z
// @address 0x655f60 ??_Greport_contact_factory@phys_report_contact_manager@@UAAPAXI@Z
// @address 0x655928 bind_report_contact_world
// @address 0x655948 ?add_contact_point_impl@phys_report_contact_manager@@UAAGABVphys_collision_proxy@@0ABUphys_process_collision_input@@AAUphys_process_collision_output@@PBVphys_simplex_cache@@AAVphys_contact_point@@@Z

namespace {
template<class T> static __forceinline T min_report_delay(T a, T b) { return a < b ? a : b; }
static __forceinline phys_entity *get_report_rigid_body(const phys_collider &collidable)
{
    if (collidable.get_type_id() == 1) return (phys_entity *)collidable.get_collider_owner();
    else return 0;
}
static __forceinline void fire_report_point_added(phys_world *world, phys_entity *agent_body_a, phys_entity *agent_body_b, phys_contact_added_event &event)
{
    phys_world_callback_util::fire_entity_contact_added(world, event);
    phys_entity_callback_util::fire_entity_contact_added(agent_body_a, event);
    phys_entity_callback_util::fire_entity_contact_added(agent_body_b, event);
}
static __forceinline void fire_report_point_removed(phys_world *world, phys_entity *agent_body_a, phys_entity *agent_body_b, phys_contact_removed_event &event)
{
    phys_world_callback_util::fire_entity_contact_removed(world, event);
    if (agent_body_a->m_collision_listeners.getSize() != 0) phys_entity_callback_util::fire_entity_contact_removed(agent_body_a, event);
    if (agent_body_b->m_collision_listeners.getSize() != 0) phys_entity_callback_util::fire_entity_contact_removed(agent_body_b, event);
}
static __forceinline void fire_report_process(phys_world *world, phys_entity *agent_body_a, phys_entity *agent_body_b, phys_contact_process_event &event)
{
    phys_world_callback_util::notify_contact_process(world, event);
    phys_entity_callback_util::notify_contact_process(agent_body_a, event);
    phys_entity_callback_util::notify_contact_process(agent_body_b, event);
}
static __forceinline __vector4 cross_vec3(const __vector4 &x, const __vector4 &y)
{
    __vector4 t0 = __vpermwi(x, 99);
    __vector4 t1 = __vpermwi(y, 135);
    __vector4 c0 = __vmulfp(t0, t1);
    t0 = __vpermwi(x, 135);
    t1 = __vpermwi(y, 99);
    __vector4 c1 = __vmulfp(t0, t1);
    return __vsubfp(c0, c1);
}
static __forceinline __vector4 motion_point_vel(const phys_motion *motion, const __vector4 &position)
{
    __vector4 rel_pos = __vsubfp(position, motion->get_center_of_mass_in_world().m_simd_quad);
    __vector4 velocity = cross_vec3(motion->get_angular_velocity().m_simd_quad, rel_pos);
    return __vaddfp(velocity, motion->get_linear_velocity().m_simd_quad);
}

static __forceinline void init_report_delay(phys_report_contact_manager *manager,
    phys_rigid_body *agent_body_a, phys_rigid_body *agent_body_b)
{
    unsigned short delay = agent_body_b->m_contact_callback_delay;
    unsigned short other_delay = agent_body_a->m_contact_callback_delay;
    if (other_delay < delay)
        delay = other_delay;
    manager->m_contact_callback_delay = delay;
}
// The complete constructor is inlined in the factory; the recorded out-of-line
// constructor ends before its shared field-initialization tail at 0x655928.
static __forceinline phys_report_contact_manager *construct_report_manager(phys_report_contact_manager *manager,
    phys_world *world, phys_rigid_body *agent_body_a, phys_rigid_body *agent_body_b)
{
    new (manager) phys_report_contact_manager(phys_fixup_tag());
    init_report_delay(manager, agent_body_a, agent_body_b);
    manager->m_world = world;
    manager->m_body_a = (phys_entity *)agent_body_a;
    manager->m_body_b = (phys_entity *)agent_body_b;
    return manager;
}
static __forceinline phys_report_contact_manager *alloc_report_manager()
{
    phys_thread_memory *memory = (phys_thread_memory *)TlsGetValue(phys_thread_memory::s_threadMemoryInstance.m_slot);
    phys_report_contact_manager *manager =
        (phys_report_contact_manager *)((phys_thread_memory *)memory)->allocate_chunk(sizeof(phys_report_contact_manager),
        (phys_memory_class)32);
    manager->m_mem_size_and_flags = sizeof(phys_report_contact_manager);
    return manager;
}
}
phys_report_contact_manager::phys_report_contact_manager(phys_world *world, phys_rigid_body *agent_body_a, phys_rigid_body *agent_body_b)
    : phys_dynamics_contact_manager(CONTACT_MANAGER_REPORT)
{
    init_report_delay(this, agent_body_a, agent_body_b);
    // match: 0x6558f0: the recorded constructor ends before its shared initialization tail at 0x655928.
    __assume(0);
}

phys_contact_manager::toi_verdict phys_report_contact_manager::add_toi_impl(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_process_collision_input &input, phys_process_collision_output &output, float time,
    phys_contact_point &point, const phys_simplex_cache *cache, float &contact_projected_vel, phys_contact_point_material &material)
{
    material.reset();
    phys_toi_point_added_event event(this, &input, &output, &agent_body_a, &agent_body_b, &point, cache, &material, time, contact_projected_vel);
    fire_report_point_added(m_world, m_body_a, m_body_b, event);
    contact_projected_vel = event.m_projected_vel;
    return static_cast<phys_contact_manager::toi_verdict>(event.m_status);
}

void phys_report_contact_manager::remove_toi_impl(const phys_collider &pair_collider_a, const phys_collider &pair_collider_b,
    phys_constraint_owner &owner, phys_contact_point_material &material)
{
    phys_entity *agent_body_a = get_report_rigid_body(pair_collider_a);
    phys_entity *agent_body_b = get_report_rigid_body(pair_collider_b);
    phys_world *world = agent_body_a->get_action_world();
    phys_contact_removed_event event(0xffff, this, &owner, &material, agent_body_a, agent_body_b);
    fire_report_point_removed(world, agent_body_a, agent_body_b, event);
}


void phys_report_contact_manager::remove_contact_point_impl(unsigned short bounding_contact_point_id, phys_constraint_owner &owner)
{
    phys_entity *agent_body_a = m_body_a;
    phys_entity *agent_body_b = m_body_b;
    phys_world *world = m_world;
    phys_contact_removed_event event(bounding_contact_point_id, this, &owner, 0, agent_body_a, agent_body_b);
    phys_world_callback_util::fire_entity_contact_removed(world, event);
    if (agent_body_a->m_collision_listeners.getSize() != 0)
        phys_entity_callback_util::fire_entity_contact_removed(agent_body_a, event);
    if (agent_body_b->m_collision_listeners.getSize() != 0)
        phys_entity_callback_util::fire_entity_contact_removed(agent_body_b, event);
}


void phys_report_contact_manager::process_toi(phys_toi_event &hit_toi, float rotate_normal, phys_array<phys_entity *> &entities_to_activate)
{
    phys_contact_confirmed_event event(phys_contact_added_event::COLLISION_EVENT_TOI,
        hit_toi.m_island_entities[0]->get_collider(),
        hit_toi.m_island_entities[1]->get_collider(),
        0, &hit_toi.m_contact_point,
        &hit_toi.m_properties,
        rotate_normal, hit_toi.m_separating_vel);
    phys_world *world = hit_toi.m_island_entities[0]->get_action_world();
    phys_world_callback_util::notify_contact_point_confirmed(world, event);
    phys_entity_callback_util::notify_contact_point_confirmed(hit_toi.m_island_entities[0], event);
    phys_entity_callback_util::notify_contact_point_confirmed(hit_toi.m_island_entities[1], event);
    rotate_normal = event.m_rotate_normal;
}

phys_report_contact_manager::report_contact_factory::report_contact_factory(phys_world *world) : m_world(world)
{
}

void phys_report_contact_manager::process_contact_impl(const phys_collider &pair_collider_a, const phys_collider &pair_collider_b,
    const phys_process_collision_input &input, phys_process_collision_data &data)
{
    if (m_contact_callback_delay-- == 0)
    {
        phys_entity *agent_body_a = (phys_entity *)pair_collider_a.get_collider_owner();
        phys_entity *agent_body_b = (phys_entity *)pair_collider_b.get_collider_owner();
        m_contact_callback_delay = min_report_delay(
            agent_body_a->m_contact_callback_delay,
            agent_body_b->m_contact_callback_delay);
        phys_contact_process_event event(this, &pair_collider_a, &pair_collider_b, &data);
        for (int i = data.get_process_contact_count() - 1; i >= 0; --i)
            event.m_contact_point_props[i] = 0;
        fire_report_process(m_world, agent_body_a, agent_body_b, event);
    }
}


phys_contact_manager *phys_report_contact_manager::report_contact_factory::create_contact_manager(const phys_collider &agent_body_a,
    const phys_collider &agent_body_b, const phys_collision_input &input)
{
    phys_rigid_body *contact_rigid_body_a = (phys_rigid_body *)agent_body_a.get_collider_owner();
    phys_rigid_body *contact_rigid_body_b = (phys_rigid_body *)agent_body_b.get_collider_owner();
    phys_report_contact_manager *memory = alloc_report_manager();
    phys_report_contact_manager *manager = construct_report_manager(memory, m_world, contact_rigid_body_a, contact_rigid_body_b);
    return (phys_contact_manager *)manager;
}

unsigned short phys_report_contact_manager::add_contact_point_impl(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_process_collision_input &input, phys_process_collision_output &output, const phys_simplex_cache *cache, phys_contact_point &point)
{
    float proj_velocity[4];
    phys_entity *listener_entity_a = m_body_a;
    phys_entity *listener_entity_b = m_body_b;
    const phys_motion *motion_state_a = listener_entity_a->get_motion();
    const phys_motion *motion_state_b = listener_entity_b->get_motion();
    __vector4 position = point.get_contact_position().m_simd_quad;
    __vector4 contact_velocity_a = motion_point_vel(motion_state_a, position);
    __vector4 velocity_b = motion_point_vel(motion_state_b, position);
    __vector4 rel_contact_vel = __vsubfp(velocity_b, contact_velocity_a);
    __stvewx(__vmsum3fp(rel_contact_vel, point.get_contact_normal().m_simd_quad), (void *)proj_velocity, 0);
    phys_world *world = m_world;
    phys_manifold_point_added_event event(0, this, &input, &output, &agent_body_a, &agent_body_b, &point, cache, 0, proj_velocity[0]);
    fire_report_point_added(world, listener_entity_a, listener_entity_b, event);
    if (event.m_status == 0) {
        m_contact_callback_delay = event.m_next_callback_delay;
        return 0;
    }
    return -1;
}

extern "C" void bind_report_contact_world(phys_report_contact_manager *manager, phys_world *world, phys_entity *agent_body_a, phys_entity *agent_body_b)
{
    manager->m_world = world;
    manager->m_body_a = agent_body_a;
    manager->m_body_b = agent_body_b;
}

// @address 0x655f40 ?cleanup@phys_contact_manager@@UAAXXZ
void phys_contact_manager::cleanup()
{
    delete this;
}

