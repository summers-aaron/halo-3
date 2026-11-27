#ifndef NETWORKING_SESSION_NETWORK_SESSION_PARAMETERS_GENERIC_H
#define NETWORKING_SESSION_NETWORK_SESSION_PARAMETERS_GENERIC_H

// The parameter templates: a parameter holding one value of t_type (c_generic_network_session_parameter_data)
// and its plain get/set form (c_generic_network_session_parameter). The header March 2007's asserts name
// (networking\session\network_session_parameters_generic.h:68..235).
//
// c_generic_network_session_parameter_data<t_type>: the base (0x2c bytes), then the value and the requested
// value, each sizeof(t_type), at +0x2c (+0x30 for an 8-aligned t_type).
//
// get() is defined inline below and returns null when the value is unavailable. The virtual overrides and
// set/request_change are only declared here; each instantiation defines set/request_change out of line
// (placed in the
// units that define them, e.g. c_generic_network_session_parameter<bool> in network_life_cycle.cpp), so they are
// prototypes here, as are the virtuals (defined by the owner unit network_session_parameters.cpp).

#include "networking/session/network_session_parameters_base.h"

template<typename t_type>
class c_generic_network_session_parameter_data : public c_network_session_parameter_base
{
public:
    c_generic_network_session_parameter_data();

protected:
    virtual void notify_reset_peer_state(long peer_index);
    virtual void notify_clear();
    virtual void notify_transition_state_to_become_host();
private:
    virtual void get_update_payload(long peer_index, void *payload) const;
    virtual long get_update_payload_size() const;
    virtual bool handle_update_payload(const void *payload);
    virtual void get_change_request_payload(long peer_index, void *payload) const;
    virtual long get_change_request_payload_size() const;
    virtual bool handle_change_request_payload(const void *payload);

protected:
    void clear_internal();

public:
    // Canonical storage is also read by session-interface parameter views.
    t_type m_data;                                  // (0x30 when t_type is 8-aligned)
    t_type m_requested_data;
};

template<typename t_type>
class c_generic_network_session_parameter : public c_generic_network_session_parameter_data<t_type>
{
public:
    c_generic_network_session_parameter();

    using c_network_session_parameter_base::m_flags;
    using c_network_session_parameter_base::m_session;
    using c_network_session_parameter_base::set_desires_change_request;
    using c_network_session_parameter_base::notify_set_change_request;

    const t_type *get() const
    {
        const t_type *result = 0;
        if (this->get_allowed())
            result = &this->m_data;
        return result;
    }
    bool set(const t_type *parameter);
    bool request_change(const t_type *parameter);
};

enum e_campaign_difficulty_level;
template <>
bool c_generic_network_session_parameter<e_campaign_difficulty_level>::set(const e_campaign_difficulty_level *parameter);

// Set the parameter when allowed, else record the value as a change request.
template<typename t_type>
static __forceinline bool parameter_request_change(c_generic_network_session_parameter<t_type> *parameter, const long *value)
{
    long requested = *value;
    bool result = false;
    if (parameter->set_allowed())
        result = ((c_generic_network_session_parameter<e_campaign_difficulty_level> *)parameter)->set(
            (const e_campaign_difficulty_level *)value);
    else if (parameter->change_request_allowed()) {
        parameter->m_requested_data = (t_type)requested;
        parameter->m_flags |= 2;
        parameter->notify_set_change_request();
        result = true;
    }
    return result;
}

#endif // NETWORKING_SESSION_NETWORK_SESSION_PARAMETERS_GENERIC_H
