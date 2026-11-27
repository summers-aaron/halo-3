#ifndef GPU_PARTICLE_CONTRAIL_GPU_H
#define GPU_PARTICLE_CONTRAIL_GPU_H

#include "gpu_particle/contrail_allocators.h"

// Functions defined in tu/graphicslibxbox/contrail_gpu.cpp.

struct s_entry_table;

template<long k_target_count> struct s_gpu_target_table;
extern "C" void contrail_gpu_initialize_targets(s_gpu_target_table<8> *table, long unused, void *owner);
extern "C" long contrail_gpu_new_row(unsigned long allocation_context, long owner);
extern "C" void contrail_gpu_destroy_links(unsigned long handle, int unused);
extern "C" s_entry_table *contrail_gpu_get_constant_table(void);
extern "C" void contrail_gpu_initialize_frame_targets(void);

extern "C" void contrail_gpu_destroy(unsigned long handle);

extern unsigned char g_contrail_gpu_block_until_idle;
extern "C" void *contrail_gpu_row_try_and_get(long handle);
void *particle_emitter_gpu_try_and_get(int handle);

#endif // GPU_PARTICLE_CONTRAIL_GPU_H
