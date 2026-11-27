#ifndef NETWORKING_SESSION_NETWORK_SESSION_MEMBERSHIP_H
#define NETWORKING_SESSION_NETWORK_SESSION_MEMBERSHIP_H

#include <stddef.h>

// c_network_session_membership: who is in a session (peers and players), held at c_network_session+0x20, 0x4ee8
// bytes. The header March 2007's asserts name (networking\session\network_session_membership.h:678..1012).
//
// Shape from the March debug build: a current and a transmitted s_network_session_shared_membership (March
// a current membership and a transmitted membership, laid out one after the other at
// +0x8 and +0x2538, network_memory.cpp 0x3b61b8), the local peer state after them, then the player add queue.
// Names follow the accessors where a field has
// one, else from the units' copies (disputes in the lane TY8 report).
//
// Methods: every method March emits for the class, in March's order (the file's methods by assert line, then the
// header's inlines, then the remaining accessors), keeping each declaration's
// signature. Bodies appear only for the inline accessors; the rest are prototypes.

#include "cseries/cseries_wrapped.h"
#include "memory/static_arrays.h"
#include "networking/session/network_session_membership_types.h"
#include "networking/transport/transport_security.h"

enum e_network_session_peer_state;
enum e_network_session_peer_properties_status_flags;
enum e_network_session_type;
enum e_peer_property_flag_test_type;
enum e_controller_index;

class c_network_session;
struct transport_address;
struct s_transport_unique_identifier;
struct s_network_message_membership_update;
struct s_network_message_membership_update_peer_properties;

#include "networking/session/network_session_peer_types.h"

// The part of the membership the host sends to its peers (checksummed by fast_checksum, built into updates).
struct s_network_session_shared_membership
{
    s_network_session_shared_membership();
    long update_number;                            // March update_number()
    long leader_peer_index;                        // March leader_peer_index()
    long host_peer_index;                          // March host_peer_index()
    long private_slot_count;                       // March private_slot_count()
    long public_slot_count;                        // March public_slot_count()
    bool friends_only;                             // true when only friends may fill the open slots
    bool request_modification;
    long peer_count;                               // March get_peer_count()
    c_static_flags<16> valid_peer_mask;            // March is_peer_valid (c_static_flags_no_init<16>::test)
    s_network_session_peer peers[16];
    long player_count;                             // March get_player_count()
    c_static_flags<16> valid_player_mask;          // March is_player_valid
    s_network_session_player players[16];
    long player_sequence_number;
};

// One local peer's view of a peer: March get_peer_needs_reestablishment / get_observer_channel_index /
// get_membership_update_number read +0 / +4 / +8 of a 12-byte entry per peer.
struct s_peer_channel
{
    bool peer_needs_reestablishment;
    long observer_channel_index;
    long membership_update_number;
};

struct s_player_add_queue_entry
{
    s_player_identifier player_identifier;
    long peer_index;
    long peer_user_index;
    long local_user_index;
    e_controller_index controller_index;
    s_player_configuration_from_client player_data;
    unsigned long flags;
};

