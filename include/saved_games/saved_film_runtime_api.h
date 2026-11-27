#ifndef SAVED_GAMES_SAVED_FILM_RUNTIME_API_H
#define SAVED_GAMES_SAVED_FILM_RUNTIME_API_H

#include "shell/shell.h"

class c_physical_memory_allocation;
class c_gui_error_manager;
class c_synchronized_long;
struct s_saved_film_slot_globals;
struct s_saved_film_work_entry;
struct s_game_state_compressed_buffer;

extern c_physical_memory_allocation g_saved_film_allocator;
extern "C" s_saved_film_slot_globals g_saved_film_slots;
extern "C" bool game_state_compress_current_state(s_game_state_compressed_buffer *buffer, long unused, unsigned long flags,
	void **buffer_out, unsigned long *size_out);
extern "C" unsigned char game_state_compress_buffer(s_game_state_compressed_buffer *, unsigned long, const void *, unsigned long, void **, unsigned long *);
extern "C" long saved_film_snippet_find_best_marker_for_tick(s_saved_film_work_entry *entries, long tick, bool seekable_only);
extern "C" void *gui_error_manager_find_queued_error(c_gui_error_manager *errors, e_controller_index controller_index, long error_name,
	const wchar_t *custom_message);
extern "C" long saved_game_files_update_metadata_async(long controller_index, long content_item_index, const wchar_t *name,
	const wchar_t *description, c_synchronized_long *completion, c_synchronized_long *done);
extern "C" void saved_film_snippet_handle_message(long message_a, long message_b, const void *message_c, long message_d);

#endif
