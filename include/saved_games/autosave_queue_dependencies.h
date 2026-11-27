#ifndef SAVED_GAMES_AUTOSAVE_QUEUE_DEPENDENCIES_H
#define SAVED_GAMES_AUTOSAVE_QUEUE_DEPENDENCIES_H

class c_game_variant;
struct s_autosave_queue_string;
enum e_saved_game_file_type;

extern "C" unsigned __int64 game_variant_compute_unique_id(const c_game_variant *variant);
extern "C" void saved_game_files_get_autosave_path(e_saved_game_file_type file_type, s_autosave_queue_string *path);

#include "saved_games/content_catalogue_iteration.h"

#endif // SAVED_GAMES_AUTOSAVE_QUEUE_DEPENDENCIES_H
