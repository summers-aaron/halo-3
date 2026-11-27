#ifndef NETWORKING_LIFE_CYCLE_MANAGER_STATE_TYPES_H
#define NETWORKING_LIFE_CYCLE_MANAGER_STATE_TYPES_H

#include "memory/static_arrays.h"
#include "networking/logic/life_cycle/life_cycle_manager.h"

class c_network_session;
class c_network_session_manager;
class c_network_observer;

// Plain-data mirror of c_life_cycle_state_manager's session/state-change head
// (include/networking/logic/life_cycle/life_cycle_handler_types.h): current_state,
// handlers and session_manager are m_current_state, m_handlers, m_session_manager,
// observer is m_observer, and the entry-data tail is the m_entry_data block.
struct s_life_cycle_manager_state {
    e_life_cycle_state current_state;
    c_static_array<c_life_cycle_state_handler *, k_life_cycle_state_count> handlers;
    c_network_session_manager *session_manager;
    c_network_session *active_squad_session;
    c_network_session *target_session;
    c_network_session *group_session;
    c_network_observer *observer;
    bool state_change_requested;
    e_life_cycle_state requested_state;
    long state_change_data_size;
    char state_change_data[16];
};

#endif // NETWORKING_LIFE_CYCLE_MANAGER_STATE_TYPES_H
