// OBJ: from the records 0x3f0ea8 0x3f14d8 0x3f1f08 0x3f21a8 0x3f2538 0x3f2578 0x3f2608 0x3f2658 0x3f2700 0x3f2780 0x3f28c8 0x3f2920 0x3f29d8 0x3f2a18 0x3f2ac8 0x3f2b38 0x3f2c70
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x3f0ea8 ?encode@c_network_message_membership_update@@SAXPAVc_bitstream@@JPBX@Z
// @address 0x3c8c80 ?player_configuration_host_encode@@YAXPAVc_bitstream@@PBUs_player_configuration_from_host@@W4e_player_configuration_encoding_type@@@Z
// @address 0x3f2538 ?encode@c_network_message_delegate_leadership@@SAXPAVc_bitstream@@JPBX@Z
// @address 0x3f2608 ?encode@c_network_message_boot_machine@@SAXPAVc_bitstream@@JPBX@Z
// @address 0x3f2700 ?encode@c_network_message_player_add@@SAXPAVc_bitstream@@JPBX@Z
// @address 0x3f28c8 ?encode@c_network_message_player_refuse@@SAXPAVc_bitstream@@JPBX@Z
// @address 0x3f29d8 ?encode@c_network_message_player_remove@@SAXPAVc_bitstream@@JPBX@Z
// @address 0x3f2ac8 ?encode@c_network_message_player_properties@@SAXPAVc_bitstream@@JPBX@Z
// @address 0x221648 ?read_bit_internal@c_bitstream@@AAA_NXZ
// @address 0x2212e0 ?read_bits_internal@c_bitstream@@AAAXPAEJ@Z
// @address 0x221768 ?read_value_internal@c_bitstream@@AAAKJ@Z
// @address 0x221398 ?write_bit_internal@c_bitstream@@AAAX_N@Z
// @address 0x221480 ?write_value_internal@c_bitstream@@QAAXKJ@Z
// @address 0x3c8850 ?player_configuration_client_decode@@YA_NPAVc_bitstream@@PAUs_player_configuration_from_client@@W4e_player_configuration_encoding_type@@@Z
// @address 0x3c8da8 ?player_configuration_host_decode@@YA_NPAVc_bitstream@@PAUs_player_configuration_from_host@@W4e_player_configuration_encoding_type@@@Z
// @address 0x2209d0 ?read_string_wchar@c_bitstream@@QAAXPBDPAGJ@Z
// @address 0x221228 ?write_bits_internal@c_bitstream@@AAAXPBEJ@Z
#include "memory/bitstream.h"
#include "memory/bitstream_wide_strings.h"
#include <stddef.h>
#include "networking/messages/network_messages_session_membership.h"
#include "networking/network_game_definitions.h"
#include "networking/player_configuration_encoding_types.h"
#include "networking/messages/network_message_type_collection.h"
#include "networking/network_globals.h"

// the 64-bit window internals (no name in the March 2007 build)

void c_network_message_delegate_leadership::encode(c_bitstream *stream, long message_storage_size, const void *message_storage)
{
    const s_network_message_delegate_leadership *message = (const s_network_message_delegate_leadership *)message_storage;
    stream->write_raw_data("session-id", &message->session_id, 64);
    stream->write_raw_data("desired-leader-address", &message->desired_leader_address, 288);
}

void c_network_message_boot_machine::encode(c_bitstream *stream, long message_storage_size, const void *message_storage)
{
    const s_network_message_boot_machine *message = (const s_network_message_boot_machine *)message_storage;
    stream->write_raw_data("session-id", &message->session_id, 64);
    stream->write_integer("reason", message->reason, 4);
    stream->write_raw_data("boot-peer-address", &message->boot_peer_address, 288);
}

void c_network_message_player_refuse::encode(c_bitstream *stream, long message_storage_size, const void *message_storage)
{
    const s_network_message_player_refuse *message = (const s_network_message_player_refuse *)message_storage;
    stream->write_raw_data("session-id", &message->session_id, 64);
    stream->write_raw_data("player-identifier", &message->player_identifier, 64);
    stream->write_integer("user-index", message->user_index, 2);
    stream->write_integer("refuse-reason", message->refuse_reason, 5);
}

void c_network_message_player_remove::encode(c_bitstream *stream, long message_storage_size, const void *message_storage)
{
    const s_network_message_player_remove *message = (const s_network_message_player_remove *)message_storage;
    stream->write_raw_data("session-id", &message->session_id, 64);
    stream->write_integer("user-index", message->user_index, 2);
}

