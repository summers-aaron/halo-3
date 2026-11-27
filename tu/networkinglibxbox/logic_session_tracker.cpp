#include "networking/logic/network_search_types.h"
// networkinglibxbox/logic_session_tracker.obj: from the records 0x402f68 0x402fd8 0x403068 0x403198 0x403278 0x4032d8 0x403378 0x403448 0x403518 0x403600 0x4036f0 0x403770 0x4037f0 0x4038a0 0x4039b0 0x403b60 0x404028 0x4040f0 0x404160
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "networking/transport/transport_qos_target_types.h"
#include "networking/session_tracker_globals_types.h"
#include "networking/logic/logic_session_tracker.h"
#include "cseries/crt_xdk.h"
#include "networking/logic/logic_session_tracker_unsuitable_reason_types.h"
#include "memory/data_types.h"
#include "memory/static_arrays.h"
#include "cseries/cseries_wrapped.h"
// @address 0x403198 ?allocate_storage@c_session_tracker@@QAA_NJJPAX@Z
// @address 0x403278 ?release_storage@c_session_tracker@@QAAXXZ
// @address 0x57ead8 ?allocate@c_rockall_heap@@QAAPAXJ@Z
// @address 0x403518 ?clear@c_session_tracker@@QAAXXZ
// @address 0x4039b0 ?update_qos_send@c_session_tracker@@AAAXJ@Z
// @address 0x404028 ?update_sessions_for_refresh@c_session_tracker@@AAAXXZ
// @address 0x4040f0 ?compare_session_desirability@@YAHPBX0@Z
#include <string.h>
#include "networking/network_game_definitions.h"
#include "networking/network_memory.h"
#include "networking/online/online_guide_xenon.h"
#include "networking/transport/transport_security_types.h"
#include "networking/network_session_tracker_session_status_types.h"
#include "networking/tracked_session_types.h"
#include "networking/logic/matchmaking_desirability_result_types.h"
#include "networking/session_tracker_types.h"
#include "networking/logic/logic_matchmaking_desirability.h"
#include "networking/network_utilities.h"
#include "networking/network_build_identifiers.h"
#include "networking/network_memory_types.h"
#include "networking/logic/life_cycle/life_cycle_globals_types.h"
#include "networking/logic/life_cycle/life_cycle_matchmaking_quality.h"
#include "networking/logic/session_tracker_class.h"

#include "networking/network_configuration.h"
#include "networking/network_configuration_types.h"
#include "networking/transport/transport_qos_types.h"
#include "networking/logic/logic_session_tracker_types.h"
#include "networking/transport/transport_qos.h"
#include "memory/data.h"
#include "memory/data_handle_access.h"
#include "networking/network_time_access.h"
// v_data preserves independent access to the QoS datum array during deletion.
extern "C" {
#include "networking/logic/logic_qos_reply_globals.h"
// @address 0x8c447c g_network_time_globals
// XNetQosRelease uses the transport QoS result layout.
#include "xbox/xnet_services.h"

}
namespace {
#pragma pack(push, 1)
struct s_xnaddr_secure_address {
    in_addr ina;
    unsigned long ina_online;
    unsigned short port_online;
    unsigned char ab_enet[6];
    unsigned char ab_online[20];
};
#pragma pack(pop)
}

namespace {
__forceinline void session_tracker_qos_delete(long qos_index)
{
    if (transport_qos_globals.initialized) {
        s_transport_qos_attempt *attempt = (s_transport_qos_attempt *)datum_try_and_get_inline(transport_qos_globals.data, qos_index);
        if (attempt) {
            if (XNetQosRelease(attempt->qos) == 0)
                attempt->qos = 0;
            datum_delete(transport_qos_globals.v_data, qos_index);
        }
    }
}
}

    __forceinline void c_session_tracker::release_qos_attempt(long index) {
        s_session_tracker_qos_attempt *attempt = &m_qos_attempts[index];
        if (attempt->qos_index != -1) {
            session_tracker_qos_delete(attempt->qos_index);
            clear_qos_attempt(index);
        }
    }
    __forceinline long c_session_tracker::get_maximum_qos_target_count(e_transport_qos_type qos_type) {
        long result = 0;
        if (qos_type == _transport_qos_type_probe_only) {
            result = 64;
        } else if (qos_type == _transport_qos_type_default) {
            result = g_network_configuration.maximum_qos_target_count;
        }
        return result;
    }

