#ifndef INCLUDE_PHYS_CONVEX_LIST_FILTER_H
#define INCLUDE_PHYS_CONVEX_LIST_FILTER_H

#include "phys/phys_object.h"

class phys_collision_proxy;
struct phys_collision_input;

class phys_convex_list_filter : public phys_ref_object {
public:
    PHYS_DECLARE_ALLOCATOR(PHYS_MEM_COLLISION_INFO);
    enum convex_list_collision_kind {
        CONVEX_LIST_MODE_DEFAULT,
        CONVEX_LIST_MODE_LIST,
        CONVEX_LIST_MODE_CONVEX
    };
    virtual convex_list_collision_kind get_convex_list_mode(const phys_collision_proxy &convex_list_proxy,
        const phys_collision_proxy &other_proxy, const phys_collision_input &input) const = 0;
};

#endif
