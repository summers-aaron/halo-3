#include "networking/logic/network_search_types.h"
// networkinglibxbox/network_broadcast_search.obj: from the records 0x905f8 0x400d30 0x402988
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "shell/shell.h"
// @address 0x402988 ?network_broadcast_search_handle_reply@@YAXPBUtransport_address@@PBUs_network_message_broadcast_reply@@@Z
// @address 0x400d30 ?network_banhammer_controller_idle_for_too_long@@YA_NW4e_controller_index@@@Z
// @address 0x905f8 ?network_broadcast_search_update@@YAXXZ
#include <string.h>
#include "networking/logic/network_broadcast_search.h"
#include "networking/transport/transport_address.h"
#include "networking/transport/transport_security.h"
#include "networking/messages/network_message_type_collection.h"
#include "networking/logic/network_banhammer_controller_types.h"
#include "networking/logic/network_banhammer.h"
#include "networking/network_configuration.h"
#include "networking/network_configuration_types.h"
#include "networking/network_time_types.h"
// @address 0x8c447c g_network_time_globals
#include "cseries/crt_xdk.h"
#include "networking/messages/network_message_broadcast_reply.h"
#include "networking/messages/network_messages_out_of_band.h"
#include "networking/network_time_access.h"
#include "networking/network_time_since_access.h"
#include "networking/messages/network_message_gateway.h"
#include "networking/logic/network_broadcast_search_types.h"
static s_network_broadcast_search_globals g_broadcast_search_globals;

void network_broadcast_search_dispose()
{
    memset(&g_broadcast_search_globals, 0, sizeof(g_broadcast_search_globals));
}

void network_broadcast_search_end()
{
    if (g_broadcast_search_globals.searching)
        // match: 0x402d88: spelled `= false`, the zero is hoisted into nonvolatile r28 and the frame grows 112->128B
        g_broadcast_search_globals.searching = (bool)(_rotl(g_broadcast_search_globals.searching & ~1u, 32) & 1);
}

static __forceinline void network_broadcast_search_session_expire(s_network_broadcast_search_globals *globals, s_available_session *session)
{
    memset(session, 0, sizeof(*session));
    globals->changed = true;
}
void network_broadcast_search_update()
{
    if (g_broadcast_search_globals.searching) {
        if (network_time_since(g_broadcast_search_globals.last_search_time) > 2000) {
            transport_address address;
            s_network_message_broadcast_search message;
            memset(&message, 0, sizeof(message));
            address.address_length = 4;
            message.protocol = 7;
            address.address.ipv4_address = 0xffffffff;
            message.search_nonce = g_broadcast_search_globals.search_nonce;
            address.port = 1001;
            g_broadcast_search_globals.message_gateway->send_message_directed(&address, _network_message_type_broadcast_search, sizeof(message), &message);
            g_broadcast_search_globals.last_search_time = network_time_get();
        }
        for (long session_index = 0; session_index < g_broadcast_search_globals.session_count; ++session_index) {
            s_available_session *session = &g_broadcast_search_globals.sessions[session_index];
            if (session->valid && network_time_since(session->last_reply_time) > 4000) {
                network_broadcast_search_session_expire(&g_broadcast_search_globals, session);
            }
        }
    }
}
bool network_banhammer_controller_idle_for_too_long(e_controller_index controller_index)
{
    s_network_banhammer_controller_data *controller = &g_network_banhammer_globals.controllers[controller_index];
    bool idle_for_too_long = false;
    if ((controller->flags & 1) != 0 &&
        (network_time_since(controller->session_start_time) >= g_network_configuration.controller_idle_timeout_seconds * 1000 ||
         network_time_since(controller->last_activity_time) >= g_network_configuration.controller_idle_timeout_seconds * 1000))
    {
        idle_for_too_long = true;
    }
    return idle_for_too_long;
}

void network_broadcast_search_handle_reply(const transport_address *from, const s_network_message_broadcast_reply *message)
{
    if (g_broadcast_search_globals.searching) {
        long existing_session_index = -1;
        long unused_session_index = -1;
        long replacement_session_index = -1;
        long session_index;
        for (session_index = 0; session_index < g_broadcast_search_globals.session_count; ++session_index) {
            s_available_session *session = &g_broadcast_search_globals.sessions[session_index];
            if (!session->valid) {
                if (unused_session_index == -1)
                    unused_session_index = session_index;
            }
            else if (transport_secure_address_compare(
                         &session->status.details.secure_address, &((const s_network_squad_status_data *)&message->status)->details.secure_address))
            {
                existing_session_index = session_index;
                break;
            } else if (((const s_network_squad_status_data *)&message->status)->details.sort_priority != 0 &&
                       ((const s_network_squad_status_data *)&message->status)->details.join_mode == 0 &&
                       ((const s_network_squad_status_data *)&message->status)->details.sort_priority < session->status.details.sort_priority) {
                replacement_session_index = session_index;
            }
        }
        if (existing_session_index != -1)
            session_index = existing_session_index;
        else if (unused_session_index != -1)
            session_index = unused_session_index;
        else if (replacement_session_index != -1)
            session_index = replacement_session_index;
        else
            session_index = -1;
        if (session_index != -1) {
            s_available_session *session = &g_broadcast_search_globals.sessions[session_index];
            if (session_index != existing_session_index)
                memset(session, 0, sizeof(*session));
            if (memcmp(&session->status, &message->status, sizeof(session->status)) != 0) {
                memcpy(&session->status, &message->status, sizeof(session->status));
                session->changed = true;
                g_broadcast_search_globals.changed = true;
            }
            session->valid = true;
            session->last_reply_time = network_time_get();
            session->replied = true;
        }
    }
}
