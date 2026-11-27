#ifndef SIMULATION_SIMULATION_MACHINE_UPDATE_TYPES_H
#define SIMULATION_SIMULATION_MACHINE_UPDATE_TYPES_H

#include "game/player_identifier_types.h"

// The machine-presence mask and sixteen 48-bit identifiers in simulation updates.
struct simulation_machine_update
{
    unsigned long machine_valid_mask;
    s_machine_identifier machine_identifiers[16];
};

typedef char simulation_machine_update_size[(sizeof(simulation_machine_update) == 0x64) ? 1 : -1];

#endif
