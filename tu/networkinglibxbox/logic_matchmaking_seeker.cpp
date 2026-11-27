// OBJ: from the records 0x43ed60 0x43ee28 0x43ef80 0x43f008 0x43f090 0x43f0f0 0x43f1a0 0x43f238 0x43f4f8 0x43f5e0 0x43f7a8 0x43fa38 0x43fac0 0x43fe50 0x43ff88 0x440288
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "networking/online/online_session_results_types.h"
#include "cseries/cseries_types.h"
#include "networking/session_tracker_globals_types.h"
#include "networking/logic/logic_session_tracker.h"
#include "math/real_math.h"
#include "cseries/cseries_wrapped.h"
#include "shell/shell.h"
#include <string.h>
#include <ppcintrinsics.h>
#include <stdlib.h>

#include "game/multiplayer_game_hopper.h"
#include "networking/online/online_session_search_xenon.h"
#include "networking/transport/transport_security.h"
#include "networking/logic/logic_matchmaking_desirability.h"
#include "game/hopper_configuration.h"
#include "game/multiplayer_game_hopper_globals.h"
#include "networking/network_session_tracker_session_status_types.h"
#include "networking/session_tracker_globals_types.h"
#include "networking/tracked_session_types.h"
#include "networking/session_tracker_types.h"

#include "networking/logic/logic_matchmaking_seeker.h"
#include "networking/logic/logic_session_tracker_types.h"
#include "networking/transport/transport_security_types.h"
#include "networking/network_time_types.h"
#include "networking/network_configuration_types.h"
#include "networking/network_configuration.h"
#include "networking/online/online_session_search_types.h"
#include "networking/transport/transport_security_globals.h"
#include "networking/network_globals.h"
#include "cseries/cseries.h"
#include "networking/logic/life_cycle/life_cycle_matchmaking_quality.h"
// @address 0x8c447c g_network_time_globals
#include "cseries/crt_xdk.h"
#include "networking/network_time_access.h"
#include "networking/online/online_constants.h"
#include "networking/logic/session_tracker_class.h"
#include "networking/online/online_guide_xenon.h"
#include "networking/online/online_session_results.h"
#include "interface/user_interface_controller.h"

// Scratch storage for the name and status temporaries:
// two sequential 16-byte slots, never shared. c2 shares the status slot
// into `name` whenever it believes the slot is never written - which is
// exactly what the accepted get_session_status `__restrict` summary claims
// (probe J: opaque call restores status@96). Merging both temporaries into
// one 32-byte struct fixes their relative offsets (name@+0, status@+16)
// and unions their live ranges across both loops, so the packer must place
// the buffer whole at r1+80 and results at r1+112. The two byte ranges are
// used in disjoint phases (ascii name in loop 1, tracker status in loop 2),
// so sharing the buffer is behaviorally identical.
// match: 0x43f238: retained accepted name/status storage pairing, documented above.
struct s_seeker_name_status {
    char name[16];
    volatile long status_data[4];
};

#include "networking/logic/logic_session_tracker.h"

static __forceinline long long_interpolate_with_bounds(long start, long end, long rounds, long round)
{
    return (long)((real)(end - start) / (real)rounds * (real)round + (real)start);
}

static __forceinline e_online_nat_type logic_matchmaking_seeker_maximum_compatible_nat_type(e_online_nat_type nat_type)
{
    switch (nat_type)
    {
    case _online_nat_type_open:
        return _online_nat_type_strict;
    case _online_nat_type_moderate:
        return _online_nat_type_moderate;
    case _online_nat_type_none:
    case _online_nat_type_strict:
        return _online_nat_type_open;
    default:
        return _online_nat_type_open;
    }
}

static __forceinline void logic_matchmaking_seeker_get_skill_limits_for_preference(const s_matchmaking_session_properties *session_properties,
    long *minimum_skill_level, long *maximum_skill_level)
{
    network_matchmaking_build_allowed_skill_range(session_properties, g_network_configuration.strict_skill_range_end, minimum_skill_level, maximum_skill_level);
}

