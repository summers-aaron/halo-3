#ifndef GAME_GAME_ENGINE_DEFAULT_H
#define GAME_GAME_ENGINE_DEFAULT_H

#include "game/game_types.h"

#include "game/team_scoring_method_types.h"
#include "simulation/simulation_queue_event_types.h"

enum e_game_engine_kill_flags;
enum e_simulation_entity_type;
template<long k_maximum_count> class c_static_wchar_string;
template<typename t_enum, typename t_storage, long k_count> class c_flags;
class c_player_traits;
struct game_engine_interface_state;
struct s_chud_navpoint;
struct s_netgame_goal_influencer;
struct s_game_engine_state_data;
struct s_game_engine_event_data;
struct s_file_reference;
union real_vector3d;

// Default game engine; game_engine_default.cpp owns the out-of-line methods.
// Virtual order follows c_slayer_engine's table at 0x82041a68.
class c_game_engine
{
public:
    virtual long get_type() const;                                                       // 0
    virtual long get_score_to_win_round() const;                                         // 1
    virtual void recompute_team_score(e_game_team team, long score_delta, e_team_scoring_method scoring_method) const; // 2, added after March
    virtual void get_score_string(long score, c_static_wchar_string<256> *string) const; // 3
    virtual void get_hud_interface_state(long player_index, game_engine_interface_state *state) const; // 4
    virtual bool initialize_for_new_map() const;                                         // 5
    virtual void dispose_from_old_map() const;                                           // 6
    virtual bool initialize_for_new_round() const;                                       // 7
    virtual void stats_reset_for_round_switch() const;                                   // 8
    virtual bool validate_team_designator_for_new_map(e_multiplayer_team_designator designator) const; // 9
    virtual void player_added(long player_index) const;                                  // 10
    virtual void player_activated(long player_index) const;                              // 11
    virtual void player_left(long player_index) const;                                   // 12
    virtual void player_rejoined(long player_index) const;                               // 13
    virtual void player_changed_indices(long old_player_index, long new_player_index) const; // 14
    virtual void player_changed_teams(long player_index) const;                           // 15
    // March release: f i; the debug body at 82d67d28 is empty.
    virtual void player_about_to_spawn(long player_index) const {}                      // 16
    virtual void player_just_spawned(long player_index) const;                           // 17
    virtual void game_ending() const;                                                   // 18
    virtual void game_starting() const;                                                 // 19
    virtual void render(long user_index) const;                                         // 20
    virtual void render_debug(long user_index) const;                                   // 21
    virtual void submit_nav_points(long user_index, long target_player_index) const;      // 22
    virtual bool build_player_nav_point(long user_index, long player_index, long target_player_index, bool target, s_chud_navpoint *navpoint) const; // 23
    virtual bool should_draw_nav_point(long user_index, long player_index) const;         // 24
    virtual void update() const;                                                       // 25
    virtual void player_update(long player_index) const;                                // 26
    virtual void assemble_baseline_traits_for_player(long player_index, c_player_traits *traits) const; // 27
    virtual void apply_game_engine_traits_for_player(long player_index, c_player_traits *traits) const; // 28
    virtual void assemble_spawn_influencers_for_player(long player_index, s_netgame_goal_influencer *influencers, long *influencer_count) const; // 29
    virtual long compare_players(long player_index, long other_player_index) const;       // 30
    virtual long compare_teams(e_game_team team, e_game_team other_team) const;            // 31
    virtual bool allow_weapon_pickup(long player_index, long object_index) const;         // 32
    virtual bool should_auto_pickup_weapon(long player_index, long object_index) const;   // 33
    virtual void player_nearby_multiplayer_weapon(long player_index, long object_index) const; // 34
    virtual long object_get_emblem_player(long object_index) const;                      // 35
    virtual float compute_object_function(long object_index, long function_name) const;  // 36
    virtual void multiplayer_weapon_register(long object_index) const;                  // 37
    virtual void multiplayer_weapon_deregister(long object_index) const;                // 38
    virtual void multiplayer_weapon_picked_up(long object_index, long player_index) const; // 39
    virtual void multiplayer_weapon_dropped(long object_index, long player_index) const; // 40
    virtual void handle_deleted_object(long object_index) const;                        // 41
    virtual long get_time_left_in_ticks(long user_index, bool a, bool b) const;           // 42
    virtual e_game_team get_defending_team_index() const;                               // 43
    virtual e_multiplayer_team_designator get_player_team_designator(long player_index) const; // 44
    virtual bool team_is_enemy(e_game_team team, e_game_team other_team) const;           // 45
    virtual void score_header_string(c_static_wchar_string<256> *string) const;           // 46
    // Infection rejects multiplayer object types 11 and 12 here.
    virtual bool is_multiplayer_object_type_allowed(long type) const;                                               // 47
    virtual void player_damaged_player(long player_index, long damage_index, long damaged_player_index, bool dead) const; // 48
    // c_game_engine is abstract: every engine overrides these three.
    virtual void player_killed_player(long killer, long dead_unit, long victim, bool betrayal,
                                      c_flags<e_game_engine_kill_flags, long, 24> flags, long streak) const = 0; // 49
    virtual void player_assisted_with_kill(long player_index) const;                     // 50
    virtual long player_killed_player_get_kill_message(long killer, long victim, bool betrayal) const; // 51
    virtual bool test_flag(long flag) const;                                            // 52
    virtual void adjust_object_gravity(long object_index, real_vector3d *gravity) const;  // 53
    virtual void prepare_for_new_state(long state) const;                               // 54
    virtual bool should_end_round(long *reason) const;                                  // 55
    virtual long get_player_state_index(long player_index, bool *valid) const;            // 56
    virtual bool should_purge_multiplayer_item(long object_index) const;                 // 57
    virtual void player_hump(long player_index) const;                                  // 58
    virtual e_simulation_entity_type get_simulation_entity_type() const;                // 59
    virtual void promote_to_simulation_authority() const;                               // 60
    virtual void recover_state_before_promotion() const;                                // 61
    virtual void build_global_baseline(s_game_engine_state_data *state_data) const;       // 62
    virtual void build_global_update(unsigned long update_mask, unsigned long *actual_update_mask, s_game_engine_state_data *state_data) const; // 63
    virtual bool apply_global_update(unsigned long update_mask, const s_game_engine_state_data *state_data) const; // 64
    virtual void build_simulation_baseline(long state_data_size, void *state_data) const; // 65
    virtual void build_simulation_update(unsigned long *update_mask, long state_data_size, void *state_data) const = 0; // 66
    virtual bool apply_simulation_update(unsigned long update_mask, long state_data_size, const void *state_data) const = 0; // 67
    virtual void build_player_baseline(short player_index, long state_data_size, void *state_data) const; // 68
    virtual void build_player_update(short player_index, unsigned long *update_mask, long state_data_size, void *state_data) const; // 69
    virtual bool apply_player_update(short player_index, unsigned long update_mask, long state_data_size, const void *state_data) const; // 70
    virtual long parse_multiplayer_string_token(long player_index, const wchar_t *token, long token_length,
                                                const s_game_engine_event_data *event, wchar_t *buffer, long buffer_size) const; // 71
    virtual bool handle_incoming_simulation_event(e_simulation_event_type event_type, const void *payload) const; // 72
    // The leader-scoring calls establish the return type of both; neither is in March's class.
    virtual bool leader_event_enabled() const;                                         // 73
    virtual bool leader_event_lead_enabled() const;                                    // 74
    // Every engine keeps this base stub, which returns NONE; nothing calls it.
    virtual long get_none_index() const;                                                // 75
    virtual long get_message_chud_reference() const;                                   // 76
    // player_update calls this when the crouch state changes.
    virtual void player_changed_crouch_state(long player_index) const;                  // 77
    virtual void emit_game_start_event(long player_index) const;                        // 78, added after March

    void dump_player_trait_settings(const char *name, const c_player_traits *traits, s_file_reference *file) const;
};

typedef char game_engine_size[(sizeof(c_game_engine) == sizeof(void *)) ? 1 : -1];

#endif // GAME_GAME_ENGINE_DEFAULT_H
