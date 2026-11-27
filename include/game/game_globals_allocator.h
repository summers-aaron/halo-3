#ifndef GAME_GAME_GLOBALS_ALLOCATOR_H
#define GAME_GAME_GLOBALS_ALLOCATOR_H

#include "cache/restricted_memory_regions.h"
#include "game/game.h"

extern t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    __tls_set_g_game_globals_allocator> g_game_globals_allocator;

template<>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    __tls_set_g_game_globals_allocator>::reserve_memory(const char *, const char *, unsigned int, long);

#endif // GAME_GAME_GLOBALS_ALLOCATOR_H
