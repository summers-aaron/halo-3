#ifndef GPU_PARTICLE_VERTEX_SHADER_H
#define GPU_PARTICLE_VERTEX_SHADER_H

#include "gpu_particle/shader_slot_types.h"

template<class ConstantList> struct s_gpu_vertex_shader {
    long shader_index;
    const s_tag_reference *shader_reference;
    s_gpu_shader<ConstantList> constants;
    long vertex_type;
    void initialize();
private:
    __forceinline c_rasterizer_constant_table_ref initialize_constant_table();
    __forceinline void initialize_shader(c_rasterizer_constant_table_ref table_ref);
};
#endif // GPU_PARTICLE_VERTEX_SHADER_H
