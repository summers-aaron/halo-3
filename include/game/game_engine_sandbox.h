#ifndef GAME_GAME_ENGINE_SANDBOX_H
#define GAME_GAME_ENGINE_SANDBOX_H

// Functions defined in tu/blamlibxbox/game_engine_sandbox.cpp.

bool game_engine_is_object_being_edited(long object_index, long * result_index);
void game_engine_sandbox_start_editing_object(long player_index, long variant_object_index);
void game_engine_sandbox_stop_editing_object(long variant_object_index, float time, bool force_attached);
void game_engine_sandbox_update_budget_quotas(long player_index, long definition_index, long maximum_count, long minimum_count);

union real_point3d;

extern "C" void game_engine_sandbox_end_round_with_value(int player_index, int value);
extern "C" void game_engine_sandbox_toggle_player_mode(unsigned long player_index);

extern "C" bool game_engine_sandbox_is_active(void);
extern "C" bool game_engine_sandbox_spawn_monitor_for_player(long player_index);
extern "C" int game_engine_sandbox_find_player_original_unit(int p0);
extern "C" void game_engine_sandbox_spawn_object_for_player(long definition_index, long player_index, const real_point3d *point);

class c_game_engine;
class c_game_engine_sandbox_variant;
class c_map_variant;
class c_player_traits;
union real_vector3d;


extern "C" bool game_engine_sandbox_is_player_in_edit_mode(unsigned long player_index);
long get_player_who_is_editing_object(long object_index);
extern "C" unsigned char game_engine_sandbox_is_object_tracked(long object_index);
extern "C" bool game_engine_sandbox_is_object_fixed(long object_index);
extern "C" bool game_engine_sandbox_is_object_soft_ceiling_limited(long object_index);
void get_offset_to_bounding_sphere_center(long object_index, union real_vector3d * offset);
extern "C" void game_engine_sandbox_set_object_connected(unsigned long object_index, long is_connected);
void snapshot_object_position(bool update, long object_index);
extern "C" void game_engine_sandbox_compute_budget_total(c_map_variant *state);
extern "C" float game_engine_sandbox_get_object_radius(unsigned long object_index);
void sandbox_update_object_position(bool update, long object_index, union real_point3d const * origin,
	union real_vector3d const * forward, union real_vector3d const * up);
extern "C" void game_engine_sandbox_track_object(int p0);
extern "C" void game_engine_sandbox_adjust_quota_counts(int quota_index, int delta);
extern "C" void game_engine_sandbox_request_properties_change(long player_index, long variant_object_index, const int *properties);
extern "C" void game_engine_sandbox_update_variant_objects_for_engine(c_map_variant *map_variant);
extern "C" void game_engine_sandbox_request_object_move(long object_index, float time);
extern "C" bool game_engine_sandbox_remove_tracked_object(long object_index, long tracked_index);
extern "C" void game_engine_sandbox_dispose_tracked_object(long object_index, long tracked_index, bool immediate);
extern "C" void game_engine_sandbox_handle_player_input(const c_game_engine *unused, long player_index);


extern "C" c_game_engine_sandbox_variant *g_sandbox_editor_variant;
extern "C" float g_sandbox_default_edit_distance;
extern "C" long g_sandbox_edit_effect_index;

extern "C" void game_engine_sandbox_update_variant_objects(c_map_variant *variant);
extern "C" bool game_engine_sandbox_find_placement_point(long definition_index, real_point3d *point, long ignore_object_index);
extern "C" void game_engine_sandbox_update_tracked_objects(void);

enum e_player_editor_request_type;
void simulation_request_player_editor(long player_index, e_player_editor_request_type request_type, bool enabled, long object_index);


#endif // GAME_GAME_ENGINE_SANDBOX_H
