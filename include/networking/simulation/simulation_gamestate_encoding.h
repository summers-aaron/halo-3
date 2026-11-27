#ifndef NETWORKING_SIMULATION_SIMULATION_GAMESTATE_ENCODING_H
#define NETWORKING_SIMULATION_SIMULATION_GAMESTATE_ENCODING_H

#include "memory/bitstream.h"

// The queue and object-state codecs use a 16-bit datum identifier followed
// by an 11-bit absolute index (the gamestate entity table has 2048 slots).
__forceinline void simulation_gamestate_index_decode(c_bitstream *packet, long *gamestate_index)
{
    long identifier = packet->read_integer("gamestgate-index-id", 16);
    long absolute_index = packet->read_integer("gamestate-index-absolute", 11);
    *gamestate_index = (identifier << 16) | absolute_index;
}

#endif
