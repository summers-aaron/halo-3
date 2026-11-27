#ifndef NETWORKING_SESSION_NETWORK_SESSION_CLASS_H
#define NETWORKING_SESSION_NETWORK_SESSION_CLASS_H

// Leaf header: the class of a network session (c_network_session::session_class(), the managed session's
// session_class, managed_session_create_host). Kept apart from network_session_types.h because
// life_cycle_handler_post_match.cpp, which includes network_session.h and network_session_parameters.h, still
// defines its own copy; fold into network_session.h with the other session enums.
// March: the class name table at 0x83ce02b8 ("offline", "system-link", "xbox-live") and asserts naming
// _network_session_class_offline, _network_session_class_xbox_live and k_network_session_class_count.
enum e_network_session_class
{
    _network_session_class_none = -1,                   // no March name; c_network_session stores it when it resets (network_session.cpp)
    _network_session_class_offline = 0,
    _network_session_class_system_link,
    _network_session_class_xbox_live,

    k_network_session_class_count
};

#endif // NETWORKING_SESSION_NETWORK_SESSION_CLASS_H
