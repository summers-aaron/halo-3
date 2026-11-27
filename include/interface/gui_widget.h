#ifndef INTERFACE_GUI_WIDGET_H
#define INTERFACE_GUI_WIDGET_H

#include <stddef.h>
#include "interface/gui_widget_types.h"
#include "interface/gui_animation_transform.h"

template<long k_maximum_count> class c_static_wchar_string;
struct s_tag_block;
struct s_core_widget_definition;
struct s_runtime_core_widget_definition;
struct s_runtime_text_widget_definition;
struct s_runtime_bitmap_widget_definition;
struct s_text_widget_block;
struct s_bitmap_widget_block;
struct s_group_widget_definition;
struct s_button_key_definition;
struct s_list_widget_block;
struct s_list_item_widget_block;
struct s_model_widget_block;
struct s_gui_skin_definition;
struct s_widget_animation_definition;
class c_font_cache;
class c_controller_input_message;
class c_gui_screen_widget;
class c_gui_text_widget;
class c_gui_bitmap_widget;
class c_gui_group_widget;
class c_gui_button_key_widget;
class c_gui_list_widget;
class c_gui_list_item_widget;
class c_gui_model_widget;

// match: 0x48b028: preserve the owner's existing setter helper boundary; direct stores change its schedule.
static __forceinline real_rgb_color *c_gui_widget_set_real_rgb_color(real_rgb_color *color, float red, float green, float blue)
{
    color->red = red;
    color->green = green;
    color->blue = blue;
    return color;
}
// match: 0x48b028: preserve the owner's existing color setter boundary.
static __forceinline real_argb_color *c_gui_widget_set_real_argb_color(real_argb_color *color, float alpha, float red, float green, float blue)
{
    color->alpha = alpha;
    c_gui_widget_set_real_rgb_color(&color->rgb, red, green, blue);
    return color;
}
// match: 0x48b028: preserve the owner's existing point setter boundary.
static __forceinline real_point3d *c_gui_widget_set_real_point3d(real_point3d *point, float x, float y, float z)
{
    point->x = x;
    point->y = y;
    point->z = z;
    return point;
}
// match: 0x48b028: preserve the owner's existing vector setter boundary.
static __forceinline real_vector2d *c_gui_widget_set_real_vector2d(real_vector2d *vector, float i, float j)
{
    vector->i = i;
    vector->j = j;
    return vector;
}
// match: 0x48b028: preserve the owner's existing point setter boundary.
static __forceinline real_point2d *c_gui_widget_set_real_point2d(real_point2d *point, float x, float y)
{
    point->x = x;
    point->y = y;
    return point;
}
class c_gui_widget;
// The robust-pointer helpers of the widget tree (0x482a48 reads, repairing a torn value; 0x48d4c0 initializes;
// both defined in c_gui_widget.cpp). Names are the tree's: March 2007 has plain pointers here.
extern "C" c_gui_widget *robust_pointer_get_repaired_value(c_robust_pointer<c_gui_widget> *pointer);
extern "C" void robust_pointer_set_value(c_robust_pointer<c_gui_widget> *pointer, c_gui_widget *value);

