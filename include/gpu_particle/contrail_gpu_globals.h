#ifndef GPU_PARTICLE_CONTRAIL_GPU_GLOBALS_H
#define GPU_PARTICLE_CONTRAIL_GPU_GLOBALS_H

#include "memory/data_types.h"
extern "C" {
extern __declspec(thread) s_data_array *g_contrail_chain_data;
}

#endif
