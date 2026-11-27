#ifndef RENDER_DEPTH_OF_FIELD_H
#define RENDER_DEPTH_OF_FIELD_H

// Functions defined in tu/graphicslibxbox/depth_of_field.cpp.

void __tls_set_g_depth_of_field_game_globals_allocator(void *address);

#include "cache/restricted_memory_regions.h"

typedef t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_depth_of_field_game_globals_allocator> depth_of_field_allocation_manager;

extern depth_of_field_allocation_manager g_depth_of_field_game_globals_allocator;

extern "C" const char k_depth_of_field_globals_name[];

extern "C" const double k_depth_of_field_smoothstep_two;

extern "C" const double literal_3_0;

struct s_depth_of_field;
struct s_depth_of_field_focus;
extern "C" __declspec(thread) s_depth_of_field *g_depth_of_field_game_globals;
extern "C" void depth_of_field_set_globals(void *address);
extern "C" float depth_of_field_compute_blur_sigma(s_depth_of_field_focus *focus);
#include "objects/object_recycling_storage.h"
static __forceinline s_depth_of_field *tls_depth_of_field_game_globals() { return (s_depth_of_field *)g_recycling_volumes; }
static __forceinline void depth_of_field_game_globals_set(s_depth_of_field *value) { g_recycling_volumes = value; }

template<>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_depth_of_field_game_globals_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits);

#endif // RENDER_DEPTH_OF_FIELD_H
