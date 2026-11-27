// graphicslibxbox/particle_block.obj: from the records 0x145588 0x145a70 0x145b68 0x1465d0 0x14aaa0 0x14d060 0x14d8e0 0x14dde0 0x14ded0 0x14e138 0x14e850 0x150288 0x150508 0x150820 0x150940 0x150bb0 0x182520 0x182718 0x182818 0x335648 0x45f390 0x45f5b8 0x45f680 0x45fc30 0x45fc40 0x45fc50 0x45fc58 0x45fc68 0x45fc78 0x45fc88 0x45fc98 0x45fca8 0x45fcb0 0x460078 0x460108 0x460210 0x4602b8 0x460330 0x460408 0x4604e0 0x4604f8 0x460528 0x460540 0x460570 0x4605a0 0x4605b8 0x4605e8 0x460618 0x460648 0x460678 0x460ac0 0x460bd8 0x4613f0 0x461a08 0x463828 0x463840 0x463af0 0x463df8 0x463eb8 0x463ed8 0x463f60 0x463f78 0x4640e0 0x559760 0x70a248 0x70a300
#include <xtl.h>
#include "gpu_particle/shared_gpu_storage.h"
#include "gpu_particle/target_table_types.h"
#include "rasterizer/rasterizer_sampler_filter_state.h"
// @address 0x8c43a8 g_effect_budget_limits
// @address 0x737664 g_light_volume_gpu_layout
// @address 0x7c6178 g_rasterizer_sampler_filter_modes
// @address 0x195e88 rasterizer_device_set_sampler_filter_modes
// @address 0x3aac8 k_real_math_identity_matrix_storage

#include "gpu_particle/contrail_chain_types.h"
#include "effects/light_volume_types.h"
#include "effects/contrail.h"
#include "effects/contrail_location.h"
#include "effects/contrail_system.h"
#include "gpu_particle/constant_table.h"
#include "geometry/geometry_definitions.h"
#include "rasterizer/xenon/rasterizer_xenon_submit.h"
#include "effects/particle_array_globals.h"
// @flags /TP /O2 /Oc
#include "physics/physics_constants_tls.h"
#include "camera/observer_globals_types.h"
#include "effects/effect_array_globals.h"
#include "effects/effect_runtime_storage.h"
#include "effects/effect_lightprobe_memory.h"
#include "effects/effect_lightprobe_slot_data.h"
#include "memory/datum_index.h"
#include "gpu_particle/gpu_particle_globals.h"
#include "objects/widgets/widgets.h"
#include "gpu_particle/particle_block_globals.h"
#include "gpu_particle/contrail_gpu.h"
#include "effects/effect_budget_limits.h"
struct s_data_array;
struct particle_block_queue_system;
#include "rasterizer/rasterizer_main_globals.h"
#include "tag_files/tag_groups.h"
#include "tag_files/tag_types.h"
#include "tag_files/tag_index.h"
#include <string.h>
#include "game/game_time_types.h"
// @address 0x14d8e0 ?set_constants@c_rasterizer_constant_table_ref@@QBAXPBVc_rasterizer_constant_desc@@PBX@Z
// @address 0x460330 particle_block_bind_render_slot_set
// @address 0x460408 ?initialize@?$s_gpu_shader@Us_update_vertex_shader_constant_list@c_particle_emitter_gpu@@@@QAAXVc_rasterizer_constant_table_ref@@@Z
// @address 0x1ec830 ?data_set_new_base_address@@YAXPAPAUs_data_array@@PAU1@@Z
// @address 0x1d6bb0 ?restricted_region_free_member@@YAXJJ@Z
// @address 0x14c8a8 ?set_render_target@c_rasterizer@@SAXJW4e_surface@1@@Z
// @address 0xaae70 ?datum_new@@YAJPAUs_data_array@@@Z
// @address 0x14cec0 ?set_vertex_stream@c_rasterizer@@SA_NPBVc_rasterizer_vertex_buffer@@@Z
// @address 0x150210 ?clear_vertex_stream@c_rasterizer@@SA_NPBVc_rasterizer_vertex_buffer@@@Z
// @address 0x14d118 ?set_vertex_shader@c_rasterizer@@SA_NPBVc_rasterizer_vertex_shader@@W4e_vertex_type@@W4e_transfer_vector_vertex_types@@W4e_entry_point@@@Z
// @address 0x14d4b8 ?set_pixel_shader@c_rasterizer@@SA_NPBVc_rasterizer_pixel_shader@@W4e_entry_point@@@Z
// @address 0x14e9b8 ?draw_primitive@c_rasterizer@@SAXW4e_primitive_type@c_rasterizer_index_buffer@@JJ@Z
// @address 0x14e850 ??$set_shader_strip@Us_particle_block_contrail_vertex_shader_constant_list@@@c_contrail_gpu@@ABAXPBU?$s_gpu_constant_table@Us_particle_block_contrail_vertex_shader_constant_list@@@@J@Z
// @address 0x182520 ??$set_shader_strip@Us_render_vertex_shader_constant_list@c_light_volume_gpu@@@c_light_volume_gpu@@ABAXPBU?$s_gpu_constant_table@Us_render_vertex_shader_constant_list@c_light_volume_gpu@@@@J@Z
// @address 0x182718 ??$set_shader_variants@Us_render_vertex_shader_constant_list@c_particle_emitter_gpu@@@c_particle_emitter_gpu@@ABAXPBU?$s_gpu_constant_table@Us_render_vertex_shader_constant_list@c_particle_emitter_gpu@@@@@Z
#include "rasterizer/rasterizer.h"
#include "cache/restricted_memory_regions.h"
#include "cseries/string_literal_pool.h"
#include "gpu_particle/particle_block.h"
#include "models/model_skinning.h"
#include "render/render.h"
#include "render_methods/render_method_submit.h"
#include "gpu_particle/common_gpu.h"
#include "gpu_particle/contrail_update_shader_pair.h"
#include "memory/data.h"
#include "sky_atm/atmosphere.h"
#include "math/real_math_types.h"
#include "rasterizer/rasterizer_vertex_definitions.h"
#include "rasterizer/rasterizer_resource_definitions.h"
#include "cache/restricted_memory_layout.h"
#include "geometry/geometry_sampling_types.h"
#include "geometry/geometry_sampling.h"
#include "gpu_particle/gpu_empty_constant_list_types.h"
#include "render_methods/render_method.h"
#include "render_methods/render_method_data.h"
#include "geometry/compression_info_types.h"
#include "game/game_globals.h"
#include "cache/restricted_memory.h"
#include "memory/tls_data_arrays.h"
#include "memory/data_types.h"
#if defined(_XBOX) && !defined(__clang__)
#include "gpu_particle/contrail_layout_types.h"
#else
#include "rasterizer/rasterizer_shader_gpr_types.h"
#include "saved_games/game_state_procs_externs.h"
#endif
#include "gpu_particle/particle_block_contrail_vertex_shader_constant_list_types.h"
#include "gpu_particle/desc_types.h"
#include "gpu_particle/shader_info_types.h"
#include "effects/light_volume_gpu_types.h"
#include "cache/cache_files.h"
#include "scenario/scenario.h"
#include "gpu_particle/contrail_target_bounds_types.h"
#include "gpu_particle/particle_block_bounds_types.h"
#include "gpu_particle/contrail_target_entry.h"
#include "rasterizer/c_rasterizer_constant_table_ref.h"
#include "render/render_player_view_observer_flags_types.h"
#include "render/lighting_interface.h"
#include "effects/particle_system.h"
#include "effects/particle_emitter.h"
#include "effects/particle_instance_types.h"
#include "effects/effect_contrail_location_types.h"
#include "interface/player_profile_global_settings.h"

class c_particle_location;
class c_contrail_system;
class c_particle_definition;


// The float4 the GPU particle shaders read as their default vertex constant: { quiet NaN, 0, 0, quiet NaN }.
union u_gpu_particle_default_constants {
    unsigned long bits[4];
    float values[4];
};

struct s_particle_block_element {
    short identifier;
    long previous_index;
    long next_index;
    unsigned short index;
    unsigned char state;
    float value;
};
typedef char particle_block_element_size[(sizeof(s_particle_block_element) == 20) ? 1 : -1];
typedef char particle_block_element_state_offset[(offsetof(s_particle_block_element, state) == 14) ? 1 : -1];
typedef char particle_block_element_value_offset[(offsetof(s_particle_block_element, value) == 16) ? 1 : -1];

#include "effects/particle_emitter_gpu_array_types.h"

#include "rasterizer/rasterizer_global_definitions.h"


#include "gpu_particle/effect_entry_types.h"
struct s_contrail_effect_definition { c_typed_tag_block<s_effect_entry> entries; };

struct s_particle_block_owner_datum { short identifier; long element_index; };

#include "rasterizer/rasterizer_shader_definitions.h"

// (spawn rendering: the queued spawn vertex batches and the shader pair they draw through)
#include "gpu_particle/gpu_buffer.h"
#include "gpu_particle/particle_block_queue_types.h"
#include "gpu_particle/contrail_queue_system.h"


#include "gpu_particle/constant_table_types.h"
#include "gpu_particle/particle_emitter_gpu.h"

// @address 0x24cd0 ?x_in_place@s_update_vertex_shader_constant_list@c_particle_emitter_gpu@@2QB_NB
// @address 0x24cd8 ?x_in_place@s_render_vertex_shader_constant_list@c_particle_emitter_gpu@@2QB_NB
// @address 0x24ce4 ?x_keys@s_update_vertex_shader_constant_list@c_particle_emitter_gpu@@2QBHB
// @address 0x24cfc ?x_keys@s_render_vertex_shader_constant_list@c_particle_emitter_gpu@@2QBHB
#if !defined(_XBOX) || defined(__clang__)
const bool c_particle_emitter_gpu::s_update_vertex_shader_constant_list::x_in_place[slot_count] = { false, false, true, true, true, false };
const bool c_particle_emitter_gpu::s_render_vertex_shader_constant_list::x_in_place[slot_count] = { false, false, false, false, false };
const int c_particle_emitter_gpu::s_update_vertex_shader_constant_list::x_keys[slot_count] = { 0x80000, 0x80001, 0x80002, 0x80003, 0x80004, 0x80005 };
const int c_particle_emitter_gpu::s_render_vertex_shader_constant_list::x_keys[slot_count] = { 0x80006, 0x80007, 0x80008, 0x80009, 0x8000a };
#endif

#include "gpu_particle/spawn_shader_pair.h"
#include "gpu_particle/vertex_shader.h"
#include "gpu_particle/particle_block_vertex_shader_initialize.h"
#include "gpu_particle/particle_emitter_gpu_shader.h"
#include "gpu_particle/particle_update_shader.h"
#include "render_methods/render_method_definitions.h"
#include "rasterizer/rasterizer.h"
#include "gpu_particle/contrail_gpu_types.h"
#include "gpu_particle/contrail_shader_globals.h"
#include "effects/contrail_states_types.h"
#include "rasterizer/rasterizer_shader_tag_types.h"
#include "render_methods/render_method_shader_definition_types.h"
#include "gpu_particle/global_vertex_shader.h"
#include "gpu_particle/particle_block_render_shader_initialize.h"
#include "effects/particle_definitions.h"
#include "effects/particle_emitter_definitions.h"
#include "effects/particle_movement_internals.h"
#include "visibility/visibility_projections_and_volumes.h"
#include "math/matrix_math_types.h"
#include "math/matrix_math_functions.h"
#include "effects/particle_location_runtime_types.h"
#include "game/game_time_inline.h"
#include "effects/particle_states.h"
#include "gpu_particle/particle_block_contrail_shader_pair.h"
#include "cache/restricted_memory_layout.h"
#include <stdlib.h>
#include "geometry/render_geometry_types.h"
#include "geometry/render_geometry_mesh_types.h"
extern "C" {
#include "rasterizer/rasterizer_state_globals.h"
}
#include "render/views/player_view_types.h"
#include "sky_atm/atmosphere_fog_interface.h"
#include "rasterizer/rasterizer_primitives.h"
#include "rasterizer/rasterizer_vertex_buffer.h"
#include "gpu_particle/gpu_queue.h"
#include "effects/particle.h"

struct render_state
{
    real_vector4d flags, flags2, flags3, first_person, scalar;
    real_vector2d uv;
    __declspec(align(16)) real_vector4d time, count, render_flags;
    float inverse_far, near_fade, inverse_angle, angle;
};

// GPU variant header: particle_render_state.fx stores count then maximum mesh size.
struct s_particle_model_variant { float variant_count; float vertex_count; };

