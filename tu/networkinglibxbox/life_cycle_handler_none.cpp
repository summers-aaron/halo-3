// networkinglibxbox/life_cycle_handler_none.obj: from the records 0x3defa0
// @flags /TP /O2 /Oc

#include "networking/logic/network_session_interface.h"
#include "networking/logic/network_join.h"
#include "networking/logic/network_join_types.h"
#include "networking/session/network_session.h"
#include "networking/logic/life_cycle/life_cycle_handler_none.h"

// @address 0x3defa0 ?enter@c_life_cycle_state_handler_none@@UAAXPAVc_life_cycle_state_handler@@JPAX@Z
void c_life_cycle_state_handler_none::enter(c_life_cycle_state_handler *from_handler, long entry_data_size, void *entry_data)
{
    c_life_cycle_state_handler::enter(from_handler, 0, 0);
    c_network_session *target_session = m_manager->m_target_squad_session;
    if (!target_session->disconnected()) {
        target_session->m_disconnect_reestablish_state = 1;
        target_session->disconnect();
    }
    network_group_session_set_disable_host_election(0);
    if (g_network_join_data.join_queue_mode != _network_join_closed_to_all_joins)
        network_join_flush_join_queue();
    g_network_join_data.join_queue_mode = _network_join_closed_to_all_joins;
}

// @address 0x5bec98 ?exit@c_life_cycle_state_handler_none@@UAAXPAVc_life_cycle_state_handler@@@Z
void c_life_cycle_state_handler_none::exit(c_life_cycle_state_handler *to_handler)
{
    c_life_cycle_state_handler::exit(to_handler);
}
