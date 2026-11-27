// blamlibxbox/scenario_map_variant.obj: from the records 0x1ce478 0x1ce628 0x1ce7e8 0x1ce8d0 0x1ce960 0x1cecb8 0x1cef80 0x1cf728 0x1cf8f0 0x1cfe20 0x1cfe70 0x1cff58 0x1d0388 0x1d08b0 0x1d0c48 0x1d0e00 0x1d0f88 0x1d11f8 0x1d1650 0x1d1a18 0x1d1ab0 0x1d1f20 0x1d20b8 0x1d2180 0x1d23f0 0x1d24c8 0x1d2758 0x1d2810 0x1d2878 0x1d2928 0x1d3140 0x1d3820 0x1d4038 0x1d4480 0x1d44e0 0x1d46a0 0x1d4930 0x1d4a28 0x1d4ed0 0x1d53b8
// @compiler xdk6021
// @address 0x7c5d34 k_null_rectangle3d_storage
// @flags /TP /O2 /Oc
#include "objects/object_name_list.h"
#include "scenario/scenario_map_variant_externs.h"
#include "math/real_math_bounds_storage.h"
#include "scenario/scenario.h"
#include "game/game_engine_list.h"
#include "objects/object_lifecycle.h"
#include "math/real_math_globals.h"
#include "memory/tls_data_arrays.h"
#include "tag_files/tag_groups.h"
#include "memory/tls_globals.h"
#include "math/matrix_math_types.h"
#include "memory/data_types.h"
#include "math/real_math_types.h"
#include "tag_files/tag_types.h"
#include "objects/object_types.h"
#include "game/game_types.h"
#include "memory/static_arrays.h"
#include "objects/object_placement.h"
#include "objects/object_iterator.h"
#include "objects/multiplayer_object_types.h"
#include "cseries/crt_xdk.h"
#if !defined(_XBOX) || defined(__clang__)
#include "cseries/byte_swapping.h"
#endif
#include <string.h>
#include <stdlib.h>
#include "objects/objects.h"
#include "objects/object_header.h"
#include "objects/object_header_access.h"
#include "objects/object_type_access.h"
#include "objects/object_node_matrices_inline.h"
#include "memory/data.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "cache/security_functions.h"
#include "game/game_engine_candy_monitor.h"
#include "game/game_engine_sandbox.h"
#include "game/game_engine_util.h"
#include "items/weapons.h"
#include "simulation/simulation_encoding.h"
#include "simulation/simulation_gamestate_entities.h"
#include "game/game_engine.h"
#include "game/game_engine_get_inline.h"
#include "main/levels.h"
#include "networking/logic/network_session_interface.h"
#include "objects/damage.h"
#include "objects/object_early_movers.h"
#include "scenario/scenario_map_variant.h"
#include "memory/crc.h"
#include "render/render_lens_flares.h"
#include "memory/bitstream.h"
#include "game/map_variant.h"
#include "game/game_globals_storage.h"
#include "items/items.h"
#include "saved_games/saved_game_files.h"
#include "saved_games/saved_game_files_operations.h"
#include "simulation/simulation_gamestate_entities.h"
#include "memory/data_types.h"
#include "game/game_engine_default.h"
#include "cache/cache_file_globals.h"
#include "objects/object_type_definition.h"
#include <new>
#include "math/real_math.h"
#include "math/matrix_math.h"
#include "main/levels_globals.h"
#include "game/game_engine_scoring_globals.h"
#include "game/game_simulation_predicates.h"
#include "scenario/scenario_light_types.h"
#include "scenario/scenario_tag.h"
#include "objects/object_datum.h"
#include "items/weapon_definitions.h"
#include "ai/weapon_datum_types.h"
#include "objects/object_definitions.h"
#include "math/real_math_constants.h"
#include "scenario/scenario_map_variant_types.h"
#include "game/player_in_game_iterator.h"
#include "scenario/scenario_objects.h"
// @address 0x1cff58 ?encode@c_map_variant@@QBAXPAVc_bitstream@@@Z
// @address 0x221030 ?compare_axes@c_bitstream@@SA_NPBTreal_vector3d@@000@Z
// @address 0x19f058 ?security_calculate_hash@@YA_NPBDK_NPAUs_network_http_request_hash@@@Z
// @address 0x3ade48 ?simulation_compare_quantized_positions@@YA_NPBTreal_point3d@@0JPBTreal_rectangle3d@@@Z
// @address 0x1ce7e8 map_variant_set_placement_properties
// @address 0x1ce8d0 ??0c_map_variant@@QAA@XZ
// @address 0x1ce960 ?create_default@c_map_variant@@QAAXW4e_map_id@@@Z
// @address 0x1cecb8 map_variant_compute_unique_id
// @address 0x1cf8f0 ?initialize_for_scenario@c_map_variant@@QAAXXZ
// @address 0x1d0f88 ?adjust_active_object_count@c_map_variant@@QAAXJJ@Z
// @address 0x1d11f8 ?register_scenario_object@c_map_variant@@QAA_NJ_N@Z
// @address 0x1d1650 ?register_object@c_map_variant@@QAAXJJ@Z
// @address 0x1d1a18 map_variant_set_scenario_object_removed
// @address 0x1d20b8 ?get_variant_object_datum@c_map_variant@@QBA_NJPAUs_variant_object_datum@@@Z
// @address 0x1d2758 ?set_variant_object_datum@c_map_variant@@QAAXJPBUs_variant_object_datum@@_N@Z
// @address 0x1d2810 ?get_object_index@c_map_variant@@QBAJW4e_object_type@@J@Z
// @address 0x1d2878 ?get_object_scenario_properties@c_map_variant@@QAA_NJPAW4e_object_type@@PAJ@Z
// @address 0x1d2928 map_variant_try_and_get_multiplayer_properties
// @address 0x1d4038 ?save_to@c_map_variant@@QBAXPAV1@@Z
// @address 0x1d4480 ?apply_multiplayer_properties@c_map_variant@@QBAXJJ@Z
// @address 0x1d44e0 ?apply_multiplayer_properties@c_map_variant@@QBAXPBUs_variant_multiplayer_object_properties_definition@@J@Z
// @address 0x1d46a0 ?setup_object_properties@c_map_variant@@SAXPAUs_variant_object_datum@@@Z
// @address 0x1d4930 ?map_variant_set_placement_marker_visibility@@YAXPAVc_map_variant@@_N@Z
// @address 0x1d53b8 ?scenario_multiplayer_object_properties_try_and_get@@YAPBUs_scenario_multiplayer_object_properties@@W4e_object_type@@J@Z

struct s_variant_multiplayer_object_properties_definition;
class c_bitstream;
union real_point3d;
union real_rectangle3d;
struct s_variant_object_datum;
union real_vector3d;







class c_bitstream;





// an object's identity in a scenario (unique id, origin bsp, type and source)





struct s_datum;

// the multiplayer properties of a scenario placement (only the quota counts are read here)


// the multiplayer properties of a placed object (an object_header_block)
// the start of every object definition: the type of the object it defines

#define SCENARIO_MAP_VARIANT_QUOTA_MAX(a, b) ((a) > (b) ? (a) : (b))
#define SCENARIO_MAP_VARIANT_QUOTA_MIN(a, b) ((a) < (b) ? (a) : (b))
struct cache_file_tag_instance;
// @address 0x2251c k_real_zero_storage
struct s_network_http_request_hash;




struct s_map_variant_object_simulation_state {
  unsigned short game_engine_flags;
  unsigned char placement_flags;
  unsigned char team_designator;
  unsigned char spare_clips;
  unsigned char spawn_rate;
  unsigned char teleporter_channel;
  char boundary_shape;
  float boundary[4];
  long quota_object_definition_index;
  unsigned char quota_placed_on_map;
  unsigned char quota_maximum_count;
  unsigned char quota_minimum_count;
  real_point3d position;
  real_vector3d forward;
  real_vector3d up;
  unsigned short flags;
  long entity_index;
  long pad4c;
  long parent_entity_index;
  long pad54;
};
typedef char s_map_variant_object_simulation_state_position_offset[(offsetof(s_map_variant_object_simulation_state, position) == 0x20) ? 1 : -1];
typedef char s_map_variant_object_simulation_state_entity_index_offset[(offsetof(s_map_variant_object_simulation_state, entity_index) == 0x48) ? 1 : -1];
typedef char s_map_variant_object_simulation_state_size[(sizeof(s_map_variant_object_simulation_state) == 0x58) ? 1 : -1];

struct s_map_variant_object_simulation_decode_state {
  unsigned short game_engine_flags;
  unsigned char placement_flags;
  unsigned char team_designator;
  unsigned char spare_clips;
  unsigned char spawn_rate;
  unsigned char teleporter_channel;
  char boundary_shape;
  float boundary[4];
  long quota_object_definition_index;
  unsigned char quota_placed_on_map;
  unsigned char quota_maximum_count;
  unsigned char quota_minimum_count;
  real_point3d position;
  real_vector3d forward;
  real_vector3d up;
  unsigned short flags;
  long pad48;
  long entity_index;
  long pad4c;
  long parent_entity_index;
};
typedef char s_map_variant_object_simulation_decode_state_position_offset[(offsetof(s_map_variant_object_simulation_decode_state, position) == 0x20) ? 1 : -1];
typedef char s_map_variant_object_simulation_decode_state_entity_index_offset[
    (offsetof(s_map_variant_object_simulation_decode_state, entity_index) == 0x4c) ? 1 : -1];
typedef char s_map_variant_object_simulation_decode_state_size[(sizeof(s_map_variant_object_simulation_decode_state) == 0x58) ? 1 : -1];

static __forceinline bool scenario_map_variant_object_has_multiplayer_properties(long object_index)
{
  return (unsigned short)object_get(object_index)->multiplayer_data_offset != 0xffff;
}





static __forceinline unsigned char scenario_map_variant_object_header_flags(long object_index)
{
  return datum_get<s_object_header>(tls_object_header_data(), object_index)->flags;
}

static __forceinline bool scenario_map_variant_collect_object_type(long object_type)
{
  return object_type == 6 || object_type == 10 || object_type == 1 || object_type == 2 || object_type == 3;
}

// the variant object of the map variant in the game options (the variant the game was started with)
static __forceinline const s_variant_object_datum *scenario_map_variant_game_options_variant_object_get(long variant_object_index)
{
  const s_variant_object_datum *variant_object = NULL;
  if (variant_object_index >= 0 && variant_object_index < 640)
    variant_object = &tls_game_globals()->options.map_variant.objects[variant_object_index];
  return variant_object;
}



// the object placement the properties are applied to

void map_variant_set_placement_properties(const s_variant_multiplayer_object_properties_definition *properties, s_scenario_object_placement *placement)
{
  placement->engine_flags = properties->game_engine_flags.get_unsafe();
  bool symmetric = properties->placement_flags.test(_variant_placement_symmetric_bit);
  if (symmetric && properties->placement_flags.test(_variant_placement_asymmetric_bit))
    placement->placement_mode = 0;
  else
    placement->placement_mode = !symmetric + 1;
  if (properties->placement_flags.test(_variant_placement_flag0_bit))
    placement->placement_flags |= 1;
  else
    placement->placement_flags &= ~1;
  if (properties->placement_flags.test(_variant_placement_flag1_bit))
    placement->placement_flags |= 2;
  else
    placement->placement_flags &= ~2;
  if (properties->team_designator != _variant_team_designator_neutral)
    placement->team = properties->team_designator;
  placement->spare_clips_copy = properties->spare_clips;
  placement->spare_clips = properties->spare_clips;
  placement->spawn_rate = properties->spawn_rate;
  placement->boundary_shape = properties->boundary_shape;
  placement->boundary_width = properties->boundary_width;
  placement->boundary_box_length = properties->boundary_box_length;
  placement->boundary_positive_height = properties->boundary_positive_height;
  placement->boundary_negative_height = properties->boundary_negative_height;
}

c_map_variant::c_map_variant()
{
  create_default(_map_id_none);
}

// 0x1ce960
void c_map_variant::create_default(e_map_id map_id)
{
  s_variant_object_datum object;
  s_variant_quota quota;
  memset(this, 0, sizeof(c_map_variant));
  this->map_id = map_id;
  this->m_built_in = true;
  for (long i = 0; i < 80; i++)
    m_simulation_indices[i] = -1;
  for (long i = 0; i < 640; i++)
    objects[i] = object;
  for (long i = 0; i < 256; i++)
    quotas[i] = quota;
  this->m_version = 12;
  for (long i = 0; i < 14; i++)
    object_type_start[i] = -1;
  this->m_number_of_variant_objects = 0;
  this->m_number_of_scenario_objects = 0;
  this->m_number_of_placeable_object_quotas = 0;
  this->m_bounds = k_null_rectangle3d_storage;
  this->m_spent_budget = 0.0f;
  this->m_game_engine_subtype = 10;
  this->m_maximum_budget = 0.0f;
  this->m_flag = 0;
  s_datum level;
  if (map_id != _map_id_none && levels_try_and_get_datum(g_level_globals.multiplayer_levels, map_id, (s_datum *)&level)) {
    saved_game_files_initialize_metadata(&m_metadata, -1, 10, level.display_name, level.description, 0, 0xe1f0);
    m_metadata.map_id = map_id;
  }
  m_original_map_signature_hash = -1;
}

