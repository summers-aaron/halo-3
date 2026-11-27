#ifndef PHYSICS_HAVOK_PROXIES_H
#define PHYSICS_HAVOK_PROXIES_H

// Functions defined in tu/blamlibxbox/havok_proxies.cpp.

bool havok_proxy_aquire_internal(long object_index, bool create_record);
void havok_proxy_release_internal(long object_index, bool delete_record);
void havok_proxies_move();
void havok_proxies_update();
long object_get_havok_proxy_index(long object_index);

extern bool g_havok_proxy_rigid_bodies_active;

void __tls_set_g_havok_proxy_data_allocator(void *allocation);
#include "cache/restricted_memory_regions.h"

extern t_restricted_allocation_manager<1,
    static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0),
    &__tls_set_g_havok_proxy_data_allocator> g_havok_proxy_data_allocator;

void havok_proxies_initialize();
void havok_proxies_dispose();
void havok_proxies_initialize_for_new_map();
void havok_proxies_dispose_from_old_map();

#endif // PHYSICS_HAVOK_PROXIES_H
