#ifndef SCENARIO_SCENARIO_MAP_VARIANT_EXTERNS_H
#define SCENARIO_SCENARIO_MAP_VARIANT_EXTERNS_H

class c_map_variant;

extern "C" bool player_is_on_local_machine(long player_index);
extern "C" bool game_engine_sandbox_update_variant_object_properties(c_map_variant *variant, long object_index);
extern "C" void game_engine_apply_map_variant_quotas(void *info, const void *map_variant);

#endif // SCENARIO_SCENARIO_MAP_VARIANT_EXTERNS_H
