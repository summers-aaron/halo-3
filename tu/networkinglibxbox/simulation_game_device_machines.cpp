// networkinglibxbox/simulation_game_device_machines.obj: from the records 0x435c88 0x43a720 0x43a728 0x43a738 0x43a7c0 0x43a810 0x43a940 0x43aa50 0x43aae8 0x43acd0 0x43ad40 0x43ae18 0x43aea0 0x43b0f0 0x43b290 0x43b3e8 0x43b448 0x709f98 0x70c898
// @flags /TP /O2 /Oc
#include "simulation/simulation_queue_event_types.h"
#include "networking/simulation/simulation_entity_definition.h"
#include "simulation/simulation_game_entity_definitions.h"
#include "networking/simulation/simulation_entity_database.h"
#include "cseries/cseries_types.h"
#include "math/real_math_types.h"
#include "math/real_math.h"
#include "memory/data_types.h"
#include "objects/object_types.h"
#include "objects/object_datum.h"
#include "objects/object_header.h"
#include "objects/object_placement.h"
#include "simulation/game_interface/simulation_game_internal.h"
// @address 0x43a720 ?event_type@c_simulation_device_touch_event_definition@@UAA?AW4e_simulation_event_type@@XZ
// @address 0x43a728 ?event_type_name@c_simulation_device_touch_event_definition@@UAAPBDXZ
// @address 0x43a738 ?entity_type_name@c_simulation_device_entity_definition@@UAAPBDXZ
// @address 0x435c88 simulation_device_initialize_update_mask
// @address 0x43a7c0 ?build_baseline_state_data@c_simulation_device_entity_definition@@UAA_NJPBXJPAX@Z
// @address 0x221648 ?read_bit_internal@c_bitstream@@AAA_NXZ
// @address 0x221768 ?read_value_internal@c_bitstream@@AAAKJ@Z
// @address 0x40d840 ?object_state_compare@c_simulation_object_entity_definition@@KA_NPAUs_simulation_object_state_data@@0@Z
// @address 0x40cb38 ?object_update_encode@c_simulation_object_entity_definition@@KA_N_NKPAKPBUs_simulation_view_telemetry_data@@PBUs_simulation_object_state_data@@PAVc_bitstream@@J00@Z
// @address 0x40d178 ?object_update_decode@c_simulation_object_entity_definition@@KA_N_NPAKPAUs_simulation_object_state_data@@PAVc_bitstream@@0@Z
// @address 0x43ae18 ?entity_creation_encode@c_simulation_device_entity_definition@@UAAXJPBXPBUs_simulation_view_telemetry_data@@PAVc_bitstream@@_N@Z
// @address 0x43b3e8 ?write_description_to_string@c_simulation_device_touch_event_definition@@UAAXPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@MJPAD@Z
#include <string.h>
#include "cache/cache_files.h"
#include "devices/devices.h"
#include "simulation/simulation_gamestate_entities.h"
#include "memory/bitstream.h"
#include "simulation/simulation_priority.h"
#include "networking/replication/replication_event.h"
#include "simulation/game_interface/simulation_game_object_data_types.h"
#include "devices/device_group.h"
#include "devices/device_datum.h"
#include "simulation/game_interface/simulation_game_device_machines.h"


#include "simulation/game_interface/simulation_game_device_definitions.h"
#include "memory/data.h"

extern "C" {
// @address 0x49a18 ??_7c_simulation_device_entity_definition@@6B@
// @address 0x709f98 ??__Eg_simulation_device_entity_definition@@YAXXZ
// @address 0x70c898 ??__Fg_simulation_device_entity_definition@@YAXXZ
// @address 0x7bb520 g_simulation_device_entity_definition
c_simulation_device_entity_definition g_simulation_device_entity_definition;
// @address 0x49aac ??_7c_simulation_device_touch_event_definition@@6B@
// @address 0x7bb524 g_simulation_device_touch_event_definition
c_simulation_device_touch_event_definition g_simulation_device_touch_event_definition;
}

