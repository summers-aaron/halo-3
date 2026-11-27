#ifndef STRUCTURES_CLUSTER_PARTITION_NEW_H
#define STRUCTURES_CLUSTER_PARTITION_NEW_H

struct cluster_partition;
class c_allocation_base;

extern "C" void cluster_partition_new(cluster_partition *partition, const char *name, long payload_size, c_allocation_base *allocator,
    c_allocation_base *data_allocator, c_allocation_base *reference_allocator);

#endif // STRUCTURES_CLUSTER_PARTITION_NEW_H
