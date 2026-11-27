#ifndef NETWORKING_LOGIC_NETWORK_BROADCAST_SEARCH_H
#define NETWORKING_LOGIC_NETWORK_BROADCAST_SEARCH_H

#include "shell/shell.h"

// Functions defined in tu/networkinglibxbox/network_broadcast_search.cpp that other units call.

struct s_network_message_broadcast_reply;
struct transport_address;

void network_broadcast_search_dispose();
void network_broadcast_search_end();
void network_broadcast_search_update();
void network_broadcast_search_handle_reply(const transport_address *from, const s_network_message_broadcast_reply *message);
bool network_banhammer_controller_idle_for_too_long(e_controller_index controller_index);

#endif // NETWORKING_LOGIC_NETWORK_BROADCAST_SEARCH_H
