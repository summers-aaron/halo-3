#ifndef INTERFACE_C_GUI_LIST_ITEM_WIDGET_H
#define INTERFACE_C_GUI_LIST_ITEM_WIDGET_H

#include "interface/gui_widget.h"
#include "interface/core_widget_definition.h"
#include <stddef.h>

// March's 26 methods are in c_gui_list_item_widget.cpp (asserts at lines 32, 120, 163).
// get_list_item_index and initialize are the trailing
// virtuals on this widget.
// (0x4bd5f8 loads +0x100), followed by initialize at slot 33.
struct s_runtime_list_item_widget_definition : s_runtime_core_widget_definition
{
    long item_label; // Label identifier copied from the widget definition during initialize.
    __forceinline s_runtime_list_item_widget_definition() : item_label(0) {}
};

class c_gui_list_item_widget : public c_gui_widget
{
public:
    c_gui_list_item_widget();
    virtual ~c_gui_list_item_widget() {}
    virtual long get_element_handle(); // slot 5
    virtual s_runtime_core_widget_definition *get_core_definition() { return &m_definition; } // slot 9
    virtual bool should_render(bool *render_with_bounds); // slot 12
    virtual bool can_receive_focus(); // slot 14
    virtual c_gui_text_widget *create_text_widget(const s_runtime_text_widget_definition *definition);
    virtual c_gui_bitmap_widget *create_bitmap_widget(const s_runtime_bitmap_widget_definition *definition);
    virtual c_gui_group_widget *create_group_widget(const s_group_widget_definition *definition);
    virtual c_gui_button_key_widget *create_button_key_widget(const s_button_key_definition *definition);
    virtual c_gui_list_widget *create_list_widget(const s_list_widget_block *definition);
    virtual c_gui_list_item_widget *create_list_item_widget(const s_list_item_widget_block *definition);
    virtual c_gui_model_widget *create_model_widget(const s_model_widget_block *definition);
    virtual bool handle_widget_back_out(); // slot 27
    virtual bool handle_widget_selected(); // slot 28
    virtual long get_list_item_index() { return m_list_item_index; } // slot 32
    virtual void initialize(const s_list_item_widget_block *definition); // slot 33

    __forceinline bool valid() { return get_element_handle() != -1; }
    __forceinline long get_animation_collection_reference_index() { return m_definition.animation_definition_index; }
    __forceinline long get_item_label() { return m_definition.item_label; }
    __forceinline void set_list_item_index(long index) { m_list_item_index = index; }
    bool invokes_submenu(long *submenu_name);
    c_gui_list_item_widget *get_sibling_list_item_widget(bool wrap, int offset);

    s_runtime_list_item_widget_definition m_definition;
    long m_list_item_index; // -1: not indexed
};

typedef char runtime_list_item_widget_definition_size[(sizeof(s_runtime_list_item_widget_definition) == 0x38) ? 1 : -1];
typedef char gui_list_item_widget_size[(sizeof(void *) != 4 || ((sizeof(c_gui_list_item_widget) == 0x108))) ? 1 : -1];
typedef char gui_list_item_widget_definition_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_list_item_widget, m_definition) == 0xc8))) ? 1 : -1];
typedef char gui_list_item_widget_index_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_list_item_widget, m_list_item_index) == 0x100))) ? 1 : -1];

#endif // INTERFACE_C_GUI_LIST_ITEM_WIDGET_H
