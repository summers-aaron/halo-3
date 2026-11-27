#ifndef INTERFACE_GUI_GAME_VARIANT_DATASOURCE_TYPES_H
#define INTERFACE_GUI_GAME_VARIANT_DATASOURCE_TYPES_H

#include <stddef.h>
#include "interface/gui_data.h"
#include "interface/gui_selected_items_game_variant.h"
#include "tag_files/file_reference.h"
#include "saved_games/saved_game_collect_files.h"

#include "multithreading/synchronized_value.h"
#include "cseries/async.h"
#include "cseries/async_block.h"
struct s_gv_enumeration_queue : s_saved_game_queue
{
    __forceinline s_gv_enumeration_queue() : s_saved_game_queue(-1, -1) {}
    ~s_gv_enumeration_queue() { cancel_and_wait(); }
    bool active_reload() const { return state.peek() != 0 && state.peek() != 3; }
    void request_cancel() { cancel.set(1); }
    void cancel_and_wait()
    {
        request_cancel();
        async_block_until_done(&done, simple_yield_function, true, false, 0, 0);
    }
};
typedef char gv_queue_write_count_offset[(sizeof(void *) != 4 || ((offsetof(s_gv_enumeration_queue, write_count) == 0x3678))) ? 1 : -1];
typedef char gv_queue_find_file_offset[(sizeof(void *) != 4 || ((offsetof(s_gv_enumeration_queue, find_file_data) == 0x3680))) ? 1 : -1];
typedef char gv_queue_thread_id_offset[(sizeof(void *) != 4 || ((offsetof(s_gv_enumeration_queue, thread_id) == 0x3a08))) ? 1 : -1];
typedef char gv_queue_done_offset[(sizeof(void *) != 4 || ((offsetof(s_gv_enumeration_queue, done) == 0x3a0c))) ? 1 : -1];
typedef char gv_queue_size[(sizeof(void *) != 4 || ((sizeof(s_gv_enumeration_queue) == 0x3a18))) ? 1 : -1];

class c_gui_game_variant_subitem_selectable_item_datasource : public c_gui_ordered_data {
public:
    __forceinline c_gui_game_variant_subitem_selectable_item_datasource() :
        m_game_engine_type(_game_engine_type_none), m_item_count(0), m_controller_index(-1), m_enumeration_complete(false) {}
    virtual void dispose();
    virtual void update();
    virtual bool is_busy();
    virtual void get_column_names(long *const names, long *count);
    virtual bool get_integer_value(long element_handle, long value_name, long *value);
    virtual bool get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *value);
    virtual bool get_string_id_value(long element_handle, long value_name, long *value);
    virtual const c_gui_selected_item *get_gui_selected_item(long index) const;
    virtual bool initialize(long name, e_game_engine_type game_engine_type, e_controller_index controller_index);
    void begin_saved_game_files_enumeration();
protected:
    virtual long get_current_item_count_internal();
public:
    e_game_engine_type m_game_engine_type;
    c_gui_game_variant_selected_item m_entries[151];
    long m_item_count;
    long m_controller_index;
    bool m_enumeration_complete;
    s_gv_enumeration_queue m_queue;
};
typedef char gv_subitem_entries_offset[
    (sizeof(void *) != 4 || offsetof(c_gui_game_variant_subitem_selectable_item_datasource, m_entries) == 0x110) ? 1 : -1];
typedef char gv_subitem_count_offset[
    (sizeof(void *) != 4 || offsetof(c_gui_game_variant_subitem_selectable_item_datasource, m_item_count) == 0x196e0) ? 1 : -1];
typedef char gv_subitem_queue_offset[
    (sizeof(void *) != 4 || offsetof(c_gui_game_variant_subitem_selectable_item_datasource, m_queue) == 0x196f0) ? 1 : -1];


#include "interface/gui_selected_items_map.h"
class c_gui_game_variant_category_datasource : public c_gui_ordered_data {
public:
    c_gui_game_variant_category_datasource(const wchar_t *title, const wchar_t *description,
        const wchar_t *default_title, const wchar_t *default_description);
private:
    void add_game_engine(c_gui_data *gui_data, e_game_engine_type game_engine_type);
public:
    virtual bool initialize(long name);
    virtual bool get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *value);
    virtual bool get_integer_value(long element_handle, long value_name, long *value);
    virtual bool get_string_id_value(long context, long string_id, long *result);
    virtual void get_column_names(long *const column_names, long *count);
protected:
    virtual long get_current_item_count_internal();
    static long get_game_engine_sort_order(c_gui_data *datasource, long game_engine_type);
    c_static_wchar_string<48> m_title;
    c_static_wchar_string<256> m_description;
    c_static_wchar_string<48> m_default_title;
    c_static_wchar_string<256> m_default_description;
public:
    struct s_game_variant_category
    {
        long game_engine_type;
        long sort_order;
        c_static_wchar_string<48> name;
        wchar_t description[256];
        long sort_key;
        __forceinline s_game_variant_category() { description[0] = 0; }
    };
protected:
    c_static_array<s_game_variant_category, 11> m_categories;
    long m_item_count;
};

#endif
