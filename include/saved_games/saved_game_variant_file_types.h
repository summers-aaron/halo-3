#ifndef SAVED_GAMES_SAVED_GAME_VARIANT_FILE_TYPES_H
#define SAVED_GAMES_SAVED_GAME_VARIANT_FILE_TYPES_H

#include "saved_games/saved_film_recording_types.h"
#include "networking/tools/blf_end_of_file_types.h"
#include "game/game_variant.h"
#include "game/map_variant.h"

struct s_blf_chunk_game_variant
{
    s_blf_header header;
    c_game_variant game_variant;
    s_blf_chunk_game_variant();
};

struct s_blffile_game_variant : s_blffile_saved_game_file
{
    s_blf_chunk_game_variant game_variant;
    s_blf_chunk_end_of_file end_of_file;
    s_blffile_game_variant();
    bool copy_to_and_validate(c_game_variant *variant, bool *valid) const;
};

struct s_blf_chunk_map_variant
{
    s_blf_header header;
    c_map_variant map_variant;
    s_blf_chunk_map_variant();
};

#pragma pack(push, 4)
// Payload of a 'mapv' chunk as returned by network_blf_read_for_known_chunk:
// the payload starts after the chunk header, keeping the alignment word that
// precedes the map variant.
struct s_blf_chunk_map_variant_payload
{
    long reserved_alignment;
    c_map_variant map_variant;
};
#pragma pack(pop)

struct s_blffile_map_variant : s_blffile_saved_game_file
{
    s_blf_chunk_map_variant map_variant;
    s_blf_chunk_end_of_file end_of_file;
    s_blffile_map_variant();
    bool copy_to_and_validate(c_map_variant *variant, bool *valid) const;
};

typedef char blffile_game_variant_size[(sizeof(s_blffile_game_variant) == 0x3bc) ? 1 : -1];
typedef char blffile_game_variant_offset[(offsetof(s_blffile_game_variant, game_variant) == 0x138) ? 1 : -1];
typedef char blffile_map_variant_size[(sizeof(s_blffile_map_variant) == 0xe1f0) ? 1 : -1];
typedef char blffile_map_variant_offset[(offsetof(s_blffile_map_variant, map_variant) == 0x138) ? 1 : -1];
typedef char blf_map_variant_payload_offset[(offsetof(s_blf_chunk_map_variant, map_variant) == 0x10) ? 1 : -1];
typedef char blf_map_variant_payload_map_offset[(offsetof(s_blf_chunk_map_variant_payload, map_variant) == 4) ? 1 : -1];

#endif
