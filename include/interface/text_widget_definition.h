#ifndef INTERFACE_TEXT_WIDGET_DEFINITION_H
#define INTERFACE_TEXT_WIDGET_DEFINITION_H

#include "interface/core_widget_definition.h"
#include "tag_files/tag_types.h"
#include <stddef.h>

// Fields read by c_gui_text_widget::add_definition_fields.
struct s_text_widget_definition
{
    s_text_widget_definition();
    s_core_widget_definition core;
    long string_id;
    long string_list;
    long color_name;
    short font;
    short pad3a;
};

struct s_text_widget_block
{
    s_text_widget_block();
    s_gui_tag_reference text_widget_template;
    s_text_widget_definition definition;
};

typedef char text_widget_definition_size[(sizeof(s_text_widget_definition) == 0x3c) ? 1 : -1];
typedef char text_widget_block_size[(sizeof(s_text_widget_block) == 0x4c) ? 1 : -1];
typedef char text_widget_font_offset[(offsetof(s_text_widget_definition, font) == 0x38) ? 1 : -1];

#endif
