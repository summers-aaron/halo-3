// networkinglibxbox/network_messages_session_protocol.obj: from the records 0x3ef768 0x3ef9f0 0x3efcf8 0x3efd48 0x3efdf0 0x3efe30 0x3efec0 0x3eff00 0x3eff90 0x3f0010 0x3f0050 0x3f00e0 0x3f0188 0x3f01d8 0x3f0240 0x3f0300 0x3f0370 0x3f0440 0x3f04b8 0x3f0548 0x3f0658 0x3f06b8 0x3f0798 0x3f0828 0x3f0948 0x3f0968
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "memory/bitstream.h"
#include <string.h>
#include "networking/logic/logic_matchmaking_desirability.h"
#include "networking/messages/network_messages_session_protocol.h"
#include "networking/network_game_definitions.h"
#include "networking/network_time.h"
#include "networking/session/network_session_parameters_ui.h"
#include "networking/transport/transport_security.h"
#include "game/player_types.h"
#include "networking/messages/network_messages_session_protocol_types.h"

#include "networking/player_configuration_encoding_types.h"
#include "networking/messages/network_message_type_collection.h"
#include "networking/transport/transport_security_types.h"
#include "cseries/crt_xdk.h"
#include "networking/network_globals.h"
#include "phys/phys_thread_memory.h"

static __forceinline void read_secure_address(c_bitstream *stream, const char *name, s_transport_secure_address *address)
{
    stream->read_raw_data(name, address, sizeof(*address) * 8);
}

