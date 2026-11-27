#ifndef GAME_PARTICLE_LOCATION_DATA_ARRAY_TYPES_H
#define GAME_PARTICLE_LOCATION_DATA_ARRAY_TYPES_H

#include "memory/data_types.h"

struct s_particle_location_datum;

// The particle-location pool is a plain data array; its `data` block holds
// the location records and callers cast it to the datum type.
struct s_particle_location_data_array : public s_data_array
{
};

#endif // GAME_PARTICLE_LOCATION_DATA_ARRAY_TYPES_H
