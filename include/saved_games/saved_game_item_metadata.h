#ifndef SAVED_GAMES_SAVED_GAME_ITEM_METADATA_H
#define SAVED_GAMES_SAVED_GAME_ITEM_METADATA_H

#include "saved_games/saved_game_file_types.h"
#include "saved_games/saved_game_item_metadata_packed.h"
#include "shell/shell.h"
#include <stddef.h>

#include "saved_games/metadata_file_type.h"
template<long k_maximum_count> class c_static_wchar_string;

struct s_saved_game_item_metadata : s_saved_game_item_metadata_packed
{
    void initialize(e_controller_index controller_index, e_saved_game_file_type file_type,
        const wchar_t *name, const wchar_t *description, unsigned __int64 size_in_bytes);
    static e_metadata_file_type get_file_type_from_saved_game_type(e_saved_game_file_type file_type);
    e_metadata_file_type get_metadata_file_type() const;
    bool get_metadata_text(long string_id, c_static_wchar_string<1024> *string) const;
};

typedef s_saved_game_item_metadata s_saved_game_item_metadata_record;

typedef char saved_game_metadata_size[(sizeof(s_saved_game_item_metadata) == 0xf8) ? 1 : -1];
typedef char saved_game_metadata_description_offset[(offsetof(s_saved_game_item_metadata, description) == 0x28) ? 1 : -1];
typedef char saved_game_metadata_author_offset[(offsetof(s_saved_game_item_metadata, author_id) == 0xc0) ? 1 : -1];
typedef char saved_game_metadata_game_id_offset[(offsetof(s_saved_game_item_metadata, game_id) == 0xf0) ? 1 : -1];

#endif // SAVED_GAMES_SAVED_GAME_ITEM_METADATA_H
