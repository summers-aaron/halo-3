#ifndef EFFECTS_PARTICLE_EMITTER_GPU_ARRAY_TYPES_H
#define EFFECTS_PARTICLE_EMITTER_GPU_ARRAY_TYPES_H

#include "memory/data_types.h"

// Shared 0x18-byte GPU block; emitter readers establish the three owner indices.
struct s_particle_emitter_gpu_block
{
    short identifier;
    long source;
    long unit_index;
    long emitter_index;
    long block_index;
    unsigned short count;
};
typedef char particle_emitter_gpu_block_size[(sizeof(s_particle_emitter_gpu_block) == 0x18) ? 1 : -1];

#endif // EFFECTS_PARTICLE_EMITTER_GPU_ARRAY_TYPES_H
