#ifndef INTERFACE_CORE_WIDGET_DEFINITION_H
#define INTERFACE_CORE_WIDGET_DEFINITION_H

#include "math/real_math_types.h"
#include "interface/gui_widget_types.h"

struct s_core_widget_definition {
    long flags;
    long name;
    short unknown;
    short font;
    s_gui_rectangle bounds[2];
    s_gui_tag_reference animation;

    s_core_widget_definition();
};

struct s_runtime_core_widget_definition {
    long flags;
    long name;
    long unknown;
    long font;
    real_rectangle2d current_bounds;
    real_rectangle2d authored_bounds;
    long animation_definition_index;

    __forceinline s_runtime_core_widget_definition()
        : flags(0), name(0), unknown(0), font(0), current_bounds(), animation_definition_index(-1) {
        current_bounds.y1 = 0.0f;
        current_bounds.y0 = 0.0f;
        current_bounds.x1 = 0.0f;
        current_bounds.x0 = 0.0f;
    }
};

typedef char core_wdef_size_tag[(sizeof(s_core_widget_definition) == 0x2c) ? 1 : -1];
typedef char core_wdef_size_runtime[(sizeof(s_runtime_core_widget_definition) == 0x34) ? 1 : -1];

#endif // INTERFACE_CORE_WIDGET_DEFINITION_H
