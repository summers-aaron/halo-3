// graphicslibxbox/contrail_gpu.obj: from the records 0x14e2f8 0x14e468 0x14e5a0 0x14ffa8 0x45fee0 0x460f78 0x460fe0 0x460ff8 0x461058 0x461070 0x461120 0x461308 0x4614b0 0x461578 0x4616a0 0x461ca8 0x461f98 0x462038 0x462058 0x4620f8 0x462118 0x4621a0 0x4621b8 0x462350 0x4623e0 0x4624e8 0x462568 0x462640 0x462718 0x462b20 0x462c68 0x462f90 0x4632b0 0x4639d8 0x463a38 0x463ba0 0x463d58 0x463e18 0x463f88 0x4641b8 0x70a258 0x70a268 0x70a278 0x70a2d0 0x70a2e0 0x70a2f0 0x70c9d8 0x70c9f0 0x70ca08 0x70ca20 0x70ca38
#include <xtl.h>
// @address 0x8c43a8 g_effect_budget_limits
// @address 0x737664 g_contrail_export_layout
// @address 0x11df630 g_restricted_address
// @address 0x11df790 g_particle_instance_owner_data
// @address 0x11df7e4 g_contrail_chain_data
// @address 0x11df7f4 g_particle_instance_location_data
// @address 0x11df810 g_contrail_data_array
// @address 0x11df814 g_particle_instance_data
#include "effects/effect_runtime_storage.h"
#include "rasterizer/rasterizer_implicit_geometry.h"
#include "gpu_particle/shared_gpu_storage.h"
#include "gpu_particle/contrail_chain_types.h"
#include "gpu_particle/constant_table.h"
// @address 0x11df89c g_contrail_gpu_buffer_address
// @address 0x11df834 g_contrail_gpu_link_data
#include "effects/particle_array_globals.h"
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "rasterizer/rasterizer_shader_gpr_globals.h"
#include "rasterizer/xenon/rasterizer_xenon_device.h"
#include "rasterizer/rasterizer_vertex_buffer.h"
#include "effects/effect_array_globals.h"
#include "gpu_particle/gpu_particle_globals.h"
#include "memory/data_types.h"
#include "gpu_particle/contrail_update_shader_pair.h"
#include "gpu_particle/gpu_constant_table_types.h"
#include "tag_files/tag_groups.h"
#include "game/game_globals.h"
#include "math/real_math_types.h"
#include "gpu_particle/contrail_vertex_declarations.h"
#include "gpu_particle/contrail_gpu_globals.h"
#include "effects/light_volume_types.h"
#include "effects/particle_instance_types.h"
#include "effects/effect_gpu_states.h"

// @address 0x11df804 g_contrail_gpu_data
__declspec(thread) s_data_array *g_contrail_gpu_data;
// @address 0x11df820 g_contrail_gpu_row_data
__declspec(thread) s_data_array *g_contrail_gpu_row_data;
__declspec(thread) s_data_array *g_contrail_gpu_link_data;
extern "C" {
__declspec(thread) void *g_contrail_gpu_buffer_address;
}

// @address 0x1ec830 ?data_set_new_base_address@@YAXPAPAUs_data_array@@PAU1@@Z
// @address 0x1d6bb0 ?restricted_region_free_member@@YAXJJ@Z
// @address 0x463810 ?__tls_set_g_animation_threads_allocator@@YAXPAX@Z
// @address 0x460210 ?initialize@?$s_gpu_pixel_shader@Us_gpu_empty_constant_list@@@@QAAXXZ
// @address 0x4623e0 ?initialize@?$s_gpu_vertex_shader@Us_spawn_vertex_shader_constant_list@c_contrail_gpu@@@@QAAXXZ
// @address 0x462568 ?initialize@?$s_gpu_global_shader@Us_render_vertex_shader_constant_list@c_contrail_gpu@@@@QAAXVc_rasterizer_constant_table_ref@@@Z
// @address 0x462640 ?initialize@?$s_gpu_shader@Us_update_vertex_shader_constant_list@c_contrail_gpu@@@@QAAXVc_rasterizer_constant_table_ref@@@Z
// @address 0x463ba0 ?frame_advance_all@c_contrail_gpu@@SAXM@Z
// @address 0x14cec0 ?set_vertex_stream@c_rasterizer@@SA_NPBVc_rasterizer_vertex_buffer@@@Z
// @address 0x14d118 ?set_vertex_shader@c_rasterizer@@SA_NPBVc_rasterizer_vertex_shader@@W4e_vertex_type@@W4e_transfer_vector_vertex_types@@W4e_entry_point@@@Z
// @address 0x14d4b8 ?set_pixel_shader@c_rasterizer@@SA_NPBVc_rasterizer_pixel_shader@@W4e_entry_point@@@Z
// @address 0x14e9b8 ?draw_primitive@c_rasterizer@@SAXW4e_primitive_type@c_rasterizer_index_buffer@@JJ@Z
#include <stdlib.h>
#include "memory/tls_globals.h"
#include "multithreading/thread_tls.h"
#include "rasterizer/rasterizer.h"
#include "game/game_time_types.h"
#include "cache/restricted_memory_regions.h"
#include "cutscene/recorded_animations.h"
#include "render_methods/render_method_submit.h"
#include "gpu_particle/contrail_gpu_types.h"
#include "gpu_particle/contrail_gpu.h"
#include "gpu_particle/particle_block.h"
#include "memory/data.h"
#include "effects/contrail_states.h"
#include "cache/restricted_memory_layout.h"
#include "effects/contrail_states_types.h"
#include "gpu_particle/gpu_empty_constant_list_types.h"
#include "cache/restricted_memory.h"
#if defined(_XBOX) && !defined(__clang__)
#include "gpu_particle/contrail_layout_types.h"
#else
#include "saved_games/game_state_procs_externs.h"
#endif
#include "gpu_particle/particle_block_contrail_vertex_shader_constant_list_types.h"
#include "effects/effect_contrail_location_types.h"
#include "rasterizer/rasterizer_global_definitions.h"
#include "gpu_particle/desc_types.h"
#include "gpu_particle/effect_parameters_types.h"
#include "gpu_particle/shader_info_types.h"
#include "gpu_particle/constant_entry_types.h"
#include "gpu_particle/effect_entry_types.h"
#include "rasterizer/rasterizer_resource_definitions.h"
#include "cache/cache_files.h"
#include "scenario/scenario.h"
#include "sky_atm/atmosphere.h"
#include "effects/light_volume_gpu_types.h"
#include "gpu_particle/contrail_target_bounds_types.h"
#include "gpu_particle/particle_block_bounds_types.h"
#include "gpu_particle/contrail_target_entry.h"
#include "rasterizer/c_rasterizer_constant_table_ref.h"
#include "rasterizer/rasterizer_vertex_buffer.h"
#include "gpu_particle/constant_table_types.h"
#include "effects/contrail_profile.h"
struct s_globals_definition;
#include "gpu_particle/spawn_shader_pair.h"
#include "rasterizer/rasterizer_shader_definitions.h"
#include "gpu_particle/vertex_shader.h"
#include "render_methods/render_method_shader_definition_types.h"
#include "rasterizer/rasterizer_shader_tag_types.h"
#include "gpu_particle/global_vertex_shader.h"
// @address 0x7be9b8 g_contrail_spawn_shaders
#if !defined(_XBOX) || defined(__clang__)
extern "C" {
s_contrail_spawn_shader_pair g_contrail_spawn_shaders =
{
    { 43, 0, { { 0, g_contrail_spawn_shaders.vertex_shader.constants.slots, 1, 0, {} } }, 11 },
    { 43, 0, { 0, g_contrail_spawn_shaders.pixel_shader.constants.slots, 1, 0, {} } }
};
}
#endif
// @address 0x7bea08 g_contrail_update_shaders
// @address 0x7bea84 g_contrail_render_vertex_shader
#if !defined(_XBOX) || defined(__clang__)
extern "C" {
s_gpu_update_shader_pair<c_contrail_gpu::s_update_vertex_shader_constant_list> g_contrail_update_shaders =
{
    44, 0, { { 0, g_contrail_update_shaders.constants.slots, 5, 0, {} } }, 11,
    { 44, 0, { 0, g_contrail_update_shaders.pixel.constants.slots, 1, 0, {} } }
};
s_gpu_global_vertex_shader<c_contrail_gpu::s_render_vertex_shader_constant_list> g_contrail_render_vertex_shader =
{
    0, { { 0, g_contrail_render_vertex_shader.constants.slots, 2, 0, {} } }, 11
};
}
#endif
// @address 0x24c70 ?x_in_place@s_update_vertex_shader_constant_list@c_contrail_gpu@@2QB_NB
// @address 0x24c78 ?x_in_place@s_render_vertex_shader_constant_list@c_contrail_gpu@@2QB_NB
// @address 0x24c80 ?x_keys@s_update_vertex_shader_constant_list@c_contrail_gpu@@2QBHB
// @address 0x24c94 ?x_keys@s_render_vertex_shader_constant_list@c_contrail_gpu@@2QBHB
#if !defined(_XBOX) || defined(__clang__)
const bool c_contrail_gpu::s_update_vertex_shader_constant_list::x_in_place[slot_count] = { false, false, true, true, true };
const bool c_contrail_gpu::s_render_vertex_shader_constant_list::x_in_place[slot_count] = { false, false };
const int c_contrail_gpu::s_update_vertex_shader_constant_list::x_keys[slot_count] = { 0x80000, 0x80001, 0x80002, 0x80003, 0x80004 };
const int c_contrail_gpu::s_render_vertex_shader_constant_list::x_keys[slot_count] = { 0x80001, 0x8000b };
#endif
// (the thread's row data array: 20-byte rows indexed by a datum handle)
#include "gpu_particle/contrail_shader_globals.h"