void c_network_message_player_add::encode(c_bitstream *stream, long message_storage_size, const void *message_storage)
{
    const s_network_message_player_add *message = (const s_network_message_player_add *)message_storage;
    stream->write_raw_data("session-id", &message->session_id, 64);
    stream->write_integer("user-index", message->user_index, 2);
    stream->write_raw_data("player-identifier", &message->player_identifier, 64);
    stream->write_integer("player-update-number", message->player_update_number, 32);
    stream->write_integer("controller-index", message->controller_index, 2);
    player_configuration_client_encode_packet(
        stream, (const s_network_game_definitions_client_configuration *)&message->configuration, _player_configuration_encoding_full);
    stream->write_integer("player-voice", message->voice, 32);
}

void c_network_message_player_properties::encode(c_bitstream *stream, long message_storage_size, const void *message_storage)
{
    const s_network_message_player_properties *message = (const s_network_message_player_properties *)message_storage;
    stream->write_raw_data("session-id", &message->session_id, 64);
    stream->write_integer("user-index", message->user_index, 2);
    stream->write_integer("update-number", message->update_number, 32);
    stream->write_integer("controller-index", message->controller_index, 2);
    player_configuration_client_encode_packet(
        stream, (const s_network_game_definitions_client_configuration *)&message->configuration, _player_configuration_encoding_full);
    stream->write_integer("player-voice", message->voice, 32);
}

static __forceinline bool bitstream_overflowed(c_bitstream *stream)
{
    bool overflowed = stream->m_current.current_stream_bit_position > (stream->m_data_size_bytes << 3);
    if (stream->m_error_occurred)
        overflowed = true;
    return overflowed;
}

typedef char membership_decode_peer_name_offset[(offsetof(s_network_message_membership_update_peer, properties.name) == 0x5a) ? 1 : -1];
typedef char membership_decode_peer_bandwidth_offset[(offsetof(s_network_message_membership_update_peer, properties.bandwidth) == 0xe0) ? 1 : -1];

typedef char membership_decode_player_number_offset[(offsetof(s_network_message_membership_update_player, player_addition_number) == 0x18) ? 1 : -1];

