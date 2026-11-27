// networkinglibxbox/simulation_game_vehicles.obj: from the records 0x435130 0x435140 0x435148 0x435158 0x4351f0 0x435268 0x435498 0x4356c8 0x435738 0x435790 0x435810 0x435998 0x435b08 0x709f68 0x70c850
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "cache/cache_files.h"
#include "memory/datum_index.h"
#include "tag_files/tag_types.h"
#include "simulation/simulation_entity_types.h"
// @address 0x221480 ?write_value_internal@c_bitstream@@QAAXKJ@Z
// @address 0x2afd40 ?compare_unit_vectors@c_bitstream@@SA_NPBTreal_vector3d@@0@Z
// @address 0x221398 ?write_bit_internal@c_bitstream@@AAAX_N@Z
// @address 0x221228 ?write_bits_internal@c_bitstream@@AAAXPBEJ@Z
// @address 0x221648 ?read_bit_internal@c_bitstream@@AAA_NXZ
// @address 0x221768 ?read_value_internal@c_bitstream@@AAAKJ@Z
#include "math/real_math_constants.h"
#include "networking/simulation/simulation_entity_definition.h"
#include "memory/tls_data_arrays.h"
#include "memory/data_types.h"
#include "networking/simulation/simulation_entity_database.h"
#include "simulation/simulation_game_entity_definitions.h"
#include "math/real_math.h"
#include "objects/object_header.h"
#include "units/vehicle_datum.h"
#include "units/vehicle_type_turret.h"
#include <string.h>
#include <math.h>
#include "units/vehicle_type_component.h"
#include "memory/bitstream.h"
#include "simulation/simulation_priority.h"
#include "simulation/game_interface/simulation_game_object_data_types.h"
#include "math/real_math_types.h"

#include "objects/object_hierarchy.h"
#include "simulation/game_interface/simulation_game_vehicles_internals.h"

// @address 0x3aa90 k_real_math_zero_vector3d_storage
// @address 0x40b128 ?object_build_baseline_state_data@c_simulation_object_entity_definition@@KA_NPBUs_simulation_object_creation_data@@PAUs_simulation_object_state_data@@@Z
// @address 0x40b6c0 ?object_handle_update@c_simulation_object_entity_definition@@KAKJKPAUs_simulation_object_state_data@@@Z
// @address 0x40c818 ?object_creation_encode@c_simulation_object_entity_definition@@KAXPBUs_simulation_object_creation_data@@PAVc_bitstream@@_N@Z
// @address 0x40cb38 ?object_update_encode@c_simulation_object_entity_definition@@KA_N_NKPAKPBUs_simulation_view_telemetry_data@@PBUs_simulation_object_state_data@@PAVc_bitstream@@J00@Z
// @address 0x40d178 ?object_update_decode@c_simulation_object_entity_definition@@KA_N_NPAKPAUs_simulation_object_state_data@@PAVc_bitstream@@0@Z
// @address 0x40d840 ?object_state_compare@c_simulation_object_entity_definition@@KA_NPAUs_simulation_object_state_data@@0@Z
// @address 0x40c9d0 ?object_calculate_update_requirements@c_simulation_object_entity_definition@@KAXPBUs_simulation_entity@@PBUs_entity_update_data@@PAJ@Z
// @address 0x40bff0 ?object_apply_update@c_simulation_object_entity_definition@@IAAXJKPBUs_simulation_object_state_data@@@Z

extern "C" {
// @address 0x491f8 ??_7c_simulation_vehicle_entity_definition@@6B@
// @address 0x709f68 ??__Eg_simulation_vehicle_entity_definition@@YAXXZ
// @address 0x70c850 ??__Fg_simulation_vehicle_entity_definition@@YAXXZ
// @address 0x7bb4c8 g_simulation_vehicle_entity_definition
c_simulation_vehicle_entity_definition g_simulation_vehicle_entity_definition;
}

// @address 0x435130 ?entity_type_name@c_simulation_vehicle_entity_definition@@UAAPBDXZ
const char *c_simulation_vehicle_entity_definition::entity_type_name() { return "vehicle"; }
// @address 0x435140 ?state_data_size@c_simulation_vehicle_entity_definition@@UAAJXZ
long c_simulation_vehicle_entity_definition::state_data_size() { return 180; }
// @address 0x435148 ?initial_update_mask@c_simulation_vehicle_entity_definition@@UAAKXZ
unsigned long c_simulation_vehicle_entity_definition::initial_update_mask() { return 0x1db3f; }
// @address 0x435158 ?object_required_to_join_game@c_simulation_vehicle_entity_definition@@UAA_NJ@Z
bool c_simulation_vehicle_entity_definition::object_required_to_join_game(long object_index) {
 long root_object_index = object_get_root_object(object_index);
 vehicle_datum *object = (vehicle_datum *)object_get_and_verify_type(root_object_index, ~0UL);
 bool result = false;
 if (object->object.identifier.get_source() != 2) result = true;
 else if (!(bool)((object->object.damage_flags >> 2) & 1)) result = true;
 if (object->object.map_variant_index != -1) result = true;
 return result;
}

