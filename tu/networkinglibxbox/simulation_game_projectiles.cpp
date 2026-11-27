// networkinglibxbox/simulation_game_projectiles.obj: from the records 0x42e8c0 0x434458 0x434748 0x4353b8 0x435bb8 0x435bc8 0x435bd8 0x435be8 0x435c30 0x435c40 0x435c50 0x435c60 0x435c70 0x435c78 0x435c98 0x435ca8 0x435cb0 0x435e60 0x435ed8 0x435f00 0x4360e0 0x436120 0x436170 0x4361f8 0x436258 0x4362c8 0x436400 0x436588 0x436600 0x436650 0x436750 0x436808 0x436818 0x436878 0x436918 0x4369f8 0x436bd0 0x436c00 0x436c10 0x436c70 0x436d50 0x436e80 0x4370b0 0x4370b8 0x4370c8 0x437128 0x437240 0x437340 0x437450 0x437458 0x437460 0x4374c0 0x4375f0 0x437720 0x4378a0 0x4378b8 0x437900 0x709f78 0x70c868
// @flags /TP /O2 /Oc
#include "simulation/game_interface/simulation_game_projectiles.h"
#include "simulation/simulation_game_entity_definitions.h"
#include <string.h>
#include "networking/simulation/simulation_entity_definition.h"
#include "networking/simulation/simulation_entity_database.h"
#include "networking/simulation/simulation_view_telemetry.h"
#include "math/matrix_math_types.h"
#include "math/real_math_types.h"
#include "objects/object_types.h"
#include "objects/object_reference_types.h"
#include "math/real_math.h"
#include "math/real_constant_pool_array_legacy.h"
#include "game/game_globals.h"
#include "objects/objects.h"
#include "scenario/scenario.h"
#include "sound/game_sound.h"
#include "cache/cache_files.h"
#include "items/projectiles.h"
#include "items/projectile_types.h"
#include "units/vehicle_definitions.h"
#include "simulation/game_interface/simulation_game_entities.h"
#include "simulation/game_interface/simulation_game_internal.h"
#include "simulation/simulation_encoding.h"
#include "simulation/simulation_gamestate_entities.h"
#include "motor/action_system.h"
#include "memory/bitstream.h"
#include "simulation/simulation_priority.h"
#include "objects/object_datum.h"
#include "memory/data_types.h"
#include "memory/data.h"
#include "memory/datum_index.h"
#include "objects/object_placement.h"
#include "simulation/game_interface/simulation_game_object_data_types.h"
#include "simulation/game_interface/simulation_game_vehicles_internals.h"
#include "simulation/simulation_entity_types.h"
struct s_simulation_object_creation_data;
struct s_simulation_object_state_data;
struct object_placement_data;
struct s_simulation_object_state_data;
class c_bitstream;
union real_point3d;
union real_rectangle3d;
// @address 0x42e8c0 ?payload_size@c_simulation_projectile_impact_effect_event_definition@@UAAJXZ
// @address 0x434748 ?prepare_state_data_for_gameworld@c_simulation_projectile_entity_definition@@UAAXKJPAX@Z
// @address 0x435bb8 ?event_type_name@c_simulation_projectile_attached_event_definition@@UAAPBDXZ
// @address 0x435bd8 ?event_type_name@c_simulation_projectile_effect_event_definition@@UAAPBDXZ
// @address 0x435c30 ?event_type_name@c_simulation_projectile_impact_effect_event_definition@@UAAPBDXZ
// @address 0x435c50 ?event_type_name@c_simulation_projectile_object_impact_effect_event_definition@@UAAPBDXZ
// @address 0x4957c ??_7c_simulation_projectile_supercombine_request_event_definition@@6B@
// @address 0x435c78 ?event_type_name@c_simulation_projectile_supercombine_request_event_definition@@UAAPBDXZ
// @address 0x435c98 ?entity_type_name@c_simulation_projectile_entity_definition@@UAAPBDXZ
// @address 0x435ca8 ?state_data_size@c_simulation_projectile_entity_definition@@UAAJXZ
// @address 0x435cb0 ?build_object_creation_data@c_simulation_projectile_entity_definition@@UAAXJJPAX@Z
// @address 0x435ed8 ?prepare_creation_data_for_gameworld@c_simulation_projectile_entity_definition@@UAAXJPAX@Z
// @address 0x4360e0 ?handle_delete_object@c_simulation_projectile_entity_definition@@UAA_NJ@Z
// @address 0x436120 ?apply_object_update@c_simulation_projectile_entity_definition@@UAAXJKJPBX@Z
// @address 0x436170 ?entity_can_be_created@c_simulation_projectile_entity_definition@@UAA_NPBUs_simulation_entity@@PBUs_simulation_view_telemetry_data@@@Z
// @address 0x4361f8 ?creation_minimum_required_bits@c_simulation_projectile_entity_definition@@UAAJPBUs_simulation_entity@@PBUs_simulation_view_telemetry_data@@@Z
// @address 0x436400 ?entity_creation_decode@c_simulation_projectile_entity_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
// @address 0x436588 ?rotate_entity_indices@c_simulation_projectile_entity_definition@@UAAXPBUs_simulation_entity@@@Z
// @address 0x436bd0 ?minimum_required_bits@c_simulation_projectile_effect_event_definition@@UAAJPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
// @address 0x436818 ?write_description_to_string@c_simulation_projectile_attached_event_definition@@UAAXPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@MJPAD@Z
// @address 0x436c00 ?calculate_relevance@c_simulation_projectile_effect_event_definition@@UAAMPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@M@Z
// @address 0x436c10 ?write_description_to_string@c_simulation_projectile_effect_event_definition@@UAAXPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@MJPAD@Z
// @address 0x4370c8 ?write_description_to_string@c_simulation_projectile_impact_effect_event_definition@@UAAXPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@MJPAD@Z
// @address 0x437450 ?payload_size@c_simulation_projectile_object_impact_effect_event_definition@@UAAJXZ
// @address 0x437458 ?minimum_required_bits@c_simulation_projectile_object_impact_effect_event_definition@@UAAJPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
// @address 0x437460 ?write_description_to_string@c_simulation_projectile_supercombine_request_event_definition@@UAAXPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@MJPAD@Z
// @address 0x4378a0 ?event_payload_encode@c_simulation_projectile_supercombine_request_event_definition@@UAAXJPBXPAVc_bitstream@@_N@Z
// @address 0x4378b8 ?event_payload_decode@c_simulation_projectile_supercombine_request_event_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
// @address 0x437240 ?event_payload_decode@c_simulation_projectile_impact_effect_event_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
// @address 0x437128 ?event_payload_encode@c_simulation_projectile_impact_effect_event_definition@@UAAXJPBXPAVc_bitstream@@_N@Z
// @address 0x436600 ?update_minimum_required_bits@c_simulation_projectile_entity_definition@@UAAJPBUs_simulation_entity@@PBUs_entity_update_data@@@Z
// @address 0x436d50 ?event_payload_decode@c_simulation_projectile_effect_event_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
// @address 0x437900 ?apply_game_event@c_simulation_projectile_supercombine_request_event_definition@@UAA_NJPBJJPBX@Z
// @address 0x436878 ?event_payload_encode@c_simulation_projectile_attached_event_definition@@UAAXJPBXPAVc_bitstream@@_N@Z
// @address 0x436c70 ?event_payload_encode@c_simulation_projectile_effect_event_definition@@UAAXJPBXPAVc_bitstream@@_N@Z
// @address 0x4370b0 ?minimum_required_bits@c_simulation_projectile_impact_effect_event_definition@@UAAJPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
// @address 0x4370b8 ?calculate_relevance@c_simulation_projectile_impact_effect_event_definition@@UAAMPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@M@Z
// @address 0x4353b8 ?create_object@c_simulation_vehicle_entity_definition@@UAAJJPBXPAKJ0@Z
// @address 0x435e60 ?handle_object_update@c_simulation_projectile_entity_definition@@UAAKJKJPAX@Z
// @address 0x436650 ?entity_update_encode@c_simulation_projectile_entity_definition@@UAA_N_NKPAKJPBXPBUs_simulation_view_telemetry_data@@PAVc_bitstream@@J0@Z
// @address 0x436750 ?entity_update_decode@c_simulation_projectile_entity_definition@@UAA_N_NPAKJPAXPAVc_bitstream@@0@Z
// @address 0x4369f8 ?apply_game_event@c_simulation_projectile_attached_event_definition@@UAA_NJPBJJPBX@Z
// @address 0x437340 ?apply_game_event@c_simulation_projectile_impact_effect_event_definition@@UAA_NJPBJJPBX@Z
// @address 0x437720 ?apply_game_event@c_simulation_projectile_object_impact_effect_event_definition@@UAA_NJPBJJPBX@Z
// @address 0x4374c0 ?event_payload_encode@c_simulation_projectile_object_impact_effect_event_definition@@UAAXJPBXPAVc_bitstream@@_N@Z
// @address 0x4375f0 ?event_payload_decode@c_simulation_projectile_object_impact_effect_event_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z

