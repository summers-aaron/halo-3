// networkinglibxbox/network_managed_session.obj: from the records 0x3fd378 0x3fd418 0x3fd428 0x3fd438 0x3fd458 0x3fd4e8 0x3fd5b8 0x3fd6f8 0x3fd888 0x3fd908 0x3fda08 0x3fdad0 0x3fdb60 0x3fdbd8 0x3fdf00 0x3fdfb0 0x3fe040 0x3fe230 0x3fe300 0x3fe418 0x3fe4a8 0x3fe5b8 0x3fe638 0x3fe7a0 0x3fe8a0 0x3fe988 0x3feb68 0x3ff238 0x3ff3c0 0x3ff7d8 0x3ff978 0x3ff9e0 0x3ffb08 0x3ffc38 0x3ffd38 0x3ffde8 0x3ffee0 0x3fffd0 0x400140 0x400260 0x400340 0x4003c0 0x400468 0x4004c0 0x4005b0 0x400620 0x4006a0 0x400748 0x4007a8 0x400860 0x4008c0 0x400920 0x42d268
// @flags /TP /O2 /Oc
#include "networking/online/online_managed_session_flags.h"
#include "networking/logic/logic_matchmaking_seeker.h"
#include "networking/online/online_session_search_xenon.h"
#include "cseries/crt_xdk.h"
#include "cseries/xdk_session.h"
#include "networking/transport/transport_globals.h"
#include "networking/transport/transport_available.h"
#include "cseries/cseries_types.h"
#include "cseries/bit_operations.h"
#include "shell/shell.h"
#include "cseries/async_xoverlapped.h"
#include "networking/logic/network_bandwidth.h"
#include "networking/online/online_xenon.h"
#include "networking/session/network_managed_session.h"
#include "networking/session/network_managed_session_status.h"
#include "networking/transport/transport_qos.h"
#include "networking/logic/logic_managed_user.h"
#include "networking/online/online_session_xenon.h"
#include "networking/session/network_session_class.h"
#include "networking/transport/transport_security.h"
#include "networking/transport/transport_security_types.h"
#include "networking/transport/transport_platform_types.h"
#include "networking/session/network_session_membership.h"
#include "networking/session/network_session.h"
#include "networking/session/network_session_types.h"
#include <string.h>
#include <stdlib.h>
#include "networking/online/online_session_types.h"
#include "networking/online/online_managed_session_access.h"
#if defined(__clang__)
#include "networking/logic/network_bandwidth_globals.h"
#endif
#include "cseries/async_xoverlapped_types.h"
#include "networking/online/online_managed_session_task.h"
#include "networking/logic/network_managed_session_services.h"
#include "networking/transport/transport_security_globals.h"
#include "networking/network_globals.h"
#include "networking/network_time_types.h"
#include "networking/logic/logic_managed_user_types.h"
#include "networking/network_time_access.h"
#include <new>
#include "interface/controller_types.h"
#include "networking/session/network_session_interface_types.h"
#include "networking/online/online_presence_accessors.h"
#include "interface/controller_inline.h"
#include "networking/session/network_session_manager.h"
#include "networking/session/network_session_manager_globals.h"
#include "networking/network_configuration_types.h"
#include "networking/network_configuration.h"
#include "networking/network_time_since_access.h"
#include "networking/logic/network_bandwidth_globals.h"
#include "xbox/xnet_services.h"
#include "networking/session/network_session_parameter_observer.h"
enum e_network_session_class;
struct s_transport_session_description;
enum e_transport_platform;
struct s_transport_secure_identifier;
// @address 0x3fda08 ?managed_session_modify_slot_counts@@YAXJJJ_N0@Z
// @address 0x3fe988 ?managed_session_cancel_new_host@@YAXJ@Z
// @address 0x400860 ?managed_session_game_start_complete@@YAXJ_NK@Z
// @address 0x4008c0 ?managed_session_game_end_complete@@YAXJ_NK@Z
// @address 0x3fe040 managed_session_get_status
// @address 0x3fe418 ?managed_session_get_id@@YA_NJPAUs_transport_secure_identifier@@@Z
// @address 0x3fd4e8 ?managed_session_who_has_this_session_id@@YAJPBUs_transport_secure_identifier@@@Z
// @address 0x3ff9e0 ?managed_session_build_removal_xuid_list@@YA_NJ@Z
// @address 0x3ffb08 ?managed_session_build_add_xuid_list@@YA_NJ@Z
// @address 0x3ffc38 add_to_player_list
// @address 0x3ffd38 remove_from_player_list
// @address 0x3fdfb0 managed_session_game_end
// @address 0x3fdf00 managed_session_game_start
// @address 0x3fe5b8 managed_session_is_valid_handle
// @address 0x400260 ?managed_session_successful_delete_complete@@YAXJ@Z
// @address 0x400468 managed_session_successful_host_migration_delete_complete
// @address 0x400620 ?managed_session_successful_players_add_complete@@YAXJ@Z
// @address 0x400748 ?managed_session_successful_players_remove_complete@@YAXJ@Z
// @address 0x20e7e8 ?overlapped_task_terminate@@YAXPAVc_overlapped_task@@@Z
// @address 0x3d3368 ?transport_qos_listener_initiate@@YA_NPBUs_transport_secure_identifier@@@Z
// @address 0x80608 GetTickCount
// @address 0x8a4c8 ?is_signed_in_to_machine@c_controller_interface@@QBA_NXZ
// @address 0x609a08 XNetUnregisterKey
// @address 0x3ff978 ?managed_session_delete_host_migration_internal@@YAXJPAUs_online_managed_session@@@Z
// @address 0x3ffee0 ?get_first_signed_in_controller_index@@YA?AW4e_controller_index@@XZ
// @address 0x3fd458 ?managed_session_can_create_session@@YA_NW4e_network_session_class@@@Z
// @address 0x3fd5b8 managed_session_create_host
// @address 0x3fd888 managed_session_delete
// @address 0x3fd908 managed_session_reset_session
// @address 0x3fdad0 managed_session_add_players
// @address 0x3fe230 ?managed_session_get_security_information@@YA_NJ_NPAUs_transport_session_description@@PAW4e_transport_platform@@@Z
// @address 0x3fe300 managed_session_retrieve_security_information
// @address 0x3fe7a0 managed_session_use_new_host
// @address 0x445108 ?filter_local_users@c_managed_session_overlapped_task@@IAAXJPB_KPB_N1@Z

struct transport_address;
enum e_transport_platform;
struct s_transport_secure_identifier;
struct s_transport_secure_address;
struct s_managed_session_creation_flags {
    unsigned char bits;
    inline bool test(int bit) const { return (bits >> bit) & 1; }
};

struct s_network_managed_session_empty_description {
    unsigned char identifier[8];
    unsigned long data[13];
};

typedef char managed_session_entry_size[(sizeof(void *) != 4 || ((sizeof(s_online_managed_session) == 0x638))) ? 1 : -1];
typedef char managed_session_desired_offset[(sizeof(void *) != 4 || ((offsetof(s_online_managed_session, desired) == 0x110))) ? 1 : -1];
typedef char managed_session_actual_offset[(sizeof(void *) != 4 || ((offsetof(s_online_managed_session, actual) == 0x3d0))) ? 1 : -1];
typedef char managed_session_globals_size[(sizeof(void *) != 4 || ((sizeof(s_online_session_manager_globals) == 0x31c8))) ? 1 : -1];
s_online_session_manager_globals online_session_manager_globals;

// match: 0x73a494: the 8-aligned value map comes first so no alignment gap follows the 24-byte id map
// @address 0x73a4c8 k_online_context_value_map
unsigned long k_online_context_value_map[] =
{
    0x00000000,
    0x00000001,
    0x00000000,
    0x00000001,
    0x00000000,
    0x00000000,
    0x00000001,
    0x00000002,
    0x00000000,
    0x00000001,
    0x00000002,
    0x00000000,
    0x00000001,
    0x00000002,
    0x00000003,
    0x00000004,
    0x00000005,
    0x00000006,
    0x00000007,
    0x00000008,
    0x00000009,
    0x0000000a,
    0x00000000,
    0x00000001,
    0x00000002,
    0x00000003,
    0x00000004,
    0x00000005,
    0x00000006,
    0x00000007,
    0x00000008,
    0x00000009,
    0x0000000a,
    0x0000000b,
    0x0000000c,
    0x0000000d,
    0x0000000e,
    0x0000000f,
    0x00000010,
    0x00000011,
    0x00000012,
    0x00000013,
    0x00000014,
    0x00000015,
    0x00000016,
    0x00000017,
    0x00000018,
    0x00000019,
    0x0000001a,
    0x0000001b,
    0x0000001c,
    0x0000001d,
    0x0000001e,
    0x0000001f,
    0x00000020,
    0x00000000,
    0x00000001,
    0x00000002,
    0x00000003,
    0x00000a00,
};

