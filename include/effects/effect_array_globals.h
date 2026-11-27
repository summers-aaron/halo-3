#ifndef EFFECTS_EFFECT_ARRAY_GLOBALS_H
#define EFFECTS_EFFECT_ARRAY_GLOBALS_H

// Thread-local data arrays for the effect systems.
struct s_data_array;
extern "C" {
extern __declspec(thread) s_data_array *g_particle_emitter_gpu_data;
extern __declspec(thread) s_data_array *g_decal_system_data;
extern __declspec(thread) s_data_array *g_contrail_system_data;
extern __declspec(thread) s_data_array *g_contrail_location_data;
}

#endif