static __forceinline void write_secure_address(c_bitstream *stream, const char *name, const s_transport_secure_address *address)
{
    stream->write_raw_data(name, address, sizeof(*address) * 8);
}
static __forceinline void network_messages_session_protocol_global_stats_encode(c_bitstream *stream, const s_queried_player_global_statistics *statistics)
{
    stream->write_bool("global-stats-valid", statistics->valid);
    if (statistics->valid)
    {
        stream->write_integer("global-stats-experience-base", statistics->experience_base, 17);
        stream->write_signed_integer("global-stats-experience-penalty", statistics->experience_penalty, 18);
        stream->write_integer("global-stats-highest-skill", statistics->highest_skill, 6);
    }
}
static __forceinline void network_messages_session_protocol_hopper_stats_encode(c_bitstream *stream, const s_queried_player_hopper_statistics *statistics)
{
    stream->write_bool("hopper-stats-valid", statistics->valid);
    if (statistics->valid)
    {
        stream->write_integer("hopper-identifier", statistics->hopper_identifier, 16);
        unsigned long mu_bits;
        memcpy(&mu_bits, &statistics->mu, sizeof(mu_bits));
        stream->write_integer("hopper-mu", mu_bits, 32);
        unsigned long sigma_bits;
        memcpy(&sigma_bits, &statistics->sigma, sizeof(sigma_bits));
        stream->write_integer("hopper-sigma", sigma_bits, 32);
        stream->write_integer("hopper-old-skill", statistics->old_skill, 6);
        stream->write_integer("hopper-games-played", statistics->games_played, 17);
        stream->write_integer("hopper-games-completed", statistics->games_completed, 17);
        stream->write_integer("hopper-games-won", statistics->games_won, 17);
    }
}
static __forceinline void network_messages_session_protocol_search_party_preferences_encode(
    c_bitstream *stream, const s_matchmaking_search_party_preferences *preferences)
{
    stream->write_integer("skill-preference", preferences->type, 2);
    if (preferences->type == 1)
    {
        stream->write_integer("skill-minimum", preferences->minimum, 6);
        stream->write_integer("skill-maximum", preferences->maximum, 6);
    }
}
// @address 0x3ef768 ?encode@c_network_message_join_request@@SAXPAVc_bitstream@@JPBX@Z
void c_network_message_join_request::encode(c_bitstream *stream, long message_storage_size, const void *message_storage)
{
    const s_network_message_join_request *message = (const s_network_message_join_request *)message_storage;
    stream->write_integer("protocol", message->protocol, 16);
    stream->write_integer("minimum-network-version", (unsigned short)message->minimum_network_version, 16);
    stream->write_integer("current-network-version", (unsigned short)message->current_network_version, 16);
    stream->write_raw_data("session-id", &message->session_id, 64);
    stream->write_raw_data("join-nonce", &message->join_nonce, 64);
    stream->write_raw_data("join-party-nonce", &message->join_party_nonce, 64);
    stream->write_integer("joining-peer-count", message->joining_peer_count, 5);
    for (long peer_index = 0; peer_index < message->joining_peer_count; peer_index++)
    {
        write_secure_address(stream, "joining-peer-address", &message->joining_peers[peer_index].address);
        stream->write_integer("joining-network-version-number", (unsigned short)message->joining_peers[peer_index].network_version, 16);
        for (long player_index = 0; player_index < 4; player_index++)
        {
            bool player_valid = message->joining_peers[peer_index].player_indices[player_index] + 1 != 0;
            stream->write_bool("joining-peer-player-valid", player_valid);
            if (player_valid)
            {
                stream->write_integer("user-player-index", message->joining_peers[peer_index].player_indices[player_index], 4);
            }
        }
    }
    stream->write_integer("joining-player-count", message->joining_player_count, 5);
    for (long player_index = 0; player_index < message->joining_player_count; player_index++)
    {
        const s_network_session_join_request_player *player = &message->joining_players[player_index];
        stream->write_raw_data("joining-peer-player-id", player->player_identifier.data, 64);
        stream->write_raw_data("joining-peer-player-xuids", &player->player_xuid, 64);
        stream->write_bool("joining-peer-is-silver-or-gold-live", player->is_silver_or_gold_live);
        stream->write_bool("joining-peer-is-online-enabled", player->is_online_enabled);
        stream->write_bool("joining-peer-unknown12", player->is_guest);
        stream->write_bool("joining-peer-unknown13", player->is_bot);
        network_messages_session_protocol_global_stats_encode(stream, &player->global_statistics);
        network_messages_session_protocol_hopper_stats_encode(stream, &player->hopper_statistics);
    }
    stream->write_bool("join-from-recruiting", message->join_from_recruiting);
    stream->write_integer("payload-type", message->payload_type, 2);
    if (message->payload_type == 2)
    {
        const s_matchmaking_search_party_properties *search_properties = &message->search_properties;
        stream->write_integer("search-seconds", search_properties->seconds, 10);
        stream->write_integer("search-latency", search_properties->latency, 13);
        network_messages_session_protocol_search_party_preferences_encode(stream, &search_properties->preferences);
        network_matchmaking_session_properties_encode(stream, &search_properties->session_properties);
    }
}
static __forceinline long network_messages_session_protocol_read_signed_integer(c_bitstream *packet, const char *name, long size_in_bits)
{
    long value = packet->read_integer(name, size_in_bits);
    if (size_in_bits < 32 && (unsigned long)(value & (1 << (size_in_bits - 1))) != 0)
        value |= ~((1 << size_in_bits) - 1);
    return value;
}
static __forceinline bool network_messages_session_protocol_global_stats_decode(
    c_bitstream *stream, s_queried_player_global_statistics *statistics, e_player_configuration_encoding_type encoding_type)
{
    statistics->valid = stream->read_bool("global-stats-valid");
    if (statistics->valid)
    {
        if (encoding_type == _player_configuration_encoding_full)
        {
            statistics->experience_base = stream->read_integer("global-stats-experience-base", 17);
            unsigned long experience_penalty = stream->read_integer("global-stats-experience-penalty", 18);
            if (experience_penalty & 0x20000)
                experience_penalty |= 0xfffc0000;
            statistics->experience_penalty = experience_penalty;
        }
        statistics->highest_skill = stream->read_integer("global-stats-highest-skill", 6);
    }
    return true;
}
// @address 0x3ef9f0 ?decode@c_network_message_join_request@@SA_NPAVc_bitstream@@JPAX@Z
bool c_network_message_join_request::decode(c_bitstream *stream, long message_storage_size, void *message_storage)
{
    s_network_message_join_request *message = (s_network_message_join_request *)message_storage;
    bool success = true;
    message->protocol = (unsigned short)stream->read_integer("protocol", 16);
    message->minimum_network_version = (short)stream->read_integer("minimum-network-version", 16);
    message->current_network_version = (short)stream->read_integer("current-network-version", 16);
    stream->read_raw_data("session-id", &message->session_id, 64);
    stream->read_raw_data("join-nonce", &message->join_nonce, 64);
    stream->read_raw_data("join-party-nonce", &message->join_party_nonce, 64);
    message->joining_peer_count = stream->read_integer("joining-peer-count", 5);
    for (long peer_index = 0; peer_index < message->joining_peer_count; peer_index++)
    {
        read_secure_address(stream, "joining-peer-address", &message->joining_peers[peer_index].address);
        message->joining_peers[peer_index].network_version = (short)stream->read_integer("joining-network-version-number", 16);
        for (long player_index = 0; player_index < 4; player_index++)
        {
            bool player_valid = stream->read_bool("joining-peer-player-valid");
            if (player_valid)
            {
                message->joining_peers[peer_index].player_indices[player_index] = stream->read_integer("user-player-index", 4);
            }
            else
            {
                message->joining_peers[peer_index].player_indices[player_index] = -1;
            }
        }
    }
    message->joining_player_count = stream->read_integer("joining-player-count", 5);
    for (long player_index = 0; player_index < message->joining_player_count; player_index++)
    {
        s_network_session_join_request_player *player = &message->joining_players[player_index];
        stream->read_raw_data("joining-peer-player-id", player->player_identifier.data, 64);
        stream->read_raw_data("joining-peer-player-xuids", &player->player_xuid, 64);
        player->is_silver_or_gold_live = stream->read_bool("joining-peer-is-silver-or-gold-live");
        player->is_online_enabled = stream->read_bool("joining-peer-is-online-enabled");
        player->is_guest = stream->read_bool("joining-peer-unknown12");
        player->is_bot = stream->read_bool("joining-peer-unknown13");
        success = success && network_messages_session_protocol_global_stats_decode(stream, &player->global_statistics, _player_configuration_encoding_full);
        success = success && player_configuration_client_hopper_stats_decode(stream, &player->hopper_statistics);
    }
    message->join_from_recruiting = stream->read_bool("join-from-recruiting");
    message->payload_type = stream->read_integer("payload-type", 2);
    if (message->payload_type == 2)
    {
        success = success && network_matchmaking_search_party_properties_decode(stream, &message->search_properties);
    }
    success = success && !stream->error_occurred();
    return success;
}

