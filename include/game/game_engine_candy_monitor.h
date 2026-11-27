#ifndef GAME_GAME_ENGINE_CANDY_MONITOR_H
#define GAME_GAME_ENGINE_CANDY_MONITOR_H

// Functions defined in tu/blamlibxbox/game_engine_candy_monitor.cpp.

long game_engine_remap_weapon_definition_index(long definition_index);
void game_engine_register_object(long object_index);

class c_multiplayer_candy_monitor_manager;

extern "C" long game_engine_remap_vehicle_definition_index(long definition_index);
extern "C" void game_engine_candy_monitor_delete_entry(int object_name);
extern "C" long game_engine_candy_monitor_count_quota_objects(c_multiplayer_candy_monitor_manager *manager, long quota_index, long definition_index);
extern "C" bool game_engine_object_is_marked_item(long object_index);

union real_point3d;

class c_map_variant;
enum e_object_type;

extern "C" int game_engine_candy_monitor_count_scenario_objects(
    const c_map_variant *map_variant, e_object_type object_type, long placement_index);
long object_get_respawn_time_seconds(long variant_index);
long scenario_object_get_abandonment_time_seconds(long object_index);
bool game_engine_object_has_been_abandoned(long object_index);
bool game_engine_player_is_nearby(const real_point3d *point, float radius);
bool game_engine_player_is_looking_at_sphere(long player_index, const real_point3d *center, float radius);
bool game_engine_vehicle_has_passengers(long vehicle_index);
extern "C" bool game_engine_object_is_spent_weapon(long object_index);



// Older callers pass a third look-radius argument, which is ignored.
bool game_engine_any_player_nearby_and_looking_at_sphere(const real_point3d *point, float radius);
static __forceinline int game_engine_players_nearby_and_looking_at_sphere(const real_point3d *point, float nearby_radius, float look_radius)
{
    typedef int (*t_legacy_sphere_query)(const real_point3d *, float, float);
    return reinterpret_cast<t_legacy_sphere_query>(game_engine_any_player_nearby_and_looking_at_sphere)(point, nearby_radius, look_radius);
}
extern "C" void game_engine_candy_monitor_attach_variant_object(long object_index, long unused);

#endif // GAME_GAME_ENGINE_CANDY_MONITOR_H
