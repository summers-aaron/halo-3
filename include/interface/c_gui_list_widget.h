#ifndef INTERFACE_C_GUI_LIST_WIDGET_H
#define INTERFACE_C_GUI_LIST_WIDGET_H

// The list widget (Bungie's interface\c_gui_list_widget.cpp: the March 2007 debug build asserts there at lines
// 110-1043). Method list, const qualifiers and access from the March debug map (55 methods); offsets from March's
// constructor and accessors. The vtable at
// 0x82055fc8 has 33 slots: c_gui_widget's 32 plus initialize(const s_list_widget_block *).
//
// Interface notes:
// - There is no get_name returning the definition name, so
//   it is not declared here (c_gui_widget::get_name is used).
// - update_render_state takes the milliseconds only, as
//   c_gui_widget's does (see gui_widget.h); March's took (c_font_cache *, unsigned long).
// - offset_horizontal_list_item_indicators constructs its font cache locally.
// Field names marked "use:" are named from what the code does with them; March has no names for data members.

#include "math/real_math_types.h"
#include "tag_files/tag_types.h"
#include "interface/core_widget_definition.h"
#include "interface/gui_widget.h"

class c_gui_data;
class c_gui_bitmap_widget;
class c_gui_list_item_widget;
class c_font_cache;
class c_controller_input_message;
struct s_list_widget_block;
struct s_list_widget_definition;
enum e_event_type;

// March: inline, c_gui_list_widget.obj 0x8369fba0.
struct s_runtime_list_widget_definition : s_runtime_core_widget_definition {
    long datasource;                        // use: copied from the tag definition
    long skin_index;                        // use: tag index, -1 when none
    long rows;                              // use: rows of a grid list (0 or 1: a plain list)
    s_tag_block list_items;                 // use: passed to create_and_add_child_list_item_widgets
    long more_items_preceeding_bitmap_index; // use: tag index of the "more items above" bitmap widget
    long more_items_following_bitmap_index;  // use: tag index of the "more items below" bitmap widget

    __forceinline s_runtime_list_widget_definition()
        : datasource(0), skin_index(-1), rows(0), list_items(), more_items_preceeding_bitmap_index(-1),
          more_items_following_bitmap_index(-1) {
        list_items.count = 0;
        list_items.address = 0;
    }
};

typedef char runtime_list_widget_definition_size[(sizeof(s_runtime_list_widget_definition) == 0x54) ? 1 : -1];

class c_gui_list_widget : public c_gui_widget {
public:
    enum e_list_scroll_direction {
        _list_scroll_direction_up = 1,
        _list_scroll_direction_left = 2,
        _list_scroll_direction_down = 3,
        _list_scroll_direction_right = 4
    };

    c_gui_list_widget();
    virtual ~c_gui_list_widget() {}
    virtual void initialize(const s_list_widget_block *list_widget_block);
    virtual void post_initialize();
    // Returns the embedded runtime definition.
    virtual s_runtime_core_widget_definition *get_core_definition() { return &m_definition; }
    virtual void update(unsigned long current_milliseconds);
    virtual void update_render_state(unsigned long current_milliseconds);
    virtual bool handle_tab(const c_controller_input_message *message);
    virtual long get_datasource_index();