// @address 0x3efcf8 ?encode@c_network_message_peer_connect@@SAXPAVc_bitstream@@JPBX@Z
void c_network_message_peer_connect::encode(c_bitstream *stream, long message_storage_size, const void *message_storage)
{
    const s_network_message_peer_connect *message = (const s_network_message_peer_connect *)message_storage;
    stream->write_integer("protocol", message->protocol, 16);
    stream->write_raw_data("session-id", &message->session_id, 64);
    stream->write_raw_data("join-nonce", &message->join_nonce, 64);
}

// @address 0x3efd48 ?decode@c_network_message_peer_connect@@SA_NPAVc_bitstream@@JPAX@Z
bool c_network_message_peer_connect::decode(c_bitstream *stream, long message_storage_size, void *message_storage)
{
    s_network_message_peer_connect *message = (s_network_message_peer_connect *)message_storage;
    bool success = true;
    message->protocol = (unsigned short)stream->read_integer("protocol", 16);
    stream->read_raw_data("session-id", &message->session_id, 64);
    stream->read_raw_data("join-nonce", &message->join_nonce, 64);
    success = success && !stream->error_occurred();
    return success;
}

// @address 0x3efdf0 ?encode@c_network_message_join_abort@@SAXPAVc_bitstream@@JPBX@Z
void c_network_message_join_abort::encode(c_bitstream *stream, long message_storage_size, const void *message_storage)
{
    const s_network_message_join_abort *message = (const s_network_message_join_abort *)message_storage;
    stream->write_raw_data("session-id", &message->session_id, 64);
    stream->write_raw_data("join-nonce", &message->join_nonce, 64);
}

