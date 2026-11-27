#include "networking/online/online_session_results_types.h"
#include "interface/controller_inline.h"
// networkinglibxbox/logic_recruiting_seeker.obj: from the records 0x445380 0x445480 0x445560 0x445820 0x4459e8 0x445bf0 0x445c78
// @flags /TP /O2 /Oc
#include "cseries/crt_xdk.h"
#include "networking/session_tracker_globals_types.h"
#include "networking/logic/logic_session_tracker.h"
#include "networking/logic/session_tracker_class.h"
#include "shell/shell.h"
#include "cseries/cseries.h"
// @address 0x445380 ?search_begin@c_recruiting_seeker@@QAA_NW4e_controller_index@@J_NPAVc_matchmaking_quality@@@Z
// @address 0x445480 ?update_session_search@c_recruiting_seeker@@AAAXXZ
// @address 0x445560 ?update_online_session_search@c_recruiting_seeker@@AAAXXZ
// @address 0x445820 ?update_tracked_session_statistics@c_recruiting_seeker@@AAAXXZ
// @address 0x4459e8 ?update_online_session_search_initiate@c_recruiting_seeker@@AAAXXZ
// @address 0x445bf0 ?mark_current_session_search_query_failed@c_recruiting_seeker@@AAAXXZ
// @address 0x445c78 ?build_online_query_from_query@c_recruiting_seeker@@AAAXPBUs_recruiting_session_search_query@@PAUs_online_session_search_parameters@@@Z
#include "networking/online/online_guide_xenon.h"
#include "networking/online/online_constants.h"
#include "cseries/language.h"
#include <string.h>
#include <ppcintrinsics.h>
#include <math.h>
#include <stdlib.h>
#include "networking/online/online_session_search_xenon.h"
#include "networking/online/online_session_results.h"
#include "networking/transport/transport_security.h"
#include "cseries/cseries_wrapped.h"
#include "networking/network_session_tracker_session_status_types.h"
#include "networking/tracked_session_types.h"
#include "networking/session_tracker_types.h"
#include "networking/logic/logic_matchmaking_seeker.h"
struct s_online_session_search_parameters;
#include "networking/logic/logic_session_tracker.h"
#include "interface/user_interface_controller.h"
class c_matchmaking_quality;
#include "networking/network_time_types.h"
#include "cseries/language_types.h"
#include "networking/logic/logic_session_tracker_types.h"
#include "networking/logic/logic_session_tracker_unsuitable_reason_types.h"
#include "networking/network_configuration_types.h"
#include "networking/transport/transport_security_types.h"
#include "networking/transport/transport_security_globals.h"
#include "networking/network_globals.h"
#include "interface/controller_types.h"
#include "networking/online/online_session_search_types.h"
#include "networking/network_configuration.h"
// The flag names are not attested (March's map gives only the type, c_flags<e_recruiting_seeker_flags, unsigned char, 6>).


#include "networking/logic/logic_recruiting_seeker_types.h"



// @address 0x8c447c g_network_time_globals
#include "networking/network_time_access.h"
typedef char recruiting_search_size[(sizeof(s_recruiting_session_search) == 0x2e0) ? 1 : -1];
typedef char recruiting_search_query_offset[(offsetof(s_recruiting_session_search, online_query) == 0xb8) ? 1 : -1];
// @address 0x7fc368 g_transport_security_globals

struct s_recruiting_search_name_status {
    char name[16];
    long status_data[4];
};

bool c_recruiting_seeker::search_begin(e_controller_index controller_index, long playlist_index, bool, c_matchmaking_quality *quality)
{
    initialize();
    controller = controller_index;
    start_time = network_time_get();
    flags.set(_recruiting_seeker_flag_bit1, false);
    playlist = playlist_index;
    if (!((c_session_tracker *)&session_tracker_globals)->allocate_storage(1, 2, quality))
        flags.set(_recruiting_seeker_flag_bit5, true);
    else
        flags.set(_recruiting_seeker_flag_bit0, true);
    return flags.test(_recruiting_seeker_flag_bit0);
}


