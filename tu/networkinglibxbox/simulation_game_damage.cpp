// networkinglibxbox/simulation_game_damage.obj: from the records 0x42bf28 0x42bf38 0x42bf48 0x42bf58 0x42bf68 0x42bf78 0x42bf88 0x42bf90 0x42bf98 0x42bff8 0x42c268 0x42c5f8 0x42c908 0x42c968 0x42c9b8 0x42ca28 0x42cc28 0x42cc30 0x42cc40 0x42cc70 0x42cce0 0x42cd70 0x42cdc0 0x42cdd0 0x42ce08 0x42ce78 0x42cef8 0x42cf08 0x42cf98 0x42d040 0x42d0b8 0x42d0e0 0x42d100 0x42d170 0x42d1e0 0x42d210 0x42d288 0x42d2a0 0x42d528 0x709f08 0x70c7c0
// @flags /TP /O2 /Oc
#include "simulation/simulation_damage_aftermath_types.h"
#include "networking/simulation/simulation_entity_definition.h"
#include "memory/tls_globals.h"
#include "objects/object_header.h"
#include "tag_files/tag_groups.h"
#include "networking/simulation/simulation_entity_database.h"
#include "memory/data_types.h"
#include "math/real_math_types.h"
#include "objects/object_types.h"
#include "objects/object_datum.h"
#include "objects/objects.h"
#include "cache/cache_files.h"
#include "objects/damage.h"
#include "simulation/game_interface/simulation_game_internal.h"
#include "simulation/simulation_encoding.h"
#include "simulation/simulation_gamestate_entities.h"
#include "simulation/simulation_priority.h"
#include "units/bipeds.h"
#include "memory/bitstream.h"
#include "scenario/scenario.h"
#include "simulation/simulation_entity_types.h"
// @address 0x4845c ??_7c_simulation_damage_aftermath_event_definition@@6B@
// @address 0x48494 ??_7c_simulation_damage_section_response_event_definition@@6B@
// @address 0x484cc ??_7c_simulation_breakable_surface_damage_event_definition@@6B@
// @address 0x42bf90 ?minimum_required_bits@c_simulation_damage_aftermath_event_definition@@UAAJPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
class c_bitstream;
union real_point3d;
union real_vector3d;
union real_rectangle3d;
struct s_model_damage_info;
struct s_model_definition;
struct s_object_definition;
#include <string.h>
#include <ppcintrinsics.h>

class c_replication_outgoing_event;


union real_rectangle3d;
#include "simulation/game_interface/simulation_game_damage_data_types.h"
#include "simulation/game_interface/simulation_game_breakable_surfaces.h"
#include "simulation/game_interface/simulation_game_statborg_entity_definition.h"
#include "simulation/game_interface/simulation_game_map_variant.h"

#include "simulation/game_interface/simulation_game_damage_event_definitions.h"




#include "models/model_damage_info.h"
#include "models/model_types.h"
#include "objects/object_definitions.h"

#include "objects/damage_response_data.h"
#include "objects/damage_section_response.h"

#include "objects/damage_aftermath_types.h"
#include "simulation/game_interface/simulation_game_damage_operations.h"
#include "math/real_math_globals.h"
#include "math/real_constant_pool.h"
#include "game/game_time_types.h"
#include "math/real_math.h"
#include "game/player_types.h"
#include "units/unit_types.h"
#include "objects/object_type_access.h"
#include "simulation/simulation_game_entity_definitions.h"
#include "simulation/simulation_game_event_definitions.h"

// @address 0x48508 ??_7c_simulation_breakable_surface_group_entity_definition@@6B@
// @address 0x709f08 ??__Eg_simulation_breakable_surface_group_entity_definition@@YAXXZ
// @address 0x70c7c0 ??__Fg_simulation_breakable_surface_group_entity_definition@@YAXXZ
// @address 0x7bb460 g_simulation_breakable_surface_group_entity_definition
c_simulation_breakable_surface_group_entity_definition g_simulation_breakable_surface_group_entity_definition;

// @address 0x7bb454 g_simulation_damage_aftermath_event_definition
c_simulation_damage_aftermath_event_definition g_simulation_damage_aftermath_event_definition;
// @address 0x7bb458 g_simulation_damage_section_response_event_definition
c_simulation_damage_section_response_event_definition g_simulation_damage_section_response_event_definition;
// @address 0x7bb45c g_simulation_breakable_surface_damage_event_definition
c_simulation_breakable_surface_damage_event_definition g_simulation_breakable_surface_damage_event_definition;

static __forceinline void damage_read_raw_data(c_bitstream *packet, const char *name, void *buffer, long size_in_bits)
{
    unsigned char *data = (unsigned char *)buffer;
    if ((unsigned long)size_in_bits >= 64)
    {
        unsigned long word_count = (unsigned long)size_in_bits / 64;
        size_in_bits -= word_count * 64;
        do
        {
            *(unsigned __int64 *)data = bitstream_read_window_bits(packet, 64);
            data += sizeof(unsigned __int64);
        } while (--word_count != 0);
    }
    if (size_in_bits != 0)
    {
        unsigned __int64 value = bitstream_read_window_bits(packet, size_in_bits) << (unsigned long)(64 - size_in_bits);
        if (size_in_bits >= 8)
        {
            unsigned long byte_count = (unsigned long)size_in_bits / 8;
            size_in_bits -= byte_count * 8;
            do
            {
                *data++ = (unsigned char)(value >> 56);
                value <<= 8;
            } while (--byte_count != 0);
        }
        if (size_in_bits != 0)
            *data = (unsigned char)(value >> 56);
    }
}



struct s_game_engine_globals;
namespace {
static __forceinline void simulation_gamestate_entity_set_object_index(long entity_index, long object_index) {
    long absolute_index = entity_index & 0xffff;
    s_simulation_gamestate_entity_datum *datum = &((s_simulation_gamestate_entity_datum *)tls_simulation_gamestate_entity_data()->data)[absolute_index];
    datum->object_index = object_index;
}
}

