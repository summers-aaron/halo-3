#ifndef SOUND_GAME_SOUND_ALLOCATORS_H
#define SOUND_GAME_SOUND_ALLOCATORS_H
#include "cache/restricted_memory_regions.h"
#include "sound/game_sound.h"
template<>
void *t_restricted_allocation_manager<1,
    static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_game_sound_globals_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits);
template<>
void *t_restricted_allocation_manager<1,
    static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_game_sound_scripted_impulses_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits);

#endif
