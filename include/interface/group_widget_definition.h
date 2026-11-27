#ifndef INTERFACE_GROUP_WIDGET_DEFINITION_H
#define INTERFACE_GROUP_WIDGET_DEFINITION_H

#include "interface/core_widget_definition.h"
#include "tag_files/tag_types.h"
#include <stddef.h>

// Group widget with list, text, bitmap, and model child widgets.
struct s_group_widget_definition
{
    s_core_widget_definition core;
    s_tag_block list_widgets;
    s_tag_block text_widgets;
    s_tag_block bitmap_widgets;
    s_tag_block model_widgets;
};

struct s_group_widget_block
{
    s_tag_reference group_widget_template;
    s_group_widget_definition definition;
};

// Button key with a string list plus text and bitmap child widgets.
struct s_button_key_definition
{
    s_core_widget_definition core;
    s_tag_reference string_list;
    s_tag_block text_widgets;
    s_tag_block bitmap_widgets;
};

struct s_button_key_block
{
    s_tag_reference button_key_template;
};

typedef char group_definition_size[(sizeof(s_group_widget_definition) == 0x5c) ? 1 : -1];
typedef char group_definition_lists[(offsetof(s_group_widget_definition, list_widgets) == 0x2c) ? 1 : -1];
typedef char group_definition_models[(offsetof(s_group_widget_definition, model_widgets) == 0x50) ? 1 : -1];
typedef char group_block_size[(sizeof(s_group_widget_block) == 0x6c) ? 1 : -1];
typedef char button_key_definition_size[(sizeof(s_button_key_definition) == 0x54) ? 1 : -1];
typedef char button_key_definition_text[(offsetof(s_button_key_definition, text_widgets) == 0x3c) ? 1 : -1];

#endif
