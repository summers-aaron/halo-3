#ifndef INTERFACE_CHUD_CHUD_DRAW_TYPES_H
#define INTERFACE_CHUD_CHUD_DRAW_TYPES_H

#include "math/real_math_types.h"
#include "tag_files/tag_types.h"
#include "interface/chud/chud_point_tracker_types.h"
#include <stddef.h>

enum e_chud_render_color_output
{
    _chud_render_color_output_real_a = 9,
    _chud_render_color_output_real_b,
    _chud_render_color_output_real_c,
    _chud_render_color_output_real_d
};

enum e_chud_render_scalar_output
{
    _chud_render_scalar_output_a = 960,
    _chud_render_scalar_output_b = 961,
    _chud_render_scalar_output_c = 962,
    _chud_render_scalar_output_d = 963,
    _chud_render_scalar_output_e = 964,
    _chud_render_scalar_output_f = 965,
};

enum e_chud_draw_anchor
{
    _chud_draw_anchor_top_left = 0,
    _chud_draw_anchor_top_right = 1,
    _chud_draw_anchor_bottom_right = 2,
    _chud_draw_anchor_bottom_left = 3,
    _chud_draw_anchor_crosshair = 4,
    _chud_draw_anchor_top_center = 5,
    _chud_draw_anchor_grenade_human = 6,
    _chud_draw_anchor_grenade_plasma = 7,
    _chud_draw_anchor_grenade_brute = 8,
    _chud_draw_anchor_grenade_fire = 9,
    _chud_draw_anchor_scoreboard_friendly = 10,
    _chud_draw_anchor_scoreboard_enemy = 11,
    _chud_draw_anchor_parent = 12,
    _chud_draw_anchor_center = 13,
    _chud_draw_anchor_backpack_weapon = 14,
    _chud_draw_anchor_equipment = 15,
    _chud_draw_anchor_reticle = 16,
    _chud_draw_anchor_messaging = 22,
    _chud_draw_anchor_bottom_center = 23,
    _chud_draw_anchor_tracked_object = 24,
    _chud_draw_anchor_state_message_right = 25,
    _chud_draw_anchor_state_message_left = 26,
    _chud_draw_anchor_message_bottom_state = 27,
    _chud_draw_anchor_message_bottom_primary = 28,
    _chud_draw_anchor_scoreboard_friendly_score_offset = 29,
    _chud_draw_anchor_scoreboard_enemy_score_offset = 30,
    _chud_draw_anchor_medal = 31,
    _chud_draw_anchor_medal_slot_one = 32,
    _chud_draw_anchor_medal_slot_two = 33,
    _chud_draw_anchor_medal_slot_three = 34,
    _chud_draw_anchor_medal_slot_four = 35,
    _chud_draw_anchor_curvature_origin = 36,
};

struct s_chud_aux_animation_data
{
    float zero;
    float scalar;
    real_vector4d texture;
};

typedef char chud_aux_animation_size[(sizeof(s_chud_aux_animation_data) == 0x18) ? 1 : -1];

struct s_chud_bitmap_info
{
    long group_index;
    long bitmap_index;
    long width;
    long height;
    real_rectangle2d bounds;
    float real_width;
    float real_height;
};
struct s_chud_render_data
{
    long vertex_shader_tag_index;
    long pixel_shader_tag_index;
    real_vector4d chud_color_outputs[6];        // pixel constants 24..29
    real_vector4d chud_scalar_output_abcd;      // pixel constant 30
    real_vector4d chud_scalar_output_ef;        // pixel constant 31
    real_vector4d texture_transform;            // vertex constant 32
};
struct s_chud_widget_placement_data
{
    union { float f0; struct { unsigned short anchor; unsigned short anchor_flags; }; };
    float anchor_x;
    float anchor_y;
    real_point2d offset;
    real_vector2d scale;
};
struct s_widget_geometry_vertex
{
    real_point2d position;
    real_point2d texcoord;
};
struct s_widget_geometry_character
{
    s_widget_geometry_vertex vertices[4];
};
struct s_widget_geometry
{
    real_point3d position;
    real_vector2d scale;
    real_point3d offset;
    real_euler_angles3d rotation;
    long texture_handle;
    long render_type;
    union { struct { float x0, x1, y0, y1; }; real_rectangle2d bounds; };
    real_rectangle2d texture_bounds;
    long character_count;
    s_widget_geometry_character characters[96];
};
struct s_chud_widget_render_data
{
    short shader_type;
    short pad2;
    short external_inputs[2];
    unsigned long color_outputs[4];
    float scalar_outputs[4];
    unsigned short color_inputs[6];
    unsigned short scalar_inputs[6];
};
struct s_anchor_basis
{
    float scale;
    real_vector3d forward;
    real_vector3d left;
    real_vector3d up;
    real_point3d position;
};