class c_network_session_membership
{
public:
    // match: 0x3b61b8: without __forceinline the session ctor calls ??0c_network_session_membership instead of
    // inlining the two shared-membership ctors and the player-add-queue memset loop.
    __forceinline c_network_session_membership() {}
    // networking\session\network_session_membership.cpp (by assert line)
    // The next two setters update the current membership and bump the update number; their bodies
    // are here (location dispute). March asserts and the event of set_peer_connection_state are compiled out.
    void set_leader_peer_index(long leader_peer_index) { m_current_membership.leader_peer_index = leader_peer_index; increment_update(); }
    void set_peer_connection_state(long peer_index, e_network_session_peer_state connection_state)
    {
        m_current_membership.peers[peer_index].connection_state = connection_state;
        if (connection_state == 8) // established
            set_join_nonce(peer_index, 0);
        increment_update();
    }
    bool add_peer(long peer_index, e_network_session_peer_state connection_state, long peer_version, const s_transport_secure_address *secure_address,
        unsigned __int64 party_nonce, unsigned __int64 join_nonce);
    void remove_peer(long peer_index);
    long get_peer_from_incoming_address(const transport_address *incoming_address) const;
    long get_peer_from_unique_identifier(const s_transport_unique_identifier *identifier) const;
    long get_peer_from_secure_address(const s_transport_secure_address *secure_address) const;
    bool host_exists_at_incoming_address(const transport_address *incoming_address) const;
    long get_next_peer(long peer_index) const;
    unsigned long get_established_peer_valid_mask() const;
    void set_peer_address(long peer_index, const s_transport_secure_address *address);
    void set_peer_properties(long peer_index, const s_network_session_peer_properties *properties);
    long get_player_from_identifier(const s_player_identifier *identifier) const;
    // left_game marks a player who already left the game
    long find_or_add_player(long peer_index, long user_index, const s_player_identifier *player_identifier, unsigned __int64 player_xuid,
        bool is_online, bool is_silver, bool is_guest, bool is_bot, bool left_game);
    void remove_player(long player_index);
    unsigned long get_established_player_valid_mask() const;
    long get_next_player(long player_index) const;
    void build_membership_update(const s_network_session_shared_membership *current, const s_network_session_shared_membership *previous,
        s_network_message_membership_update *update) const;
    bool handle_membership_update(const s_network_message_membership_update *message);
    bool peer_property_flag_test(e_peer_property_flag_test_type type, e_network_session_peer_properties_status_flags flag) const;
    __forceinline bool is_player_in_player_add_queue(const s_player_identifier *identifier) const { return find_player_in_player_add_queue(identifier) != -1; }
    bool add_player_to_player_add_queue(const s_player_identifier *player_identifier, long peer_index, long peer_user_index, long local_user_index,
        e_controller_index controller_index, const s_player_configuration_from_client *player_data_from_client, unsigned long flags);
    void remove_player_from_player_add_queue(const s_player_identifier *identifier);
    void commit_player_from_player_add_queue(const s_player_identifier *identifier);
    void verify_membership() const;
    void verify_local_state() const;

    // networking\session\network_session_membership.h (March inlines, by assert line)
    s_network_session_peer *get_local_peer() { return &m_current_membership.peers[m_local_peer_index]; }
    const s_network_session_peer *get_local_peer() const { return &m_current_membership.peers[m_local_peer_index]; }
    s_network_session_peer *get_host_peer() { return &m_current_membership.peers[m_current_membership.host_peer_index]; }
    const s_network_session_peer *get_host_peer() const { return &m_current_membership.peers[m_current_membership.host_peer_index]; }
    const s_network_session_peer *get_leader_peer() const { return &m_current_membership.peers[m_current_membership.leader_peer_index]; }
    s_network_session_peer *get_peer(long peer_index) { return &m_current_membership.peers[peer_index]; }
    const s_network_session_peer *get_peer(long peer_index) const { return &m_current_membership.peers[peer_index]; }
    // March: NULL unless the peer's user has a player (March calls get_peer and get_player; written against the
    // fields, which network_session_host find_good_peer_to_be_host 0x4260c8 needs: through the accessors 3 registers move)
    const s_network_session_player *get_player_from_peer(long peer_index, long user_index) const
    {
        const s_network_session_peer *peer = &m_current_membership.peers[peer_index];
        const s_network_session_player *player = 0;
        if (peer->user_player_indices[user_index] != -1)
            player = &m_current_membership.players[peer->user_player_indices[user_index]];
        return player;
    }
    long get_observer_channel_index(long peer_index) const { return m_peer_channels[peer_index].observer_channel_index; }
    long get_host_observer_channel_index() const { return m_peer_channels[m_current_membership.host_peer_index].observer_channel_index; }
    long get_membership_update_number(long peer_index) const { return m_peer_channels[peer_index].membership_update_number; }
    void set_membership_update_number(long peer_index, long update_number) { m_peer_channels[peer_index].membership_update_number = update_number; }
    bool get_peer_needs_reestablishment(long peer_index) const { return m_peer_channels[peer_index].peer_needs_reestablishment; }
    void set_peer_needs_reestablishment(long peer_index,
        bool needs_reestablishment) { m_peer_channels[peer_index].peer_needs_reestablishment = needs_reestablishment; }
    const s_player_identifier *get_player_identifier(long player_index) const { return &m_current_membership.players[player_index].identifier; }
    unsigned __int64 get_player_xuid(long player_index) const { return m_current_membership.players[player_index].configuration.client.xuid; }
    // March: the xuid only for a silver/gold player on another peer or a local player (body not written: unused)
    unsigned __int64 get_usable_player_xuid(long player_index) const;
    bool is_player_silver_or_gold_live(long player_index) const { return m_current_membership.players[player_index].configuration.client.online; }
    bool is_player_online_enabled(long player_index) const { return m_current_membership.players[player_index].configuration.client.silver; }
    const s_player_configuration *get_player_data(long player_index) const { return &m_current_membership.players[player_index].configuration; }
    s_network_session_player *get_player(long player_index) { return &m_current_membership.players[player_index]; }
    const s_network_session_player *get_player(long player_index) const { return &m_current_membership.players[player_index]; }

