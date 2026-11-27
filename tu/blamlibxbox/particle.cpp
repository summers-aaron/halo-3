// blamlibxbox/particle.obj: from the records 0x146298 0x146438 0x149478 0x2b37b0 0x2b3b50 0x2b3c50 0x2b6580 0x2b6598 0x2b6620 0x2b6638 0x2b6698 0x2b66e8 0x2b6700 0x2b6760 0x2b6878 0x2b6918
#include "effects/particle_array_globals.h"
#include "effects/effect_runtime_storage.h"
// @address 0x11df790 g_particle_instance_owner_data
__declspec(thread) s_data_array *g_particle_instance_owner_data;
// @address 0x11df7f4 g_particle_instance_location_data
__declspec(thread) s_data_array *g_particle_instance_location_data;
// @address 0x11df814 g_particle_instance_data
__declspec(thread) s_data_array *g_particle_instance_data;
// @address 0x11df7e4 g_contrail_chain_data
__declspec(thread) s_data_array *g_contrail_chain_data;
// @address 0x8c43a8 g_effect_budget_limits
// @flags /TP /O2
#include "memory/data_types.h"
#include "multithreading/thread_tls.h"
#include "tag_files/tag_index.h"
#include "tag_files/tag_types.h"
#include "objects/object_reference_types.h"
// @address 0x2b6580 ?__tls_set_g_memory_pool_allocator@@YAXPAX@Z
#include <math.h>
#include <new>
#include "math/matrix_math.h"
#include <ppcintrinsics.h>
#include "cache/restricted_memory_regions.h"
#include "cache/restricted_memory.h"
#include "scenario/scenario.h"
#include "sound/game_sound.h"
#include "gpu_particle/particle_block.h"
#include "effects/particle.h"
#include "effects/particle_emitter.h"
#include "effects/particle_system.h"
#include "effects/particle_system_definitions.h"
#include "effects/effect_contrail_location_types.h"
#include "gpu_particle/light_volume_gpu.h"
#include "memory/data.h"
#include "effects/contrail_states.h"
#include "game/materials.h"
#include "math/real_math_globals.h"
#include "cache/cache_files.h"
#include "gpu_particle/contrail_gpu.h"
#include "gpu_particle/gpu_particle_globals.h"
#include "cache/cache_file_globals.h"
#include "gpu_particle/effect_parameters_types.h"
#include "tag_files/tag_groups.h"
#include "cache/restricted_memory_layout.h"
#include "effects/effect_pending_pool.h"
#include "effects/effect_gpu_states.h"
#include "effects/particle_globals.h"
// @address 0x119a44c g_particle_location_type_has_capacity
// @address 0x119a450 g_particle_instance_type_has_capacity
extern "C" {
bool g_particle_location_type_has_capacity[4];
bool g_particle_instance_type_has_capacity[4];
}

#include "effects/particle_instance_types.h"
#include "effects/particle_location_runtime_types.h"
#include "effects/light_volume_types.h"
#include "effects/particle_movement_internals.h"
#include "effects/particle_location_limits.h"
#include "effects/particle_definitions.h"
#include "effects/particle_states.h"
#include "math/random_math_range.h"
#include "memory/datum_index.h"
#include "sound/game_sound_types.h"
#include "effects/material_effect_types.h"
#include "effects/material_effects.h"
#include "effects/effects.h"

struct s_particle_impact_effect
{
    real_point3d position;
    real_vector3d normal;
    real_vector3d direction;
    c_global_material_type target_material;
};

// @address 0xa31760 g_effect_pending_pool
#include "multithreading/interlocked_slist_push.h"

struct s_particle_system_limits_owner;
static __forceinline s_particle_instance *particle_instance_get(long index)
{
    return &((s_particle_instance *)((s_data_array *)g_particle_instance_data)->data)[(unsigned short)index];
}
static __forceinline void particle_instance_set_uninitialized(float *value)
{
    *(unsigned long *)value = 0xffffffff;
}
static __forceinline float particle_instance_real_random(unsigned long *seed)
{
    random_seed_next(seed);
    return (float)(*seed >> 16) * 1.5259022e-05f;
}


