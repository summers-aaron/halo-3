#ifndef GPU_PARTICLE_TARGET_TABLE_TYPES_H
#define GPU_PARTICLE_TARGET_TABLE_TYPES_H

#include <stddef.h>
#include "gpu_particle/contrail_target_bounds_types.h"

class s_gpu_storage_interface;

// Export targets keep their entry pointers before the aligned GPU bounds.
template<long k_target_count>
struct s_gpu_target_table
{
    s_gpu_target_table() :
        entries(slots)
    {
    }

    s_gpu_storage_interface **entries;
    s_gpu_storage_interface *slots[k_target_count];
    __declspec(align(16)) s_contrail_target_bounds bounds[k_target_count];
};

typedef char light_volume_target_bounds_offset[(sizeof(void *) != 4 || ((offsetof(s_gpu_target_table<4>, bounds) == 0x20))) ? 1 : -1];
typedef char particle_target_bounds_offset[(sizeof(void *) != 4 || ((offsetof(s_gpu_target_table<11>, bounds) == 0x30))) ? 1 : -1];

#endif // GPU_PARTICLE_TARGET_TABLE_TYPES_H
