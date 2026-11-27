#ifndef SAVED_GAMES_AUTOSAVE_QUEUE_VARIANTS_H
#define SAVED_GAMES_AUTOSAVE_QUEUE_VARIANTS_H

#include "saved_games/saved_game_file_types.h"

struct s_autosave_queue_game_variant;
struct s_autosave_queue_string;

extern "C" unsigned __int64 game_variant_compute_unique_id(const s_autosave_queue_game_variant *variant);
extern "C" void game_variant_set_from(s_autosave_queue_game_variant *variant, const s_autosave_queue_game_variant *source);
extern "C" void saved_game_files_get_autosave_path(e_saved_game_file_type file_type, s_autosave_queue_string *path);

#endif // SAVED_GAMES_AUTOSAVE_QUEUE_VARIANTS_H
