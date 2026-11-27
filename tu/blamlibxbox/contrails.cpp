// blamlibxbox/contrails.obj: from the records 0x14b590 0x14b6e8 0x14bb18 0x14bdd8 0x232ce8 0x2b4548 0x2b4620 0x2b46d8 0x2b4770 0x2b4840 0x2b4948 0x2b49b0 0x2b4a98 0x2b4b50 0x2b4c38 0x2b4d28 0x2b4ee8 0x2b4f50 0x2b4fa8 0x2b5090 0x2b5130 0x2b5200 0x2b52f0 0x2b5308 0x2b5558 0x2b5630 0x2b5830 0x2b5990 0x2b5b08 0x2b5d00 0x2b5d08 0x2b5e68 0x2b6018 0x2b60d0 0x2b6170 0x2b6190 0x2b6230 0x2b6250 0x2b62f0 0x2b6310 0x2b63b0 0x2b6ff8 0x45f268 0x45f2c8 0x45fd80 0x45fe20 0x461210 0x552d60
// @address 0x8c43a8 g_effect_budget_limits
// @address 0x11df7f8 g_light_volume_location_data
#include "effects/effect_runtime_storage.h"
// @address 0x11df790 g_particle_instance_owner_data
// @address 0x11df7f4 g_particle_instance_location_data
// @address 0x11df814 g_particle_instance_data
// @address 0x11df804 g_contrail_gpu_data
// @address 0x11df820 g_contrail_gpu_row_data
#include "effects/particle_array_globals.h"
#include "memory/tls_data_arrays.h"
// @flags /TP /O2
#include <xtl.h>
#include "math/matrix_math_types.h"
#include "effects/effect_array_globals.h"
#include "memory/data_types.h"
#include "gpu_particle/contrail_update_shader_pair.h"
// @address 0x2b7830 ?evaluate@?$c_editable_property@Vc_light_volume_states@@@@QBAMPBVc_light_volume_states@@@Z
// @address 0x232ce8 ?__tls_set_g_contrail_location_allocator@@YAXPAX@Z
// @address 0x2b6ff8 ?__tls_set_g_contrail_profile_allocator@@YAXPAX@Z
// @address 0x45f2c8 ?__tls_set_g_contrail_allocator@@YAXPAX@Z
// @address 0x552d60 ?__tls_set_g_contrail_system_allocator@@YAXPAX@Z
// @address 0x2b4d10 ?__tls_set_g_decal_array_allocator@@YAXPAX@Z
// @address 0x2b5e50 ?__tls_set_g_hs_thread_non_deterministic_data_allocator@@YAXPAX@Z
// @address 0x2b4530 ?__tls_set_g_object_list_header_data_allocator@@YAXPAX@Z
// @address 0x1ec580 ?data_delete_all@@YAXPAUs_data_array@@@Z
// @address 0x1ec160 ?data_new@@YAPAUs_data_array@@PBDJJJPAVc_allocation_base@@@Z
// @address 0x1ec830 ?data_set_new_base_address@@YAXPAPAUs_data_array@@PAU1@@Z
// @address 0xaae70 ?datum_new@@YAJPAUs_data_array@@@Z
// @address 0x4614b0 ?initialize_shaders@c_contrail_gpu@@CAXXZ
// @address 0x1d6bb0 ?restricted_region_free_member@@YAXJJ@Z
// @address 0x46af10 ?rasterizer_vertex_get_stride_from_declaration@@YAEPBUrasterizer_vertex_declaration@@@Z
// @address 0x2b4a98 ?query_effect@c_contrail_system@@QAAXM@Z
#include "cseries/crt_xdk.h"
#include <stdlib.h>
#include "tag_files/tag_types.h"
#include "cache/restricted_memory_regions.h"
#include "effects/contrails.h"
#include "effects/decals.h"
#include "hs/hs_runtime.h"
#include "hs/object_lists.h"
#include "rasterizer/rasterizer_vertex_definitions.h"
#include "effects/effects.h"
#include "gpu_particle/contrail_gpu_types.h"
#include "gpu_particle/contrail_gpu.h"
#include "gpu_particle/particle_block.h"
#include "effects/particle.h"
#include "effects/effect_contrail_location_types.h"
#include "effects/particle_system_limits_owner_types.h"
#include "effects/transparent_sort_layer_types.h"
#include "effects/event_status_types.h"
#include "effects/effect_contrail_profile_types.h"
#include "render/render_objects_static_lighting.h"
#include "rasterizer/rasterizer_vertex_types.h"
#include "math/real_math_types.h"
#include "effects/light_volume_states.h"
#include "effects/light_volumes_globals.h"
#include "cache/restricted_memory.h"
#include "cache/restricted_memory_registration.h"
#include "math/function_definitions.h"
#include "effects/contrails_globals.h"
#include "gpu_particle/gpu_particle_globals.h"
#include "memory/data.h"
#include "math/random_math_range.h"
#include "cache/cache_files.h"
#include "visibility/visibility_projections_and_volumes.h"
#include "math/real_math_globals.h"

#include "effects/effect_lightprobe_slot_data.h"


#include "effects/effect_budget_limits.h"

#include "effects/effect_gpu_states.h"

#include "gpu_particle/contrail_queue_system.h"
#include "gpu_particle/spawn_shader_pair.h"
#include "gpu_particle/contrail_shader_globals.h"
#include "effects/contrail_profile.h"
#include "effects/contrail.h"
#include "effects/contrail_location.h"
#include "effects/contrail_system.h"
#include "effects/editable_property.h"
#include "effects/contrail_definition_types.h"
#include "tag_files/tag_groups.h"
#include "effects/contrail_states_types.h"
#include "cache/restricted_memory_layout.h"
#include "multithreading/thread_tls.h"
#include "cache/restricted_memory_regions_inline.h"
#include "effects/contrail_gpu.h"
#include "math/real_math.h"
#include "scenario/scenario_resource_registry.h"

