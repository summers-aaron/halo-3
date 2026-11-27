#ifndef NETWORKING_ONLINE_ONLINE_LSP_MANAGER_H
#define NETWORKING_ONLINE_ONLINE_LSP_MANAGER_H

#include "memory/static_arrays.h"
#include "multithreading/synchronization.h"
#include "multithreading/critical_section_scope.h"
#include "networking/online_lsp_server_acquire_result_types.h"
#include "networking/online/online_lsp.h"

#include "networking/online/online_lsp_service_type.h"

// March networking\online\online_lsp.cpp (asserts at lines 145..609). The public and private
// out-of-line methods are declared below; the layout is the one the manager implementation
// defines. March's other inline helpers (anyone_connecting, anyone_connected, has_had_recent_activity,
// search_results_still_fresh, find_empty_slot_index, get_recommended_retry_count, go_into_crash_mode)
// join the class with their owner unit.
class c_online_lsp_manager
{
public:
    c_online_lsp_manager();
    static c_online_lsp_manager *get();
    void update();
    __forceinline void go_into_crash_mode()
    {
        internal_critical_section_enter(28);
        reset();
        m_allow_raw_connections = true;
        internal_critical_section_leave(28);
    }
    e_online_lsp_server_acquire_result acquire_server(e_online_lsp_service_type service_type,
        long *out_connection_token, long *ip_address, unsigned short *port, const char *service);
    void server_connected(long token);
    void disconnect_from_server(long token, bool silent);
private:
    struct s_activated_server
    {
        long server_index;
        unsigned long address;
        unsigned short port;
    };
    struct s_lsp_connection
    {
        long state;
        long type;
        long server_index;
        c_static_string<48> service;
    };
    __forceinline void clear_client(long client_index)
    {
        m_connections[client_index].state = 0;
        m_connections[client_index].type = -1;
        m_connections[client_index].server_index = -1;
        m_connections[client_index].service.clear();
    }
    __forceinline long find_slot_index_from_token(long token) const
    {
        long slot_index = -1;
        for (long client_index = 0; client_index < 16; client_index++)
        {
            if (m_connections[client_index].state != 0 && m_connections[client_index].server_index == token)
            {
                slot_index = client_index;
                break;
            }
        }
        return slot_index;
    }
    bool anyone_connecting(long type);
    bool anyone_connected(long type);
    bool has_had_recent_activity();
    void clear_activated_servers();
    void reset();
    bool register_best_server(e_online_lsp_service_type service_type, long port, long *ip_address, unsigned short *out_port);
    long get_best_server_index(e_online_lsp_service_type service_type, long *out_score);
    static bool service_type_in_list(const c_static_string<4> *service_type, const c_static_string<200> *service_list);

    bool m_available;
    bool m_allow_raw_connections;
    s_activated_server m_activated_servers[5];
    long m_activated_server_count;
    unsigned long m_timestamp;
    long m_state;
    long m_substate;
    long m_counts[20];
    s_lsp_connection m_connections[16];
    long m_next_connection_token;
};

extern c_online_lsp_manager g_online_lsp_manager;

// March: static c_online_lsp_manager::get() returns the single manager.
__forceinline c_online_lsp_manager *c_online_lsp_manager::get()
{
    return &g_online_lsp_manager;
}

#endif // NETWORKING_ONLINE_ONLINE_LSP_MANAGER_H
