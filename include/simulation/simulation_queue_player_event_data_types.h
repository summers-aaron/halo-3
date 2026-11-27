#ifndef SIMULATION_SIMULATION_QUEUE_PLAYER_EVENT_DATA_TYPES_H
#define SIMULATION_SIMULATION_QUEUE_PLAYER_EVENT_DATA_TYPES_H

struct s_simulation_queue_player_event_data
{
    bool active;
    long respawn_target_absolute_player_index;
};

typedef char s_simulation_queue_player_event_data_size[(sizeof(s_simulation_queue_player_event_data) == 8) ? 1 : -1];

#endif // SIMULATION_SIMULATION_QUEUE_PLAYER_EVENT_DATA_TYPES_H
