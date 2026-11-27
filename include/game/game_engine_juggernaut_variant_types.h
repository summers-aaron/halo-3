#ifndef GAME_GAME_ENGINE_JUGGERNAUT_VARIANT_TYPES_H
#define GAME_GAME_ENGINE_JUGGERNAUT_VARIANT_TYPES_H

enum e_juggernaut_flags
{
    _juggernaut_flags_allied_against_juggernaut = 0,
    _juggernaut_flags_respawn_on_lone_juggernaut,
    _juggernaut_flags_destination_zones_enabled,
    k_juggernaut_flags
};

// Value names follow the option strings of the juggernaut settings menu (mainmenu.map string table:
// juggernaut_selection_initial_random/first_kill/first_death, juggernaut_selection_next_killer/killed_by/
// unchanged/random, juggernaut_zones_movement_none/.../on_arrival/on_switch, juggernaut_zones_order random/sequence),
// listed in menu order.
enum e_juggernaut_initial_juggernaut_settings
{
    _juggernaut_initial_juggernaut_random = 0,
    _juggernaut_initial_juggernaut_first_kill,
    _juggernaut_initial_juggernaut_first_death,
    k_juggernaut_initial_juggernaut_settings_count
};

enum e_juggernaut_next_juggernaut_settings
{
    _juggernaut_next_juggernaut_killer = 0,
    _juggernaut_next_juggernaut_killed_by,
    _juggernaut_next_juggernaut_unchanged,
    _juggernaut_next_juggernaut_random,
    k_juggernaut_next_juggernaut_settings_count
};

// The eight values between none and on_arrival are the timed movement periods.
enum e_juggernaut_zone_movement_settings
{
    _juggernaut_zone_movement_none = 0,
    _juggernaut_zone_movement_on_arrival = 9,
    _juggernaut_zone_movement_on_switch,
    k_juggernaut_zone_movement_settings_count
};

enum e_juggernaut_zone_order_settings
{
    _juggernaut_zone_order_random = 0,
    _juggernaut_zone_order_sequence,
    k_juggernaut_zone_order_settings_count
};




#endif // GAME_GAME_ENGINE_JUGGERNAUT_VARIANT_TYPES_H
