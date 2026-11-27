#ifndef NETWORKING_NETWORK_TIME_H
#define NETWORKING_NETWORK_TIME_H

// Functions defined in tu/networkinglibxbox/network_time.cpp that other units call.

struct s_transport_secure_identifier;
class c_simulation_view;

long network_time_since(c_simulation_view *view);
unsigned long network_session_time_get_time(const s_transport_secure_identifier *transport_secure_identifier);

#endif // NETWORKING_NETWORK_TIME_H
