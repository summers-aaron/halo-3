#ifndef INCLUDE_OBJECTS_OBJECT_CLUSTER_TLS_H
#define INCLUDE_OBJECTS_OBJECT_CLUSTER_TLS_H

#include "structures/cluster_partition_types.h"


// March 2007 TLS slot 0x831df618.
extern __declspec(thread) cluster_partition noncollideable_object_cluster_partition;


// March 2007 TLS slot 0x831df60c.
extern __declspec(thread) cluster_partition collideable_object_cluster_partition;

#endif // INCLUDE_OBJECTS_OBJECT_CLUSTER_TLS_H
