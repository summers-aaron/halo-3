#ifndef INTERFACE_C_GUI_BITMAP_WIDGET_H
#define INTERFACE_C_GUI_BITMAP_WIDGET_H

// Bitmap widget: draws a bitmap sprite, emblem, or custom image with tint and blending.
#include "interface/gui_widget.h"
#include "interface/core_widget_definition.h"
#include "game/player_types.h"

struct bitmap_data;
struct s_bitmap_widget_definition;
struct s_bitmap_widget_block;

struct s_runtime_bitmap_widget_definition : s_runtime_core_widget_definition
{
    long bitmap_definition_index;
    long shader_definition_index;
    long blend_method;
    short initial_sprite_sequence;
    short initial_sprite_frame;
    long value_override_list;
    long value_identifier;
    __forceinline s_runtime_bitmap_widget_definition() : bitmap_definition_index(-1), shader_definition_index(-1), blend_method(0),
        initial_sprite_sequence(0), initial_sprite_frame(0), value_override_list(0), value_identifier(0) {}
};

struct s_gui_bitmap_widget_render_data : s_gui_widget_render_data
{
    long bitmap_definition_index;
    union
    {
        struct { short sequence_index; short frame_index; } sprite;
        s_emblem_info emblem;
        long custom_storage_index;
    };
    unsigned long tint;
    real_point2d texture_offset;
    long blend_method;
    long shader_definition_index;
};

class c_gui_bitmap_widget : public c_gui_widget
{
public:
    c_gui_bitmap_widget();
protected:
    virtual e_animation_state get_ambient_state();
public:
    virtual void initialize(const struct s_bitmap_widget_block *bitmap_widget_block);
    virtual void set_animated_state_baseline(s_animation_transform *baseline);
    virtual bool within_focus_chain();
    virtual void update_render_state(unsigned long current_milliseconds);
    virtual const struct bitmap_data *get_current_bitmap() const;
    virtual real_rectangle2d *get_current_bounds(real_rectangle2d *out);
    virtual s_runtime_core_widget_definition *get_core_definition();
    virtual void assemble_render_data(s_gui_widget_render_data *render_data, const rectangle2d *window_bounds,
        e_controller_index local_controller_index, bool a, bool b, bool c);
    static void add_definition_fields(const struct s_bitmap_widget_definition *source_definition,
        struct s_runtime_bitmap_widget_definition *dest_definition, real_rectangle2d *positioning_bounds, bool replace);
    long m_override_bitmap_index;
    long m_bitmap_override;
    long m_sequence_override;
    s_runtime_bitmap_widget_definition m_definition;
    __forceinline long get_value_override_list() { return m_definition.value_override_list; }
    __forceinline long get_value_identifier() { return m_definition.value_identifier; }
    // March 83661120 stores the frame override at +0xcc (March +0xa4); update_render_state copies it to the frame index.
    __forceinline void set_sprite_frame(long frame) { m_bitmap_override = frame; }
};

typedef char gui_bitmap_widget_size[(sizeof(void *) != 4 || ((sizeof(c_gui_bitmap_widget) == 0x120))) ? 1 : -1];
typedef char gui_bitmap_render_data_size[(sizeof(s_gui_bitmap_widget_render_data) == 0x4c) ? 1 : -1];

extern "C" void c_gui_bitmap_widget_render(const s_gui_bitmap_widget_render_data *render_data, const rectangle2d *window_bounds);

#endif
