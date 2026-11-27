#ifndef GAME_GAME_ENGINE_ASSAULT_H
#define GAME_GAME_ENGINE_ASSAULT_H

#include "game/game_types.h"


#include "game/game_engine_ctf.h"

class c_assault_engine : public c_ctf_engine {
public:
    virtual long get_type() const;
    virtual e_simulation_entity_type get_simulation_entity_type() const;
    virtual void get_hud_interface_state(long player_index, struct game_engine_interface_state *state) const;
    virtual bool initialize_for_new_map() const;
    virtual void dispose_from_old_map() const;
    virtual void submit_nav_points(long user_index, long target_player_index) const;
    virtual bool build_player_nav_point(long user_index, long player_index, long target_player_index, bool target, s_chud_navpoint *navpoint) const;
    virtual void apply_game_engine_traits_for_player(long player_index, c_player_traits *traits) const;
    virtual void assemble_spawn_influencers_for_player(long player_index, s_netgame_goal_influencer *influencers, long *influencer_count) const;
    virtual bool allow_weapon_pickup(long player_index, long object_index) const;
    virtual void multiplayer_weapon_picked_up(long weapon_index, long unit_index) const;
    virtual void multiplayer_weapon_dropped(long weapon_index, long unit_index) const;
    virtual void player_killed_player(long killer_player_index, long object_index, long victim_player_index, bool betrayal,
        c_flags<e_game_engine_kill_flags, long, 24> kill_flags, long damage) const;
    virtual long player_killed_player_get_kill_message(long killer_player_index, long killed_player_index, bool is_betrayal) const;
    virtual void build_simulation_baseline(long state_data_size, void *state_data) const;
    virtual void build_simulation_update(unsigned long *flags, long state_data_size, void *state_data) const;
    virtual bool apply_simulation_update(unsigned long update_mask, long state_data_size, const void *state_data) const;
    virtual void emit_game_start_event(long player_index) const;

    virtual long get_object_definition_index() const;
    virtual void process_player_holding_object(long player_index, long object_index) const;
    virtual bool update_object(const s_multiplayer_weapon_tracker *object_tracker) const;
    virtual void initialize_object_data(long object_index) const;
    virtual short get_sudden_death_time() const;
    virtual void emit_object_returned_event(long object_index, bool dropped) const;
    virtual bool object_should_exist(long team) const;
    virtual bool is_team_near_object(long object_index, float distance) const;
    virtual bool verify_state() const;
};
extern c_assault_engine internal_assault_engine;

// Functions defined in tu/blamlibxbox/game_engine_assault.cpp.

float assault_bomb_get_arming_theta(long unit_index);

class c_area;

extern "C" void assault_reset_bomb(long bomb_index);
void explode_bomb(long bomb_index);
void bomb_place_on_enemy_post(long weapon_index, long player_index, c_area *area);
extern "C" void assault_trigger_bomb_effect(long weapon_index, long effect_index, long sound_index, long damage_index, long effect_type);

struct s_multiplayer_object_boundary_geometry_data;

extern "C" bool team_in_own_goal_area(const s_multiplayer_object_boundary_geometry_data *boundary, e_multiplayer_team_designator team, long *out);

#endif // GAME_GAME_ENGINE_ASSAULT_H