// (dynamic vertex buffers: refresh both buffers of each of two slots)
#include "gpu_particle/gpu_buffer.h"
#include "gpu_particle/contrail_queue_system.h"
#include "gpu_particle/gpu_storage_inline.h"
#include "gpu_particle/particle_block_queue_types.h"
#include "gpu_particle/contrail_shader_initializers.h"
#include "cache/restricted_memory_layout.h"
#include <string.h>
__declspec(align(16)) struct s_contrail_strip_entry {
    float value;
};
#include "effects/contrail.h"
#include "effects/contrail_location.h"
#include "effects/contrail_system.h"
#include "effects/contrail_definition_types.h"
struct s_contrail_gpu_overall_state {
    union { float first_flag; float first_flag_register[4]; };
    union { float second_flag; float second_flag_register[4]; };
    union { float profile_type; float profile_type_register[4]; };
    union { float profile_count; float profile_count_register[4]; };
    union { real_vector2d uv_tiling_rate; float uv_tiling_rate_register[4]; };
    union { real_vector2d uv_scroll_rate; float uv_scroll_rate_register[4]; };
    union { real_vector2d initial; float initial_register[4]; };
    union { float time; float time_register[4]; };
    union { real_point3d origin; float origin_register[4]; };
    float origin_fade_scale;
    float origin_fade_cutoff;
    float edge_fade_scale;
    float edge_fade_cutoff;
    float states[16][4];
};
typedef char contrail_gpu_overall_state_size[(sizeof(s_contrail_gpu_overall_state) == 0x1a0) ? 1 : -1];
#include "gpu_particle/common_gpu.h"
#include "effects/effect_budget_limits.h"
#include "gpu_particle/effect_constants_types.h"
struct contrail_gpu_tag_definition {
    c_typed_tag_block<s_effect_entry> entries;
};
#include "rasterizer/rasterizer_vertex_definitions.h"
#include "render_methods/render_method_definitions.h"
#include "gpu_particle/particle_block_contrail_shader_pair.h"
#include "sky_atm/atmosphere_fog_interface.h"
#include "render_methods/render_method_data.h"
struct s_color_pack_constants {
    __vector4 scale;
    __vector4 alpha_scale;
    __vector4 offset;
};

// Packed queue values: the pack intrinsics return the bits in a vector register.
union u_gpu_queue_bits { __vector4 vector; unsigned __int64 i64; unsigned long i32; };

static __forceinline c_contrail_gpu::s_row *contrail_gpu_row_get(long row_index)
{
    return &((c_contrail_gpu::s_row *)((s_data_array *)g_contrail_gpu_row_data)->data)[(unsigned short)row_index];
}
// @address 0x4614b0 ?initialize_shaders@c_contrail_gpu@@CAXXZ
void c_contrail_gpu::initialize_shaders()
{
    g_contrail_spawn_shaders.vertex_shader.initialize();
    g_contrail_spawn_shaders.pixel_shader.initialize();
    s_game_globals *game_globals = global_game_globals;
    c_rasterizer_globals *rasterizer_globals;
    if (game_globals->rasterizer_globals_ref.index != -1)
        rasterizer_globals = static_cast<c_rasterizer_globals *>(tag_get(0x72617367, game_globals->rasterizer_globals_ref.index));
    else
        rasterizer_globals = 0;
    g_contrail_update_shaders.shader_reference = &rasterizer_globals->get_explicit_shaders(g_contrail_update_shaders.shader_index)->vertex;
    const c_rasterizer_vertex_shader *vertex_shader = static_cast<const c_rasterizer_vertex_shader *>(tag_get(0x76747368,
        g_contrail_update_shaders.shader_reference->index));
    c_rasterizer_compiled_vertex_shader *compiled_shader = const_cast<c_rasterizer_compiled_vertex_shader *>(
        vertex_shader->get_compiled_shader((e_vertex_type)g_contrail_update_shaders.vertex_type, _entry_point_default, 0));
    c_rasterizer_constant_table_ref table_ref;
    table_ref.table = compiled_shader ? compiled_shader->get_constant_table() : 0;
    g_contrail_update_shaders.constants.initialize(table_ref);
    g_contrail_update_shaders.pixel.initialize();
}
template<>
void s_gpu_vertex_shader<c_contrail_gpu::s_spawn_vertex_shader_constant_list>::initialize()
{
    // match: 0x4623e0: this-aliasing assumption folds member accesses to the spawn-shader global
    __assume(this == &g_contrail_spawn_shaders.vertex_shader);
    s_game_globals *game_globals = global_game_globals;
    c_rasterizer_globals *rasterizer_globals;
    if (game_globals->rasterizer_globals_ref.index != -1)
        rasterizer_globals = static_cast<c_rasterizer_globals *>(tag_get(0x72617367, game_globals->rasterizer_globals_ref.index));
    else
        rasterizer_globals = 0;
    shader_reference = &rasterizer_globals->get_explicit_shaders(shader_index)->vertex;
    const c_rasterizer_vertex_shader *vertex_shader = static_cast<const c_rasterizer_vertex_shader *>(tag_get(0x76747368, shader_reference->index));
    c_rasterizer_compiled_vertex_shader *compiled_shader = const_cast<c_rasterizer_compiled_vertex_shader *>(
        vertex_shader->get_compiled_shader((e_vertex_type)vertex_type, _entry_point_default, 0));
    c_rasterizer_constant_table_ref table_ref;
    table_ref.table = compiled_shader ? compiled_shader->get_constant_table() : 0;
    constants.table = table_ref.table;
    int entry_index = gpu_constant_table_find_entry(constants.table, 0x80000);
    if (entry_index != -1)
        constants.slots[0].entry = constants.table->entries[entry_index];
    constants.slots[0].required = false;
    constants.any_required = constants.any_required || constants.slots[0].required;
}
// @address 0x4624e8 ?initialize_constant_table@?$s_gpu_global_vertex_shader@Us_render_vertex_shader_constant_list@c_contrail_gpu@@@@AAAPAUs_entry_table@@XZ
template<>
s_entry_table *s_gpu_global_vertex_shader<c_contrail_gpu::s_render_vertex_shader_constant_list>::initialize_constant_table()
{
    // match: 0x4624e8: this-aliasing assumption folds member accesses to the render-shader global
    __assume(this == &g_contrail_render_vertex_shader);
    long pass_index = this->vertex_type;
    const s_tag_reference *source = this->shader_reference;
    s_render_method_shader_definition *render_method =
        (s_render_method_shader_definition *)((char *)global_tag_instances[(unsigned short)source->index].definition);
    s_rasterizer_global_vertex_shader_definition *shader =
        (s_rasterizer_global_vertex_shader_definition *)((char *)global_tag_instances[(unsigned short)render_method->shared_vertex_shader.index].definition);
    s_tag_block *passes = (s_tag_block *)shader->vertex_types.address;
    s_tag_block *pass = &passes[pass_index];
    long constant_index = ((s_global_vertex_shader_entry_point *)pass->address)->constant_index;
    c_rasterizer_compiled_vertex_shader *constants = (c_rasterizer_compiled_vertex_shader *)shader->shaders.address;
    c_rasterizer_compiled_vertex_shader *constant = &constants[constant_index];
    return constant ? constant->get_constant_table() : 0;
}
template<>
void s_gpu_global_shader<c_contrail_gpu::s_render_vertex_shader_constant_list>::initialize(c_rasterizer_constant_table_ref table_ref)
{
    this->shader = table_ref.table;
    const int *key = c_contrail_gpu::s_render_vertex_shader_constant_list::x_keys;
    for (int slot_index = 0; slot_index < 2; slot_index++) {
        int entry_index = gpu_constant_table_find_entry(this->shader, *key);
        if (entry_index != -1)
            static_cast<s_constant_entry &>(slots[slot_index]) = this->shader->entries[entry_index];
        slots[slot_index].owned = c_contrail_gpu::s_render_vertex_shader_constant_list::x_in_place[slot_index];
        valid = valid || slots[slot_index].owned;
        key++;
    }
}
template<>
__forceinline void s_gpu_global_vertex_shader<c_contrail_gpu::s_render_vertex_shader_constant_list>::initialize(const s_tag_reference *reference)
{
    shader_reference = reference;
    c_rasterizer_constant_table_ref table_ref;
    table_ref.table = initialize_constant_table();
    constants.initialize(table_ref);
}
template<>
void s_gpu_shader<c_contrail_gpu::s_update_vertex_shader_constant_list>::initialize(c_rasterizer_constant_table_ref table_ref)
{
    this->table = table_ref.table;
    const int *key = c_contrail_gpu::s_update_vertex_shader_constant_list::x_keys;
    for (int slot_index = 0; slot_index < 5; slot_index++) {
        int entry_index = gpu_constant_table_find_entry(this->table, *key);
        if (entry_index != -1)
            slots[slot_index].entry = this->table->entries[entry_index];
        slots[slot_index].required = c_contrail_gpu::s_update_vertex_shader_constant_list::x_in_place[slot_index];
        any_required = any_required || slots[slot_index].required;
        key++;
    }
}

// (thread-local handles of the particle emitter GPU allocator family, field layout 2)

// @address 0x462038 ?deallocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_contrail_gpu_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<>
void t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_contrail_gpu_allocator>::deallocate(void *)
{
    g_contrail_gpu_data = 0;
    restricted_region_free_member(2, m_member_index);
}

// @address 0x4620f8 ?deallocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_contrail_row_gpu_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<>
void t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_contrail_row_gpu_allocator>::deallocate(void *)
{
    g_contrail_gpu_row_data = 0;
    restricted_region_free_member(2, m_member_index);
}
s_queued_gpu_buffer_system<c_contrail_gpu::s_layout, 128> c_contrail_gpu::x_queued_buffer_system;
// @address 0x70a2d0 ??__E?x_layout@c_contrail_gpu@@2Us_layout@1@A@@YAXXZ
c_contrail_gpu::s_layout c_contrail_gpu::x_layout;