// @address 0x43ee28 ?search_begin@c_matchmaking_seeker@@QAAXPBUs_matchmaking_seeker_search_start_parameters@@PBUs_matchmaking_session_properties@@PAVc_matchmaking_quality@@@Z
void c_matchmaking_seeker::search_begin(const s_matchmaking_seeker_search_start_parameters *search_start_parameters,
    const s_matchmaking_session_properties *session_properties, c_matchmaking_quality *matchmaking_quality)
{
    m_flags.m_storage = 0;
    m_current_session_search_index = -1;
    m_session_search_failure_count = 0;
    m_sessions_found = 0;
    m_session_search_count = 0;
    m_sessions_failed = 0;
    m_sessions_contacted = 0;
    memset(m_session_searches, 0, sizeof(m_session_searches));
    m_search_state = 0;
    m_search_state_timeout = 0;
    m_search_preference = _matchmaking_search_preference_none;
    m_initial_search_start_time = 0;
    m_last_search_time = 0;
    memset(&m_session_properties, 0, sizeof(m_session_properties));
    e_matchmaking_search_preference search_preference = search_start_parameters->search_preference;
    m_ranked = false;
    m_game_mode = 0;
    m_game_type = 0;
    set_search_properties(session_properties, search_preference);
    unsigned long start_time = network_time_get();
    bool force_desperation = search_start_parameters->force_desperation;
    m_initial_search_start_time = start_time;
    if (force_desperation)
    {
        long sessions_found_count = search_start_parameters->initial_desperation_sessions_found_count;
        m_session_search_count = search_start_parameters->initial_desperation_session_search_count;
        m_sessions_failed = sessions_found_count;
        m_search_state = 2;
        m_search_state_timeout = g_network_configuration.matchmaking_search_state_timeout;
    }
    if (search_start_parameters->flag_2_enabled)
        m_flags.m_storage |= 2;
    else
        m_flags.m_storage &= ~2;
    if (!((c_session_tracker *)&session_tracker_globals)->allocate_storage(1, 2, matchmaking_quality))
    {
        m_flags.m_storage |= 64;
        return;
    }
    m_flags.m_storage |= 1;
}
void c_matchmaking_seeker::add_session_searches_to_matchmaking_quality(c_matchmaking_quality *matchmaking_quality)
{
    if ((m_flags.m_storage & 1) != 0)
    {
        for (long search_index = 0; search_index <= m_current_session_search_index; ++search_index)
            matchmaking_quality->add_session_search(&m_session_searches[search_index]);
    }
}
unsigned long c_matchmaking_seeker::get_current_search_start_time() const
{
    unsigned long start_time = network_time_get();
    if (m_current_session_search_index >= 0)
        start_time = m_session_searches[m_current_session_search_index].start_time;
    return start_time;
}
// @address 0x43f0f0 ?set_session_properties@c_matchmaking_seeker@@QAAXPBUs_matchmaking_session_properties@@@Z
void c_matchmaking_seeker::set_session_properties(const s_matchmaking_session_properties *session_properties)
{
    if ((m_flags.m_storage & 4) != 0)
        m_flags.m_storage &= ~4;
    m_flags.m_storage &= ~16;
    ((c_session_tracker *)&session_tracker_globals)->clear();
    set_search_properties(session_properties, m_search_preference);
    if (m_search_state == 3)
    {
        long search_index = m_current_session_search_index - 1;
        m_search_state = 2;
        m_search_state_timeout = g_network_configuration.matchmaking_search_state_timeout;
        if (m_current_session_search_index - 1 < 0)
            search_index = 0;
        m_current_session_search_index = search_index;
    }
}
void c_matchmaking_seeker::set_search_properties(const s_matchmaking_session_properties *session_properties, e_matchmaking_search_preference search_preference)
{
    m_session_properties = *session_properties;
    m_search_preference = search_preference;
    const c_hopper_configuration *hopper_configuration = multiplayer_game_hoppers_get_current_hopper_configuration();
    m_game_mode = online_constant_get_game_mode_by_xlast_index(hopper_configuration->get_xlast_index());
    m_ranked = hopper_configuration->is_ranked();
    m_game_type = online_constant_get_game_type(m_ranked);
}

