#ifndef SAVED_GAMES_SAVED_FILM_SCOREBOARD_H
#define SAVED_GAMES_SAVED_FILM_SCOREBOARD_H

#include "saved_games/saved_film_globals.h"
#include "saved_games/saved_film_tick.h"

static __forceinline long saved_film_scoreboard_playback_tick()
{
    return g_saved_film_playback_tick.peek();
}

// Whether the scoreboard message has been shown.
static __forceinline bool &saved_film_scoreboard_message_shown()
{
    return g_saved_film_timer_globals.scoreboard_message_shown;
}

#endif
