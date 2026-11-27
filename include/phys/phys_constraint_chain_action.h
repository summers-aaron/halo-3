#ifndef INCLUDE_PHYS_CONSTRAINT_CHAIN_ACTION_H
#define INCLUDE_PHYS_CONSTRAINT_CHAIN_ACTION_H

#include "phys/phys_action.h"

class phys_constraint_chain;

class phys_constraint_chain_action : public phys_action {
protected:
    friend class phys_constraint_chain;
    phys_constraint_chain_action(phys_constraint_chain *chain) : phys_action(0), m_query_constraint(chain) {}
public:
    virtual ~phys_constraint_chain_action() {}
    virtual void apply_action(const phys_step_info &step_info);
    virtual phys_action *clone(const phys_array<phys_entity *> &entities, const phys_array<phys_volume *> &phantoms) const;
    virtual void on_entity_removed(phys_entity *entity);
    inline phys_constraint_chain *get_chain_constraint() { return m_query_constraint; }
    virtual void get_entities(phys_array<phys_entity *> &entities);
    phys_constraint_chain *m_query_constraint;
    phys_constraint_chain_action(phys_fixup_tag flag) : phys_action(flag) {}
};

typedef char phys_constraint_chain_action_size[(sizeof(void *) != 4 || ((sizeof(phys_constraint_chain_action) == 28) )) ? 1 : -1];

#endif