class c_particle_model_definition {
public:
    static const c_particle_model_definition *get(c_tag_index index) { return TAG_GET('pmdf', const c_particle_model_definition, index); }
    __forceinline void set_vertex_stream() const
    {
        c_rasterizer::set_vertex_stream((const c_rasterizer_vertex_buffer *)render_geometry_get_vertex_buffer(&m_geometry,
            (unsigned short)((const s_mesh *)m_geometry.meshes.address)->vertex_buffer_indices[0]));
        render_mesh_submit_compression((const s_compression_info *)m_geometry.compression_info.address);
    }
    __forceinline void get_gpu_variant_data(s_gpu_variant_data *variant_data) const;
private:
    s_render_geometry m_geometry;
public:
    long m_variant_count;
    TAG_POINTER(const s_particle_model_variant) m_variants;
};

struct s_gpu_variant_data {
    s_gpu_global_vertex_shader<c_particle_emitter_gpu::s_render_vertex_shader_constant_list> *shader;
    long vertex_type;
    bool per_particle_draw;
    c_rasterizer_index_buffer::e_primitive_type primitive_type;
    long vertex_count;
    long primitive_count;
};

// Packed vertex values: the pack intrinsics return the bits in a vector register.
union u_gpu_pack_bits { __vector4 vector; unsigned __int64 i64; unsigned long i32; };

static __forceinline s_particle_block_element *particle_block_get(long block_index)
{
    return (s_particle_block_element *)((s_data_array *)g_particle_emitter_gpu_data)->data + (block_index & 0xffff);
}

// match: 0x145a70: the full integer argument passes without byte narrowing.

// @address 0x145a70 ?particle_block_allocate_element_slot@@YAHPAUs_particle_emitter_gpu_block@@HM@Z
int particle_block_allocate_element_slot(s_particle_emitter_gpu_block *reference, int type, float value)
{
    int result;
    long element_index = reference->block_index;
    s_particle_block_element *element = 0;
    if (element_index == -1 || (element = particle_block_get(element_index)) == 0 || element->state >= 16) {
        element_index = particle_block_element_allocate(type, element_index);
        if (element_index != -1) {
            reference->block_index = element_index;
            element = particle_block_get(element_index);
        }
    }
    if (element_index != -1) {
        float maximum = element->value;
        if (maximum <= value)
            maximum = value;
        unsigned char state = element->state;
        element->value = maximum;
        element->state = state + 1;
        reference->count++;
        result = ((element->index + 1) << 4) - element->state;
    } else {
        result = -1;
    }
    return result;
}

// @address 0x14aaa0 particle_block_delete_element
extern "C" void particle_block_delete_element(long block_index, bool delete_following)
{
    if (block_index != -1)
    {
        s_particle_block_element *block = particle_block_get(block_index);
        if (block->previous_index != -1)
            particle_block_get(block->previous_index)->next_index = block->next_index;
        if (block->next_index != -1)
            particle_block_get(block->next_index)->previous_index = block->previous_index;
        if (delete_following)
            particle_block_delete_element(block->next_index, true);
        ::datum_delete((s_data_array *)g_particle_emitter_gpu_data, block_index);
    }
}

// @address 0x14d060 ?own_constants@s_gpu_constant_table_base@@QBAXXZ
void s_gpu_constant_table_base::own_constants() const
{
    if (this->valid) {
        for (unsigned long i = 0; i < this->desc_count; i++) {
            c_rasterizer_constant_desc *desc = &this->descs[i];
            if (desc->owned) {
                if (!this->shader->is_pixel)
                    c_rasterizer::get_device()->GpuOwnVertexShaderConstantF(desc->start, desc->count);
                else
                    c_rasterizer::get_device()->GpuOwnPixelShaderConstantF(desc->start, desc->count);
            }
        }
    }
}

// 0x14d8e0: c_rasterizer_constant_table_ref::set_constants

// @address 0x14dde0 ?disown_constants@s_gpu_constant_table_base@@QBAXXZ
void s_gpu_constant_table_base::disown_constants() const
{
    if (this->valid) {
        for (unsigned long i = 0; i < this->desc_count; i++) {
            c_rasterizer_constant_desc *desc = &this->descs[i];
            if (desc->owned) {
                if (!this->shader->is_pixel)
                    c_rasterizer::get_device()->GpuDisownVertexShaderConstantF(desc->start, desc->count);
                else
                    c_rasterizer::get_device()->GpuDisownPixelShaderConstantF(desc->start, desc->count);
            }
        }
    }
}

void c_rasterizer_constant_table_ref::set_constants(const c_rasterizer_constant_desc *constant_desc, const void *data) const
{
    const s_gpu_constant_table_base *table = (const s_gpu_constant_table_base *)this;
    if (constant_desc->count) {
        if (constant_desc->owned) {
            if (!table->shader->is_pixel)
                c_rasterizer::get_device()->GpuLoadVertexShaderConstantF4Pointer(constant_desc->start, data, (constant_desc->count + 3) & -4);
            else
                c_rasterizer::get_device()->GpuLoadPixelShaderConstantF4Pointer(constant_desc->start, data, (constant_desc->count + 3) & -4);
        } else {
            if (!table->shader->is_pixel)
                c_rasterizer::get_device()->SetVertexShaderConstantF(constant_desc->start, (const float *)data, constant_desc->count);
            else
                c_rasterizer::get_device()->SetPixelShaderConstantF(constant_desc->start, (const float *)data, constant_desc->count);
        }
    }
}
static __forceinline c_rasterizer_constant_table_ref get_shader_constant_table(c_rasterizer_compiled_vertex_shader *shader) {
    c_rasterizer_constant_table_ref result;
    result.table = shader ? shader->get_constant_table() : 0;
    return result;
}

extern "C" {
// @address 0x119a4ec g_particle_element_alloc_success
unsigned char g_particle_element_alloc_success[4];
// @address 0x119a4f8 g_contrail_element_alloc_success
unsigned char g_contrail_element_alloc_success[4];
// @address 0x119a4fc g_contrail_link_alloc_success
unsigned char g_contrail_link_alloc_success[4];
}

// @address 0x145b68 ?particle_block_element_allocate@@YAHHH@Z
// 0x145b68
int particle_block_element_allocate(int type, int owner)
{
    type = (unsigned char)type;
    int result = -1;
    if (((s_data_array *)g_particle_emitter_gpu_data)->actual_count + 1
        <= g_effect_budget_limits->table->blocks[_particle_gpu_row_budget].limits[type].maximum_count) {
        long index = datum_new((s_data_array *)g_particle_emitter_gpu_data);
        s_particle_block_element *element = particle_block_get(index);
        element->value = 0.0f;
        element->index = (unsigned short)index;
        element->state = 0;
        element->previous_index = -1;
        element->next_index = owner;
        s_particle_block_owner_datum *datum = (s_particle_block_owner_datum *)particle_emitter_gpu_try_and_get(owner);
        if (datum)
            datum->element_index = index;
        g_particle_element_alloc_success[type] = 1;
        result = index;
    } else {
        g_particle_element_alloc_success[type] = 0;
    }
    return result;
}

static __forceinline s_contrail_chain_element *particle_block_contrail_chain_element_get(long index)
{
    return (s_contrail_chain_element *)g_contrail_chain_data->data + (index & 0xffff);
}
static __forceinline s_contrail_chain_link *particle_block_contrail_chain_link_try_get(s_data_array *array, long index)
{
    if (index == -1)
        return 0;
    long absolute_index = index & 0xffff;
    if (absolute_index >= array->first_unallocated)
        return 0;
    s_contrail_chain_link *link = (s_contrail_chain_link *)(array->data + array->size * absolute_index);
    if (link->identifier == 0 || link->identifier != (index >> 16))
        return 0;
    return link;
}
static __forceinline long particle_block_contrail_chain_link_new(s_particle_system_limits_owner *limits, unsigned char type, long next_index)
{
    long result;
    if (g_contrail_gpu_link_data->actual_count + 1 > limits->table->blocks[_contrail_gpu_budget].limits[type].maximum_count)
        result = -1;
    else
    {
        result = datum_new(g_contrail_gpu_link_data);
        s_contrail_chain_link *link = (s_contrail_chain_link *)g_contrail_gpu_link_data->data + (result & 0xffff);
        link->index = (unsigned short)result;
        link->previous = -1;
        link->next = next_index;
        s_contrail_chain_link *next = particle_block_contrail_chain_link_try_get(g_contrail_gpu_link_data, next_index);
        if (next)
            next->previous = result;
        g_contrail_link_alloc_success[type] = 1;
    }
    return result;
}
static __forceinline void particle_block_contrail_chain_element_delete(long index)
{
    if (index != -1)
    {
        s_contrail_chain_element *element = particle_block_contrail_chain_element_get(index);
        contrail_gpu_destroy_links(element->first_link, 1);
        element->first_link = -1;
        element->link_count = 0;
        ::datum_delete(g_contrail_chain_data, index);
    }
}
// @address 0x1465d0 particle_block_new_contrail_element
extern "C" long particle_block_new_contrail_element(unsigned char type, long instance_index)
{
    long element_index = datum_new(g_contrail_chain_data);
    s_contrail_chain_element *element = particle_block_contrail_chain_element_get(element_index);
    element->instance_index = instance_index;
    s_particle_instance *instance = datum_get<s_particle_instance>(g_particle_instance_data, element->instance_index);
    s_effect_contrail_location *location = datum_get<s_effect_contrail_location>(g_particle_instance_location_data, instance->location_index);
    c_light_volume_system *owner = datum_get<c_light_volume_system>(g_particle_instance_owner_data, location->owner_index);
    s_contrail_effect_definition *definition = TAG_GET('cntl', s_contrail_effect_definition, owner->m_definition_index);
    s_effect_entry *entry = &definition->entries[instance->definition_index];
    element->first_link = -1;
    element->link_count = 0;
    unsigned long profile = 0;
    if (entry->profile_bytes > 0)
    {
        s_particle_system_limits_owner *limits = g_effect_budget_limits;
        do
        {
            long link_index = particle_block_contrail_chain_link_new(limits, type, element->first_link);
            if (link_index == -1)
            {
                g_contrail_element_alloc_success[type] = 0;
                particle_block_contrail_chain_element_delete(element_index);
                return -1;
            }
            element->first_link = link_index;
            profile += 16;
        } while (profile < entry->profile_bytes);
    }
    g_contrail_element_alloc_success[type] = 1;
    return element_index;
}
template<> __forceinline c_rasterizer_constant_table_ref s_gpu_pixel_shader<s_gpu_empty_constant_list>::initialize_constant_table() {
    c_rasterizer_pixel_shader *shader = (c_rasterizer_pixel_shader *)tag_get('pixl', shader_reference->index);
    c_rasterizer_compiled_pixel_shader *compiled_shader = shader->get_compiled_shader(_entry_point_default, 0);
    c_rasterizer_constant_table_ref result;
    result.table = compiled_shader ? compiled_shader->get_constant_table() : 0;
    return result;
}
class c_particle_definition;
class c_particle_state_list;
class c_particle_location;
struct s_particle_emitter_gpu_block;
class c_constant_table;
// @address 0x7be7d0 g_particle_spawn_shaders
#if !defined(_XBOX) || defined(__clang__)
extern "C" {
s_particle_spawn_shader_pair g_particle_spawn_shaders =
{
    { 27, 0, { { 0, g_particle_spawn_shaders.vertex_shader.constants.slots, 1, 0, {} } }, 10 },
    { 27, 0, { 0, g_particle_spawn_shaders.pixel_shader.constants.slots, 1, 0, {} } }
};
}
#endif
template<class ConstantList>
__forceinline c_rasterizer_constant_table_ref s_gpu_vertex_shader<ConstantList>::initialize_constant_table()
    {
        const c_rasterizer_vertex_shader *vertex_shader = (const c_rasterizer_vertex_shader *)tag_get('vtsh', shader_reference->index);
        c_rasterizer_compiled_vertex_shader *compiled_shader
            = (c_rasterizer_compiled_vertex_shader *)vertex_shader->get_compiled_shader((e_vertex_type)vertex_type, _entry_point_default, 0);
        return get_shader_constant_table(compiled_shader);
    }
