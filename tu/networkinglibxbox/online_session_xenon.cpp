// networkinglibxbox/online_session_xenon.obj: from the records 0x4449d8 0x444c10 0x444d78 0x444f58 0x445038 0x445108
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include <xtl.h>
#include "cseries/cseries_types.h"
#include "cseries/cseries_wrapped.h"
#include "cseries/async_xoverlapped.h"
#include "networking/online/online_session_xenon.h"
#include "networking/session/network_managed_session.h"
// @address 0x4449d8 ?calculate_slot_counts@@YAXPAUs_online_session@@JJPAJ1@Z
// @address 0x444c10 ?calculate_slot_counts@@YAKPAUs_online_session@@0PAJ1@Z
// @address 0x20e6a0 ?overlapped_task_start_internal@@YA_NPAVc_overlapped_task@@PBDJ@Z
// @address 0x4007a8 ?managed_session_players_remove_complete@@YAXJ_NK@Z
// @address 0x445038 ?build_xsession_flags_from_online_session_flags@@YAKV?$c_flags_no_init@W4e_online_session_flags@@G$08@@@Z
// @address 0x8a4c8 ?is_signed_in_to_machine@c_controller_interface@@QBA_NXZ
#include "networking/online/online_managed_session_types.h"
#include "interface/controller_types.h"
#include "interface/controller_inline.h"

enum
{
    k_maximum_players = 16,
};

// Twin of c_managed_session_overlapped_task::filter_local_users: full-image builds mark called @address
// functions noinline, so process_remove_players calls this forceinline twin to keep the loop inline here.
static __forceinline void online_session_xenon_filter_task_local_users(c_managed_session_overlapped_task *task, long player_count,
    const unsigned __int64 *player_xuids, const bool *player_filter, const bool *player_private_slots)
{
    long filtered_player_count = 0;
    for (long player_index = 0; player_index < player_count; player_index++)
    {
        if (player_filter[player_index])
        {
            task->m_player_xuids[filtered_player_count] = player_xuids[player_index];
            if (player_private_slots)
                task->m_private_slots[filtered_player_count] = player_private_slots[player_index];
            filtered_player_count++;
        }
    }
    task->m_player_count = filtered_player_count;
}

void calculate_slot_counts(s_online_session *session, long private_slots, long public_slots,
    long *private_slot_count, long *public_slot_count)
{
    long private_player_count = 0;
    long public_player_count = 0;
    for (unsigned long player_index = 0; player_index < k_maximum_players; player_index++)
    {
        if (session->players[player_index].flags.test(_online_session_player_valid))
        {
            if (session->players[player_index].flags.test(_online_session_player_private_slot))
                private_player_count++;
            else
                public_player_count++;
        }
    }
    long available_private_slots = private_slots - private_player_count > 0 ? private_slots - private_player_count : 0;
    long available_public_slots = public_slots - public_player_count > 0 ? public_slots - public_player_count : 0;
    long new_private_slot_count = (available_private_slots - public_player_count > 0 ? available_private_slots - public_player_count : 0) +
        private_player_count;
    long new_public_slot_count =
        (available_public_slots - private_player_count > 0 ? available_public_slots - private_player_count : 0) + public_player_count;
    for (unsigned long player_index = 0; player_index < k_maximum_players; player_index++)
    {
        if (session->players[player_index].flags.test(_online_session_player_valid) &&
            !session->players[player_index].flags.test(_online_session_player_joined))
        {
            if (session->players[player_index].flags.test(_online_session_player_private_slot))
                new_private_slot_count--;
            else
                new_public_slot_count--;
        }
    }
    *private_slot_count = new_private_slot_count;
    *public_slot_count = new_public_slot_count;
}

// @address 0x6be060 XSessionGetDetails

unsigned long calculate_slot_counts(s_online_session *session, s_online_session *current_session,
    long *private_slot_count, long *public_slot_count)
{
    long public_slots = session->public_slot_count;
    long private_slots = session->private_slot_count;
    long existing_private_player_count = 0;
    long existing_public_player_count = 0;
    unsigned long session_flags = 0;
    long new_private_slots, new_public_slots;
    long current_private_slots, current_public_slots;
    calculate_slot_counts(session, private_slots, public_slots, &new_private_slots, &new_public_slots);
    calculate_slot_counts(current_session, private_slots, public_slots, &current_private_slots, &current_public_slots);
    unsigned long details_size = 0x280;
    unsigned char details_buffer[0x280];
    XSESSION_LOCAL_DETAILS *details = (XSESSION_LOCAL_DETAILS *)details_buffer;
    if (current_session->handle && current_session->handle != INVALID_HANDLE_VALUE &&
        XSessionGetDetails(current_session->handle, &details_size, details, 0) == 0)
    {
        for (unsigned long player_index = 0; player_index < details->dwReturnedMemberCount; player_index++)
        {
            if (details->pSessionMembers[player_index].dwFlags & XSESSION_MEMBER_FLAGS_PRIVATE_SLOT)
                existing_private_player_count++;
            else
                existing_public_player_count++;
        }
        session_flags = details->dwFlags;
    }
    long additional_private_slots = new_private_slots - current_private_slots < 0 ? 0 : new_private_slots - current_private_slots;
    long additional_public_slots = new_public_slots - current_public_slots < 0 ? 0 : new_public_slots - current_public_slots;
    long total_private_slots = new_private_slots > current_private_slots ? new_private_slots : current_private_slots;
    total_private_slots = total_private_slots > existing_private_player_count ? total_private_slots : existing_private_player_count;
    total_private_slots += additional_private_slots;
    long total_public_slots = new_public_slots > current_public_slots ? new_public_slots : current_public_slots;
    total_public_slots = total_public_slots > existing_public_player_count ? total_public_slots : existing_public_player_count;
    total_public_slots += additional_public_slots;
    if (session->flags.test(_online_session_flag_join_via_presence_disabled_while_in_progress))
        session_flags |= XSESSION_CREATE_JOIN_VIA_PRESENCE_FRIENDS_ONLY;
    else
        session_flags &= ~XSESSION_CREATE_JOIN_VIA_PRESENCE_FRIENDS_ONLY;
    *private_slot_count = total_private_slots;
    *public_slot_count = total_public_slots;
    return session_flags;
}

