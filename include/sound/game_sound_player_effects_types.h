#ifndef SOUND_GAME_SOUND_PLAYER_EFFECTS_TYPES_H
#define SOUND_GAME_SOUND_PLAYER_EFFECTS_TYPES_H

struct s_player_effect
{
    unsigned char active;
    int sound_effect_index;
    float time;
    int definition_index;
    union { int effect_index; float scale; } value;
};

struct s_player_effects_globals
{
    s_player_effect effects[2];
};

typedef char player_effects_globals_size[(sizeof(s_player_effects_globals) == 40) ? 1 : -1];
extern "C" { extern __declspec(thread) s_player_effects_globals *g_player_effects_globals; }

#endif // SOUND_GAME_SOUND_PLAYER_EFFECTS_TYPES_H
