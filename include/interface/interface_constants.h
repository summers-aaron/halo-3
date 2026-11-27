#ifndef INTERFACE_INTERFACE_CONSTANTS_H
#define INTERFACE_INTERFACE_CONSTANTS_H

// Functions defined in tu/interfacelibxbox/interface_constants.cpp that other units call.

union rectangle2d;
union point2d;
union real_vector2d;
enum e_authored_ui_aspect_ratio;

void interface_get_current_window_settings(rectangle2d *bounds, rectangle2d *, rectangle2d *scaled_bounds, rectangle2d *);
void calculate_aspect_ratio_scaling(void);
void calculate_authoring_to_target_dimensions_scaling(void);
e_authored_ui_aspect_ratio interface_get_closest_authored_ui_aspect_ratio(void);
real_vector2d interface_get_authoring_aspect_conversion_scaling(e_authored_ui_aspect_ratio aspect_ratio);
extern "C" {
extern real_vector2d g_interface_authoring_to_target_dimensions_scaling;
extern real_vector2d g_interface_aspect_ratio_scaling;
extern long g_interface_display_width;
extern long g_interface_display_height;
extern long g_interface_render_width;
extern long g_interface_render_height;
extern int g_interface_authoring_target_width;
extern int g_interface_authoring_target_height;
extern point2d k_authored_ui_dimensions[2];
}

#endif // INTERFACE_INTERFACE_CONSTANTS_H