// @address 0x73a494 k_online_context_id_map
unsigned long k_online_context_id_map[] =
{
    0x0000800b,
    0x0000800a,
    0x00000016,
    0x00000013,
    0x00000014,
    0x00000015,
};

enum { k_maximum_players = 16 };

static __forceinline bool test_bit16(unsigned short flags, long bit)
{
  bool result = (flags >> bit) & 1;
  return result;
}

typedef int BOOL;


const char *c_managed_session_overlapped_task::get_context_string() const
{
    const char *context = "unknown";
    switch (m_task_type) {
    case 0: context = "session create"; break;
    case 1: context = "session delete"; break;
    case 3: context = "session modify"; break;
    case 4: context = "session add players"; break;
    case 5: context = "session remove players"; break;
    case 6: context = "session start"; break;
    case 7: context = "session end"; break;
    case 2:
    default: break;
    }
    return context;
}

void c_managed_session_overlapped_task::success(unsigned long return_result)
{
    m_complete = true;
    m_result = return_result;
}

void c_managed_session_overlapped_task::failure(unsigned long calling_result, unsigned long overlapped_error,
    unsigned long overlapped_extended_error)
{
    m_complete = false;
    m_result = overlapped_extended_error;
}

void c_managed_session_overlapped_task::complete()
{
    m_callback(m_context, m_complete, m_result);
}


// @address 0x7fc368 g_transport_security_globals
bool managed_session_can_create_session(enum e_network_session_class _sym_p0)
{
    bool has_controller = get_first_signed_in_controller_index() != _controller_none;
    bool session_available = g_transport_security_globals.address_resolved;
    bool online_available = ::online_local_xuid_is_online_enabled();

    if (((int)_sym_p0) == 2)
        return has_controller && session_available && online_available;
    if (((int)_sym_p0) == 1)
        return session_available;
    return true;
}

long managed_session_who_has_this_session_id(struct s_transport_secure_identifier const * session_id)
{
    long result = -1;
    for (long managed_session_index = 0;
        managed_session_index < sizeof(online_session_manager_globals.sessions) / sizeof(online_session_manager_globals.sessions[0]);
        managed_session_index++)
    {
        if ((online_session_get(managed_session_index)->flags & (1 << 0)) != 0)
        {
            s_online_managed_session *session = online_session_get(managed_session_index);
            if (memcmp(session_id, &session->desired.description, sizeof(s_transport_secure_identifier)) == 0 ||
                memcmp(session_id, &session->actual.description.secure_identifier,
                    sizeof(s_transport_secure_identifier)) == 0)
            {
                if (result != -1)
                    return -1;
                result = managed_session_index;
            }
        }
    }
    return result;
}

struct transport_address;
struct s_transport_secure_address;
extern "C" long managed_session_create_host(long session_class, s_managed_session_creation_flags creation_flags, long platform,
    long private_slot_count, long public_slot_count, const s_online_context *contexts, long context_count)
{
    long index = managed_session_allocate();
    if (index != -1) {
        s_online_managed_session *session = online_session_get(index);
        session->session_class = session_class;
        session->platform = 0;
        session->desired.private_slot_count = 0;
        session->desired.public_slot_count = 0;
        session_flag_set(session->flags, 1, creation_flags.test(_managed_session_creation_master));
        operation_flag_set(session->desired.flags.m_storage, 0, true);
        operation_flag_set(session->desired.flags.m_storage, 5, true);
        operation_flag_set(session->desired.flags.m_storage, 2, true);
        operation_flag_set(session->desired.flags.m_storage, 3, creation_flags.test(_session_creation_flag_matchmaking));
        operation_flag_set(session->desired.flags.m_storage, 1, creation_flags.test(_session_creation_flag_squad));
        operation_flag_set(session->desired.flags.m_storage, 4, creation_flags.test(_session_creation_flag_group));
        s_online_managed_session *create = session;
        create->creation_context_count = 2;
        memcpy(create->creation_contexts, contexts, sizeof(s_online_context) * 2);
        operation_flag_set(session->secondary_flags, 2, true);
    }
    return index;
}

// @address 0x3fd6f8 ?managed_session_create_client@@YAJW4e_network_session_class@@V?$c_flags@W4e_managed_session_creation_flags@@E$03@@W4e_transport_platform@@JJPBUs_transport_secure_identifier@@PBUs_transport_secure_address@@PBUs_transport_secure_key@@PBUs_online_context@@J@Z
long managed_session_create_client(e_network_session_class session_class,
    c_flags<e_managed_session_creation_flags, unsigned char, 4> creation_flags,
    e_transport_platform platform, long private_slot_count, long public_slot_count,
    const s_transport_secure_identifier *session_id, const s_transport_secure_address *host_address,
    const s_transport_secure_key *key, const s_online_context *contexts, long context_count)
{
    long index = managed_session_allocate();
    if (index != -1) {
        s_online_managed_session *session = online_session_get(index);
        session->session_class = session_class;
        session->platform = platform;
        session->desired.private_slot_count = 0;
        session->desired.public_slot_count = 16;
        session->desired.description.secure_address = *host_address;
        session->desired.description.secure_identifier = *session_id;
        session->desired.description.secure_key = *key;
        session_flag_set(session->flags, 1, creation_flags.test(_managed_session_creation_master));
        operation_flag_set(session->desired.flags.m_storage, 5, true);
        operation_flag_set(session->desired.flags.m_storage, 2, true);
        // These bits keep the same positions when c_network_session copies them to the managed flags.
        operation_flag_set(session->desired.flags.m_storage, 3, creation_flags.test((e_managed_session_creation_flags)_session_creation_flag_matchmaking));
        operation_flag_set(session->desired.flags.m_storage, 1, creation_flags.test((e_managed_session_creation_flags)_session_creation_flag_squad));
        operation_flag_set(session->desired.flags.m_storage, 4, creation_flags.test((e_managed_session_creation_flags)_session_creation_flag_group));
        s_online_managed_session *create = session;
        create->creation_context_count = 2;
        memcpy(create->creation_contexts, contexts, sizeof(s_online_context) * 2);
        operation_flag_set(session->secondary_flags, 2, true);
    }
    return index;
}

extern "C" void managed_session_delete(long managed_session_index)
{
    s_online_managed_session *session = online_session_get(managed_session_index);
    if (!online_managed_session_test_bit(session->flags, 4) && (session->primary_flags & 0x7ff) == 0) {
        operation_flag_set(session->secondary_flags, 0, true);
        operation_flag_set(session->secondary_flags, 1, true);
        managed_session_delete_session_internal(managed_session_index, session);
        return;
    }
    operation_flag_set(session->secondary_flags, 0, true);
    operation_flag_set(session->secondary_flags, 1, true);
    session_flag_set(session->flags, 2, false);
    session->modification_time = 0;
}

// @address 0x8c447c g_network_time_globals
extern "C" void managed_session_reset_session(long index, bool reset_time)
{
    s_online_managed_session *session = online_session_get(index);
    operation_flag_set(session->secondary_flags, 0, true);
    operation_flag_set(session->desired.flags.m_storage, 0, true);
    session->host_migration_requires_new_key = true;
    operation_flag_set(session->secondary_flags, 4, false);
    operation_flag_set(session->secondary_flags, 2, true);
    operation_flag_set(session->secondary_flags, 7, true);
    operation_flag_set(session->secondary_flags, 8, true);
    if (reset_time) {
        session_flag_set(session->flags, 2, true);
        session->modification_time = g_network_time_globals.enabled ? g_network_time_globals.time : GetTickCount();
    } else {
        session_flag_set(session->flags, 2, false);
        session->modification_time = 0;
    }
}

void managed_session_modify_slot_counts(long index, long public_slot_count, long private_slot_count,
    bool friends_only, bool request_modification)
{
    s_online_managed_session *session = online_session_get(index);
    if (session->desired.private_slot_count != public_slot_count || session->desired.public_slot_count != private_slot_count ||
        test_bit16(session->desired.flags.m_storage, 9) != friends_only) {
        session->desired.private_slot_count = public_slot_count;
        session->desired.public_slot_count = private_slot_count;
        if (friends_only)
            session->desired.flags.m_storage |= 1 << 9;
        else
            session->desired.flags.m_storage &= ~(1 << 9);
        operation_flag_set(session->secondary_flags, 7, true);
        session_flag_set(session->flags, 2, false);
        session->modification_time = 0;
    }
    if (request_modification && !online_managed_session_test_bit(session->flags, 1) && !online_managed_session_test_bit(session->flags, 18))
        session_flag_set(session->flags, 17, true);
}

extern "C" void managed_session_add_players(long index, const unsigned __int64 *player_identifiers,
    const bool *player_flags, const bool *player_flags_2, long player_count)
{
    s_online_managed_session *session = online_session_get(index);
    add_to_player_list(session->desired.players, 16, player_identifiers, player_flags, player_flags_2, player_count);
    session_flag_set(session->flags, 10, true);
    operation_flag_set(session->secondary_flags, 7, true);
    operation_flag_set(session->secondary_flags, 8, true);
    session_flag_set(session->flags, 2, false);
    session->modification_time = 0;
}