// @address 0x402f68 ?network_session_tracker_mark_session_unsuitable@@YA_NPBUs_transport_session_description@@W4e_network_session_tracker_session_unsuitable_reason@@@Z
bool network_session_tracker_mark_session_unsuitable(const s_transport_session_description *description,
    e_network_session_tracker_session_unsuitable_reason reason)
{
    return ((c_session_tracker *)&session_tracker_globals)->mark_session_unsuitable(description, reason);
}

// @address 0x402fd8 ?network_session_tracker_mark_session_undesirable@@YA_NPBUs_transport_session_description@@W4e_network_session_tracker_session_undesirable_reason@@@Z
bool network_session_tracker_mark_session_undesirable(const s_transport_session_description *description,
    e_network_session_tracker_session_undesirable_reason reason)
{
    return ((c_session_tracker *)&session_tracker_globals)->mark_session_undesireable(description, reason);
}

bool c_session_tracker::initialize(bool initializing)
{
    // match: 0x403068: pins this to session_tracker_globals for direct global addressing
    __assume(this == (c_session_tracker *)&session_tracker_globals);
    m_state = 0;
    m_session_count = 0;
    m_session_storage_size = 0;
    m_sessions = 0;
    m_unsuitable_session_count = 0;
    m_unsuitable_session_maximum = 0;
    m_unsuitable_session_storage_size = 0;
    m_unsuitable_sessions = 0;
    m_maximum_qos_attempt_count = 0;
    for (long attempt_index = 0; attempt_index < m_qos_attempts.get_count(); ++attempt_index) {
        clear_qos_attempt(attempt_index);
    }
    m_flags.clear();
    return true;
}

bool c_session_tracker::allocate_storage(long sort_method, long qos_status_data_type, void *quality)
{
    // match: 0x403198: pins this to session_tracker_globals for direct global addressing
    __assume(this == (c_session_tracker *)&session_tracker_globals);
    bool result = false;
    m_state = 1;
    m_quality = (c_matchmaking_quality *)quality;
    m_sort_method = 2;
    m_maximum_qos_attempt_count = ((g_network_configuration.session_qos_target_limit > 2
                                       ? g_network_configuration.session_qos_target_limit
                                       : 2) < 10
            ? (g_network_configuration.session_qos_target_limit > 2
                      ? g_network_configuration.session_qos_target_limit
                      : 2)
            : 10);
    m_session_storage_size = 0x7f8a0;
    m_sessions = (s_session_tracker_session_data *)network_shared_memory_globals.heap->allocate(m_session_storage_size);
    m_unsuitable_session_maximum = g_network_configuration.unsuitable_count;
    m_unsuitable_session_storage_size = m_unsuitable_session_maximum * sizeof(s_transport_secure_identifier);
    m_unsuitable_sessions = (s_transport_secure_identifier *)network_shared_memory_globals.heap->allocate(m_unsuitable_session_storage_size);
    if (m_sessions && m_unsuitable_sessions) {
        m_flags.set(_session_tracker_initialized_bit, true);
        result = true;
    } else {
        release_storage();
    }
    return result;
}

void c_session_tracker::release_storage()
{
    // match: 0x403278: pins this to session_tracker_globals for direct global addressing
    __assume(this == (c_session_tracker *)&session_tracker_globals);
    if (m_sessions) {
        ::network_heap_free_block((void *)(m_sessions));
    }
    if (m_unsuitable_sessions) {
        ::network_heap_free_block((void *)(m_unsuitable_sessions));
    }
    clear();
    initialize(true);
}