// 0x1cecb8
extern "C" unsigned __int64 map_variant_compute_unique_id(const c_map_variant *input_variant)
{
  const c_map_variant *hash_source;
  unsigned __int64 computed_hash[4];
  __declspec(align(8)) unsigned char variant_storage[sizeof(c_map_variant)];
  if (input_variant->m_built_in) {
    new (variant_storage) c_map_variant();
    c_map_variant *variant_copy = (c_map_variant *)variant_storage;
    input_variant->save_to(variant_copy);
    hash_source = variant_copy;
    variant_copy->m_metadata.date = 0;
  } else {
    hash_source = input_variant;
  }
  security_calculate_hash((const char *)hash_source, sizeof(*hash_source), false, (s_network_http_request_hash *)computed_hash);
  return computed_hash[0];
}

// 0x1cf8f0
void c_map_variant::initialize_for_scenario()
{
  scenario *scenario = global_scenario;
  short scenario_object_count = 0;

  create_default((e_map_id)scenario->map_id);
  m_original_map_signature_hash = crc_checksum_buffer(-1, g_cache_file_globals.header.signature, sizeof(g_cache_file_globals.header.signature));
  m_number_of_variant_objects = 0;

  for (e_object_type object_type = _object_type_first; object_type < k_object_type_count; object_type = e_object_type(object_type + 1))
  {
    if (scenario_map_variant_collect_object_type(object_type))
    {
      object_type_definition *definition = object_type_definitions[object_type];
      if (definition->scenario_datums_offset != -1 && definition->scenario_palette_offset != -1)
      {
        long element_size;
        s_tag_block *datums = scenario_get_object_type_scenario_datums(scenario, object_type, &element_size);
        object_type_start[object_type] = scenario_object_count;
        scenario_object_count += datums->count;
        m_number_of_variant_objects += datums->count;
      }
    }
  }

  m_number_of_scenario_objects = m_number_of_variant_objects;
  m_number_of_placeable_object_quotas = 0;

  if (global_scenario)
  {
#define ADD_PALETTE_QUOTAS(palette) \
    for (long palette_index = 0; palette_index < global_scenario->palette.count; palette_index++) \
    { \
      struct scenario *palette_scenario = global_scenario; \
      scenario_object_palette_entry *entry = &((scenario_object_palette_entry *)palette_scenario->palette.address)[palette_index]; \
      s_variant_quota quota; \
      quota.object_definition_index = entry->object.index; \
      quota.maximum_allowed = (unsigned char)(entry->maximum_allowed ? entry->maximum_allowed : -1); \
      quota.price = entry->price; \
      quotas[m_number_of_placeable_object_quotas++] = quota; \
    }
    ADD_PALETTE_QUOTAS(sandbox_vehicles)
    ADD_PALETTE_QUOTAS(sandbox_weapons)
    ADD_PALETTE_QUOTAS(sandbox_equipment)
    ADD_PALETTE_QUOTAS(sandbox_scenery)
    ADD_PALETTE_QUOTAS(sandbox_machines)
    ADD_PALETTE_QUOTAS(sandbox_sound_scenery)
    ADD_PALETTE_QUOTAS(sandbox_crates)
#undef ADD_PALETTE_QUOTAS
  }

  m_maximum_budget = global_scenario->budget;
}


// @address 0x1cfe20 map_variant_find_quota_index
long map_variant_find_quota_index(c_map_variant *map_variant, long definition_index)
{
  long result = -1;
  long index = 0;
  short count = map_variant->m_number_of_placeable_object_quotas;
  if (count > 0) {
    s_variant_quota *quota = &map_variant->quotas[0];
    do {
      if (quota->object_definition_index == definition_index) {
        result = index;
        break;
      }
      index++;
      quota++;
    } while (index < count);
  }
  return result;
}

struct s_saved_game_item_metadata_packed;

// 0x1d08b0
// @address 0x1d08b0 map_variant_reset_transient_state
bool map_variant_reset_transient_state(c_map_variant *map_variant, c_map_variant *other)
{
  bool result = true;
  map_variant->m_flag = 0;
  for (long i = 0; i < 80; i++)
    map_variant->m_simulation_indices[i] = -1;
  for (long i = 0; i < 80; i++)
    other->m_simulation_indices[i] = -1;
  if (map_variant->m_number_of_variant_objects != 0) {
    for (long variant_object_index = 0; variant_object_index < map_variant->m_number_of_variant_objects; variant_object_index++) {
      s_variant_object_datum *object = &map_variant->objects[variant_object_index];
      s_variant_object_datum *other_object = &other->objects[variant_object_index];
      object->helper_object_index = 0;
      object->object_index = 0;
      object->reuse_timeout = 0;
      if (!object->flags.is_empty()) {
        if (!object->flags.test(_variant_object_placement_edited_bit) && variant_object_index < map_variant->m_number_of_scenario_objects) {
          memset(&object->properties, 0, sizeof(object->properties));
        } else {
          result = result && simulation_compare_quantized_positions(&object->position, &other_object->position, 16, &map_variant->m_bounds);
          result = result && c_bitstream::compare_axes(&object->forward, &object->up, &other_object->forward, &other_object->up);
          result = result &&
              c_bitstream::compare_quantized_reals(
                  object->properties.boundary_box_length, other_object->properties.boundary_box_length, 0.0f, 60.0f, 16, false, false);
          result = result &&
              c_bitstream::compare_quantized_reals(object->properties.boundary_negative_height,
                  other_object->properties.boundary_negative_height, 0.0f, 60.0f, 16, false, false);
          result = result &&
              c_bitstream::compare_quantized_reals(object->properties.boundary_positive_height,
                  other_object->properties.boundary_positive_height, 0.0f, 60.0f, 16, false, false);
          result = result &&
              c_bitstream::compare_quantized_reals(
                  object->properties.boundary_width, other_object->properties.boundary_width, 0.0f, 60.0f, 16, false, false);
        }
        memset(&object->position, 0, sizeof(object->position));
        memset(&other_object->position, 0, sizeof(other_object->position));
        memset(&object->forward, 0, sizeof(object->forward));
        memset(&other_object->forward, 0, sizeof(other_object->forward));
        memset(&object->up, 0, sizeof(object->up));
        memset(&other_object->up, 0, sizeof(other_object->up));
        memset(&object->properties.boundary_width, 0, 4 * sizeof(object->properties.boundary_width));
        memset(&other_object->properties.boundary_width, 0, 4 * sizeof(other_object->properties.boundary_width));
      } else {
        memset(object, 0, sizeof(*object));
        object->parent_object_identifier.clear();
        object->properties.initialize_to_default();
      }
    }
  }
  return result;
}

struct s_map_variant_validation;

// 0x1d0e00
// @address 0x1d0e00 map_variant_can_create_scenario_object
bool map_variant_can_create_scenario_object(c_map_variant *map_variant, e_object_type object_type, long scenario_index,
    s_variant_multiplayer_object_properties_definition *properties)
{
  bool result = true;
  if (map_variant_is_valid((const s_map_variant_validation *)map_variant)) {
    long object_index = map_variant->object_type_start[object_type];
    if (object_index != -1) {
      s_variant_object_datum *variant_object = &map_variant->objects[object_index + scenario_index];
      if (!variant_object->flags.is_empty() && !variant_object->flags.test(_variant_object_placement_registered_bit)) {
        result = !variant_object->flags.test(_variant_object_placement_scenario_object_removed_bit);
        if (properties != 0)
          *properties = variant_object->properties;
      }
    }
  }
  return result;
}

long c_map_variant::get_object_index(e_object_type object_type, long scenario_index) const
{
  long object_index = -1;
  bool valid_type = object_type == 6 || object_type == 10 || object_type == 1 || object_type == 2 || object_type == 3;
  if (valid_type) {
    if (scenario_index != -1)
      object_index = object_type_start[object_type] + scenario_index;
  }
  return object_index;
}

bool c_map_variant::get_object_scenario_properties(long object_index, e_object_type *object_type, long *scenario_index)
{
  bool result = false;
  bool active = objects[object_index].flags.test(_variant_object_placement_runtime_candidate_bit);
  *object_type = _object_type_none;
  *scenario_index = -1;
  if (active) {
    for (long type_index = 0; type_index < 14; type_index += 2) {
      if (object_type_start[type_index] != -1) {
        if (object_type_start[type_index] > object_index)
          break;
        *object_type = (e_object_type)type_index;
      }
      if (object_type_start[type_index + 1] != -1) {
        if (object_type_start[type_index + 1] > object_index)
          break;
        *object_type = (e_object_type)(type_index + 1);
      }
    }
    *scenario_index = object_index - object_type_start[*object_type];
    result = true;
  }
  return result;
}

const s_scenario_multiplayer_object_properties *map_variant_try_and_get_multiplayer_properties(c_map_variant *map_variant, long object_index)
{
  const s_scenario_multiplayer_object_properties *result = 0;
  struct {
    long index;
    e_object_type kind;
  } local;
  local.index = -1;
  if (map_variant->get_object_scenario_properties(object_index, &local.kind, &local.index)) {
    long element_size;
    s_tag_block *block = scenario_get_object_type_scenario_datums(global_scenario, _object_type_crate, &element_size);
    void *base = tag_block_get_range_with_size(block, local.index, 1, element_size);
    switch (local.kind) {
    case 1:
    case 2:
      result = (const s_scenario_multiplayer_object_properties *)&((const s_scenario_unit_placement *)base)->multiplayer;
      break;
    case 3:
      result = (const s_scenario_multiplayer_object_properties *)&((const s_scenario_equipment *)base)->multiplayer;
      break;
    case 6:
      result = (const s_scenario_multiplayer_object_properties *)&((const s_scenario_scenery_placement *)base)->multiplayer;
      break;
    case _object_type_crate:
      result = (const s_scenario_multiplayer_object_properties *)&((const s_scenario_crate_placement *)base)->multiplayer;
      break;
    }
  }
  return result;
}

// 0x1d4480
void c_map_variant::apply_multiplayer_properties(long variant_object_index, long object_index) const
{
  long target_object_index;
  s_variant_object_datum variant_object;
  target_object_index = object_index;
  this->get_variant_object_datum(variant_object_index, &variant_object);
  if (target_object_index == -1)
    target_object_index = variant_object.object_index;
  if (target_object_index != -1)
    this->apply_multiplayer_properties(&variant_object.properties, target_object_index);
}

// 0x1d44e0
void c_map_variant::apply_multiplayer_properties(struct s_variant_multiplayer_object_properties_definition const * properties, long object_index) const
{
  s_object_datum *object = object_get(object_index);
  if ((unsigned short)object->multiplayer_data_offset != 0xffff) {
    s_multiplayer_object_properties *data = (s_multiplayer_object_properties *)object_header_block_get(object, &object->multiplayer_properties);
    data->boundary_shape = (e_multiplayer_object_boundary_shape)properties->boundary_shape;
    data->spawn_time = properties->spawn_rate;
    data->box_length = properties->boundary_box_length;
    data->box_top = properties->boundary_negative_height;
    data->box_height = properties->boundary_positive_height;
    data->box_width = properties->boundary_width;
    data->flags.set(_multiplayer_object_spawn_at_start, properties->placement_flags.test(_variant_placement_flag0_bit));
    data->flags.set(_multiplayer_object_spawn_on_disturbance, properties->placement_flags.test(_variant_placement_flag1_bit));
    unsigned int symmetric = (properties->placement_flags.get_unsafe() >> 2) & 1;
    unsigned int asymmetric = (properties->placement_flags.get_unsafe() >> 3) & 1;
    if (symmetric != asymmetric) {
      if (symmetric)
        data->symmetry = _game_engine_symmetric_placement_symmetric;
      else
        data->symmetry = _game_engine_symmetric_placement_asymmetric;
    } else
      data->symmetry = _game_engine_symmetric_placement_default;
    data->spawn_sequence = properties->spare_clips;
    data->team_designator = (e_multiplayer_team_designator)(e_variant_team_designator)properties->team_designator;
    data->teleporter_channel = (e_teleporter_channel)properties->spare_clips;
    if (object_get_type(object_index) == 2) {
      weapon_definition *definition = TAG_GET('weap', weapon_definition, object->definition_index);
      if (definition->magazines.count() > 0) {
        s_weapon_magazine_definition *magazine = (s_weapon_magazine_definition *)definition->magazines.address;
        if (magazine) {
          short rounds[2];
          rounds[0] = (properties->spare_clips + 1) * magazine->rounds_loaded_maximum;
          rounds[1] = 0;
          weapon_set_total_rounds(object_index, rounds);
        }
      }
    }
    if (object->map_variant_index != -1)
      simulation_action_sandbox_group_update(object->map_variant_index, 4, 0);
  }
}

