#include "interface/gui_selected_item_metadata.h"
#ifndef INTERFACE_GUI_SELECTED_ITEMS_GAME_VARIANT_DEPENDENCIES_H
#define INTERFACE_GUI_SELECTED_ITEMS_GAME_VARIANT_DEPENDENCIES_H

#include "memory/static_arrays.h"
class c_game_variant;
struct s_gui_selected_items_block;
struct s_saved_game_item_metadata_packed;
struct s_file_reference;
class c_content_catalogue;
class c_gui_game_variant_subitem_selectable_item_datasource;
class c_synchronized_long;

// Typed external dependencies of gui_selected_items_game_variant.cpp.
extern "C" {
void gui_game_variant_begin_autosave_queue_enumeration(c_gui_game_variant_subitem_selectable_item_datasource *self);
void *game_engine_variant_initialize_default(c_game_variant *variant, long engine_type);
bool saved_game_files_get_metadata_string(const void *metadata, long string_id, c_static_wchar_string<1024> *string);
long content_catalogue_find_item_by_file_reference(const c_content_catalogue *catalogue, const s_file_reference *file);
long saved_game_files_read(long catalogue_index, long content_item_index, void *buffer, unsigned long size, c_synchronized_long *completion,
    c_synchronized_long *done);
void saved_game_files_initialize_metadata(s_saved_game_item_metadata_packed *metadata, long index, int file_type, const wchar_t *name,
    const wchar_t *description, unsigned __int64 identifier, unsigned __int64 size_in_bytes);
bool async_block_until_done(
    c_synchronized_long *done, bool (*yield_function)(c_synchronized_long *), bool idle, bool with_idle, long file, long line);
}
#endif