// @address 0x3f14d8 ?decode@c_network_message_membership_update@@SA_NPAVc_bitstream@@JPAX@Z
bool c_network_message_membership_update::decode(c_bitstream *stream, long message_storage_size, void *message_storage)
{
    s_network_message_membership_update *message = (s_network_message_membership_update *)message_storage;
    bool valid = true;
    stream->read_raw_data("session-id", &message->session_id, 64);
    message->update_number = stream->read_integer("update-number", 32);
    if (stream->read_bool("complete-update"))
    {
        message->incremental_update_number = -1;
    }
    else
    {
        message->incremental_update_number = stream->read_integer("incremental-update-number", 32);
        message->baseline_checksum = stream->read_integer("baseline-checksum", 32);
    }
    message->peer_count = (short)stream->read_integer("peer-count", 5);
    message->player_count = (short)stream->read_integer("player-count", 5);
    if (message->peer_count >= 0 && message->peer_count <= 32 && message->player_count >= 0 && message->player_count <= 32)
    {
        for (long peer_index = 0; peer_index < message->peer_count; peer_index++)
        {
            s_network_message_membership_update_peer *peer = &message->peers[peer_index];
            peer->peer_index = stream->read_integer("peer-index", 4);
            peer->update_type = stream->read_integer("peer-update-type", 1);
            valid = valid && peer->peer_index >= 0 && peer->peer_index < 16;
            valid = valid && peer->update_type >= 0 && peer->update_type < 2;
            if (peer->update_type == 1)
            {
                peer->connection_state = stream->read_integer("peer-connection-state", 4);
                valid = valid && peer->connection_state >= 0 && peer->connection_state < 9;
                peer->info_updated = stream->read_bool("peer-info-updated");
                if (peer->info_updated)
                {
                    stream->read_raw_data("peer-address", &peer->address, 288);
                    stream->read_raw_data("peer-party-nonce", &peer->party_nonce, 64);
                    stream->read_raw_data("peer-join-nonce", &peer->join_nonce, 64);
                    peer->network_version_number = stream->read_integer("network-version-number", 32);
                    peer->creation_timestamp = stream->read_integer("peer-creation-timestamp", 32);
                }
                peer->properties_updated = stream->read_bool("peer-properties-updated");
                if (peer->properties_updated)
                {
                    peer->properties.name_updated = stream->read_bool("peer-name-updated");
                    if (peer->properties.name_updated)
                    {
                        stream->read_string_wchar("peer-name", peer->properties.name, 16);
                        stream->read_string_wchar("peer-session-name", peer->properties.session_name, 32);
                    }
                    peer->properties.has_hard_drive = stream->read_bool("has-hard-drive");
                    peer->properties.ready_hopper_id = (unsigned short)stream->read_integer("ready-hopper-id", 16);
                    peer->properties.game_start_error = stream->read_integer("game-start-error", 6);
                    peer->properties.unknown_updated = stream->read_bool("unknown-updated");
                    if (peer->properties.unknown_updated)
                        peer->properties.unknown_value = stream->read_integer("unknown-value", 32);
                    peer->properties.map_updated = stream->read_bool("peer-map-updated");
                    if (peer->properties.map_updated)
                    {
                        peer->properties.map_status = stream->read_integer("peer-map-status", 3);
                        peer->properties.map_progress_percentage = stream->read_integer("peer-map-progress-percentage", 7);
                        valid = valid && peer->properties.map_status >= 0 && peer->properties.map_status < 5;
                        valid = valid && peer->properties.map_progress_percentage >= 0 && peer->properties.map_progress_percentage <= 100;
                    }
                    peer->properties.connection_updated = stream->read_bool("peer-connection-updated");
                    if (peer->properties.connection_updated)
                    {
                        if (stream->read_bool("estimated-bandwidth-present"))
                            stream->read_raw_data("estimated-bandwidth", &peer->properties.bandwidth, 64);
                        else
                            peer->properties.bandwidth = -1;
                    }
                    peer->properties.bandwidth_updated = stream->read_bool("bandwidth-updated");
                    if (peer->properties.bandwidth_updated)
                        peer->properties.bandwidth_value = stream->read_integer("bandwidth-value", 32);
                    peer->properties.addresses_updated = stream->read_bool("addresses-updated");
                    if (peer->properties.addresses_updated)
                    {
                        read_bytes32(stream, peer->properties.address.bytes);
                        read_bytes32(stream, peer->properties.online_address.bytes);
                        peer->properties.address_flag = stream->read_bool("peer-address-flag");
                        peer->properties.nat_type = stream->read_integer("nat-type", 2);
                        peer->properties.connectivity_badness_rating = stream->read_integer("connectivity-badness-rating", 7);
                        peer->properties.host_badness_rating = stream->read_integer("host-badness-rating", 7);
                        peer->properties.client_badness_rating = stream->read_integer("client-badness-rating", 7);
                        peer->properties.language = stream->read_integer("language", 4);
                        valid = valid && peer->properties.connectivity_badness_rating >= 0 && peer->properties.connectivity_badness_rating <= 100;
                        valid = valid && peer->properties.host_badness_rating >= 0 && peer->properties.host_badness_rating <= 100;
                        valid = valid && peer->properties.client_badness_rating >= 0 && peer->properties.client_badness_rating <= 100;
                        peer->properties.connectivity_mask = (unsigned short)stream->read_integer("peer-connectivity-mask", 16);
                        peer->properties.probe_mask = (unsigned short)stream->read_integer("peer-probe-mask", 16);
                        peer->properties.latency_min = stream->read_integer("peer-latency-min", 11);
                        peer->properties.latency_est = stream->read_integer("peer-latency-est", 11);
                        peer->properties.latency_max = stream->read_integer("peer-latency-max", 11);
                        valid = valid && peer->properties.latency_min >= 0 && peer->properties.latency_min <= 2000;
                        valid = valid && peer->properties.latency_est >= 0 && peer->properties.latency_est <= 2000;
                        valid = valid && peer->properties.latency_max >= 0 && peer->properties.latency_max <= 2000;
                    }
                    peer->properties.values_updated = stream->read_bool("values-updated");
                    if (peer->properties.values_updated)
                    {
                        peer->properties.determinism_version = stream->read_integer("determinism-version", 8);
                        peer->properties.determinism_compatible_version = stream->read_integer("determinism-compatible-version", 8);
                    }
                    peer->properties.flags_updated = stream->read_bool("flags-updated");
                    if (peer->properties.flags_updated)
                    {
                        peer->properties.flags = stream->read_integer("flags", 17);
                        valid = valid && (((peer->properties.flags & ~0x1ffff) == 0) & 1u);
                    }
                }
            }
        }
        for (long player_index = 0; player_index < message->player_count; player_index++)
        {
            s_network_message_membership_update_player *player = &message->players[player_index];
            player->player_index = stream->read_integer("player-index", 4);
            player->update_type = stream->read_integer("update-type", 1);
            valid = valid && player->player_index >= 0 && player->player_index < 16;
            valid = valid && player->update_type >= 0 && player->update_type < 2;
            if (player->update_type == 1)
            {
                player->location_updated = stream->read_bool("player-location-updated");
                if (player->location_updated)
                {
                    stream->read_raw_data("identifier", &player->identifier, 64);
                    player->peer_index = (unsigned short)stream->read_integer("peer-index", 4);
                    player->peer_user_index = (unsigned short)stream->read_integer("peer-user-index", 2);
                    player->player_addition_number = stream->read_integer("player-addition-number", 20);
                    player->occupies_a_public_slot = stream->read_bool("player-occupies-a-public-slot");
                    valid = valid && (short)player->peer_index >= 0 && (short)player->peer_index < 16;
                    valid = valid && (short)player->peer_user_index >= 0 && (short)player->peer_user_index < 4;
                }
                player->properties_updated = stream->read_bool("player-properties-updated");
                if (player->properties_updated)
                {
                    player->player_update_number = stream->read_integer("player-update-number", 32);
                    player->controller_index = stream->read_integer("controller-index", 3) - 1;
                    bool client_valid = player_configuration_client_decode(
                        stream, &player->configuration.client, _player_configuration_encoding_full);
                    valid = valid && client_valid;
                    bool host_valid = player_configuration_host_decode(
                        stream, &player->configuration.host, _player_configuration_encoding_full);
                    valid = valid && host_valid;
                    player->voice = stream->read_integer("player-voice", 32);
                }
            }
        }
    }
    else
    {
        valid = false;
    }
    message->player_addition_number_updated = stream->read_bool("player-addition-number-updated");
    if (message->player_addition_number_updated)
        message->player_addition_number = stream->read_integer("player-addition-number", 20);
    message->leader_updated = stream->read_bool("leader-updated");
    if (message->leader_updated)
        message->leader_peer_index = stream->read_integer("leader-peer_index", 4);
    message->host_updated = stream->read_bool("host-updated");
    if (message->host_updated)
        message->host_peer_index = stream->read_integer("host-peer_index", 4);
    message->slot_counts_updated = stream->read_bool("slot-counts-updated");
    if (message->slot_counts_updated)
    {
        message->private_slot_count = stream->read_integer("private-slot-count", 5);
        message->public_slot_count = stream->read_integer("public-slot-count", 5);
        message->are_slots_locked = stream->read_bool("are-slots-locked");
        message->request_modification = stream->read_bool("unknown-27bd");
    }
    message->checksum = stream->read_integer("checksum", 32);
    valid = valid && !bitstream_overflowed(stream);
    valid = valid && (message->incremental_update_number == -1 || message->incremental_update_number < message->update_number);
    return valid;
}

