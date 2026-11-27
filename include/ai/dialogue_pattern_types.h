#ifndef AI_DIALOGUE_PATTERN_TYPES_H
#define AI_DIALOGUE_PATTERN_TYPES_H

// Object filters used by the adlg vocalization patterns.
enum e_dialogue_object_filter
{
    _dialogue_object_any = 0,
    _dialogue_object_player = 1,
    _dialogue_object_actor = 2,
    _dialogue_object_living_biped = 3,
    _dialogue_object_dead_biped = 4,
    _dialogue_object_vehicle = 5,
    _dialogue_object_projectile = 6,
    _dialogue_object_actor_or_player = 7,
    _dialogue_object_turret = 8,
    _dialogue_object_vehicle_occupant = 9,
    _dialogue_object_turret_occupant = 10,
    _dialogue_object_deployed_turret = 11,
    _dialogue_object_driver = 12,
    _dialogue_object_gunner = 13,
    _dialogue_object_passenger = 14,
    _dialogue_object_postcombat = 15,
    _dialogue_object_postcombat_winner = 16,
    _dialogue_object_postcombat_loser = 17,
    _dialogue_object_player_spartan = 18,
    _dialogue_object_player_elite = 19,
    _dialogue_object_heretic = 20,
    _dialogue_object_scary = 21,
    _dialogue_object_only_vehicle_operator = 22,
    _dialogue_object_normal_dialogue = 23,
    _dialogue_object_special_dialogue = 24,
    _dialogue_object_grenade = 25,
    _dialogue_object_speech_disabled = 26,
    _dialogue_object_flood = 27,
    _dialogue_object_pureform = 28,
    _dialogue_object_player_with_empty_seat = 29
};

// Speaker selection for vocalization_pattern::speaker_type. Value 3 is unhandled;
// the speaker stays NONE.
enum e_dialogue_speaker_type
{
    _dialogue_speaker_unit = 0,
    _dialogue_speaker_other_unit = 1,
    _dialogue_speaker_nearby_non_enemy = 2,
    _dialogue_speaker_nearby_enemy = 4,
    _dialogue_speaker_vehicle_occupant = 5,
    _dialogue_speaker_joint_member = 6,
    _dialogue_speaker_task_member = 7,
    _dialogue_speaker_squad_leader = 8,
    _dialogue_speaker_joint_leader = 9,
    _dialogue_speaker_clump_member = 10,
    _dialogue_speaker_nearby_non_enemy_same_type = 11,
    _dialogue_speaker_none = 12
};

// Speaker behavior filter for vocalization_pattern::executing_state.
enum e_dialogue_speaker_behavior
{
    _dialogue_speaker_behavior_any = 0,
    _dialogue_speaker_behavior_combat,
    _dialogue_speaker_behavior_engage,
    _dialogue_speaker_behavior_search,
    _dialogue_speaker_behavior_cover,
    _dialogue_speaker_behavior_retreat,
    _dialogue_speaker_behavior_follow,
    _dialogue_speaker_behavior_shoot,
    _dialogue_speaker_behavior_clump_idle,
    _dialogue_speaker_behavior_clump_combat,
    _dialogue_speaker_behavior_fought_brutes,
    _dialogue_speaker_behavior_fought_flood
};

// Listener selection for vocalization_pattern::listener_type. Only 0, 1, 3 and 12
// are handled; any other value leaves the listener NONE.
enum e_dialogue_listener_type
{
    _dialogue_listener_unit = 0,
    _dialogue_listener_target = 1,
    _dialogue_listener_target_prop = 3,
    _dialogue_listener_none = 12
};

// Range tests for pattern_match_spatial_relation, shared by
// vocalization_pattern::spatial_relation and the event unit/target relations.
// Value 0 skips the test.
enum e_dialogue_spatial_relation
{
    _dialogue_spatial_none = 0,
    _dialogue_spatial_within_1m = 1,
    _dialogue_spatial_within_2_5m = 2,
    _dialogue_spatial_2_5m_to_8m = 3,
    _dialogue_spatial_beyond_8m = 4,
    _dialogue_spatial_beyond_15m = 5,
    _dialogue_spatial_in_front_within_3m = 6,
    _dialogue_spatial_behind_within_3m = 7,
    _dialogue_spatial_above = 8,
    _dialogue_spatial_below = 9,
    _dialogue_spatial_clump_3_or_fewer = 10,
    _dialogue_spatial_within_weapon_range = 11
};

// Vocalization actions for s_dialogue_vocalization::action. Each value selects the
// animation g_involuntary_vocalization_names[action] plays, directed at the target
// through setup_animation (1, 3, 4, 5, 6, 7, 9) or submitted as an impulse (2, 8, 10, 11).
enum e_dialogue_action
{
    _dialogue_action_none = 0,
    _dialogue_action_shakefist = 1,
    _dialogue_action_cheer = 2,
    _dialogue_action_surprise_front = 3,
    _dialogue_action_surprise_back = 4,
    _dialogue_action_taunt = 5,
    _dialogue_action_brace = 6,
    _dialogue_action_point = 7,
    _dialogue_action_hold = 8,
    _dialogue_action_wave = 9,
    _dialogue_action_advance = 10,
    _dialogue_action_fallback = 11
};

#endif // AI_DIALOGUE_PATTERN_TYPES_H