void c_recruiting_seeker::update_session_search()
{
    long session_count;
    long session_index;
    s_network_session_tracker_session_status status;
    if (!flags.test(_recruiting_seeker_flag_bit2) && !flags.test(_recruiting_seeker_flag_bit4))
        update_online_session_search_initiate();
    if (flags.test(_recruiting_seeker_flag_bit2))
        update_online_session_search();
    if (flags.test(_recruiting_seeker_flag_bit4)) {
        session_count = session_tracker_globals.count;
        for (session_index = 0; session_index < session_count; ++session_index)
        {
            ((c_session_tracker *)&session_tracker_globals)->get_session_status(session_index, &status);
            if (status.status != 3)
                continue;
            s_network_session_tracker_session_data session_data;
            if (!((c_session_tracker *)&session_tracker_globals)->get_session_data(session_index, &session_data))
            {
                ::network_session_tracker_mark_session_unsuitable(&session_data.description,
                    _session_unsuitable_session_data_unavailable);
                continue;
            }
        }
        update_tracked_session_statistics();
    }
}

static __forceinline bool logic_recruiting_seeker_online_search_get_results(s_online_search_results *results)
{
    return online_session_build_search_result_from_results_buffer(
        (char *)online_session_search_globals.search_task + 8, results);
}
static __forceinline const s_transport_secure_address *logic_recruiting_seeker_transport_address_get_safe()
{
    return &g_transport_security_globals.secure_address;
}
static __forceinline void logic_recruiting_seeker_wchar_to_ascii(const wchar_t *source, char *destination, long count, long *written)
{
    long length = 0;
    const wchar_t *src = source;
    char *dst = destination;
    long remaining = count;
    while (remaining > 0) {
        wchar_t character = *src;
        if (remaining == 1) *dst = 0;
        else if (character <= 127) *dst = (char)character;
        else *dst = '?';
        ++src; ++dst; --remaining; ++length;
        if (character == 0) break;
    }
    if (written) *written = length;
}
static __forceinline void logic_recruiting_seeker_clear_unsuitable_sessions()
{
    void *unsuitable = session_tracker_globals.unsuitable;
    long unsuitable_size = session_tracker_globals.unsuitable_size;
    session_tracker_globals.unsuitable_count = 0;
    memset(unsuitable, 0, unsuitable_size);
}

void c_recruiting_seeker::update_online_session_search()
{
    s_recruiting_search_name_status buffer;
    e_online_session_search_status status = online_session_search_get_status();
    bool search_complete = false;
    bool cleanup = true;
    bool failed = false;
    switch (status) {
    case _online_session_search_status_none: failed = true; break;
    case _online_session_search_status_pending: cleanup = false; break;
    case _online_session_search_status_success: search_complete = true; break;
    case _online_session_search_status_failure: failed = true; break;
    // match: 0x445560: default __assume(0) omits the cmplwi-3/bgt status guard, keeps bl get_status hoisted, and pins the bool color r11
    default: __assume(0);
    }
    if (search_complete) {
        s_online_search_results results;
        results.count = 0;
        memset(results.entries, 0, sizeof(results.entries));
        if (!logic_recruiting_seeker_online_search_get_results(&results)) {
            failed = true;
        } else {
            flags.set(_recruiting_seeker_flag_bit4, true);
            m_searches[m_current_session_search_index].stats.count = results.count;
            m_searches[m_current_session_search_index].stats.finish = network_time_get();
            time = network_time_get();
            e += results.count;
            const s_transport_secure_address *local_address = logic_recruiting_seeker_transport_address_get_safe();
            for (long index = 0; index < (long)results.count; index++) {
                s_online_search_result *result = &results.entries[index];
                csstrnzcpy(buffer.name, "UNKNOWN", 16);
                logic_recruiting_seeker_wchar_to_ascii(result->name, buffer.name, 16, 0);
                if (!transport_secure_address_compare(local_address, &result->description.secure_address)) {
                    ((c_session_tracker *)&session_tracker_globals)->add_session(buffer.name, &result->description);
                }
            }
            long session_count = session_tracker_globals.count;
            if (session_count > 0) {
                long unsuitable_count = 0;
                long undesirable_count = 0;
                for (long session_index = 0; session_index < session_count; session_index++) {
                    ((c_session_tracker *)&session_tracker_globals)->get_session_status(session_index,
                        (s_network_session_tracker_session_status *)buffer.status_data);
                    long tracker_status_type = buffer.status_data[0];
                    if (tracker_status_type == 5) unsuitable_count++;
                    else if (tracker_status_type == 4) undesirable_count++;
                }
                if (unsuitable_count > 0 && unsuitable_count + undesirable_count == session_count) logic_recruiting_seeker_clear_unsuitable_sessions();
            }
        }
    }
    if (failed) mark_current_session_search_query_failed();
    if (cleanup) flags.set(_recruiting_seeker_flag_bit2, false);
}