// @address 0x43ef80 ?get_session_to_join@c_matchmaking_seeker@@QAA_NPAUs_suitable_matchmaking_session@@@Z
bool c_matchmaking_seeker::get_session_to_join(s_suitable_matchmaking_session *suitable_matchmaking_session)
{
    bool found;
    long session_count;
    found = false;
    session_count = session_tracker_globals.count;
    if ((m_flags.m_storage & 16) != 0)
    {
        update_tracked_sessions();
        // Volatile: the out-param is written through the callee's `__restrict`
        // output pointer;
        // volatile keeps the status read after the call.
        // match: 0x43ef80: retained accepted status-load workaround for the restricted callee summary.
        volatile s_network_session_tracker_session_status status;
        long session_index;
        for (session_index = 0; session_index < session_count; ++session_index)
        {
            ((c_session_tracker *)&session_tracker_globals)->get_session_status(session_index, (s_network_session_tracker_session_status *)&status);
            if (status.status == 3)
            {
                ((c_session_tracker *)&session_tracker_globals)
                    ->get_session_data(session_index, (s_network_session_tracker_session_data *)suitable_matchmaking_session);
                found = true;
                break;
            }
        }
    }
    return found;
}

void c_matchmaking_seeker::update_tracked_sessions(void)
{
    long session_count;
    long session_index;
    bool joinable;
    e_network_session_tracker_session_undesirable_reason reason;
    s_network_session_tracker_session_status status;
    session_count = session_tracker_globals.count;
    for (session_index = 0; session_index < session_count; ++session_index)
    {
        ((c_session_tracker *)&session_tracker_globals)->get_session_status(session_index, &status);
        if (status.status != 3)
            continue;
        s_network_session_tracker_session_data session_data;
        if (!((c_session_tracker *)&session_tracker_globals)->get_session_data(session_index, &session_data))
        {
            network_session_tracker_mark_session_unsuitable(&session_data.description, _session_unsuitable_session_data_unavailable);
            continue;
        }
        joinable = true;
        s_matchmaking_session_search &search = m_session_searches[m_current_session_search_index];
        if (!(bool)(_rotl(search.query.flags.m_storage, 26) & 1))
        {
            if (session_data.qos_status.ping_msec_median > search.query.maximum_latency)
            {
                reason = _session_undesirable_latency;
                joinable = false;
            }
        }
        if (!joinable)
            network_session_tracker_mark_session_undesirable(&session_data.description, reason);
    }
}

void c_matchmaking_seeker::mark_current_session_search_query_failed(void)
{
    ++m_session_search_failure_count;
    ++m_session_searches[m_current_session_search_index].failure_count;
    m_session_searches[m_current_session_search_index].failure_time = network_time_get();
    m_flags.m_storage |= 8;
}
void c_matchmaking_seeker::build_query_strict_skill(long step, s_matchmaking_session_search_query *query)
{
    bool final_step = step == g_network_configuration.strict_skill_steps;

    query->search_type = 0;
    query->step = step;
    query->preference = m_search_preference;
    query->hopper_identifier = m_session_properties.hopper_identifier;
    query->hopper_ranking = m_session_properties.player_count;
    query->composition_flag_2 = ((m_session_properties.composition_flags & (1 << _network_session_matchmaking_composition_flag_2)) != 0);
    query->nat_type = logic_matchmaking_seeker_maximum_compatible_nat_type((e_online_nat_type)m_session_properties.nat_type);

    long skill_range = long_interpolate_with_bounds(g_network_configuration.strict_skill_range_start,
        g_network_configuration.strict_skill_range_end, g_network_configuration.strict_skill_steps, step);
    network_matchmaking_build_allowed_skill_range(&m_session_properties, skill_range, &query->minimum_skill, &query->maximum_skill);
    if (final_step)
    {
        long minimum_skill_level = 0;
        long maximum_skill_level = 0;
        logic_matchmaking_seeker_get_skill_limits_for_preference(&m_session_properties, &minimum_skill_level, &maximum_skill_level);
    }

    long skill_level_delta = long_interpolate_with_bounds(g_network_configuration.strict_skill_level_delta_start,
        g_network_configuration.strict_skill_level_delta_end, g_network_configuration.strict_skill_steps, step);
    query->minimum_average_skill_level = PIN(m_session_properties.average_skill_level - skill_level_delta, 1, 50);
    query->maximum_average_skill_level = PIN(m_session_properties.average_skill_level + skill_level_delta, 1, 50);

    real fraction = (real)step / (real)g_network_configuration.strict_skill_steps;
    real mu_delta = interpolate_linear(g_network_configuration.strict_mu_delta_start, g_network_configuration.strict_mu_delta_end, fraction);
    query->minimum_average_mu = m_session_properties.average_mu - mu_delta;
    query->maximum_average_mu = m_session_properties.average_mu + mu_delta;

    if (m_ranked)
        query->flags.set(_matchmaking_session_search_query_progressive, true);

    query->language = m_session_properties.language;
    query->country = m_session_properties.country;
    query->region = m_session_properties.region;
    query->maximum_latency = g_network_configuration.maximum_latency;
    query->composition_flag_0 = ((m_session_properties.composition_flags & (1 << _network_session_matchmaking_composition_flag_0)) != 0);

    if (final_step)
    {
        query->flags.set(_matchmaking_session_search_query_final_strict_step, true);
    }
    else
    {
        long experience_delta = long_interpolate_with_bounds(g_network_configuration.strict_experience_delta_start, 13,
            g_network_configuration.strict_skill_steps - 1, step);
        query->minimum_average_experience = PIN(m_session_properties.average_experience - experience_delta, 0, 13);
        query->maximum_average_experience = PIN(m_session_properties.average_experience + experience_delta, 0, 13);
    }
}

