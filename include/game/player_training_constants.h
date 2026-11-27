#ifndef GAME_PLAYER_TRAINING_CONSTANTS_H
#define GAME_PLAYER_TRAINING_CONSTANTS_H

// Training indices identified by their trigger and completion conditions.
enum e_player_training_type
{
    _player_training_flashlight = 0,
    _player_training_crouch = 1,
    _player_training_look_left_and_right = 2,
    // Names below from the March 2007 debug map's player_training.obj string pool
    // (local/reference/h3-0308/halo3_cache_debug.map), cross-checked against each
    // training's trigger vehicle type and completion action flags.
    _player_training_activate_device_hold = 5,
    _player_training_enter_vehicle_hold = 6,
    _player_training_weapon_swap_hold = 7,
    _player_training_dual_wield_hold = 8,
    _player_training_fire_secondary_weapon = 9,
    _player_training_vehicle_look = 10,
    _player_training_vehicle_move = 11,
    _player_training_look = 12,
    _player_training_move = 13,
    _player_training_unzoom = 14,
    _player_training_weapon_kill = 15,
    _player_training_zoom = 16,
    _player_training_grenade_kill = 17,
    _player_training_headshot_kill = 18,
    _player_training_assassination = 19,
    _player_training_vehicle_boarding = 20,
    _player_training_shield_recharge = 21,
    _player_training_shield_depleted = 22,
    _player_training_reload_primary_weapon = 23,
    _player_training_warthog_slide = 24,
    _player_training_ghost_guns = 25,
    _player_training_banshee_boost = 26,
    _player_training_banshee_bomb = 27,
    _player_training_ghost_boost = 28,
    _player_training_vehicle_trick = 29,
    _player_training_voice_chat = 30,
    _player_training_stealth = 31,
    _player_training_equipment = 32,
    _player_training_jump = 33,
    _player_training_reload_secondary_weapon = 34,
    k_player_training_count = 35
};

#endif // GAME_PLAYER_TRAINING_CONSTANTS_H
