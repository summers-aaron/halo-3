#ifndef GAME_GAME_ENGINE_HUD_STATE_TYPES_H
#define GAME_GAME_ENGINE_HUD_STATE_TYPES_H
#include "tag_files/tag_types.h"
#include <stddef.h>
// Original-platform MultiplayerGlobals.StateResponse (TagTool), 0x24 bytes.
struct s_hud_state_entry {
    unsigned short flags;
    unsigned short alignment2;
    short state;
    unsigned short alignment6;
    long without_teams_value;
    long with_teams_value;
    s_tag_reference unused;
    long alignment20;
};
typedef char hud_state_size[(sizeof(s_hud_state_entry) == 36) ? 1 : -1];
typedef char hud_state_message_offset[(offsetof(s_hud_state_entry, without_teams_value) == 8) ? 1 : -1];
#endif
