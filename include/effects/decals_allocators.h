#ifndef EFFECTS_DECALS_ALLOCATORS_H
#define EFFECTS_DECALS_ALLOCATORS_H

#include "cache/restricted_memory_regions.h"
#include "effects/decal_system_types.h"
#include "effects/decals.h"

void __tls_set_g_effect_lightprobe_slot_data_allocator(void *);
void __tls_set_g_decal_data_allocator(void *);

extern t_restricted_allocation_manager<4, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_effect_lightprobe_slot_data_allocator> g_decal_system_allocator;
extern t_restricted_allocation_manager<4, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_decal_data_allocator> g_decal_array_allocator;

extern t_restricted_allocation_manager<3, &c_decal_system::adopt_old_render_message_queue, static_cast<void (*)(void *)>(0),
    &__tls_set_g_decal_message_queue_allocator> g_decal_message_queue_allocator;
template<>
void *t_restricted_allocation_manager<3, &c_decal_system::adopt_old_render_message_queue, static_cast<void (*)(void *)>(0),
    &__tls_set_g_decal_message_queue_allocator>::reserve_memory(const char *name, const char *type, unsigned int size,
    long alignment_bits);

#endif // EFFECTS_DECALS_ALLOCATORS_H
