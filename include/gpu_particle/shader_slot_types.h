#ifndef GPU_PARTICLE_SHADER_SLOT_TYPES_H
#define GPU_PARTICLE_SHADER_SLOT_TYPES_H

#include "gpu_particle/constant_entry_types.h"
#include "gpu_particle/constant_table_types.h"
#include "gpu_particle/particle_block_contrail_vertex_shader_constant_list_types.h"

struct s_tag_reference;
class c_rasterizer_constant_table_ref;

static __forceinline int gpu_constant_table_find_entry(const s_entry_table *table, int key)
{
    for (int entry_index = 0; entry_index < table->count; entry_index++) {
        if (key == table->entries[entry_index].key)
            return entry_index;
    }
    return -1;
}

struct s_slot
{
    s_constant_entry entry;
    bool required;
};

template<int SlotCount>
struct c_slot_set
{
    s_entry_table *table;
    s_slot *slots_base;
    int slot_count;
    unsigned char any_required;
    s_slot slots[SlotCount];
};

template<typename ConstantList>
struct s_gpu_shader : c_slot_set<ConstantList::slot_count>
{
#if !defined(_XBOX) || defined(__clang__)
    // Brace-initializes the slot set of a statically defined shader.
    s_gpu_shader() = default;
    constexpr s_gpu_shader(const c_slot_set<ConstantList::slot_count> &slot_set) : c_slot_set<ConstantList::slot_count>(slot_set) {}
#endif
    void initialize(c_rasterizer_constant_table_ref table_ref);
};

template<>
struct s_gpu_shader<s_particle_block_contrail_vertex_shader_constant_list>
    : s_gpu_constant_table<s_particle_block_contrail_vertex_shader_constant_list>
{
#if !defined(_XBOX) || defined(__clang__)
    s_gpu_shader() = default;
    constexpr s_gpu_shader(const s_gpu_constant_table<s_particle_block_contrail_vertex_shader_constant_list> &constant_table)
        : s_gpu_constant_table<s_particle_block_contrail_vertex_shader_constant_list>(constant_table) {}
#endif
    void initialize(c_rasterizer_constant_table_ref table_ref);
};

template<typename ConstantList>
struct s_gpu_global_shader : s_gpu_constant_table<ConstantList>
{
#if !defined(_XBOX) || defined(__clang__)
    s_gpu_global_shader() = default;
    constexpr s_gpu_global_shader(const s_gpu_constant_table<ConstantList> &constant_table)
        : s_gpu_constant_table<ConstantList>(constant_table) {}
#endif
    void initialize(c_rasterizer_constant_table_ref table_ref);
};

template<class ConstantList>
struct s_gpu_pixel_shader
{
    long shader_index;
    s_tag_reference *shader_reference;
    c_slot_set<1> constants;

    void initialize();
    __forceinline c_rasterizer_constant_table_ref initialize_constant_table();
};

struct s_gpu_empty_constant_list;
template<> void s_gpu_pixel_shader<s_gpu_empty_constant_list>::initialize();

#endif // GPU_PARTICLE_SHADER_SLOT_TYPES_H
