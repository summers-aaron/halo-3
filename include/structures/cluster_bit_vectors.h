#ifndef STRUCTURES_CLUSTER_BIT_VECTORS_H
#define STRUCTURES_CLUSTER_BIT_VECTORS_H

#include "memory/static_arrays.h"
#include "objects/cluster_reference.h"

struct s_game_cluster_bit_vectors
{
    c_static_flags_no_init<256> bit_vectors[16];
};

static __forceinline bool game_clusters_test(const s_game_cluster_bit_vectors *vectors, s_cluster_reference reference)
{
    return vectors->bit_vectors[reference.bsp_index].test(reference.cluster_index);
}

typedef char game_cluster_bit_vectors_size[(sizeof(long) != 4 || (sizeof(s_game_cluster_bit_vectors) == 0x200)) ? 1 : -1];

#endif // STRUCTURES_CLUSTER_BIT_VECTORS_H