struct s_simulation_object_creation_data;
struct s_simulation_object_state_data;
struct object_placement_data;

#include "math/real_constant_pool.h"

union real_rectangle3d;

#include "networking/replication/replication_entity.h"
struct s_simulation_object_creation_data;
struct s_simulation_object_state_data;
#include "simulation/game_interface/simulation_game_object_entity_definition.h"
#include "networking/replication/replication_event.h"
#include "simulation/simulation_node_space_encoding_types.h"
#include "objects/object_header_access.h"
#include "memory/tls_data_arrays.h"

#include "networking/simulation/simulation_event_definition.h"

#include "sound/game_sound_types.h"

#include "game/player_types.h"

#include "items/projectile_definitions.h"
#include "networking/network_configuration_types.h"
#include "networking/network_configuration.h"
#include "game/materials.h"
#include "math/matrix_math_functions.h"
#include "scenario/cluster_reference_helpers.h"
#include "simulation/game_interface/simulation_game_object_creation_inline.h"
#include "tag_files/tag_groups.h"

static __forceinline void simulation_game_projectiles_projectile_set_action(long object_index, short action)
{
    projectile_datum *projectile = ((projectile_datum *)object_get_and_verify_type(object_index, 32));
    if (action > projectile->projectile.action)
        projectile->projectile.action = (signed char)action;
}
static __forceinline long simulation_game_projectiles_object_creation_required_bits()
{
    long bits = 98;
    bits += 12;
    return bits;
}
static __forceinline long rotate_index(long index)
{
  int salt = (int)((unsigned long)index >> 28);
  salt = (salt + 1) % 16;
  return (salt << 28) | (index & 0x3ff);
}

static __forceinline void simulation_game_projectiles_object_prepare_state(unsigned long update_mask, s_projectile_state_data *state)
{
  if ((update_mask & 0x800) != 0)
    state->object.parent_gamestate_index = simulation_entity_get_gamestate_index(state->object.parent_entity_index);
}

static __forceinline long simulation_game_projectiles_effect_required_bits(const s_simulation_projectile_effect_event_data *payload)
{
    long bits = 72;
    if ((payload->flags & 4) != 0)
    {
        bits += 28;
        if (payload->has_material_effect)
            bits += 8;
    }
    return bits;
}

extern "C" {
// @address 0x49408 ??_7c_simulation_projectile_entity_definition@@6B@
// @address 0x709f78 ??__Eg_simulation_projectile_entity_definition@@YAXXZ
// @address 0x70c868 ??__Fg_simulation_projectile_entity_definition@@YAXXZ
// @address 0x7bb4d8 g_simulation_projectile_entity_definition
c_simulation_projectile_entity_definition g_simulation_projectile_entity_definition;
// @address 0x4949c ??_7c_simulation_projectile_attached_event_definition@@6B@
// @address 0x7bb4dc g_simulation_projectile_attached_event_definition
c_simulation_projectile_attached_event_definition g_simulation_projectile_attached_event_definition;
// @address 0x494d4 ??_7c_simulation_projectile_effect_event_definition@@6B@
// @address 0x7bb4e0 g_simulation_projectile_effect_event_definition
c_simulation_projectile_effect_event_definition g_simulation_projectile_effect_event_definition;
// @address 0x4950c ??_7c_simulation_projectile_impact_effect_event_definition@@6B@
// @address 0x7bb4e4 g_simulation_projectile_impact_effect_event_definition
c_simulation_projectile_impact_effect_event_definition g_simulation_projectile_impact_effect_event_definition;
// @address 0x49544 ??_7c_simulation_projectile_object_impact_effect_event_definition@@6B@
// @address 0x7bb4e8 g_simulation_projectile_object_impact_effect_event_definition
c_simulation_projectile_object_impact_effect_event_definition g_simulation_projectile_object_impact_effect_event_definition;
// @address 0x7bb4ec g_simulation_supercombine_request_event_definition
c_simulation_projectile_supercombine_request_event_definition g_simulation_supercombine_request_event_definition;
}
static __forceinline object_datum *simulation_game_projectiles_object_datum(long object_index)
{
    return (object_datum *)(*datum_get<s_object_header>(object_header_data, object_index)).datum;
}
static __forceinline real_matrix4x3 *simulation_game_projectiles_object_node_matrices(long object_index, long *count)
{
    object_datum *object = simulation_game_projectiles_object_datum(object_index);
    *count = object->node_matrices.size / sizeof(real_matrix4x3);
    return (real_matrix4x3 *)((char *)object + object->node_matrices.offset);
}
static __forceinline c_global_material_type simulation_game_projectiles_material_from_index(short index)
{
    c_global_material_type material;
    material.m_index = -1;
    if (index != -1)
    {
        if (index >= 0 && index < global_game_globals->material_count)
            material.m_index = (short)index;
    }
    else
        material.m_index = -1;
    return material;
}

// 0x42e8c0
long c_simulation_projectile_impact_effect_event_definition::payload_size()
{
  return sizeof(s_simulation_projectile_impact_effect_event_data);
}

// 0x434748
void c_simulation_object_entity_definition::object_entity_prepare_state_data(unsigned long update_mask, long size, void *state)
{
  unsigned long masked = update_mask & 0x3fff;
  if (masked != 0)
    simulation_game_projectiles_object_prepare_state(masked, (s_projectile_state_data *)state);
}

long c_simulation_vehicle_entity_definition::create_object(long p1, const void *p2, unsigned long *p3, long p4, const void *p5)
{
  const s_simulation_object_creation_data *thing = (const s_simulation_object_creation_data *)p2;
  long result = -1;
  struct s_projectile_placement
{
    char pad0[0xc];
    long value;
    char pad10[0x154 - 0x10];
    unsigned short a;
    char pad156[5];
    unsigned char b;
    char pad15c[0x180 - 0x15c];
};
  // match: 0x4353b8: the 0x180-byte placement prefix is reserved on the stack.
  s_projectile_placement local;
  local.a = 0;
  local.b = 0;
  if (thing->definition_index != -1 && tag_try_and_get_unsafe(0x76656869, thing->definition_index) != 0)
  {
    ::c_simulation_object_entity_definition::object_setup_placement_data((s_simulation_object_creation_data const *)p2,
        (s_simulation_object_state_data const *)p5, (unsigned long *)(p3), (object_placement_data *)&local);
    local.value = ((const s_simulation_projectile_creation_data *)p2)->owner_player_index;
    result = object_create_object(
        (s_simulation_object_creation_data const *)p2, (s_simulation_object_state_data const *)p5, (unsigned long *)(p3),
        (object_placement_data *)&local);
    if (result != -1)
    {
      object_set_at_rest(result, true);
      unsigned char flag = (((const vehicle_definition *)global_tag_instances[datum_index_to_absolute_index(thing->definition_index)].definition)->flags
        >> 13) & 1;
      if (flag)
        action_submit_event(result, 19);
    }
  }
  return result;
}

// 0x435bb8
const char *c_simulation_projectile_attached_event_definition::event_type_name()
{
  return "projectile_attached";
}

// 0x435bd8
const char *c_simulation_projectile_effect_event_definition::event_type_name()
{
  return "projectile_effect";
}

// 0x435c30
const char *c_simulation_projectile_impact_effect_event_definition::event_type_name()
{
  return "projectile_impact_effect";
}

// 0x435c50
const char *c_simulation_projectile_object_impact_effect_event_definition::event_type_name()
{
  return "projectile_object_impact_effect";
}

// @address 0x435c70 ?event_type@c_simulation_projectile_supercombine_request_event_definition@@UAA?AW4e_simulation_event_type@@XZ
e_simulation_event_type c_simulation_projectile_supercombine_request_event_definition::event_type()
{
  return _simulation_event_type_projectile_supercombine_request;
}

// 0x435c78
const char *c_simulation_projectile_supercombine_request_event_definition::event_type_name()
{
  return "projectile_supercombine_request";
}

