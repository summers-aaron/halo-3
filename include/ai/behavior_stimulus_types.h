#ifndef AI_BEHAVIOR_STIMULUS_TYPES_H
#define AI_BEHAVIOR_STIMULUS_TYPES_H

enum e_behavior_stimulus
{
    _behavior_stimulus_surprise = 0,
    _behavior_stimulus_danger = 1,
    _behavior_stimulus_friend_body = 2,
    _behavior_stimulus_shield_depleted = 3,
    _behavior_stimulus_leader_killed = 4,
    _behavior_stimulus_peer_killed_nearby = 5,
    _behavior_stimulus_peer_killed = 6,
    _behavior_stimulus_stuck_with_grenade = 7,
    _behavior_stimulus_super_detonation = 8,
    _behavior_stimulus_destructible_obstacle = 10,
    _behavior_stimulus_cover_friend = 11,
    _behavior_stimulus_activity = 12,
    _behavior_stimulus_changed_target = 13,
    _behavior_stimulus_cover_failed = 14,
    _behavior_stimulus_broken = 15,
    _behavior_stimulus_leader_abandoned = 16,
    _behavior_stimulus_just_flanked = 17,
    _behavior_stimulus_enemy_vehicle = 18,
    _behavior_stimulus_turtle = 19,
    _behavior_stimulus_flood_disperse = 20
};

#endif // AI_BEHAVIOR_STIMULUS_TYPES_H
