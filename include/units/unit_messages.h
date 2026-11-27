#ifndef UNITS_UNIT_MESSAGES_H
#define UNITS_UNIT_MESSAGES_H

// NONE in action selection is -1. first_person_weapon_state_from_message maps idle (0) to the "idle" state.
// Each name follows the animation string id first_person_weapon_state_from_message maps it to.
// The overheat messages start the matching _action_*_weapon_overheat_* actions
// (unit_handle_message_from_weapon): weapon_update sends enter when the weapon
// overheats and exit once its heat reaches the recovery threshold.
enum e_unit_messages
{
    _unit_message_none = -1,
    _unit_message_idle = 0,
    _unit_message_fire_1 = 1,
    _unit_message_fire_2 = 2,
    _unit_message_misfire_1 = 3,
    _unit_message_misfire_2 = 4,
    _unit_message_light_on = 5,
    _unit_message_light_off = 6,
    _unit_message_reload_empty = 7,
    _unit_message_reload_full = 8,
    _unit_message_reload_enter = 9,
    _unit_message_reload_continue_empty = 10,
    _unit_message_reload_continue_full = 11,
    _unit_message_reload_finished = 12,
    _unit_message_fully_charged = 13,
    _unit_message_overheat_enter = 14,
    _unit_message_overheat_exit = 15,
    _unit_message_melee = 17,
    _unit_message_put_away_primary = 18,
    _unit_message_put_away_secondary = 19,
    _unit_message_ready_primary = 20,
    _unit_message_ready_secondary = 21,
    _unit_message_ready_overheated_primary = 22,
    _unit_message_ready_overheated_secondary = 23,
    _unit_message_put_away_end_primary = 24,
    _unit_message_put_away_end_secondary = 25,
    _unit_message_throw_grenade = 26,
    _unit_message_throw_overheated = 27,
    _unit_message_throw_equipment = 28,
    // First-person melee events, each paired with its melee animation name
    // (unit_action_melee_player_submit in tu/objectslibxbox/actions.cpp).
    _unit_message_melee_1st_hit = 29,
    _unit_message_melee_1st_hit_to_idle = 30,
    _unit_message_melee_2nd_hit = 31,
    _unit_message_melee_2nd_hit_to_idle = 32,
    _unit_message_melee_3rd_hit = 33,
    _unit_message_melee_strike_1 = 34,
    _unit_message_melee_strike_2 = 35,
    _unit_message_melee_strike_3 = 36,
    _unit_message_melee_strike_4 = 37,
    _unit_message_melee_dash = 38,
    _unit_message_melee_lunge = 39,
    _unit_message_melee_dash_airborne = 40,
    _unit_message_melee_lunge_airborne = 41,
    _unit_message_melee_lunge_unaimed = 42,
    _unit_message_melee_lunge_unaimed_airborne = 43
};

#endif // UNITS_UNIT_MESSAGES_H
