#ifndef INTERFACE_GUI_CUSTOM_BITMAP_STORAGE_DEPENDENCIES_H
#define INTERFACE_GUI_CUSTOM_BITMAP_STORAGE_DEPENDENCIES_H

struct s_autosave_queue;
class c_gui_custom_bitmap_widget;
class c_synchronized_long;
class c_sound_cache_hash_source;
struct s_sound_cache_hash_state;
template<typename T> class c_basic_buffer;

extern "C" void cache_file_bulk_reader_initialize_hash_state(s_sound_cache_hash_state *state, unsigned long read_size,
    c_basic_buffer<void> buffer, c_sound_cache_hash_source *source, bool hash_enabled);
extern "C" unsigned char gui_custom_bitmap_widget_load_bitmap_from_buffer(
    c_gui_custom_bitmap_widget *widget, bool a, const char *buffer, long size, long aspect_index);
extern "C" bool async_block_until_done(
    c_synchronized_long *done, bool (*yield_function)(c_synchronized_long *), bool idle, bool with_idle, long file, long line);
extern "C" long autosave_queue_clear_entries(s_autosave_queue *manager);
extern "C" void autosave_queue_finish_task(s_autosave_queue *manager);


#endif
