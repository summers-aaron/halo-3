#ifndef SOUND_GAME_SOUND_EXTERNS_H
#define SOUND_GAME_SOUND_EXTERNS_H
#include "cutscene/cinematic_globals_types.h"

#include "math/real_math_constants.h"
#include "game/player_mapping_globals.h"

// Shared declarations used by the game-sound implementation.
#include "sound/scripted_sound_constants.h"
#include "sound/sound_data.h"
#include "sound/game_sound_globals.h"
#include "interface/chud/chud_dispose_state_types.h"

struct s_data_array;
struct ai_player_state;
struct s_user_globals;
struct s_cinematic_globals_definition;
struct s_controllers;
#include "game/game_globals_storage.h"
struct real_matrix4x3;
#include "effects/effect_data.h"
// One scripted impulse sound slot (March asserts name the slot scripted_impulse_sound).
struct s_scripted_impulse_sound
{
    long definition_index;
    long start_time;
    long sound_index;
    union
    {
        long permutation;
        struct
        {
            signed char pitch_range_index;
            signed char permutation_index;
            unsigned char unit_source;
            unsigned char reserved;
        };
    };
};
typedef char scripted_impulse_sound_size[(sizeof(s_scripted_impulse_sound) == 0x10) ? 1 : -1];

enum { k_maximum_scripted_impulse_sounds = 32 };

extern "C" {
extern __declspec(thread) s_scripted_impulse_sound *g_game_sound_scripted_impulses;
}

#endif
