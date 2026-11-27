#ifndef GAME_MULTIPLAYER_RUNTIME_GLOBALS_H
#define GAME_MULTIPLAYER_RUNTIME_GLOBALS_H

#include "tag_files/tag_types.h"
#include <stddef.h>

// Halo 3 original runtime tag layout; grenade limits are read at +0x10c/+0x110.
// Field sequence corroborated by TagTool MultiplayerGlobals.MultiplayerRuntimeBlock.
struct s_multiplayer_runtime_globals_definition
{
    s_tag_reference editor_biped;
    s_tag_reference editor_helper_object;
    s_tag_reference flag;
    s_tag_reference ball;
    s_tag_reference bomb;
    s_tag_reference vip_influence_area;
    s_tag_reference in_game_strings;
    s_tag_block sounds;
    s_tag_block looping_sounds;
    s_tag_block general_events;
    s_tag_block flavor_events;
    s_tag_block slayer_events;
    s_tag_block ctf_events;
    s_tag_block oddball_events;
    s_tag_block king_events;
    s_tag_block vip_events;
    s_tag_block juggernaut_events;
    s_tag_block territories_events;
    s_tag_block assault_events;
    s_tag_block infection_events;
    long maximum_fragmentation_grenades;
    long maximum_plasma_grenades;
    s_tag_block multiplayer_constants;
    s_tag_block state_responses;
    s_tag_reference scoreboard_emblem_bitmap;
    s_tag_reference scoreboard_dead_bitmap;
    s_tag_reference hill_shader;
    s_tag_reference game_intro_messages[11];
};

typedef char multiplayer_runtime_frag_offset[(offsetof(s_multiplayer_runtime_globals_definition, maximum_fragmentation_grenades) == 0x10c) ? 1 : -1];
typedef char multiplayer_runtime_plasma_offset[(offsetof(s_multiplayer_runtime_globals_definition, maximum_plasma_grenades) == 0x110) ? 1 : -1];
typedef char multiplayer_runtime_size[(sizeof(s_multiplayer_runtime_globals_definition) == 0x20c) ? 1 : -1];

#endif
