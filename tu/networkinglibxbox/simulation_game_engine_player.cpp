// networkinglibxbox/simulation_game_engine_player.obj: from the records 0x429c18 0x429c20 0x429c30 0x429c40 0x429c48 0x429c58 0x429ca8 0x429cb8 0x429cc0 0x429d30 0x429d80 0x429df0 0x429f90 0x42a010 0x42a048 0x42a060 0x42a0c0 0x42a0e8 0x42a168 0x42a1e8 0x42a290 0x42a360 0x42a858 0x42abe0 0x42ac60 0x42acc0 0x42ad20 0x42ad88 0x42ae68 0x42ae70 0x42aef8 0x42af58 0x42b3c0 0x42b768 0x709ee8 0x70c790
// @flags /TP /O2 /Oc
#include "simulation/simulation_queue_event_types.h"
#include "networking/simulation/simulation_entity_definition.h"
#include "memory/tls_globals.h"
#include <string.h>
#include "networking/simulation/simulation_entity_database.h"
#include "math/real_math_types.h"
#include "cseries/cseries_wrapped.h"
#include "memory/data_types.h"
#include "game/players.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "game/game_engine_simulation.h"
#include "game/game_engine_util.h"
#include "simulation/simulation_encoding.h"
#include "game/game_engine_player_traits.h"
#include "render/render_lens_flares.h"
#include "memory/bitstream.h"
#include "game/map_variant.h"
#include "simulation/game_interface/simulation_game_engine_player.h"
#include "simulation/simulation_priority.h"
#include "game/player_types.h"
#include "math/real_math_constants.h"
#include "simulation/simulation_entity_types.h"
#include "simulation/simulation_game_entity_definitions.h"
#include "simulation/game_interface/simulation_game_internal.h"
// @address 0x221fc k_real_constant_pool
// @address 0x42abe0 simulation_game_engine_player_index_from_gamestate_index

#include "game/game_engine_default.h"
#include "game/game_engine_event_data.h"
#include "game/game_engine_scoring_globals.h"
#include "networking/replication/replication_event.h"
#include "game/player_traits.h"
#include "game/game_engine_list.h"
#include "simulation/game_interface/simulation_game_engine_player_types.h"

// match: 0x42af58, 0x42b3c0: named bounds preserve the shared 1.0f constant-pool base.
// @rdata 0x8202251c pooled
static const float k_player_editor_minimum_distance = 0.0f;
// @rdata 0x82022ba4 pooled
static const float k_player_editor_maximum_boundary = 100.0f;

static __forceinline c_game_engine *current_engine(void)
{
    c_game_engine *result;
    if (tls_game_engine_globals()->game_engine_index >= 1 && tls_game_engine_globals()->game_engine_index <= 10)
        result = game_engines[tls_game_engine_globals()->game_engine_index];
    else
        result = 0;
    return result;
}
static __forceinline int engine_slot(short index)
{
    int result = -1;
    if (current_engine() != 0)
        result = tls_game_engine_globals()->player_gamestate_indices[index];
    return result;
}






static __forceinline void game_engine_globals_build_player_baseline(short player_index, long state_data_size, void *state_data)
{
    c_game_engine *engine = current_engine();
    engine->build_player_baseline(player_index, state_data_size, state_data);
}
static __forceinline void game_engine_globals_build_player_update(short player_index, unsigned long *update_mask, long state_data_size, void *state_data)
{
    c_game_engine *engine = current_engine();
    engine->build_player_update(player_index, update_mask, state_data_size, state_data);
}
static __forceinline bool game_engine_globals_apply_player_update(short player_index, unsigned long update_mask, long state_data_size, const void *state_data)
{
    c_game_engine *engine = current_engine();
    bool result = engine->apply_player_update(player_index, update_mask, state_data_size, state_data);
    return result;
}
static __forceinline player_datum *editor_player_try_and_get_absolute(long player_index)
{
    player_datum *result = 0;
    s_data_array *players = tls_player_data();
    if (player_index != -1) {
        if (player_index >= 0 && player_index < players->first_unallocated) {
            player_datum *player = (player_datum *)(players->data + player_index * players->size);
            if (player->identifier != 0)
                result = player;
        }
    }
    return result;
}

namespace {
static __forceinline player_datum *simulation_game_engine_player_respawn_request_player_try_and_get_absolute(
    const s_data_array *array, long absolute_index)
{
    player_datum *result = 0;
    if (absolute_index != -1)
    {
        if (absolute_index >= 0 && absolute_index < array->first_unallocated)
        {
            player_datum *player = (player_datum *)(array->data + absolute_index * array->size);
            if (player->identifier != 0)
                result = player;
        }
    }
    return result;
}
static __forceinline long simulation_game_engine_player_respawn_request_datum_absolute_index_to_index(const s_data_array *array, long absolute_index)
{
    long result = -1;
    if (absolute_index != -1)
    {
        const player_datum *player = (const player_datum *)(array->data + array->size * absolute_index);
        result = (player->identifier << 16) | absolute_index;
    }
    return result;
}
static __forceinline long simulation_game_engine_player_respawn_request_player_index_from_absolute_player_index(long absolute_player_index)
{
    return simulation_game_engine_player_respawn_request_datum_absolute_index_to_index(tls_player_data(), absolute_player_index);
}
}

static __forceinline void game_engine_globals_set_player_gamestate_index(short player_index, long gamestate_index)
{
    tls_game_engine_globals()->player_gamestate_indices[player_index] = gamestate_index;
}

