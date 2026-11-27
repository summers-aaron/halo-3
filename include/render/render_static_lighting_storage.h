#ifndef RENDER_RENDER_STATIC_LIGHTING_STORAGE_H
#define RENDER_RENDER_STATIC_LIGHTING_STORAGE_H
#include "render/camera_fx_globals_types.h"

struct s_data_array;
extern "C" {
extern __declspec(thread) s_data_array * g_static_lighting_effect_attachments;
}

#endif
