#ifndef EFFECTS_EFFECT_NODE_ATTACHMENT_TYPES_H
#define EFFECTS_EFFECT_NODE_ATTACHMENT_TYPES_H

#include <stddef.h>
#include "math/real_math_types.h"

struct effects_node_attachment {
    short identifier;
    unsigned char type;
    long definition_index;
    float age;
    real_point3d world_point;
    long object_index;
    short node_index;
    real_point3d local_point;
};

typedef char effect_node_attachment_size[(sizeof(effects_node_attachment) == 0x2c) ? 1 : -1];
typedef char effect_node_attachment_object_offset[(offsetof(effects_node_attachment, object_index) == 0x18) ? 1 : -1];
typedef char effect_node_attachment_local_offset[(offsetof(effects_node_attachment, local_point) == 0x20) ? 1 : -1];

#endif
