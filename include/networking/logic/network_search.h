#ifndef NETWORKING_LOGIC_NETWORK_SEARCH_H
#define NETWORKING_LOGIC_NETWORK_SEARCH_H

// Functions defined in tu/networkinglibxbox/network_search.cpp that other units call.

struct s_available_session;

bool network_search_initialize();
s_available_session *network_search_session(long index);
void network_search_end();
extern "C" bool network_search_begin(long controller);

#endif // NETWORKING_LOGIC_NETWORK_SEARCH_H
