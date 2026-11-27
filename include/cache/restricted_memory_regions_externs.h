#ifndef CACHE_RESTRICTED_MEMORY_REGIONS_EXTERNS_H
#define CACHE_RESTRICTED_MEMORY_REGIONS_EXTERNS_H
#include "render/camera_fx_globals_types.h"
#include "render/render_player_view_globals.h"

// Region alias flags occupy the five TLS bytes immediately after the base table.
// Per-region alias flags (TLS 0x831df644), owned by restricted_memory_regions.cpp:
// March 2007 debug name and type (restricted_region_add_alias stores a byte per region).
extern __declspec(thread) bool g_restricted_alias[];

// These region allocations publish their address through shared TLS slots.
// Region 2 hosts the "render game globals" allocation, which render code views as the camera settings array.
static __forceinline void *restricted_region2_address() { return g_player_view_camera_settings; }
static __forceinline void *restricted_camera_fx_address() { return camera_fx_globals; }

static __forceinline void restricted_region2_set_address(void *address) { g_player_view_camera_settings = (render_player_view_camera_settings *)address; }
static __forceinline void restricted_camera_fx_set_address(void *address) { camera_fx_globals = (s_camera_fx_globals *)address; }

#include "cache/restricted_memory_regions.h"
typedef t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &restricted_region_set_region2_address> restricted_region2_allocation_manager;

#endif // CACHE_RESTRICTED_MEMORY_REGIONS_EXTERNS_H