// @address 0x429c18 ?event_type@c_simulation_player_respawn_request_event_definition@@UAA?AW4e_simulation_event_type@@XZ
// @address 0x429c20 ?event_type_name@c_simulation_player_respawn_request_event_definition@@UAAPBDXZ
// @address 0x429c48 ?event_type_name@c_simulation_player_editor_request_event_definition@@UAAPBDXZ
// @address 0x221228 ?write_bits_internal@c_bitstream@@AAAXPBEJ@Z
// @address 0x221398 ?write_bit_internal@c_bitstream@@AAAX_N@Z
// @address 0x221480 ?write_value_internal@c_bitstream@@QAAXKJ@Z
// @address 0x265bc8 ?encode@c_player_trait_shield_vitality@@QBAXPAVc_bitstream@@@Z
// @address 0x266110 ?encode@c_player_trait_weapons@@QBAXPAVc_bitstream@@@Z
// @address 0x446e88 ?make_room_for_update@c_entity_update_encode_helper@@QAA_NPAVc_bitstream@@JJJK@Z
// @address 0x446ef8 ?write_component_header@c_entity_update_encode_helper@@QAA_NJPBD@Z
// @address 0x446f80 ?finish_component@c_entity_update_encode_helper@@QAAXXZ
// @address 0x446800 ?simulation_calculate_entity_update_priority@@YAMPBUs_simulation_entity@@PBUs_entity_update_data@@PAMPAJ@Z
// @address 0x1858b8 ?csnzprintf@@YAPADPADIPBDZZ
// @address 0xa0a10 ?current_game_engine@@YAPBVc_game_engine@@XZ
// @address 0x29f860 ?game_engine_globals_get_player_gamestate_index@@YAJF@Z
// @address 0x221768 ?read_value_internal@c_bitstream@@AAAKJ@Z
// @address 0x42ad20 ?event_payload_decode@c_simulation_player_respawn_request_event_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
// @address 0x42ae68 ?payload_size@c_simulation_player_editor_request_event_definition@@UAAJXZ

// @address 0x1d9b70 ?player_try_and_get_player_index_from_absolute_player_index@@YAJJ@Z
// @address 0x2212e0 ?read_bits_internal@c_bitstream@@AAAXPAEJ@Z
// @address 0x221648 ?read_bit_internal@c_bitstream@@AAA_NXZ
// @address 0x265c30 ?decode@c_player_trait_shield_vitality@@QAAXPAVc_bitstream@@@Z
// @address 0x266198 player_trait_weapons_decode
// @address 0x266610 ?decode@c_player_trait_movement@@QAAXPAVc_bitstream@@@Z
// @address 0x266780 ?decode@c_player_trait_appearance@@QAAXPAVc_bitstream@@@Z
// @address 0x2668a0 ?decode@c_player_trait_sensors@@QAAXPAVc_bitstream@@@Z
// @address 0x2afc18 bitstream_decode_unit_vector
// @address 0x42ac60 ?write_description_to_string@c_simulation_player_respawn_request_event_definition@@UAAXPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@MJPAD@Z
// @address 0x42acc0 ?event_payload_encode@c_simulation_player_respawn_request_event_definition@@UAAXJPBXPAVc_bitstream@@_N@Z
// @address 0x42aef8 ?write_description_to_string@c_simulation_player_editor_request_event_definition@@UAAXPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@MJPAD@Z
// @address 0x42b768 ?apply_game_event@c_simulation_player_editor_request_event_definition@@UAA_NJPBJJPBX@Z
e_simulation_event_type c_simulation_player_respawn_request_event_definition::event_type()
{
    return _simulation_event_type_player_respawn_request;
}
const char *c_simulation_player_respawn_request_event_definition::event_type_name()
{
    return "player_respawn_request";
}
const char *c_simulation_player_editor_request_event_definition::event_type_name()
{
    return "player_editor_request";
}
// @address 0x429ca8 ?entity_type_name@c_simulation_game_engine_player_entity_definition@@UAAPBDXZ
const char *c_simulation_game_engine_player_entity_definition::entity_type_name()
{
    return "game-engine-player";
}
// @address 0x429cb8 ?state_data_size@c_simulation_game_engine_player_entity_definition@@UAAJXZ
long c_simulation_game_engine_player_entity_definition::state_data_size()
{
    return 92;
}
// @address 0x429cc0 ?promote_game_entity_to_authority@c_simulation_game_engine_player_entity_definition@@UAA_NJ@Z
bool c_simulation_game_engine_player_entity_definition::promote_game_entity_to_authority(long gamestate_index)
{
    bool result = false;
    long player_index = -1;
    player_index = simulation_game_engine_player_index_from_gamestate_index(gamestate_index);
    if (player_index != -1) {
        if (gameworld_attachment_valid(gamestate_index)) {
            result = true;
        }
    }
    return result;
}
// @address 0x429d30 ?gameworld_attachment_valid@c_simulation_game_engine_player_entity_definition@@UAA_NJ@Z
bool c_simulation_game_engine_player_entity_definition::gameworld_attachment_valid(long gamestate_index)
{
    bool result = false;
    long player_index = -1;
    player_index = simulation_game_engine_player_index_from_gamestate_index(gamestate_index);
    if (player_index != -1) {
        long player_gamestate_index = game_engine_globals_get_player_gamestate_index((short)player_index);
        if (player_gamestate_index == gamestate_index)
            result = true;

    }
    return result;
}
// @address 0x429df0 ?update_minimum_required_bits@c_simulation_game_engine_player_entity_definition@@UAAJPBUs_simulation_entity@@PBUs_entity_update_data@@@Z
long c_simulation_game_engine_player_entity_definition::update_minimum_required_bits(const s_simulation_entity *entity,
    const s_entity_update_data *update)
{
    const unsigned long *flags = &update->update_mask;

    int result = 0x7fffffff;
    if (*flags & 0x1)
        result = 45;
    if ((*flags & 0x2) && result >= 33)
        result = 33;
    if ((*flags & 0x4) && result >= 45)
        result = 45;
    if ((*flags & 0x8) && result >= 26)
        result = 26;
    if ((*flags & 0x10) && result >= 29)
        result = 29;
    if ((*flags & 0x20) && result >= 23)
        result = 23;
    if ((*flags & 0x40) && result >= 20)
        result = 20;
    if ((*flags & 0x80) && result >= 18)
        result = 18;
    if ((*flags & 0x100) && result >= 66)
        result = 66;
    if ((*flags & 0x200) && result >= 24)
        result = 24;
    if ((*flags & 0x400) && result >= 22)
        result = 22;
    if ((*flags & 0x800) && result >= 27)
        result = 27;
    if ((*flags & 0x1000) && result >= 18)
        result = 18;
    if ((*flags & 0x2000) && result >= 18)
        result = 18;
    if ((*flags & 0x4000) && result >= 18)
        result = 18;
    if ((*flags & 0x8000) && result >= 34)
        result = 34;
    if (((unsigned int)(*flags & 0x10000)) == 0)
        return result;
    if (result < 36)
        return result;
    return 36;
}
// @address 0x429f90 ?write_update_description_to_string@c_simulation_game_engine_player_entity_definition@@UAAXPBUs_simulation_entity@@PBUs_entity_update_data@@JPAD@Z
void c_simulation_game_engine_player_entity_definition::write_update_description_to_string(
    const s_simulation_entity *entity, const s_entity_update_data *update_data, long buffer_size, char *buffer)
{
    float relevance = 0.0f;
    long period = 0;
    simulation_calculate_entity_update_priority(entity, update_data, &relevance, &period);
    csnzprintf(buffer, buffer_size, "player update: relevance=%5.3f: period=%d", relevance, period);
}

