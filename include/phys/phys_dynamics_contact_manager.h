#ifndef INCLUDE_PHYS_DYNAMICS_CONTACT_MANAGER_H
#define INCLUDE_PHYS_DYNAMICS_CONTACT_MANAGER_H

#include "phys/phys_collide.h"

class phys_world;
class phys_entity;
class phys_constraint;
class phys_contact_point_props;
struct phys_simple_constraint_init_input;
struct phys_body_vel;
struct phys_toi_event;

class phys_dynamics_contact_manager : public phys_contact_manager {
public:
    __forceinline phys_dynamics_contact_manager(Type type) : phys_contact_manager(type) {}
    virtual __forceinline ~phys_dynamics_contact_manager() {}
    virtual phys_contact_point_props *get_contact_point_props(unsigned short id) { return 0; }
    virtual phys_contact_point *get_process_contact_point(unsigned short id) { return 0; }
    virtual void get_all_contact_point_ids(phys_array<unsigned short> &ids) const {}
    virtual Type get_type_id() = 0;
    virtual void toi_response_begin_callback(const phys_contact_point &point,
        phys_simple_constraint_init_input &body_input_a, phys_body_vel &contact_velocity_a,
        phys_simple_constraint_init_input &body_input_b, phys_body_vel &velocity_b) {}
    virtual void toi_response_end_callback(const phys_contact_point &point, float impulse,
        phys_simple_constraint_init_input &body_input_a, phys_body_vel &contact_velocity_a,
        phys_simple_constraint_init_input &body_input_b, phys_body_vel &velocity_b) {}
    virtual phys_constraint *get_chain_constraint() { return 0; }
    virtual void process_toi(phys_toi_event &event, float rotate_normal, phys_array<phys_entity *> &entities_to_activate) {}

protected:
    phys_world *m_world;
};

typedef char phys_dynamics_contact_manager_size[(sizeof(void *) != 4 || ((sizeof(phys_dynamics_contact_manager) == 16) )) ? 1 : -1];

#endif
