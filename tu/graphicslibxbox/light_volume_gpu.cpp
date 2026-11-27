// graphicslibxbox/light_volume_gpu.obj: from the records 0x1820c0 0x1820c8 0x1822d0 0x23d578 0x23d590 0x23d5d8 0x23d5f0 0x23d608 0x23db48 0x23dbe8 0x4638f0 0x464008 0x4645d0 0x464600 0x4646b0 0x464798 0x4649c8 0x464a28 0x464ae0 0x464b90 0x464d48 0x464f60 0x4652a0 0x465340 0x465360 0x465400 0x465420 0x4654a8 0x4654c0 0x465540 0x465618 0x4656f0 0x465b08 0x465c48 0x465f48 0x466098 0x466398 0x70a310 0x70a320 0x70a330 0x70ca68 0x70ca80 0x70ca98
#include "gpu_particle/shared_gpu_storage.h"
#include "effects/effect_runtime_storage.h"
// @address 0x11df790 g_particle_instance_owner_data
// @address 0x11df794 g_light_volume_system_data
// @address 0x11df7e8 g_light_volume_gpu_data
__declspec(thread) s_data_array *g_light_volume_gpu_data;
// @address 0x11df838 g_light_volume_gpu_row_data
__declspec(thread) s_data_array *g_light_volume_gpu_row_data;
// @address 0x11df818 g_light_volume_data
// @address 0x11df7f4 g_particle_instance_location_data
// @address 0x11df814 g_particle_instance_data
// @flags /TP /O2 /Oc
#include <xtl.h>
#include "multithreading/thread_tls.h"
#include "game/game_time_types.h"
#include "gpu_particle/gpu_constant_table_types.h"
#include <string.h>
#include <math.h>
#include "tag_files/tag_groups.h"
#include "memory/data.h"
#include "tag_files/tag_index.h"
#include "tag_files/tag_types.h"
#include "cache/restricted_memory_regions.h"
#include "interface/chud/chud.h"
#include "rasterizer/rasterizer_vertex_definitions.h"
#include "render_methods/render_method_submit.h"
#include "gpu_particle/particle_block.h"
#include "effects/particle.h"
#include "gpu_particle/light_volume_gpu.h"
#include "sky_atm/atmosphere.h"
#include "math/real_math_types.h"
#include "rasterizer/rasterizer_resource_definitions.h"
#include "cache/restricted_memory_layout.h"
#include "cache/restricted_memory.h"
#include "effects/particle_instance_types.h"
#include "gpu_particle/gpu_empty_constant_list_types.h"
#include "saved_games/game_state_procs_externs.h"
#include "effects/effect_contrail_location_types.h"
#include "gpu_particle/desc_types.h"
#include "gpu_particle/effect_parameters_types.h"
#include "gpu_particle/shader_info_types.h"
#include "gpu_particle/constant_entry_types.h"
#include "gpu_particle/pass_state_types.h"
#include "gpu_particle/effect_entry_types.h"
#include "gpu_particle/effect_constants_types.h"
#include "cache/cache_files.h"
#include "scenario/scenario.h"
#include "game/game_globals.h"
#include "rasterizer/rasterizer_global_definitions.h"
#include "gpu_particle/common_gpu.h"
#include "gpu_particle/contrail_target_entry.h"
#include "gpu_particle/contrail_target_bounds_types.h"
#include "gpu_particle/light_volume_gpu_globals.h"
#include "gpu_particle/gpu_buffer.h"
// @address 0x11df7dc g_static_lighting_effect_attachments
// @address 0x11df7e4 g_contrail_chain_data
// @address 0x11df7f8 g_light_volume_location_data
// @address 0x2251c k_light_volume_gpu_zero
#include "gpu_particle/shader_slot_types.h"
#include "rasterizer/c_rasterizer_constant_table_ref.h"
#include "rasterizer/rasterizer_shader_definitions.h"
#include "rasterizer/rasterizer_shader_tag_types.h"

#include "effects/light_volume_states.h"
#include "gpu_particle/particle_block_bounds_types.h"
#include "sky_atm/atmosphere_fog_interface.h"
#include "render_methods/render_method_shader_definition_types.h"
#include "gpu_particle/vertex_shader.h"
#include "gpu_particle/vertex_shader_initialize.h"
#include "gpu_particle/constant_table_types.h"
#include "effects/light_volume_location.h"
#include "effects/effects_globals.h"
#include "gpu_particle/constant_table.h"
#include "render_methods/render_method_definitions.h"
#include "rasterizer/rasterizer.h"
#include "rasterizer/xenon/rasterizer_xenon_device.h"
#include "cache/restricted_memory_region_members.h"
#include "gpu_particle/light_volume_gpu_shader_initialize.h"

class c_render_method;
enum e_vertex_type;
enum e_transfer_vector_vertex_types;
enum e_entry_point;
class c_rasterizer_vertex_buffer;
struct s_data_iterator;
struct s_data_array;

template<class ConstantList> struct s_gpu_constant_table;

class c_light_volume_states;
class c_light_volume_system;
#include "effects/light_volume_gpu.h"

// @rdata 0x82024ba0..0x82024bd0
// @address 0x24ba0 ?x_in_place@s_update_vertex_shader_constant_list@c_light_volume_gpu@@2QB_NB
const bool c_light_volume_gpu::s_update_vertex_shader_constant_list::x_in_place[slot_count] = { false, false, false, true, true, true, false };
// @address 0x24ba8 ?x_in_place@s_render_vertex_shader_constant_list@c_light_volume_gpu@@2QB_NB
const bool c_light_volume_gpu::s_render_vertex_shader_constant_list::x_in_place[slot_count] = { false, false };
// @address 0x24bac ?x_keys@s_update_vertex_shader_constant_list@c_light_volume_gpu@@2QBHB
const int c_light_volume_gpu::s_update_vertex_shader_constant_list::x_keys[slot_count] = { 0x80000, 0x8000b, 0x80001, 0x80002, 0x80003, 0x80004, 0x8000c };
// @address 0x24bc8 ?x_keys@s_render_vertex_shader_constant_list@c_light_volume_gpu@@2QBHB
const int c_light_volume_gpu::s_render_vertex_shader_constant_list::x_keys[slot_count] = { 0x8000b, 0x80001 };
// @rdata-end

// @address 0x24bd8 k_light_volume_render_required
// @address 0x24bf8 k_light_volume_render_keys
#if !defined(_XBOX) || defined(__clang__)
extern "C" {
const bool k_light_volume_render_required[2] = { false, false };
const int k_light_volume_render_keys[2] = { 0x8000b, 0x80001 };
}
#endif

struct s_gpu_effect_definition { c_typed_tag_block<s_effect_entry> entries; };


#include "gpu_particle/contrail_chain_types.h"
#include "effects/particle_location_limits.h"
#include "gpu_particle/target_table_types.h"
#include "effects/effect_budget_limits.h"
// @address 0x8c43a8 g_effect_budget_limits

#include "effects/light_volume_types.h"
#include "render_methods/render_method_data.h"



// c_light_volume_gpu::x_overall_storage


