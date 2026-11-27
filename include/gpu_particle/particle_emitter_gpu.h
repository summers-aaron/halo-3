#ifndef GPU_PARTICLE_PARTICLE_EMITTER_GPU_H
#define GPU_PARTICLE_PARTICLE_EMITTER_GPU_H

#include "gpu_particle/constant_table_types.h"
#include "gpu_particle/gpu_storage.h"
#include "gpu_particle/target_table_types.h"

struct s_particle_emitter_gpu_block;
class c_constant_table;
class c_particle_state_list;

extern "C" void particle_block_frame_advance_emitter(s_particle_emitter_gpu_block *object, float dt, c_constant_table *table, c_particle_state_list *states);

class c_particle_emitter_gpu {
    friend void particle_block_frame_advance_emitter(s_particle_emitter_gpu_block *object, float dt, c_constant_table *table, c_particle_state_list *states);
public:
    struct s_spawn_vertex_shader_constant_list { enum { slot_count = 1 }; };
    struct s_render_vertex_shader_constant_list {
        enum { slot_count = 5 };
        static const bool x_in_place[slot_count];
        static const int x_keys[slot_count];
    };
    struct s_update_vertex_shader_constant_list {
        enum { slot_count = 6 };
        static const bool x_in_place[slot_count];
        static const int x_keys[slot_count];
    };
    struct s_layout;
    static s_layout x_layout;
    static __forceinline void initialize_for_map();
    static __forceinline void dispose_from_map();
    static __forceinline void dispose_shaders();
    static void spawn_all();
    static void frame_advance_all(float elapsed_time);
    void render() const;
public:
    long m_identifier; // March create fills offsets 4-20, never 0: datum salt (cf. c_contrail_gpu::identifier).
    long m_system_index;
    long m_definition_index;
    long m_emitter_index;
    long m_first_row_index;
private:
    template<class ConstantList> void set_shader_state(const s_gpu_constant_table<ConstantList> *table, const c_particle_state_list *states) const;
    template<class ConstantList> void set_shader_render_state(const s_gpu_constant_table<ConstantList> *table, unsigned long flags) const;
    template<class ConstantList> void set_shader_variants(const s_gpu_constant_table<ConstantList> *table) const;
    template<class ConstantList> void set_shader_update_state(const s_gpu_constant_table<ConstantList> *table, const class c_particle_state_list *states) const;
    static void initialize_shaders();
};

// The vertex layout of one GPU particle: eleven elements behind the layout's element table.
struct c_particle_emitter_gpu::s_layout : s_gpu_target_table<11>
{
    s_layout();

    s_gpu_storage_float4 position;
    s_gpu_storage_half4 velocity;
    s_gpu_storage_ushort4n random;
    s_gpu_storage_ushort4n random2;
    s_gpu_storage_ushort4n scale;
    s_gpu_storage_half4 time;
    s_gpu_storage_half2 size;
    s_gpu_storage_ushort2n texture_coordinates;
    s_gpu_storage_dec3n rotation;
    s_gpu_storage_color primary_color;
    s_gpu_storage_color color;
};

template<>
void c_particle_emitter_gpu::set_shader_state<c_particle_emitter_gpu::s_update_vertex_shader_constant_list>(
    const s_gpu_constant_table<c_particle_emitter_gpu::s_update_vertex_shader_constant_list> *table,
    const c_particle_state_list *states) const;

template<>
void c_particle_emitter_gpu::set_shader_update_state<c_particle_emitter_gpu::s_update_vertex_shader_constant_list>(
    const s_gpu_constant_table<c_particle_emitter_gpu::s_update_vertex_shader_constant_list> *table,
    const c_particle_state_list *states) const;

#endif
