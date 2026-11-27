#ifndef GAME_GAME_ENGINE_CTF_H
#define GAME_GAME_ENGINE_CTF_H

#include "game/game_types.h"


#include "game/game_engine_default.h"

struct s_static_spawn_zone;
struct s_multiplayer_weapon_tracker;

// c_ctf_engine adds ten CTF-specific virtuals after the base game-engine virtuals,
// declared below in that order (79 static_spawn_zone_is_valid_for_player .. 88 verify_state).
class c_ctf_engine : public c_game_engine {
public:
    virtual long get_type() const;
    virtual void get_hud_interface_state(long player_index, struct game_engine_interface_state *state) const;
    virtual bool initialize_for_new_map() const;
    virtual void dispose_from_old_map() const;
    virtual bool initialize_for_new_round() const;
    virtual bool validate_team_designator_for_new_map(e_multiplayer_team_designator team_designator) const;
    virtual void player_activated(long player_index) const;
    virtual void game_starting() const;
    virtual void submit_nav_points(long user_index, long target_player_index) const;
    virtual bool build_player_nav_point(long user_index, long player_index, long target_player_index, bool target, s_chud_navpoint *navpoint) const;
    virtual void update() const;
    virtual void player_update(long player_index) const;
    virtual void apply_game_engine_traits_for_player(long player_index, c_player_traits *traits) const;
    virtual void assemble_spawn_influencers_for_player(long player_index, s_netgame_goal_influencer *influencers, long *influencer_count) const;
    virtual long compare_teams(e_game_team team_a, e_game_team team_b) const;
    virtual bool allow_weapon_pickup(long player_index, long object_index) const;
    virtual long object_get_emblem_player(long object_index) const;
    virtual void multiplayer_weapon_register(long weapon_index) const;
    virtual void multiplayer_weapon_picked_up(long weapon_index, long unit_index) const;
    virtual void multiplayer_weapon_dropped(long weapon_index, long unit_index) const;
    virtual long get_time_left_in_ticks(long time, bool advance, bool sudden_death) const;
    virtual e_game_team get_defending_team_index() const;
    virtual void player_killed_player(long killer_player_index, long object_index, long victim_player_index, bool betrayal,
        c_flags<e_game_engine_kill_flags, long, 24> kill_flags, long damage) const;
    virtual long player_killed_player_get_kill_message(long killer_player_index, long killed_player_index, bool is_betrayal) const;
    virtual bool test_flag(long flag) const;
    virtual void prepare_for_new_state(long state) const;
    virtual e_simulation_entity_type get_simulation_entity_type() const;
    virtual void promote_to_simulation_authority() const;
    virtual void recover_state_before_promotion() const;
    virtual void build_simulation_baseline(long state_data_size, void *state_data) const;
    virtual void build_simulation_update(unsigned long *flags, long state_data_size, void *state_data) const;
    virtual bool apply_simulation_update(unsigned long update_mask, long state_data_size, const void *state_data) const;
    virtual bool leader_event_enabled() const;
    virtual void emit_game_start_event(long player_index) const;

    virtual bool static_spawn_zone_is_valid_for_player(long player_index, const s_static_spawn_zone *zone) const;
    virtual long get_object_definition_index() const;
    virtual void process_player_holding_object(long player_index, long identifier) const;
    virtual bool update_object(const s_multiplayer_weapon_tracker *tracker) const;
    virtual void initialize_object_data(long identifier) const;
    virtual short get_sudden_death_time() const;
    virtual void emit_object_returned_event(long identifier, bool returned) const;
    virtual bool object_should_exist(long identifier) const;
    // True when the object's team is near it; other teams are ignored.
    virtual bool is_team_near_object(long object_index, float radius) const;
    virtual bool verify_state() const;

    static bool team_flag_is_at_home(e_multiplayer_team_designator team, long *flag_weapon_index);
};
// Functions defined in tu/blamlibxbox/game_engine_ctf.cpp.

struct s_chud_navpoint;

bool ensure_all_flags_exist(void);
bool ctf_engine_build_flag_nav_point(s_chud_navpoint *navpoint, long object_index, bool carried, bool at_home, bool away);
bool team_near_object(long object_index, float distance, bool include_all);

struct game_engine_interface_state;

extern "C" void game_engine_ctf_update_sudden_death_state(long player_index, game_engine_interface_state *state);
extern "C" bool game_engine_ctf_team_has_flag_carrier(long team_index);

class c_area;
union real_point3d;

void create_all_flags(void);
void flag_reset(long flag_object_index, bool returned);
void side_switch_ctf_generate_side_event(long player_index);
extern "C" bool player_within_distance_of_position(long player_index, const real_point3d *point, float distance_squared);
extern "C" bool player_in_enemy_area(long player_index, c_area **area);
void create_flag(long flag_index);
void player_score(long player_index, e_game_team team);
void side_switch_ctf_switch_sides(e_game_team team);

extern c_ctf_engine internal_ctf_engine;

#endif // GAME_GAME_ENGINE_CTF_H
