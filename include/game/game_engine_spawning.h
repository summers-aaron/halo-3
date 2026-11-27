#ifndef GAME_GAME_ENGINE_SPAWNING_H
#define GAME_GAME_ENGINE_SPAWNING_H

// Functions defined in tu/blamlibxbox/game_engine_spawning.cpp.

union real_point3d;

bool game_engine_unit_can_spawn_at_location_unobstructed(long unit_definition_index, const real_point3d *location, bool);
long game_engine_find_best_player_respawn_object_index(long player_index, bool);
void game_engine_add_starting_equipment(long unit_index);
bool game_engine_should_spawn_player(long player_index);
bool game_engine_limited_lives_active(void);
extern "C" void game_engine_get_spawn_object_location(long object_index, long unit_definition_index, real_point3d *location);
void game_engine_find_available_spawn_location_objects(long player_index, unsigned long team_player_index,
    unsigned long match_flags, long *object_indices, long *count);
extern "C" bool game_engine_compare_spawn_scores(long index_a, long index_b, const void *context);

#endif // GAME_GAME_ENGINE_SPAWNING_H
