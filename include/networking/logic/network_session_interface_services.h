#ifndef NETWORKING_LOGIC_NETWORK_SESSION_INTERFACE_SERVICES_H
#define NETWORKING_LOGIC_NETWORK_SESSION_INTERFACE_SERVICES_H

extern "C++" {
struct s_network_session;
class c_network_session;
enum e_squad_join_permission;

extern "C" long network_session_get_maximum_player_count(const s_network_session *session);
extern "C" bool network_life_cycle_in_session(s_network_session **session);
extern "C" void network_life_cycle_handle_lost_connection(void);
extern "C" bool network_life_cycle_try_and_get_squad_session(c_network_session **session);
bool network_life_cycle_in_group_session(c_network_session **session);
extern "C" void __stdcall XLaunchNewImage(const char *, unsigned long);
e_squad_join_permission user_interface_squad_get_join_permission(void);
}

#endif // NETWORKING_LOGIC_NETWORK_SESSION_INTERFACE_SERVICES_H