void c_map_variant::setup_object_properties(s_variant_object_datum *variant_object)
{
  if (scenario_map_variant_object_has_multiplayer_properties(variant_object->object_index)) {
    s_object_datum *object = object_get(variant_object->object_index);
    s_multiplayer_object_properties *data = (s_multiplayer_object_properties *)object_header_block_get(object, &object->multiplayer_properties);
    variant_object->properties.initialize_to_default();
    variant_object->properties.game_engine_flags.set_unsafe(data->game_engine_flags.get_unsafe());
    variant_object->properties.boundary_shape = (e_multiplayer_object_boundary_shape)data->boundary_shape;
    variant_object->properties.placement_flags.set(_variant_placement_flag0_bit, data->flags.test(_multiplayer_object_spawn_at_start));
    variant_object->properties.placement_flags.set(_variant_placement_flag1_bit, data->flags.test(_multiplayer_object_spawn_on_disturbance));
    variant_object->properties.placement_flags.set(_variant_placement_asymmetric_bit, true);
    variant_object->properties.placement_flags.set(_variant_placement_symmetric_bit, true);
    if (data->symmetry == _game_engine_symmetric_placement_symmetric)
      variant_object->properties.placement_flags.set(_variant_placement_asymmetric_bit, false);
    else if (data->symmetry == _game_engine_symmetric_placement_asymmetric)
      variant_object->properties.placement_flags.set(_variant_placement_symmetric_bit, false);
    variant_object->properties.team_designator = (e_variant_team_designator)(e_multiplayer_team_designator)data->team_designator;
    if (data->spawn_time >= 0 && data->spawn_time < 255)
      variant_object->properties.spawn_rate = (unsigned char)data->spawn_time;
    else
      variant_object->properties.spawn_rate = 0;
    if (variant_object->properties.boundary_shape != _multiplayer_object_boundary_shape_unused) {
      variant_object->properties.boundary_box_length = (float)fabs(data->box_length);
      variant_object->properties.boundary_negative_height = (float)fabs(data->box_top);
      variant_object->properties.boundary_positive_height = (float)fabs(data->box_height);
      variant_object->properties.boundary_width = (float)fabs(data->box_width);
    } else {
      memset(&variant_object->properties.boundary_width, 0, 4 * sizeof(variant_object->properties.boundary_width));
    }
    variant_object->properties.teleporter_channel = data->object_type;
    if (object_get_type(variant_object->object_index) == 2) {
      weapon_definition *definition = TAG_GET('weap', weapon_definition, object->definition_index);
      if (definition->magazines.count() > 0) {
        s_weapon_magazine_definition *magazine = (s_weapon_magazine_definition *)definition->magazines.address;
        if (magazine) {
          long rounds_maximum = magazine->rounds_loaded_maximum;
          if (rounds_maximum != 0)
            variant_object->properties.spare_clips = (unsigned char)(weapon_get_rounds_inventory(variant_object->object_index, 0, true) / rounds_maximum);
        }
      }
    } else {
      if (data->object_type == 9 || data->object_type == 10 || data->object_type == 11)
        variant_object->properties.spare_clips = data->teleporter_channel;
      else
        variant_object->properties.spare_clips = data->spawn_sequence;
    }
  }
}

const float k_scenario_map_variant_real_epsilon = 0.0001f;




// 0x1d4930
void map_variant_set_placement_marker_visibility(c_map_variant *map_variant, bool show_objects)
{
    long player_count = 0;
    c_player_in_game_iterator players;
    players.begin();
    if (players.next()) {
        do {
            if (player_is_on_local_machine(players.get_index()))
                player_count++;
        } while (players.next());
    }
    bool multiple_players = player_count > 1;
    if (multiple_players)
        show_objects = true;
    if (map_variant->m_flag != show_objects) {
        for (long i = 0; i < map_variant->m_number_of_variant_objects; i++) {
            long helper_object_index = map_variant->objects[i].helper_object_index;
            if (helper_object_index != -1)
                object_set_hidden(helper_object_index, !show_objects);
        }
        map_variant->m_flag = show_objects;
    }
}

// 0x1d4a28
// a simulation gamestate entity (the simulation entity of an object's gamestate index)
// the state of one variant object as the simulation sends it

static __forceinline void scenario_map_variant_object_get_entity_index(long object_index, long *entity_index)
{
  long gamestate_index = object_get(object_index)->gamestate_index;
  if (gamestate_index != -1) {
    long index = datum_get<s_simulation_gamestate_entity_datum>(tls_simulation_gamestate_entity_data(), gamestate_index)->entity_index;
    if (index != -1)
      *entity_index = index;
  }
}

// @address 0x1d4a28 map_variant_encode_simulation_chunk
bool map_variant_encode_simulation_chunk(c_map_variant *map_variant, long chunk_index, unsigned long *update_mask, long state_count,
    s_map_variant_object_simulation_state *states)
{
  unsigned long new_mask = 0;
  for (long i = 0; i < 8; i++) {
    long variant_object_index = chunk_index * 8 + i;
    if (variant_object_index < map_variant->m_number_of_variant_objects) {
      if (*update_mask & (1 << (i * 4 + 1))) {
        if (map_variant->objects[variant_object_index].object_index != -1)
          scenario_map_variant_object_get_entity_index(map_variant->objects[variant_object_index].object_index, &states[i].entity_index);
        else
          states[i].entity_index = -1;
        states[i].flags = map_variant->objects[variant_object_index].flags.get_unsafe();
        if ((unsigned char)map_variant->objects[variant_object_index].parent_object_identifier.m_type != 0xff) {
          long parent_object_index = map_variant->objects[variant_object_index].parent_object_identifier.find_object_index();
          if (parent_object_index != -1)
            scenario_map_variant_object_get_entity_index(parent_object_index, &states[i].parent_entity_index);
        } else {
          states[i].parent_entity_index = -1;
        }
        new_mask |= (1 << (i * 4 + 1));
      }
      if (*update_mask & (1 << (i * 4))) {
        if (fabs(states[i].position.x - map_variant->objects[variant_object_index].position.x) >= k_scenario_map_variant_real_epsilon ||
            fabs(states[i].position.y - map_variant->objects[variant_object_index].position.y) >= k_scenario_map_variant_real_epsilon ||
            fabs(states[i].position.z - map_variant->objects[variant_object_index].position.z) >= k_scenario_map_variant_real_epsilon ||
            fabs(states[i].forward.i - map_variant->objects[variant_object_index].forward.i) >= k_scenario_map_variant_real_epsilon ||
            fabs(states[i].forward.j - map_variant->objects[variant_object_index].forward.j) >= k_scenario_map_variant_real_epsilon ||
            fabs(states[i].forward.k - map_variant->objects[variant_object_index].forward.k) >= k_scenario_map_variant_real_epsilon ||
            fabs(states[i].up.i - map_variant->objects[variant_object_index].up.i) >= k_scenario_map_variant_real_epsilon ||
            fabs(states[i].up.j - map_variant->objects[variant_object_index].up.j) >= k_scenario_map_variant_real_epsilon ||
            fabs(states[i].up.k - map_variant->objects[variant_object_index].up.k) >= k_scenario_map_variant_real_epsilon) {
          new_mask |= (1 << (i * 4));
          states[i].position = map_variant->objects[variant_object_index].position;
          states[i].forward = map_variant->objects[variant_object_index].forward;
          states[i].up = map_variant->objects[variant_object_index].up;
        }
      }
      if (*update_mask & (1 << (i * 4 + 2))) {
        if (!map_variant->objects[variant_object_index].flags.is_empty() &&
            !map_variant->objects[variant_object_index].flags.test(_variant_object_placement_scenario_object_removed_bit) &&
            (map_variant->objects[variant_object_index].flags.test(_variant_object_placement_edited_bit) ||
                map_variant->objects[variant_object_index].flags.test(_variant_object_placement_runtime_created_bit)))
            {
          if (states[i].game_engine_flags != map_variant->objects[variant_object_index].properties.game_engine_flags.get_unsafe() ||
              states[i].placement_flags != map_variant->objects[variant_object_index].properties.placement_flags.get_unsafe() ||
              states[i].spawn_rate != map_variant->objects[variant_object_index].properties.spawn_rate ||
              states[i].spare_clips != map_variant->objects[variant_object_index].properties.spare_clips ||
              states[i].team_designator != map_variant->objects[variant_object_index].properties.team_designator ||
              memcmp(states[i].boundary, &map_variant->objects[variant_object_index].properties.boundary_width, sizeof(states[i].boundary)) != 0 ||
              states[i].boundary_shape != map_variant->objects[variant_object_index].properties.boundary_shape) {
            new_mask |= (1 << (i * 4 + 2));
            states[i].game_engine_flags = map_variant->objects[variant_object_index].properties.game_engine_flags.get_unsafe();
            states[i].placement_flags = map_variant->objects[variant_object_index].properties.placement_flags.get_unsafe();
            states[i].spawn_rate = map_variant->objects[variant_object_index].properties.spawn_rate;
            states[i].spare_clips = map_variant->objects[variant_object_index].properties.spare_clips;
            states[i].team_designator = map_variant->objects[variant_object_index].properties.team_designator;
            memcpy(states[i].boundary, &map_variant->objects[variant_object_index].properties.boundary_width, sizeof(states[i].boundary));
            states[i].boundary_shape = map_variant->objects[variant_object_index].properties.boundary_shape;
            states[i].flags = map_variant->objects[variant_object_index].flags.get_unsafe();
          }
        }
      }
      if (*update_mask & (1 << (i * 4 + 3))) {
        if (map_variant->objects[variant_object_index].quota_index != -1) {
          s_variant_quota *quota = &tls_game_engine_globals()->map_info.quotas[map_variant->objects[variant_object_index].quota_index];
          if (states[i].quota_maximum_count != quota->maximum_count || states[i].quota_minimum_count != quota->minimum_count ||
              states[i].quota_placed_on_map != quota->placed_on_map)
          {
            new_mask |= (1 << (i * 4 + 3));
            states[i].quota_object_definition_index = quota->object_definition_index;
            states[i].quota_placed_on_map = quota->placed_on_map;
            states[i].quota_maximum_count = quota->maximum_count;
            states[i].quota_minimum_count = quota->minimum_count;
          }
        }
      }
    }
  }
  *update_mask = new_mask;
  return true;
}

// 0x1d53b8
const s_scenario_multiplayer_object_properties *scenario_multiplayer_object_properties_try_and_get(e_object_type type, long index)
{
  const s_scenario_multiplayer_object_properties *result = NULL;
  if ((unsigned int)type <= _object_type_effect_scenery) {
    object_type_definition *definition = object_type_definitions[type];
    s_tag_block *block = (s_tag_block *)((char *)global_scenario + (short)definition->scenario_datums_offset);
    if (index >= 0 && index < block->count) {
      const void *placement = tag_block_get_element_with_size(block, index, definition->scenario_datum_size);
      if (placement) {
        switch (type) {
        case _object_type_vehicle:
        case _object_type_weapon:
          result = (const s_scenario_multiplayer_object_properties *)&((const s_scenario_unit_placement *)placement)->multiplayer;
          break;
        case _object_type_equipment:
          result = (const s_scenario_multiplayer_object_properties *)&((const s_scenario_equipment *)placement)->multiplayer;
          break;
        case _object_type_scenery:
          result = (const s_scenario_multiplayer_object_properties *)&((const s_scenario_scenery_placement *)placement)->multiplayer;
          break;
        case _object_type_crate:
          result = (const s_scenario_multiplayer_object_properties *)&((const s_scenario_crate_placement *)placement)->multiplayer;
          break;
        }
      }
    }
  }
  return result;
}

// the vector from a point to the origin
static __forceinline real_vector3d *scenario_map_variant_vector_to_origin3d(const real_point3d *point, real_vector3d *result)
{
  result->i = -point->x;
  result->j = -point->y;
  result->k = -point->z;
  return result;
}



static __forceinline float scenario_map_variant_distance_squared_from_origin3d(const real_point3d *point)
{
  real_vector3d vector;
  return magnitude_squared3d(scenario_map_variant_vector_to_origin3d(point, &vector));
}

// 0x1d0f88
void c_map_variant::adjust_active_object_count(long quota_index, long notify)
{
  if (game_simulation::game_is_predicted())
    return;

  s_variant_quota *quota = &quotas[quota_index];
  const s_object_definition *definition =
      TAG_GET('obje', const s_object_definition, quota->object_definition_index);
  e_object_type object_type = (e_object_type)definition->object_type;

  byte active_object_count = 0;
  long scenario_object_count = m_number_of_scenario_objects;
  long object_type_end = scenario_object_count;
  long object_type_start_index = object_type_start[object_type];
  e_object_type type = (e_object_type)(object_type + 1);
  while (type < k_object_type_count) {
    if (object_type_start[type] != -1) {
      object_type_end = object_type_start[type];
      break;
    }
    type = e_object_type(type + 1);
  }

  for (long index = object_type_start_index; index < object_type_end; ++index) {
    s_variant_object_datum *object = &objects[index];
    if (object->quota_index == quota_index && !object->flags.is_empty() &&
        !object->flags.test(_variant_object_placement_scenario_object_removed_bit) &&
        !object->flags.test(_variant_object_placement_runtime_created_bit))
      ++active_object_count;
  }

  for (long index = scenario_object_count; index < m_number_of_variant_objects; ++index) {
    s_variant_object_datum *object = &objects[index];
    if (object->quota_index == quota_index && !object->flags.is_empty())
      ++active_object_count;
  }

  if (quota->placed_on_map == active_object_count)
    return;

  if ((unsigned char)notify != 0) {
    game_engine_sandbox_adjust_quota_counts(quota_index, active_object_count - quota->placed_on_map);
    return;
  }

  quota->placed_on_map = active_object_count;
  quota->maximum_count = SCENARIO_MAP_VARIANT_QUOTA_MIN(SCENARIO_MAP_VARIANT_QUOTA_MAX(quota->maximum_count, active_object_count), quota->maximum_allowed);
  quota->minimum_count = SCENARIO_MAP_VARIANT_QUOTA_MIN(SCENARIO_MAP_VARIANT_QUOTA_MAX(quota->minimum_count, 0), active_object_count);
}

