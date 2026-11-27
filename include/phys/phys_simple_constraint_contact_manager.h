#ifndef INCLUDE_PHYS_SIMPLE_CONSTRAINT_CONTACT_MANAGER_H
#define INCLUDE_PHYS_SIMPLE_CONSTRAINT_CONTACT_MANAGER_H

#include "phys/phys_dynamics_contact_manager.h"
#include "phys/phys_contact_manager_factory.h"
#include "phys/phys_constraint.h"
#include "phys/phys_simple_contact_data.h"
#include "phys/phys_thread_memory.h"

class phys_world;
class phys_rigid_body;
class phys_entity;
class phys_collision_proxy;
class phys_collider;
class phys_simplex_cache;
class phys_constraint_owner;
class phys_contact_point;
class phys_contact_point_props;
class phys_contact_point_material;
class phys_statistics_collector;
struct phys_process_collision_input;
struct phys_process_collision_output;
struct phys_process_collision_data;
struct phys_collision_input;
struct phys_simple_constraint_init_input;
struct phys_body_vel;
struct phys_toi_event;

// The contact manager the world creates by default: it keeps the contact points of a body pair in a simple contact
// constraint (tu/hkdynamics/hkSimpleConstraintContactMgr.cpp).
class phys_simple_contact_manager : public phys_dynamics_contact_manager {
public:
    phys_simple_contact_manager(phys_world *, phys_rigid_body *, phys_rigid_body *);
    virtual ~phys_simple_contact_manager();
    static __forceinline void *operator new(size_t, void *memory) { return memory; }
    static __forceinline void operator delete(void *memory) {
        phys_simple_contact_manager *manager = static_cast<phys_simple_contact_manager *>(memory);
        phys_thread_memory::get_instance_ptr()->free_chunk(memory, manager->m_mem_size_and_flags, PHYS_MEM_CONTACT);
    }
    unsigned short m_reserved_contact_points;
    unsigned short m_flags;
    phys_simple_contact_data m_contact_data;
    phys_constraint m_contact_constraint;
    virtual unsigned short add_contact_point_impl(const phys_collision_proxy &, const phys_collision_proxy &, const phys_process_collision_input &,
        phys_process_collision_output &, const phys_simplex_cache *, phys_contact_point &);
    virtual void remove_contact_point_impl(unsigned short, phys_constraint_owner &);
    virtual void process_contact_impl(const phys_collider &, const phys_collider &, const phys_process_collision_input &, struct phys_process_collision_data &);
    virtual phys_contact_point_props *get_contact_point_props(unsigned short id);
    virtual phys_contact_point *get_process_contact_point(unsigned short id);
    virtual phys_contact_manager::Type get_type_id();
    virtual phys_contact_manager::toi_verdict add_toi_impl(const phys_collision_proxy &, const phys_collision_proxy &, const phys_process_collision_input &,
        phys_process_collision_output &, float, phys_contact_point &, const phys_simplex_cache *, float &, phys_contact_point_material &);
    virtual void remove_toi_impl(const phys_collider &, const phys_collider &, phys_constraint_owner &, phys_contact_point_material &);
    virtual void process_toi(phys_toi_event &, float, phys_array<phys_entity *> &);
    virtual phys_constraint *get_chain_constraint();
    virtual phys_result reserve_contact_points_impl(int contact_reserve_count);
    virtual void toi_response_begin_callback(
        const phys_contact_point &, phys_simple_constraint_init_input &, phys_body_vel &, phys_simple_constraint_init_input &, phys_body_vel &);
    virtual void toi_response_end_callback(const phys_contact_point &, float, phys_simple_constraint_init_input &, phys_body_vel &,
        phys_simple_constraint_init_input &, phys_body_vel &);
    virtual void get_all_contact_point_ids(phys_array<unsigned short> &ids) const;
    class report_contact_factory : public phys_contact_manager_factory {
    public:
        phys_world *m_world;
        report_contact_factory(phys_world *world);
        virtual __forceinline ~report_contact_factory() {}
        virtual phys_contact_manager *create_contact_manager(const phys_collider &, const phys_collider &, const struct phys_collision_input &);
    };

protected:
    virtual void gather_memory_stats(phys_statistics_collector *collector) const;
};

#endif // HAVOK_HK_SIMPLE_CONSTRAINT_CONTACT_MGR_H
