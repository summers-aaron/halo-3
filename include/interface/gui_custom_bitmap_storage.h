#ifndef INTERFACE_GUI_CUSTOM_BITMAP_STORAGE_H
#define INTERFACE_GUI_CUSTOM_BITMAP_STORAGE_H

// Functions defined in tu/interfacelibxbox/gui_custom_bitmap_storage.cpp that other units call.

#include "interface/gui_custom_bitmap_storage_item.h"
#include "interface/gui_custom_bitmap_storage_manager.h"
#include "memory/data_types.h"

struct s_gui_custom_bitmap_storage_globals
{
    // The array pointer doubles as the storage manager: the manager's only
    // field is that pointer, so bitmap calls go through the manager object.
    union
    {
        s_data_array *data;
        c_gui_custom_bitmap_storage_manager manager;
    };
    void *buffer;
    long buffer_size;
};

struct gui_custom_bitmap_storage_slot : s_datum_header
{
    long reference_count;
    long state;
    c_gui_custom_bitmap_storage_item item;
};

typedef char gui_custom_bitmap_globals_buffer_offset[(sizeof(void *) != 4 || offsetof(s_gui_custom_bitmap_storage_globals, buffer) == 4) ? 1 : -1];
typedef char gui_custom_bitmap_globals_size_offset[(sizeof(void *) != 4 || offsetof(s_gui_custom_bitmap_storage_globals, buffer_size) == 8) ? 1 : -1];
typedef char gui_custom_bitmap_slot_item_offset[(sizeof(void *) != 4 || offsetof(gui_custom_bitmap_storage_slot, item) == 12) ? 1 : -1];
typedef char gui_custom_bitmap_item_initialized_offset[(sizeof(void *) != 4 || offsetof(c_gui_custom_bitmap_storage_item, m_initialized) == 60) ? 1 : -1];
struct s_autosave_queue;
struct c_gui_custom_bitmap_widget;

extern "C" { extern s_gui_custom_bitmap_storage_globals g_gui_custom_bitmap_storage_data; }

extern "C" s_autosave_queue *g_gui_custom_bitmap_storage_manager;

extern "C" void gui_custom_bitmap_storage_release(s_data_array **array_reference, unsigned long index);
extern "C" c_gui_custom_bitmap_storage_item *gui_custom_bitmap_storage_try_and_get(s_data_array **array_reference, unsigned long index);
extern "C" s_autosave_queue *gui_custom_bitmap_storage_get_manager(long controller_index);
extern "C" bool gui_custom_bitmap_storage_load_chunk_into_widget(
    s_autosave_queue *queue, c_gui_custom_bitmap_widget *widget, bool use_compressed_format, long aspect_index);
extern "C" void gui_custom_bitmap_storage_release_manager(void);
extern "C" void gui_custom_bitmap_storage_reset(void);

extern "C" void gui_custom_bitmap_storage_release_texture(c_gui_custom_bitmap_storage_item *storage);

#endif // INTERFACE_GUI_CUSTOM_BITMAP_STORAGE_H
