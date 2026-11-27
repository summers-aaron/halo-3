#ifndef GAME_MULTIPLAYER_CONSTANTS_H
#define GAME_MULTIPLAYER_CONSTANTS_H

#include <stddef.h>
#include "tag_files/tag_types.h"

struct s_player_spawn_influence
{
    float inner_radius, outer_radius, upper_height, lower_height, weight;
};

struct s_netgame_goal_spawn_influence
{
    float inner_radius, outer_radius, upper_height, lower_height, weight;
};


struct s_weapon_spawn_influence
{
    s_tag_reference weapon;
    float inner_radius, outer_radius, height, weight;
};
struct s_vehicle_spawn_influence
{
    s_tag_reference vehicle;
    float inner_radius;
    float velocity_scale;
    float minimum_speed;
    float weight;
};
struct s_equipment_spawn_influence
{
    s_tag_reference equipment;
    float weight;
};
struct s_projectile_spawn_influence
{
    s_tag_reference projectile;
    float velocity_scale;
    float inner_radius;
    float weight;
};
struct s_multiplayer_constants
{
    s_player_spawn_influence ally_bias_influence;
    s_player_spawn_influence enemy_bias_influence;
    s_player_spawn_influence vehicle_influence;
    s_player_spawn_influence projectile_influence;
    s_player_spawn_influence dead_teammate_influence;
    float dead_teammate_influence_duration;
    c_typed_tag_block<s_weapon_spawn_influence> weapon_selections;
    c_typed_tag_block<s_vehicle_spawn_influence> vehicle_selections;
    c_typed_tag_block<s_projectile_spawn_influence> projectile_selections;
    c_typed_tag_block<s_equipment_spawn_influence> equipment_selections;
    s_netgame_goal_spawn_influence netgame_goal_influences[8];
    float maximum_random_spawn_bias;
    float teleporter_recharge_time;
    float grenade_danger_weight;
    float grenade_danger_inner_radius;
    float grenade_danger_outer_radius;
    float grenade_danger_lead_time;
    float vehicle_danger_minimum_speed;
    float vehicle_danger_weight;
    float vehicle_danger_radius;
    float vehicle_danger_lead_time;
    float vehicle_nearby_player_distance;
    s_tag_reference hill_bitmap;
    float flag_touch_return_radius;
    float flag_enemy_contest_radius;
    float flag_enemy_contest_fallback_radius;
    float territories_waypoint_vertical_offset;
    s_tag_reference bomb_explosion_effect;
    s_tag_reference bomb_explosion_sound;
    s_tag_reference bomb_explosion_damage;
    s_tag_reference bomb_reset_effect;
    s_tag_reference editor_effect;
};

typedef char multiplayer_constants_teleporter_recharge_offset[(offsetof(s_multiplayer_constants, teleporter_recharge_time) == 0x13c) ? 1 : -1];

typedef char multiplayer_constants_bomb_effect_offset[(offsetof(s_multiplayer_constants, bomb_explosion_effect) == 0x184) ? 1 : -1];
typedef char multiplayer_constants_bomb_reset_offset[(offsetof(s_multiplayer_constants, bomb_reset_effect) == 0x1b4) ? 1 : -1];

typedef char multiplayer_constants_editor_effect_index_offset[(offsetof(s_multiplayer_constants, editor_effect)
    + offsetof(s_tag_reference, index) == 0x1d0) ? 1 : -1];

#endif
