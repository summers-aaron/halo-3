#ifndef EFFECTS_PARTICLE_ARRAY_GLOBALS_H
#define EFFECTS_PARTICLE_ARRAY_GLOBALS_H

struct s_data_array;

extern "C" {
extern __declspec(thread) s_data_array *g_particle_system_data_array;
extern __declspec(thread) s_data_array *g_particle_location_data_array;
extern __declspec(thread) s_data_array *g_particle_emitter_data;
extern __declspec(thread) s_data_array *g_particle_gpu_block_data;
extern __declspec(thread) s_data_array *g_contrail_gpu_data;
extern __declspec(thread) s_data_array *g_contrail_gpu_row_data;
extern __declspec(thread) s_data_array *g_contrail_gpu_link_data;
}

#endif
