#ifndef INTERFACE_C_GUI_WIDGET_H
#define INTERFACE_C_GUI_WIDGET_H

#include "interface/gui_widget_enums.h"

// Functions defined in tu/interfacelibxbox/c_gui_widget.cpp.

class c_function_definition;
class c_gui_list_item_widget;
class c_gui_list_widget;
class c_gui_screen_widget;
class c_gui_widget;
union real_rectangle2d;
union real_vector2d;
union rectangle2d;
struct s_animation_transform;
struct s_gui_bitmap_keyframe;
struct s_gui_texture_offset_keyframe;
struct s_gui_widget_render_data;
struct s_widget_animation_definition;

extern "C" void gui_widget_render_data(long user_index, const s_gui_widget_render_data *render_data, const rectangle2d *window_bounds, bool is_screenshot);
extern "C" c_gui_widget *gui_widget_get_parent_group_widget(c_gui_widget *widget);
#include "interface/gui_widget_hierarchy.h"
extern "C" c_gui_list_item_widget *gui_widget_get_parent_list_item_widget(c_gui_widget *widget);
extern "C" void gui_widget_append_child(c_gui_widget *parent, c_gui_widget *child);
extern "C" bool gui_widget_is_descendant_or_self(c_gui_widget *object, c_gui_widget *target);
#include "interface/gui_widget_navigation.h"
void scale_and_position_rectangle_for_display_resolution(e_gui_widget_type widget_type, real_rectangle2d *rectangle, e_widget_anchoring anchoring);
extern "C" void *robust_pointer_resolve_majority_value(void **values);
extern "C" void gui_screen_widget_reload_and_update_datasources(c_gui_screen_widget *p0);
extern "C" void gui_widget_apply_animation_state(
    c_gui_widget *widget, e_animation_state animation_state, unsigned long start_time, unsigned long current_milliseconds,
    const s_widget_animation_definition *animation_definition, s_animation_transform *transform,
    real_vector2d *aspect_ratio_scale, bool initialize, bool combine_states, bool *finished);


extern "C" void gui_widget_animate_rotation_origin(s_animation_transform *transform, float time, bool combine,
    const s_gui_texture_offset_keyframe *start, const s_gui_texture_offset_keyframe *end, const c_function_definition *function);
extern "C" void gui_widget_animate_bitmap(s_animation_transform *transform, float time, bool combine,
    const s_gui_bitmap_keyframe *start, const s_gui_bitmap_keyframe *end, const c_function_definition *function);

template<typename t_type> class c_robust_pointer;
template<typename t_type> bool c_gui_widget_robust_pointer_valid(c_robust_pointer<t_type> *pointer);

#endif // INTERFACE_C_GUI_WIDGET_H
