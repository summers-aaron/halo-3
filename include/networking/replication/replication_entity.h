#ifndef NETWORKING_REPLICATION_REPLICATION_ENTITY_H
#define NETWORKING_REPLICATION_REPLICATION_ENTITY_H

#include "memory/bitstream.h"

static __forceinline void replication_entity_index_encode(c_bitstream *packet, long entity_index)
{
    packet->write_integer("entity-absolute-index", entity_index & 0x3ff, 10);
    packet->write_integer("entity-seed", (unsigned long)entity_index >> 28, 4);
}

static __forceinline void replication_entity_index_decode(c_bitstream *packet, long *entity_index)
{
    long absolute_index = packet->read_integer("entity-absolute-index", 10);
    unsigned char seed = (unsigned char)packet->read_integer("entity-seed", 4);
    *entity_index = absolute_index | (seed << 28);
}
union real_point3d;

extern "C" void simulation_read_quantized_node_space_position(c_bitstream *packet, real_point3d *position, long bounds_index, long bits_per_component);

#endif // NETWORKING_REPLICATION_REPLICATION_ENTITY_H
