#ifndef MATH_RANDOM_SEED_ALLOCATOR_H
#define MATH_RANDOM_SEED_ALLOCATOR_H

#include "cache/restricted_memory_regions.h"

void __tls_set_g_deterministic_random_seed_ptr_allocator(void *allocator);
typedef t_restricted_allocation_manager<1,
	static_cast<void (*)(void *)>(0),
	static_cast<void (*)(void *)>(0),
	&__tls_set_g_deterministic_random_seed_ptr_allocator> t_deterministic_random_seed_allocation_manager;
extern t_deterministic_random_seed_allocation_manager g_deterministic_random_seed_ptr_allocator;

#endif // MATH_RANDOM_SEED_ALLOCATOR_H
