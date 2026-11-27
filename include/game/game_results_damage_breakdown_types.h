#ifndef GAME_GAME_RESULTS_DAMAGE_BREAKDOWN_TYPES_H
#define GAME_GAME_RESULTS_DAMAGE_BREAKDOWN_TYPES_H

enum e_game_results_damage_breakdown_type
{
    _game_results_damage_breakdown_type_none = 0,
    _game_results_damage_breakdown_type_all = 1,
    // Carnage kill-breakdown columns count these (killbreakdown_weapon/melee/grenade);
    // ai_dialogue tests grenade kills against _grenade.
    _game_results_damage_breakdown_type_weapon = 2,
    _game_results_damage_breakdown_type_melee = 3,
    _game_results_damage_breakdown_type_grenade = 4,
    _game_results_damage_breakdown_type_vehicle = 5
};

#endif // GAME_GAME_RESULTS_DAMAGE_BREAKDOWN_TYPES_H