struct s_gpu_constant_table_base;

struct cache_file_tag_instance;

const float k_real_one = 1.0f;
const float k_real_zero = 0.0f;

// @address 0x1822d0 light_volume_gpu_set_effect_constants
void light_volume_gpu_set_effect_constants(s_handle *handle, s_pass_state *state, const s_effect_parameters *parameters)
{
    s_effect_constants constants;
    s_particle_instance *instance = datum_get<s_particle_instance>(g_particle_instance_data, handle->index);
    s_effect_contrail_location *location = datum_get<s_effect_contrail_location>(g_particle_instance_location_data, instance->location_index);
    c_light_volume_system *owner = datum_get<c_light_volume_system>(g_particle_instance_owner_data, location->owner_index);
    s_gpu_effect_definition *definition = TAG_GET('ltvl', s_gpu_effect_definition, owner->m_definition_index);
    s_effect_entry *entry = &definition->entries[instance->definition_index];
    location = datum_get<s_effect_contrail_location>(g_particle_instance_location_data, instance->location_index);
    constants.count.x = (float)entry->count;
    constants.scale.x = entry->scale;
    constants.offset.x = instance->offset;
    constants.length_times_density.x = instance->length * instance->density;
    constants.inverse_density.x = k_real_one / instance->density;
    constants.profile_length.x = instance->profile_length;
    constants.point = location->position;
    constants.vector = location->direction;
    constants.parameters[0].x = k_real_zero;
    memset(&constants.parameters[1], 0, sizeof(constants.parameters) - sizeof(constants.parameters[0]));
    for (int i = 0; i < 10; i++) {
        if (entry->flags & (1 << i))
            constants.parameters[i].x = parameters->values[i];
    }
    ((c_rasterizer_constant_table_ref const *)(s_gpu_constant_table_base *)state)
        ->set_constants(&((s_gpu_constant_table_base *)state)->descs[1], (void const *)(&constants));
}

struct s_data_array;

void __tls_set_g_light_volume_row_gpu_allocator(void *object)
{
    data_set_new_base_address(&g_static_lighting_effect_attachments, (s_data_array *)object);
}

// @address 0x783194 ?g_light_volume_row_gpu_allocator@@3V?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_light_volume_row_gpu_allocator@@YAXPAX@Z@@A
t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_light_volume_row_gpu_allocator> g_light_volume_row_gpu_allocator;

class c_allocation_base;

// @address 0x23d590 light_volume_row_gpu_initialize
extern "C" void light_volume_row_gpu_initialize(void)
{
    g_static_lighting_effect_attachments = data_new("screen_effect", 64, 44, 0, (c_allocation_base *)&g_light_volume_row_gpu_allocator);
}

// @address 0x23d5d8 light_volume_row_gpu_reset
extern "C" void light_volume_row_gpu_reset(void)
{
    s_data_array *data = (s_data_array *)g_static_lighting_effect_attachments;
    data->valid = 1;
    data_delete_all(data);
}

// @address 0x23d5f0 light_volume_row_gpu_disable
extern "C" void light_volume_row_gpu_disable(void)
{
    s_data_array *data = (s_data_array *)g_static_lighting_effect_attachments;
    data->valid = 0;
}

// @address 0x23d608 light_volume_row_gpu_dispose
extern "C" void light_volume_row_gpu_dispose(void)
{
    s_data_array *data = (s_data_array *)g_static_lighting_effect_attachments;
    if (data != 0)
    {
        data_dispose(data);
        g_static_lighting_effect_attachments = 0;
    }
}

static __forceinline s_data_array *get_light_volume_row_array()
{
    return g_light_volume_gpu_row_data;
}

__forceinline c_light_volume_gpu::s_row *c_light_volume_gpu::s_row::get(long row_index)
{
    return datum_get<s_row>(get_light_volume_row_array(), row_index);
}


// @address 0x7bf2f4 g_contrail_frame_vertex_buffer

inline c_light_volume_gpu::s_layout::s_layout() :
    position(0, 0),
    size(3, 0),
    color(8, 0)
{
    slots[0] = &position;
    slots[1] = &size;
    slots[2] = &color;
}

// @address 0x7bf330 ?x_layout@c_light_volume_gpu@@2Us_layout@1@A
c_light_volume_gpu::s_layout c_light_volume_gpu::x_layout;

struct rasterizer_vertex_declaration;

// @address 0x4638f0 light_volume_gpu_initialize
extern "C" void light_volume_gpu_initialize(void)
{
    g_contrail_chain_data = data_new("c_light_volume_gpu", 160, 16, 0, (c_allocation_base *)&g_light_volume_gpu_main_allocator);
    g_contrail_chain_data = data_new("c_light_volume_gpu::s_row", 160, 12, 0, (c_allocation_base *)&g_light_volume_gpu_row_allocator);
    common_gpu_vertex_buffer *configuration = &g_contrail_frame_vertex_buffer;
    int stride = rasterizer_vertex_get_stride_from_declaration(&g_rasterizer_vertex_buffer_declarations[configuration->format]);
    int size = stride;
    const int *dimensions = &configuration->element_size;
    size *= dimensions[0];
    size *= dimensions[-1];
    g_light_volume_gpu_storage_address = g_light_volume_emitter_allocation.reserve_memory(
        "c_particle_emitter_gpu", "x_gamestate_storage", (size + 127) & 0xffffff80, 7);
}

// @address 0x4645d0 ?__tls_set_g_light_volume_gpu_allocator@@YAXPAX@Z
void __tls_set_g_light_volume_gpu_allocator(void *object)
{
    data_set_new_base_address(&g_light_volume_gpu_data, (s_data_array *)object);
}

// @address 0x465340 ?deallocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_light_volume_gpu_allocator@@YAXPAX@Z@@UAAXPAX@Z
template <>
void t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_light_volume_gpu_allocator>::deallocate(void *memory)
{
    g_light_volume_gpu_data = 0;
    restricted_region_free_member(2, m_member_index);
}

// @address 0x465400 ?deallocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_light_volume_row_gpu_allocator@@YAXPAX@Z@@UAAXPAX@Z
template <>
void t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_light_volume_row_gpu_allocator>::deallocate(void *memory)
{
    g_light_volume_gpu_row_data = 0;
    restricted_region_free_member(2, m_member_index);
}

// @address 0x4649c8 ?destroy@c_light_volume_gpu@@SAXJ@Z
void c_light_volume_gpu::destroy(long volume_index) {
    if (volume_index != -1) {
        c_light_volume_gpu *volume = get(volume_index);
        s_row::destroy(volume->m_first_row_index, true);
        volume->m_first_row_index = -1;
        volume->m_row_count = 0;
        ::datum_delete((s_data_array *)g_light_volume_gpu_data, volume_index);
    }
}

void c_light_volume_gpu::s_row::destroy(long row_index, bool destroy_chain)
{
    if (row_index != -1) {
        s_row *row = get(row_index);
        if (row->previous != -1) {
            s_row *previous_row = get(row->previous);
            previous_row->next = row->next;
        }
        if (row->next != -1) {
            s_row *next_row = get(row->next);
            next_row->previous = row->previous;
        }
        destroy(row->next, true);
        ::datum_delete(get_light_volume_row_array(), row_index);
    }
}

