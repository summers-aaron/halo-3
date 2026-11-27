#ifndef STRUCTURES_CLUSTER_PARTITION_TYPES_H
#define STRUCTURES_CLUSTER_PARTITION_TYPES_H

struct s_data_array;

struct cluster_partition
{
    s_data_array *data;
    s_data_array *references;
    long *cluster_heads;
};

struct s_cluster_partition_element
{
    int datum_header;
    int datum_index;
    long next;
};

struct s_cluster_partition_reference
{
    unsigned short identifier;
    unsigned short flags;
    long cluster_datum;
    long next;
};

#if !defined(_WIN64) && !defined(__LP64__)
typedef char cluster_partition_size[(sizeof(cluster_partition) == 12) ? 1 : -1];
typedef char cluster_partition_element_size[(sizeof(s_cluster_partition_element) == 12) ? 1 : -1];
typedef char cluster_partition_reference_size[(sizeof(s_cluster_partition_reference) == 12) ? 1 : -1];
#endif

#endif // STRUCTURES_CLUSTER_PARTITION_TYPES_H