// @address 0x3f1f08 ?encode@c_network_message_peer_properties@@SAXPAVc_bitstream@@JPBX@Z
void c_network_message_peer_properties::encode(c_bitstream *stream, long message_storage_size, const void *message_storage)
{
    const s_network_message_peer_properties *message = (const s_network_message_peer_properties *)message_storage;
    stream->write_raw_data("session-id", &message->session_id, 64);
    stream->write_raw_data("secure-address", &message->secure_address, 288);
    stream->write_string_wchar("peer-name", (const unsigned short *)message->properties.peer_name, 16);
    stream->write_string_wchar("peer-session-name", (const unsigned short *)message->properties.peer_session_name, 32);
    stream->write_bool("peer-has-hard-drive", message->properties.has_hard_drive);
    stream->write_integer("ready-hopper-id", message->properties.ready_hopper_id, 16);
    stream->write_integer("game-start-error", message->properties.game_start_error, 6);
    stream->write_integer("estimated-downstream-bandwidth", message->properties.estimated_downstream_bandwidth, 32);
    stream->write_integer("peer-map-status", message->properties.map_status, 3);
    stream->write_integer("peer-map-progress-percentage", message->properties.map_progress_percentage, 7);
    stream->write_integer("estimated-upstream-bandwidth", message->properties.estimated_upstream_bandwidth, 32);
    stream->write_raw_data("data", &message->properties.bandwidth, 64);
    stream->write_raw_data("data", &message->properties.address, 32);
    stream->write_raw_data("data", &message->properties.online_address, 32);
    stream->write_bool("peer-address-flag", message->properties.address_flag);
    stream->write_integer("connectivity-badness-rating", message->properties.connectivity_badness_rating, 7);
    stream->write_integer("host-badness-rating", message->properties.host_badness_rating, 7);
    stream->write_integer("client-badness-rating", message->properties.client_badness_rating, 7);
    stream->write_integer("nat-type", message->properties.nat_type, 2);
    stream->write_integer("peer-connectivity-mask", message->properties.connectivity_mask, 16);
    stream->write_integer("peer-probe-mask", message->properties.probe_mask, 16);
    stream->write_integer("peer-latency-min", message->properties.latency_min, 11);
    stream->write_integer("peer-latency-est", message->properties.latency_est, 11);
    stream->write_integer("peer-latency-max", message->properties.latency_max, 11);
    stream->write_integer("language", message->properties.language, 4);
    stream->write_integer("determinism-version", message->properties.determinism_version, 8);
    stream->write_integer("determinism-compatible-version", message->properties.determinism_compatible_version, 8);
    stream->write_integer("flags", message->properties.flags, 17);
}