static __forceinline void *contrail_gpu_lock_vertex_buffer(s_gpu_buffer *buffer)
{
    void *result = 0;
    if (c_rasterizer::get_device() && buffer->m_vertex_count) {
        D3DVertexBuffer *vertex_buffer = buffer->m_d3d_buffer;
        if (g_restricted_address[2]) {
            unsigned short count = buffer->m_vertex_count;
            result = D3DVertexBuffer_Lock(vertex_buffer, 0, buffer->m_vertex_stride * count, 0);
        }
        else {
            uintptr_t address = vertex_buffer->Format.dword[0] & ~3u;
            result = reinterpret_cast<void *>(address);
        }
    }
    return result;
}
static __forceinline void contrail_gpu_unlock_vertex_buffer(s_gpu_buffer *buffer)
{
    if (c_rasterizer::get_device() && buffer->m_vertex_count && g_restricted_address[2])
        D3DVertexBuffer_Unlock(buffer->m_d3d_buffer);
}
static __forceinline void contrail_gpu_refresh_vertex_buffer(s_gpu_buffer *buffer)
{
    buffer->create((c_rasterizer_vertex_buffer::e_vertex_buffer_declaration)buffer->format,
        buffer->element_size * buffer->element_count);
    buffer->address = contrail_gpu_lock_vertex_buffer(buffer);
    contrail_gpu_unlock_vertex_buffer(buffer);
}
// @address 0x4621b8 contrail_gpu_refresh_vertex_buffers
extern "C" void contrail_gpu_refresh_vertex_buffers(void)
{
    for (long i = 0; i < 2; i++) {
        contrail_gpu_refresh_vertex_buffer(&c_contrail_gpu::x_queued_buffer_system.frames[i].first);
        contrail_gpu_refresh_vertex_buffer(&c_contrail_gpu::x_queued_buffer_system.frames[i].second);
    }
}

// @address 0x461f98 ?allocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_contrail_gpu_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<>
void *t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_contrail_gpu_allocator>::allocate(unsigned int size, const char *name)
{
    int member_index = g_restricted_regions[2].add_member(name, "", size, 0,
        (field_proc_t)__tls_set_g_contrail_gpu_allocator, 0, 0);
    unsigned int offset = g_restricted_regions[2].get_member_offset(member_index);
    data_set_new_base_address((s_data_array **)&g_contrail_gpu_data, (s_data_array *)(g_restricted_address[2] + offset));
    m_member_index = member_index;
    m_thread_id = get_current_thread_index();
    return g_restricted_address[2] + g_restricted_regions[2].get_member_offset(member_index);
}

// @address 0x462058 ?allocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_contrail_row_gpu_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<>
void *t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_contrail_row_gpu_allocator>::allocate(unsigned int size, const char *name)
{
    int member_index = g_restricted_regions[2].add_member(name, "", size, 0,
        (field_proc_t)__tls_set_g_contrail_row_gpu_allocator, 0, 0);
    unsigned int offset = g_restricted_regions[2].get_member_offset(member_index);
    data_set_new_base_address((s_data_array **)&g_contrail_gpu_row_data, (s_data_array *)(g_restricted_address[2] + offset));
    m_member_index = member_index;
    m_thread_id = get_current_thread_index();
    return g_restricted_address[2] + g_restricted_regions[2].get_member_offset(member_index);
}

// (render-target vertex layout: three entries side by side in one vertex)

#define CONTRAIL_GPU_MAX(a, b) ((a) > (b) ? (a) : (b))

static __forceinline bool contrail_gpu_declaration_full(const s_declaration_element *end, long count)
{
    return 64 - count < 64 && end[-1].Stream >= 1;
}

// @address 0x462718 contrail_gpu_initialize_targets
void contrail_gpu_initialize_targets(s_gpu_target_table<8> *table, long unused, void *owner)
{
    unsigned long largest = 0;
    unsigned long total = 0;
    for (unsigned long i = 0; i < 8; i++) {
        s_gpu_storage_interface *entry = table->entries[i];
        total += entry->size();
        largest = CONTRAIL_GPU_MAX(largest, entry->size());
    }
    unsigned long stride = ((total + largest - 1) / largest) * largest;
    unsigned long offset = 0;
    for (unsigned long i = 0; i < 8; i++) {
        s_gpu_storage_interface *entry = table->entries[i];
        entry->owner = owner;
        entry->layout(stride, offset);
        offset += entry->size();
        real_point2d size;
        size.x = (float)entry->stride;
        size.y = (float)entry->offset;
        table->bounds[i].size = size;
    }
    unsigned long padding = stride - offset;
    D3DDECLTYPE padding_type;
    unsigned char padding_kind;
    switch (padding) {
    case 3 * sizeof(float):
        padding_type = D3DDECLTYPE_FLOAT3;
        padding_kind = 2;
        break;
    case 2 * sizeof(float):
        padding_type = D3DDECLTYPE_FLOAT2;
        padding_kind = 1;
        break;
    case sizeof(float):
        padding_type = D3DDECLTYPE_FLOAT1;
        padding_kind = 0;
        break;
    case 0:
        padding_kind = 18;
        padding_type = D3DDECLTYPE_UNUSED;
        break;
    default:
        // match: 0x462718: keeps padding_type/padding_kind in registers; without it both spill to stack slots and the frame grows 0xd0 to 0xe0
        __assume(0);
    }

    rasterizer_vertex_declaration *vertex_declaration = &g_rasterizer_vertex_buffer_declarations[18];
    vertex_declaration->index = 18;
    for (unsigned long i = 0; i < 8; i++) {
        s_gpu_storage_interface *entry = table->entries[i];
        rasterizer_vertex_register_mapping *register_mapping = &vertex_declaration->register_mappings[i];
        register_mapping->attribute = (signed char)entry->semantic;
        register_mapping->format = entry->kind();
        register_mapping->usage = (unsigned char)k_vertex_semantic_usages[entry->semantic];
        register_mapping->usage_index = (unsigned char)entry->usage_index;
    }
    long count = 8;
    if (padding != 0) {
        vertex_declaration->register_mappings[8].attribute = 0;
        vertex_declaration->register_mappings[8].format = padding_kind;
        vertex_declaration->register_mappings[8].usage = 0;
        vertex_declaration->register_mappings[8].usage_index = 15;
        count = 9;
    }
    vertex_declaration->register_mappings[count].attribute = -1;

    long declaration_index = 11;
    s_declaration_element *end = c_vertex_declaration_table::base_vertex_decls[declaration_index];
    while (memcmp(end, &k_vertex_declaration_terminator, sizeof(s_declaration_element)) != 0)
        end++;
    if (!contrail_gpu_declaration_full(end, end - c_vertex_declaration_table::base_vertex_decls[declaration_index])) {
        for (unsigned long i = 0; i < 8; i++) {
            s_gpu_storage_interface *entry = table->entries[i];
            s_declaration_element *element = &end[i];
            element->Stream = 1;
            element->Offset = (unsigned short)(entry->size() * entry->offset);
            element->Type = entry->declaration_type();
            element->Method = D3DDECLMETHOD_DEFAULT;
            element->Usage = (unsigned char)k_vertex_semantic_usages[entry->semantic];
            element->UsageIndex = (unsigned char)entry->usage_index;
        }
        long element_count = 8;
        if (padding != 0) {
            element_count = 9;
            end[8].Stream = 1;
            end[8].Offset = (unsigned short)offset;
            end[8].Type = padding_type;
            end[8].Method = D3DDECLMETHOD_DEFAULT;
            end[8].Usage = 0;
            end[8].UsageIndex = 15;
        }
        memcpy(&end[element_count], &k_vertex_declaration_terminator, sizeof(s_declaration_element));
    }
}

