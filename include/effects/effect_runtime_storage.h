#ifndef EFFECTS_EFFECT_RUNTIME_STORAGE_H
#define EFFECTS_EFFECT_RUNTIME_STORAGE_H

struct s_data_array;

// Image-backed shared storage; address annotations remain in the using units.
extern "C" {
extern __declspec(thread) s_data_array * g_particle_instance_owner_data;
extern __declspec(thread) s_data_array *g_light_volume_data;
extern __declspec(thread) s_data_array *g_light_volume_gpu_data;
extern __declspec(thread) s_data_array *g_light_volume_gpu_row_data;
extern __declspec(thread) s_data_array *g_light_volume_system_data;
extern __declspec(thread) s_data_array * g_particle_instance_data;
extern __declspec(thread) s_data_array * g_particle_instance_location_data;
extern __declspec(thread) s_data_array *g_contrail_chain_data;
}

#endif
