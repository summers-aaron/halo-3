#ifndef NETWORKING_LIFE_CYCLE_QOS_REQUEST_TYPES_H
#define NETWORKING_LIFE_CYCLE_QOS_REQUEST_TYPES_H

#include "math/real_math_types.h"
#include "memory/data_types.h"

struct s_life_cycle_qos_request { long size; long words[6]; };

// Lightweight XDK QoS statistics bridge; request is the 28-byte XNQOSLISTENSTATS block.
#ifndef _XTL_
extern "C" int XNetQosGetListenStats(const void *identifier, s_life_cycle_qos_request *statistics);
#endif

#endif // NETWORKING_LIFE_CYCLE_QOS_REQUEST_TYPES_H