void c_session_tracker::get_session_status(long session_index, s_network_session_tracker_session_status *status)
{
    // match: 0x4032d8: __restrict on output removes alias reloads across status stores
    s_network_session_tracker_session_status *__restrict output = status;
    // match: 0x4032d8: pins this to session_tracker_globals for direct global addressing
    __assume(this == (c_session_tracker *)&session_tracker_globals);
    s_session_tracker_session_data *session = &m_sessions[session_index];
    memset(output, 0, sizeof(*output));
    if (session->flags.test(_session_tracker_session_unsuitable_bit)) {
        output->status = 5;
        output->unsuitable_reason = session->unsuitable_reason;
    } else if (session->flags.test(_session_tracker_session_undesirable_bit)) {
        output->status = 4;
        output->undesirable_reason = session->undesirable_reason;
    } else if (!session->qos_complete[0]) {
        output->status = 1;
    } else if (!session->qos_complete[1]) {
        output->status = 2;
    } else {
        output->status = 3;
    }
}

// @address 0x403378 ?get_session_data@c_session_tracker@@QAA_NJPAUs_network_session_tracker_session_data@@@Z
bool c_session_tracker::get_session_data(long session_index, s_network_session_tracker_session_data *data)
{
    // match: 0x403378: pins this to session_tracker_globals for direct global addressing
    __assume(this == (c_session_tracker *)&session_tracker_globals);
    bool result = false;
    s_network_session_tracker_session_status status;
    get_session_status(session_index, &status);
    if (status.status == 3) {
        s_session_tracker_session_data *session = &m_sessions[session_index];
        csstrnzcpy(data->name, session->name, sizeof(data->name));
        data->description = session->description;
        data->qos_status = session->qos_status;
        data->qos_data = session->qos_data;
        data->quality = session->quality;
        result = true;
    }
    return result;
}

// @address 0x403448 ?update@c_session_tracker@@QAAXXZ
void c_session_tracker::update()
{
    // match: 0x403448: value range assumption guides branch lowering
    __assume(this == (c_session_tracker *)&session_tracker_globals);
    if (m_flags.test(_session_tracker_initialized_bit)) {
        update_sessions_for_refresh();
        for (long qos_attempt_index = 0; qos_attempt_index < m_maximum_qos_attempt_count; ++qos_attempt_index) {
            update_qos_receive(qos_attempt_index);
            update_qos_send(qos_attempt_index);
        }
        if (m_flags.test(_session_tracker_allocated_bit)) {
            bool sorted = false;
            switch (m_state) {
            case 0:
                sorted = true;
                break;
            case 1:
                sorted = update_sort_by_desirability();
                break;
            }
            if (sorted) {
                m_flags.set(_session_tracker_allocated_bit, false);
            }
        }
    }
}

void c_session_tracker::clear()
{
    // match: 0x403518: pins this to session_tracker_globals for direct global addressing
    __assume(this == (c_session_tracker *)&session_tracker_globals);
    for (long qos_attempt_index = 0; qos_attempt_index < m_qos_attempts.get_count(); ++qos_attempt_index) {
        release_qos_attempt(qos_attempt_index);
    }
    m_session_count = 0;
    memset(m_sessions, 0, m_session_storage_size);
}

bool c_session_tracker::add_session(const char *name, const s_transport_session_description *description)
{
    // match: 0x403600: pins this to session_tracker_globals for direct global addressing
    __assume(this == (c_session_tracker *)&session_tracker_globals);
    bool result = false;
    if (m_session_count < 100) {
        s_session_tracker_session_data *tracked_session = &m_sessions[m_session_count];
        memset(tracked_session, 0, sizeof(*tracked_session));
        csstrnzcpy(tracked_session->name, name, sizeof(tracked_session->name));
        tracked_session->description = *description;
        tracked_session->session_index = m_session_count;
        tracked_session->qos_index = -1;
        tracked_session->qos_target_index = -1;
        tracked_session->flags.set(_session_tracker_session_unsuitable_bit,
            session_is_unsuitable(&description->secure_identifier));
        if (tracked_session->flags.test(_session_tracker_session_unsuitable_bit)) {
            tracked_session->unsuitable_reason = _session_previously_unsuitable;
        }
        tracked_session->flags.set(_session_tracker_session_valid_bit, true);
        ++m_session_count;
    }
    return result;
}

