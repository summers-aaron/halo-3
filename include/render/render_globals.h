#include "rasterizer/rasterizer_device_states.h"
#ifndef RENDER_RENDER_GLOBALS_H
#define RENDER_RENDER_GLOBALS_H
#include "rasterizer/rasterizer_sampler_filter_state.h"

#include "math/real_math_types.h"
#include "math/real_math_constants.h"
struct render_player_view;
class c_player_view;
struct s_object_render_thread_message;
template<class t_message, long k_count> class t_message_queue;


extern "C" {
extern __declspec(thread) t_message_queue<s_object_render_thread_message, 2048> *g_object_message_queue;
}

// Both spellings name the single player-view array both subsystems share; a second
// array definition would emit a duplicate dynamic initializer, so the rasterizer
// alias spells through the static member.
#define g_rasterizer_player_views c_player_view::x_global_player_views

#endif
