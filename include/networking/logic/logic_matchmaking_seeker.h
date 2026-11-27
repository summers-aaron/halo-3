#ifndef NETWORKING_LOGIC_LOGIC_MATCHMAKING_SEEKER_H
#define NETWORKING_LOGIC_LOGIC_MATCHMAKING_SEEKER_H

#include "networking/online/online_context_types.h"
#include "networking/online/online_search_parameters.h"

#include "cseries/cseries_types.h"
#include "cseries/cseries_wrapped.h"
#include "memory/static_arrays.h"
#include "networking/logic/logic_matchmaking_desirability.h"
#include "networking/logic/logic_matchmaking_seeker_search_types.h"
#include <stddef.h>
#include "networking/online/online_nat_types.h"

enum e_network_session_matchmaking_composition_flags
{
    _network_session_matchmaking_composition_flag_0,
    _network_session_matchmaking_composition_flag_1,
    _network_session_matchmaking_composition_flag_2,
    k_network_session_matchmaking_composition_flag_count = 4
};

enum e_matchmaking_session_search_query_flags
{
    _matchmaking_session_search_query_flag_0,
    _matchmaking_session_search_query_flag_1,
    _matchmaking_session_search_query_final_strict_step,
    _matchmaking_session_search_query_progressive,
    k_matchmaking_session_search_query_flag_count = 9
};

enum { _online_nat_type_none = _online_nat_type_unknown };

enum e_matchmaking_seeker_flags
{
    _matchmaking_seeker_flag_0,
    _matchmaking_seeker_flag_1,
    _matchmaking_seeker_online_search_in_progress_bit,
    _matchmaking_seeker_query_failed_bit,
    _matchmaking_seeker_online_search_results_being_tracked_bit,
    _matchmaking_seeker_flag_5,
    _matchmaking_seeker_flag_6,
    k_matchmaking_seeker_flag_count
};

struct s_matchmaking_session_search_query
{
    long search_type;
    long step;
    unsigned short hopper_identifier;
    long minimum_skill;
    long maximum_skill;
    long hopper_ranking;
    bool composition_flag_2;
    long nat_type;
    long minimum_average_skill_level;
    long maximum_average_skill_level;
    real minimum_average_mu;
    real maximum_average_mu;
    long minimum_average_experience;
    long maximum_average_experience;
    long language;
    long country;
    long region;
    long maximum_latency;
    bool composition_flag_0;
    long preference;
    c_flags<e_matchmaking_session_search_query_flags, short, k_matchmaking_session_search_query_flag_count> flags;
    short pad52;
    long pad54;
};

#pragma pack(push, 4)

struct s_matchmaking_session_search {          /* 824 bytes */
    long result_count;
    unsigned long start_time;
    unsigned long end_time;
    long failure_count;
    long repeat_count;
    unsigned long failure_time;
    long search_type;
    long uncontacted;
    long contacting;
    long contacted;
    long unsuitable;
    c_static_array<long, 12> unsuitable_reasons;
    long suitable;
    c_static_array<long, 2> suitable_reasons;
    long join_failures[20];
    s_matchmaking_session_search_query query;
    s_online_session_search_parameters online_query;
    long pad334;
};
#pragma pack(pop)

class c_matchmaking_quality;
struct s_suitable_matchmaking_session;

