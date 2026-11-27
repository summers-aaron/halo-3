#ifndef INCLUDE_PHYS_POST_COLLIDE_LISTENER_H
#define INCLUDE_PHYS_POST_COLLIDE_LISTENER_H
class phys_world;
class phys_step_info;
class phys_post_collide_listener {
public:
    virtual ~phys_post_collide_listener() {}
    virtual void post_collide_callback(phys_world *world, const phys_step_info &step_info) = 0;
};
#endif