struct s_game_globals;

// @address 0x7bf690 g_light_volume_gpu_update_shader
#if !defined(_XBOX) || defined(__clang__)
extern "C" {
s_light_volume_update_shader_pair g_light_volume_gpu_update_shader =
{
    { 69, 0, { { 0, g_light_volume_gpu_update_shader.vertex_shader.constants.slots, 7, 0, {} } }, 21 },
    { 69, 0, { 0, g_light_volume_gpu_update_shader.pixel_shader.constants.slots, 1, 0, {} } }
};
}
#endif

extern "C" {
// @address 0x119a500 g_light_volume_gpu_allocation_succeeded
unsigned char g_light_volume_gpu_allocation_succeeded[4];
// @address 0x119a504 g_light_volume_gpu_row_allocation_succeeded
unsigned char g_light_volume_gpu_row_allocation_succeeded[4];
}

static __forceinline unsigned long light_volume_gpu_definition_max(const s_light_volume_definition *definition)
{
    unsigned long count = definition->max_profile_count;
    return count > 128 ? 128 : count;
}
static __forceinline s_contrail_chain_link *light_volume_gpu_row_link_try_get(s_data_array *data, long index)
{
    if (index == -1)
        return 0;
    long absolute_index = index & 0xffff;
    if (absolute_index >= data->first_unallocated)
        return 0;
    s_contrail_chain_link *row = (s_contrail_chain_link *)(data->data + data->size * absolute_index);
    if (row->identifier == 0 || row->identifier != (index >> 16))
        return 0;
    return row;
}
static __forceinline long light_volume_gpu_create_row(unsigned char platform, long next_index, const s_particle_system_limits_owner *limits)
{
    long result;
    if ((unsigned long)((s_data_array *)g_light_volume_gpu_row_data)->actual_count + 1 > limits->table->blocks[8].limits[platform].maximum_count)
    {
        result = -1;
    }
    else
    {
        result = ::datum_new((s_data_array *)g_light_volume_gpu_row_data);
        s_contrail_chain_link *row =
            (s_contrail_chain_link *)(((s_data_array *)g_light_volume_gpu_row_data)->data + (result & 0xffff) * sizeof(s_contrail_chain_link));
        row->index = (unsigned short)result;
        row->previous = -1;
        row->next = next_index;
        s_contrail_chain_link *next = light_volume_gpu_row_link_try_get((s_data_array *)g_light_volume_gpu_row_data, next_index);
        if (next)
            next->previous = result;
        g_light_volume_gpu_row_allocation_succeeded[platform] = 1;
    }
    return result;
}

// @address 0x464798 ?create@c_light_volume_gpu@@SAJEJ@Z
long c_light_volume_gpu::create(unsigned char platform, long light_volume_index)
{
    long index = ::datum_new((s_data_array *)g_light_volume_gpu_data);
    c_light_volume_gpu *volume = c_light_volume_gpu::get(index);
    volume->m_effect_index = light_volume_index;
    const c_light_volume *light_volume = c_light_volume::get(volume->m_effect_index);
    const s_light_volume_definition *definition = light_volume->get_definition();
    volume->m_first_row_index = -1;
    volume->m_row_count = 0;
    unsigned long max_profiles = light_volume_gpu_definition_max(definition);
    unsigned long profile = 0;
    if (max_profiles != 0)
    {
        const s_particle_system_limits_owner *limits = g_effect_budget_limits;
        do
        {
            long row_index = light_volume_gpu_create_row(platform, volume->m_first_row_index, limits);
            if (row_index != -1)
            {
                volume->m_first_row_index = row_index;
            }
            else
            {
                g_light_volume_gpu_allocation_succeeded[platform] = 0;
                if (index != -1)
                {
                    c_light_volume_gpu *destroy_volume = c_light_volume_gpu::get(index);
                    c_light_volume_gpu::s_row::destroy(destroy_volume->m_first_row_index, true);
                    destroy_volume->m_first_row_index = -1;
                    destroy_volume->m_row_count = 0;
                    ::datum_delete((s_data_array *)g_light_volume_gpu_data, index);
                }
                return -1;
            }
            profile += 16;
        } while (profile < max_profiles);
    }
    g_light_volume_gpu_allocation_succeeded[platform] = 1;
    return index;
}

static __forceinline const c_light_volume *light_volume_gpu_shader_volume_get_for_render(long index)
{
    long element_size = ((const s_data_array *)g_light_volume_data)->size;
    // match: 0x464f60: size assumption scales the index by shift-add instead of mullw
    __assume(element_size == sizeof(c_light_volume));
    // match: 0x464f60: reference-bound index temp keeps the needed load order
    const unsigned long &absolute_index = (unsigned short)index;
    return (const c_light_volume *)(((const s_data_array *)g_light_volume_data)->data + absolute_index * element_size);
}
static __forceinline const s_light_volume_definition *light_volume_gpu_render_definition(const c_light_volume *volume)
{
    // match: 0x464f60: reference-bound index temp keeps the needed load order
    const unsigned long &location_index = (unsigned short)volume->m_location_index;
    const c_light_volume_location *location =
        &((const c_light_volume_location *)((s_data_array *)g_light_volume_location_data)->data)[(unsigned short)location_index];
    // match: 0x464f60: reference-bound index temp keeps the needed load order
    const unsigned long &system_index = (unsigned short)location->m_system_index;
    const c_light_volume_system *system =
        &((const c_light_volume_system *)((s_data_array *)g_light_volume_system_data)->data)[(unsigned short)system_index];
    const unsigned long tag_index = (unsigned short)(long)system->m_definition_index;
    const s_light_volume_system_definition *const &definition =
        TAG_GET('ltvl', const s_light_volume_system_definition, tag_index);
    return definition->get_light_volume_definition(volume->get_definition_index());
}
static __forceinline float light_volume_gpu_profile_product(const c_light_volume *volume)
{
    float length = volume->get_length();
    float density = volume->get_profile_density();
    return density * length;
}
static __forceinline const c_light_volume_location *light_volume_gpu_shader_volume_get_location(const c_light_volume *volume)
{
    return c_light_volume_location::get(volume->m_location_index);
}

