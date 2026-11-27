#ifndef RENDER_CAMERA_FX_GLOBALS_TYPES_H
#define RENDER_CAMERA_FX_GLOBALS_TYPES_H

struct s_global_modifier {
    unsigned long flags;
    float exposure_scale;
    float exposure_offset;
    float current_b;
    float current_a;
    float start_a;
    float target_a;
    float start_b;
    float target_b;
    long start_time;
    long end_time;
    union { long pulse_ticks; long instant_autoexposure; };
    long hold_ticks;
};
struct s_camera_fx_globals : s_global_modifier {
};


// Per-thread camera effects state for scripted exposure control.
extern __declspec(thread) s_camera_fx_globals *camera_fx_globals;

#endif
