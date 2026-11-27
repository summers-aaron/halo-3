#ifndef INCLUDE_PHYS_VOLUME_LISTENER_H
#define INCLUDE_PHYS_VOLUME_LISTENER_H
#include "phys/phys_dynamics.h"
class phys_volume_listener {
public:
    virtual ~phys_volume_listener() {}
    virtual void volume_added_callback(phys_volume *phantom) {}
    virtual void volume_removed_callback(phys_volume *phantom) {}
    virtual void volume_shape_set_callback(phys_volume *phantom) {
        if (phantom->get_action_world()) {
            volume_removed_callback(phantom);
            volume_added_callback(phantom);
        }
    }
    virtual void volume_deleted_callback(phys_volume *phantom) {}
};
#endif