void __tls_set_g_contrail_location_allocator(void *object)
{
    data_set_new_base_address((s_data_array **)&g_effect_lightprobe_slot_data, (s_data_array *)object);
}
static __forceinline void invalidate_data_array(s_data_array *array)
{
    array->valid = false;
}

// @address 0x7be9b8 g_contrail_spawn_shaders

static __forceinline void contrails_make_data_valid(s_data_array *data)
{
    data->valid = true;
    data_delete_all(data);
}
__forceinline c_contrail_gpu *c_contrail_gpu::get(long index)
{
        return datum_get<c_contrail_gpu>((s_data_array *)g_contrail_gpu_data, index);
    }
__forceinline void c_contrail_gpu::initialize_for_map()
{
        initialize_shaders();
        x_queued_buffer_system.reset_active();
        contrails_make_data_valid((s_data_array *)g_contrail_gpu_data);
        contrails_make_data_valid((s_data_array *)g_contrail_gpu_row_data);
    }
__forceinline void c_contrail_gpu::dispose_from_map()
{
        invalidate_data_array((s_data_array *)g_contrail_gpu_row_data);
        invalidate_data_array((s_data_array *)g_contrail_gpu_data);
        x_queued_buffer_system.reset_active();
        dispose_shaders();
    }

// @address 0x7bea08 g_contrail_update_shaders

__forceinline void c_contrail_gpu::dispose_shaders()
{
    g_contrail_spawn_shaders.dispose();
    g_contrail_update_shaders.dispose();
    g_contrail_render_vertex_shader.shader_reference = 0;
}


__forceinline void c_contrail_profile::initialize_for_map() { contrails_make_data_valid((s_data_array *)g_contrail_profile_data); }
__forceinline void c_contrail_profile::dispose_from_map() { invalidate_data_array((s_data_array *)g_contrail_profile_data); }
__forceinline void c_contrail::initialize_for_map() { contrails_make_data_valid(g_contrail_data_array); }
__forceinline void c_contrail::dispose_from_map() { invalidate_data_array(g_contrail_data_array); }
__forceinline void c_contrail_location::initialize_for_map() { contrails_make_data_valid(g_contrail_location_data); }
__forceinline void c_contrail_location::dispose_from_map() { invalidate_data_array(g_contrail_location_data); }

static __forceinline float contrail_real_random_range(unsigned long *seed, float minimum, float maximum)
{
    random_seed_next(seed);
    float fraction = (float)(*seed >> 16) * 1.5259022e-05f;
    return minimum + fraction * (maximum - minimum);
}
__forceinline float c_contrail::real_internal_random_range(float minimum, float maximum) const
{
    return contrail_real_random_range(&random_seed, minimum, maximum);
}


__forceinline long c_contrail_gpu::create(long owner)
{
    long index = datum_new((s_data_array *)g_contrail_gpu_data);
    c_contrail_gpu *gpu = &((c_contrail_gpu *)((s_data_array *)g_contrail_gpu_data)->data)[(unsigned short)index];
    gpu->contrail_index = owner;
    gpu->first_row_index = -1;
    gpu->profile_count = 0;
    gpu->flags = 0;
    return index;
}
static __forceinline const c_contrail_definition *contrail_get_definition(const c_contrail *contrail)
{
    const c_contrail_location *location = c_contrail_location::get(contrail->location_index);
    const char *system_data = g_contrail_system_data->data;
    const c_contrail_system *system = (const c_contrail_system *)(system_data + (unsigned short)location->system_index * sizeof(c_contrail_system));
    const s_contrail_system_definition *definition =
        (const s_contrail_system_definition *)((void *)tag_get(0, system->definition_index));
    return &definition->contrails[contrail->definition_index];
}

// @address 0x8c43b4 k_contrail_default_used_flags
extern "C" {
unsigned long k_contrail_default_used_flags;
}
// @address 0x14b590 ?frame_advance_all@c_contrail_system@@SAXM@Z
void c_contrail_system::frame_advance_all(float delta_time)
{
    if (!(delta_time <= 0.0f)) {
        c_data_iterator<c_contrail_system> iterator;
        iterator.begin(g_contrail_system_data);
        while (iterator.next()) {
            c_contrail_system *system = iterator.get_datum();
            system->query_effect(delta_time);
            if (system->get_should_pulse()) system->pulse(delta_time, false);
        }
    }
    c_data_iterator<c_contrail_system> iterator;
    iterator.begin(g_contrail_system_data);
    while (iterator.next()) {
        c_contrail_system *system = iterator.get_datum();
        if (system->get_should_frame_advance()) system->frame_advance(delta_time);
        if (!system->get_should_live() && !system->get_should_frame_advance()) contrail_system_destroy(iterator.get_index());
    }
}
// @address 0x14bdd8 contrail_evaluate_instance_properties
static __forceinline s_contrails_property_row *contrails_property_row_get(const s_particle_instance *instance)
{
    const s_effect_contrail_location *entry = datum_get<s_effect_contrail_location>(g_particle_instance_location_data, instance->location_index);
    const s_effect_contrail_profile *location = datum_get<s_effect_contrail_profile>(g_particle_instance_owner_data, entry->owner_index);
    const s_contrails_property_block *block =
        (const s_contrails_property_block *)((void *)tag_get(0, location->effect_index));
    return &block->rows[instance->definition_index];
}
static __forceinline void contrails_property_evaluate(s_contrails_property_row *row, long property_index, long *value, s_object_14 *states)
{
    if (*value == -1 || !row->properties[property_index].is_constant_over_time())
        *(float *)value = row->properties[property_index].evaluate((const c_light_volume_states *)states);
}
void contrail_evaluate_instance_properties(s_particle_instance *instance, float unused, s_object_14 *states)
{
    if (instance != states->source) {
        unsigned long valid = states->valid;
        states->source = instance;
        states->valid = valid & ~0x30;
        contrail_states_update_values(states, 48);
    }
    s_contrails_property_row *row = contrails_property_row_get(instance);
    contrails_property_evaluate(row, 0, (long *)&instance->length, states);
    contrails_property_evaluate(row, 1, (long *)&instance->offset, states);
    contrails_property_evaluate(row, 2, (long *)&instance->density, states);
    contrails_property_evaluate(row, 3, (long *)&instance->profile_length, states);
}
// @address 0x2b4548 contrails_initialize
extern "C" void contrails_initialize()
{
    g_contrail_system_data = data_new("contrail_system", 160, sizeof(c_contrail_system), 0, (c_allocation_base *)&g_contrail_system_allocator);
    g_contrail_location_data = data_new("contrail", 160, sizeof(c_contrail_location), 0,
        (c_allocation_base *)&g_contrail_location_allocator);
    g_contrail_data_array = data_new("contrail", 160, sizeof(c_contrail), 0, (c_allocation_base *)&g_contrail_allocator);
    g_contrail_profile_data = data_new("contrail_profile", 1, sizeof(c_contrail_profile), 0,
        (c_allocation_base *)&g_contrail_profile_allocator);
    contrail_gpu_initialize();
}