static __forceinline long logic_recruiting_seeker_session_count() { return session_tracker_globals.count; }
static __forceinline bool logic_recruiting_seeker_session_contacted(long session_index)
{
    s_tracked_session *session = &session_tracker_globals.sessions[session_index];
    bool result = false;
    if (session->qos_complete[0] || session->qos_complete[1])
        result = true;
    return result;
}

void c_recruiting_seeker::update_tracked_session_statistics()
{
    long session_count = logic_recruiting_seeker_session_count();
    s_recruiting_search_statistics *stats = &m_searches[m_current_session_search_index].stats;
    m_contacted_count = 0;
    stats->c1 = 0;
    stats->c2 = 0;
    stats->c3 = 0;
    stats->c5 = 0;
    stats->by_a.set_all(0);
    stats->c4 = 0;
    stats->by_a.set_all(0);
    for (long index = 0; index < session_count; index++)
    {
        if (logic_recruiting_seeker_session_contacted(index))
            m_contacted_count++;
        volatile s_network_session_tracker_session_status status;
        ((c_session_tracker *)&session_tracker_globals)->get_session_status(index, (s_network_session_tracker_session_status *)&status);
        switch (status.status)
        {
        case 1: stats->c1++; break;
        case 2: stats->c2++; break;
        case 3: stats->c3++; break;
        case 5: stats->c4++; stats->by_a[status.unsuitable_reason]++; break;
        case 4: stats->c5++; stats->by_b[status.undesirable_reason]++; break;
        // match: 0x445820: default __assume(0) omits the cmplwi-4/bgt status guard and pins the tracker-base colors r27/r28
        default: __assume(0);
        }
    }
    if (stats->c4 + stats->c5 == session_count)
        flags.set(_recruiting_seeker_flag_bit4, false);
}


static __forceinline long get_gamer_zone(long controller_index) { return (*controller_get((e_controller_index)controller_index)).player_profile.gamer_zone; }
static __forceinline e_online_data_type online_constant_get_property_data_type(long id) { return k_online_property_data_type_map[id]; }

void c_recruiting_seeker::update_online_session_search_initiate()
{
    bool advance = false;
    bool repeat = false;
    s_recruiting_session_search *current;
    if (m_current_session_search_index != -1) {
        if (flags.get_unsafe() & 8) {
            long time = m_searches[m_current_session_search_index].stats.failure_time;
            if ((long)network_time_get() - time > g_network_configuration.search_stage_timeout) {
                repeat = true;
                advance = true;
            }
        } else {
            long time = this->time;
            if ((long)network_time_get() - time > g_network_configuration.search_start_delay)
                advance = true;
        }
    } else {
        advance = true;
    }
    if (advance && m_current_session_search_index == 15) {
        long time = m_searches[15].stats.start;
        if ((long)network_time_get() - time < g_network_configuration.search_final_delay)
            advance = false;
        else
            repeat = true;
    }
    if (advance) {
        d++;
        flags.set_unsafe(flags.get_unsafe() & ~8);
        ((c_session_tracker *)&session_tracker_globals)->clear();
        g = m_contacted_count + g;
        if (repeat) {
            m_searches[m_current_session_search_index].stats.unknown++;
        } else {
            s_recruiting_session_search *stage;
            m_current_session_search_index++;
            a++;
            stage = &m_searches[m_current_session_search_index];
            build_online_query_from_query((const s_recruiting_session_search_query *)stage, &stage->online_query);
        }
        current = &m_searches[m_current_session_search_index];
        if (!::online_session_search_initiate(&current->online_query)) {
            mark_current_session_search_query_failed();
            return;
        }
        m_searches[m_current_session_search_index].stats.start = (long)network_time_get();
        flags.set_unsafe(flags.get_unsafe() | 4);
    }
}

