#ifndef INCLUDE_PHYS_SIMPLE_CONTACT_DATA_H
#define INCLUDE_PHYS_SIMPLE_CONTACT_DATA_H

#include <stddef.h>
#include "phys/phys_constraint_data.h"
#include "phys/phys_contact_point_id_manager.h"

class phys_constraint_query_in;
struct phys_simple_contact_atom;
class phys_contact_point;
class phys_contact_point_props;
struct phys_simple_constraint_init_input;
struct phys_body_vel;

class phys_simple_contact_data : public phys_constraint_data {
public:
    phys_simple_contact_data(phys_constraint *constraint);
    phys_simple_contact_data();
    virtual ~phys_simple_contact_data();
    static __forceinline void operator delete(void *memory) {
        phys_simple_contact_data *data = static_cast<phys_simple_contact_data *>(memory);
        phys_thread_memory *thread = phys_thread_memory::get_instance_ptr();
        ((phys_thread_memory *)thread)->free_chunk((void *)(memory), data->m_mem_size_and_flags, (phys_memory_class)42);
    }
    virtual void get_constraint_info(phys_constraint_data::constraint_info &info) const;
    virtual void get_runtime_info(phys_bool want_runtime, phys_constraint_data::runtime_info &info) const;
    virtual phys_solver_results *get_solver_results(void *runtime);
    virtual phys_bool is_valid_aabb() const;
    virtual int get_type_id() const;
    virtual void collision_response_begin_callback(const phys_contact_point &cp, phys_simple_constraint_init_input &init_input_a,
        phys_body_vel &body_vel_a, phys_simple_constraint_init_input &init_input_b, phys_body_vel &body_vel_b);
    virtual void collision_response_end_callback(const phys_contact_point &cp, float applied_impulse,
        phys_simple_constraint_init_input &init_input_a, phys_body_vel &body_vel_a, phys_simple_constraint_init_input &init_input_b, phys_body_vel &body_vel_b);
    void release_contact_point(phys_constraint_owner &owner, unsigned short id);
    unsigned short allocate_contact_point(phys_constraint_owner &owner, phys_contact_point **point, phys_contact_point_props **properties);

    phys_contact_point_id_manager m_id_manager;
    void *m_visitor_client_data;
    phys_constraint *m_contact_constraint;
    phys_simple_contact_atom *m_contact_atom;
    int m_atom_size;
};

typedef char phys_simple_contact_data_size[(sizeof(void *) != 4 || ((sizeof(phys_simple_contact_data) == 48) )) ? 1 : -1];
typedef char phys_simple_contact_data_id_offset[(sizeof(void *) != 4 || offsetof(phys_simple_contact_data, m_id_manager) == 12) ? 1 : -1];
typedef char phys_simple_contact_data_constraint_offset[(sizeof(void *) != 4 || offsetof(phys_simple_contact_data, m_contact_constraint) == 36) ? 1 : -1];
typedef char phys_simple_contact_data_atom_offset[(sizeof(void *) != 4 || offsetof(phys_simple_contact_data, m_contact_atom) == 40) ? 1 : -1];

void phys_simple_contact_fire_callbacks(phys_simple_contact_data *contact_constraint_data, const phys_constraint_query_in *in,
    phys_simple_contact_atom *atom);

#endif // HAVOK_HK_SIMPLE_CONTACT_CONSTRAINT_DATA_H