// @address 0x42a010 ?build_creation_data@c_simulation_game_engine_player_entity_definition@@UAAXJJPAX@Z
void c_simulation_game_engine_player_entity_definition::build_creation_data(
    long gamestate_index, long creation_data_size, void *creation_data)
{
    s_simulation_game_engine_player_creation_data *data = (s_simulation_game_engine_player_creation_data *)creation_data;
    data->absolute_player_index = (short)simulation_game_engine_player_index_from_gamestate_index(gamestate_index);
}
// @address 0x42a048 ?entity_creation_encode@c_simulation_game_engine_player_entity_definition@@UAAXJPBXPBUs_simulation_view_telemetry_data@@PAVc_bitstream@@_N@Z
void c_simulation_game_engine_player_entity_definition::entity_creation_encode(
    long creation_data_size, const void *creation_data, const s_simulation_view_telemetry_data *telemetry,
    c_bitstream *packet, bool encoding_is_for_debug)
{
    const s_simulation_game_engine_player_creation_data *data = (const s_simulation_game_engine_player_creation_data *)creation_data;
    packet->write_integer("absolute-player-index", data->absolute_player_index, 5);
}
// @address 0x42a060 ?entity_creation_decode@c_simulation_game_engine_player_entity_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
bool c_simulation_game_engine_player_entity_definition::entity_creation_decode(
    long creation_data_size, void *creation_data, c_bitstream *packet, bool decoding_is_for_debug)
{
    s_simulation_game_engine_player_creation_data *data = (s_simulation_game_engine_player_creation_data *)creation_data;
    data->absolute_player_index = (short)packet->read_integer("absolute-player-index", 5);
    bool result = data->absolute_player_index >= 0 && data->absolute_player_index < 16;
    return result;
}
// @address 0x42a0c0 ?create_game_entity@c_simulation_game_engine_player_entity_definition@@UAA_NJJPBXKJ0@Z
bool c_simulation_game_engine_player_entity_definition::create_game_entity(
    long gamestate_index, long creation_data_size, const void *creation_data,
    unsigned long initial_update_mask, long state_data_size, const void *state_data)
{
    const s_simulation_game_engine_player_creation_data *data = (const s_simulation_game_engine_player_creation_data *)creation_data;
    short player_index = data->absolute_player_index;
    game_engine_globals_set_player_gamestate_index(player_index, gamestate_index);
    return true;
}