// @address 0x3fc550 ?payload_size@c_simulation_projectile_supercombine_request_event_definition@@UAAJXZ
long c_simulation_projectile_supercombine_request_event_definition::payload_size() { return 4; }
// @address 0x42d380 ?number_of_entity_references@c_simulation_projectile_supercombine_request_event_definition@@UAAJXZ
long c_simulation_projectile_supercombine_request_event_definition::number_of_entity_references() { return 1; }
// @address 0x42d298 ?reference_delays_entity_deletion@c_simulation_projectile_supercombine_request_event_definition@@UAA_NXZ
bool c_simulation_projectile_supercombine_request_event_definition::reference_delays_entity_deletion() { return false; }
// @address 0x42d380 ?event_can_be_transmitted@c_simulation_projectile_supercombine_request_event_definition@@UAA_NPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
bool c_simulation_projectile_supercombine_request_event_definition::event_can_be_transmitted(
    const c_replication_outgoing_event *, const s_simulation_view_telemetry_data *) { return true; }
// @address 0x42e628 ?minimum_required_bits@c_simulation_projectile_supercombine_request_event_definition@@UAAJPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
long c_simulation_projectile_supercombine_request_event_definition::minimum_required_bits(
    const c_replication_outgoing_event *, const s_simulation_view_telemetry_data *) { return 12; }
// @address 0x4311b8 ?calculate_relevance@c_simulation_projectile_supercombine_request_event_definition@@UAAMPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@M@Z
float c_simulation_projectile_supercombine_request_event_definition::calculate_relevance(
    const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry, float relevance)
{
    return simulation_calculate_event_relevance(this, event, relevance, telemetry, 0);
}
// @address 0x435c88 ?event_requirements@c_simulation_projectile_supercombine_request_event_definition@@UAA?AUs_simulation_event_requirements@@PBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
s_simulation_event_requirements c_simulation_projectile_supercombine_request_event_definition::event_requirements(
    const c_replication_outgoing_event *, const s_simulation_view_telemetry_data *)
{
    s_simulation_event_requirements requirements;
    requirements.reference_count = 1;
    requirements.reference_delays_entity_deletion = false;
    requirements.can_be_transmitted = true;
    requirements.minimum_required_bits = 12;
    return requirements;
}
// @address 0x5bec98 ?prepare_event_data_for_gameworld@c_simulation_projectile_supercombine_request_event_definition@@UAAXJPAX@Z
void c_simulation_projectile_supercombine_request_event_definition::prepare_event_data_for_gameworld(long, void *) {}

// 0x435c98
const char *c_simulation_projectile_entity_definition::entity_type_name()
{
  return "projectile";
}

// 0x435ca8
long c_simulation_projectile_entity_definition::state_data_size()
{
  // Entity creation reserves the complete projectile state buffer.
  return sizeof(s_projectile_state_data);
}

static __forceinline projectile_datum *simulation_game_projectiles_build_datum(long object_index)
{
    return (projectile_datum *)(*datum_get<s_object_header>(object_header_data, object_index)).datum;
}
static __forceinline bool simulation_game_projectiles_targeting_valid(const s_projectile_targeting_data *target)
{
    return target->object_index != -1 && object_try_and_get_and_verify_type(target->object_index, -1) != 0;
}
static __forceinline long simulation_game_projectiles_gamestate_entity_index(long gamestate_index)
{
    s_simulation_gamestate_entity_datum *entity = datum_get<s_simulation_gamestate_entity_datum>(g_simulation_gamestate_entity_data, gamestate_index);
    return entity->entity_index;
}
void c_simulation_projectile_entity_definition::build_object_creation_data(long object_index, long size, void *creation_data)
{
    s_simulation_projectile_creation_data *data = (s_simulation_projectile_creation_data *)creation_data;
    projectile_datum *projectile = simulation_game_projectiles_build_datum(object_index);
    memset(data, 0, sizeof(*data));
    object_build_creation_data(object_index, &data->object_creation);
    if (projectile->object.damage_owner.player_index != -1)
        data->owner_player_index = datum_index_to_absolute_index(projectile->object.damage_owner.player_index);
    else
        data->owner_player_index = -1;
    data->target_entity_index = -1;
    data->target_model_target_index = -1;
    if (simulation_game_projectiles_targeting_valid((const s_projectile_targeting_data *)&projectile->projectile.target_object_index))
    {
        projectile_datum *target = simulation_game_projectiles_build_datum(projectile->projectile.target_object_index);
        long gamestate_index = target->object.gamestate_index;
        long entity_index = -1;
        if (gamestate_index != -1)
        {
            entity_index = simulation_game_projectiles_gamestate_entity_index(gamestate_index);
        }
        data->target_entity_index = entity_index;
        data->target_model_target_index = projectile->projectile.target_node_index;
    }
    data->tracer = (projectile->projectile.flags.m_storage & (1 << 1)) != 0;
    data->disable_deceleration = (projectile->projectile.flags.m_storage & (1 << 8)) != 0;
}

// 0x435e60
unsigned long c_simulation_projectile_entity_definition::handle_object_update(long p1, unsigned long p2, long p3, void *p4)
{
  unsigned long result = ::c_simulation_object_entity_definition::object_handle_update(p1, p2 & 0x3fff, (s_simulation_object_state_data *)p4);
  if ((p2 & 0x4000) != 0)
  {
    unsigned char at_rest = (unsigned char)((simulation_game_projectiles_object_datum(p1)->physics_flags >> 9) & 1);
    if (at_rest != ((s_projectile_entity_update_state *)p4)->at_rest_byte)
    {
      result |= 0x4000;
      ((s_projectile_entity_update_state *)p4)->at_rest_byte = at_rest;
    }
  }
  return result;
}

// 0x435ed8
void c_simulation_projectile_entity_definition::prepare_creation_data_for_gameworld(long size, void *creation_data)
{
  s_simulation_projectile_creation_data *creation = (s_simulation_projectile_creation_data *)creation_data;
  creation->target_gamestate_index = simulation_entity_get_gamestate_index(creation->target_entity_index);
}

// 0x4360e0
bool c_simulation_projectile_entity_definition::handle_delete_object(long object_index)
{
  simulation_game_projectiles_projectile_set_action(object_index, 2);
  return false;
}

// 0x436120
void c_simulation_projectile_entity_definition::apply_object_update(long object_index, unsigned long update_mask, long state_data_size, const void *state_data)
{
  const s_projectile_state_data *state = (const s_projectile_state_data *)state_data;
  if (update_mask & 0x3fff)
    object_apply_update(object_index, update_mask & 0x3fff, (const s_simulation_object_state_data *)state);
  if (update_mask & 0x4000)
    object_set_at_rest(object_index, state->at_rest);
}
bool c_simulation_projectile_entity_definition::entity_can_be_created(
    const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry)
{
    const s_simulation_projectile_creation_data *creation_data = (const s_simulation_projectile_creation_data *)entity->creation_data;
    const s_projectile_state_data *state_data = (const s_projectile_state_data *)entity->state_data;
    bool valid = true;
    if (creation_data->target_entity_index != -1 && !telemetry->provider->entity_is_active(creation_data->target_entity_index))
        valid = false;
    return valid && object_can_be_created(
        &creation_data->object_creation, &state_data->object, telemetry);
}

// 0x4361f8
long c_simulation_projectile_entity_definition::creation_minimum_required_bits(
    const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry)
{
  const s_simulation_projectile_creation_data *creation_data = (const s_simulation_projectile_creation_data *)entity->creation_data;
  long target_bits;
  if (creation_data->target_entity_index != -1)
    target_bits = 14;
  else
    target_bits = 0;
  return simulation_game_projectiles_object_creation_required_bits() + 1 + 4 * (creation_data->owner_player_index != -1) + 1 + target_bits +
    (creation_data->target_entity_index != -1 ? (creation_data->target_model_target_index != -1 ? 5 : 0) + 1 : 0) + 2;
}

extern "C" {
 /* "projectile creation: relevance=%5.3f" */
}

// @address 0x436258 ?write_creation_description_to_string@c_simulation_projectile_entity_definition@@UAAXPBUs_simulation_entity@@PBUs_simulation_view_telemetry_data@@JPAD@Z
void c_simulation_projectile_entity_definition::write_creation_description_to_string(
    const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry, long buffer_size, char *buffer)
{
    float relevance;

    relevance = 0.0f;
    simulation_calculate_entity_creation_priority(entity, telemetry, &relevance);
    csnzprintf(buffer, buffer_size, "projectile creation: relevance=%5.3f", relevance);
}

