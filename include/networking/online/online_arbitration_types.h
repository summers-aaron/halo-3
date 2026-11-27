#ifndef NETWORKING_ONLINE_ONLINE_ARBITRATION_TYPES_H
#define NETWORKING_ONLINE_ONLINE_ARBITRATION_TYPES_H

#include "cseries/async_xoverlapped_types.h"
#include <stddef.h>

// The session handle and nonce follow the results buffer and identify the session to register.
// The 4096-byte buffer begins immediately after the overlapped-task base.
class c_arbitration_overlapped_task : public c_overlapped_task
{
public:
    __forceinline c_arbitration_overlapped_task(const char *file, long line) : c_overlapped_task(file, line) {}
    virtual const char *get_context_string() const;
    virtual unsigned long start(void *overlapped);

    unsigned char m_results_buffer[4096];
    void *m_session;
    unsigned __int64 m_session_nonce;
};

struct s_online_arbitration_globals
{
    bool initialized;
    c_arbitration_overlapped_task *registration;
};

// Registration returns one entry per XUID, carrying its machine's trust value.
struct s_online_arbitration_registrant
{
    unsigned __int64 machine_id;
    long trustworthiness;
    unsigned __int64 xuid;
};

struct s_online_arbitration_result
{
    long registrant_count;
    s_online_arbitration_registrant registrants[16];
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char online_arbitration_task_session_offset[(sizeof(void *) != 4 || offsetof(c_arbitration_overlapped_task, m_session) == 0x1008) ? 1 : -1];
typedef char online_arbitration_task_nonce_offset[(sizeof(void *) != 4 || offsetof(c_arbitration_overlapped_task, m_session_nonce) == 0x1010) ? 1 : -1];
typedef char online_arbitration_globals_size[(sizeof(void *) != 4 || sizeof(s_online_arbitration_globals) == 8) ? 1 : -1];
typedef char online_arbitration_registrant_size[(sizeof(s_online_arbitration_registrant) == 24) ? 1 : -1];
typedef char online_arbitration_result_size[(sizeof(s_online_arbitration_result) == 0x188) ? 1 : -1];
#endif

#endif // NETWORKING_ONLINE_ONLINE_ARBITRATION_TYPES_H
