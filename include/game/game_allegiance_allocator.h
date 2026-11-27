#ifndef GAME_GAME_ALLEGIANCE_ALLOCATOR_H
#define GAME_GAME_ALLEGIANCE_ALLOCATOR_H

#include "cache/restricted_memory_regions.h"
#include "game/game_allegiance.h"

// The allegiance globals allocator's members, specialized in tu/blamlibxbox/game_allegiance.cpp.
template <>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_game_allegiance_globals_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits);

#endif // GAME_GAME_ALLEGIANCE_ALLOCATOR_H