// @address 0x3efe30 ?decode@c_network_message_join_abort@@SA_NPAVc_bitstream@@JPAX@Z
bool c_network_message_join_abort::decode(c_bitstream *stream, long message_storage_size, void *message_storage)
{
    s_network_message_join_abort *message = (s_network_message_join_abort *)message_storage;
    bool success = true;
    stream->read_raw_data("session-id", &message->session_id, 64);
    stream->read_raw_data("join-nonce", &message->join_nonce, 64);
    success = success && !stream->error_occurred();
    return success;
}

// @address 0x3efec0 ?encode@c_network_message_join_refuse@@SAXPAVc_bitstream@@JPBX@Z
void c_network_message_join_refuse::encode(c_bitstream *stream, long message_storage_size, const void *message_storage)
{
    const s_network_message_join_refuse *message = (const s_network_message_join_refuse *)message_storage;
    stream->write_raw_data("session-id", &message->session_id, 64);
    stream->write_integer("reason", message->reason, 5);
}

// @address 0x3eff00 ?decode@c_network_message_join_refuse@@SA_NPAVc_bitstream@@JPAX@Z
bool c_network_message_join_refuse::decode(c_bitstream *stream, long message_storage_size, void *message_storage)
{
    s_network_message_join_refuse *message = (s_network_message_join_refuse *)message_storage;
    stream->read_raw_data("session-id", &message->session_id, 64);
    message->reason = (e_network_join_refuse_reason)stream->read_integer("reason", 5);
    bool success = !stream->error_occurred();
    return success;
}

// @address 0x3eff90 ?encode@c_network_message_peer_reestablish@@SAXPAVc_bitstream@@JPBX@Z
void c_network_message_peer_reestablish::encode(c_bitstream *stream, long message_storage_size, const void *message_storage)
{
    const s_network_message_peer_reestablish *message = (const s_network_message_peer_reestablish *)message_storage;
    stream->write_raw_data("session-id", &message->session_id, 64);
}

// @address 0x3f0010 ?encode@c_network_message_session_boot@@SAXPAVc_bitstream@@JPBX@Z
void c_network_message_session_boot::encode(c_bitstream *stream, long message_storage_size, const void *message_storage)
{
    const s_network_message_session_boot *message = (const s_network_message_session_boot *)message_storage;
    stream->write_raw_data("session-id", &message->session_id, 64);
    stream->write_integer("reason", message->reason, 4);
}

// @address 0x3f0050 ?decode@c_network_message_session_boot@@SA_NPAVc_bitstream@@JPAX@Z
bool c_network_message_session_boot::decode(c_bitstream *stream, long message_storage_size, void *message_storage)
{
    s_network_message_session_boot *message = (s_network_message_session_boot *)message_storage;
    bool success = true;
    stream->read_raw_data("session-id", &message->session_id, 64);
    message->reason = stream->read_integer("reason", 4);
    success = success && !stream->error_occurred();
    return success;
}