    void add_definition_fields(const s_list_widget_definition *definition, bool require_nonzero_bounds);
    void create_and_add_additional_items_indicators_bitmaps();
    long get_scroll_position() { return m_scroll_position; } // March: out of line in c_gui_list_widget.cpp
    void set_scroll_position(long scroll_position);
    long get_item_count(bool include_unindexed);
    long get_selectable_item_count();
    bool list_has_more_elements_preceeding();
    bool list_has_more_elements_following();
    c_gui_data *get_data();
    long get_focused_item_index() { return m_focused_item_index; } // March: out of line in c_gui_list_widget.cpp
    bool set_focused_item_index(long focused_item_index, bool transfer_focus);
    long get_focused_element_handle();
    bool set_focused_element_handle(long element_handle, bool transfer_focus);
    bool set_focused_datasource_element_from_value(long value_name, long value, bool transfer_focus);
    long get_element_handle_from_list_item_index(long list_item_index);
    bool invoke_submenu(long submenu_name, long datasource_name, bool transfer_focus);
    bool invoke_submenu(const s_list_widget_block *submenu_definition, long datasource_name, bool transfer_focus);
    void close_active_submenu(c_gui_list_widget *submenu);
    void dispose_submenu(c_gui_list_widget *submenu);
    void get_child_list_item_widget_bounds(real_rectangle2d *bounds);
    c_gui_list_item_widget *try_and_get_child_list_item_widget_by_element_handle(long element_handle);
    void get_child_list_item_text_bounds(c_font_cache *font_cache, rectangle2d *bounds, bool *all_text_precached);

    long get_datasource_name() { return m_datasource_name; }
    c_gui_bitmap_widget *get_more_items_preceeding_indicator() { return m_more_items_preceeding_indicator; }
    c_gui_bitmap_widget *get_more_items_following_indicator() { return m_more_items_following_indicator; }
    void set_selectable_item_cap_count(long count) { m_selectable_item_cap_count = count; }
    bool list_selection_visible_without_focus() const { return (m_definition.flags & 0x40) != 0; }
    bool list_wraps() const { return (m_definition.flags & 0x20) != 0; }
    bool scrolls_horizontally() { return (m_definition.flags & 0x10) != 0; }
    bool scrolls_vertically() { return !scrolls_horizontally(); }
    bool non_interactive() { return (m_definition.flags & 0x80) != 0; }
    bool is_submenu() const { return m_is_submenu; }
    void mark_as_submenu(bool is_submenu) { m_is_submenu = is_submenu; }
    bool is_submenu_that_needs_disposal() const { return m_is_submenu_that_needs_disposal; }
    void mark_as_submenu_that_needs_disposal(bool needs_disposal) { m_is_submenu_that_needs_disposal = needs_disposal; }
    bool has_active_submenu() const { return m_submenu_owner_list_item != 0; }
    c_gui_list_item_widget *get_submenu_owner_list_item() const { return m_submenu_owner_list_item; }

    friend class c_gui_roster_list_widget;
private:
    bool handle_grid_tab(c_gui_list_item_widget *list_item_widget, e_event_type event_type);
    bool handle_tab_to_list_item(c_gui_list_item_widget *from_list_item, c_gui_list_item_widget *to_list_item);
    bool handle_tab_direction(c_gui_list_item_widget *list_item_widget, e_list_scroll_direction direction);
    void update_indicator_bitmap_widget_animation_states();
    void offset_horizontal_list_item_indicators();
    void set_datasource_name(long datasource_name) { m_datasource_name = datasource_name; }

public:
    // Units still read some of these directly; Bungie's access for data members is not attested.
    s_runtime_list_widget_definition m_definition;
    long m_datasource_name;
    c_gui_bitmap_widget *m_more_items_preceeding_indicator;
    c_gui_bitmap_widget *m_more_items_following_indicator;
    long m_focused_item_index;
    long m_last_scroll_direction;                             // use: handle_tab_direction stores the direction, update() reads and clears it
    long m_scroll_position;
    long m_selectable_item_cap_count;                         // -1: no cap
    c_gui_list_item_widget *m_submenu_owner_list_item;
    bool m_is_submenu;
    bool m_is_submenu_that_needs_disposal;
};

typedef char gui_list_widget_size[(sizeof(void *) != 4 || ((sizeof(c_gui_list_widget) == 0x140) )) ? 1 : -1];

// Functions defined in tu/interfacelibxbox/c_gui_list_widget.cpp.


#endif // INTERFACE_C_GUI_LIST_WIDGET_H
