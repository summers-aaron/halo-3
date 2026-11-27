#ifndef INTERFACE_MODEL_WIDGET_DEFINITION_H
#define INTERFACE_MODEL_WIDGET_DEFINITION_H

#include "interface/core_widget_definition.h"
#include "tag_files/tag_types.h"
#include <stddef.h>

struct s_model_widget_tag_element_body
{
    real_rectangle2d bounds4;
    real_rectangle2d bounds20;
    s_tag_block distance_function_block;
    s_tag_block height_function_block;
};
struct s_model_widget_tag_element
{
    long identifier;
    s_model_widget_tag_element_body body;
};

// The owning widget reads
// the element count and address immediately after the 0x2c-byte core.
struct s_model_widget_definition
{
    s_core_widget_definition core;
    c_typed_tag_block<s_model_widget_tag_element> elements;
};

struct s_model_widget_block
{
    s_tag_reference model_widget_template;
    s_model_widget_definition definition;
};

typedef char model_widget_definition_size[(sizeof(s_model_widget_definition) == 0x38) ? 1 : -1];
typedef char model_widget_block_size[(sizeof(s_model_widget_block) == 0x48) ? 1 : -1];
typedef char model_widget_elements_offset[(offsetof(s_model_widget_definition, elements) == 0x2c) ? 1 : -1];

#endif
