// networkinglibxbox/online_lsp_xenon.obj: from the records 0x3ed9d0 0x3ed9e0 0x3eda68 0x3edb80 0x3edbe0 0x3edc18 0x3edc90 0x96bb8
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "cseries/crt_xdk.h"
#include "math/random_math.h"
#include "memory/static_arrays.h"
#include <string.h>
#include "cseries/async_xoverlapped.h"
#include "cseries/async_xoverlapped_memory.h"
#include "networking/online/online_lsp_xenon.h"
#include "networking/online/online_lsp_search_task.h"
#include "networking/online/xtitle_server_info_types.h"
#include "networking/tools/http_client.h"
#include "xbox/xnet_services.h"
#include "xbox/online_services.h"

// @address 0x80a300 ?g_online_lsp_globals@@3Us_online_lsp_globals@@A
s_online_lsp_globals g_online_lsp_globals;

namespace {

static __forceinline void shuffle_servers(XTITLE_SERVER_INFO *servers, unsigned long count) {
    for (unsigned long index = 0; index < count - 1; ++index) {
        long swap_index = random_range((short)index, (short)count);
        XTITLE_SERVER_INFO temporary = servers[index];
        servers[index] = servers[swap_index];
        servers[swap_index] = temporary;
    }
}
}
const char *c_lsp_search_task::get_context_string() const {
    return "lsp search";
}
unsigned long c_lsp_search_task::start(void *overlapped) {
    unsigned long buffer_size = 0;
    unsigned long error_status = XTitleServerCreateEnumerator(0, 16, &buffer_size, &m_enumeration_handle);
    if (error_status == 0) {
        m_overlapped_results_buffer = (XTITLE_SERVER_INFO *)::overlapped_malloc_tracked(buffer_size, (char const *)(0), 0);
        if (m_overlapped_results_buffer != 0) {
            error_status = XEnumerate(m_enumeration_handle, m_overlapped_results_buffer, buffer_size, 0, overlapped);
        } else {
            error_status = 1223;
        }
    }
    return error_status;
}
void c_lsp_search_task::success(unsigned long count) {
    m_server_address_count = count;
    if (m_server_address_count > 0) {
        memcpy(m_server_addresses, m_overlapped_results_buffer, m_server_address_count * sizeof(XTITLE_SERVER_INFO));
        for (unsigned long index = 0; index < m_server_address_count; ++index) {
            c_static_string<200> description(m_server_addresses[index].description);
        }
        shuffle_servers(m_server_addresses, m_server_address_count);
    }
}
void c_lsp_search_task::complete() {
    if (m_enumeration_handle != 0 && m_enumeration_handle != (void *)-1) {
        CloseHandle(m_enumeration_handle);
        m_enumeration_handle = (void *)-1;
    }
    if (m_overlapped_results_buffer != 0) {
        overlapped_free(m_overlapped_results_buffer);
        m_overlapped_results_buffer = 0;
    }
}
void c_lsp_search_task::reset() {
    c_overlapped_task::reset();
    m_server_address_count = 0;
}

// @address 0x3edc18 ?reset_immediate@@YAXXZ
void reset_immediate()
{
    unregister_all_servers();
    if (g_online_lsp_globals.search_task->busy())
        overlapped_task_terminate(g_online_lsp_globals.search_task);
    g_online_lsp_globals.current_state = 0;
}

void unregister_all_servers()
{
    for (unsigned long index = 0; index < 16; ++index) {
        if (g_online_lsp_globals.registered_servers[index].registered) {
            long ip_address = g_online_lsp_globals.registered_servers[index].ip_address;
            unsigned long error_status = XNetUnregisterInAddr(g_online_lsp_globals.registered_servers[index].ip_address);
            if (error_status != 0) {
                c_static_string<16> ip_address_string;
                c_http_client::get_ip_address_string(ip_address, &ip_address_string);
            }
            g_online_lsp_globals.registered_servers[index].registered = false;
            g_online_lsp_globals.registered_servers[index].ip_address = 0;
        }
    }
}

// @address 0x96bb8 ?online_lsp_update@@YAXXZ
void online_lsp_update(void)
{
    if (!online_lsp_service_available())
        g_online_lsp_globals.desired_state = 0;
    if (g_online_lsp_globals.current_state == 0)
    {
        if (g_online_lsp_globals.desired_state == 1 && !g_online_lsp_globals.search_task->busy())
        {
            unregister_all_servers();
            g_online_lsp_globals.search_task->reset();
            if (overlapped_task_start_internal(g_online_lsp_globals.search_task, "..\\networking\\online\\online_lsp_xenon.cpp", 261))
                g_online_lsp_globals.current_state = 1;
        }
    }
    else if (online_lsp_service_available())
    {
        if (g_online_lsp_globals.desired_state == 0)
        {
            if (!g_online_lsp_globals.search_task->busy())
            {
                if (g_online_lsp_globals.current_state == 1)
                    g_online_lsp_globals.current_state = 0;
                reset_immediate();
            }
        }
        else
        {
            switch (g_online_lsp_globals.search_task->get_task_state())
            {
            case 0:
                break;
            case 1:
            case 2:
            case 3:
                g_online_lsp_globals.current_state = 1;
                g_online_lsp_globals.desired_state = 1;
                break;
            case 5:
                g_online_lsp_globals.current_state = 3;
                g_online_lsp_globals.desired_state = 3;
                break;
            case 4:
                if (g_online_lsp_globals.search_task->get_found_server_count() == 0)
                {
                    g_online_lsp_globals.current_state = 3;
                    g_online_lsp_globals.desired_state = 3;
                }
                else
                {
                    g_online_lsp_globals.current_state = 2;
                    g_online_lsp_globals.desired_state = 2;
                }
                break;
            default:
                // match: 0x96bb8: unreachable hint omits dead branch check
                __assume(0);
            }
        }
    }
}
