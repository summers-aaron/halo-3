#ifndef GAME_GAME_ENGINE_GROUP_H
#define GAME_GAME_ENGINE_GROUP_H

// Functions defined in tu/blamlibxbox/game_engine_group.cpp that other units call.

struct s_game_engine_player_traits;
struct s_game_engine_settings_definition;

const s_game_engine_settings_definition *game_engine_settings_try_and_get();
const s_game_engine_player_traits *game_engine_settings_player_traits_get_by_name(long name);

#endif // GAME_GAME_ENGINE_GROUP_H