namespace {
static __forceinline c_contrail_gpu::s_row *contrail_gpu_strip_datum_try_and_get(s_data_array *data, long index)
{
    if (index == -1)
        return 0;
    long absolute_index = (unsigned short)index;
    if (absolute_index >= data->first_unallocated)
        return 0;
    c_contrail_gpu::s_row *datum = (c_contrail_gpu::s_row *)(data->data + data->size * absolute_index);
    if (datum->identifier == 0 || datum->identifier != (index >> 16))
        return 0;
    return datum;
}
}
// 0x462f90: loads the render vertex shader constants of one contrail: its overall state, one 16-byte register each
static __forceinline const c_contrail *contrail_gpu_get(long index)
{
    return &((c_contrail *)((s_data_array *)g_contrail_data_array)->data)
        [DATUM_INDEX_TO_ABSOLUTE_INDEX(index)];
}
static __forceinline const c_contrail_location *contrail_location_get(const c_contrail *contrail)
{
    return &((c_contrail_location *)((s_data_array *)g_contrail_location_data)->data)
        [DATUM_INDEX_TO_ABSOLUTE_INDEX(contrail->location_index)];
}
static __forceinline const c_contrail_definition *get_contrail_definition(const c_contrail *contrail)
{
    const c_contrail_location *location = datum_get<c_contrail_location>(g_contrail_location_data, contrail->location_index);
    const c_contrail_system *system = datum_get<c_contrail_system>(g_contrail_system_data, location->system_index);
    const s_contrail_system_definition *system_definition = (const s_contrail_system_definition *)tag_get(0, system->definition_index);
    return &system_definition->contrails[contrail->definition_index];
}
// @address 0x462f90 ??$set_shader_state@Us_render_vertex_shader_constant_list@c_contrail_gpu@@@c_contrail_gpu@@ABAXPBU?$s_gpu_constant_table@Us_render_vertex_shader_constant_list@c_contrail_gpu@@@@PBVc_contrail_states@@@Z
template<>
void c_contrail_gpu::set_shader_state<c_contrail_gpu::s_render_vertex_shader_constant_list>(
    const s_gpu_constant_table<c_contrail_gpu::s_render_vertex_shader_constant_list> *table, const c_contrail_states *contrail_states) const
{
    const c_contrail *contrail = datum_get<c_contrail>(g_contrail_data_array, contrail_index);
    const c_contrail_definition *definition = get_contrail_definition(contrail);
    const c_contrail_location *location = datum_get<c_contrail_location>(g_contrail_location_data, contrail->location_index);
    s_contrail_gpu_overall_state state;
    state.first_flag = (float)definition->first_flag;
    state.second_flag = (float)definition->second_flag;
    state.profile_type = (float)definition->profile_type;
    state.profile_count = (float)profile_count;
    state.uv_tiling_rate = definition->uv_tiling_rate;
    state.uv_scroll_rate = definition->uv_scroll_rate;
    state.initial = contrail->initial;
    state.time = (float)tls_game_time_globals()->time * tls_game_time_globals()->tick_length;
    state.origin = location->last_position;
    state.origin_fade_scale = 1.0f / definition->origin_fade_range;
    state.origin_fade_cutoff = definition->origin_fade_cutoff;
    state.edge_fade_scale = 1.0f / (definition->edge_fade_range * 0.017453292f);
    state.edge_fade_cutoff = definition->edge_fade_cutoff * 0.017453292f;
    state.states[0][0] = 0.0f;
    memset(&state.states[1], 0, sizeof(state.states) - sizeof(state.states[0]));
    for (long state_index = 0; state_index < 16; state_index++) {
        if (((1 << state_index) & definition->used_states & 0xfec0) != 0)
            state.states[state_index][0] = contrail_states->values[state_index];
    }
    ((c_rasterizer_constant_table_ref const *)(s_gpu_constant_table_base *)table)
        ->set_constants(table->descs, (void const *)(&state));
}
// @address 0x462c68 ??$set_shader_state@Us_update_vertex_shader_constant_list@c_contrail_gpu@@@c_contrail_gpu@@ABAXPBU?$s_gpu_constant_table@Us_update_vertex_shader_constant_list@c_contrail_gpu@@@@PBVc_contrail_states@@@Z
template<>
void c_contrail_gpu::set_shader_state<c_contrail_gpu::s_update_vertex_shader_constant_list>(
    const s_gpu_constant_table<c_contrail_gpu::s_update_vertex_shader_constant_list> *table, const c_contrail_states *contrail_states) const
{
    const c_contrail *contrail = datum_get<c_contrail>(g_contrail_data_array, contrail_index);
    const c_contrail_definition *definition = get_contrail_definition(contrail);
    const c_contrail_location *location = datum_get<c_contrail_location>(g_contrail_location_data, contrail->location_index);
    s_contrail_gpu_overall_state state;
    state.first_flag = (float)definition->first_flag;
    state.second_flag = (float)definition->second_flag;
    state.profile_type = (float)definition->profile_type;
    state.profile_count = (float)profile_count;
    state.uv_tiling_rate = definition->uv_tiling_rate;
    state.uv_scroll_rate = definition->uv_scroll_rate;
    state.initial = contrail->initial;
    state.time = (float)tls_game_time_globals()->time * tls_game_time_globals()->tick_length;
    state.origin = location->last_position;
    state.origin_fade_scale = 1.0f / definition->origin_fade_range;
    state.origin_fade_cutoff = definition->origin_fade_cutoff;
    state.edge_fade_scale = 1.0f / (definition->edge_fade_range * 0.017453292f);
    state.edge_fade_cutoff = definition->edge_fade_cutoff * 0.017453292f;
    state.states[0][0] = 0.0f;
    memset(&state.states[1], 0, sizeof(state.states) - sizeof(state.states[0]));
    for (long state_index = 0; state_index < 16; state_index++) {
        if (((1 << state_index) & definition->used_states & 0xfec0) != 0)
            state.states[state_index][0] = contrail_states->values[state_index];
    }
    ((c_rasterizer_constant_table_ref const *)(s_gpu_constant_table_base *)table)
        ->set_constants((c_rasterizer_constant_desc const *)(&table->descs[1]), (void const *)(&state));
}
// gathers up to 8 rows of the profile chain into strip entries, newest row last
// @address 0x4632b0 ??$set_shader_strip@Us_render_vertex_shader_constant_list@c_contrail_gpu@@@c_contrail_gpu@@ABAXPBU?$s_gpu_constant_table@Us_render_vertex_shader_constant_list@c_contrail_gpu@@@@@Z
template<>
void c_contrail_gpu::set_shader_strip<c_contrail_gpu::s_render_vertex_shader_constant_list>(
    const s_gpu_constant_table<c_contrail_gpu::s_render_vertex_shader_constant_list> *table) const
{
    s_contrail_strip_entry entries[8] = { 0.0f };
    long count = 0;
    c_contrail_gpu::s_row *row;
    long index;
    for (index = first_row_index; (row = contrail_gpu_strip_datum_try_and_get((s_data_array *)g_contrail_gpu_row_data,
        index)) != 0 && count < 8; index = row->owner)
        count++;
    s_contrail_strip_entry *entry = &entries[count - 1];
    for (index = first_row_index; (row = contrail_gpu_strip_datum_try_and_get((s_data_array *)g_contrail_gpu_row_data, index)) != 0; index = row->owner) {
        entry->value = (float)row->index;
        entry--;
    }
    ((c_rasterizer_constant_table_ref const *)(s_gpu_constant_table_base *)table)
        ->set_constants((c_rasterizer_constant_desc const *)(&table->descs[1]), (void const *)(entries));
}
// the same refresh over the particle system's vertex buffer slots

// @address 0x45fee0 particle_block_refresh_vertex_buffers
extern "C" void particle_block_refresh_vertex_buffers(void *storage)
{
    for (long i = 0; i < 2; i++) {
        contrail_gpu_refresh_vertex_buffer(&g_particle_block_queue_system.frames[i].first);
        contrail_gpu_refresh_vertex_buffer(&g_particle_block_queue_system.frames[i].second);
    }
}

// 0x460f78: a member of the constant table: sets the constant run of one entry, with count scaled to vec4 units

// @address 0x460f78 ?set_scaled@c_constant_table@@QAAXJJPBX@Z
void c_constant_table::set_scaled(long index, long count, const void *data)
{
    c_rasterizer_constant_desc desc;
    c_rasterizer_constant_desc *entry = &entries[index];
    desc.register_range = entry->register_range;
    long key = entry->key;
    long ownership_data = entry->ownership_data;
    desc.count = (unsigned char)(count << 2);
    desc.key = key;
    desc.ownership_data = ownership_data;
    table.set_constants(&desc, data);
}

// @address 0x460fe0 ?__tls_set_g_contrail_gpu_allocator@@YAXPAX@Z
void __tls_set_g_contrail_gpu_allocator(void *address)
{
    data_set_new_base_address((s_data_array **)&g_contrail_gpu_data, (s_data_array *)address);
}

// (datum lookup over the thread's strip data array: the same try-and-get as set_shader_strip uses)
// @address 0x460ff8 contrail_gpu_row_try_and_get
extern "C" inline void *contrail_gpu_row_try_and_get(long handle)
{
    s_data_array *data = (s_data_array *)g_contrail_gpu_row_data;
    void *result = 0;
    if (handle != -1) {
        long absolute_index = (unsigned short)handle;
        if (absolute_index < data->first_unallocated) {
            short *datum = (short *)(data->data + data->size * absolute_index);
            long identifier = *datum;
            if (identifier != 0 && identifier == (handle >> 16))
                result = datum;
        }
    }
    return result;
}

// @address 0x461058 ?__tls_set_g_contrail_row_gpu_allocator@@YAXPAX@Z
void __tls_set_g_contrail_row_gpu_allocator(void *address)
{
    data_set_new_base_address((s_data_array **)&g_contrail_gpu_row_data, (s_data_array *)address);
}

// 0x461070: sets the thread's current value (nothing when unchanged or zero), passes it on, then refreshes each of the
// eight entries and keeps a copy of its bounds

extern "C" {
// @address 0x7be978 g_contrail_gpu_vertex_buffer
}
#if !defined(_XBOX) || defined(__clang__)
common_gpu_vertex_buffer g_contrail_gpu_vertex_buffer = { 0x12, 0x10, 0xa0 };
#endif
// @address 0x461070 contrail_gpu_set_buffer_address
extern "C" void contrail_gpu_set_buffer_address(void *address)
{
    c_contrail_gpu::s_layout *layout = &c_contrail_gpu::x_layout;
    if (address != g_contrail_gpu_buffer_address && address != 0) {
        g_contrail_gpu_buffer_address = address;
        common_gpu_vertex_buffer_set_address(&g_contrail_gpu_vertex_buffer, address);
        for (unsigned int i = 0; i < 8; i++) {
            s_gpu_storage_interface *entry = layout->entries[i];
            entry->refresh();
            layout->bounds[i].bounds = entry->bounds;
        }
    }
}

// @address 0x461120 ??0s_layout@c_contrail_gpu@@QAA@XZ
inline c_contrail_gpu::s_layout::s_layout() :
    position(0, 0),
    velocity(0, 1),
    random(0, 2),
    unused_half4(3, 0),
    unused_ushort4n(3, 2),
    length(3, 3),
    unused_color(8, 0),
    color(8, 1)
{
    slots[0] = &position;
    slots[1] = &velocity;
    slots[2] = &random;
    slots[3] = &unused_half4;
    slots[4] = &unused_ushort4n;
    slots[5] = &length;
    slots[6] = &unused_color;
    slots[7] = &color;
}

