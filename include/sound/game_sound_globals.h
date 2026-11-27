#ifndef SOUND_GAME_SOUND_GLOBALS_H
#define SOUND_GAME_SOUND_GLOBALS_H

#include "memory/static_arrays.h"

struct s_game_sound_listener
{
    long cluster_loop_index;
    long active_fog_definition_index;
    bool tracked_as_underwater;
};

struct game_sound_environment_parameters
{
    // March passes these six fields to sound_manager_set_global_environment as one s_sound_global_environment_parameters.
    float global_environment_values[4];
    long global_environment_index;
    float global_environment_value;
    bool ambience_details_enabled;
    bool suppress_ambience_update_on_revert;
    // Rises toward 1 while a cinematic plays and falls back to 0 after; copied into s_sound_manager_globals::timeline_duck.
    float timeline_duck;
};

struct game_sound_globals
{
    c_static_array<s_game_sound_listener, 4> listeners;
    // game time at which the latest scripted-speech impulse ends
    long scripted_speech_timer;
    long scripted_dialog_timer;
    c_static_array<unsigned short, 128> occlusion;
    game_sound_environment_parameters environment;
};

typedef char game_sound_environment_size[(sizeof(game_sound_environment_parameters) == 32) ? 1 : -1];
typedef char game_sound_listener_size[(sizeof(s_game_sound_listener) == 12) ? 1 : -1];
typedef char game_sound_globals_size[(sizeof(game_sound_globals) == 344) ? 1 : -1];
extern "C" { extern __declspec(thread) game_sound_globals *g_game_sound_globals; }

#endif
