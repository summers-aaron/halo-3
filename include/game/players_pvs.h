#ifndef GAME_PLAYERS_PVS_H
#define GAME_PLAYERS_PVS_H

#include "memory/static_arrays.h"
struct s_game_cluster_bit_vectors;

void players_compute_combined_pvs(s_game_cluster_bit_vectors *pvs, bool use_observers, c_static_array<c_static_array<long, 256>, 16> *cluster_activation);

#endif // GAME_PLAYERS_PVS_H