// @address 0x427490 ?state_data_size@c_simulation_device_entity_definition@@UAAJXZ
long c_simulation_device_entity_definition::state_data_size()
{
  return sizeof(s_simulation_device_state_data);
}

// @address 0x42e7e8 ?entity_type@c_simulation_device_entity_definition@@UAA?AW4e_simulation_entity_type@@XZ
e_simulation_entity_type c_simulation_device_entity_definition::entity_type()
{
  return _simulation_entity_type_device;
}

// Initializes the device update mask.
extern "C" void simulation_device_initialize_update_mask(int *flags)
{
    *flags = 0x280000c;
}

e_simulation_event_type c_simulation_device_touch_event_definition::event_type()
{
  return _simulation_event_type_device_touch;
}

const char *c_simulation_device_touch_event_definition::event_type_name()
{
  return "device_touch";
}

const char *c_simulation_device_entity_definition::entity_type_name()
{
  return "device";
}


void c_simulation_device_touch_event_definition::write_description_to_string(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance, long buffer_size, char *buffer)
{
  csnzprintf(buffer, buffer_size, "device-touch: relevance %5.3f", calculate_relevance(event, telemetry, relevance));
}


// 0x43a7c0: clears the 176-byte baseline state, then builds it from the creation data
bool c_simulation_device_entity_definition::build_baseline_state_data(
    long creation_data_size, const void *creation_data, long state_data_size, void *state_data)
{
    bool result = false;
    memset(state_data, 0, sizeof(s_simulation_device_state_data));
    if ((unsigned char)c_simulation_object_entity_definition::object_build_baseline_state_data(
            (const s_simulation_object_creation_data *)creation_data, (s_simulation_object_state_data *)state_data) != 0)
    {
        result = true;
    }
    return result;
}

// 0x43acd0: the "device creation" debug line with the object's relevance

// @address 0x43acd0 ?write_creation_description_to_string@c_simulation_device_entity_definition@@UAAXPBUs_simulation_entity@@PBUs_simulation_view_telemetry_data@@JPAD@Z
void c_simulation_device_entity_definition::write_creation_description_to_string(
    const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry, long buffer_size, char *buffer)
{
    float relevance = 0.0f;
    simulation_calculate_entity_creation_priority(entity, telemetry, &relevance);
    csnzprintf(buffer, buffer_size, "device creation: relevance=%5.3f", relevance);
}

// @address 0x43ad40 ?update_minimum_required_bits@c_simulation_device_entity_definition@@UAAJPBUs_simulation_entity@@PBUs_entity_update_data@@@Z
long c_simulation_device_entity_definition::update_minimum_required_bits(const s_simulation_entity *entity,
    const s_entity_update_data *update)
{
    long base;
    unsigned long bits = update->update_mask;
    object_calculate_update_requirements(entity, update, &base);
    long count = base + 4;

    if ((bits & 0x4000) != 0 && count > 32) {
        count = 32;
    }
    if ((bits & 0x8000) != 0 && count > 32) {
        count = 32;
    }
    if ((bits & 0x10000) != 0 && count > 32) {
        count = 32;
    }
    if ((bits & 0x20000) != 0 && count > 32) {
        count = 32;
    }
    return count;
}


void c_simulation_device_entity_definition::entity_creation_encode(long creation_data_size, const void *creation_data,
    const s_simulation_view_telemetry_data *telemetry, c_bitstream *packet, bool final_update)
{
    c_simulation_object_entity_definition::object_creation_encode((const s_simulation_object_creation_data *)creation_data, packet, final_update);
}


const real k_device_compare_minimum = 0.0f;
const real k_device_compare_maximum = 1.0f;