class c_network_session_membership;
// @address 0x6bd730 XSessionModify
// @address 0x6bd898 XSessionJoinRemote
// @address 0x6bd940 XSessionLeaveRemote

static __forceinline void player_flag_set(unsigned short &flags, long bit, bool value)
{
    if (value)
        flags |= 1 << bit;
    else
        flags &= ~(1 << bit);
}

// @address 0x3fdbd8 ?managed_session_synchronize_to_player_list@@YA_NJPAVc_network_session_membership@@@Z
bool managed_session_synchronize_to_player_list(long index, c_network_session_membership *membership)
{
    s_online_managed_session *session = online_session_get(index);
    s_online_session_player *players = session->desired.players;
    long count = 0;
    bool result = true;
    memset(players, 0, sizeof(s_online_session_player) * 16);
    for (long player_index = (membership)->get_first_player(); player_index != -1;
        player_index = (membership)->get_next_player(player_index))
    {
        s_network_session_player *player = membership->get_player(player_index);
        unsigned __int64 xuid = player->configuration.client.xuid;
        if (xuid == 0)
            continue;
        long duplicate = 0;
        if (count > 0)
        {
            const unsigned __int64 *scan = &players->identifier;
            do
            {
                if (xuid == *scan)
                    break;
                duplicate++;
                scan += 2;
            } while (duplicate < count);
            if (duplicate < count)
                continue;
        }
        players[count].identifier = xuid;
        player_flag_set(players[count].flags.m_storage, 0, true);
        player_flag_set(players[count].flags.m_storage, 2, player->configuration.client.silver);
        player_flag_set(players[count].flags.m_storage, 1, !player->left_game);
        count++;
    }
    if (::managed_session_build_removal_xuid_list(index))
    {
        if (session->session_class == 2)
        {
            c_managed_session_overlapped_task *task = &session->task;
            bool remove_ok = true;
            long queued_count = session->queued_player_count;
            long filtered_count = 0;
            long i = 0;
            if (queued_count > 0)
            {
                unsigned __int64 *dest = task->m_player_xuids;
                const unsigned __int64 *src = session->queued_player_xuids;
                do
                {
                    if (session->queued_player_filter[i])
                    {
                        *dest = *src;
                        dest++;
                        filtered_count++;
                    }
                    src++;
                    i++;
                } while (i < queued_count);
            }
            task->m_player_count = filtered_count;
            if (filtered_count != 0 && XSessionLeaveRemote(session->pending.handle, filtered_count, task->m_player_xuids, 0) != 0)
                remove_ok = false;
            result = remove_ok;
        }
        if (session->queued_player_count != 0)
            remove_from_player_list(session->actual.players, 16, session->queued_player_xuids, session->queued_player_count);
        session->queued_player_count = 0;
    }
    bool success = result;
    if (success && session->session_class == 2 && session->desired.handle != 0)
    {
        if (!online_managed_session_test_bit(session->flags, 18))
        {
            success = true;
            s_online_session *slot_state = &session->desired;
            long private_slot_count;
            long public_slot_count;
            unsigned long session_flags = calculate_slot_counts(slot_state,
                &session->actual, &private_slot_count, &public_slot_count);
            if (XSessionModify(slot_state->handle, session_flags, public_slot_count, private_slot_count, 0) != 0)
                success = false;
            if (online_managed_session_test_bit(session->flags, 17))
                session_flag_set(session->flags, 18, true);
        }
        session->actual.flags.m_storage = session->pending.flags.m_storage;
        session->actual.public_slot_count = session->pending.public_slot_count;
        session->actual.private_slot_count = session->pending.private_slot_count;
    }
    if (success && ::managed_session_build_add_xuid_list(index))
    {
        if (session->session_class == 2)
        {
            c_managed_session_overlapped_task *task = &session->task;
            success = true;
            task->filter_local_users(session->queued_player_count, session->queued_player_xuids, (const bool *)session->queued_player_filter,
                (const bool *)session->queued_player_private_slots);
            if (task->m_player_count != 0 &&
        XSessionJoinRemote(session->pending.handle, task->m_player_count, task->m_player_xuids,
            (const int *)task->m_private_slots, 0) != 0)
                success = false;
        }
        managed_session_successful_players_add_complete(index);
    }
    session_flag_set(session->flags, 11, false);
    session_flag_set(session->flags, 12, false);
    return success;
}

extern "C" void managed_session_game_start(long index)
{
  unsigned short query;
  managed_session_get_status(&query, index);
  if (test_bit16(query, 9) && !test_bit16(query, 12) && !test_bit16(query, 11))
    managed_session_game_end(index);
  s_online_managed_session *session = online_session_get(index);
  operation_flag_set(session->secondary_flags, 10, true);
  session_flag_set(session->flags, 13, false);
  session_flag_set(session->flags, 14, false);
  session_flag_set(session->flags, 2, false);
  session->modification_time = 0;
}

extern "C" void managed_session_game_end(long index)
{
    s_online_managed_session *session = online_session_get(index);
    if (test_bit16(session->secondary_flags, 10)) {
        operation_flag_set(session->secondary_flags, 10, false);
        session_flag_set(online_session_get(index)->flags, 13, true);
        session_flag_set(online_session_get(index)->flags, 15, true);
    } else {
        operation_flag_set(session->secondary_flags, 9, true);
        session_flag_set(session->flags, 15, false);
        session_flag_set(session->flags, 16, false);
    }
    session_flag_set(session->flags, 2, false);
    session->modification_time = 0;
}

unsigned short *managed_session_get_status(unsigned short *result, long index)
{
  s_online_managed_session *entry = &online_session_manager_globals.sessions[index];
  *result = 0;
  unsigned long flags = entry->flags;
  if (online_managed_session_test_bit(flags, 4))
    *result |= 2;
  if (online_managed_session_test_bit(flags, 5))
    *result |= 4;
  unsigned short primary = entry->primary_flags;
  if (test_bit16(primary, 2) || test_bit16(entry->secondary_flags, 2))
    *result |= 1;
  if (online_managed_session_test_bit(flags, 6))
    *result |= 8;
  if (online_managed_session_test_bit(flags, 7))
    *result |= 16;
  if (test_bit16(primary, 4) || test_bit16(entry->secondary_flags, 4))
    *result |= 8;
  if (online_managed_session_test_bit(flags, 11))
    *result |= 64;
  if (online_managed_session_test_bit(flags, 12))
    *result |= 128;
  if (online_managed_session_test_bit(flags, 10))
    *result |= 32;
  if (test_bit16(primary, 10) || test_bit16(entry->secondary_flags, 10))
    *result |= 256;
  if (online_managed_session_test_bit(flags, 13))
    *result |= 512;
  if (online_managed_session_test_bit(flags, 14))
    *result |= 1024;
  if (test_bit16(primary, 9) || test_bit16(entry->secondary_flags, 9))
    *result |= 2048;
  if (online_managed_session_test_bit(flags, 15))
    *result |= 4096;
  if (online_managed_session_test_bit(flags, 16))
    *result |= 8192;
  return result;
}

bool managed_session_get_security_information(
    long index, bool include_private, struct s_transport_session_description *description, enum e_transport_platform *platform)
{
    bool result = false;
    s_online_managed_session *entry = online_session_get(index);
    if (index != -1) {
        unsigned long flags = entry->flags;
        if (online_managed_session_test_bit(flags, 4)) {
            if (description) {
                if (include_private && online_managed_session_test_bit(flags, 6) &&
                    online_managed_session_test_bit(flags, 7) && !online_managed_session_test_bit(flags, 9))
                    memcpy(description, &entry->previous_description, sizeof(*description));
                else
                    memcpy(description, &entry->actual.description, sizeof(*description));
            }
            if (platform)
                *platform = (e_transport_platform)entry->platform;
            result = true;
        }
    }
    return result;
}

extern "C" long managed_session_retrieve_security_information(const transport_address *usable_address, long platform,
    s_transport_session_description *session_description, s_transport_secure_address *secure_address)
{
    long result = -1;
    s_transport_secure_identifier identifier;
    if (::transport_secure_identifier_retrieve((transport_address const *)usable_address, _transport_platform_xenon,
            &identifier, secure_address))
    {
        for (unsigned long index = 0; index < 8; ++index) {
            if (online_session_get(index)->flags & 1) {
                s_online_managed_session *session = online_session_get(index);
                if (memcmp(&identifier, &session->actual.description, sizeof(identifier)) == 0) {
                    result = index;
                    memcpy(session_description, &session->actual.description, sizeof(*session_description));
                    break;
                } else if (memcmp(&identifier, &session->previous_description, sizeof(identifier)) == 0) {
                    result = index;
                    memcpy(session_description, &session->previous_description, sizeof(*session_description));
                    break;
                }
            }
        }
    }
    return result;
}