// @address 0x4351f0 ?build_baseline_state_data@c_simulation_vehicle_entity_definition@@UAA_NJPBXJPAX@Z
bool c_simulation_vehicle_entity_definition::build_baseline_state_data(
    long creation_data_size, const void *creation_data, long state_data_size, void *state_data)
{
 bool result = false;
 s_simulation_vehicle_state_data *state = (s_simulation_vehicle_state_data *)state_data;
 memset(state, 0, sizeof(s_simulation_vehicle_state_data));
 if (c_simulation_object_entity_definition::object_build_baseline_state_data(
         (const s_simulation_object_creation_data *)creation_data, (s_simulation_object_state_data *)state))
 {
  state->auto_aiming_vector = state->object_state.forward;
  result = true;
 }
 return result;
}

// @address 0x435738 ?entity_creation_encode@c_simulation_vehicle_entity_definition@@UAAXJPBXPBUs_simulation_view_telemetry_data@@PAVc_bitstream@@_N@Z
// @address 0x435790 ?update_minimum_required_bits@c_simulation_vehicle_entity_definition@@UAAJPBUs_simulation_entity@@PBUs_entity_update_data@@@Z
long c_simulation_vehicle_entity_definition::update_minimum_required_bits(
 const s_simulation_entity *entity, const s_entity_update_data *update)
{
 unsigned long bits = update->update_mask;
 long value;
 object_calculate_update_requirements(entity, update, &value);
 value += 3;
 if ((bits & 0x4000) != 0 && value > 21) {
  value = 21;
 }
 if ((bits & 0x8000) != 0 && value > 36) {
  value = 36;
 }
 if ((bits & 0x10000) != 0 && value > 25) {
  value = 25;
 }
 return value;
}


// 0x435738
void c_simulation_object_entity_definition::object_entity_variant_creation_encode(long creation_data_size, const void *creation_data,
    const s_simulation_view_telemetry_data *telemetry, c_bitstream *packet, bool quantize)
{
 const s_simulation_vehicle_creation_data *vehicle_creation = (const s_simulation_vehicle_creation_data *)creation_data;
 c_simulation_object_entity_definition::object_creation_encode(vehicle_creation, packet, quantize);
 packet->write_raw_data("variant-name", vehicle_creation->variant_name, 32);
}
 // @address 0x1858b8 ?csnzprintf@@YAPADPADIPBDZZ
extern "C" {
 /* "vehicle creation: relevance=%5.3f" */
}

// @address 0x4356c8 ?write_creation_description_to_string@c_simulation_vehicle_entity_definition@@UAAXPBUs_simulation_entity@@PBUs_simulation_view_telemetry_data@@JPAD@Z
void c_simulation_vehicle_entity_definition::write_creation_description_to_string(
    const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry, long buffer_size, char *buffer)
{
    float relevance;

    relevance = 0.0f;
    simulation_calculate_entity_creation_priority(entity, telemetry, &relevance);
    csnzprintf(buffer, buffer_size, "vehicle creation: relevance=%5.3f", relevance);
}

// @address 0x435998 ?entity_update_decode@c_simulation_vehicle_entity_definition@@UAA_N_NPAKJPAXPAVc_bitstream@@0@Z
bool c_simulation_vehicle_entity_definition::entity_update_decode(
    bool initial_update, unsigned long *update_mask, long state_data_size, void *state_data, c_bitstream *packet, bool is_replicated)
{
    s_simulation_vehicle_state_data *state = (s_simulation_vehicle_state_data *)state_data;
    bool success = true;
    unsigned char object_success = c_simulation_object_entity_definition::object_update_decode(
        initial_update, update_mask, (s_simulation_object_state_data *)state, packet, is_replicated);
    success = success && object_success;
    if (packet->read_bool("auto-trigger-exists"))
    {
        state->auto_turret_enabled = packet->read_bool("auto-turret-enabled");
        state->auto_primary_trigger = packet->read_bool("auto-primary-trigger");
        state->auto_secondary_trigger = packet->read_bool("auto-secondary-trigger");
        *update_mask |= 0x4000;
    }
    if (packet->read_bool("auto-aiming-exists"))
    {
        unsigned long packed = packet->read_integer("auto-aiming-vector", 19);
        bitstream_decode_unit_vector(packed, &state->auto_aiming_vector);
        *update_mask |= 0x8000;
        success = success && ::valid_real_normal3d(&state->auto_aiming_vector);
    }
    if (packet->read_bool("emp-timer-exists"))
    {
        state->emp_timer = packet->read_integer("emp-timer", 8);
        *update_mask |= 0x10000;
    }
    success = success && !packet->overflowed();
    return success;
}

