#ifndef RENDER_CAMERA_FX_SETTINGS_H
#define RENDER_CAMERA_FX_SETTINGS_H

// Functions defined in tu/graphicslibxbox/camera_fx_settings.cpp.

struct s_global_modifier;

void animate_global_exposure_modifier(s_global_modifier *modifier, float delta_time);
extern "C" void camera_fx_set_globals_address(void *value);
extern "C" void camera_fx_animate_exposure(float start_a, float target_a, float start_b, float target_b, float duration_seconds);
extern "C" void camera_fx_set_exposure(float scale);

class c_camera_fx_settings;

extern "C" void camera_fx_settings_initialize(c_camera_fx_settings *s, bool enabled);

#include "cache/restricted_memory_regions.h"

typedef t_restricted_allocation_manager<3,
    static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0),
    &camera_fx_set_globals_address> camera_fx_settings_allocation_manager;

extern camera_fx_settings_allocation_manager g_camera_fx_globals_allocator;

extern "C" void camera_fx_allocate_globals(void);
extern "C" void camera_fx_initialize_globals(void);

#endif // RENDER_CAMERA_FX_SETTINGS_H
