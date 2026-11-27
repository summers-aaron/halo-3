#ifndef ANIMATIONS_ANIMATION_GRAPH_NODE_TYPES_H
#define ANIMATIONS_ANIMATION_GRAPH_NODE_TYPES_H
#include <stddef.h>
// Hierarchy indices used by orientation computation and matrix construction.
struct animation_graph_node {
    long name;
    short next_sibling_node_index;
    short first_child_node_index;
    short parent_node_index;
    unsigned char flags;
    char unmodeled_node_data[0x20 - 0xb]; // tag bytes past flags; readers use name/indices/flags only
};
typedef char animation_graph_node_size[(sizeof(animation_graph_node) == 0x20) ? 1 : -1];
typedef char animation_graph_node_parent_offset[(offsetof(animation_graph_node, parent_node_index) == 8) ? 1 : -1];
typedef char animation_graph_node_flags_offset[(offsetof(animation_graph_node, flags) == 0xa) ? 1 : -1];
#endif