// @address 0x42a0e8 ?delete_game_entity@c_simulation_game_engine_player_entity_definition@@UAA_NJ@Z
bool c_simulation_game_engine_player_entity_definition::delete_game_entity(long gamestate_index)
{
    bool result = false;
    long player_index = -1;
    player_index = simulation_game_engine_player_index_from_gamestate_index(gamestate_index);
    if (player_index != -1) {
        if (gameworld_attachment_valid(gamestate_index)) {
            game_engine_globals_set_player_gamestate_index((short)player_index, -1);
            result = true;
        }
    }
    return result;
}
// @address 0x42a168 ?build_baseline_state_data@c_simulation_game_engine_player_entity_definition@@UAA_NJPBXJPAX@Z
bool c_simulation_game_engine_player_entity_definition::build_baseline_state_data(
    long creation_data_size, const void *creation_data, long state_data_size, void *state_data)
{
    const s_simulation_game_engine_player_creation_data *data = (const s_simulation_game_engine_player_creation_data *)creation_data;
    bool result = true;
    game_engine_globals_build_player_baseline(data->absolute_player_index, state_data_size, state_data);
    return result;
}
// @address 0x42a1e8 ?build_updated_state_data@c_simulation_game_engine_player_entity_definition@@UAA_NPBUs_simulation_entity@@PAKJPAX@Z
bool c_simulation_game_engine_player_entity_definition::build_updated_state_data(
    const s_simulation_entity *entity, unsigned long *update_mask, long state_data_size, void *state_data)
{
    bool result = false;
    long player_index = simulation_game_engine_player_index_from_gamestate_index(entity->gamestate_index);
    if (player_index != -1) {
        game_engine_globals_build_player_update((short)player_index, update_mask, state_data_size, state_data);
        result = true;
    }
    return result;
}
// @address 0x42a290 ?update_game_entity@c_simulation_game_engine_player_entity_definition@@UAA_NJKJPBX@Z
bool c_simulation_game_engine_player_entity_definition::update_game_entity(
    long gamestate_index, unsigned long update_mask, long state_data_size, const void *state_data)
{
    bool result = false;
    long player_index = simulation_game_engine_player_index_from_gamestate_index(gamestate_index);
    if (player_index != -1) {
        if (gameworld_attachment_valid(gamestate_index) &&
            game_engine_globals_apply_player_update((short)player_index, update_mask, state_data_size, state_data)) {
            result = true;
        }
    }
    return result;
}
namespace {
static __forceinline void simulation_game_engine_player_encode_movement(const c_player_trait_movement *traits, c_bitstream *packet)
{
    packet->write_integer("player-traits-movement-speed", traits->m_speed, 4);
    packet->write_integer("player-traits-movement-gravity", traits->m_gravity, 3);
    packet->write_integer("player-traits-movement-vehicle-usage", traits->m_vehicle_usage, 2);
}
static __forceinline void simulation_game_engine_player_encode_appearance(const c_player_trait_appearance *traits, c_bitstream *packet)
{
    packet->write_integer("player-traits-appearance-active-camo", traits->m_active_camo, 3);
    packet->write_integer("player-traits-appearance-waypoint", traits->m_waypoint, 2);
    packet->write_integer("player-traits-appearance-aura", traits->m_aura, 3);
    packet->write_integer("player-traits-appearance-forced-change-color", traits->m_forced_change_color, 4);
}
static __forceinline void simulation_game_engine_player_encode_sensors(const c_player_trait_sensors *traits, c_bitstream *packet)
{
    packet->write_integer("player-traits-sensors-motion-tracker", traits->m_motion_tracker, 3);
    packet->write_integer("player-traits-sensors-motion-tracker-range", traits->m_motion_tracker_range, 3);
}
}
// 0x42a360
// @address 0x42a360 ?entity_update_encode@c_simulation_game_engine_player_entity_definition@@UAA_N_NKPAKJPBXPBUs_simulation_view_telemetry_data@@PAVc_bitstream@@J0@Z
bool c_simulation_game_engine_player_entity_definition::entity_update_encode(bool initial_update, unsigned long update_mask,
    unsigned long *written_update_mask, long state_data_size, const void *state_data,
    const s_simulation_view_telemetry_data *telemetry, c_bitstream *packet, long maximum_bits, bool final_update)
{
    const s_simulation_game_engine_player_state_data *data = (const s_simulation_game_engine_player_state_data *)state_data;
    bool result = false;
    c_entity_update_encode_helper helper;
    if (helper.make_room_for_update(packet, maximum_bits, 0, 17, update_mask))
    {
        if (helper.write_component_header(0, "respawn-timer-exists"))
        {
            packet->write_integer("respawn-timer", data->respawn_timer_ticks, 16);
            packet->write_raw_data("dead-location", &data->dead_location, 96);
        }
        helper.finish_component();
        if (helper.write_component_header(1, "shield-traits-exist"))
            data->traits.m_shield_vitality_traits.encode(packet);
        helper.finish_component();
        if (helper.write_component_header(2, "weapon-traits-exist"))
            data->traits.m_weapon_traits.encode(packet);
        helper.finish_component();
        if (helper.write_component_header(3, "movement-traits-exist"))
            simulation_game_engine_player_encode_movement(&data->traits.m_movement_traits, packet);
        helper.finish_component();
        if (helper.write_component_header(4, "appearance-traits-exist"))
            simulation_game_engine_player_encode_appearance(&data->traits.m_appearance_traits, packet);
        helper.finish_component();
        if (helper.write_component_header(5, "sensor-traits-exist"))
            simulation_game_engine_player_encode_sensors(&data->traits.m_sensor_traits, packet);
        helper.finish_component();
        if (helper.write_component_header(6, "waypoint-action-exists"))
            packet->write_integer("waypoint-action", data->waypoint_action, 3);
        helper.finish_component();
        if (helper.write_component_header(7, "blocking-teleporter-exists"))
            packet->write_bool("blocking-teleporter", data->blocking_teleporter);
        helper.finish_component();
        if (helper.write_component_header(8, "netdebug-exists"))
        {
            packet->write_bool("is-host", data->is_host);
            if (data->is_host)
            {
                packet->write_integer("host-estimated-bps", data->host_estimated_bps, 16);
                packet->write_integer("host-transmission-rate", data->host_transmission_rate, 16);
                packet->write_integer("host-transmission-bps", data->host_transmission_bps, 16);
            }
            packet->write_integer("client-rtt-msec", data->client_rtt_msec, 16);
            packet->write_integer("client-packet-rate", data->client_packet_rate, 16);
            packet->write_integer("client-bandwidth-bps", data->client_bandwidth_bps, 16);
            packet->write_integer("client-packet-loss", data->client_packet_loss, 7);
        }
        helper.finish_component();
        if (helper.write_component_header(9, "lives-remaining-exists"))
            packet->write_integer("lives-remaining", data->lives_remaining + 1, 7);
        helper.finish_component();
        if (helper.write_component_header(10, "last-betrayer-exists"))
            packet->write_integer("last-betrayer", data->last_betrayer + 1, 5);
        helper.finish_component();
        if (helper.write_component_header(11, "respawn-timer-exists"))
            packet->write_integer("respawn-timer", data->respawn_timer, 10);
        helper.finish_component();
        if (helper.write_component_header(12, "vehicle-entrance-ban-exists"))
            packet->write_bool("vehicle-entrance-ban", data->vehicle_entrance_ban);
        helper.finish_component();
        if (helper.write_component_header(13, "active-in-game-exists"))
            packet->write_bool("active-in-game", data->active_in_game);
        helper.finish_component();
        if (helper.write_component_header(14, "sitting-out-exists"))
            packet->write_bool("sitting-out", data->sitting_out);
        helper.finish_component();
        if (helper.write_component_header(15, "desired-respawn-player-exists"))
            packet->write_integer("desired-respawn-player",
                (data->desired_respawn_player == -1 ? -1 : (unsigned short)data->desired_respawn_player) + 1, 16);
        helper.finish_component();
        if (helper.write_component_header(16, "player-control-aiming-exists"))
            packet->write_unit_vector("player-control-aiming", &data->player_control_aiming);
        helper.finish_component();
        helper.finish_update(written_update_mask);
        result = true;
    }
    return result;
}
// 0x42a858
// @address 0x42a858 ?entity_update_decode@c_simulation_game_engine_player_entity_definition@@UAA_N_NPAKJPAXPAVc_bitstream@@0@Z
bool c_simulation_game_engine_player_entity_definition::entity_update_decode(bool initial_update, unsigned long *update_mask,
    long state_data_size, void *state_data, c_bitstream *packet, bool final_update)
{
    s_simulation_game_engine_player_state_data *data = (s_simulation_game_engine_player_state_data *)state_data;
    unsigned long mask = 0;
    if (packet->read_bool("respawn-timer-exists"))
    {
        data->respawn_timer_ticks = packet->read_integer("respawn-timer", 16);
        packet->read_raw_data("dead-location", &data->dead_location, 96);
        mask |= 1;
    }
    if (packet->read_bool("shield-traits-exist"))
    {
        data->traits.m_shield_vitality_traits.decode(packet);
        mask |= 2;
    }
    if (packet->read_bool("weapon-traits-exist"))
    {
        player_trait_weapons_decode(&data->traits.m_weapon_traits, packet);
        mask |= 4;
    }
    if (packet->read_bool("movement-traits-exist"))
    {
        data->traits.m_movement_traits.decode(packet);
        mask |= 8;
    }
    if (packet->read_bool("appearance-traits-exist"))
    {
        data->traits.m_appearance_traits.decode(packet);
        mask |= 16;
    }
    if (packet->read_bool("sensor-traits-exist"))
    {
        data->traits.m_sensor_traits.decode(packet);
        mask |= 32;
    }
    if (packet->read_bool("waypoint-action-exists"))
    {
        data->waypoint_action = (signed char)packet->read_integer("waypoint-action", 3);
        mask |= 64;
    }
    if (packet->read_bool("blocking-teleporter-exists"))
    {
        data->blocking_teleporter = packet->read_bool("blocking-teleporter");
        mask |= 128;
    }
    if (packet->read_bool("netdebug-exists"))
    {
        long *netdebug_words = (long *)&data->is_host;
        for (long netdebug_index = 0; netdebug_index < 4; netdebug_index++)
            netdebug_words[netdebug_index] = 0;
        data->is_host = packet->read_bool("is-host");
        if (data->is_host)
        {
            data->host_estimated_bps = (unsigned short)packet->read_integer("host-estimated-bps", 16);
            data->host_transmission_rate = (unsigned short)packet->read_integer("host-transmission-rate", 16);
            data->host_transmission_bps = (unsigned short)packet->read_integer("host-transmission-bps", 16);
        }
        data->client_rtt_msec = (unsigned short)packet->read_integer("client-rtt-msec", 16);
        data->client_packet_rate = (unsigned short)packet->read_integer("client-packet-rate", 16);
        data->client_bandwidth_bps = (unsigned short)packet->read_integer("client-bandwidth-bps", 16);
        data->client_packet_loss = (unsigned short)packet->read_integer("client-packet-loss", 7);
        mask |= 256;
    }
    if (packet->read_bool("lives-remaining-exists"))
    {
        data->lives_remaining = (short)(packet->read_integer("lives-remaining", 7) - 1);
        mask |= 512;
    }
    if (packet->read_bool("last-betrayer-exists"))
    {
        data->last_betrayer = packet->read_integer("last-betrayer", 5) - 1;
        mask |= 1024;
    }
    if (packet->read_bool("respawn-timer-exists"))
    {
        data->respawn_timer = (unsigned short)packet->read_integer("respawn-timer", 10);
        mask |= 2048;
    }
    if (packet->read_bool("vehicle-entrance-ban-exists"))
    {
        data->vehicle_entrance_ban = packet->read_bool("vehicle-entrance-ban");
        mask |= 4096;
    }
    if (packet->read_bool("active-in-game-exists"))
    {
        data->active_in_game = packet->read_bool("active-in-game");
        mask |= 8192;
    }
    if (packet->read_bool("sitting-out-exists"))
    {
        data->sitting_out = packet->read_bool("sitting-out");
        mask |= 16384;
    }
    if (packet->read_bool("desired-respawn-player-exists"))
    {
        data->desired_respawn_player = (short)(packet->read_integer("desired-respawn-player", 16) - 1);
        if (data->desired_respawn_player != -1)
            data->desired_respawn_player = (short)player_try_and_get_player_index_from_absolute_player_index(data->desired_respawn_player);
        mask |= 32768;
    }
    if (packet->read_bool("player-control-aiming-exists"))
    {
        packet->read_unit_vector("player-control-aiming", &data->player_control_aiming);
        mask |= 65536;
    }
    bool result = true;
    result = result && mask != 0;
    *update_mask = mask;
    return result;
}
// 0x429d80: the "player creation" debug line with the object's relevance

