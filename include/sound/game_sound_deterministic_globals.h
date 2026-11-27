#ifndef SOUND_GAME_SOUND_DETERMINISTIC_GLOBALS_H
#define SOUND_GAME_SOUND_DETERMINISTIC_GLOBALS_H

#include "cache/restricted_memory_regions.h"

struct s_game_sound_deterministic_globals { unsigned long permutation_flags[0x1300 / 4]; };
void __tls_set_g_future_development_allocator(void *);
typedef t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    __tls_set_g_future_development_allocator> t_game_sound_deterministic_allocator;
extern t_game_sound_deterministic_allocator g_future_development_allocator;
extern "C" {
extern __declspec(thread) s_game_sound_deterministic_globals *g_game_sound_deterministic_multiplayer_globals;
extern const char k_game_sound_deterministic_allocation_name[];
extern s_game_sound_deterministic_globals *g_game_sound_deterministic_globals;
extern s_game_sound_deterministic_globals *g_game_sound_deterministic_non_multiplayer_globals;
}

template<>
void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    __tls_set_g_future_development_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits);

#endif // SOUND_GAME_SOUND_DETERMINISTIC_GLOBALS_H
