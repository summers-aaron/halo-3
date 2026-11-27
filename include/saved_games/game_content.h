#ifndef SAVED_GAMES_GAME_CONTENT_H
#define SAVED_GAMES_GAME_CONTENT_H

#include "saved_games/game_content_types.h"
#include "saved_games/saved_game_files.h"
#include "game/game_types.h"

e_saved_game_file_type game_content_type_to_saved_game_file_type(e_game_content_type content_type);
extern "C" e_game_engine_type game_content_type_to_game_engine_type(e_game_content_type content_type);

#endif // SAVED_GAMES_GAME_CONTENT_H
