#include "tag_files/files.h"
#ifndef SAVED_GAMES_SAVED_GAME_FILES_GLOBALS_H
#define SAVED_GAMES_SAVED_GAME_FILES_GLOBALS_H
#include "main/game_preferences_types.h"

#include "saved_games/saved_game_files_context_types.h"
#include "saved_games/game_state_lock.h"
#include "interface/game_tag_parser.h"
#include "interface/user_interface_error_manager.h"

struct s_saved_game_files_globals
{
    long flags;
    s_saved_game_files_context context;
    c_magic_string_game_tag_parser parsers[4];
};

extern "C" s_saved_game_files_globals g_saved_game_files_globals;

extern "C" signed char g_saved_game_files_pending_dirty;
extern "C" long g_saved_game_files_name_counters[];
extern "C" char g_global_preferences_storage[];
extern "C" s_game_state_shared_lock g_game_state_shared_lock;
extern "C" long g_marketplace_active_controller_count;

#include "saved_games/saved_film_storage.h"

#endif // SAVED_GAMES_SAVED_GAME_FILES_GLOBALS_H