// @address 0x42d298 ?event_type@c_simulation_damage_aftermath_event_definition@@UAA?AW4e_simulation_event_type@@XZ
e_simulation_event_type c_simulation_damage_aftermath_event_definition::event_type() { return _simulation_event_type_damage_aftermath; }
// @address 0x42d380 ?event_type@c_simulation_damage_section_response_event_definition@@UAA?AW4e_simulation_event_type@@XZ
e_simulation_event_type c_simulation_damage_section_response_event_definition::event_type() { return _simulation_event_type_damage_section_response; }
// @address 0x3fc598 ?event_type@c_simulation_breakable_surface_damage_event_definition@@UAA?AW4e_simulation_event_type@@XZ
e_simulation_event_type c_simulation_breakable_surface_damage_event_definition::event_type() { return _simulation_event_type_breakable_surface_damage; }
// @address 0x42bf28 ?event_type_name@c_simulation_damage_aftermath_event_definition@@UAAPBDXZ
const char *c_simulation_damage_aftermath_event_definition::event_type_name() { return "damage_aftermath"; }
// @address 0x42bf48 ?event_type_name@c_simulation_damage_section_response_event_definition@@UAAPBDXZ
const char *c_simulation_damage_section_response_event_definition::event_type_name() { return "damage_section_response"; }
// @address 0x42bf68 ?event_type_name@c_simulation_breakable_surface_damage_event_definition@@UAAPBDXZ
const char *c_simulation_breakable_surface_damage_event_definition::event_type_name() { return "breakable_surface_damage"; }
// @address 0x42bf88 ?payload_size@c_simulation_damage_aftermath_event_definition@@UAAJXZ
long c_simulation_damage_aftermath_event_definition::payload_size() { return 80; }
// @address 0x42e628 ?payload_size@c_simulation_damage_section_response_event_definition@@UAAJXZ
long c_simulation_damage_section_response_event_definition::payload_size() { return 12; }
// @address 0x42e770 ?payload_size@c_simulation_breakable_surface_damage_event_definition@@UAAJXZ
long c_simulation_breakable_surface_damage_event_definition::payload_size() { return 24; }
// @address 0x3fc598 ?number_of_entity_references@c_simulation_damage_aftermath_event_definition@@UAAJXZ
long c_simulation_damage_aftermath_event_definition::number_of_entity_references() { return 2; }
// @address 0x42d380 ?number_of_entity_references@c_simulation_damage_section_response_event_definition@@UAAJXZ
long c_simulation_damage_section_response_event_definition::number_of_entity_references() { return 1; }
// @address 0x42d298 ?number_of_entity_references@c_simulation_breakable_surface_damage_event_definition@@UAAJXZ
long c_simulation_breakable_surface_damage_event_definition::number_of_entity_references() { return 0; }
// @address 0x42d298 ?reference_delays_entity_deletion@c_simulation_damage_aftermath_event_definition@@UAA_NXZ
bool c_simulation_damage_aftermath_event_definition::reference_delays_entity_deletion() { return false; }
// @address 0x42d380 ?reference_delays_entity_deletion@c_simulation_damage_section_response_event_definition@@UAA_NXZ
bool c_simulation_damage_section_response_event_definition::reference_delays_entity_deletion() { return true; }
// @address 0x42d298 ?reference_delays_entity_deletion@c_simulation_breakable_surface_damage_event_definition@@UAA_NXZ
bool c_simulation_breakable_surface_damage_event_definition::reference_delays_entity_deletion() { return false; }
// @address 0x42d380 ?event_can_be_transmitted@c_simulation_damage_aftermath_event_definition@@UAA_NPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
bool c_simulation_damage_aftermath_event_definition::event_can_be_transmitted(
    const c_replication_outgoing_event *, const s_simulation_view_telemetry_data *) { return true; }
// @address 0x42d380 ?event_can_be_transmitted@c_simulation_damage_section_response_event_definition@@UAA_NPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
bool c_simulation_damage_section_response_event_definition::event_can_be_transmitted(
    const c_replication_outgoing_event *, const s_simulation_view_telemetry_data *) { return true; }
// @address 0x42d380 ?event_can_be_transmitted@c_simulation_breakable_surface_damage_event_definition@@UAA_NPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
bool c_simulation_breakable_surface_damage_event_definition::event_can_be_transmitted(
    const c_replication_outgoing_event *, const s_simulation_view_telemetry_data *) { return true; }
long c_simulation_damage_aftermath_event_definition::minimum_required_bits(const c_replication_outgoing_event *, const s_simulation_view_telemetry_data *) {
    long minimum_required_bits = 66;
    minimum_required_bits += simulation_definition_table_index_bits();
    return minimum_required_bits;
}
// @address 0x42e810 ?minimum_required_bits@c_simulation_damage_section_response_event_definition@@UAAJPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
long c_simulation_damage_section_response_event_definition::minimum_required_bits(
    const c_replication_outgoing_event *, const s_simulation_view_telemetry_data *) { return 9; }
