#ifndef MAIN_MAIN_LOOP_GLOBALS_H
#define MAIN_MAIN_LOOP_GLOBALS_H

// Render, simulation, and main-loop state consumed by main.cpp.
struct D3DDevice;
#include "cache/geometry_storage.h"
#include "effects/decal_message_queue_tls.h"
#include "game/player_mapping_globals.h"
#include "rasterizer/rasterizer_main_globals.h"
class c_window_manager;
#include "game/game_globals_storage.h"
struct s_user_globals;
#include "interface/chud/chud_user_data_types.h"
#include "items/equipment_globals.h"
struct s_object_render_thread_message;
template<class t_message, long k_count> class t_message_queue;
extern "C" {
extern bool g_rasterizer_frame_has_ended;
extern char k_main_empty_string[];
extern unsigned int g_active_cinematic_zone_mask;
extern unsigned long g_active_designer_zone_mask;
extern c_window_manager g_window_manager;
extern __declspec(thread) t_message_queue<s_object_render_thread_message, 2048> *g_object_message_queue;
}

#endif // MAIN_MAIN_LOOP_GLOBALS_H
