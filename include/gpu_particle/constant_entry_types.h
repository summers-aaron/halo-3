#ifndef GPU_PARTICLE_CONSTANT_ENTRY_TYPES_H
#define GPU_PARTICLE_CONSTANT_ENTRY_TYPES_H

struct s_constant_entry
{
    int key;
    union
    {
        long register_range;
        struct { unsigned short start; unsigned char count; };
    };
};

#endif // GPU_PARTICLE_CONSTANT_ENTRY_TYPES_H
