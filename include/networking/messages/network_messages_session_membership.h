#ifndef NETWORKING_MESSAGES_NETWORK_MESSAGES_SESSION_MEMBERSHIP_H
#define NETWORKING_MESSAGES_NETWORK_MESSAGES_SESSION_MEMBERSHIP_H

// The session membership messages (types 26..33 of the message type collection) and their encoders/decoders,
// defined in tu/networkinglibxbox/network_messages_session_membership.cpp. The registration sizes in
// network_message_types_register_session_membership (10184, 240, 44, 48, 240, 24, 12, 232) fix each layout; the
// 8-byte alignment of the player configuration and of the peer properties (they hold 64-bit fields) accounts for
// every gap between the fields.

#include "game/player_configuration.h"
#include "game/player_identifier_types.h"
#include "networking/in_addr_types.h"
#include "networking/transport/transport_security.h"
#include "networking/transport/transport_security_types.h"

class c_bitstream;
class c_network_message_type_collection;

// The peer properties a peer sends its host (peer-properties) and the host replicates (membership-update).
struct s_network_message_peer_properties_data
{
    wchar_t peer_name[16];
    wchar_t peer_session_name[32];
    long estimated_upstream_bandwidth;
    long estimated_downstream_bandwidth;
    long map_status;
    long map_progress_percentage;
    unsigned __int64 bandwidth;
    long ready_hopper_id;
    long game_start_error;
    bool has_hard_drive;
    in_addr address;
    in_addr online_address;
    bool address_flag;
    long nat_type;
    long connectivity_badness_rating;
    long host_badness_rating;
    long client_badness_rating;
    unsigned short connectivity_mask;
    unsigned short probe_mask;
    long latency_min;
    long latency_est;
    long latency_max;
    long language;
    long determinism_version;
    long determinism_compatible_version;
    unsigned long flags;
};

struct s_network_message_peer_properties
{
    s_transport_secure_identifier session_id;
    s_transport_secure_address secure_address;
    s_network_message_peer_properties_data properties;
};

struct s_network_message_delegate_leadership
{
    s_transport_secure_identifier session_id;
    s_transport_secure_address desired_leader_address;
};

#include "networking/messages/network_messages_session_players.h"

// membership-update: one peer's changes; the properties group starts at an 8-byte boundary (its bandwidth is 64-bit).
struct s_network_message_membership_update_peer_properties
{
    bool name_updated;
    unsigned short name[16];
    unsigned short session_name[32];
    bool has_hard_drive;
    unsigned short ready_hopper_id;
    long game_start_error;
    bool unknown_updated;
    long unknown_value;
    bool map_updated;
    long map_status;
    long map_progress_percentage;
    bool connection_updated;
    __int64 bandwidth;
    bool bandwidth_updated;
    long bandwidth_value;
    bool addresses_updated;
    in_addr address;
    in_addr online_address;
    bool address_flag;
    long nat_type;
    long connectivity_badness_rating;
    long host_badness_rating;
    long client_badness_rating;
    long language;
    unsigned short connectivity_mask;
    unsigned short probe_mask;
    long latency_min;
    long latency_est;
    long latency_max;
    bool values_updated;
    long determinism_version;
    long determinism_compatible_version;
    bool flags_updated;
    unsigned long flags;
};

struct s_network_message_membership_update_peer
{
    s_network_message_membership_update_peer() { properties.flags = 0; }
    long peer_index;
    long update_type;
    long connection_state;
    bool info_updated;
    s_transport_secure_address address;
    unsigned __int64 party_nonce;
    unsigned __int64 join_nonce;
    long network_version_number;
    long creation_timestamp;
    bool properties_updated;
    s_network_message_membership_update_peer_properties properties;
};

struct s_network_message_membership_update_player
{
    long player_index;
    long update_type;
    bool location_updated;
    s_player_identifier identifier;
    unsigned short peer_index;
    unsigned short peer_user_index;
    long player_addition_number;
    bool occupies_a_public_slot;
    bool properties_updated;
    long player_update_number;
    long controller_index;
    s_player_configuration configuration;
    unsigned long voice;
};