// the record's own views of the skill query and seeker
// Reconciled with the shared query and seeker definition in the owning header.

static __forceinline void logic_matchmaking_seeker_set_query_flag(short *flags, int bit, bool value)
{
    if (value)
        *flags |= 1 << bit;
    else
        *flags &= ~(1 << bit);
}

// @address 0x43fe50 ?build_query_skill@c_matchmaking_seeker@@AAAXJPAUs_matchmaking_session_search_query@@@Z
void c_matchmaking_seeker::build_query_skill(long step, s_matchmaking_session_search_query *query)
{
    bool last = step == g_network_configuration.skill_steps;
    build_query_strict_skill(g_network_configuration.strict_skill_steps, query);
    query->search_type = 1;
    query->step = step;
    query->preference = m_search_preference;
    if (m_search_preference != 2)
        query->maximum_latency = g_network_configuration.skill_limit;
    if (m_ranked) {
        if (step >= 0 || last)
            logic_matchmaking_seeker_set_query_flag(&query->flags.m_storage, 4, true);
        if (m_search_preference != 3 && (step >= 1 || last))
            logic_matchmaking_seeker_set_query_flag(&query->flags.m_storage, 5, true);
        if (step >= 3 || last)
            logic_matchmaking_seeker_set_query_flag(&query->flags.m_storage, 7, true);
    } else {
        if (step >= 0 || last) {
            logic_matchmaking_seeker_set_query_flag(&query->flags.m_storage, 4, true);
            logic_matchmaking_seeker_set_query_flag(&query->flags.m_storage, 3, true);
            logic_matchmaking_seeker_set_query_flag(&query->flags.m_storage, 7, true);
            if (m_search_preference != 3)
                logic_matchmaking_seeker_set_query_flag(&query->flags.m_storage, 5, true);
        }
    }
}


