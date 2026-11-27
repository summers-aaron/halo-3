#ifndef INTERFACE_GUI_SELECTED_ITEMS_MAP_H
#define INTERFACE_GUI_SELECTED_ITEMS_MAP_H

#include "interface/gui_data.h"
#include "memory/static_arrays.h"
#include <stddef.h>
#include "interface/gui_simple_selected_item.h"
#include "tag_files/file_reference.h"
#include "saved_games/saved_game_collect_files.h"
#include "game/game_types.h"
#include "multithreading/synchronized_value.h"

#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"

struct s_gui_selected_items_map_category_entry
{
    long type;
    long id;
    c_static_wchar_string<48> name;
    c_static_wchar_string<256> text;
    long value;
};

class c_gui_map_category_datasource : public c_gui_ordered_data
{
public:
    // match: 0x4ebd48: restrict-qualified this preserves shipped-image flag-load scheduling.
    virtual bool initialize(long name) __restrict;
    virtual void get_column_names(long *const names, long *count);
protected:
    virtual long get_current_item_count_internal();
public:
    c_gui_map_category_datasource(const wchar_t *title, const wchar_t *description, const wchar_t *default_title, const wchar_t *default_description);
    virtual bool get_text_value(long context, long string_id, c_static_wchar_string<1024> *value);
    virtual bool get_string_id_value(long context, long string_id, long *result);
    virtual bool get_integer_value(long element_handle, long value_name, long *value);

    c_static_wchar_string<48> m_first_name;
    c_static_wchar_string<256> m_first_description;
    c_static_wchar_string<48> m_second_name;
    c_static_wchar_string<256> m_second_description;
    s_gui_selected_items_map_category_entry m_items[50];
    long m_item_count;
};

typedef char gui_map_category_entry_size[(sizeof(s_gui_selected_items_map_category_entry) == 0x26c) ? 1 : -1];
typedef char gui_map_category_items_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_map_category_datasource, m_items) == 0x5cc))) ? 1 : -1];
typedef char gui_map_category_count_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_map_category_datasource, m_item_count) == 0x7ee4))) ? 1 : -1];

int map_selected_item_sort_proc(const void *a, const void *b);

class c_gui_map_selected_item : public c_gui_simple_selected_item
{
public:
    c_gui_map_selected_item();
    c_gui_map_selected_item(const s_gui_selected_items_block *metadata, e_gui_selected_item_location location,
        long controller_index, const s_file_reference *file, long sort_order, long, unsigned char enabled, unsigned char corrupt);
    virtual bool get_file_path(char *buffer, long size) const;
    __forceinline long get_special_item_type() const { return m_special_type; }
    __forceinline long get_sort_order() const { return sort; }
    __forceinline long get_map_id() const { return m_metadata.map_id; }
    c_gui_map_selected_item &operator=(const c_gui_map_selected_item &other);
    bool get_map_variant(class c_map_variant *map_variant) const;
private:
    long controller_index;
    s_file_reference file;
    long sort;
};

struct s_gui_map_saved_game_queue : s_saved_game_queue
{
    __forceinline s_gui_map_saved_game_queue() : s_saved_game_queue(-1, -1) {}
    ~s_gui_map_saved_game_queue(); // cancels the enumeration and waits for it

    __forceinline bool active() const { return state.peek() != 0 && state.peek() != 3; }
};

typedef char gui_map_queue_find_file_data_offset[(sizeof(void *) != 4 || ((offsetof(s_gui_map_saved_game_queue, find_file_data) == 0x3680))) ? 1 : -1];
typedef char gui_map_queue_thread_id_offset[(sizeof(void *) != 4 || ((offsetof(s_gui_map_saved_game_queue, thread_id) == 0x3a08))) ? 1 : -1];
typedef char gui_map_queue_done_offset[(sizeof(void *) != 4 || ((offsetof(s_gui_map_saved_game_queue, done) == 0x3a0c))) ? 1 : -1];
typedef char gui_map_queue_size[(sizeof(void *) != 4 || ((sizeof(s_gui_map_saved_game_queue) == 0x3a18))) ? 1 : -1];

class c_gui_map_subitem_selectable_item_datasource : public c_gui_ordered_data {
public:
    virtual void get_column_names(long *const names, long *count);
    virtual bool get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *value);
    virtual bool get_integer_value(long element_handle, long value_name, long *value);
    virtual bool get_string_id_value(long context, long string_id, long *result);
    virtual const c_gui_selected_item *get_gui_selected_item(long index) const;
    __forceinline long item_count() const { return m_selected_item_count; }
    __forceinline const c_gui_map_selected_item *item_at(long index) const { return &m_entries[index]; }
    long is_enumerating();
    void cancel();
    virtual bool initialize(long name, e_map_id map_id, e_controller_index controller_index);
#if !defined(_XBOX) || defined(__clang__)
    // dispose() is cancel() (0x4ec448), update() the enumeration pump (0x4ec498), is_busy() is_enumerating()
    // (0x4ecca0).
    virtual void dispose() { cancel(); }
    virtual void update() { begin_enumeration(m_controller_index, static_cast<e_map_id>(m_map_id)); }
    virtual bool is_busy() { return is_enumerating() != 0; }
#endif
protected:
    virtual long get_current_item_count_internal();
    void begin_saved_game_files_enumeration(long controller_index, e_map_id map_id);
    void begin_autosave_queue_enumeration(long controller_index, e_map_id map_id);
    void begin_enumeration(long controller_index, e_map_id map_id);
public:
    c_gui_map_subitem_selectable_item_datasource();
protected:
    long m_map_id;
    c_gui_map_selected_item m_entries[151];
    long m_selected_item_count;
    long m_controller_index;
    unsigned char m_enumeration_complete;
    s_gui_map_saved_game_queue m_queue;
};

#endif // INTERFACE_GUI_SELECTED_ITEMS_MAP_H
