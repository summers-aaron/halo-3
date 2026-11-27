#ifndef OBJECTS_WIDGETS_WIDGET_TYPE_H
#define OBJECTS_WIDGETS_WIDGET_TYPE_H

#include "math/matrix_math_types.h"
#include "objects/object_reference_types.h"
#include <stddef.h>

// The 0x40-byte widget dispatch table used by objects.cpp and main.cpp.
struct s_widget_type
{
    unsigned long group_tag;
    long flags;
    void (*initialize)();
    void (*initialize_for_new_map)();
    void (*dispose_from_old_map)();
    void (*dispose)();
    long (*new_on_map)(long, long, real_matrix4x3 *);
    void (*delete_widget)(long);
    void (*transfer)(long, long, long);
    void (*update)(float);
    void (*render_debug)(long, long);
    void (*render_opaque)(long, long);
    bool (*get_is_transparent)(long);
    void (*render_transparent)(const void *, long);
    void (*get_sorting_position)(long, real_point3d *, s_location *, long *);
    void (*render_unattached)(long);
};

typedef char widget_type_size[(sizeof(void *) != 4 || sizeof(s_widget_type) == 0x40) ? 1 : -1];
typedef char widget_type_update_offset[(sizeof(void *) != 4 || offsetof(s_widget_type, update) == 0x24) ? 1 : -1];

extern "C" s_widget_type g_widget_types[];

#endif // OBJECTS_WIDGETS_WIDGET_TYPE_H
