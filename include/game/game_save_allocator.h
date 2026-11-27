#ifndef GAME_GAME_SAVE_ALLOCATOR_H
#define GAME_GAME_SAVE_ALLOCATOR_H

#include "cache/restricted_memory_regions.h"

void __tls_set_g_game_save_globals_allocator(void *);
typedef t_restricted_allocation_manager<1,
    static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0),
    __tls_set_g_game_save_globals_allocator> t_game_save_globals_allocator;
extern t_game_save_globals_allocator g_game_save_globals_allocator;

template<>
void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_game_save_globals_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits);

#endif // GAME_GAME_SAVE_ALLOCATOR_H