// @address 0x3f00e0 ?decode@c_network_message_host_handoff@@SA_NPAVc_bitstream@@JPAX@Z
bool c_network_message_host_handoff::decode(c_bitstream *stream, long message_storage_size, void *message_storage)
{
    s_network_message_host_handoff *message = (s_network_message_host_handoff *)message_storage;
    bool success = true;
    stream->read_raw_data("session-id", &message->session_id, 64);
    message->host_peer_index = (short)stream->read_integer("host-peer-index", 4);
    read_secure_address(stream, "host-address", &message->host_address);
    success = success && !stream->error_occurred();
    return success;
}

// @address 0x3f0188 ?encode@c_network_message_host_handoff@@SAXPAVc_bitstream@@JPBX@Z
void c_network_message_host_handoff::encode(c_bitstream *stream, long message_storage_size, const void *message_storage)
{
    const s_network_message_host_handoff *message = (const s_network_message_host_handoff *)message_storage;
    stream->write_raw_data("session-id", &message->session_id, 64);
    stream->write_integer("host-peer-index", message->host_peer_index, 4);
    write_secure_address(stream, "host-address", &message->host_address);
}

// @address 0x3f01d8 ?encode@c_network_message_host_reestablish@@SAXPAVc_bitstream@@JPBX@Z
void c_network_message_host_reestablish::encode(c_bitstream *stream, long message_storage_size, const void *message_storage)
{
    const s_network_message_host_reestablish *message = (const s_network_message_host_reestablish *)message_storage;
    stream->write_raw_data("session-id", &message->session_id, 64);
    stream->write_raw_data("new-session-id", &message->new_session_id, 64);
    stream->write_raw_data("new-session-host-address", &message->new_session_host_address, 288);
    stream->write_raw_data("new-session-key", &message->new_session_key, 128);
    stream->write_integer("secure-key-platform", message->secure_key_platform, 1);
}

// @address 0x3f0240 ?decode@c_network_message_host_reestablish@@SA_NPAVc_bitstream@@JPAX@Z
bool c_network_message_host_reestablish::decode(c_bitstream *stream, long message_storage_size, void *message_storage)
{
    s_network_message_host_reestablish *message = (s_network_message_host_reestablish *)message_storage;
    bool success = true;
    stream->read_raw_data("session-id", &message->session_id, 64);
    stream->read_raw_data("new-session-id", &message->new_session_id, 64);
    stream->read_raw_data("new-session-host-address", &message->new_session_host_address, 288);
    stream->read_raw_data("new-session-key", &message->new_session_key, 128);
    message->secure_key_platform = stream->read_integer("secure-key-platform", 1);
    success = success && !stream->error_occurred();
    return success;
}

// @address 0x3f0300 ?encode@c_network_message_host_decline@@SAXPAVc_bitstream@@JPBX@Z
void c_network_message_host_decline::encode(c_bitstream *stream, long message_storage_size, const void *message_storage)
{
    const s_network_message_host_decline *message = (const s_network_message_host_decline *)message_storage;
    stream->write_raw_data("session-id", &message->session_id, 64);
    stream->write_bool("session-exists", message->session_exists);
    if ((signed char)message->session_exists)
    {
        stream->write_bool("peer-exists", message->peer_exists);
        stream->write_bool("host-exists", message->host_exists);
        if ((signed char)message->host_exists)
        {
            write_secure_address(stream, "host-address", &message->host_address);
        }
    }
}

// @address 0x3f0370 ?decode@c_network_message_host_decline@@SA_NPAVc_bitstream@@JPAX@Z
bool c_network_message_host_decline::decode(c_bitstream *stream, long message_storage_size, void *message_storage)
{
    s_network_message_host_decline *message = (s_network_message_host_decline *)message_storage;
    bool success = true;
    stream->read_raw_data("session-id", &message->session_id, 64);
    message->session_exists = stream->read_bool("session-exists");
    if (message->session_exists)
    {
        message->peer_exists = stream->read_bool("peer-exists");
        message->host_exists = stream->read_bool("host-exists");
        if (message->host_exists)
        {
            read_secure_address(stream, "host-address", &message->host_address);
        }
    }
    success = success && !stream->error_occurred();
    return success;
}

