#ifndef OBJECTS_OBJECTS_HIERARCHY_LIST_H
#define OBJECTS_OBJECTS_HIERARCHY_LIST_H
#include <stddef.h>
#include "objects/object_datum.h"

struct s_objects_index_set {
    unsigned long bits[64];
    void set(long absolute_index) { bits[absolute_index >> 5] |= 1 << (absolute_index & 31); }
    bool test(long absolute_index) const { return (bits[absolute_index >> 5] & (1 << (absolute_index & 31))) != 0; }
};
struct s_objects_index_list {
    long indices[2048];
    long count;
    void add(long index) { long i = count++; indices[i] = index; }
};
struct s_objects_definition_list {
    long indices[2048];
    long count;
    void add(const object_datum *object) { long i = count++; indices[i] = object->definition_index; }
};
struct s_objects_hierarchy_list {
    long cluster_object_count;
    s_objects_index_set object_set;
    s_objects_index_list object_indices;
    s_objects_definition_list definition_indices;
};

typedef char objects_hierarchy_definition_offset[(offsetof(s_objects_hierarchy_list, definition_indices) == 0x2108) ? 1 : -1];
typedef char objects_hierarchy_size[(sizeof(s_objects_hierarchy_list) == 0x410c) ? 1 : -1];
#endif