// @address 0x43b290 ?entity_state_lossy_compare@c_simulation_device_entity_definition@@UAA_NPAX0J@Z
bool c_simulation_device_entity_definition::entity_state_lossy_compare(void *first, void *second, long state_data_size)
{
    s_simulation_device_state_data *first_state = (s_simulation_device_state_data *)first;
    s_simulation_device_state_data *second_state = (s_simulation_device_state_data *)second;
    bool equal = c_simulation_object_entity_definition::object_state_compare((s_simulation_object_state_data *)first, (s_simulation_object_state_data *)second);
    equal = equal &&
        c_bitstream::compare_quantized_reals(
            first_state->position, second_state->position, k_device_compare_minimum, k_device_compare_maximum, 14, false, false);
    first_state->position = second_state->position = k_device_compare_minimum;
    equal = equal &&
        c_bitstream::compare_quantized_reals(first_state->position_group, second_state->position_group, k_device_compare_minimum,
            k_device_compare_maximum, 14, false, false);
    first_state->position_group = second_state->position_group = k_device_compare_minimum;
    equal = equal &&
        c_bitstream::compare_quantized_reals(
            first_state->power, second_state->power, k_device_compare_minimum, k_device_compare_maximum, 14, false, false);
    first_state->power = second_state->power = k_device_compare_minimum;
    equal = equal &&
        c_bitstream::compare_quantized_reals(
            first_state->power_group, second_state->power_group, k_device_compare_minimum, k_device_compare_maximum, 14, false, false);
    first_state->power_group = second_state->power_group = k_device_compare_minimum;
    return equal;
}

// 0x43b0f0
const float k_one = 1.0f;
const float k_zero = 0.0f;
// @address 0x43b0f0 ?entity_update_decode@c_simulation_device_entity_definition@@UAA_N_NPAKJPAXPAVc_bitstream@@0@Z
bool c_simulation_device_entity_definition::entity_update_decode(
    bool initial_update, unsigned long *update_mask, long state_data_size, void *state_data, c_bitstream *packet, bool unknown)
{
    s_simulation_device_state_data *state = (s_simulation_device_state_data *)state_data;
    bool success = true;
    bool object_success = c_simulation_object_entity_definition::object_update_decode(
        initial_update, update_mask, (s_simulation_object_state_data *)state, packet, unknown);
    if (packet->read_bool("position-exists"))
    {
        state->position = packet->read_quantized_real("position", k_zero, k_one, 14, false);
        *update_mask |= 1UL << 14;
    }
    if (packet->read_bool("position-group-position-exists"))
    {
        state->position_group = packet->read_quantized_real("position-group-position", k_zero, k_one, 14, false);
        *update_mask |= 1UL << 15;
    }
    if (packet->read_bool("power-exists")) { state->power = packet->read_quantized_real("power", k_zero, k_one, 14, false); *update_mask |= 1UL << 16; }
    if (packet->read_bool("power-group-power-exists"))
    {
        state->power_group = packet->read_quantized_real("power-group-power", k_zero, k_one, 14, false);
        *update_mask |= 1UL << 17;
    }
    success = success && object_success;
    success = success && !packet->overflowed();
    return success;
}

// @address 0x43a810 ?handle_object_update@c_simulation_device_entity_definition@@UAAKJKJPAX@Z
unsigned long c_simulation_device_entity_definition::handle_object_update(long object_index, unsigned long update_mask, long state_data_size, void *state_data)
{
    s_simulation_device_state_data *state = (s_simulation_device_state_data *)state_data;
    device_datum *device = (device_datum *)object_get_and_verify_type(object_index, NONE);
    unsigned long updated_mask = 0;
    unsigned long object_update_mask = update_mask & 0x3fff;
    updated_mask |= c_simulation_object_entity_definition::object_handle_update(object_index, object_update_mask, (s_simulation_object_state_data *)state);
    if ((update_mask & 0x4000) != 0 && state->position != device->device_position) {
        state->position = device->device_position;
        updated_mask |= 0x4000;
    }
    if ((update_mask & 0x8000) != 0 && device->position_group_index != -1) {
        s_device_group_datum *group = datum_get<s_device_group_datum>(tls_device_groups_data(), device->position_group_index);
        if (state->position_group != group->actual_value) {
            state->position_group = group->actual_value;
            updated_mask |= 0x8000;
        }
    }
    if ((update_mask & 0x10000) != 0 && state->power != device->power) {
        state->power = device->power;
        updated_mask |= 0x10000;
    }
    if ((update_mask & 0x20000) != 0 && device->power_group_index != -1) {
        s_device_group_datum *group = datum_get<s_device_group_datum>(tls_device_groups_data(), device->power_group_index);
        if (state->power_group != group->actual_value) {
            state->power_group = group->actual_value;
            updated_mask |= 0x20000;
        }
    }
    return updated_mask;
}



