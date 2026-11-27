// networkinglibxbox/transport.obj: from the records 0x3a7360 0x3a7440 0x3a74a0 0x3a74f8 0x3a7710
// @flags /TP /O2 /Oc
// @address 0x3a74a0 ?transport_network_available@@YA_NXZ
// @address 0x74b80c g_runtime_state_allocation
// @address 0x6bd1d8 XOnlineStartup

#include <xtl.h>
#include "memory/runtime_state_allocation.h"
#include "networking/session/session_network_message_types.h"
#include <string.h>
#include "networking/transport/transport.h"
#include "networking/transport/transport_qos.h"
#include "networking/transport/transport_qos_globals.h"
#include "networking/transport/transport_security.h"
#include "memory/data.h"
#include "memory/normal_allocation.h"
#include "networking/network_globals.h"
#include "networking/transport/transport_security_globals.h"


#include "networking/logic/network_session_interface.h"

#if defined(_XBOX) && !defined(__clang__)
#include "networking/transport/transport_types.h"
// match: 0x3a74f8: internal linkage keeps transport_startup in the r29 base / r26..r28 flag register order
static s_transport_globals transport_startup_state;
#else
#include "networking/transport/transport_globals.h"
s_transport_globals transport_startup_state;
#endif
#if defined(_XBOX) && !defined(__clang__)
static s_transport_qos_globals transport_qos_globals;
#else
s_transport_qos_globals transport_qos_globals;
#endif

extern "C" {
// @address 0x119a484 g_main_last_ethernet_link_status
unsigned long g_main_last_ethernet_link_status;
// @address 0x119a48c g_transport_qos_data_storage
s_data_array *g_transport_qos_data_storage;
}

void transport_register_transition_functions(void (*startup)(void *), void (*shutdown)(void *), void (*reset)(void *), void *context)
{
    transport_startup_state.startup_functions[transport_startup_state.transition_function_count] = startup;
    transport_startup_state.shutdown_functions[transport_startup_state.transition_function_count] = shutdown;
    transport_startup_state.reset_functions[transport_startup_state.transition_function_count] = reset;
    transport_startup_state.callback_datas[transport_startup_state.transition_function_count] = context;
    ++transport_startup_state.transition_function_count;
}

void transport_reset()
{
    for (long index = 0; index < transport_startup_state.transition_function_count; ++index)
    {
        void (*reset)(void *) = transport_startup_state.reset_functions[index];
        if (reset)
            reset(transport_startup_state.callback_datas[index]);
    }
}

static __forceinline void transport_initialize_qos(void)
{
    transport_qos_globals.initialized_storage = 0;
    transport_qos_globals.data = 0;
    s_data_array *data = g_transport_qos_data_storage;
    const char *name = "transport qos attempts";
    c_allocation_base *allocator = &g_runtime_state_allocation;
    if (!data) {
        data = (s_data_array *)allocator->allocate(data_allocation_size(32, sizeof(s_transport_qos_attempt), 0), name);
        g_transport_qos_data_storage = data;
    }
    memset(data, 0, data_allocation_size(32, sizeof(s_transport_qos_attempt), 0));
    transport_qos_globals.data = data;
    data_initialize(data, name, 32, sizeof(s_transport_qos_attempt), 0, allocator);
    transport_qos_globals.data->flags |= 4;
}

// @address 0x3a7360 ?transport_initialize@@YAXXZ
void transport_initialize(void)
{
    memset(&transport_startup_state, 0, sizeof(transport_startup_state));
    transport_security_initialize();
    transport_initialize_qos();
    transport_startup_state.initialized = true;
    if (transport_network_available()) {
        transport_startup();
    }
}

// @address 0x3a7440 ?transport_dispose@@YAXXZ
void transport_dispose(void)
{
    if (transport_startup_state.initialized) {
        transport_shutdown();
        transport_qos_shutdown();
        if (transport_qos_globals.data) {
            transport_qos_globals.data = 0;
        }
        transport_startup_state.transition_function_count = 0;
        transport_startup_state.initialized = false;
    }
}

