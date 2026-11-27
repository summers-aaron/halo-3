#ifndef SOUND_GAME_SOUND_DETERMINISTIC_H
#define SOUND_GAME_SOUND_DETERMINISTIC_H

// Functions defined in tu/blamlibxbox/game_sound_deterministic.cpp.

struct s_game_sound_deterministic_permutation;

void game_sound_deterministic_pick_permutation(long sound_index, s_game_sound_deterministic_permutation *permutation, bool *out_exhausted);
float game_sound_deterministic_get_permutation_playing_time(long sound_index, const s_game_sound_deterministic_permutation *permutation);
bool game_sound_deterministic_permutation_valid(long sound_index, const s_game_sound_deterministic_permutation *permutation);
extern "C" void game_sound_deterministic_set_region_address(void *address);

void game_sound_deterministic_initialize(void);
extern "C" void game_sound_deterministic_dispose(void);
void game_sound_deterministic_initialize_for_new_map();
void game_sound_deterministic_dispose_from_old_map();

#endif // SOUND_GAME_SOUND_DETERMINISTIC_H
