#ifndef GAME_GAME_ENGINE_NOTIFICATIONS_H
#define GAME_GAME_ENGINE_NOTIFICATIONS_H

#include "game/game_types.h"

// Functions defined in tu/blamlibxbox/game_engine_notifications.cpp that other units call.

void game_engine_player_nearby_item(long player_index, long object_index);
short game_engine_notify_shot_fired(long player_index, unsigned char flags);
void game_engine_player_deleted(long player_index);
void game_engine_player_indices_swapped(long player_index1, long player_index2);
void game_engine_player_changed_indices(long old_player_index, long new_player_index);
void game_engine_player_rejoined(long player_index);
void game_engine_multiplayer_weapon_register(long weapon_index, short multiplayer_weapon_identifier);
void game_engine_multiplayer_weapon_deregister(long weapon_index);
void game_engine_multiplayer_weapon_picked_up(long weapon_index, long unit_index);
void item_update_player_inventory(long weapon_index);
void game_engine_multiplayer_weapon_drop_internal(long weapon_index, long unit_index);
void game_engine_player_left(long player_index);
void game_engine_unit_entered_vehicle(long unit_index, long vehicle_index);
void game_engine_player_added(long player_index);
void game_engine_player_damaged_player(long player_index, long damage_index, long damaged_player_index, bool dead);
extern "C" void game_engine_record_vehicle_exit(long object_index, long weapon_index);
extern "C" void game_engine_notify_player_activated(long object_index);
extern "C" void game_engine_player_prepare_to_change_team(long player_index, long new_team);
extern "C" void game_engine_mark_user_unit_changed(long player_index, long object_index);
extern "C" void game_engine_notify_player_killed(long killer, long dead_unit, long victim, bool betrayal, unsigned char damage, bool airborne);
void game_engine_player_changed_teams(long player_index, e_game_team team, e_game_team team2);

#endif // GAME_GAME_ENGINE_NOTIFICATIONS_H
