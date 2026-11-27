#ifndef EFFECTS_DECAL_MESSAGE_TYPES_H
#define EFFECTS_DECAL_MESSAGE_TYPES_H

#include "objects/cluster_reference.h"

// Render-thread decal message: type 0 adds the cluster to the decal PVS, 1 removes it.
class c_decal_message_union {
public:
    unsigned long type;
    s_cluster_reference cluster_reference;
};

typedef char decal_message_union_size[(sizeof(c_decal_message_union) == 8) ? 1 : -1];

#endif
