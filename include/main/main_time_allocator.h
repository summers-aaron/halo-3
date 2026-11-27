#ifndef MAIN_MAIN_TIME_ALLOCATOR_H
#define MAIN_MAIN_TIME_ALLOCATOR_H

#include "cache/restricted_memory_regions.h"
#include "main/main_time.h"

extern t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), &main_time_globals_post_copy_update,
    &__tls_set_g_main_time_globals_allocator> g_main_time_globals_allocator;

#endif // MAIN_MAIN_TIME_ALLOCATOR_H
