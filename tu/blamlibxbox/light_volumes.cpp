// blamlibxbox/light_volumes.obj: from the records 0x14bf30 0x156d30 0x2b6938 0x2b69d8 0x2b69f8 0x2b6a98 0x2b6ab8 0x2b6ad0 0x2b6b58 0x2b6c58 0x2b6d40 0x2b6dc0 0x2b6ec8 0x2b7010 0x2b71b8 0x2b7220 0x2b7278 0x2b7290 0x2b7420 0x2b7480 0x2b7600 0x2b7608 0x2b7730 0x2b7830 0x2b78e8 0x2b7988 0x2b79a8 0x2b7a48 0x2b7a68 0x2b7b08
#include "effects/effect_runtime_storage.h"
#include "visibility/visibility_projections_and_volumes.h"
#include "tag_files/tag_groups.h"
#include "memory/data_types.h"
#include "math/matrix_math.h"
#include <ppcintrinsics.h>
#include <vectorintrinsics.h>
#include "cache/restricted_memory_regions.h"
#include "effects/contrails.h"
#include "effects/effects.h"
#include "memory/data.h"
#include "effects/light_volumes.h"
#include "effects/particle.h"
#include "effects/transparent_sort_layer_types.h"
#include "effects/event_status_types.h"
#include "visibility/visibility_test.h"
#include "math/function_definitions.h"
#include "cache/restricted_memory.h"
#include "math/matrix_math_types.h"
#include "tag_files/tag_types.h"
#include "effects/light_volumes_globals.h"
#include "cache/restricted_memory_registration.h"
#include "effects/effect_budget_limits.h"
#include "effects/light_volume_types.h"
#include "effects/light_volume_gpu.h"
#include "cache/restricted_memory_layout.h"
#include "effects/light_volume_allocators.h"
#include "cache/restricted_memory_regions_inline.h"
#include "multithreading/thread_tls.h"
#include "effects/light_volume_allocators_inline.h"
#include "effects/light_volume_location.h"
#include "math/random_math_range.h"
#include "effects/light_volume_states.h"
#include "effects/editable_property.h"
#include "render_methods/render_method.h"
#include "render/render_objects_static_lighting.h"
#include "visibility/visibility_region_types.h"
#include "visibility/visibility_collection.h"
#include "visibility/visibility_globals.h"
#include "effects/light_volume_allocator_globals.h"
// @address 0x11df794 g_light_volume_system_data
__declspec(thread) s_data_array *g_light_volume_system_data;
// @address 0x11df818 g_light_volume_data
__declspec(thread) s_data_array *g_light_volume_data;
// @address 0x11df7e8 g_light_volume_gpu_data
// @address 0x11df838 g_light_volume_gpu_row_data
// @address 0x11df7f4 g_particle_instance_location_data
// @address 0x11df814 g_particle_instance_data
// @flags /TP /O2
// @address 0x2b7278 ?__tls_set_g_light_volume_allocator@@YAXPAX@Z
// @address 0x2b7420 ?destroy@c_light_volume@@SAXJ@Z

class c_light_volume_system;
class c_light_volume_states;

// @address 0x119a454 g_light_volume_system_allocation_succeeded
// @address 0x119a458 g_light_volume_location_allocation_succeeded
// @address 0x119a45c g_light_volume_allocation_succeeded
extern "C" {
char g_light_volume_system_allocation_succeeded[4];
bool g_light_volume_location_allocation_succeeded[4];
bool g_light_volume_allocation_succeeded[4];
}

// @address 0x8c43a8 g_effect_budget_limits
// @address 0x11df7f8 g_light_volume_location_data


// @address 0x2b6938 ?allocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_light_volume_location_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<2,
    static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_light_volume_location_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// @address 0x2b69d8 ?deallocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_light_volume_location_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<2,
    static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_light_volume_location_allocator>::deallocate(void *memory)
{
    g_particle_instance_location_data = 0;
    restricted_region_free_member(2, m_member_index);
}

// @address 0x2b69f8 ?allocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_light_volume_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<2,
    static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_light_volume_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// @address 0x2b6a98 ?deallocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_light_volume_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<2,
    static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_light_volume_allocator>::deallocate(void *memory)
{
    g_particle_instance_data = 0;
    restricted_region_free_member(2, m_member_index);
}