struct s_chud_runtime_widget_datum
{
    short identifier;
    signed char widget_index;
    signed char collection_index;
    signed char definition_index;
    unsigned char flags;
    short state;
    short state_time;
    long next_index;
    float external_inputs[2];
};


struct s_chud_cortana_suck_definition
{
    float maximum_distance;
    float near_config[24];
    s_tag_reference near_sound;
    float far_config[24];
    s_tag_reference far_sound;
};


class c_curved_hud_test;
extern "C" void chud_compute_projection_point(c_curved_hud_test *, float, float, real_point2d *);

class c_curved_hud_test
{
    friend void chud_compute_projection_point(c_curved_hud_test *, float, float, real_point2d *);
public:
    // Projects the HUD onto the curved surface described by g_chud_draw_globals (curvature, pixel bounds).
    c_curved_hud_test();
    void inverse_transform(real_point2d *point, float *x, float *y);
    void set_constants(long);
private:
    // Four 16-byte rows uploaded to vertex shader constants 24..27.
    real_vector4d m_shader_constants[4];
    // tan of the half-angles: i = horizontal (angle * aspect), j = vertical.
    real_vector2d m_corner_tangents;
    real_point2d m_origin;
    real_vector2d m_half_size;
    float m_horizontal_angle;
    float m_aspect_ratio;
    float m_distance;
    // Angle spanned by the top edge divided by the HUD width in pixels.
    float m_radians_per_pixel;
    real_vector2d m_projection_scale;
};


struct s_chud_cortana_effect_basis
{
    real_point2d position;
    real_vector2d velocity;
};

class c_chud_cortana_effect;
struct s_chud_persistent_display_state;
extern "C" void chud_cortana_effect_render(c_chud_cortana_effect *, long);

class c_chud_cortana_effect
{
    friend void chud_cortana_effect_render(c_chud_cortana_effect *, long);
    friend void chud_persistent_display_state_reset(s_chud_persistent_display_state *);
    friend void chud_dispose_from_old_map();
public:
    static void begin_frame();
    void set_constants(float blend, float amount, float fade);
    void update_noise(float blend);
    void compute_effect_config(long user_index);
    void render_debug(long user_index);
    void update(long user_index);

private:
    long object_index;
    long marker_name;
    bool active;
    float radius;
    real_vector4d m_vertex_constant0;
    s_chud_cortana_effect_basis m_vertex_constant1;
    real_vector4d m_vertex_constant3;
    s_chud_cortana_effect_basis m_vertex_constant4;
    float m_pixel_constant_z;
    float m_pixel_constant_w;
    real_point3d m_position;
    c_chud_point_tracker m_tracker;
    float m_effect_blend;
    float m_pixel_constant_tail;
    float m_desired_scale;
    float m_scale;
    // match: 0x471440: reset traverses all four contiguous sound handles.
    union
    {
        struct { long m_sounds[2]; long m_secondary_sounds[2]; };
        long m_sound_handles[4];
    };
    union
    {
        struct { long m_state0; long m_state1; };
        long m_states[2];
    };
    // The blended heading config contains twelve bounds followed by its sound reference.
    real_bounds m_noise_bounds;
    real_bounds m_vertex_bounds[5];
    real_bounds m_pixel_bounds[6];
    s_tag_reference sound_reference;
};


typedef char chud_cortana_effect_size[(sizeof(c_chud_cortana_effect) == 0x128) ? 1 : -1];

typedef char chud_draw_layout_check_1[(sizeof(s_chud_runtime_widget_datum) == 0x18) ? 1 : -1];
typedef char chud_draw_layout_check_2[(sizeof(s_chud_cortana_suck_definition) == 0xe4) ? 1 : -1];
typedef char chud_draw_layout_check_3[(offsetof(s_chud_cortana_suck_definition, near_sound) == 0x64) ? 1 : -1];
typedef char chud_draw_layout_check_4[(offsetof(s_chud_cortana_suck_definition, far_sound) == 0xd4) ? 1 : -1];
typedef char chud_draw_layout_check_5[(sizeof(c_curved_hud_test) == 0x70) ? 1 : -1];
typedef char chud_widget_render_data_size_check[(sizeof(s_chud_widget_render_data) == 0x40) ? 1 : -1];
typedef char chud_widget_render_data_external_inputs_check[(offsetof(s_chud_widget_render_data, external_inputs) == 4) ? 1 : -1];

#endif
