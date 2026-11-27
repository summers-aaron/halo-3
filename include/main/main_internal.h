#ifndef MAIN_MAIN_INTERNAL_H
#define MAIN_MAIN_INTERNAL_H

#include "cache/restricted_memory_regions.h"
#include "main/main.h"
#include "main/main_time.h"

extern t_restricted_allocation_manager<3, &main_thread_combine_timing_data, static_cast<void (*)(void *)>(0), &__tls_set_g_main_gamestate_timing_data_allocator>
    g_main_gamestate_timing_data_allocator;
extern t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_main_render_timing_data_allocator> g_main_render_timing_data_allocator;

struct D3DDevice;
struct simulation_update;
struct s_simulation_update_metadata;
extern "C" void simulation_build_game_update(bool, struct simulation_update *, s_simulation_update_metadata *);
#include "rasterizer/xenon/rasterizer_xenon_device.h"
#include "cache/cache_files_windows.h"
extern "C" long _InterlockedAnd(long volatile *destination, long value);

// Specializations supplied by main_time.cpp and main.cpp, respectively.
template<>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), &main_time_globals_post_copy_update,
    &__tls_set_g_main_time_globals_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits);
template<> void t_restricted_allocation_manager<3, &main_thread_combine_timing_data, static_cast<void (*)(void *)>(0),
    &__tls_set_g_main_gamestate_timing_data_allocator>::deallocate(void *memory);

#endif