// @address 0x43b448 ?apply_game_event@c_simulation_device_touch_event_definition@@UAA_NJPBJJPBX@Z
bool c_simulation_device_touch_event_definition::apply_game_event(long reference_count, const long *references, long payload_size, const void *payload)
{
    long device_index = simulation_gamestate_entity_try_and_get_object_index(references[0], 0x190);
    long unit_index = simulation_gamestate_entity_try_and_get_object_index(references[1], 0x1003);
    if (device_index != -1 && unit_index != -1) {
        object_datum *unit = (object_datum *)object_get_and_verify_type(unit_index, NONE);
        bool dead = ((unit->damage_flags >> 2) & 1) != 0;
        if (!dead) {
            object_datum *device = (object_datum *)object_get_and_verify_type(device_index, NONE);
            if (distance_squared3d(&unit->bounding_center, &device->bounding_center) < 4.0f)
                device_touched(device_index, unit_index);
        }
    }
    return false;
}

static const real k_device_position_minimum = 0.0f;
static const real k_device_position_maximum = 1.0f;
// @address 0x43aae8 ?apply_device_position_update@c_simulation_device_entity_definition@@SAXJ_N0M0M0M0M@Z
void c_simulation_device_entity_definition::apply_device_position_update(long object_index, bool force, bool position_changed,
    float position, bool position_group_changed, float position_group_position, bool power_changed, float power, bool power_group_changed,
    float power_group_power)
{
    device_datum *device = (device_datum *)object_get_and_verify_type(object_index, NONE);
    if (force || position_changed)
    {
        if (!(position < k_device_position_minimum || position > k_device_position_maximum))
        {
            device->device_position = position;
            device->position_velocity = 0.0f;
            device_set_animation_changed(object_index);
            if (device->position_group_index != -1)
                datum_get<s_device_group_datum>(tls_device_groups_data(), device->position_group_index)->actual_value = position;
        }
    }
    if (force || position_group_changed)
    {
        if (!(position_group_position < k_device_position_minimum || position_group_position > k_device_position_maximum))
        {
            if (device->position_group_index != -1)
            {
                device_group_set_desired_value(object_index, device->position_group_index, position_group_position);
                if (force)
                    device_set_actual_position(object_index, position_group_position);
            }
        }
    }
    if (force || power_changed)
    {
        if (!(power < k_device_position_minimum || power > k_device_position_maximum))
        {
            device->power = power;
            device->power_velocity = 0.0f;
            device_set_animation_changed(object_index);
            if (device->power_group_index != -1)
                datum_get<s_device_group_datum>(tls_device_groups_data(), device->power_group_index)->actual_value = power;
        }
    }
    if (force || power_group_changed)
    {
        if (!(power_group_power < k_device_position_minimum || power_group_power > k_device_position_maximum))
        {
            if (device->power_group_index != -1)
            {
                device_group_set_desired_value(object_index, device->power_group_index, power_group_power);
                if (force)
                    device_set_power(object_index, power_group_power);
            }
        }
    }
}


