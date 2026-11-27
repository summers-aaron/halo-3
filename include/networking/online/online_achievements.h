#ifndef NETWORKING_ONLINE_ONLINE_ACHIEVEMENTS_H
#define NETWORKING_ONLINE_ONLINE_ACHIEVEMENTS_H

#include "shell/shell.h"

enum e_achievement;

extern "C" const unsigned long k_online_achievement_ids[49];
// @address 0x8086b8 g_achievement_progress
extern "C" unsigned long g_achievement_progress[49];

// Functions defined in tu/networkinglibxbox/online_achievements.cpp that other units call.

void achievements_initialize(void);
void achievements_dispose();
void achievements_update(float elapsed);
bool gamer_has_achievement(e_controller_index controller_index, e_achievement achievement);
void gamer_grant_achievement(e_controller_index controller_index, e_achievement achievement);


#endif // NETWORKING_ONLINE_ONLINE_ACHIEVEMENTS_H
