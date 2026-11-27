#ifndef INCLUDE_OBJECTS_OBJECT_CLUSTER_REFERENCE_H
#define INCLUDE_OBJECTS_OBJECT_CLUSTER_REFERENCE_H

#include "objects/cluster_reference.h"
struct s_cluster_reference_iterator;

extern "C" s_cluster_reference objects_cluster_reference_get_first(s_cluster_reference_iterator *iterator, long object_index);
extern "C" s_cluster_reference objects_cluster_reference_get_next(s_cluster_reference_iterator *iterator, long object_index);

#endif // INCLUDE_OBJECTS_OBJECT_CLUSTER_REFERENCE_H