// @address 0x3f21a8 ?decode@c_network_message_peer_properties@@SA_NPAVc_bitstream@@JPAX@Z
bool c_network_message_peer_properties::decode(c_bitstream *stream, long message_storage_size, void *message_storage)
{
    s_network_message_peer_properties *out = (s_network_message_peer_properties *)message_storage;
    stream->read_raw_data("data", (void *)&out->session_id, 64);
    stream->read_raw_data("data", (void *)&out->secure_address, 288);
    stream->read_string_wchar("peer-name", (unsigned short *)out->properties.peer_name, 16);
    stream->read_string_wchar("peer-session-name", (unsigned short *)out->properties.peer_session_name, 32);
    out->properties.has_hard_drive = stream->read_bool("peer-has-hard-drive");
    out->properties.ready_hopper_id = stream->read_integer("ready-hopper-id", 16);
    out->properties.game_start_error = stream->read_integer("game-start-error", 6);
    out->properties.estimated_downstream_bandwidth = stream->read_integer("estimated-downstream-bandwidth", 32);
    out->properties.map_status = stream->read_integer("peer-map-status", 3);
    out->properties.map_progress_percentage = stream->read_integer("peer-map-progress-percentage", 7);
    out->properties.estimated_upstream_bandwidth = stream->read_integer("estimated-upstream-bandwidth", 32);
    stream->read_raw_data("data", (void *)&out->properties.bandwidth, 64);
    read_bytes32(stream, out->properties.address.bytes);
    read_bytes32(stream, out->properties.online_address.bytes);
    out->properties.address_flag = stream->read_bool("peer-address-flag");
    out->properties.connectivity_badness_rating = stream->read_integer("connectivity-badness-rating", 7);
    out->properties.host_badness_rating = stream->read_integer("host-badness-rating", 7);
    out->properties.client_badness_rating = stream->read_integer("client-badness-rating", 7);
    out->properties.nat_type = stream->read_integer("nat-type", 2);
    out->properties.connectivity_mask = (short)stream->read_integer("peer-connectivity-mask", 16);
    out->properties.probe_mask = (short)stream->read_integer("peer-probe-mask", 16);
    out->properties.latency_min = stream->read_integer("peer-latency-min", 11);
    out->properties.latency_est = stream->read_integer("peer-latency-est", 11);
    out->properties.latency_max = stream->read_integer("peer-latency-max", 11);
    out->properties.language = stream->read_integer("language", 4);
    out->properties.determinism_version = stream->read_integer("determinism-version", 8);
    out->properties.determinism_compatible_version = stream->read_integer("determinism-compatible-version", 8);
    out->properties.flags = stream->read_integer("flags", 17);
    bool valid = true;
    valid = valid && !bitstream_overflowed(stream);
    valid = valid && out->properties.connectivity_badness_rating >= 0 && out->properties.connectivity_badness_rating <= 100;
    valid = valid && out->properties.host_badness_rating >= 0 && out->properties.host_badness_rating <= 100;
    valid = valid && out->properties.client_badness_rating >= 0 && out->properties.client_badness_rating <= 100;
    valid = valid && out->properties.map_status >= 0 && out->properties.map_status < 5;
    valid = valid && out->properties.map_progress_percentage >= 0 && out->properties.map_progress_percentage <= 100;
    valid = valid && (((out->properties.flags & ~0x1ffff) == 0) & 1u);
    return valid;
}

// @address 0x3f2578 ?decode@c_network_message_delegate_leadership@@SA_NPAVc_bitstream@@JPAX@Z
bool c_network_message_delegate_leadership::decode(c_bitstream *stream, long message_storage_size, void *message_storage)
{
    s_network_message_delegate_leadership *out = (s_network_message_delegate_leadership *)message_storage;
    stream->read_raw_data("data", (void *)&out->session_id, 64);
    stream->read_raw_data("data", (void *)&out->desired_leader_address, 288);
    bool success = true;
    success = success && !bitstream_overflowed(stream);
    return success;
}