// 0x1d11f8
bool c_map_variant::register_scenario_object(long object_index, bool from_game_options)
{
  bool result = true;
  s_object_datum *object = object_get(object_index);
  long object_type = object->object_type;
  if (scenario_map_variant_collect_object_type(object_type)) {
    long variant_object_index = object_type_start[object_type] + object->scenario_datum_index;
    object->map_variant_index = (short)variant_object_index;
    s_variant_object_datum *variant_object = &objects[variant_object_index];
    variant_object->object_index = object_index;
    if (!variant_object->flags.test(_variant_object_placement_edited_bit)) {
      if (!variant_object->flags.test(_variant_object_placement_spawn_from_object_bit)) {
        ::object_get_orientation(object_index, &variant_object->forward, &variant_object->up);
        object_get_origin(object_index, &variant_object->position);
      }
      if (from_game_options) {
        const s_variant_object_datum *original = scenario_map_variant_game_options_variant_object_get(variant_object_index);
        if (original->flags.test(_variant_object_placement_spawn_from_object_bit)) {
          variant_object->parent_object_identifier = original->parent_object_identifier;
          variant_object->flags.set(_variant_object_placement_spawn_from_object_bit, true);
          variant_object->flags.set(_variant_object_placement_spawn_relative_bit, original->flags.test(_variant_object_placement_spawn_relative_bit));
          variant_object->position = original->position;
          variant_object->forward = original->forward;
          variant_object->up = original->up;
        }
      }
    }
    if (variant_object->flags.test(_variant_object_placement_spawn_from_object_bit)) {
      if (variant_object->flags.test(_variant_object_placement_spawn_relative_bit)) {
        long parent = object_index;
        long ultimate_parent = -1;
        while (parent != -1) {
          ultimate_parent = parent;
          parent = object_get(parent)->parent_object_index;
        }
        if (ultimate_parent != object_index)
          object_detach(object_index);
      }
      map_variant_update_parented_object_position(this, variant_object_index);
    }
    bool update_quota = false;
    variant_object->quota_index = map_variant_find_quota_index(this, object->definition_index);
    s_variant_quota quota;
    if (variant_object->quota_index == -1) {
      quota.object_definition_index = object->definition_index;
      quota.minimum_count = 0;
      quota.maximum_count = 0;
      variant_object->quota_index = m_number_of_placeable_object_quotas++;
      update_quota = true;
    } else {
      quota = quotas[variant_object->quota_index];
      if (quota.maximum_count == 0)
        update_quota = true;
    }
    if (update_quota) {
      const s_scenario_multiplayer_object_properties *properties = scenario_multiplayer_object_properties_try_and_get(
          (e_object_type)object_type, object->scenario_datum_index);
      if (properties) {
        if ((signed char)properties->minimum_count >= 0 && (signed char)properties->minimum_count < quota.minimum_count)
          quota.minimum_count = (signed char)properties->minimum_count;
        if (properties->maximum_count >= 0 && properties->maximum_count > quota.maximum_count)
          quota.maximum_count = properties->maximum_count;
      }
      if (quota.maximum_count == 0 && quota.maximum_allowed != 0)
        quota.maximum_count = quota.maximum_allowed;
      if (current_game_engine() && current_game_engine()->get_type() == 5) {
        if (quota.maximum_count > quota.maximum_allowed)
          quota.maximum_count = quota.maximum_allowed;
      }
      quotas[variant_object->quota_index] = quota;
    }
    if (!variant_object->flags.test(_variant_object_placement_edited_bit) || game_simulation::game_is_predicted())
      setup_object_properties(variant_object);
    (this)->apply_multiplayer_properties/*0x1d4480*/(variant_object_index, object_index);
    variant_object->flags.set(_variant_object_placement_runtime_created_bit, false);
    variant_object->flags.set(_variant_object_placement_scenario_object_removed_bit, false);
    variant_object->flags.set(_variant_object_placement_occupied_slot_bit, true);
    variant_object->flags.set(_variant_object_placement_runtime_candidate_bit, true);
    if (from_game_options)
      variant_object->flags.set(_variant_object_placement_edited_bit, true);
    ::map_variant_create_helper_object(variant_object_index);
  }
  return result;
}

// 0x1d1650
void c_map_variant::register_object(long object_index, long placement_index)
{
  s_object_datum *object = object_get(object_index);
  bool valid = true;
  if (object->parent_object_index != -1) {
    long parent = object_index;
    long ultimate_parent = -1;
    while (parent != -1) {
      ultimate_parent = parent;
      parent = object_get(parent)->parent_object_index;
    }
    if (!object_early_mover_is_owner(ultimate_parent, false))
      valid = false;
  }
  if (scenario_map_variant_collect_object_type(object->object_type) && valid) {
    if (placement_index == -1) {
      for (long index = m_number_of_scenario_objects; index < m_number_of_variant_objects; index++) {
        if (objects[index].flags.is_empty() && objects[index].reuse_timeout == 0) {
          placement_index = index;
          break;
        }
      }
      if (placement_index == -1) {
        if (m_number_of_variant_objects == 640)
          object->map_variant_index = -1;
        else
          placement_index = m_number_of_variant_objects++;
      }
    } else {
      if (m_number_of_variant_objects <= placement_index)
        m_number_of_variant_objects = (short)(placement_index + 1);
      if (placement_index < m_number_of_scenario_objects)
        placement_index = -1;
    }
  } else {
    placement_index = -1;
  }
  if (placement_index != -1) {
    s_variant_object_datum *slot = &objects[placement_index];
    slot->object_index = object_index;
    slot->flags.set(_variant_object_placement_occupied_slot_bit, true);
    slot->quota_index = map_variant_find_quota_index(this, object->definition_index);
    if (slot->flags.test(_variant_object_placement_runtime_created_bit))
      slot->flags.set(_variant_object_placement_runtime_created_bit, false);
    object->map_variant_index = (short)placement_index;
    if (!slot->flags.test(_variant_object_placement_edited_bit) || game_simulation::game_is_predicted())
      setup_object_properties(slot);
    ::object_get_orientation(object_index, &slot->forward, &slot->up);
    if (slot->flags.test(_variant_object_placement_spawn_from_object_bit)) {
      map_variant_update_parented_object_position(this, placement_index);
    } else {
      object_get_origin(object_index, &slot->position);
      ::object_get_orientation(object_index, &slot->forward, &slot->up);
    }
    slot->flags.set(_variant_object_placement_edited_bit, true);
    ::map_variant_create_helper_object(placement_index);
  } else {
    object->map_variant_index = -1;
  }
}

// 0x1d1a18
void map_variant_set_scenario_object_removed(c_map_variant *map_variant, e_object_type object_type, long scenario_index,
    const s_variant_multiplayer_object_properties_definition *properties)
{
  if (scenario_map_variant_collect_object_type(object_type)) {
    long variant_object_index = map_variant->object_type_start[object_type] + scenario_index;
    s_variant_object_datum *variant_object = &map_variant->objects[variant_object_index];
    variant_object->flags.set(_variant_object_placement_occupied_slot_bit, true);
    variant_object->flags.set(_variant_object_placement_runtime_candidate_bit, true);
    variant_object->flags.set(_variant_object_placement_scenario_object_removed_bit, true);
    if (properties) {
      map_variant->objects[variant_object_index].properties = *properties;
    }
  }
}





static __forceinline real_point3d *scenario_map_variant_point_from_vector3d(const real_point3d *point, const real_vector3d *vector, real_point3d *result)
{
  result->x = point->x + vector->i;
  result->y = point->y + vector->j;
  result->z = point->z + vector->k;
  return result;
}

struct s_location;


// @address 0x1cf728 map_variant_update_parented_object_position
void map_variant_update_parented_object_position(c_map_variant *map_variant, long placement_index)
{
  if (map_variant_is_valid((const s_map_variant_validation *)map_variant) && !game_simulation::game_is_predicted()) {
    s_variant_object_datum *placement = &map_variant->objects[placement_index];
    if (!placement->flags.is_empty()) {
      long object_index = placement->object_index;
      if (object_index != -1) {
        long parent_object_index = placement->parent_object_identifier.find_object_index();
        if (parent_object_index != -1) {
          s_object_datum *object = object_get(object_index);
          real_point3d parent_origin;
          object_get_origin(parent_object_index, &parent_origin);
          real_matrix4x3 matrix;
          memcpy(&matrix, object_get_node_matrix(parent_object_index, 0), sizeof(matrix));
          matrix.position = *global_origin3d;
          real_point3d position;
          real_vector3d forward;
          real_vector3d up;
          matrix4x3_transform_point(&matrix, &placement->position, &position);
          matrix4x3_transform_vector(&matrix, &placement->forward, &forward);
          matrix4x3_transform_vector(&matrix, &placement->up, &up);
          position.x = -position.x;
          position.y = -position.y;
          position.z = -position.z;
          real_point3d *parent_origin_point = &parent_origin;
          position.x = parent_origin_point->x + position.x;
          position.y = parent_origin_point->y + position.y;
          position.z = parent_origin_point->z + position.z;
          object_set_position_direct(object_index, &position, &forward, &up, 0, false);
          ::simulation_action_object_update(placement->object_index, 6);
          if (placement->flags.test(_variant_object_placement_spawn_relative_bit)) {
            object_attach_to_node_with_type(parent_object_index, placement->object_index, 0);
            object->flags |= 0x4000000;
            ::simulation_action_object_update(placement->object_index, 0x800);
          }
        }
      }
    }
  }
}





// @address 0x1d0c48 map_variant_test_equal
extern "C" unsigned char map_variant_test_equal(const c_map_variant *map_variant)
{
  unsigned char first_buffer[sizeof(c_map_variant)];
  unsigned char second_buffer[sizeof(c_map_variant)];
  c_bitstream first_packet;
  c_bitstream second_packet;
  first_packet.m_data_size_alignment = 1;
  first_packet.m_data = first_buffer;
  first_packet.m_data_max = first_buffer + sizeof(first_buffer);
  first_packet.m_data_size_bytes = sizeof(first_buffer);
  first_packet.reset(0);
  second_packet.m_data_size_alignment = 1;
  second_packet.m_data = second_buffer;
  second_packet.m_data_max = second_buffer + sizeof(second_buffer);
  second_packet.m_data_size_bytes = sizeof(second_buffer);
  second_packet.reset(0);
  first_packet.begin_writing();
  map_variant->encode(&first_packet);
  long first_size = (first_packet.m_current.current_stream_bit_position + first_packet.m_current.window_bits_used + 7) / 8;
  first_packet.finish_writing(0);
  second_packet.begin_writing();
  map_variant->encode(&second_packet);
  long second_size = (second_packet.m_current.current_stream_bit_position + second_packet.m_current.window_bits_used + 7) / 8;
  second_packet.finish_writing(0);
  long result;
  if (first_size == second_size && memcmp(first_buffer, second_buffer, first_size) == 0)
    result = 1;
  else
    result = 0;
  return result;
}

// 0x1d1f20
// @address 0x1d1f20 map_variant_get_object_transform
void map_variant_get_object_transform(c_map_variant *map_variant, long variant_object_index, real_point3d *position, real_vector3d *forward, real_vector3d *up)
{
  if (variant_object_index >= 0 && variant_object_index < map_variant->m_number_of_variant_objects) {
    s_variant_object_datum *variant_object = &map_variant->objects[variant_object_index];
    real_point3d object_position = variant_object->position;
    real_vector3d object_forward = variant_object->forward;
    real_vector3d object_up = variant_object->up;
    if (variant_object->flags.test(_variant_object_placement_spawn_from_object_bit) && (unsigned char)variant_object->parent_object_identifier.m_type != 0xff) {
      long parent_object_index = variant_object->parent_object_identifier.find_object_index();
      if (parent_object_index != -1) {
        real_point3d parent_origin;
        real_matrix4x3 parent_matrix;
        real_point3d point;
        real_vector3d offset;
        object_get_origin(parent_object_index, &parent_origin);
        parent_matrix = *object_get_node_matrix(parent_object_index, 0);
        parent_matrix.position = *global_origin3d;
        matrix4x3_transform_point(&parent_matrix, &variant_object->position, &point);
        scenario_map_variant_vector_to_origin3d(&point, &offset);
        scenario_map_variant_point_from_vector3d(&parent_origin, &offset, &object_position);
      }
    }
    if (position)
      *position = object_position;
    if (forward)
      *forward = object_forward;
    if (up)
      *up = object_up;
  }
}

// 0x1d20b8
bool c_map_variant::get_variant_object_datum(long object_index, s_variant_object_datum *variant_object) const
{
  bool result = false;
  if (!game_simulation::game_is_predicted()) {
    if (m_number_of_variant_objects != 0) {
      if (object_index >= 0 && object_index < m_number_of_variant_objects) {
        *variant_object = objects[object_index];
        result = true;
      }
    } else {
      memset(variant_object, 0, sizeof(*variant_object));
    }
  } else if (object_index >= 0 && object_index < 640) {
    *variant_object = objects[object_index];
    result = true;
  }
  return result;
}





