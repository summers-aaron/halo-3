#ifndef INCLUDE_PHYS_WORLD_DELETION_LISTENER_H
#define INCLUDE_PHYS_WORLD_DELETION_LISTENER_H
class phys_world;
class phys_world_deletion_listener {
public:
    virtual ~phys_world_deletion_listener() {}
    virtual void world_deleted_callback(phys_world *world) = 0;
};
#endif
