#ifndef INTERFACE_BITMAP_WIDGET_DEFINITION_H
#define INTERFACE_BITMAP_WIDGET_DEFINITION_H

#include "interface/core_widget_definition.h"
#include <stddef.h>

// Tag references at +0x2c and +0x3c; March constructor 0x8369c1b8.
struct s_bitmap_widget_definition
{
    s_bitmap_widget_definition();

    s_core_widget_definition core;
    s_gui_tag_reference bitmap;
    s_tag_reference shader;
    unsigned short blend_method;
    unsigned short initial_sprite_sequence;
    unsigned short initial_sprite_frame;
    short pad52;
    long value_override_list;
    long value_identifier;
};

struct s_bitmap_widget_block
{
    s_bitmap_widget_block();

    s_gui_tag_reference bitmap_widget_template;
    s_bitmap_widget_definition definition;
};

typedef char bitmap_widget_definition_size[(sizeof(s_bitmap_widget_definition) == 0x5c) ? 1 : -1];
typedef char bitmap_widget_definition_bitmap_offset[(offsetof(s_bitmap_widget_definition, bitmap) == 0x2c) ? 1 : -1];
typedef char bitmap_widget_definition_shader_offset[(offsetof(s_bitmap_widget_definition, shader) == 0x3c) ? 1 : -1];
typedef char bitmap_widget_block_definition_offset[(offsetof(s_bitmap_widget_block, definition) == 0x10) ? 1 : -1];

#endif