// @address 0x1d2180 map_variant_set_object_parent
extern "C" void map_variant_set_object_parent(long object_index, long parent_object_index)
{
  scenario *scenario_data = global_scenario;
  s_object_datum *object = object_get(object_index);
  if ((unsigned short)object->map_variant_index != 0xffff && (unsigned short)object->scenario_datum_index != 0xffff && scenario_data) {
    s_object_datum *parent = object_get(parent_object_index);
    if (parent_object_index != object_index && (unsigned short)parent->scenario_datum_index != 0xffff) {
      s_variant_object_datum placement;
      const s_scenario_object *object_placement = scenario_object_try_and_get(scenario_data, (e_object_type)object->object_type, object->scenario_datum_index);
      const s_scenario_object *parent_placement = scenario_object_try_and_get(scenario_data, (e_object_type)parent->object_type, parent->scenario_datum_index);
      real_point3d object_position = object_placement->position;
      real_point3d parent_position = parent_placement->position;
      real_vector3d object_forward, object_up;
      real_vector3d parent_forward, parent_up;
      vectors3d_from_euler_angles3d(&object_forward, &object_up, &object_placement->rotation);
      vectors3d_from_euler_angles3d(&parent_forward, &parent_up, &parent_placement->rotation);
      tls_game_engine_globals()->map_info.get_variant_object_datum(object->map_variant_index, &placement);
      real_vector3d offset;
      vector_from_points3d(&object_position, &parent_position, &offset);
      real_matrix4x3 parent_matrix;
      matrix4x3_rotation_from_vectors(&parent_matrix, &parent_forward, &parent_up);
      parent_matrix.position = *global_origin3d;
      matrix4x3_inverse_transform_vector(&parent_matrix, &offset, &offset);
      matrix4x3_inverse_transform_vector(&parent_matrix, &object_forward, &placement.forward);
      matrix4x3_inverse_transform_vector(&parent_matrix, &object_up, &placement.up);
      c_flags<e_variant_object_placement_flags, unsigned short, k_variant_object_placement_flags_count> placement_flags = placement.flags;
      placement_flags.set(_variant_object_placement_spawn_from_object_bit, true);
      placement.flags = placement_flags;
      placement.position.x = offset.i;
      placement.position.y = offset.j;
      placement.position.z = offset.k;
      placement.parent_object_identifier = parent->parent_object_identifier;
      if (game_engine_sandbox_is_object_fixed(object_index) && !((bool)((object->physics_flags >> 4) & 1))) {
        if ((unsigned char)object->parent_node_index != 0xff)
          object_detach(object_index);
        placement_flags.set(_variant_object_placement_spawn_relative_bit, true);
        placement.flags = placement_flags;
      }
      tls_game_engine_globals()->map_info.set_variant_object_datum(object->map_variant_index, &placement, false);
      map_variant_update_parented_object_position(&tls_game_engine_globals()->map_info, object->map_variant_index);
      simulation_action_sandbox_group_update(object->map_variant_index, 3, false);
    }
  }
}


static __forceinline long scenario_map_variant_object_index_from_name(short name_index)
{
  long result = -1;
  if ((unsigned short)name_index != 0xffff && name_index >= 0 && name_index < 2048)
    result = g_object_name_list[name_index];
  return result;
}

// @address 0x1d23f0 map_variant_set_object_parent_from_name
extern "C" void map_variant_set_object_parent_from_name(c_map_variant *map_variant, long placement_index)
{
  s_variant_object_datum *placement = &map_variant->objects[placement_index];
  if (placement->flags.test(_variant_object_placement_runtime_candidate_bit) && !placement->flags.test(_variant_object_placement_spawn_from_object_bit)) {
    long object_index = placement->object_index;
    if (object_index != -1) {
      e_object_type object_type;
      long scenario_index;
      if (tls_game_engine_globals()->map_info.get_object_scenario_properties(placement_index, &object_type, &scenario_index)) {
        const s_scenario_multiplayer_object_properties *properties = scenario_multiplayer_object_properties_try_and_get(object_type, scenario_index);
        if (properties) {
          long parent_object_index = scenario_map_variant_object_index_from_name(properties->parent_name_index);
          if (parent_object_index != -1)
            map_variant_set_object_parent(object_index, parent_object_index);
        }
      }
    }
  }
}

static __forceinline const s_variant_object_datum *scenario_map_variant_try_get_object(const game_globals_storage *globals, long index)
{
  const s_variant_object_datum *result = 0;
  if ((unsigned long)index <= 639)
    result = &globals->options.map_variant.objects[index];
  return result;
}

// @address 0x1d24c8 map_variant_update_object_parents
extern "C" void map_variant_update_object_parents(c_map_variant *map_variant)
{
  for (long index = 0; index < map_variant->m_number_of_scenario_objects; index++) {
    long scenario_index = -1;
    e_object_type object_type;
    bool update_attachment = true;
    if (map_variant->get_object_scenario_properties(index, &object_type, &scenario_index)) {
      game_globals_storage *globals = tls_game_globals();
      if (!globals->options.map_variant.m_built_in) {
        c_map_variant *original = &globals->options.map_variant;
        if (map_variant_is_valid((const s_map_variant_validation *)original)) {
          long original_index = original->get_object_index(object_type, scenario_index);
          if (original_index != -1 && scenario_map_variant_try_get_object(globals, original_index)->flags.test(_variant_object_placement_edited_bit))
            update_attachment = false;
        }
      }
    }
    if (update_attachment && !map_variant->objects[index].flags.test(_variant_object_placement_edited_bit))
      map_variant_set_object_parent_from_name(map_variant, index);
  }
}

// 0x1d2758
void c_map_variant::set_variant_object_datum(long variant_object_index, const s_variant_object_datum *variant_object, bool edited)
{
  if (variant_object_index >= 0 && variant_object_index < 640) {
    s_variant_object_datum *object = &objects[variant_object_index];
    *object = *variant_object;
    if (!object->flags.is_empty() && edited) {
      object->flags.set(_variant_object_placement_edited_bit, true);
      m_built_in = false;
    }
    if (object->object_index != -1) {
      if (scenario_map_variant_object_header_flags(object->object_index) & 0x10)
        object->object_index = -1;
    }
    compute_total_damage(variant_object_index);
  }
}

// 0x1d4038
void c_map_variant::save_to(c_map_variant *destination) const
{
  *destination = *this;
  for (long variant_object_index = 0; variant_object_index < destination->m_number_of_variant_objects; variant_object_index++) {
    s_variant_object_datum *object = &destination->objects[variant_object_index];
    if (object->flags.test(_variant_object_placement_runtime_created_bit)) {
      object->flags.set(_variant_object_placement_runtime_created_bit, false);
      object->flags.set(_variant_object_placement_occupied_slot_bit, true);
    }
    object->helper_object_index = -1;
    if (object->flags.test(_variant_object_placement_edited_bit)) {
      bool invalid = false;
      if (scenario_map_variant_distance_squared_from_origin3d(&object->position) < k_scenario_map_variant_real_epsilon) {
        if (object->object_index != -1)
          object_get_origin(object->object_index, &object->position);
        else
          invalid = true;
      }
      if (magnitude_squared3d(&object->forward) < k_scenario_map_variant_real_epsilon ||
          magnitude_squared3d(&object->up) < k_scenario_map_variant_real_epsilon)
      {
        if (object->object_index != -1)
          ::object_get_orientation(object->object_index, &object->forward, &object->up);
        else
          invalid = true;
      }
      if (object->quota_index == -1)
        invalid = true;
      if (invalid) {
        if (object->flags.test(_variant_object_placement_runtime_candidate_bit))
          object->flags.set(_variant_object_placement_scenario_object_removed_bit, true);
        else
          object->flags.clear();
      }
    }
    object->object_index = -1;
  }
}


#if !defined(_XBOX) || defined(__clang__)
// The raw-data fields of a packed map variant (the world bounds, the two budgets, a variant object's parent identifier,
// each placeable object quota) go between memory and the stream byte for byte, so their wire form is the Xbox's memory
// image: big-endian fields. The title servers publish the matchmaking map variants in that form (the 'mvar' chunk of a
// hopper's map variant file) and peers exchange it the same way (the map variant session parameter, game options), so
// a little-endian host converts each such block to host order after reading it and writes a converted copy. Each field
// is swapped as an integer (a swapped float can be a NaN pattern a float register would not keep); the swap is its own
// inverse. Without it a served quota's object definition 0xed680bf2 reads as 0xf20b68ed and
// game_engine_spawn_variant_objects faults on tag_get's result when a matchmaking game starts.
static __forceinline void scenario_map_variant_swap_long(void *field)
{
  unsigned long value;
  memcpy(&value, field, sizeof(value));
  value = byte_swap_long(value);
  memcpy(field, &value, sizeof(value));
}

static __forceinline void scenario_map_variant_swap_bounds(real_rectangle3d *bounds)
{
  for (long index = 0; index < 3; index++) {
    scenario_map_variant_swap_long(&bounds->m[index][0]);
    scenario_map_variant_swap_long(&bounds->m[index][1]);
  }
}

static __forceinline void scenario_map_variant_swap_object_identifier(c_object_identifier *identifier)
{
  scenario_map_variant_swap_long(&identifier->m_unique_id);
  unsigned short origin_bsp_index = (unsigned short)identifier->m_origin_bsp_index;
  identifier->m_origin_bsp_index = (short)((origin_bsp_index >> 8) | (origin_bsp_index << 8));
}

static __forceinline void scenario_map_variant_swap_quota(s_variant_quota *quota)
{
  scenario_map_variant_swap_long(&quota->object_definition_index);
  scenario_map_variant_swap_long(&quota->price);
}
#endif

// 0x1cff58
void c_map_variant::encode(c_bitstream *packet) const
{
  saved_game_files_encode_metadata((const s_saved_game_item_metadata_packed *)&m_metadata, packet);
  packet->write_integer("map-variant-version", m_version, 8);
  packet->write_integer("map-variant-checksum", m_original_map_signature_hash, 32);
  packet->write_integer("number-of-scenario-objects", m_number_of_scenario_objects, 10);
  packet->write_integer("number-of-variant-objects", m_number_of_variant_objects, 10);
  packet->write_integer("number-of-placeable-object-quotas", m_number_of_placeable_object_quotas, 9);
  packet->write_integer("map-id", map_id, 32);
  packet->write_bool("built-in", m_built_in);
#if defined(_XBOX) && !defined(__clang__)
  packet->write_raw_data("world-bounds", &m_bounds, 192);
  packet->write_integer("game-engine-subtype", m_game_engine_subtype, 4);
  packet->write_raw_data("maximum-budget", &m_maximum_budget, 32);
  packet->write_raw_data("spent-budget", &m_spent_budget, 32);
#else
  real_rectangle3d wire_bounds = m_bounds;
  scenario_map_variant_swap_bounds(&wire_bounds);
  packet->write_raw_data("world-bounds", &wire_bounds, 192);
  packet->write_integer("game-engine-subtype", m_game_engine_subtype, 4);
  float wire_budgets[2] = { m_maximum_budget, m_spent_budget };
  scenario_map_variant_swap_long(&wire_budgets[0]);
  scenario_map_variant_swap_long(&wire_budgets[1]);
  packet->write_raw_data("maximum-budget", &wire_budgets[0], 32);
  packet->write_raw_data("spent-budget", &wire_budgets[1], 32);
#endif
  long object_index = 0;
  if (object_index < m_number_of_variant_objects) {
    for (; object_index < m_number_of_variant_objects; object_index++) {
    const s_variant_object_datum *object = &objects[object_index];
    if (object->flags.is_empty()) {
      packet->write_bool("variant-object-exists", false);
    } else {
      packet->write_bool("variant-object-exists", true);
      packet->write_integer("variant-object-flags", object->flags.get_unsafe(), 16);
      packet->write_integer("variant-object-quota-index", object->quota_index, 32);
      if (object->flags.test(_variant_object_placement_spawn_from_object_bit)) {
        packet->write_bool("variant-object-parent-exists", true);
#if defined(_XBOX) && !defined(__clang__)
        packet->write_raw_data("variant-object-parent", &object->parent_object_identifier, 64);
#else
        c_object_identifier wire_parent = object->parent_object_identifier;
        scenario_map_variant_swap_object_identifier(&wire_parent);
        packet->write_raw_data("variant-object-parent", &wire_parent, 64);
#endif
      } else {
        packet->write_bool("variant-object-parent-exists", false);
      }
      if ((object->flags.test(_variant_object_placement_edited_bit) || (object_index >= m_number_of_scenario_objects))) {
        packet->write_bool("variant-object-position-exists", true);
        simulation_write_quantized_position(packet, &object->position, 16, false, &m_bounds);
        packet->write_axes("variant-object-axes", &object->forward, &object->up);
        packet->write_integer("variant-object-teleporter-channel", (signed char)object->properties.teleporter_channel, 8);
        packet->write_integer("variant-object-placement-flags", object->properties.placement_flags.get_unsafe(), 8);
        packet->write_integer("variant-object-engine-flags", object->properties.game_engine_flags.get_unsafe(), 16);
        packet->write_integer("variant-object-spare-clips", object->properties.spare_clips, 8);
        packet->write_integer("variant-object-spawn-rate", object->properties.spawn_rate, 8);
        packet->write_integer("variant-object-team", object->properties.team_designator, 8);
        packet->write_integer("variant-object-boundary-shape", object->properties.boundary_shape, 8);
        switch (object->properties.boundary_shape) {
        case 1:
          packet->write_quantized_real("variant-object-boundary", object->properties.boundary_width, 0.0f, 60.0f, 16, false, false);
          break;
        case 2:
          packet->write_quantized_real("variant-object-boundary", object->properties.boundary_width, 0.0f, 60.0f, 16, false, false);
          packet->write_quantized_real("variant-object-boundary", object->properties.boundary_positive_height, 0.0f, 60.0f, 16, false, false);
          packet->write_quantized_real("variant-object-boundary", object->properties.boundary_negative_height, 0.0f, 60.0f, 16, false, false);
          break;
        case 3:
          packet->write_quantized_real("variant-object-boundary", object->properties.boundary_width, 0.0f, 60.0f, 16, false, false);
          packet->write_quantized_real("variant-object-boundary", object->properties.boundary_box_length, 0.0f, 60.0f, 16, false, false);
          packet->write_quantized_real("variant-object-boundary", object->properties.boundary_positive_height, 0.0f, 60.0f, 16, false, false);
          packet->write_quantized_real("variant-object-boundary", object->properties.boundary_negative_height, 0.0f, 60.0f, 16, false, false);
          break;
        }
      } else {
        packet->write_bool("variant-object-position-exists", false);
      }
    }
    }
  }
  const short *type_start = object_type_start;
  unsigned long remaining_types = 14;
  do {
    packet->write_integer("map-variant-object-type-map", *type_start + 1, 9);
    type_start++;
    remaining_types--;
  } while (remaining_types != 0);
  long quota_index = 0;
  if (quota_index < m_number_of_placeable_object_quotas) {
    for (; quota_index < m_number_of_placeable_object_quotas; quota_index++) {
#if defined(_XBOX) && !defined(__clang__)
      packet->write_raw_data("variant-object-quota", &quotas[quota_index], 96);
#else
      s_variant_quota wire_quota = quotas[quota_index];
      scenario_map_variant_swap_quota(&wire_quota);
      packet->write_raw_data("variant-object-quota", &wire_quota, 96);
#endif
    }
  }
}

