#ifndef RENDER_RENDER_OBJECT_STORAGE_H
#define RENDER_RENDER_OBJECT_STORAGE_H

struct s_render_object_globals;
extern "C" {
extern __declspec(thread) s_render_object_globals *g_render_object_globals;
}

#endif
