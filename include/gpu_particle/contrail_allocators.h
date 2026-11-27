#ifndef GPU_PARTICLE_CONTRAIL_ALLOCATORS_H
#define GPU_PARTICLE_CONTRAIL_ALLOCATORS_H

#include "cache/restricted_memory_regions.h"

void __tls_set_g_contrail_gpu_allocator(void *address);
void __tls_set_g_contrail_row_gpu_allocator(void *address);
extern "C" void contrail_gpu_set_buffer_address(void *address);

extern t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_contrail_gpu_allocator> g_contrail_gpu_allocator;

extern "C" {
extern t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_contrail_row_gpu_allocator> g_contrail_gpu_row_allocator;
// Holds the contrail vertex buffer's member of restricted region 2; the member's address goes to
// contrail_gpu_set_buffer_address.
extern t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &contrail_gpu_set_buffer_address> g_contrail_gpu_emitter_allocator;
}

#endif // GPU_PARTICLE_CONTRAIL_ALLOCATORS_H
