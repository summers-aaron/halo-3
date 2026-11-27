#ifndef INCLUDE_PHYS_REPORT_CONTACT_MANAGER_H
#define INCLUDE_PHYS_REPORT_CONTACT_MANAGER_H

#include "phys/phys_dynamics_contact_manager.h"
#include "phys/phys_contact_manager_factory.h"

class phys_world;
class phys_rigid_body;
class phys_entity;
class phys_collision_proxy;
class phys_collider;
class phys_simplex_cache;
class phys_constraint_owner;
class phys_contact_point;
class phys_contact_point_material;
class phys_statistics_collector;
struct phys_process_collision_input;
struct phys_process_collision_output;
struct phys_process_collision_data;
struct phys_collision_input;
struct phys_toi_event;

// A contact manager that only reports contact points to the collision listeners and adds no constraint
// (tu/hkdynamics/hkReportContactMgr.cpp).
class phys_report_contact_manager : public phys_dynamics_contact_manager
{
public:
    using phys_dynamics_contact_manager::m_world;
    virtual phys_result reserve_contact_points_impl(int) { return PHYS_SUCCESS; }
    virtual Type get_type_id() { return CONTACT_MANAGER_REPORT; }
    phys_report_contact_manager(phys_world *world, phys_rigid_body *agent_body_a, phys_rigid_body *agent_body_b);
    __forceinline phys_report_contact_manager(phys_fixup_tag) : phys_dynamics_contact_manager(CONTACT_MANAGER_REPORT) {}
    virtual phys_contact_manager::toi_verdict add_toi_impl(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
        const phys_process_collision_input &input, phys_process_collision_output &output, float time,
        phys_contact_point &point, const phys_simplex_cache *cache, float &contact_projected_vel, phys_contact_point_material &material);
    virtual unsigned short add_contact_point_impl(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
        const phys_process_collision_input &input, phys_process_collision_output &output, const phys_simplex_cache *cache, phys_contact_point &point);
    virtual void remove_toi_impl(const phys_collider &agent_body_a, const phys_collider &agent_body_b,
        phys_constraint_owner &owner, phys_contact_point_material &material);
    virtual void process_contact_impl(const phys_collider &agent_body_a, const phys_collider &agent_body_b,
        const phys_process_collision_input &input, phys_process_collision_data &data);
    virtual void remove_contact_point_impl(unsigned short bounding_contact_point_id, phys_constraint_owner &owner);
    virtual void process_toi(phys_toi_event &event, float rotate_normal, phys_array<phys_entity *> &entities_to_activate);
    class report_contact_factory : public phys_contact_manager_factory
    {
    public:
        report_contact_factory(phys_world *world);
        virtual __forceinline ~report_contact_factory() {}
        virtual __forceinline void gather_memory_stats(phys_statistics_collector *collector) const {}
        virtual phys_contact_manager *create_contact_manager(const phys_collider &agent_body_a,
            const phys_collider &agent_body_b, const phys_collision_input &input);
        phys_world *m_world;
    };
    phys_entity *m_body_a;
    phys_entity *m_body_b;
    short m_contact_callback_delay;
};

#endif // HAVOK_HK_REPORT_CONTACT_MGR_H