// @address 0x429d80 ?write_creation_description_to_string@c_simulation_game_engine_player_entity_definition@@UAAXPBUs_simulation_entity@@PBUs_simulation_view_telemetry_data@@JPAD@Z
void c_simulation_game_engine_player_entity_definition::write_creation_description_to_string(
    const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry, long buffer_size, char *buffer)
{
    float relevance = 0.0f;
    simulation_calculate_entity_creation_priority(entity, telemetry, &relevance);
    csnzprintf(buffer, buffer_size, "player creation: relevance=%5.3f", relevance);
}
int simulation_game_engine_player_index_from_gamestate_index(int gamestate_index)
{
    int result = -1;
    for (int player_index = 0; player_index < 16; player_index++) {
        if (engine_slot((short)player_index) == gamestate_index) {
            result = player_index;
            break;
        }
    }
    return result;
}

void c_simulation_player_respawn_request_event_definition::write_description_to_string(
    const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry,
    float relevance, long buffer_size, char *buffer)
{
    csnzprintf(buffer, buffer_size, "player respawn request: relevance=%5.3f", calculate_relevance(event, telemetry, relevance));
}
void c_simulation_player_respawn_request_event_definition::event_payload_encode(
    long payload_size, const void *payload, c_bitstream *packet, bool encoding_is_for_debug)
{
    const s_player_respawn_target_request_event_data *data = (const s_player_respawn_target_request_event_data *)payload;
    packet->write_integer("request-player-index", data->request_player_index, 5);
    packet->write_integer("desired-respawn-player", (data->desired_respawn_player == -1 ? -1 : (unsigned short)data->desired_respawn_player) + 1, 16);
}
bool c_simulation_player_respawn_request_event_definition::event_payload_decode(
    long payload_size, void *payload, c_bitstream *packet, bool decoding_is_for_debug)
{
    s_player_respawn_target_request_event_data *data = (s_player_respawn_target_request_event_data *)payload;
    data->request_player_index = (short)packet->read_integer("request-player-index", 5);
    data->desired_respawn_player = (short)(packet->read_integer("desired-respawn-player", 16) - 1);
    return true;
}
// @address 0x42ad88 ?apply_game_event@c_simulation_player_respawn_request_event_definition@@UAA_NJPBJJPBX@Z
bool c_simulation_player_respawn_request_event_definition::apply_game_event(long reference_gamestate_count,
    const long *gamestate_indices, long payload_size, const void *payload)
{
    const s_player_respawn_target_request_event_data *request = (const s_player_respawn_target_request_event_data *)payload;
    bool success = false;
    if (request->request_player_index >= 0 && request->request_player_index < 16 &&
        (request->desired_respawn_player == -1 ||
            (request->desired_respawn_player >= 0 && request->desired_respawn_player < 16)) &&
        request->desired_respawn_player != request->request_player_index)
    {
        player_datum *player = simulation_game_engine_player_respawn_request_player_try_and_get_absolute(
            tls_player_data(), request->request_player_index);
        if (player && player->unit_index == -1)
        {
            player->respawn_target_player_index = simulation_game_engine_player_respawn_request_player_index_from_absolute_player_index(
                request->desired_respawn_player);
            ::simulation_action_game_engine_player_update(request->request_player_index, 0x8000);
            success = true;
        }
    }
    return success;
}

long c_simulation_player_editor_request_event_definition::payload_size()
{
    return sizeof(s_player_editor_request_event_data);
}