static __forceinline e_online_data_type logic_matchmaking_seeker_get_property_data_type(long property_id)
{
    return k_online_property_data_type_map[property_id];
}
void c_matchmaking_seeker::build_online_query_from_query(const s_matchmaking_session_search_query *query, s_online_session_search_parameters *parameters)
{
    long hopper_identifier = query->hopper_identifier;
    parameters->controller_index = user_interface_controller_get_first_xbox_live_signed_in_non_guest();
    if (parameters->controller_index == -1)
        parameters->controller_index = 0;
    parameters->query_type = 0;
    parameters->context_count = 0;
    parameters->contexts[parameters->context_count].id = 1;
    parameters->contexts[parameters->context_count].value = m_game_type;
    ++parameters->context_count;
    parameters->contexts[parameters->context_count].id = 0;
    parameters->contexts[parameters->context_count].value = m_game_mode;
    ++parameters->context_count;
    parameters->property_count = 0;
    parameters->properties[parameters->property_count].id = 28;
    parameters->properties[parameters->property_count].type = logic_matchmaking_seeker_get_property_data_type(28);
    parameters->properties[parameters->property_count].value = hopper_identifier;
    ++parameters->property_count;
    parameters->properties[parameters->property_count].id = 56;
    parameters->properties[parameters->property_count].type = logic_matchmaking_seeker_get_property_data_type(56);
    parameters->properties[parameters->property_count].value = query->minimum_skill;
    ++parameters->property_count;
    parameters->properties[parameters->property_count].id = 57;
    parameters->properties[parameters->property_count].type = logic_matchmaking_seeker_get_property_data_type(57);
    parameters->properties[parameters->property_count].value = query->maximum_skill;
    ++parameters->property_count;
    parameters->properties[parameters->property_count].id = 38;
    parameters->properties[parameters->property_count].type = logic_matchmaking_seeker_get_property_data_type(38);
    parameters->properties[parameters->property_count].value = query->composition_flag_2 != 0;
    ++parameters->property_count;
    parameters->properties[parameters->property_count].id = 48;
    parameters->properties[parameters->property_count].type = logic_matchmaking_seeker_get_property_data_type(48);
    parameters->properties[parameters->property_count].value = query->nat_type;
    ++parameters->property_count;
    if (query->hopper_ranking > 8)
    {
        parameters->properties[parameters->property_count].id = 47;
        parameters->properties[parameters->property_count].type = logic_matchmaking_seeker_get_property_data_type(47);
    }
    else
    {
        long hopper_ranking_property = query->hopper_ranking + 38;
        parameters->properties[parameters->property_count].id = hopper_ranking_property;
        parameters->properties[parameters->property_count].type = logic_matchmaking_seeker_get_property_data_type(hopper_ranking_property);
    }
    parameters->properties[parameters->property_count].value = 1;
    ++parameters->property_count;
    if (!(bool)(_rotl(query->flags.m_storage, 25) & 1))
    {
        parameters->properties[parameters->property_count].id = 34;
        parameters->properties[parameters->property_count].type = logic_matchmaking_seeker_get_property_data_type(34);
        parameters->properties[parameters->property_count].value = query->composition_flag_0 != 0;
        ++parameters->property_count;
    }
    if (!(bool)(_rotl(query->flags.m_storage, 27) & 1))
    {
        parameters->properties[parameters->property_count].id = 35;
        parameters->properties[parameters->property_count].type = logic_matchmaking_seeker_get_property_data_type(35);
        parameters->properties[parameters->property_count].value = query->region;
        ++parameters->property_count;
    }
    if (!(bool)(_rotl(query->flags.m_storage, 29) & 1))
    {
        parameters->properties[parameters->property_count].id = 36;
        parameters->properties[parameters->property_count].type = logic_matchmaking_seeker_get_property_data_type(36);
        parameters->properties[parameters->property_count].value = query->language;
        ++parameters->property_count;
    }
    if (!(bool)(_rotl(query->flags.m_storage, 28) & 1))
    {
        parameters->properties[parameters->property_count].id = 37;
        parameters->properties[parameters->property_count].type = logic_matchmaking_seeker_get_property_data_type(37);
        parameters->properties[parameters->property_count].value = query->country;
        ++parameters->property_count;
    }
    if (!(bool)(query->flags.m_storage & 1))
    {
        parameters->properties[parameters->property_count].id = 50;
        parameters->properties[parameters->property_count].type = logic_matchmaking_seeker_get_property_data_type(50);
        parameters->properties[parameters->property_count].value = query->minimum_average_skill_level;
        ++parameters->property_count;
        parameters->properties[parameters->property_count].id = 51;
        parameters->properties[parameters->property_count].type = logic_matchmaking_seeker_get_property_data_type(51);
        parameters->properties[parameters->property_count].value = query->maximum_average_skill_level;
        ++parameters->property_count;
    }
    if (!(bool)(_rotl(query->flags.m_storage, 30) & 1))
    {
        parameters->properties[parameters->property_count].id = 52;
        parameters->properties[parameters->property_count].type = logic_matchmaking_seeker_get_property_data_type(52);
        parameters->properties[parameters->property_count].value = query->minimum_average_experience;
        ++parameters->property_count;
        parameters->properties[parameters->property_count].id = 53;
        parameters->properties[parameters->property_count].type = logic_matchmaking_seeker_get_property_data_type(53);
        parameters->properties[parameters->property_count].value = query->maximum_average_experience;
        ++parameters->property_count;
    }
    if (!(bool)(_rotl(query->flags.m_storage, 31) & 1))
    {
        parameters->properties[parameters->property_count].id = 54;
        parameters->properties[parameters->property_count].type = logic_matchmaking_seeker_get_property_data_type(54);
        parameters->properties[parameters->property_count].real_value = query->minimum_average_mu;
        ++parameters->property_count;
        parameters->properties[parameters->property_count].id = 55;
        parameters->properties[parameters->property_count].type = logic_matchmaking_seeker_get_property_data_type(55);
        parameters->properties[parameters->property_count].real_value = query->maximum_average_mu;
        ++parameters->property_count;
    }
}

