#ifndef SAVED_GAMES_SAVED_GAME_FILES_BLF_VARIANT_H
#define SAVED_GAMES_SAVED_GAME_FILES_BLF_VARIANT_H

#ifdef HALO3_64BIT
#include <stddef.h>

class c_game_variant;

struct s_blf_game_variant_payload
{
    long engine_type;
    unsigned long reserved_vtable_slot;
    char engine_data[0x25c];
};

static_assert(sizeof(s_blf_game_variant_payload) == 0x264, "mpvr payload size");
static_assert(offsetof(s_blf_game_variant_payload, engine_data) == 8, "mpvr engine data offset");

void game_variant_encode_blf(void *destination, const c_game_variant *variant);
#endif

#endif
