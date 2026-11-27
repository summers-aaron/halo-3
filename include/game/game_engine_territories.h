#ifndef GAME_GAME_ENGINE_TERRITORIES_H
#define GAME_GAME_ENGINE_TERRITORIES_H

#include "game/game_engine_default.h"
#include "game/game_engine_kill_flags_types.h"
#include "cseries/cseries_wrapped.h"

// Functions defined in tu/blamlibxbox/game_engine_territories.cpp.

float blink(float minimum, float maximum, float frequency);
void update_territory_flag_color(long territory_index);
long create_territory_flag(long territory_index);
void update_territory(long territory_index);


class c_territories_engine : public c_game_engine
{
public:
    virtual long get_type() const;
    virtual long get_score_to_win_round() const;
    virtual void get_hud_interface_state(long player_index, struct game_engine_interface_state *state) const;
    virtual bool initialize_for_new_map() const;
    virtual bool initialize_for_new_round() const;
    virtual void submit_nav_points(long user_index, long target_player_index) const;
    virtual void update() const;
    virtual void apply_game_engine_traits_for_player(long player_index, class c_player_traits *traits) const;
    virtual void assemble_spawn_influencers_for_player(long player_index, struct s_netgame_goal_influencer *influencers, long *count) const;
    virtual bool allow_weapon_pickup(long player_index, long object_index) const;
    virtual void multiplayer_weapon_register(long weapon_index) const;
    virtual long get_time_left_in_ticks(long time_left, bool include_sudden_death, bool include_grace_period) const;
    virtual e_game_team get_defending_team_index() const;
    virtual void player_killed_player(
        long killer, long victim, long dead_unit, bool suicide, c_flags<e_game_engine_kill_flags, long, 24> kill_flags, long damage) const;
    virtual bool should_end_round(long *winner) const;
    virtual e_simulation_entity_type get_simulation_entity_type() const;
    virtual void recover_state_before_promotion() const;
    virtual void build_simulation_baseline(long state_size, void *state) const;
    virtual void build_simulation_update(unsigned long *update_mask, long state_data_size, void *state_data) const;
    virtual bool apply_simulation_update(unsigned long update_mask, long state_data_size, const void *state_data) const;
    virtual long parse_multiplayer_string_token(long type, const wchar_t *token, long token_length,
        const s_game_engine_event_data *event_data, wchar_t *buffer, long buffer_size) const;
    virtual bool leader_event_enabled() const;
    virtual void emit_game_start_event(long player_index) const;
};

extern c_territories_engine internal_territories_engine;

#endif // GAME_GAME_ENGINE_TERRITORIES_H
