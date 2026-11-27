#ifndef NETWORKING_NETWORK_TEAM_PICKER_H
#define NETWORKING_NETWORK_TEAM_PICKER_H

// Functions defined in tu/networkinglibxbox/network_team_picker.cpp that other units call.

struct s_network_team_picker_player;

bool network_team_picker_can_add_whole_squad(long player_count, long team_count, long maximum_team_size,
    long maximum_imbalance, long new_player_count, const s_network_team_picker_player *new_players);
bool network_team_picker_pick_teams(long a, long b, long c, long d, bool e, bool f, bool g, bool h,
    long player_count, const s_network_team_picker_player *players, long *imbalance, long *assignments);

#endif // NETWORKING_NETWORK_TEAM_PICKER_H