// @address 0x3f2658 ?decode@c_network_message_boot_machine@@SA_NPAVc_bitstream@@JPAX@Z
bool c_network_message_boot_machine::decode(c_bitstream *stream, long message_storage_size, void *message_storage)
{
    s_network_message_boot_machine *out = (s_network_message_boot_machine *)message_storage;
    stream->read_raw_data("data", (void *)&out->session_id, 64);
    out->reason = stream->read_integer("reason", 4);
    stream->read_raw_data("data", (void *)&out->boot_peer_address, 288);
    bool success = true;
    success = success && !bitstream_overflowed(stream);
    return success;
}

// @address 0x3f2780 ?decode@c_network_message_player_add@@SA_NPAVc_bitstream@@JPAX@Z
bool c_network_message_player_add::decode(c_bitstream *stream, long message_storage_size, void *message_storage)
{
    s_network_message_player_add *message = (s_network_message_player_add *)message_storage;
    bool success = true;
    stream->read_raw_data("session-id", &message->session_id, 64);
    message->user_index = stream->read_integer("user-index", 2);
    stream->read_raw_data("player-identifier", &message->player_identifier, 64);
    message->player_update_number = stream->read_integer("player-update-number", 32);
    message->controller_index = stream->read_integer("controller-index", 2);
    success = success &&
        player_configuration_client_decode(
            stream, &message->configuration, _player_configuration_encoding_full);
    message->voice = stream->read_integer("player-voice", 32);
    success = success && !bitstream_overflowed(stream);
    success = success && message->user_index >= 0 && message->user_index < 4;
    success = success && message->controller_index >= 0 && message->controller_index < 4;
    return success;
}

// @address 0x3f2920 ?decode@c_network_message_player_refuse@@SA_NPAVc_bitstream@@JPAX@Z
bool c_network_message_player_refuse::decode(c_bitstream *stream, long message_storage_size, void *message_storage)
{
    s_network_message_player_refuse *out = (s_network_message_player_refuse *)message_storage;
    stream->read_raw_data("data", (void *)&out->session_id, 64);
    stream->read_raw_data("data", (void *)&out->player_identifier, 64);
    out->user_index = stream->read_integer("user-index", 2);
    out->refuse_reason = stream->read_integer("refuse-reason", 5);
    bool success = true;
    success = success && !bitstream_overflowed(stream);
    return success;
}

// @address 0x3f2a18 ?decode@c_network_message_player_remove@@SA_NPAVc_bitstream@@JPAX@Z
bool c_network_message_player_remove::decode(c_bitstream *stream, long message_storage_size, void *message_storage)
{
    s_network_message_player_remove *message = (s_network_message_player_remove *)message_storage;
    bool success = true;
    stream->read_raw_data("data", &message->session_id, 64);
    message->user_index = stream->read_integer("user-index", 2);
    success = success && !bitstream_overflowed(stream);
    success = success && message->user_index >= 0 && message->user_index < 4;
    return success;
}

// @address 0x3f2b38 ?decode@c_network_message_player_properties@@SA_NPAVc_bitstream@@JPAX@Z
bool c_network_message_player_properties::decode(c_bitstream *stream, long message_storage_size, void *message_storage)
{
    s_network_message_player_properties *message = (s_network_message_player_properties *)message_storage;
    bool success = true;
    stream->read_raw_data("session-id", &message->session_id, 64);
    message->user_index = stream->read_integer("user-index", 2);
    message->update_number = stream->read_integer("update-number", 32);
    message->controller_index = stream->read_integer("controller-index", 2);
    success = success &&
        player_configuration_client_decode(
            stream, &message->configuration, _player_configuration_encoding_full);
    message->voice = stream->read_integer("player-voice", 32);
    success = success && !bitstream_overflowed(stream);
    success = success && message->user_index >= 0 && message->user_index < 4;
    success = success && message->controller_index >= 0 && message->controller_index < 4;
    return success;
}

