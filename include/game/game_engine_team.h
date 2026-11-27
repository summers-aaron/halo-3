#ifndef GAME_GAME_ENGINE_TEAM_H
#define GAME_GAME_ENGINE_TEAM_H

// Functions defined in tu/blamlibxbox/game_engine_team.cpp that other units call.

enum e_game_team;
enum e_multiplayer_team_designator;
enum e_map_id;
class c_game_variant;

bool game_engine_is_team_active(e_game_team team);
#include "game/game_engine_team_relationships.h"
e_multiplayer_team_designator game_engine_team_index_to_team_designator(e_game_team team);
e_game_team game_engine_next_active_team(e_game_team team);
e_game_team game_engine_next_inactive_team(e_game_team team);
long game_engine_variant_get_maximum_team_count(const c_game_variant *variant, e_map_id multiplayer_map_id);
extern "C" bool game_engine_team_is_out_of_lives(e_game_team team);

bool game_engine_has_teams(void);
void game_engine_build_valid_team_mapping(void);
void game_engine_handle_new_team_mapping();
void game_engine_build_teams(void);
void game_engine_recompute_active_teams(void);
extern "C" bool game_engine_is_team_ever_active(unsigned int index);
extern "C" int game_engine_teams_get_number_of_players_alive(int team);
extern "C" bool game_engine_teams_player_can_change_teams(int player_index);

#endif // GAME_GAME_ENGINE_TEAM_H
