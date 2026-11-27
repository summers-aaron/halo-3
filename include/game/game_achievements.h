#ifndef GAME_GAME_ACHIEVEMENTS_H
#define GAME_GAME_ACHIEVEMENTS_H

#include "shell/shell.h"

enum e_achievement;

void gamer_grant_achievement(e_controller_index controller_index, e_achievement achievement);
void game_grant_achievement_to_local_player(e_controller_index controller_index, e_achievement achievement, bool force);
bool game_conditions_allow_achievement(long player_index, e_achievement achievement);

#endif // GAME_GAME_ACHIEVEMENTS_H
