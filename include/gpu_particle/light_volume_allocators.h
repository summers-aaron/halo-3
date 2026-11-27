#ifndef GPU_PARTICLE_LIGHT_VOLUME_ALLOCATORS_H
#define GPU_PARTICLE_LIGHT_VOLUME_ALLOCATORS_H

#include "cache/restricted_memory_regions.h"

void __tls_set_g_animation_threads_allocator(void *object);
extern "C" void particle_block_set_contrail_link_array(void *address);
void __tls_set_g_light_volume_gpu_allocator(void *object);
void __tls_set_g_light_volume_row_gpu_allocator(void *object);

extern "C" {
extern t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_animation_threads_allocator> g_light_volume_gpu_main_allocator;
extern t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &particle_block_set_contrail_link_array> g_light_volume_gpu_row_allocator;
extern t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_light_volume_gpu_allocator> g_beam_gpu_allocator;
extern t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_light_volume_row_gpu_allocator> g_beam_gpu_row_allocator;
}

#endif // GPU_PARTICLE_LIGHT_VOLUME_ALLOCATORS_H