// Whether the entry is valid (flag bit 4), copying out its 8-byte key when it is (the key is cleared
// first). The clear and the copy are memset and memcpy: the XDK expands both to counted byte loops.
bool managed_session_get_id(long index, s_transport_secure_identifier *identifier)
{
  bool result = false;
  if (identifier)
    memset(identifier, 0, sizeof(*identifier));
  if (index != -1) {
    s_online_managed_session *entry = &online_session_manager_globals.sessions[index];
    bool valid = online_managed_session_test_bit(entry->flags, 4);
    if (valid) {
      result = true;
      if (identifier)
        memcpy(identifier, &entry->actual.description.secure_identifier, sizeof(entry->actual.description.secure_identifier));
    }
  }
  return result;
}

long managed_session_count_master_sessions()
{
    long master_session_count = 0;
    unsigned long session_index;
    s_online_managed_session *session;
    for (session_index = 0; session_index < 8; session_index++) {
        if (online_managed_session_test_bit(online_session_get(session_index)->flags, 0)) {
            session = online_session_get(session_index);
            if (online_managed_session_test_bit(session->flags, 1)) {
                master_session_count++;
            }
        }
    }
    return master_session_count;
}
static __forceinline bool network_managed_session_description_is_empty(const s_transport_session_description *description)
{
    s_network_managed_session_empty_description empty = {0};
    return memcmp(description, &empty, sizeof(*description)) == 0;
}
// @address 0x3fe638 ?managed_session_connect_client_to_new_host@@YAXJPBUs_transport_session_description@@W4e_transport_platform@@@Z
void managed_session_connect_client_to_new_host(long index, const s_transport_session_description *description,
    e_transport_platform platform)
{
    s_online_managed_session *session = online_session_get(index);
    if (network_managed_session_description_is_empty(description)) {
        managed_session_completely_reset_session_on_error(index);
        return;
    }
    if ((bool)(_rotl(session->flags, 26) & 1))
        managed_session_cancel_new_host(index);
    session->host_migration_requires_new_key = false;
    session->previous_host = (bool)((session->desired.flags.m_storage >> 0) & 1);
    session->previous_description = *description;
    if (memcmp(description, &session->actual.description, sizeof(*description)) != 0) {
        operation_flag_set(session->secondary_flags, 4, true);
        session_flag_set(session->flags, 2, false);
        session->modification_time = 0;
    } else {
        managed_session_successful_host_migration_creation_complete(index);
    }
    session_flag_set(session->flags, 6, true);
}

static __forceinline bool network_managed_session_get_handle(long index, void **handle)
{
    bool result = false;
    if (index != -1) {
        s_online_managed_session *session = online_session_get(index);
        if (session->actual.handle && session->actual.handle != (void *)-1) {
            *handle = session->actual.handle;
            result = true;
        }
    }
    return result;
}

extern "C" unsigned char managed_session_is_valid_handle(void *handle)
{
    void *session_handle;
    unsigned char result = false;
    for (unsigned long index = 0; index < 8; index++) {
        if (network_managed_session_get_handle(index, &session_handle) && session_handle == handle) {
            result = true;
            break;
        }
    }
    return result;
}

static __forceinline void set_session_host(unsigned short &flags, bool host)
{
    if (host)
        flags |= 1;
    else
        flags &= ~1;
}

static __forceinline void restore_previous_description(s_online_managed_session *session)
{
    s_transport_session_description description = session->actual.description;
    session->desired.description = session->previous_description;
    session->pending.description = session->previous_description;
    session->actual.description = session->previous_description;
    session->previous_description = description;
}

extern "C" void managed_session_use_new_host(long session_index)
{
    s_online_managed_session *session = online_session_get(session_index);
    restore_previous_description(session);
    session_flag_set(session->flags, 9, true);
    set_session_host(session->desired.flags.m_storage, session->host_migration_requires_new_key);
    set_session_host(session->pending.flags.m_storage, session->host_migration_requires_new_key);
    set_session_host(session->actual.flags.m_storage, session->host_migration_requires_new_key);
}

// @address 0x3fe8a0 ?managed_session_commit_new_host@@YAXJ@Z
void managed_session_commit_new_host(long index)
{
    s_online_managed_session *session = online_session_get(index);
    if (online_managed_session_test_bit(session->flags, 7))
    {
        if (!online_managed_session_test_bit(session->flags, 9))
            managed_session_use_new_host(index);
        operation_flag_set(session->secondary_flags, 3, true);
        set_session_host(session->desired.flags.m_storage, session->host_migration_requires_new_key);
        set_session_host(session->pending.flags.m_storage, session->host_migration_requires_new_key);
        set_session_host(session->actual.flags.m_storage, session->host_migration_requires_new_key);
    }
    session_flag_set(session->flags, 6, false);
    session_flag_set(session->flags, 9, false);
}

void managed_session_cancel_new_host(long index)
{
    s_online_managed_session *session = online_session_get(index);
    operation_flag_set(session->secondary_flags, 4, false);
    if (session->session_class == 2 && online_managed_session_test_bit(session->flags, 7)) {
        if (!online_managed_session_test_bit(session->flags, 9))
            restore_previous_description(session);
        operation_flag_set(session->secondary_flags, 3, true);
    } else {
        if (online_managed_session_test_bit(session->flags, 9))
            restore_previous_description(session);
        if (test_bit16(session->primary_flags, 4) || online_managed_session_test_bit(session->flags, 7))
            operation_flag_set(session->secondary_flags, 3, true);
        set_session_host(session->desired.flags.m_storage, session->previous_host);
        set_session_host(session->pending.flags.m_storage, session->previous_host);
        set_session_host(session->actual.flags.m_storage, session->previous_host);
    }
    session_flag_set(session->flags, 6, false);
    session_flag_set(session->flags, 9, false);
}

// match: 0x609a08: SDK header adoption is blocked by this unit's current types/API arguments: 'DWORD' : redefinition; different basic types.


// @address 0x3a0d50 ?transport_secure_key_create@@YA_NPAUs_transport_session_description@@W4e_transport_platform@@@Z
// @address 0x6099f8 XNetRegisterKey
// match: 0x6099f8: SDK header adoption is blocked by this unit's current types/API arguments: 'DWORD' : redefinition; different basic types.

// @address 0x20e6a0 ?overlapped_task_start_internal@@YA_NPAVc_overlapped_task@@PBDJ@Z
static __forceinline bool network_managed_session_register_key(
    const s_transport_session_description *description, e_transport_platform platform)
{
    bool result = false;
    if (platform == _transport_platform_xenon &&
        XNetRegisterKey(&description->secure_identifier, &description->secure_key) == 0)
        result = true;
    return result;
}
static __forceinline void network_managed_session_process_host_migration(c_managed_session_overlapped_task *task,
    long context, void (*callback)(long, bool, unsigned long), s_online_session *session,
    bool is_host, s_transport_session_description *description)
{
    task->m_task_type = 2;
    task->m_session = session;
    task->m_description = description;
    task->m_context = context;
    task->m_callback = callback;
    task->m_complete = false;
    task->m_result = 0;
    task->m_is_host = is_host;
    if (!overlapped_task_start_internal(task, "..\\networking\\online\\online_session_xenon.cpp", 362))
        task->complete();
}
// forward declarations for the session create/delete task starters (0x3ff238, 0x3ff3c0)
struct s_data_array;
extern "C" {

// @address 0x609aa0 XNetQosListen
// match: 0x609aa0: SDK header adoption is blocked by this unit's current types/API arguments: 'DWORD' : redefinition; different basic types.

// match: 0x5a0070: SDK header adoption is blocked by this unit's current types/API arguments: 'DWORD' : redefinition; different basic types.

}
struct s_managed_user_globals;
static __forceinline void network_managed_session_free(long index)
{
    s_online_managed_session *session = online_session_get(index);
    if (session->task.get_task_state() == _overlapped_task_state_pending)
        overlapped_task_terminate(&session->task);
    session_flag_set(session->flags, 0, false);
}
static __forceinline void network_managed_session_process_delete(c_managed_session_overlapped_task *task,
    long context, void (*callback)(long, bool, unsigned long), s_online_session *session)
{
    task->m_task_type = 1;
    task->m_session = session;
    task->m_context = context;
    task->m_callback = callback;
    task->m_complete = false;
    task->m_result = 0;
    if (!overlapped_task_start_internal(task, "..\\networking\\online\\online_session_xenon.cpp", 339))
        task->complete();
}
static __forceinline void network_managed_session_process_create(c_managed_session_overlapped_task *task,
    long context, void (*callback)(long, bool, unsigned long), s_online_session *session, unsigned short creation_flags)
{
    task->m_task_type = 0;
    task->m_session = session;
    task->m_context = context;
    task->m_callback = callback;
    task->m_complete = false;
    task->m_result = 0;
    task->m_creation_flags = creation_flags;
    if (!overlapped_task_start_internal(task, "..\\networking\\online\\online_session_xenon.cpp", 320))
        task->complete();
}
// @address 0x3ff238 ?managed_session_delete_session_internal@@YAXJPAUs_online_managed_session@@@Z
void managed_session_delete_session_internal(long index, s_online_managed_session *session)
{
    if (!online_managed_session_test_bit(session->flags, 4)) {
        session_flag_set(session->flags, 4, false);
        session_flag_set(session->flags, 5, false);
        if (test_bit16(session->secondary_flags, 1)) {
            operation_flag_set(session->secondary_flags, 1, false);
            network_managed_session_free(index);
        }
        return;
    }
    if (session->session_class == 0) {
        managed_session_successful_delete_complete(index);
    } else {
        if (transport_qos_globals.initialized)
            XNetQosListen((const s_transport_secure_identifier *)&session->actual.description, 0, 0, 0, 16);
        if (session->session_class == 1) {
            if (session->platform == 0)
                XNetUnregisterKey(&session->pending.description);
            managed_session_successful_delete_complete(index);
        } else {
            operation_flag_set(session->primary_flags, 0, true);
            operation_flag_set(online_session_manager_globals.current_operation_flags, 0, true);
            network_managed_session_process_delete(&session->task,
                index, managed_session_deletion_complete, &session->actual);
        }
    }
}

