#ifndef AI_AI_VEHICLES_H
#define AI_AI_VEHICLES_H

#include "game/game_types.h"

// Counts seats without the boarding flag.
short ai_vehicle_count_nonboarding_seats(long object_index, bool team);

// Functions defined in tu/ailibxbox/ai_vehicles.cpp.

struct seat_entry_info;
enum e_motor_program;

long ai_get_root_vehicle(long vehicle_index);
short ai_vehicle_get_num_flip_actors(long object_index);
void actor_handle_seat_exit(long actor_index, long vehicle_index, short seat_index);
void actor_handle_seat_exit(long actor_index, long vehicle_index, short seat_index, e_motor_program *motor_program);
void ai_handle_driver_change(long unit_index);
void actor_attach_to_seat(long actor_index, long vehicle_index, short seat_index, float timeout, bool instant);
void actor_attach_to_vehicle(long actor_index, long vehicle_index, bool force);
bool ai_vehicle_choose_seat(long actor_index);
bool ai_vehicle_check_seat(long actor_index, seat_entry_info *seat_entry);
bool ai_vehicle_cancel_reservation(long actor_index);
void ai_vehicle_handle_allegiance_change(long actor_index);
bool ai_vehicle_turret_valid_angle(long actor_index, long vehicle_index, long prop_index);
bool ai_vehicle_desirable(long actor_index, long vehicle_index, bool p2, bool p3);
bool actor_handle_seat_attach(long actor_index, long vehicle_index, short seat_index);
bool ai_vehicle_evaluate_seat(long actor_index, long unit_index, short seat_index, bool use_3d, seat_entry_info *info);
void ai_player_handle_seat_exit(long player_index, long vehicle_index, short seat_index);
float ai_unit_get_seat_score(long actor_index, long vehicle_index, short seat_index, float distance, bool immediate);
extern "C" void ai_player_handle_seat_enter(long player_index, long vehicle_index, short seat_index);
bool ai_vehicle_reserve_seat(long actor_index, long vehicle_index, long seat_index, float distance);
extern "C" float ai_unit_get_seat_preference(int object_index, short slot_index, unsigned char a, unsigned char b, unsigned char c);
bool ai_vehicle_seats_available(long actor_index, long vehicle_index);
bool ai_vehicle_occupied_by_enemy(long vehicle_index, e_game_team team);

struct unit_seat_source;

bool ai_unit_seat_allowed(long actor_index, long unit_index, short seat_index, e_game_team team);
bool ai_unit_seat_disallowed(long actor_index, long unit_index, short seat_index, e_game_team team);
void ai_vehicle_lock_actors(long actor_index, e_game_team team);
extern "C" bool ai_vehicle_find_seat(long actor_index, long unit_index, unit_seat_source *seats, short seat_count, seat_entry_info * info);
long ai_vehicle_get_reservation(long vehicle_index, long seat_index, enum e_game_team team);
short ai_vehicle_get_num_reservations(long unit_index, e_game_team team, bool include_self);
long ai_vehicle_get_driver(long unit_index, enum e_game_team team);

#endif // AI_AI_VEHICLES_H
