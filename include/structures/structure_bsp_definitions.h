#ifndef STRUCTURES_STRUCTURE_BSP_DEFINITIONS_H
#define STRUCTURES_STRUCTURE_BSP_DEFINITIONS_H

#include "objects/object_reference_types.h"

// Functions defined in tu/blamlibxbox/structure_bsp_definitions.cpp that other units call.

extern "C" bool structure_bsp_get_actor_clusters(long index, unsigned long *bit_vector, long count);

struct s_game_cluster_bit_vectors;

void structure_bsp_compute_cluster_active_pvs(s_cluster_reference cluster_reference, s_game_cluster_bit_vectors *pvs);

#endif // STRUCTURES_STRUCTURE_BSP_DEFINITIONS_H
