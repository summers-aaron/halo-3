#ifndef MAIN_MAIN_LOOP_DEPENDENCIES_H
#define MAIN_MAIN_LOOP_DEPENDENCIES_H

enum e_main_pregame_frame;
enum e_async_completion;
struct s_async_task;
struct s_main_crash_report_progress;
class c_cache_file_tag_resource_runtime_manager;
struct chud_scripting_globals_tls;
#include "main/main_render_loading_types.h"
struct s_online_files_encrypted_writer;
struct s_controller_signin_cache_entry;

void main_render_pregame(e_main_pregame_frame pregame_frame, const char *text);
extern "C++" s_main_crash_report_progress g_main_crash_upload_progress;
extern "C" long g_experience_rank_icon_element_handles[14][4];
extern "C" unsigned char window_manager_is_profile_screen_present(s_controller_signin_cache_entry *signin_cache);
extern "C" e_async_completion close_file_callback(s_async_task *);
extern "C" void tag_resource_update(c_cache_file_tag_resource_runtime_manager *manager);
extern "C" void game_engine_update_waypoint_timers(chud_scripting_globals_tls *scripting_globals);
extern "C" unsigned char game_state_try_and_get_compressed_buffer(void *buffer);
extern "C" void saved_film_get_player_views(void *out);
extern "C" void data_compress_encrypted_writer_dispose(s_online_files_encrypted_writer *writer);

#endif // MAIN_MAIN_LOOP_DEPENDENCIES_H
