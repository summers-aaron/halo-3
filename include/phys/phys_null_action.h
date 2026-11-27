#ifndef INCLUDE_PHYS_NULL_ACTION_H
#define INCLUDE_PHYS_NULL_ACTION_H

#include "phys/phys_action.h"

class phys_null_action : public phys_action {
public:
    PHYS_DECLARE_ALLOCATOR(PHYS_MEM_ACTION);
    inline phys_null_action() : phys_action(0) {}
    virtual void apply_action(const phys_step_info &step_info) {}
    virtual void get_entities(phys_array<phys_entity *> &entities) {}
    virtual void on_entity_removed(phys_entity *entity) {}
    virtual phys_action *clone(const phys_array<phys_entity *> &entities, const phys_array<phys_volume *> &phantoms) const { return 0; }
    static inline phys_null_action *PHYS_CALL get_null_action() { return 0; }
};

#endif