// @address 0x435b08 ?entity_state_lossy_compare@c_simulation_vehicle_entity_definition@@UAA_NPAX0J@Z
bool c_simulation_vehicle_entity_definition::entity_state_lossy_compare(void *first, void *second, long state_data_size)
{
 s_simulation_vehicle_state_data *first_state = (s_simulation_vehicle_state_data *)first;
 s_simulation_vehicle_state_data *second_state = (s_simulation_vehicle_state_data *)second;
 bool result = c_simulation_object_entity_definition::object_state_compare((s_simulation_object_state_data *)first, (s_simulation_object_state_data *)second)
  && c_bitstream::compare_unit_vectors(&first_state->auto_aiming_vector, &second_state->auto_aiming_vector);
 second_state->auto_aiming_vector = k_real_math_zero_vector3d_storage;
 first_state->auto_aiming_vector = second_state->auto_aiming_vector;
 return result;
}

// @address 0x446e88 ?make_room_for_update@c_entity_update_encode_helper@@QAA_NPAVc_bitstream@@JJJK@Z
// @address 0x446ef8 ?write_component_header@c_entity_update_encode_helper@@QAA_NJPBD@Z
// @address 0x446f80 ?finish_component@c_entity_update_encode_helper@@QAAXXZ

// @address 0x435810 ?entity_update_encode@c_simulation_vehicle_entity_definition@@UAA_N_NKPAKJPBXPBUs_simulation_view_telemetry_data@@PAVc_bitstream@@J0@Z
bool c_simulation_vehicle_entity_definition::entity_update_encode(bool initial_update, unsigned long update_mask,
    unsigned long *update_mask_written, long state_data_size, const void *state_data, const s_simulation_view_telemetry_data *telemetry,
    c_bitstream *packet, long minimum_bits, bool lossy)
{
 const s_simulation_vehicle_state_data *state = (const s_simulation_vehicle_state_data *)state_data;
 bool result = false;
 unsigned long object_update_mask = update_mask & 0x3fff;
 long object_minimum_bits = minimum_bits + 3;
 if (c_simulation_object_entity_definition::object_update_encode(initial_update, object_update_mask, update_mask_written, telemetry,
         (const s_simulation_object_state_data *)state, packet, object_minimum_bits, this->ensure_object_position_update_quantization_inside_bsp(), lossy))
 {
  c_entity_update_encode_helper helper;
  long first_component = 14;
  long component_count = 17 - first_component;
  unsigned long vehicle_update_mask = update_mask & 0x1c000;
  if (helper.make_room_for_update(packet, minimum_bits, first_component, component_count, vehicle_update_mask))
  {
   if (helper.write_component_header(14, "auto-trigger-exists"))
   {
    packet->write_bool("auto-turret-enabled", state->auto_turret_enabled);
    packet->write_bool("auto-primary-trigger", state->auto_primary_trigger);
    packet->write_bool("auto-secondary-trigger", state->auto_secondary_trigger);
   }
   helper.finish_component();
   if (helper.write_component_header(15, "auto-aiming-exists"))
   {
    packet->write_integer("auto-aiming-vector", bitstream_encode_unit_vector(&state->auto_aiming_vector), 19);
   }
   helper.finish_component();
   if (helper.write_component_header(16, "emp-timer-exists"))
   {
    packet->write_integer("emp-timer", state->emp_timer, 8);
   }
   helper.finish_component();
   helper.finish_update(update_mask_written);
   result = true;
  }
 }
 return result;
}

