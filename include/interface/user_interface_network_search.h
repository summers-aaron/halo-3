#ifndef INTERFACE_USER_INTERFACE_NETWORK_SEARCH_H
#define INTERFACE_USER_INTERFACE_NETWORK_SEARCH_H

// Functions defined in tu/interfacelibxbox/user_interface_network_search.cpp that other units call.

struct s_network_squad_status_data;

long user_interface_available_squads_iterate(long index);
const s_network_squad_status_data *user_interface_available_squad_get_status(long index);

#endif // INTERFACE_USER_INTERFACE_NETWORK_SEARCH_H