// @address 0x2b6ab8 ?__tls_set_g_light_volume_system_allocator@@YAXPAX@Z
void __tls_set_g_light_volume_system_allocator(void *object)
{
    data_set_new_base_address((s_data_array **)&g_light_volume_system_data, (s_data_array *)object);
}

// @address 0x2b6ad0 ?initialize_for_map@c_light_volume_system@@SAXXZ
void c_light_volume_system::initialize_for_map()
{
    data_make_valid((s_data_array *)g_light_volume_system_data);
    data_make_valid((s_data_array *)g_light_volume_location_data);
    data_make_valid((s_data_array *)g_light_volume_data);
    c_light_volume_gpu::initialize_shaders();
    data_make_valid((s_data_array *)g_light_volume_gpu_data);
    data_make_valid((s_data_array *)g_light_volume_gpu_row_data);
}

// @address 0x2b6b58 light_volume_system_new
extern "C" long light_volume_system_new(unsigned char type, long effect_index, long handle, unsigned char pad8, long pad_a, long unused, unsigned char pad_1a)
{
    long index = -1;
    if (((s_data_array *)g_light_volume_system_data)->actual_count + 1 <= g_effect_budget_limits->table->blocks[5].limits[type].maximum_count)
    {
        index = ::datum_new((s_data_array *)g_light_volume_system_data);
        c_light_volume_system *system = c_light_volume_system::get(index);
        system->m_age = 0.0f;
        system->m_effect_scale_a = 0.0f;
        system->m_definition_index = effect_index;
        system->m_effect_scale_b = 0.0f;
        system->m_priority = type;
        system->m_effect_index = handle;
        system->m_event_block_index = pad8;
        system->m_part_block_index = pad_a;
        system->m_first_location_index = -1;
        system->first_person_type = pad_1a;
        system->m_first_person_flags = -1;
        system->m_lod = 0;
        system->set_should_live(true);
        system->set_should_frame_advance(true);
        system->set_should_render(true);
        system->set_ever_frame_advanced(false);
        g_light_volume_system_allocation_succeeded[type] = 1;
    }
    else
    {
        g_light_volume_system_allocation_succeeded[type] = 0;
    }
    return index;
}

// @address 0x2b6c58 ?add_light_volume_location@c_light_volume_system@@QAAJPBUreal_matrix4x3@@K@Z
long c_light_volume_system::add_light_volume_location(struct real_matrix4x3 const * matrix, unsigned long value)
{
    s_data_array *systems = (s_data_array *)g_light_volume_system_data;
    long handle = (long)(this - (c_light_volume_system *)systems->data) |
        ((unsigned long)this->m_identifier << 16);
    long index = light_volume_location_new(this->m_priority, handle, matrix, value);
    if (index != -1)
    {
        if (this->m_first_location_index == -1)
            this->m_first_location_index = index;
        else
        {
            c_light_volume_location *location = c_light_volume_location::get(this->m_first_location_index);
            if (location->m_sibling_index == -1)
                location->m_sibling_index = index;
            else
                c_light_volume_location::get(location->m_sibling_index)->add_sibling(index);
        }
    }
    return index;
}


class c_light_volume_states;

__forceinline const c_light_volume_location *c_light_volume::get_parent_location() const
{
    return c_light_volume_location::get(m_location_index);
}

// @address 0x2b71b8 ?destroy@c_light_volume_location@@SAXJ@Z
void c_light_volume_location::destroy(long location_index)
{
    c_light_volume_location *location = get(location_index);
    if (location->m_light_volume_index != -1)
        c_light_volume::destroy(location->m_light_volume_index);
    if (location->m_sibling_index != -1)
        c_light_volume_location::destroy(location->m_sibling_index);
    ::datum_delete((s_data_array *)g_light_volume_location_data, location_index);
}

// @address 0x2b7220 ?add_sibling@c_light_volume_location@@QAAXJ@Z
void c_light_volume_location::add_sibling(long sibling_index)
{
    if (m_sibling_index == -1)
        m_sibling_index = sibling_index;
    else
    {
        c_light_volume_location *sibling = get(m_sibling_index);
        sibling->add_sibling(sibling_index);
    }
}

void __tls_set_g_light_volume_allocator(void *object)
{
    data_set_new_base_address((s_data_array **)&g_light_volume_data, (s_data_array *)object);
}

static __forceinline void light_volumes_set_uninitialized(float *value)
{
    *(unsigned long *)value = 0xffffffff;
}