// @address 0x435268 ?handle_object_update@c_simulation_vehicle_entity_definition@@UAAKJKJPAX@Z
unsigned long c_simulation_vehicle_entity_definition::handle_object_update(
    long object_index, unsigned long update_mask, long state_data_size, void *state_data_raw)
{
 s_simulation_vehicle_state_data *state = (s_simulation_vehicle_state_data *)state_data_raw;
 vehicle_datum *object = (vehicle_datum *)object_get_and_verify_type(object_index, 2);
 unsigned long result = c_simulation_object_entity_definition::object_handle_update(
     object_index, update_mask & 0x3fff, (s_simulation_object_state_data *)state_data_raw);
 if ((update_mask & 0x4000) != 0)
 {
  unsigned short vehicle_flags = object->vehicle.flags;
  bool auto_turret_enabled = (vehicle_flags >> 7) & 1;
  bool auto_primary_trigger = (vehicle_flags >> 8) & 1;
  bool auto_secondary_trigger = (vehicle_flags >> 9) & 1;
  if (state->auto_turret_enabled != auto_turret_enabled || state->auto_primary_trigger != auto_primary_trigger ||
      state->auto_secondary_trigger != auto_secondary_trigger)
  {
   result |= 0x4000;
   state->auto_turret_enabled = auto_turret_enabled;
   state->auto_primary_trigger = auto_primary_trigger;
   state->auto_secondary_trigger = auto_secondary_trigger;
  }
 }
 if ((update_mask & 0x8000) != 0)
 {
  if (fabsf(state->auto_aiming_vector.i - object->vehicle.forward.i) >= k_real_math_epsilon_storage ||
      fabsf(state->auto_aiming_vector.j - object->vehicle.forward.j) >= k_real_math_epsilon_storage ||
      fabsf(state->auto_aiming_vector.k - object->vehicle.forward.k) >= k_real_math_epsilon_storage)
  {
   state->auto_aiming_vector = object->vehicle.forward;
   result |= 0x8000;
  }
 }
 if ((update_mask & 0x10000) != 0)
 {
  short emp_timer = object->unit.stun_ticks;
  if (state->emp_timer != emp_timer)
  {
   state->emp_timer = emp_timer;
   result |= 0x10000;
  }
 }
 return result;
}

// @address 0x435498 ?apply_object_update@c_simulation_vehicle_entity_definition@@UAAXJKJPBX@Z
void c_simulation_vehicle_entity_definition::apply_object_update(long object_index, unsigned long update_mask, long state_data_size, const void *state_data_raw)
{
 const s_simulation_vehicle_state_data *state = (const s_simulation_vehicle_state_data *)state_data_raw;
 vehicle_datum *object = (vehicle_datum *)object_get_and_verify_type(object_index, 2);
 unsigned long masked = update_mask & 0x3fff;
 if (masked != 0)
  object_apply_update(object_index, masked, (const s_simulation_object_state_data *)state_data_raw);
 float zero = 0.0f;
 if ((update_mask & 0x4000) != 0)
 {
  if (state->auto_turret_enabled != 0)
  {
   bool auto_turret_set = ((object->vehicle.flags >> 7) & 1) != 0;
   if (!auto_turret_set)
   {
    c_vehicle_type_turret *auto_control = (c_vehicle_type_turret *)vehicle_get_auto_control(&object->vehicle.type_state, object_index);
    if (auto_control != 0)
    {
     vehicle_datum *object2 = (vehicle_datum *)object_get_and_verify_type(object_index, 2);
     auto_control->m_auto_control.active = false;
     const s_damage_owner *owner = damage_owner_unknown();
     auto_control->m_auto_control.damage_owner = *owner;
     auto_control->m_auto_control.target_object_index = -1;
     auto_control->m_auto_control.target_time = -1;
     auto_control->m_auto_control.minimum_range_start_time = -1;
     auto_control->m_auto_control.target_lost_time = -1;
     auto_control->m_volume_index = -1;
     auto_control->m_min_range = zero;
     auto_control->m_max_range = zero;
     auto_control->m_lost_seconds = zero;
     object2->vehicle.forward = object2->object.forward;
    }
   }
  }
  if (state->auto_turret_enabled != 0)
   object->vehicle.flags |= 128;
  else
   object->vehicle.flags &= ~128;
  if (state->auto_primary_trigger != 0)
   object->vehicle.flags |= 0x100;
  else
   object->vehicle.flags &= ~0x100;
  if (state->auto_secondary_trigger != 0)
   object->vehicle.flags |= 0x200;
  else
   object->vehicle.flags &= ~0x200;
 }
 if ((update_mask & 0x8000) != 0)
 {
  object->vehicle.forward = state->auto_aiming_vector;
 }
 if ((update_mask & 0x10000) != 0)
 {
  long emp_timer = state->emp_timer;
  bool clear = (emp_timer > 0 && object->unit.stun_ticks == 0);
  object->unit.stun_ticks = (unsigned short)emp_timer;
  if (clear)
  {
   long count = 0;
   const s_vehicle_type_definition *definition = (const s_vehicle_type_definition *)
       global_tag_instances[datum_index_to_absolute_index(object->object.definition)].definition;
   if (definition->count > 0)
   {
    float *zero_ptr = object->unit.deplete;
    do {
     *zero_ptr = zero;
     count++;
     zero_ptr++;
    } while (count < definition->count);
   }
  }
 }
}