extern "C" {
// @address 0x119a4f4 g_contrail_row_alloc_success
unsigned char g_contrail_row_alloc_success[4];
}
// 0x461308: allocates a row for limit type `allocation` while the thread's row array is under that type's limit,
// links it to the datum `owner` names and returns its index; else -1. Records the outcome per type in g_contrail_row_alloc_success.
// @address 0x461308 contrail_gpu_new_row
extern "C" long contrail_gpu_new_row(unsigned long allocation_context, long owner)
{
    long result = -1;
    if (((s_data_array *)g_contrail_gpu_row_data)->actual_count + 1
        <= g_effect_budget_limits->table->blocks[_contrail_gpu_row_budget].limits[(unsigned char)allocation_context].maximum_count) {
        long index = ::datum_new((s_data_array *)g_contrail_gpu_row_data);
        c_contrail_gpu::s_row *row = datum_get<c_contrail_gpu::s_row>((s_data_array *)g_contrail_gpu_row_data, index);
        row->lifespan = 0.0f;
        row->index = (unsigned short)index;
        row->allocated = 0;
        row->next_row_index = -1;
        row->owner = owner;
        c_contrail_gpu::s_row *previous_row = (c_contrail_gpu::s_row *)contrail_gpu_row_try_and_get(owner);
        if (previous_row)
            previous_row->next_row_index = index;
        g_contrail_row_alloc_success[(unsigned char)allocation_context] = 1;
        result = index;
    } else {
        g_contrail_row_alloc_success[(unsigned char)allocation_context] = 0;
    }
    return result;
}

// (chain element: release the chain its head names, then free the element itself)

// @address 0x4639d8 contrail_gpu_destroy
extern "C" void contrail_gpu_destroy(unsigned long handle)
{
    if (handle != 0xffffffff) {
        s_contrail_chain_element *element = &((s_contrail_chain_element *)g_contrail_chain_data->data)[(unsigned short)handle];
        contrail_gpu_destroy_links(element->first_link, 1);
        element->first_link = 0xffffffff;
        element->link_count = 0;
        ::datum_delete(g_contrail_chain_data, handle);
    }
}

// (free-list unlink: take the datum out of its neighbours' links, then free the chain behind it)

// @address 0x463a38 contrail_gpu_destroy_links
extern "C" void contrail_gpu_destroy_links(unsigned long handle, int unused)
{
    if (handle != 0xffffffff) {
        s_contrail_chain_link *links = (s_contrail_chain_link *)g_contrail_gpu_link_data->data;
        s_contrail_chain_link *datum = &links[(unsigned short)handle];
        if (datum->previous != 0xffffffff)
            links[(unsigned short)datum->previous].next = datum->next;
        if (datum->next != 0xffffffff)
            ((s_contrail_chain_link *)g_contrail_gpu_link_data->data)[(unsigned short)datum->next].previous = datum->previous;
        contrail_gpu_destroy_links(datum->next, 1);
        ::datum_delete(g_contrail_gpu_link_data, handle);
    }
}

// (the frame's overall vertex stream: a single dynamic buffer the render targets export into)
extern "C" {
// @address 0x7bf2f4 g_contrail_frame_vertex_buffer
// @address 0x7bf4b4 g_effect_shader_cache
}
#if !defined(_XBOX) || defined(__clang__)
s_gpu_buffer g_contrail_frame_vertex_buffer = { 0x13, 0x10, 0xa0 };
#endif

// @address 0x463d58 ?allocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_animation_threads_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<>
void *t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_animation_threads_allocator>::allocate(unsigned int size, const char *name)
{
    int member_index = g_restricted_regions[2].add_member(name, "", size, 0,
        (field_proc_t)__tls_set_g_animation_threads_allocator, 0, 0);
    unsigned int offset = g_restricted_regions[2].get_member_offset(member_index);
    data_set_new_base_address((s_data_array **)&g_contrail_chain_data, (s_data_array *)(g_restricted_address[2] + offset));
    m_member_index = member_index;
    m_thread_id = get_current_thread_index();
    return g_restricted_address[2] + g_restricted_regions[2].get_member_offset(member_index);
}

// @address 0x463e18 ?allocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?particle_block_set_contrail_link_array@@YAXPAX@Z@@UAAPAXIPBD@Z
template<>
void *t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &particle_block_set_contrail_link_array>::allocate(unsigned int size, const char *name)
{
    int member_index = g_restricted_regions[2].add_member(name, "", size, 0,
        particle_block_set_contrail_link_array, 0, 0);
    unsigned int offset = g_restricted_regions[2].get_member_offset(member_index);
    data_set_new_base_address((s_data_array **)&g_contrail_gpu_link_data, (s_data_array *)(g_restricted_address[2] + offset));
    m_member_index = member_index;
    m_thread_id = get_current_thread_index();
    return g_restricted_address[2] + g_restricted_regions[2].get_member_offset(member_index);
}

// 0x463f88: the render constant table of the shader instance at g_effect_shader_cache
// @address 0x463f88 contrail_gpu_get_constant_table
extern "C" s_entry_table *contrail_gpu_get_constant_table(void)
{
    long pass_index = g_effect_shader_cache.vertex_type;
    const s_tag_reference *source = g_effect_shader_cache.shader_reference;
    s_render_method_shader_definition *render_method =
        (s_render_method_shader_definition *)((char *)global_tag_instances[(unsigned short)source->index].definition);
    s_rasterizer_global_vertex_shader_definition *shared_shader =
        (s_rasterizer_global_vertex_shader_definition *)((char *)global_tag_instances[(unsigned short)render_method->shared_vertex_shader.index].definition);
    s_tag_block *passes = (s_tag_block *)shared_shader->vertex_types.address;
    s_tag_block *pass = &passes[pass_index];
    long constant_index = ((s_global_vertex_shader_entry_point *)pass->address)->constant_index;
    c_rasterizer_compiled_vertex_shader *constants = (c_rasterizer_compiled_vertex_shader *)shared_shader->shaders.address;
    c_rasterizer_compiled_vertex_shader *constant = &constants[constant_index];
    return constant ? constant->get_constant_table() : 0;
}

// @address 0x4641b8 contrail_gpu_initialize_frame_targets
void contrail_gpu_initialize_frame_targets(void)
{
    unsigned long largest = 0;
    unsigned long total = 0;
    for (unsigned long i = 0; i < 3; i++) {
        s_gpu_storage_interface *entry = c_light_volume_gpu::x_layout.entries[i];
        total += entry->size();
        largest = CONTRAIL_GPU_MAX(largest, entry->size());
    }
    unsigned long stride = ((total + largest - 1) / largest) * largest;
    unsigned long offset = 0;
    for (unsigned long i = 0; i < 3; i++) {
        s_gpu_storage_interface *entry = c_light_volume_gpu::x_layout.entries[i];
        entry->owner = &g_contrail_frame_vertex_buffer;
        entry->layout(stride, offset);
        offset += entry->size();
        real_point2d size;
        size.x = (float)entry->stride;
        size.y = (float)entry->offset;
        c_light_volume_gpu::x_layout.bounds[i].size = size;
    }
    unsigned long padding = stride - offset;
    D3DDECLTYPE padding_type;
    unsigned char padding_kind;
    switch (padding) {
    case 3 * sizeof(float):
        padding_type = D3DDECLTYPE_FLOAT3;
        padding_kind = 2;
        break;
    case 2 * sizeof(float):
        padding_type = D3DDECLTYPE_FLOAT2;
        padding_kind = 1;
        break;
    case sizeof(float):
        padding_type = D3DDECLTYPE_FLOAT1;
        padding_kind = 0;
        break;
    case 0:
        padding_kind = 18;
        padding_type = D3DDECLTYPE_UNUSED;
        break;
    default:
        // match: 0x4641b8: keeps padding_type/padding_kind in registers; without it both spill to stack slots and the frame grows 0xd0 to 0xe0
        __assume(0);
    }

    rasterizer_vertex_declaration *vertex_declaration = &g_rasterizer_vertex_buffer_declarations[19];
    vertex_declaration->index = 19;
    for (unsigned long i = 0; i < 3; i++) {
        s_gpu_storage_interface *entry = c_light_volume_gpu::x_layout.entries[i];
        rasterizer_vertex_register_mapping *register_mapping = &vertex_declaration->register_mappings[i];
        register_mapping->attribute = (signed char)entry->semantic;
        register_mapping->format = entry->kind();
        register_mapping->usage = (unsigned char)k_vertex_semantic_usages[entry->semantic];
        register_mapping->usage_index = (unsigned char)entry->usage_index;
    }
    long count = 3;
    if (padding != 0) {
        vertex_declaration->register_mappings[3].attribute = 0;
        vertex_declaration->register_mappings[3].format = padding_kind;
        vertex_declaration->register_mappings[3].usage = 0;
        vertex_declaration->register_mappings[3].usage_index = 15;
        count = 4;
    }
    vertex_declaration->register_mappings[count].attribute = -1;

    long declaration_index = 12;
    s_declaration_element *end = c_vertex_declaration_table::base_vertex_decls[declaration_index];
    while (memcmp(end, &k_vertex_declaration_terminator, sizeof(s_declaration_element)) != 0)
        end++;
    if (!contrail_gpu_declaration_full(end, end - c_vertex_declaration_table::base_vertex_decls[declaration_index])) {
        for (unsigned long i = 0; i < 3; i++) {
            s_gpu_storage_interface *entry = c_light_volume_gpu::x_layout.entries[i];
            s_declaration_element *element = &end[i];
            element->Stream = 1;
            element->Offset = (unsigned short)(entry->size() * entry->offset);
            element->Type = entry->declaration_type();
            element->Method = D3DDECLMETHOD_DEFAULT;
            element->Usage = (unsigned char)k_vertex_semantic_usages[entry->semantic];
            element->UsageIndex = (unsigned char)entry->usage_index;
        }
        long element_count = 3;
        if (padding != 0) {
            element_count = 4;
            end[3].Stream = 1;
            end[3].Offset = (unsigned short)offset;
            end[3].Type = padding_type;
            end[3].Method = D3DDECLMETHOD_DEFAULT;
            end[3].Usage = 0;
            end[3].UsageIndex = 15;
        }
        memcpy(&end[element_count], &k_vertex_declaration_terminator, sizeof(s_declaration_element));
    }
}

