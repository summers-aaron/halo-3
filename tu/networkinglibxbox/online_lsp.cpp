// networkinglibxbox/online_lsp.obj: from the records 0x96988 0x3a3070 0x3a3490 0x3a3530 0x3a3610 0x3a3668 0x3a36e8 0x3a3850 0x3a3a10 0x3a3c30
// @compiler xdk6130
// @flags /TP /O2
#include "cseries/crt_xdk.h"
#include "cseries/cseries.h"
#include "multithreading/synchronization_types.h"
#include "networking/network_configuration_types.h"
#include "math/random_math.h"
#include "math/random_math_range.h"
#include "memory/static_arrays.h"
#include <string.h>
#include "multithreading/synchronization.h"
#include "multithreading/critical_section_scope.h"
#include "networking/online/online_lsp_xenon.h"
#include "networking/online/online_lsp_search_task.h"
#include "networking/online/online_xenon.h"
#include "networking/online_lsp_server_acquire_result_types.h"
#include "networking/online/online_lsp.h"
#include "networking/online/xtitle_server_info_types.h"
#include "xbox/xnet_services.h"
// @address 0x3a3490 ?server_connected@c_online_lsp_manager@@QAAXJ@Z
// @address 0x3a3530 ?disconnect_from_server@c_online_lsp_manager@@QAAXJ_N@Z
// @address 0x3a3610 ?clear_activated_servers@c_online_lsp_manager@@AAAXXZ
// @address 0x3a3668 ?reset@c_online_lsp_manager@@AAAXXZ
// @address 0x3a36e8 ?get_best_server_index@c_online_lsp_manager@@AAAJW4e_online_lsp_service_type@@PAJ@Z
// @address 0x3a3850 ?service_type_in_list@c_online_lsp_manager@@CA_NPBV?$c_static_string@$03@@PBV?$c_static_string@$0MI@@@@Z
// @address 0x3a3c30 ?length@?$c_static_string@$0MI@@@QBAJXZ
// @address 0x96988 ?update@c_online_lsp_manager@@QAAXXZ
#include "networking/network_configuration.h"
namespace {
class c_online_lsp_critical_section_scope {
public:
    long m_critical_section;
    bool m_locked;
public:
    __forceinline c_online_lsp_critical_section_scope(long critical_section)
        : m_critical_section(critical_section), m_locked(false)
    {
        if (m_critical_section != -1)
            internal_critical_section_enter(m_critical_section);
        m_locked = true;
    }
    __forceinline ~c_online_lsp_critical_section_scope()
    {
        if (m_locked)
            internal_critical_section_leave(m_critical_section);
    }
};
class c_online_lsp_update_scope {
public:
    long m_critical_section;
public:
    __forceinline c_online_lsp_update_scope(long critical_section)
        : m_critical_section(critical_section)
    {
        internal_critical_section_enter(m_critical_section);
    }
    __forceinline ~c_online_lsp_update_scope()
    {
        // match: volatile pointer cast preserves memory access order
        _InterlockedDecrement(&g_synch_globals.critical_sections[28].lock_count.value);
        RtlLeaveCriticalSection(&g_synch_globals.critical_sections[28].critical_section);
    }
};
}
#include "networking/online/online_lsp_manager.h"
struct s_lsp_raw_server {
    c_static_string<16> address;
    unsigned short port;
    c_static_string<200> description;
};
// match: 0x709320: constructor is inlined into global initialization.
__forceinline c_online_lsp_manager::c_online_lsp_manager()
{
    m_next_connection_token = 0;
    memset(m_counts, 0, sizeof(m_counts));
    clear_activated_servers();
}
__forceinline bool c_online_lsp_manager::anyone_connecting(long type)
{
    bool result = false;
    for (long i = 0; i < 16; i++) {
        if (m_connections[i].type == type && m_connections[i].state == 1) {
            result = true;
            break;
        }
    }
    return result;
}
__forceinline bool c_online_lsp_manager::anyone_connected(long type)
{
    bool result = false;
    for (long i = 0; i < 16; i++) {
        if (m_connections[i].type == type && m_connections[i].state == 2) {
            result = true;
            break;
        }
    }
    return result;
}
__forceinline bool c_online_lsp_manager::has_had_recent_activity()
{
    bool result = false;
    unsigned long now = GetTickCount();
    long elapsed = now - m_timestamp;
    if (elapsed < g_network_configuration.online_lsp.idle_timeout)
        result = true;
    return result;
}
c_online_lsp_manager g_online_lsp_manager;
const s_lsp_raw_server k_additional_raw_upload_address_list[1] = { { "0.0.0.0", 0, "" } };
const c_static_string<4> g_server_descriptions[5] = { "ttl", "usr", "shr", "web", "dbg" };