// @address 0x4311b8 ?calculate_relevance@c_simulation_damage_aftermath_event_definition@@UAAMPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@M@Z
float c_simulation_damage_aftermath_event_definition::calculate_relevance(
    const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry, float relevance)
{
    return simulation_calculate_event_relevance(
        this, event, relevance, telemetry, 0);
}
// @address 0x4311b8 ?calculate_relevance@c_simulation_damage_section_response_event_definition@@UAAMPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@M@Z
float c_simulation_damage_section_response_event_definition::calculate_relevance(
    const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry, float relevance)
{
    return simulation_calculate_event_relevance(
        this, event, relevance, telemetry, 0);
}
// @address 0x5bec98 ?prepare_event_data_for_gameworld@c_simulation_damage_aftermath_event_definition@@UAAXJPAX@Z
void c_simulation_damage_aftermath_event_definition::prepare_event_data_for_gameworld(long, void *) {}
// @address 0x5bec98 ?prepare_event_data_for_gameworld@c_simulation_damage_section_response_event_definition@@UAAXJPAX@Z
void c_simulation_damage_section_response_event_definition::prepare_event_data_for_gameworld(long, void *) {}
// @address 0x5bec98 ?prepare_event_data_for_gameworld@c_simulation_breakable_surface_damage_event_definition@@UAAXJPAX@Z
void c_simulation_breakable_surface_damage_event_definition::prepare_event_data_for_gameworld(long, void *) {}
// @address 0x42bf98 ?write_description_to_string@c_simulation_damage_aftermath_event_definition@@UAAXPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@MJPAD@Z
void c_simulation_damage_aftermath_event_definition::write_description_to_string(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance, long buffer_size, char *buffer)
{
    csnzprintf(buffer, buffer_size, "damage aftermath: relevance=%5.3f", calculate_relevance(event, telemetry, relevance));
}
// @address 0x42c908 ?write_description_to_string@c_simulation_damage_section_response_event_definition@@UAAXPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@MJPAD@Z
void c_simulation_damage_section_response_event_definition::write_description_to_string(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance, long buffer_size, char *buffer)
{
    csnzprintf(buffer, buffer_size, "damage section response: relevance=%5.3f", calculate_relevance(event, telemetry, relevance));
}
// @address 0x42c968 ?event_payload_encode@c_simulation_damage_section_response_event_definition@@UAAXJPBXPAVc_bitstream@@_N@Z
void c_simulation_damage_section_response_event_definition::event_payload_encode(
    long payload_size, const void *payload, c_bitstream *packet, bool use_encoding)
{
    const s_simulation_damage_section_response_event_data *event_data = (const s_simulation_damage_section_response_event_data *)payload;
    packet->write_integer("damage-section-index", event_data->damage_section_index, 4);
    packet->write_integer("response-index", event_data->response_index, 4);
    packet->write_integer("damage-section-response-type", event_data->response_type, 1);
}
// @address 0x42c9b8 ?event_payload_decode@c_simulation_damage_section_response_event_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
bool c_simulation_damage_section_response_event_definition::event_payload_decode(long payload_size, void *payload, c_bitstream *packet, bool use_encoding) {
    s_simulation_damage_section_response_event_data *event_data = (s_simulation_damage_section_response_event_data *)payload;
    event_data->damage_section_index = packet->read_integer("damage-section-index", 4);
    event_data->response_index = packet->read_integer("response-index", 4);
    event_data->response_type = packet->read_integer("damage-section-response-type", 1);
    return true;
}
// @address 0x42cc40 ?write_description_to_string@c_simulation_breakable_surface_damage_event_definition@@UAAXPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@MJPAD@Z
void c_simulation_breakable_surface_damage_event_definition::write_description_to_string(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance, long buffer_size, char *buffer)
{
    csnzprintf(buffer, buffer_size, "damage section response");
}
// @address 0x42cce0 ?event_payload_decode@c_simulation_breakable_surface_damage_event_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
bool c_simulation_breakable_surface_damage_event_definition::event_payload_decode(long payload_size, void *payload, c_bitstream *packet, bool use_encoding)
{
    s_simulation_breakable_surface_damage_event_data *event_data = (s_simulation_breakable_surface_damage_event_data *)payload;
    event_data->instanced_geometry_instance_index = packet->read_integer("instanced-geometry-instance-index", 12) - 1;
    event_data->seed_surface_index = packet->read_integer("seed-surface-index", 17) - 1;
    damage_read_raw_data(packet, "location", event_data->location, 32);
    ::simulation_read_quantized_position(packet, &event_data->position, 13, 0);
    return true;
}


// @address 0x42cdc0 ?entity_type_name@c_simulation_breakable_surface_group_entity_definition@@UAAPBDXZ
const char *c_simulation_breakable_surface_group_entity_definition::entity_type_name() { return "breakable-surface-group"; }
// @address 0x42cdd0 ?gameworld_attachment_valid@c_simulation_breakable_surface_group_entity_definition@@UAA_NJ@Z
bool c_simulation_breakable_surface_group_entity_definition::gameworld_attachment_valid(long gamestate_index) {
    bool valid = false;
    if (simulation_gamestate_entity_get_object_index(gamestate_index) != -1) {
        valid = true;
    }
    return valid;
}
// @address 0x42cef8 ?entity_creation_encode@c_simulation_breakable_surface_group_entity_definition@@UAAXJPBXPBUs_simulation_view_telemetry_data@@PAVc_bitstream@@_N@Z
void c_simulation_breakable_surface_group_entity_definition::entity_creation_encode(long creation_data_size, const void *creation_data,
    const s_simulation_view_telemetry_data *telemetry, c_bitstream *bitstream, bool use_encoding)
{
    const s_simulation_breakable_surface_group_creation_data *group_creation = (const s_simulation_breakable_surface_group_creation_data *)creation_data;
    bitstream->write_raw_data("group_index", &group_creation->group_index, 16);
}

// @address 0x42cf08 ?entity_creation_decode@c_simulation_breakable_surface_group_entity_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
bool c_simulation_breakable_surface_group_entity_definition::entity_creation_decode(
    long creation_data_size, void *creation_data, c_bitstream *packet, bool use_encoding)
{
    bool result = false;
    s_simulation_breakable_surface_group_creation_data *group_creation = (s_simulation_breakable_surface_group_creation_data *)creation_data;
    damage_read_raw_data(packet, "group_index", &group_creation->group_index, 16);
    result = !packet->overflowed();
    result = result && group_creation->group_index != 0xffff;
    return result;
}

// @address 0x42cf98 ?entity_update_encode@c_simulation_breakable_surface_group_entity_definition@@UAA_N_NKPAKJPBXPBUs_simulation_view_telemetry_data@@PAVc_bitstream@@J0@Z
bool c_simulation_breakable_surface_group_entity_definition::entity_update_encode(bool initial_update, unsigned long update_mask,
    unsigned long *update_mask_out, long state_data_size, const void *state_data, const s_simulation_view_telemetry_data *telemetry,
    c_bitstream *bitstream, long required_leave_space_bits, bool use_encoding)
{
    const s_simulation_breakable_surface_group_state_data *group_state = (const s_simulation_breakable_surface_group_state_data *)state_data;
    bool result = false;
    c_entity_update_encode_helper helper;
    if (helper.make_room_for_update(bitstream, required_leave_space_bits, 0, 1, update_mask))
    {
        if (helper.write_component_header(0, "surface-group-update-exists"))
        {
            bitstream->write_raw_data("flags", group_state->damaged_surfaces, 32);
        }
        helper.finish_component();
        helper.finish_update(update_mask_out);
        result = true;
    }
    return result;
}

// @address 0x42d040 ?entity_update_decode@c_simulation_breakable_surface_group_entity_definition@@UAA_N_NPAKJPAXPAVc_bitstream@@0@Z
bool c_simulation_breakable_surface_group_entity_definition::entity_update_decode(
    bool initial_update, unsigned long *update_mask, long state_data_size, void *state_data, c_bitstream *packet, bool use_encoding)
{
    s_simulation_breakable_surface_group_state_data *group_state = (s_simulation_breakable_surface_group_state_data *)state_data;
    if (packet->read_bool("surface-group-update-exists"))
    {
        damage_read_raw_data(packet, "flags", group_state->damaged_surfaces, 32);
        *update_mask |= 1;
    }
    return true;
}