typedef c_flags<e_managed_session_creation_flags, unsigned char, 8> t_nms_flags;
static __forceinline bool nms_is_master(t_nms_flags flags) { return flags.test(_managed_session_creation_master); }
static __forceinline t_nms_flags nms_make_flags(unsigned char bits) { t_nms_flags flags; flags.m_storage = bits; return flags; }
// @address 0x3ff3c0 ?managed_session_create_session_internal@@YAXJPAUs_online_managed_session@@@Z
void managed_session_create_session_internal(long index, s_online_managed_session *session)
{
    bool failed = false;
    session->modification_time = 0;
    session->create_failed_network_unavailable = false;
    session->create_failed_online_disabled = false;
    session->desired.controller_index = get_first_signed_in_controller_index();
    if (online_managed_session_test_bit(session->flags, 3)) {
        session->session_class = 0;
        session_flag_set(session->flags, 3, false);
    }
    if (session->desired.controller_index == _controller_none && session->session_class == 2) {
        failed = true;
    } else if (session->session_class != 0 &&
        !(transport_available() && g_transport_security_globals.address_resolved)) {
        failed = true;
        session->create_failed_network_unavailable = true;
    } else if (session->session_class == 2 && !::online_local_xuid_is_online_enabled()) {
        failed = true;
        session->create_failed_online_disabled = true;
    } else {
        unsigned short desired_flags = session->desired.flags.m_storage;
        if (!nms_is_master(nms_make_flags(desired_flags)) &&
            index !=
                ::managed_session_who_has_this_session_id(
                    &session->desired.description.secure_identifier))
        {
            failed = true;
            session->create_failed_session_id_in_use = true;
        } else {
            session->pending.controller_index = session->desired.controller_index;
            session->pending.flags.m_storage = desired_flags;
            session->pending.public_slot_count = session->desired.public_slot_count;
            session->pending.private_slot_count = session->desired.private_slot_count;
            session->pending.nonce = session->desired.nonce;
            session->pending.description = session->desired.description;
            session->pending.handle = session->desired.handle;
            if (session->session_class == 0) {
                managed_session_successful_creation_complete(index);
            } else if (session->session_class == 1) {
                if (nms_is_master(nms_make_flags(desired_flags)))
                    failed = !transport_secure_key_create(&session->pending.description, (e_transport_platform)session->platform);
                if (!failed)
                    failed = !network_managed_session_register_key(&session->pending.description, (e_transport_platform)session->platform);
                if (!failed)
                    managed_session_successful_creation_complete(index);
            } else {
                s_online_managed_session *create = session;
                for (long context_index = 0; context_index < create->creation_context_count; context_index++) {
                    long user_index = session->desired.controller_index;
                    const s_online_context *context = &create->creation_contexts[context_index];
                    unsigned long context_id = context->id;
                    unsigned long context_value = context->value;
                    s_managed_user_context_data *user_context = &managed_user_globals.users[user_index].contexts[context_id];
                    unsigned long map_id = k_online_context_id_map[context_id];
                    unsigned long map_value = k_online_context_value_map[context_value];
                    user_context->value = context_value;
                    user_context->valid = true;
                    XUserSetContextEx(user_index, map_id, map_value, 0);
                }
                unsigned short creation_flags = 0x3fd;
                if (session->session_class != 1)
                    creation_flags = 0x3ff;
                operation_flag_set(session->primary_flags, 2, true);
                operation_flag_set(online_session_manager_globals.current_operation_flags, 2, true);
                network_managed_session_process_create(&session->task,
                    index, managed_session_creation_complete, &session->pending, creation_flags);
            }
        }
    }
    if (failed) {
        session_flag_set(session->flags, 4, false);
        session_flag_set(session->flags, 5, true);
        if (test_bit16(session->secondary_flags, 8)) {
            operation_flag_set(session->secondary_flags, 8, false);
            ::managed_session_build_add_xuid_list(index);
            managed_session_successful_players_add_complete(index);
        }
        if (test_bit16(session->secondary_flags, 10)) {
            operation_flag_set(session->secondary_flags, 10, false);
            session_flag_set(online_session_get(index)->flags, 13, true);
        }
        if (test_bit16(session->secondary_flags, 9)) {
            operation_flag_set(session->secondary_flags, 9, false);
            session_flag_set(online_session_get(index)->flags, 15, true);
        }
        operation_flag_set(session->secondary_flags, 2, true);
        session_flag_set(session->flags, 2, true);
        session->modification_time = network_time_get();
    }
}

// @address 0x3ff7d8 ?managed_session_create_host_migration_internal@@YAXJPAUs_online_managed_session@@@Z
void managed_session_create_host_migration_internal(long index, s_online_managed_session *session)
{
    if (session->host_migration_requires_new_key && session->previous_host) {
        memcpy(&session->previous_description, &session->actual.description, sizeof(session->previous_description));
        managed_session_successful_host_migration_creation_complete(index);
        return;
    }
    if (session->session_class == 0) {
        managed_session_successful_host_migration_creation_complete(index);
    } else if (session->session_class == 1) {
        bool failed = false;
        if (session->host_migration_requires_new_key)
            failed = !transport_secure_key_create(&session->previous_description, (e_transport_platform)session->platform);
        if (!failed)
            failed = !network_managed_session_register_key(&session->previous_description, (e_transport_platform)session->platform);
        if (!failed)
            session_flag_set(session->flags, 8, true);
        managed_session_successful_host_migration_creation_complete(index);
    } else {
        if (session->host_migration_requires_new_key)
            memset(&session->previous_description, 0, sizeof(session->previous_description));
        operation_flag_set(session->primary_flags, 4, true);
        operation_flag_set(online_session_manager_globals.current_operation_flags, 4, true);
        network_managed_session_process_host_migration(&session->task,
            index, (void (*)(long, bool, unsigned long))managed_session_host_migration_creation_complete, &session->actual,
            session->host_migration_requires_new_key, &session->previous_description);
        return;
    }
}

void managed_session_delete_host_migration_internal(long index, s_online_managed_session *session)
{
    if (online_managed_session_test_bit(session->flags, 8))
    {
        if (session->session_class != 0 && session->session_class == 1 && session->platform == 0)
            XNetUnregisterKey(&session->previous_description);
    }
    managed_session_successful_host_migration_delete_complete(index);
}

bool managed_session_build_removal_xuid_list(long index)
{
    s_online_managed_session *entry = online_session_get(index);
    for (long i = 0; i < 16; i++)
    {
        s_online_session_player *request = &entry->actual.players[i];
        if (test_bit16(request->flags, 0))
        {
            bool found = false;
            for (unsigned long j = 0; j < 16; j++)
            {
                s_online_session_player *player = &entry->desired.players[j];
                if (test_bit16(player->flags.m_storage, 0) && memcmp(&player->identifier, &request->identifier, sizeof(player->identifier)) == 0
                    && test_bit16(request->flags, 1) == test_bit16(player->flags.m_storage, 1))
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                entry->queued_player_xuids[entry->queued_player_count] = request->identifier;
                entry->queued_player_filter[entry->queued_player_count] = test_bit16(request->flags, 2);
                entry->queued_player_count++;
            }
        }
    }
    return entry->queued_player_count > 0;
}

