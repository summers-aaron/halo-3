#ifndef SAVED_GAMES_GAME_STATE_PENDING_REQUEST_H
#define SAVED_GAMES_GAME_STATE_PENDING_REQUEST_H

struct s_game_state_pending_request
{
    bool enabled;
    unsigned char armed;
    unsigned long last_time;
    long state;
    char name[128];
};

extern "C" s_game_state_pending_request g_game_state_pending_request;

#endif
