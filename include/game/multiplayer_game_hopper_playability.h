#ifndef GAME_MULTIPLAYER_GAME_HOPPER_PLAYABILITY_H
#define GAME_MULTIPLAYER_GAME_HOPPER_PLAYABILITY_H

enum e_session_game_start_error;
class c_network_session_membership;

extern "C" void multiplayer_game_hopper_update_player_history(
    long controller_index, unsigned short hopper_identifier, const unsigned __int64 *player_identifiers, long player_count);
extern e_session_game_start_error multiplayer_game_is_playable(unsigned short hopper_identifier, bool is_matchmaking, bool check_hopper,
    const c_network_session_membership *membership, unsigned short *out_player_error_mask);

#endif // GAME_MULTIPLAYER_GAME_HOPPER_PLAYABILITY_H
