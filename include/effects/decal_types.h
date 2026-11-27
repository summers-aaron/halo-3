#ifndef EFFECTS_DECAL_TYPES_H
#define EFFECTS_DECAL_TYPES_H

#include "math/real_math_types.h"
#include "rasterizer/rasterizer_vertex_buffer.h"
#include "rasterizer/rasterizer_index_buffer.h"
#include "memory/lruv_cache.h"
#include "effects/decals.h"

template<typename t_type, long k_count> class c_static_sized_dynamic_array;
struct s_shader_extern_info;
struct s_decal_collision_result;
extern "C" bool decals_build_decal_mesh(c_decal *, unsigned char, const c_static_sized_dynamic_array<s_decal_collision_result, 16> *);

struct rasterizer_vertex_world
{
    real_point3d position;
    real_point2d texcoord;
    unsigned long normal, tangent, binormal;
};
class c_decal;
class c_decal {
public:
    class c_vertex_allocator;
    long m_datum_header;
    long m_definition_index;
    long m_system_index;
    long m_next_decal_index;
    long m_first_cache_handle;
    long m_second_cache_handle;
    unsigned short m_first_cache_count;
    unsigned short m_second_cache_count;
    unsigned long m_flags;
    float m_age;
    float m_lifespan;
    float m_fade_time;
    union {
        struct { float m_width; float m_radius; };
        real_point2d m_half_size;
    };
    real_point2d m_texture_origin;
    real_point2d m_texture_size;
    rasterizer_vertex_world m_vertices[4];
    class c_index_allocator;
private:
    c_vertex_allocator *get_vertex_allocator() const;
    c_index_allocator *get_index_allocator() const;
    friend void ::decal_destroy(long);
    friend bool ::decals_build_decal_mesh(c_decal *, unsigned char, const c_static_sized_dynamic_array<s_decal_collision_result, 16> *);
    float get_fade() const;
    long get_vertex_start() const;
    long get_index_start() const;
    friend void ::decals_add_decal_to_render_list(const c_decal *);
    static const c_decal *x_currently_submitting_decal;
    __forceinline void set_alive(bool alive)
    {
        if (alive)
            m_flags |= 1;
        else
            m_flags &= ~1;
    }

public:
    static long create(unsigned char type, long system_index, long definition_index);
    static const s_shader_extern_info x_shader_extern_info;
    static float shader_function_callback(long object_index, long function_name, long definition_index);
    static __forceinline void initialize_for_game();
    static void dispose_from_game();
    void render() const;
    void choose_sprite();
    template<class t_buffer, class t_vertex> class c_mesh_allocator {
    public:
        c_mesh_allocator(unsigned long maximum_count) : m_maximum_count(maximum_count) {}
        virtual void create_buffer() = 0;
    protected:
        long m_maximum_count;
        t_buffer m_buffer;
        s_lruv_cache *m_cache;
        void *m_locked_data;
    public:
        __forceinline s_lruv_cache *get_cache() const { return m_cache; }
        __forceinline void *get_locked_data() const { return m_locked_data; }
        __forceinline void clear_cache() { m_cache = 0; m_locked_data = 0; }
        __forceinline t_buffer *get_buffer() { return &m_buffer; }
        void initialize_for_game();
        __forceinline void dispose_gpu_block(s_lruv_cache *block);
        __forceinline long new_elements(long count) { return lruv_block_new(m_cache, -1, count * sizeof(t_vertex), 0); }
        template<class, class> friend class c_mesh_allocator;
        friend class c_decal;
    protected:
        static void delete_block_proc(void *block, long size, bool purge) {}
        static bool locked_block_proc(void *block, long size) { return true; }
        static unsigned char usage_block_proc(void *block, long size);
    };
    // March decals.cpp: each allocator's create_buffer makes the D3D buffer its mesh allocator manages.
    class c_vertex_allocator : public c_mesh_allocator<c_rasterizer_vertex_buffer, rasterizer_vertex_world> {
    public:
        c_vertex_allocator(unsigned long maximum_count) : c_mesh_allocator<c_rasterizer_vertex_buffer, rasterizer_vertex_world>(maximum_count) {}
        virtual void create_buffer();
    };
    class c_index_allocator : public c_mesh_allocator<c_rasterizer_index_buffer, unsigned short> {
    public:
        c_index_allocator(unsigned long maximum_count) : c_mesh_allocator<c_rasterizer_index_buffer, unsigned short>(maximum_count) {}
        virtual void create_buffer();
    };
};

// Defined in tu/blamlibxbox/decals.cpp, once for each buffer the decal meshes use.
template<> void c_decal::c_mesh_allocator<c_rasterizer_vertex_buffer, rasterizer_vertex_world>::initialize_for_game();
template<> void c_decal::c_mesh_allocator<c_rasterizer_index_buffer, unsigned short>::initialize_for_game();

extern "C" {
extern c_decal::c_vertex_allocator g_decal_preplaced_vertex_allocator;
extern c_decal::c_index_allocator g_decal_preplaced_index_allocator;
extern c_decal::c_vertex_allocator g_decal_runtime_vertex_allocator;
extern c_decal::c_index_allocator g_decal_runtime_index_allocator;
}

#endif
