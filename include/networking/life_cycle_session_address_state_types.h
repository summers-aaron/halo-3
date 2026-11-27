#ifndef NETWORKING_LIFE_CYCLE_SESSION_ADDRESS_STATE_TYPES_H
#define NETWORKING_LIFE_CYCLE_SESSION_ADDRESS_STATE_TYPES_H

#include "memory/data_types.h"
#include "networking/life_cycle_qos_request_types.h"

struct s_life_cycle_session_address_state { bool valid; unsigned long last_update; s_life_cycle_qos_request address; };

#endif // NETWORKING_LIFE_CYCLE_SESSION_ADDRESS_STATE_TYPES_H
