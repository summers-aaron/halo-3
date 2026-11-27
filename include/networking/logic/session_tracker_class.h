#ifndef NETWORKING_LOGIC_SESSION_TRACKER_CLASS_H
#define NETWORKING_LOGIC_SESSION_TRACKER_CLASS_H
#include "memory/static_arrays.h"
#include "networking/logic/logic_session_tracker_types.h"
#include "networking/logic/logic_session_tracker_unsuitable_reason_types.h"
#include "networking/network_session_tracker_session_status_types.h"
#include "networking/transport/transport_qos_types.h"

class c_matchmaking_quality;

class c_session_tracker {
public:
    bool initialize(bool initializing);
    bool allocate_storage(long sort_method, long qos_status_data_type, void *quality);
    void release_storage();
    void get_session_status(long session_index, s_network_session_tracker_session_status *status);
    bool get_session_data(long session_index, s_network_session_tracker_session_data *data);
    void update();
    void clear();
    bool add_session(const char *name, const s_transport_session_description *description);
    __forceinline bool mark_session_undesireable(const s_transport_session_description *description,
        e_network_session_tracker_session_undesirable_reason reason) {
        long session_index = get_tracked_session_index(&description->secure_identifier);
        bool result = false;
        if (session_index != -1) {
            s_session_tracker_session_data *session = &m_sessions[session_index];
            mark_tracked_session_undesireable(session, reason);
            result = true;
        }
        return result;
    }
    __forceinline bool mark_session_unsuitable(const s_transport_session_description *description,
        e_network_session_tracker_session_unsuitable_reason reason) {
        long session_index = get_tracked_session_index(&description->secure_identifier);
        bool result = false;
        if (session_index != -1) {
            s_session_tracker_session_data *session = &m_sessions[session_index];
            mark_tracked_session_unsuitable(session, reason);
            result = true;
        }
        return result;
    }
private:
    void mark_tracked_session_unsuitable(s_session_tracker_session_data *session,
        e_network_session_tracker_session_unsuitable_reason reason);
    __forceinline void mark_tracked_session_undesireable(s_session_tracker_session_data *session,
        e_network_session_tracker_session_undesirable_reason reason) {
        if (!session->flags.test(_session_tracker_session_undesirable_bit)) {
            session->flags.set(_session_tracker_session_undesirable_bit, true);
            session->undesirable_reason = reason;
            m_flags.set(_session_tracker_allocated_bit, true);
        }
    }
    __forceinline void add_session_to_unsuitable_sessions(const s_transport_secure_identifier *session_id) {
        if (!session_is_unsuitable(session_id) && m_unsuitable_session_count < m_unsuitable_session_maximum) {
            memcpy(&m_unsuitable_sessions[m_unsuitable_session_count], session_id, sizeof(*session_id));
            ++m_unsuitable_session_count;
        }
    }
    long get_tracked_session_index(const s_transport_secure_identifier *session_id);
    bool session_is_unsuitable(const s_transport_secure_identifier *session_id);
    void build_qos_target_list(e_transport_qos_type qos_type, long *target_indices,
        long maximum_count, long *target_count);
    __forceinline void clear_qos_attempt(long index) {
        s_session_tracker_qos_attempt *attempt = &m_qos_attempts[index];
        attempt->target_count = 0;
        attempt->start_time = 0;
        attempt->qos_index = -1;
        attempt->qos_type = -1;
    }
    bool update_sort_by_desirability();
    void update_qos_send(long qos_attempt_index);
    void update_qos_receive(long qos_attempt_index);
    void update_sessions_for_refresh();
    __forceinline void release_qos_attempt(long index);
    __forceinline long get_maximum_qos_target_count(e_transport_qos_type qos_type);
    long m_state;
    long m_sort_method;
public:
    c_flags_no_init<e_session_tracker_flags,unsigned char,2> m_flags;
private:
    c_matchmaking_quality *m_quality;
    long m_session_count;
    long m_session_storage_size;
    s_session_tracker_session_data *m_sessions;
    long m_unsuitable_session_count;
    long m_unsuitable_session_maximum;
    long m_unsuitable_session_storage_size;
    s_transport_secure_identifier *m_unsuitable_sessions;
    long m_maximum_qos_attempt_count;
    c_static_array<s_session_tracker_qos_attempt,10> m_qos_attempts;
};

typedef char session_tracker_class_size[(sizeof(void *) != 4 || ((sizeof(c_session_tracker) == 0xd0))) ? 1 : -1];
#endif