// @address 0x4362c8 ?entity_creation_encode@c_simulation_projectile_entity_definition@@UAAXJPBXPBUs_simulation_view_telemetry_data@@PAVc_bitstream@@_N@Z
void c_simulation_projectile_entity_definition::entity_creation_encode(long creation_data_size, const void *creation_data,
    const s_simulation_view_telemetry_data *telemetry, c_bitstream *packet, bool final_update)
{
  const s_simulation_projectile_creation_data *data = (const s_simulation_projectile_creation_data *)creation_data;
  c_simulation_object_entity_definition::object_creation_encode((const s_simulation_object_creation_data *)creation_data, packet, final_update);
  packet->write_bool("owner-player-index", data->owner_player_index != -1);
  if (data->owner_player_index != -1)
    packet->write_integer("owner-player-index", data->owner_player_index, 4);
  bool target_exists = false;
  if (final_update) {
    packet->write_bool("target-entity-index", data->target_entity_index != -1);
    if (data->target_entity_index != -1) {
      replication_entity_index_encode(packet, data->target_entity_index);
      target_exists = true;
    }
  } else {
    packet->write_bool("target-gamestate-index", data->target_gamestate_index != -1);
    if (data->target_gamestate_index != -1) {
      packet->write_integer("target-gamestate-index", data->target_gamestate_index, 32);
      target_exists = true;
    }
  }
  if (target_exists) {
    packet->write_bool("target-model-target-index", data->target_model_target_index != -1);
    if (data->target_model_target_index != -1)
      packet->write_integer("target-model-target-index", data->target_model_target_index, 5);
  }
  packet->write_bool("tracer", data->tracer);
  packet->write_bool("disable-deceleration", data->disable_deceleration);
}

bool c_simulation_projectile_entity_definition::entity_creation_decode(long creation_data_size, void *creation_data, c_bitstream *packet, bool final_update)
{
  s_simulation_projectile_creation_data *decoded = (s_simulation_projectile_creation_data *)creation_data;
  bool success = true;
  success = c_simulation_object_entity_definition::object_creation_decode(
                (s_simulation_object_creation_data *)creation_data, packet, final_update) && success;
  if (packet->read_bool("owner-player-valid")) {
    decoded->owner_player_index = packet->read_integer("owner-player-index", 4);
    success = success && decoded->owner_player_index >= 0 && decoded->owner_player_index < 16;
  } else {
    decoded->owner_player_index = -1;
  }
  if (packet->read_bool("target-valid")) {
    if (final_update) {
      long low = packet->read_integer("entity-index", 10);
      decoded->target_entity_index = (packet->read_integer("target-entity-index", 4) << 28) | low;
    } else {
      decoded->target_gamestate_index = packet->read_integer("target-gamestate-index", 32);
    }
    if (packet->read_bool("target-model-target-valid"))
      decoded->target_model_target_index = packet->read_integer("target-model-target-index", 5);
    else
      decoded->target_model_target_index = -1;
  } else {
    decoded->target_entity_index = -1;
    decoded->target_gamestate_index = -1;
    decoded->target_model_target_index = -1;
  }
  decoded->tracer = packet->read_bool("tracer");
  decoded->disable_deceleration = packet->read_bool("disable-deceleration");
  return success && !packet->overflowed();
}

void c_simulation_projectile_entity_definition::rotate_entity_indices(const s_simulation_entity *entity)
{
  s_projectile_state_data *data = (s_projectile_state_data *)entity->state_data;
  s_simulation_projectile_creation_data *creation_data = (s_simulation_projectile_creation_data *)entity->creation_data;
  if (data->object.parent_entity_index != -1)
    data->object.parent_entity_index = rotate_index(data->object.parent_entity_index);
  if (creation_data->target_entity_index != -1)
    creation_data->target_entity_index = rotate_index(creation_data->target_entity_index);
}

long c_simulation_projectile_entity_definition::update_minimum_required_bits(const s_simulation_entity *entity, const s_entity_update_data *update)
{
    unsigned long update_mask = update->update_mask;
    long minimum_bits;
    object_calculate_update_requirements(entity, update, &minimum_bits);
    minimum_bits += 1;
    if ((update_mask & 0x4000) != 0)
    {
        long projectile_bits = 16;
        minimum_bits = minimum_bits > projectile_bits ? projectile_bits : minimum_bits;
    }
    return minimum_bits;
}

// 0x436650
bool c_simulation_projectile_entity_definition::entity_update_encode(bool p1, unsigned long p2, unsigned long *p3, long p4, const void *p5,
    const s_simulation_view_telemetry_data *telemetry, c_bitstream *bitstream, long p8, bool p9)
{
  bool result = false;
  if (c_simulation_object_entity_definition::object_update_encode(p1,
    p2 & 0x3fff,
    p3,
    telemetry,
    (const s_simulation_object_state_data *)p5,
    bitstream,
    p8 + 1,
    ensure_object_position_update_quantization_inside_bsp(),
    p9))
  {
    c_entity_update_encode_helper local;
    if (local.make_room_for_update(bitstream, p8, 14, 1, p2 & 0x4000))
    {
      if (local.write_component_header(14, "at-rest-exists"))
        bitstream->write_bool("at-rest", ((const s_projectile_entity_update_state *)p5)->at_rest);
      local.finish_component();
      *p3 = local.m_update_written_mask | *p3;
      result = true;
    }
  }
  return result;
}

// 0x436750
bool c_simulation_projectile_entity_definition::entity_update_decode(bool p1, unsigned long *p2, long p3, void *p4, c_bitstream *bitstream, bool p6)
{
  s_projectile_entity_update_state *state = (s_projectile_entity_update_state *)p4;
  bool object_success = c_simulation_object_entity_definition::object_update_decode(p1, p2, (s_simulation_object_state_data *)state, bitstream, p6);
  bool success = true;
  if (bitstream->read_bool("at-rest-updated"))
  {
    state->at_rest = bitstream->read_bool("at-rest");
    *p2 |= 1 << 14;
  }
  success = success && object_success;
  success = success && !bitstream->overflowed();
  return success;
}

long c_simulation_projectile_effect_event_definition::minimum_required_bits(const c_replication_outgoing_event *p,
    const s_simulation_view_telemetry_data *telemetry)
{
    const s_simulation_projectile_effect_event_data *q = (const s_simulation_projectile_effect_event_data *)p->get_payload_data();
    int result = 72;
    if ((q->flags & 4) != 0) {
        unsigned char f = q->has_material_effect;
        result = 100;
        if (f != 0)
            result = 108;
    }
    return result;
}

// 0x436c00
float c_simulation_projectile_effect_event_definition::calculate_relevance(
    const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry, float maximum_distance)
{
  const s_simulation_projectile_effect_event_data *payload = (const s_simulation_projectile_effect_event_data *)event->get_payload_data();
  return simulation_calculate_position_relevance(&payload->position, maximum_distance, telemetry);
}

struct s_effect_vector;

static __forceinline void simulation_game_projectiles_sound_detonation_effect(long definition_index,
    const real_point3d *position, const real_vector3d *forward)
{
    const projectile_definition *definition =
        (const projectile_definition *)global_tag_instances[datum_index_to_absolute_index(definition_index)].definition;
    if (definition->detonation_sound.index != -1)
    {
        s_location location;
        struct { s_sound_location sound; s_location location; } spatial;
        scenario_location_from_point(&location, position);
        spatial.sound.position = *position;
        game_sound_location_set_rotation_from_direction(&spatial.sound, forward);
        spatial.sound.velocity = *global_zero_vector3d;
        spatial.location = location;
        unattached_impulse_sound_new(definition->detonation_sound.index, &spatial.sound, 1.0f);
    }
}

static inline bool simulation_game_projectiles_location_valid(const s_location *location)
{
    return cluster_reference_valid(&location->cluster_reference);
}

static __forceinline const s_location *simulation_game_projectiles_location_set(s_location *location,
    long bsp_index, long cluster_index, long leaf_index)
{
    location->leaf_index = (unsigned short)leaf_index;
    cluster_reference_set(&location->cluster_reference, bsp_index, cluster_index);
    return location;
}

// @address 0x436e80
// @address 0x436e80 ?apply_game_event@c_simulation_projectile_effect_event_definition@@UAA_NJPBJJPBX@Z
bool c_simulation_projectile_effect_event_definition::apply_game_event(long reference_gamestate_count,
    const long *gamestate_indices, long payload_size, const void *payload)
{
    const s_simulation_projectile_effect_event_data *event_data = (const s_simulation_projectile_effect_event_data *)payload;
    if (event_data->flags != 0 && event_data->definition_index != -1 && tag_try_and_get_unsafe(0x70726f6a, event_data->definition_index))
    {
        s_location effect_location;
        location_invalidate(&effect_location);
        bool location_valid = false;
        if ((event_data->flags & 1) != 0 || (event_data->flags & 2) != 0 || (event_data->flags & 4) != 0)
        {
            if (event_data->has_material_effect)
                simulation_game_projectiles_location_set(&effect_location, 0, event_data->material_effect_index, 0);
            if (!event_data->has_material_effect || simulation_game_projectiles_location_valid(&effect_location))
                location_valid = true;
        }
        if (((event_data->flags & 1) != 0 || (event_data->flags & 2) != 0) && location_valid)
            projectile_create_detonation_effect(event_data->definition_index, &event_data->position, &event_data->forward, 0, 0, 0,
                (event_data->flags & 2) != 0, (event_data->flags & 16) != 0, &effect_location, 0);
        if ((event_data->flags & 8) != 0)
            simulation_game_projectiles_sound_detonation_effect(event_data->definition_index, &event_data->position, &event_data->forward);
        if ((event_data->flags & 4) != 0)
        {
            c_global_material_type material = simulation_game_projectiles_material_from_index((short)event_data->material_index);
            if ((event_data->material_index == -1 || material.m_index != -1) && location_valid)
                projectile_create_impact_effects(event_data->definition_index, &event_data->position, &event_data->forward, 0, material,
                    &event_data->material_normal, &effect_location);
        }
    }
    return true;
}

