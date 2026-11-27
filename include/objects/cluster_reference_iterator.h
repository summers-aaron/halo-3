#ifndef OBJECTS_CLUSTER_REFERENCE_ITERATOR_H
#define OBJECTS_CLUSTER_REFERENCE_ITERATOR_H

struct cluster_partition;

struct s_cluster_reference_iterator
{
    const cluster_partition *partition;
    long reference_index;
};

#endif
