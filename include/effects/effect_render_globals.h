#ifndef EFFECTS_EFFECT_RENDER_GLOBALS_H
#define EFFECTS_EFFECT_RENDER_GLOBALS_H
#include <stddef.h>
// Flags read by effects.cpp and hs_function.cpp; weather cache read by
// render_objects.cpp and atmosphere.cpp at +8 (128 indices).
struct s_effect_render_globals
{
    bool render_unknown0;
    bool camera_relative;
    bool patchy_fog;
    unsigned char first_person_hidden;
    bool enabled;
    long cached_indices[128];
};
typedef char effect_render_weather_offset[(offsetof(s_effect_render_globals, cached_indices) == 8) ? 1 : -1];
extern "C" { extern __declspec(thread) s_effect_render_globals *g_effect_render_globals; }

static __forceinline s_effect_render_globals *tls_effect_render_globals() { return g_effect_render_globals; }

#endif
