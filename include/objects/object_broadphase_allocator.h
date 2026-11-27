#ifndef OBJECTS_OBJECT_BROADPHASE_ALLOCATOR_H
#define OBJECTS_OBJECT_BROADPHASE_ALLOCATOR_H

#include "cache/restricted_memory_regions.h"
#include "objects/object_broadphase.h"

extern t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), &object_broadphase_post_copy_fixup,
    &__tls_set_g_object_broadphase_allocator> g_object_broadphase_allocator;

#endif // OBJECTS_OBJECT_BROADPHASE_ALLOCATOR_H