// @address 0x43a940 ?create_object@c_simulation_device_entity_definition@@UAAJJPBXPAKJ0@Z
long c_simulation_device_entity_definition::create_object(long p1, const void *p2, unsigned long *p3, long p4, const void *p5)
{
    const s_simulation_object_creation_data *creation = (const s_simulation_object_creation_data *)p2;
    const s_simulation_device_state_data *state = (const s_simulation_device_state_data *)p5;
    long result = -1;
    object_placement_data placement;
    if (creation->definition_index != -1 && tag_try_and_get_unsafe(0x64657669, creation->definition_index) != 0)
    {
        ::c_simulation_object_entity_definition::object_setup_placement_data((s_simulation_object_creation_data const *)p2,
            (s_simulation_object_state_data const *)p5, (unsigned long *)(p3), (object_placement_data *)&placement);
        long object_index = this->object_create_object((const s_simulation_object_creation_data *)p2,
                                    (const s_simulation_object_state_data *)p5, p3, &placement);
        if (object_index != -1)
        {
            if (((*p3 & (1 << 14)) != 0) || ((*p3 & (1 << 15)) != 0) || ((*p3 & (1 << 16)) != 0) || ((*p3 & (1 << 17)) != 0))
            {
            apply_device_position_update(object_index, true, ((*p3 & (1 << 14)) != 0), state->position, ((*p3 & (1 << 15)) != 0),
                state->position_group, ((*p3 & (1 << 16)) != 0), state->power, ((*p3 & (1 << 17)) != 0), state->power_group);
                *p3 &= ~0x3c000;
            }
        }
        result = object_index;
    }
    return result;
}

// @address 0x43aa50 ?apply_object_update@c_simulation_device_entity_definition@@UAAXJKJPBX@Z
void c_simulation_device_entity_definition::apply_object_update(long p0, unsigned long p1, long p2, void const *p3)
{
    const s_simulation_device_state_data *state_data = (const s_simulation_device_state_data *)p3;
    long update_mask = p1 & 0x3fff;
    if (update_mask != 0)
        this->object_apply_update(p0, update_mask, (const s_simulation_object_state_data *)p3);
    if (!(p1 & (1u << 14)) && !(p1 & (1u << 15)) && !(p1 & (1u << 16)) && !(p1 & (1u << 17)))
        return;
    apply_device_position_update(p0, false, (p1 & (1u << 14)) != 0, state_data->position, (p1 & (1u << 15)) != 0,
        state_data->position_group, (p1 & (1u << 16)) != 0, state_data->power, (p1 & (1u << 17)) != 0, state_data->power_group);
}



// @address 0x43aea0 ?entity_update_encode@c_simulation_device_entity_definition@@UAA_N_NKPAKJPBXPBUs_simulation_view_telemetry_data@@PAVc_bitstream@@J0@Z
bool c_simulation_device_entity_definition::entity_update_encode(bool initial_update, unsigned long update_mask,
    unsigned long *update_mask_written, long state_data_size, const void *state_data, const s_simulation_view_telemetry_data *telemetry,
    c_bitstream *packet, long minimum_bits, bool force)
{
    const s_simulation_device_state_data *state = (const s_simulation_device_state_data *)state_data;
    bool result = false;
    unsigned long object_update_mask = update_mask & 0x3fff;
    long object_minimum_bits = minimum_bits + 4;
    if (c_simulation_object_entity_definition::object_update_encode(initial_update, object_update_mask, update_mask_written, telemetry,
            (const s_simulation_object_state_data *)state, packet, object_minimum_bits, ensure_object_position_update_quantization_inside_bsp(), force))
    {
        c_entity_update_encode_helper helper;
        long first_component = 14;
        long component_count = 4;
        unsigned long device_update_mask = update_mask & 0x3c000;
        if (helper.make_room_for_update(packet, minimum_bits, first_component, component_count, device_update_mask))
        {
            if (helper.write_component_header(14, "position-exists"))
                packet->write_quantized_real("position", state->position, k_device_compare_minimum, k_device_compare_maximum, 14, false, false);
            helper.finish_component();
            if (helper.write_component_header(15, "position-group-position-exists"))
                packet->write_quantized_real(
                    "position-group-position", state->position_group, k_device_compare_minimum, k_device_compare_maximum, 14, false, false);
            helper.finish_component();
            if (helper.write_component_header(16, "power-exists"))
                packet->write_quantized_real("power", state->power, k_device_compare_minimum, k_device_compare_maximum, 14, false, false);
            helper.finish_component();
            if (helper.write_component_header(17, "power-group-power-exists"))
                packet->write_quantized_real("power-group-power", state->power_group, k_device_compare_minimum, k_device_compare_maximum, 14, false, false);
            helper.finish_component();
            helper.finish_update(update_mask_written);
            result = true;
        }
    }
    return result;
}

