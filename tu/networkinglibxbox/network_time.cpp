// networkinglibxbox/network_time.obj: from the records 0x3d3508 0x3d8418
// @flags /TP /O2 /Oc
// @address 0x3d3508 ?network_session_time_get_time@@YAKPBUs_transport_secure_identifier@@@Z
// @address 0x3d8418 ?network_time_since@@YAJPAVc_simulation_view@@@Z
// @address 0x80608 GetTickCount

#include "networking/network_time.h"
#include "networking/session/network_session.h"
#include "networking/network_time_types.h"


#include "networking/session/network_session_manager.h"

#include "simulation/simulation_view.h"

#include "cseries/crt_xdk.h"
#include "networking/transport/transport_security_types.h"

// @address 0x8c447c g_network_time_globals
extern "C" {
s_network_time_override g_network_time_globals;
}

long network_time_since(c_simulation_view *view)
{
    c_simulation_view *v = view;
    v->m_catchup_finish_time = g_network_time_globals.enabled ? g_network_time_globals.time : GetTickCount();
    if (v->m_catchup_attempt != -1) {
        v->m_catchup_attempt = -1;
        v->m_catchup_data = 0;
        v->m_catchup_start = -1;
        v->m_catchup_end = -1;
    }
    return v->m_catchup_finish_time;
}

static __forceinline unsigned long network_time_add_session_tick(unsigned long session_time, unsigned long tick)
{
    return session_time + tick;
}

unsigned long network_session_time_get_time(const s_transport_secure_identifier *transport_secure_identifier)
{
    unsigned long result = 0;
    c_network_session_manager *session_manager = g_network_time_globals.session_manager;
    if (session_manager != 0) {
        c_network_session *session = session_manager->get_session(transport_secure_identifier);
        if (session != 0 && session->time_exists()) {
            result = network_time_add_session_tick(session->m_time, GetTickCount());
        }
    }
    return result;
}