// @address 0x146298 ?particle_new_location_from_spawn@@YAJEJPAUs_particle_spawn_parameters@@J@Z
long particle_new_location_from_spawn(unsigned char type, long system_index, s_particle_spawn_parameters *spawn, long seed)
{
    long result = -1;
    if ((unsigned long)(g_particle_instance_location_data->actual_count + 1) <=
        g_effect_budget_limits->table->blocks[_particle_instance_location_budget].limits[type].maximum_count) {
        long location_index = datum_new(g_particle_instance_location_data);
        s_particle_instance *previous = 0;
        long i = 0;
        s_effect_contrail_location *location = &((s_effect_contrail_location *)g_particle_instance_location_data->data)[(unsigned short)location_index];
        location->next_index = -1;
        location->owner_index = system_index;
        location->first_instance_index = -1;
        location->position = spawn->matrix.position;
        location->direction = spawn->matrix.forward;
        s_light_volume_system_definition *definition = (s_light_volume_system_definition *)
            global_tag_instances[datum_index_to_absolute_index(((c_light_volume_system *)
                ((s_data_array *)g_particle_instance_owner_data)->data)[(unsigned short)system_index].m_definition_index)].definition;
        if (definition->light_volumes.count > 0) {
            do {
                long instance_index = particle_instance_new(type, location_index, i, seed);
                if (instance_index != -1) {
                    if (previous)
                        previous->next = instance_index;
                    else
                        location->first_instance_index = instance_index;
                    previous = particle_instance_get(instance_index);
                }
                ++i;
            } while (i < definition->light_volumes.count);
        }
        g_particle_location_type_has_capacity[type] = true;
        result = location_index;
    } else {
        g_particle_location_type_has_capacity[type] = false;
    }
    return result;
}

// @address 0x146438 ?particle_instance_new@@YAJEJJK@Z
long particle_instance_new(unsigned char type, long location, long definition, unsigned long seed)
{
    long result = -1;
    if (((s_data_array *)g_particle_instance_data)->actual_count + 1 <=
        g_effect_budget_limits->table->blocks[_particle_instance_budget].limits[type].maximum_count) {
        long index = datum_new((s_data_array *)g_particle_instance_data);
        s_particle_instance *instance = particle_instance_get(index);
        instance->location_index = location;
        instance->next = -1;
        instance->definition_index = definition;
        instance->flags = 0;
        particle_instance_set_uninitialized(&instance->length);
        particle_instance_set_uninitialized(&instance->offset);
        particle_instance_set_uninitialized(&instance->density);
        particle_instance_set_uninitialized(&instance->profile_length);
        instance->random_seed = seed;
        for (long i = 0; i < 2; ++i)
            instance->random_values[i] = particle_instance_real_random(&instance->random_seed);
        instance->gpu_index = particle_block_new_contrail_element(type, index);
        if (instance->gpu_index == -1) {
            particle_instance_delete(index);
            index = -1;
        }
        g_particle_instance_type_has_capacity[type] = true;
        result = index;
    } else {
        g_particle_instance_type_has_capacity[type] = false;
    }
    return result;
}

void __tls_set_g_memory_pool_allocator(void *object)
{
    data_set_new_base_address((s_data_array **)&g_particle_instance_owner_data, (s_data_array *)object);
}


// @address 0x2b6598 particle_initialize_for_new_map
extern "C" void particle_initialize_for_new_map(void)
{
    data_make_valid((s_data_array *)g_particle_instance_owner_data);
    data_make_valid(g_particle_instance_location_data);
    data_make_valid((s_data_array *)g_particle_instance_data);
    particle_block_initialize_contrail_shaders();
    data_make_valid((s_data_array *)g_contrail_chain_data);
    data_make_valid((s_data_array *)g_contrail_gpu_link_data);
}


