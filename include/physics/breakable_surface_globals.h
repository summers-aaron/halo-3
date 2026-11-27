#ifndef PHYSICS_BREAKABLE_SURFACE_GLOBALS_H
#define PHYSICS_BREAKABLE_SURFACE_GLOBALS_H

extern "C" long g_breakable_surface_frame_index;

extern "C" unsigned char g_breakable_surface_stencil_ref;

extern "C" __declspec(thread) void *g_breakable_surface_globals;

extern bool breakable_surface_effect_enabled;

#endif
