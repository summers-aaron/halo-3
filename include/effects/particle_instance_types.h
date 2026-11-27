#ifndef EFFECTS_PARTICLE_INSTANCE_TYPES_H
#define EFFECTS_PARTICLE_INSTANCE_TYPES_H

#include <stddef.h>

// Particle instance record.
struct s_particle_instance
{
    long identifier, next, gpu_index, definition_index, location_index, flags;
    float length, offset, density, profile_length;
    unsigned long random_seed;
    float random_values[2];
};

typedef char particle_instance_size[(sizeof(s_particle_instance) == 0x34) ? 1 : -1];
typedef char particle_instance_location_offset[(offsetof(s_particle_instance, location_index) == 0x10) ? 1 : -1];
typedef char particle_instance_length_offset[(offsetof(s_particle_instance, length) == 0x18) ? 1 : -1];

#endif