template<class ConstantList>
__forceinline void s_gpu_vertex_shader<ConstantList>::initialize_shader(c_rasterizer_constant_table_ref table_ref)
    {
        constants.table = table_ref.table;
        for (long index = 0; index < ConstantList::slot_count; ++index) {
            long constant_index = gpu_constant_table_find_entry(constants.table, 0x80000);
            if (constant_index != -1)
                constants.slots[index].entry = constants.table->entries[constant_index];
            constants.slots[index].required = false;
            constants.any_required = constants.any_required || constants.slots[index].required;
        }
    }
// @address 0x7be820 g_particle_update_shaders
#if !defined(_XBOX) || defined(__clang__)
extern "C" {
s_update_shader_pair g_particle_update_shaders =
{
    { 26, 0, { { 0, g_particle_update_shaders.vertex_shader.constants.slots, 6, 0, {} } }, 10 },
    { 26, 0, { 0, g_particle_update_shaders.pixel_shader.constants.slots, 1, 0, {} } }
};
}
#endif
// @address 0x7be8a8 g_particle_render_vertex_shader
// @address 0x7be900 g_particle_render_model_vertex_shader
#if !defined(_XBOX) || defined(__clang__)
extern "C" {
s_gpu_global_vertex_shader<c_particle_emitter_gpu::s_render_vertex_shader_constant_list> g_particle_render_vertex_shader =
{
    0, { { 0, g_particle_render_vertex_shader.constants.slots, 5, 0, {} } }, _vertex_type_particle
};
s_gpu_global_vertex_shader<c_particle_emitter_gpu::s_render_vertex_shader_constant_list> g_particle_render_model_vertex_shader =
{
    0, { { 0, g_particle_render_model_vertex_shader.constants.slots, 5, 0, {} } }, _vertex_type_particle_model
};
}
#endif
class c_rasterizer_vertex_buffer;
class c_rasterizer_vertex_shader;
class c_rasterizer_pixel_shader;
// @address 0x7be978 g_contrail_gpu_vertex_buffer
// @address 0x8c4842 g_contrail_gpu_block_until_idle
// @address 0x8c4841 g_particle_gpu_block_until_idle

static __forceinline void particle_block_emitter_set_spawn_gprs()
{
#if defined(_XBOX) && !defined(__clang__)
    if (g_light_volume_gpu_layout.gpr_allocation != 4)
#else
    if (g_rasterizer_gpr_allocation != 4)
#endif
    {
        (*rasterizer_shader_gpr_get_globals())[0] = 112;
        (*rasterizer_shader_gpr_get_globals())[1] = 16;
        D3DDevice_SetShaderGPRAllocation(c_rasterizer::get_device(), 0, 112, 16);
#if defined(_XBOX) && !defined(__clang__)
        g_light_volume_gpu_layout.gpr_allocation = 4;
#else
        g_rasterizer_gpr_allocation = 4;
#endif
    }
}
static __forceinline void particle_block_emitter_set_spawn_shader_pair()
{
    ((s_gpu_constant_table_base *)&g_contrail_spawn_shaders.vertex_shader.constants)->own_constants();
    c_rasterizer::set_vertex_shader(static_cast<const c_rasterizer_vertex_shader *>(tag_get('vtsh',
        g_contrail_spawn_shaders.vertex_shader.shader_reference->index)),
                 (e_vertex_type)g_contrail_spawn_shaders.vertex_shader.vertex_type, _transfer_vector_vertex_none, _entry_point_default);
    ((s_gpu_constant_table_base *)&g_contrail_spawn_shaders.pixel_shader.constants)->own_constants();
    c_rasterizer::set_pixel_shader(
        static_cast<const c_rasterizer_pixel_shader *>(tag_get('pixl', g_contrail_spawn_shaders.pixel_shader.shader_reference->index)),
        _entry_point_default);
}
static __forceinline void particle_block_emitter_unset_spawn_shader_pair()
{
    ((s_gpu_constant_table_base *)&g_contrail_spawn_shaders.vertex_shader.constants)->disown_constants();
    ((s_gpu_constant_table_base *)&g_contrail_spawn_shaders.pixel_shader.constants)->disown_constants();
}
static __forceinline void particle_block_emitter_set_spawn_memexport()
{
    s_gpu_constant_table_base *table = (s_gpu_constant_table_base *)&g_contrail_spawn_shaders.vertex_shader.constants;
    ((c_rasterizer_constant_table_ref const *)table)
        ->set_constants(table->descs, (void const *)(&c_contrail_gpu::x_layout.bounds[0].bounds));
}
// @address 0x14ded0 ?spawn_all@c_particle_emitter_gpu@@SAXXZ
void c_particle_emitter_gpu::spawn_all()
{
    if (c_contrail_gpu::x_queued_buffer_system.next->count > 0) {
        particle_block_emitter_set_spawn_gprs();
        c_rasterizer::set_render_target(0, (c_rasterizer::e_surface)0);
        D3DDevice_SetRenderState_ColorWriteEnable(c_rasterizer::get_device(), 0);
        c_rasterizer::set_render_target(1, (c_rasterizer::e_surface)0);
        c_rasterizer::set_render_target(2, (c_rasterizer::e_surface)0);
        c_rasterizer::get_device()->SetVertexShaderConstantF(32, g_gpu_particle_default_constants, 1);
        c_contrail_gpu::x_queued_buffer_system.swap();
        c_rasterizer::set_vertex_stream(&c_contrail_gpu::x_queued_buffer_system.current->first);
        c_rasterizer::set_vertex_stream(&c_contrail_gpu::x_queued_buffer_system.current->second);
        particle_block_emitter_set_spawn_shader_pair();
        c_rasterizer::get_device()->BeginExport(0, g_contrail_gpu_vertex_buffer.get_d3d_buffer(), 0);
        particle_block_emitter_set_spawn_memexport();
        c_rasterizer::draw_primitive(c_rasterizer_index_buffer::_primitive_type_point_list, 0, c_contrail_gpu::x_queued_buffer_system.current->count);
        c_rasterizer::get_device()->EndExport(0, g_contrail_gpu_vertex_buffer.get_d3d_buffer(), 0);
        c_rasterizer::clear_vertex_stream(&c_contrail_gpu::x_queued_buffer_system.current->first);
        c_rasterizer::clear_vertex_stream(&c_contrail_gpu::x_queued_buffer_system.current->second);
        particle_block_emitter_unset_spawn_shader_pair();
        if (g_contrail_gpu_block_until_idle)
            c_rasterizer::get_device()->BlockUntilIdle();
        else
#if defined(_XBOX) && !defined(__clang__)
            g_light_volume_gpu_layout.fence = c_rasterizer::get_device()->InsertFence();
#else
            g_game_state_render_fence = c_rasterizer::get_device()->InsertFence();
#endif
    }
}
// (the update shader pair of the contrail GPU, whose counter block owns the u4 memexport values)
class c_contrail_states;
// @address 0x7bea08 g_contrail_update_shaders
// @address 0x8c43b4 k_contrail_default_used_flags

// @address 0x84740 ?data_iterator_next@@YAPAXPAUs_data_iterator@@@Z
class c_contrail_gpu;
// @address 0x14e138 ?frame_advance_all@c_particle_emitter_gpu@@SAXM@Z
void c_particle_emitter_gpu::frame_advance_all(float elapsed_time)
{
    if (((s_data_array *)g_contrail_gpu_data)->first_unallocated > 0) {
        c_rasterizer::set_render_target(0, (c_rasterizer::e_surface)0);
        D3DDevice_SetRenderState_ColorWriteEnable(c_rasterizer::get_device(), 0);
        c_rasterizer::set_render_target(1, (c_rasterizer::e_surface)0);
        c_rasterizer::set_render_target(2, (c_rasterizer::e_surface)0);
        c_rasterizer::set_vertex_stream(&g_contrail_gpu_vertex_buffer);
        ((s_gpu_constant_table_base *)&g_contrail_update_shaders.constants)->own_constants();
        c_rasterizer::set_vertex_shader(
            static_cast<const c_rasterizer_vertex_shader *>(tag_get('vtsh', g_contrail_update_shaders.shader_reference->index)),
            (e_vertex_type)g_contrail_update_shaders.vertex_type,
            _transfer_vector_vertex_none, _entry_point_default);
        ((s_gpu_constant_table_base *)&g_contrail_update_shaders.pixel.constants)->own_constants();
        c_rasterizer::set_pixel_shader(
            static_cast<const c_rasterizer_pixel_shader *>(tag_get('pixl', g_contrail_update_shaders.pixel.shader_reference->index)),
            _entry_point_default);
        c_rasterizer::get_device()->BeginExport(0, g_contrail_gpu_vertex_buffer.get_d3d_buffer(), 0);
        s_gpu_constant_table_base *constant_table = (s_gpu_constant_table_base *)&g_contrail_update_shaders.constants;
        ((c_rasterizer_constant_table_ref const *)constant_table)
            ->set_constants(constant_table->descs, (void const *)(&c_contrail_gpu::x_layout.bounds[0].bounds));
        c_contrail_states states;
        c_data_iterator<c_contrail_gpu> iterator;
        iterator.begin((s_data_array *)g_contrail_gpu_data);
        while (iterator.next())
            particle_block_frame_advance_contrail(iterator.get_datum(), elapsed_time, &states);
        c_rasterizer::get_device()->EndExport(0, g_contrail_gpu_vertex_buffer.get_d3d_buffer(), 0);
        ((s_gpu_constant_table_base *)&g_contrail_update_shaders.constants)->disown_constants();
        ((s_gpu_constant_table_base *)&g_contrail_update_shaders.pixel.constants)->disown_constants();
#if defined(_XBOX) && !defined(__clang__)
        g_light_volume_gpu_layout.fence = c_rasterizer::get_device()->InsertFence();
#else
        g_game_state_render_fence = c_rasterizer::get_device()->InsertFence();
#endif
    }
}
// The s_gpu_storage vtables and virtuals the particle layouts instantiate.
// @address 0x4b89c ??_7?$s_gpu_storage@Treal_vector4d@@$0A@$0CG@$06$01$02$0BKCDKG@$0?0@@6B@
// @address 0x4604e0 ?layout@?$s_gpu_storage@Treal_vector4d@@$0A@$0CG@$06$01$02$0BKCDKG@$0?0@@UAAXKK@Z
// @address 0x4604f8 ?refresh@?$s_gpu_storage@Treal_vector4d@@$0A@$0CG@$06$01$02$0BKCDKG@$0?0@@UAAXXZ
// @address 0x42e748 ?size@?$s_gpu_storage@Treal_vector4d@@$0A@$0CG@$06$01$02$0BKCDKG@$0?0@@UAAKXZ
// @address 0x45fc28 ?kind@?$s_gpu_storage@Treal_vector4d@@$0A@$0CG@$06$01$02$0BKCDKG@$0?0@@UAAJXZ
// @address 0x45fc30 ?declaration_type@?$s_gpu_storage@Treal_vector4d@@$0A@$0CG@$06$01$02$0BKCDKG@$0?0@@UAAKXZ
// @address 0x4b8b0 ??_7?$s_gpu_storage@_K$0A@$0CA@$06$00$0BA@$0BKCDGA@$04@@6B@
// @address 0x460528 ?layout@?$s_gpu_storage@_K$0A@$0CA@$06$00$0BA@$0BKCDGA@$04@@UAAXKK@Z
// @address 0x460540 ?refresh@?$s_gpu_storage@_K$0A@$0CA@$06$00$0BA@$0BKCDGA@$04@@UAAXXZ
// @address 0x42e650 ?size@?$s_gpu_storage@_K$0A@$0CA@$06$00$0BA@$0BKCDGA@$04@@UAAKXZ
// @address 0x42e748 ?kind@?$s_gpu_storage@_K$0A@$0CA@$06$00$0BA@$0BKCDGA@$04@@UAAJXZ
// @address 0x45fc40 ?declaration_type@?$s_gpu_storage@_K$0A@$0CA@$06$00$0BA@$0BKCDGA@$04@@UAAKXZ
// @address 0x4b8c4 ??_7?$s_gpu_storage@_K$0A@$0BK@$0A@$00$09$0BKCAFK@$03@@6B@
// @address 0x460528 ?layout@?$s_gpu_storage@_K$0A@$0BK@$0A@$00$09$0BKCAFK@$03@@UAAXKK@Z
// @address 0x460570 ?refresh@?$s_gpu_storage@_K$0A@$0BK@$0A@$00$09$0BKCAFK@$03@@UAAXXZ
// @address 0x42e650 ?size@?$s_gpu_storage@_K$0A@$0BK@$0A@$00$09$0BKCAFK@$03@@UAAKXZ
// @address 0x45fc50 ?kind@?$s_gpu_storage@_K$0A@$0BK@$0A@$00$09$0BKCAFK@$03@@UAAJXZ
// @address 0x45fc58 ?declaration_type@?$s_gpu_storage@_K$0A@$0BK@$0A@$00$09$0BKCAFK@$03@@UAAKXZ
// @address 0x4b8d8 ??_7?$s_gpu_storage@K$0A@$0BJ@$01$00$06$0CMCCFJ@$00@@6B@
// @address 0x4605a0 ?layout@?$s_gpu_storage@K$0A@$0BJ@$01$00$06$0CMCCFJ@$00@@UAAXKK@Z
// @address 0x4605b8 ?refresh@?$s_gpu_storage@K$0A@$0BJ@$01$00$06$0CMCCFJ@$00@@UAAXXZ
// @address 0x3fc550 ?size@?$s_gpu_storage@K$0A@$0BJ@$01$00$06$0CMCCFJ@$00@@UAAKXZ
// @address 0x4313c8 ?kind@?$s_gpu_storage@K$0A@$0BJ@$01$00$06$0CMCCFJ@$00@@UAAJXZ
// @address 0x45fc68 ?declaration_type@?$s_gpu_storage@K$0A@$0BJ@$01$00$06$0CMCCFJ@$00@@UAAKXZ
// @address 0x4b8ec ??_7?$s_gpu_storage@K$0A@$0BJ@$0A@$00$08$0CMCAFJ@$00@@6B@
// @address 0x4605a0 ?layout@?$s_gpu_storage@K$0A@$0BJ@$0A@$00$08$0CMCAFJ@$00@@UAAXKK@Z
// @address 0x4605e8 ?refresh@?$s_gpu_storage@K$0A@$0BJ@$0A@$00$08$0CMCAFJ@$00@@UAAXXZ
// @address 0x3fc550 ?size@?$s_gpu_storage@K$0A@$0BJ@$0A@$00$08$0CMCAFJ@$00@@UAAKXZ
// @address 0x42e810 ?kind@?$s_gpu_storage@K$0A@$0BJ@$0A@$00$08$0CMCAFJ@$00@@UAAJXZ
// @address 0x45fc78 ?declaration_type@?$s_gpu_storage@K$0A@$0BJ@$0A@$00$08$0CMCAFJ@$00@@UAAKXZ
// @address 0x4b900 ??_7?$s_gpu_storage@K$0A@$0BP@$06$00$0P@$0CMCDFP@$02@@6B@
// @address 0x4605a0 ?layout@?$s_gpu_storage@K$0A@$0BP@$06$00$0P@$0CMCDFP@$02@@UAAXKK@Z
// @address 0x460618 ?refresh@?$s_gpu_storage@K$0A@$0BP@$06$00$0P@$0CMCDFP@$02@@UAAXXZ
// @address 0x3fc550 ?size@?$s_gpu_storage@K$0A@$0BP@$06$00$0P@$0CMCDFP@$02@@UAAKXZ
// @address 0x42e678 ?kind@?$s_gpu_storage@K$0A@$0BP@$06$00$0P@$0CMCDFP@$02@@UAAJXZ
// @address 0x45fc88 ?declaration_type@?$s_gpu_storage@K$0A@$0BP@$06$00$0P@$0CMCDFP@$02@@UAAKXZ
// @address 0x4b914 ??_7?$s_gpu_storage@K$00$05$0A@$01$05$0BICIIG@$0A@@@6B@
// @address 0x4605a0 ?layout@?$s_gpu_storage@K$00$05$0A@$01$05$0BICIIG@$0A@@@UAAXKK@Z
// @address 0x460648 ?refresh@?$s_gpu_storage@K$00$05$0A@$01$05$0BICIIG@$0A@@@UAAXXZ
// @address 0x3fc550 ?size@?$s_gpu_storage@K$00$05$0A@$01$05$0BICIIG@$0A@@@UAAKXZ
// @address 0x4311b0 ?kind@?$s_gpu_storage@K$00$05$0A@$01$05$0BICIIG@$0A@@@UAAJXZ
// @address 0x45fc98 ?declaration_type@?$s_gpu_storage@K$00$05$0A@$01$05$0BICIIG@$0A@@@UAAKXZ
// @address 0x4b928 ??_7?$s_gpu_storage@K$0A@$06$00$01$0BB@$0CKCBIH@$01@@6B@
// @address 0x4605a0 ?layout@?$s_gpu_storage@K$0A@$06$00$01$0BB@$0CKCBIH@$01@@UAAXKK@Z
// @address 0x460678 ?refresh@?$s_gpu_storage@K$0A@$06$00$01$0BB@$0CKCBIH@$01@@UAAXXZ
// @address 0x3fc550 ?size@?$s_gpu_storage@K$0A@$06$00$01$0BB@$0CKCBIH@$01@@UAAKXZ
// @address 0x45fca8 ?kind@?$s_gpu_storage@K$0A@$06$00$01$0BB@$0CKCBIH@$01@@UAAJXZ
// @address 0x45fcb0 ?declaration_type@?$s_gpu_storage@K$0A@$06$00$01$0BB@$0CKCBIH@$01@@UAAKXZ

