#ifndef NETWORKING_SESSION_TRACKER_GLOBALS_TYPES_H
#define NETWORKING_SESSION_TRACKER_GLOBALS_TYPES_H

#include "networking/tracked_session_types.h"
#include "networking/logic/logic_session_tracker_types.h"
#include <stddef.h>

class c_matchmaking_quality;

// session_tracker_globals is also viewed as c_session_tracker (see the casts in
// logic_session_tracker.cpp), so the leading members carry the class member names.
struct s_session_tracker_globals
{
    long m_state;
    long m_sort_method;
    c_flags_no_init<e_session_tracker_flags, unsigned char, 2> m_flags;
    c_matchmaking_quality *m_quality;
    long count;
    long m_session_storage_size;
    s_tracked_session *sessions;
    long unsuitable_count;
    long m_unsuitable_session_maximum;
    long unsuitable_size;
    void *unsuitable;
    void *quality;
    s_session_tracker_qos_attempt qos_attempts[10];
};

typedef char session_tracker_globals_size[(sizeof(void *) != 4 || sizeof(s_session_tracker_globals) == 0xd0) ? 1 : -1];
typedef char session_tracker_globals_state_offset[(sizeof(void *) != 4 || offsetof(s_session_tracker_globals, m_state) == 0x0) ? 1 : -1];
typedef char session_tracker_globals_sort_method_offset[(sizeof(void *) != 4 || offsetof(s_session_tracker_globals, m_sort_method) == 0x4) ? 1 : -1];
typedef char session_tracker_globals_flags_offset[(sizeof(void *) != 4 || offsetof(s_session_tracker_globals, m_flags) == 0x8) ? 1 : -1];
typedef char session_tracker_globals_quality_offset[(sizeof(void *) != 4 || offsetof(s_session_tracker_globals, m_quality) == 0xc) ? 1 : -1];
typedef char session_tracker_globals_storage_size_offset[(sizeof(void *) != 4 || offsetof(s_session_tracker_globals, m_session_storage_size) == 0x14) ? 1 : -1];
typedef char session_tracker_globals_unsuitable_maximum_offset[
    (sizeof(void *) != 4 || offsetof(s_session_tracker_globals, m_unsuitable_session_maximum) == 0x20) ? 1 : -1];

#endif