// @address 0x2b4620 ?initialize_for_map@c_contrail_system@@SAXXZ
void c_contrail_system::initialize_for_map()
{
    contrails_make_data_valid(g_contrail_system_data);
    c_contrail_location::initialize_for_map();
    c_contrail::initialize_for_map();
    c_contrail_profile::initialize_for_map();
    c_contrail_gpu::initialize_for_map();
}

// @address 0x2b46d8 ?dispose_from_map@c_contrail_system@@SAXXZ
void c_contrail_system::dispose_from_map()
{
    c_contrail_gpu::dispose_from_map();
    c_contrail_profile::dispose_from_map();
    c_contrail::dispose_from_map();
    c_contrail_location::dispose_from_map();
    invalidate_data_array(g_contrail_system_data);
}

static __forceinline void contrails_destroy_system(long index)
{
    if (index != -1) {
        c_contrail_system *system = c_contrail_system::get(index);
        long location_index = system->first_location;
        system->first_location = -1;
        if (location_index != -1) {
            c_contrail_location::destroy(location_index);
        }
        ::datum_delete(g_contrail_system_data, index);
    }
}

// @address 0x2b4770 contrail_systems_dispose_unused
extern "C" void contrail_systems_dispose_unused(const s_game_non_bsp_zone_set *old_zone_set, const s_game_non_bsp_zone_set *new_zone_set,
    c_scenario_resource_registry *pending_zone_registry)
{
    c_data_iterator<c_contrail_system> iterator;
    iterator.begin(g_contrail_system_data);
    while (iterator.next()) {
        c_contrail_system *system = iterator.get_datum();
        long definition_index = system->definition_index & 65535;
        if (!pending_zone_registry->is_tag_available(definition_index))
            contrails_destroy_system(iterator.get_index());
    }
}

// @address 0x2b4840 ?create@c_contrail_system@@SAJEJJEJJE@Z
long c_contrail_system::create(unsigned char type, long definition_index, long owner, unsigned char priority, long location, long unused, unsigned char extra)
{
    long result = -1;
    if (g_contrail_system_data->actual_count + 1 <=
        g_effect_budget_limits->table->blocks[_contrail_system_budget].limits[type].maximum_count) {
        long index = datum_new(g_contrail_system_data);
        c_contrail_system *system = get(index);
        system->definition_index = definition_index;
        system->type = type;
        system->owner = owner;
        system->priority = priority;
        system->location = (short)location;
        system->first_location = -1;
        system->age = 0.0f;
        system->pulse_time = 0.0f;
        system->distance = 0.0f;
        system->extra = extra;
        system->bsp_index = -1;
        system->lod = 0;
        system->set_flag(1, true);
        system->set_flag(2, true);
        system->set_flag(4, false);
        system->set_flag(8, false);
        system->set_flag(16, false);
        g_contrail_system_creation_enabled[type] = true;
        result = index;
    } else {
        g_contrail_system_creation_enabled[type] = false;
    }
    return result;
}

// @address 0x2b4948 contrail_system_destroy
extern "C" void contrail_system_destroy(long index)
{
    contrails_destroy_system(index);
}

static __forceinline void contrails_location_add_sibling(c_contrail_location *location, long index)
{
    if (location->sibling == -1) location->sibling = index;
    else {
        c_contrail_location *next = c_contrail_location::get(location->sibling);
        next->add_sibling(index);
    }
}
// @address 0x2b49b0 ?add_contrail_location@c_contrail_system@@QAAJPBUreal_matrix4x3@@K@Z
long c_contrail_system::add_contrail_location(const real_matrix4x3 *matrix, unsigned long seed)
{
    long system_index = (this - (c_contrail_system *)g_contrail_system_data->data) | (identifier << 16);
    long index = c_contrail_location::create(type, system_index, matrix, seed);
    if (index != -1) {
        if (first_location == -1) first_location = index;
        else {
            c_contrail_location *location = c_contrail_location::get(first_location);
            contrails_location_add_sibling(location, index);
        }
    }
    return index;
}