// @address 0x45f390 ??0s_layout@c_particle_emitter_gpu@@QAA@XZ
inline c_particle_emitter_gpu::s_layout::s_layout() :
    position(0, 1),
    velocity(0, 2),
    random(0, 3),
    random2(0, 4),
    scale(3, 2),
    time(3, 3),
    size(3, 4),
    texture_coordinates(3, 5),
    rotation(4, 1),
    primary_color(8, 0),
    color(8, 1)
{
    slots[0] = &position;
    slots[1] = &velocity;
    slots[2] = &random;
    slots[3] = &random2;
    slots[4] = &scale;
    slots[5] = &time;
    slots[6] = &size;
    slots[7] = &texture_coordinates;
    slots[8] = &rotation;
    slots[9] = &primary_color;
    slots[10] = &color;
}

// @address 0x45f5b8 ?initialize_shaders@c_particle_emitter_gpu@@CAXXZ
void c_particle_emitter_gpu::initialize_shaders()
{
    g_particle_spawn_shaders.initialize();
    g_particle_update_shaders.initialize();
}

// @address 0x460078 ??0particle_block_queue_frame@@QAA@XZ
particle_block_queue_frame::particle_block_queue_frame()
{
    first.format = 16;
    first.element_count = 0x240;
    first.element_size = 1;
    second.format = 17;
    second.element_count = 0x240;
    second.element_size = 1;
}

// @address 0x460108 ?initialize@?$s_gpu_vertex_shader@Us_spawn_vertex_shader_constant_list@c_particle_emitter_gpu@@@@QAAXXZ
template<> void s_gpu_vertex_shader<c_particle_emitter_gpu::s_spawn_vertex_shader_constant_list>::initialize()
{
    // match: 0x460108: value range assumption guides branch lowering
    __assume(this == &g_particle_spawn_shaders.vertex_shader);
    s_game_globals *game_globals = global_game_globals;
    c_rasterizer_globals *rasterizer_globals;
    if (game_globals->rasterizer_globals_ref.index != -1)
        rasterizer_globals = (c_rasterizer_globals *)tag_get('rasg', game_globals->rasterizer_globals_ref.index);
    else
        rasterizer_globals = 0;
    shader_reference = &rasterizer_globals->get_explicit_shaders(shader_index)->vertex;
    const c_rasterizer_vertex_shader *vertex_shader = (const c_rasterizer_vertex_shader *)tag_get('vtsh', shader_reference->index);
    c_rasterizer_compiled_vertex_shader *compiled_shader = (c_rasterizer_compiled_vertex_shader *)vertex_shader->get_compiled_shader((e_vertex_type)vertex_type,
        _entry_point_default, 0);
    c_rasterizer_constant_table_ref table_ref = get_shader_constant_table(compiled_shader);
    constants.table = table_ref.table;
    long constant_index = gpu_constant_table_find_entry(constants.table, 0x80000);
    if (constant_index != -1)
        constants.slots[0].entry = constants.table->entries[constant_index];
    constants.slots[0].required = false;
    constants.any_required = constants.any_required || constants.slots[0].required;
}


// @address 0x460210 ?initialize@?$s_gpu_pixel_shader@Us_gpu_empty_constant_list@@@@QAAXXZ
template<> void s_gpu_pixel_shader<s_gpu_empty_constant_list>::initialize() {
    s_game_globals *game_globals = global_game_globals;
    c_rasterizer_globals *rasterizer_globals = game_globals->rasterizer_globals_ref.index != -1
        ? (c_rasterizer_globals *)tag_get('rasg', game_globals->rasterizer_globals_ref.index) : 0;
    shader_reference = const_cast<s_tag_reference *>(&rasterizer_globals->get_explicit_shaders(shader_index)->pixel);
    constants.table = initialize_constant_table().table;
}
namespace {
static __forceinline const s_render_method_shader_definition *particle_render_method_get(long index)
{
    return (const s_render_method_shader_definition *)tag_get('rmdf', index);
}
}
// @address 0x4602b8 ?initialize_constant_table@?$s_gpu_global_vertex_shader@Us_render_vertex_shader_constant_list@c_particle_emitter_gpu@@@@AAAPAUs_entry_table@@XZ
template<> inline s_entry_table *s_gpu_global_vertex_shader<c_particle_emitter_gpu::s_render_vertex_shader_constant_list>::initialize_constant_table()
{
    const s_render_method_shader_definition *render_method = particle_render_method_get(shader_reference->index);
    s_rasterizer_global_vertex_shader_definition *vertex_shader = (s_rasterizer_global_vertex_shader_definition *)tag_get('glvs',
        render_method->shared_vertex_shader.index);
    s_tag_block *vertex_entries = &((s_tag_block *)vertex_shader->vertex_types.address)[vertex_type];
    s_rasterizer_global_shader_entry_point *vertex_entry = (s_rasterizer_global_shader_entry_point *)vertex_entries->address;
    c_rasterizer_compiled_vertex_shader *compiled_shader = &((c_rasterizer_compiled_vertex_shader *)vertex_shader->shaders.address)[vertex_entry->shader_index];
    return get_shader_constant_table(compiled_shader).table;
}

/* Binds the slot set to a table: each slot takes the table entry with its
   key (if any) and its required flag; any_required accumulates the flags.
   The template family differs in the slot count and the two per-slot tables. */
void particle_block_bind_render_slot_set(
    s_gpu_global_shader<c_particle_emitter_gpu::s_render_vertex_shader_constant_list> *slot_set, s_entry_table *table)
{
  slot_set->shader = table;
  const int *key = c_particle_emitter_gpu::s_render_vertex_shader_constant_list::x_keys;
  for (int i = 0; i < 5; i++) {
    int index = gpu_constant_table_find_entry(slot_set->shader, *key);
    if (index != -1)
      static_cast<s_constant_entry &>(slot_set->slots[i]) = slot_set->shader->entries[index];
    slot_set->slots[i].owned = c_particle_emitter_gpu::s_render_vertex_shader_constant_list::x_in_place[i];
    slot_set->valid = slot_set->valid || slot_set->slots[i].owned;
    key++;
  }
}

/* Binds the slot set to a table: each slot takes the table entry with its
   key (if any) and its required flag; any_required accumulates the flags.
   The template family differs in the slot count and the two per-slot tables. */
template<>
void s_gpu_shader<c_particle_emitter_gpu::s_update_vertex_shader_constant_list>::initialize(c_rasterizer_constant_table_ref table_ref)
{
  this->table = table_ref.table;
  const int *key = c_particle_emitter_gpu::s_update_vertex_shader_constant_list::x_keys;
  for (int i = 0; i < 6; i++) {
    int index = gpu_constant_table_find_entry(this->table, *key);
    if (index != -1)
      this->slots[i].entry = this->table->entries[index];
    this->slots[i].required = c_particle_emitter_gpu::s_update_vertex_shader_constant_list::x_in_place[i];
    this->any_required = this->any_required || this->slots[i].required;
    key++;
  }
}

