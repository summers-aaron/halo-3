#ifndef INCLUDE_PHYS_TOI_RESOURCE_MANAGER_H
#define INCLUDE_PHYS_TOI_RESOURCE_MANAGER_H

#include "phys/phys_object.h"
#include "phys/phys_scalar_types.h"

struct phys_toi_event;
struct phys_toi_resources;
class phys_constraint;
class phys_contact_point;
class phys_contact_point_props;
template <class T> class phys_array;

enum phys_toi_resource_response {
    PHYS_TOI_RESOURCE_MANAGER_RESPONSE_CONTINUE = 0,
    PHYS_TOI_RESOURCE_MANAGER_RESPONSE_NO_EXPAND_CONTINUE = 1,
    PHYS_TOI_RESOURCE_MANAGER_RESPONSE_BACKSTEP = 2
};

// Interface for setting up and freeing time-of-impact solving resources.
class phys_toi_resource_manager : public phys_ref_object {
public:
    struct constraint_violation_info {
        phys_constraint *m_contact_constraint;
        const phys_contact_point *m_contact_point;
        const phys_contact_point_props *m_contact_point_props;
    };
    virtual __forceinline ~phys_toi_resource_manager() {}
    virtual phys_result begin_toi_setup_resources(const phys_toi_event &event,
        const phys_array<phys_toi_event> &other_events, phys_toi_resources &resources_out) = 0;
    // March 0x83593348/0x83593358 supply the base defaults.
    virtual phys_toi_resource_response on_toi_unsolvable(phys_array<constraint_violation_info> &) {
        return PHYS_TOI_RESOURCE_MANAGER_RESPONSE_CONTINUE;
    }
    virtual phys_toi_resource_response on_resources_depleted() {
        return PHYS_TOI_RESOURCE_MANAGER_RESPONSE_NO_EXPAND_CONTINUE;
    }
    virtual void end_toi_and_free_resources(const phys_toi_event &event,
        const phys_array<phys_toi_event> &other_events, const phys_toi_resources &resources) = 0;
};

#endif