bool managed_session_build_add_xuid_list(long index)
{
    s_online_managed_session *entry = online_session_get(index);
    for (long i = 0; i < 16; i++)
    {
        s_online_session_player *player = &entry->desired.players[i];
        if (test_bit16(player->flags.m_storage, 0))
        {
            bool found = false;
            for (unsigned long j = 0; j < 16; j++)
            {
                s_online_session_player *request = &entry->actual.players[j];
                if (test_bit16(request->flags, 0) && memcmp(&player->identifier, &request->identifier, sizeof(player->identifier)) == 0)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                entry->queued_player_xuids[entry->queued_player_count] = player->identifier;
                entry->queued_player_filter[entry->queued_player_count] = test_bit16(player->flags.m_storage, 2);
                entry->queued_player_private_slots[entry->queued_player_count] = test_bit16(player->flags.m_storage, 1);
                entry->queued_player_count++;
            }
        }
    }
    return entry->queued_player_count > 0;
}

extern "C" void add_to_player_list(s_online_session_player *players, long player_count, const unsigned __int64 *xuids,
    const bool *flags, const bool *private_flags, long count)
{
    for (long i = 0; i < count; i++)
    {
        long empty_index = -1;
        if (xuids[i] != 0)
        {
            long j;
            for (j = 0; j < 16; j++)
            {
                s_online_session_player *p = &players[j];
                if (!(p->flags.m_storage & 1) && empty_index == -1)
                    empty_index = j;
                else if (memcmp(&xuids[i], &p->identifier, sizeof(p->identifier)) == 0)
                    break;
            }
            if (j == 16)
            {
                s_online_session_player *p = &players[empty_index];
                p->identifier = xuids[i];
                p->flags.m_storage |= 1;
                if (private_flags[i])
                    p->flags.m_storage |= 2;
                if (flags[i])
                    p->flags.m_storage |= 4;
            }
        }
    }
}

extern "C" void remove_from_player_list(s_online_session_player *players, long player_count, const unsigned __int64 *identifiers,
    long identifier_count)
{
    for (long i = 0; i < identifier_count; i++)
    {
        if (identifiers[i])
        {
            s_online_session_player *player;
            long j;
            for (j = 0, player = players; j < 16; j++, player++)
            {
                if ((player->flags.m_storage & 1) && memcmp(&player->identifier, &identifiers[i], sizeof(player->identifier)) == 0)
                {
                    player->flags.m_storage &= ~1;
                    memset(player, 0, sizeof(*player));
                    break;
                }
            }
        }
    }
}

// @address 0x46b80 ??_7c_managed_session_overlapped_task@@6B@
// @address 0x3ffde8 ?managed_session_allocate@@YAJXZ
long managed_session_allocate(void)
{
  s_online_managed_session *entries = online_session_manager_globals.sessions;
  unsigned long index = 0;
  for (; index < 8; index++) {
    if (!(entries[index].flags & 1))
      break;
  }
  if (index != 8) {
    s_online_managed_session *entry = &entries[index];
    memset(entry, 0, sizeof(s_online_managed_session));
    entry->flags = 0;
    entry->session_class = -1;
    session_flag_set(entry->flags, 0, true);
    entry->primary_flags = 0;
    entry->secondary_flags = 0;
    memset(&entry->desired.controller_index, 0, sizeof(entry->desired));
    memset(&entry->pending, 0, sizeof(entry->pending));
    memset(&entry->actual, 0, sizeof(entry->actual));
    new (&entry->task) c_managed_session_overlapped_task;
  }
  return index;
}

static __forceinline bool network_managed_session_xuid_is_guest_account(unsigned __int64 xuid)
{
    unsigned int account_flags = (unsigned int)(xuid >> 48);
    bool result = (account_flags & 15) == 9 && (account_flags & 192) != 0;
    return result;
}

static __forceinline bool network_managed_session_controller_is_guest(const c_controller_interface *controller)
{
    bool result = false;
    if (controller->get_user_index() >= 0 && controller->get_user_index() < 4)
        result = network_managed_session_xuid_is_guest_account(online_presence_user_slot(controller->get_user_index()).xuid);
    return result;
}

e_controller_index get_first_signed_in_controller_index()
{
    e_controller_index result = _controller_none;
    for (e_controller_index index = first_controller(); index != _controller_none;
        index = next_controller(index))
    {
        c_controller_interface *controller = controller_get(index);
        if (controller_in_use(controller) && !network_managed_session_controller_is_guest(controller))
        {
            result = index;
            break;
        }
    }
    return result;
}

static __forceinline void network_bandwidth_set_online_network_environment(bool online)
{
#if defined(_XBOX) && !defined(__clang__)
    // match: 0x3fffd0: the store at +24 and observer load at +12 keep the listener base in r31 across the estimate update.
    s_bandwidth_globals &bandwidth = transport_qos_shutdown_globals.bandwidth;
#else
    s_bandwidth_globals &bandwidth = network_bandwidth_globals;
#endif
    bandwidth.online_network_environment = online;
    bandwidth.observer->set_online_network_environment(online);
    ::network_bandwidth_update_estimate();
}
static __forceinline bool network_managed_session_qos_listener_initiate(const s_transport_secure_identifier *identifier)
{
    bool result = false;
    if (transport_qos_get_globals().initialized) {
        long error = XNetQosListen(identifier, 0, 0, 0, 1);
        if (error == 0)
            result = true;
    }
    return result;
}
// @address 0x3fffd0 ?managed_session_successful_creation_complete@@YAXJ@Z
void managed_session_successful_creation_complete(long index)
{
    s_online_managed_session *session = online_session_get(index);
    session->actual.flags.m_storage = session->pending.flags.m_storage;
    session->actual.controller_index = session->pending.controller_index;
    session->actual.public_slot_count = session->pending.public_slot_count;
    session->actual.private_slot_count = session->pending.private_slot_count;
    session->actual.nonce = session->pending.nonce;
    session->actual.description = session->pending.description;
    session->actual.handle = session->pending.handle;
    session->desired.description = session->pending.description;
    session->desired.handle = session->actual.handle;
    session_flag_set(session->flags, 4, true);
    session_flag_set(session->flags, 5, false);
    bool has_players = false;
    for (unsigned long player_index = 0; player_index < 16; ++player_index) {
        if (test_bit16(session->actual.players[player_index].flags, 0)) {
            has_players = true;
            break;
        }
    }
    if (has_players) {
        memset(session->actual.players, 0, sizeof(session->actual.players));
        session_flag_set(session->flags, 10, true);
        operation_flag_set(session->secondary_flags, 7, true);
        operation_flag_set(session->secondary_flags, 8, true);
    }
    if (session->session_class != 0) {
        bool online = session->session_class == 2;
        network_bandwidth_set_online_network_environment(online);
        network_managed_session_qos_listener_initiate(&session->actual.description.secure_identifier);
    }
}
// @address 0x400140 ?managed_session_creation_complete@@YAXJ_NK@Z
void managed_session_creation_complete(long index, bool success, unsigned long error)
{
    s_online_managed_session *session = online_session_get(index);
    operation_flag_set(session->primary_flags, 2, false);
    operation_flag_set(online_session_manager_globals.current_operation_flags, 2, false);
    if (!success) {
        session->pending.flags.m_storage = session->actual.flags.m_storage;
        session->pending.controller_index = session->actual.controller_index;
        session->pending.public_slot_count = session->actual.public_slot_count;
        session->pending.private_slot_count = session->actual.private_slot_count;
        session->pending.nonce = session->actual.nonce;
        session->pending.description = session->actual.description;
        session->pending.handle = session->actual.handle;
        session_flag_set(session->flags, 4, false);
        session_flag_set(session->flags, 5, true);
        operation_flag_set(session->secondary_flags, 2, true);
        session_flag_set(session->flags, 2, true);
        session->modification_time = g_network_time_globals.enabled ? g_network_time_globals.time : GetTickCount();
    } else {
        managed_session_successful_creation_complete(index);
    }
}

void managed_session_successful_delete_complete(long index)
{
    s_online_managed_session *session = online_session_get(index);
    session->actual.handle = 0;
    session_flag_set(session->flags, 4, false);
    session_flag_set(session->flags, 5, false);
    if (test_bit16(session->secondary_flags, 1)) {
        operation_flag_set(session->secondary_flags, 1, false);
        network_managed_session_free(index);
    } else {
        memset(&session->previous_description, 0, sizeof(session->previous_description));
        memset(&session->actual, 0, sizeof(session->actual));
        memset(&session->pending, 0, sizeof(session->pending));
    }
}

// @address 0x400340 ?managed_session_deletion_complete@@YAXJ_NK@Z
void managed_session_deletion_complete(long index, bool success, unsigned long error)
{
    s_online_managed_session *session = online_session_get(index);
    operation_flag_set(session->primary_flags, 0, false);
    operation_flag_set(online_session_manager_globals.current_operation_flags, 0, false);
    CloseHandle(session->actual.handle);
    session->desired.handle = 0;
    session->pending.handle = 0;
    session->actual.handle = 0;
    managed_session_successful_delete_complete(index);
}

