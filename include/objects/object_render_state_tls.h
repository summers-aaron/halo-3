#ifndef OBJECTS_OBJECT_RENDER_STATE_TLS_H
#define OBJECTS_OBJECT_RENDER_STATE_TLS_H

struct s_data_array;

extern "C" {
extern __declspec(thread) s_data_array *g_cached_object_render_states;
extern __declspec(thread) long *g_object_render_data;
extern long g_object_render_data_count;
}
static __forceinline s_data_array *tls_object_render_state_data() { return g_cached_object_render_states; }

#endif // OBJECTS_OBJECT_RENDER_STATE_TLS_H
