#ifndef GPU_PARTICLE_DESC_TYPES_H
#define GPU_PARTICLE_DESC_TYPES_H

#include "gpu_particle/constant_entry_types.h"

// The 12-byte descriptor is copied as three words before its component count is overridden.
class c_rasterizer_constant_desc : public s_constant_entry
{
public:
    union {
        long ownership_data;
        struct { unsigned char owned; };
    };
};
typedef char gpu_constant_desc_size[(sizeof(c_rasterizer_constant_desc) == 12) ? 1 : -1];

#endif // GPU_PARTICLE_DESC_TYPES_H
