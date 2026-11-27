#ifndef EFFECTS_LIGHT_VOLUME_GPU_TYPES_H
#define EFFECTS_LIGHT_VOLUME_GPU_TYPES_H
#include <stddef.h>
#include "gpu_particle/gpu_storage.h"
#include "gpu_particle/target_table_types.h"
#include "memory/data_types.h"
template<class ConstantList> struct s_gpu_constant_table;
class c_light_volume_states;
struct s_effect_parameters;
class c_light_volume_system;

class c_light_volume_gpu
{
    friend class c_light_volume_system;
public:
    static long create(unsigned char platform, long light_volume_index);
    static void dispose_from_game();
    static void destroy(long volume_index);
    static void frame_advance_all(float elapsed_time);
    static __forceinline c_light_volume_gpu *get(long index);
    void render(const c_light_volume_states *states) const;
    void render(const s_effect_parameters *parameters) const;
    long m_identifier;
    long m_effect_index;
    long m_first_row_index;
    unsigned short m_row_count;
    unsigned short m_flags;
    struct s_row {
        long identifier;
        long previous;
        long next;
        static __forceinline s_row *get(long row_index);
        static void destroy(long row_index, bool destroy_chain);
    };
    struct s_render_vertex_shader_constant_list
    {
        enum { slot_count = 2 };
        static const bool x_in_place[slot_count];
        static const int x_keys[slot_count];
    };
    struct s_update_vertex_shader_constant_list
    {
        enum { slot_count = 7 };
        static const bool x_in_place[slot_count];
        static const int x_keys[slot_count];
    };
    struct s_layout;
    static s_layout x_layout;
private:
    template<class ConstantList> void set_shader_state(const s_gpu_constant_table<ConstantList> *table,
        const c_light_volume_states *states, long profile_count) const;
    template<class ConstantList> void set_shader_functions(const s_gpu_constant_table<ConstantList> *table) const;
    template<class ConstantList> void set_shader_strip(const s_gpu_constant_table<ConstantList> *table, long value) const;
    template<class ConstantList> void set_shader_strip(const s_gpu_constant_table<ConstantList> *table) const;
    void frame_advance(c_light_volume_states *states);
    static void initialize_shaders();
};


// The vertex layout of one GPU light volume profile: three elements behind the layout's element table.
struct c_light_volume_gpu::s_layout : s_gpu_target_table<3>
{
    s_layout();

    s_gpu_storage_float4 position;
    s_gpu_storage_half2 size;
    s_gpu_storage_color color;
};

// The render layout of one GPU light volume: four elements behind the layout's element table.
struct s_light_volume_gpu_target_table : s_gpu_target_table<4>
{
    s_light_volume_gpu_target_table();

    s_gpu_storage_float4 position;
    s_gpu_storage_ushort4n unused_ushort4n;
    s_gpu_storage_half4 unused_half4;
    s_gpu_storage_color color;
};

typedef char light_volume_gpu_size[(sizeof(c_light_volume_gpu) == 0x10) ? 1 : -1];

#endif
