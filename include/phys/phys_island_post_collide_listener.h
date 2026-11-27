#ifndef INCLUDE_PHYS_ISLAND_POST_COLLIDE_LISTENER_H
#define INCLUDE_PHYS_ISLAND_POST_COLLIDE_LISTENER_H
class phys_island;
class phys_step_info;
class phys_island_post_collide_listener {
public:
    virtual ~phys_island_post_collide_listener() {}
    virtual void post_collide_callback(phys_island *island, const phys_step_info &step_info) = 0;
};
#endif