    // no assert names the file (March emits them; the simple accessors are inlines there)
    bool all_peers_established() const;
    bool all_player_experience_ratings_are_set() const;
    bool clone_for_connect(const c_network_session_membership *membership, c_network_session *network_session);
    bool commit_players();
    bool does_player_exist(const s_player_identifier *identifier, long peer_index, long user_index) const;
    bool has_membership() const { return m_current_membership.update_number != -1; }
    // March: a result local that stays false unless the membership exists (and has a host)
    bool is_host() const
    {
        bool result = false;
        if (has_membership() && m_current_membership.host_peer_index != -1)
            result = m_local_peer_index == m_current_membership.host_peer_index;
        return result;
    }
    bool is_leader() const
    {
        bool result = false;
        if (has_membership())
            result = m_local_peer_index == m_current_membership.leader_peer_index;
        return result;
    }
    bool is_peer_a_bad_client(long peer_index) const;
    bool is_peer_connected(long peer_index) const;
    // True when the peer's connection state is established.
    bool is_peer_established(long peer_index) const { return m_current_membership.peers[peer_index].connection_state == 8; }
    bool is_peer_joined(long peer_index) const;
    bool is_peer_valid(long peer_index) const { return m_current_membership.valid_peer_mask.test(peer_index); }
    bool is_peer_visible_in_roster(long peer_index) const;
    bool is_player_established(long player_index) const { return is_peer_established(get_player(player_index)->peer_index); }
    bool is_player_valid(long player_index) const { return m_current_membership.valid_player_mask.test(player_index); }
    bool is_remote_peer(long peer_index) const { return get_observer_channel_index(peer_index) != -1; }
    bool peer_property_flag_test_any_peer(e_network_session_peer_properties_status_flags flag) const;
    c_network_session *get_session() { return m_session; }
    const c_network_session *get_session() const { return m_session; }
    const s_network_session_peer *get_peers() const { return m_current_membership.peers; }
    const s_network_session_peer *get_raw_peer(long peer_index) const { return &m_current_membership.peers[peer_index]; }
    const s_network_session_player *get_players() const { return m_current_membership.players; }
    const s_network_session_player *get_raw_player(long player_index) const { return &m_current_membership.players[player_index]; }
    const s_network_session_shared_membership *get_current_membership() const { return &m_current_membership; }
    const s_network_session_shared_membership *get_transmitted_membership() const { return &m_transmitted_membership; }
    __forceinline const s_player_add_queue_entry *get_first_player_from_player_add_queue() const
    {
        const s_player_add_queue_entry *result = 0;
        if (m_player_add_queue_first != m_player_add_queue_count)
            result = &m_player_add_queue[m_player_add_queue_first];
        return result;
    }
    const s_player_add_queue_entry *get_player_add_queue_entry(long index) const;
    const s_transport_secure_address *get_peer_address(long peer_index) const { return &m_current_membership.peers[peer_index].secure_address; }
    e_network_session_peer_state get_peer_connection_state(long peer_index) const { return get_peer(peer_index)->connection_state; }
    e_network_session_type session_type() const;
    long get_established_peer_count() const;
    long get_established_player_count() const;
    long get_first_peer() const;
    long get_first_player() const;
    long get_peer_count() const { return m_current_membership.peer_count; }
    long get_peer_from_observer_channel(long observer_channel_index) const;
    long get_peer_index_of_player_in_queue(const s_player_identifier *identifier) const;
    long get_player_count() const { return m_current_membership.player_count; }
    long host_peer_index() const { return m_current_membership.host_peer_index; }
    long host_transition_peer_index() const { return m_host_transition_peer_index; }
    long leader_peer_index() const { return m_current_membership.leader_peer_index; }
    long local_membership_update_number() const { return m_local_membership_update_number; }
    long local_peer_index() const { return m_local_peer_index; }
    long private_slot_count() const { return m_current_membership.private_slot_count; }
    long public_slot_count() const { return m_current_membership.public_slot_count; }
    long update_number() const { return m_current_membership.update_number; }
    s_network_session_peer *get_raw_peer(long peer_index) { return &m_current_membership.peers[peer_index]; }
    s_network_session_player *get_players() { return m_current_membership.players; }
    s_network_session_player *get_raw_player(long player_index) { return &m_current_membership.players[player_index]; }
    unsigned __int64 get_join_nonce(long peer_index) const { return get_peer(peer_index)->join_nonce; }
    unsigned long get_creation_timestamp(long peer_index) const { return get_peer(peer_index)->join_start_time; }
    unsigned long get_peer_valid_mask() const { return m_current_membership.valid_peer_mask.get_bits_direct()[0]; }
    unsigned long get_player_valid_mask() const { return m_current_membership.valid_player_mask.get_bits_direct()[0]; }
    // March inlines (by-value copies of the masks)
    c_static_flags<16> get_peer_valid_flags() const { return m_current_membership.valid_peer_mask; }
    c_static_flags<16> get_player_valid_flags() const { return m_current_membership.valid_player_mask; }
    void clear_player_properties(long player_index);
    void clear_update_number() { m_current_membership.update_number = 0; }
    void copy_current_to_transmitted();
    void dump_membership(const s_network_session_shared_membership *membership) const;
    void flush_all_local_state() { m_player_add_queue_first = 0; m_player_add_queue_count = 0; }
    void idle();
    void increment_update() { ++m_current_membership.update_number; ++m_local_membership_update_number; }
    void lock_slots();
    void player_data_updated() { increment_update(); }
    __forceinline void network_session_set_slot_counts(long public_slot_count, long private_slot_count, bool friends_only);
    void reset_membership();
    void reset_transmitted_membership_state();
    void set_host_peer_index(long peer_index) { m_current_membership.host_peer_index = peer_index; }
    void set_host_transition_peer_index(long peer_index) { m_host_transition_peer_index = peer_index; }
    void set_join_nonce(long peer_index, unsigned __int64 join_nonce) { m_current_membership.peers[peer_index].join_nonce = join_nonce; }
    void set_local_peer_index(long peer_index) { m_local_peer_index = peer_index; }
    void set_peer_party_nonce(long peer_index, unsigned __int64 party_nonce) { m_current_membership.peers[peer_index].party_nonce = party_nonce; }
    void set_player_host_properties(long player_index, const s_player_configuration_from_host *configuration);
    void set_player_properties(long player_index, long desired_configuration_version, e_controller_index controller_index,
        const s_player_configuration_from_client *configuration, unsigned long voice_settings);
    void set_session(c_network_session *session) { m_session = session; }
    // Set the slot counts and the friends-only flag. The first count is stored at
    // private_slot_count (March private_slot_count() reads it; the units called it public: dispute)
    void set_slot_counts(long private_slot_count, long public_slot_count, bool friends_only);
    void set_update_number();
    void update_player_data(long player_index, const s_player_configuration *configuration);

private:
    friend class c_network_session;
    bool add_peer_internal(long peer_index, e_network_session_peer_state connection_state, long peer_version, const s_transport_secure_address *secure_address,
        unsigned __int64 party_nonce, unsigned __int64 join_nonce, unsigned long join_start_time);
    void remove_peer_internal(long peer_index);
    s_network_session_player *add_player_internal(long player_index, const s_player_identifier *identifier, long peer_index, long peer_user_index,
        long player_sequence_number, bool left_game);
    void remove_player_internal(long player_index);
    void update_player_configuration_from_host(long player_index);
    long find_player_in_player_add_queue(const s_player_identifier *identifier) const;
    void clear();
    static void apply_peer_properties_update(s_network_session_peer_properties *properties, const s_network_message_membership_update_peer_properties *update);
    static void build_peer_properties_update(const s_network_session_peer_properties *properties, const s_network_session_peer_properties *previous,
        s_network_message_membership_update_peer_properties *update);

public:  // access not attested for the fields; units still read and write several directly
    c_network_session *m_session;                          // March get_session / set_session
    s_network_session_shared_membership m_current_membership;     // March get_current_membership
    s_network_session_shared_membership m_transmitted_membership; // March get_transmitted_membership
    unsigned long m_transmitted_baseline_checksum;         // copy_current_to_transmitted stores its fast_checksum
    bool m_host_reestablished;
    bool m_committed;                                      // March commit_players clears it
    bool m_dirty;
    long m_host_transition_peer_index;                     // March host_transition_peer_index
    long m_local_peer_index;                               // March local_peer_index
    long m_local_membership_update_number;                 // March local_membership_update_number
    s_peer_channel m_peer_channels[16];
    s_player_add_queue_entry m_player_add_queue[4];
    long m_player_add_queue_first;                         // March flush_all_local_state clears both
    long m_player_add_queue_count;
};