// @address 0x335648 ?__tls_set_g_particle_emitter_gpu_data_array_allocator@@YAXPAX@Z
void __tls_set_g_particle_emitter_gpu_data_array_allocator(void *object)
{
    data_set_new_base_address(&g_widget_data, (s_data_array *)object);
}
// @address 0x559760 ?__tls_set_g_particle_row_data_array_allocator@@YAXPAX@Z
void __tls_set_g_particle_row_data_array_allocator(void *object)
{
    data_set_new_base_address(&dynamic_firing_set_data, (s_data_array *)object);
}
__forceinline c_particle_system *system_get(long index) {return &*((c_particle_system *)((s_data_array *)g_particle_system_data_array)->data + (index&0xffff));}
__forceinline c_particle_system_definition *get_system_definition(c_particle_system *s)
{
    const s_effect_definition *e = TAG_GET('effe', const s_effect_definition, s->m_tag_index);
    return &((s_particle_movement_effect_event *)e->events.address)[s->m_def_index].particle_systems[s->m_sub_index];
}
__forceinline c_particle_emitter *emitter_get(long index) {return &*((c_particle_emitter *)((s_data_array *)g_particle_emitter_data)->data + (index&0xffff));}
__forceinline c_particle_location *get_location(c_particle_emitter *e) {
    return &*((c_particle_location *)((s_data_array *)g_particle_location_data_array)->data + (e->m_location_index&0xffff));
}
__forceinline unsigned char get_first_person(c_particle_location *l) {return l->state&1;}
__forceinline const real_vector2d *get_uv_scroll_rate(const c_particle_emitter_definition *d) {return &d->uv;}
__forceinline long get_particle_definition_index(c_particle_system *s) {return get_system_definition(s)->get_particle_definition_index();}
__forceinline unsigned char get_override_near_fade(c_particle_system_definition *s) {return (s->flags>>11)&1;}
__forceinline void set_struct(const s_gpu_constant_table_base *table, long index, const render_state *state)
{
    ((c_rasterizer_constant_table_ref const *)table)
        ->set_constants(&table->descs[index], (void const *)(state));
}




// @address 0x8c43ac g_particle_default_used_states

// Bit test in mask spelling; the shifted flag_test compiles differently at these sites.
static __forceinline bool flag_test_mask(unsigned long flags, long bit)
{
    return (flags & (1UL << bit)) != 0;
}
static __forceinline s_particle_block_element *particle_block_element_try_get(long element_index)
{
    s_particle_block_element *result = 0;
    s_data_array *elements = (s_data_array *)g_particle_emitter_gpu_data;
    if (element_index != -1 && (unsigned short)element_index < elements->first_unallocated) {
        s_particle_block_element *element = (s_particle_block_element *)(elements->data + (unsigned short)element_index * elements->size);
        if (element->identifier != 0 && element->identifier == (element_index >> 16))
            result = element;
    }
    return result;
}

// @address 0x150508 particle_block_frame_advance_emitter
extern "C" void particle_block_frame_advance_emitter(s_particle_emitter_gpu_block *object, float dt, c_constant_table *table, c_particle_state_list *states)
{
    c_particle_emitter *emitter = emitter_get(object->emitter_index);
    if (object->block_index == -1 || !flag_test_mask(emitter->m_flags, 3))
        return;

    c_particle_system *system = system_get(object->source);
    c_particle_system_definition *definition = get_system_definition(system);

    BOOL use_fade = flag_test_mask(system->m_flags, 7) && system->m_effect_index != -1;
    c_rasterizer::get_device()->SetVertexShaderConstantB(20, &use_fade, 1);
    if (use_fade)
        render_setup_fade_constants(0, definition->near_cutoff_distance, definition->far_cutoff_distance);

    BOOL use_occlusion = flag_test_mask(system->m_flags, 8) && flag_test_mask(definition->flags, 13) && system->m_effect_index != -1;
    c_rasterizer::get_device()->SetVertexShaderConstantB(21, &use_occlusion, 1);
    if (use_occlusion)
        render_setup_occlusion_constants(0);

    states->set_particle_system((const c_particle_system *)system);
    (states)->set_particle_location((const c_particle_location *)get_location(emitter));
    particle_block_set_emitter_args(object, table);
    ((const c_particle_emitter_gpu *)object)->set_shader_state<c_particle_emitter_gpu::s_update_vertex_shader_constant_list>(
        (const s_gpu_constant_table<c_particle_emitter_gpu::s_update_vertex_shader_constant_list> *)table, states);
    ((const c_particle_emitter_gpu *)object)->set_shader_update_state<c_particle_emitter_gpu::s_update_vertex_shader_constant_list>(
        (const s_gpu_constant_table<c_particle_emitter_gpu::s_update_vertex_shader_constant_list> *)table, states);
    c_rasterizer::get_device()->SetVertexShaderConstantF(22, g_gpu_particle_default_constants, 1);

    long element_index = object->block_index;
    s_particle_block_element *element;
    while ((element = particle_block_element_try_get(element_index)) != 0) {
        float lifetime = element->value;
        if (lifetime < 0.0f) {
            object->count -= element->state;
            if (element_index == object->block_index)
                object->block_index = element->next_index;
            long next_index = element->next_index;
            particle_block_delete_element(element_index, false);
            element_index = next_index;
        } else {
            element->value = lifetime - dt;
            c_rasterizer::draw_primitive(c_rasterizer_index_buffer::_primitive_type_point_list, ((element->index + 1) << 4) - element->state, element->state);
            if (g_particle_gpu_block_until_idle)
                c_rasterizer::get_device()->BlockUntilIdle();
            element_index = element->next_index;
        }
    }
}
// @address 0x150820 particle_block_set_emitter_args
extern "C" void particle_block_set_emitter_args(s_particle_emitter_gpu_block *object, c_constant_table *table)
{
    c_particle_system *system = system_get(object->source);
    const c_particle_system_definition *definition = get_system_definition(system);
    const c_particle_emitter_definition *emitter = definition->get_emitter(object->unit_index);
    table->set_scaled(2, emitter->value_a, emitter->value_b);
    table->set(3, (unsigned char)emitter->count_a, emitter->arg_a);
    table->set(4, (unsigned char)emitter->count_b, emitter->arg_b);
}
// @address 0x182818 ??$set_shader_render_state@Us_render_vertex_shader_constant_list@c_particle_emitter_gpu@@@c_particle_emitter_gpu@@ABAXPBU?$s_gpu_constant_table@Us_render_vertex_shader_constant_list@c_particle_emitter_gpu@@@@K@Z
template<> void c_particle_emitter_gpu::set_shader_render_state<c_particle_emitter_gpu::s_render_vertex_shader_constant_list>(
 const s_gpu_constant_table<s_render_vertex_shader_constant_list> *table, unsigned long flags) const {
 c_particle_system *system=system_get(m_system_index);
 const c_particle_system_definition *definition=get_system_definition(system);
 const c_particle_definition *particle=TAG_GET('prt3', c_particle_definition, get_particle_definition_index(system));
 c_particle_emitter *emitter=emitter_get(m_emitter_index);
 const c_particle_emitter_definition *emitter_def=definition->get_emitter(m_definition_index);
 render_state state;
 state.flags.x=(float)particle->flags;
 state.flags2.x=(float)particle->flags2;
 state.flags3.x=(float)particle->flags3;
 state.first_person.x=(float)get_first_person(get_location(emitter));
 state.scalar.x=particle->scalar;
 state.uv=*get_uv_scroll_rate(emitter_def);
 state.time.x=(float)game_time_get()*game_tick_length();
 state.count.x=(float)particle->count;
 state.render_flags.x=(float)flags;
 state.inverse_far=1.0f/definition->far_fade;
 state.near_fade=get_override_near_fade((c_particle_system_definition*)definition)?definition->override_fade:definition->near_fade;
 state.inverse_angle=1.0f/(particle->angle*0.017453292f);
 state.angle=particle->near_fade*0.017453292f;
 set_struct((const s_gpu_constant_table_base *)table,3,&state);
}

static __forceinline s_particle_block_element *particle_block_link_get(long datum_index)
{
    return (s_particle_block_element *)((s_data_array *)g_contrail_gpu_row_data)->data + (datum_index & 0xffff);
}

// @address 0x4613f0 particle_block_delete_contrail_block
// 0x4613f0: unlinks a block from its neighbours and deletes it, and with delete_following the blocks after it too
extern "C" void particle_block_delete_contrail_block(long block_index, bool delete_following)
{
    if (block_index != -1)
    {
        s_particle_block_element *block = particle_block_link_get(block_index);
        if (block->previous_index != -1)
            particle_block_link_get(block->previous_index)->next_index = block->next_index;
        if (block->next_index != -1)
            particle_block_link_get(block->next_index)->previous_index = block->previous_index;
        if (delete_following)
            particle_block_delete_contrail_block(block->next_index, true);
        ::datum_delete((s_data_array *)g_contrail_gpu_row_data, block_index);
    }
}

// @address 0x463828 particle_block_set_contrail_link_array
extern "C" void particle_block_set_contrail_link_array(void *address)
{
    data_set_new_base_address(&g_contrail_gpu_link_data, (s_data_array *)address);
}


// @address 0x7bf420 g_contrail_frame_shader_pair

// @address 0x463af0 particle_block_initialize_contrail_shaders
extern "C" void particle_block_initialize_contrail_shaders(void)
{
    g_contrail_frame_shader_pair.initialize();
}


// @address 0x463df8 ?deallocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_animation_threads_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<>
void t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_animation_threads_allocator>::deallocate(void *memory)
{
    g_contrail_chain_data = 0;
    restricted_region_free_member(2, m_member_index);
}

// @address 0x463eb8 ?deallocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?particle_block_set_contrail_link_array@@YAXPAX@Z@@UAAXPAX@Z
template<>
void t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &particle_block_set_contrail_link_array>::deallocate(void *memory)
{
    g_contrail_gpu_link_data = 0;
    restricted_region_free_member(2, m_member_index);
}

// (restricted memory members: field layout 2)

// @address 0x463840 particle_block_update_entry_bounds
// 0x463840: sets the thread's current value (nothing when unchanged or zero), passes it on, then refreshes
// each of the 3 entries and keeps a copy of its bounds
class common_gpu_vertex_buffer;

// @address 0x7bf2f4 g_contrail_frame_vertex_buffer

void particle_block_update_entry_bounds(void *value)
{
    if (value != g_light_volume_gpu_storage_address && value != 0)
    {
        g_light_volume_gpu_storage_address = value;
        common_gpu_vertex_buffer_set_address(&g_contrail_frame_vertex_buffer, value);
        for (unsigned int i = 0; i < 3; i++)
        {
            s_gpu_storage_interface *entry = c_light_volume_gpu::x_layout.entries[i];
            entry->refresh();
            c_light_volume_gpu::x_layout.bounds[i].bounds = entry->bounds;
        }
    }
}

// @address 0x463ed8 ?reserve_memory@?$t_restricted_allocation_manager@$01$0A@$0A@$1?particle_block_update_entry_bounds@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
template<>
void *t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &particle_block_update_entry_bounds>::reserve_memory(const char *name, const char *type, unsigned int size,
    long alignment_bits)
{
    long member_index = g_restricted_regions[2].add_member(name, type, size, alignment_bits,
        particle_block_update_entry_bounds, 0, 0);
    unsigned int offset = g_restricted_regions[2].get_member_offset(member_index);
    particle_block_update_entry_bounds(g_restricted_address[2] + offset);
    m_member_index = member_index;
    m_thread_id = g_registered_thread_index;
    return g_restricted_address[2] + g_restricted_regions[2].get_member_offset(member_index);
}

// @address 0x463f60 ?allocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?particle_block_update_entry_bounds@@YAXPAX@Z@@UAAPAXIPBD@Z
template<>
void *t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &particle_block_update_entry_bounds>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// @address 0x463f78 ?deallocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?particle_block_update_entry_bounds@@YAXPAX@Z@@UAAXPAX@Z
template<>
void t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &particle_block_update_entry_bounds>::deallocate(void *)
{
    restricted_region_free_member(2, m_member_index);
}

extern "C" {
// @address 0x4b9f8 ??_7?$t_restricted_allocation_manager@$01$0A@$0A@$1?particle_block_update_entry_bounds@@YAXPAX@Z@@6B@
// @address 0x70a300 ??__Eg_light_volume_emitter_allocation@@YAXXZ
// @address 0x70ca50 ??__Fg_light_volume_emitter_allocation@@YAXXZ
// @address 0x7bf2e8 g_light_volume_emitter_allocation
t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &particle_block_update_entry_bounds> g_light_volume_emitter_allocation;
}

