#ifndef INCLUDE_PHYS_DEFAULT_CONVEX_LIST_FILTER_H
#define INCLUDE_PHYS_DEFAULT_CONVEX_LIST_FILTER_H

#include "phys/phys_convex_list_filter.h"

class phys_default_convex_list_filter : public phys_convex_list_filter {
public:
    phys_default_convex_list_filter() {}
    phys_default_convex_list_filter(phys_fixup_tag) {}
    virtual __forceinline ~phys_default_convex_list_filter() {}
    virtual convex_list_collision_kind get_convex_list_mode(const phys_collision_proxy &agent_body_a,
        const phys_collision_proxy &agent_body_b, const phys_collision_input &input) const;
};

#endif