void c_online_lsp_manager::server_connected(long token) {
    c_online_lsp_critical_section_scope lock(28);
    long slot_index = find_slot_index_from_token(token);
    if (slot_index != -1 && m_connections[slot_index].state == 1)
        m_connections[slot_index].state = 2;
}
void c_online_lsp_manager::disconnect_from_server(long token, bool silent) {
    c_online_lsp_critical_section_scope lock(28);
    long slot_index = find_slot_index_from_token(token);
    if (slot_index != -1) {
        if (!silent) {
            long entry_index = m_connections[slot_index].type;
            m_counts[m_activated_servers[entry_index].server_index]++;
        }
        clear_client(slot_index);
        m_timestamp = GetTickCount();
    }
}
void c_online_lsp_manager::clear_activated_servers() {
    for (long service_index = 0; service_index < 5; service_index++) {
        m_activated_servers[service_index].server_index = -1;
        m_activated_servers[service_index].address = 0;
        m_activated_servers[service_index].port = 0;
    }
    for (long client_index = 0; client_index < 16; client_index++) {
        clear_client(client_index);
    }
}
void c_online_lsp_manager::reset() {
    m_state = 0;
    m_substate = 0;
    memset(m_counts, 0, sizeof(m_counts));
    online_lsp_deactivate();
    clear_activated_servers();
    m_activated_server_count = 0;
}
void c_online_lsp_manager::update() {
    bool idle = true;
    bool service_available = online_lsp_service_available();
    c_online_lsp_update_scope lock(28);
    for (long type = 0; type < 5; type++) {
        idle &= !anyone_connecting(type);
        idle &= !anyone_connected(type);
    }
    bool currently_available = online_lsp_service_available();
    if (currently_available != m_available) {
        reset();
        m_available = currently_available;
    }
    if (idle && !has_had_recent_activity() && service_available && g_online_lsp_globals.desired_state == 2) {
        online_lsp_deactivate();
        clear_activated_servers();
    }
    ::online_lsp_update();
}
long c_online_lsp_manager::get_best_server_index(e_online_lsp_service_type service_type, long *out_score) {
    long best_index = -1;
    long best_score = g_network_configuration.online_lsp.minimum_score;
    long server_index;
    for (server_index = 0; server_index < m_state; server_index++) {
        char description[200];
        bool description_valid = false;
        XTITLE_SERVER_INFO *server = g_online_lsp_globals.search_task->get_found_server(server_index);
        description[0] = 0;
        if (server) {
            strncpy(description, server->description, 200);
            description[199] = 0;
            description_valid = true;
        }
        if (description_valid && service_type_in_list(&g_server_descriptions[(long)service_type], (const c_static_string<200> *)description) &&
            m_counts[server_index] < best_score) {
            best_index = server_index;
            best_score = m_counts[best_index];
        }
    }
    for (server_index = 0; server_index < m_substate; server_index++) {
        if (service_type_in_list(&g_server_descriptions[(long)service_type], &k_additional_raw_upload_address_list[server_index].description) &&
            m_counts[server_index + m_state] < best_score) {
            best_index = server_index + m_state;
            best_score = m_counts[best_index];
        }
    }
    if (out_score)
        *out_score = best_score;
    return best_index;
}
bool c_online_lsp_manager::service_type_in_list(const c_static_string<4> *service_type, const c_static_string<200> *service_list) {
    char remaining[208];
    char entry[200];
    const char *first_hit;
    const char *second_hit;
    bool done;
    long separator_offset;
    long rest_start;
    long rest_length;
    long copy_length;
    strncpy((char *)remaining, service_list->m_string, 200);
    remaining[199] = 0;
    entry[0] = 0;
    done = (service_list->length() == 0);
    while (!(unsigned char)done) {
        if (((const c_static_string<200> *)remaining)->length() <= 0)
            break;
        first_hit = strstr((char *)remaining, ",");
        if (first_hit == 0 || (long)(first_hit - remaining) == -1)
            break;
        separator_offset = -1;
        if (((const c_static_string<200> *)remaining)->length() > 0) {
            second_hit = strstr((char *)remaining, ",");
            if (second_hit != 0) {
                separator_offset = (long)(second_hit - remaining);
                if (separator_offset > 0 && separator_offset <= ((const c_static_string<200> *)remaining)->length()) {
                    copy_length = separator_offset + 1;
                    if (copy_length > 200)
                        copy_length = 200;
                    strncpy((char *)entry, (char *)remaining, copy_length);
                    entry[copy_length - 1] = 0;
                }
            }
        }
        done = (strncmp((char *)entry, service_type->m_string, 200) == 0) | done;
        strncpy((char *)entry, (char *)remaining, 200);
        entry[199] = 0;
        rest_length = ((const c_static_string<200> *)entry)->length() - separator_offset - 1;
        rest_start = separator_offset + 1;
        if (rest_start >= 0 && rest_length > 0 && rest_start + rest_length <= ((const c_static_string<200> *)entry)->length()) {
            copy_length = rest_length + 1;
            if (copy_length > 200)
                copy_length = 200;
            strncpy((char *)remaining, (char *)entry + rest_start, copy_length);
            remaining[copy_length - 1] = 0;
        }
    }
    return (strncmp((char *)remaining, service_type->m_string, 200) == 0) | done;
}
template<long k_maximum_count>
long c_static_string<k_maximum_count>::length() const {
    return csstrnlen(this->m_string, k_maximum_count);
}
template long c_static_string<200>::length() const;