struct s_network_message_membership_update
{
    s_network_message_membership_update();
    s_transport_secure_identifier session_id;
    long update_number;
    long incremental_update_number;
    long baseline_checksum;
    short peer_count;
    short player_count;
    s_network_message_membership_update_peer peers[16];
    s_network_message_membership_update_player players[16];
    bool player_addition_number_updated;
    long player_addition_number;
    bool leader_updated;
    long leader_peer_index;
    bool host_updated;
    long host_peer_index;
    bool slot_counts_updated;
    long private_slot_count;
    long public_slot_count;
    bool are_slots_locked;
    bool request_modification;
    long checksum;
};

typedef char network_messages_session_membership_size_peer_properties[(sizeof(s_network_message_peer_properties) == 240) ? 1 : -1];
typedef char network_messages_session_membership_size_delegate_leadership[(sizeof(s_network_message_delegate_leadership) == 44) ? 1 : -1];
typedef char network_messages_session_membership_size_boot_machine[(sizeof(s_network_message_boot_machine) == 48) ? 1 : -1];
typedef char network_messages_session_membership_size_player_add[(sizeof(s_network_message_player_add) == 240) ? 1 : -1];
typedef char network_messages_session_membership_size_player_refuse[(sizeof(s_network_message_player_refuse) == 24) ? 1 : -1];
typedef char network_messages_session_membership_size_player_remove[(sizeof(s_network_message_player_remove) == 12) ? 1 : -1];
typedef char network_messages_session_membership_size_player_properties[(sizeof(s_network_message_player_properties) == 232) ? 1 : -1];
typedef char network_messages_session_membership_size_update_peer[(sizeof(s_network_message_membership_update_peer) == 0x138) ? 1 : -1];
typedef char network_messages_session_membership_size_update_player[(sizeof(s_network_message_membership_update_player) == 0x140) ? 1 : -1];
typedef char network_messages_session_membership_size_update[(sizeof(s_network_message_membership_update) == 10184) ? 1 : -1];

class c_network_message_membership_update
{
public:
    static void encode(c_bitstream *packet, long message_storage_size, const void *message_storage);
    static bool decode(c_bitstream *packet, long message_storage_size, void *message_storage);
};

class c_network_message_peer_properties
{
public:
    static bool decode(c_bitstream *packet, long message_storage_size, void *message_storage);
    static void encode(c_bitstream *packet, long message_storage_size, const void *message_storage);
};

class c_network_message_delegate_leadership
{
public:
    static bool decode(c_bitstream *packet, long message_storage_size, void *message_storage);
    static void encode(c_bitstream *packet, long message_storage_size, const void *message_storage);
};

class c_network_message_boot_machine
{
public:
    static bool decode(c_bitstream *packet, long message_storage_size, void *message_storage);
    static void encode(c_bitstream *packet, long message_storage_size, const void *message_storage);
};

class c_network_message_player_add
{
public:
    static bool decode(c_bitstream *packet, long message_storage_size, void *message_storage);
    static void encode(c_bitstream *packet, long message_storage_size, const void *message_storage);
};

class c_network_message_player_refuse
{
public:
    static bool decode(c_bitstream *packet, long message_storage_size, void *message_storage);
    static void encode(c_bitstream *packet, long message_storage_size, const void *message_storage);
};

class c_network_message_player_remove
{
public:
    static bool decode(c_bitstream *packet, long message_storage_size, void *message_storage);
    static void encode(c_bitstream *packet, long message_storage_size, const void *message_storage);
};

class c_network_message_player_properties
{
public:
    static bool decode(c_bitstream *packet, long message_storage_size, void *message_storage);
    static void encode(c_bitstream *packet, long message_storage_size, const void *message_storage);
};

extern "C" void network_message_types_register_session_membership(c_network_message_type_collection *message_collection);

#endif // NETWORKING_MESSAGES_NETWORK_MESSAGES_SESSION_MEMBERSHIP_H
