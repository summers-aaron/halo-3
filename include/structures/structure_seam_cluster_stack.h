#ifndef STRUCTURES_STRUCTURE_SEAM_CLUSTER_STACK_H
#define STRUCTURES_STRUCTURE_SEAM_CLUSTER_STACK_H

#include "objects/object_reference_types.h"

struct s_cluster_stack
{
    long m_count;
    s_cluster_reference m_elements[32];

    enum e_uninitialized { _uninitialized };
    __forceinline s_cluster_stack() : m_count(0) {}
    __forceinline explicit s_cluster_stack(e_uninitialized) {}
    __forceinline long count() const { return m_count; }
    __forceinline bool full() const { return m_count == 32; }
    __forceinline long push() { return m_count++; }
    __forceinline s_cluster_reference *get_top() { return &m_elements[m_count - 1]; }
    __forceinline void push_back(const s_cluster_reference &reference) { m_elements[push()] = reference; }
};

typedef char structure_seam_cluster_stack_size[(sizeof(s_cluster_stack) == 0x44) ? 1 : -1];

#endif // STRUCTURES_STRUCTURE_SEAM_CLUSTER_STACK_H