void c_recruiting_seeker::mark_current_session_search_query_failed()
{
    m_failure_count++;
    m_searches[m_current_session_search_index].stats.failures++;
    m_searches[m_current_session_search_index].stats.failure_time = network_time_get();
    flags.set(_recruiting_seeker_flag_bit3, true);
}

void c_recruiting_seeker::build_online_query_from_query(const s_recruiting_session_search_query *search_query, s_online_session_search_parameters *parameters)
{
    const s_recruiting_session_search_query *query = (const s_recruiting_session_search_query *)this;
    parameters->controller_index = query->controller_index;
    if (parameters->controller_index == -1)
        parameters->controller_index = 0;
    parameters->query_type = 1;
    parameters->context_count = 0;
    parameters->contexts[parameters->context_count].id = 1;
    parameters->contexts[parameters->context_count].value = 3;
    parameters->context_count++;
    parameters->contexts[parameters->context_count].id = 0;
    parameters->contexts[parameters->context_count].value = 22;
    parameters->context_count++;

    long value = -1;
    if (query->flags & (1 << 3)) value = 56;
    else if (query->flags & (1 << 4)) value = 55;
    else if (query->flags & (1 << 5)) value = 57;
    else if (query->flags & (1 << 6)) value = 58;
    if (value != -1) {
        parameters->contexts[parameters->context_count].id = 2;
        parameters->contexts[parameters->context_count].value = value;
        parameters->context_count++;
    }
    value = -1;
    if (query->flags & (1 << 7)) value = 5;
    else if (query->flags & (1 << 8)) value = 6;
    else if (query->flags & (1 << 9)) value = 7;
    if (value != -1) {
        parameters->contexts[parameters->context_count].id = 3;
        parameters->contexts[parameters->context_count].value = value;
        parameters->context_count++;
    }
    value = -1;
    if (query->flags & (1 << 10)) value = 8;
    else if (query->flags & (1 << 11)) value = 9;
    else if (query->flags & (1 << 12)) value = 10;
    if (value != -1) {
        parameters->contexts[parameters->context_count].id = 4;
        parameters->contexts[parameters->context_count].value = value;
        parameters->context_count++;
    }
    value = -1;
    if (query->flags & (1 << 13)) value = 12;
    else if (query->flags & (1 << 14)) value = 13;
    else if (query->flags & (1 << 15)) value = 14;
    else if (query->flags & (1 << 16)) value = 15;
    else if (query->flags & (1 << 17)) value = 16;
    else if (query->flags & (1 << 18)) value = 17;
    else if (query->flags & (1 << 19)) value = 18;
    else if (query->flags & (1 << 20)) value = 19;
    else if (query->flags & (1 << 21)) value = 20;
    else if (query->flags & (1 << 22)) value = 21;
    if (value != -1) {
        parameters->contexts[parameters->context_count].id = 5;
        parameters->contexts[parameters->context_count].value = value;
        parameters->context_count++;
    }

    parameters->property_count = 0;
    if (query->flags & (1 << 0)) {
        parameters->properties[parameters->property_count].id = 0;
        // match: 0x445c78: __restrict on type hoists the g_current_language load+cmpwi above the property stores (bare pointer sinks them 2 instrs)
        c_enum<e_online_data_type, unsigned char, 0, 255> *__restrict type = &parameters->properties[parameters->property_count].type;
        *type = online_constant_get_property_data_type(0);
        parameters->properties[parameters->property_count].value = get_current_language();
        parameters->property_count++;
    }
    if (query->flags & (1 << 1)) {
        long gamer_zone = MIN(MAX(get_gamer_zone(MIN(MAX(query->controller_index, 0), 3)), 0), 4);
        parameters->properties[parameters->property_count].id = 1;
        parameters->properties[parameters->property_count].type = online_constant_get_property_data_type(1);
        parameters->properties[parameters->property_count].value = gamer_zone;
        parameters->property_count++;
    }
    if (query->flags & (1 << 2)) {
        parameters->properties[parameters->property_count].id = 32;
        parameters->properties[parameters->property_count].type = online_constant_get_property_data_type(32);
        parameters->properties[parameters->property_count].value = 0;
        parameters->property_count++;
    }
}
