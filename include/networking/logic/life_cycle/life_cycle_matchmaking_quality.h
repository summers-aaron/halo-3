#ifndef NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_MATCHMAKING_QUALITY_H
#define NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_MATCHMAKING_QUALITY_H

#include "networking/logic/logic_matchmaking_seeker.h"
#include "networking/session/network_session_parameter_matchmaking_types.h"

#include "networking/in_addr_types.h"
#include "networking/transport/transport_qos_types.h"

struct s_matchmaking_quality_address {
    in_addr ina;
    unsigned long ina_online;
    unsigned short port_online;
    unsigned char ab_online[20];
};
struct s_matchmaking_quality_qos_sample {
    s_matchmaking_quality_address address;
    s_transport_qos_result qos_result;
};

class c_matchmaking_quality
{
public:
    enum { k_serialized_data_size = 0x9870 };
    void initialize();
    void add_qos_result(const s_transport_session_description *session_description, const s_transport_qos_result *qos_result);
    void submit_to_webstats();
    __forceinline void set_arbitration_time(unsigned long time) { m_arbitration_time = time; }
    __forceinline void set_prepare_map_time(unsigned long time) { m_prepare_map_time = time; }
    __forceinline void set_host_selection_time(unsigned long time) { m_host_selection_time = time; }
    __forceinline void set_left_host_selection() { m_left_host_selection = true; }
    __forceinline void set_aborted() { m_aborted = true; }
    __forceinline void set_left_arbitration() { m_left_arbitration = true; }
    __forceinline void set_not_enough_hosts() { m_not_enough_hosts = true; }
    __forceinline void set_registration_timed_out() { m_registration_timed_out = true; }
    __forceinline void set_completion_timed_out() { m_completion_timed_out = true; }
    __forceinline void set_left_prepare_map() { m_left_prepare_map = true; }
    __forceinline void add_session_search(const s_matchmaking_session_search *search)
    {
        if (m_session_search_count < 16)
        {
            s_matchmaking_session_search *session_search = &m_session_searches[m_session_search_count];
            *session_search = *search;
            ++m_session_search_count;
        }
    }

    void set_matchmaking_quality_address_from_secure_address(
        const s_transport_secure_address *secure_address,
        s_matchmaking_quality_address *matchmaking_quality_address_out);

    bool m_needs_submission;
    // March initialize() skips these bytes (its memset starts at m_aborted); preserved for layout.
    unsigned char : 8;
    unsigned char : 8;
    unsigned char : 8;
    unsigned char : 8;
    unsigned char : 8;
    unsigned char : 8;
    unsigned char : 8;
    bool m_aborted;
    unsigned char m_gather_timed_out;
    unsigned char m_left_assemble;
    bool m_left_arbitration;
    bool m_not_enough_hosts;
    bool m_left_host_selection;
    bool m_left_prepare_map;
    bool m_vetoed;
    bool m_registration_timed_out;
    bool m_completion_timed_out;
    bool m_event_9_pending;
    bool m_event_10_pending;
    bool m_event_10_alternate_pending;
    s_game_hopper_picked_game m_vetoed_game;
    long m_ping;
    long m_search_time;
    long m_gather_time;
    unsigned long m_arbitration_time;
    unsigned long m_host_selection_time;
    unsigned long m_prepare_map_time;
    long in_match_time;
    s_matchmaking_quality_address m_local_address;
    long m_qos_sample_count;
    c_static_array<s_matchmaking_quality_qos_sample, 400> m_qos_samples;
    long m_session_search_count;
    s_matchmaking_session_search m_session_searches[16];
    bool hopper_data_valid;
    s_transport_qos_result hopper_data;
    bool nat_type_valid;
    long nat_type;
};

typedef char matchmaking_quality_m_aborted_offset[(offsetof(c_matchmaking_quality, m_aborted) == 8) ? 1 : -1];
typedef char matchmaking_quality_m_arbitration_time_offset[(offsetof(c_matchmaking_quality, m_arbitration_time) == 0x90) ? 1 : -1];
typedef char matchmaking_quality_m_prepare_map_time_offset[(offsetof(c_matchmaking_quality, m_prepare_map_time) == 0x98) ? 1 : -1];
typedef char matchmaking_quality_m_host_selection_time_offset[(offsetof(c_matchmaking_quality, m_host_selection_time) == 0x94) ? 1 : -1];
typedef char matchmaking_quality_m_left_host_selection_offset[(offsetof(c_matchmaking_quality, m_left_host_selection) == 0xd) ? 1 : -1];
typedef char matchmaking_quality_in_match_time_offset[(offsetof(c_matchmaking_quality, in_match_time) == 0x9c) ? 1 : -1];
typedef char matchmaking_quality_m_session_search_count_offset[
    (sizeof(void *) != 4 || ((offsetof(c_matchmaking_quality, m_session_search_count) == 0x64c4))) ? 1 : -1];
typedef char matchmaking_quality_hopper_data_valid_offset[(sizeof(void *) != 4 || ((offsetof(c_matchmaking_quality, hopper_data_valid) == 0x9848))) ? 1 : -1];

typedef char matchmaking_quality_qos_count_offset[(offsetof(c_matchmaking_quality, m_qos_sample_count) == 0xc0) ? 1 : -1];
typedef char matchmaking_quality_qos_samples_offset[(sizeof(void *) != 4 || ((offsetof(c_matchmaking_quality, m_qos_samples) == 0xc4))) ? 1 : -1];
typedef char matchmaking_quality_ping_offset[(offsetof(c_matchmaking_quality, m_ping) == 0x84) ? 1 : -1];
typedef char matchmaking_quality_event_9_offset[(offsetof(c_matchmaking_quality, m_event_9_pending) == 0x12) ? 1 : -1];
typedef char matchmaking_quality_event_10_offset[(offsetof(c_matchmaking_quality, m_event_10_pending) == 0x13) ? 1 : -1];
typedef char matchmaking_quality_event_10_alternate_offset[
    (offsetof(c_matchmaking_quality, m_event_10_alternate_pending) == 0x14) ? 1 : -1];
typedef char matchmaking_quality_size[(sizeof(void *) != 4 || ((sizeof(c_matchmaking_quality) == 0x9874))) ? 1 : -1];
#endif
