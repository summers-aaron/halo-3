#ifndef NETWORKING_ONLINE_ONLINE_MANAGED_SESSION_TASK_H
#define NETWORKING_ONLINE_ONLINE_MANAGED_SESSION_TASK_H

#include "cseries/async_xoverlapped_types.h"
#include "networking/online/online_session_types.h"

class c_network_session_membership;
// Fields shared by network_managed_session.cpp and online_session_xenon.cpp.
// Natural alignment supplies the gaps before the XUID array and after bools.
class c_managed_session_overlapped_task : public c_overlapped_task
{
    friend bool managed_session_synchronize_to_player_list(long, c_network_session_membership *);
public:
    __forceinline c_managed_session_overlapped_task() : c_overlapped_task(0, 0) { }
    virtual const char *get_context_string() const;
    virtual unsigned long start(void *overlapped);
    virtual void success(unsigned long return_result);
    virtual void failure(unsigned long calling_result, unsigned long overlapped_error,
        unsigned long overlapped_extended_error);
    virtual void complete();
    __forceinline void process_modify(long context, void (*callback)(long, bool, unsigned long), s_online_session *session,
        s_online_session *desired_session, s_online_session *actual_session);
    __forceinline void process_add_players(long context, void (*callback)(long, bool, unsigned long), s_online_session *session,
        const unsigned __int64 *player_xuids, const bool *player_filter, const bool *player_private_slots, long player_count);
    __forceinline void process_game_start(long context, void (*callback)(long, bool, unsigned long), s_online_session *session);
    __forceinline void process_game_end(long context, void (*callback)(long, bool, unsigned long), s_online_session *session);
    void process_remove_players(long context, void (*callback)(long, bool, unsigned long), s_online_session *session,
        const unsigned __int64 *player_xuids, const bool *player_filter, long player_count);
protected:
    void filter_local_users(long player_count, const unsigned __int64 *player_xuids, const bool *player_filter,
        const bool *player_private_slots);
public:
    long m_task_type;
    s_online_session *m_session;
    s_online_session *m_desired_session;
    s_online_session *m_actual_session;
    long m_context;
    void (*m_callback)(long, bool, unsigned long);
    bool m_complete;
    unsigned long m_result;
    unsigned short m_creation_flags;
    bool m_is_host;
    s_transport_session_description *m_description;
    long m_player_count;
    unsigned __int64 m_player_xuids[16];
    long m_private_slots[16];
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char online_managed_task_size[(sizeof(void *) != 4 || ((sizeof(c_managed_session_overlapped_task) == 0xf8))) ? 1 : -1];
#endif

#endif
