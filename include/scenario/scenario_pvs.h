#ifndef SCENARIO_SCENARIO_PVS_H
#define SCENARIO_SCENARIO_PVS_H

#include "objects/object_reference_types.h"

// Functions defined in tu/blamlibxbox/scenario_pvs.cpp that other units call.

struct s_game_cluster_bit_vectors;
struct s_scenario_pvs_row;

void scenario_zone_set_pvs_write_row(s_game_cluster_bit_vectors *clusters, const s_scenario_pvs_row *row);
void scenario_zone_set_pvs_get_row(long scenario_index,s_scenario_pvs_row *output,long zone_index,s_cluster_reference reference,bool doors);

#endif // SCENARIO_SCENARIO_PVS_H
