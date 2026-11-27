#ifndef GPU_PARTICLE_CONTRAIL_TARGET_ENTRY_H
#define GPU_PARTICLE_CONTRAIL_TARGET_ENTRY_H

#include "gpu_particle/particle_block_bounds_types.h"

// One vertex element of a GPU layout: its usage, the buffer it streams from, the fetch constant built for that
// buffer and its stride and offset in elements of the stored type. s_gpu_storage<...> supplies the virtuals.
class __declspec(novtable) s_gpu_storage_interface
{
public:
    s_gpu_storage_interface(long semantic, long usage_index) :
        semantic(semantic),
        usage_index(usage_index)
    {
    }

    virtual void layout(unsigned long vertex_stride, unsigned long element_offset) = 0;
    virtual void refresh() = 0;
    virtual unsigned long size() = 0;
    virtual long kind() = 0;
    virtual unsigned long declaration_type() = 0;

    long semantic;
    long usage_index;
    void *owner;
    s_particle_block_bounds bounds;
    unsigned long stride;
    unsigned long offset;
};

typedef char gpu_storage_interface_size[(sizeof(void *) != 4 || sizeof(s_gpu_storage_interface) == 0x28) ? 1 : -1];

#endif // GPU_PARTICLE_CONTRAIL_TARGET_ENTRY_H
