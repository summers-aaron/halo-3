#ifndef EFFECTS_LIGHT_VOLUME_GPU_H
#define EFFECTS_LIGHT_VOLUME_GPU_H

#include <stddef.h>
#include "memory/data_types.h"
#include "memory/data.h"
#include "effects/effect_runtime_storage.h"

template<class ConstantList> struct s_gpu_constant_table;
class c_light_volume_states;
class c_light_volume_system;

#include "effects/light_volume_gpu_types.h"

__forceinline c_light_volume_gpu *c_light_volume_gpu::get(long volume_index)
    {
        return datum_get<c_light_volume_gpu>(g_light_volume_gpu_data, volume_index);
    }

extern "C" void light_volume_row_gpu_initialize(void);
extern "C" void light_volume_row_gpu_reset(void);
extern "C" void light_volume_row_gpu_disable(void);
extern "C" void light_volume_row_gpu_dispose(void);

#endif
