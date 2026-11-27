#ifndef NETWORKING_MESSAGES_NETWORK_MESSAGE_HANDLER_H
#define NETWORKING_MESSAGES_NETWORK_MESSAGE_HANDLER_H

#include "networking/messages/network_message_type_collection.h"
#include "networking/transport/transport_address.h"

class c_network_channel;
class c_network_link;
class c_network_message_gateway;
class c_network_observer;
class c_network_session_manager;
struct s_network_message_join_abort;
struct s_network_message_join_refuse;
struct s_network_message_leave_session;
struct s_network_message_election;
struct s_network_message_parameters_request;
struct s_network_message_broadcast_search;

class c_network_message_handler
{
public:
    c_network_message_handler() : m_initialized(false) {}
    void handle_out_of_band_message(const transport_address *address, e_network_message_type message_type, long message_size, const void *message);
    void handle_channel_message(c_network_channel *channel, e_network_message_type message_type, long message_size, const void *message);

private:
    void handle_join_abort(const transport_address *address, const s_network_message_join_abort *message);
    void handle_join_refuse(const transport_address *address, const s_network_message_join_refuse *message);
    void handle_leave_session(const transport_address *address, const s_network_message_leave_session *message);
    void handle_election(const transport_address *address, const s_network_message_election *message);
    void handle_parameters_request(c_network_channel *channel, const s_network_message_parameters_request *message);
    void handle_broadcast_search(const transport_address *address, const s_network_message_broadcast_search *message);

public: // Initialization and disposal also access this state in network_globals.cpp.
    // March initialize_handler: initialized at 0, link at 4, message types at 8.
    bool m_initialized;
    c_network_link *m_link;
    const c_network_message_type_collection *m_message_types;
    c_network_message_gateway *m_message_gateway;
    c_network_observer *m_observer;
    c_network_session_manager *m_session_manager;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char network_message_handler_size[(sizeof(void *) != 4 || ((sizeof(c_network_message_handler) == 0x18))) ? 1 : -1];
#endif

#endif // NETWORKING_MESSAGES_NETWORK_MESSAGE_HANDLER_H
