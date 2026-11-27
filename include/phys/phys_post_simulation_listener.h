#ifndef INCLUDE_PHYS_POST_SIMULATION_LISTENER_H
#define INCLUDE_PHYS_POST_SIMULATION_LISTENER_H
class phys_world;
class phys_entity;
class phys_post_simulation_listener {
public:
    virtual ~phys_post_simulation_listener() {}
    virtual void post_simulation_callback(phys_world *world) = 0;
    virtual void on_inactive_entity_moved(phys_entity *entity) {}
};
#endif