static __forceinline bool online_lsp2_translate_server(XTITLE_SERVER_INFO *server, long *ip_address)
{
    bool result = false;
    if (online_local_xuid_is_online_enabled()) {
        long translated_address;
        if (XNetServerToInAddr(server->address, 0x4d530064, &translated_address) == 0) {
            result = true;
            *ip_address = translated_address;
        }
    }
    return result;
}
static __forceinline bool online_lsp2_activate_and_retrieve_server(long server_index, long *ip_address)
{
    bool result = false;
    XTITLE_SERVER_INFO *server = g_online_lsp_globals.search_task->get_found_server(server_index);
    if (g_online_lsp_globals.registered_servers[server_index].registered) {
        *ip_address = g_online_lsp_globals.registered_servers[server_index].ip_address;
        result = true;
    } else if (online_lsp2_translate_server(server, ip_address)) {
        g_online_lsp_globals.registered_servers[server_index].registered = true;
        g_online_lsp_globals.registered_servers[server_index].ip_address = *ip_address;
        result = true;
    }
    return result;
}
// @address 0x3a3a10 ?register_best_server@c_online_lsp_manager@@AAA_NW4e_online_lsp_service_type@@JPAJPAG@Z
bool c_online_lsp_manager::register_best_server(e_online_lsp_service_type service_type, long port, long *ip_address, unsigned short *out_port)
{
    bool result = false;
    if (anyone_connected(service_type)) {
        *ip_address = m_activated_servers[service_type].address;
        *out_port = m_activated_servers[service_type].port;
        result = true;
    } else {
        long best_score = g_network_configuration.online_lsp.minimum_score;
        long best_server_index = get_best_server_index(service_type, &best_score);
        if (best_server_index != -1) {
            if (best_server_index == m_activated_servers[service_type].server_index) {
                *ip_address = m_activated_servers[service_type].address;
                *out_port = m_activated_servers[service_type].port;
                result = true;
            } else if (best_server_index < m_state) {
                if (online_lsp2_activate_and_retrieve_server(best_server_index, ip_address)) {
                    *out_port = (unsigned short)port;
                    result = true;
                }
            } else {
                *ip_address = inet_addr(k_additional_raw_upload_address_list[best_server_index - m_state].address.get_string());
                *out_port = k_additional_raw_upload_address_list[best_server_index - m_state].port;
                result = true;
            }
        }
        if (result) {
            m_activated_servers[service_type].server_index = best_server_index;
            m_activated_servers[service_type].address = *ip_address;
            m_activated_servers[service_type].port = *out_port;
        }
    }
    return result;
}

