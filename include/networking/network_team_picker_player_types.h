#ifndef NETWORKING_NETWORK_TEAM_PICKER_PLAYER_TYPES_H
#define NETWORKING_NETWORK_TEAM_PICKER_PLAYER_TYPES_H

// Input record shared by team picking and matchmaking composition. The three
// fields are the squad, peer and skill.
struct s_network_team_picker_player
{
    long squad_index;
    long peer_index;
    long skill_level;
};

#endif // NETWORKING_NETWORK_TEAM_PICKER_PLAYER_TYPES_H