long c_session_tracker::get_tracked_session_index(const s_transport_secure_identifier *session_id)
{
    // match: 0x4036f0: pins this to session_tracker_globals for direct global addressing
    __assume(this == (c_session_tracker *)&session_tracker_globals);
    long result = -1;
    for (long index = 0; index < m_session_count; ++index) {
        s_session_tracker_session_data *session = &m_sessions[index];
        if (memcmp(&m_sessions[index].secure_identifier, session_id, sizeof(*session_id)) == 0) {
            result = index;
            break;
        } else {
            continue;
        }
    }
    return result;
}

bool c_session_tracker::session_is_unsuitable(const s_transport_secure_identifier *session_id)
{
    // match: 0x403770: pins this to session_tracker_globals for direct global addressing
    __assume(this == (c_session_tracker *)&session_tracker_globals);
    bool result = false;
    for (long index = 0; index < m_unsuitable_session_count; ++index) {
        if (memcmp(&m_unsuitable_sessions[index], session_id, sizeof(*session_id)) == 0) {
            result = true;
            break;
        } else {
            continue;
        }
    }
    return result;
}

void c_session_tracker::mark_tracked_session_unsuitable(s_session_tracker_session_data *session,
    e_network_session_tracker_session_unsuitable_reason reason)
{
    // match: 0x4037f0: pins this to session_tracker_globals for direct global addressing
    __assume(this == (c_session_tracker *)&session_tracker_globals);
    if (!session->flags.test(_session_tracker_session_unsuitable_bit)) {
        add_session_to_unsuitable_sessions(&session->secure_identifier);
        session->flags.set(_session_tracker_session_unsuitable_bit, true);
        session->unsuitable_reason = reason;
        m_flags.set(_session_tracker_allocated_bit, true);
    }
}

void c_session_tracker::build_qos_target_list(e_transport_qos_type qos_type,
    long *target_indices, long maximum_count, long *target_count)
{
    // match: 0x4038a0: __restrict on targets removes alias reloads in target fill loop
    long *__restrict targets = target_indices;
    // match: 0x4038a0: pins this to session_tracker_globals for direct global addressing
    __assume(this == (c_session_tracker *)&session_tracker_globals);
    // match: 0x4038a0: fixes maximum_count bound so c2 folds count checks
    __assume(maximum_count == 64);
    long maximum_qos_target_count = get_maximum_qos_target_count(qos_type);
    long count = 0;
    for (long index = 0; index < m_session_count; ++index) {
        s_session_tracker_session_data *session = &m_sessions[index];
        if (session->qos_index == -1 && !session->flags.test(_session_tracker_session_unsuitable_bit)
            && !session->flags.test(_session_tracker_session_undesirable_bit)) {
            bool add_target = false;
            if (count < maximum_qos_target_count && count < maximum_count) {
                if (!session->qos_complete[qos_type]) {
                    add_target = true;
                }
                if (qos_type == _transport_qos_type_default) {
                    if (session->flags.test(_session_tracker_session_retry_qos_bit)) {
                        add_target = true;
                    }
                    add_target = add_target && session->qos_complete[0];
                }
                if (add_target) {
                    targets[count] = index;
                    ++count;
                }
                continue;
            } else {
                break;
            }
        } else {
            continue;
        }
    }
    *target_count = count;
}

