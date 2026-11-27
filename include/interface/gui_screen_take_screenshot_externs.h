#ifndef INTERFACE_GUI_SCREEN_TAKE_SCREENSHOT_EXTERNS_H
#define INTERFACE_GUI_SCREEN_TAKE_SCREENSHOT_EXTERNS_H

// External dependencies used by gui_screen_take_screenshot.
template<long k_maximum_count> class c_static_wchar_string;
struct s_data_array;
struct levels_map_datum;
class c_content_catalogue;
enum e_controller_index;
enum e_async_completion;
struct s_async_task;

extern "C" void text_group_get_string(long tag_index, long string_id, c_static_wchar_string<1024> *string);
extern "C" bool levels_try_and_get_datum(s_data_array *data, int unique_id, levels_map_datum *definition);
extern "C" bool content_catalogue_is_item_name_valid(c_content_catalogue *catalogue, e_controller_index controller, const wchar_t *name, long content_type);
extern "C" e_async_completion saved_game_files_write_screenshot(s_async_task *task);

struct s_saved_film_playback;
struct s_file_reference;
extern "C" long D3DDevice_IsFencePending(long fence);
extern "C" void saved_film_encode_screenshot_header(char *buffer, long size, e_controller_index controller, long content_type,
    const wchar_t *title, const wchar_t *description, __int64 length, long flags);
extern "C" long saved_film_initialize_upload(s_saved_film_playback *playback, e_controller_index controller, long content_index,
    const s_file_reference *source, unsigned __int64 game_instance);

extern "C" {

extern s_saved_film_playback *g_saved_film_playback;

}

struct D3DBaseTexture;
#include "saved_games/saved_film_recording_types.h"
extern "C" {
long XGSetTextureHeader(unsigned long, unsigned long, unsigned long, long, long, long, long, long, long, D3DBaseTexture *, long *, long *);
unsigned char main_screenshot_start_capture(
    long width, long height, unsigned long texture_width, unsigned long texture_height, long buffer, long buffer_size, long volatile *lock);

}

#endif // INTERFACE_GUI_SCREEN_TAKE_SCREENSHOT_EXTERNS_H
