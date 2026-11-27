#ifndef EFFECTS_LIGHT_VOLUME_ALLOCATORS_INLINE_H
#define EFFECTS_LIGHT_VOLUME_ALLOCATORS_INLINE_H

#include "effects/light_volume_allocators.h"
#include "effects/light_volumes_globals.h"
#include "effects/light_volumes.h"
#include "effects/particle.h"
#include "cache/restricted_memory_registration.h"
#include "cache/restricted_memory_regions_inline.h"
#include "memory/data.h"
#include "multithreading/thread_tls.h"

template<> __forceinline void *t_restricted_allocation_manager<2,
    static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_light_volume_location_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    m_member_index = restricted_region_register_member(2, name, type, size, alignment_bits, particle_location_set_base_address, 0, 0);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(2, m_member_index);
}

template<> __forceinline void *t_restricted_allocation_manager<2,
    static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_light_volume_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    m_member_index = restricted_region_register_member(2, name, type, size, alignment_bits, particle_instance_set_base_address, 0, 0);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(2, m_member_index);
}

namespace {
static __forceinline long light_volumes_restricted_region_add_member(const char *name, const char *type,
    unsigned int size, long alignment_bits, field_proc_t set_address, s_data_array **data_array)
{
    long member_index = restricted_memory_register_member(&g_restricted_regions[2], name, type, size,
        alignment_bits, set_address, 0, 0);
    void *address = restricted_region_get_member_address(2, member_index);
    data_set_new_base_address(data_array, (s_data_array *)address);
    return member_index;
}
}

template<> __forceinline void *t_restricted_allocation_manager<2,
    static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_light_volume_system_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    m_member_index = light_volumes_restricted_region_add_member(name, type, size, alignment_bits,
        __tls_set_g_light_volume_system_allocator, &g_light_volume_system_data);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(2, m_member_index);
}

#endif // EFFECTS_LIGHT_VOLUME_ALLOCATORS_INLINE_H
