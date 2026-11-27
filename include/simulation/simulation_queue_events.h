#ifndef SIMULATION_SIMULATION_QUEUE_EVENTS_H
#define SIMULATION_SIMULATION_QUEUE_EVENTS_H

// Functions defined in tu/networkinglibxbox/simulation_queue_events.cpp.

#include "simulation/simulation_queue_event_types.h"

bool decode_event_from_buffer(unsigned char *buffer, long buffer_size, s_simulation_queue_decoded_event_data *event);
extern "C" void simulation_queue_event_insert(long type, long count, const long *references, long size, void *payload);
extern "C" bool encode_event_to_buffer(unsigned char *buffer, long buffer_size, long *out_buffer_size,
    e_simulation_event_type event_type, long reference_count, long *references, void *payload, long payload_size);

#endif // SIMULATION_SIMULATION_QUEUE_EVENTS_H
