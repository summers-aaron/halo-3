#ifndef GAME_GAME_ENGINE_INFECTION_TYPES_H
#define GAME_GAME_ENGINE_INFECTION_TYPES_H

#include "game/game_engine_default.h"
#include "game/game_engine_kill_flags_types.h"
#include "cseries/cseries_wrapped.h"

class c_infection_engine : public c_game_engine
{
public:
    virtual long get_type() const;
    virtual void get_hud_interface_state(long player_index, game_engine_interface_state *state) const;
    virtual bool initialize_for_new_map() const;
    virtual void dispose_from_old_map() const;
    virtual bool initialize_for_new_round() const;
    virtual void stats_reset_for_round_switch() const;
    virtual void player_added(long player_index) const;
    virtual void player_left(long player_index) const;
    virtual void player_changed_indices(long old_index, long new_index) const;
    virtual void player_about_to_spawn(long player_index) const;
    virtual void submit_nav_points(long user_index, long target_player_index) const;
    virtual void update() const;
    virtual void player_update(long player_index) const;
    virtual void apply_game_engine_traits_for_player(long player_index, c_player_traits *player_traits) const;
    virtual void assemble_spawn_influencers_for_player(long player_index, s_netgame_goal_influencer *influencers, long *influencer_count) const;
    virtual e_multiplayer_team_designator get_player_team_designator(long player_index) const;
    virtual bool team_is_enemy(e_game_team team, e_game_team other_team) const;
    virtual bool is_multiplayer_object_type_allowed(long type) const;
    virtual void player_killed_player(long killer, long assistant, long victim, bool betrayal,
        c_flags<e_game_engine_kill_flags, long, 24> flags, long damage) const;
    virtual bool test_flag(long flag) const;
    virtual void prepare_for_new_state(long new_state) const;
    virtual e_simulation_entity_type get_simulation_entity_type() const;
    virtual void build_simulation_baseline(long state_data_size, void *state_data) const;
    virtual void build_simulation_update(unsigned long *update_mask, long state_data_size, void *state_data) const;
    virtual bool apply_simulation_update(unsigned long update_mask, long update_sequence, const void *state_data) const;
    virtual void emit_game_start_event(long player_index) const;
};

extern c_infection_engine internal_infection_engine;

#endif