// @address 0x465c48 ??$set_shader_state@Us_update_vertex_shader_constant_list@c_light_volume_gpu@@@c_light_volume_gpu@@ABAXPBU?$s_gpu_constant_table@Us_update_vertex_shader_constant_list@c_light_volume_gpu@@@@PBVc_light_volume_states@@J@Z
template<>
void c_light_volume_gpu::set_shader_state<c_light_volume_gpu::s_update_vertex_shader_constant_list>(
    const s_gpu_constant_table<c_light_volume_gpu::s_update_vertex_shader_constant_list> *table, const c_light_volume_states *states, long profile_count) const
{
    c_light_volume *volume = c_light_volume::get(m_effect_index);
    const s_light_volume_definition *definition = volume->get_definition();
    const c_light_volume_location *location = light_volume_gpu_shader_volume_get_location(volume);
    light_volume_gpu_shader_overall overall;
    overall.type.value = (float)definition->type;
    overall.blend.value = (float)definition->blend;
    overall.flags.value = (float)definition->appearance_flags;
    overall.offset.value = volume->m_density;
    overall.parameter.value = (float)profile_count;
    overall.density.value = 1.0f / (volume->m_profile_length * volume->m_offset);
    overall.length.value = volume->m_offset;
    overall.profile_length.value = volume->m_offset / volume->m_length;
    overall.uv_tiling_rate.value = definition->uv_tiling_rate;
    overall.uv_scroll_rate.value = definition->uv_scroll_rate;
    overall.time.value = (float)game_time_globals->time * game_time_globals->tick_length;
    overall.origin.value = location->m_position;
    overall.direction.value = location->m_forward;
    overall.profile.scale = 1.0f / definition->profile_scale;
    overall.profile.bias = definition->profile_bias;
    overall.profile.angle_scale = 1.0f / (definition->angle_scale * 0.017453292f);
    overall.profile.angle_bias = definition->angle_bias * 0.017453292f;
    overall.states[0].value = 0.0f;
    memset(&overall.states[1], 0, sizeof(overall.states) - sizeof(overall.states[0]));
    for (long index = 0; index < 12; index++) {
        if (definition->flags & (1 << index))
            overall.states[index].value = states->get_state_value(index);
    }
    ((c_rasterizer_constant_table_ref const *)(s_gpu_constant_table_base *)table)
        ->set_constants(&table->descs[2], (void const *)(&overall));
}

// @address 0x465b08 ??$set_shader_functions@Us_update_vertex_shader_constant_list@c_light_volume_gpu@@@c_light_volume_gpu@@ABAXPBU?$s_gpu_constant_table@Us_update_vertex_shader_constant_list@c_light_volume_gpu@@@@@Z
template<>
void c_light_volume_gpu::set_shader_functions<c_light_volume_gpu::s_update_vertex_shader_constant_list>(
    const s_gpu_constant_table<s_update_vertex_shader_constant_list> *table) const
{
    const c_light_volume *volume = c_light_volume::get(m_effect_index);
    const s_light_volume_definition *data = volume->get_definition();
    ((c_constant_table *)table)->set_scaled(3, data->gpu_functions.count, data->gpu_functions.address);
    ((c_constant_table *)table)->set(4, (unsigned char)data->gpu_colors.count, data->gpu_colors.address);
    ((c_constant_table *)table)->set(5, (unsigned char)data->gpu_properties.count, data->gpu_properties.address);
}

// @address 0x465f48 ??$set_shader_strip@Us_update_vertex_shader_constant_list@c_light_volume_gpu@@@c_light_volume_gpu@@ABAXPBU?$s_gpu_constant_table@Us_update_vertex_shader_constant_list@c_light_volume_gpu@@@@@Z
template<>
void c_light_volume_gpu::set_shader_strip<c_light_volume_gpu::s_update_vertex_shader_constant_list>(
    const s_gpu_constant_table<s_update_vertex_shader_constant_list> *table) const
{
    struct s_strip { light_volume_gpu_scalar rows[8]; };
    s_strip strip = { 0.0f };
    long row_count = 0;
    s_contrail_chain_link *row;
    long row_index;
    for (row_index = m_first_row_index;
        (row = light_volume_gpu_row_link_try_get((s_data_array *)g_light_volume_gpu_row_data, row_index)) != 0 && row_count < 8; row_index = row->next)
        row_count++;
    long row_number = 0;
    for (row_index = m_first_row_index; (row = light_volume_gpu_row_link_try_get((s_data_array *)g_light_volume_gpu_row_data,
        row_index)) != 0; row_index = row->next) {
        strip.rows[row_count - row_number - 1].value = (float)row->index;
        row_number++;
    }
    ((c_rasterizer_constant_table_ref const *)(s_gpu_constant_table_base *)table)
        ->set_constants(&table->descs[1], (void const *)(&strip));
}

// @address 0x466098 ??$set_shader_state@Us_render_vertex_shader_constant_list@c_light_volume_gpu@@@c_light_volume_gpu@@ABAXPBU?$s_gpu_constant_table@Us_render_vertex_shader_constant_list@c_light_volume_gpu@@@@PBVc_light_volume_states@@J@Z
template<>
void c_light_volume_gpu::set_shader_state<c_light_volume_gpu::s_render_vertex_shader_constant_list>(
    const s_gpu_constant_table<c_light_volume_gpu::s_render_vertex_shader_constant_list> *table, const c_light_volume_states *states, long profile_count) const
{
    c_light_volume *volume = c_light_volume::get(m_effect_index);
    const s_light_volume_definition *definition = volume->get_definition();
    const c_light_volume_location *location = light_volume_gpu_shader_volume_get_location(volume);
    light_volume_gpu_shader_overall overall;
    overall.type.value = (float)definition->type;
    overall.blend.value = (float)definition->blend;
    overall.flags.value = (float)definition->appearance_flags;
    overall.offset.value = volume->m_density;
    overall.parameter.value = (float)profile_count;
    overall.density.value = 1.0f / (volume->m_profile_length * volume->m_offset);
    overall.length.value = volume->m_offset;
    overall.profile_length.value = volume->m_offset / volume->m_length;
    overall.uv_tiling_rate.value = definition->uv_tiling_rate;
    overall.uv_scroll_rate.value = definition->uv_scroll_rate;
    overall.time.value = (float)game_time_globals->time * game_time_globals->tick_length;
    overall.origin.value = location->m_position;
    overall.direction.value = location->m_forward;
    overall.profile.scale = 1.0f / definition->profile_scale;
    overall.profile.bias = definition->profile_bias;
    overall.profile.angle_scale = 1.0f / (definition->angle_scale * 0.017453292f);
    overall.profile.angle_bias = definition->angle_bias * 0.017453292f;
    overall.states[0].value = 0.0f;
    memset(&overall.states[1], 0, sizeof(overall.states) - sizeof(overall.states[0]));
    for (long index = 0; index < 12; index++) {
        if (definition->flags & (1 << index))
            overall.states[index].value = states->get_state_value(index);
    }
    ((c_rasterizer_constant_table_ref const *)(s_gpu_constant_table_base *)table)
        ->set_constants(&table->descs[1], (void const *)(&overall));
}

// @address 0x466398 ??$set_shader_strip@Us_render_vertex_shader_constant_list@c_light_volume_gpu@@@c_light_volume_gpu@@ABAXPBU?$s_gpu_constant_table@Us_render_vertex_shader_constant_list@c_light_volume_gpu@@@@@Z
template<>
void c_light_volume_gpu::set_shader_strip<c_light_volume_gpu::s_render_vertex_shader_constant_list>(
    const s_gpu_constant_table<s_render_vertex_shader_constant_list> *table) const
{
    struct s_strip { light_volume_gpu_scalar rows[8]; };
    s_strip strip = { 0.0f };
    long row_count = 0;
    s_contrail_chain_link *row;
    long row_index;
    for (row_index = m_first_row_index;
        (row = light_volume_gpu_row_link_try_get((s_data_array *)g_light_volume_gpu_row_data, row_index)) != 0 && row_count < 8; row_index = row->next)
        row_count++;
    long row_number = 0;
    for (row_index = m_first_row_index; (row = light_volume_gpu_row_link_try_get((s_data_array *)g_light_volume_gpu_row_data,
        row_index)) != 0; row_index = row->next) {
        strip.rows[row_count - row_number - 1].value = (float)row->index;
        row_number++;
    }
    ((c_rasterizer_constant_table_ref const *)(s_gpu_constant_table_base *)table)
        ->set_constants(&table->descs[0], (void const *)(&strip));
}

