#ifndef SAVED_GAMES_SAVED_FILM_TIMER_CONTROL_TYPES_H
#define SAVED_GAMES_SAVED_FILM_TIMER_CONTROL_TYPES_H

// Timer control view: playback speed at +0x101fc; request state at +0x10248.
struct s_saved_film_timer_control
{
    char state[0x101fc];
    float playback_speed;
    char timer_state[0x10248 - 0x10200];
    bool request_pending;
    long requested_state;
};

#endif