static __forceinline unsigned short online_lsp3_random_port()
{
    s_online_lsp_configuration *configuration = &g_network_configuration.online_lsp;
    short lower = (short)configuration->port;
    long upper = configuration->port + configuration->port_count;
    return (unsigned short)random_range(get_local_random_seed_address(), (short)lower, (short)upper);
}
static __forceinline void online_lsp3_initialize_configuration()
{
    s_online_lsp_configuration empty;
    memset(&empty, 0, sizeof(empty));
    s_online_lsp_configuration *configuration = &g_network_configuration.online_lsp;
    if (memcmp(configuration, &empty, sizeof(empty)) == 0) {
        configuration->port = 1001;
        configuration->port_count = 2;
        configuration->minimum_score = 3;
        configuration->search_timeout = 300000;
        configuration->idle_timeout = 60000;
    }
}
// @address 0x3a3070 ?acquire_server@c_online_lsp_manager@@QAA?AW4e_online_lsp_server_acquire_result@@W4e_online_lsp_service_type@@PAJ1PAGPBD@Z
e_online_lsp_server_acquire_result c_online_lsp_manager::acquire_server(
    e_online_lsp_service_type service_type, long *out_connection_token, long *ip_address, unsigned short *port, const char *service)
{
    e_online_lsp_server_acquire_result result = acquire_failed;
    long slot_index = -1;
    for (long client_index = 0; client_index < 16; client_index++) {
        if (m_connections[client_index].state == 0) {
            slot_index = client_index;
            break;
        }
    }
    long random_port = online_lsp3_random_port();
    c_online_lsp_update_scope lock(28);
    online_lsp3_initialize_configuration();
    if (slot_index != -1) {
        if (!m_allow_raw_connections && anyone_connecting(service_type)) {
            result = acquire_pending;
        } else {
            bool currently_available = online_lsp_service_available();
            if (currently_available != m_available) {
                reset();
                m_available = currently_available;
            }
            if (online_lsp_service_available()) {
                long state = g_online_lsp_globals.desired_state;
                bool connected = false;
                for (long type = 0; type < 5; type++)
                    connected |= anyone_connected(type);
                if (!connected && state == 2) {
                    bool fresh = false;
                    long elapsed = GetTickCount() - m_activated_server_count;
                    if (elapsed < g_network_configuration.online_lsp.search_timeout)
                        fresh = true;
                    if (!fresh) {
                        reset();
                        state = 0;
                    }
                }
                switch (state) {
                case 0:
                    g_online_lsp_globals.desired_state = 1;
                    memset(m_counts, 0, sizeof(m_counts));
                    m_activated_server_count = GetTickCount();
                    result = acquire_pending;
                    break;
                case 1:
                    result = acquire_pending;
                    break;
                case 3:
                    online_lsp_deactivate();
                    clear_activated_servers();
                    m_state = 0;
                    if (m_substate > 0 && register_best_server(service_type, random_port, ip_address, port))
                        result = acquire_success;
                    break;
                case 2:
                    m_state = g_online_lsp_globals.search_task->get_found_server_count();
                    m_substate = 0;
                    if (register_best_server(service_type, random_port, ip_address, port)) {
                        result = acquire_success;
                    } else {
                        online_lsp_deactivate();
                        clear_activated_servers();
                    }
                    break;
                default:
                    // match: 0x3a3070: unreachable hint omits dead branch check
                    __assume(0);
                }
            } else if (register_best_server(service_type, random_port, ip_address, port)) {
                result = acquire_success;
            }
        }
    }
    if (result == acquire_success) {
        m_connections[slot_index].state = 1;
        m_connections[slot_index].type = service_type;
        m_connections[slot_index].server_index = m_next_connection_token;
        m_next_connection_token++;
        m_connections[slot_index].service.set(service);
        *out_connection_token = m_connections[slot_index].server_index;
    }
    m_timestamp = GetTickCount();
    return result;
}
