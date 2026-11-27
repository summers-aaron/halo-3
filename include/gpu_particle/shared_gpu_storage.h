#ifndef GPU_PARTICLE_SHARED_GPU_STORAGE_H
#define GPU_PARTICLE_SHARED_GPU_STORAGE_H

extern "C" {
extern const float *g_gpu_particle_default_constants;
extern __declspec(thread) void *g_light_volume_gpu_storage_address;
}

#endif // GPU_PARTICLE_SHARED_GPU_STORAGE_H