// (effect instance constants: count, scale, timing, location and the overridable parameters)

// @address 0x14e5a0 contrail_gpu_set_effect_constants
extern "C" void contrail_gpu_set_effect_constants(s_handle *handle, s_gpu_constant_table_base *state, const s_effect_parameters *parameters)
{
    s_effect_constants constants;
    s_particle_instance *instance = datum_get<s_particle_instance>(g_particle_instance_data, handle->index);
    s_effect_contrail_location *location = datum_get<s_effect_contrail_location>(g_particle_instance_location_data, instance->location_index);
    c_light_volume_system *owner = datum_get<c_light_volume_system>(g_particle_instance_owner_data, location->owner_index);
    contrail_gpu_tag_definition *definition = TAG_GET('ltvl', contrail_gpu_tag_definition, owner->m_definition_index);
    s_effect_entry *entry = &definition->entries[instance->definition_index];
    location = datum_get<s_effect_contrail_location>(g_particle_instance_location_data, instance->location_index);
    constants.count.x = (float)entry->count;
    constants.scale.x = entry->scale;
    constants.offset.x = instance->offset;
    constants.length_times_density.x = instance->length * instance->density;
    constants.inverse_density.x = 1.0f / instance->density;
    constants.profile_length.x = instance->profile_length;
    constants.point = location->position;
    constants.vector = location->direction;
    constants.parameters[0].x = 0.0f;
    memset(&constants.parameters[1], 0, sizeof(constants.parameters) - sizeof(constants.parameters[0]));
    for (int i = 0; i < 10; i++) {
        if (entry->flags & (1 << i))
            constants.parameters[i].x = parameters->values[i];
    }
    ((c_rasterizer_constant_table_ref const *)(s_gpu_constant_table_base *)state)
        ->set_constants((c_rasterizer_constant_desc const *)&state->descs[2], (void const *)(&constants));
}

// @address 0x14e468 ?set_shader_functions@c_contrail_gpu@@ABAXPAVc_constant_table@@@Z
void c_contrail_gpu::set_shader_functions(c_constant_table *table) const
{
    s_particle_instance *instance = datum_get<s_particle_instance>(g_particle_instance_data, contrail_index);
    s_effect_contrail_location *location = datum_get<s_effect_contrail_location>(g_particle_instance_location_data, instance->location_index);
    c_light_volume_system *effect_owner = datum_get<c_light_volume_system>(g_particle_instance_owner_data, location->owner_index);
    contrail_gpu_tag_definition *definition =
        TAG_GET('ltvl', contrail_gpu_tag_definition, effect_owner->m_definition_index);
    s_effect_entry *data = &definition->entries[instance->definition_index];
    ((c_constant_table *)table)->set_scaled(3, data->gpu_functions.count, data->gpu_functions.address);
    ((c_constant_table *)table)->set(4, (unsigned char)data->gpu_colors.count, data->gpu_colors.address);
    ((c_constant_table *)table)->set(5, (unsigned char)data->gpu_properties.count, data->gpu_properties.address);
}

// (update-list gpu data: one 620-byte entry per contrail definition; property/function/color runs)

// @address 0x462b20 ??$set_shader_functions@Us_update_vertex_shader_constant_list@c_contrail_gpu@@@c_contrail_gpu@@ABAXPBU?$s_gpu_constant_table@Us_update_vertex_shader_constant_list@c_contrail_gpu@@@@@Z
template<>
void c_contrail_gpu::set_shader_functions<c_contrail_gpu::s_update_vertex_shader_constant_list>(
    const s_gpu_constant_table<c_contrail_gpu::s_update_vertex_shader_constant_list> *table) const
{
    const c_contrail *contrail = datum_get<c_contrail>(g_contrail_data_array, contrail_index);
    const c_contrail_definition *definition = get_contrail_definition(contrail);
    ((c_constant_table *)table)->set_scaled(2, definition->gpu_functions.count, definition->gpu_functions.address);
    ((c_constant_table *)table)->set(3, (unsigned char)definition->gpu_colors.count, definition->gpu_colors.address);
    ((c_constant_table *)table)->set(4, (unsigned char)definition->gpu_properties.count, definition->gpu_properties.address);
}

// @address 0x70a258 ??__Eg_contrail_gpu_allocator@@YAXXZ
// @address 0x70c9d8 ??__Fg_contrail_gpu_allocator@@YAXXZ
t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_contrail_gpu_allocator> g_contrail_gpu_allocator;

extern "C" {
// @address 0x70a268 ??__Eg_contrail_gpu_row_allocator@@YAXXZ
// @address 0x70c9f0 ??__Fg_contrail_gpu_row_allocator@@YAXXZ
// @address 0x7be960 g_contrail_gpu_row_allocator
t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_contrail_row_gpu_allocator> g_contrail_gpu_row_allocator;
// @address 0x70a278 ??__Eg_contrail_gpu_emitter_allocator@@YAXXZ
// @address 0x70ca08 ??__Fg_contrail_gpu_emitter_allocator@@YAXXZ
// @address 0x7be96c g_contrail_gpu_emitter_allocator
t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &contrail_gpu_set_buffer_address> g_contrail_gpu_emitter_allocator;
// @address 0x70a2e0 ??__Eg_light_volume_gpu_main_allocator@@YAXXZ
// @address 0x70ca20 ??__Fg_light_volume_gpu_main_allocator@@YAXXZ
// @address 0x7bf2d0 g_light_volume_gpu_main_allocator
t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_animation_threads_allocator> g_light_volume_gpu_main_allocator;
// @address 0x70a2f0 ??__Eg_light_volume_gpu_row_allocator@@YAXXZ
// @address 0x70ca38 ??__Fg_light_volume_gpu_row_allocator@@YAXXZ
// @address 0x7bf2dc g_light_volume_gpu_row_allocator
t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &particle_block_set_contrail_link_array> g_light_volume_gpu_row_allocator;
}

// @address 0x4b9a0 ??_7?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_contrail_gpu_allocator@@YAXPAX@Z@@6B@
// @address 0x4b9a8 ??_7?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_contrail_row_gpu_allocator@@YAXPAX@Z@@6B@
// @address 0x4b9b0 ??_7?$t_restricted_allocation_manager@$01$0A@$0A@$1?contrail_gpu_set_buffer_address@@YAXPAX@Z@@6B@
// @address 0x4b9e8 ??_7?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_animation_threads_allocator@@YAXPAX@Z@@6B@
// @address 0x4b9f0 ??_7?$t_restricted_allocation_manager@$01$0A@$0A@$1?particle_block_set_contrail_link_array@@YAXPAX@Z@@6B@

// 0x14ffa8: draws the batch the next frame buffer collected when it is not empty
class c_rasterizer_vertex_shader;
class c_rasterizer_pixel_shader;
extern "C" {
// @address 0x7be794 g_particle_gpu_vertex_buffer
// @address 0x8c4841 g_particle_gpu_block_until_idle
unsigned char g_particle_gpu_block_until_idle;
}
static __forceinline long contrail_gpu_set_spawn_gprs()
{
#if defined(_XBOX) && !defined(__clang__)
    long previous_allocation = g_contrail_export_layout.gpr_allocation;
#else
    long previous_allocation = g_rasterizer_gpr_allocation;
#endif
    if (previous_allocation != 4) {
        (*rasterizer_shader_gpr_get_globals())[0] = 112;
        (*rasterizer_shader_gpr_get_globals())[1] = 16;
        D3DDevice_SetShaderGPRAllocation(c_rasterizer::get_device(), 0, 112, 16);
#if defined(_XBOX) && !defined(__clang__)
        g_contrail_export_layout.gpr_allocation = 4;
#else
        g_rasterizer_gpr_allocation = 4;
#endif
    }
    return previous_allocation;
}
static __forceinline void contrail_gpu_set_spawn_shader_pair(s_particle_spawn_shader_pair *pair)
{
    s_gpu_vertex_shader<c_particle_emitter_gpu::s_spawn_vertex_shader_constant_list> *vertex = &pair->vertex_shader;
    ((s_gpu_constant_table_base *)&vertex->constants)->own_constants();
    c_rasterizer::set_vertex_shader((const c_rasterizer_vertex_shader *)tag_get(0x76747368, vertex->shader_reference->index),
        (e_vertex_type)vertex->vertex_type, _transfer_vector_vertex_none, _entry_point_default);
    s_gpu_pixel_shader<s_gpu_empty_constant_list> *pixel = &pair->pixel_shader;
    ((s_gpu_constant_table_base *)&pixel->constants)->own_constants();
    c_rasterizer::set_pixel_shader((const c_rasterizer_pixel_shader *)tag_get(0x7069786c, pixel->shader_reference->index), _entry_point_default);
}
static __forceinline void contrail_gpu_unset_spawn_shader_pair(s_particle_spawn_shader_pair *pair)
{
    ((s_gpu_constant_table_base *)&pair->vertex_shader.constants)->disown_constants();
    ((s_gpu_constant_table_base *)&pair->pixel_shader.constants)->disown_constants();
}
static __forceinline void contrail_gpu_set_spawn_memexport(s_gpu_constant_table_base *table)
{
    s_gpu_target_table<11> &layout = c_particle_emitter_gpu::x_layout;
    ((c_rasterizer_constant_table_ref const *)table)
        ->set_constants((c_rasterizer_constant_desc const *)(table->descs), (void const *)(layout.bounds));
}
static __forceinline void contrail_gpu_swap_spawn_frames(particle_block_queue_system *system)
{
    long index = system->next - system->frames;
    system->current = &system->frames[index];
    system->next = &system->frames[(index + 1) % 2];
    system->next->count = 0;
}
// @address 0x14ffa8 ?spawn_all@c_contrail_gpu@@SAXXZ
void c_contrail_gpu::spawn_all()
{
    particle_block_queue_system &frames = g_particle_block_queue_system;
    s_particle_spawn_shader_pair *passes = &g_particle_spawn_shaders;
    if (frames.next->count > 0) {
        contrail_gpu_set_spawn_gprs();
        c_rasterizer::set_render_target(0, (c_rasterizer::e_surface)0);
        D3DDevice_SetRenderState_ColorWriteEnable(c_rasterizer::get_device(), 0);
        c_rasterizer::set_render_target(1, (c_rasterizer::e_surface)0);
        c_rasterizer::set_render_target(2, (c_rasterizer::e_surface)0);
        c_rasterizer::get_device()->SetVertexShaderConstantF(32, g_gpu_particle_default_constants, 1);
        contrail_gpu_swap_spawn_frames(&frames);
        ::c_rasterizer::set_vertex_stream((c_rasterizer_vertex_buffer const *)&frames.current->first);
        ::c_rasterizer::set_vertex_stream((c_rasterizer_vertex_buffer const *)&frames.current->second);
        contrail_gpu_set_spawn_shader_pair(passes);
        c_rasterizer::get_device()->BeginExport(0, g_particle_gpu_vertex_buffer.get_d3d_buffer(), 0);
        contrail_gpu_set_spawn_memexport((s_gpu_constant_table_base *)&passes->vertex_shader.constants);
        c_rasterizer::draw_primitive(c_rasterizer_index_buffer::_primitive_type_point_list, 0, frames.current->count);
        c_rasterizer::get_device()->EndExport(0, g_particle_gpu_vertex_buffer.get_d3d_buffer(), 0);
        ::c_rasterizer::clear_vertex_stream((c_rasterizer_vertex_buffer const *)&frames.current->first);
        ::c_rasterizer::clear_vertex_stream((c_rasterizer_vertex_buffer const *)&frames.current->second);
        contrail_gpu_unset_spawn_shader_pair(passes);
        if (g_particle_gpu_block_until_idle)
            c_rasterizer::get_device()->BlockUntilIdle();
        else
#if defined(_XBOX) && !defined(__clang__)
            g_contrail_export_layout.fence = c_rasterizer::get_device()->InsertFence();
#else
            g_game_state_render_fence = c_rasterizer::get_device()->InsertFence();
#endif
    }
}