class c_matchmaking_seeker
{
public:
    c_flags<e_matchmaking_seeker_flags, unsigned char, k_matchmaking_seeker_flag_count> m_flags;
    long m_current_session_search_index;
    long m_search_state;
    long m_search_state_timeout;
    long m_sessions_found;
    long m_session_search_failure_count;
    long m_session_search_count;
    long m_sessions_contacted;
    long m_sessions_failed;
    unsigned long m_initial_search_start_time;
    long m_last_search_time;
    long pad44;
    s_matchmaking_session_search m_session_searches[16];
    s_matchmaking_session_properties m_session_properties;
    e_matchmaking_search_preference m_search_preference;
    bool m_ranked;
    long m_game_mode;
    long m_game_type;
public:
    __forceinline unsigned long get_initial_search_start_time() const { return m_initial_search_start_time; }
    __forceinline bool search_in_progress() const { return (m_flags.m_storage & 1) != 0; }
    __forceinline long get_total_sessions_contacted_count() const
    {
        long count = 0;
        if (search_in_progress()) count = m_sessions_contacted + m_sessions_failed;
        return count;
    }
    __forceinline long get_current_sessions_joinable_count() const
    {
        long count = 0;
        if (m_current_session_search_index >= 0)
            count = m_session_searches[m_current_session_search_index].contacted;
        return count;
    }
    __forceinline long get_session_search_stage() const { return m_search_state; }
    __forceinline long get_total_session_search_count() const
    {
        long count = 0;
        if (search_in_progress()) count = m_session_search_count;
        return count;
    }
    __forceinline void get_current_skill_range(long *minimum_skill, long *maximum_skill) const
    {
        *minimum_skill = 1;
        *maximum_skill = 1;
        if (m_current_session_search_index >= 0)
        {
            *minimum_skill = m_session_searches[m_current_session_search_index].query.minimum_skill;
            *maximum_skill = m_session_searches[m_current_session_search_index].query.maximum_skill;
        }
    }
    void search_begin(const s_matchmaking_seeker_search_start_parameters *search_start_parameters,
        const s_matchmaking_session_properties *session_properties, c_matchmaking_quality *matchmaking_quality);
    bool get_session_to_join(s_suitable_matchmaking_session *suitable_matchmaking_session);
    void add_session_searches_to_matchmaking_quality(c_matchmaking_quality *matchmaking_quality);
    unsigned long get_current_search_start_time() const;
    void set_session_properties(const s_matchmaking_session_properties *session_properties);
    void update();
private:
    void set_search_properties(const s_matchmaking_session_properties *session_properties, e_matchmaking_search_preference search_preference);
    void mark_current_session_search_query_failed();
    void update_tracked_sessions();
    void build_query_strict_skill(long step, s_matchmaking_session_search_query *query);
    void build_online_query_from_query(const s_matchmaking_session_search_query *query, s_online_session_search_parameters *parameters);
    void update_online_session_search();
    void update_tracked_session_statistics();
    void update_online_session_search_initiate();
    void build_query_skill(long step, s_matchmaking_session_search_query *query);
    void build_query_any(long step, s_matchmaking_session_search_query *query);
};

typedef char matchmaking_online_property_size[(sizeof(s_online_property) == 24) ? 1 : -1];
typedef char matchmaking_search_join_failures_offset[(offsetof(s_matchmaking_session_search, join_failures) == 0x68) ? 1 : -1];
typedef char matchmaking_seeker_search_index_offset[(offsetof(c_matchmaking_seeker, m_current_session_search_index) == 4) ? 1 : -1];
typedef char matchmaking_seeker_searches_offset[(offsetof(c_matchmaking_seeker, m_session_searches) == 0x30) ? 1 : -1];
typedef char matchmaking_session_search_query_size[(sizeof(s_matchmaking_session_search_query) == 0x58) ? 1 : -1];
typedef char matchmaking_session_search_size[(sizeof(s_matchmaking_session_search) == 0x338) ? 1 : -1];
typedef char matchmaking_session_search_query_offset[(offsetof(s_matchmaking_session_search, query) == 0xb8) ? 1 : -1];
typedef char matchmaking_session_search_online_query_offset[(offsetof(s_matchmaking_session_search, online_query) == 0x110) ? 1 : -1];

enum e_online_data_type;
extern e_online_data_type k_online_property_data_type_map[];
#endif // NETWORKING_LOGIC_LOGIC_MATCHMAKING_SEEKER_H
