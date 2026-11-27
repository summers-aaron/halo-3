#ifndef GAME_GAME_ENGINE_END_CONDITION_H
#define GAME_GAME_ENGINE_END_CONDITION_H

// Values used by game_engine, hs_function, and the sandbox engine.
// Names below 19 are March k_game_engine_end_conditions strings verbatim.
enum e_game_engine_end_condition
{
    game_engine_end_condition_none = 0,
    hs_function_end_condition_won = 1,
    game_engine_end_condition_round_end_team_scoring = 5,
    game_engine_end_condition_round_end_scoring = 6,
    game_engine_end_condition_game_end_rounds = 7,
    game_engine_end_condition_game_end_rounds_team_early_victory = 8,
    game_engine_end_condition_game_end_rounds_early_victory = 9,
    game_engine_end_condition_game_end_rounds_exceeded = 10,
    game_engine_end_condition_round_end_time_ran_out = 11,
    game_engine_end_condition_game_end_external = 12,
    game_engine_end_condition_game_end_invalid_team_mapping = 13,
    game_engine_end_condition_game_end_exceeded_maximum_rounds = 15,
    game_engine_end_condition_juggernaut_out_of_lives = 17,
    game_engine_end_condition_lone_juggernaut = 18,
    // March k_game_engine_end_conditions[20] "round_end_infection" (Sept drops March [19], shifting 19+ down one).
    game_engine_end_condition_infection = 19,
    // Side-switch CTF ends the round when the defending team is eliminated (c_ctf_engine::update).
    game_engine_ctf_end_condition_20 = 20,
    game_engine_sandbox_end_condition_22 = 22,
    game_engine_end_condition_vip_killed = 23
};

#endif
