#ifndef SIMULATION_GAME_INTERFACE_ENTITY_LOCAL_FLAGS_TYPES_H
#define SIMULATION_GAME_INTERFACE_ENTITY_LOCAL_FLAGS_TYPES_H

#include "memory/data_types.h"

struct s_entity_local_flags
{
    unsigned char flags;
    char flags_padding[3];
};

#endif // SIMULATION_GAME_INTERFACE_ENTITY_LOCAL_FLAGS_TYPES_H
