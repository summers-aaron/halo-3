#ifndef GAME_GAME_ENGINE_EVENTS_H
#define GAME_GAME_ENGINE_EVENTS_H

struct s_game_engine_event_data;

void game_engine_send_event(s_game_engine_event_data *event_data);
void game_engine_handle_time_messages(
    int time_message_index, int results_absolute_index, long cause_player_datum, long effect_player_datum, int *message_flags);

#endif // GAME_GAME_ENGINE_EVENTS_H