// @address 0x2b7290 ?create@c_light_volume@@SAJEJJK@Z
long c_light_volume::create(unsigned char type, long location, long definition, unsigned long seed)
{
    long result = -1;
    if (((s_data_array *)g_light_volume_data)->actual_count + 1 <= g_effect_budget_limits->table->blocks[7].limits[type].maximum_count)
    {
        long index = ::datum_new((s_data_array *)g_light_volume_data);
        c_light_volume *volume = c_light_volume::get(index);
        volume->m_location_index = location;
        volume->m_next_light_volume_index = -1;
        volume->m_definition_index = definition;
        volume->m_flags = 0;
        light_volumes_set_uninitialized(&volume->m_length);
        light_volumes_set_uninitialized(&volume->m_density);
        light_volumes_set_uninitialized(&volume->m_profile_length);
        volume->m_random_seed = seed;
        for (long i = 0; i < 2; ++i)
            volume->m_random_values[i] = _real_random_range(&volume->m_random_seed, 0.0f, 1.0f);
        volume->m_gpu_light_volume_index = c_light_volume_gpu::create(type, index);
        if (volume->m_gpu_light_volume_index == -1)
        {
            c_light_volume::destroy(index);
            index = -1;
        }
        g_light_volume_allocation_succeeded[type] = true;
        result = index;
    }
    else
    {
        g_light_volume_allocation_succeeded[type] = false;
    }
    return result;
}

// @address 0x2b7420 ?destroy@c_light_volume@@SAXJ@Z
void c_light_volume::destroy(long light_volume_index)
{
    c_light_volume *light_volume = get(light_volume_index);
    if (light_volume->m_gpu_light_volume_index != -1)
        c_light_volume_gpu::destroy(light_volume->m_gpu_light_volume_index);
    if (light_volume->m_next_light_volume_index != -1)
        c_light_volume::destroy(light_volume->m_next_light_volume_index);
    ::datum_delete((s_data_array *)g_light_volume_data, light_volume_index);
}

struct s_function_definition;
// @address 0x8c442c k_light_volume_states_initial_flags
extern "C" {
unsigned long k_light_volume_states_initial_flags;
}




// @address 0x2b6d40 ?query_effect@c_light_volume_system@@QAAXM@Z
void c_light_volume_system::query_effect(float dt)
{
    if (!(dt <= 0.0f))
        effect_update_light_volume_system(this, dt);
    unsigned long status = effect_get_event_status(m_effect_index, m_event_block_index, m_part_block_index, &m_age);
    if (status >= 1) {
        if (status != 1)
            set_should_live(false);
        else
            m_flags &= ~6;
    } else {
        m_flags |= 6;
    }
}

// @address 0x2b6dc0 ?frame_advance@c_light_volume_system@@QAAXM@Z
void c_light_volume_system::frame_advance(float dt)
{
    c_light_volume_states states;
    (&states)->set_light_volume_system(this);
    for (long location_index = m_first_location_index; location_index != -1; )
    {
        c_light_volume_location *location = c_light_volume_location::get(location_index);
        states.set_light_volume_location(location);
        for (long volume_index = location->m_light_volume_index; volume_index != -1; )
        {
            c_light_volume *volume = c_light_volume::get(volume_index);
            volume->frame_advance(dt, &states);
            volume_index = volume->m_next_light_volume_index;
        }
        location_index = location->m_sibling_index;
    }
    set_ever_frame_advanced(true);
}

// @address 0x2b6ec8 light_volume_system_render
extern "C" void light_volume_system_render(c_light_volume_system *system, long user_index, long pass)
{
    bool visible = true;
    long mode = system->first_person_type;
    bool user_bit = (system->m_first_person_flags & (1 << user_index)) != 0;
    if ((mode == 1 && !user_bit) || (mode == 2 && user_bit))
        visible = false;
    if (pass != 1)
        visible = false;
    if (visible)
    {
        bool first_person = user_bit && mode != 0;
        for (long location_index = system->m_first_location_index; location_index != -1; )
        {
            c_light_volume_location *location = c_light_volume_location::get(location_index);
            if (first_person == (location->first_person_flags & 1))
            {
                for (long volume_index = location->m_light_volume_index; volume_index != -1; )
                {
                    c_light_volume *volume = c_light_volume::get(volume_index);
                    volume->submit();
                    volume_index = volume->m_next_light_volume_index;
                }
            }
            location_index = location->m_sibling_index;
        }
    }
}

