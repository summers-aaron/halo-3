#ifndef GAME_GAME_ENGINE_EVENT_DATA_H
#define GAME_GAME_ENGINE_EVENT_DATA_H

#include <stddef.h>

// Shared 40-byte event passed to game_engine_send_event and the
// simulation/event-response consumers. March's send-event assertion checks
// the identifier field. The event
// initializers store a word at +0x20 and a halfword at +0x24.
struct s_game_engine_event_data
{
    long type;
    long event;
    long identifier;
    union { long flags; long audience_player_index; };
    long cause_player_index;
    long cause_team_index;
    long effect_player_index;
    long effect_team_index;
    long data;
    short data_word;
};

typedef char game_engine_event_data_size[(sizeof(s_game_engine_event_data) == 40) ? 1 : -1];
typedef char game_engine_event_data_off_data_word[(offsetof(s_game_engine_event_data, data_word) == 0x24) ? 1 : -1];

static __forceinline void game_engine_initialize_event(long event_type, long type, s_game_engine_event_data *event_data)
{
    event_data->type = event_type;
    event_data->event = type;
    event_data->identifier = -1;
    event_data->flags = -1;
    event_data->cause_player_index = -1;
    event_data->cause_team_index = -1;
    event_data->effect_player_index = -1;
    event_data->effect_team_index = -1;
    event_data->data = 0;
    event_data->data_word = -1;
}

#endif // GAME_GAME_ENGINE_EVENT_DATA_H
