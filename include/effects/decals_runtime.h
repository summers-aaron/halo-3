#include "effects/decal_message_queue_tls.h"
#ifndef EFFECTS_DECALS_RUNTIME_H
#define EFFECTS_DECALS_RUNTIME_H
#include "effects/effect_budget_limits.h"
#include "math/real_math_types.h"
#include "rasterizer/rasterizer.h"
struct s_visibility_globals;
struct s_decals_render_container;
extern "C" {
extern s_visibility_globals g_visibility_globals;
extern long g_rasterizer_current_z_buffer_mode;
}
#endif