// @address 0x42e748 ?creation_data_size@c_simulation_device_entity_definition@@UAAJXZ
long c_simulation_device_entity_definition::creation_data_size()
{
    return sizeof(s_simulation_object_creation_data);
}

// @address 0x433b30 ?update_flag_count@c_simulation_device_entity_definition@@UAAJXZ
long c_simulation_device_entity_definition::update_flag_count()
{
    return 18;
}

// @address 0x43a748 ?initial_update_mask@c_simulation_device_entity_definition@@UAAKXZ
unsigned long c_simulation_device_entity_definition::initial_update_mask()
{
    return 0x3db3f;
}

// @address 0x42d380 ?entity_type_is_gameworld_object@c_simulation_device_entity_definition@@UAA_NXZ
bool c_simulation_device_entity_definition::entity_type_is_gameworld_object()
{
    return true;
}

// @address 0x433fa0 ?entity_can_be_created@c_simulation_device_entity_definition@@UAA_NPBUs_simulation_entity@@PBUs_simulation_view_telemetry_data@@@Z
bool c_simulation_device_entity_definition::entity_can_be_created(const s_simulation_entity *entity,
    const s_simulation_view_telemetry_data *telemetry)
{
    return object_entity_can_be_created(entity, telemetry);
}

// @address 0x433fc0 ?creation_minimum_required_bits@c_simulation_device_entity_definition@@UAAJPBUs_simulation_entity@@PBUs_simulation_view_telemetry_data@@@Z
long c_simulation_device_entity_definition::creation_minimum_required_bits(const s_simulation_entity *entity,
    const s_simulation_view_telemetry_data *telemetry)
{
    return 110;
}

// @address 0x43ae28 ?entity_creation_decode@c_simulation_device_entity_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
bool c_simulation_device_entity_definition::entity_creation_decode(long creation_data_size, void *creation_data, c_bitstream *packet,
    bool final_update)
{
    return object_entity_creation_decode(creation_data_size, creation_data, packet, final_update);
}

// @address 0x42d380 ?entity_creation_lossy_compare@c_simulation_device_entity_definition@@UAA_NPAX0J@Z
bool c_simulation_device_entity_definition::entity_creation_lossy_compare(void *first, void *second, long size)
{
    return true;
}

// @address 0x43add8 ?rotate_entity_indices@c_simulation_device_entity_definition@@UAAXPBUs_simulation_entity@@@Z
void c_simulation_device_entity_definition::rotate_entity_indices(const s_simulation_entity *entity)
{
    object_entity_rotate_indices(entity);
}

// @address 0x5bec98 ?prepare_creation_data_for_gameworld@c_simulation_device_entity_definition@@UAAXJPAX@Z
void c_simulation_device_entity_definition::prepare_creation_data_for_gameworld(long size, void *creation_data)
{
}

// @address 0x434748 ?prepare_state_data_for_gameworld@c_simulation_device_entity_definition@@UAAXKJPAX@Z
void c_simulation_device_entity_definition::prepare_state_data_for_gameworld(unsigned long update_mask, long size, void *state)
{
    object_entity_prepare_state_data(update_mask, size, state);
}