static __forceinline void simulation_game_projectiles_copy_vector3d(real_vector3d *destination, const real_vector3d *source)
{
    *destination = *source;
}

void c_simulation_projectile_attached_event_definition::write_description_to_string(
    const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry, float maximum_distance, long buffer_size, char *buffer)
{
  csnzprintf(buffer, buffer_size, "projectile attached: relevance=%5.3f", calculate_relevance(event, telemetry, maximum_distance));
}
void c_simulation_projectile_effect_event_definition::write_description_to_string(
    const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry, float maximum_distance, long buffer_size, char *buffer)
{
  csnzprintf(buffer, buffer_size, "projectile effect: relevance=%5.3f", calculate_relevance(event, telemetry, maximum_distance));
}

// 0x436878
void c_simulation_projectile_attached_event_definition::event_payload_encode(long payload_size, const void *payload, c_bitstream *packet, bool quantize)
{
  const s_simulation_projectile_attached_event_data *event_data = (const s_simulation_projectile_attached_event_data *)payload;
  packet->write_bool("attached-to-object", event_data->attached_to_object);
  if (event_data->attached_to_object)
  {
    packet->write_integer("node-index", event_data->node_index, 8);
    packet->write_integer("position-encoding-type", event_data->position_encoding_type, 1);
    simulation_write_quantized_node_space_position(
        packet, &event_data->position, (e_simulation_node_space_encoding_type)event_data->position_encoding_type, 13);
  }
  else
  {
    simulation_write_quantized_position(packet, &event_data->position, 13, false, 0);
  }
  packet->write_raw_data("location", event_data->location, 32);
}

// 0x436918
// @address 0x436918 ?event_payload_decode@c_simulation_projectile_attached_event_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
bool c_simulation_projectile_attached_event_definition::event_payload_decode(long payload_size, void *payload, c_bitstream *packet, bool quantize)
{
    s_simulation_projectile_attached_event_data *event_data = (s_simulation_projectile_attached_event_data *)payload;
    event_data->attached_to_object = packet->read_bool("attached-to-object");
    if (event_data->attached_to_object)
    {
        event_data->node_index = packet->read_integer("node-index", 8);
        event_data->position_encoding_type = packet->read_integer("position-encoding-type", 1);
        simulation_read_quantized_node_space_position(packet, &event_data->position, event_data->position_encoding_type, 13);
    }
    else
    {
        event_data->node_index = -1;
        event_data->position_encoding_type = 0;
        ::simulation_read_quantized_position(packet, &event_data->position, 13, 0);
    }
    unsigned char *location = event_data->location;
    unsigned __int64 value = bitstream_read_window_bits(packet, 32);
    __int64 shifted = value << 32;
    unsigned long count = 4;
    do
    {
        unsigned __int64 byte = (unsigned __int64)shifted >> 56;
        count = count - 1;
        shifted = shifted << 8;
        *location = (unsigned char)byte;
        location = location + 1;
    } while (count != 0);
    return true;
}

static __forceinline player_datum *simulation_game_projectiles_player_try_and_get_absolute(
    const s_data_array *array, long absolute_index)
{
    player_datum *result = 0;
    if (absolute_index != -1 && absolute_index >= 0 && absolute_index < array->first_unallocated)
    {
        player_datum *player = (player_datum *)(array->data + absolute_index * array->size);
        if (player->identifier != 0)
            result = player;
    }
    return result;
}
static __forceinline long simulation_game_projectiles_player_index_from_absolute(const s_data_array *array, long absolute_index)
{
    player_datum *player = (player_datum *)(array->data + absolute_index * array->size);
    return (player->identifier << 16) | absolute_index;
}
// @address 0x435f00 ?create_object@c_simulation_projectile_entity_definition@@UAAJJPBXPAKJ0@Z
long c_simulation_projectile_entity_definition::create_object(long creation_data_size, const void *creation_data,
    unsigned long *flags, long state_data_size, const void *state_data)
{
    const s_simulation_projectile_creation_data *creation = (const s_simulation_projectile_creation_data *)creation_data;
    const s_simulation_object_creation_data *object_creation = (const s_simulation_object_creation_data *)creation_data;
    object_placement_data placement;
    long result = -1;
    if (object_creation->definition_index != -1 && tag_try_and_get_unsafe('proj', object_creation->definition_index) != 0)
    {
        c_simulation_object_entity_definition::object_setup_placement_data((const s_simulation_object_creation_data *)creation_data,
            (const s_simulation_object_state_data *)state_data, flags, &placement);
        if (creation->owner_player_index != -1)
        {
            player_datum *player = simulation_game_projectiles_player_try_and_get_absolute(player_data, creation->owner_player_index);
            if (player)
            {
                long player_index = simulation_game_projectiles_player_index_from_absolute(player_data, creation->owner_player_index);
                placement.owner_object_index = player->unit_index;
                placement.player_index = player_index;
                placement.team_index = player->configuration.host.team_index;
                placement.damage_owner.set(player->unit_index, player_index, player->configuration.host.team_index);
            }
        }
        result = object_create_object(
            (s_simulation_object_creation_data const *)creation_data, (s_simulation_object_state_data const *)state_data,
            (unsigned long *)(flags), &placement);
        if (result != -1)
        {
            if (creation->disable_deceleration)
                ((projectile_datum *)object_get_and_verify_type(result, 32))->projectile.flags.m_storage |= 0x100;
            if (creation->target_entity_index != -1)
            {
                long target_object_index = creation->target_gamestate_index;
                if (target_object_index != -1)
                {
                    s_projectile_targeting_data targeting_data;
                    targeting_data.object_index = target_object_index;
                    targeting_data.model_target_index = (short)creation->target_model_target_index;
                    memcpy(&((projectile_datum *)object_get_and_verify_type(result, 32))->projectile.target_object_index,
    &targeting_data,
    sizeof(targeting_data));
                }
            }
            if (!creation->tracer)
                ((projectile_datum *)object_get_and_verify_type(result, 32))->projectile.flags.m_storage &= ~2;
        }
    }
    return result;
}

// 0x436c70
void c_simulation_projectile_effect_event_definition::event_payload_encode(long payload_size, const void *payload, c_bitstream *packet, bool quantize)
{
  const s_simulation_projectile_effect_event_data *event_data = (const s_simulation_projectile_effect_event_data *)payload;
  packet->write_integer("projectile-effect-flag", event_data->flags, 5);
  simulation_write_definition_index("projectile-definition-index", packet, event_data->definition_index);
  simulation_write_quantized_position(packet, &event_data->position, 12, true, 0);
  packet->write_unit_vector("forward", &event_data->forward);
  if (event_data->flags & 4)
  {
    packet->write_integer("material-index", event_data->material_index + 1, 8);
    packet->write_unit_vector("material-normal", &event_data->material_normal);
    packet->write_bool("has-material-effect", event_data->has_material_effect);
    if (event_data->has_material_effect)
      packet->write_integer("material-effect-index", event_data->material_effect_index, 8);
  }
}

bool c_simulation_projectile_effect_event_definition::event_payload_decode(long payload_size, void *payload, c_bitstream *packet, bool quantize)
{
    s_simulation_projectile_effect_event_data *event_data = (s_simulation_projectile_effect_event_data *)payload;
    event_data->flags = packet->read_integer("projectile-effect-flag", 5);
    event_data->definition_index = simulation_read_definition_index("projectile-definition-index", packet);
    ::simulation_read_quantized_position(packet, &event_data->position, 12, 0);
    packet->read_unit_vector("forward", &event_data->forward);
    if ((event_data->flags & 4) != 0)
    {
        event_data->material_index = packet->read_integer("material-index", 8) - 1;
        packet->read_unit_vector("material-normal", &event_data->material_normal);
        event_data->has_material_effect = packet->read_bool("has-material-effect");
        if (event_data->has_material_effect)
            event_data->material_effect_index = packet->read_integer("material-effect-index", 8);
    }
    else
    {
        event_data->material_index = -1;
        simulation_game_projectiles_copy_vector3d(&event_data->material_normal, &k_real_math_identity_matrix_storage.up);
        event_data->material_effect_index = -1;
        event_data->has_material_effect = false;
    }
    return true;
}
static __forceinline bool projectile_can_attach(long projectile, long parent)
{
    bool result = true;
    while (parent != -1) {
        if (parent == projectile) {
            result = false;
            break;
        }
        parent = simulation_game_projectiles_object_datum(parent)->parent_object_index;
    }
    return result;
}

