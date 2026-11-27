#ifndef INCLUDE_PHYS_ENTITY_LISTENER_H
#define INCLUDE_PHYS_ENTITY_LISTENER_H
#include "phys/phys_dynamics.h"
class phys_entity_listener {
public:
    virtual ~phys_entity_listener() {}
    virtual void on_entity_added(phys_entity *entity) {}
    virtual void on_entity_removed(phys_entity *entity) {}
    virtual void on_entity_shape_set(phys_entity *entity) {
        if (entity->get_action_world()) {
            on_entity_removed(entity);
            on_entity_added(entity);
        }
    }
    virtual void on_entity_deleted(phys_entity *entity) {}
};
#endif
