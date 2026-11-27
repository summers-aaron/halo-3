#ifndef SOUND_GAME_SOUND_PLAYER_EFFECTS_H
#define SOUND_GAME_SOUND_PLAYER_EFFECTS_H

// Functions defined in tu/blamlibxbox/game_sound_player_effects.cpp that other units call.

void game_sound_player_effects_update(float elapsed);

// Named allocator objects owned by this unit.
template<long, void (*)(void *), void (*)(void *), void (*)(void *)>
class t_restricted_allocation_manager;
void __tls_set_g_player_effects_globals_allocator(void *allocation);
extern t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_player_effects_globals_allocator> g_player_effects_globals_allocator;

typedef t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_player_effects_globals_allocator> t_player_effects_globals_allocator;

void game_sound_player_effects_initialize(void);
void game_sound_player_effects_dispose(void);
void game_sound_player_effects_initialize_for_new_map(void);
void game_sound_player_effects_dispose_from_old_map(void);

#endif // SOUND_GAME_SOUND_PLAYER_EFFECTS_H
