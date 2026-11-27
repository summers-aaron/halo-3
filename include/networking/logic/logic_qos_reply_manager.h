#ifndef NETWORKING_LOGIC_LOGIC_QOS_REPLY_MANAGER_H
#define NETWORKING_LOGIC_LOGIC_QOS_REPLY_MANAGER_H

#include "networking/logic/network_search_types.h"
#include "networking/online/online_guide_xenon.h"
#include "networking/transport/transport_qos_types.h"

struct s_session_qos_status : s_network_squad_status_data
{
};

// match: 0x709c18: novtable drops the dead abstract-base vfptr store across the status ctor
class __declspec(novtable) c_session_qos_reply_manager
{
public:
    void update();
protected:
    virtual bool get_session_id(s_transport_secure_identifier *session_id) = 0;
    virtual bool desires_qos_reply_block_set() = 0;
    virtual bool get_qos_status(s_network_squad_status_data *status) = 0;
    virtual long get_qos_reply_bandwidth_limit() = 0;
    virtual void notify_qos_reply_block_set() {}

public:
    bool initialized;
    unsigned long last_update;
    s_session_qos_status status;
};

class c_squad_session_qos_reply_manager : public c_session_qos_reply_manager
{
protected:
    virtual bool get_session_id(s_transport_secure_identifier *session_id);
    virtual bool desires_qos_reply_block_set();
    virtual bool get_qos_status(s_network_squad_status_data *status);
    virtual long get_qos_reply_bandwidth_limit();
};

class c_group_session_qos_reply_manager : public c_session_qos_reply_manager
{
protected:
    virtual bool get_session_id(s_transport_secure_identifier *session_id);
    virtual bool desires_qos_reply_block_set();
    virtual bool get_qos_status(s_network_squad_status_data *status);
    virtual long get_qos_reply_bandwidth_limit();
    virtual void notify_qos_reply_block_set();
};


struct s_live_service_qos_flags
{
    unsigned char bits;
    bool test(long bit) const
    {
        return (bits & (1 << bit)) != 0;
    }
    void set(long bit, bool enabled)
    {
        if (enabled)
            bits = (unsigned char)(bits | (1 << bit));
        else
            bits = (unsigned char)(bits & ~(1 << bit));
    }
};
class c_live_service_qos_manager
{
public:
    void update();
    s_transport_qos_result result;
    s_live_service_qos_flags flags;
    long qos_index;
    unsigned long last_attempt;
};

struct s_qos_reply_manager_globals
{
    bool initialized;
    c_live_service_qos_manager live_service_qos;
    c_squad_session_qos_reply_manager squad_session_qos_reply;
    c_group_session_qos_reply_manager group_session_qos_reply;
};

extern s_qos_reply_manager_globals g_qos_reply_manager_globals;

#endif