// @address 0x2b4b50 ?pulse@c_contrail_system@@QAAXM_N@Z
// @address 0x2b4a98 ?query_effect@c_contrail_system@@QAAXM@Z
void c_contrail_system::query_effect(float delta_time)
{
    if (!(delta_time <= 0.0f))
        effect_update_contrail_system((s_effect_update_location *)this, delta_time);
    unsigned long status = effect_get_event_status(owner, priority, location, &age);
    if (status >= 1) {
        if (status != 1)
            flags &= ~3;
        else
            set_flag(2, age == 0.0f);
    } else {
        flags |= 2;
    }
}
void c_contrail_system::pulse(float delta_time, bool force)
{
    c_contrail_states states;
    states.set_contrail_system(this);
    long index = first_location;
    while (index != -1) {
        c_contrail_location *location = c_contrail_location::get(index);
        contrail_location_pulse(location, delta_time, force || !get_should_pulse(), &states);
        index = location->sibling;
    }
    set_flag(4, true);
}
// @address 0x2b4d28 ?create@c_contrail_location@@SAJEJPBUreal_matrix4x3@@K@Z
long c_contrail_location::create(unsigned char type, long parent, const real_matrix4x3 *matrix, unsigned long seed)
{
    long result = -1;
    if (g_contrail_location_data->actual_count + 1 <=
        g_effect_budget_limits->table->blocks[_contrail_location_budget].limits[type].maximum_count) {
        result = datum_new(g_contrail_location_data);
        c_contrail_location *location = get(result);
        location->sibling = -1;
        location->system_index = parent;
        location->contrail_index = -1;
        location->matrix = *matrix;
        location->last_position = matrix->position;
        location->velocity = *global_zero_vector3d;
        const c_contrail_system *system = c_contrail_system::get(parent);
        const s_contrail_system_definition *definition =
            (const s_contrail_system_definition *)tag_get(0, system->definition_index);
        c_contrail *previous = 0;
        for (long i = 0; i < definition->count; ++i) {
            long index = c_contrail::create(type, result, i, seed);
            if (index != -1) {
                if (previous) previous->sibling = index;
                else location->contrail_index = index;
                previous = c_contrail::get(index);
            }
        }
        g_contrail_location_creation_enabled[type] = true;
    } else {
        g_contrail_location_creation_enabled[type] = false;
    }
    return result;
}

// @address 0x2b4ee8 ?destroy@c_contrail_location@@SAXJ@Z
void c_contrail_location::destroy(long index)
{
    c_contrail_location *location = get(index);
    if (location->contrail_index != -1) {
        c_contrail::destroy(location->contrail_index);
    }
    if (location->sibling != -1) {
        destroy(location->sibling);
    }
    ::datum_delete(g_contrail_location_data, index);
}

// @address 0x2b4f50 ?add_sibling@c_contrail_location@@QAAXJ@Z
void c_contrail_location::add_sibling(long index)
{
    if (sibling == -1) {
        sibling = index;
    } else {
        c_contrail_location *next;
        next = get(sibling);
        next->add_sibling(index);
    }
}

// @address 0x2b4fa8 contrail_location_update_matrix
extern "C" void contrail_location_update_matrix(c_contrail_location *location, float delta_time, const real_matrix4x3 *matrix)
{
    real_point3d old_position = location->matrix.position;
    location->matrix = *matrix;
    real_vector3d velocity;
    if (delta_time == 0.0f) {
        velocity = *global_zero_vector3d;
    } else {
        float inverse_delta_time = 1.0f / delta_time;
        velocity.i = (old_position.x - location->matrix.position.x) * inverse_delta_time;
        velocity.j = (old_position.y - location->matrix.position.y) * inverse_delta_time;
        velocity.k = (old_position.z - location->matrix.position.z) * inverse_delta_time;
    }
    location->velocity = velocity;
}

// @address 0x2b5090 contrail_location_pulse
extern "C" void contrail_location_pulse(c_contrail_location *location, float delta_time, bool force, c_contrail_states *states)
{
    if (location != states->location) {
        states->location = location;
        states->flags &= ~0x800u;
        states->update_states(0x800);
    }
    long index = location->contrail_index;
    while (index != -1) {
        c_contrail *contrail = c_contrail::get(index);
        contrail->pulse(delta_time, force, states);
        index = contrail->sibling;
    }
}


__forceinline void c_contrail::frame_advance(float delta_time)
{
    c_contrail_gpu *gpu = c_contrail_gpu::get(gpu_index);
    set_should_frame_advance(gpu->get_alive());
}

// @address 0x2b5130 ?frame_advance@c_contrail_location@@QAAXM@Z
void c_contrail_location::frame_advance(float delta_time)
{
    bool should_frame_advance = false;
    long index = contrail_index;
    while (index != -1) {
        c_contrail *contrail = c_contrail::get(index);
        contrail->frame_advance(delta_time);
        should_frame_advance = should_frame_advance || contrail->get_should_frame_advance();
        index = contrail->get_next_sibling_index();
    }
    set_should_frame_advance(should_frame_advance);
}

// @address 0x2b52f0 contrail_set_base_address
extern "C" void contrail_set_base_address(void *object)
{
    data_set_new_base_address(&g_contrail_data_array, (s_data_array *)object);
}

// @address 0x2b5308 ?create@c_contrail@@SAJEJJK@Z
long c_contrail::create(unsigned char type, long parent, long definition_index, unsigned long seed)
{
    long index = -1;
    if (g_contrail_data_array->actual_count + 1 <= g_effect_budget_limits->table->blocks[_contrail_budget].limits[type].maximum_count) {
        index = datum_new(g_contrail_data_array);
        c_contrail *contrail = get(index);
        contrail->location_index = parent;
        contrail->definition_index = definition_index;
        contrail->sibling = -1;
        contrail->first_profile = -1;
        contrail->gpu_index = c_contrail_gpu::create(index);
        const c_contrail_definition *definition = contrail_get_definition(contrail);
        contrail->flags = 0;
        contrail->set_should_frame_advance(false);
        contrail->random_seed = seed;
        for (long random_index = 0; random_index < 2; ++random_index)
            contrail->random[random_index] = contrail->real_internal_random_range(0.0f, 1.0f);
        contrail->age = 0.0f;
        contrail->distance = 0.0f;
        contrail->position = *global_origin3d;
        contrail->initial0 = (bool)((definition->flags >> 3) & 1) ? contrail->random[0] : 0.0f;
        contrail->initial1 = (bool)((definition->flags >> 4) & 1) ? contrail->random[1] : 0.0f;
        contrail->property0 = -1;
        contrail->property1 = -1;
        g_contrail_creation_enabled[type] = true;
    } else {
        g_contrail_creation_enabled[type] = false;
    }
    return index;
}

