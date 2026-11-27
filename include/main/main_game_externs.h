#ifndef MAIN_MAIN_GAME_EXTERNS_H
#define MAIN_MAIN_GAME_EXTERNS_H

#include <xtl.h>
#include "game/game_types.h"
#include "effects/decal_message_queue_tls.h"
class c_cache_file_tag_resource_runtime_manager;
struct s_cache_file_globals;

extern "C" { void scenario_tags_unload(void); }
extern "C" { int cache_file_tag_resource_reset_active_state(void); }
extern "C" void cache_file_tag_resource_runtime_manager_dispose_from_old_map(c_cache_file_tag_resource_runtime_manager *manager);
extern "C" { bool content_catalogue_make_ready_for_path(const char *path, bool validate); }
extern "C" { void cache_file_tag_resource_set_active_state(void *manager, long scenario_index, long zone_data, void *state); }
extern "C" { bool render_is_loading_complete(long controller_count); }
void user_interface_set_last_campaign_level(e_campaign_id campaign_id, e_map_id map_id);
extern "C" { void main_reset_global_requests(long); }
extern s_cache_file_globals g_cache_file_globals;

// Data and imported calls used by main_game.obj; placement remains in the owning units.
struct s_game_system;
struct s_main_flag_state;
#include "interface/user_interface_clock.h"
#include "networking/logic/life_cycle/life_cycle_globals_types.h"
#include "cseries/language_types.h"
#include "input/input_globals.h"
#include "objects/object_render_thread_message.h"
#include "effects/decal_message_types.h"
#include "effects/effect_message.h"
#include "multithreading/message_queue.h"
struct main_game_mode_globals;
#include "cache/geometry_globals.h"

extern "C" unsigned long XTLGetLanguage(void);
#include "game/game_system_types.h"

extern "C" int main_game_submit_live_request(int controller_index, int request_id,
    const void *request_data, int request_size, const void *response_data, void *overlapped);


#include "saved_games/game_state_xenon_globals.h"
struct s_object_render_thread_message;
template<class t_message, long k_count> class t_message_queue;
extern "C" { extern __declspec(thread) t_message_queue<s_object_render_thread_message, 2048> *g_object_message_queue; }
#include "cache/cache_files_windows_paths.h"
#include "cache/cache_files_windows.h"
#include "effects/effect_message_queue_tls.h"
#include "cseries/xenon_platform_api.h"

#include "main/main_render_loading_types.h"
extern "C++" s_main_flag_state main_globals;

#endif // MAIN_MAIN_GAME_EXTERNS_H