// @address 0x42ae70 ?minimum_required_bits@c_simulation_player_editor_request_event_definition@@UAAJPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
long c_simulation_player_editor_request_event_definition::minimum_required_bits(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    const s_player_editor_request_event_data *payload = (const s_player_editor_request_event_data *)event->get_payload_data();
    unsigned int event_type = payload->event_type;
    int result = 11;

    switch (event_type)
    {
    case 0:
        result = 12;
        if (payload->flags == 0)
            result = 184;
        break;
    case 2:
        result = 105;
        break;
    case 1:
    case 3:
        result = 43;
        break;
    case 6:
        result = 219;
        break;
    case 7:
        result = 59;
        break;
    }
    return result;
}

void c_simulation_player_editor_request_event_definition::write_description_to_string(
    const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry,
    float relevance, long buffer_size, char *buffer)
{
    csnzprintf(buffer, buffer_size, "player editor request: relevance=%5.3f", calculate_relevance(event, telemetry, relevance));
}
bool c_simulation_player_editor_request_event_definition::apply_game_event(
    long reference_gamestate_count, const long *reference_gamestate_indices, long payload_size, const void *payload)
{
    const s_player_editor_request_event_data *data = (const s_player_editor_request_event_data *)payload;
    bool result = false;
    if (data->request_player_index >= 0 && data->request_player_index < 16) {
        player_datum *player = editor_player_try_and_get_absolute(data->request_player_index);
        if (player && player->unit_index != -1) {
            const c_game_engine *engine = current_game_engine();
            result = engine->handle_incoming_simulation_event(event_type(), payload);
        }
    }
    return result;
}


static __forceinline c_map_variant *simulation_game_engine_player_runtime_map_variant()
{
    return &tls_game_engine_globals()->map_info;
}
static __forceinline void simulation_game_engine_player_write_optional_axes(c_bitstream *packet, const real_vector3d *forward, const real_vector3d *up)
{
    if (valid_real_vector3d_axes2(forward, up)) {
        packet->write_bool("axes-exist", true);
        packet->write_axes("placement-axes", forward, up);
    } else {
        packet->write_bool("axes-exist", false);
    }
}
// @address 0x42af58 ?event_payload_encode@c_simulation_player_editor_request_event_definition@@UAAXJPBXPAVc_bitstream@@_N@Z
void c_simulation_player_editor_request_event_definition::event_payload_encode(
    long payload_size, const void *payload, c_bitstream *packet, bool encoding_is_for_debug)
{
    const s_player_editor_request_event_data *data = (const s_player_editor_request_event_data *)payload;
    packet->write_integer("editor-player-index", data->request_player_index, 5);
    packet->write_integer("editor-request-type", data->event_type, 5);
    switch (data->event_type) {
    case 0:
        packet->write_bool("editor-desired-mode", data->flags);
        if (!data->flags) {
            simulation_write_quantized_position(packet, &data->player_position, 16, true, 0);
            packet->write_axes("player-placement-axes", &data->player_forward, &data->player_up);
            simulation_write_quantized_position(packet, &data->position, 16, true, 0);
            simulation_game_engine_player_write_optional_axes(packet, &data->forward, &data->up);
        }
        break;
    case 2:
        simulation_write_quantized_position(packet, &data->position, 16, true, 0);
        packet->write_axes("placement-axes", &data->forward, &data->up);
        packet->write_quantized_real("distance", data->distance, k_player_editor_minimum_distance, 1.0f, 8, false, false);
        break;
    case 4:
        packet->write_integer("edited-object-id", data->edited_object_id, 32);
        break;
    case 5:
        simulation_write_quantized_position(packet, &data->position, 16, true, 0);
    case 1:
    case 3:
        packet->write_integer("variant-index", data->variant_index, 32);
        break;
    case 6:
        {
            packet->write_integer("variant-index", data->variant_index, 32);
            s_variant_object_datum object;
            simulation_game_engine_player_runtime_map_variant()->get_variant_object_datum(data->variant_index, &object);
            if (memcmp(&data->properties.boundary_width, &object.properties.boundary_width,
                    sizeof(data->properties.boundary_width) + sizeof(data->properties.boundary_positive_height) +
                    sizeof(data->properties.boundary_negative_height) + sizeof(data->properties.boundary_box_length)) != 0 ||
                data->properties.boundary_shape != object.properties.boundary_shape) {
                packet->write_bool("shape-data-exists", true);
                packet->write_integer("shape-type", data->properties.boundary_shape, 4);
                const float maximum = k_player_editor_maximum_boundary;
                const float minimum = k_player_editor_minimum_distance;
                packet->write_quantized_real("shape-boundary-width-radius", data->properties.boundary_width, minimum, maximum, 16, true, false);
                packet->write_quantized_real("shape-boundary-positive-height", data->properties.boundary_positive_height, minimum, maximum, 16, true, false);
                packet->write_quantized_real("shape-boundary-negative-height", data->properties.boundary_negative_height, minimum, maximum, 16, true, false);
                packet->write_quantized_real("shape-boundary-box-length", data->properties.boundary_box_length, minimum, maximum, 16, true, false);
            } else {
                packet->write_bool("shape-data-exists", false);
            }
            if (data->properties.spawn_rate != object.properties.spawn_rate) {
                packet->write_bool("spawn-time-exists", true);
                packet->write_integer("spawn_time", data->properties.spawn_rate, 8);
            } else packet->write_bool("spawn-time-exists", false);
            if (data->properties.spare_clips != object.properties.spare_clips) {
                packet->write_bool("shared-storage-exists", true);
                packet->write_integer("shared-storage", data->properties.spare_clips, 8);
            } else packet->write_bool("shared-storage-exists", false);
            if (data->properties.placement_flags != object.properties.placement_flags) {
                packet->write_bool("flags-exist", true);
                packet->write_integer("flags", data->properties.placement_flags, 8);
            } else packet->write_bool("flags-exist", false);
            if (data->properties.team_designator != object.properties.team_designator) {
                packet->write_bool("team-affiliation-exists", true);
                packet->write_integer("team-affiliation", data->properties.team_designator, 8);
            } else packet->write_bool("team-affiliation-exists", false);
        }
        break;
    case 7:
        packet->write_integer("variant-index", data->variant_index, 32);
        packet->write_integer("minimum-count", data->minimum_count, 8);
        packet->write_integer("maximum-count", data->maximum_count, 8);
        break;
    }
}


