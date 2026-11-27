#ifndef OBJECTS_OBJECT_NODE_MATRICES_INLINE_H
#define OBJECTS_OBJECT_NODE_MATRICES_INLINE_H

#include "objects/object_datum.h"
#include "objects/object_header.h"
#include "math/matrix_math_types.h"

namespace {
static __forceinline real_matrix4x3 *object_get_node_matrices(long object_index, long *count) {
    object_datum *object = object_get(object_index);
    *count = object->node_matrices.size / sizeof(real_matrix4x3);
    return (real_matrix4x3 *)((char *)object + object->node_matrices.offset);
}
static __forceinline real_matrix4x3 *object_get_node_matrix(long object_index, short node_index) {
    long node_count;
    real_matrix4x3 *node_matrices = object_get_node_matrices(object_index, &node_count);
    return &node_matrices[node_index];
}
}

#endif