// @address 0x42d0b8 ?build_creation_data@c_simulation_breakable_surface_group_entity_definition@@UAAXJJPAX@Z
void c_simulation_breakable_surface_group_entity_definition::build_creation_data(long gamestate_index, long creation_data_size, void *creation_data) {
    s_simulation_breakable_surface_group_creation_data *group_creation = (s_simulation_breakable_surface_group_creation_data *)creation_data;
    group_creation->group_index = (short)simulation_gamestate_entity_get_object_index(gamestate_index);
}
// @address 0x42d0e0 ?build_baseline_state_data@c_simulation_breakable_surface_group_entity_definition@@UAA_NJPBXJPAX@Z
bool c_simulation_breakable_surface_group_entity_definition::build_baseline_state_data(
    long creation_data_size, const void *creation_data, long baseline_state_data_size, void *baseline_state_data)
{
    memset(baseline_state_data, 0, sizeof(s_simulation_breakable_surface_group_state_data));
    return true;
}
// @address 0x42d170 ?create_game_entity@c_simulation_breakable_surface_group_entity_definition@@UAA_NJJPBXKJ0@Z
bool c_simulation_breakable_surface_group_entity_definition::create_game_entity(long gamestate_index, long creation_data_size,
    const void *creation_data, unsigned long initial_update_mask, long state_data_size, const void *state_data)
{
    const s_simulation_breakable_surface_group_creation_data *group_creation = (const s_simulation_breakable_surface_group_creation_data *)creation_data;
    simulation_gamestate_entity_set_object_index(gamestate_index, (short)group_creation->group_index);
    if (initial_update_mask != 0) {
        update_game_entity(gamestate_index, initial_update_mask, state_data_size, state_data);
    }
    return true;
}

// 0x42ce08: the "breakable surface group creation" debug line with the object's relevance

// @address 0x42ce08 ?write_creation_description_to_string@c_simulation_breakable_surface_group_entity_definition@@UAAXPBUs_simulation_entity@@PBUs_simulation_view_telemetry_data@@JPAD@Z
void c_simulation_breakable_surface_group_entity_definition::write_creation_description_to_string(
    const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry, long size, char *buffer)
{
    float relevance = 0.0f;
    simulation_calculate_entity_creation_priority(entity, telemetry, &relevance);
    csnzprintf(buffer, size, "breakable surface group creation: relevance=%5.3f", relevance);
}

// @address 0x42ce78 ?write_update_description_to_string@c_simulation_breakable_surface_group_entity_definition@@UAAXPBUs_simulation_entity@@PBUs_entity_update_data@@JPAD@Z
void c_simulation_breakable_surface_group_entity_definition::write_update_description_to_string(
    const s_simulation_entity *entity, const s_entity_update_data *update_data, long buffer_size, char *buffer)
{
    float relevance = 0.0f;
    long period = 0;
    simulation_calculate_entity_update_priority(entity, update_data, &relevance, &period);
    csnzprintf(buffer, buffer_size, "breakable surface group update:relevance=%5.3f: period=%d", relevance, period);
}

// @address 0x42d210 ?promote_game_entity_to_authority@c_simulation_game_statborg_entity_definition@@UAA_NJ@Z
bool c_simulation_game_statborg_entity_definition::promote_game_entity_to_authority(long gamestate_index)
{
    bool result = false;
    if (gameworld_attachment_valid(gamestate_index))
        result = true;
    return result;
}

// @address 0x42d2a0 ?promote_game_entity_to_authority@c_simulation_map_variant_group_entity_definition@@UAA_NJ@Z
bool c_simulation_map_variant_group_entity_definition::promote_game_entity_to_authority(long gamestate_index)
{
    bool success = false;
    if (gameworld_attachment_valid(gamestate_index)) {
        long group_index = simulation_gamestate_entity_get_object_index(gamestate_index);
        if (group_index != -1) {
            breakable_surface_group_set_gamestate_index(group_index, gamestate_index);
            success = true;
        }
    }
    return success;
}

// @address 0x42bf38 ?event_requirements@c_simulation_damage_aftermath_event_definition@@UAA?AUs_simulation_event_requirements@@PBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
s_simulation_event_requirements c_simulation_damage_aftermath_event_definition::event_requirements(
    const c_replication_outgoing_event *, const s_simulation_view_telemetry_data *)
{
    s_simulation_event_requirements requirements;
    requirements.reference_count = 2;
    requirements.reference_delays_entity_deletion = false;
    requirements.can_be_transmitted = true;
    requirements.minimum_required_bits = 66 + simulation_definition_table_index_bits();
    return requirements;
}

// @address 0x42bf58 ?event_requirements@c_simulation_damage_section_response_event_definition@@UAA?AUs_simulation_event_requirements@@PBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
s_simulation_event_requirements c_simulation_damage_section_response_event_definition::event_requirements(
    const c_replication_outgoing_event *, const s_simulation_view_telemetry_data *)
{
    s_simulation_event_requirements requirements;
    requirements.reference_count = 1;
    requirements.reference_delays_entity_deletion = true;
    requirements.can_be_transmitted = true;
    requirements.minimum_required_bits = 9;
    return requirements;
}

// @address 0x42bf78 ?event_requirements@c_simulation_breakable_surface_damage_event_definition@@UAA?AUs_simulation_event_requirements@@PBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
s_simulation_event_requirements c_simulation_breakable_surface_damage_event_definition::event_requirements(
    const c_replication_outgoing_event *, const s_simulation_view_telemetry_data *)
{
    s_simulation_event_requirements requirements;
    requirements.reference_count = 0;
    requirements.reference_delays_entity_deletion = false;
    requirements.can_be_transmitted = true;
    requirements.minimum_required_bits = 142;
    return requirements;
}

// @address 0x42cc28 ?minimum_required_bits@c_simulation_breakable_surface_damage_event_definition@@UAAJPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
long c_simulation_breakable_surface_damage_event_definition::minimum_required_bits(
    const c_replication_outgoing_event *, const s_simulation_view_telemetry_data *) { return 142; }

// @address 0x42cc30 ?calculate_relevance@c_simulation_breakable_surface_damage_event_definition@@UAAMPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@M@Z
float c_simulation_breakable_surface_damage_event_definition::calculate_relevance(
    const c_replication_outgoing_event *, const s_simulation_view_telemetry_data *, float) { return 0.0f; }

// @address 0x42cc70 ?event_payload_encode@c_simulation_breakable_surface_damage_event_definition@@UAAXJPBXPAVc_bitstream@@_N@Z
void c_simulation_breakable_surface_damage_event_definition::event_payload_encode(
    long payload_size, const void *payload, c_bitstream *packet, bool use_encoding)
{
    const s_simulation_breakable_surface_damage_event_data *event_data = (const s_simulation_breakable_surface_damage_event_data *)payload;
    packet->write_integer("instanced-geometry-instance-index", event_data->instanced_geometry_instance_index + 1, 12);
    packet->write_integer("seed-surface-index", event_data->seed_surface_index + 1, 17);
    packet->write_raw_data("location", event_data->location, 32);
    simulation_write_quantized_position(packet, &event_data->position, 13, false, 0);
}