typedef char network_session_membership_size_peer_properties[(sizeof(s_network_session_peer_properties) == 0xc0) ? 1 : -1];
typedef char network_session_membership_size_peer[(sizeof(s_network_session_peer) == 0x118) ? 1 : -1];
typedef char network_session_membership_size_player[(sizeof(s_network_session_player) == 0x138) ? 1 : -1];
typedef char network_session_membership_size_shared[(sizeof(s_network_session_shared_membership) == 0x2530) ? 1 : -1];
typedef char network_session_membership_size_queue_entry[(sizeof(s_player_add_queue_entry) == 0xe8) ? 1 : -1];
typedef char network_session_membership_size[(sizeof(c_network_session_membership) == 0x4ee8) ? 1 : -1];
typedef char network_session_membership_offset_transmitted[(offsetof(c_network_session_membership, m_transmitted_membership) == 0x2538) ? 1 : -1];
typedef char network_session_membership_offset_local_peer[(offsetof(c_network_session_membership, m_local_peer_index) == 0x4a74) ? 1 : -1];
typedef char network_session_membership_offset_queue[(offsetof(c_network_session_membership, m_player_add_queue) == 0x4b40) ? 1 : -1];

// Functions defined in tu/networkinglibxbox/network_session_membership.cpp that other units call.

#include "networking/session/network_session_membership_api.h"

#endif // NETWORKING_SESSION_NETWORK_SESSION_MEMBERSHIP_H
