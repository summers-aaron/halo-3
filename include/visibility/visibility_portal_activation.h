#ifndef VISIBILITY_VISIBILITY_PORTAL_ACTIVATION_H
#define VISIBILITY_VISIBILITY_PORTAL_ACTIVATION_H

// Functions defined in tu/blamlibxbox/visibility_portal_activation.cpp that other units call.

void __tls_set_g_active_portal_bitvectors_allocator(void *allocator);
void portal_activation_clear_portals(void);

#include "cache/restricted_memory_regions.h"

extern t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0), &__tls_set_g_active_portal_bitvectors_allocator> g_active_portal_bitvectors_allocator;

void portal_activation_initialize(void);
void portal_activation_dispose(void);
void portal_activation_initialize_for_new_map(void);
void portal_activation_dispose_from_old_map(void);
void portal_activation_dispose_from_old_structure_bsp(unsigned long structure_bsp_mask);

#endif // VISIBILITY_VISIBILITY_PORTAL_ACTIVATION_H