// Registers a relocatable block as a member of field layout 2 (see
// c_restricted_memory::add_member), fixes up its pointers in this thread's copy of the layout
// (data_set_new_base_address, which also publishes it through a thread-local handle) and
// returns its per-thread address. One of a family of identical allocators that
// differ only in the layout index, the update callback and the handle.

// @address 0x2b6620 particle_location_set_base_address
void particle_location_set_base_address(void *object)
{
    data_set_new_base_address((s_data_array **)&g_particle_instance_location_data, (s_data_array *)object);
}

// @address 0x2b6638 particle_delete_location_chain
extern "C" void particle_delete_location_chain(int index)
{
    s_effect_contrail_location *datum =
        datum_get<s_effect_contrail_location>(g_particle_instance_location_data, index);

    if (datum->first_instance_index != -1)
        particle_instance_delete(datum->first_instance_index);
    if (datum->next_index != -1)
        particle_delete_location_chain(datum->next_index);
    ::datum_delete((s_data_array *)g_particle_instance_location_data, index);
}

// @address 0x2b6698 particle_location_append
void particle_location_append(s_effect_contrail_location *node, int value)
{
    while (node->next_index != -1)
        node = &((s_effect_contrail_location *)g_particle_instance_location_data->data)
            [datum_index_to_absolute_index(node->next_index)];
    node->next_index = value;
}

// @address 0x2b66e8 particle_instance_set_base_address
void particle_instance_set_base_address(void *object)
{
    data_set_new_base_address((s_data_array **)&g_particle_instance_data, (s_data_array *)object);
}

static __forceinline s_particle_instance *particle_list_datum_get(long index)
{
    return &((s_particle_instance *)((s_data_array *)g_particle_instance_data)->data)[(unsigned short)index];
}

// @address 0x2b6700 particle_instance_delete
void particle_instance_delete(int index)
{
    s_particle_instance *datum = particle_list_datum_get(index);

    if (datum->gpu_index != -1)
        contrail_gpu_destroy(datum->gpu_index);
    if (datum->next != -1)
        particle_instance_delete(datum->next);
    ::datum_delete((s_data_array *)g_particle_instance_data, index);
}

// The owning unit still exports the legacy C entry point.

// @address 0x2b6878 ?allocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_memory_pool_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_memory_pool_allocator>::allocate(unsigned int size, const char *name)
{
  int member_index = g_restricted_regions[2].add_member(name, "", size, 0, __tls_set_g_memory_pool_allocator, 0, 0);
  unsigned int offset = g_restricted_regions[2].get_member_offset(member_index);
  data_set_new_base_address((s_data_array **)&g_particle_instance_owner_data, (s_data_array *)(g_restricted_address[2] + offset));
  m_member_index = member_index;
  m_thread_id = get_current_thread_index();
  return g_restricted_address[2] + g_restricted_regions[2].get_member_offset(member_index);
}


// @address 0x2b6918 ?deallocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_memory_pool_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_memory_pool_allocator>::deallocate(void *address)
{
  g_particle_instance_owner_data = 0;
  restricted_region_free_member(2, m_member_index);
}

class c_particle;

