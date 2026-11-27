#ifndef SAVED_GAMES_SAVED_FILM_SNIPPET_INTERNALS_H
#define SAVED_GAMES_SAVED_FILM_SNIPPET_INTERNALS_H

#include "multithreading/synchronized_value.h"
#include "saved_games/saved_film_recording_types.h"

enum
{
    k_saved_film_snippet_marker_count = 16
};

extern "C" void saved_film_snippet_set_cameras_from_states(s_saved_film_snippet_camera *states);

extern "C" c_interlocked_long g_saved_film_snippet_tick;

#endif // SAVED_GAMES_SAVED_FILM_SNIPPET_INTERNALS_H
