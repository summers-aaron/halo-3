#ifndef GAME_PLAYER_RUMBLE_H
#define GAME_PLAYER_RUMBLE_H

// Functions defined in tu/blamlibxbox/player_rumble.cpp.

void rumble_player_impulse(long player_index, long rumble_index, long rumble_type, float intensity);
void rumble_clear_all_now(void);
void rumble_update(float elapsed_seconds);
extern "C" void __tls_set_g_rumble_globals_allocator(void *value);
extern "C" void rumble_register_procs(void);

class c_rumble_globals_allocator;
extern c_rumble_globals_allocator g_rumble_globals_allocator;

void rumble_initialize(void);
void rumble_dispose(void);
void rumble_initialize_for_new_map(void);
void rumble_dispose_from_old_map(void);

#endif // GAME_PLAYER_RUMBLE_H