// A node in the widget tree, with animation state and redundant links to its relatives.
class c_gui_widget {
public:
    c_gui_widget(e_gui_widget_type type);
protected:
    virtual e_animation_state get_ambient_state(); // slot 0
public:
    virtual ~c_gui_widget() { delete_all_children(); } // slot 1
    virtual void initialize(); // slot 2
    virtual void post_initialize(); // slot 3
    virtual void dispose(); // slot 4
    virtual long get_element_handle(); // slot 5
    virtual long get_datasource_index(); // slot 6
    virtual bool get_enabled() const; // slot 7
    virtual void set_enabled(bool enabled); // slot 8
    virtual s_runtime_core_widget_definition *get_core_definition() = 0; // slot 9
    virtual real_rectangle2d *get_current_bounds(real_rectangle2d *out); // slot 10
    virtual real_rectangle2d *get_container_current_bounds(real_rectangle2d *out); // slot 11
    // match: 0x48bd90: the inline definition in c_gui_widget.cpp also supplies the vtable entry.
    virtual __forceinline bool should_render(bool *render_with_bounds);
    virtual real_argb_color *get_cumulative_color_tint(real_argb_color *tint); // slot 13
    virtual bool can_receive_focus() { return false; } // slot 14
    virtual bool within_focus_chain(); // slot 15
    virtual c_gui_text_widget *create_text_widget(const s_runtime_text_widget_definition *definition); // slot 16
    virtual c_gui_bitmap_widget *create_bitmap_widget(const s_runtime_bitmap_widget_definition *definition); // slot 17
    virtual c_gui_group_widget *create_group_widget(const s_group_widget_definition *definition); // slot 18
    virtual c_gui_button_key_widget *create_button_key_widget(const s_button_key_definition *definition); // slot 19
    virtual c_gui_list_widget *create_list_widget(const s_list_widget_block *definition); // slot 20
    virtual c_gui_list_item_widget *create_list_item_widget(const s_list_item_widget_block *definition); // slot 21
    virtual c_gui_model_widget *create_model_widget(const s_model_widget_block *definition); // slot 22
    virtual void update(unsigned long current_milliseconds); // slot 23
    virtual void update_render_state(unsigned long current_milliseconds); // slot 24
    // match: 0x48b028: shared inline baseline initialization also supplies the vtable entry.
    virtual __forceinline void set_animated_state_baseline(s_animation_transform *baseline)
    {
        c_gui_widget_set_real_argb_color(&baseline->color, 1.0f, 1.0f, 1.0f, 1.0f);
        c_gui_widget_set_real_point3d(&baseline->position, 0.0f, 0.0f, 0.0f);
        c_gui_widget_set_real_vector2d(&baseline->scale, 1.0f, 1.0f);
        c_gui_widget_set_real_point2d(&baseline->rotation_origin, 0.0f, 0.0f);
        baseline->rotation_angle = 0.0f;
        baseline->rotation_sine = 0.0f;
        baseline->rotation_cosine = 1.0f;
        c_gui_widget_set_real_point2d(&baseline->rotation_pivot, 0.0f, 0.0f);
        c_gui_widget_set_real_point2d(&baseline->texture_offset, 0.0f, 0.0f);
        baseline->bitmap_sequence_index = 0;
        baseline->bitmap_frame_index = 0;
        baseline->font = 0;
    }
    virtual void assemble_render_data(s_gui_widget_render_data *render_data, const rectangle2d *window_bounds,
        e_controller_index local_controller_index, bool a, bool b, bool c); // slot 26
    virtual bool handle_widget_back_out() { return false; } // slot 27
    virtual bool handle_widget_selected() { return false; } // slot 28
    virtual bool handle_tab(const c_controller_input_message *message); // slot 29
    virtual bool handle_alt_tab(const c_controller_input_message *message); // slot 30
    virtual bool get_string_by_string_id(long string_identifier, c_static_wchar_string<1024> *buffer); // slot 31

    static void add_definition_fields(e_gui_widget_type widget_type, const s_core_widget_definition *source_definition,
        s_runtime_core_widget_definition *dest_definition, const real_rectangle2d *parent_bounds, bool require_nonzero_bounds);
    static void apply_relative_positioning(real_rectangle2d *bounds, const real_rectangle2d *parent_bounds);
    c_gui_text_widget *create_and_add_child_text_widget(const s_text_widget_block *text_widget_block);
    void create_and_add_children_from_skin_definition(const s_gui_skin_definition *skin_definition, long animation_index, short font_index);
    c_gui_bitmap_widget *create_and_add_child_bitmap_widget(const s_bitmap_widget_block *bitmap_widget_block);
    void create_and_add_child_list_item_widgets(const s_tag_block *list_items_block, long gui_skin_tag_index);

