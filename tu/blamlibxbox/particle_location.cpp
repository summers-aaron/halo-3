#include "effects/particle_array_globals.h"
// blamlibxbox/particle_location.obj: from the records 0x1430b0 0x1434a0 0x14b4a0 0x2b64c0 0x2b6560
// @compiler xdk6130
// @flags /TP /O2
// @address 0x11df78c g_particle_location_data_array
__declspec(thread) s_data_array *g_particle_location_data_array;
#include "tag_files/tag_groups.h"
#include "math/matrix_math_types.h"
#include <stdlib.h>
#include "tag_files/tag_types.h"
#include "cache/restricted_memory_regions.h"
#include "gpu_particle/particle_block.h"
#include "effects/particle_emitter.h"
#include "effects/particle_location.h"
#include "effects/particle_system.h"
#include "effects/particle_system_definitions.h"
#include "effects/effect_definitions.h"
#include "memory/data.h"
#include "effects/particle_emitter_gpu_array_types.h"
#include "cache/restricted_memory_layout.h"
#include "cache/restricted_memory_registration.h"
#include "effects/particle_location_runtime_api.h"
#include "multithreading/thread_tls.h"
#include "cache/restricted_memory_regions_inline.h"
#include "cache/restricted_memory.h"
#include "effects/particle_states.h"
#include "math/matrix_math.h"
#include "memory/datum_index.h"
#include "memory/data_inline.h"
#include "cache/restricted_memory_region_members.h"
#include "effects/particle_location_limits.h"
namespace {
struct c_particle_location_data_publisher {
    static __forceinline void publish(void *address)
    {
        data_set_new_base_address((s_data_array **)&g_particle_location_data_array, (s_data_array *)address);
    }
};
}


static __forceinline bool particle_location_test_long_flag(unsigned long flags, long bit)
{
    return (_rotl(flags, 32 - bit) & 1) != 0;
}
static __forceinline const c_particle_system_definition *get_location_system_definition(
    const c_particle_system *system)
{
    const s_effect_definition *effect =
        TAG_GET('effe', const s_effect_definition, system->m_tag_index);
    const s_particle_location_effect_event *event = &static_cast<const s_particle_location_effect_event *>(effect->events.address)[system->m_def_index];
    return &event->particle_systems[system->m_sub_index];
}

