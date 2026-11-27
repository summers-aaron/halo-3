#ifndef INTERFACE_CHUD_CHUD_DRAW_H
#define INTERFACE_CHUD_CHUD_DRAW_H

#include "main/main_screenshot.h"

struct s_chud_runtime_widget_datum;
extern "C" void chud_draw_widget(long user_index, s_chud_runtime_widget_datum *widget, long definition_tag_index, bool text);

// Functions defined in tu/interfacelibxbox/chud_draw.cpp.

union real_rectangle2d;
struct s_widget_geometry;

void chud_widget_geometry_add_character(s_widget_geometry *geometry, const real_rectangle2d *texture_bounds, const real_rectangle2d *bounds, float scale);
bool chud_user_should_draw(long user_index);

class c_chud_point_tracker;
struct s_directional_damage;
union real_point2d;

extern "C" float chud_draw_get_string_width(long, const wchar_t *string);
extern "C" void chud_draw_get_normalized_screen_point(long user_index, real_point2d *screen_point);
extern "C" void chud_point_tracker_update_screen_position(c_chud_point_tracker *tracker, float maximum_distance);
extern "C" void chud_draw_damage_indicators_and_curvature(s_directional_damage *entries, long user_index);

union real_point3d;
union real_vector3d;
union real_vector4d;
struct real_matrix4x3;
struct s_chud_draw_widget_data;
struct s_chud_render_data;
struct s_chud_widget_render_data;

extern "C" void chud_draw_cortana_effect_background(real_rectangle2d *rect, const real_point3d *position, const real_vector3d *up,
    const real_vector3d *left, real_rectangle2d *texture_rect, void (*callback)(), long shader_index);
float chud_build_geometry_from_navpoint(long user_index, long navpoint_index, long sprite_index, float rotation,
    float scale, struct s_widget_geometry * geometry);
void chud_draw_widget_geometry(s_widget_geometry *widget_geometry, real_matrix4x3 *anchor_basis, bool p2, bool p3);
extern "C" bool chud_build_widget_render_data(s_chud_draw_widget_data *draw, s_chud_render_data *render_data, bool text);
extern "C" void chud_render_data_fetch_color_output(long user_index, short output_index, s_chud_widget_render_data *widget, real_vector4d *output);
extern "C" unsigned char chud_compute_anchor_basis(long user_index, s_chud_draw_widget_data *draw, long anchor, real_matrix4x3 *basis);

struct s_chud_animation_definition;
struct s_chud_aux_animation_data;

extern const wchar_t k_minus_sign_string[];
extern "C" bool g_chud_default_widget_init_needed;
extern "C" s_chud_widget_render_data g_chud_default_widget_render_data;
extern "C" const float k_widescreen_aspect_ratio;
extern "C" const wchar_t k_two_string_format[];
void chud_apply_widget_animation_definition(float time, s_chud_animation_definition *animation_definition, real_matrix4x3 *anchor_basis,
    s_widget_geometry *geometry, s_chud_aux_animation_data *aux_animation_data);

extern "C" bool chud_draw_directional_damage_overlay(s_directional_damage *entries, long user_index);
extern "C" bool chud_draw_damage_indicators(long user_index);
extern "C" void chud_draw_curved_hud(int user_index);

#endif // INTERFACE_CHUD_CHUD_DRAW_H
