#include "sound/scripted_sound_constants.h"
#ifndef HS_HS_FUNCTION_EXTERNS_H
#define HS_HS_FUNCTION_EXTERNS_H

#include "input/input.h"
#include "math/real_math_types.h"
#include "render/camera_fx_settings.h"

struct real_decibel;

// March 360cache_debug:runtime_platform.obj names this type and global;
// its enumerators are not recoverable, so the list stays empty.
enum e_runtime_platform_type
{
};
extern e_runtime_platform_type active_type;

extern "C" {
extern unsigned char g_vehicle_ghost_effects_enabled;
}
#endif
