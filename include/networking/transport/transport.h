#ifndef NETWORKING_TRANSPORT_TRANSPORT_H
#define NETWORKING_TRANSPORT_TRANSPORT_H

// Functions defined in tu/networkinglibxbox/transport.cpp that other units call.

struct s_data_array;

void transport_register_transition_functions(void (*startup)(void *), void (*shutdown)(void *), void (*reset)(void *), void *context);
void transport_reset();
void transport_initialize(void);
void transport_dispose(void);
bool transport_network_available(void);
void transport_shutdown(void);
void transport_startup(void);

// Data transport.cpp keeps: the storage the transport qos attempt array is allocated into.
// (The last ethernet link status is declared in main/main_global_declarations.h.)
extern "C" {
extern s_data_array *g_transport_qos_data_storage;
}

#endif // NETWORKING_TRANSPORT_TRANSPORT_H
