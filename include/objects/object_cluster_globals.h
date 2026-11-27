#ifndef OBJECTS_OBJECT_CLUSTER_GLOBALS_H
#define OBJECTS_OBJECT_CLUSTER_GLOBALS_H
#include "objects/recycling_groups.h"
#include "objects/object_cluster_tls.h"

#include "structures/cluster_partition_types.h"

struct groups;

// wpl/alias.c requires these fixed TLS slot linker names (memory/tls_globals.h).

static __forceinline cluster_partition &tls_collideable_object_cluster_partition()
{
    return collideable_object_cluster_partition;
}

static __forceinline cluster_partition &tls_noncollideable_object_cluster_partition()
{
    return noncollideable_object_cluster_partition;
}

static __forceinline s_data_array *&tls_recycling_groups()
{
    return recycling_groups;
}

#endif