// @address 0x4003c0 ?managed_session_successful_host_migration_creation_complete@@YAXJ@Z
void managed_session_successful_host_migration_creation_complete(long managed_session_index)
{
    s_online_managed_session *session = online_session_get(managed_session_index);

    session_flag_set(session->flags, 7, true);
    operation_flag_set(session->desired.flags.m_storage, 0, session->host_migration_requires_new_key);
    operation_flag_set(session->pending.flags.m_storage, 0, session->host_migration_requires_new_key);
    operation_flag_set(session->actual.flags.m_storage, 0, session->host_migration_requires_new_key);
    if (session->session_class == 1)
        transport_qos_listener_initiate(&session->actual.description.secure_identifier);
}

extern "C" void managed_session_successful_host_migration_delete_complete(long index)
{
    s_online_managed_session *session = online_session_get(index);
    session_flag_set(session->flags, 7, false);
    session_flag_set(session->flags, 8, false);
    if (!test_bit16(session->secondary_flags, 4))
        memset(&session->previous_description, 0, sizeof(session->previous_description));
}

// @address 0x4004c0 managed_session_host_migration_creation_complete
extern "C" void managed_session_host_migration_creation_complete(long index, bool success)
{
    s_online_managed_session *session = online_session_get(index);
    operation_flag_set(session->primary_flags, 4, false);
    operation_flag_set(online_session_manager_globals.current_operation_flags, 4, false);
    if (network_managed_session_description_is_empty(&session->previous_description))
        success = false;
    if (success) {
        session_flag_set(session->flags, 8, true);
        managed_session_successful_host_migration_creation_complete(index);
    } else {
        managed_session_completely_reset_session_on_error(index);
    }
}
static __forceinline void network_managed_session_successful_modify_complete(long index)
{
    s_online_managed_session *session = online_session_get(index);
    session->actual.flags.m_storage = session->pending.flags.m_storage;
    session->actual.public_slot_count = session->pending.public_slot_count;
    session->actual.private_slot_count = session->pending.private_slot_count;
}
// @address 0x4005b0 ?managed_session_modify_complete@@YAXJ_NK@Z
void managed_session_modify_complete(long index, bool success, unsigned long error)
{
    s_online_managed_session *session = online_session_get(index);
    operation_flag_set(session->primary_flags, 7, false);
    operation_flag_set(online_session_manager_globals.current_operation_flags, 7, false);
    if (success) {
        network_managed_session_successful_modify_complete(index);
    } else {
        session->pending.flags.m_storage = session->actual.flags.m_storage;
        session->pending.public_slot_count = session->actual.public_slot_count;
        session->pending.private_slot_count = session->actual.private_slot_count;
        managed_session_completely_reset_session_on_error(index);
    }
}

void managed_session_successful_players_add_complete(long index)
{
    s_online_managed_session *session = online_session_get(index);
    unsigned long flags = session->flags;
    if (session->queued_player_count != 0)
        add_to_player_list(session->actual.players, 16, session->queued_player_xuids,
            (const bool *)session->queued_player_filter, (const bool *)session->queued_player_private_slots, session->queued_player_count);
    session_flag_set(flags, 11, true);
    session->queued_player_count = 0;
    session_flag_set(flags, 10, false);
    session->flags = flags;
}

// @address 0x4006a0 ?managed_session_players_add_complete@@YAXJ_NK@Z
void managed_session_players_add_complete(long index, bool success, unsigned long error)
{
    s_online_managed_session *session = online_session_get(index);
    operation_flag_set(session->primary_flags, 8, false);
    operation_flag_set(online_session_manager_globals.current_operation_flags, 8, false);
    if (success) {
        managed_session_successful_players_add_complete(index);
    } else {
        unsigned long flags = session->flags;
        if (session->queued_player_count != 0)
            remove_from_player_list(session->desired.players, 16, session->queued_player_xuids, session->queued_player_count);
        session_flag_set(flags, 12, true);
        session->queued_player_count = 0;
        session_flag_set(flags, 10, false);
        session->flags = flags;
    }
}

void managed_session_successful_players_remove_complete(long index)
{
    s_online_managed_session *session = online_session_get(index);
    if (session->queued_player_count != 0)
        remove_from_player_list(session->actual.players, 16, session->queued_player_xuids, session->queued_player_count);
    session->queued_player_count = 0;
}

// @address 0x4007a8 ?managed_session_players_remove_complete@@YAXJ_NK@Z
void managed_session_players_remove_complete(long index, bool success, unsigned long error)
{
    s_online_managed_session *session = online_session_get(index);
    operation_flag_set(session->primary_flags, 6, false);
    operation_flag_set(online_session_manager_globals.current_operation_flags, 6, false);
    if (success) {
        managed_session_successful_players_remove_complete(index);
    } else {
        operation_flag_set(session->secondary_flags, 6, true);
        session_flag_set(session->flags, 2, true);
        session->modification_time = network_time_get();
        session->queued_player_count = 0;
    }
}

static __forceinline void managed_session_successful_game_start_complete(long index)
{
    s_online_managed_session *session = online_session_get(index);
    session_flag_set(session->flags, 13, true);
}

void managed_session_game_start_complete(long index, bool success, unsigned long error)
{
    s_online_managed_session *session = online_session_get(index);
    operation_flag_set(session->primary_flags, 10, false);
    operation_flag_set(online_session_manager_globals.current_operation_flags, 10, false);
    if (success)
        managed_session_successful_game_start_complete(index);
    else
        session_flag_set(session->flags, 14, true);
}

static __forceinline void managed_session_successful_game_end_complete(long index)
{
    s_online_managed_session *session = online_session_get(index);
    session_flag_set(session->flags, 15, true);
}

void managed_session_game_end_complete(long index, bool success, unsigned long error)
{
    s_online_managed_session *session = online_session_get(index);
    operation_flag_set(session->primary_flags, 9, false);
    operation_flag_set(online_session_manager_globals.current_operation_flags, 9, false);
    if (success)
        managed_session_successful_game_end_complete(index);
    else
        session_flag_set(session->flags, 16, true);
}

class c_network_observer;


// @address 0x3cf700 ?disband_and_reestablish_as_host@c_network_session@@QAAX_N@Z
static __forceinline c_network_session *network_managed_session_find(long index)
{
    c_network_session *result = 0;
    for (unsigned long session_index = 0; session_index < 3; ++session_index) {
        if (g_network_session_manager->m_sessions[session_index] &&
            !g_network_session_manager->m_sessions[session_index]->disconnected() &&
            g_network_session_manager->m_sessions[session_index]->managed_session_index() == index) {
            result = g_network_session_manager->m_sessions[session_index];
            break;
        }
    }
    return result;
}
// @address 0x400920 ?managed_session_completely_reset_session_on_error@@YAXJ@Z
void managed_session_completely_reset_session_on_error(long index)
{
    c_network_session *session = 0;
    session = network_managed_session_find(index);
    if (session)
        session->disband_and_reestablish_as_host(false);
    else
        managed_session_reset_session(index, false);
}

// @address 0x3fdb60 managed_session_remove_players

extern "C" void managed_session_remove_players(long index, const unsigned __int64 *identifiers, long identifier_count)
{
    s_online_managed_session *session = online_session_get(index);
    remove_from_player_list(session->desired.players, 16, identifiers, identifier_count);
    operation_flag_set(session->secondary_flags, 6, true);
    operation_flag_set(session->secondary_flags, 7, true);
    session_flag_set(session->flags, 2, false);
    session->modification_time = 0;
}

// @address 0x42d268 managed_session_get_context_string
extern "C" const char *managed_session_get_context_string(void)
{
    return "map-variant";
}

__forceinline void c_managed_session_overlapped_task::process_modify(long context, void (*callback)(long, bool, unsigned long),
    s_online_session *session, s_online_session *desired_session, s_online_session *actual_session)
{
    m_task_type = 3;
    m_session = session;
    m_desired_session = desired_session;
    m_actual_session = actual_session;
    m_context = context;
    m_callback = callback;
    m_complete = false;
    m_result = 0;
    if (!overlapped_task_start_internal(this, "..\\networking\\online\\online_session_xenon.cpp", 385))
        complete();
}

__forceinline void c_managed_session_overlapped_task::process_add_players(long context, void (*callback)(long, bool, unsigned long),
    s_online_session *session, const qword *player_xuids, const bool *player_filter, const bool *player_private_slots, long player_count)
{
    m_task_type = 4;
    m_session = session;
    m_context = context;
    m_callback = callback;
    m_complete = false;
    m_result = 0;
    filter_local_users(player_count, player_xuids, player_filter, player_private_slots);
    if (m_player_count == 0)
    {
        m_complete = true;
        complete();
    }
    else if (!overlapped_task_start_internal(this, "..\\networking\\online\\online_session_xenon.cpp", 415))
    {
        complete();
    }
}

