#ifndef GAME_GAME_ENGINE_ODDBALL_H
#define GAME_GAME_ENGINE_ODDBALL_H

// Functions defined in tu/blamlibxbox/game_engine_oddball.cpp.

#include "math/real_math_types.h"
#include "memory/tls_globals.h"
#include "game/game_engine_scoring_globals.h"
#include "game/game_engine_default.h"
#include "game/game_engine_kill_flags_types.h"
#include "cseries/cseries_wrapped.h"

bool oddball_find_ball_spawn_position(long ball_index, real_point3d *point, long *weapon_definition, long *object_index);
void oddball_reset_all(void);
void oddball_create_ball(long ball_index);

static __forceinline s_oddball_globals *get_oddball_globals()
{
    return &game_engine_globals->oddball;
}

class c_game_engine_oddball_variant;
extern c_game_engine_oddball_variant *g_oddball_variant;
class c_oddball_engine : public c_game_engine
{
public:
    virtual long get_type() const;
    virtual void get_score_string(long score, c_static_wchar_string<256> *score_string) const;
    virtual void get_hud_interface_state(long player_index, game_engine_interface_state *state) const;
    virtual bool initialize_for_new_map() const;
    virtual void dispose_from_old_map() const;
    virtual void submit_nav_points(long user_index, long player_index) const;
    virtual bool build_player_nav_point(long user_index, long player_index, long target_player_index, bool target,
        s_chud_navpoint *navpoint) const;
    virtual void update() const;
    virtual void apply_game_engine_traits_for_player(long player_index, c_player_traits *traits) const;
    virtual void assemble_spawn_influencers_for_player(long player_index,
        s_netgame_goal_influencer *influencers, long *influencer_count) const;
    virtual bool allow_weapon_pickup(long player_index, long object_index) const;
    virtual bool should_auto_pickup_weapon(long player_index, long weapon_index) const;
    virtual float compute_object_function(long object_index, long function_name) const;
    virtual void multiplayer_weapon_picked_up(long weapon_index, long player_index) const;
    virtual void multiplayer_weapon_dropped(long weapon_index, long player_index) const;
    virtual void player_killed_player(
        long killer, long assistant, long victim, bool teamkill, c_flags<e_game_engine_kill_flags, long, 24> flags, long damage) const;
    virtual void prepare_for_new_state(long new_state) const;
    virtual long get_player_state_index(long player_index, bool *state_is_valid) const;
    virtual e_simulation_entity_type get_simulation_entity_type() const;
    virtual void build_simulation_baseline(long state_data_size, void *state_data) const;
    virtual void build_simulation_update(unsigned long *update_mask, long state_data_size, void *state_data) const;
    virtual bool apply_simulation_update(unsigned long update_mask, long state_data_size, const void *state_data) const;
    virtual bool leader_event_lead_enabled() const;
    virtual void emit_game_start_event(long player_index) const;
};

extern c_oddball_engine internal_oddball_engine;

#endif // GAME_GAME_ENGINE_ODDBALL_H