// @address 0x3f0ea8 ?encode@c_network_message_membership_update@@SAXPAVc_bitstream@@JPBX@Z
void c_network_message_membership_update::encode(c_bitstream *stream, long message_storage_size, const void *message_storage)
{
    const s_network_message_membership_update *message = (const s_network_message_membership_update *)message_storage;
    stream->write_raw_data("session-id", &message->session_id, 64);
    stream->write_integer("update-number", message->update_number, 32);
    if (message->incremental_update_number == -1)
    {
        stream->write_bool("complete-update", true);
    }
    else
    {
        stream->write_bool("complete-update", false);
        stream->write_integer("incremental-update-number", message->incremental_update_number, 32);
        stream->write_integer("baseline-checksum", message->baseline_checksum, 32);
    }
    stream->write_integer("peer-count", message->peer_count, 5);
    stream->write_integer("player-count", message->player_count, 5);
    for (long peer_index = 0; peer_index < message->peer_count; peer_index++)
    {
        const s_network_message_membership_update_peer *peer = &message->peers[peer_index];
        stream->write_integer("peer-index", peer->peer_index, 4);
        stream->write_integer("peer-update-type", peer->update_type, 1);
        if (peer->update_type == 1)
        {
            stream->write_integer("peer-connection-state", peer->connection_state, 4);
            stream->write_bool("peer-info-updated", peer->info_updated);
            if (peer->info_updated)
            {
                stream->write_raw_data("peer-address", &peer->address, 288);
                stream->write_raw_data("peer-party-nonce", &peer->party_nonce, 64);
                stream->write_raw_data("peer-join-nonce", &peer->join_nonce, 64);
                stream->write_integer("network-version-number", peer->network_version_number, 32);
                stream->write_integer("peer-creation-timestamp", peer->creation_timestamp, 32);
            }
            stream->write_bool("peer-properties-updated", peer->properties_updated);
            if (peer->properties_updated)
            {
                stream->write_bool("peer-name-updated", peer->properties.name_updated);
                if (peer->properties.name_updated)
                {
                    stream->write_string_wchar("peer-name", peer->properties.name, 16);
                    stream->write_string_wchar("peer-session-name", peer->properties.session_name, 32);
                }
                stream->write_bool("has-hard-drive", peer->properties.has_hard_drive);
                stream->write_integer("ready-hopper-id", peer->properties.ready_hopper_id, 16);
                stream->write_integer("game-start-error", peer->properties.game_start_error, 6);
                stream->write_bool("unknown-updated", peer->properties.unknown_updated);
                if (peer->properties.unknown_updated)
                    stream->write_integer("unknown-value", peer->properties.unknown_value, 32);
                stream->write_bool("peer-map-updated", peer->properties.map_updated);
                if (peer->properties.map_updated)
                {
                    stream->write_integer("peer-map-status", peer->properties.map_status, 3);
                    stream->write_integer("peer-map-progress-percentage", peer->properties.map_progress_percentage, 7);
                }
                stream->write_bool("peer-connection-updated", peer->properties.connection_updated);
                if (peer->properties.connection_updated)
                {
                    stream->write_bool("estimated-bandwidth-present", peer->properties.bandwidth != -1);
                    if (peer->properties.bandwidth != -1)
                        stream->write_raw_data("estimated-bandwidth", &peer->properties.bandwidth, 64);
                }
                stream->write_bool("bandwidth-updated", peer->properties.bandwidth_updated);
                if (peer->properties.bandwidth_updated)
                    stream->write_integer("bandwidth-value", peer->properties.bandwidth_value, 32);
                stream->write_bool("addresses-updated", peer->properties.addresses_updated);
                if (peer->properties.addresses_updated)
                {
                    stream->write_raw_data("data", &peer->properties.address, 32);
                    stream->write_raw_data("data", &peer->properties.online_address, 32);
                    stream->write_bool("peer-address-flag", peer->properties.address_flag);
                    stream->write_integer("nat-type", peer->properties.nat_type, 2);
                    stream->write_integer("connectivity-badness-rating", peer->properties.connectivity_badness_rating, 7);
                    stream->write_integer("host-badness-rating", peer->properties.host_badness_rating, 7);
                    stream->write_integer("client-badness-rating", peer->properties.client_badness_rating, 7);
                    stream->write_integer("language", peer->properties.language, 4);
                    stream->write_integer("peer-connectivity-mask", peer->properties.connectivity_mask, 16);
                    stream->write_integer("peer-probe-mask", peer->properties.probe_mask, 16);
                    stream->write_integer("peer-latency-min", peer->properties.latency_min, 11);
                    stream->write_integer("peer-latency-est", peer->properties.latency_est, 11);
                    stream->write_integer("peer-latency-max", peer->properties.latency_max, 11);
                }
                stream->write_bool("values-updated", peer->properties.values_updated);
                if (peer->properties.values_updated)
                {
                    stream->write_integer("determinism-version", peer->properties.determinism_version, 8);
                    stream->write_integer("determinism-compatible-version", peer->properties.determinism_compatible_version, 8);
                }
                stream->write_bool("flags-updated", peer->properties.flags_updated);
                if (peer->properties.flags_updated)
                    stream->write_integer("flags", peer->properties.flags, 17);
            }
        }
    }
    for (long player_index = 0; player_index < message->player_count; player_index++)
    {
        const s_network_message_membership_update_player *player = &message->players[player_index];
        stream->write_integer("player-index", player->player_index, 4);
        stream->write_integer("update-type", player->update_type, 1);
        if (player->update_type == 1)
        {
            stream->write_bool("player-location-updated", player->location_updated);
            if (player->location_updated)
            {
                stream->write_raw_data("identifier", &player->identifier, 64);
                stream->write_integer("peer-index", (short)player->peer_index, 4);
                stream->write_integer("peer-user-index", (short)player->peer_user_index, 2);
                stream->write_integer("player-addition-number", player->player_addition_number, 20);
                stream->write_bool("player-occupies-a-public-slot", player->occupies_a_public_slot);
            }
            stream->write_bool("player-properties-updated", player->properties_updated);
            if (player->properties_updated)
            {
                stream->write_integer("player-update-number", player->player_update_number, 32);
                stream->write_integer("controller-index", player->controller_index + 1, 3);
                player_configuration_client_encode_packet(stream,
                    (const s_network_game_definitions_client_configuration *)&player->configuration.client,
                    _player_configuration_encoding_full);
                player_configuration_host_encode(
                    stream, &player->configuration.host, _player_configuration_encoding_full);
                stream->write_integer("player-voice", player->voice, 32);
            }
        }
    }
    stream->write_bool("player-addition-number-updated", message->player_addition_number_updated);
    if (message->player_addition_number_updated)
        stream->write_integer("player-addition-number", message->player_addition_number, 20);
    stream->write_bool("leader-updated", message->leader_updated);
    if (message->leader_updated)
        stream->write_integer("leader-peer_index", message->leader_peer_index, 4);
    stream->write_bool("host-updated", message->host_updated);
    if (message->host_updated)
        stream->write_integer("host-peer_index", message->host_peer_index, 4);
    stream->write_bool("slot-counts-updated", message->slot_counts_updated);
    if (message->slot_counts_updated)
    {
        stream->write_integer("private-slot-count", message->private_slot_count, 5);
        stream->write_integer("public-slot-count", message->public_slot_count, 5);
        stream->write_bool("are-slots-locked", message->are_slots_locked);
        stream->write_bool("unknown-27bd", message->request_modification);
    }
    stream->write_integer("checksum", message->checksum, 32);
}