// @address 0x8c43a8 g_effect_budget_limits
// @address 0x119a444 g_particle_location_type_created
// 16 entries: indexed by the particle-system type byte shared with g_particle_system_type_created,
// which exactly fills 0x119a434..0x119a444; all bytes are zero in the shipped image.
extern "C" { bool g_particle_location_type_created[16]; }
static __forceinline c_particle_location *particle_location_get(long location_index)
{
    return &((c_particle_location *)g_particle_location_data_array->data)[datum_index_to_absolute_index(location_index)];
}
static __forceinline long particle_location_emitter_get_index(const c_particle_emitter *emitter)
{
    return (long)(emitter - ((c_particle_emitter *)((s_data_array *)g_particle_emitter_data)->data)) | ((long)emitter->m_identifier << 16);
}
static __forceinline void particle_location_add_emitter(c_particle_location *location, c_particle_emitter *emitter)
{
    long emitter_index = particle_location_emitter_get_index(emitter);
    emitter->m_next_emitter = -1;
    if (location->first_emitter_index == -1)
        location->first_emitter_index = emitter_index;
    if (location->last_emitter_index != -1)
        datum_get<c_particle_emitter>(g_particle_emitter_data, location->last_emitter_index)->m_next_emitter = emitter_index;
    location->last_emitter_index = emitter_index;
}
// @address 0x1430b0 particle_location_new
extern "C" long particle_location_new(unsigned char type, long system_index, unsigned long flags)
{
    long result = -1;
    s_data_array *locations = (s_data_array *)g_particle_location_data_array;
    if (locations->actual_count + 1 <= g_effect_budget_limits->table->blocks[_particle_location_budget].limits[type].maximum_count) {
        result = datum_new((s_data_array *)g_particle_location_data_array);
        c_particle_location *location = particle_location_get(result);
        location->first_emitter_index = -1;
        location->last_emitter_index = -1;
        location->next_location_index = -1;
        location->matrix = *global_identity4x3;
        location->velocity = *global_zero_vector3d;
        location->age = 0.0f;
        location->state = 0;
        location->scale = 1.0f;
        location->system_index = system_index;
        location->flags = (unsigned char)flags;
        const c_particle_system *system = &((c_particle_system *)g_particle_system_data_array->data)[datum_index_to_absolute_index(system_index)];
        const c_particle_system_definition *definition = get_location_system_definition(system);
        for (long i = 0; i < definition->get_emitter_count(); ++i) {
            long emitter_index = particle_emitter_new(system->m_type, result, i);
            if (emitter_index != -1) {
                particle_location_add_emitter(location,
                    &((c_particle_emitter *)g_particle_emitter_data->data)[datum_index_to_absolute_index(emitter_index)]);
            }
        }
        g_particle_location_type_created[type] = true;
    } else {
        g_particle_location_type_created[type] = false;
    }
    return result;
}
// @address 0x1434a0 particle_location_update
extern "C" int particle_location_update(s_particle_location_datum *location, float delta, c_particle_state_list *state_list)
{
    long count = 0;
    c_particle_system *system =
        &((c_particle_system *)g_particle_system_data_array->data)[datum_index_to_absolute_index(location->system_index)];
    const c_particle_system_definition *definition = get_location_system_definition(system);
    bool flag = particle_location_test_long_flag(location->state, 2) || !particle_location_test_long_flag(location->state, 3);
    bool update_emitters = location->scale > 0.0f
        && (flag || flag_test(definition->flags, 1));
    bool remove_emitters = location->scale == 0.0f
        || (!flag && (definition->flags & 1) == 0);
    long emitter_index = location->first_emitter_index;
    if (emitter_index != -1) {
        do {
            c_particle_emitter *emitter =
                &((c_particle_emitter *)((s_data_array *)g_particle_emitter_data)->data)[(unsigned short)emitter_index];
            if (update_emitters)
                count += particle_emitter_spawn_particles(emitter, delta, state_list);
            else if (remove_emitters)
                count -= particle_emitter_clear_particles((c_particle_emitter *)emitter);
            emitter_index = emitter->m_next_emitter;
        } while (emitter_index != -1);
    }
    unsigned long flags = location->state | 16;
    location->state = flags;
    if (!update_emitters)
        return count;
    if (!particle_location_test_long_flag(flags, 1)) {
        location->state = flags | 2;
        location->bounding_sphere_center = location->matrix.position;
        return count;
    }
    if (definition->get_coordinate_system() != 1) {
        float radius = location->bounding_sphere_radius;
        const real_point3d *sphere_center = &location->bounding_sphere_center;
        const real_point3d *sphere_point = &location->matrix.position;
        float sphere_dy = sphere_center->y - sphere_point->y;
        float sphere_dx = sphere_center->x - sphere_point->x;
        float sphere_dz = sphere_center->z - sphere_point->z;
        bool inside_sphere = true;
        if (sphere_dx * sphere_dx + sphere_dz * sphere_dz + sphere_dy * sphere_dy > radius * radius)
            inside_sphere = false;
        if (!inside_sphere) {
            real_vector3d offset;
            vector_from_points3d(&location->bounding_sphere_center, &location->matrix.position, &offset);
            float length = normalize3d(&offset);
            float scale = (length - radius) * 0.5f;
            point_from_line3d(&location->bounding_sphere_center, &offset, scale,
                &location->bounding_sphere_center);
            location->bounding_sphere_radius += scale;
        }
    }
    return count;
}
// @address 0x14b4a0 particle_location_delete
extern "C" void particle_location_delete(unsigned long location_index)
{
    c_particle_location *location = datum_get<c_particle_location>(g_particle_location_data_array, location_index);
    long emitter_index = location->first_emitter_index;
    while (emitter_index != -1)
    {
        c_particle_emitter *emitter = datum_get<c_particle_emitter>(g_particle_emitter_data, emitter_index);
        long next_emitter_index = emitter->m_next_emitter;
        particle_emitter_clear_particles((c_particle_emitter *)emitter);
        long gpu_index = emitter->m_gpu_index;
        if (gpu_index != -1)
        {
            s_particle_emitter_gpu_block *gpu =
                &((s_particle_emitter_gpu_block *)particle_location_gpu_data_get()->data)[datum_index_to_absolute_index(gpu_index)];
            particle_block_delete_element(gpu->block_index, true);
            gpu->block_index = -1;
            gpu->count = 0;
            ::datum_delete((s_data_array *)particle_location_gpu_data_get(), gpu_index);
        }
        ::datum_delete((s_data_array *)g_particle_emitter_data, emitter_index);
        emitter_index = next_emitter_index;
    }
    ::datum_delete((s_data_array *)g_particle_location_data_array, location_index);
}
// @address 0x2b64c0 ?allocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_particle_location_data_array_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_particle_location_data_array_allocator>::allocate(unsigned int size, const char *name)
{
    m_member_index = restricted_region_add_member<c_particle_location_data_publisher>(
        2, name, "", size, 0, __tls_set_g_particle_location_data_array_allocator, 0, 0);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(2, m_member_index);
}
// @address 0x2b6560 ?deallocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_particle_location_data_array_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_particle_location_data_array_allocator>::deallocate(void *address)
{
    g_particle_location_data_array = 0;
    restricted_region_free_member(2, m_member_index);
}

t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_particle_location_data_array_allocator> g_particle_location_data_array_allocator;