// @address 0x464ae0 ?initialize_shaders@c_light_volume_gpu@@CAXXZ
void c_light_volume_gpu::initialize_shaders()
{
    g_light_volume_gpu_update_shader.initialize();
}

// c_light_volume_states: the per-pass state block the GPU walks the volumes with
// @address 0x8c442c k_light_volume_states_initial_flags

extern "C" {
// @address 0x8c4849 g_gpu_particle_block_until_idle
unsigned char g_gpu_particle_block_until_idle;
// @address 0x8c484a g_light_volume_gpu_block_until_idle
unsigned char g_light_volume_gpu_block_until_idle;
}
// c_light_volume_states: the per-pass state block the GPU walks the volumes with
static __forceinline const c_light_volume_system *light_volume_gpu_shader_volume_get_system(const c_light_volume *volume)
{
    const c_light_volume_location *location = light_volume_gpu_shader_volume_get_location(volume);
    return datum_get<c_light_volume_system>(g_light_volume_system_data, location->m_system_index);
}

class c_rasterizer_vertex_shader;
class c_rasterizer_pixel_shader;
class c_rasterizer_vertex_buffer;
inline s_light_volume_gpu_target_table::s_light_volume_gpu_target_table() :
    position(0, 0),
    unused_ushort4n(3, 0),
    unused_half4(3, 1),
    color(8, 0)
{
    slots[0] = &position;
    slots[1] = &unused_ushort4n;
    slots[2] = &unused_half4;
    slots[3] = &color;
}

extern "C" {

// @address 0x7bf508 g_light_volume_gpu_buffer
#if !defined(_XBOX) || defined(__clang__)
common_gpu_vertex_buffer g_light_volume_gpu_buffer = { 0x1d, 0x10, 0xa0 };
#endif

// @address 0x7bf550 g_light_volume_gpu_target_table
// The braced temporary constant-folds entries/slots/storages into the image (a plain `S g[1];`
// emits a dynamic initializer); the array-of-one keeps the caller's decay use compiling.
s_light_volume_gpu_target_table g_light_volume_gpu_target_table[1] = { s_light_volume_gpu_target_table() };

}

// The GPU-side copy of each active light volume's bounds is refreshed whenever
// the thread's current volume value changes.


// @address 0x11df8a4 g_light_volume_gpu_buffer_address
extern "C" {
__declspec(thread) void *g_light_volume_gpu_buffer_address;
}

// @address 0x464600 ?light_volume_gpu_set_buffer_address@@YAXPAX@Z
void light_volume_gpu_set_buffer_address(void *value)
{
    if (value != g_light_volume_gpu_buffer_address && value != 0)
    {
        g_light_volume_gpu_buffer_address = value;
        common_gpu_vertex_buffer_set_address(&g_light_volume_gpu_buffer, value);
        s_gpu_target_table<4> *table = (s_gpu_target_table<4> *)g_light_volume_gpu_target_table;
        for (unsigned int entry_index = 0; entry_index < 4; entry_index++)
        {
            s_gpu_storage_interface *entry = table->entries[entry_index];
            entry->refresh();
            table->bounds[entry_index].bounds = entry->bounds;
        }
    }
}

// @address 0x464b90 ?frame_advance_all@c_light_volume_gpu@@SAXM@Z
void c_light_volume_gpu::frame_advance_all(float elapsed_time)
{
    if (((s_data_array *)g_light_volume_gpu_data)->first_unallocated > 0) {
        c_rasterizer::set_render_target(0, (c_rasterizer::e_surface)0);
        D3DDevice_SetRenderState_ColorWriteEnable(c_rasterizer::get_device(), 0);
        c_rasterizer::set_render_target(1, (c_rasterizer::e_surface)0);
        c_rasterizer::set_render_target(2, (c_rasterizer::e_surface)0);
        ::c_rasterizer::set_vertex_stream(&g_light_volume_gpu_buffer);
        ((s_gpu_constant_table_base *)&g_light_volume_gpu_update_shader.vertex_shader.constants)->own_constants();
        c_rasterizer::set_vertex_shader(
            TAG_GET('vtsh', const c_rasterizer_vertex_shader, g_light_volume_gpu_update_shader.vertex_shader.shader_reference->index),
            (e_vertex_type)g_light_volume_gpu_update_shader.vertex_shader.vertex_type,
            _transfer_vector_vertex_none, _entry_point_default);
        ((s_gpu_constant_table_base *)&g_light_volume_gpu_update_shader.pixel_shader.constants)->own_constants();
        c_rasterizer::set_pixel_shader(
            TAG_GET('pixl', const c_rasterizer_pixel_shader, g_light_volume_gpu_update_shader.pixel_shader.shader_reference->index),
            _entry_point_default);
        D3DDevice_BeginExport(c_rasterizer::get_device(), 0, g_light_volume_gpu_buffer.get_d3d_buffer(), 0);
        s_gpu_constant_table_base *constant_table = (s_gpu_constant_table_base *)&g_light_volume_gpu_update_shader.vertex_shader.constants;
        ((c_rasterizer_constant_table_ref const *)constant_table)
            ->set_constants(
                constant_table->descs, (void const *)((s_gpu_target_table<4> *)g_light_volume_gpu_target_table)->bounds);
#if defined(_XBOX) && !defined(__clang__)
        // match: 0x464b90: the volatile zero load uses r10 after the argument setup. A literal moves the load earlier and uses r11.
        float zero = k_light_volume_gpu_zero;
#else
        float zero = 0.0f;
#endif
        c_light_volume_states states(zero);
        c_data_iterator<c_light_volume_gpu> iterator;
        iterator.begin((s_data_array *)g_light_volume_gpu_data);
        while (iterator.next())
        {
            c_light_volume_gpu *datum = iterator.get_datum();
            if (datum->m_first_row_index != -1)
                datum->frame_advance(&states);
        }
        D3DDevice_EndExport(c_rasterizer::get_device(), 0, g_light_volume_gpu_buffer.get_d3d_buffer(), 0);
        ((s_gpu_constant_table_base *)&g_light_volume_gpu_update_shader.vertex_shader.constants)->disown_constants();
        ((s_gpu_constant_table_base *)&g_light_volume_gpu_update_shader.pixel_shader.constants)->disown_constants();
        g_game_state_render_fence = D3DDevice_InsertFence(c_rasterizer::get_device());
    }
}

class c_restricted_memory;

