#ifndef SAVED_GAMES_SAVED_FILM_RUNTIME_H
#define SAVED_GAMES_SAVED_FILM_RUNTIME_H

#include "shell/shell.h"

class c_async_double_buffer;
class c_saved_film;
struct game_options;
#ifdef HALO3_64BIT
struct s_file_reference;
#endif
struct s_film_replay_metadata;
struct simulation_update;

// Recording and playback operations shared with the simulation runtime.
extern "C" {
void saved_film_dispose(void);
int saved_film_get_save_state(void);
void saved_film_set_replay_metadata(int flags, const s_film_replay_metadata *metadata);
void saved_film_update_playback_speed(long tick);
void saved_film_set_playback_rate(float value, unsigned char flag);
bool saved_film_is_recording(void);
#ifdef HALO3_64BIT
bool saved_film_is_recording_to_file(const s_file_reference *file);
#endif
c_async_double_buffer *saved_film_get_async_buffer(void);
void saved_film_reset_recording_state(void);
void saved_film_get_replay_metadata(s_film_replay_metadata *metadata);
bool saved_film_update_storage(c_saved_film *film, const simulation_update *update);
bool saved_film_new_recording(c_saved_film *film, const char *file_path, const game_options *options, e_controller_index controller_index);
}

#endif // SAVED_GAMES_SAVED_FILM_RUNTIME_H
