#ifndef INCLUDE_PHYS_NULL_CONTACT_MANAGER_FACTORY_H
#define INCLUDE_PHYS_NULL_CONTACT_MANAGER_FACTORY_H

#include "phys/phys_collide.h"
#include "phys/phys_contact_manager_factory.h"

class phys_null_contact_manager : public phys_contact_manager {
public:
    __forceinline phys_null_contact_manager() : phys_contact_manager(CONTACT_MANAGER_NULL) {}
    virtual __forceinline ~phys_null_contact_manager() {}
    virtual phys_contact_point_id add_contact_point_impl(
        const phys_collision_proxy &, const phys_collision_proxy &, const phys_process_collision_input &,
        phys_process_collision_output &, const phys_simplex_cache *, phys_contact_point &);
    virtual phys_result reserve_contact_points_impl(int);
    virtual void remove_contact_point_impl(phys_contact_point_id, phys_constraint_owner &);
    virtual void process_contact_impl(
        const phys_collider &, const phys_collider &, const phys_process_collision_input &, phys_process_collision_data &);
    virtual toi_verdict add_toi_impl(
        const phys_collision_proxy &, const phys_collision_proxy &, const phys_process_collision_input &, phys_process_collision_output &,
        float, phys_contact_point &, const phys_simplex_cache *, float &, phys_contact_point_material &);
    virtual void remove_toi_impl(const phys_collider &, const phys_collider &, phys_constraint_owner &, phys_contact_point_material &);
    virtual void cleanup();
};

class phys_null_contact_manager_factory : public phys_contact_manager_factory {
public:
    virtual phys_contact_manager *create_contact_manager(const phys_collider &a, const phys_collider &b, const phys_collision_input &input);
    phys_null_contact_manager m_contact_manager;
    __forceinline phys_null_contact_manager_factory() {}
};

#endif