// @address 0x4640e0 ?initialize@?$s_gpu_shader@Us_particle_block_contrail_vertex_shader_constant_list@@@@QAAXVc_rasterizer_constant_table_ref@@@Z
// 0x4640e0: binds the slot set to a table (the seven-slot member of the template family of 0x460330)
// @address 0x24bd0 k_particle_block_7slot_required
// @address 0x24bdc k_particle_block_7slot_keys
// @address 0x2490c k_real_math_epsilon_storage
#if !defined(_XBOX) || defined(__clang__)
extern "C" {
const float k_real_math_epsilon_storage = 0.0001f;
bool k_particle_block_7slot_required[7] = { false, false, false, true, true, true, false };
int k_particle_block_7slot_keys[7] = { 0x80000, 0x8000b, 0x80001, 0x80002, 0x80003, 0x80004, 0x8000c };
}
#endif

void s_gpu_shader<s_particle_block_contrail_vertex_shader_constant_list>::initialize(c_rasterizer_constant_table_ref table_ref)
{
  this->shader = table_ref.table;
  const int *key = k_particle_block_7slot_keys;
  for (int i = 0; i < 7; i++) {
    int index = gpu_constant_table_find_entry(this->shader, *key);
    if (index != -1)
      static_cast<s_constant_entry &>(this->slots[i]) = this->shader->entries[index];
    this->slots[i].owned = k_particle_block_7slot_required[i];
    this->valid = this->valid || this->slots[i].owned;
    key++;
  }
}

// @address 0x14e850 ??$set_shader_strip@Us_particle_block_contrail_vertex_shader_constant_list@@@c_contrail_gpu@@ABAXPBU?$s_gpu_constant_table@Us_particle_block_contrail_vertex_shader_constant_list@@@@J@Z
template<>
void c_contrail_gpu::set_shader_strip<s_particle_block_contrail_vertex_shader_constant_list>(
    const s_gpu_constant_table<s_particle_block_contrail_vertex_shader_constant_list> *table, long value) const
{
    real_vector4d all[9];
    all[1].x = visibility_projections_and_volumes_zero;
    all[0].x = (float)value;
    memset(&all[2], 0, sizeof(all) - 2 * sizeof(all[0]));
    long first = first_row_index;
    long row_index = first;
    long count = 0;
    s_contrail_chain_link *row;
    while ((row = particle_block_contrail_chain_link_try_get(g_contrail_gpu_link_data, row_index)) != 0 && count < 8) {
        row_index = row->next;
        count++;
    }
    real_vector4d *entry = &all[count];
    row_index = first;
    while ((row = particle_block_contrail_chain_link_try_get(g_contrail_gpu_link_data, row_index)) != 0) {
        entry->x = (float)(unsigned __int64)row->index;
        row_index = row->next;
        entry--;
    }
    ((c_rasterizer_constant_table_ref const *)(s_gpu_constant_table_base *)table)
        ->set_constants(&table->descs[1], (void const *)(all));
}
// @address 0x182520 ??$set_shader_strip@Us_render_vertex_shader_constant_list@c_light_volume_gpu@@@c_light_volume_gpu@@ABAXPBU?$s_gpu_constant_table@Us_render_vertex_shader_constant_list@c_light_volume_gpu@@@@J@Z
template<>
void c_light_volume_gpu::set_shader_strip<c_light_volume_gpu::s_render_vertex_shader_constant_list>(
    const s_gpu_constant_table<s_render_vertex_shader_constant_list> *table, long value) const
{
    real_vector4d all[9];
    all[1].x = visibility_projections_and_volumes_zero;
    all[0].x = (float)value;
    memset(&all[2], 0, sizeof(all) - 2 * sizeof(all[0]));
    long first = m_first_row_index;
    long row_index = first;
    long count = 0;
    s_contrail_chain_link *row;
    while ((row = particle_block_contrail_chain_link_try_get(g_contrail_gpu_link_data, row_index)) != 0 && count < 8) {
        row_index = row->next;
        count++;
    }
    real_vector4d *entry = &all[count];
    row_index = first;
    while ((row = particle_block_contrail_chain_link_try_get(g_contrail_gpu_link_data, row_index)) != 0) {
        entry->x = (float)(unsigned __int64)row->index;
        row_index = row->next;
        entry--;
    }
    ((c_rasterizer_constant_table_ref const *)(s_gpu_constant_table_base *)table)
        ->set_constants(&table->descs[0], (void const *)(all));
}

// ---- 0x182718 set_shader_variants support ----
static __forceinline const c_particle_system *particle_block_gpu_system_get(long index)
{
    const s_data_array *systems = (const s_data_array *)g_particle_system_data_array;
    return (const c_particle_system *)systems->data + (index & 0xffff);
}


// @address 0x182718 ??$set_shader_variants@Us_render_vertex_shader_constant_list@c_particle_emitter_gpu@@@c_particle_emitter_gpu@@ABAXPBU?$s_gpu_constant_table@Us_render_vertex_shader_constant_list@c_particle_emitter_gpu@@@@@Z
template<>
void c_particle_emitter_gpu::set_shader_variants<c_particle_emitter_gpu::s_render_vertex_shader_constant_list>(
    const s_gpu_constant_table<s_render_vertex_shader_constant_list> *table) const
{
    const c_particle_system *system = particle_block_gpu_system_get(m_system_index);
    const c_particle_definition *definition = system->get_particle_definition();
    ((c_rasterizer_constant_table_ref const *)(s_gpu_constant_table_base *)table)
        ->set_constants(&table->descs[2], (void const *)(definition->m_sprite_data));
    c_rasterizer_constant_desc desc;
    desc = table->descs[1];
    desc.count = (unsigned char)definition->m_sprite_count;
    ((c_rasterizer_constant_table_ref const *)(s_gpu_constant_table_base *)table)
        ->set_constants(&desc, (void const *)(definition->m_frames_data));
    if ((_rotl(definition->flags2, 17) & 1) != 0) {
        const c_particle_model_definition *model_definition = c_particle_model_definition::get(c_tag_index(definition->particle_model));
        desc = table->descs[0];
        const void *frames = model_definition->m_variants;
        desc.count = (unsigned char)model_definition->m_variant_count;
        ((c_rasterizer_constant_table_ref const *)(s_gpu_constant_table_base *)table)
            ->set_constants(&desc, (void const *)(frames));
    }
}

// ---- 0x150940 set_shader_state (27-entry conditional copy) ----


// @address 0x150940 ??$set_shader_state@Us_update_vertex_shader_constant_list@c_particle_emitter_gpu@@@c_particle_emitter_gpu@@ABAXPBU?$s_gpu_constant_table@Us_update_vertex_shader_constant_list@c_particle_emitter_gpu@@@@PBVc_particle_state_list@@@Z
template<>
void c_particle_emitter_gpu::set_shader_state<c_particle_emitter_gpu::s_update_vertex_shader_constant_list>(
    const s_gpu_constant_table<s_update_vertex_shader_constant_list> *table, const c_particle_state_list *states) const
{
    real_vector4d dest[27];
    c_particle_system *system = system_get(m_system_index);
    const s_effect_definition *effect = TAG_GET('effe', const s_effect_definition, system->m_tag_index);
    s_particle_movement_effect_event *event = &((s_particle_movement_effect_event *)effect->events.address)[system->m_def_index];
    c_particle_system_definition *system_def = &event->particle_systems[system->m_sub_index];
    const c_particle_emitter_definition *emitter_def = system_def->get_emitter(m_definition_index);
    for (int bit = 0; bit < 27; bit++) {
        if ((((1 << bit) & emitter_def->get_used_particle_states()) & 0xfe117b02) != 0)
            dest[bit].x = states->get_state_value(bit);
    }
    c_rasterizer_constant_desc desc;
    desc = table->descs[1];
    desc.count = 27;
    ((c_rasterizer_constant_table_ref const *)table)->set_constants(&desc, (void const *)(dest));
}

// ---- 0x461a08 frame_advance (CLEAN RECORD morph-0x461a08.cpp, KIT-adapted) ----


struct s_state;
struct s_object;
class c_contrail_profile;

static __forceinline void particle_block_upload_vertex_shader_constant(
    long start, long count, const real_vector4d *data)
{
    c_rasterizer::get_device()->SetVertexShaderConstantF(start, (const float *)data, count);
}
static __forceinline c_contrail_gpu::s_row *particle_block_try_get_row(long index) {
    c_contrail_gpu::s_row *result = 0;
    s_data_array *a = (s_data_array *)g_contrail_gpu_row_data;
    if (index != -1 && (unsigned short)index < a->first_unallocated) {
        c_contrail_gpu::s_row *r = (c_contrail_gpu::s_row *)(a->data + (unsigned short)index * a->size);
        if (r->identifier != 0 && r->identifier == (index >> 16)) result = r;
    }
    return result;
}
// @address 0x461a08 particle_block_frame_advance_contrail
extern "C" void particle_block_frame_advance_contrail(void *datum, float elapsed, void *states_in)
{
    c_contrail_gpu *self = (c_contrail_gpu *)datum;
    c_contrail_states *s = (c_contrail_states *)states_in;
    if (self->first_row_index == -1) return;
    const c_contrail *t = datum_get<const c_contrail>(g_contrail_data_array, self->contrail_index);
    particle_block_upload_vertex_shader_constant(33, 1, (const real_vector4d *)&elapsed);
    particle_block_upload_vertex_shader_constant(32, 1, (const real_vector4d *)g_gpu_particle_default_constants);
    ((c_contrail_states *)(s_state *)s)
        ->set_contrail_system((const c_contrail_system *)((s_object *)c_contrail_system::get(
            c_contrail_location::get(t->location_index)->system_index)));
    s->set_contrail_location(c_contrail_location::get(t->location_index));
    s->set_contrail(t);
    ((const c_contrail_gpu *)self)
        ->set_shader_functions<c_contrail_gpu::s_update_vertex_shader_constant_list>(
            (const s_gpu_constant_table<c_contrail_gpu::s_update_vertex_shader_constant_list> *)
                &g_contrail_update_shaders.constants);
    ((const c_contrail_gpu *)self)
        ->set_shader_state<c_contrail_gpu::s_update_vertex_shader_constant_list>(
            (const s_gpu_constant_table<c_contrail_gpu::s_update_vertex_shader_constant_list> *)
                &g_contrail_update_shaders.constants,
            (const c_contrail_states *)s);
    c_contrail_gpu::s_row *r = 0;
    long index = self->first_row_index;
    while ((r = particle_block_try_get_row(index)) != 0) {
        r->lifespan -= elapsed;
        if (r->lifespan < 0.0f) {
            self->profile_count -= r->allocated;
            if (index == self->first_row_index) { self->first_row_index = r->owner; self->flags &= ~1; }
            long next = r->owner;
            particle_block_delete_contrail_block(index, 0);
            index = next;
        } else {
            if (r->allocated != 0)
            {
                c_rasterizer::draw_primitive(c_rasterizer_index_buffer::_primitive_type_point_list, r->index << 4, r->allocated);
                if (g_contrail_gpu_block_until_idle)
                    c_rasterizer::get_device()->BlockUntilIdle();
            }
            index = r->owner;
        }
    }
}


// @address 0x150bb0 ??$set_shader_update_state@Us_update_vertex_shader_constant_list@c_particle_emitter_gpu@@@c_particle_emitter_gpu@@ABAXPBU?$s_gpu_constant_table@Us_update_vertex_shader_constant_list@c_particle_emitter_gpu@@@@PBVc_particle_state_list@@@Z
template<>
void c_particle_emitter_gpu::set_shader_update_state<c_particle_emitter_gpu::s_update_vertex_shader_constant_list>(
    const s_gpu_constant_table<s_update_vertex_shader_constant_list> *table_ref, const c_particle_state_list *states) const
{
    s_gpu_constant_table_base *table = (s_gpu_constant_table_base *)table_ref;
    float x_value = 0.0f;
    float y_value = 0.0f;
    float z_value = 0.0f;
    c_particle_system *system = system_get(m_system_index);
    c_particle_system_definition *definition = get_system_definition(system);
    const c_particle_emitter_definition *emitter = definition->get_emitter(0) + m_definition_index;
    const c_particle_movement_definition *movement = emitter->get_particle_movement();
    if (!emitter->is_cpu() && movement && (movement->m_flags & 1)) {
        const c_editable_property<c_particle_state_list> *property = movement->get_property_by_index(0, 0);
        y_value = 1.0f;
        float x;
        if (property)
            x = property->evaluate(states);
        else
            x = y_value;
        x_value = g_physics_constants->gravity * x;
        property = movement->get_property_by_index(0, 1);
        if (property)
            y_value = property->evaluate(states);
        property = movement->get_property_by_index(0, 2);
        if (property)
            z_value = property->evaluate(states);
        else
            z_value = 0.0f;
    }
    struct s_movement_constants
    {
        real_vector4d x;
        real_vector4d y;
        real_vector4d z;
    };
    s_movement_constants constants;
    constants.x.x = x_value;
    constants.y.x = y_value;
    constants.z.x = z_value;
    ((c_rasterizer_constant_table_ref const *)table)->set_constants(&table->descs[5], (void const *)(&constants));
}

