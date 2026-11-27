#ifndef GAME_GAME_ENGINE_INTERFACE_STATE_TYPES_H
#define GAME_GAME_ENGINE_INTERFACE_STATE_TYPES_H

#include <stddef.h>

// The HUD interface state getter owns the common
// 0x1b8-byte state. Mode-specific writers name the fields in its unused spans:
// King 0x2ea000, CTF 0x2e3ff0, Territories 0x2f0630, and Sandbox 0x271c30.
struct game_engine_interface_state
{
    long game_type;
    wchar_t variant_name[32];
    bool sudden_death;
    bool sudden_death_active;
    wchar_t sudden_death_text[20];
    bool show_time_remaining;
    bool stats_valid;
    bool team_in_hill;
    unsigned long own_color;
    long own_score;
    bool rival_valid;
    bool enemy_in_hill;
    unsigned long rival_color;
    long rival_score;
    long rival_index;
    bool leading;
    long score_to_win;
    bool talker_valid;
    wchar_t talker_name[32];
    bool flag_return_visible;
    float flag_return_progress;
    unsigned long flag_return_color;
    wchar_t flag_return_text[64];
    char spare[0x1a4 - 0x160];
    union { unsigned long ctf_flags; unsigned long variant_custom_flags; };
    long editing_state;
    float budget_fraction;
    float budget;
    float budget_used;
};

typedef char game_engine_interface_state_size[(sizeof(game_engine_interface_state) == 0x1b8) ? 1 : -1];
typedef char game_engine_interface_sudden_death_offset[(offsetof(game_engine_interface_state, sudden_death) == 0x44) ? 1 : -1];
typedef char game_engine_interface_team_in_hill_offset[(offsetof(game_engine_interface_state, team_in_hill) == 0x70) ? 1 : -1];
typedef char game_engine_interface_enemy_in_hill_offset[(offsetof(game_engine_interface_state, enemy_in_hill) == 0x7d) ? 1 : -1];
typedef char game_engine_interface_flag_return_offset[(offsetof(game_engine_interface_state, flag_return_visible) == 0xd6) ? 1 : -1];
typedef char game_engine_interface_flag_text_offset[(offsetof(game_engine_interface_state, flag_return_text) == 0xe0) ? 1 : -1];
typedef char game_engine_interface_ctf_flags_offset[(offsetof(game_engine_interface_state, ctf_flags) == 0x1a4) ? 1 : -1];
typedef char game_engine_interface_editing_state_offset[(offsetof(game_engine_interface_state, editing_state) == 0x1a8) ? 1 : -1];
typedef char game_engine_interface_budget_offset[(offsetof(game_engine_interface_state, budget_fraction) == 0x1ac) ? 1 : -1];

#endif // GAME_GAME_ENGINE_INTERFACE_STATE_TYPES_H
