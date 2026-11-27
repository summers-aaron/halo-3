// networkinglibxbox/transport_qos.obj: from the records 0x3d2e50 0x3d2f40 0x3d3088 0x3d30f0 0x3d3160 0x3d3258 0x3d3368
// @flags /TP /O2 /J /Oc
#include "networking/transport/transport_qos_target_types.h"
#include "cseries/cseries_types.h"
#include <string.h>
#include "networking/transport/transport_qos.h"
#include "networking/transport/transport_qos_globals.h"
#include "memory/data_types.h"
#include "memory/datum_index.h"
#include "networking/transport/transport_security_types.h"

#include "memory/data_array_functions.h"
#include "networking/transport/transport_qos_types.h"

// @address 0x80a2b8 transport_qos_shutdown_globals
#include "xbox/xnet_services.h"
// @address 0x609b40 XNetQosRelease
#include "networking/network_configuration.h"
#include "networking/network_configuration_types.h"
// @address 0x84740 ?data_iterator_next@@YAPAXPAUs_data_iterator@@@Z

// 0x3d2e50
struct s_transport_qos_attempt_iterator { void *datum; s_data_iterator iterator; };

// @address 0x3d2e50 ?transport_qos_shutdown@@YAXXZ
void transport_qos_shutdown(void)
{
    void *attempt;
    int index;
    s_data_array *data;
    unsigned short index16;
    s_transport_qos_attempt *datum;
    short identifier;
    s_transport_qos *handle;
    void *next;
    s_transport_qos_attempt_iterator iterator;
    if (transport_qos_get_globals().initialized == 0)
        return;
    iterator.iterator.data = transport_qos_get_globals().data;
    iterator.iterator.absolute_index = -1;
    iterator.iterator.index = -1;
    attempt = data_iterator_next(&iterator.iterator);
    iterator.datum = attempt;
    while (attempt != 0) {
        if (transport_qos_get_globals().initialized != 0) {
            index = (int)iterator.iterator.index;
            if (index != -1) {
                data = transport_qos_get_globals().data;
                index16 = datum_index_to_absolute_index(index);
                if (index16 < data->first_unallocated) {
                    datum = (s_transport_qos_attempt *)(data->data + data->element_size * index16);
                    identifier = datum->identifier;
                    if (identifier != 0 && identifier == (index >> 16)) {
                        handle = datum->qos;
                        if (XNetQosRelease(handle) == 0)
                            datum->qos = 0;
                        ::datum_delete(transport_qos_get_globals().data, index);
                    }
                }
            }
        }
        next = data_iterator_next(&iterator.iterator);
        iterator.datum = next;
        attempt = next;
    }
#if defined(_XBOX) && !defined(__clang__)
    // match: 0x3d2e50: integer-derived address preserves reloads across calls.
    (*(s_data_array **)((int)&transport_qos_get_globals() + 4))->valid = 0;
#else
    transport_qos_globals.data->valid = 0;
#endif
    transport_qos_get_globals().initialized = 0;
    return;
}

// 0x3d2f40
// @address 0x3d2f40 ?transport_qos_target_new@@YAJW4e_transport_qos_type@@JPBUs_transport_qos_target@@J@Z
long transport_qos_target_new(e_transport_qos_type qos_type, long target_count, const s_transport_qos_target *targets, long bandwidth)
{
    long result = -1;
    const void *qos_xnaddr[64];
    const void *qos_kid[64];
    const void *qos_key[64];
    s_transport_qos *qos;
    if (transport_qos_get_globals().initialized) {
        long probes = g_network_configuration.transport_qos_probe_count[qos_type];
        long bps = bandwidth;
        if (bps == -1)
            bps = g_network_configuration.transport_qos_bandwidth[qos_type];
        target_count = target_count > 64 ? 64 : target_count;
        for (long i = 0; i < target_count; i++) {
            qos_kid[i] = &targets[i].session_id;
            qos_key[i] = &targets[i].key;
            qos_xnaddr[i] = &targets[i].address;
        }
        if (XNetQosLookup(target_count, qos_xnaddr, qos_kid, qos_key, 0, 0, 0, probes, bps, 0, 0, &qos) == 0) {
            result = ::datum_new(transport_qos_get_globals().data);
            if (result != -1) {
                s_transport_qos_attempt *datum = &((s_transport_qos_attempt *)transport_qos_get_globals().data->data)[datum_index_to_absolute_index(result)];
                datum->active = 1;
                datum->qos = qos;
            } else {
                XNetQosRelease(qos);
            }
        }
    }
    return result;
}

