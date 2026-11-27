#ifndef INTERFACE_GUI_WIDGET_ENUMS_H
#define INTERFACE_GUI_WIDGET_ENUMS_H

enum e_gui_widget_type {
    _gui_widget_type_none = -1,
    _gui_text_widget = 0,
    _gui_bitmap_widget = 1,
    _gui_model_widget = 2,
    _gui_group_widget = 3,
    _gui_button_key_widget = 4,
    _gui_list_item_widget = 5,
    _gui_slider_widget = 6,
    _gui_list_widget = 7,
    _gui_screen_widget = 8,
    k_number_of_gui_widget_types = 9
};

// Names from the March 2007 debug build's animation state string list.
enum e_animation_state {
    _animation_state_focused_ambient,
    _animation_state_unfocused_ambient,
    _animation_state_focused_disabled_ambient,
    _animation_state_unfocused_disabled_ambient,
    _animation_state_focused_alternate_ambient,
    _animation_state_unfocused_alternate_ambient,
    _animation_state_transition_from_screen,
    _animation_state_transition_to_screen,
    _animation_state_transition_back_from_screen,
    _animation_state_transition_back_to_screen,
    _animation_state_cycle_display_panes_previous,
    _animation_state_cycle_display_panes_next,
    _animation_state_list_display_group_transition_in,
    _animation_state_list_display_group_transition_out,
    _animation_state_previous_control_received_focus,
    _animation_state_previous_control_lost_focus,
    _animation_state_next_control_received_focus,
    _animation_state_next_control_lost_focus,
    _animation_state_indicator_ambient_additional_items,
    _animation_state_indicator_ambient_no_additional_items,
    _animation_state_indicator_activated_additional_items,
    _animation_state_indicator_activated_no_additional_items,
    _animation_state_load_submenu_focused,
    _animation_state_load_submenu_unfocused,
    _animation_state_unload_submenu_focused,
    _animation_state_unload_submenu_unfocused,
    _animation_state_load_as_submenu,
    _animation_state_unload_as_submenu,
    _animation_state_child_submenu_ambient_focused,
    _animation_state_child_submenu_ambient_unfocused,
    _animation_state_custom0,
    _animation_state_custom1,
    _animation_state_custom_transition_in0,
    _animation_state_custom_transition_out0,
    _animation_state_custom_transition_in1,
    _animation_state_custom_transition_out1,
    k_number_of_animation_states
};

enum e_widget_anchoring {
    _widget_anchoring_none = 0
};

enum e_authored_ui_aspect_ratio {
    _authored_ui_aspect_ratio_four_by_three = 0,
    _authored_ui_aspect_ratio_sixteen_by_nine = 1,
    k_authored_ui_aspect_ratio_count = 2
};

typedef char gui_wtypes_size_type[(sizeof(e_gui_widget_type) == 4) ? 1 : -1];
typedef char gui_wtypes_size_anim[(sizeof(e_animation_state) == 4) ? 1 : -1];
typedef char gui_wtypes_size_anchor[(sizeof(e_widget_anchoring) == 4) ? 1 : -1];
typedef char gui_wtypes_size_aspect[(sizeof(e_authored_ui_aspect_ratio) == 4) ? 1 : -1];

#endif // INTERFACE_GUI_WIDGET_ENUMS_H
