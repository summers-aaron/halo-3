#ifndef INCLUDE_PHYS_CONSTRAINT_CHAIN_H
#define INCLUDE_PHYS_CONSTRAINT_CHAIN_H

#include "phys/phys_constraint.h"
#include "phys/phys_constraint_data.h"
#include "phys/phys_dynamics.h"

class phys_constraint_chain_action;

class phys_chain_data : public phys_constraint_data {
public:
    inline phys_chain_data() {}
    phys_chain_data(phys_fixup_tag flag) : phys_constraint_data(flag) {}
    virtual int get_constraint_info_count() = 0;
};

class phys_constraint_chain : public phys_constraint {
public:
    PHYS_DECLARE_ALLOCATOR(PHYS_MEM_CONSTRAINT);
    phys_constraint_chain(phys_chain_data *data);
    ~phys_constraint_chain();
    inline phys_chain_data *get_chain_data() const { return static_cast<phys_chain_data *>(m_data); }
    virtual void on_entity_removed(phys_entity *entity);
    virtual constraint_instance_kind get_type_id() const { return CONSTRAINT_KIND_CHAIN; }
    void add_chain_entity(phys_entity *entity);
    inline void push_front_entity(phys_entity *entity) {
        m_chained_entities.insert_at(0, entity);
        entity->add_object_reference();
    }
    inline int get_chain_constraint_count() { return phys_math::max_of_two(0, m_chained_entities.getSize() - 1); }
    phys_array<phys_entity *> m_chained_entities;
    phys_constraint_chain_action *m_chain_action;
    phys_constraint_chain(phys_fixup_tag flag) : phys_constraint(flag), m_chained_entities(flag) {}
};

#endif
