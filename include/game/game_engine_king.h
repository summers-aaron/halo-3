#ifndef GAME_GAME_ENGINE_KING_H
#define GAME_GAME_ENGINE_KING_H

#include "game/game_engine_king_settings_types.h"
#include "game/game_engine_default.h"

extern "C" {
// Legacy call signature retained with the koth-traits owner.
short king_get_hill_move_in_seconds(c_game_engine_king_variant *variant);
}

class c_king_engine : public c_game_engine {
public:
    virtual long get_type() const;
    virtual e_simulation_entity_type get_simulation_entity_type() const;
    virtual bool initialize_for_new_round() const;
    virtual void promote_to_simulation_authority() const;
    virtual bool leader_event_lead_enabled() const;
    virtual void player_killed_player(
        long killer, long assistant, long victim, bool betrayal, c_flags<e_game_engine_kill_flags, long, 24> flags, long damage) const;
    virtual float compute_object_function(long object_index, long function_name) const;
    virtual void dispose_from_old_map() const;
    virtual void get_hud_interface_state(long player_index, game_engine_interface_state *state) const;
    virtual void score_header_string(c_static_wchar_string<256> *string) const;
    virtual void apply_game_engine_traits_for_player(long player_index, c_player_traits *player_traits) const;
    virtual void player_about_to_spawn(long player_index) const;
    virtual void build_simulation_baseline(long state_data_size, void *state_data) const;
    virtual void build_simulation_update(unsigned long *update_mask, long state_data_size, void *state_data) const;
    virtual bool apply_simulation_update(unsigned long update_mask, long state_data_size, const void *state_data) const;
    virtual void recompute_team_score(e_game_team team, long score_delta, e_team_scoring_method scoring_method) const;
    virtual void emit_game_start_event(long player_index) const;
    virtual bool initialize_for_new_map() const;
    virtual void update() const;
    virtual void player_update(long player_index) const;
    virtual void submit_nav_points(long user_index, long player_index) const;
};

extern c_king_engine internal_king_engine;

// Functions defined in tu/blamlibxbox/game_engine_king.cpp.

unsigned short teams_in_hill_from_players_in_hill(unsigned short players_in_hill);
void king_engine_handle_hill_messages(long old_players_in_hill, long new_players_in_hill);
void king_engine_update_scoring(void);

struct player_datum;
player_datum *king_engine_player_get(long player_index);

template<typename t_type> unsigned long count_bits(t_type value);
template<> unsigned long count_bits<unsigned long>(unsigned long value);

#endif // GAME_GAME_ENGINE_KING_H
