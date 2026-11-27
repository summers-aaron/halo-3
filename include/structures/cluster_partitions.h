#ifndef STRUCTURES_CLUSTER_PARTITIONS_H
#define STRUCTURES_CLUSTER_PARTITIONS_H

#include "objects/object_reference_types.h"

// Functions defined in tu/blamlibxbox/cluster_partitions.cpp that other units call.

struct cluster_partition;
union real_point3d;
struct s_game_cluster_bit_vectors;
struct s_location;

void cluster_partition_make_valid(cluster_partition *partition);
void cluster_partition_update_payload(cluster_partition *partition, long datum_index, long first_reference, long payload_size, const void *payload);
void cluster_partition_reconnect(cluster_partition *partition, long datum_index, long *first_cluster_reference, const real_point3d *position,
    float radius, const s_location *location, const s_game_cluster_bit_vectors *bits, long payload_size, const void *payload, bool *overflow);
extern "C" void cluster_partition_disconnect(cluster_partition *partition, long key, long *index);
long cluster_partition_get_first_datum_and_payload(const cluster_partition *partition, long *iterator, s_cluster_reference cluster,
    long payload_size, const void **payload);

#endif // STRUCTURES_CLUSTER_PARTITIONS_H
