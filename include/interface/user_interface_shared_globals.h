#ifndef INTERFACE_USER_INTERFACE_SHARED_GLOBALS_H
#define INTERFACE_USER_INTERFACE_SHARED_GLOBALS_H

#include "math/real_math_types.h"
#include "tag_files/tag_types.h"
#include <stddef.h>

struct s_named_text_color
{
    long name;
    real_argb_color color;
};

struct s_tint_color_list
{
    int count;
    TAG_POINTER(real_argb_color) colors;
    TAG_POINTER(void) definition;
};

struct s_main_menu_character
{
    char object_name[32];
    char squad_name[32];
    long starting_location_name;
};

struct s_user_interface_shared_globals
{
    short incremental_text_update_period;
    short incremental_text_block_character;
    float first;
    float second;
    float third;
    s_tag_reference global_strings;
    s_tag_reference damage_reporting_strings;
    s_tag_reference looping_sound;
    long fade_time;
    real_argb_color default_text_color;
    real_argb_color shadow_color;
    c_typed_tag_block<s_named_text_color> text_colors;
    s_tag_block tint_color_lists;
    s_tag_reference sound_effects;
    s_tag_block alert_descriptions;
    s_tag_block dialogs;
    s_tag_block global_datasources;
    real_vector2d authoring_aspect_conversion_scaling[2];
    float blur_x;
    float blur_y;
    s_main_menu_character characters[2];
    long hold_delay;
    unsigned long repeat_delay;
    unsigned long repeat_interval;
    long timeout;
};

#define UI_SHARED_OFFSET(member, offset) typedef char ui_shared_##member##_offset[(offsetof(s_user_interface_shared_globals, member) == offset) ? 1 : -1]
UI_SHARED_OFFSET(incremental_text_update_period, 0x0);
UI_SHARED_OFFSET(incremental_text_block_character, 0x2);
UI_SHARED_OFFSET(global_strings, 0x10);
UI_SHARED_OFFSET(damage_reporting_strings, 0x20);
UI_SHARED_OFFSET(looping_sound, 0x30);
UI_SHARED_OFFSET(fade_time, 0x40);
UI_SHARED_OFFSET(default_text_color, 0x44);
UI_SHARED_OFFSET(shadow_color, 0x54);
UI_SHARED_OFFSET(text_colors, 0x64);
UI_SHARED_OFFSET(tint_color_lists, 0x70);
UI_SHARED_OFFSET(sound_effects, 0x7c);
UI_SHARED_OFFSET(alert_descriptions, 0x8c);
UI_SHARED_OFFSET(dialogs, 0x98);
UI_SHARED_OFFSET(global_datasources, 0xa4);
UI_SHARED_OFFSET(authoring_aspect_conversion_scaling, 0xb0);
UI_SHARED_OFFSET(blur_x, 0xc0);
UI_SHARED_OFFSET(characters, 0xc8);
UI_SHARED_OFFSET(hold_delay, 0x150);
UI_SHARED_OFFSET(repeat_delay, 0x154);
UI_SHARED_OFFSET(repeat_interval, 0x158);
UI_SHARED_OFFSET(timeout, 0x15c);
#undef UI_SHARED_OFFSET

typedef char user_interface_shared_globals_size[(sizeof(s_user_interface_shared_globals) == 0x160) ? 1 : -1];

#endif // INTERFACE_USER_INTERFACE_SHARED_GLOBALS_H