// @address 0x42cd70 ?apply_game_event@c_simulation_breakable_surface_damage_event_definition@@UAA_NJPBJJPBX@Z
bool c_simulation_breakable_surface_damage_event_definition::apply_game_event(long, const long *, long, const void *payload)
{
    const s_simulation_breakable_surface_damage_event_data *event_data = (const s_simulation_breakable_surface_damage_event_data *)payload;
    breakable_surface_break_from_record(31 - _CountLeadingZeros(-g_active_structure_bsp_mask & g_active_structure_bsp_mask),
        event_data->instanced_geometry_instance_index, event_data->seed_surface_index,
        &event_data->position, (const s_breakable_surfaces_event_material *)event_data->location);
    return false;
}

// @address 0x42d100 ?build_updated_state_data@c_simulation_breakable_surface_group_entity_definition@@UAA_NPBUs_simulation_entity@@PAKJPAX@Z
bool c_simulation_breakable_surface_group_entity_definition::build_updated_state_data(const s_simulation_entity *entity,
    unsigned long *update_flags, long size, void *state)
{
    unsigned long flags = *update_flags;
    long *pending = static_cast<long *>(state);
    bool success = true;
    if ((flags & 1) != 0)
    {
        long group_index = simulation_gamestate_entity_get_object_index(entity->gamestate_index);
        if (group_index != -1)
        {
            success = false;
            if (*pending != 0)
            {
                *update_flags = flags | 1;
                *pending = 0;
            }
        }
        else
        {
            success = false;
        }
    }
    return success;
}

// @address 0x42d1e0 ?update_game_entity@c_simulation_breakable_surface_group_entity_definition@@UAA_NJKJPBX@Z
bool c_simulation_breakable_surface_group_entity_definition::update_game_entity(long gamestate_index, unsigned long, long, const void *)
{
    gameworld_attachment_valid(gamestate_index);
    return true;
}

// @address 0x42d288 ?initial_update_mask@c_simulation_map_variant_group_entity_definition@@UAAKXZ
unsigned long c_simulation_map_variant_group_entity_definition::initial_update_mask()
{
    return 0xCCCCCCCC;
}

// @address 0x42d528 ?build_creation_data@c_simulation_map_variant_group_entity_definition@@UAAXJJPAX@Z
void c_simulation_map_variant_group_entity_definition::build_creation_data(long gamestate_index, long size, void *creation_data)
{
    *static_cast<long *>(creation_data) = simulation_gamestate_entity_get_object_index(gamestate_index);
}


// @rdata 0x8202251c 4
static const float k_dmg_zero = 0.0f;
// @rdata 0x82024448 4
static const float k_dmg_25 = 25.0f;
// @rdata 0x820242d8 4
static const float k_dmg_2 = 2.0f;
// @rdata 0x82022ba0 4
static const float k_dmg_16 = 16.0f;
// @rdata 0x820242d4 4
static const float k_dmg_3 = 3.0f;

