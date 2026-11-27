#ifndef GPU_PARTICLE_PARTICLE_BLOCK_STRIP_OWNER_TYPES_H
#define GPU_PARTICLE_PARTICLE_BLOCK_STRIP_OWNER_TYPES_H

struct particle_block_strip_owner {
    long : 32;
    long : 32;
    long first_row;
};

#endif // GPU_PARTICLE_PARTICLE_BLOCK_STRIP_OWNER_TYPES_H
