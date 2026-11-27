#ifndef INTERFACE_GUI_SELECTED_ITEMS_SAVED_FILM_H
#define INTERFACE_GUI_SELECTED_ITEMS_SAVED_FILM_H

// Functions defined in tu/interfacelibxbox/gui_selected_items_saved_film.cpp.

#include "interface/gui_selected_item_metadata.h"
#include "tag_files/file_reference.h"
#include "saved_games/saved_game_collect_files.h"
#include "interface/gui_data.h"
#include "interface/gui_simple_selected_item.h"
#include "memory/static_arrays.h"

// Film categories, named after the title string id each category list entry shows.
enum e_saved_film_category
{
    _saved_film_category_none = 0,
    _saved_film_category_recent_films = 1,
    _saved_film_category_film_clip = 2,
    _saved_film_category_campaign_film = 3,
    _saved_film_category_multiplayer_film = 4,
    _saved_film_category_editor_film = 5,
};

// String ids of the values the saved-film datasources answer.
enum e_saved_film_string_id
{
    _saved_film_string_category = 254,
    _saved_film_string_locked = 0x1fa,
    _saved_film_string_title = 0x29d,
    _saved_film_string_description = 0x29e,
    _saved_film_string_game_engine_index = 0x1000e,
    _saved_film_string_map_id = 0x10029,
    _saved_film_string_corrupt_file = 0x102ff,
    _saved_film_string_item_saved = 0x1030a,
    _saved_film_string_image_index = 0x10327,
    _saved_film_string_special_status = 0x103d7,
    _saved_film_string_map_image_type = 0x105b8,
    _saved_film_string_new = 0x105bb,
};

class c_gui_saved_film_selected_item : public c_gui_simple_selected_item
{
public:
    virtual bool get_file_path(char *buffer, long size) const;
    c_gui_saved_film_selected_item();
    c_gui_saved_film_selected_item(const s_saved_game_item_metadata_packed *metadata, e_saved_film_category category,
        e_gui_selected_item_location location, long controller, const s_file_reference *reference, long,
        bool corrupt, bool created);
    c_gui_saved_film_selected_item &operator=(const c_gui_saved_film_selected_item &other);

    long controller_index;
    s_file_reference file_reference;
    unsigned __int64 m_sort_key;
    e_saved_film_category m_saved_film_category;
    e_saved_film_category get_saved_film_category() const { return m_saved_film_category; }
    bool get_variants(void *game_options, class c_game_variant *game_variant, class c_map_variant *map_variant) const;
};
struct c_gui_saved_film_subitem_datasource;


extern "C" void gui_saved_film_screen_load_from_catalogue(c_gui_saved_film_subitem_datasource *screen);
extern "C" int gui_saved_film_item_compare(const c_gui_saved_film_selected_item *item_a, const c_gui_saved_film_selected_item *item_b);
extern "C" void gui_saved_film_screen_load_from_queue(c_gui_saved_film_subitem_datasource *screen);

#include <ppcintrinsics.h>
class gui_selected_items_saved_film_synchronized_long
{
public:
    volatile long value;
    gui_selected_items_saved_film_synchronized_long() {}
    __forceinline gui_selected_items_saved_film_synchronized_long(long initial_value) { value = initial_value; }
    long peek() const { long result = value; __lwsync(); return result; }
};
struct s_async_state
{
    s_saved_game_queue queue;
    __forceinline s_async_state() : queue(-1, -1) {}
    ~s_async_state(); // cancels the enumeration and waits for it
};
struct c_gui_saved_film_subitem_datasource : c_gui_ordered_data {
 virtual bool initialize(long name);
 virtual void dispose();
 virtual bool is_busy();
 virtual bool get_integer_value(long element_handle, long value_name, long *value);
 virtual bool get_text_value(long index, long string_id, c_static_wchar_string<1024> *string);
 virtual bool get_string_id_value(long element_handle, long string_id, long *result);
 virtual void update();
 virtual void get_column_names(long *const column_names, long *column_count);
 virtual const c_gui_selected_item *get_gui_selected_item(long index) const;
protected:
 virtual long get_current_item_count_internal();
public:
 int unused;
 c_gui_saved_film_selected_item items[151]; int item_count,category,controller; bool busy; s_async_state async;
 c_gui_saved_film_subitem_datasource(int,int);

};
struct s_saved_film_category
{
    wchar_t title[48];
    wchar_t description[256];
    e_saved_film_category category;
    long mode;
};
class c_gui_saved_film_category_datasource : public c_gui_ordered_data
{
public:
    __forceinline c_gui_saved_film_category_datasource(long screen_index) : c_gui_ordered_data()
    {
        m_screen_index = screen_index;
        m_category_count = 0;
    }
    virtual bool get_text_value(long index, long value_name, c_static_wchar_string<1024> *value);
    virtual bool initialize(long name);
    virtual void get_column_names(long *const names, long *count);
    virtual bool get_integer_value(long element_handle, long value_name, long *value);
    virtual bool get_string_id_value(long element_handle, long value_name, long *value);
protected:
    virtual long get_current_item_count_internal();
private:
    void add_category(long title_id, long description_id, e_saved_film_category category, long mode);
public:
    c_static_array<s_saved_film_category, 7> m_categories;
    long m_category_count;
    long m_screen_index;
};
#endif // INTERFACE_GUI_SELECTED_ITEMS_SAVED_FILM_H