// @address 0x1ce478 ?find_definition_index@c_map_variant@@SAXJJJ@Z

void c_map_variant::find_definition_index(long object_index, long placement_index, long notify)
{
  game_globals_storage *game = tls_game_globals();
  if (game->options.game_mode == 2) {
    s_object_datum *object = datum_get<s_object_header>(tls_object_header_data(), object_index)->datum;
    // match: 0x1ce478: a zero-code web for the widened offset orders the TLS base before the variant pointer (r29, r28)
    const unsigned long &multiplayer_data_offset = (unsigned short)object->multiplayer_data_offset;
    if (multiplayer_data_offset != 0xffff) {
      if (object->scenario_datum_index != -1) {
        c_map_variant *variant = &game->options.map_variant;
        bool from_game_options = false;
        if (map_variant_is_valid((const s_map_variant_validation *)variant)) {
          if (variant->m_map_id != -1) {
            from_game_options = game_engine_sandbox_update_variant_object_properties(variant, object_index);
          }
        }
        game_engine_globals->map_info.register_scenario_object(object_index, from_game_options);
      } else {
        long index = placement_index;
        if (placement_index == -1) {
          index = object->map_variant_index;
        }
        game_engine_globals->map_info.register_object(object_index, index);
      }
      if (game_engine_sandbox_is_active()) {
        short index = object->map_variant_index;
        if (index != -1) {
          if (!game_simulation::game_is_predicted()) {
            simulation_action_sandbox_group_update(index, 2, false);
          }
        }
      }
      if (object->map_variant_index != -1) {
        c_map_variant *variant = &game_engine_globals->map_info;
        long quota_index = map_variant_find_quota_index(variant, object->definition_index);
        if (quota_index != -1) {
          variant->adjust_active_object_count(quota_index, notify == 0);
          if (!game_simulation::game_is_predicted()) {
            simulation_action_sandbox_group_update(object->map_variant_index, 8, false);
          }
        }
      }
    }
  }
}

// 0x1cfe70
// @address 0x1cfe70 map_variant_detach_object
void map_variant_detach_object(c_map_variant *map_variant, long object_index)
{
  s_object_datum *object = object_get(object_index);
  short placement_index = object->map_variant_index;
  if (placement_index != -1) {
    if (map_variant->objects[placement_index].helper_object_index == object_index) {
      map_variant->objects[placement_index].helper_object_index = -1;
      object->map_variant_index = -1;
    } else {
      if (!game_simulation::game_is_predicted()) {
        map_variant->remove_object(placement_index, _object_removal_origin_0);
      } else {
        game_engine_sandbox_stop_editing_object(placement_index, k_real_zero_storage, false);
        placement_index = object->map_variant_index;
        map_variant->objects[placement_index].object_index = -1;
      }
    }
  }
}

// 0x1d1ab0
// @address 0x1d1ab0 ?remove_object@c_map_variant@@QAAXJW4e_object_removal_origin@@@Z
void c_map_variant::remove_object(long variant_object_index, e_object_removal_origin origin)
{
  if (variant_object_index != -1 && variant_object_index >= 0 && variant_object_index < m_number_of_variant_objects) {
    s_variant_object_datum *entry = &objects[variant_object_index];
    long quota_index = entry->quota_index;
    bool remove_permanently = origin == 2;
    game_engine_sandbox_stop_editing_object(variant_object_index, 0.0f, 0);
    long object_index = entry->object_index;
    if ((unsigned int)object_index != 0xffffffff) {
      if (remove_permanently)
        game_engine_candy_monitor_delete_entry(object_get(object_index)->map_variant_index);
      if (object_index != -1) {
        s_object_datum *object = object_get(object_index);
        object->map_variant_index = -1;
        if (!game_simulation::game_is_predicted())
          ::simulation_action_object_update((unsigned int)object_index, 8192);
      }
    } else if (remove_permanently) {
      game_engine_candy_monitor_delete_entry(variant_object_index);
    }
    long child_index = entry->helper_object_index;
    bool delete_child = false;
    if (!entry->flags.test(_variant_object_placement_preserve_helper_bit) || remove_permanently)
      delete_child = true;
    if (entry->flags.test(_variant_object_placement_runtime_candidate_bit)) {
      if (!entry->flags.test(_variant_object_placement_runtime_created_bit) && remove_permanently)
        entry->flags.set(_variant_object_placement_scenario_object_removed_bit, true);
    } else if (!entry->flags.test(_variant_object_placement_runtime_created_bit) && remove_permanently) {
      long saved_quota_index = entry->quota_index;
      memset(entry, 0, sizeof(*entry));
      entry->parent_object_identifier.m_type = -1;
      entry->parent_object_identifier.m_source = -1;
      entry->parent_object_identifier.m_origin_bsp_index = -1;
      entry->parent_object_identifier.m_unique_id = -1;
      entry->helper_object_index = -1;
      entry->reuse_timeout = 5;
      entry->quota_index = saved_quota_index;
    }
    entry->object_index = -1;
    if (game_engine_sandbox_is_active() && !game_simulation::game_is_predicted())
      simulation_action_sandbox_group_update(variant_object_index, 2, false);
    if (delete_child && child_index != -1) {
      object_get(child_index)->map_variant_index = -1;
      object_delete(child_index);
      entry->helper_object_index = -1;
    }
    if (remove_permanently && quota_index != -1) {
      adjust_active_object_count(quota_index, 1);
      simulation_action_sandbox_group_update(variant_object_index, 8, false);
    }
  }
}

// @address 0x1d3140 ?map_variant_update_flag_bomb_placements@@YAXPAVc_map_variant@@J@Z
void map_variant_update_flag_bomb_placements(c_map_variant *map_variant, long game_engine)
{
  long teams = 0;
  long flag_asymmetric = 0;
  long flag_symmetric = 0;
  long bomb_asymmetric = 0;
  long bomb_symmetric = 0;
  long scenario_count = map_variant->m_number_of_scenario_objects;
  long first = map_variant->object_type_start[10];
  for (long index = first; index < scenario_count; index++) {
    const s_scenario_multiplayer_object_properties *properties = map_variant_try_and_get_multiplayer_properties(map_variant, index);
    s_variant_object_datum *object = &map_variant->objects[index];
    signed char type = (signed char)object->properties.teleporter_channel;
    if (type == 15 && object->properties.game_engine_flags.test((e_scenario_game_engine)game_engine)) {
      if (properties)
        teams |= 1 << properties->team_designator;
      if (!object->flags.test(_variant_object_placement_scenario_object_removed_bit)) {
        if (object->properties.placement_flags.test(_variant_placement_symmetric_bit))
          flag_symmetric |= 1 << (e_variant_team_designator)object->properties.team_designator;
        if (object->properties.placement_flags.test(_variant_placement_asymmetric_bit))
          flag_asymmetric |= 1 << (e_variant_team_designator)object->properties.team_designator;
      }
    }
    if (type == 17 && object->properties.game_engine_flags.test((e_scenario_game_engine)game_engine)) {
      if (properties)
        teams |= 1 << properties->team_designator;
      if (!object->flags.test(_variant_object_placement_scenario_object_removed_bit)) {
        if (object->properties.placement_flags.test(_variant_placement_symmetric_bit))
          bomb_symmetric |= 1 << (e_variant_team_designator)object->properties.team_designator;
        if (object->properties.placement_flags.test(_variant_placement_asymmetric_bit))
          bomb_asymmetric |= 1 << (e_variant_team_designator)object->properties.team_designator;
      }
    }
  }
  for (long index = map_variant->m_number_of_scenario_objects; index < map_variant->m_number_of_variant_objects; index++) {
    s_variant_object_datum *object = &map_variant->objects[index];
    if (!object->flags.is_empty()) {
      signed char type = (signed char)object->properties.teleporter_channel;
      if (type == 15 && object->properties.game_engine_flags.test((e_scenario_game_engine)game_engine)) {
        teams |= 1 << (e_variant_team_designator)object->properties.team_designator;
        if (object->properties.placement_flags.test(_variant_placement_symmetric_bit))
          flag_symmetric |= 1 << (e_variant_team_designator)object->properties.team_designator;
        if (object->properties.placement_flags.test(_variant_placement_asymmetric_bit))
          flag_asymmetric |= 1 << (e_variant_team_designator)object->properties.team_designator;
      }
      if (type == 17 && object->properties.game_engine_flags.test((e_scenario_game_engine)game_engine)) {
        teams |= 1 << (e_variant_team_designator)object->properties.team_designator;
        if (object->properties.placement_flags.test(_variant_placement_symmetric_bit))
          bomb_symmetric |= 1 << (e_variant_team_designator)object->properties.team_designator;
        if (object->properties.placement_flags.test(_variant_placement_asymmetric_bit))
          bomb_asymmetric |= 1 << (e_variant_team_designator)object->properties.team_designator;
      }
    }
  }
  if ((flag_symmetric & flag_asymmetric & teams) != teams) {
    for (long team = 0; team < 10; team++) {
      long team_mask = 1 << team;
      if (!(teams & team_mask) || (flag_asymmetric & team_mask))
        continue;
      if (game_engine == 0) {
        if (team != 0)
          continue;
      } else if (game_engine == 6 && team == 0) {
        continue;
      }
      for (long index = first; index < scenario_count; index++) {
        s_variant_object_datum *object = &map_variant->objects[index];
        if (object->properties.teleporter_channel == 15 && object->properties.game_engine_flags.test((e_scenario_game_engine)game_engine)) {
          const s_scenario_multiplayer_object_properties *properties = map_variant_try_and_get_multiplayer_properties(map_variant, index);
          if (properties && team == properties->team_designator && (properties->symmetry == 0 || properties->symmetry == 2)) {
            object->flags.set_unsafe((object->flags.get_unsafe() & 0xffd6) | 9);
            object->properties.team_designator = (e_variant_team_designator)team;
            object->properties.placement_flags.set(_variant_placement_symmetric_bit, true);
            object->properties.placement_flags.set(_variant_placement_asymmetric_bit, true);
            if (properties->symmetry == 0) {
              flag_symmetric |= team_mask;
            } else if (properties->symmetry == 2) {
              object->properties.placement_flags.set(_variant_placement_symmetric_bit, false);
            } else {
              object->properties.placement_flags.set(_variant_placement_asymmetric_bit, false);
            }
            break;
          }
        }
      }
    }
    for (long team = 0; team < 10; team++) {
      long team_mask = 1 << team;
      if (!(teams & team_mask) || (flag_symmetric & team_mask))
        continue;
      for (long index = first; index < scenario_count; index++) {
        s_variant_object_datum *object = &map_variant->objects[index];
        if (object->properties.teleporter_channel == 15 && object->properties.game_engine_flags.test((e_scenario_game_engine)game_engine)) {
          const s_scenario_multiplayer_object_properties *properties = map_variant_try_and_get_multiplayer_properties(map_variant, index);
          if (properties && team == properties->team_designator && (properties->symmetry == 0 || properties->symmetry == 1)) {
            object->flags.set_unsafe((object->flags.get_unsafe() & 0xffd6) | 9);
            object->properties.team_designator = (e_variant_team_designator)team;
            object->properties.placement_flags.set(_variant_placement_symmetric_bit, true);
            object->properties.placement_flags.set(_variant_placement_asymmetric_bit, true);
            if (properties->symmetry != 0) {
              if (properties->symmetry == 2)
                object->properties.placement_flags.set(_variant_placement_symmetric_bit, false);
              else
                object->properties.placement_flags.set(_variant_placement_asymmetric_bit, false);
            }
            break;
          }
        }
      }
    }
  }
  if ((bomb_symmetric & bomb_asymmetric & teams) != teams) {
    for (long team = 0; team < 10; team++) {
      long team_mask = 1 << team;
      if (!(teams & team_mask) || (bomb_asymmetric & team_mask))
        continue;
      if (team == 8)
        continue;
      if (game_engine == 0) {
        if (team == 0)
          continue;
      } else if (game_engine == 6 && team != 0) {
        continue;
      }
      for (long index = first; index < scenario_count; index++) {
        s_variant_object_datum *object = &map_variant->objects[index];
        if (object->properties.teleporter_channel == 17 && object->properties.game_engine_flags.test((e_scenario_game_engine)game_engine)) {
          const s_scenario_multiplayer_object_properties *properties = map_variant_try_and_get_multiplayer_properties(map_variant, index);
          if (properties && team == properties->team_designator && (properties->symmetry == 0 || properties->symmetry == 2)) {
            object->flags.set_unsafe((object->flags.get_unsafe() & 0xffd6) | 9);
            object->properties.team_designator = (e_variant_team_designator)team;
            object->properties.placement_flags.set(_variant_placement_symmetric_bit, true);
            object->properties.placement_flags.set(_variant_placement_asymmetric_bit, true);
            if (properties->symmetry == 0) {
              bomb_symmetric |= team_mask;
            } else if (properties->symmetry == 2) {
              object->properties.placement_flags.set(_variant_placement_symmetric_bit, false);
            } else {
              object->properties.placement_flags.set(_variant_placement_asymmetric_bit, false);
            }
            break;
          }
        }
      }
    }
    for (long team = 0; team < 10; team++) {
      long team_mask = 1 << team;
      if (!(teams & team_mask) || (bomb_symmetric & team_mask))
        continue;
      if (team == 8)
        continue;
      for (long index = first; index < scenario_count; index++) {
        s_variant_object_datum *object = &map_variant->objects[index];
        if (object->properties.teleporter_channel == 17 && object->properties.game_engine_flags.test((e_scenario_game_engine)game_engine)) {
          const s_scenario_multiplayer_object_properties *properties = map_variant_try_and_get_multiplayer_properties(map_variant, index);
          if (properties && team == properties->team_designator && (properties->symmetry == 0 || properties->symmetry == 1)) {
            object->flags.set_unsafe((object->flags.get_unsafe() & 0xffd6) | 9);
            object->properties.team_designator = (e_variant_team_designator)team;
            object->properties.placement_flags.set(_variant_placement_symmetric_bit, true);
            object->properties.placement_flags.set(_variant_placement_asymmetric_bit, true);
            if (properties->symmetry != 0) {
              if (properties->symmetry == 2)
                object->properties.placement_flags.set(_variant_placement_symmetric_bit, false);
              else
                object->properties.placement_flags.set(_variant_placement_asymmetric_bit, false);
            }
            break;
          }
        }
      }
    }
  }
}


