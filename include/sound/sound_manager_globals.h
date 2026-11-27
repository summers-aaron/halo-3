#include "scenario/scenario.h"
#ifndef INCLUDE_SOUND_SOUND_MANAGER_GLOBALS_H
#define INCLUDE_SOUND_SOUND_MANAGER_GLOBALS_H

#include "sound/sound_data.h"
#include "multithreading/registered_thread_index.h"
#include "game/player_mapping_globals.h"
#include "saved_games/game_state_lock.h"
#include "math/real_math_constants.h"
#include "sound/sound_xaudio_xenon_mix_master_globals.h"

struct s_data_array;
struct s_user_globals;
#include "game/game_globals_storage.h"
#include "game/game_time_types.h"
union real_vector3d;

struct s_xaudio_source_state;
struct s_sound_preferences;
struct sound_environment;

extern "C" float k_sound_distance_delay_factor;
extern "C" const sound_environment k_default_reverb_definition;
extern "C" s_sound_preferences g_sound_preferences;

extern bool debug_sound_force_first_person_listener;

extern "C" {
extern float g_sound_master_gain[];
extern long g_sound_initialization_count;
extern bool g_sound_always_update;
extern bool g_sound_transmission_enabled;

extern const real_vector3d k_real_math_zero_vector_storage;
}

#endif // INCLUDE_SOUND_SOUND_MANAGER_GLOBALS_H
