#ifndef GPU_PARTICLE_SHADER_INFO_TYPES_H
#define GPU_PARTICLE_SHADER_INFO_TYPES_H

#include "gpu_particle/constant_entry_types.h"

#include "tag_files/tag_types.h"

struct s_entry_table
{
    int count;
    TAG_POINTER(s_constant_entry) entries;
    long : 32;
    unsigned char is_pixel;
};

#endif // GPU_PARTICLE_SHADER_INFO_TYPES_H
