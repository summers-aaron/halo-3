#ifndef CUTSCENE_RECORDED_ANIMATION_GLOBALS_H
#define CUTSCENE_RECORDED_ANIMATION_GLOBALS_H

#include "cache/restricted_memory_regions.h"
#include "cutscene/recorded_animations.h"

struct s_recorded_animation_codec;
extern "C" s_recorded_animation_codec *k_recorded_animation_codecs[];
extern "C" s_recorded_animation_codec k_recorded_animation_codec_definitions[];
extern t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    __tls_set_g_animation_threads_allocator> g_animation_threads_allocator;

struct s_data_array;
extern __declspec(thread) s_data_array *animation_threads;
static __forceinline s_data_array *&animation_threads_data() { return animation_threads; }

extern "C" void __tls_set_g_cinematic_light_globals_allocator(void *value);
template<> void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_cinematic_light_globals_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits);
template<> void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_cinematic_light_globals_allocator>::deallocate(
    void *address);

#endif // CUTSCENE_RECORDED_ANIMATION_GLOBALS_H