// @address 0x3f0440 ?decode@c_network_message_session_disband@@SA_NPAVc_bitstream@@JPAX@Z
bool c_network_message_session_disband::decode(c_bitstream *stream, long message_storage_size, void *message_storage)
{
    s_network_message_session_disband *message = (s_network_message_session_disband *)message_storage;
    bool success = true;
    stream->read_raw_data("session-id", &message->session_id, 64);
    success = success && !stream->error_occurred();
    return success;
}

// @address 0x3f04b8 ?encode@c_network_message_election@@SAXPAVc_bitstream@@JPBX@Z
void c_network_message_election::encode(c_bitstream *stream, long message_storage_size, const void *message_storage)
{
    const s_network_message_election *message = (const s_network_message_election *)message_storage;
    stream->write_raw_data("session-id", &message->session_id, 64);
    write_secure_address(stream, "host-address", &message->host_address);
    stream->write_integer("host-update-number", message->host_update_number, 16);
    stream->write_integer("peer-count", message->peer_count, 5);
    for (long peer_index = 0; peer_index < message->peer_count; peer_index++)
    {
        write_secure_address(stream, "peer-address", &message->peer_addresses[peer_index]);
        stream->write_integer("peer-update-number", message->peer_update_numbers[peer_index], 16);
    }
}

// @address 0x3f0548 ?decode@c_network_message_election@@SA_NPAVc_bitstream@@JPAX@Z
bool c_network_message_election::decode(c_bitstream *stream, long message_storage_size, void *message_storage)
{
    s_network_message_election *message = (s_network_message_election *)message_storage;
    stream->read_raw_data("session-id", &message->session_id, 64);
    read_secure_address(stream, "host-address", &message->host_address);
    message->host_update_number = stream->read_integer("host-update-number", 16);
    message->peer_count = stream->read_integer("peer-count", 5);
    bool success = (unsigned long)message->peer_count <= 16;
    if (success)
    {
        for (long peer_index = 0; peer_index < message->peer_count; peer_index++)
        {
            read_secure_address(stream, "peer-address", &message->peer_addresses[peer_index]);
            message->peer_update_numbers[peer_index] = stream->read_integer("peer-update-number", 16);
        }
    }
    success = success && !stream->error_occurred();
    return success;
}

// @address 0x3f0658 ?encode@c_network_message_election_refuse@@SAXPAVc_bitstream@@JPBX@Z
void c_network_message_election_refuse::encode(c_bitstream *stream, long message_storage_size, const void *message_storage)
{
    const s_network_message_election_refuse *message = (const s_network_message_election_refuse *)message_storage;
    stream->write_raw_data("session-id", &message->session_id, 64);
    stream->write_integer("refusal-reason", message->refusal_reason, 4);
    stream->write_bool("alternate-host-valid", message->alternate_host_valid);
    if ((signed char)message->alternate_host_valid)
    {
        stream->write_raw_data("alternate-host-address", &message->alternate_host_address, sizeof(s_transport_secure_address) * 8);
    }
}

// @address 0x3f06b8 ?decode@c_network_message_election_refuse@@SA_NPAVc_bitstream@@JPAX@Z
bool c_network_message_election_refuse::decode(c_bitstream *stream, long message_storage_size, void *message_storage)
{
    s_network_message_election_refuse *message = (s_network_message_election_refuse *)message_storage;
    bool success = true;
    stream->read_raw_data("session-id", &message->session_id, 64);
    message->refusal_reason = stream->read_integer("refusal-reason", 4);
    message->alternate_host_valid = stream->read_bool("alternate-host-valid");
    if (message->alternate_host_valid)
    {
        read_secure_address(stream, "alternate-host-address", &message->alternate_host_address);
    }
    success = success && !stream->error_occurred();
    success = success && message->refusal_reason > 0 && message->refusal_reason < 11;
    return success;
}