static __forceinline void particle_block_scale_direction(const float *source, float scale, float *dest)
{
    dest[0] = source[0] * scale;
    dest[1] = source[1] * scale;
    dest[2] = source[2] * scale;
}
// @address 0x460ac0 particle_block_set_direction_constants
extern "C" void particle_block_set_direction_constants(const c_particle_emitter_gpu *emitter, s_gpu_constant_table_base *table)
{
    struct { real_vector4d forward, up, depth; } constants;
    const s_observer_result *view = g_particle_render_camera;
    c_particle_system *system = system_get(emitter->m_system_index);
    const c_particle_definition *definition = TAG_GET('prt3', const c_particle_definition, get_particle_definition_index(system));
    particle_block_scale_direction(g_particle_render_camera->velocity.n, definition->forward_scale, constants.forward.n);
    particle_block_scale_direction(view->angular_velocity.n, definition->up_scale, constants.up.n);
    constants.depth.x = definition->depth_scale;
    memset(&constants.depth.y, 0, sizeof(float) * 2);
    ((c_rasterizer_constant_table_ref const *)table)->set_constants(&table->descs[4], (void const *)(&constants));
}


// (frame advance of every gpu particle emitter through the update shader pair)

// @address 0x7be794 g_particle_gpu_vertex_buffer
#if !defined(_XBOX) || defined(__clang__)
common_gpu_vertex_buffer g_particle_gpu_vertex_buffer = { 0x10, 0x10, 0x1c0 };
#endif
// @address 0x8c46e8 g_effect_camera_active

// @address 0x150288 particle_block_frame_advance_all
extern "C" void particle_block_frame_advance_all(float dt)
{
    if (g_particle_gpu_block_data->first_unallocated > 0) {
        c_rasterizer::set_render_target(0, (c_rasterizer::e_surface)0);
        D3DDevice_SetRenderState_ColorWriteEnable(c_rasterizer::get_device(), 0);
        c_rasterizer::set_render_target(1, (c_rasterizer::e_surface)0);
        c_rasterizer::set_render_target(2, (c_rasterizer::e_surface)0);
        c_rasterizer::set_vertex_stream(&g_particle_gpu_vertex_buffer);
        if (g_effect_camera_active) {
            c_rasterizer::set_sampler_address_mode(
                21, c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp);
            if (g_rasterizer_sampler_filter_modes[21] != c_rasterizer::_sampler_filter_mode_bilinear) {
                g_rasterizer_sampler_filter_modes[21] = c_rasterizer::_sampler_filter_mode_bilinear;
                rasterizer_device_set_sampler_filter_modes(c_rasterizer::get_device(), 21,
                    c_rasterizer::_sampler_filter_mode_bilinear, c_rasterizer::_sampler_filter_mode_bilinear);
            }
            c_rasterizer::set_aliased_surface_as_texture(21, (c_rasterizer::e_surface)2);
        }
        c_rasterizer::get_device()->SetVertexShaderConstantF(21, &dt, 1);
        c_particle_state_list states;
        states.set_null_particle();
        ((s_gpu_constant_table_base *)&g_particle_update_shaders.vertex_shader.constants)->own_constants();
        c_rasterizer::set_vertex_shader(
            static_cast<const c_rasterizer_vertex_shader *>(tag_get('vtsh', g_particle_update_shaders.vertex_shader.shader_reference->index)),
            (e_vertex_type)g_particle_update_shaders.vertex_shader.vertex_type,
            _transfer_vector_vertex_none, _entry_point_default);
        ((s_gpu_constant_table_base *)&g_particle_update_shaders.pixel_shader.constants)->own_constants();
        c_rasterizer::set_pixel_shader(
            static_cast<const c_rasterizer_pixel_shader *>(tag_get('pixl', g_particle_update_shaders.pixel_shader.shader_reference->index)),
            _entry_point_default);
        c_rasterizer::get_device()->BeginExport(0, g_particle_gpu_vertex_buffer.get_d3d_buffer(), 0);
        s_gpu_constant_table_base *constant_table = (s_gpu_constant_table_base *)&g_particle_update_shaders.vertex_shader.constants;
        ((c_rasterizer_constant_table_ref const *)constant_table)
            ->set_constants(constant_table->descs, (void const *)(c_particle_emitter_gpu::x_layout.bounds));
        c_data_iterator<s_particle_emitter_gpu_block> iterator;
        iterator.begin(g_particle_gpu_block_data);
        while (iterator.next())
            particle_block_frame_advance_emitter(iterator.get_datum(), dt, (c_constant_table *)constant_table, &states);
        c_rasterizer::get_device()->EndExport(0, g_particle_gpu_vertex_buffer.get_d3d_buffer(), 0);
        ((s_gpu_constant_table_base *)&g_particle_update_shaders.vertex_shader.constants)->disown_constants();
        ((s_gpu_constant_table_base *)&g_particle_update_shaders.pixel_shader.constants)->disown_constants();
#if defined(_XBOX) && !defined(__clang__)
        g_light_volume_gpu_layout.fence = c_rasterizer::get_device()->InsertFence();
#else
        g_game_state_render_fence = c_rasterizer::get_device()->InsertFence();
#endif
    }
}

struct s_weighted_atmosphere_parameters;
#include "effects/particle_system_globals.h" // tmp/probe (GAME-CHANGES F63): g_particle_render_model_vertex_shader (0x827be900)
// @address 0x7c8728 g_particle_model_gpu_variant
extern "C" {
s_gpu_variant_data g_particle_model_gpu_variant =
{
#if !defined(_XBOX) || defined(__clang__)
    (s_gpu_global_vertex_shader<c_particle_emitter_gpu::s_render_vertex_shader_constant_list> *)&g_particle_render_model_vertex_shader,
#else
    (s_gpu_global_vertex_shader<c_particle_emitter_gpu::s_render_vertex_shader_constant_list> *)0x827be900,
#endif
    _vertex_type_particle_model,
    true,
    c_rasterizer_index_buffer::_primitive_type_triangle_strip,
    0,
    0
};
}
// @address 0x4b884 k_particle_default_gpu_variant
#if !defined(_XBOX) || defined(__clang__)
extern "C" {
s_gpu_variant_data k_particle_default_gpu_variant =
{
#if !defined(_XBOX) || defined(__clang__)
    &g_particle_render_vertex_shader,
#else
    (s_gpu_global_vertex_shader<c_particle_emitter_gpu::s_render_vertex_shader_constant_list> *)0x827be8a8,
#endif
    _vertex_type_particle,
    false,
    c_rasterizer_index_buffer::_primitive_type_quad_list,
    4,
    1
};
}
#endif

__forceinline void c_particle_model_definition::get_gpu_variant_data(s_gpu_variant_data *variant_data) const
    {
        variant_data->vertex_count = (unsigned long)m_variants->vertex_count;
        variant_data->primitive_count = variant_data->vertex_count - 2;
    }
static __forceinline void particle_block_restore_atmosphere_constants()
{
    if (atmosphere_constants_index != -2) {
        atmosphere_constants_index = -2;
        c_atmosphere_fog_interface::set_constant(&c_atmosphere_fog_interface::m_default_parameters, true, true);
    }
}
static __forceinline const s_geometry_sample *particle_block_get_geometry_sample(long lightprobe_index)
{
    const s_geometry_sample *sample = &c_geometry_sampler::m_default_lightprobe_sample;
    if (lightprobe_index != -1) {
        const s_effect_lightprobe *lightprobe = &g_effect_lightprobe_memory[datum_index_to_absolute_index(lightprobe_index)];
        if (lightprobe->flags.test(0))
            sample = lightprobe->flags.test(1) ? &lightprobe->sample : &lightprobe->sample;
    }
    return sample;
}

// Unindexed device draw; inlined here (the shared c_rasterizer::draw_primitive is out-of-line).
static __forceinline void draw_primitive_unindexed(c_rasterizer_index_buffer::e_primitive_type type, long start_vertex, long primitive_count)
{
    c_rasterizer::get_device()->SetIndices(0);
    c_rasterizer::get_device()->DrawPrimitive(g_primitive_types[type], start_vertex, primitive_count);
}
static __forceinline long particle_block_row_used_count(const s_particle_block_element *row)
{
    return row->state;
}
static __forceinline long particle_block_row_used_start(const s_particle_block_element *row)
{
    return ((row->index + 1) << 4) - particle_block_row_used_count(row);
}

static __forceinline const real_matrix4x3 *particle_block_get_render_matrix(c_particle_emitter *emitter, bool local_space, real_matrix4x3 *storage)
{
    const real_matrix4x3 *matrix = local_space ? &emitter->m_matrix : &k_real_math_identity_matrix_storage;
    if (get_first_person(get_location(emitter))) {
        *storage = *matrix;
        if (local_space) {
            real_matrix4x3 camera;
            c_player_view::get_player_render_camera_orientation(&camera);
            matrix4x3_compute_product(&camera, storage, storage);
        }
        return storage;
    }
    return matrix;
}

// @address 0x45f680 ?render@c_particle_emitter_gpu@@QBAXXZ
void c_particle_emitter_gpu::render() const
{
    if (m_first_row_index == -1)
        return;

    c_particle_system *system = system_get(m_system_index);
    c_particle_emitter *emitter = emitter_get(m_emitter_index);
    const c_particle_system_definition *definition = get_system_definition(system);
    bool local_space = definition->get_coordinate_system() != 0;
    const c_particle_definition *particle = TAG_GET('prt3', const c_particle_definition, definition->get_particle_definition_index());

    real_matrix4x3 render_matrix;
    const real_matrix4x3 *matrix = particle_block_get_render_matrix(emitter, local_space, &render_matrix);
    c_rasterizer::get_device()->SetVertexShaderConstantF(33, &matrix->forward.i, 1);
    c_rasterizer::get_device()->SetVertexShaderConstantF(34, &matrix->left.i, 1);
    c_rasterizer::get_device()->SetVertexShaderConstantF(35, &matrix->up.i, 1);
    c_rasterizer::get_device()->SetVertexShaderConstantF(36, &matrix->position.i, 1);
    c_rasterizer::set_vertex_stream(&g_particle_gpu_vertex_buffer);
    c_rasterizer::get_device()->SetVertexShaderConstantF(32, g_gpu_particle_default_constants, 1);

    if (flag_test_mask(particle->flags2, 10))
        particle_block_restore_atmosphere_constants();

    DWORD cull_mode;
    if (flag_test_mask(particle->flags2, 9)) {
        c_rasterizer::get_device()->GetRenderState(D3DRS_CULLMODE, &cull_mode);
        c_rasterizer::get_device()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    }

    s_gpu_variant_data *variant;
    if (flag_test_mask(particle->flags2, 15)) {
        variant = &g_particle_model_gpu_variant;
        const c_particle_model_definition *model = c_particle_model_definition::get(c_tag_index(particle->particle_model));
        model->set_vertex_stream();
        model->get_gpu_variant_data(variant);
    } else {
        variant = &k_particle_default_gpu_variant;
    }

    const s_tag_reference *render_method = &particle->render_method;
    if (!variant->shader->is_initialized())
        variant->shader->initialize(render_method);
    set_shader_variants<s_render_vertex_shader_constant_list>(variant->shader->get_constant_table());
    set_shader_render_state<s_render_vertex_shader_constant_list>(variant->shader->get_constant_table(), variant->vertex_count);
    if (flag_test_mask(particle->flags2, 8))
        particle_block_set_direction_constants(this, (s_gpu_constant_table_base *)variant->shader->get_constant_table());
    if (flag_test_mask(particle->flags2, 11)) {
        const s_geometry_sample *sample = particle_block_get_geometry_sample(system->m_lightprobe_index);
        c_lighting_interface::calculate_and_set_ravi_constants(sample->red, sample->green, sample->blue, 1.0f);
    }

    long definition_index = system->m_tag_index;
    c_render_method_data::m_valid = true;
    if (render_method_submit(definition_index, (const c_render_method *)render_method, (e_vertex_type)variant->vertex_type,
            _transfer_vector_vertex_none, _entry_point_default, false))
    {
        long row_index = m_first_row_index;
        const s_particle_block_element *row;
        while ((row = particle_block_element_try_get(row_index)) != 0) {
            if (variant->per_particle_draw) {
                for (unsigned long index = 0; index < (unsigned long)particle_block_row_used_count(row); index++)
                    draw_primitive_unindexed(variant->primitive_type,
                        (particle_block_row_used_start(row) + index) * variant->vertex_count, variant->primitive_count);
            } else {
                c_rasterizer::draw_primitive(variant->primitive_type, particle_block_row_used_start(row) * variant->vertex_count,
                    variant->primitive_count *particle_block_row_used_count(row));
            }
            if (g_particle_gpu_block_until_idle)
                c_rasterizer::get_device()->BlockUntilIdle();
            row_index = row->next_index;
        }
    }
    if (flag_test_mask(particle->flags2, 9))
        c_rasterizer::get_device()->SetRenderState(D3DRS_CULLMODE, cull_mode);
    c_render_method_data::m_valid = false;
    render_method_submit_valid();
}
// ---- 0x460bd8: build particle vertex declaration ----