// @address 0x149478 particle_queue_effect_message
extern "C" void particle_queue_effect_message(c_tag_index owner_index, long definition_index, unsigned char flags_a, unsigned char flags_b,
    const real_point3d *position, const real_vector3d *direction, const real_vector3d *velocity,
    float scale_a, float scale_b, const unsigned short *bounds)
{
    s_effect_pool_node *node = (s_effect_pool_node *)InterlockedPopEntrySList(&g_effect_pending_pool.free_list);
    __lwsync();
    if (node != 0) {
        effects_async_payload *payload = &node->creation;
        node->definition_index = owner_index;
        node->reserved = 0;
        payload->point = *position;
        payload->forward = *direction;
        payload->up = *velocity;
        payload->scale_a = scale_a;
        payload->scale_b = scale_b;
        payload->object_index = definition_index;
        payload->lightprobe_slot = flags_a;
        payload->user_mask = flags_b;
        payload->location_hint = *(const s_location *)bounds;
        normalize3d_with_default(&payload->forward, global_down3d);
        __lwsync();
        InterlockedPushEntrySList(&g_effect_pending_pool.pending, node);
    }
}
// @address 0x2b3b50 ?spawn_sound@c_particle@@ABAXPBUs_particle_collision_entry@@PBVc_particle_collision_event@@@Z
void c_particle::spawn_sound(const s_particle_collision_entry *entry, const c_particle_collision_event *) const
{
    struct {
        s_sound_location sound;
        s_location loc;
    } frame;
    // match: 0x2b3b50: binding velocity first makes the copies use the rotation address and derive velocity at -24.
    const real_vector3d *velocity = global_zero_vector3d;
    const real_quaternion *rotation = global_identity_quaternion;
    frame.sound.position = m_position;
    frame.sound.rotation = *rotation;
    frame.sound.velocity = *velocity;
    scenario_location_from_point(&frame.loc, &frame.sound.position);
    float magnitude = sqrtf(m_velocity.i * m_velocity.i + m_velocity.j * m_velocity.j + m_velocity.k * m_velocity.k) - 0.5f;
    float volume = magnitude > 0.0f ? (magnitude < 1.0f ? magnitude : 1.0f) : 0.0f;
    unattached_impulse_sound_new(entry->reference.index, &frame.sound, volume);
}

struct light_volume_gpu_submit_owner;
extern "C" {
// @address 0x8c4428 k_contrail_default_dirty_mask
}
// @address 0x2b6760 particle_submit_gpu_state
extern "C" void particle_submit_gpu_state(s_particle_instance *instance)
{
    s_object_14 states;
    states.definition = 0;
    states.location = 0;
    states.source = 0;
    states.pad30 = 0.0f;
    states.valid = 0x200;
    states.dirty = k_contrail_default_dirty_mask;
    contrail_states_set_definition((s_object_14 *)&states,
        (s_definition_14 *)&((c_light_volume_system *)((s_data_array *)g_particle_instance_owner_data)->data)[
            (unsigned short)((s_effect_contrail_location *)g_particle_instance_location_data->data)[(unsigned short)instance->location_index].owner_index]);
    s_effect_contrail_location *location = &((s_effect_contrail_location *)g_particle_instance_location_data->data)[(unsigned short)instance->location_index];
    if (location != states.location) {
        states.location = location;
        contrail_states_update_values((s_object_14 *)&states, 0);
    }
    if (instance != states.source) {
        unsigned long valid = states.valid;
        states.source = instance;
        states.valid = valid & ~0x30;
        contrail_states_update_values((s_object_14 *)&states, 0x30);
    }
    ((const c_light_volume_gpu *)((s_data_array *)g_contrail_chain_data)->data)
        [datum_index_to_absolute_index(instance->gpu_index)].render(
        (const s_effect_parameters *)&states);
}

static __forceinline c_global_material_type particle_material_from_index(short index)
{
    c_global_material_type material;
    material.m_index = index;
    return material;
}

// @address 0x2b3c50 particle_handle_impact_effect
extern "C" void particle_handle_impact_effect(c_particle *particle, s_particle_collision_entry *source,
    s_particle_impact_effect *impact, long sweetener_size)
{
    s_location location;
    long material_effect_values[7];
    scenario_location_from_point(&location, &particle->m_position);
    if (impact)
    {
        short target_material_index = impact->target_material.m_index;
        float magnitude = sqrtf(particle->m_velocity.j * particle->m_velocity.j + particle->m_velocity.i * particle->m_velocity.i +
            particle->m_velocity.k * particle->m_velocity.k) - 0.5f;
        float intensity = magnitude > 0.0f ? (magnitude < 1.0f ? magnitude : 1.0f) : 0.0f;
        const real_vector3d *direction = &impact->direction;
        const real_vector3d *normal = &impact->normal;
        material_effect_find_effects(false, &impact->position, source->reference.index,
            k_global_material_type_invalid, particle_material_from_index(target_material_index), true, _effect_type_particle_impact,
            (e_sweetener_size)sweetener_size, material_effect_values);
        effect_spawn_impact_effects(material_effect_values, &impact->position, normal, direction, &location, 0);
        game_sound_new_material_effect_sounds(material_effect_values, &impact->position, normal, direction, (const long *)&location, -1, intensity);
    }
}


