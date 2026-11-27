#ifndef OBJECTS_DAMAGE_REPORTING_TYPES_H
#define OBJECTS_DAMAGE_REPORTING_TYPES_H

// Weapon reporting types used by multiplayer kill notifications.
enum e_damage_reporting_type
{
    _damage_reporting_type_unknown = 0,

    // Weapon-streak categories counted by c_game_statborg::record_player_kill.
    _damage_reporting_type_13 = 13,
    _damage_reporting_type_sniper_rifle = 14,
    _damage_reporting_type_beam_rifle = 15,
    _damage_reporting_type_missile_launcher = 19,
    _damage_reporting_type_rocket_launcher = 20,
    _damage_reporting_type_spartan_laser = 21,
    _damage_reporting_type_flame_thrower = 23,
    _damage_reporting_type_25 = 25,
    _damage_reporting_type_firebomb_grenade = 30,

    // Vehicle categories (names from g_damage_reporting_type_string_list).
    _damage_reporting_type_banshee = 39,
    _damage_reporting_type_ghost = 40,
    _damage_reporting_type_mongoose = 41,
    _damage_reporting_type_scorpion = 42,
    _damage_reporting_type_spectre_driver = 44,
    _damage_reporting_type_warthog_driver = 46,
    _damage_reporting_type_wraith = 49,
    _damage_reporting_type_wraith_antiinfantry = 50,
    _damage_reporting_type_tank = 51,
    _damage_reporting_type_chopper = 52,
    _damage_reporting_type_hornet = 53,
    _damage_reporting_type_mantis = 54,
    _damage_reporting_type_mauler = 55,

    _damage_reporting_type_count = 61
};

#endif // OBJECTS_DAMAGE_REPORTING_TYPES_H
