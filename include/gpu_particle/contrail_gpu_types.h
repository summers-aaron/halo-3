#ifndef GPU_PARTICLE_CONTRAIL_GPU_TYPES_H
#define GPU_PARTICLE_CONTRAIL_GPU_TYPES_H

#include "gpu_particle/gpu_storage.h"
#include "gpu_particle/target_table_types.h"

class c_contrail_states;
class c_constant_table;

class c_contrail_profile;
struct s_object_14;
class c_contrail_gpu;
template<typename ConstantList> struct s_gpu_constant_table;
template<class t_layout, long k_gpu_max> struct s_queued_gpu_buffer_system;
extern "C" void particle_block_frame_advance_contrail(void *, float, void *);

class c_contrail_gpu {
public:
    struct s_spawn_vertex_shader_constant_list { enum { slot_count = 1 }; };
    struct s_render_vertex_shader_constant_list {
        enum { slot_count = 2 };
        static const bool x_in_place[slot_count];
        static const int x_keys[slot_count];
    };
    struct s_update_vertex_shader_constant_list {
        enum { slot_count = 5 };
        static const bool x_in_place[slot_count];
        static const int x_keys[slot_count];
    };
    // The vertex layout of one GPU contrail profile: eight elements behind the layout's element table.
    struct s_layout : s_gpu_target_table<8>
    {
        s_layout();

        s_gpu_storage_float4 position;
        s_gpu_storage_half4 velocity;
        s_gpu_storage_ushort4n random;
        s_gpu_storage_half4 unused_half4;
        s_gpu_storage_ushort4n unused_ushort4n;
        s_gpu_storage_half2 length;
        s_gpu_storage_color unused_color;
        s_gpu_storage_color color;
    };
    static s_layout x_layout;
    static s_queued_gpu_buffer_system<s_layout, 128> x_queued_buffer_system;
    __forceinline bool get_alive() const { return profile_count > 0; }
    static void initialize_for_map();
    static void dispose_from_map();
    static __forceinline void dispose_from_game();
    static c_contrail_gpu *get(long index);
    static __forceinline long create(long owner);
    static void spawn_all();
    static void frame_advance_all(float delta_time);
    void render(const c_contrail_states *contrail_states) const;
    void queue_profile(unsigned long queue_context, const c_contrail_profile *profile, bool temporary);
    struct s_row {
        short identifier;
        long next_row_index;
        long owner;
        unsigned short index;
        unsigned char allocated;
        float lifespan;
        __forceinline long get_last_allocated() const { return (index << 4) + allocated - 1; }
    };
private:
    friend class c_contrail;
    static void initialize_shaders();
    static void dispose_shaders();
    void set_shader_functions(c_constant_table *table) const;
    template<typename ConstantList> void set_shader_strip(const s_gpu_constant_table<ConstantList> *table, long value) const;
    template<typename ConstantList> void set_shader_strip(const s_gpu_constant_table<ConstantList> *table) const;
    template<typename ConstantList> void set_shader_state(const s_gpu_constant_table<ConstantList> *table, const c_contrail_states *contrail_states) const;
    template<typename ConstantList> void set_shader_functions(const s_gpu_constant_table<ConstantList> *table) const;
    unsigned long allocate_profile(unsigned long allocation, const c_contrail_profile *profile);
    void frame_advance(s_object_14 *states);
    friend void particle_block_frame_advance_contrail(void *, float, void *);
    __forceinline bool get_prev_profile_temporary() const { return (flags & 1) != 0; }
    long identifier;
    long contrail_index;
    long first_row_index;
    unsigned short profile_count;
    unsigned short flags;
};


typedef char contrail_gpu_size[(sizeof(c_contrail_gpu) == 0x10) ? 1 : -1];
typedef char contrail_gpu_row_size[(sizeof(c_contrail_gpu::s_row) == 0x14) ? 1 : -1];

template<>
void c_contrail_gpu::set_shader_functions<c_contrail_gpu::s_update_vertex_shader_constant_list>(
    const s_gpu_constant_table<c_contrail_gpu::s_update_vertex_shader_constant_list> *table) const;
template<>
void c_contrail_gpu::set_shader_state<c_contrail_gpu::s_update_vertex_shader_constant_list>(
    const s_gpu_constant_table<c_contrail_gpu::s_update_vertex_shader_constant_list> *table,
    const c_contrail_states *contrail_states) const;

#endif
