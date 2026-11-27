#ifndef MOTOR_ACTIONS_H
#define MOTOR_ACTIONS_H

#include "units/motor_enums.h"

// Functions defined in tu/objectslibxbox/actions.cpp.

struct action_request;

bool unit_action_open_submit(long object_index, struct action_request const * source);
extern "C" bool unit_action_start_melee_strike(long unit_index, long melee_type, long target_object_index, unsigned char target_is_forced, long weapon_slot);
void deploy_turret(long unit_index);
void leap_anticipation_finished(long unit_index, e_action action);

struct s_unit_action_player_melee_strike;
extern "C" const s_unit_action_player_melee_strike k_player_melee_strikes[];

#endif // MOTOR_ACTIONS_H
