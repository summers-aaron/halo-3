#ifndef INTERFACE_CHUD_CHUD_GLOBALS_DEFINITIONS_H
#define INTERFACE_CHUD_CHUD_GLOBALS_DEFINITIONS_H

#include "tag_files/tag_types.h"
#include "interface/chud_draw_globals.h"
#include <stddef.h>

struct chud_table_row
{
    long key;
    long colors[28];
    long entry_count;
    TAG_POINTER(chud_table_entry) entries;
    long entries_definition;
    s_tag_block sounds;
    s_tag_reference equipment_pickup_sound;
    s_tag_reference grenade_pickup_sounds[4];
    s_tag_reference curvature_bitmap;
    s_tag_reference curvature_vertex_bitmap;
    s_tag_reference navpoint_arrow_bitmap;
    s_tag_reference navpoint_bitmap;
    s_tag_reference interface_references[4];
    s_tag_reference string_list;
    s_tag_reference medal_bitmap;
    s_tag_block medals;
    s_tag_reference medal_animation;
    s_tag_reference cortana_channel;
    s_tag_reference test_bitmap1;
    s_tag_reference test_bitmap2;
    s_tag_reference damage_flash_effect;
    s_tag_reference damage_flash_sound;
    float grenade_anchor_offset;
    float no_secondary_weapon_value;
    float alternate_value;
    float secondary_weapon_value;
    float highlight_value;
    float scoreboard_spacing;
    float navpoint_minimum_scale;
    float navpoint_maximum_scale;
};
struct chud_shader_pair { s_tag_reference vertex_shader; s_tag_reference pixel_shader; };
struct player_training_chud_entry
{
    long text;
    unsigned short delay;
    unsigned short completion_count;
    unsigned short ticks;
    unsigned short delay_seconds;
    float seconds;
    unsigned short flags;
};

struct s_chud_globals_definition
{
    long row_count;
    TAG_POINTER(chud_table_row) rows;
    long row_definition;
    long shader_count;
    TAG_POINTER(chud_shader_pair) shaders;
    long shader_definition;
    s_tag_block suck_profiles;
    s_tag_block cortana_definitions;
    s_tag_block training_data;
    s_tag_reference emblem_bitmap;
    s_tag_reference medal_bitmap;
    s_tag_reference medal_animation;
    float medal_width;
    float medal_height;
    float medal_offset_x;
    float medal_offset_y;
    float scoreboard_top_y;
    float scoreboard_spacing;
    s_tag_reference curvature_bitmap;
    float curvature_scale;
    float blip_radius[6];
    s_tag_reference blip_shape;
    s_tag_reference death_effect;
    s_tag_reference cinematic_strip_resource;
    s_tag_reference cinematic_strip_bitmap;
};
typedef char chud_globals_cortana_offset[(offsetof(s_chud_globals_definition, cortana_definitions) == 0x24) ? 1 : -1];
typedef char chud_globals_death_effect_offset[(offsetof(s_chud_globals_definition, death_effect) + offsetof(s_tag_reference, index) == 0xcc) ? 1 : -1];
typedef char chud_globals_strip_bitmap_offset[
    (offsetof(s_chud_globals_definition, cinematic_strip_bitmap) + offsetof(s_tag_reference, index) == 0xec) ? 1 : -1];

typedef char chud_table_row_string_list_offset[(offsetof(chud_table_row, string_list) + offsetof(s_tag_reference, index) == 0x168) ? 1 : -1];
typedef char chud_table_row_sequence_offset[(offsetof(chud_table_row, medal_bitmap) + offsetof(s_tag_reference, index) == 0x178) ? 1 : -1];
typedef char chud_table_row_sequence_values_offset[(offsetof(chud_table_row, medals) + offsetof(s_tag_block, address) == 0x180) ? 1 : -1];

typedef char chud_navpoint_minimum_scale_offset[(offsetof(chud_table_row, navpoint_minimum_scale) == 0x200) ? 1 : -1];
typedef char chud_navpoint_maximum_scale_offset[(offsetof(chud_table_row, navpoint_maximum_scale) == 0x204) ? 1 : -1];
typedef char chud_table_row_size[(sizeof(chud_table_row) == 0x208) ? 1 : -1];
typedef char chud_globals_definition_size[(sizeof(s_chud_globals_definition) == 0xf0) ? 1 : -1];

typedef char chud_globals_emblem_offset[(offsetof(s_chud_globals_definition, emblem_bitmap) + offsetof(s_tag_reference, index) == 0x48) ? 1 : -1];
typedef char chud_globals_blip_radius_offset[(offsetof(s_chud_globals_definition, blip_radius) == 0x98) ? 1 : -1];
typedef char chud_globals_blip_shape_offset[(offsetof(s_chud_globals_definition, blip_shape) + offsetof(s_tag_reference, index) == 0xbc) ? 1 : -1];
typedef char chud_row_navpoint_arrow_offset[(offsetof(chud_table_row, navpoint_arrow_bitmap) + offsetof(s_tag_reference, index) == 0x108) ? 1 : -1];
typedef char chud_row_navpoint_offset[(offsetof(chud_table_row, navpoint_bitmap) + offsetof(s_tag_reference, index) == 0x118) ? 1 : -1];
typedef char chud_row_medals_offset[(offsetof(chud_table_row, medals) == 0x17c) ? 1 : -1];
typedef char chud_row_medal_animation_offset[(offsetof(chud_table_row, medal_animation) + offsetof(s_tag_reference, index) == 0x194) ? 1 : -1];
typedef char chud_row_anchor_spacing_offset[(offsetof(chud_table_row, grenade_anchor_offset) == 0x1e8) ? 1 : -1];
typedef char chud_row_anchor_offset[(offsetof(chud_table_row, scoreboard_spacing) == 0x1fc) ? 1 : -1];
typedef char chud_row_colors_offset[(offsetof(chud_table_row, colors) == 4) ? 1 : -1];
typedef char chud_row_medal_bitmap_offset[(offsetof(chud_table_row, medal_bitmap) + offsetof(s_tag_reference, index) == 0x178) ? 1 : -1];
typedef char chud_globals_training_offset[(offsetof(s_chud_globals_definition, training_data) == 0x30) ? 1 : -1];
typedef char chud_grenade_pickup_sounds_offset[(offsetof(chud_table_row, grenade_pickup_sounds) == 0x9c) ? 1 : -1];

typedef char chud_equipment_pickup_sound_offset[(offsetof(chud_table_row, equipment_pickup_sound) + offsetof(s_tag_reference, index) == 0x98) ? 1 : -1];

#endif
