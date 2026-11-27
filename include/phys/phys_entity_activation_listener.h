#ifndef INCLUDE_PHYS_ENTITY_ACTIVATION_LISTENER_H
#define INCLUDE_PHYS_ENTITY_ACTIVATION_LISTENER_H
class phys_entity;
class phys_entity_activation_listener {
public:
    virtual ~phys_entity_activation_listener();
    virtual void on_entity_deactivated(phys_entity *entity) = 0;
    virtual void on_entity_activated(phys_entity *entity) = 0;
};
#endif
