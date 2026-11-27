#ifndef GAME_GAME_RESULTS_TEAM_DATA_TYPES_H
#define GAME_GAME_RESULTS_TEAM_DATA_TYPES_H

struct s_game_results_team_data
{
    bool team_exists;
    signed char standing;
    short final_score;
};

__declspec(align(8)) struct s_game_results_teams
{
    s_game_results_team_data team[16];
};

typedef char game_results_team_data_size[(sizeof(s_game_results_team_data) == 0x4) ? 1 : -1];
typedef char game_results_teams_size[(sizeof(s_game_results_teams) == 0x40) ? 1 : -1];

#endif // GAME_GAME_RESULTS_TEAM_DATA_TYPES_H
