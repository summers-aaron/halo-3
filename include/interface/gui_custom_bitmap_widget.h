#ifndef INTERFACE_GUI_CUSTOM_BITMAP_WIDGET_H
#define INTERFACE_GUI_CUSTOM_BITMAP_WIDGET_H

#include "interface/c_gui_bitmap_widget.h"
#include "interface/custom_map_image_types.h"
#include "game/game_types.h"
#include "multithreading/synchronized_value.h"
#include "tag_files/file_reference.h"
#include <stddef.h>

struct c_gui_custom_bitmap_widget;
struct s_gui_start_menu_settings_screenshots;
class c_gui_list_item_widget;

extern "C" unsigned char gui_custom_bitmap_widget_load_bitmap_from_buffer(c_gui_custom_bitmap_widget *self,
    bool use_compressed_format, const char *buffer, long size, long aspect_index);
extern "C" void start_menu_settings_update_preview_bitmap(s_gui_start_menu_settings_screenshots *screen,
    c_gui_list_widget *list, c_gui_custom_bitmap_widget *bitmap);
extern "C" void start_menu_settings_reset_item_bitmap(s_gui_start_menu_settings_screenshots *screen, c_gui_list_item_widget *list_item);

// This widget holds the bitmap storage handle, the asynchronous file load
// state, and the requested and loaded map image paths.
struct c_gui_custom_bitmap_widget : c_gui_bitmap_widget
{
    c_gui_custom_bitmap_widget();
    int m_storage_index;
    void *m_async_load_buffer;
    long m_async_load_size;
    // The chooser calls this bitmap_index; it holds the async load task ID,
    // while m_storage_index identifies the allocated bitmap storage.
    long bitmap_index;
    c_synchronized_long m_async_task_done;
    c_synchronized_long m_async_task_cancel;
    c_synchronized_long m_async_task_success;
    s_file_reference m_file_reference;
    bool m_loading;
    int m_task_id;
    char m_map_image[256];        // requested by set_map_image
    char m_loaded_map_image[256]; // the path update() last started loading

    void set_map_image(e_custom_map_image_type type, e_map_id map_id, bool loading);
    long get_allocated_size();
    long get_overhang_size();
    bool is_ready(bool *loading);
#if !defined(_XBOX) || defined(__clang__)
    // should_render() is is_ready() (0x4923b0): render only while visible with no load in flight.
    virtual bool should_render(bool *render_with_bounds) { return is_ready(render_with_bounds); }
#endif
    int share_bitmap(c_gui_custom_bitmap_widget *other);
    virtual void dispose();
    virtual const bitmap_data *get_current_bitmap() const;
    virtual void update(unsigned long current_milliseconds);
    virtual void assemble_render_data(s_gui_widget_render_data *render_data, const rectangle2d *window_bounds,
        e_controller_index local_controller_index, bool a, bool b, bool c);
private:
    bool get_dimensions(long *width, long *height);
    void reset();
    friend void start_menu_settings_update_preview_bitmap(s_gui_start_menu_settings_screenshots *screen,
        c_gui_list_widget *list, c_gui_custom_bitmap_widget *bitmap);
    friend void start_menu_settings_reset_item_bitmap(s_gui_start_menu_settings_screenshots *screen, c_gui_list_item_widget *list_item);
    friend unsigned char gui_custom_bitmap_widget_load_bitmap_from_buffer(
        c_gui_custom_bitmap_widget *self, bool use_compressed_format, const char *buffer, long size, long aspect_index);
};

typedef char gui_custom_bitmap_widget_size[(sizeof(void *) != 4 || sizeof(c_gui_custom_bitmap_widget) == 0x458) ? 1 : -1];
typedef char gui_custom_bitmap_widget_storage_offset[(sizeof(void *) != 4 || offsetof(c_gui_custom_bitmap_widget, m_storage_index) == 0x120) ? 1 : -1];
typedef char gui_custom_bitmap_widget_task_offset[(sizeof(void *) != 4 || offsetof(c_gui_custom_bitmap_widget, bitmap_index) == 0x12c) ? 1 : -1];
typedef char gui_custom_bitmap_widget_file_offset[(sizeof(void *) != 4 || offsetof(c_gui_custom_bitmap_widget, m_file_reference) == 0x13c) ? 1 : -1];
typedef char gui_custom_bitmap_widget_map_image_offset[(sizeof(void *) != 4 || offsetof(c_gui_custom_bitmap_widget, m_map_image) == 0x254) ? 1 : -1];
typedef char gui_custom_bitmap_widget_loaded_image_offset[(sizeof(void *) != 4 || offsetof(c_gui_custom_bitmap_widget, m_loaded_map_image) == 0x354) ? 1 : -1];

#endif // INTERFACE_GUI_CUSTOM_BITMAP_WIDGET_H
