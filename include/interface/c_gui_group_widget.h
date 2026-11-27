#ifndef INTERFACE_C_GUI_GROUP_WIDGET_H
#define INTERFACE_C_GUI_GROUP_WIDGET_H

// The group widget (Bungie's interface\c_gui_group_widget.cpp: the March 2007 debug build asserts there at lines
// 98-312; the same file holds c_gui_button_key_widget). Methods, const qualifiers and access from the March debug
// map (21 methods). The layout extends the base widget with the group's own fields;
// a group widget occupies 0x138 bytes (allocated by c_gui_widget::create_group_widget,
// 0x8248ae38; March 0x110).
//
// The vtable has 33 slots: c_gui_widget's 32 plus
// initialize(const s_group_widget_block *, bool) at slot 32. It overrides slot 9 get_core_definition (folded with
// the model widget's, 0x824e8138) and slots 16-22, the create_*_widget factories (folded with the list item
// widget's, 0x824e7890..0x824e79d0, except slot 21 create_list_item_widget, 0x824e1628, in this unit).
//
// This class declares no get_name override; the vtable has no slot for it (as
// for c_gui_list_widget), so it is not declared here (c_gui_widget::get_name is used).
// Field names marked "use:" are named from what the code does with them; March has no names for data members.

#include "math/real_math_types.h"
#include "tag_files/tag_types.h"
#include "interface/core_widget_definition.h"
#include "interface/gui_widget.h"

struct s_group_widget_block;
struct s_group_widget_definition;
struct s_list_widget_block;
struct s_list_item_widget_block;
struct s_model_widget_block;
struct s_text_widget_block;
struct s_bitmap_widget_block;
struct s_button_key_definition;
struct s_runtime_text_widget_definition;
struct s_runtime_bitmap_widget_definition;

// The runtime copy of the group's tag definition. The four child blocks are plain s_tag_block: the constructor
// zero-fills all four (value-initialised members), which a c_typed_tag_block member (not plain data to this
// compiler) would not be.
struct s_runtime_group_widget_definition : s_runtime_core_widget_definition {
    s_tag_block list_widgets;                              // s_list_widget_block
    s_tag_block text_widgets;                              // s_text_widget_block
    s_tag_block bitmap_widgets;                            // s_bitmap_widget_block
    s_tag_block model_widgets;                             // s_model_widget_block
    long string_list_index;                                // use: text_group_get_string's tag index, -1 when none

    s_runtime_group_widget_definition();
};

typedef char runtime_group_widget_definition_size[(sizeof(s_runtime_group_widget_definition) == 0x68) ? 1 : -1];

class c_gui_group_widget : public c_gui_widget {
public:
    __forceinline c_gui_group_widget() : c_gui_widget(_gui_group_widget), m_dispose_as_display_group(false) {}
    __forceinline c_gui_group_widget(e_gui_widget_type type) : c_gui_widget(type), m_dispose_as_display_group(false) {}
    virtual ~c_gui_group_widget() {}                                                                       // slot 1
    virtual s_runtime_core_widget_definition *get_core_definition() { return &m_definition; }                // slot 9
    virtual c_gui_text_widget *create_text_widget(const s_runtime_text_widget_definition *definition);       // slot 16
    virtual c_gui_bitmap_widget *create_bitmap_widget(const s_runtime_bitmap_widget_definition *definition); // slot 17
    virtual c_gui_group_widget *create_group_widget(const s_group_widget_definition *definition);           // slot 18
    virtual c_gui_button_key_widget *create_button_key_widget(const s_button_key_definition *definition);   // slot 19
    virtual c_gui_list_widget *create_list_widget(const s_list_widget_block *definition);                   // slot 20
    virtual c_gui_list_item_widget *create_list_item_widget(const s_list_item_widget_block *definition);    // slot 21
    virtual c_gui_model_widget *create_model_widget(const s_model_widget_block *definition);                // slot 22
    virtual void initialize(const s_group_widget_block *group_widget_block, bool create_children);           // slot 32

    void add_definition_fields(const s_group_widget_definition *group_widget_definition, bool create_children);
    bool contains_submenu_control_definition(long submenu_name, s_list_widget_block *submenu_definition);
    static bool group_widget_loaded_on_initialization(const s_group_widget_block *group_widget_block);
    bool get_dispose_as_display_group() { return m_dispose_as_display_group; } // March: inline
    void set_dispose_as_display_group(bool dispose_as_display_group) { m_dispose_as_display_group = dispose_as_display_group; } // March: inline

protected:
    void create_and_add_children_from_definition();

public:
    // Units still read these directly; Bungie's access for data members is not attested.
    s_runtime_group_widget_definition m_definition;
    bool m_dispose_as_display_group;                // March get/set_dispose_as_display_group; the constructor clears it
};

typedef char gui_group_widget_size[(sizeof(void *) != 4 || ((sizeof(c_gui_group_widget) == 0x138))) ? 1 : -1];

#endif // INTERFACE_C_GUI_GROUP_WIDGET_H
