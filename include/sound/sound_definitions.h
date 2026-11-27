#ifndef SOUND_SOUND_DEFINITIONS_H
#define SOUND_SOUND_DEFINITIONS_H

#include "sound/game_sound_definitions.h"

// Functions defined in tu/blamlibxbox/sound_definitions.cpp that other units call.

struct s_cache_file_sound_definition;

float sound_definition_get_maximum_distance(long sound_index);
float sound_definition_get_silence_distance(long sound_index);
float sound_definition_get_minimum_distance(long sound_index);
short sound_definition_find_pitch_range_by_pitch(struct s_cache_file_sound_definition * _sym_p0, float x, short hint);
short sound_definition_next_permutation(struct s_cache_file_sound_definition * definition, short index);
float sound_definition_get_permutation_play_time(long sound_index, long pitch_range_index, long permutation_index);
float sound_permutation_get_real_mouth_aperture(long sound_index, short pitch_range_index, short permutation_index, short mouth_aperture_index);
short sound_definition_next_permutation_deterministic(struct s_cache_file_sound_definition * definition, short pitch_range_index,
    short last_permutation_index, unsigned long * _sym_p3, unsigned long * seed, bool * exhausted, bool invert_skip);
extern "C" float sound_definition_get_permutation_play_time_for_language(long sound_index, long pitch_range_index, long permutation_index, long block_index);

// Data defined in tu/blamlibxbox/sound_definitions.cpp that other units use.

extern const long sound_sample_rate_samples_per_second[];

struct real_decibel;

real_decibel sound_definition_map_gain_db(const s_cache_file_sound_definition *definition, real_decibel decibel, float fraction);

#endif // SOUND_SOUND_DEFINITIONS_H