__forceinline void c_light_volume_system::pulse_all(float dt)
{
    c_data_iterator<c_light_volume_system> iterator;
    iterator.begin((s_data_array *)g_light_volume_system_data);
    while (iterator.next())
        iterator.get_datum()->query_effect(dt);
}

__forceinline void c_light_volume_system::destroy(long system_index)
{
    if (system_index != -1)
    {
        c_light_volume_system *system = get(system_index);
        long location_index = system->m_first_location_index;
        system->m_first_location_index = -1;
        if (location_index != -1)
            c_light_volume_location::destroy(location_index);
        datum_delete((s_data_array *)g_light_volume_system_data, system_index);
    }
}

// @address 0x14bf30 ?frame_advance_all@c_light_volume_system@@SAXM@Z
void c_light_volume_system::frame_advance_all(float dt)
{
    if (dt > 0.0f)
        pulse_all(dt);
    c_data_iterator<c_light_volume_system> iterator;
    iterator.begin((s_data_array *)g_light_volume_system_data);
    while (iterator.next())
    {
        c_light_volume_system *system = iterator.get_datum();
        if (!system->get_should_live())
            c_light_volume_system::destroy(iterator.get_index());
        else if (system->get_should_frame_advance())
            system->frame_advance(dt);
    }
}




// @address 0x2b7010 light_volume_location_new
extern "C" long light_volume_location_new(unsigned char type, unsigned long handle, const real_matrix4x3 *matrix, unsigned long value)
{
    long index = -1;
    if (((s_data_array *)g_light_volume_location_data)->actual_count + 1 <= g_effect_budget_limits->table->blocks[6].limits[type].maximum_count)
    {
        index = ::datum_new((s_data_array *)g_light_volume_location_data);
        c_light_volume_location *location = c_light_volume_location::get(index);
        location->m_sibling_index = -1;
        location->m_system_index = handle;
        location->m_light_volume_index = -1;
        location->m_position = matrix->position;
        location->m_forward = matrix->forward;
        c_light_volume_system *system = c_light_volume_system::get(handle);
        s_light_volume_system_definition *block = TAG_GET('ltvl', s_light_volume_system_definition, system->m_definition_index);
        c_light_volume *previous = 0;
        for (long definition_index = 0; definition_index < block->light_volumes.count; definition_index++)
        {
            long light_volume_index = c_light_volume::create(type, index, definition_index, value);
            if (light_volume_index != -1)
            {
                if (previous)
                    previous->m_next_light_volume_index = light_volume_index;
                else
                    location->m_light_volume_index = light_volume_index;
                previous = c_light_volume::get(light_volume_index);
            }
        }
        g_light_volume_location_allocation_succeeded[type] = true;
    }
    else
    {
        g_light_volume_location_allocation_succeeded[type] = false;
    }
    return index;
}

// @address 0x2b7480 ?frame_advance@c_light_volume@@QAAXMPAVc_light_volume_states@@@Z
void c_light_volume::frame_advance(float dt, c_light_volume_states *states)
{
    states->set_light_volume(this);
    const s_light_volume_definition *definition = get_definition();
    definition->evaluate_length_if_not_constant_over_time(states, &m_length);
    definition->evaluate_density_if_not_constant_over_time(states, &m_density);
    definition->evaluate_profile_length_if_not_constant_over_time(states, &m_profile_length);
    float length = c_light_volume_location::get(m_location_index)->m_length;
    if (length == 0.0f || length > m_length)
        length = m_length;
    m_offset = length;
}


static __forceinline __vector4 visibility_test_set(float x, float y, float z, float w)
{
    __vector4 xyzw = __lvlx(&x, 0);
    __vector4 yv = __lvlx(&y, 0);
    __vector4 zw = __lvlx(&z, 0);
    __vector4 wv = __lvlx(&w, 0);
    xyzw = __vrlimi(xyzw, yv, 4, 3);
    zw = __vrlimi(zw, wv, 4, 3);
    xyzw = __vrlimi(xyzw, zw, 3, 2);
    return xyzw;
}

static __forceinline __vector4 visibility_test_replicate(float value)
{
    __vector4 result = __lvlx(&value, 0);
    return __vspltw(result, 0);
}

