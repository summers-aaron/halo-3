#ifndef NETWORKING_TRANSPORT_TRANSPORT_QOS_H
#define NETWORKING_TRANSPORT_TRANSPORT_QOS_H

#include "networking/transport/transport_qos_types.h"

// Functions defined in tu/networkinglibxbox/transport_qos.cpp that other units call.

struct s_transport_qos_result;
struct s_transport_secure_identifier;

void transport_qos_shutdown(void);
void transport_qos_delete(long qos_index);
bool transport_qos_is_complete(long attempt_index);
bool transport_qos_get_result(long attempt_index, long target_index, s_transport_qos_result *out_result);
bool transport_qos_listener_initiate(const s_transport_secure_identifier *identifier);

struct s_transport_qos_target;

long transport_qos_target_new(e_transport_qos_type qos_type, long target_count, const s_transport_qos_target *targets, long bandwidth);
e_transport_qos_target_status transport_qos_get_target_status(long attempt_index, long target_index);

#endif // NETWORKING_TRANSPORT_TRANSPORT_QOS_H