// @address 0x1d3820 map_variant_reset_from_encoding
extern "C" void map_variant_reset_from_encoding(c_map_variant *map_variant)
{
  struct { unsigned char data[0x7000]; long tail[2]; } buffer;
  c_bitstream packet;
  packet.m_data_size_alignment = 1;
  packet.m_data = buffer.data;
  packet.m_data_max = buffer.data + sizeof(buffer.data);
  packet.m_data_size_bytes = sizeof(buffer.data);
  packet.reset(0);
  packet.begin_writing();
  map_variant->encode(&packet);
  packet.finish_writing(0);
  packet.reset(3);
  map_variant_decode(map_variant, &packet);
}

// the state of one variant object as the simulation applies it (decode direction)

// 0x1d4ed0
// @address 0x1d4ed0 map_variant_decode_simulation_chunk
bool map_variant_decode_simulation_chunk(c_map_variant *map_variant, long chunk_index, unsigned long update_mask, long state_count,
    const s_map_variant_object_simulation_decode_state *states)
{
  for (long i = 0; i < 8; i++) {
    long variant_object_index = chunk_index * 8 + i;

    if (((1 << (i * 4)) & update_mask) != 0 && (map_variant->objects[variant_object_index].flags.get_unsafe() & 0x3ff) != 0) {
      map_variant->objects[variant_object_index].position = states[i].position;
      map_variant->objects[variant_object_index].forward = states[i].forward;
      map_variant->objects[variant_object_index].up = states[i].up;
    }
    if (((1 << (i * 4 + 1)) & update_mask) != 0) {
      if (variant_object_index >= map_variant->m_number_of_scenario_objects || states[i].flags != 0 ||
          (map_variant->objects[variant_object_index].flags.get_unsafe() & 0x3ff) == 0)
      {
        long entity_index = states[i].entity_index;
        if (entity_index != -1) {
          long object_index = simulation_gamestate_entity_get_object_index(entity_index);
          if (object_index != -1 && map_variant->objects[variant_object_index].object_index != object_index) {
            bool update_mapping = true;
            short placement_index = object_get(object_index)->map_variant_index;
            if (placement_index != -1) {
              if (placement_index != variant_object_index)
                update_mapping = false;
              else if (map_variant->objects[variant_object_index].object_index != -1)
                update_mapping = false;
            } else if (map_variant->objects[variant_object_index].object_index != -1) {
              update_mapping = false;
            }
            if (update_mapping)
              c_map_variant::find_definition_index(object_index, variant_object_index, true);
          }
        } else {
          short state_flags = (short)states[i].flags;
          e_object_removal_origin origin = _object_removal_origin_0;
          if (state_flags == 0)
            origin = _object_removal_origin_2;
          else if ((state_flags & (1 << 3)) != 0 && (state_flags & (1 << 5)) != 0)
            origin = _object_removal_origin_2;
          map_variant->remove_object(variant_object_index, origin);
        }
        if ((bool)(_rotl(states[i].flags, 26) & 1))
          map_variant->objects[variant_object_index].flags.set(_variant_object_placement_runtime_created_bit, true);
        else
          map_variant->objects[variant_object_index].flags.set(_variant_object_placement_runtime_created_bit, false);
        if ((bool)(_rotl(states[i].flags, 29) & 1))
          map_variant->objects[variant_object_index].flags.set(_variant_object_placement_runtime_candidate_bit, true);
        else
          map_variant->objects[variant_object_index].flags.set(_variant_object_placement_runtime_candidate_bit, false);
        if ((bool)(_rotl(states[i].flags, 27) & 1))
          map_variant->objects[variant_object_index].flags.set(_variant_object_placement_scenario_object_removed_bit, true);
        else
          map_variant->objects[variant_object_index].flags.set(_variant_object_placement_scenario_object_removed_bit, false);
        long parent_entity_index = states[i].parent_entity_index;
        if (parent_entity_index != -1) {
          long parent_object_index = simulation_gamestate_entity_get_object_index(parent_entity_index);
          if (parent_object_index != -1) {
            s_object_datum *parent_object = object_get(parent_object_index);
            unsigned int parent_flags = map_variant->objects[variant_object_index].flags.get_unsafe() | 0x100;
            map_variant->objects[variant_object_index].parent_object_identifier = parent_object->parent_object_identifier;
            map_variant->objects[variant_object_index].flags.set_unsafe(parent_flags);
            if ((bool)(_rotl(states[i].flags, 23) & 1))
              parent_flags |= 0x200;
            else
              parent_flags &= 0xfdff;
            map_variant->objects[variant_object_index].flags.set_unsafe(parent_flags);
          }
        } else {
          map_variant->objects[variant_object_index].parent_object_identifier.clear();
          map_variant->objects[variant_object_index].flags.set_unsafe(
              map_variant->objects[variant_object_index].flags.get_unsafe() & (unsigned short)~(0x100 | 0x200));
        }
      }
    }
    if (((1 << (i * 4 + 2)) & update_mask) != 0) {
      map_variant->objects[variant_object_index].properties.game_engine_flags.set_unsafe(states[i].game_engine_flags);
      map_variant->objects[variant_object_index].properties.placement_flags.set_unsafe(states[i].placement_flags);
      map_variant->objects[variant_object_index].properties.spawn_rate = states[i].spawn_rate;
      map_variant->objects[variant_object_index].properties.spare_clips = states[i].spare_clips;
      map_variant->objects[variant_object_index].properties.team_designator = (e_variant_team_designator)states[i].team_designator;
      memcpy(&map_variant->objects[variant_object_index].properties.boundary_width, states[i].boundary, sizeof(states[i].boundary));
      map_variant->objects[variant_object_index].properties.boundary_shape = (e_multiplayer_object_boundary_shape)states[i].boundary_shape;
      if ((bool)(_rotl(states[i].flags, 31) & 1))
        map_variant->objects[variant_object_index].flags.set(_variant_object_placement_edited_bit, true);
      else
        map_variant->objects[variant_object_index].flags.set(_variant_object_placement_edited_bit, false);
      if ((bool)(_rotl(states[i].flags, 26) & 1))
        map_variant->objects[variant_object_index].flags.set(_variant_object_placement_runtime_created_bit, true);
      else
        map_variant->objects[variant_object_index].flags.set(_variant_object_placement_runtime_created_bit, false);
      if ((bool)(_rotl(states[i].flags, 29) & 1))
        map_variant->objects[variant_object_index].flags.set(_variant_object_placement_runtime_candidate_bit, true);
      else
        map_variant->objects[variant_object_index].flags.set(_variant_object_placement_runtime_candidate_bit, false);
      if ((bool)(_rotl(states[i].flags, 27) & 1))
        map_variant->objects[variant_object_index].flags.set(_variant_object_placement_scenario_object_removed_bit, true);
      else
        map_variant->objects[variant_object_index].flags.set(_variant_object_placement_scenario_object_removed_bit, false);
      c_map_variant *game_variant = &tls_game_engine_globals()->map_info;
      s_variant_object_datum local_datum;
      game_variant->get_variant_object_datum(variant_object_index, &local_datum);
      long datum_object_index = local_datum.object_index;
      if (datum_object_index != -1)
        (game_variant)
            ->apply_multiplayer_properties(
                &local_datum.properties,
                datum_object_index);
    }
    if (((1 << (i * 4 + 3)) & update_mask) != 0) {
      long found_index = 0;
      c_map_variant *game_variant = &tls_game_engine_globals()->map_info;
      short quota_count = game_variant->m_number_of_placeable_object_quotas;
      if (quota_count > 0) {
        long quota_definition_index = states[i].quota_object_definition_index;
        s_variant_object_datum *quota_entry = &map_variant->objects[variant_object_index];
        s_variant_quota *quota = &game_variant->quotas[0];
        do {
          if (quota->object_definition_index == quota_definition_index) {
            if (found_index != -1) {
              if (quota_entry->quota_index == -1)
                quota_entry->quota_index = found_index;
              s_variant_quota *found_quota = &tls_game_engine_globals()->map_info.quotas[found_index];
              found_quota->placed_on_map = states[i].quota_placed_on_map;
              found_quota->maximum_count = states[i].quota_maximum_count;
              found_quota->minimum_count = states[i].quota_minimum_count;
            }
            break;
          }
          found_index++;
          quota++;
        } while (found_index < quota_count);
      }
    }
  }
  return true;
}


static __forceinline void scenario_map_variant_read_small_raw_data(c_bitstream *packet, void *data, long size_in_bits)
{
  unsigned char *bytes = (unsigned char *)data;
  unsigned __int64 value = bitstream_read_window_bits(packet, size_in_bits) << (unsigned long)(64 - size_in_bits);
  if (size_in_bits >= 8) {
    unsigned long byte_count = (unsigned long)size_in_bits / 8;
    size_in_bits -= byte_count * 8;
    do {
      *bytes++ = (unsigned char)(value >> 56);
      value <<= 8;
    } while (--byte_count != 0);
  }
  if (size_in_bits != 0)
    *bytes = (unsigned char)(value >> 56);
}