// @address 0x2b5558 ?destroy@c_contrail@@SAXJ@Z
void c_contrail::destroy(long index)
{
    c_contrail *contrail = get(index);
    long profile_index = contrail->first_profile;
    while (profile_index != -1) {
        c_contrail_profile *profile = c_contrail_profile::get(profile_index);
        long next_profile_index = profile->get_next_sibling_index();
        c_contrail_profile::destroy(profile_index);
        profile_index = next_profile_index;
    }
    long gpu_index = contrail->gpu_index;
    if (gpu_index != -1) {
        c_contrail_gpu *gpu = c_contrail_gpu::get(gpu_index);
        particle_block_delete_contrail_block(gpu->first_row_index, true);
        gpu->profile_count = 0;
        gpu->first_row_index = -1;
        gpu->flags = 0;
        ::datum_delete((s_data_array *)g_contrail_gpu_data, gpu_index);
    }
    long sibling_index = contrail->sibling;
    if (sibling_index != -1) {
        destroy(sibling_index);
    }
    ::datum_delete(g_contrail_data_array, index);
}

static __forceinline c_contrail_gpu *contrails_gpu_get(long index)
{
    return datum_get<c_contrail_gpu>((s_data_array *)g_contrail_gpu_data, index);
}
// @address 0x2b5830 ?create_profile@c_contrail@@AAAXKPAVc_contrail_states@@_NPBTreal_point3d@@M@Z
void c_contrail::create_profile(unsigned long queue_context, c_contrail_states *states, bool temporary, const real_point3d *direction, float profile_distance)
{
    long profile_index = first_profile;
    if (profile_index == -1 || !c_contrail_profile::get(profile_index)->get_temporary())
        profile_index = c_contrail_profile::create(this);
    if (profile_index != -1) {
        c_contrail_profile *profile = c_contrail_profile::get(profile_index);
        if (profile_index != first_profile) {
            profile->sibling = first_profile;
            first_profile = profile_index;
        }
        initialize_profile(profile, states, direction, profile_distance);
        profile->set_temporary(temporary);
        if (!temporary) {
            flags |= 2;
            position = profile->position;
            distance = profile->cumulative_length;
        }
        c_contrail_gpu *gpu = contrails_gpu_get(gpu_index);
        gpu->queue_profile(queue_context, (c_contrail_profile const *)profile, temporary);
        c_contrail_profile::destroy(profile_index);
        first_profile = -1;
    }
}

// @address 0x2b5d00 ?render_callback@c_contrail@@CAXPBXJ@Z
void c_contrail::render_callback(const void *data, long user_data)
{
    ((const c_contrail *)data)->render();
}

static __forceinline const c_contrail_location *contrails_profile_location_get(long index)
{
    return datum_get<c_contrail_location>(g_contrail_location_data, index);
}
// @address 0x2b5d08 ?render@c_contrail@@QBAXXZ
void c_contrail::render() const
{
    c_contrail_states states;
    const c_contrail_location *location = contrails_profile_location_get(location_index);
    const char *system_data = g_contrail_system_data->data;
    states.set_contrail_system((const c_contrail_system *)(system_data + (unsigned short)location->system_index * sizeof(c_contrail_system)));
    if (c_contrail_location::get(location_index) != states.location) {
        states.location = c_contrail_location::get(location_index);
        states.flags &= ~0x800u;
        states.update_states(0x800);
    }
    if (this != states.contrail) {
        states.contrail = this;
        states.flags &= ~0x600u;
        states.update_states(0x600);
    }
    c_contrail_gpu::get(gpu_index)->render(&states);
}

// @address 0x2b5e68 ?create@c_contrail_profile@@SAJPBVc_contrail@@@Z
long c_contrail_profile::create(const c_contrail *contrail)
{
    long index = datum_new((s_data_array *)g_contrail_profile_data);
    if (index != -1) {
        c_contrail_profile *profile = get(index);
        profile->sibling = -1;
        profile->flags = 0;
        profile->age = 0.0f;
        for (long random_index = 0; random_index < 4; ++random_index) {
            profile->random[random_index] = (unsigned short)(long)(contrail->real_internal_random_range(0.0f, 1.0f) * 65535.0f);
        }
        g_contrail_profile_creation_enabled = true;
    } else {
        g_contrail_profile_creation_enabled = false;
    }
    return index;
}


// @address 0x2b6018 ?evaluate@?$c_editable_property@Vc_contrail_states@@@@QBAMPBVc_contrail_states@@@Z
float c_editable_property<c_contrail_states>::evaluate(const c_contrail_states *states) const
{
    if (m_flags & 0x20)
        return m_constant;
    float value = m_function.evaluate_scalar(states->values[m_input], states->values[m_range]);
    int modifier_input = m_modifier_input;
    if (modifier_input != -1)
    {
        switch (m_modifier)
        {
        case 1: value = states->values[modifier_input] + value; break;
        case 2: value = states->values[modifier_input] * value; break;
        }
    }
    return value;
}

// @address 0x2b60d0 ?allocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_contrail_system_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_contrail_system_allocator>::allocate(unsigned int size, const char *name)
{
    long member_index = g_restricted_regions[2].add_member(name, "", size, 0, __tls_set_g_object_list_header_data_allocator, 0, 0);
    data_set_new_base_address(&g_contrail_system_data, (s_data_array *)restricted_region_get_member_address(2, member_index));
    m_member_index = member_index;
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(2, m_member_index);
}