static __forceinline unsigned long network_messages_session_protocol_encode_timestamps(c_bitstream *stream, const s_network_message_time_synchronize *message)
{
    if ((unsigned long)message->synchronization_stage >= 1)
    {
        unsigned long timestamp = network_session_time_get_time(&message->session_id);
        stream->write_integer("client-timestamp-0", message->client_timestamp[0], 32);
        stream->write_integer("authority-timestamp-0", message->authority_timestamp[0], 32);
        return timestamp;
    }
    else
    {
        return GetTickCount();
    }
}

// @address 0x3f0798 ?encode@c_network_message_time_synchronize@@SAXPAVc_bitstream@@JPBX@Z
void c_network_message_time_synchronize::encode(c_bitstream *stream, long message_storage_size, const void *message_storage)
{
    const s_network_message_time_synchronize *message = static_cast<const s_network_message_time_synchronize *>(message_storage);
    stream->write_raw_data("session-id", &message->session_id, 64);
    stream->write_integer("synchronization-stage", message->synchronization_stage, 1);
    unsigned long timestamp = network_messages_session_protocol_encode_timestamps(stream, message);
    stream->write_integer("timestamp", timestamp, 32);
}

// @address 0x3f0828 ?decode@c_network_message_time_synchronize@@SA_NPAVc_bitstream@@JPAX@Z
bool c_network_message_time_synchronize::decode(c_bitstream *stream, long message_storage_size, void *message_storage)
{
    s_network_message_time_synchronize *message = (s_network_message_time_synchronize *)message_storage;
    bool success = true;
    stream->read_raw_data("session-id", &message->session_id, 64);
    message->synchronization_stage = stream->read_integer("synchronization-stage", 1);
    success = success && !stream->error_occurred();
    success = success && message->synchronization_stage >= 0 && message->synchronization_stage < 2;
    if (success)
    {
        if ((unsigned long)message->synchronization_stage >= 1)
        {
            message->client_timestamp[0] = stream->read_integer("client-timestamp-0", 32);
            message->client_timestamp[1] = GetTickCount();
            message->authority_timestamp[0] = stream->read_integer("authority-timestamp-0", 32);
            message->authority_timestamp[1] = stream->read_integer("authority-timestamp-1", 32);
        }
        else
        {
            message->client_timestamp[0] = stream->read_integer("client-timestamp-0", 32);
            message->client_timestamp[1] = -1;
            message->authority_timestamp[0] = network_session_time_get_time((const s_transport_secure_identifier *)message);
            message->authority_timestamp[1] = -1;
        }
    }
    return success;
}

