#ifndef SAVED_GAMES_AUTOSAVE_QUEUE_TYPES_H
#define SAVED_GAMES_AUTOSAVE_QUEUE_TYPES_H

#include "saved_games/autosave_queue_functions.h"

#include "interface/gui_simple_selected_item.h"
#include "tag_files/file_reference.h"
#include "multithreading/synchronized_value.h"
#include <stddef.h>
#include <string.h>

class c_gui_screenshot_selected_item : public c_gui_simple_selected_item
{
public:
    c_gui_screenshot_selected_item();
    c_gui_screenshot_selected_item(const s_saved_game_item_metadata_packed *metadata, const char *author,
        unsigned __int64 date, unsigned __int64 size_in_bytes, e_gui_selected_item_location location,
        long controller, const s_file_reference *reference, bool corrupt);
    __forceinline c_gui_screenshot_selected_item(const c_gui_screenshot_selected_item &other) :
        c_gui_simple_selected_item(other)
    {
        controller_index = other.controller_index;
        file_reference = other.file_reference;
        memcpy(author, other.author, sizeof(author));
        date = other.date;
        size_in_bytes = other.size_in_bytes;
        bitmap_list_item_index = other.bitmap_list_item_index;
    }
    c_gui_screenshot_selected_item &operator=(const c_gui_screenshot_selected_item &other);
    virtual bool get_file_path(char *buffer, long size) const;
    void mark_bitmap_load_failed() { m_flag15 = true; }

    long controller_index;
    s_file_reference file_reference;
    char author[16];
    unsigned __int64 date;
    unsigned __int64 size_in_bytes;
    long bitmap_list_item_index;
};

struct s_autosave_queue_file_header;

struct s_autosave_queue
{
    // match: 0x221d70: defer the lock and completion stores until after the manager pointer is published.
    __forceinline s_autosave_queue()
        : initialized(false), reference_count(0),
          lock(c_synchronized_long::s_defer_initialization()),
          task_done(c_synchronized_long::s_defer_initialization()) {}

    bool initialized;
    volatile long reference_count; // Bitmap storage manager retains/releases the queue at +4.
    c_gui_screenshot_selected_item items[50];
    long item_count;
    long busy;
    long task_id;
    c_synchronized_long lock;
    c_synchronized_long task_done;
    long controller_index;
    unsigned char catalogue_enumerated;
    long content_item_index;
    s_autosave_queue_file_header *buffer;
    unsigned long buffer_size;
    unsigned long file_size;
    long loading_item_index;
    long selected_index;
    unsigned char selection_valid;
    wchar_t name_text[16];
    bool name_valid;
    wchar_t description_text[128];
    bool description_valid;
    long pending_index;
};

typedef char autosave_queue_bitmap_index_offset[
    (sizeof(void *) != 4 || ((offsetof(c_gui_screenshot_selected_item, bitmap_list_item_index) == 0x2c8))) ? 1 : -1];
typedef char autosave_queue_bitmap_failed_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_screenshot_selected_item, m_flag15) == 0x15))) ? 1 : -1];
typedef char autosave_queue_entry_type_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_screenshot_selected_item, m_type) == 8))) ? 1 : -1];
typedef char autosave_queue_entry_metadata_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_screenshot_selected_item, m_metadata) == 0x18))) ? 1 : -1];

typedef char autosave_queue_entry_size[(sizeof(void *) != 4 || ((sizeof(c_gui_screenshot_selected_item) == 0x2d0))) ? 1 : -1];
typedef char autosave_queue_items_offset[(offsetof(s_autosave_queue, items) == 8) ? 1 : -1];
typedef char autosave_queue_count_offset[(sizeof(void *) != 4 || ((offsetof(s_autosave_queue, item_count) == 0x8ca8))) ? 1 : -1];
typedef char autosave_queue_selected_offset[(sizeof(void *) != 4 || ((offsetof(s_autosave_queue, selected_index) == 0x8cd8))) ? 1 : -1];
typedef char autosave_queue_selection_valid_offset[(sizeof(void *) != 4 || ((offsetof(s_autosave_queue, selection_valid) == 0x8cdc))) ? 1 : -1];

#endif