// @address 0x7bf420 g_contrail_frame_shader_pair
#if !defined(_XBOX) || defined(__clang__)
extern "C" {
s_particle_block_contrail_shader_pair g_contrail_frame_shader_pair =
{
    { 57, 0, { { 0, g_contrail_frame_shader_pair.vertex_shader.constants.slots, 7, 0, {} } }, 12 },
    { 57, 0, { 0, g_contrail_frame_shader_pair.pixel_shader.constants.slots, 1, 0, {} } }
};
}
#endif
extern "C" {
// @address 0x8c4428 k_contrail_default_dirty_mask
}
// (the state the effect shaders cache across one export: the current system, location and volume)
extern "C" {
struct s_object_14;
struct s_definition_14;

}

// Advances this contrail, reusing the caller's cached export state.
// @address 0x14e2f8 ?frame_advance@c_contrail_gpu@@AAAXPAUs_object_14@@@Z
void c_contrail_gpu::frame_advance(s_object_14 *states)
{
    s_particle_instance *instance = datum_get<s_particle_instance>(g_particle_instance_data, this->contrail_index);
    long count = (long)(instance->density * instance->length);
    if (count > 0) {
        contrail_states_set_definition((s_object_14 *)states,
            (s_definition_14 *)&((c_light_volume_system *)g_particle_instance_owner_data->data)
            [(unsigned short)((s_effect_contrail_location *)g_particle_instance_location_data->data)[(unsigned short)instance->location_index].owner_index]);
        s_effect_contrail_location *location = &((s_effect_contrail_location *)g_particle_instance_location_data->data)
            [(unsigned short)instance->location_index];
        if (location != states->location) {
            states->location = location;
            contrail_states_update_values((s_object_14 *)states, 0);
        }
        if (instance != states->source) {
            unsigned long valid = states->valid;
            states->source = instance;
            states->valid = valid & ~0x30;
            contrail_states_update_values((s_object_14 *)states, 48);
        }
        ((c_rasterizer_constant_table_ref const *)&g_contrail_frame_shader_pair.vertex_shader.constants)
            ->set_constants(
                &g_contrail_frame_shader_pair.vertex_shader.constants.descs[6],
                (void const *)(g_gpu_particle_default_constants));
        set_shader_functions((c_constant_table *)&g_contrail_frame_shader_pair.vertex_shader.constants);
        contrail_gpu_set_effect_constants((s_handle *)this, &g_contrail_frame_shader_pair.vertex_shader.constants,
            (const s_effect_parameters *)states);
        set_shader_strip(&g_contrail_frame_shader_pair.vertex_shader.constants, count);
        c_rasterizer::draw_primitive(c_rasterizer_index_buffer::_primitive_type_point_list, 0, count);
        if (g_gpu_particle_block_until_idle)
            c_rasterizer::get_device()->BlockUntilIdle();
    }
}
static __forceinline void contrail_gpu_frame_set_shader_pair()
{
    g_contrail_frame_shader_pair.vertex_shader.constants.own_constants();
    c_rasterizer::set_vertex_shader((const c_rasterizer_vertex_shader *)tag_get(0x76747368, g_contrail_frame_shader_pair.vertex_shader.shader_reference->index),
        (e_vertex_type)g_contrail_frame_shader_pair.vertex_shader.vertex_type, _transfer_vector_vertex_none, _entry_point_default);
    ((s_gpu_constant_table_base *)&g_contrail_frame_shader_pair.pixel_shader.constants)->own_constants();
    c_rasterizer::set_pixel_shader(
        (const c_rasterizer_pixel_shader *)tag_get(0x7069786c, g_contrail_frame_shader_pair.pixel_shader.shader_reference->index),
        _entry_point_default);
}
static __forceinline void contrail_gpu_frame_unset_shader_pair()
{
    g_contrail_frame_shader_pair.vertex_shader.constants.disown_constants();
    ((s_gpu_constant_table_base *)&g_contrail_frame_shader_pair.pixel_shader.constants)->disown_constants();
}
static __forceinline void contrail_gpu_frame_set_memexport()
{
    ((c_rasterizer_constant_table_ref const *)&g_contrail_frame_shader_pair.vertex_shader.constants)
        ->set_constants(g_contrail_frame_shader_pair.vertex_shader.constants.descs,
            (void const *)(&c_light_volume_gpu::x_layout.bounds[0]));
}
// @address 0x463ba0 ?frame_advance_all@c_contrail_gpu@@SAXM@Z
void c_contrail_gpu::frame_advance_all(float delta_time)
{
    if (g_contrail_chain_data->first_unallocated > 0) {
        c_rasterizer::set_render_target(0, (c_rasterizer::e_surface)0);
        D3DDevice_SetRenderState_ColorWriteEnable(c_rasterizer::get_device(), 0);
        c_rasterizer::set_render_target(1, (c_rasterizer::e_surface)0);
        c_rasterizer::set_render_target(2, (c_rasterizer::e_surface)0);
        c_rasterizer::set_vertex_stream(&g_contrail_frame_vertex_buffer);
        contrail_gpu_frame_set_shader_pair();
        c_rasterizer::get_device()->BeginExport(0, g_contrail_frame_vertex_buffer.get_d3d_buffer(), 0);
        contrail_gpu_frame_set_memexport();
        s_object_14 states;
        states.definition = 0;
        states.location = 0;
        states.source = 0;
        states.pad30 = 0.0f;
        states.valid = 0x200;
        states.dirty = k_contrail_default_dirty_mask;
        c_data_iterator<c_contrail_gpu> iterator;
        iterator.begin(g_contrail_chain_data);
        while (iterator.next()) {
            c_contrail_gpu *contrail = iterator.get_datum();
            if (contrail->first_row_index != -1)
                contrail->frame_advance(&states);
        }
        c_rasterizer::get_device()->EndExport(0, g_contrail_frame_vertex_buffer.get_d3d_buffer(), 0);
        contrail_gpu_frame_unset_shader_pair();
#if defined(_XBOX) && !defined(__clang__)
        g_contrail_export_layout.fence = c_rasterizer::get_device()->InsertFence();
#else
        g_game_state_render_fence = c_rasterizer::get_device()->InsertFence();
#endif
    }
}

// @address 0x461578 ?allocate_profile@c_contrail_gpu@@AAAKKPBVc_contrail_profile@@@Z
unsigned long c_contrail_gpu::allocate_profile(unsigned long allocation, const c_contrail_profile *profile)
{
    s_row *row = 0;
    if (first_row_index != -1)
        row = contrail_gpu_row_get(first_row_index);
    if (!get_prev_profile_temporary()) {
        if (!row || row->allocated >= 16) {
            long index = contrail_gpu_new_row(allocation, first_row_index);
            if (index != -1) {
                first_row_index = index;
                row = contrail_gpu_row_get(first_row_index);
            } else {
                return -1;
            }
        }
        row->allocated++;
        profile_count++;
    }
    row->lifespan = row->lifespan > (1.0f - profile->get_age()) * profile->get_lifespan()
        ? row->lifespan : (1.0f - profile->get_age()) * profile->get_lifespan();
    return row->get_last_allocated();
}
// @address 0x462118 ?reserve_memory@?$t_restricted_allocation_manager@$01$0A@$0A@$1?contrail_gpu_set_buffer_address@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
template<>
void *t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &contrail_gpu_set_buffer_address>::reserve_memory(const char *name, const char *type, unsigned int size,
    long alignment_bits)
{
    long member_index = g_restricted_regions[2].add_member(name, type, size, alignment_bits,
        contrail_gpu_set_buffer_address, 0, 0);
    unsigned int offset = g_restricted_regions[2].get_member_offset(member_index);
    contrail_gpu_set_buffer_address(g_restricted_address[2] + offset);
    m_member_index = member_index;
    m_thread_id = get_current_thread_index();
    return g_restricted_address[2] + g_restricted_regions[2].get_member_offset(member_index);
}

