#ifndef GPU_PARTICLE_PARTICLE_BLOCK_H
#define GPU_PARTICLE_PARTICLE_BLOCK_H

// Functions defined in tu/graphicslibxbox/particle_block.cpp.

#include "cache/restricted_memory_regions.h"

void __tls_set_g_particle_emitter_gpu_data_array_allocator(void *object);
void __tls_set_g_particle_row_data_array_allocator(void *object);

template<long k_target_count> struct s_gpu_target_table;
int particle_block_element_allocate(int type, int owner);
struct particle_block_strip_owner;
struct s_gpu_constant_table_base;


extern "C" void particle_block_delete_element(long block_index, bool delete_following);
extern "C" long particle_block_new_contrail_element(unsigned char type, long instance_index);
extern "C" void particle_block_delete_contrail_block(long block_index, bool delete_following);
extern "C" void particle_block_set_contrail_link_array(void *address);
extern "C" void particle_block_initialize_contrail_shaders(void);
extern "C" void particle_block_frame_advance_all(float dt);
extern "C" void particle_block_add_vertex_declaration(s_gpu_target_table<1> *build, long row, void *owner);

class c_constant_table;
class c_particle_state_list;
class c_particle_emitter_gpu;
struct s_particle_emitter_gpu_block;
struct s_gpu_constant_table_base;

extern "C" void particle_block_set_emitter_args(s_particle_emitter_gpu_block *object, c_constant_table *table);
extern "C" void particle_block_update_entry_bounds(void *value);

template<>
void *t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &particle_block_update_entry_bounds>::reserve_memory(const char *name, const char *type, unsigned int size,
    long alignment_bits);

extern "C" {
extern t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &particle_block_update_entry_bounds> g_light_volume_emitter_allocation;
}
extern "C" void particle_block_initialize_spawn_slot_list(void);

#include "gpu_particle/shader_slot_types.h"
#include "gpu_particle/particle_emitter_gpu.h"
extern "C" void particle_block_bind_render_slot_set(
    s_gpu_global_shader<c_particle_emitter_gpu::s_render_vertex_shader_constant_list> *slot_set, s_entry_table *table);
#endif // GPU_PARTICLE_PARTICLE_BLOCK_H
