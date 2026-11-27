#include "interface/gui_selected_item_metadata.h"
#include "saved_games/saved_game_item_metadata.h"
#ifndef INTERFACE_GUI_SELECTED_ITEMS_SAVED_FILM_EXTERNS_H
#define INTERFACE_GUI_SELECTED_ITEMS_SAVED_FILM_EXTERNS_H

// External dependencies used by gui_selected_items_saved_film.
struct s_blf_saved_film;
#include "saved_games/content_catalogue_types.h"
#include "saved_games/saved_game_collect_files.h"
struct s_file_reference;
class gui_selected_items_saved_film_synchronized_long;
class c_synchronized_long;
template<long k_maximum_count> class c_static_wchar_string;
struct s_saved_game_item_metadata;

extern "C" unsigned char saved_film_parse_options_from_buffer(const s_blf_saved_film *, void *, void *, void *, bool *);

extern "C" bool async_block_until_done(gui_selected_items_saved_film_synchronized_long *done,
    bool (*yield_function)(c_synchronized_long *), bool idle, bool with_idle, long file, long line);
extern "C" bool saved_game_files_get_metadata_string(const void *metadata, long string_id, c_static_wchar_string<1024> *string);

extern "C" long _InterlockedExchange(volatile long *target, long value);

#include "saved_games/saved_game_file_category_types.h"
#include "saved_games/saved_game_file_types.h"
extern "C" {

int saved_game_files_read(int, int, s_blf_saved_film *, unsigned long, gui_selected_items_saved_film_synchronized_long *,
    gui_selected_items_saved_film_synchronized_long *);
int async_read_entire_file(const char *, s_blf_saved_film *, unsigned long, int, int, int,
    gui_selected_items_saved_film_synchronized_long *, gui_selected_items_saved_film_synchronized_long *);
}

#endif // INTERFACE_GUI_SELECTED_ITEMS_SAVED_FILM_EXTERNS_H
