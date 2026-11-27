#ifndef SAVED_GAMES_SAVED_GAME_ITEM_METADATA_PACKED_H
#define SAVED_GAMES_SAVED_GAME_ITEM_METADATA_PACKED_H

#include <stddef.h>

// On-disk metadata passed by saved_game_files.cpp and online_files.cpp.
// Keep its distinct type name: it appears in saved-game function signatures.
struct s_saved_game_item_metadata_packed
{
    unsigned __int64 unique_id;
    wchar_t name[16];
    char description[128];
    char author[16];
    long file_type;
    bool author_is_online;
    char author_flag_zero_fill[3]; // Required zero bytes in stored metadata.
    unsigned __int64 author_id;
    unsigned __int64 size_in_bytes;
    unsigned __int64 date;
    long length_seconds;
    long campaign_id;
    long map_id;
    long game_engine_type;
    long campaign_difficulty;
    unsigned short campaign_insertion_point;
    short campaign_survival_enabled;
    unsigned __int64 game_id;
};

typedef char saved_game_metadata_packed_size[(sizeof(s_saved_game_item_metadata_packed) == 0xf8) ? 1 : -1];
typedef char saved_game_metadata_packed_size_offset[(offsetof(s_saved_game_item_metadata_packed, size_in_bytes) == 0xc8) ? 1 : -1];

#endif // SAVED_GAMES_SAVED_GAME_ITEM_METADATA_PACKED_H