void c_session_tracker::update_qos_send(long qos_attempt_index)
{
    // match: 0x4039b0: pins this to session_tracker_globals for direct global addressing
    __assume(this == (c_session_tracker *)&session_tracker_globals);
    s_session_tracker_qos_attempt *qos_attempt = &m_qos_attempts[qos_attempt_index];
    if (qos_attempt->qos_index == -1) {
        long qos_target_indices[64];
        long qos_target_count = 0;
        e_transport_qos_type qos_type = _transport_qos_type_probe_only;
        build_qos_target_list(_transport_qos_type_probe_only, qos_target_indices, 64, &qos_target_count);
        if (qos_target_count == 0) {
            build_qos_target_list(_transport_qos_type_default, qos_target_indices, 64, &qos_target_count);
            qos_type = _transport_qos_type_default;
        }
        if (qos_target_count > 0) {
            s_transport_qos_target qos_targets[64];
            for (long target_index = 0; target_index < qos_target_count; ++target_index) {
                long qos_target_index = qos_target_indices[target_index];
                s_session_tracker_session_data *session = &m_sessions[qos_target_index];
                qos_targets[target_index].session_id = session->description.secure_identifier;
                memcpy(&qos_targets[target_index].key, &session->description.secure_key, sizeof(qos_targets[target_index].key));
                memcpy(&qos_targets[target_index].address, &session->description.secure_address, sizeof(qos_targets[target_index].address));
            }
            qos_attempt->qos_index = transport_qos_target_new(qos_type, qos_target_count, qos_targets,
                g_network_configuration.qos_bandwidth);
            if (qos_attempt->qos_index != -1) {
                for (long target_index = 0; target_index < qos_target_count; ++target_index) {
                    long qos_target_index = qos_target_indices[target_index];
                    s_session_tracker_session_data *session = &m_sessions[qos_target_index];
                    session->qos_index = qos_attempt_index;
                    session->qos_target_index = target_index;
                    session->qos_target_status = 0;
                }
                qos_attempt->target_count = qos_target_count;
                qos_attempt->start_time = network_time_get();
                qos_attempt->qos_type = qos_type;
            }
        }
    }
}

void c_matchmaking_quality::set_matchmaking_quality_address_from_secure_address(
    const s_transport_secure_address *secure_address,
    s_matchmaking_quality_address *matchmaking_quality_address_out)
{
    const s_xnaddr_secure_address *xnaddr = (const s_xnaddr_secure_address *)secure_address;
    matchmaking_quality_address_out->ina = xnaddr->ina;
    matchmaking_quality_address_out->ina_online = xnaddr->ina_online;
    matchmaking_quality_address_out->port_online = xnaddr->port_online;
    memcpy(matchmaking_quality_address_out->ab_online, xnaddr->ab_online, sizeof(matchmaking_quality_address_out->ab_online));
}

void c_matchmaking_quality::add_qos_result(
    const s_transport_session_description *session_description,
    const s_transport_qos_result *qos_result)
{
    if (m_qos_sample_count < m_qos_samples.get_count()) {
        s_matchmaking_quality_qos_sample *sample = &m_qos_samples[m_qos_sample_count];
        set_matchmaking_quality_address_from_secure_address(&session_description->secure_address, &sample->address);
        sample->qos_result = *qos_result;
        ++m_qos_sample_count;
    }
}