// @address 0x2b6170 ?deallocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_contrail_system_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_contrail_system_allocator>::deallocate(void *address)
{
    g_contrail_system_data = 0;
    restricted_region_free_member(2, m_member_index);
}

// @address 0x2b6190 ?allocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_contrail_location_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_contrail_location_allocator>::allocate(unsigned int size, const char *name)
{
    long member_index = g_restricted_regions[2].add_member(name, "", size, 0, __tls_set_g_decal_array_allocator, 0, 0);
    data_set_new_base_address(&g_contrail_location_data, (s_data_array *)restricted_region_get_member_address(2, member_index));
    m_member_index = member_index;
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(2, m_member_index);
}

// @address 0x2b6230 ?deallocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_contrail_location_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_contrail_location_allocator>::deallocate(void *address)
{
    g_contrail_location_data = 0;
    restricted_region_free_member(2, m_member_index);
}

// @address 0x2b6250 ?allocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_contrail_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_contrail_allocator>::allocate(unsigned int size, const char *name)
{
    long member_index = g_restricted_regions[2].add_member(name, "", size, 0, contrail_set_base_address, 0, 0);
    data_set_new_base_address(&g_contrail_data_array, (s_data_array *)restricted_region_get_member_address(2, member_index));
    m_member_index = member_index;
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(2, m_member_index);
}

// @address 0x2b62f0 ?deallocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_contrail_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_contrail_allocator>::deallocate(void *address)
{
    g_contrail_data_array = 0;
    restricted_region_free_member(2, m_member_index);
}

// @address 0x2b6310 ?allocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_contrail_profile_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_contrail_profile_allocator>::allocate(unsigned int size, const char *name)
{
    long member_index = g_restricted_regions[2].add_member(name, "", size, 0, __tls_set_g_hs_thread_non_deterministic_data_allocator, 0, 0);
    data_set_new_base_address(&g_contrail_profile_data, (s_data_array *)restricted_region_get_member_address(2, member_index));
    m_member_index = member_index;
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(2, m_member_index);
}

// @address 0x2b63b0 ?deallocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_contrail_profile_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_contrail_profile_allocator>::deallocate(void *address)
{
    g_contrail_profile_data = 0;
    restricted_region_free_member(2, m_member_index);
}

void __tls_set_g_contrail_profile_allocator(void *object)
{
    data_set_new_base_address(&g_light_volume_location_data, (s_data_array *)object);
}

// @address 0x45f268 ?particle_emitter_gpu_try_and_get@@YAPAXH@Z
void *particle_emitter_gpu_try_and_get(int handle)
{
    s_data_array *data = (s_data_array *)g_particle_emitter_gpu_data;
    void *result = 0;
    if (handle != -1) {
        int absolute_index = handle & 0xffff;
        if (absolute_index < data->first_unallocated) {
            short *datum = (short *)(data->data + data->size * absolute_index);
            int identifier = *datum;
            if (identifier != 0 && identifier == (handle >> 16))
                result = datum;
        }
    }
    return result;
}

void __tls_set_g_contrail_allocator(void *object)
{
    data_set_new_base_address((s_data_array **)&g_particle_emitter_gpu_data, (s_data_array *)object);
}

void __tls_set_g_contrail_system_allocator(void *object)
{
    data_set_new_base_address(&tracking_data, (s_data_array *)object);
}

// @address 0x2b4c38 ?frame_advance@c_contrail_system@@QAAXM@Z
void c_contrail_system::frame_advance(float delta_time)
{
    bool should_frame_advance = false;
    for (long location_index = first_location; location_index != -1;) {
        c_contrail_location *location = c_contrail_location::get(location_index);
        location->frame_advance(delta_time);
        should_frame_advance = should_frame_advance || (location->flags & 1) != 0;
        location_index = location->sibling;
    }
    set_flag(4, should_frame_advance);
    set_flag(8, should_frame_advance);
    set_flag(16, true);
}


// @address 0x45fd80 ?allocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_particle_emitter_gpu_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_particle_emitter_gpu_allocator>::allocate(unsigned int size, const char *name)
{
    long member_index = restricted_memory_register_member(&g_restricted_regions[2], name, "", size, 0,
        __tls_set_g_contrail_allocator, 0, 0);
    data_set_new_base_address((s_data_array **)&g_particle_emitter_gpu_data,
        (s_data_array *)restricted_region_get_member_address(2, member_index));
    m_member_index = member_index;
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(2, member_index);
}
// @address 0x45fe20 ?deallocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_particle_emitter_gpu_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_particle_emitter_gpu_allocator>::deallocate(void *address)
{
    g_particle_emitter_gpu_data = 0;
    restricted_region_free_member(2, m_member_index);
}
// @address 0x7be978 g_contrail_gpu_vertex_buffer
// @address 0x7be960 g_contrail_gpu_row_allocator
// @address 0x7be96c g_contrail_gpu_emitter_allocator

// @address 0x461210 contrail_gpu_initialize
extern "C" void contrail_gpu_initialize(void)
{
    g_contrail_gpu_data = (s_data_array *)data_new((char *)"c_contrail_gpu", 160, 16, 0, (c_allocation_base *)&g_contrail_gpu_allocator);
    g_contrail_gpu_data = (s_data_array *)data_new((char *)"c_contrail_gpu::s_row", 160, 20, 0, (c_allocation_base *)&g_contrail_gpu_row_allocator);
    int size = rasterizer_vertex_get_stride_from_declaration(
        &g_rasterizer_vertex_buffer_declarations[g_contrail_gpu_vertex_buffer.format]);
    int count = g_contrail_gpu_vertex_buffer.element_count;
    size *= g_contrail_gpu_vertex_buffer.element_size;
    size *= count;
    g_contrail_gpu_buffer_address = g_contrail_gpu_emitter_allocator.reserve_memory("c_particle_emitter_gpu",
        "x_gamestate_storage", (size + 127) & 0xffffff80, 7);
    contrail_gpu_refresh_vertex_buffers(&c_contrail_gpu::x_queued_buffer_system);
    return;
}
// @address 0x14bb18 contrail_update_system_instances
// @address 0x8c4428 k_contrail_default_dirty_mask
extern "C" {
unsigned long k_contrail_default_dirty_mask;
}

