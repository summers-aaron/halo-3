#ifndef GAME_GAME_ENGINE_JUGGERNAUT_H
#define GAME_GAME_ENGINE_JUGGERNAUT_H

#include "game/game_types.h"
#include "game/game_engine_default.h"

struct game_engine_interface_state;

class c_juggernaut_engine : public c_game_engine {
public:
    virtual long get_type() const;
    virtual void get_hud_interface_state(long player_index, game_engine_interface_state *state) const;
    virtual bool initialize_for_new_map() const;
    virtual bool initialize_for_new_round() const;
    virtual void player_changed_indices(long old_player_index, long new_player_index) const;
    virtual void player_about_to_spawn(long player_index) const;
    virtual void submit_nav_points(long user_index, long target_player_index) const;
    virtual bool build_player_nav_point(long user_index, long player_index, long target_player_index,
    bool target, s_chud_navpoint *navpoint) const;
    virtual void update() const;
    virtual void apply_game_engine_traits_for_player(long player_index, c_player_traits *traits) const;
    virtual bool team_is_enemy(e_game_team team, e_game_team other_team) const;
    virtual void player_killed_player(
        long killer, long assistant, long victim, bool betrayal, c_flags<e_game_engine_kill_flags, long, 24> kill_flags, long damage) const;
    virtual e_simulation_entity_type get_simulation_entity_type() const;
    virtual void promote_to_simulation_authority() const;
    virtual void build_simulation_baseline(long state_data_size, void *state_data) const;
    virtual void build_simulation_update(unsigned long *update_flags, long player_index, void *state) const;
    virtual bool apply_simulation_update(unsigned long update_mask, long state_data_size, const void *state_data) const;
    virtual void emit_game_start_event(long player_index) const;
};

extern c_juggernaut_engine internal_juggernaut_engine;

// Functions defined in tu/blamlibxbox/game_engine_juggernaut.cpp.

void randomly_choose_juggernaut();
void set_new_juggernaut(long player_index);
void update_delay_timers();
void player_became_juggernaut(long player_index);
bool player_is_juggernaut(long player_index);
void cycle_zones(bool send_event);

#endif // GAME_GAME_ENGINE_JUGGERNAUT_H