static __forceinline bool logic_matchmaking_seeker_session_contacted(long index)
{
    bool result = false;
    s_tracked_session *session = &session_tracker_globals.sessions[index];
    if (session->qos_complete[0] || session->qos_complete[1])
        result = true;
    return result;
}

// @address 0x43f5e0 ?update_tracked_session_statistics@c_matchmaking_seeker@@AAAXXZ
void c_matchmaking_seeker::update_tracked_session_statistics()
{
    long session_count = session_tracker_globals.count;
    s_matchmaking_session_search &search = m_session_searches[m_current_session_search_index];
    m_sessions_contacted = 0;
    search.uncontacted = 0;
    search.contacting = 0;
    search.contacted = 0;
    search.suitable = 0;
    search.unsuitable_reasons.set_all(0);
    search.unsuitable = 0;
    search.unsuitable_reasons.set_all(0);
    for (long index = 0; index < session_count; ++index) {
        if (logic_matchmaking_seeker_session_contacted(index))
            ++m_sessions_contacted;
        // Volatile: the out-param is written through the callee's `__restrict`
        // output pointer;
        // volatile keeps the status reads after the call.
        // match: 0x43f5e0: retained accepted status-load workaround for the restricted callee summary.
        volatile s_network_session_tracker_session_status status;
        ((c_session_tracker *)&session_tracker_globals)->get_session_status(index, (s_network_session_tracker_session_status *)&status);
        switch (status.status) {
        case 1: ++search.uncontacted; break;
        case 2: ++search.contacting; break;
        case 3: ++search.contacted; break;
        case 5:
            ++search.unsuitable;
            ++search.unsuitable_reasons[status.unsuitable_reason];
            break;
        case 4:
            ++search.suitable;
            ++search.suitable_reasons[status.undesirable_reason];
            break;
        default:
            // match: 0x43f5e0: retained accepted unreachable-status assumption.
            __assume(0);
        }
    }
    if (search.unsuitable + search.suitable == session_count)
        m_flags.m_storage &= ~16;
}

// @address 0x7fc368 g_transport_security_globals

// Returns the local secure address; a unit-prefixed
// `static __forceinline` helper with the same body keeps the inline
// boundary.
static __forceinline const void *logic_matchmaking_seeker_inl_address_get() { return &g_transport_security_globals.secure_address; }

