#ifndef GPU_PARTICLE_CONSTANT_TABLE_H
#define GPU_PARTICLE_CONSTANT_TABLE_H

#include "gpu_particle/gpu_constant_table_types.h"
#include "rasterizer/c_rasterizer_constant_table_ref.h"

class c_constant_table
{
public:
    void set_scaled(long index, long count, const void *data);
    __forceinline void set(long index, unsigned char count, const void *arg)
    {
        set_entry(entries[index], count, arg);
    }
    __forceinline void set_entry(c_rasterizer_constant_desc entry, unsigned char count, const void *arg)
    {
        c_rasterizer_constant_desc local = entry;
        local.count = count;
        table.set_constants(&local, arg);
    }
private:
    c_rasterizer_constant_table_ref table;
    c_rasterizer_constant_desc *entries;
};

#endif
