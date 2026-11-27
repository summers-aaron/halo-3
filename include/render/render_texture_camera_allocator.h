#ifndef RENDER_RENDER_TEXTURE_CAMERA_ALLOCATOR_H
#define RENDER_RENDER_TEXTURE_CAMERA_ALLOCATOR_H

#include "cache/restricted_memory.h"

void __tls_set_g_render_texture_camera_globals_allocator(void *allocator);
template <>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0), &__tls_set_g_render_texture_camera_globals_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment);
extern t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_render_texture_camera_globals_allocator> g_render_texture_camera_globals_allocator;

#endif // RENDER_RENDER_TEXTURE_CAMERA_ALLOCATOR_H