// @address 0x1d0388 map_variant_decode
extern "C" unsigned char map_variant_decode(c_map_variant *map_variant, c_bitstream *packet)
{
  saved_game_files_decode_metadata((s_saved_game_item_metadata_packed *)&map_variant->m_metadata, packet);
  map_variant->m_version = (short)packet->read_integer("map-variant-version", 8);
  map_variant->m_original_map_signature_hash = packet->read_integer("map-variant-checksum", 32);
  map_variant->m_number_of_scenario_objects = (short)packet->read_integer("number-of-scenario-objects", 10);
  map_variant->m_number_of_variant_objects = (short)packet->read_integer("number-of-variant-objects", 10);
  map_variant->m_number_of_placeable_object_quotas = (short)packet->read_integer("number-of-placeable-object-quotas", 9);
  bool valid = map_variant->m_number_of_variant_objects >= 0 && map_variant->m_number_of_variant_objects <= 640;
  valid = valid && map_variant->m_number_of_placeable_object_quotas >= 0 && map_variant->m_number_of_placeable_object_quotas <= 256;
  map_variant->map_id = packet->read_integer("map-id", 32);
  map_variant->m_built_in = packet->read_bool("built-in");
  packet->read_raw_data("world-bounds", &map_variant->m_bounds, 192);
  map_variant->m_game_engine_subtype = packet->read_integer("game-engine-subtype", 4);
  scenario_map_variant_read_small_raw_data(packet, &map_variant->m_maximum_budget, 32);
  scenario_map_variant_read_small_raw_data(packet, &map_variant->m_spent_budget, 32);
#if !defined(_XBOX) || defined(__clang__)
  scenario_map_variant_swap_bounds(&map_variant->m_bounds);
  scenario_map_variant_swap_long(&map_variant->m_maximum_budget);
  scenario_map_variant_swap_long(&map_variant->m_spent_budget);
#endif
  if (valid) {
    for (long object_index = 0; object_index < 640; object_index++) {
      s_variant_object_datum *object = &map_variant->objects[object_index];
      if (object_index < map_variant->m_number_of_variant_objects) {
        if (packet->read_bool("variant-object-exists")) {
          object->flags.set_unsafe((unsigned short)packet->read_integer("variant-object-flags", 16));
          object->quota_index = packet->read_integer("variant-object-quota-index", 32);
          if (packet->read_bool("variant-object-parent-exists")) {
            packet->read_raw_data("variant-object-parent", &object->parent_object_identifier, 64);
#if !defined(_XBOX) || defined(__clang__)
            scenario_map_variant_swap_object_identifier(&object->parent_object_identifier);
#endif
          } else
            object->parent_object_identifier.clear();
          if (packet->read_bool("variant-object-position-exists")) {
            ::simulation_read_quantized_position(
                packet, &object->position, 16, &map_variant->m_bounds);
            packet->read_axes("variant-object-axes", &object->forward, &object->up);
            object->properties.teleporter_channel = (unsigned char)packet->read_integer("variant-object-teleporter-channel", 8);
            object->properties.placement_flags.set_unsafe((unsigned char)packet->read_integer("variant-object-placement-flags", 8));
            object->properties.game_engine_flags.set_unsafe((unsigned short)packet->read_integer("variant-object-engine-flags", 16));
            object->properties.spare_clips = (unsigned char)packet->read_integer("variant-object-spare-clips", 8);
            object->properties.spawn_rate = (unsigned char)packet->read_integer("variant-object-spawn-rate", 8);
            object->properties.team_designator = (e_variant_team_designator)packet->read_integer("variant-object-team", 8);
            object->properties.boundary_shape = (e_multiplayer_object_boundary_shape)(char)packet->read_integer("variant-object-boundary-shape", 8);
            switch (object->properties.boundary_shape) {
            case 1:
              object->properties.boundary_width = packet->read_quantized_real("variant-object-boundary", 0.0f, 60.0f, 16, false);
              break;
            case 2:
              object->properties.boundary_width = packet->read_quantized_real("variant-object-boundary", 0.0f, 60.0f, 16, false);
              object->properties.boundary_positive_height = packet->read_quantized_real("variant-object-boundary", 0.0f, 60.0f, 16, false);
              object->properties.boundary_negative_height = packet->read_quantized_real("variant-object-boundary", 0.0f, 60.0f, 16, false);
              break;
            case 3:
              object->properties.boundary_width = packet->read_quantized_real("variant-object-boundary", 0.0f, 60.0f, 16, false);
              object->properties.boundary_box_length = packet->read_quantized_real("variant-object-boundary", 0.0f, 60.0f, 16, false);
              object->properties.boundary_positive_height = packet->read_quantized_real("variant-object-boundary", 0.0f, 60.0f, 16, false);
              object->properties.boundary_negative_height = packet->read_quantized_real("variant-object-boundary", 0.0f, 60.0f, 16, false);
              break;
            }
          }
        } else {
          object->parent_object_identifier.clear();
          object->properties.initialize_to_default();
        }
      } else {
        variant_object_datum_initialize(object);
      }
    }
    short *type_start = map_variant->object_type_start;
    unsigned long remaining_types = 14;
    do {
      *type_start = (short)(packet->read_integer("map-variant-object-type-map", 9) - 1);
      type_start++;
      remaining_types--;
    } while (remaining_types != 0);
    for (long quota_index = 0; quota_index < 256; quota_index++) {
      if (quota_index < map_variant->m_number_of_placeable_object_quotas) {
        packet->read_raw_data("variant-object-quota", &map_variant->quotas[quota_index], 96);
#if !defined(_XBOX) || defined(__clang__)
        scenario_map_variant_swap_quota(&map_variant->quotas[quota_index]);
#endif
      } else
        map_variant->quotas[quota_index].initialize_to_default();
    }
  }
  return valid;
}

// @address 0x1ce628 ?map_variant_create_helper_object@@YAXJ@Z
void map_variant_create_helper_object(long placement_index)
{
    if (game_engine_get() && game_engine_get()->get_type() == 5 && placement_index != -1)
    {
        s_variant_object_datum variant_object;
        tls_game_engine_globals()->map_info.get_variant_object_datum(placement_index, &variant_object);
        bool quota_valid = true;
        if (variant_object.quota_index != -1 && !(tls_game_engine_globals()->map_info.quotas[variant_object.quota_index].price >= 0.0f))
            quota_valid = false;
        if (quota_valid && datum_get<s_object_header>(tls_object_header_data(), variant_object.object_index)->datum->parent_object_identifier.m_type != 6)
        {
            if (variant_object.helper_object_index != -1)
                object_delete(variant_object.helper_object_index);
            object_placement_data placement;
            object_placement_data_new(&placement, game_engine_get_game_object_definition_index(1), -1, 0);
            variant_object.helper_object_index = object_new(&placement);
            if (variant_object.helper_object_index != -1)
            {
                object_set_scale(variant_object.helper_object_index, 0.3f, 0.0f, true);
                tls_game_engine_globals()->map_info.set_variant_object_datum(placement_index, &variant_object, false);
                datum_get<s_object_header>(tls_object_header_data(), variant_object.helper_object_index)->datum->map_variant_index = (short)placement_index;
                if (tls_game_engine_globals()->map_info.m_flag == 0)
                    object_set_hidden(variant_object.helper_object_index, true);
            }
        }
    }
}

// @address 0x1cef80 map_variant_initialize_for_new_map
extern "C" void map_variant_initialize_for_new_map(c_map_variant *map_variant)
{
  c_object_iterator<s_object_datum> iterator;
  iterator.begin(0xffffffff, 0);

  if (iterator.next()) {
    do {
      s_object_datum *object = iterator.get_datum();
      if (scenario_map_variant_collect_object_type(object->object_type)) {
        long variant_index = object->map_variant_index;
        if (variant_index == -1) {
          short scenario_index = object->scenario_datum_index;
          if (scenario_index != -1)
            variant_index = map_variant->get_object_index((e_object_type)object->object_type, scenario_index);
        }
        if (variant_index != -1) {
          s_variant_object_datum *variant = &map_variant->objects[variant_index];
          if (!variant->flags.is_empty() &&
              !variant->flags.test(_variant_object_placement_scenario_object_removed_bit) &&
              !variant->flags.test(_variant_object_placement_registered_bit)) {
            if (variant->object_index == iterator.get_index()) {
              game_engine_register_object(iterator.get_index());
              continue;
            }
            if (variant->object_index != -1) {
              if (game_engine_sandbox_is_active()) {
                object_delete(iterator.get_index());
                continue;
              }
              map_variant->remove_object(variant_index, _object_removal_origin_2);
            }
            c_map_variant::find_definition_index(iterator.get_index(), variant_index, 1);
            game_engine_register_object(iterator.get_index());
          } else {
            c_map_variant::find_definition_index(iterator.get_index(), variant_index, 1);
            game_engine_register_object(iterator.get_index());
          }
        } else {
          bool delete_object = true;
          if (object->parent_object_index != -1)
            delete_object = false;
          if (object->parent_object_identifier.m_source == 4)
            delete_object = false;
          if (object_is_or_contains_player(iterator.get_index()))
            delete_object = false;
          if (delete_object)
            object_delete(iterator.get_index());
        }
      }
    } while (iterator.next());
  }

  for (long index = 0; index < map_variant->m_number_of_variant_objects; index++) {
    s_variant_object_datum *variant = &map_variant->objects[index];
    if (!variant->flags.is_empty()) {
      bool removed = false;
      if (variant->flags.test(_variant_object_placement_runtime_candidate_bit) &&
          !variant->flags.test(_variant_object_placement_scenario_object_removed_bit)) {
        if (variant->object_index != -1) {
          long object_index = variant->object_index;
          if (object_get(object_index)->scenario_datum_index == -1) {
            map_variant->remove_object(index, _object_removal_origin_2);
            object_delete(object_index);
            removed = true;
            simulation_action_sandbox_group_update(index, 2, false);
          }
        }
      }
      if (removed)
        continue;

      if (variant->flags.test(_variant_object_placement_edited_bit) || variant->object_index != -1) {
        real_vector3d forward;
        real_vector3d up;
        if (variant->object_index != -1) {
          object_get_orientation(variant->object_index, &forward, &up);
        } else {
          forward = *global_forward3d;
          up = *global_up3d;
        }

        bool updated = false;
        if (!valid_real_vector3d_axes2(&variant->forward, &variant->up)) {
          variant->forward = forward;
          variant->up = up;
          updated = true;
        }
        if (updated)
          simulation_action_sandbox_group_update(index, 1, false);

        if (game_engine_sandbox_is_active() &&
            !variant->flags.test(_variant_object_placement_runtime_candidate_bit) &&
            variant->object_index == -1) {
          if (variant->quota_index != -1)
            variant->flags.set(_variant_object_placement_occupied_slot_bit, true);
          else
            variant_object_datum_initialize(variant);
        }
      }

      if (variant->object_index != -1) {
        variant->properties.initialize_to_default();
        c_map_variant::setup_object_properties(variant);
      }
    } else {
      variant_object_datum_initialize(variant);
    }
  }

  game_engine_spawn_variant_objects(&tls_game_globals()->options.map_variant);
  if (game_engine_sandbox_is_active())
    game_engine_spawn_variant_objects(map_variant);

  for (long quota_index = 0; quota_index < 256; quota_index++)
    map_variant->quotas[quota_index].placed_on_map = 0;

  for (long index = 0; index < map_variant->m_number_of_variant_objects; index++) {
    s_variant_object_datum *variant = &map_variant->objects[index];
    if (!variant->flags.is_empty() &&
        variant->quota_index != -1 &&
        !variant->flags.test(_variant_object_placement_scenario_object_removed_bit) &&
        !variant->flags.test(_variant_object_placement_registered_bit)) {
      s_variant_quota *quota = &map_variant->quotas[variant->quota_index];
      unsigned char placed_on_map = quota->placed_on_map;
      if (placed_on_map < quota->maximum_allowed) {
        quota->placed_on_map = placed_on_map + 1;
      } else {
        if (variant->object_index != -1)
          objects_purge_deleted_object(variant->object_index);
        map_variant->remove_object(index, _object_removal_origin_2);
        quota->placed_on_map = placed_on_map;
      }
    }
  }

  for (long quota_index = 0; quota_index < 256; quota_index++) {
    s_variant_quota *quota = &map_variant->quotas[quota_index];
    quota->maximum_count = SCENARIO_MAP_VARIANT_QUOTA_MIN(SCENARIO_MAP_VARIANT_QUOTA_MAX(quota->maximum_count, quota->placed_on_map), quota->maximum_allowed);
    quota->minimum_count = SCENARIO_MAP_VARIANT_QUOTA_MIN(SCENARIO_MAP_VARIANT_QUOTA_MAX(quota->minimum_count, 0), quota->placed_on_map);
  }

  map_variant_update_object_parents(map_variant);

  if (map_variant_is_valid((const s_map_variant_validation *)&tls_game_globals()->options.map_variant)) {
    if (!game_engine_sandbox_is_active()) {
      for (long index = 0; index < map_variant->m_number_of_scenario_objects; index++) {
        s_variant_object_datum *variant = &map_variant->objects[index];
        long object_index = variant->object_index;
        if (object_index == -1)
          continue;
        const s_variant_object_datum *original = scenario_map_variant_game_options_variant_object_get(index);
        s_object_datum *object = object_get(object_index);
        if (object->object_type == 6 && original != 0 && original->flags.test(_variant_object_placement_edited_bit)) {
          if (object->parent_object_index != -1) {
            object_detach(object_index);
            object_set_position_direct(variant->object_index, &original->position, &original->forward, &original->up, 0, false);
            map_variant_update_parented_object_position(map_variant, index);
          } else {
            object_set_position_direct(object_index, &original->position, &original->forward, &original->up, 0, false);
          }
        }
      }
    }
  }

  if (!game_engine_sandbox_is_active() && !tls_game_globals()->options.map_variant.m_built_in) {
    c_map_variant *original = &tls_game_globals()->options.map_variant;
    if (map_variant_is_valid((const s_map_variant_validation *)original))
      game_engine_apply_map_variant_quotas(map_variant, original);
  }
}