// 0x4369f8
bool c_simulation_projectile_attached_event_definition::apply_game_event(
    long reference_count, const long *gamestate_indices, long payload_size, const void *payload)
{
    const s_simulation_projectile_attached_event_data *data = (const s_simulation_projectile_attached_event_data *)payload;
    bool result = false;
    long projectile_index = simulation_gamestate_entity_try_and_get_object_index(gamestate_indices[0], 32);
    long parent_index = simulation_gamestate_entity_try_and_get_object_index(gamestate_indices[1], -1);
    if (projectile_index != -1) {
        const s_location *location = (const s_location *)data->location;
        if (data->location[0] == 255)
            location = 0;
        if (!(data->attached_to_object && parent_index == -1)
            && !(data->attached_to_object && data->node_index == -1)
            && !(!data->attached_to_object && data->node_index != -1)) {
            real_point3d position = data->position;
            bool valid = true;
            if (data->attached_to_object && data->node_index != -1) {
                long node_count = 0;
                real_matrix4x3 *matrices = simulation_game_projectiles_object_node_matrices(parent_index, &node_count);
                if (data->node_index < 0 || data->node_index >= node_count)
                    valid = false;
                else
                    matrix4x3_transform_point(&matrices[data->node_index], &data->position, &position);
            }
            if (valid && data->attached_to_object) {
                if (!projectile_can_attach(projectile_index, parent_index))
                    valid = false;
            }
            if (valid) {
                projectile_attach(projectile_index, parent_index, (short)data->node_index, &position, location, true);
                result = true;
            }
        }
    }
    return result;
}

// 0x4370b0
long c_simulation_projectile_impact_effect_event_definition::minimum_required_bits(
    const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry)
{
    long bits = 72;
    bits += 13;
    return bits;
}

// 0x4370b8
float c_simulation_projectile_impact_effect_event_definition::calculate_relevance(
    const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry, float maximum_distance)
{
    const s_simulation_projectile_impact_effect_event_data *payload = (const s_simulation_projectile_impact_effect_event_data *)event->get_payload_data();
    return simulation_calculate_position_relevance(&payload->position, maximum_distance, telemetry);
}

void c_simulation_projectile_impact_effect_event_definition::write_description_to_string(
    const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry, float maximum_distance, long buffer_size, char *buffer)
{
  csnzprintf(buffer, buffer_size, "projectile impact effect: relevance=%5.3f", calculate_relevance(event, telemetry, maximum_distance));
}
// 0x437128
void c_simulation_projectile_impact_effect_event_definition::event_payload_encode(long payload_size, const void *payload, c_bitstream *packet, bool quantize)
{
    const s_simulation_projectile_impact_effect_event_data *event_data = (const s_simulation_projectile_impact_effect_event_data *)payload;
    simulation_write_definition_index("projectile-definition-index", packet, event_data->definition_index);
#if defined(_XBOX) && !defined(__clang__)
    const float *bounds = &real_constant_pool_zero_array()[-200];
    float maximum = bounds[0];
    float minimum = bounds[200];
#else
    float maximum = 1.0f;
    float minimum = 0.0f;
#endif
    packet->write_quantized_real("material-effect-scale", event_data->scale, minimum, maximum, 7, true, false);
    packet->write_quantized_real("material-effect-error", event_data->error, minimum, maximum, 7, true, false);
    packet->write_unit_vector("impact-direction-normal", &event_data->direction);
    simulation_write_quantized_position(packet, &event_data->position, 12, true, 0);
    packet->write_unit_vector("collision-plane-normal", &event_data->normal);
    packet->write_integer("material-index", event_data->material_index + 1, 8);
    packet->write_bool("flag", event_data->extra);
}

// 0x437240
bool c_simulation_projectile_impact_effect_event_definition::event_payload_decode(long payload_size, void *payload, c_bitstream *packet, bool quantize)
{
    s_simulation_projectile_impact_effect_event_data *event_data = (s_simulation_projectile_impact_effect_event_data *)payload;
    event_data->definition_index = simulation_read_definition_index("projectile-definition-index", packet);
#if defined(_XBOX) && !defined(__clang__)
    float minimum = real_constant_pool_zero_array()[0];
    float maximum = real_constant_pool_zero_array()[-200];
#else
    float minimum = 0.0f;
    float maximum = 1.0f;
#endif
    event_data->scale = packet->read_quantized_real("material-effect-scale", minimum, maximum, 7, true);
    event_data->error = packet->read_quantized_real("material-effect-error", minimum, maximum, 7, true);
    packet->read_unit_vector("impact-direction-normal", &event_data->direction);
    ::simulation_read_quantized_position(packet, &event_data->position, 12, 0);
    packet->read_unit_vector("collision-plane-normal", &event_data->normal);
    event_data->material_index = packet->read_integer("material-index", 8) - 1;
    event_data->extra = packet->read_bool("flag");
    return true;
}

// 0x437340
bool c_simulation_projectile_impact_effect_event_definition::apply_game_event(
    long reference_gamestate_count, const long *gamestate_indices, long payload_size, const void *payload)
{
  const s_simulation_projectile_impact_effect_event_data *event_data = (const s_simulation_projectile_impact_effect_event_data *)payload;
  if (event_data->definition_index == -1 || tag_try_and_get_unsafe(0x70726f6a, event_data->definition_index) == 0)
    return true;
  if (event_data->scale < 0.0f || event_data->scale > 1.0f)
    return true;
  if (event_data->error < 0.0f || event_data->error > 1.0f)
    return true;
  c_global_material_type material = simulation_game_projectiles_material_from_index(event_data->material_index);
  if (event_data->material_index != -1 && material.m_index == -1)
    return true;
  projectile_create_scaled_impact_effect(event_data->definition_index, event_data->scale, event_data->error, &event_data->direction,
      &event_data->position, &event_data->normal, material, false, -1, -1, false, event_data->extra);
  return true;
}

// 0x437450
long c_simulation_projectile_object_impact_effect_event_definition::payload_size()
{
  return sizeof(s_simulation_projectile_object_impact_effect_event_data);
}

// 0x437458
long c_simulation_projectile_object_impact_effect_event_definition::minimum_required_bits(
    const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry)
{
  long bits = 82;
  bits += 12;
  return bits;
}

void c_simulation_projectile_supercombine_request_event_definition::write_description_to_string(
    const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry, float maximum_distance, long buffer_size, char *buffer)
{
  csnzprintf(buffer, buffer_size, "projectile object impact effect: relevance=%5.3f", calculate_relevance(event, telemetry, maximum_distance));
}

// 0x4374c0
void c_simulation_projectile_object_impact_effect_event_definition::event_payload_encode(
    long payload_size, const void *payload, c_bitstream *packet, bool quantize)
{
    const s_simulation_projectile_object_impact_effect_event_data *event_data = (const s_simulation_projectile_object_impact_effect_event_data *)payload;
    simulation_write_definition_index("projectile-definition-index", packet, event_data->definition_index);
#if defined(_XBOX) && !defined(__clang__)
    const float *bounds = &real_constant_pool_zero_array()[-200];
    float maximum = bounds[0];
    float minimum = bounds[200];
#else
    float maximum = 1.0f;
    float minimum = 0.0f;
#endif
    packet->write_quantized_real("material-effect-scale", event_data->scale, minimum, maximum, 7, true, false);
    packet->write_quantized_real("material-effect-error", event_data->error, minimum, maximum, 7, true, false);
    packet->write_unit_vector("impact-direction-normal", &event_data->direction);
    packet->write_integer("position-encoding-type", event_data->position_encoding_type, 1);
    simulation_write_quantized_node_space_position(
        packet, &event_data->position, (e_simulation_node_space_encoding_type)event_data->position_encoding_type, 12);
    packet->write_unit_vector("collision-plane-normal", &event_data->normal);
    packet->write_integer("material-index", event_data->material_index + 1, 8);
    packet->write_integer("collision_node_index", event_data->node_index, 8);
    packet->write_bool("detonation-timer-started", event_data->effect);
    packet->write_bool("flag", event_data->extra);
}

