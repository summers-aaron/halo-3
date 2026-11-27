#ifndef INTERFACE_C_GUI_TEXT_WIDGET_H
#define INTERFACE_C_GUI_TEXT_WIDGET_H

// Text widget base class with shared text source and render state.
// March c_gui_text_widget methods and c_gui_sized_text_widget pure overrides establish slots 32-37.
#include "interface/gui_widget.h"
#include "interface/c_gui_list_item_widget.h"
#include "interface/core_widget_definition.h"
#include "interface/runtime_text_widget_definition.h"

class c_user_interface_text;
class c_font_cache;
struct s_text_widget_definition;
struct s_text_widget_block;
extern "C" c_gui_list_item_widget *gui_widget_get_parent_list_item_widget(c_gui_widget *widget);

class c_gui_text_widget : public c_gui_widget
{
public:
    struct s_text_source_data
    {
        long type;
        long source_id;
        long string_id;
        __forceinline s_text_source_data() : type(-1), source_id(-1), string_id(-1) {}
    };

    c_gui_text_widget();
    virtual ~c_gui_text_widget() {}
    virtual s_runtime_core_widget_definition *get_core_definition() { return &definition.core; }
    virtual bool within_focus_chain()
    {
        c_gui_widget *list_item = gui_widget_get_parent_list_item_widget(this);
        if (list_item != 0)
            return list_item->within_focus_chain();
        return c_gui_widget::within_focus_chain();
    }
    virtual void update(unsigned long current_milliseconds);

    s_text_source_data text_source;
    s_runtime_text_widget_definition definition;
    virtual real_argb_color *get_cumulative_color_tint(real_argb_color *color);
    virtual c_user_interface_text *get_text_internal(void) = 0;
    virtual unsigned int get_text_buffer_size() const = 0;
    virtual void initialize(const struct s_text_widget_block *text_widget_block);
    virtual void set_text(const wchar_t *text);
    virtual void set_text_from_string_id(c_gui_screen_widget *gui_screen_widget, long string_id);
    virtual const wchar_t *get_text(void);
    bool get_text_bounds(c_font_cache *font_cache, rectangle2d *rectangle);
    virtual void assemble_render_data(s_gui_widget_render_data *render_data, const rectangle2d *window_bounds,
        e_controller_index local_controller_index, bool a, bool b, bool c);
    virtual void set_animated_state_baseline(s_animation_transform *baseline);
    virtual void update_render_state(unsigned long current_milliseconds);
    static void add_definition_fields(const s_text_widget_definition *source_definition, s_runtime_text_widget_definition *dest_definition,
        real_rectangle2d *positioning_bounds, bool replace);
private:
    void compute_layout_bounds(const rectangle2d *rectangle, real_point2d origin, float x, float y, float z, float scale_x, float scale_y,
        real_point2d *out_origin, real_rectangle2d *out_flat, real_rectangle2d *out_rectangle, float *out_scale);
    void set_text_internal(const wchar_t *text, const s_text_source_data *source_data, bool force);
    bool set_string_from_string_id(c_static_wchar_string<1024> *static_wchar_string, c_gui_widget *gui_widget, long string_id, bool force);
};

typedef char gui_text_widget_size[(sizeof(void *) != 4 || ((sizeof(c_gui_text_widget) == 0x128))) ? 1 : -1];

#endif
