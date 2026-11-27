#ifndef GAME_GAME_ENGINE_SETTINGS_DEFINITIONS_H
#define GAME_GAME_ENGINE_SETTINGS_DEFINITIONS_H

#include <stddef.h>

#include "tag_files/tag_types.h"
#include "game/game_engine_player_traits.h"
#include "game/universal_types.h"

// The settings tag: the player-traits block precedes the ten variant blocks.
struct s_game_engine_settings_definition
{
    long flags;
    s_tag_block player_traits;
    s_tag_block slayer;
    s_tag_block oddball;
    s_tag_block ctf;
    s_tag_block king;
    s_tag_block vip;
    s_tag_block juggernaut;
    s_tag_block territories;
    s_tag_block assault;
    s_tag_block infection;
    s_tag_block sandbox;
};

struct s_multiplayer_globals_definition
{
    s_tag_block universal;
    s_tag_block runtime;
};

typedef char game_engine_settings_size[(sizeof(s_game_engine_settings_definition) == 0x88) ? 1 : -1];
typedef char game_engine_settings_off_slayer[(offsetof(s_game_engine_settings_definition, slayer) == 0x10) ? 1 : -1];
typedef char multiplayer_universal_globals_size[(sizeof(s_multiplayer_universal_globals_definition) == 0xb4) ? 1 : -1];
typedef char multiplayer_universal_off_sandbox_properties[(offsetof(s_multiplayer_universal_globals_definition, sandbox_object_properties) == 0x58) ? 1 : -1];
typedef char multiplayer_universal_off_settings[(offsetof(s_multiplayer_universal_globals_definition, game_engine_settings) == 0xa4) ? 1 : -1];
typedef char multiplayer_universal_off_characters[(offsetof(s_multiplayer_universal_globals_definition, customized_characters) == 0x2c) ? 1 : -1];
typedef char multiplayer_globals_off_runtime[(offsetof(s_multiplayer_globals_definition, runtime) == 0xc) ? 1 : -1];
typedef char multiplayer_globals_size[(sizeof(s_multiplayer_globals_definition) == 0x18) ? 1 : -1];

#endif // GAME_GAME_ENGINE_SETTINGS_DEFINITIONS_H