    c_gui_widget *get_child_widget(e_gui_widget_type type, long name);
    __forceinline c_gui_text_widget *get_child_text_widget(long name) { return (c_gui_text_widget *)get_child_widget(_gui_text_widget, name); }
    __forceinline c_gui_bitmap_widget *get_child_bitmap_widget(long name) { return (c_gui_bitmap_widget *)get_child_widget(_gui_bitmap_widget, name); }
    __forceinline c_gui_group_widget *get_child_group_widget(long name) { return (c_gui_group_widget *)get_child_widget(_gui_group_widget, name); }
    __forceinline c_gui_button_key_widget *get_child_button_key_widget(long name)
    {
        return (c_gui_button_key_widget *)get_child_widget(_gui_button_key_widget, name);
    }
    __forceinline c_gui_list_widget *get_child_list_widget(long name) { return (c_gui_list_widget *)get_child_widget(_gui_list_widget, name); }
    __forceinline c_gui_list_item_widget *get_child_list_item_widget(long name)
    {
        return (c_gui_list_item_widget *)get_child_widget(_gui_list_item_widget, name);
    }
    __forceinline c_gui_model_widget *get_child_model_widget(long name) { return (c_gui_model_widget *)get_child_widget(_gui_model_widget, name); }
    real_rectangle2d *get_authored_bounds(real_rectangle2d *out);
    gui_real_rectangle2d *get_unprojected_bounds(gui_real_rectangle2d *bounds, bool apply_translation, bool apply_scale, bool apply_rotation);
    const s_widget_animation_definition *get_widget_animation_definition(e_animation_state state);
    c_gui_screen_widget *get_parent_screen();
    void set_visible(bool visible);
    __forceinline void set_child_visible(e_gui_widget_type type, long name, bool visible)
    {
        c_gui_widget *child = get_child_widget(type, name);
        if (child)
            child->set_visible(visible);
    }
    __forceinline void set_child_enabled(e_gui_widget_type type, long name, bool enabled)
    {
        c_gui_widget *child = get_child_widget(type, name);
        if (child)
            child->set_enabled(enabled);
    }
    void set_child_bitmap_sprite_frame(long name, long frame);
    void update_animation(unsigned long current_milliseconds);
    void set_full_animation_state(const s_animation_transform *state, bool recursive);
    void start_animation(e_animation_state animation_state, bool recursive);
    void start_animation_at_time(e_animation_state animation_state, unsigned long milliseconds, bool recursive);
    __forceinline bool is_animation_active(e_animation_state animation_state)
    {
        return (m_animated_state.animation_state_flags & (1i64 << animation_state)) != 0;
    }
    __forceinline s_animation_transform *get_animated_state() { return &m_animated_state; }
    __forceinline const s_animation_transform *get_animated_state() const { return &m_animated_state; }
    const bool can_be_disposed();
    const bool can_all_children_be_disposed();
    void delete_all_children();
    c_gui_widget *get_deepest_widget_that_can_receive_focus();
    __forceinline c_gui_widget *get_parent() { return robust_pointer_get_repaired_value(&m_parent); }
    __forceinline c_gui_widget *get_children() { return robust_pointer_get_repaired_value(&m_children); }
    __forceinline c_gui_widget *get_next() { return robust_pointer_get_repaired_value(&m_next); }
    __forceinline c_gui_widget *get_previous() { return robust_pointer_get_repaired_value(&m_previous); }
    __forceinline void set_parent(c_gui_widget *widget) { robust_pointer_set_value(&m_parent, widget); }
    __forceinline void set_children(c_gui_widget *widget) { robust_pointer_set_value(&m_children, widget); }
    __forceinline void set_next(c_gui_widget *widget) { robust_pointer_set_value(&m_next, widget); }
    __forceinline void set_previous(c_gui_widget *widget) { robust_pointer_set_value(&m_previous, widget); }
    e_controller_index get_any_responding_controller() const;
    e_controller_index get_driving_controller() const;
    bool controller_can_drive(e_controller_index controller_index);
    __forceinline long get_name() const { return m_name; }
    __forceinline e_gui_widget_type get_type() const { return m_type; }
    __forceinline bool get_visible() const { return m_visible; }
    __forceinline void modulate_tint_color(const real_argb_color *color) {
        m_animated_state.color.alpha *= color->alpha;
        m_animated_state.color.rgb.red *= color->red;
        m_animated_state.color.rgb.green *= color->green;
        m_animated_state.color.rgb.blue *= color->blue;
    }

private:
    unsigned long get_animation_state_timer(e_animation_state state);
    void animate(unsigned long current_milliseconds);

protected:
    void animate_recursively(unsigned long current_milliseconds);
    __forceinline long get_controller_mask() const { return (long)(m_controller_mask & 15); }

public:
    union {
        e_gui_widget_type m_type;                   // (after 4B vfptr + 4B 8-byte alignment pad)
        e_gui_widget_type type;
    };
    bool m_visible;
    bool m_enabled;
    bool m_animation_looping;
    unsigned char m_unknown;
    c_robust_pointer<c_gui_widget> m_parent;
    c_robust_pointer<c_gui_widget> m_children;
    c_robust_pointer<c_gui_widget> m_next;
    c_robust_pointer<c_gui_widget> m_previous;
    union {
        long m_name;
        long name;
    };
    unsigned __int64 m_controller_mask;
    s_animation_transform m_animated_state;
    unsigned long m_last_animated_milliseconds;
};

typedef char gui_widget_size_base[(sizeof(void *) != 4 || ((sizeof(c_gui_widget) == 0xc8))) ? 1 : -1];
// The vfptr and the controller mask require eight-byte member alignment.
typedef char gui_widget_type_offset[(offsetof(c_gui_widget, m_type) == 0x8) ? 1 : -1];
typedef char gui_widget_visible_offset[(offsetof(c_gui_widget, m_visible) == 0xc) ? 1 : -1];
typedef char gui_widget_parent_offset[(offsetof(c_gui_widget, m_parent) == 0x10) ? 1 : -1];
typedef char gui_widget_children_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_widget, m_children) == 0x1c))) ? 1 : -1];
typedef char gui_widget_next_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_widget, m_next) == 0x28))) ? 1 : -1];
typedef char gui_widget_previous_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_widget, m_previous) == 0x34))) ? 1 : -1];
typedef char gui_widget_name_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_widget, m_name) == 0x40))) ? 1 : -1];
typedef char gui_widget_controller_mask_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_widget, m_controller_mask) == 0x48))) ? 1 : -1];
typedef char gui_widget_animated_state_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_widget, m_animated_state) == 0x50))) ? 1 : -1];
typedef char gui_widget_last_animated_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_widget, m_last_animated_milliseconds) == 0xc0))) ? 1 : -1];

// Parent traversal helpers defined in c_gui_widget.cpp.
extern "C" c_gui_widget *gui_widget_get_parent_group_widget(c_gui_widget *widget);

#endif // INTERFACE_GUI_WIDGET_H
