#ifndef NETWORKING_TRANSPORT_TRANSPORT_QOS_TYPES_H
#define NETWORKING_TRANSPORT_TRANSPORT_QOS_TYPES_H

#include <stddef.h>

enum e_transport_qos_type
{
    _transport_qos_type_probe_only,
    _transport_qos_type_default,
};

enum e_transport_qos_target_status
{
    _transport_qos_target_idle,
    _transport_qos_target_probing,
    _transport_qos_target_timed_out,
    _transport_qos_target_unavailable,
    _transport_qos_target_failed,
    _transport_qos_target_complete,
};

// The result block returned by XNetQosLookup.
#ifdef HALO3_64BIT
#pragma pack(push, 1)
#endif
struct s_transport_qos_target_state
{
    unsigned char flags;
    unsigned char reserved;
    unsigned short probes_sent;
    unsigned short probes_received;
    unsigned short data_block_size;
    const void *data_block;
    unsigned short ping_msec_minimum;
    unsigned short ping_msec_median;
    unsigned long bandwidth_upstream_bps;
    unsigned long bandwidth_downstream_bps;
};

struct s_transport_qos
{
    unsigned long target_count;
    unsigned long unresolved;
    s_transport_qos_target_state targets[1];
};
#ifdef HALO3_64BIT
#pragma pack(pop)
#endif

// Datum array shared by transport_qos and the session tracker.
struct s_transport_qos_attempt
{
    short identifier;
    long active;
    s_transport_qos *qos;
};

typedef char transport_qos_attempt_active_offset[(offsetof(s_transport_qos_attempt, active) == 4) ? 1 : -1];
#ifdef HALO3_64BIT
typedef char transport_qos_target_state_size[(sizeof(s_transport_qos_target_state) == 0x1c) ? 1 : -1];
typedef char transport_qos_targets_offset[(offsetof(s_transport_qos, targets) == 8) ? 1 : -1];
#else
typedef char transport_qos_target_state_size[(sizeof(void *) != 4 || sizeof(s_transport_qos_target_state) == 0x18) ? 1 : -1];
typedef char transport_qos_targets_offset[(sizeof(void *) != 4 || (offsetof(s_transport_qos, targets) == 8)) ? 1 : -1];
#endif
typedef char transport_qos_attempt_qos_offset[(sizeof(void *) != 4 || (offsetof(s_transport_qos_attempt, qos) == 8)) ? 1 : -1];
typedef char transport_qos_attempt_size[(sizeof(void *) != 4 || sizeof(s_transport_qos_attempt) == 0xc) ? 1 : -1];

struct s_transport_qos_result
{
    long probes_sent;
    long probes_received;
    long ping_msec_minimum;
    long ping_msec_median;
    long bandwidth_upstream_bps;
    long bandwidth_downstream_bps;
    long data_block_size;
    const void *data_block;
};

typedef char transport_qos_result_data_size_offset[(offsetof(s_transport_qos_result, data_block_size) == 0x18) ? 1 : -1];
typedef char transport_qos_result_data_offset[(sizeof(void *) != 4 || (offsetof(s_transport_qos_result, data_block) == 0x1c)) ? 1 : -1];
typedef char transport_qos_result_size[(sizeof(void *) != 4 || sizeof(s_transport_qos_result) == 0x20) ? 1 : -1];

#endif // NETWORKING_TRANSPORT_TRANSPORT_QOS_TYPES_H