// @address 0x42b3c0 ?event_payload_decode@c_simulation_player_editor_request_event_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
bool c_simulation_player_editor_request_event_definition::event_payload_decode(
    long payload_size, void *payload, c_bitstream *packet, bool decoding_is_for_debug)
{
    s_player_editor_request_event_data *data = (s_player_editor_request_event_data *)payload;
    data->request_player_index = (short)packet->read_integer("editor-player-index", 5);
    data->event_type = packet->read_integer("editor-request-type", 5);
    switch (data->event_type) {
    case 0:
        data->flags = packet->read_bool("editor-desired-mode");
        if (!data->flags) {
            ::simulation_read_quantized_position(packet, &data->player_position, 16, 0);
            packet->read_axes("player-placement-axes", &data->player_forward, &data->player_up);
            ::simulation_read_quantized_position(packet, &data->position, 16, 0);
            if (packet->read_bool("axes-exist"))
                packet->read_axes("placement-axes", &data->forward, &data->up);
            else {
                memset(&data->forward, 0, sizeof(data->forward));
                memset(&data->up, 0, sizeof(data->up));
            }
        }
        break;
    case 2:
        ::simulation_read_quantized_position(packet, &data->position, 16, 0);
        packet->read_axes("placement-axes", &data->forward, &data->up);
        data->distance = packet->read_quantized_real(
            "distance", k_player_editor_minimum_distance, 1.0f, 8, false);
        break;
    case 4:
        data->edited_object_id = packet->read_integer("edited-object-id", 32);
        break;
    case 5:
        ::simulation_read_quantized_position(packet, &data->position, 16, 0);
    case 1:
    case 3:
        data->variant_index = packet->read_integer("variant-index", 32);
        break;
    case 6:
        {
            data->variant_index = packet->read_integer("variant-index", 32);
            s_variant_object_datum object;
            simulation_game_engine_player_runtime_map_variant()->get_variant_object_datum(data->variant_index, &object);
            data->properties = object.properties;
            if (packet->read_bool("shape-data-exists")) {
                long shape = packet->read_integer("shape-type", 4);
                // match: 0x42b3c0: pointer __restrict qualifier prevents alias reload
                char * __restrict shape_out = &data->properties.boundary_shape.m_storage;
                const float minimum = k_player_editor_minimum_distance;
                const float maximum = k_player_editor_maximum_boundary;
                *shape_out = (char)shape;
                data->properties.boundary_width = dequantize_real(packet->read_integer("shape-boundary-width-radius", 16),
                    minimum, maximum, 16, true);
                data->properties.boundary_positive_height = dequantize_real(packet->read_integer("shape-boundary-positive-height", 16),
                    minimum, maximum, 16, true);
                data->properties.boundary_negative_height = dequantize_real(packet->read_integer("shape-boundary-negative-height", 16),
                    minimum, maximum, 16, true);
                data->properties.boundary_box_length = dequantize_real(packet->read_integer("shape-boundary-box-length", 16),
                    minimum, maximum, 16, true);
            }
            if (packet->read_bool("spawn-time-exists"))
                data->properties.spawn_rate = (unsigned char)packet->read_integer("spawn_time", 8);
            if (packet->read_bool("shared-storage-exists"))
                data->properties.spare_clips = (unsigned char)packet->read_integer("shared-storage", 8);
            if (packet->read_bool("flags-exist"))
                data->properties.placement_flags = (unsigned char)packet->read_integer("flags", 8);
            if (packet->read_bool("team-affiliation-exists"))
                data->properties.team_designator = (e_variant_team_designator)packet->read_integer("team-affiliation", 8);
        }
        break;
    case 7:
        data->variant_index = packet->read_integer("variant-index", 32);
        data->minimum_count = (unsigned char)packet->read_integer("minimum-count", 8);
        data->maximum_count = (unsigned char)packet->read_integer("maximum-count", 8);
        break;
    }
    return true;
}

// @address 0x480e8 ??_7c_simulation_game_engine_player_entity_definition@@6B@
// @address 0x709ee8 ??__Eg_simulation_game_engine_player_entity_definition@@YAXXZ
// @address 0x70c790 ??__Fg_simulation_game_engine_player_entity_definition@@YAXXZ
// @address 0x7bb444 g_simulation_game_engine_player_entity_definition
c_simulation_game_engine_player_entity_definition g_simulation_game_engine_player_entity_definition;

extern "C" {
// @address 0x48160 ??_7c_simulation_player_respawn_request_event_definition@@6B@
// @address 0x7bb448 g_simulation_player_respawn_request_event_definition
c_simulation_player_respawn_request_event_definition g_simulation_player_respawn_request_event_definition;
// @address 0x48198 ??_7c_simulation_player_editor_request_event_definition@@6B@
// @address 0x7bb44c g_simulation_player_editor_request_event_definition
c_simulation_player_editor_request_event_definition g_simulation_player_editor_request_event_definition;
}

// @address 0x45fc50 ?entity_type@c_simulation_game_engine_player_entity_definition@@UAA?AW4e_simulation_entity_type@@XZ
e_simulation_entity_type c_simulation_game_engine_player_entity_definition::entity_type()
{
    return _simulation_entity_type_game_engine_player;
}

// @address 0x3fc598 ?creation_data_size@c_simulation_game_engine_player_entity_definition@@UAAJXZ
long c_simulation_game_engine_player_entity_definition::creation_data_size()
{
    return sizeof(s_simulation_game_engine_player_creation_data);
}

// @address 0x45fca8 ?update_flag_count@c_simulation_game_engine_player_entity_definition@@UAAJXZ
long c_simulation_game_engine_player_entity_definition::update_flag_count()
{
    return 17;
}

// @address 0x42d298 ?initial_update_mask@c_simulation_game_engine_player_entity_definition@@UAAKXZ
unsigned long c_simulation_game_engine_player_entity_definition::initial_update_mask()
{
    return 0;
}

// @address 0x42d380 ?entity_replication_required_for_view_activation@c_simulation_game_engine_player_entity_definition@@UAA_NPBUs_simulation_entity@@@Z
bool c_simulation_game_engine_player_entity_definition::entity_replication_required_for_view_activation(const s_simulation_entity *entity)
{
    return true;
}

// @address 0x42d298 ?entity_type_is_gameworld_object@c_simulation_game_engine_player_entity_definition@@UAA_NXZ
bool c_simulation_game_engine_player_entity_definition::entity_type_is_gameworld_object()
{
    return false;
}