// @address 0x23db48 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_light_volume_row_gpu_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template <>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_light_volume_row_gpu_allocator>::allocate(unsigned int size, const char *name)
{
    int member_index = g_restricted_regions[3].add_member(name, "", size, 0, __tls_set_g_light_volume_row_gpu_allocator, 0, 0);
    unsigned int offset = g_restricted_regions[3].get_member_offset(member_index);
    data_set_new_base_address(&g_static_lighting_effect_attachments,
        (s_data_array *)(g_restricted_address[3] + offset));
    m_member_index = member_index;
    m_thread_id = get_current_thread_index();
    return g_restricted_address[3] + g_restricted_regions[3].get_member_offset(member_index);
}

// @address 0x23dbe8 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_light_volume_row_gpu_allocator@@YAXPAX@Z@@UAAXPAX@Z
template <>
void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_light_volume_row_gpu_allocator>::deallocate(void *memory)
{
    g_static_lighting_effect_attachments = 0;
    restricted_region_free_member(3, m_member_index);
}

// @address 0x4652a0 ?allocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_light_volume_gpu_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template <>
void *t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_light_volume_gpu_allocator>::allocate(unsigned int size, const char *name)
{
    int member_index = g_restricted_regions[2].add_member(name, "", size, 0, __tls_set_g_light_volume_gpu_allocator, 0, 0);
    unsigned int offset = g_restricted_regions[2].get_member_offset(member_index);
    data_set_new_base_address(&g_light_volume_gpu_data, (s_data_array *)(g_restricted_address[2] + offset));
    m_member_index = member_index;
    m_thread_id = get_current_thread_index();
    return g_restricted_address[2] + g_restricted_regions[2].get_member_offset(member_index);
}

// @address 0x465360 ?allocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_light_volume_row_gpu_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template <>
void *t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_light_volume_row_gpu_allocator>::allocate(unsigned int size, const char *name)
{
    int member_index = g_restricted_regions[2].add_member(name, "", size, 0, __tls_set_g_user_widget_allocator_0, 0, 0);
    unsigned int offset = g_restricted_regions[2].get_member_offset(member_index);
    data_set_new_base_address(&g_light_volume_gpu_row_data,
        (s_data_array *)(g_restricted_address[2] + offset));
    m_member_index = member_index;
    m_thread_id = get_current_thread_index();
    return g_restricted_address[2] + g_restricted_regions[2].get_member_offset(member_index);
}

// @address 0x465420 ?reserve_memory@?$t_restricted_allocation_manager@$01$0A@$0A@$1?light_volume_gpu_set_buffer_address@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
template<>
void *t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &light_volume_gpu_set_buffer_address>::reserve_memory(const char *name, const char *type, unsigned int size, long alignment_bits)
{
    int member_index = g_restricted_regions[2].add_member(name, type, size, alignment_bits,
        light_volume_gpu_set_buffer_address, 0, 0);
    unsigned int offset = g_restricted_regions[2].get_member_offset(member_index);
    light_volume_gpu_set_buffer_address(g_restricted_address[2] + offset);
    m_member_index = member_index;
    m_thread_id = get_current_thread_index();
    return g_restricted_address[2] + g_restricted_regions[2].get_member_offset(member_index);
}

// @address 0x4654a8 ?allocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?light_volume_gpu_set_buffer_address@@YAXPAX@Z@@UAAPAXIPBD@Z
template<>
void *t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &light_volume_gpu_set_buffer_address>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

template<>
void t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &light_volume_gpu_set_buffer_address>::deallocate(void *)
{
    restricted_region_free_member(2, m_member_index);
}

extern "C" {
// @address 0x70a310 ??__Eg_beam_gpu_allocator@@YAXXZ
// @address 0x70ca68 ??__Fg_beam_gpu_allocator@@YAXXZ
// @address 0x7bf4e4 g_beam_gpu_allocator
t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_light_volume_gpu_allocator> g_beam_gpu_allocator;
// @address 0x70a320 ??__Eg_beam_gpu_row_allocator@@YAXXZ
// @address 0x70ca80 ??__Fg_beam_gpu_row_allocator@@YAXXZ
// @address 0x7bf4f0 g_beam_gpu_row_allocator
t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_light_volume_row_gpu_allocator> g_beam_gpu_row_allocator;
// @address 0x70a330 ??__Eg_beam_gpu_buffer_allocation@@YAXXZ
// @address 0x70ca98 ??__Fg_beam_gpu_buffer_allocation@@YAXXZ
// @address 0x7bf4fc g_beam_gpu_buffer_allocation
t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &light_volume_gpu_set_buffer_address> g_beam_gpu_buffer_allocation;
}

// @address 0x4646b0 beam_gpu_initialize
extern "C" void beam_gpu_initialize(void)
{
    g_light_volume_gpu_data = data_new("c_beam_gpu", 160, 16, 0, (c_allocation_base *)&g_beam_gpu_allocator);
    g_light_volume_gpu_data = data_new("c_beam_gpu::s_row", 160, 12, 0, (c_allocation_base *)&g_beam_gpu_row_allocator);
    common_gpu_vertex_buffer *configuration = &g_light_volume_gpu_buffer;
    int stride = rasterizer_vertex_get_stride_from_declaration(&g_rasterizer_vertex_buffer_declarations[configuration->format]);
    int size = stride;
    const int *dimensions = &configuration->element_size;
    size *= dimensions[0];
    size *= dimensions[-1];
    g_light_volume_gpu_buffer_address = g_beam_gpu_buffer_allocation.reserve_memory(
        "c_particle_emitter_gpu", "x_gamestate_storage", (size + 127) & 0xffffff80, 7);
}

// @address 0x7bf724 g_light_volume_gpu_render_shader
#if !defined(_XBOX) || defined(__clang__)
extern "C" {
s_gpu_global_vertex_shader<c_light_volume_gpu::s_render_vertex_shader_constant_list> g_light_volume_gpu_render_shader =
{
    0, { { 0, g_light_volume_gpu_render_shader.constants.slots, 2, 0, {} } }, 21
};
}
#endif
// @address 0x4654c0 ?initialize_constant_table@?$s_gpu_global_vertex_shader@Us_render_vertex_shader_constant_list@c_light_volume_gpu@@@@AAAPAUs_entry_table@@XZ
template<>
s_entry_table *s_gpu_global_vertex_shader<c_light_volume_gpu::s_render_vertex_shader_constant_list>::initialize_constant_table()
{
    // match: 0x4654c0: value range assumption guides branch lowering
    __assume(this == &g_light_volume_gpu_render_shader);
    const s_render_method_shader_definition *render_method = (const s_render_method_shader_definition *)tag_get('rmdf', shader_reference->index);
    s_rasterizer_global_vertex_shader_definition *vertex_shader = (s_rasterizer_global_vertex_shader_definition *)tag_get('glvs',
        render_method->shared_vertex_shader.index);
    c_typed_tag_block<s_global_vertex_shader_entry_point> *vertex_entries =
        &((c_typed_tag_block<s_global_vertex_shader_entry_point> *)vertex_shader->vertex_types.address)[vertex_type];
    s_global_vertex_shader_entry_point *vertex_entry = &(*vertex_entries)[0];
    c_rasterizer_compiled_vertex_shader *compiled_shader =
        &((c_rasterizer_compiled_vertex_shader *)vertex_shader->shaders.address)[vertex_entry->constant_index];
    return compiled_shader ? compiled_shader->get_constant_table() : 0;
}

