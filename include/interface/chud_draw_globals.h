#ifndef INTERFACE_CHUD_DRAW_GLOBALS_H
#define INTERFACE_CHUD_DRAW_GLOBALS_H

#include <stddef.h>

#include "math/real_math_types.h"

// The curvature record is also the draw/view scaling record at +0x90.
struct s_chud_curvature_info {
    long resolution_flags;
    float field_of_view, aspect_ratio, depth_offset;
    long virtual_width;
    long virtual_height;
    real_point2d origin;
    float blip_scale, blip_forward_scale;
    real_vector2d safe_bounds_scale;
    real_vector2d pixel_bounds_inset;
    float notification_vertical_offset;
    float state_left_right_vertical_offset;
    float message_anchor_vertical_offset;
    float state_message_vertical_offset;
    float medal_width, medal_height;
    float state_text_scale, message_height_scale, message_offset_scale;
    long message_row_count;
};
typedef s_chud_curvature_info chud_table_entry;
typedef char chud_curvature_info_size[(sizeof(s_chud_curvature_info) == 0x60) ? 1 : -1];
typedef char chud_curvature_info_count_offset[(offsetof(s_chud_curvature_info, message_row_count) == 0x5c) ? 1 : -1];

struct s_chud_draw_globals {
    real_rectangle2d display_bounds;
    real_rectangle2d display_safe_bounds;
    real_rectangle2d display_window_bounds;
    real_rectangle2d display_window_safe_bounds;
    real_rectangle2d virtual_bounds;
    real_rectangle2d virtual_safe_bounds;
    union {
        real_rectangle2d virtual_window_bounds;
        struct { float left, right, top, bottom; };
    };
    real_rectangle2d virtual_window_safe_bounds;
    rectangle2d pixel_bounds;
    float width, height;
    s_chud_curvature_info *curvature_info;
    real_point2d origin;
    long resolution;
    float global_hud_alpha;
    float curvature_amount;
    long user_index;
    long skin_index;
    int : 32;
    real_vector2d curvature_pixel_scale;
};
extern "C" s_chud_draw_globals g_chud_draw_globals;

#if defined(_XBOX)
typedef char s_chud_curvature_info_blip_scale_check[(offsetof(s_chud_curvature_info, blip_scale) == 0x20) ? 1 : -1];
typedef char s_chud_curvature_info_safe_bounds_scale_check[(offsetof(s_chud_curvature_info, safe_bounds_scale) == 0x28) ? 1 : -1];
typedef char s_chud_curvature_info_pixel_bounds_inset_check[(offsetof(s_chud_curvature_info, pixel_bounds_inset) == 0x30) ? 1 : -1];
typedef char s_chud_curvature_info_notification_vertical_offset_check[(offsetof(s_chud_curvature_info, notification_vertical_offset) == 0x38) ? 1 : -1];
typedef char s_chud_draw_globals_pixel_bounds_check[(offsetof(s_chud_draw_globals, pixel_bounds) == 0x80) ? 1 : -1];
typedef char s_chud_draw_globals_width_check[(offsetof(s_chud_draw_globals, width) == 0x88) ? 1 : -1];
typedef char s_chud_draw_globals_curvature_info_check[(offsetof(s_chud_draw_globals, curvature_info) == 0x90) ? 1 : -1];
typedef char s_chud_draw_globals_global_hud_alpha_check[(sizeof(void *) != 4 || offsetof(s_chud_draw_globals, global_hud_alpha) == 0xa0) ? 1 : -1];
typedef char s_chud_draw_globals_curvature_amount_check[(sizeof(void *) != 4 || offsetof(s_chud_draw_globals, curvature_amount) == 0xa4) ? 1 : -1];
typedef char s_chud_draw_globals_user_index_check[(sizeof(void *) != 4 || offsetof(s_chud_draw_globals, user_index) == 0xa8) ? 1 : -1];
typedef char s_chud_draw_globals_curvature_pixel_scale_check[(sizeof(void *) != 4 || offsetof(s_chud_draw_globals, curvature_pixel_scale) == 0xb4) ? 1 : -1];
typedef char s_chud_draw_globals_size_check[(sizeof(void *) != 4 || sizeof(s_chud_draw_globals) == 0xbc) ? 1 : -1];
#endif

#endif // INTERFACE_CHUD_DRAW_GLOBALS_H
