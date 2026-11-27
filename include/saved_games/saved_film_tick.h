#ifndef SAVED_GAMES_SAVED_FILM_TICK_H
#define SAVED_GAMES_SAVED_FILM_TICK_H

#include "multithreading/synchronized_value.h"

// saved_film.cpp owns the interlocked playback tick.
extern "C" c_interlocked_long g_saved_film_playback_tick;

#endif
