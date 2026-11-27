#ifndef INCLUDE_PHYS_VOLUME_OVERLAP_LISTENER_H
#define INCLUDE_PHYS_VOLUME_OVERLAP_LISTENER_H

#include "phys/phys_base_types.h"

class phys_volume;
class phys_collider;

enum phys_collider_accept { PHYS_COLLIDER_ACCEPT = 0, PHYS_COLLIDER_REJECT = 1 };
struct phys_collider_added_event {
    const phys_volume *m_overlap_volume;
    const phys_collider *m_collider;
    mutable phys_collider_accept m_collider_accept;
};
struct phys_collider_removed_event {
    const phys_volume *m_overlap_volume;
    const phys_collider *m_collider;
    phys_bool m_collider_was_added;
};
class phys_volume_overlap_listener {
public:
    virtual void collider_added_callback(const phys_collider_added_event &event) = 0;
    virtual void collider_removed_callback(const phys_collider_removed_event &event) = 0;
    virtual ~phys_volume_overlap_listener() {}
};

typedef char phys_collider_added_event_size[(sizeof(void *) != 4 || ((sizeof(phys_collider_added_event) == 12) )) ? 1 : -1];
typedef char phys_collider_removed_event_size[(sizeof(void *) != 4 || ((sizeof(phys_collider_removed_event) == 12) )) ? 1 : -1];

#endif