// @address 0x403b60 ?update_qos_receive@c_session_tracker@@AAAXJ@Z
void c_session_tracker::update_qos_receive(long qos_attempt_index)
{
    // match: 0x403b60: value range assumption guides branch lowering
    __assume(this == (c_session_tracker *)&session_tracker_globals);
    s_session_tracker_qos_attempt *qos_attempt = &m_qos_attempts[qos_attempt_index];
    if (qos_attempt->qos_index != -1) {
        bool complete = transport_qos_is_complete(qos_attempt->qos_index);
        for (long session_index = 0; session_index < m_session_count; ++session_index) {
            s_session_tracker_session_data *tracked_session = &m_sessions[session_index];
            if (tracked_session->qos_index == qos_attempt_index) {
                e_transport_qos_target_status target_status = transport_qos_get_target_status(
                    qos_attempt->qos_index, tracked_session->qos_target_index);
                if (target_status != tracked_session->qos_target_status) {
                    bool finished = true;
                    tracked_session->qos_target_status = target_status;
                    switch (target_status) {
                    case _transport_qos_target_idle:
                    case _transport_qos_target_probing:
                    case _transport_qos_target_timed_out:
                        finished = false;
                        break;
                    case _transport_qos_target_failed:
                        mark_tracked_session_unsuitable(tracked_session, _session_unsuitable_qos_failed);
                        break;
                    case _transport_qos_target_unavailable:
                        mark_tracked_session_unsuitable(tracked_session, _session_unsuitable_qos_unavailable);
                        break;
                    case _transport_qos_target_complete: {
                        bool got_result = transport_qos_get_result(
                            qos_attempt->qos_index, tracked_session->qos_target_index,
                            &tracked_session->qos_status);
                        if (!got_result) {
                            mark_tracked_session_unsuitable(tracked_session, _session_unsuitable_qos_result_failed);
                        } else if (tracked_session->qos_status.data_block_size == 0) {
                            mark_tracked_session_unsuitable(tracked_session, _session_unsuitable_qos_empty_data_block);
                        } else {
                            s_network_squad_status_data status_data;
                            if (network_session_qos_status_decode_from_block(tracked_session->qos_status.data_block_size,
                                    tracked_session->qos_status.data_block, &status_data)) {
                                if (network_build_is_compatible(
                                        status_data.type, status_data.version,
                                        status_data.build)) {
                                    if (m_sort_method == status_data.state) {
                                        if (m_quality != 0
                                            && qos_attempt->qos_type == _transport_qos_type_default
                                            && !(tracked_session->flags.m_storage & (1 << _session_tracker_session_retry_qos_bit))) {
                                            m_quality->add_qos_result(
                                                &tracked_session->description,
                                                &tracked_session->qos_status);
                                        }
                                        memcpy(&tracked_session->qos_data, &status_data, sizeof(tracked_session->qos_data));
                                        tracked_session->flags.set(_session_tracker_session_retry_qos_bit, false);
                                        tracked_session->qos_completion_time = network_time_get();
                                        tracked_session->qos_complete[qos_attempt->qos_type] = 1;
                                    } else {
                                        mark_tracked_session_unsuitable(tracked_session,
                                            _session_unsuitable_qos_unexpected_payload_type);
                                    }
                                } else {
                                    mark_tracked_session_unsuitable(tracked_session,
                                        _session_unsuitable_qos_version_incompatible);
                                }
                            } else {
                                mark_tracked_session_unsuitable(tracked_session,
                                    _session_unsuitable_qos_decode_failed);
                            }
                            tracked_session->qos_status.data_block_size = 0;
                            tracked_session->qos_status.data_block = 0;
                            m_flags.set(_session_tracker_allocated_bit, true);
                        }
                        break;
                    }
                    default:
                        // match: 0x403b60: unreachable hint omits dead branch check
                        __assume(0);
                    }
                    if (finished) {
                        tracked_session->qos_index = -1;
                        tracked_session->qos_target_index = -1;
                    }
                }
            }
        }
        bool timed_out = false;
        if (!complete && qos_attempt->qos_type == _transport_qos_type_default
            && network_time_since_inline(qos_attempt->start_time) > g_network_configuration.qos_timeout) {
            timed_out = true;
        }
        if (complete || timed_out) {
            for (long session_index = 0; session_index < m_session_count; ++session_index) {
                s_session_tracker_session_data *tracked_session = &m_sessions[session_index];
                if (tracked_session->qos_index == qos_attempt_index
                    && !(tracked_session->flags.m_storage & (1 << _session_tracker_session_unsuitable_bit))
                    && !(tracked_session->flags.m_storage & (1 << _session_tracker_session_undesirable_bit))) {
                    if (timed_out) {
                        mark_tracked_session_unsuitable(tracked_session, _session_unsuitable_qos_timed_out);
                    }
                    tracked_session->qos_index = -1;
                    tracked_session->qos_target_index = -1;
                }
            }
            release_qos_attempt(qos_attempt_index);
        }
    }
}