static __forceinline void simulation_game_damage_copy_vector(real_vector3d *out, const real_vector3d *in)
{
    *out = *in;
}
static __forceinline void simulation_game_damage_read_unit_vector(c_bitstream *packet, const char *name, real_vector3d *vector)
{
    long quantized = packet->read_integer(name, 19);
    bitstream_decode_unit_vector(quantized, vector);
}
static __forceinline float simulation_game_damage_read_quantized_real(
    c_bitstream *packet, const char *name, float minimum, float maximum, long bits, bool midpoint)
{
    long quantized = packet->read_integer(name, bits);
    return dequantize_real(quantized, minimum, maximum, bits, midpoint);
}
// @address 0x42c268 ?event_payload_decode@c_simulation_damage_aftermath_event_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
bool c_simulation_damage_aftermath_event_definition::event_payload_decode(long payload_size, void *payload, c_bitstream *packet, bool use_encoding)
{
    s_simulation_damage_aftermath_event_data *event_data = (s_simulation_damage_aftermath_event_data *)payload;
    event_data->damage_definition_index = simulation_read_definition_index("damage-definition-index", packet);
    event_data->damage_owner_player_index = (short)(packet->read_integer("damage-owner-player-index", 5) - 1);
    event_data->damage_owner_object_index = 0;
    event_data->direction_valid = packet->read_bool("direction-valid");
    if (event_data->direction_valid)
        simulation_game_damage_read_unit_vector(packet, "direction", &event_data->direction);
    else {
        simulation_game_damage_copy_vector(&event_data->direction, global_zero_vector3d);
    }
    event_data->epicenter_valid = packet->read_bool("epicenter-valid");
    float minimum;
    if (event_data->epicenter_valid) {
#if defined(_XBOX) && !defined(__clang__)
        // match: 0x42c268: 0.0f (-0x1db8), 25.0f (+0x174), 2.0f, 16.0f and 3.0f load off one 0x820242d4 base
        // held in r29; spelled literals take a lis each and shift the saved gprs from r28 to r29
        minimum = k_real_constant_pool_three[-1902];
#else
        minimum = 0.0f;
#endif
        simulation_game_damage_read_unit_vector(packet, "epicenter-direction", &event_data->epicenter_direction);
        event_data->epicenter_distance = simulation_game_damage_read_quantized_real(packet, "epicenter-direction", minimum,
#if defined(_XBOX) && !defined(__clang__)
            k_real_constant_pool_three[93],
#else
            25.0f,
#endif
            12, false);
    } else {
#if defined(_XBOX) && !defined(__clang__)
        minimum = k_real_constant_pool_three[-1902];
#else
        minimum = 0.0f;
#endif
    }
#if defined(_XBOX) && !defined(__clang__)
    float maximum_scale = k_real_constant_pool_three[1];
#else
    float maximum_scale = 2.0f;
#endif
    event_data->scale = simulation_game_damage_read_quantized_real(packet, "scale", minimum, maximum_scale, 5, false);
    event_data->shake_scale = simulation_game_damage_read_quantized_real(packet, "shake-scale", minimum, maximum_scale, 5, false);
    event_data->flags = 0;
    if (packet->read_bool("shield-depleted-flag")) event_data->flags |= 2; else event_data->flags &= ~2;
    if (packet->read_bool("area-of-effect-flag")) event_data->flags |= 4; else event_data->flags &= ~4;
    if (packet->read_bool("any-damage-dealt-flag")) event_data->flags |= 8; else event_data->flags &= ~8;
    if (packet->read_bool("damaged-by-friendly-flag")) event_data->flags |= 16; else event_data->flags &= ~16;
    if (packet->read_bool("silent-flag")) event_data->flags |= 32; else event_data->flags &= ~32;
    if (packet->read_bool("killed-instantly-flag")) event_data->flags |= 64; else event_data->flags &= ~64;
    if (packet->read_bool("outside-aoe-dmg-range")) event_data->flags |= 256; else event_data->flags &= ~256;
    if (packet->read_bool("primary-recipient-bit")) event_data->flags |= 128; else event_data->flags &= ~128;
    if (packet->read_bool("unknown-flag")) event_data->flags |= 1024; else event_data->flags &= ~1024;
    event_data->special_death_type = packet->read_integer("special-death-type", 3);
    long special_death_type = (event_data->special_death_type > 0 ? event_data->special_death_type : 0) < 4
        ? (event_data->special_death_type > 0 ? event_data->special_death_type : 0)
        : 4;
#if defined(_XBOX) && !defined(__clang__)
    float maximum_body_damage = k_real_constant_pool_three[-1485];
#else
    float maximum_body_damage = 16.0f;
#endif
    event_data->special_death_type = special_death_type;
    event_data->body_damage = simulation_game_damage_read_quantized_real(packet, "body-damage", minimum, maximum_body_damage, 8, false);
    event_data->shield_damage = simulation_game_damage_read_quantized_real(packet, "shield-damage", minimum,
#if defined(_XBOX) && !defined(__clang__)
        k_real_constant_pool_three[0],
#else
        3.0f,
#endif
        6, false);
    event_data->body_part = (short)(packet->read_integer("body-part", 4) - 1);
    event_data->node_index = (short)(packet->read_integer("node-index", 8) - 1);
    event_data->ping_type = packet->read_integer("ping-type", 3);
    event_data->damage_reporting_info = (unsigned char)packet->read_integer("damage-reporting-info", 8);
    return true;
}
// @address 0x42bff8 ?event_payload_encode@c_simulation_damage_aftermath_event_definition@@UAAXJPBXPAVc_bitstream@@_N@Z
void c_simulation_damage_aftermath_event_definition::event_payload_encode(long payload_size, const void *payload, c_bitstream *packet, bool use_encoding)
{
    const s_simulation_damage_aftermath_event_data *event_data = (const s_simulation_damage_aftermath_event_data *)payload;
    simulation_write_definition_index("damage-definition-index", packet, event_data->damage_definition_index);
    packet->write_integer("damage-owner-player-index", event_data->damage_owner_player_index + 1, 5);
    packet->write_bool("direction-valid", event_data->direction_valid);
    if (event_data->direction_valid)
        packet->write_unit_vector("direction", &event_data->direction);
    packet->write_bool("epicenter-valid", event_data->epicenter_valid);
    float minimum;
    if (event_data->epicenter_valid)
    {
        minimum = k_dmg_zero;
        packet->write_unit_vector("epicenter-direction", &event_data->epicenter_direction);
        packet->write_quantized_real("epicenter-direction", event_data->epicenter_distance, minimum, k_dmg_25, 12, false, false);
    }
    else
    {
        minimum = k_dmg_zero;
    }
    float maximum_scale = k_dmg_2;
    packet->write_quantized_real("scale", event_data->scale, minimum, maximum_scale, 5, false, false);
    packet->write_quantized_real("shake-scale", event_data->shake_scale, minimum, maximum_scale, 5, false, false);
    packet->write_bool("shield-depleted-flag", (event_data->flags >> 1) & 1);
    packet->write_bool("area-of-effect-flag", (event_data->flags >> 2) & 1);
    packet->write_bool("any-damage-dealt-flag", (event_data->flags >> 3) & 1);
    packet->write_bool("damaged-by-friendly-flag", (event_data->flags >> 4) & 1);
    packet->write_bool("silent-flag", (event_data->flags >> 5) & 1);
    packet->write_bool("killed-instantly-flag", (event_data->flags >> 6) & 1);
    packet->write_bool("outside-aoe-dmg-range", (event_data->flags >> 8) & 1);
    packet->write_bool("primary-recipient-bit", (event_data->flags >> 7) & 1);
    packet->write_bool("unknown-flag", (event_data->flags >> 10) & 1);
    packet->write_integer("special-death-type", event_data->special_death_type, 3);
    float maximum_body_damage = k_dmg_16;
    packet->write_integer("body-damage", quantize_real(event_data->body_damage, minimum, maximum_body_damage, 8, false, true), 8);
    float maximum_shield_damage = k_dmg_3;
    packet->m_encoded_value_bits += 8;
    packet->write_quantized_real("shield-damage", event_data->shield_damage, minimum, maximum_shield_damage, 6, false, true);
    packet->write_integer("body-part", event_data->body_part + 1, 4);
    packet->write_integer("node-index", event_data->node_index + 1, 8);
    packet->write_integer("ping-type", event_data->ping_type, 3);
    packet->write_integer("damage-reporting-info", event_data->damage_reporting_info, 8);
}
// @address 0x1858b8 ?csnzprintf@@YAPADPADIPBDZZ
// @address 0xa31c8 ?dequantize_real@@YAMJMMJ_N@Z
// @address 0x8a1ecc ?g_active_structure_bsp_mask@@3KA
// @address 0x446e88 ?make_room_for_update@c_entity_update_encode_helper@@QAA_NPAVc_bitstream@@JJJK@Z
// @address 0x446800 ?simulation_calculate_entity_update_priority@@YAMPBUs_simulation_entity@@PBUs_entity_update_data@@PAMPAJ@Z
// @address 0x3af138 ?simulation_gamestate_entity_get_object_index@@YAJJ@Z
// @address 0x40ebd8 ?simulation_read_definition_index@@YAJPBDPAVc_bitstream@@@Z
// @address 0x3adbd8 ?simulation_write_quantized_position@@YAXPAVc_bitstream@@PBTreal_point3d@@J_NPBTreal_rectangle3d@@@Z
// @address 0x221228 ?write_bits_internal@c_bitstream@@AAAXPBEJ@Z
// @address 0x446ef8 ?write_component_header@c_entity_update_encode_helper@@QAA_NJPBD@Z
// @address 0x221648 ?read_bit_internal@c_bitstream@@AAA_NXZ
// @address 0x221768 ?read_value_internal@c_bitstream@@AAAKJ@Z
// @address 0x221480 ?write_value_internal@c_bitstream@@QAAXKJ@Z
// @address 0x446f80 ?finish_component@c_entity_update_encode_helper@@QAAXXZ

