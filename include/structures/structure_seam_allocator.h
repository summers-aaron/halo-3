#ifndef STRUCTURES_STRUCTURE_SEAM_ALLOCATOR_H
#define STRUCTURES_STRUCTURE_SEAM_ALLOCATOR_H

#include "cache/restricted_memory_regions.h"

void __tls_set_g_structure_seam_globals_allocator(void *allocator);
typedef t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_structure_seam_globals_allocator> c_structure_seam_globals_allocator;
extern c_structure_seam_globals_allocator g_structure_seam_globals_allocator;

#endif // STRUCTURES_STRUCTURE_SEAM_ALLOCATOR_H