// @address 0x42d380 ?entity_can_be_created@c_simulation_game_engine_player_entity_definition@@UAA_NPBUs_simulation_entity@@PBUs_simulation_view_telemetry_data@@@Z
bool c_simulation_game_engine_player_entity_definition::entity_can_be_created(const s_simulation_entity *entity,
    const s_simulation_view_telemetry_data *telemetry)
{
    return true;
}

// @address 0x428ad0 ?creation_minimum_required_bits@c_simulation_game_engine_player_entity_definition@@UAAJPBUs_simulation_entity@@PBUs_simulation_view_telemetry_data@@@Z
long c_simulation_game_engine_player_entity_definition::creation_minimum_required_bits(const s_simulation_entity *entity,
    const s_simulation_view_telemetry_data *telemetry)
{
    return 5;
}


// @address 0x42d380 ?entity_state_lossy_compare@c_simulation_game_engine_player_entity_definition@@UAA_NPAX0J@Z
bool c_simulation_game_engine_player_entity_definition::entity_state_lossy_compare(void *first, void *second, long size)
{
    return true;
}

// @address 0x42d380 ?entity_creation_lossy_compare@c_simulation_game_engine_player_entity_definition@@UAA_NPAX0J@Z
bool c_simulation_game_engine_player_entity_definition::entity_creation_lossy_compare(void *first, void *second, long size)
{
    return true;
}

// @address 0x5bec98 ?rotate_entity_indices@c_simulation_game_engine_player_entity_definition@@UAAXPBUs_simulation_entity@@@Z
void c_simulation_game_engine_player_entity_definition::rotate_entity_indices(const s_simulation_entity *entity)
{
}

// @address 0x5bec98 ?prepare_creation_data_for_gameworld@c_simulation_game_engine_player_entity_definition@@UAAXJPAX@Z
void c_simulation_game_engine_player_entity_definition::prepare_creation_data_for_gameworld(long size, void *creation_data)
{
}

// @address 0x5bec98 ?prepare_state_data_for_gameworld@c_simulation_game_engine_player_entity_definition@@UAAXKJPAX@Z
void c_simulation_game_engine_player_entity_definition::prepare_state_data_for_gameworld(unsigned long update_mask, long size, void *state)
{
}

// @address 0x3fc550 ?payload_size@c_simulation_player_respawn_request_event_definition@@UAAJXZ
long c_simulation_player_respawn_request_event_definition::payload_size()
{
    return sizeof(s_player_respawn_target_request_event_data);
}

// @address 0x42d298 ?number_of_entity_references@c_simulation_player_respawn_request_event_definition@@UAAJXZ
long c_simulation_player_respawn_request_event_definition::number_of_entity_references()
{
    return 0;
}

// @address 0x42d298 ?reference_delays_entity_deletion@c_simulation_player_respawn_request_event_definition@@UAA_NXZ
bool c_simulation_player_respawn_request_event_definition::reference_delays_entity_deletion()
{
    return false;
}

// @address 0x42d380 ?event_can_be_transmitted@c_simulation_player_respawn_request_event_definition@@UAA_NPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
bool c_simulation_player_respawn_request_event_definition::event_can_be_transmitted(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    return true;
}

// @address 0x426e90 ?minimum_required_bits@c_simulation_player_respawn_request_event_definition@@UAAJPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
long c_simulation_player_respawn_request_event_definition::minimum_required_bits(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    return 11;
}

// @address 0x4311b8 ?calculate_relevance@c_simulation_player_respawn_request_event_definition@@UAAMPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@M@Z
float c_simulation_player_respawn_request_event_definition::calculate_relevance(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance)
{
    return simulation_calculate_event_relevance(this, event, relevance, telemetry, 0);
}

// @address 0x429c30 ?event_requirements@c_simulation_player_respawn_request_event_definition@@UAA?AUs_simulation_event_requirements@@PBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
s_simulation_event_requirements c_simulation_player_respawn_request_event_definition::event_requirements(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    s_simulation_event_requirements requirements;
    requirements.reference_count = 0;
    requirements.reference_delays_entity_deletion = false;
    requirements.can_be_transmitted = true;
    requirements.minimum_required_bits = 11;
    return requirements;
}

// @address 0x5bec98 ?prepare_event_data_for_gameworld@c_simulation_player_respawn_request_event_definition@@UAAXJPAX@Z
void c_simulation_player_respawn_request_event_definition::prepare_event_data_for_gameworld(long payload_size, void *payload)
{
}

// @address 0x429c40 ?event_type@c_simulation_player_editor_request_event_definition@@UAA?AW4e_simulation_event_type@@XZ
e_simulation_event_type c_simulation_player_editor_request_event_definition::event_type()
{
    return _simulation_event_type_player_editor_request;
}

// @address 0x42d298 ?number_of_entity_references@c_simulation_player_editor_request_event_definition@@UAAJXZ
long c_simulation_player_editor_request_event_definition::number_of_entity_references()
{
    return 0;
}

// @address 0x42d298 ?reference_delays_entity_deletion@c_simulation_player_editor_request_event_definition@@UAA_NXZ
bool c_simulation_player_editor_request_event_definition::reference_delays_entity_deletion()
{
    return false;
}

// @address 0x42d380 ?event_can_be_transmitted@c_simulation_player_editor_request_event_definition@@UAA_NPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
bool c_simulation_player_editor_request_event_definition::event_can_be_transmitted(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    return true;
}

// @address 0x4311b8 ?calculate_relevance@c_simulation_player_editor_request_event_definition@@UAAMPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@M@Z
float c_simulation_player_editor_request_event_definition::calculate_relevance(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance)
{
    return simulation_calculate_event_relevance(this, event, relevance, telemetry, 0);
}

// @address 0x429c58 ?event_requirements@c_simulation_player_editor_request_event_definition@@UAA?AUs_simulation_event_requirements@@PBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
s_simulation_event_requirements c_simulation_player_editor_request_event_definition::event_requirements(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    s_simulation_event_requirements requirements;
    requirements.reference_count = 0;
    requirements.reference_delays_entity_deletion = false;
    requirements.can_be_transmitted = true;
    requirements.minimum_required_bits = c_simulation_player_editor_request_event_definition::minimum_required_bits(event, telemetry);
    return requirements;
}

// @address 0x5bec98 ?prepare_event_data_for_gameworld@c_simulation_player_editor_request_event_definition@@UAAXJPAX@Z
void c_simulation_player_editor_request_event_definition::prepare_event_data_for_gameworld(long payload_size, void *payload)
{
}