// @address 0x433b30 ?entity_type@c_simulation_vehicle_entity_definition@@UAA?AW4e_simulation_entity_type@@XZ
e_simulation_entity_type c_simulation_vehicle_entity_definition::entity_type()
{
    return _simulation_entity_type_vehicle;
}

// @address 0x42e7c0 ?creation_data_size@c_simulation_vehicle_entity_definition@@UAAJXZ
long c_simulation_vehicle_entity_definition::creation_data_size()
{
    return 20;
}

// @address 0x45fca8 ?update_flag_count@c_simulation_vehicle_entity_definition@@UAAJXZ
long c_simulation_vehicle_entity_definition::update_flag_count()
{
    return 17;
}

// @address 0x42d380 ?entity_type_is_gameworld_object@c_simulation_vehicle_entity_definition@@UAA_NXZ
bool c_simulation_vehicle_entity_definition::entity_type_is_gameworld_object()
{
    return true;
}

// @address 0x433fa0 ?entity_can_be_created@c_simulation_vehicle_entity_definition@@UAA_NPBUs_simulation_entity@@PBUs_simulation_view_telemetry_data@@@Z
bool c_simulation_vehicle_entity_definition::entity_can_be_created(const s_simulation_entity *entity,
    const s_simulation_view_telemetry_data *telemetry)
{
    return object_can_be_created(0, static_cast<const s_simulation_object_state_data *>(entity->state_data), telemetry);
}

// @address 0x42cc28 ?creation_minimum_required_bits@c_simulation_vehicle_entity_definition@@UAAJPBUs_simulation_entity@@PBUs_simulation_view_telemetry_data@@@Z
long c_simulation_vehicle_entity_definition::creation_minimum_required_bits(const s_simulation_entity *entity,
    const s_simulation_view_telemetry_data *telemetry)
{
    return 142;
}

void c_simulation_vehicle_entity_definition::entity_creation_encode(long creation_data_size, const void *creation_data,
    const s_simulation_view_telemetry_data *telemetry, c_bitstream *packet, bool quantize)
{
    object_entity_variant_creation_encode(creation_data_size, creation_data, telemetry, packet, quantize);
}

// @address 0x42d380 ?entity_creation_lossy_compare@c_simulation_vehicle_entity_definition@@UAA_NPAX0J@Z
bool c_simulation_vehicle_entity_definition::entity_creation_lossy_compare(void *first, void *second, long size)
{
    return true;
}

// @address 0x43add8 ?rotate_entity_indices@c_simulation_vehicle_entity_definition@@UAAXPBUs_simulation_entity@@@Z
void c_simulation_vehicle_entity_definition::rotate_entity_indices(const s_simulation_entity *entity)
{
    object_entity_rotate_indices(entity);
}

// @address 0x5bec98 ?prepare_creation_data_for_gameworld@c_simulation_vehicle_entity_definition@@UAAXJPAX@Z
void c_simulation_vehicle_entity_definition::prepare_creation_data_for_gameworld(long size, void *creation_data)
{
}

// @address 0x434748 ?prepare_state_data_for_gameworld@c_simulation_vehicle_entity_definition@@UAAXKJPAX@Z
void c_simulation_vehicle_entity_definition::prepare_state_data_for_gameworld(unsigned long update_mask, long size, void *state)
{
    object_entity_prepare_state_data(update_mask, size, state);
}

// @address 0x42d298 ?handle_delete_object@c_simulation_vehicle_entity_definition@@UAA_NJ@Z
bool c_simulation_vehicle_entity_definition::handle_delete_object(long object_index)
{
    return false;
}

// @address 0x42d380 ?promote_object_to_authority@c_simulation_vehicle_entity_definition@@UAA_NJ@Z
bool c_simulation_vehicle_entity_definition::promote_object_to_authority(long object_index)
{
    return true;
}

// @address 0x42d298 ?ensure_object_position_update_quantization_inside_bsp@c_simulation_vehicle_entity_definition@@UAA_NXZ
bool c_simulation_vehicle_entity_definition::ensure_object_position_update_quantization_inside_bsp()
{
    return false;
}

void c_simulation_vehicle_entity_definition::write_update_description_to_string(const s_simulation_entity *entity,
    const s_entity_update_data *update, long size, char *buffer)
{
}
