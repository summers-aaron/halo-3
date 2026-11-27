#ifndef GAME_GAME_ENGINE_VIP_H
#define GAME_GAME_ENGINE_VIP_H

#include "game/game_types.h"
#include "game/game_engine_default.h"

// Functions defined in tu/blamlibxbox/game_engine_vip.cpp.

struct vip_s_implicit_geometry_shape;

extern "C" void multiplayer_object_build_boundary_primitive(const vip_s_implicit_geometry_shape *shape, const void *color, long object_index);
void calculate_influence_boundary_geometry(unsigned int object_index, void *area, int unused);
extern "C" void vip_select_next_destination_zone(unsigned char send_event);
void sanity_check_vips();
bool player_was_vip(long player_index, e_multiplayer_team_designator *team_designator);
bool player_is_vip(long player_index, e_multiplayer_team_designator *team_designator);
void randomly_choose_vip(e_multiplayer_team_designator team_designator);
void set_team_vip(e_multiplayer_team_designator team_designator, long player_index);

// March game_engine_vip.cpp: VIP overrides the shared default-engine interface.
class c_vip_engine : public c_game_engine
{
public:
    virtual long get_type() const;
    virtual void get_hud_interface_state(long player_index, game_engine_interface_state *interface_state) const;
    virtual bool initialize_for_new_map() const;
    virtual bool initialize_for_new_round() const;
    virtual void player_changed_indices(long old_player_index, long new_player_index) const;
    virtual void player_changed_teams(long player_index) const;
    virtual void player_about_to_spawn(long player_index) const;
    virtual void player_just_spawned(long player_index) const;
    virtual void submit_nav_points(long user_index, long target_player_index) const;
    virtual bool build_player_nav_point(long user_index, long player_index, long target_player_index, bool target, s_chud_navpoint *navpoint) const;
    virtual void update() const;
    virtual void player_update(long player_index) const;
    virtual void apply_game_engine_traits_for_player(long player_index, c_player_traits *traits) const;
    virtual e_game_team get_defending_team_index() const;
    virtual void player_killed_player(long killer_player_index, long object_index, long victim_player_index, bool betrayal,
        c_flags<e_game_engine_kill_flags, long, 24> kill_flags, long damage) const;
    virtual e_simulation_entity_type get_simulation_entity_type() const;
    virtual void promote_to_simulation_authority() const;
    virtual void recover_state_before_promotion() const;
    virtual void build_simulation_baseline(long state_data_size, void *state_data) const;
    virtual void build_simulation_update(unsigned long *update_mask, long state_data_size, void *state_data) const;
    virtual bool apply_simulation_update(unsigned long update_mask, long state_data_size, const void *state_data) const;
    virtual void emit_game_start_event(long player_index) const;
};

typedef char vip_engine_size[(sizeof(c_vip_engine) == sizeof(c_game_engine)) ? 1 : -1];

extern c_vip_engine internal_vip_engine;

#endif // GAME_GAME_ENGINE_VIP_H
