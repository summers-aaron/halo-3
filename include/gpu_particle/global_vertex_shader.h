#ifndef GPU_PARTICLE_GLOBAL_VERTEX_SHADER_H
#define GPU_PARTICLE_GLOBAL_VERTEX_SHADER_H

#include "gpu_particle/shader_slot_types.h"
#include "gpu_particle/constant_table_types.h"
#include "tag_files/tag_types.h"

// The shader reference precedes the slot set and the selected vertex type.
template<class ConstantList>
struct s_gpu_global_vertex_shader
{
    const s_tag_reference *shader_reference;
    s_gpu_global_shader<ConstantList> constants;
    long vertex_type;
    friend class c_contrail_gpu;
private:
    s_entry_table *initialize_constant_table();
public:
    __forceinline bool is_initialized() const { return shader_reference != 0; }
    __forceinline void dispose() { shader_reference = 0; }
    __forceinline void initialize(const s_tag_reference *reference);
    __forceinline const s_gpu_constant_table<ConstantList> *get_constant_table() const
    {
        return &constants;
    }
};

#endif // GPU_PARTICLE_GLOBAL_VERTEX_SHADER_H
