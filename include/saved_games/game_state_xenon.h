#ifndef SAVED_GAMES_GAME_STATE_XENON_H
#define SAVED_GAMES_GAME_STATE_XENON_H

// Functions defined in tu/blamlibxbox/game_state_xenon.cpp.

void game_state_create_or_open_file(void);
extern "C" bool game_state_xenon_open_file(long file_index, const char *path);
extern "C" char *game_state_xenon_try_and_get_data(const char *path);
extern "C" long game_state_xenon_get_file_version(const char *path);

struct s_xenon_game_state_globals;
struct s_cache_file_header;

extern "C" s_xenon_game_state_globals *game_state_xenon_get_globals(void);
extern "C" bool game_state_xenon_is_file_valid(long file_index);
extern "C" bool game_state_read_from_file(long file_index);
extern "C" bool game_state_xenon_compare_file_times(const s_cache_file_header *snapshot, unsigned long *mismatch_mask);
extern "C" long game_state_xenon_find_file_by_path(const char *path);
extern "C" void game_state_xenon_initialize_globals(void);

#endif // SAVED_GAMES_GAME_STATE_XENON_H