typedef void (*session_protocol_encode_proc)(c_bitstream *, long, const void *);
typedef bool (*session_protocol_decode_proc)(c_bitstream *, long, void *);
typedef bool (*session_protocol_compare_proc)(long, void *, void *);
typedef void (*session_protocol_dispose_proc)(long, void *);
// @address 0x8c4450 g_network_message_type_collection
static __forceinline void network_messages_session_protocol_register_type(c_network_message_type_collection *c, long id, const char *name,
    long flags, long minimum_size, long maximum_size, session_protocol_encode_proc encode, session_protocol_decode_proc decode,
    session_protocol_compare_proc compare, session_protocol_dispose_proc dispose)
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
// @address 0x3f0968 ?network_message_types_register_session_protocol@@YAXPAVc_network_message_type_collection@@@Z
void network_message_types_register_session_protocol(c_network_message_type_collection *message_collection)
{
    c_network_message_type_collection **home = &message_collection;
    c_network_message_type_collection *c = g_network_message_type_collection;
    (void)home;
    network_messages_session_protocol_register_type(
        c, 8, "join-request", 0, 0x890, 0x890, &c_network_message_join_request::encode, &c_network_message_join_request::decode, 0, 0);
    network_messages_session_protocol_register_type(
        c, 9, "peer-connect", 0, 24, 24, &c_network_message_peer_connect::encode, &c_network_message_peer_connect::decode, 0, 0);
    network_messages_session_protocol_register_type(
        c, 10, "join-abort", 0, 16, 16, &c_network_message_join_abort::encode, &c_network_message_join_abort::decode, 0, 0);
    network_messages_session_protocol_register_type(
        c, 11, "join-refuse", 0, 12, 12, &c_network_message_join_refuse::encode, &c_network_message_join_refuse::decode, 0, 0);
    network_messages_session_protocol_register_type(c, 12, "leave-session", 0, 8, 8, &c_network_message_peer_reestablish::encode,
        (session_protocol_decode_proc)ui_game_mode_decode_function, 0, 0);
    network_messages_session_protocol_register_type(c, 13, "leave-acknowledge", 0, 8, 8, &c_network_message_peer_reestablish::encode,
        (session_protocol_decode_proc)ui_game_mode_decode_function, 0, 0);
    network_messages_session_protocol_register_type(
        c, 14, "session-disband", 0, 8, 8, &c_network_message_peer_reestablish::encode, &c_network_message_session_disband::decode, 0, 0);
    network_messages_session_protocol_register_type(
        c, 15, "session-boot", 0, 12, 12, &c_network_message_session_boot::encode, &c_network_message_session_boot::decode, 0, 0);
    network_messages_session_protocol_register_type(
        c, 16, "host-handoff", 0, 46, 46, &c_network_message_host_handoff::encode, &c_network_message_host_handoff::decode, 0, 0);
    network_messages_session_protocol_register_type(
        c, 17, "peer-handoff", 0, 46, 46, &c_network_message_host_handoff::encode, &c_network_message_host_handoff::decode, 0, 0);
    network_messages_session_protocol_register_type(
        c, 18, "host-transition", 0, 8, 8, &c_network_message_peer_reestablish::encode, &c_network_message_session_disband::decode, 0, 0);
    network_messages_session_protocol_register_type(c, 19, "host-reestablish", 0, 72, 72, &c_network_message_host_reestablish::encode,
        &c_network_message_host_reestablish::decode, 0, 0);
    network_messages_session_protocol_register_type(
        c, 20, "host-decline", 0, 48, 48, &c_network_message_host_decline::encode, &c_network_message_host_decline::decode, 0, 0);
    network_messages_session_protocol_register_type(
        c, 21, "peer-reestablish", 0, 8, 8, &c_network_message_peer_reestablish::encode, &c_network_message_session_disband::decode, 0, 0);
    network_messages_session_protocol_register_type(
        c, 22, "peer-establish", 0, 8, 8, &c_network_message_peer_reestablish::encode, &c_network_message_session_disband::decode, 0, 0);
    network_messages_session_protocol_register_type(
        c, 23, "election", 0, 0x2b4, 0x2b4, &c_network_message_election::encode, &c_network_message_election::decode, 0, 0);
    network_messages_session_protocol_register_type(
        c, 24, "election-refuse", 0, 52, 52, &c_network_message_election_refuse::encode, &c_network_message_election_refuse::decode, 0, 0);
    network_messages_session_protocol_register_type(c, 25, "time-synchronize", 0, 28, 28, &c_network_message_time_synchronize::encode,
        &c_network_message_time_synchronize::decode, &c_network_message_time_synchronize::compare,
        (session_protocol_dispose_proc)&halo_external_breakpoint);
}

// @address 0x3f0948 ?compare@c_network_message_time_synchronize@@SA_NJPAX0@Z
bool c_network_message_time_synchronize::compare(long message_storage_size, void *message_storage, void *comparison_storage) {
    s_network_message_time_synchronize *message = static_cast<s_network_message_time_synchronize *>(comparison_storage);
    memset(message->client_timestamp, -1, sizeof(message->client_timestamp));
    memset(message->authority_timestamp, -1, sizeof(message->authority_timestamp));
    return true;
}

