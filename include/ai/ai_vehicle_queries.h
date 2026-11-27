#ifndef AI_AI_VEHICLE_QUERIES_H
#define AI_AI_VEHICLE_QUERIES_H

// match: 0x503d08, 0x53dfb8, 0x53e230: callers pass a full datum index;
// the vehicle implementation narrows it when indexing the actor array.
extern "C" unsigned char ai_vehicle_enterable(long actor_index, int vehicle_index, float radius, unsigned char check_distance);

#endif
