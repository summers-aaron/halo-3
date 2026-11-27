#ifndef GAME_PLAYER_TRAITS_SOURCE_TYPES_H
#define GAME_PLAYER_TRAITS_SOURCE_TYPES_H

struct s_player_traits_shield_source
{
    unsigned char damage_resistance;
    unsigned char shield_multiplier;
    unsigned char shield_recharge_rate;
    unsigned char headshot_immunity;
    unsigned char vampirism;
};
struct s_player_traits_movement_source
{
    unsigned char speed;
    unsigned char gravity;
    unsigned char vehicle_usage;
};
struct s_player_traits_sensors_source
{
    long motion_tracker;
    long motion_tracker_range;
};

#endif
