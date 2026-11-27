#ifndef SAVED_GAMES_GAME_STATE_XENON_EXTERNS_H
#define SAVED_GAMES_GAME_STATE_XENON_EXTERNS_H

struct s_xenon_game_state_globals;
struct s_game_state_xenon_write_task;
struct s_file_handle;

#include "saved_games/game_state_xenon_globals.h"
extern "C" long async_write_file_callback(s_game_state_xenon_write_task *task);
extern "C" { void cache_files_windows_get_canonical_map_path(const char *path, char *buffer); }
extern "C" { bool cached_map_file_open(s_file_handle *file_handle, const char *path); }

#endif // SAVED_GAMES_GAME_STATE_XENON_EXTERNS_H
