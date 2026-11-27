#ifndef AI_OBJECTIVE_TASK_FILTERS_H
#define AI_OBJECTIVE_TASK_FILTERS_H

// Task filters whose predicates are shared by objective squad assignment.
// Actor and vehicle filter names follow the actor type and ai_type_name string_id each one tests.
enum e_task_filter
{
    _task_filter_none = 0,
    _task_filter_highest_spawn_count = 1,
    _task_filter_below_spawn_threshold = 2,
    _task_filter_arbiter = 3,
    _task_filter_player_in_vehicle = 4,
    _task_filter_infantry = 7,
    _task_filter_health_above_threshold = 8,
    _task_filter_health_above_half = 9,
    _task_filter_health_above_three_quarters = 10,
    _task_filter_health_below_threshold = 11,
    _task_filter_health_below_half = 12,
    _task_filter_health_below_three_quarters = 13,
    _task_filter_team_human = 14,
    _task_filter_team_covenant = 15,
    _task_filter_team_flood = 16,
    _task_filter_team_sentinel = 17,
    _task_filter_team_heretic = 18,
    _task_filter_team_prophet = 19,
    _task_filter_elite = 20,
    _task_filter_jackal = 21,
    _task_filter_grunt = 22,
    _task_filter_hunter = 23,
    _task_filter_marine = 24,
    _task_filter_flood_combat = 25,
    _task_filter_flood_carrier = 26,
    _task_filter_sentinel = 27,
    _task_filter_brute = 28,
    _task_filter_prophet = 29,
    _task_filter_bugger = 30,
    _task_filter_flood_pureform = 31,
    _task_filter_guardian = 32,
    _task_filter_sniper = 33,
    _task_filter_vehicle = 34,
    _task_filter_scorpion = 35,
    _task_filter_ghost = 36,
    _task_filter_warthog = 37,
    _task_filter_spectre = 38,
    _task_filter_wraith = 39,
    _task_filter_phantom = 40,
    _task_filter_pelican = 41,
    _task_filter_banshee = 42,
    _task_filter_hornet = 43,
    _task_filter_chopper = 44,
    _task_filter_mauler = 45,
    _task_filter_mongoose = 46
};

#endif // AI_OBJECTIVE_TASK_FILTERS_H