template<>
void s_gpu_global_shader<c_light_volume_gpu::s_render_vertex_shader_constant_list>::initialize(c_rasterizer_constant_table_ref table_ref)
{
    s_entry_table *table = table_ref.table;
    this->shader = table;
    const int *key = c_light_volume_gpu::s_render_vertex_shader_constant_list::x_keys;
    for (int slot_index = 0; slot_index < 2; slot_index++)
    {
        int entry_index = gpu_constant_table_find_entry(this->shader, *key);
        if (entry_index != -1)
            static_cast<s_constant_entry &>(slots[slot_index]) = this->shader->entries[entry_index];
        slots[slot_index].owned = c_light_volume_gpu::s_render_vertex_shader_constant_list::x_in_place[slot_index];
        valid = valid || slots[slot_index].owned;
        key++;
    }
}

// @address 0x464008 light_volume_gpu_shader_initialize
extern "C" void light_volume_gpu_shader_initialize(s_gpu_global_shader<c_light_volume_gpu::s_render_vertex_shader_constant_list> *shader, s_entry_table *table)
{
    shader->shader = table;
    const int *key = k_light_volume_render_keys;
    for (int slot_index = 0; slot_index < 2; slot_index++)
    {
        int entry_index = gpu_constant_table_find_entry(shader->shader, *key);
        if (entry_index != -1)
            static_cast<s_constant_entry &>(shader->slots[slot_index]) = shader->shader->entries[entry_index];
        shader->slots[slot_index].owned = k_light_volume_render_required[slot_index];
        shader->valid = shader->valid || shader->slots[slot_index].owned;
        key++;
    }
}

struct s_weighted_atmosphere_parameters;
class c_render_method;
struct D3DDevice;
extern "C" {
// @address 0x7bf4b4 g_effect_shader_cache


}

// @address 0x1820c8 ?render@c_light_volume_gpu@@QBAXPBUs_effect_parameters@@@Z
void c_light_volume_gpu::render(const s_effect_parameters *parameters) const
{
    if (m_first_row_index != -1)
    {
        s_particle_instance *instance = datum_get<s_particle_instance>(g_particle_instance_data, m_effect_index);
        s_effect_contrail_location *location = datum_get<s_effect_contrail_location>(g_particle_instance_location_data, instance->location_index);
        c_light_volume_system *owner_datum = datum_get<c_light_volume_system>(g_particle_instance_owner_data, location->owner_index);
        s_gpu_effect_definition *definition = TAG_GET('ltvl', s_gpu_effect_definition, owner_datum->m_definition_index);
        s_effect_entry *entry = &definition->entries[instance->definition_index];
        c_rasterizer::set_vertex_stream(&g_contrail_frame_vertex_buffer);
        float product = instance->density * instance->length;
        long count = (long)product;
        if (count > 0)
        {
            bool flag = ((entry->count >> 1) & 1) != 0;
            if (flag && atmosphere_constants_index != -2)
            {
                atmosphere_constants_index = -2;
                c_atmosphere_fog_interface::set_constant(&c_atmosphere_fog_interface::m_default_parameters, true, true);
            }
            const c_render_method *render_method = (const c_render_method *)&entry->render_method_reference;
            if (!g_effect_shader_cache.is_initialized())
            {
                g_effect_shader_cache.shader_reference = &entry->render_method_reference;
                light_volume_gpu_shader_initialize(
                    &g_effect_shader_cache.constants,
                    contrail_gpu_get_constant_table(&g_effect_shader_cache));
            }
            light_volume_gpu_set_effect_constants((s_handle *)this,
                (s_pass_state *)(&g_effect_shader_cache.constants), parameters);
            set_shader_strip(g_effect_shader_cache.get_constant_table(), count);
            c_render_method_data::m_valid = true;
            s_effect_contrail_location *submit_location
                = datum_get<s_effect_contrail_location>(g_particle_instance_location_data, instance->location_index);
            c_light_volume_system *submit_owner = datum_get<c_light_volume_system>(g_particle_instance_owner_data, submit_location->owner_index);
            if (::render_method_submit(submit_owner->m_definition_index, render_method,
                    _vertex_type_light_volume, _transfer_vector_vertex_none,
                    _entry_point_default, false))
            {
                c_rasterizer::draw_primitive(c_rasterizer_index_buffer::_primitive_type_quad_list, 0, count);
                if (g_gpu_particle_block_until_idle != 0)
                    D3DDevice_BlockUntilIdle((D3DDevice *)c_rasterizer::get_device());
            }
            c_render_method_data::m_valid = false;
            render_method_submit_valid();
        }
    }
}

template<>
void s_gpu_shader<c_light_volume_gpu::s_update_vertex_shader_constant_list>::initialize(c_rasterizer_constant_table_ref table_ref)
{
    s_entry_table *table = table_ref.table;
    this->table = table;
    const int *key = c_light_volume_gpu::s_update_vertex_shader_constant_list::x_keys;
    for (int slot_index = 0; slot_index < 7; slot_index++)
    {
        int entry_index = gpu_constant_table_find_entry(this->table, *key);
        if (entry_index != -1)
            slots[slot_index].entry = this->table->entries[entry_index];
        slots[slot_index].required = c_light_volume_gpu::s_update_vertex_shader_constant_list::x_in_place[slot_index];
        any_required = any_required || slots[slot_index].required;
        key++;
    }
}

#define LIGHT_VOLUME_GPU_MAX(a, b) ((a) > (b) ? (a) : (b))

static __forceinline bool light_volume_gpu_declaration_full(const s_declaration_element *end, long count)
{
    return 64 - count < 64 && end[-1].Stream >= 1;
}