// @address 0x4621a0 ?allocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?contrail_gpu_set_buffer_address@@YAXPAX@Z@@UAAPAXIPBD@Z
template<>
void *t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &contrail_gpu_set_buffer_address>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

template<>
void t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &contrail_gpu_set_buffer_address>::deallocate(void *)
{
    restricted_region_free_member(2, m_member_index);
}

// @address 0x462350 ??0?$s_queued_gpu_buffer@Us_layout@c_contrail_gpu@@$0IA@@@QAA@XZ
template<>
s_queued_gpu_buffer<c_contrail_gpu::s_layout, 128>::s_queued_gpu_buffer()
{
    first.format = 18;
    first.element_count = 0x100;
    first.element_size = 1;
    second.format = 17;
    second.element_count = 0x100;
    second.element_size = 1;
}

// (render: submit the profile ribbons of one contrail through the render vertex shader)
struct s_weighted_atmosphere_parameters;
// @address 0x8c4842 ?g_contrail_gpu_block_until_idle@@3EA
unsigned char g_contrail_gpu_block_until_idle;
class c_render_method;

// @address 0x461ca8 ?render@c_contrail_gpu@@QBAXPBVc_contrail_states@@@Z
void c_contrail_gpu::render(const c_contrail_states *contrail_states) const
{
    long saved_cull_mode;
    if (first_row_index == -1 || profile_count <= 1)
        return;
    const c_contrail *contrail = contrail_gpu_get(contrail_index);
    const c_contrail_location *location = contrail_location_get(contrail);
    const c_contrail_system *system = &((c_contrail_system *)((s_data_array *)g_contrail_system_data)->data)
        [DATUM_INDEX_TO_ABSOLUTE_INDEX(location->system_index)];
    long system_tag_index = system->definition_index;
    const s_contrail_system_definition *system_definition = (const s_contrail_system_definition *)tag_get(0, system_tag_index);
    long definition_index = contrail->definition_index;
    const c_contrail_definition *definition = &system_definition->contrails[definition_index];
    c_rasterizer::set_vertex_stream(&g_contrail_gpu_vertex_buffer);
    c_rasterizer::get_device()->SetVertexShaderConstantF(32, g_gpu_particle_default_constants, 1);
    unsigned short profile = profile_count;
    // match: 0x461ca8: value range assumption guides branch lowering
    __assume(profile >= 2);
    unsigned long doubled_profile = (unsigned long)profile * 2;
    long strip_primitive_count = (long)doubled_profile - 2;
    unsigned long strip_count;
    switch (definition->first_flag)
    {
    case 0:
        strip_count = 1;
        break;
    case 1:
        strip_count = 2;
        break;
    default:
        strip_count = definition->second_flag;
        break;
    }
    if (((unsigned char)(((unsigned long)definition->profile_type >> 7) & 1) != 0) && atmosphere_constants_index != -2)
    {
        atmosphere_constants_index = -2;
        c_atmosphere_fog_interface::set_constant(&c_atmosphere_fog_interface::m_default_parameters, true, true);
    }
    if (((unsigned char)(((unsigned long)definition->profile_type >> 1) & 1) != 0))
    {
        saved_cull_mode = D3DDevice_GetRenderState_CullMode(c_rasterizer::get_device());
        D3DDevice_SetRenderState_CullMode(c_rasterizer::get_device(), 0);
    }
    const c_render_method *render_method = &definition->shader;
    if (!g_contrail_render_vertex_shader.is_initialized())
        g_contrail_render_vertex_shader.initialize((const s_tag_reference *)render_method);
    set_shader_state<s_render_vertex_shader_constant_list>(g_contrail_render_vertex_shader.get_constant_table(), contrail_states);
    set_shader_strip<s_render_vertex_shader_constant_list>(g_contrail_render_vertex_shader.get_constant_table());
    c_render_method_data::m_valid = true;
    if (render_method_submit(((c_contrail_system *)((s_data_array *)g_contrail_system_data)->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(
        ((c_contrail_location *)((s_data_array *)g_contrail_location_data)->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(contrail->location_index)]
        .system_index)].definition_index, render_method, _vertex_type_contrail,
        _transfer_vector_vertex_none, _entry_point_default, false))
    {
        if (strip_count != 0)
        {
            long strip_offset = 0;
            unsigned long strips_remaining = strip_count;
            do
            {
                c_rasterizer::draw_primitive(c_rasterizer_index_buffer::_primitive_type_triangle_strip, strip_offset, strip_primitive_count);
                strips_remaining--;
                strip_offset += doubled_profile;
            } while (strips_remaining != 0);
        }
        if (g_contrail_gpu_block_until_idle)
            c_rasterizer::get_device()->BlockUntilIdle();
    }
    if (((unsigned char)(((unsigned long)definition->profile_type >> 1) & 1) != 0))
        D3DDevice_SetRenderState_CullMode(c_rasterizer::get_device(), saved_cull_mode);
    c_render_method_data::m_valid = false;
    render_method_submit_valid();
}

// @rdata 0x82024ca0..0x82024cd0
static const s_color_pack_constants k_color_pack = {
    {6.07967376708984375e-5f, 6.07967376708984375e-5f, 6.07967376708984375e-5f, 6.07967376708984375e-5f},
    {0.0001218318939208984375f, 0.0001218318939208984375f, 0.0001218318939208984375f, 2.384185791015625e-7f},
    {3.0f, 3.0f, 3.0f, 3.0f}
};
// @rdata-end

__vector4 gpu_queue_pack_color_vec(const __vector4 &value)
{
    return __vmaddfp(value, k_color_pack.scale, k_color_pack.offset);
}

__vector4 gpu_queue_pack_rotation_vec(const __vector4 &value)
{
    return __vmaddfp(value, k_color_pack.alpha_scale, k_color_pack.offset);
}

static __forceinline void contrail_gpu_queue_half4(s_gpu_storage_half4 *storage, unsigned long index, const __vector4 &value)
{
    u_gpu_queue_bits packed;
    packed.vector = __vpkd3d(value, value, VPACK_FLOAT16_4, VPACK_64LO, 2);
    storage->set_value(index, packed.i64);
}

static __forceinline void contrail_gpu_queue_half2(s_gpu_storage_half2 *storage, unsigned long index, const __vector4 &value)
{
    u_gpu_queue_bits packed;
    packed.vector = __vpkd3d(value, value, VPACK_FLOAT16_2, VPACK_64LO, 3);
    storage->set_value(index, packed.i32);
}

static __forceinline void contrail_gpu_queue_color(s_gpu_storage_color *storage, unsigned long index, const __vector4 &value)
{
    u_gpu_queue_bits packed;
    packed.vector = gpu_queue_pack_color_vec(value);
    packed.vector = __vpkd3d(packed.vector, packed.vector, VPACK_D3DCOLOR, VPACK_32, 3);
    storage->set_value(index, packed.i32);
}

static __forceinline void contrail_gpu_queue_address(s_gpu_storage_ushort2 *storage, unsigned long index, const __vector4 &value)
{
    __vector4 converted = __vctuxs(value, 0);
    u_gpu_queue_bits packed;
    packed.vector = __vpkuwus(converted, converted);
    storage->set_value(index, packed.i32);
}

// @address 0x4616a0 ?queue_profile@c_contrail_gpu@@QAAXKPBVc_contrail_profile@@_N@Z
void c_contrail_gpu::queue_profile(unsigned long queue_context, const c_contrail_profile *profile, bool temporary)
{
    unsigned long allocation = queue_context & 0xff;
    unsigned long index = x_queued_buffer_system.next->count;
    if (index + 1 <= g_effect_budget_limits->table->blocks[_contrail_gpu_profile_budget].limits[allocation].maximum_count && profile_count < 128) {
        unsigned long profile_address = allocate_profile(queue_context, profile);
        if (profile_address != -1) {
            if (temporary)
                flags |= 1;
            else
                flags &= ~1;
            real_vector4d position;
            position.x = profile->get_position()->x;
            position.y = profile->get_position()->y;
            position.z = profile->get_position()->z;
            position.w = 0.0f;
            __vector4 velocity;
            velocity.v[0] = profile->get_velocity()->i;
            velocity.v[1] = profile->get_velocity()->j;
            velocity.v[2] = profile->get_velocity()->k;
            velocity.v[3] = profile->get_scale();
            const unsigned __int64 *random = reinterpret_cast<const unsigned __int64 *>(profile->get_random_seed_ref());
            __vector4 length;
            length.v[0] = profile->get_cumulative_length();
            length.v[1] = profile->get_lifespan();
            length.v[2] = 0.0f;
            length.v[3] = 0.0f;
            __vector4 color;
            color.v[0] = profile->get_color()->red;
            color.v[1] = profile->get_color()->green;
            color.v[2] = profile->get_color()->blue;
            color.v[3] = profile->get_color()->alpha;
            unsigned long column = profile_address % 16;
            unsigned long row = profile_address / 16;
            x_queued_buffer_system.next->write_lock();
            s_queued_gpu_buffer<s_layout, 128> *layout = x_queued_buffer_system.next;
            layout->position.set_value(index, position);
            contrail_gpu_queue_half4(&layout->velocity, index, velocity);
            layout->random.set_value(index, *random);
            contrail_gpu_queue_half2(&layout->length, index, length);
            contrail_gpu_queue_color(&layout->color, index, color);
            __vector4 address;
            address.v[0] = (float)column;
            address.v[1] = (float)row;
            address.v[2] = 0.0f;
            address.v[3] = 0.0f;
            contrail_gpu_queue_address(&x_queued_buffer_system.next->address_group.address, index, address);
            x_queued_buffer_system.next->count++;
            x_queued_buffer_system.next->unlock();
        }
    }
}

