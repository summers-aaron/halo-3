#ifndef SAVED_GAMES_SAVED_GAME_FILES_METADATA_H
#define SAVED_GAMES_SAVED_GAME_FILES_METADATA_H

#include "saved_games/saved_game_item_metadata.h"

extern "C" void saved_game_files_initialize_metadata_from_game(s_saved_game_item_metadata_record *out, unsigned int controller_index, int file_type,
    const wchar_t *name, const wchar_t *description, unsigned __int64 size_in_bytes);

#endif