extern "C" void contrail_update_system_instances(s_effect_contrail_profile *profile, float delta_time)
{
    s_object_14 states;
    states.pad30 = visibility_projections_and_volumes_zero;
    states.definition = 0;
    states.location = 0;
    states.source = 0;
    states.valid = 512;
    states.dirty = k_contrail_default_dirty_mask;
    contrail_states_set_definition((char *)&states, (char *)profile);
    long location_index = profile->location_index;
    if (location_index != -1) {
        do {
            s_effect_contrail_location *location = datum_get<s_effect_contrail_location>(g_particle_instance_location_data, location_index);
            if (location != states.location) {
                states.location = location;
                contrail_states_update_values((char *)&states, 0);
            }
            location_index = location->first_instance_index;
            if (location_index != -1) {
                do {
                    s_particle_instance *instance = datum_get<s_particle_instance>((s_data_array *)g_particle_instance_data, location_index);
                    contrail_evaluate_instance_properties(instance, delta_time, &states);
                    location_index = instance->next;
                } while (location_index != -1);
            }
            location_index = location->next_index;
        } while (location_index != -1);
    }
    profile->flags = (unsigned short)(profile->flags | 8);
    return;
}


static __forceinline void contrails_profile_delete(long index)
{
    s_effect_contrail_profile *profile = datum_get<s_effect_contrail_profile>(g_particle_instance_owner_data, index);
    long location_index = profile->location_index;
    profile->location_index = -1;
    if (location_index != -1)
        particle_delete_location_chain(location_index);
    ::datum_delete(g_particle_instance_owner_data, index);
}
// @address 0x14b6e8 contrail_owners_frame_advance_all
extern "C" void contrail_owners_frame_advance_all(float delta_time)
{
    c_data_iterator<s_effect_contrail_profile> iterator;
    if (!(delta_time <= visibility_projections_and_volumes_zero)) {
        iterator.begin(g_particle_instance_owner_data);
        while (iterator.next())
            effect_update_contrail_profile(iterator.get_datum(), delta_time);
    }
    iterator.begin(g_particle_instance_owner_data);
    while (iterator.next()) {
        if (!(iterator.get_datum()->flags & 1)) {
            if (iterator.get_index() != -1)
                contrails_profile_delete(iterator.get_index());
        } else {
            unsigned char active = (iterator.get_datum()->flags >> 1) & 1;
            if (active)
                contrail_update_system_instances(iterator.get_datum(), delta_time);
        }
    }
}

static __forceinline const c_contrail_definition *contrails_profile_definition_get(const c_contrail *contrail)
{
    const c_contrail_location *location = contrails_profile_location_get(contrail->location_index);
    const char *system_data = g_contrail_system_data->data;
    const c_contrail_system *system = (const c_contrail_system *)(system_data + (unsigned short)location->system_index * sizeof(c_contrail_system));
    const s_contrail_system_definition *definition =
        (const s_contrail_system_definition *)((void *)tag_get(0, system->definition_index));
    return &definition->contrails[contrail->definition_index];
}
// @rdata 0x8202490c
static const float k_contrail_profile_epsilon = 0.0001f;

// @address 0x2b5990 contrail_create_profiles
extern "C" void contrail_create_profiles(c_contrail *contrail, unsigned long queue_context, c_contrail_states *states, bool temporary,
    const real_point3d *direction, float delta_distance)
{
    float end = contrail->distance + delta_distance;
    float current = contrail->distance;
    const c_contrail_definition *definition = contrails_profile_definition_get(contrail);
    if ((bool)((contrail->flags >> 1) & 1) && (bool)((definition->flags >> 5) & 1)) {
        float first = definition->first_profile_distance;
        if (first > current + k_contrail_profile_epsilon && first < end - k_contrail_profile_epsilon) {
            contrail->create_profile(queue_context, states, false, direction, first - current);
            current = first;
        }
        float second = definition->first_profile_distance + definition->profile_distance;
        if (second > current + k_contrail_profile_epsilon && second < end - k_contrail_profile_epsilon) {
            contrail->create_profile(queue_context, states, false, direction, second - current);
            current = second;
        }
    }
    contrail->create_profile(queue_context, states, temporary, direction, end - current);
}

// match: the canonical noalias declaration changes the accepted pulse schedule.