void c_managed_session_overlapped_task::process_remove_players(long context,
    void (*callback)(long, bool, unsigned long), s_online_session *session,
    const unsigned __int64 *player_xuids, const bool *player_filter, long player_count)
{
    m_task_type = 5;
    m_session = session;
    m_context = context;
    m_callback = managed_session_players_remove_complete;
    m_complete = false;
    m_result = 0;
    online_session_xenon_filter_task_local_users(this, player_count, player_xuids, player_filter, 0);
    if (m_player_count == 0)
    {
        m_complete = true;
        complete();
    }
    else if (!overlapped_task_start_internal(this, "..\\networking\\online\\online_session_xenon.cpp", 444))
    {
        complete();
    }
}

unsigned long build_xsession_flags_from_online_session_flags(c_flags_no_init<e_online_session_flags, unsigned short, 9> flags)
{
    unsigned long xsession_flags = 0;
    if (flags.test(_online_session_flag_host))
        xsession_flags |= XSESSION_CREATE_HOST;
    if (flags.test(_online_session_flag_presence))
        xsession_flags |= XSESSION_CREATE_USES_PRESENCE;
    if (flags.test(_online_session_flag_stats))
        xsession_flags |= XSESSION_CREATE_USES_STATS;
    if (flags.test(_online_session_flag_matchmaking))
        xsession_flags |= XSESSION_CREATE_USES_MATCHMAKING;
    if (flags.test(_online_session_flag_arbitration))
        xsession_flags |= XSESSION_CREATE_USES_ARBITRATION;
    if (flags.test(_online_session_flag_peer_network))
        xsession_flags |= XSESSION_CREATE_USES_PEER_NETWORK;
    if (flags.test(_online_session_flag_join_via_presence_disabled))
        xsession_flags |= XSESSION_CREATE_INVITES_DISABLED;
    if (flags.test(_online_session_flag_join_via_presence_friends_only))
        xsession_flags |= XSESSION_CREATE_JOIN_VIA_PRESENCE_DISABLED;
    if (flags.test(_online_session_flag_join_disabled_while_in_progress))
        xsession_flags |= XSESSION_CREATE_JOIN_IN_PROGRESS_DISABLED;
    if (flags.test(_online_session_flag_join_via_presence_disabled_while_in_progress))
        xsession_flags |= XSESSION_CREATE_JOIN_VIA_PRESENCE_FRIENDS_ONLY;
    return xsession_flags;
}

unsigned long c_managed_session_overlapped_task::start(void *overlapped)
{
    unsigned long result;
    switch (m_task_type)
    {
    case 0:
    {
        c_flags_no_init<e_online_session_flags, unsigned short, 9> flags;
        flags.set_unsafe(m_session->flags.m_storage & m_creation_flags);
        if (controller_get((e_controller_index)m_session->controller_index)->is_signed_in_to_machine())
            result = XSessionCreate(build_xsession_flags_from_online_session_flags(flags), m_session->controller_index,
                m_session->public_slot_count, m_session->private_slot_count, &m_session->nonce,
                (XSESSION_INFO *)&m_session->description, (PXOVERLAPPED)overlapped,
                &m_session->handle);
        else
            result = 0x80004005;
        break;
    }
    case 1:
        result = XSessionDelete(m_session->handle, (PXOVERLAPPED)overlapped);
        break;
    case 2:
    {
        unsigned long user_index = m_is_host ? m_session->controller_index : 254;
        result = XSessionMigrateHost(m_session->handle, user_index, (XSESSION_INFO *)m_description, (PXOVERLAPPED)overlapped);
        break;
    }
    case 3:
    {
        long private_slot_count, public_slot_count;
        unsigned long flags = calculate_slot_counts(m_desired_session, m_actual_session, &private_slot_count, &public_slot_count);
        result = XSessionModify(m_session->handle, flags, public_slot_count, private_slot_count, (PXOVERLAPPED)overlapped);
        break;
    }
    case 4:
        result = XSessionJoinRemote(m_session->handle, m_player_count, m_player_xuids,
            (const BOOL *)m_private_slots, (PXOVERLAPPED)overlapped);
        break;
    case 5:
        result = XSessionLeaveRemote(m_session->handle, m_player_count, m_player_xuids, (PXOVERLAPPED)overlapped);
        break;
    case 6:
        result = XSessionStart(m_session->handle, 0, (PXOVERLAPPED)overlapped);
        break;
    case 7:
        result = XSessionEnd(m_session->handle, (PXOVERLAPPED)overlapped);
        break;
    }
    return result;
}

void c_managed_session_overlapped_task::filter_local_users(
    long player_count, const unsigned __int64 *player_xuids, const bool *player_filter, const bool *player_private_slots)
{
    online_session_xenon_filter_task_local_users(this, player_count, player_xuids, player_filter, player_private_slots);
}