typedef void (*session_membership_encode_proc)(c_bitstream *, long, const void *);
typedef bool (*session_membership_decode_proc)(c_bitstream *, long, void *);
typedef bool (*session_membership_compare_proc)(long, void *, void *);
typedef void (*session_membership_dispose_proc)(long, void *);
// @address 0x8c4450 g_network_message_type_collection
static __forceinline void session_membership_register_type(c_network_message_type_collection *c, long id, const char *name, long flags,
    long minimum_size, long maximum_size, session_membership_encode_proc encode, session_membership_decode_proc decode,
    session_membership_compare_proc compare, session_membership_dispose_proc dispose)
{
    s_network_message_type_definition *type = &c->m_message_types[id];
    type->name = name;
    type->flags = flags;
    type->minimum_size = minimum_size;
    type->maximum_size = maximum_size;
    type->encode = encode;
    type->decode = decode;
    type->compare = compare;
    type->dispose = dispose;
    type->initialized = 1;
}
// @address 0x3f2c70 network_message_types_register_session_membership
extern "C" void network_message_types_register_session_membership(c_network_message_type_collection *message_collection)
{
    c_network_message_type_collection **home = &message_collection;
    c_network_message_type_collection *c = g_network_message_type_collection;
    session_membership_register_type(c, 26, "membership-update", 0, 10184, 10184, &c_network_message_membership_update::encode,
        &c_network_message_membership_update::decode, 0, 0);
    session_membership_register_type(c, 27, "peer-properties", 0, 240, 240, &c_network_message_peer_properties::encode,
        &c_network_message_peer_properties::decode, 0, 0);
    session_membership_register_type(c, 28, "delegate-leadership", 0, 44, 44, &c_network_message_delegate_leadership::encode,
        &c_network_message_delegate_leadership::decode, 0, 0);
    session_membership_register_type(c, 29, "boot-machine", 0, 48, 48, &c_network_message_boot_machine::encode, &c_network_message_boot_machine::decode, 0, 0);
    session_membership_register_type(c, 30, "player-add", 0, 240, 240, &c_network_message_player_add::encode, &c_network_message_player_add::decode, 0, 0);
    session_membership_register_type(
        c, 31, "player-refuse", 0, 24, 24, &c_network_message_player_refuse::encode, &c_network_message_player_refuse::decode, 0, 0);
    session_membership_register_type(
        c, 32, "player-remove", 0, 12, 12, &c_network_message_player_remove::encode, &c_network_message_player_remove::decode, 0, 0);
    session_membership_register_type(c, 33, "player-properties", 0, 232, 232, &c_network_message_player_properties::encode,
        &c_network_message_player_properties::decode, 0, 0);
}
