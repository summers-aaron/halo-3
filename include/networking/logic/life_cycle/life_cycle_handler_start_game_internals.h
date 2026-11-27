#ifndef NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_HANDLER_START_GAME_INTERNALS_H
#define NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_HANDLER_START_GAME_INTERNALS_H

#include "networking/logic/life_cycle/peer_request_types.h"
#include "networking/session/network_session_parameters_base.h"
#include "networking/session/network_session_parameters_generic.h"

class c_peer_request_parameter : public c_network_session_parameter_base
{
public:
    bool set(const s_peer_request *value);
    __forceinline bool request_change(const s_peer_request *value)
    {
        bool result = false;
        if (set_allowed())
            result = set(value);
        else if (change_request_allowed()) {
            m_requested = *value;
            set_desires_change_request();
            result = true;
        }
        return result;
    }
    virtual void get_update_payload(long peer_index, void *payload) const;
    virtual long get_update_payload_size() const;
    virtual bool handle_update_payload(const void *payload);
    virtual void get_change_request_payload(long peer_index, void *payload) const;
    virtual long get_change_request_payload_size() const;
    virtual bool handle_change_request_payload(const void *payload);
    virtual void notify_reset_peer_state(long peer_index);
    virtual void notify_clear();
    virtual void notify_transition_state_to_become_host();
    using c_network_session_parameter_base::set_update_required;
    using c_network_session_parameter_base::m_flags;
    using c_network_session_parameter_base::notify_set_change_request;
    s_peer_request m_current;
    s_peer_request m_requested;
};

template <>
bool c_generic_network_session_parameter<unsigned __int64>::set(const unsigned __int64 *parameter);

#endif // NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_HANDLER_START_GAME_INTERNALS_H
