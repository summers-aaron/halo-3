#ifndef GAME_GAME_ENGINE_SANDBOX_ENGINE_H
#define GAME_GAME_ENGINE_SANDBOX_ENGINE_H

#include "game/game_engine_default.h"

class c_sandbox_engine : public c_game_engine {
public:
  virtual long get_type(void) const;
  virtual void get_hud_interface_state(long, struct game_engine_interface_state *) const;
  virtual bool initialize_for_new_map(void) const;
  virtual void dispose_from_old_map(void) const;
  virtual bool initialize_for_new_round(void) const;
  virtual void player_added(long player_index) const;
  virtual void player_activated(long player_index) const;
  virtual void player_left(long player_index) const;
  virtual void player_just_spawned(long player_index) const;
  virtual void game_starting(void) const;
  virtual void update(void) const;
  virtual void player_update(long player_index) const;
  virtual void apply_game_engine_traits_for_player(long player_index, c_player_traits *traits) const;
  virtual void handle_deleted_object(long object_index) const;
  virtual void score_header_string(c_static_wchar_string<256> *string) const;
  virtual void player_killed_player(long killer, long dead_unit, long victim, bool betrayal,
                                    c_flags<e_game_engine_kill_flags, long, 24> flags, long streak) const;
  virtual void adjust_object_gravity(long object_index, real_vector3d *gravity) const;
  virtual void prepare_for_new_state(long state) const;
  virtual void player_hump(long player_index) const;
  virtual e_simulation_entity_type get_simulation_entity_type(void) const;
  virtual void promote_to_simulation_authority(void) const;
  virtual void build_simulation_baseline(long, void *) const;
  virtual void build_simulation_update(unsigned long *update_mask, long state_data_size, void *state_data) const;
  virtual bool apply_simulation_update(unsigned long update_mask, long state_data_size, const void *state_data) const;
  virtual bool handle_incoming_simulation_event(e_simulation_event_type event_type, const void *payload) const;

  static void player_delete_object(long player_index, long object_index);
  void toggle_player_mode(long player_index) const;
  static void player_unlock_object(long player_index, float time, bool force_attached);
  static void player_lock_object_for_edit(long player_index, long object_index);
  void update_local_player(long player_index) const;
  static void manipulate_object(long player_index, long object_index);
  void rotate_object(long player_index, long object_index, float x, float y, float z) const;
  static bool shoot_probe_for_player(long player_index, long *hit_object);
};

extern c_sandbox_engine internal_sandbox_engine;

#endif