// 0x4375f0
bool c_simulation_projectile_object_impact_effect_event_definition::event_payload_decode(long payload_size, void *payload, c_bitstream *packet, bool quantize)
{
    s_simulation_projectile_object_impact_effect_event_data *event_data = (s_simulation_projectile_object_impact_effect_event_data *)payload;
    event_data->definition_index = simulation_read_definition_index("projectile-definition-index", packet);
#if defined(_XBOX) && !defined(__clang__)
    float minimum = real_constant_pool_zero_array()[0];
    float maximum = real_constant_pool_zero_array()[-200];
#else
    float minimum = 0.0f;
    float maximum = 1.0f;
#endif
    event_data->scale = packet->read_quantized_real("material-effect-scale", minimum, maximum, 7, true);
    event_data->error = packet->read_quantized_real("material-effect-error", minimum, maximum, 7, true);
    packet->read_unit_vector("impact-direction-normal", &event_data->direction);
    event_data->position_encoding_type = packet->read_integer("position-encoding-type", 1);
    simulation_read_quantized_node_space_position(packet, &event_data->position, event_data->position_encoding_type, 12);
    packet->read_unit_vector("collision-plane-normal", &event_data->normal);
    event_data->material_index = packet->read_integer("material-index", 8) - 1;
    event_data->node_index = packet->read_integer("collision_node_index", 8);
    event_data->effect = packet->read_bool("detonation-timer-started");
    event_data->extra = packet->read_bool("flag");
    return true;
}

// 0x437720
bool c_simulation_projectile_object_impact_effect_event_definition::apply_game_event(
    long reference_gamestate_count, const long *gamestate_indices, long payload_size, const void *payload)
{
  const s_simulation_projectile_object_impact_effect_event_data *event_data = (const s_simulation_projectile_object_impact_effect_event_data *)payload;
  if (gamestate_indices[0] == -1)
    return true;
  long object_index = simulation_gamestate_entity_get_object_index(gamestate_indices[0]);
  if (object_index == -1)
    return true;
  if (event_data->definition_index == -1 || tag_try_and_get_unsafe(0x70726f6a, event_data->definition_index) == 0)
    return true;
  if (event_data->scale < 0.0f || event_data->scale > 1.0f)
    return true;
  if (event_data->error < 0.0f || event_data->error > 1.0f)
    return true;
  c_global_material_type material = simulation_game_projectiles_material_from_index(event_data->material_index);
  if (event_data->material_index != -1 && material.m_index == -1)
    return true;
  long node_count = 0;
  real_matrix4x3 *matrices = simulation_game_projectiles_object_node_matrices(object_index, &node_count);
  if (event_data->node_index < 0 || event_data->node_index >= node_count)
    return true;
  real_point3d position;
  matrix4x3_transform_point(&matrices[event_data->node_index], &event_data->position, &position);
  projectile_create_scaled_impact_effect(event_data->definition_index, event_data->scale, event_data->error, &event_data->direction,
      &position, &event_data->normal, material, true, object_index, event_data->node_index, event_data->effect, event_data->extra);
  return true;
}

// 0x4378a0
void c_simulation_projectile_supercombine_request_event_definition::event_payload_encode(
    long payload_size, const void *payload, c_bitstream *packet, bool quantize)
{
  simulation_write_definition_index("projectile-definition-index", packet, *(const long *)payload);
}

// 0x4378b8
bool c_simulation_projectile_supercombine_request_event_definition::event_payload_decode(long payload_size, void *payload, c_bitstream *packet, bool quantize)
{
  *(long *)payload = simulation_read_definition_index("projectile-definition-index", packet);
  return true;
}

struct s_network_configuration;

static __forceinline bool simulation_game_projectiles_definition_test(unsigned long flags, long bit)
{
    bool result = (flags & (1 << bit)) != 0;
    return result;
}
static __forceinline void simulation_game_projectiles_flags_set(unsigned long *flags, long bit, bool value)
{
    if (value)
        *flags |= 1 << bit;
    else
        *flags &= ~(1 << bit);
}

bool c_simulation_projectile_supercombine_request_event_definition::apply_game_event(
    long reference_gamestate_count, const long *gamestate_indices, long payload_size, const void *payload)
{
    const s_simulation_projectile_supercombine_request_event_data *event_data = (const s_simulation_projectile_supercombine_request_event_data *)payload;
    long attached_object_index = -1;
    if (gamestate_indices[0] != -1)
        attached_object_index = simulation_gamestate_entity_get_object_index(gamestate_indices[0]);
    if (attached_object_index != -1 && event_data->definition_index != -1 && tag_try_and_get_unsafe(0x70726f6a, event_data->definition_index))
    {
        projectile_definition *definition = TAG_GET(0x70726f6a, projectile_definition, event_data->definition_index);
        if (simulation_game_projectiles_definition_test(definition->targeting_flags, 3))
        {
            long projectile_index = -1;
            long attached_count = projectile_count_attached_to_object_for_supercombine(
                attached_object_index, event_data->definition_index, &projectile_index, 0);
            if (attached_count > 0 && projectile_index != -1)
            {
                float ratio = (float)attached_count / (float)definition->supercombine_count;
                if (ratio > g_network_configuration.projectile_supercombine_minimum_fraction)
                {
                    projectile_datum *projectile = ((projectile_datum *)object_get_and_verify_type(projectile_index, 32));
                    simulation_game_projectiles_flags_set(&projectile->projectile.flags.m_storage, 16, true);
                    simulation_game_projectiles_flags_set(&projectile->projectile.flags.m_storage, 17, true);
                    simulation_game_projectiles_projectile_set_action(projectile_index, 1);
                }
            }
        }
    }
    return true;
}

typedef char simulation_projectile_state_size[(sizeof(s_projectile_state_data) == 164) ? 1 : -1];

// @address 0x437b18 ?entity_type@c_simulation_projectile_entity_definition@@UAA?AW4e_simulation_entity_type@@XZ
e_simulation_entity_type c_simulation_projectile_entity_definition::entity_type()
{
    return _simulation_entity_type_projectile;
}

// @address 0x3fcc90 ?creation_data_size@c_simulation_projectile_entity_definition@@UAAJXZ
long c_simulation_projectile_entity_definition::creation_data_size()
{
    return 36;
}

// @address 0x42e678 ?update_flag_count@c_simulation_projectile_entity_definition@@UAAJXZ
long c_simulation_projectile_entity_definition::update_flag_count()
{
    return 15;
}

// @address 0x434458 ?initial_update_mask@c_simulation_projectile_entity_definition@@UAAKXZ
unsigned long c_simulation_projectile_entity_definition::initial_update_mask()
{
    return 0x5b3f;
}

// @address 0x42d380 ?entity_type_is_gameworld_object@c_simulation_projectile_entity_definition@@UAA_NXZ
bool c_simulation_projectile_entity_definition::entity_type_is_gameworld_object()
{
    return true;
}

// @address 0x436808 ?entity_state_lossy_compare@c_simulation_projectile_entity_definition@@UAA_NPAX0J@Z
bool c_simulation_projectile_entity_definition::entity_state_lossy_compare(void *first, void *second, long size)
{
    return object_state_compare((s_simulation_object_state_data *)first, (s_simulation_object_state_data *)second);
}

// @address 0x42d380 ?entity_creation_lossy_compare@c_simulation_projectile_entity_definition@@UAA_NPAX0J@Z
bool c_simulation_projectile_entity_definition::entity_creation_lossy_compare(void *first, void *second, long size)
{
    return true;
}

void c_simulation_projectile_entity_definition::prepare_state_data_for_gameworld(unsigned long update_mask, long size, void *state)
{
    object_entity_prepare_state_data(update_mask, size, state);
}

// @address 0x42d298 ?object_required_to_join_game@c_simulation_projectile_entity_definition@@UAA_NJ@Z
bool c_simulation_projectile_entity_definition::object_required_to_join_game(long object_index)
{
    return false;
}

// @address 0x42d380 ?promote_object_to_authority@c_simulation_projectile_entity_definition@@UAA_NJ@Z
bool c_simulation_projectile_entity_definition::promote_object_to_authority(long object_index)
{
    return true;
}

// @address 0x42d380 ?ensure_object_position_update_quantization_inside_bsp@c_simulation_projectile_entity_definition@@UAA_NXZ
bool c_simulation_projectile_entity_definition::ensure_object_position_update_quantization_inside_bsp()
{
    return true;
}

// @address 0x45fc28 ?event_type@c_simulation_projectile_attached_event_definition@@UAA?AW4e_simulation_event_type@@XZ
e_simulation_event_type c_simulation_projectile_attached_event_definition::event_type()
{
    return _simulation_event_type_projectile_attached;
}

