#ifndef GPU_PARTICLE_LIGHT_VOLUME_GPU_H
#define GPU_PARTICLE_LIGHT_VOLUME_GPU_H

// Functions defined in tu/graphicslibxbox/light_volume_gpu.cpp.

#include "effects/light_volume_gpu_types.h"
#include "gpu_particle/shader_slot_types.h"

template<> void s_gpu_shader<c_light_volume_gpu::s_update_vertex_shader_constant_list>::initialize(c_rasterizer_constant_table_ref table_ref);

// Overall-state constant layout the update/render shader states upload.
__declspec(align(16)) struct light_volume_gpu_scalar { float value; };
__declspec(align(16)) struct light_volume_gpu_pair { real_vector2d value; };
__declspec(align(16)) struct light_volume_gpu_position { real_point3d value; };
__declspec(align(16)) struct light_volume_gpu_direction { real_vector3d value; };
__declspec(align(16)) struct light_volume_gpu_profile { float scale, bias, angle_scale, angle_bias; };
struct light_volume_gpu_shader_overall {
    light_volume_gpu_scalar type, blend, flags, offset, parameter, density, length, profile_length;
    light_volume_gpu_pair uv_tiling_rate, uv_scroll_rate;
    light_volume_gpu_scalar time;
    light_volume_gpu_position origin;
    light_volume_gpu_direction direction;
    light_volume_gpu_profile profile;
    light_volume_gpu_scalar states[12];
};

struct s_effect_parameters;
struct s_particle_instance;

extern "C" void light_volume_gpu_initialize(void);
extern "C" void beam_gpu_initialize(void);
extern "C" void light_volume_submit_particle_instance(s_particle_instance *instance);

struct s_handle;
struct s_pass_state;

extern "C" void light_volume_gpu_set_effect_constants(s_handle *handle, s_pass_state *state, const s_effect_parameters *parameters);

#endif // GPU_PARTICLE_LIGHT_VOLUME_GPU_H