// @rdata 0x820221fc
static const float k_contrails_pulse_one = 1.0f;
// @address 0x2b5b08 ?pulse@c_contrail@@QAAXM_NPAVc_contrail_states@@@Z
void c_contrail::pulse(float delta_time, bool force, c_contrail_states *states)
{
    const c_contrail_location *location = contrails_profile_location_get(location_index);
    const char *system_data = g_contrail_system_data->data;
    long parent_system_index = location->system_index;
    long local_definition_index = definition_index;
    // direction is normalized as a vector, then passed on as the profile start point
    union {
        real_vector3d vector;
        real_point3d point;
    } direction;
    direction.vector.i = location->matrix.position.x-position.x;
    float location_z = location->matrix.position.z;
    float location_y = location->matrix.position.y;
    direction.vector.j = location_y-position.y;
    direction.vector.k = location_z-position.z;
    const c_contrail_system *system = (const c_contrail_system *)(system_data + (unsigned short)parent_system_index * sizeof(c_contrail_system));
    const s_contrail_system_definition *system_definition =
        (const s_contrail_system_definition *)((void *)tag_get(0, system->definition_index));
    const c_contrail_definition *definition = &system_definition->contrails[local_definition_index];
    float length = normalize3d(&direction.vector);
    if ((bool)((flags >> 1) & 1) && length <= k_contrail_profile_epsilon)
        return;
    float remaining_distance = length;
    unsigned long type = c_contrail_system::get(contrails_profile_location_get(location_index)->system_index)->type;
    if (this != states->contrail) {
        states->contrail = this;
        states->flags &= ~0x600u;
        states->update_states(0x600);
    }
    emission_rate = definition->emission_rate.evaluate(states);
    age += emission_rate * delta_time;
    float one = 1.0f;
    if (!(age < one)) {
        float profile_distance = remaining_distance / age;
        do {
            contrail_create_profiles(this, type, states, false, &direction.point, profile_distance);
            age -= one;
            remaining_distance -= profile_distance;
        } while (age >= one);
    }
    if ((bool)((flags >> 1) & 1) && remaining_distance <= k_contrail_profile_epsilon)
        return;
    contrail_create_profiles(this, type, states, !force, &direction.point, remaining_distance);
}

__forceinline void c_contrail::submit() const
{
    const c_contrail_system *system = c_contrail_system::get(c_contrail_location::get(location_index)->system_index);
    const s_contrail_system_definition *definition =
        (const s_contrail_system_definition *)((void *)tag_get(0, system->definition_index));
    const c_contrail_definition *contrail_definition = &definition->contrails[definition_index];
    render_objects_add_transparent_element(&c_contrail_location::get(location_index)->matrix.position, 0, 0.0f,
        (e_transparent_sort_layer)contrail_definition->sort_layer, render_callback, this, -1, 0.0f);
}

static __forceinline const c_contrail *contrails_contrail_get(long index)
{
    return datum_get<c_contrail>(g_contrail_data_array, index);
}

// @address 0x2b5200 ?submit@c_contrail_location@@QAAXXZ
void c_contrail_location::submit()
{
    long index = contrail_index;
    while (index != -1)
    {
        const c_contrail *contrail = contrails_contrail_get(index);
        contrail->submit();
        index = contrail->get_next_sibling_index();
    }
}

// @address 0x744474 k_contrail_profile_default_tint
extern "C" {
const real_argb_color k_contrail_profile_default_tint = { 0.0f, 1.0f, 1.0f, 1.0f };
}
// @address 0x2b5630 ?initialize_profile@c_contrail@@AAAXPAVc_contrail_profile@@PAVc_contrail_states@@PBTreal_point3d@@M@Z
void c_contrail::initialize_profile(c_contrail_profile *profile, c_contrail_states *states, const real_point3d *direction, float distance)
{
    unsigned long state_flags = states->flags;

    const c_contrail_location *location = datum_get<c_contrail_location>(g_contrail_location_data, location_index);
    const c_contrail_system *system = datum_get<c_contrail_system>(g_contrail_system_data, location->system_index);
    const s_contrail_system_definition *system_definition =
        (const s_contrail_system_definition *)((void *)tag_get(0, system->definition_index));
    const c_contrail_definition *definition = &system_definition->contrails[definition_index];

    states->profile = profile;
    states->flags = state_flags & 0xfffffffeu & 0xffffffc3u;
    states->update_states(0x3d);

    float lifespan = definition->lifespan.evaluate(states);
    profile_lifespan = lifespan;
    profile->lifespan = lifespan;

    real_point3d offset;
    offset.x = direction->x * distance + position.x;
    offset.y = direction->y * distance + position.y;
    offset.z = direction->z * distance + position.z;
    profile->position = offset;

    if ((bool)((flags >> 1) & 1))
        profile->cumulative_length = this->distance + distance;

    real_argb_color sampled_tint;
    const real_argb_color *tint;
    if ((bool)((definition->flags >> 0) & 1)) {
        long lightprobe_index = system->lightprobe_slot_index;
        sampled_tint = effect_get_lightmap_tint(lightprobe_index);
        tint = &sampled_tint;
    } else {
        tint = &k_contrail_profile_default_tint;
    }
    profile->color = *tint;

    if ((bool)((definition->flags >> 2) & 1))
        profile->scale = system->pulse_time;
    else
        profile->scale = 1.0f;

    profile->velocity = *global_zero_vector3d;
}

t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_contrail_system_allocator> g_contrail_system_allocator;
t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_contrail_location_allocator> g_contrail_location_allocator;
t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0), &__tls_set_g_contrail_allocator> g_contrail_allocator;
t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_contrail_profile_allocator> g_contrail_profile_allocator;
t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_particle_emitter_gpu_allocator> g_particle_emitter_gpu_allocator;


// @address 0x119a438 g_contrail_system_creation_enabled
bool g_contrail_system_creation_enabled[3];

// @address 0x119a43c g_contrail_location_creation_enabled
bool g_contrail_location_creation_enabled[3];

// @address 0x119a440 g_contrail_creation_enabled
bool g_contrail_creation_enabled[3];

// @address 0x119a443 g_contrail_profile_creation_enabled
bool g_contrail_profile_creation_enabled;

// @address 0x11df788 g_contrail_system_data
__declspec(thread) s_data_array *g_contrail_system_data;
// @address 0x11df7f0 g_contrail_location_data
__declspec(thread) s_data_array *g_contrail_location_data;
// @address 0x11df810 g_contrail_data_array
__declspec(thread) s_data_array *g_contrail_data_array;

// @address 0x11df7d0 g_particle_emitter_gpu_data
__declspec(thread) s_data_array *g_particle_emitter_gpu_data;

// @address 0x11df854 g_contrail_profile_data
__declspec(thread) s_data_array *g_contrail_profile_data;