typedef char simulation_projectile_attached_size[(sizeof(s_simulation_projectile_attached_event_data) == 28) ? 1 : -1];
// @address 0x429c18 ?payload_size@c_simulation_projectile_attached_event_definition@@UAAJXZ
long c_simulation_projectile_attached_event_definition::payload_size()
{
    return sizeof(s_simulation_projectile_attached_event_data);
}

// @address 0x3fc598 ?number_of_entity_references@c_simulation_projectile_attached_event_definition@@UAAJXZ
long c_simulation_projectile_attached_event_definition::number_of_entity_references()
{
    return 2;
}

// @address 0x42d298 ?reference_delays_entity_deletion@c_simulation_projectile_attached_event_definition@@UAA_NXZ
bool c_simulation_projectile_attached_event_definition::reference_delays_entity_deletion()
{
    return false;
}

// @address 0x42d380 ?event_can_be_transmitted@c_simulation_projectile_attached_event_definition@@UAA_NPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
bool c_simulation_projectile_attached_event_definition::event_can_be_transmitted(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    return true;
}

// @address 0x42bf88 ?minimum_required_bits@c_simulation_projectile_attached_event_definition@@UAAJPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
long c_simulation_projectile_attached_event_definition::minimum_required_bits(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    return 80;
}

// @address 0x4311b8 ?calculate_relevance@c_simulation_projectile_attached_event_definition@@UAAMPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@M@Z
float c_simulation_projectile_attached_event_definition::calculate_relevance(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance)
{
    return simulation_calculate_event_relevance(this, event, relevance, telemetry, 0);
}

// @address 0x435bc8 ?event_requirements@c_simulation_projectile_attached_event_definition@@UAA?AUs_simulation_event_requirements@@PBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
s_simulation_event_requirements c_simulation_projectile_attached_event_definition::event_requirements(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    s_simulation_event_requirements requirements;
    requirements.reference_count = 2;
    requirements.reference_delays_entity_deletion = false;
    requirements.can_be_transmitted = true;
    requirements.minimum_required_bits = 80;
    return requirements;
}

// @address 0x5bec98 ?prepare_event_data_for_gameworld@c_simulation_projectile_attached_event_definition@@UAAXJPAX@Z
void c_simulation_projectile_attached_event_definition::prepare_event_data_for_gameworld(long payload_size, void *payload)
{
}

// @address 0x3fc550 ?event_type@c_simulation_projectile_effect_event_definition@@UAA?AW4e_simulation_event_type@@XZ
e_simulation_event_type c_simulation_projectile_effect_event_definition::event_type()
{
    return _simulation_event_type_projectile_effect;
}

typedef char simulation_projectile_effect_size[(sizeof(s_simulation_projectile_effect_event_data) == 56) ? 1 : -1];
// @address 0x42e8c0 ?payload_size@c_simulation_projectile_effect_event_definition@@UAAJXZ
long c_simulation_projectile_effect_event_definition::payload_size()
{
    return sizeof(s_simulation_projectile_effect_event_data);
}

// @address 0x42d298 ?number_of_entity_references@c_simulation_projectile_effect_event_definition@@UAAJXZ
long c_simulation_projectile_effect_event_definition::number_of_entity_references()
{
    return 0;
}

// @address 0x42d298 ?reference_delays_entity_deletion@c_simulation_projectile_effect_event_definition@@UAA_NXZ
bool c_simulation_projectile_effect_event_definition::reference_delays_entity_deletion()
{
    return false;
}

// @address 0x42d380 ?event_can_be_transmitted@c_simulation_projectile_effect_event_definition@@UAA_NPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
bool c_simulation_projectile_effect_event_definition::event_can_be_transmitted(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    return true;
}

// @address 0x435be8 ?event_requirements@c_simulation_projectile_effect_event_definition@@UAA?AUs_simulation_event_requirements@@PBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
s_simulation_event_requirements c_simulation_projectile_effect_event_definition::event_requirements(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    s_simulation_event_requirements requirements;
    requirements.reference_count = 0;
    requirements.reference_delays_entity_deletion = false;
    requirements.can_be_transmitted = true;
    requirements.minimum_required_bits = c_simulation_projectile_effect_event_definition::minimum_required_bits(event, telemetry);
    return requirements;
}

// @address 0x5bec98 ?prepare_event_data_for_gameworld@c_simulation_projectile_effect_event_definition@@UAAXJPAX@Z
void c_simulation_projectile_effect_event_definition::prepare_event_data_for_gameworld(long payload_size, void *payload)
{
}

// @address 0x428ad0 ?event_type@c_simulation_projectile_impact_effect_event_definition@@UAA?AW4e_simulation_event_type@@XZ
e_simulation_event_type c_simulation_projectile_impact_effect_event_definition::event_type()
{
    return _simulation_event_type_projectile_impact_effect;
}

// @address 0x42d298 ?number_of_entity_references@c_simulation_projectile_impact_effect_event_definition@@UAAJXZ
long c_simulation_projectile_impact_effect_event_definition::number_of_entity_references()
{
    return 0;
}

// @address 0x42d298 ?reference_delays_entity_deletion@c_simulation_projectile_impact_effect_event_definition@@UAA_NXZ
bool c_simulation_projectile_impact_effect_event_definition::reference_delays_entity_deletion()
{
    return false;
}

// @address 0x42d380 ?event_can_be_transmitted@c_simulation_projectile_impact_effect_event_definition@@UAA_NPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
bool c_simulation_projectile_impact_effect_event_definition::event_can_be_transmitted(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    return true;
}

// @address 0x435c40 ?event_requirements@c_simulation_projectile_impact_effect_event_definition@@UAA?AUs_simulation_event_requirements@@PBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
s_simulation_event_requirements c_simulation_projectile_impact_effect_event_definition::event_requirements(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    s_simulation_event_requirements requirements;
    requirements.reference_count = 0;
    requirements.reference_delays_entity_deletion = false;
    requirements.can_be_transmitted = true;
    requirements.minimum_required_bits = 85;
    return requirements;
}

// @address 0x5bec98 ?prepare_event_data_for_gameworld@c_simulation_projectile_impact_effect_event_definition@@UAAXJPAX@Z
void c_simulation_projectile_impact_effect_event_definition::prepare_event_data_for_gameworld(long payload_size, void *payload)
{
}

// @address 0x4311b0 ?event_type@c_simulation_projectile_object_impact_effect_event_definition@@UAA?AW4e_simulation_event_type@@XZ
e_simulation_event_type c_simulation_projectile_object_impact_effect_event_definition::event_type()
{
    return _simulation_event_type_projectile_object_impact_effect;
}

// @address 0x42d380 ?number_of_entity_references@c_simulation_projectile_object_impact_effect_event_definition@@UAAJXZ
long c_simulation_projectile_object_impact_effect_event_definition::number_of_entity_references()
{
    return 1;
}

// @address 0x42d298 ?reference_delays_entity_deletion@c_simulation_projectile_object_impact_effect_event_definition@@UAA_NXZ
bool c_simulation_projectile_object_impact_effect_event_definition::reference_delays_entity_deletion()
{
    return false;
}

// @address 0x42d380 ?event_can_be_transmitted@c_simulation_projectile_object_impact_effect_event_definition@@UAA_NPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
bool c_simulation_projectile_object_impact_effect_event_definition::event_can_be_transmitted(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    return true;
}

// @address 0x4311b8 ?calculate_relevance@c_simulation_projectile_object_impact_effect_event_definition@@UAAMPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@M@Z
float c_simulation_projectile_object_impact_effect_event_definition::calculate_relevance(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance)
{
    return simulation_calculate_event_relevance(this, event, relevance, telemetry, 0);
}

// @address 0x437460 ?write_description_to_string@c_simulation_projectile_object_impact_effect_event_definition@@UAAXPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@MJPAD@Z
void c_simulation_projectile_object_impact_effect_event_definition::write_description_to_string(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance, long buffer_size, char *buffer)
{
    csnzprintf(buffer, buffer_size, "projectile object impact effect: relevance=%5.3f", calculate_relevance(event, telemetry, relevance));
}

// @address 0x435c60 ?event_requirements@c_simulation_projectile_object_impact_effect_event_definition@@UAA?AUs_simulation_event_requirements@@PBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
s_simulation_event_requirements c_simulation_projectile_object_impact_effect_event_definition::event_requirements(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    s_simulation_event_requirements requirements;
    requirements.reference_count = 1;
    requirements.reference_delays_entity_deletion = false;
    requirements.can_be_transmitted = true;
    requirements.minimum_required_bits = 94;
    return requirements;
}

// @address 0x5bec98 ?prepare_event_data_for_gameworld@c_simulation_projectile_object_impact_effect_event_definition@@UAAXJPAX@Z
void c_simulation_projectile_object_impact_effect_event_definition::prepare_event_data_for_gameworld(long payload_size, void *payload)
{
}