// @address 0x3d3088 ?transport_qos_delete@@YAXJ@Z
// 0x3d3088
void transport_qos_delete(long qos_index)
{
    if (transport_qos_get_globals().initialized) {
        s_transport_qos_attempt *datum = (s_transport_qos_attempt *)datum_try_and_get(transport_qos_get_globals().data, qos_index);
        if (datum) {
            if (XNetQosRelease(datum->qos) == 0)
                datum->qos = 0;
            ::datum_delete(transport_qos_get_globals().data, qos_index);
        }
    }
}

// 0x3d30f0
// @address 0x3d30f0 ?transport_qos_is_complete@@YA_NJ@Z
bool transport_qos_is_complete(long attempt_index)
{
    bool result = true;
    if (transport_qos_get_globals().initialized) {
        s_transport_qos_attempt *attempt = (s_transport_qos_attempt *)datum_try_and_get(transport_qos_get_globals().data, attempt_index);
        if (attempt)
            result = attempt->qos->unresolved == 0;
    }
    return result;
}

// 0x3d3160
// @address 0x3d3160 ?transport_qos_get_target_status@@YA?AW4e_transport_qos_target_status@@JJ@Z
e_transport_qos_target_status transport_qos_get_target_status(long attempt_index, long target_index)
{
    e_transport_qos_target_status result = _transport_qos_target_unavailable;
    if (transport_qos_get_globals().initialized) {
        s_transport_qos_attempt *attempt = (s_transport_qos_attempt *)datum_try_and_get(transport_qos_get_globals().data, attempt_index);
        if (attempt) {
            s_transport_qos_target_state *target = &attempt->qos->targets[target_index];
            if (target->flags & 1) {
                if (target->flags & 4)
                    result = _transport_qos_target_failed;
                else
                    result = (target->flags & 2) ? _transport_qos_target_complete : _transport_qos_target_unavailable;
            } else if (target->flags & 16) {
                result = _transport_qos_target_timed_out;
            } else if (target->probes_sent > 0) {
                result = _transport_qos_target_probing;
            } else {
                result = _transport_qos_target_idle;
            }
        }
    }
    return result;
}

// 0x3d3258
// @address 0x3d3258 ?transport_qos_get_result@@YA_NJJPAUs_transport_qos_result@@@Z
bool transport_qos_get_result(long attempt_index, long target_index, s_transport_qos_result *out_result)
{
    bool result = false;
    if (transport_qos_get_globals().initialized) {
        s_transport_qos_attempt *attempt = (s_transport_qos_attempt *)datum_try_and_get(transport_qos_get_globals().data, attempt_index);
        if (attempt) {
            s_transport_qos_target_state *target = &attempt->qos->targets[target_index];
            if ((target->flags & 2) && ((target->flags & 16) || (target->flags & 1))) {
                memset(out_result, 0, sizeof(*out_result));
                out_result->probes_sent = target->probes_sent;
                out_result->probes_received = target->probes_received;
                out_result->ping_msec_minimum = target->ping_msec_minimum;
                out_result->ping_msec_median = target->ping_msec_median;
                out_result->bandwidth_upstream_bps = target->bandwidth_upstream_bps;
                out_result->bandwidth_downstream_bps = target->bandwidth_downstream_bps;
                if (target->flags & 8) {
                    out_result->data_block_size = target->data_block_size;
                    out_result->data_block = target->data_block;
                }
                result = true;
            }
        }
    }
    return result;
}

// @address 0x3d3368 ?transport_qos_listener_initiate@@YA_NPBUs_transport_secure_identifier@@@Z
bool transport_qos_listener_initiate(const s_transport_secure_identifier *identifier)
{
    bool result = false;
    if (transport_qos_get_globals().initialized && XNetQosListen(identifier, 0, 0, 0, 1) == 0)
        result = true;
    return result;
}