__forceinline void c_managed_session_overlapped_task::process_game_start(long context, void (*callback)(long, bool, unsigned long),
    s_online_session *session)
{
    m_task_type = 6;
    m_session = session;
    m_context = context;
    m_callback = callback;
    m_complete = false;
    m_result = 0;
    if (!overlapped_task_start_internal(this, "..\\networking\\online\\online_session_xenon.cpp", 569))
        complete();
}

__forceinline void c_managed_session_overlapped_task::process_game_end(long context, void (*callback)(long, bool, unsigned long),
    s_online_session *session)
{
    m_task_type = 7;
    m_session = session;
    m_context = context;
    m_callback = callback;
    m_complete = false;
    m_result = 0;
    if (!overlapped_task_start_internal(this, "..\\networking\\online\\online_session_xenon.cpp", 590))
        complete();
}

template<class t_flags, class t_enum>
static __forceinline bool managed_session_test_flags(const t_flags &flags, t_enum bit)
{
    bool result = (flags.m_storage & (1 << bit)) != 0;
    return result;
}

// @address 0x3feb68 ?managed_session_process_pending_operations@@YAXJ@Z
void managed_session_process_pending_operations(long index)
{
    s_online_managed_session *session = online_session_get(index);

    if (managed_session_test_flags(session->managed_flags, _online_managed_session_create_failed_bit) &&
        session->desired.controller_index == -1 && session->session_class == _network_session_class_xbox_live)
    {
        if (get_first_signed_in_controller_index() != _controller_none)
        {
            session->pending_operation_bits.set(_online_managed_session_operation_create_bit, true);
            session->managed_flags.set(_online_managed_session_retry_pending_bit, false);
            session->modification_time = 0;
        }
    }
    else if (managed_session_test_flags(session->managed_flags, _online_managed_session_create_failed_bit) && session->create_failed_network_unavailable)
    {
        if (g_transport_security_globals.address_resolved)
        {
            session->create_failed_network_unavailable = false;
            session->pending_operation_bits.set(_online_managed_session_operation_create_bit, true);
            session->managed_flags.set(_online_managed_session_retry_pending_bit, false);
            session->modification_time = 0;
        }
    }
    else if (managed_session_test_flags(session->managed_flags, _online_managed_session_create_failed_bit) && session->create_failed_online_disabled)
    {
        if (online_local_xuid_is_online_enabled())
        {
            session->create_failed_online_disabled = false;
            session->pending_operation_bits.set(_online_managed_session_operation_create_bit, true);
            session->managed_flags.set(_online_managed_session_retry_pending_bit, false);
            session->modification_time = 0;
        }
    }
    else if (managed_session_test_flags(session->managed_flags, _online_managed_session_create_failed_bit) && session->create_failed_session_id_in_use &&
        test_bit16(session->desired.flags.m_storage, 0))
    {
        if (index == managed_session_who_has_this_session_id(&session->desired.description.secure_identifier))
        {
            session->create_failed_session_id_in_use = false;
            session->pending_operation_bits.set(_online_managed_session_operation_create_bit, true);
            session->managed_flags.set(_online_managed_session_retry_pending_bit, false);
            session->modification_time = 0;
        }
    }
    else if (managed_session_test_flags(session->managed_flags, _online_managed_session_retry_pending_bit) &&
        network_time_since(session->modification_time) >= g_network_configuration.managed_session_retry_interval)
    {
        session->managed_flags.set(_online_managed_session_retry_pending_bit, false);
        session->modification_time = 0;
    }

    if (online_session_manager_globals.current_operations.is_empty() &&
        !session->pending_operation_bits.is_empty() &&
        (!managed_session_test_flags(session->managed_flags, _online_managed_session_retry_pending_bit) ||
            managed_session_test_flags(session->pending_operation_bits, _online_managed_session_operation_delete_bit)))
    {
        long pending = 0;
        c_online_managed_session_operation_flags operation;
        pending = session->pending_operation_bits.get_unsafe();
        operation.set_unsafe((unsigned short)(pending & ~(pending & (pending - 1))));

        if (managed_session_test_flags(session->managed_flags, _online_managed_session_established_bit) ||
            managed_session_test_flags(operation, _online_managed_session_operation_create_bit) ||
            managed_session_test_flags(operation, _online_managed_session_operation_delete_bit))
        {
            session->pending_operation_bits &= ~operation;

            if (managed_session_test_flags(operation, _online_managed_session_operation_delete_bit))
            {
                session->pending_operation_bits.set(_online_managed_session_operation_remove_players_bit, false);
                if (managed_session_test_flags(session->managed_flags, _online_managed_session_host_migration_bit))
                {
                    managed_session_delete_host_migration_internal(index, session);
                    session->pending_operation_bits.set(_online_managed_session_operation_delete_host_migration_bit, false);
                    session->pending_operation_bits.set(_online_managed_session_operation_delete_bit, true);
                }
                else
                {
                    managed_session_delete_session_internal(index, session);
                    session = 0;
                }
            }
            else if (managed_session_test_flags(operation, _online_managed_session_operation_create_bit))
            {
                managed_session_create_session_internal(index, session);
            }
            else if (managed_session_test_flags(operation, _online_managed_session_operation_delete_host_migration_bit))
            {
                managed_session_delete_host_migration_internal(index, session);
            }
            else if (managed_session_test_flags(operation, _online_managed_session_operation_create_host_migration_bit))
            {
                managed_session_create_host_migration_internal(index, session);
            }
            else if (managed_session_test_flags(operation, _online_managed_session_operation_reset_players_bit))
            {
                memset(session->actual.players, 0, sizeof(session->actual.players));
                session->pending_operation_bits.set(_online_managed_session_operation_add_players_bit, true);
            }
            else if (managed_session_test_flags(operation, _online_managed_session_operation_remove_players_bit))
            {
                if (managed_session_build_removal_xuid_list(index))
                {
                    if (session->session_class == 0 || session->session_class == 1)
                    {
                        managed_session_successful_players_remove_complete(index);
                    }
                    else
                    {
                        session->current_operation_bits = operation;
                        online_session_manager_globals.current_operations = operation;
                        session->task.process_remove_players(index, managed_session_players_remove_complete,
                            &session->pending, session->queued_player_xuids, (const bool *)session->queued_player_filter, session->queued_player_count);
                    }
                }
            }
            else if (managed_session_test_flags(operation, _online_managed_session_operation_modify_bit))
            {
                session->pending.flags.m_storage = session->desired.flags.m_storage;
                session->pending.public_slot_count = session->desired.public_slot_count;
                session->pending.private_slot_count = session->desired.private_slot_count;
                if (session->session_class == 0 || session->session_class == 1)
                {
                    network_managed_session_successful_modify_complete(index);
                }
                else if (!managed_session_test_flags(session->managed_flags, _online_managed_session_modify_locked_bit))
                {
                    session->current_operation_bits = operation;
                    online_session_manager_globals.current_operations = operation;
                    session->task.process_modify(index, managed_session_modify_complete,
                        &session->pending, &session->desired, &session->actual);
                    if (managed_session_test_flags(session->managed_flags, _online_managed_session_modify_locked_pending_bit))
                        session->managed_flags.set(_online_managed_session_modify_locked_bit, true);
                }
                else
                {
                    network_managed_session_successful_modify_complete(index);
                }
            }
            else if (managed_session_test_flags(operation, _online_managed_session_operation_add_players_bit))
            {
                if (managed_session_build_add_xuid_list(index))
                {
                    if (session->session_class == 0 || session->session_class == 1)
                    {
                        managed_session_successful_players_add_complete(index);
                    }
                    else
                    {
                        session->current_operation_bits = operation;
                        online_session_manager_globals.current_operations = operation;
                        session->task.process_add_players(index, managed_session_players_add_complete,
                            &session->pending, session->queued_player_xuids, (const bool *)session->queued_player_filter,
                            (const bool *)session->queued_player_private_slots, session->queued_player_count);
                    }
                }
                else
                {
                    managed_session_successful_players_add_complete(index);
                }
            }
            else if (managed_session_test_flags(operation, _online_managed_session_operation_game_end_bit))
            {
                if (session->session_class == 0 || session->session_class == 1)
                {
                    managed_session_successful_game_end_complete(index);
                }
                else
                {
                    session->current_operation_bits = operation;
                    online_session_manager_globals.current_operations = operation;
                    session->task.process_game_end(index, managed_session_game_end_complete,
                        &session->pending);
                }
            }
            else
            {
                if (session->session_class == 0 || session->session_class == 1)
                {
                    managed_session_successful_game_start_complete(index);
                }
                else
                {
                    session->current_operation_bits = operation;
                    online_session_manager_globals.current_operations = operation;
                    session->task.process_game_start(index, managed_session_game_start_complete,
                        &session->pending);
                }
            }
        }
    }
}
