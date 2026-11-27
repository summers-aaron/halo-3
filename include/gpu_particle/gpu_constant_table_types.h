#ifndef GPU_PARTICLE_GPU_CONSTANT_TABLE_TYPES_H
#define GPU_PARTICLE_GPU_CONSTANT_TABLE_TYPES_H

#include "gpu_particle/desc_types.h"
#include "gpu_particle/shader_info_types.h"

struct s_gpu_constant_table_base
{
    s_entry_table *shader;
    c_rasterizer_constant_desc *descs;
    unsigned long desc_count;
    unsigned char valid;

    void own_constants() const;
    void disown_constants() const;
};

#endif