// @address 0x43f238 ?update_online_session_search@c_matchmaking_seeker@@AAAXXZ
void c_matchmaking_seeker::update_online_session_search()
{
    // The out-param is written through the accepted callee's `__restrict`
    // output pointer;
    // only the post-call field load goes through a volatile pointer, pinning
    // it below the call. Copied to a temp so the two compares share a
    // single load.
    s_seeker_name_status buffer;
    e_online_session_search_status search_status = online_session_search_get_status();
    bool search_complete = false;
    bool cleanup = true;
    bool failed = false;
    switch (search_status) {
    case 0: failed = true; break;
    case 1: cleanup = false; break;
    case 2: search_complete = true; break;
    case 3: failed = true; break;
    default:
        // match: 0x43f238: retained accepted unreachable-search-status assumption.
        __assume(0);
    }
    if (search_complete) {
        s_online_search_results results;
        results.count = 0;
        memset(results.entries, 0, sizeof(results.entries));
        if (!online_session_build_search_result_from_results_buffer((const char *)online_session_search_globals.search_task + 8,
            &results)) {
            failed = true;
        } else {
            m_flags.m_storage |= 16;
            m_session_searches[m_current_session_search_index].result_count = results.count;
            m_session_searches[m_current_session_search_index].end_time = network_time_get();
            m_last_search_time = network_time_get();
            m_sessions_found += results.count;
            const void *local_address = logic_matchmaking_seeker_inl_address_get();
            for (long index = 0; index < (long)results.count; ++index) {
                s_online_search_result *result = &results.entries[index];
                strncpy(buffer.name, "UNKNOWN", 16);
                buffer.name[15] = 0;
                const wchar_t *input = result->name;
                char *output = buffer.name;
                long remaining = 16;
                wchar_t character;
                do {
                    character = *input;
                    if (remaining == 1) *output = 0;
                    else if (character <= 127) *output = (char)character;
                    else *output = '?';
                    ++input;
                    ++output;
                    --remaining;
                } while (character && remaining > 0);
                if (!transport_secure_address_compare(
                        (const s_transport_secure_address *)local_address, &result->description.secure_address))
                    ((c_session_tracker *)&session_tracker_globals)->add_session(buffer.name, &result->description);
            }
            long session_count = session_tracker_globals.count;
            if (session_count > 0) {
                long unsuitable = 0;
                long undesirable = 0;
                for (long index = 0; index < session_count; ++index) {
                    ((c_session_tracker *)&session_tracker_globals)->get_session_status(index, (s_network_session_tracker_session_status *)buffer.status_data);
                    // match: 0x43f238: retained accepted post-call status load for the restricted summary.
                    long type = buffer.status_data[0];
                    if (type == 5) ++unsuitable;
                    else if (type == 4) ++undesirable;
                }
                if (unsuitable > 0 && unsuitable + undesirable == session_count) {
                    session_tracker_globals.unsuitable_count = 0;
                    memset(session_tracker_globals.unsuitable, 0, session_tracker_globals.unsuitable_size);
                }
            }
        }
    }
    if (failed) mark_current_session_search_query_failed();
    if (cleanup) m_flags.m_storage &= ~4;
}

// @address 0x43ff88 ?build_query_any@c_matchmaking_seeker@@AAAXJPAUs_matchmaking_session_search_query@@@Z
void c_matchmaking_seeker::build_query_any(long step, s_matchmaking_session_search_query *query)
{
    bool last = step == g_network_configuration.matchmaking_search_state_timeout;
    build_query_skill(g_network_configuration.skill_steps, query);
    query->search_type = 2;
    query->step = step;
    query->preference = m_search_preference;
    if (query->preference != 1) {
        long delta = long_interpolate_with_bounds(g_network_configuration.strict_skill_range_end,
            multiplayer_game_hopper_compute_skill_match_delta(m_session_properties.maximum_skill_level),
            g_network_configuration.matchmaking_search_state_timeout, step);
        network_matchmaking_build_allowed_skill_range(
            &m_session_properties, delta, &query->minimum_skill, &query->maximum_skill);
    }
    if (m_search_preference != 2) {
        if (last)
            logic_matchmaking_seeker_set_query_flag(&query->flags.m_storage, 6, true);
        else {
            long limit = long_interpolate_with_bounds(g_network_configuration.skill_limit,
                g_network_configuration.any_maximum_latency, g_network_configuration.matchmaking_search_state_timeout - 1, step);
            query->maximum_latency = limit;
        }
    }
    if (last)
        logic_matchmaking_seeker_set_query_flag(&query->flags.m_storage, 1, true);
    else {
        float fraction = (float)step / (float)(g_network_configuration.matchmaking_search_state_timeout - 1);
        float delta = interpolate_linear(g_network_configuration.strict_mu_delta_end,
            g_network_configuration.any_mu_delta, fraction);
        query->minimum_average_mu = m_session_properties.average_mu - delta;
        query->maximum_average_mu = m_session_properties.average_mu + delta;
    }
    if (last)
        logic_matchmaking_seeker_set_query_flag(&query->flags.m_storage, 0, true);
    else {
        long delta = long_interpolate_with_bounds(g_network_configuration.strict_skill_level_delta_end,
            g_network_configuration.any_skill_level_delta, g_network_configuration.matchmaking_search_state_timeout - 1, step);
        query->minimum_average_skill_level = (m_session_properties.average_skill_level - delta > 1 ? m_session_properties.average_skill_level - delta : 1) < 50
            ? (m_session_properties.average_skill_level - delta > 1 ? m_session_properties.average_skill_level - delta : 1)
            : 50;
        query->maximum_average_skill_level = (m_session_properties.average_skill_level + delta > 1 ? m_session_properties.average_skill_level + delta : 1) < 50
            ? (m_session_properties.average_skill_level + delta > 1 ? m_session_properties.average_skill_level + delta : 1)
            : 50;
    }
}

