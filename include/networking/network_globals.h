#ifndef NETWORKING_NETWORK_GLOBALS_H
#define NETWORKING_NETWORK_GLOBALS_H

// Functions defined in tu/networkinglibxbox/network_globals.cpp.

class c_network_link;
extern "C" c_network_link *g_network_link;
class c_network_message_handler;
class c_network_message_type_collection;
struct s_transport_security_globals;

// Transport security storage owned by network_globals.obj at 0x827fc368.
extern "C" s_transport_security_globals g_transport_security_globals;

// The message registry pointer stored by network_globals.cpp at 0x828c4450.
extern "C" c_network_message_type_collection *g_network_message_type_collection;

void network_receive(void);
void network_send(void);
c_network_link *&network_globals_link_storage();
c_network_message_type_collection *&network_globals_message_types_storage();
c_network_message_handler *&network_globals_handler_storage();

class c_network_message_gateway;

void network_startup_transport(void *);
void network_shutdown_transport(void *);
extern "C" void network_update_transport(void);
extern "C" void network_update_online_users(void);
c_network_message_gateway *&network_globals_gateway_storage();
void network_initialize();
void network_dispose();

// Data defined in tu/networkinglibxbox/network_globals.cpp that other units use.

struct s_network_globals;

extern s_network_globals network_globals;

// Network storage anchored by address tags in network_globals.cpp.
struct c_network_storage_cache;
struct network_globals_guide_view;
#if defined(_XBOX) && !defined(__clang__)
struct s_network_send_state;
struct s_network_interfaces;
#endif

extern "C" {
extern "C++" c_network_storage_cache g_network_storage_cache;
extern network_globals_guide_view g_network_online_guide_state;
#if defined(_XBOX) && !defined(__clang__)
extern s_network_send_state g_network_send_state;
extern s_network_interfaces g_network_interfaces;
#endif
}

#endif // NETWORKING_NETWORK_GLOBALS_H
