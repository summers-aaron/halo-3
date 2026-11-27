#ifndef INCLUDE_SOUND_SOUND_MANAGER_RUNTIME_H
#define INCLUDE_SOUND_SOUND_MANAGER_RUNTIME_H

#include "memory/static_arrays.h"
struct s_sound_impulse_definition;
struct s_sound_tracker;
struct real_decibel;
struct s_sound_output_parameters;
enum e_sound_impulse_new_failure_reason;
enum e_sound_datum_flag_bits;
enum e_sound_event_stop_reason;
enum e_sound_pause_state;

extern "C++" bool sound_definition_can_start_new_impulse_sound(long definition_index, const s_sound_impulse_definition *impulse,
    long *sound_priority, e_sound_impulse_new_failure_reason *failure_reason);
extern "C" unsigned char sound_make_permutation_ready_by_index(unsigned short sound_index, bool *out_ready);
bool sound_track_and_update(long sound_index, const s_sound_tracker *tracker, c_flags<e_sound_datum_flag_bits, unsigned short, 11> *flags);
extern "C" void sound_stop_and_unlock(unsigned long sound_index, e_sound_event_stop_reason reason);
void sound_pause(e_sound_pause_state mode, bool paused);
short sound_find_channel(long sound_index, e_sound_event_stop_reason *reason);
void update_channel_for_looping_sound(short channel_index, real_decibel gain, const s_sound_output_parameters *output);
void update_channel_for_impulse_sound(short channel_index, real_decibel gain, const s_sound_output_parameters *output);

struct audibility_table;
struct s_game_sound_transmission_listener;
struct s_game_sound_obstruction_parameters;
extern "C" {
void platform_sound_get_status(void *incoming);
void actor_perception_initialize_audibility_table(audibility_table *table, long scenario_index, long zone_set_index);
void game_sound_transmission_update_sources(
    audibility_table *table, s_game_sound_transmission_listener *listener, s_game_sound_obstruction_parameters *parameters);
}

#endif // INCLUDE_SOUND_SOUND_MANAGER_RUNTIME_H
