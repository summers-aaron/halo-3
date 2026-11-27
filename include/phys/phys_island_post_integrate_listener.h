#ifndef INCLUDE_PHYS_ISLAND_POST_INTEGRATE_LISTENER_H
#define INCLUDE_PHYS_ISLAND_POST_INTEGRATE_LISTENER_H
class phys_island;
class phys_step_info;
class phys_island_post_integrate_listener {
public:
    virtual ~phys_island_post_integrate_listener() {}
    virtual void post_integrate_callback(phys_island *island, const phys_step_info &step_info) = 0;
};
#endif