// @address 0x156d30 visibility_volume_test_sphere_planes
#ifdef HALO3_64BIT
__attribute__((used))
#endif
extern "C" inline long visibility_volume_test_sphere_planes(const __vector4 *planes, const real_point3d *center, float radius)
{
    __vector4 c = visibility_test_set(center->x, center->y, center->z, 1.0f);
    __vector4 r = visibility_test_replicate(radius);
    return visibility_volume_test_sphere_vmx(c, r, planes[0], planes[1], planes[2], planes[3], planes[4], planes[5]);
}


// @address 0x9e7ea0 g_visibility_globals

static __forceinline c_visibility_collection *light_volumes_get_global_camera_collection()
{
    return (c_visibility_collection *)&g_visibility_globals.collection_input;
}

// @address 0x2b7600 ?render_callback@c_light_volume@@CAXPBXJ@Z
void c_light_volume::render_callback(const void *light_volume, long user_data)
{
    static_cast<const c_light_volume *>(light_volume)->render();
}

// @address 0x2b7730 ?submit@c_light_volume@@QBAXXZ
void c_light_volume::submit() const
{
    const c_light_volume_location *location = c_light_volume_location::get(m_location_index);
    if (visibility_volume_test_sphere_planes(
        light_volumes_get_global_camera_collection()->get_region()->projections[0].volume.vector_planes,
        location->get_origin(), get_length())) {
        e_transparent_sort_layer sort_layer = (e_transparent_sort_layer)get_definition()->get_shader()->get_sort_layer();
        render_objects_add_transparent_element(location->get_origin(), 0, 0.0f, sort_layer, render_callback, this, -1, 0.0f);
    }
}

// @address 0x2b7830 ?evaluate@?$c_editable_property@Vc_light_volume_states@@@@QBAMPBVc_light_volume_states@@@Z
float c_editable_property<c_light_volume_states>::evaluate(const c_light_volume_states *states) const
{
    return c_editable_property_base::evaluate(states);
}



// @address 0x2b78e8 ?allocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_light_volume_system_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<2,
    static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_light_volume_system_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// @address 0x2b7988 ?deallocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_light_volume_system_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<2,
    static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_light_volume_system_allocator>::deallocate(void *memory)
{
    g_light_volume_system_data = 0;
    restricted_region_free_member(2, m_member_index);
}

// @address 0x2b79a8 ?allocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_light_volume_location_data_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<2,
    static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_light_volume_location_data_allocator>::allocate(unsigned int size, const char *name)
{
    m_member_index = light_volumes_restricted_region_add_member(name, "", size, 0,
        __tls_set_g_contrail_profile_allocator, (s_data_array **)&g_light_volume_location_data);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(2, m_member_index);
}

// @address 0x2b7a48 ?deallocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_light_volume_location_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<2,
    static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_light_volume_location_data_allocator>::deallocate(void *memory)
{
    g_light_volume_location_data = 0;
    restricted_region_free_member(2, m_member_index);
}

// @address 0x2b7a68 ?allocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_light_volume_data_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<2,
    static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_light_volume_data_allocator>::allocate(unsigned int size, const char *name)
{
    m_member_index = light_volumes_restricted_region_add_member(name, "", size, 0,
        __tls_set_g_light_volume_allocator, (s_data_array **)&g_light_volume_data);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(2, m_member_index);
}

// @address 0x2b7b08 ?deallocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_light_volume_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<2,
    static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_light_volume_data_allocator>::deallocate(void *memory)
{
    g_light_volume_data = 0;
    restricted_region_free_member(2, m_member_index);
}

// @address 0x2b7608 ?render@c_light_volume@@QBAXXZ
void c_light_volume::render() const
{
    c_light_volume_states states;
    states.set_light_volume_system(get_parent_location()->get_parent_system());
    states.set_light_volume_location(get_parent_location());
    states.set_light_volume(this);
    const c_light_volume_gpu *gpu = c_light_volume_gpu::get(m_gpu_light_volume_index);
    gpu->render(&states);
}

// @address 0xea4f8 ?evaluate_scalar@c_function_definition@@QBAMMM@Z


t_light_volume_location_allocator g_light_volume_location_allocator;
t_light_volume_allocator g_light_volume_allocator;
t_light_volume_system_allocator g_light_volume_system_allocator;
t_light_volume_location_data_allocator g_light_volume_location_data_allocator;
t_light_volume_data_allocator g_light_volume_data_allocator;

