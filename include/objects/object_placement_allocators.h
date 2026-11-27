#ifndef OBJECTS_OBJECT_PLACEMENT_ALLOCATORS_H
#define OBJECTS_OBJECT_PLACEMENT_ALLOCATORS_H

#include "cache/restricted_memory_regions.h"

void __tls_set_g_object_activation_regions_allocator(void *address);
void __tls_set_g_object_placement_globals_allocator(void *address);

typedef t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
	&__tls_set_g_object_activation_regions_allocator> c_object_activation_regions_allocation_manager;
extern c_object_activation_regions_allocation_manager g_object_activation_regions_allocator;
typedef t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
	&__tls_set_g_object_placement_globals_allocator> c_object_placement_globals_allocation_manager;
extern c_object_placement_globals_allocation_manager g_object_placement_globals_allocator;

#endif // OBJECTS_OBJECT_PLACEMENT_ALLOCATORS_H
