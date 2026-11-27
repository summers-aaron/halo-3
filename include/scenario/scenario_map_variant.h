#ifndef SCENARIO_SCENARIO_MAP_VARIANT_H
#define SCENARIO_SCENARIO_MAP_VARIANT_H

#include "objects/object_type_enums.h"

// Functions defined in tu/blamlibxbox/scenario_map_variant.cpp.

class c_bitstream;
class c_map_variant;

extern "C" unsigned __int64 map_variant_compute_unique_id(const c_map_variant *variant);
void map_variant_set_placement_marker_visibility(c_map_variant *map_variant, bool show_objects);
extern "C" void map_variant_update_object_parents(c_map_variant *map_variant);
void map_variant_update_flag_bomb_placements(c_map_variant *map_variant, long game_engine);
extern "C" void map_variant_reset_from_encoding(c_map_variant *map_variant);
extern "C" unsigned char map_variant_decode(c_map_variant *map_variant, c_bitstream *packet);

union real_point3d;
union real_vector3d;
struct s_map_variant_object_simulation_decode_state;
struct s_map_variant_object_simulation_state;
struct s_scenario_multiplayer_object_properties;
struct s_scenario_object_placement;
struct s_variant_multiplayer_object_properties_definition;

extern "C" void map_variant_set_placement_properties(const s_variant_multiplayer_object_properties_definition *properties,
	s_scenario_object_placement *placement);
extern "C" long map_variant_find_quota_index(c_map_variant *map_variant, long definition_index);
extern "C" bool map_variant_reset_transient_state(c_map_variant *map_variant, c_map_variant *other);
extern "C" const s_scenario_multiplayer_object_properties *map_variant_try_and_get_multiplayer_properties(c_map_variant *map_variant, long object_index);
extern "C" bool map_variant_encode_simulation_chunk(c_map_variant *map_variant, long chunk_index, unsigned long *update_mask,
	long state_count, s_map_variant_object_simulation_state *states);
extern "C" void map_variant_update_parented_object_position(c_map_variant *map_variant, long placement_index);
extern "C" void map_variant_get_object_transform(c_map_variant *map_variant, long variant_object_index, real_point3d *position,
	real_vector3d *forward, real_vector3d *up);
extern "C" void map_variant_detach_object(c_map_variant *map_variant, long object_index);
extern "C" bool map_variant_decode_simulation_chunk(c_map_variant *map_variant, long chunk_index, unsigned long update_mask,
	long state_count, const s_map_variant_object_simulation_decode_state *states);
void map_variant_create_helper_object(long placement_index);
extern "C" bool map_variant_can_create_scenario_object(c_map_variant *map_variant, e_object_type object_type,
	long scenario_index, s_variant_multiplayer_object_properties_definition *properties);
const s_scenario_multiplayer_object_properties *scenario_multiplayer_object_properties_try_and_get(e_object_type type, long index);
extern "C" void map_variant_set_scenario_object_removed(c_map_variant *map_variant, e_object_type object_type,
	long scenario_index, const s_variant_multiplayer_object_properties_definition *properties);

#endif // SCENARIO_SCENARIO_MAP_VARIANT_H
