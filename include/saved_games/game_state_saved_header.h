#ifndef SAVED_GAMES_GAME_STATE_SAVED_HEADER_H
#define SAVED_GAMES_GAME_STATE_SAVED_HEADER_H

struct game_state_header;

extern "C" bool game_state_saved_game_header_valid(const game_state_header *header);

#endif // SAVED_GAMES_GAME_STATE_SAVED_HEADER_H
