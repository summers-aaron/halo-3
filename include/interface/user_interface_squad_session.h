#ifndef INTERFACE_USER_INTERFACE_SQUAD_SESSION_H
#define INTERFACE_USER_INTERFACE_SQUAD_SESSION_H

struct s_network_session;
struct s_network_squad_status_data;

extern "C" void user_interface_leave_squad(unsigned char leave_reason);
extern "C" bool user_interface_squad_try_and_get_session(s_network_session **session);
extern "C" bool user_interface_remote_squad_is_joinable(const s_network_squad_status_data *squad);

#endif // INTERFACE_USER_INTERFACE_SQUAD_SESSION_H