// 0x3a74a0
bool transport_network_available(void)
{
    bool available = transport_startup_state.initialized;
    unsigned long link_status = XNetGetEthernetLinkStatus();
    if (g_main_last_ethernet_link_status != link_status) {
        g_main_last_ethernet_link_status = link_status;
    }
    if (!(link_status & XNET_ETHERNET_LINK_ACTIVE)) {
        available = false;
    }
    return available;
}

// 0x3a7710
// @address 0x3a7710 ?transport_shutdown@@YAXXZ
void transport_shutdown(void)
{
    s_transport_globals *globals = &transport_startup_state;
    if (globals->started != 0) {
        network_session_interface_handle_message(_session_network_message_transport_shutdown);
        int count = globals->transition_function_count;
        int index = 0;
        if (count > 0) {
            do {
                transport_transition_function_t callback = globals->shutdown_functions[index];
                if (callback != 0) {
                    callback(globals->callback_datas[index]);
                    count = globals->transition_function_count;
                }
                ++index;
            } while (index < count);
        }
        transport_qos_shutdown();
        globals->started = 0;
        XOnlineCleanup();
        WSACleanup();
        XNetCleanup();
    }
}





static __forceinline void transport_startup_security_init(void)
{
    XNADDR xnaddr;
    memset(&xnaddr, 0, sizeof(xnaddr));
    XNetGetTitleXnAddr(&xnaddr);
    for (int identifier_index = 0; identifier_index < 6; identifier_index++) {
        g_transport_security_globals.local_identifier[identifier_index] = xnaddr.abEnet[identifier_index];
    }
    transport_secure_address_resolve();
}

static __forceinline void transport_startup_qos_init(void)
{
    if (!transport_qos_globals.initialized) {
        transport_qos_globals.data->valid = true;
        data_delete_all(transport_qos_globals.data);
        transport_qos_globals.initialized = true;
    }
}

// 0x3a74f8
// @address 0x3a74f8 ?transport_startup@@YAXXZ
void transport_startup(void)
{
    if (!transport_startup_state.started) {
        bool success = true;
        bool winsock_started = false;
        bool xnet_started = false;
        bool online_started = false;
        WSADATA wsadata = {0};
        if (success) {
            XNetStartupParams params = {0};
            params.cfgSizeOfStruct = sizeof(XNetStartupParams);
            params.cfgFlags = 0;
            params.cfgSockMaxDgramSockets = 16;
            params.cfgSockMaxStreamSockets = 24;
            params.cfgSockDefaultRecvBufsizeInK = 8;
            params.cfgSockDefaultSendBufsizeInK = 8;
            params.cfgKeyRegMax = 10;
            params.cfgSecRegMax = 40;
            params.cfgQosDataLimitDiv4 = 255;
            params.cfgQosSrvMaxSimultaneousResponses = 32;
            XNetGetEthernetLinkStatus();
            DWORD error = XNetStartup(&params);
            if (error == 0) {
                xnet_started = true;
            } else {
                success = false;
            }
        }
        if (success) {
            DWORD error = WSAStartup(2, &wsadata);
            if (error == 0) {
                winsock_started = true;
            } else {
                success = false;
            }
        }
        if (success) {
            if (XOnlineStartup() == 0) {
                online_started = true;
            } else {
                success = false;
            }
        }
        if (success) {
            transport_startup_state.started = true;
            transport_startup_security_init();
            transport_startup_qos_init();
            for (long callback_index = 0; callback_index < transport_startup_state.transition_function_count; callback_index++) {
                void (*callback)(void *) = transport_startup_state.startup_functions[callback_index];
                if (callback) {
                    callback(transport_startup_state.callback_datas[callback_index]);
                }
            }
        } else {
            if (online_started) {
                XOnlineCleanup();
            }
            if (winsock_started) {
                WSACleanup();
            }
            if (xnet_started) {
                XNetCleanup();
            }
        }
    }
}
