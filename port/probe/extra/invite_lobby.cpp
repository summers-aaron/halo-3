// port/probe/extra (port/GAME-CHANGES.md, "Game invites and lobby codes"): the lobby the players are in, reported to
// the port's game invites (port/sdk/online/invite.h) once per UI update (update_xenon_messages, before the
// notifications are read). PC-only game code, compiled like a game unit; the console build has no such unit.
//
// On the console an invite names the Xbox Live session that has the inviter's presence, which XAM knows from
// XSessionCreate (the port's xlive_offline.cpp reports those too). A System Link lobby has no XSession: its sessions
// are keys the game registers with XNet. So the lobby is read where the game keeps it, as its own post-match rematch
// does (life_cycle_handler_post_match.cpp): the life cycle's squad session, once established, and its managed
// session's security information (managed_session_get_security_information: the session id, the host's secure
// address and the key exchange key, an XSESSION_INFO's layout). A member's is the host's
// (managed_session_create_client keeps the description it joined), so any player in the lobby can share it. Offline
// (session class 0), between sessions or before the life cycle starts there is no lobby another console could join.
#include <xtl.h>
#include <string.h>
#include "cseries/cseries.h"
#include "networking/logic/life_cycle/life_cycle_globals_access.h"
#include "networking/logic/life_cycle/life_cycle_globals_types.h"
#include "networking/session/network_managed_session.h"
#include "networking/session/network_session.h"
#include "networking/session/network_session_class.h"
#include "networking/transport/transport_platform_types.h"
#include "networking/transport/transport_security_types.h"
#include "online/invite.h"

typedef char invite_lobby_description_size[
    sizeof(s_transport_session_description) == PORT_INVITE_SESSION_SIZE ? 1 : -1];

extern "C" void invite_lobby_update(void)
{
    port_invite_lobby lobby;
    memset(&lobby, 0, sizeof(lobby));
    if (g_network_life_cycle_globals.initialized) {
        c_network_session *squad = g_network_life_cycle_globals.manager.get_active_squad_session();
        s_transport_session_description description;
        e_transport_platform platform;
        if (squad != NULL && squad->established() && squad->session_class() != _network_session_class_offline &&
            squad->session_class() != _network_session_class_none &&
            managed_session_get_security_information(squad->managed_session_index(), false, &description, &platform) &&
            platform == _transport_platform_xenon) {
            memcpy(lobby.session, &description, sizeof(description));
            lobby.session_class = squad->session_class() == _network_session_class_xbox_live
                                      ? PORT_INVITE_CLASS_LIVE
                                      : PORT_INVITE_CLASS_SYSTEM_LINK;
            lobby.host = squad->is_host();
        }
    }
    port_invite_set_squad(&lobby);
}
