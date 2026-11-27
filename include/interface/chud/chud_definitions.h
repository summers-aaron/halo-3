#ifndef INTERFACE_CHUD_CHUD_DEFINITIONS_H
#define INTERFACE_CHUD_CHUD_DEFINITIONS_H

#include "cseries/cseries_types.h"
#include "tag_files/tag_types.h"
#include <stddef.h>

enum
{
    _chud_widget_text_font_full_screen_hud_message = 0,
    _chud_widget_text_font_split_screen_hud_message,
    _chud_widget_text_font_terminal,
    _chud_widget_text_font_body_text,
    _chud_widget_text_font_title,
    _chud_widget_text_font_super_large,
    _chud_widget_text_font_large_body_text,
    _chud_widget_text_font_english_body_text,
    _chud_widget_text_font_hud_number,
    _chud_widget_text_font_subtitle,
    _chud_widget_text_font_main_menu,
};

// March chud.obj 8363c278 picks elite_sound (+0x18) instead of sound when its alternate flag is set.
struct chud_sound_reference
{
    s_tag_reference sound;
    long mask;
    float gain;
    s_tag_reference elite_sound;
};
typedef char chud_sound_reference_size[(sizeof(chud_sound_reference) == 0x28) ? 1 : -1];
typedef char chud_sound_reference_elite_sound_offset[(offsetof(chud_sound_reference, elite_sound) == 0x18) ? 1 : -1];

struct s_chud_widget_animation_entry
{
    unsigned short flags;
    short input;
    s_tag_reference animation;
};

struct s_chud_widget_animation_data
{
    s_chud_widget_animation_entry animations[6];
    long compute_type_duration_msec(long animation_index, bool looping);
};

typedef char chud_widget_animation_entry_size[(sizeof(s_chud_widget_animation_entry) == 20) ? 1 : -1];
typedef char chud_widget_animation_entry_animation_offset[(offsetof(s_chud_widget_animation_entry, animation) == 4) ? 1 : -1];
typedef char chud_widget_animation_data_size[(sizeof(s_chud_widget_animation_data) == 0x78) ? 1 : -1];

struct state
{
    unsigned short active_flags[8];
    unsigned short impulse_grenade_flags;
    unsigned short active_flags_12;
    unsigned short active_flags_14;
    unsigned short active_flags_16;
    unsigned short impulse_ammo_flags;
    unsigned short weapon_active_flags[3];
    unsigned short flashing;
    unsigned short weaponFlashing;
    unsigned short hide;
};

struct s_chud_widget_base
{
    string_id name;
    short scripting_class;
    unsigned char flags;
    unsigned char anchor;
    c_typed_tag_block<state> states;
    s_tag_block placement;
    s_tag_block animations;
    s_tag_block render_data;

    long compute_state_duration_msec(long state, bool looping);
};

typedef char chud_widget_base_size[(sizeof(s_chud_widget_base) == 0x38) ? 1 : -1];
typedef char chud_widget_base_animation_offset[(offsetof(s_chud_widget_base, animations) == 0x20) ? 1 : -1];

struct s_chud_bitmap_widget
{
    s_chud_widget_base base;
    long runtime_index;
    unsigned short flags;
    unsigned short pad3e;
    s_tag_reference bitmap;
    signed char sequence_index;
};
struct s_chud_text_widget
{
    s_chud_widget_base base;
    long runtime_index;
    unsigned short flags;
    short font;
    long string_id;
};
struct s_chud_widget_collection {
 string_id name; short scripting_class; unsigned char flags; unsigned char anchor; c_typed_tag_block<state> states;
 s_tag_block placement; s_tag_block animations; s_tag_block render_data;
 long bitmap_widget_count; TAG_POINTER(s_chud_bitmap_widget) bitmap_widgets; long bitmap_widget_definition;
 long text_widget_count; TAG_POINTER(s_chud_text_widget) text_widgets; long text_widget_definition;
 __forceinline s_chud_widget_base *get_widget_by_index(long index) {
  if (index < bitmap_widget_count)
   return &bitmap_widgets[index].base;
  return &text_widgets[index - bitmap_widget_count].base;
 }
 __forceinline long get_widget_count() { return bitmap_widget_count + text_widget_count; }
 __forceinline long get_widget_runtime_index(long index) {
  if (index < bitmap_widget_count)
   return bitmap_widgets[index].runtime_index;
  return text_widgets[index - bitmap_widget_count].runtime_index;
 }
};

struct s_chud_definition
{
    long widget_collection_count;
    TAG_POINTER(s_chud_widget_collection) widget_collections;
    long widget_collection_definition;
    long rounds_loaded_warning;
    long rounds_total_warning;
    long age_warning;
};
typedef char chud_definition_loaded_warning_offset[(offsetof(s_chud_definition, rounds_loaded_warning) == 0xc) ? 1 : -1];
typedef char chud_definition_total_warning_offset[(offsetof(s_chud_definition, rounds_total_warning) == 0x10) ? 1 : -1];
typedef char chud_definition_age_warning_offset[(offsetof(s_chud_definition, age_warning) == 0x14) ? 1 : -1];

typedef char chud_bitmap_widget_flags_offset[(offsetof(s_chud_bitmap_widget, flags) == 0x3c) ? 1 : -1];
typedef char chud_bitmap_widget_bitmap_offset[(offsetof(s_chud_bitmap_widget, bitmap) == 0x40) ? 1 : -1];
typedef char chud_bitmap_widget_size[(sizeof(s_chud_bitmap_widget) == 0x54) ? 1 : -1];
typedef char chud_text_widget_flags_offset[(offsetof(s_chud_text_widget, flags) == 0x3c) ? 1 : -1];
typedef char chud_text_widget_string_offset[(offsetof(s_chud_text_widget, string_id) == 0x40) ? 1 : -1];
typedef char chud_text_widget_size[(sizeof(s_chud_text_widget) == 0x44) ? 1 : -1];
typedef char chud_widget_collection_size[(sizeof(s_chud_widget_collection) == 0x50) ? 1 : -1];

// Animation channels occupy seven tag blocks, followed by the period.
// March 836a63f8 returns the unsigned period at +0x58 without any local variables.
struct s_chud_animation_definition
{
    unsigned short flags;
    unsigned short pad2;
    s_tag_block position;
    s_tag_block rotation;
    s_tag_block scale;
    s_tag_block unused;
    s_tag_block scalar;
    s_tag_block scalar_zero;
    s_tag_block texture;
    unsigned long period_msec;

    unsigned long get_period_msec() { return period_msec; }
};

typedef char chud_animation_definition_size[(sizeof(s_chud_animation_definition) == 0x5c) ? 1 : -1];
typedef char chud_animation_definition_period_offset[(offsetof(s_chud_animation_definition, period_msec) == 0x58) ? 1 : -1];

// Functions defined in tu/interfacelibxbox/chud_definitions.cpp that other units call.

extern "C" void chud_animation_interpolate_reals(const float *a, const float *b, float t, float *out, long count);

#endif // INTERFACE_CHUD_CHUD_DEFINITIONS_H
