#ifndef NETWORKING_LOGIC_LOGIC_SESSION_TRACKER_TYPES_H
#define NETWORKING_LOGIC_LOGIC_SESSION_TRACKER_TYPES_H

#include "cseries/cseries_wrapped.h"
#include "networking/transport/transport_security_types.h"
#include "networking/logic/network_search_types.h"
#include "networking/transport/transport_qos_types.h"

enum e_network_session_tracker_session_undesirable_reason {
    _session_undesirable_none,
    _session_undesirable_latency
};
enum e_session_tracker_flags { _session_tracker_initialized_bit, _session_tracker_allocated_bit };
enum e_session_tracker_session_flags {
    _session_tracker_session_valid_bit,
    _session_tracker_session_retry_qos_bit,
    _session_tracker_session_unsuitable_bit,
    _session_tracker_session_undesirable_bit
};

struct s_network_session_tracker_session_data {
    char name[16];
    s_transport_session_description description;
    s_transport_qos_result qos_status;
    s_network_squad_status_data qos_data;
    long quality;
};
struct s_session_tracker_session_data {
    c_flags_no_init<e_session_tracker_session_flags,unsigned char,4> flags;
    char name[16];
    long session_index;
    union {
        s_transport_secure_identifier secure_identifier;
        s_transport_session_description description;
    };
    long undesirable_reason;
    long unsuitable_reason;
    unsigned char qos_complete[2];
    long qos_index;
    long qos_target_index;
    long qos_target_status;
    s_transport_qos_result qos_status;
    s_network_squad_status_data qos_data;
    unsigned long qos_completion_time;
    bool quality_valid;
    long quality;
};
struct s_session_tracker_qos_attempt {
    long qos_index;
    long qos_type;
    long target_count;
    long start_time;
};

typedef char session_tracker_qos_status_size[(sizeof(void *) != 4 || ((sizeof(s_transport_qos_result) == 0x20))) ? 1 : -1];
typedef char session_tracker_qos_attempt_size[(sizeof(s_session_tracker_qos_attempt) == 0x10) ? 1 : -1];

typedef char network_tracker_qos_offset[(sizeof(void *) != 4 || ((offsetof(s_network_session_tracker_session_data, qos_data) == 0x70))) ? 1 : -1];
typedef char network_tracker_quality_offset[(sizeof(void *) != 4 || ((offsetof(s_network_session_tracker_session_data, quality) == 0x1438))) ? 1 : -1];
typedef char network_tracker_data_size[(sizeof(void *) != 4 || ((sizeof(s_network_session_tracker_session_data) == 0x1440))) ? 1 : -1];
typedef char tracker_session_index_offset[(offsetof(s_session_tracker_session_data, session_index) == 0x14) ? 1 : -1];
typedef char tracker_qos_index_offset[(offsetof(s_session_tracker_session_data, qos_index) == 0x60) ? 1 : -1];
typedef char tracker_qos_data_offset[(sizeof(void *) != 4 || ((offsetof(s_session_tracker_session_data, qos_data) == 0x90))) ? 1 : -1];
typedef char tracker_quality_valid_offset[(sizeof(void *) != 4 || ((offsetof(s_session_tracker_session_data, quality_valid) == 0x145c))) ? 1 : -1];
typedef char tracker_quality_offset[(sizeof(void *) != 4 || ((offsetof(s_session_tracker_session_data, quality) == 0x1460))) ? 1 : -1];
typedef char tracker_session_data_size[(sizeof(void *) != 4 || ((sizeof(s_session_tracker_session_data) == 0x1468))) ? 1 : -1];

#endif