// @address 0x460bd8 particle_block_add_vertex_declaration
extern "C" void particle_block_add_vertex_declaration(s_gpu_target_table<1> *build, long row, void *owner)
{
    s_gpu_storage_interface *entry = *build->entries;
    unsigned long largest = entry->size();
    unsigned long total = largest;
    largest = entry->size();
    largest = entry->size();
    entry = *build->entries;
    unsigned long stride = ((total + largest - 1) / largest) * largest;
    entry->owner = owner;
    entry->layout(stride, 0);
    unsigned long used = entry->size();
    unsigned long padding = stride - used;
    real_point2d size;
    size.x = (float)entry->stride;
    size.y = (float)entry->offset;
    build->bounds[0].size = size;
    long padding_type;
    unsigned long padding_kind;
    switch (padding) {
    case _vertex_declaration_padding_twelve:
        padding_type = 0x2a23b9;
        padding_kind = 2;
        break;
    case _vertex_declaration_padding_eight:
        padding_type = 0x2c23a5;
        padding_kind = 1;
        break;
    case _vertex_declaration_padding_four:
        padding_type = 0x2c83a4;
        padding_kind = 0;
        break;
    case _vertex_declaration_padding_none:
        padding_kind = 18;
        padding_type = 0xffffffff;
        break;
    default:
        // match: 0x460bd8: unreachable hint omits dead branch check
        __assume(0);
    }
    rasterizer_vertex_declaration *vertex_declaration = &g_rasterizer_vertex_buffer_declarations[17];
    vertex_declaration->index = 17;
    entry = *build->entries;
    vertex_declaration->register_mappings[0].attribute = (signed char)entry->semantic;
    unsigned char kind = entry->kind();
    vertex_declaration->register_mappings[0].format = kind;
    vertex_declaration->register_mappings[0].usage = (unsigned char)k_vertex_semantic_usages[entry->semantic];
    vertex_declaration->register_mappings[0].usage_index = (unsigned char)entry->usage_index;
    long element_count = 1;
    if (padding != 0) {
        vertex_declaration->register_mappings[1].attribute = 0;
        vertex_declaration->register_mappings[1].format = padding_kind;
        vertex_declaration->register_mappings[1].usage = 0;
        vertex_declaration->register_mappings[1].usage_index = 15;
        element_count = 2;
    }
    vertex_declaration->register_mappings[element_count].attribute = -1;
    s_declaration_element *term = c_vertex_declaration_table::base_vertex_decls[row];
    const s_declaration_element *terminator = &k_vertex_declaration_terminator;
    if (memcmp(term, terminator, sizeof(s_declaration_element)) != 0) {
        do {
            term++;
        } while (memcmp(term, terminator, sizeof(s_declaration_element)) != 0);
    }
    long count = ((unsigned char *)term - (unsigned char *)c_vertex_declaration_table::base_vertex_decls
        - row * (long)sizeof(c_vertex_declaration_table::base_vertex_decls[0])) / 12;
    bool skip = 64 - count < 64 && term[-1].Stream >= 2;
    if (skip)
        return;
    entry = *build->entries;
    term->Stream = 2;
    term->Offset = (unsigned short)(entry->size() * entry->offset);
    unsigned long declaration_type = entry->declaration_type();
    term->Type = declaration_type;
    term->Method = 0;
    term->Usage = (unsigned char)k_vertex_semantic_usages[entry->semantic];
    term->UsageIndex = (unsigned char)entry->usage_index;
    long declaration_element_count = 1;
    if (padding != 0) {
        declaration_element_count = 2;
        term[1].Stream = 2;
        term[1].Offset = (unsigned short)used;
        term[1].Type = padding_type;
        term[1].Method = 0;
        term[1].Usage = 0;
        term[1].UsageIndex = 15;

    }
    s_declaration_element *end = term + declaration_element_count;
    *end = *terminator;
}


particle_block_queue_system g_particle_block_queue_system;
c_particle_emitter_gpu::s_layout c_particle_emitter_gpu::x_layout;

extern "C" {
// @address 0x119a4f0 g_particle_queue_alloc_success
unsigned char g_particle_queue_alloc_success[4];
}


static __forceinline void particle_block_queue_half4(s_gpu_storage_half4 *storage, unsigned long index, const __vector4 &value) {
    u_gpu_pack_bits packed;
    packed.vector = __vpkd3d(value, value, VPACK_FLOAT16_4, VPACK_64LO, 2);
    storage->set_value(index, packed.i64);
}
static __forceinline void particle_block_queue_fixed4(s_gpu_storage_ushort4n *storage, unsigned long index, const __vector4 &value) {
    __vector4 converted = __vctuxs(value, 16);
    u_gpu_pack_bits packed;
    packed.vector = __vpkuwus(converted, converted);
    storage->set_value(index, packed.i64);
}
static __forceinline void particle_block_queue_half2(s_gpu_storage_half2 *storage, unsigned long index, const __vector4 &value) {
    u_gpu_pack_bits packed;
    packed.vector = __vpkd3d(value, value, VPACK_FLOAT16_2, VPACK_64LO, 3);
    storage->set_value(index, packed.i32);
}
static __forceinline void particle_block_queue_rotation(s_gpu_storage_dec3n *storage, unsigned long index, const __vector4 &value) {
    u_gpu_pack_bits packed;
    packed.vector = gpu_queue_pack_rotation_vec(value);
    packed.vector = __vpkd3d(packed.vector, packed.vector, VPACK_NORMPACKED32, VPACK_64LO, 3);
    storage->set_value(index, packed.i32);
}
static __forceinline void particle_block_queue_color(s_gpu_storage_color *storage, unsigned long index, const __vector4 &value) {
    u_gpu_pack_bits packed;
    packed.vector = gpu_queue_pack_color_vec(value);
    packed.vector = __vpkd3d(packed.vector, packed.vector, VPACK_D3DCOLOR, VPACK_32, 3);
    storage->set_value(index, packed.i32);
}
static __forceinline void particle_block_queue_address(s_gpu_storage_ushort2 *storage, unsigned long index, const __vector4 &value) {
    __vector4 converted = __vctuxs(value, 0);
    u_gpu_pack_bits packed;
    packed.vector = __vpkuwus(converted, converted);
    storage->set_value(index, packed.i32);
}

// @address 0x145588 particle_block_queue_gpu_particle
extern "C" unsigned long particle_block_queue_gpu_particle(
    s_particle_emitter_gpu_block *reference, int type, const c_particle *particle, float elapsed_time, bool temporary)
{
    unsigned long allocation = (unsigned char)type;
    unsigned long index = g_particle_block_queue_system.next->count;
    if (index + 1 > g_effect_budget_limits->table->blocks[_particle_queued_gpu_budget].limits[allocation].maximum_count) {
        g_particle_queue_alloc_success[allocation] = false;
        return -1;
    }
    unsigned long particle_address = particle->m_gpu_index == -1 ?
        particle_block_allocate_element_slot(reference, type, (1.0f - particle->m_age + elapsed_time) / particle->m_inverse_lifespan) : particle->m_gpu_index;
    if (particle_address == -1) {
        g_particle_queue_alloc_success[allocation] = false;
        return -1;
    }
    g_particle_queue_alloc_success[allocation] = true;
    real_vector4d position;
    position.x = particle->m_position.x;
    position.y = particle->m_position.y;
    position.z = particle->m_position.z;
    position.w = -1.0f;
    __vector4 velocity;
    velocity.v[0] = particle->m_velocity.i;
    velocity.v[1] = particle->m_velocity.j;
    velocity.v[2] = particle->m_velocity.k;
    velocity.v[3] = 0.0f;
    const unsigned __int64 *random = reinterpret_cast<const unsigned __int64 *>(particle->m_random_seed);
    const unsigned __int64 *random2 = reinterpret_cast<const unsigned __int64 *>(particle->m_random_seed + 4);
    __vector4 scale;
    scale.v[0] = particle->m_random_offset;
    scale.v[1] = 0.0f;
    scale.v[2] = 0.0f;
    scale.v[3] = 0.0f;
    __vector4 time;
    time.v[0] = particle->m_correlation_1;
    time.v[1] = temporary ? 1.0e10f : particle->m_inverse_lifespan;
    time.v[2] = particle->m_age;
    time.v[3] = 0.0f;
    __vector4 size;
    size.v[0] = particle->m_rotational_velocity;
    size.v[1] = particle->m_property_values.m_animation_rate;
    size.v[2] = 0.0f;
    size.v[3] = 0.0f;
    __vector4 rotation;
    rotation.v[0] = particle->m_axis.i;
    rotation.v[1] = particle->m_axis.j;
    rotation.v[2] = particle->m_axis.k;
    rotation.v[3] = 0.0f;
    __vector4 color;
    color.v[0] = particle->m_color.rgb.red;
    color.v[1] = particle->m_color.rgb.green;
    color.v[2] = particle->m_color.rgb.blue;
    color.v[3] = particle->m_color.alpha;
    unsigned long column = particle_address % 16;
    unsigned long row = particle_address / 16;
    g_particle_block_queue_system.next->write_lock();
    particle_block_queue_frame *layout = g_particle_block_queue_system.next;
    layout->position.set_value(index, position);
    particle_block_queue_half4(&layout->velocity, index, velocity);
    layout->random.set_value(index, *random);
    layout->random2.set_value(index, *random2);
    particle_block_queue_fixed4(&layout->scale, index, scale);
    particle_block_queue_half4(&layout->time, index, time);
    particle_block_queue_half2(&layout->size, index, size);
    particle_block_queue_rotation(&layout->rotation, index, rotation);
    particle_block_queue_color(&layout->color, index, color);
    __vector4 address;
    address.v[0] = (float)column;
    address.v[1] = (float)row;
    address.v[2] = 0.0f;
    address.v[3] = 0.0f;
    particle_block_queue_address(&g_particle_block_queue_system.next->address_group.address, index, address);
    g_particle_block_queue_system.next->count++;
    g_particle_block_queue_system.next->unlock();
    return particle_address;
}

// @address 0x7bf860 g_gpu_particle_default_constants
#if defined(_XBOX) && !defined(__clang__)
extern "C" {
const float *g_gpu_particle_default_constants = (const float *)0x820249f4;
}
#else
static const u_gpu_particle_default_constants k_gpu_particle_default_constant_values = {{0x7fc00000, 0x00000000, 0x00000000, 0x7fc00000}};

extern "C" {
const float *g_gpu_particle_default_constants = k_gpu_particle_default_constant_values.values;
}
#endif

// @rdata 0x82024684..0x820246bc
// @address 0x24684 g_xenon_profile_setting_ids
extern "C" {
const unsigned long g_xenon_profile_setting_ids[14] = {
    XPROFILE_TITLE_SPECIFIC1,
    XPROFILE_OPTION_CONTROLLER_VIBRATION,
    XPROFILE_GAMER_YAXIS_INVERSION,
    XPROFILE_GAMERCARD_ZONE,
    XPROFILE_GAMERCARD_REGION,
    XPROFILE_OPTION_VOICE_MUTED,
    XPROFILE_OPTION_VOICE_THRU_SPEAKERS,
    XPROFILE_OPTION_VOICE_VOLUME,
    XPROFILE_GAMER_DIFFICULTY,
    XPROFILE_GAMER_CONTROL_SENSITIVITY,
    XPROFILE_GAMER_PREFERRED_COLOR_FIRST,
    XPROFILE_GAMER_PREFERRED_COLOR_SECOND,
    XPROFILE_GAMER_ACTION_AUTO_AIM,
    XPROFILE_GAMER_ACTION_AUTO_CENTER
};
}
// @rdata-end