void c_session_tracker::update_sessions_for_refresh()
{
    // match: 0x404028: pins this to session_tracker_globals for direct global addressing
    __assume(this == (c_session_tracker *)&session_tracker_globals);
    for (long session_index = 0; session_index < m_session_count; ++session_index) {
        s_session_tracker_session_data *session = &m_sessions[session_index];
        if (session->qos_complete[1]
            && !session->flags.test(_session_tracker_session_unsuitable_bit)
            && !session->flags.test(_session_tracker_session_undesirable_bit)
            && !session->flags.test(_session_tracker_session_retry_qos_bit)
            && network_time_since_inline(session->qos_completion_time) > g_network_configuration.qos_refresh_interval)
            {
            session->flags.set(_session_tracker_session_retry_qos_bit, true);
        }
    }
}

int compare_session_desirability(const void *a, const void *b)
{
    const s_session_tracker_session_data *session_a = (const s_session_tracker_session_data *)a;
    const s_session_tracker_session_data *session_b = (const s_session_tracker_session_data *)b;
    int result;
    if (!session_a->quality_valid && !session_b->quality_valid) {
        result = 0;
    } else if (session_a->quality_valid && !session_b->quality_valid) {
        result = -1;
    } else if (!session_a->quality_valid && session_b->quality_valid) {
        result = 1;
    } else if (session_a->quality == session_b->quality) {
        result = 0;
    } else if (session_a->quality > session_b->quality) {
        result = -1;
    } else {
        result = 1;
    }
    return result;
}

// @address 0x404160 ?update_sort_by_desirability@c_session_tracker@@AAA_NXZ
bool c_session_tracker::update_sort_by_desirability()
{
    bool result = false;
    bool have_search_properties = false;
    long computed_desirability[1];
    s_network_session_tracker_session_status session_status;
    s_matchmaking_search_party_properties search_party_properties;
    if (g_network_life_cycle_globals.initialized) {
        int mode = g_network_life_cycle_globals.manager.m_current_state;
        if (mode != 9) {
            if (mode == 10) {
                have_search_properties = g_network_life_cycle_globals.find_and_gather
                    .get_search_party_properties(&search_party_properties);
            }
        } else {
            have_search_properties = g_network_life_cycle_globals.matchmaking_find_match
                .get_search_party_properties(&search_party_properties);
        }
    }
    if (have_search_properties) {
        int i = 0;
        result = 1;
        for (; i < session_tracker_globals.count; i++) {
            s_session_tracker_session_data *session = &((s_session_tracker_session_data *)session_tracker_globals.sessions)[i];
            ((c_session_tracker *)&session_tracker_globals)->get_session_status(i, &session_status);
            int type = (int)session_status.status;
            session->quality_valid = false;
            if (type == 2 || type == 3) {
                long connection_latency;
                computed_desirability[0] = 0;
                connection_latency = 0;
                if (type == 3) {
                    connection_latency = session->qos_status.ping_msec_median;
                }
                search_party_properties.latency = connection_latency;
                if (network_matchmaking_compute_session_desirability(
                        &session->qos_data.details.gather_party_properties,
                        &search_party_properties, computed_desirability) == 2) {
                    long value = computed_desirability[0];
                    session->quality_valid = true;
                    session->quality = value;
                }
            }
        }
        qsort(session_tracker_globals.sessions, (unsigned int)session_tracker_globals.count,
            sizeof(s_session_tracker_session_data), compare_session_desirability);
    }
    return result;
}

// @address 0x830c8a00 ?session_tracker_globals@@3Us_session_tracker_globals@@A
s_session_tracker_globals session_tracker_globals;