// @address 0x4656f0 light_volume_gpu_initialize_targets
extern "C" void light_volume_gpu_initialize_targets(void)
{
    s_gpu_target_table<4> *targets = (s_gpu_target_table<4> *)g_light_volume_gpu_target_table;
    unsigned long largest = 0;
    unsigned long total = 0;
    for (unsigned long i = 0; i < 4; i++) {
        s_gpu_storage_interface *entry = targets->entries[i];
        total += entry->size();
        largest = LIGHT_VOLUME_GPU_MAX(largest, entry->size());
    }
    unsigned long stride = ((total + largest - 1) / largest) * largest;
    unsigned long offset = 0;
    for (unsigned long i = 0; i < 4; i++) {
        s_gpu_storage_interface *entry = targets->entries[i];
        entry->owner = &g_light_volume_gpu_buffer;
        entry->layout(stride, offset);
        offset += entry->size();
        real_point2d size;
        size.x = (float)entry->stride;
        size.y = (float)entry->offset;
        targets->bounds[i].size = size;
    }
    unsigned long padding = stride - offset;
    int padding_type;
    unsigned char padding_kind;
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
        // match: 0x4656f0: unreachable hint omits dead branch check
        __assume(0);
    }

    rasterizer_vertex_declaration *vertex_declaration = &g_rasterizer_vertex_buffer_declarations[29];
    vertex_declaration->index = 29;
    for (unsigned long i = 0; i < 4; i++) {
        s_gpu_storage_interface *entry = targets->entries[i];
        rasterizer_vertex_register_mapping *register_mapping = &vertex_declaration->register_mappings[i];
        register_mapping->attribute = (signed char)entry->semantic;
        register_mapping->format = entry->kind();
        register_mapping->usage = (unsigned char)k_vertex_semantic_usages[entry->semantic];
        register_mapping->usage_index = (unsigned char)entry->usage_index;
    }
    long count = 4;
    if (padding != 0) {
        vertex_declaration->register_mappings[4].attribute = 0;
        vertex_declaration->register_mappings[4].format = padding_kind;
        vertex_declaration->register_mappings[4].usage = 0;
        vertex_declaration->register_mappings[4].usage_index = 15;
        count = 5;
    }
    vertex_declaration->register_mappings[count].attribute = -1;

    long declaration_index = 21;
    s_declaration_element *end = c_vertex_declaration_table::base_vertex_decls[declaration_index];
    while (memcmp(end, &k_vertex_declaration_terminator, sizeof(s_declaration_element)) != 0)
        end++;
    if (!light_volume_gpu_declaration_full(end, end - c_vertex_declaration_table::base_vertex_decls[declaration_index])) {
        for (unsigned long i = 0; i < 4; i++) {
            s_gpu_storage_interface *entry = targets->entries[i];
            s_declaration_element *element = &end[i];
            element->Stream = 1;
            element->Offset = (unsigned short)(entry->size() * entry->offset);
            element->Type = entry->declaration_type();
            element->Method = 0;
            element->Usage = (unsigned char)k_vertex_semantic_usages[entry->semantic];
            element->UsageIndex = (unsigned char)entry->usage_index;
        }
        long element_count = 4;
        if (padding != 0) {
            element_count = 5;
            end[4].Stream = 1;
            end[4].Offset = (unsigned short)offset;
            end[4].Type = padding_type;
            end[4].Method = 0;
            end[4].Usage = 0;
            end[4].UsageIndex = 15;
        }
        memcpy(&end[element_count], &k_vertex_declaration_terminator, sizeof(s_declaration_element));
    }
}

// @address 0x1820c0 light_volume_submit_particle_instance
extern "C" void light_volume_submit_particle_instance(s_particle_instance *instance)
{
    particle_submit_gpu_state(instance);
}

// @address 0x464d48 ?frame_advance@c_light_volume_gpu@@AAAXPAVc_light_volume_states@@@Z
void c_light_volume_gpu::frame_advance(c_light_volume_states *states)
{
    const c_light_volume *volume = c_light_volume::get(m_effect_index);
    long profile_count = MAX(MIN(128, (long)(float)ceil(light_volume_gpu_profile_product(volume))), 2);
    states->set_light_volume_system((const c_light_volume_system *)light_volume_gpu_shader_volume_get_system(volume));
    states->set_light_volume_location((const c_light_volume_location *)light_volume_gpu_shader_volume_get_location(volume));
    states->set_light_volume((const c_light_volume *)volume);
    s_gpu_constant_table<s_update_vertex_shader_constant_list> *table =
        (s_gpu_constant_table<s_update_vertex_shader_constant_list> *)&g_light_volume_gpu_update_shader.vertex_shader.constants;
    ((c_rasterizer_constant_table_ref const *)(s_gpu_constant_table_base *)table)
        ->set_constants(&table->descs[6], (void const *)g_gpu_particle_default_constants);
    set_shader_functions(table);
    set_shader_state(table, states, profile_count);
    set_shader_strip(table);
    c_rasterizer::draw_primitive(c_rasterizer_index_buffer::_primitive_type_point_list, 0, profile_count);
    if (g_light_volume_gpu_block_until_idle != 0)
        D3DDevice_BlockUntilIdle((D3DDevice *)c_rasterizer::get_device());
}

// @address 0x464f60 ?render@c_light_volume_gpu@@QBAXPBVc_light_volume_states@@@Z
void c_light_volume_gpu::render(const c_light_volume_states *states) const
{
    if (m_first_row_index != -1)
    {
        const c_light_volume *volume = light_volume_gpu_shader_volume_get_for_render(m_effect_index);
        const s_light_volume_definition *definition = light_volume_gpu_render_definition(volume);
        c_rasterizer::set_vertex_stream(&g_light_volume_gpu_buffer);
        long profile_count = MAX(MIN(128, (long)(float)ceil(light_volume_gpu_profile_product(volume))), 2);
        long vertex_count = profile_count * 2;
        long primitive_count = vertex_count - 2;
        unsigned long sides;
        if (definition->type >= 1)
        {
            if (definition->type != 1)
                sides = definition->blend;
            else
                sides = 2;
        }
        else
            sides = 1;
        if (definition->get_fogged() && atmosphere_constants_index != -2)
        {
            atmosphere_constants_index = -2;
            c_atmosphere_fog_interface::set_constant(&c_atmosphere_fog_interface::m_default_parameters, true, true);
        }
        unsigned long previous_cull_mode;
        if (definition->appearance_flags & 1)
        {
            previous_cull_mode = D3DDevice_GetRenderState_CullMode(c_rasterizer::get_device());
            D3DDevice_SetRenderState_CullMode(c_rasterizer::get_device(), 0);
        }
        const c_render_method *render_method = (const c_render_method *)&definition->render_method_reference;
        if (!g_light_volume_gpu_render_shader.is_initialized())
            g_light_volume_gpu_render_shader.initialize((const s_tag_reference *)render_method);
        set_shader_state(g_light_volume_gpu_render_shader.get_constant_table(), states, profile_count);
        set_shader_strip(g_light_volume_gpu_render_shader.get_constant_table());
        c_render_method_data::m_valid = true;
        if (render_method_submit(light_volume_gpu_shader_volume_get_system(volume)->m_definition_index,
            render_method, k_number_of_vertex_types,
            _transfer_vector_vertex_none, _entry_point_default, false))
        {
            if (sides)
            {
                long first_vertex = 0;
                unsigned long remaining = sides;
                do
                {
                    c_rasterizer::draw_primitive(c_rasterizer_index_buffer::_primitive_type_triangle_strip, first_vertex, primitive_count);
                    first_vertex += vertex_count;
                } while (--remaining);
            }
            if (g_light_volume_gpu_block_until_idle)
                D3DDevice_BlockUntilIdle((D3DDevice *)c_rasterizer::get_device());
        }
        if (definition->appearance_flags & 1)
            D3DDevice_SetRenderState_CullMode(c_rasterizer::get_device(), previous_cull_mode);
        c_render_method_data::m_valid = false;
        render_method_submit_valid();
    }
}

// @address 0x11df8a0 g_light_volume_gpu_storage_address
__declspec(thread) void *g_light_volume_gpu_storage_address;
