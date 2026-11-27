#ifndef SAVED_GAMES_SAVED_FILM_H
#define SAVED_GAMES_SAVED_FILM_H

#include "shell/shell.h"
#include "saved_games/saved_film_runtime.h"
#include "saved_games/saved_film_work_types.h"

// Functions defined in tu/blamlibxbox/saved_film.cpp.

long saved_film_get_current_tick(void);
void saved_film_notify_game_state_load(void);

enum e_map_memory_configuration;
class c_async_double_buffer;
struct s_film_replay_metadata;
struct s_saved_film_playback;
struct s_saved_film_slot_globals;

extern "C" long saved_film_play(void);
extern "C" void saved_film_delete_crash_reports(void);
extern "C" void saved_film_initialize(void);
extern "C" bool saved_film_can_load(long controller_index, const char *name);
extern "C" bool saved_film_set_playback_game_speed(float speed);
extern "C" void saved_film_set_pending_seek(int mode);
extern "C" void saved_film_handle_save_with_profile_name(long controller_index);
extern "C" void saved_film_handle_resume_without_save(void);
int saved_film_get_target_tick(void);
extern "C" bool saved_film_try_and_get_game_state(void);
extern "C" bool saved_film_can_skip(int mode);
extern "C" void saved_film_reset_playback_state(void);
extern "C" unsigned long saved_film_is_update_pending(void);
extern "C" void saved_film_reset_playback_upload(s_saved_film_playback *playback);
extern "C" long *saved_film_try_and_get_slot(s_saved_film_slot_globals *globals, long index);
extern "C" bool saved_film_test_slot_available(void);
extern "C" void saved_film_update_state_checkpoint(void);
extern "C" int saved_film_find_seek_entry(int mode);
extern "C" void saved_film_reset_work_globals(void);
extern "C" void saved_film_handle_start_recording(void);
extern "C" bool saved_film_reset_cameras_to_authored(void);
extern "C" void saved_film_initialize_timer(void);
extern "C" void saved_film_update_playback_download(s_saved_film_playback *unused);
extern "C" void saved_film_set_player_remap(bool value);
extern "C" void saved_film_handle_game_state_load();
extern "C" void saved_film_update(void);
extern "C" bool saved_film_load_game_state(long index);

class c_saved_film;
struct s_saved_film_work_entry;
struct simulation_update;

extern "C" int saved_film_get_marker_count(const s_saved_film_work_entry *entries);
extern "C" bool saved_film_try_and_get_timer_elapsed(float *elapsed_seconds);
extern "C" unsigned char saved_film_set_playback_limit(int limit, unsigned char flag);
extern "C" bool saved_film_has_pending_seek(void);
bool saved_film_can_control_playback(void);
extern "C" wchar_t *saved_film_get_map_display_name(wchar_t *destination, long size);
extern "C" bool saved_film_initialize_from_name(long controller_index, const char *name);
extern "C" void saved_film_update_playback_screen(s_saved_film_playback *playback);
extern "C" bool saved_film_update_storage_bytes(c_saved_film *film, const void *data, long size);
extern "C" bool saved_film_initialize_from_file(c_saved_film *film, const char *file_path, long controller_index, bool ignore_version);
extern "C" void saved_film_initialize_history_storage(e_map_memory_configuration configuration);
void saved_film_close(void);
extern "C" void saved_film_new_playback(e_map_memory_configuration configuration);
extern "C" void saved_film_dispose_playback(void);
extern "C" s_saved_film_work_entry *saved_film_new_work_entry(s_saved_film_work_entry *entries, bool flag);
extern "C" int saved_film_find_next_work_entry(s_saved_film_work_entry *entries, int threshold);
extern "C" int saved_film_find_evictable_work_entry(s_saved_film_work_entry *entries);
extern "C" void saved_film_reset_overlapped_task(void);
extern "C" void saved_film_handle_pending_screen_update(void);
extern "C" void saved_film_update_recording(const simulation_update *update);
extern "C" void saved_film_render_load_screen(s_saved_film_playback *playback, long unused);
extern "C" unsigned char saved_film_update_buffer_padding(void);
extern "C" unsigned char saved_film_try_and_get_verified_game_state(c_saved_film *film, long *chunk_type, void *buffer,
	long max_size, long *out_size, long *out_extra);
extern "C" bool saved_film_try_and_get_update_header(long *output);
extern "C" void saved_film_handle_keyboard_save(e_controller_index controller_index);

struct game_options;
class c_physical_memory_allocation;
template<long> class c_static_wchar_string;

extern "C" bool saved_film_can_record(const game_options *options);
extern "C" void saved_film_get_unique_name(long controller_index, c_static_wchar_string<16> *out_name);
extern "C" void saved_film_get_default_description(long controller_index, c_static_wchar_string<128> *out_description);
extern c_physical_memory_allocation g_saved_film_memory_allocator;

class c_allocation_base;
#include "bink/bink_sdk.h"

extern "C" s_saved_film_playback *g_saved_film_playback;
extern "C" void saved_film_get_player_views(void *out);

extern "C" void saved_film_reset_active_playback(void);
#endif // SAVED_GAMES_SAVED_FILM_H