// match: 0x43f7a8: retain the accepted Boolean predicate boundary using the shared layout's scalar fields.
static __forceinline bool logic_matchmaking_seeker_sequence_holding(long search_index, unsigned char flags)
{
    if (search_index == -1)
        return false;
    return (flags >> 3) & 1;
}


// @address 0x43f7a8 ?update_online_session_search_initiate@c_matchmaking_seeker@@AAAXXZ
void c_matchmaking_seeker::update_online_session_search_initiate()
{
    bool advance = false;
    bool repeat = false;
    if (m_current_session_search_index != -1) {
        if (logic_matchmaking_seeker_sequence_holding(m_current_session_search_index, m_flags.m_storage)) {
            long start = m_session_searches[m_current_session_search_index].failure_time;
            if ((int)network_time_get() - start > g_network_configuration.search_stage_timeout) {
                repeat = true;
                advance = true;
            }
        } else {
            long start = m_last_search_time;
            if ((int)network_time_get() - start > g_network_configuration.search_start_delay)
                advance = true;
        }
    } else {
        advance = true;
    }
    if (advance && m_search_state == 3) {
        long start = m_session_searches[m_current_session_search_index].start_time;
        if ((int)network_time_get() - start < g_network_configuration.search_final_delay)
            advance = false;
        else
            repeat = true;
    }
    if (!advance)
        return;
    m_session_search_count++;
    m_flags.m_storage &= ~8;
    ((c_session_tracker *)&session_tracker_globals)->clear();
    m_sessions_failed += m_sessions_contacted;
    m_sessions_contacted = 0;
    if (repeat) {
        m_session_searches[m_current_session_search_index].repeat_count++;
    } else {
        long steps;
        m_current_session_search_index++;
        s_matchmaking_session_search *entry = &m_session_searches[m_current_session_search_index];
        switch (m_search_state) {
        case 0:
            build_query_strict_skill(m_search_state_timeout, &entry->query);
            steps = g_network_configuration.strict_skill_steps;
            break;
        case 1:
            build_query_skill(m_search_state_timeout, &entry->query);
            steps = g_network_configuration.skill_steps;
            break;
        default:
            build_query_any(m_search_state_timeout, &entry->query);
            steps = g_network_configuration.matchmaking_search_state_timeout;
            break;
        }
        build_online_query_from_query(&entry->query, &entry->online_query);
        if (++m_search_state_timeout > steps) {
            m_search_state_timeout = 0;
            m_search_state++;
        }
    }
    s_matchmaking_session_search *current = &m_session_searches[m_current_session_search_index];
    if (!online_session_search_initiate(&current->online_query)) {
        mark_current_session_search_query_failed();
        return;
    }
    m_session_searches[m_current_session_search_index].start_time = (int)network_time_get();
    m_session_searches[m_current_session_search_index].search_type = m_session_searches[m_current_session_search_index].query.search_type;
    m_flags.m_storage |= 4;
}

// --- 0x43ed60 c_matchmaking_seeker::update ---
// @address 0x43ed60 ?update@c_matchmaking_seeker@@QAAXXZ
void c_matchmaking_seeker::update(void)
{
    unsigned char flags = m_flags.m_storage;
    if ((flags & 0x40) == 0) {
        if ((flags & 4) == 0 && (flags & 0x10) == 0) {
            if (online_session_search_get_status() == 1) {
                if ((flags & 0x20) == 0)
                    m_flags.m_storage = flags | 0x20;
            } else {
                if ((flags & 0x20) != 0)
                    m_flags.m_storage = flags & 0xdf;
                update_online_session_search_initiate();
            }
        }
        if ((m_flags.m_storage & 4) != 0)
            update_online_session_search();
        if ((m_flags.m_storage & 0x10) != 0) {
            update_tracked_sessions();
            update_tracked_session_statistics();
        }
    }
}