// @address 0x43a758 ?build_object_creation_data@c_simulation_device_entity_definition@@UAAXJJPAX@Z
void c_simulation_device_entity_definition::build_object_creation_data(long object_index, long creation_data_size, void *creation_data)
{
    object_entity_build_creation_data(object_index, creation_data_size, creation_data);
}

// @address 0x42d380 ?object_required_to_join_game@c_simulation_device_entity_definition@@UAA_NJ@Z
bool c_simulation_device_entity_definition::object_required_to_join_game(long object_index)
{
    return true;
}

// @address 0x42d298 ?handle_delete_object@c_simulation_device_entity_definition@@UAA_NJ@Z
bool c_simulation_device_entity_definition::handle_delete_object(long object_index)
{
    return false;
}

// @address 0x42d380 ?promote_object_to_authority@c_simulation_device_entity_definition@@UAA_NJ@Z
bool c_simulation_device_entity_definition::promote_object_to_authority(long object_index)
{
    return true;
}

// @address 0x42d298 ?ensure_object_position_update_quantization_inside_bsp@c_simulation_device_entity_definition@@UAA_NXZ
bool c_simulation_device_entity_definition::ensure_object_position_update_quantization_inside_bsp()
{
    return false;
}

// @address 0x42d298 ?payload_size@c_simulation_device_touch_event_definition@@UAAJXZ
long c_simulation_device_touch_event_definition::payload_size()
{
    return 0;
}

// @address 0x3fc598 ?number_of_entity_references@c_simulation_device_touch_event_definition@@UAAJXZ
long c_simulation_device_touch_event_definition::number_of_entity_references()
{
    return 2;
}

// @address 0x42d298 ?reference_delays_entity_deletion@c_simulation_device_touch_event_definition@@UAA_NXZ
bool c_simulation_device_touch_event_definition::reference_delays_entity_deletion()
{
    return false;
}

// @address 0x42d380 ?event_can_be_transmitted@c_simulation_device_touch_event_definition@@UAA_NPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
bool c_simulation_device_touch_event_definition::event_can_be_transmitted(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    return true;
}

// @address 0x42d298 ?minimum_required_bits@c_simulation_device_touch_event_definition@@UAAJPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
long c_simulation_device_touch_event_definition::minimum_required_bits(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    return 0;
}

// @address 0x4311b8 ?calculate_relevance@c_simulation_device_touch_event_definition@@UAAMPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@M@Z
float c_simulation_device_touch_event_definition::calculate_relevance(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance)
{
    return simulation_calculate_event_relevance(this, event, relevance, telemetry, 0);
}

// @address 0x42e7d8 ?event_requirements@c_simulation_device_touch_event_definition@@UAA?AUs_simulation_event_requirements@@PBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
s_simulation_event_requirements c_simulation_device_touch_event_definition::event_requirements(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    s_simulation_event_requirements requirements;
    requirements.reference_count = 2;
    requirements.reference_delays_entity_deletion = false;
    requirements.can_be_transmitted = true;
    requirements.minimum_required_bits = 0;
    return requirements;
}

// @address 0x5bec98 ?event_payload_encode@c_simulation_device_touch_event_definition@@UAAXJPBXPAVc_bitstream@@_N@Z
void c_simulation_device_touch_event_definition::event_payload_encode(long payload_size, const void *payload, c_bitstream *packet, bool quantize)
{
}

// @address 0x42d380 ?event_payload_decode@c_simulation_device_touch_event_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
bool c_simulation_device_touch_event_definition::event_payload_decode(long payload_size, void *payload, c_bitstream *packet, bool quantize)
{
    return true;
}

// @address 0x5bec98 ?prepare_event_data_for_gameworld@c_simulation_device_touch_event_definition@@UAAXJPAX@Z
void c_simulation_device_touch_event_definition::prepare_event_data_for_gameworld(long payload_size, void *payload)
{
}
