#ifndef GAME_GAME_ENGINE_TRAITS_EXTERNS_H
#define GAME_GAME_ENGINE_TRAITS_EXTERNS_H

#include "game/game_types.h"
#include "saved_games/saved_game_file_types.h"

class c_bitstream;
#include "saved_games/saved_game_item_metadata.h"

extern "C" void saved_game_files_initialize_metadata(s_saved_game_item_metadata_record *out, unsigned int index, int value,
    const wchar_t *name, const wchar_t *description, unsigned __int64 identifier, unsigned __int64 size_in_bytes);
e_saved_game_file_type game_engine_index_to_saved_game_file_type(e_game_engine_type engine_type);
extern "C" void saved_game_files_decode_metadata(void *metadata, c_bitstream *stream);
extern "C" const short k_game_engine_zone_variant_times[];

#endif // GAME_GAME_ENGINE_TRAITS_EXTERNS_H
