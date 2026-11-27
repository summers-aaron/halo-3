#ifndef NETWORKING_ONLINE_ONLINE_LSP_XENON_H
#define NETWORKING_ONLINE_ONLINE_LSP_XENON_H

#include <stddef.h>

class c_lsp_search_task;

struct s_online_lsp_registered_server
{
    bool registered;
    long ip_address;
};

struct s_online_lsp_globals
{
    c_lsp_search_task *search_task;
    long current_state;
    long desired_state;
    s_online_lsp_registered_server registered_servers[16];
};

typedef char online_lsp_registered_server_size[(sizeof(s_online_lsp_registered_server) == 8) ? 1 : -1];
typedef char online_lsp_globals_size[(sizeof(void *) != 4 || ((sizeof(s_online_lsp_globals) == 0x8c) )) ? 1 : -1];
typedef char online_lsp_servers_offset[(sizeof(void *) != 4 || ((offsetof(s_online_lsp_globals, registered_servers) == 0xc) )) ? 1 : -1];

extern s_online_lsp_globals g_online_lsp_globals;

// Functions defined in tu/networkinglibxbox/online_lsp_xenon.cpp.

void reset_immediate();

inline void online_lsp_deactivate()
{
    g_online_lsp_globals.desired_state = 0;
    if (g_online_lsp_globals.current_state == 2 || g_online_lsp_globals.current_state == 3)
        reset_immediate();
}
void online_lsp_update(void);
bool online_lsp_service_available();
void unregister_all_servers();

#endif // NETWORKING_ONLINE_ONLINE_LSP_XENON_H