// @address 0x2b37b0 particle_handle_collision_event
extern "C" void particle_handle_collision_event(const c_particle *particle, long type, long definition_index,
    const c_particle_collision_event *collision_event, const c_particle_state_list *state_list)
{
    const s_data_array *emitters = (const s_data_array *)g_particle_emitter_data;
    const s_data_array *locations = (const s_data_array *)g_particle_location_data_array;
    const s_data_array *particles = (const s_data_array *)g_particle_system_data_array;
    const c_particle_emitter *emitter = &((c_particle_emitter *)emitters->data)[(particle->m_emitter_index & 0xffffUL)];
    const c_particle_location *location = &((c_particle_location *)locations->data)[(emitter->m_location_index & 0xffffUL)];
    const c_particle_system *part = &((c_particle_system *)particles->data)[(location->system_index & 0xffffUL)];
    const s_effect_definition *tag = (const s_effect_definition *)
        global_tag_instances[datum_index_to_absolute_index(part->m_tag_index)].definition;
    const c_particle_system_definition *group = &((const s_particle_movement_effect_event *)tag->events.address)[part->m_def_index]
        .particle_systems[part->m_sub_index];
    const s_particle_collision_effect *def;
    if (group->get_particle_definition_index() != -1)
        def = (const s_particle_collision_effect *)
            global_tag_instances[datum_index_to_absolute_index(group->get_particle_definition_index())].definition;
    else
        def = 0;
    float zero = 0.0f;
    for (long count = 0; (unsigned)count < def->count; count++) {
        const s_particle_collision_entry *entry = &def->entries[count];
        if (type == (long)entry->type) {
            long owner = entry->reference.index;
            if (owner != -1) {
                unsigned long group_tag = g_cache_file_globals.tags_header->groups[global_tag_instances[(short)owner].group_index].group_tags[0];
                if (group_tag != 0x65666665) {
                    if (group_tag != 0x666f6f74) {
                        particle->spawn_sound(entry, collision_event);
                    } else {
                        particle_handle_impact_effect((c_particle *)particle, (s_particle_collision_entry *)entry,
                            (s_particle_impact_effect *)collision_event, !(bool)((def->flags >> 4) & 1) ? -1 : 0);
                    }
                } else {
                    const real_point3d *position;
                    const real_vector3d *direction;
                    const real_vector3d *velocity;
                    if (collision_event) {
                        position = &collision_event->m_position;
                        direction = &collision_event->m_forward;
                        velocity = &collision_event->m_normal;
                    } else {
                        position = &particle->m_position;
                        direction = &particle->m_velocity;
                        velocity = &global_identity4x3->up;
                    }
                    unsigned char flag_a = entry->flag_a;
                    float scale_a = zero;
                    float scale_b = zero;
                    if (((1 << flag_a) & 0xffefffff) != 0)
                        scale_a = state_list->get_state_value(flag_a);
                    unsigned char flag_b = entry->flag_b;
                    if (((1 << flag_b) & 0xffefffff) != 0)
                        scale_b = state_list->get_state_value(flag_b);
                    long effect_storage;
                    c_tag_index *effect = new (&effect_storage) c_tag_index(owner);
                    particle_queue_effect_message(*effect, definition_index, flag_a, flag_b, position, direction, velocity,
                        scale_a, scale_b, (const unsigned short *)&part->m_location);
                }
            }
        }
    }
}

t_particle_restricted_allocation_manager g_memory_pool_allocator;

