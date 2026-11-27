#ifndef GAME_GAME_TIME_ALLOCATOR_H
#define GAME_GAME_TIME_ALLOCATOR_H

#include "cache/restricted_memory_regions.h"
#include "game/game_time.h"

typedef t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_game_time_globals_allocator> game_time_globals_allocator_type;
extern game_time_globals_allocator_type g_game_time_globals_allocator;

#endif // GAME_GAME_TIME_ALLOCATOR_H
