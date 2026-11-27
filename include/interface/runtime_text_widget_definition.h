#ifndef INTERFACE_RUNTIME_TEXT_WIDGET_DEFINITION_H
#define INTERFACE_RUNTIME_TEXT_WIDGET_DEFINITION_H

#include "interface/core_widget_definition.h"

struct s_runtime_text_widget_definition
{
    s_runtime_core_widget_definition core;
    long string_id;
    long string_list;
    real_argb_color color;
    long font;
    __forceinline s_runtime_text_widget_definition() : string_id(0), string_list(0), color(), font(0)
    {
        color.alpha = 0.0f;
        color.rgb.red = 0.0f;
        color.rgb.green = 0.0f;
        color.rgb.blue = 0.0f;
    }
};

typedef char runtime_text_widget_definition_size[(sizeof(s_runtime_text_widget_definition) == 0x50) ? 1 : -1];

#endif
