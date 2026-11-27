#ifndef INCLUDE_PHYS_BSD_SOCKET_H
#define INCLUDE_PHYS_BSD_SOCKET_H

extern "C" {
    extern signed char g_phys_bsd_network_initialized;
}

// Functions defined in tu/hkbase/hkBsdSocket.cpp that other units call.

void phys_bsd_network_quit();
void phys_bsd_network_init(void);

#endif // HAVOK_HK_BSD_SOCKET_H