static __forceinline void *simulation_game_damage_object_header_block_get_with_count(
    long object_index, const object_datum *object, unsigned int element_size, long *count)
{
    *count = object->damage_section_bytes / element_size;
    return (char *)object + object->damage_section_offset;
}
static __forceinline void simulation_game_damage_section_response_fire_from_simulation_event(
    long object_index, long damage_section_index, long response_index, long response_type)
{
    const s_model_damage_info *damage_info = object_get_damage_info(object_index);
    if (damage_info)
    {
        s_damage_owner damage_owner = k_damage_owner_unknown;
        if (response_type == 1)
        {
            s_damage_section *section = &((s_damage_section *)damage_info->sections.address)[damage_section_index];
            s_instantaneous_damage_response *response = &((s_instantaneous_damage_response *)section->responses.address)[response_index];
            damage_spawn_effect_from_marker_name(
                response->delay_effect.index, object_index, &damage_owner, response->delay_effect_marker, TEST_BIT(response->flags, 20), false);
        }
        else
        {
            s_damage_response_data context;
            context.section_ref = 0;
            memset(&context.definition, 0, sizeof(context) - sizeof(context.section_ref));
            damage_handle_section_response(damage_info, object_index, &damage_owner, 0, damage_section_index, response_index, &context);
        }
    }
}
// @address 0x42ca28 ?apply_game_event@c_simulation_damage_section_response_event_definition@@UAA_NJPBJJPBX@Z
bool c_simulation_damage_section_response_event_definition::apply_game_event(
    long reference_gamestate_count, long const *gamestate_indices, long payload_size, void const *payload)
{
    bool result = false;
    const s_simulation_damage_section_response_event_data *data = (const s_simulation_damage_section_response_event_data *)payload;
    long object_index = -1;
    if (gamestate_indices[0] != -1)
        object_index = simulation_gamestate_entity_get_object_index(gamestate_indices[0]);
    if (object_index != -1)
    {
        object_datum *object = object_get(object_index);
        s_object_definition *object_definition = TAG_GET('obje', s_object_definition, object->definition_index);
        if (object_definition->model.index != -1)
        {
            long damage_section_count = 0;
            s_damage_object_section_state *damage_sections =
                (s_damage_object_section_state *)simulation_game_damage_object_header_block_get_with_count(
                    object_index, object, sizeof(s_damage_object_section_state), &damage_section_count);
            if (data->damage_section_index >= 0 && data->damage_section_index < damage_section_count)
            {
                s_damage_section *section = 0;
                s_model_definition *model_definition = TAG_GET('mode', s_model_definition, object_definition->model.index);
                s_model_damage_info *damage_info = (s_model_damage_info *)model_definition->damage_info.address;
                if (model_definition->damage_info.count > 0)
                {
                    section = &((s_damage_section *)damage_info->sections.address)[data->damage_section_index];
                    if (data->response_index >= 0 && data->response_index < section->responses.count)
                    {
                        if (data->response_type >= 0 && data->response_type < 2)
                        {
                            s_damage_object_section_state *damage_section = &damage_sections[data->damage_section_index];
                            s_instantaneous_damage_response *response = &((s_instantaneous_damage_response *)section->responses.address)[data->response_index];
                            if (!TEST_BIT(damage_section->flags, data->response_index))
                            {
                                if (data->response_type != 1 || response->delay_effect.index != -1)
                                {
                                    simulation_game_damage_section_response_fire_from_simulation_event(
                                        object_index, data->damage_section_index, data->response_index, data->response_type);
                                    result = true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return result;
}

static __forceinline player_datum *simulation_game_damage_try_player(s_data_array *data, long absolute_index)
{
    player_datum *result = 0;
    if (absolute_index >= 0 && absolute_index < data->first_unallocated)
    {
        player_datum *player =
            (player_datum *)(data->data + absolute_index * data->size);
        if (player->identifier != 0)
            result = player;
    }
    return result;
}

// @address 0x42c5f8 ?apply_game_event@c_simulation_damage_aftermath_event_definition@@UAA_NJPBJJPBX@Z
bool c_simulation_damage_aftermath_event_definition::apply_game_event(long reference_gamestate_count,
    const long *gamestate_indices, long payload_size, const void *payload)
{
    const s_simulation_damage_aftermath_event_data *event_data =
        (const s_simulation_damage_aftermath_event_data *)payload;
    long object_index = -1;
    long second_object_index = -1;
    bool result = false;
    if (gamestate_indices[0] != -1)
        object_index = simulation_gamestate_entity_get_object_index(gamestate_indices[0]);
    if (gamestate_indices[1] != -1)
        second_object_index = simulation_gamestate_entity_get_object_index(gamestate_indices[1]);
    if (object_index != -1)
    {
        long owner_player_index = -1;
        long team = -1;
        long absolute_player_index = event_data->damage_owner_player_index;
        if (absolute_player_index != -1)
        {
            player_datum *player =
                simulation_game_damage_try_player(tls_player_data(), absolute_player_index);
            if (player)
            {
                team = player->configuration.host.team;
                owner_player_index = ((long)player->identifier << 16) | absolute_player_index;
            }
        }
        if (tag_try_and_get_unsafe(0x6a707421, event_data->damage_definition_index) != 0)
        {
            real_point3d origin;
            s_damage_aftermath_result_data result_data;
            memset(&result_data, 0, sizeof(result_data));
            result_data.owner.set(second_object_index, owner_player_index, team);
            result_data.damage_definition_index = event_data->damage_definition_index;
            if (event_data->direction_valid)
                result_data.direction = event_data->direction;
            result_data.flags = event_data->flags;
            result_data.scale = event_data->scale;
            result_data.shake_scale = event_data->shake_scale;
            result_data.ping_type = event_data->ping_type;
            result_data.body_part = event_data->body_part;
            result_data.node_index = event_data->node_index;
            float body_damage = event_data->body_damage;
            float shield_damage = event_data->shield_damage;
            result_data.body_damage = body_damage;
            result_data.shield_damage = shield_damage;
            result_data.reporting_info = event_data->damage_reporting_info;
            object_get_origin(object_index, &origin);
            if (event_data->epicenter_valid)
            {
                real_vector3d offset;
                scale_vector3d(&event_data->epicenter_direction, event_data->epicenter_distance, &offset);
                result_data.epicenter.x = offset.i + origin.x;
                result_data.epicenter.y = offset.j + origin.y;
                result_data.epicenter.z = offset.k + origin.z;
            }
            else
                result_data.epicenter = origin;
            float zero = 0.0f;
            result_data.distance = zero;
            if (TEST_BIT((long)result_data.flags, 1))
                object_set_shield_depleted(object_index, true, damage_owner_unknown());
            if (((1 << object_get_type(object_index)) & 0x1003) != 0 && (shield_damage > zero || body_damage > zero))
                ((unit_datum *)object_get_and_verify_type(object_index, NONE))->unit.last_damage_time = tls_game_time_globals()->time;
            object_apply_damage_aftermath(object_index, &result_data);
            if (((1 << object_get_type(object_index)) & 1) != 0 && event_data->special_death_type != 0)
            {
                ((unit_datum *)object_get_and_verify_type(object_index, NONE))->unit.death_state =
                    (unsigned char)event_data->special_death_type;
                biped_died(object_index, event_data->node_index, &event_data->direction, false, true);
            }
        }
        result = true;
    }
    return result;
}

typedef char simulation_breakable_surface_group_data_size[(sizeof(s_simulation_breakable_surface_group_state_data) == 4 &&
    sizeof(s_simulation_breakable_surface_group_creation_data) == 2) ? 1 : -1];

// @address 0x42e628 ?entity_type@c_simulation_breakable_surface_group_entity_definition@@UAA?AW4e_simulation_entity_type@@XZ
e_simulation_entity_type c_simulation_breakable_surface_group_entity_definition::entity_type()
{
    return _simulation_entity_type_breakable_surface_group;
}

// @address 0x3fc550 ?state_data_size@c_simulation_breakable_surface_group_entity_definition@@UAAJXZ
long c_simulation_breakable_surface_group_entity_definition::state_data_size()
{
    return 4;
}

// @address 0x3fc598 ?creation_data_size@c_simulation_breakable_surface_group_entity_definition@@UAAJXZ
long c_simulation_breakable_surface_group_entity_definition::creation_data_size()
{
    return 2;
}

// @address 0x42d380 ?update_flag_count@c_simulation_breakable_surface_group_entity_definition@@UAAJXZ
long c_simulation_breakable_surface_group_entity_definition::update_flag_count()
{
    return 1;
}

// @address 0x42d380 ?initial_update_mask@c_simulation_breakable_surface_group_entity_definition@@UAAKXZ
unsigned long c_simulation_breakable_surface_group_entity_definition::initial_update_mask()
{
    return 1;
}

// @address 0x42d380 ?entity_replication_required_for_view_activation@c_simulation_breakable_surface_group_entity_definition@@UAA_NPBUs_simulation_entity@@@Z
bool c_simulation_breakable_surface_group_entity_definition::entity_replication_required_for_view_activation(const s_simulation_entity *entity)
{
    return true;
}

// @address 0x42d298 ?entity_type_is_gameworld_object@c_simulation_breakable_surface_group_entity_definition@@UAA_NXZ
bool c_simulation_breakable_surface_group_entity_definition::entity_type_is_gameworld_object()
{
    return false;
}

// @address 0x42d380 ?entity_can_be_created@c_simulation_breakable_surface_group_entity_definition@@UAA_NPBUs_simulation_entity@@PBUs_simulation_view_telemetry_data@@@Z
bool c_simulation_breakable_surface_group_entity_definition::entity_can_be_created(const s_simulation_entity *entity,
    const s_simulation_view_telemetry_data *telemetry)
{
    return true;
}

// @address 0x42e748 ?creation_minimum_required_bits@c_simulation_breakable_surface_group_entity_definition@@UAAJPBUs_simulation_entity@@PBUs_simulation_view_telemetry_data@@@Z
long c_simulation_breakable_surface_group_entity_definition::creation_minimum_required_bits(const s_simulation_entity *entity,
    const s_simulation_view_telemetry_data *telemetry)
{
    return 16;
}

// @address 0x429c40 ?update_minimum_required_bits@c_simulation_breakable_surface_group_entity_definition@@UAAJPBUs_simulation_entity@@PBUs_entity_update_data@@@Z
long c_simulation_breakable_surface_group_entity_definition::update_minimum_required_bits(const s_simulation_entity *entity, const s_entity_update_data *update)
{
    return 33;
}

// @address 0x42d380 ?entity_state_lossy_compare@c_simulation_breakable_surface_group_entity_definition@@UAA_NPAX0J@Z
bool c_simulation_breakable_surface_group_entity_definition::entity_state_lossy_compare(void *first, void *second, long size)
{
    return true;
}

// @address 0x42d380 ?entity_creation_lossy_compare@c_simulation_breakable_surface_group_entity_definition@@UAA_NPAX0J@Z
bool c_simulation_breakable_surface_group_entity_definition::entity_creation_lossy_compare(void *first, void *second, long size)
{
    return true;
}

// @address 0x5bec98 ?rotate_entity_indices@c_simulation_breakable_surface_group_entity_definition@@UAAXPBUs_simulation_entity@@@Z
void c_simulation_breakable_surface_group_entity_definition::rotate_entity_indices(const s_simulation_entity *entity)
{
}

// @address 0x5bec98 ?prepare_creation_data_for_gameworld@c_simulation_breakable_surface_group_entity_definition@@UAAXJPAX@Z
void c_simulation_breakable_surface_group_entity_definition::prepare_creation_data_for_gameworld(long size, void *creation_data)
{
}

// @address 0x5bec98 ?prepare_state_data_for_gameworld@c_simulation_breakable_surface_group_entity_definition@@UAAXKJPAX@Z
void c_simulation_breakable_surface_group_entity_definition::prepare_state_data_for_gameworld(unsigned long update_mask, long size, void *state)
{
}


// @address 0x42d210 ?delete_game_entity@c_simulation_breakable_surface_group_entity_definition@@UAA_NJ@Z
bool c_simulation_breakable_surface_group_entity_definition::delete_game_entity(long gamestate_index)
{
    bool result = false;
    if (gameworld_attachment_valid(gamestate_index))
        result = true;
    return result;
}

// @address 0x42d210 ?promote_game_entity_to_authority@c_simulation_breakable_surface_group_entity_definition@@UAA_NJ@Z
bool c_simulation_breakable_surface_group_entity_definition::promote_game_entity_to_authority(long gamestate_index)
{
    bool result = false;
    if (gameworld_attachment_valid(gamestate_index))
        result = true;
    return result;
}
