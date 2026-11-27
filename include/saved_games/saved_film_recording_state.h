#ifndef SAVED_GAMES_SAVED_FILM_RECORDING_STATE_H
#define SAVED_GAMES_SAVED_FILM_RECORDING_STATE_H

// Save-flow states, named for the operations in saved_film_update_save_flow.
enum e_saved_film_recording_state
{
    _saved_film_recording_waiting_for_description = 10,
    _saved_film_recording_copy = 11,
    _saved_film_recording_waiting_for_copy = 12,
    _saved_film_recording_update_metadata = 13,
    _saved_film_recording_waiting_for_metadata = 14,
    _saved_film_recording_waiting_for_playback = 15,
};

#endif
