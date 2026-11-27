#ifndef GAME_GAME_RESULTS_REPLICATOR_H
#define GAME_GAME_RESULTS_REPLICATOR_H
#include "game/game_results_team_data_types.h"
#include "game/game_results_player_data.h"
#include "game/game_results_statistics_types.h"
struct s_results_players { s_game_results_player_data player[16]; };
struct s_results_statistics { union { char data[19424]; s_game_results_statistics values; }; };
struct s_results_machines { union { char data[352]; s_entry machine[16]; }; };
struct s_game_results_incremental
{
    bool finalized;
    bool summary_valid;
    long summary_value;
    bool finish_valid;
    long finish_value;
    s_results_players players;
    s_game_results_teams teams;
    s_results_statistics statistics;
    s_results_machines machines;
};
class c_simulation_view;
class c_game_results_replicator {
public:
    void handle_view_establishment(bool established);
    void update();
private:
    void send_game_results_update();
public:
    c_simulation_view *context;
    bool fatal_error;
    unsigned char m_sending_updates;
    unsigned char m_receiving_updates;
    char pad7;
    long m_next_update_number;
    int padc;
    s_game_results_incremental m_incremental;
    long m_last_update_time;
};
#endif
